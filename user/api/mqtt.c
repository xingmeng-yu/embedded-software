#include "mqtt.h"
#include "wifi.h"
#include "delay.h"
#include "mqttpacket.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "dht11.h"

#define WIFI_TX_MAX_LEN 1024
u8 wifi_tx_buff[WIFI_TX_MAX_LEN]="\0";//存放报文的缓冲区
int wifi_tx_len = 0;//wifi发送数据的个数


/*
函数功能：客户端连接MQTT服务器
参数1：client_id -- 设备ID号
参数2：username  -- 用户名
参数3：passwd -- 用户密码
参数4：timeout -- 等待应答时长 单位是ms
返回值：=0 成功  >=0 失败
1 -- 等待应答超时
2 -- 数据不完整
3 -- 0x01连接已拒绝，不支持的协议版本,服务端不支持客户端请求的MQTT协议级别
4 -- 0x02连接已拒绝，不合格的客户端标识符,客户端标识符是正确的UTF-8编码，但服务端不允许使用
5 -- 0x03连接已拒绝，服务端不可用网络连接已建立，但MQTT服务不可用
6 -- 0x04连接已拒绝，无效的用户名或密码 用户名或密码的数据格式无效
7 -- 0x05连接已拒绝，未授权 客户端未被授权连接到此服务器
8 -- CONNECT 报文生成失败
*/
u8 MQTT_Connect(char *client_id,char *username,char *passwd,u32 timeout)
{
	u8 res = 0;//保存返回值
	//1.初始化MQTT Connect报文
	MQTTPacket_connectData options = MQTTPacket_connectData_initializer;
	//options.struct_id -- 不用配置，上述操作已经配置好(MQTC)
	//options.struct_version -- 不用配置，上述操作已经配置好(0)
	//options.MQTTVersion = 4;//3.1.1 不用配置，上述操作已经配置好(4)
	options.clientID.cstring = client_id;//客户端ID号
	options.keepAliveInterval = 600;//保持连接时间 -- 600s
	options.cleansession = 1;//关闭上次会话
//	options.willFlag = 0;//关闭遗嘱 -- 上述操作已经配置好(0)
	options.username.cstring = username;//用户名
	options.password.cstring = passwd;//用户密码
	//把结构体 options 中填写的连接参数，按照 MQTT 协议规范打包成一个二进制 CONNECT 报文
	wifi_tx_len = MQTTSerialize_connect(wifi_tx_buff,WIFI_TX_MAX_LEN,&options);
	if(wifi_tx_len == 0)
		return 8;
	//2.发送报文
	wifi_msg.rx_count = 0;
	wifi_msg.over_flag = 0;
	WIFI_SendNData(wifi_tx_buff,wifi_tx_len);
	//3.等待服务器应答（返回 MQTT CONNACK 报文）+解析
	//3.1 等待接收服务器应答（MQTT CONNACK 报文）
	while(timeout != 0 && wifi_msg.over_flag == 0){
		timeout--;
		Delay_nms_nop(1);
	}
	if(timeout == 0){//判断是否是时间溢出
		res = 1;
		goto over;
	}
	//3.2 收到消息 -- 解析
	//                   固定报头             可变报头
	//CONNACK报文格式   0x20 0x02     0x00/0x01(会话状态)  0x00~0x05(连接状态)
	//3.2.1 解析数据是否完整
	if(wifi_msg.rx_count < 4){
		res = 2;
		goto over;
	}
	//3.2.2 解析连接状态
	switch(wifi_msg.rx_buff[3])
	{
		case 0:res = 0;break;
		default:res = wifi_msg.rx_buff[3]+2;break;
	}
over:
	wifi_msg.rx_count = 0;
	wifi_msg.over_flag = 0;
	return res;
}
/*
MQTT断开连接的报文  -- 固定报头（0xE0 0x00）
返回值：成功 = 0
        失败 > 0 
8 -- 构成报文失败
*/
u8 MQTT_DisConnect(void)
{
	//1.构成报文
	wifi_tx_len = MQTTSerialize_disconnect(wifi_tx_buff,WIFI_TX_MAX_LEN);
	if(wifi_tx_len == 0){
		return 8;
	}
	//2.通过WIFI发送报文
	WIFI_SendNData(wifi_tx_buff,wifi_tx_len);	
	return 0;
}

/*
保存主题需要的消息
*/
#define PUBLISH_DATA_MAX_LEN 512
char publish_data_buff[PUBLISH_DATA_MAX_LEN]="\0";

/*
MQTT 给某个主题，发布消息
topic -- 主题名
data -- 给主题发送的消息

返回值：0 -- 成功
        >0 -- 失败
8 -- 生成报文失败
*/
u8 MQTT_Publish(char *topic,char *data)
{
	MQTTString temp = {0};//保存接收到的主题名
	temp.cstring = topic;
	int len = strlen(data);
	//构成1个publish报文
	wifi_tx_len = MQTTSerialize_publish(wifi_tx_buff,WIFI_TX_MAX_LEN,0,0,0,0,temp,(u8 *)data,len);
	if(wifi_tx_len <= 0)
		return 8;
	//发送报文
	WIFI_SendNData(wifi_tx_buff,wifi_tx_len);
	
	return 0;
}
/*
主题1的消息内容 -- 构成字符串，存储到publish_data_buff
主题1：给云传输设备的相关参数（温湿度、烟雾、甲醛、空气质量、CO2等等）
$oc/devices/68db40414609c465bf280dc7_Device1/sys/properties/report
*/
extern struct KQM6600_DATA kqm6600_data ;
extern u16 light_value;

int my_light;
int my_tem;
float my_voc;
int my_co2;
float my_ch2o;


char *GetTopic1Data(void)
{
	memset(publish_data_buff,0,PUBLISH_DATA_MAX_LEN);//清零
	sprintf(publish_data_buff,
	"{\
    \"id\": \"123\",\
    \"version\": \"1.0\",\
    \"params\": {\
        \"light\": {\
            \"value\":%d\
        },\
        \"tem\": {\
            \"value\": %d\
        },\
				\"hum\": {\
            \"value\": %.2lf\
        },\
				\"VOC\": {\
            \"value\":%.2lf\
        },\
				\"CO2\": {\
            \"value\":%d\
        },\
				\"CH2O\": {\
            \"value\":%.2lf\
        }\
    }\
}",light_value,(u32)tem,hum,kqm6600_data.VOC,kqm6600_data.CO2,kqm6600_data.CH2O);
	return publish_data_buff;
}

char *GetTopic2Data(void)
{
	memset(publish_data_buff,0,PUBLISH_DATA_MAX_LEN);//清零
	sprintf(publish_data_buff,
	"{\
	\"id\": \"38\",\
	\"code\": 200,\
	\"msg\": \"success\"\
}");
	return publish_data_buff;
}
//MQTT_Publish(MQTT_TOPIC1,GetTopic1Data());
/*
订阅1个主题
参数：topic -- 要订阅的主题
      timeout -- 等待订阅应答
返回值：0 -- 成功
       >0 -- 失败
8 -- 构成订阅报文失败
1 -- 应答溢出
2 -- 接收数据不完整
3 -- 订阅失败
*/
u8 MQTT_SubscribeOneTopic(char *topic,u32 timeout)
{
	u8 res = 0;
	MQTTString topics[1] = {0};
	topics[0].cstring = topic;//传递第一个订阅的主题
	int qoss[1] = {0};
	qoss[0] = 0;//Qos = 0
	
	//1.制作订阅报文
	wifi_tx_len = MQTTSerialize_subscribe(wifi_tx_buff,WIFI_TX_MAX_LEN,0,10,1,topics,qoss);
	if(wifi_tx_len <= 0)
		return 8;
	//2.发送报文
	wifi_msg.rx_count = 0;
	wifi_msg.over_flag = 0;
	WIFI_SendNData(wifi_tx_buff,wifi_tx_len);
	//3.接收订阅应答报文
	//3.1 等待接收服务器应答（MQTT 订阅报文）
	while(timeout != 0 && wifi_msg.over_flag == 0){
		timeout--;
		Delay_nms_nop(1);
	}
	if(timeout == 0){//判断是否是时间溢出
		res = 1;
		goto over;
	}
	//3.2 收到消息 -- 解析
	//            固定报头         可变报头             有效载荷
	//报文格式   0x90 0x03       0xXXXX(报文标识符)     0x80(失败)
	//3.2.1 解析数据是否完整
	if(wifi_msg.rx_count < 5){
		res = 2;
		goto over;
	}
	//3.2.2 解析连接状态
	switch(wifi_msg.rx_buff[4])
	{
		case 0x80:res = 3;break;
		default:res = 0;break;
	}
over:
	wifi_msg.rx_count = 0;
	wifi_msg.over_flag = 0;
	
	return res;	
}
#include "beep.h"
void MQTT_RecvOnenet(void)
{
	if(wifi_msg.over_flag == 0) return ;
	char temp = 0;
	char *pos = strstr((char *)&wifi_msg.rx_buff[4],"beep");
	if(pos == NULL){
		goto over;
	}
	temp = *(pos+6);
	if(temp == '1'){
		BEEP_ON();
	}else if(temp == '0'){
		BEEP_OFF();
	}
	MQTT_Publish(MQTT_TOPIC2,GetTopic2Data());
	
over:
	wifi_msg.over_flag = 0;
	wifi_msg.rx_count = 0;
}


/*
MQTT操作（MQTT Connect报文发送   MQTT订阅  MQTT 发布）
*/
u8 mqtt_step = 0;
void MQTT_Opreation(void)
{
	char tx_buff[64]="\0";
	switch(mqtt_step)
	{
		case 0://MQTT Connect报文发送
			if(MQTT_Connect(MQTT_CLIENT_ID,MQTT_USERNAME,MQTT_PASSWD,10000) == 0){
				mqtt_step++;
			}else
				break;
		case 1://MQTT订阅
			if(MQTT_SubscribeOneTopic(MQTT_SUB_TOPIC2,5000) == 0){
				mqtt_step++;
			}else
				break;
		case 2://MQTT 发布
			MQTT_Publish(MQTT_TOPIC1,GetTopic1Data());
			break;
	}
}



