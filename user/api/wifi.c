#include "wifi.h"
#include "string.h"
#include "stdio.h"
#include "delay.h"
#include "cjson.h"
#include "stm32_eval_spi_flash.h"
#include "mqtt.h"

struct WIFI_MESSAGE wifi_msg = {.rx_count = 0,.over_flag = 0};
u8 wifi_step = 0;
u8 net_tiem_step=0;
u8 net_time_state_flag=0;
u8 ap_step = 0;//进入AP配置的步骤
char wifi_ssid[16]="xingyu";//要连接的热点名字
char wifi_passwd[16]="zpy123456";//要连接的热点密码

char server_ip1[]="36.137.226.30";
u32 server_port1 = 44125;

char server_ip2[]="bemfa.com";
u32 server_port2 = 8344;

char server_ip3[]="192.168.43.47";
u32 server_port3 = 8888;

u8 wifi_state = 0;//保存WIFI的状态
u32 WIFI_FLASH_FLAG = 0xAAAA5555;//

/*
1.通信接口初始化（与ESP8266进行通信串口）--USART3
  PE6 -- ESP_EN
	USART3：1+8+0+1 115200
*/
void ESP8266_ENConfig(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	ESP_EN_HIGH;//使能EN管脚
}

void USART3_IT_Config(void)
{
	//中断源
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	//中断通道
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;//中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;//优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//优先级
	NVIC_Init(&NVIC_InitStruct);
}

/*
函数作用：串口3初始化
参数：boud -- 波特率
返回值：无
*/
void USART3_Config(u32 boud)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//配置复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	USART_InitTypeDef USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = boud;//波特率
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//全双工
	USART_InitStruct.USART_Parity = USART_Parity_No;//不使用奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位为1
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//字长为8
	USART_Init(USART3,&USART_InitStruct);
	//串口中断初始化
	USART3_IT_Config();
	//3.USART1使能
	USART_Cmd(USART3,ENABLE);
}
//串口中断服务函数
void USART3_IRQHandler(void)
{
	u8 data = 0;
	//判断是否是接收中断
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET){
		//请中断标志位
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		//数据处理
		//1.读出来数据
		data = USART3->DR;
		//调试功能：
		USART1->DR = data;
		if(wifi_msg.rx_count < WIFI_RX_LEN){//保存数据
			wifi_msg.rx_buff[wifi_msg.rx_count] = data;
			wifi_msg.rx_count++;
		}
	}else if(USART_GetITStatus(USART3,USART_IT_IDLE) == SET)
	{
		USART_ClearITPendingBit(USART3,USART_IT_IDLE);
		data = USART3->DR;//清除空闲中断标志位
		wifi_msg.over_flag = 1;
	}
}

void WIFI_Config(void)
{
	USART3_Config(115200);
	ESP8266_ENConfig();
}

/*
发送1个字节数据
data:要发送的数据
*/
void USART3_SendOneData(u8 data)
{
	//等待上次数据发送完成
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
	//发送新的数据
	USART3->DR = data;//USART_SendData(USART1,data);
}
//发送字符串
//pdata -- 指向要发送的字符串
void USART3_SendString(char *pdata)
{
	while(*pdata)//判断是否遇到'\0'
	{
		USART3_SendOneData((u8)*pdata);
		pdata++;
	}
}

//发送N个字节数据
//pdata -- 指向要发送的数据的首地址
//len -- 要发送数据的长度
void USART3_SendNData(u8 *pdata,u32 len)
{
	for(u32 i=0;i<len;i++)//判断是否遇到'\0'
	{
		USART3_SendOneData(pdata[i]);
	}
}

//WIFI发送函数
void WIFI_SendNData(u8 *pdata,u32 len)
{
	USART3_SendNData(pdata,len);
}

/*
函数作用：发送AT指令，并且解析是否应答成功
参数：
pcmd -- 指向要发送的AT指令字符串
pack -- 指向要接收的应答字符串
timeout -- 等待应答最长时间 -- ms为基础单位
flag -- 0 -- 不等待应答   1 -- 等待应答
返回值：0 -- 成功  >0 失败
1 -- 时间溢出，没有收到应答字符串
2 -- 不等待应答的返回值
*/
u8 WIFI_SendCmdAndRecvAck(char *pcmd,char *pack,u32 timeout,u8 flag)
{
	//清除WIFI接收缓冲区rx_count  rx_buff
	wifi_msg.rx_count = 0;
	memset(wifi_msg.rx_buff,0,WIFI_RX_LEN);
	//发送指令
	WIFI_SendNData((u8 *)pcmd,strlen(pcmd));
	WIFI_SendNData((u8 *)"\r\n",2);
	
	//是否等待应答
	if(flag == 0){
		Delay_nms_nop(timeout);
		return 2;
	}
	//判断是否接收到应答字符串
	do{
		if(strstr((char *)wifi_msg.rx_buff,pack) != NULL){
			wifi_msg.rx_count = 0;//清零，防止接收其他数据，不从0开始
			return 0;//找到应答字符串
		}
		Delay_nms_nop(1);
	}while(timeout--);
	wifi_msg.rx_count = 0;//清零，防止接收其他数据，不从0开始
	return 1;
}

/*
WIFI底层操作（配置：测试AT 连接热点  连接服务器  发送数据）
*/
void WIFI_SetSTAAndConnectServer(void)
{
	char tx_buff[64]="\0";
//	char pstr[] = "cmd=2&uid=41ea9352a65aaac49f3dcc2c0e82be77&topic=CO2&msg=200\r\n";
	switch(wifi_step)
	{
		case 0://AT指令，检测ESP8266是否存在
			if(WIFI_SendCmdAndRecvAck("AT","OK",2000,1) == 0){
				wifi_step++;
			}else
				break;
		case 1://AT+CWMODE=1,设置STA模式
			if(WIFI_SendCmdAndRecvAck("AT+CWMODE=1","OK",2000,1) == 0){
				wifi_step++;
			}else
				break;
		case 2://AT+CWJAP,连接热点
			sprintf(tx_buff,"AT+CWJAP=\"%s\",\"%s\"",wifi_ssid,wifi_passwd);
			if(WIFI_SendCmdAndRecvAck(tx_buff,"GOT IP",5000,1) == 0)
			{
				wifi_step++;
			}else
				break;
		case 3://AT+CIPSTART,连接服务器
			sprintf(tx_buff,"AT+CIPSTART=\"TCP\",\"%s\",%d",MQTT_IP,MQTT_PORT);
			if(WIFI_SendCmdAndRecvAck(tx_buff,"CONNECT",5000,1) == 0)
			{
				wifi_step++;
			}else
				break;
		case 4://AT+CIPMODE=1,设置为透传模式
			if(WIFI_SendCmdAndRecvAck("AT+CIPMODE=1","OK",2000,1) == 0)
			{
				wifi_step++;
			}else
				break;
		case 5://AT+CIPSEND,发送数据 -- >
			if(WIFI_SendCmdAndRecvAck("AT+CIPSEND",">",2000,1) == 0)
			{
				wifi_step = 0;//下次联网做准备
				WIFI_StateChange(1);//WIFI状态切换为状态1 -- 联网状态并且发送数据
			}
			break;
	}
}


/*
WIFI底层操作（配置：测试AT 连接热点  连接pdd服务器 时间 ）
*/
void WIFI_GET_TIME(void)
{
	int time=0;
	char tx_buff[64]="\0";
	switch(net_tiem_step)
	{
		case 0://AT指令，检测ESP8266是否存在
			if(WIFI_SendCmdAndRecvAck("AT","OK",2000,1) == 0){
				net_tiem_step++;
			}else
				break;
		case 1://AT+CWMODE=1,设置STA模式
			if(WIFI_SendCmdAndRecvAck("AT+CWMODE=1","OK",2000,1) == 0){
				net_tiem_step++;
			}else
				break;
		case 2://AT+CWJAP,连接热点
			sprintf(tx_buff,"AT+CWJAP=\"%s\",\"%s\"",wifi_ssid,wifi_passwd);
			if(WIFI_SendCmdAndRecvAck(tx_buff,"GOT IP",5000,1) == 0)
			{
				net_tiem_step++;
			}else
				break;
		case 3://
			if(WIFI_SendCmdAndRecvAck("AT+CIPMUX=0","OK",5000,1) == 0)
			{
				net_tiem_step++;
			}else
				break;
		case 4://AT+CIPSTART,连接服务器
			sprintf(tx_buff,"AT+CIPSTART=\"TCP\",\"%s\",%d",NET_PDD_IP,NET_PDD_PORT);
			if(WIFI_SendCmdAndRecvAck(tx_buff,"CONNECT",5000,1) == 0)
			{
				net_tiem_step++;
			}else
				break;
		case 5://AT+CIPMODE=1,设置为透传模式
			if(WIFI_SendCmdAndRecvAck("AT+CIPMODE=1","OK",2000,1) == 0)
			{
				net_tiem_step++;
			}else
				break;
		case 6://AT+CIPSEND,发送数据 -- >
			if(WIFI_SendCmdAndRecvAck("AT+CIPSEND",">",2000,1) == 0)
			{
				net_tiem_step++;
			}
			break;
		case 7:
			WIFI_SendNData((u8 *)"GET http://api.pinduoduo.com/api/server/_stm\r\n",strlen("GET http://api.pinduoduo.com/api/server/_stm\r\n"));
			Delay_nms_nop(1000);
			net_tiem_step++;
			break;
		case 8://解析数据
			time=parse_net_time((char *)wifi_msg.rx_buff);
			if(time!=0){
				//解析成功，保存RTC值
				RTC_SetCounter(time);
				RTC_WaitForLastTask();
				net_tiem_step=0;
				WIFI_StateChange(0);
			}else{
				net_tiem_step=7;
			}
			break;
	}
}
/*
01 -- 打开蜂鸣器
02 -- 关闭蜂鸣器
*/
#include "beep.h"
void WIFI_DataAnaly_AIThinker(void)
{
	u8 cmd = 0;
	if(wifi_msg.over_flag == 0) return ;//判断是否有数据到来
	//解析数据
	cmd = wifi_msg.rx_buff[0];
	switch(cmd)
	{
		case '1':BEEP_ON();break;
		case '2':BEEP_OFF();break;
	}
over:	
	wifi_msg.over_flag = 0;
	wifi_msg.rx_count = 0;
}

void parse_json_info(const char *json_str)
{
    // 1. 解析JSON字符串
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        printf("JSON解析失败\r\n");
        return;
    }
    // 2. 获取各个字段的值
    cJSON *v1 = cJSON_GetObjectItem(root, "wifiname");
    cJSON *v2 = cJSON_GetObjectItem(root, "password");
    // 3. 根据解析结果执行操作
		printf("v1:%s\r\n",v1->valuestring);//名称
		printf("v2:%s\r\n",v2->valuestring);//密码
		//拷贝到保存地方
		strcpy(wifi_ssid,v1->valuestring);
		strcpy(wifi_passwd,v2->valuestring);
    // 4. 释放内存
    cJSON_Delete(root);
}

//AP配网
void WIFI_SetAPModeAndGetInfo(void)
{
	char *pos = NULL;
	switch(ap_step)
	{
		case 0://AT指令
			if(WIFI_SendCmdAndRecvAck("AT","OK",2000,1) == 0){
				ap_step++;
			}else
				break;
		case 1://AT+CWMODE=2 设置为AP模式
			if(WIFI_SendCmdAndRecvAck("AT+CWMODE=2","OK",2000,1) == 0){
				ap_step++;
			}else
				break;
		case 2://AT+CWSAP -- 启动热点 AT+CWSAP="xyd","12345678",6,4
			if(WIFI_SendCmdAndRecvAck("AT+CWSAP=\"zpy\",\"12345678\",6,4","OK",2000,1) == 0){
				ap_step++;
			}else
				break;
		case 3://AT+CIPMUX=1 -- 服务器设置1对多
			if(WIFI_SendCmdAndRecvAck("AT+CIPMUX=1","OK",2000,1) == 0){
				ap_step++;
			}else
				break;
		case 4://AT+CIPSERVER=1,6666 -- 启动服务器
			if(WIFI_SendCmdAndRecvAck("AT+CIPSERVER=1,6666","OK",2000,1) == 0){
				ap_step++;
			}else
				break;
		case 5://判断是否接收完数据 -- 判断是否收到alarm_m字符串
			if(strstr((char *)wifi_msg.rx_buff,"alarm_m") != NULL){
				ap_step++;
			}else
				break;
		case 6://接收完成，json解析
			pos = strstr((char *)wifi_msg.rx_buff,"{");
			parse_json_info((char *)pos);
			//把解析出来的账号和密码保存到外部flash中
		  sFLASH_EraseSector(WIFI_FLASH_ADDR);
		  sFLASH_WriteBuffer((u8 *)&WIFI_FLASH_FLAG,WIFI_FLASH_ADDR,4);//存放标志
			sFLASH_WriteBuffer((u8 *)wifi_ssid,WIFI_FLASH_ADDR+4,sizeof(wifi_ssid));
			sFLASH_WriteBuffer((u8 *)wifi_passwd,WIFI_FLASH_ADDR+4+16,sizeof(wifi_passwd));
		  
			printf("存入Flash成功\r\n");
			ap_step = 0;
			WIFI_StateChange(0);//wifi状态变为 0 
	}
}


void WIFI_Run(void)
{
	switch(wifi_state)
	{
		case 0://WIFI处于断网状态 --需要做的动作： AT测试、设置STA模式、加入热点、连接服务器、进入透传、进入发数据状态（AT+CIPSEND）
			WIFI_SetSTAAndConnectServer();
			break;
		case 1://WIFI处于联网状态，需要做的动作：在透传模式下，发送数据。
			MQTT_Opreation();
			break;
		case 2://WIFI处于AP配网状态，需要做的动作：AP配网 -- AT测试、设置AP模式、创建热点、配置服务器、等待用户连接和传输相关信息。
			WIFI_SetAPModeAndGetInfo();
			break;
		case 3://获取时间
			WIFI_GET_TIME();
			break;
	}
}

void WIFI_StateChange(u8 new_state)
{
	//只要原来的状态是1 2 3 都需要进行退出透传模式的操作
	if(wifi_state == 1 || wifi_state == 2 || wifi_state == 3)
	{
		WIFI_SendNData((u8 *)"+++",3);//退出透传模式
		Delay_nms_nop(100);
	}
	//修改当前网络状态
	wifi_state = new_state;
	//重启WIFI模组
	WIFI_SendCmdAndRecvAck("AT+RST","OK",5000,0);//重启WIFI
}

//从flash中读取ESP8266要连接热点的名称和密码
void WIFI_ReadInfoFormFlash(void)
{
	u32 new_flag = 0x0;
	sFLASH_ReadBuffer((u8 *)&new_flag,WIFI_FLASH_ADDR,4);
	if(new_flag == WIFI_FLASH_FLAG){
		sFLASH_ReadBuffer((u8 *)wifi_ssid,WIFI_FLASH_ADDR+4,16);
		sFLASH_ReadBuffer((u8 *)wifi_passwd,WIFI_FLASH_ADDR+4+16,16);
	}else{
		WIFI_SendCmdAndRecvAck("AT+RST","OK",5000,0);//重启WIFI
		wifi_state = 2;
	}
}


#include "time.h"
#include "rtc.h"
#include "stdlib.h"
//raw -- 传输进来的 {"server_time":1 785 743 562  614}
//                                 1 234 567 8910 11 12 13   
//返回值：成功返回 时间戳
//        失败返回 0
int parse_net_time(const char *raw)
{
		int now_time = 0;//保存时间戳
		char *pos1 = NULL;//时间戳起始位置
	  char *pos2 = NULL;//时间戳结束位置
	  char buff[16]="\0";
	  char len = 0;
		pos1 = strstr(raw,":");
	  if(pos1 == NULL)
			return 0;
		pos1++;//指向时间戳起始位置
		pos2 = strstr(raw,"}");
		if(pos2 == NULL)
			return 0;
		pos2--;//指向时间戳结束位置(秒的位置)
		len = pos2 - pos1+1;
		for(int i=0;i<(len-3);i++)//舍弃后三位
		{
			buff[i] = pos1[i];
		}
		now_time = atoi(buff) + 8*3600;
    return now_time;
}

