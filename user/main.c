#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"
#include "queue.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lvgl_app.h"
#include "gui_guider.h"
#include "math.h"

//外部资源
extern lv_ui myui;
extern lv_ui *ui;

extern lv_obj_t *screen_table;
/* ========== 传感器ID定义 ========== */
#define SENSOR_ID_DHT11    1
#define SENSOR_ID_KQM6600  2
#define SENSOR_ID_BH1750   3
#define SENSOR_ID_MQ2      4
#define SENSOR_ID_RTC      5

/* ========== 传感器数据结构 ========== */
struct SENSOR_DATA
{
	u8 sensor_id;       // 传感器ID
	u8 buff[24];        // 存放传感器数据
};

/* ========== 全局变量 ========== */
u32 beep_count = 0;    // 响的次数*2 = 开+关 BEEP_Toggle(); 执行2次
void BEEP_Set(u32 count, u32 time);

BaseType_t res = 0;     // 保存任务创建时，返回值

/********/
SemaphoreHandle_t kqm6600_sem;  // 定义1个二值信号量
QueueHandle_t lcd_queue;         // 定义1个消息队列

EventGroupHandle_t myeg = NULL;  // 定义1个事件组句柄
TimerHandle_t mytimer = NULL;     // 定义1个软件定时器句柄

extern char buf6[30];            // RTC时间缓冲区

//LED2-流水灯
TaskHandle_t handle_led2=NULL;
void Task_LED2(void *p)
{
	p=p;
	while(1){
		LEDx_ON(LED2_Port,LED2_Pin);
		LEDx_OFF(LED3_Port,LED3_Pin);
		LEDx_OFF(LED4_Port,LED4_Pin);
		vTaskDelay(200);
		
		LEDx_OFF(LED2_Port,LED2_Pin);
		LEDx_ON(LED3_Port,LED3_Pin);
		LEDx_OFF(LED4_Port,LED4_Pin);
		vTaskDelay(200);
		
		LEDx_OFF(LED2_Port,LED2_Pin);
		LEDx_OFF(LED3_Port,LED3_Pin);
		LEDx_ON(LED4_Port,LED4_Pin);
		vTaskDelay(200);
	}
}

/* ========== KEY1任务 -- 按键任务 ========== */
TaskHandle_t handle_key1 = NULL;
void Task_KEY1(void *p)
{
	uint8_t key = 0;
	p = p;
	u8 count=0;
	
	while(1){
		key = Key1_GetValue();
		if(key == 1){
			printf("key1被按下\r\n");

			count++;
			if(count==2) count=0;
			switch (count){
				case 0:lv_scr_load(myui.screen_1);break;
				case 1:lv_scr_load(myui.index);break;
			}
			
		}
		vTaskDelay(10);
	}
}

/* ========== DHT11任务 -- 温湿度传感器任务 ========== */
TaskHandle_t handle_dht11 = NULL;
void Task_DHT11(void *p)
{
	struct SENSOR_DATA dht11_data = {0};
	u8 *p1 = NULL;
	p = p;
	while(1){
		DHT11_Test(); // 获取温湿度数据
		// 给LCD消息队列发送消息
		dht11_data.sensor_id = SENSOR_ID_DHT11;
		// 拷贝温度(4字节 float)
		p1 = (u8 *)&tem;
		for(int i = 0; i < 4; i++){
			dht11_data.buff[i] = *p1;
			p1++;
		}
		// 拷贝湿度(4字节 float)
		p1 = (u8 *)&hum;
		for(int i = 0; i < 4; i++){
			dht11_data.buff[i + 4] = *p1;
			p1++;
		}
		xQueueSend(lcd_queue, &dht11_data, 0);
		vTaskDelay(2000); // 2s
	}
}

/* ========== KQM6600任务 -- 空气质量传感器任务 ========== */
TaskHandle_t handle_kqm6600 = NULL;
void Task_KQM6600(void *p)
{
	struct SENSOR_DATA kqm_data = {0};
	u8 *p1 = NULL;
	p = p;
	KQM6600_Config();
	while(1){
		// 通过串口中断接收完成，发送信号量
		xSemaphoreTake(kqm6600_sem, portMAX_DELAY);
		// 解析传感器数据
		KQM6600_DataAnaly();

		// 解析完成后，将CO2/CH2O/VOC打包发送给LCD任务
		kqm_data.sensor_id = SENSOR_ID_KQM6600;
		// 拷贝CO2(4字节 int)
		p1 = (u8 *)&kqm6600_data.CO2;
		for(int i = 0; i < 4; i++){
			kqm_data.buff[i] = *p1;
			p1++;
		}
		// 拷贝CH2O(4字节 float)
		p1 = (u8 *)&kqm6600_data.CH2O;
		for(int i = 0; i < 4; i++){
			kqm_data.buff[i + 4] = *p1;
			p1++;
		}
		// 拷贝VOC(4字节 float)
		p1 = (u8 *)&kqm6600_data.VOC;
		for(int i = 0; i < 4; i++){
			kqm_data.buff[i + 8] = *p1;
			p1++;
		}
		xQueueSend(lcd_queue, &kqm_data, 0);
	}
}

/* ========== BH1750任务 -- 光照传感器任务 ========== */
TaskHandle_t handle_bh1750 = NULL;
void Task_BH1750(void *p)
{
	struct SENSOR_DATA bh1750_data = {0};
	u8 *p1 = NULL;
	p = p;
	// IIC初始化
	BH1750_Config();
	while(1){
		// 获取光照值
		taskENTER_CRITICAL(); // 进入临界段
		BH1750_GetValue();
		taskEXIT_CRITICAL();  // 退出临界段
		printf("light:%d\r\n", light_value);

		// 将光照值发送给LCD任务
		bh1750_data.sensor_id = SENSOR_ID_BH1750;
		p1 = (u8 *)&light_value;
		for(int i = 0; i < 4; i++){
			bh1750_data.buff[i] = *p1;
			p1++;
		}
		xQueueSend(lcd_queue, &bh1750_data, 0);

		vTaskDelay(1000);
	}
}

/* ========== MQ2任务 -- 烟雾传感器任务 ========== */
TaskHandle_t handle_mq2 = NULL;
void Task_MQ2(void *p)
{
	struct SENSOR_DATA mq2_data = {0};
	u8 *p1 = NULL;
	p = p;
	// ADC初始化+DMA初始化
	ADC1_Config();
	ADC1_DMAConfig();
	while(1){
		// 获取烟雾值
		ADC1_GetValue3();

		// 将烟雾值发送给LCD任务
		mq2_data.sensor_id = SENSOR_ID_MQ2;
		p1 = (u8 *)&smoke_value;  // 假设ADC值为全局变量 adc_value
		for(int i = 0; i < 4; i++){
			mq2_data.buff[i] = *p1;
			p1++;
		}
		xQueueSend(lcd_queue, &mq2_data, 0);

		vTaskDelay(5000);
	}
}

//LCD任务
TaskHandle_t handle_lcd = NULL;//句柄
void Task_LCD(void *p)
{
	struct SENSOR_DATA recv_data = {0};
	p = p;
	while(1){
		//等待消息队列中有信息
		xQueueReceive(lcd_queue,&recv_data,portMAX_DELAY);
		//根据传感器id号，进行相应的数据解析
		if(recv_data.sensor_id == SENSOR_ID_DHT11){
			printf("lcd --- tem:%.2f\r\n",*(float *)&recv_data.buff[0]);
			printf("lcd --- hum:%.1f\r\n",*(float *)&recv_data.buff[4]);
			
			lv_label_set_text_fmt(myui.screen_1_label_tem_value,"%.2f",*(float *)&recv_data.buff[0]);
			lv_label_set_text_fmt(myui.screen_1_label_hum_value,"%.1f",*(float *)&recv_data.buff[4]);
		}else if(recv_data.sensor_id == SENSOR_ID_KQM6600){
			printf("lcd --- CO2:%d\r\n",*(int *)&recv_data.buff[0]);
			printf("lcd --- CH2O:%.2f\r\n",*(float *)&recv_data.buff[4]);
			printf("lcd --- VOC:%.2f\r\n",*(float *)&recv_data.buff[8]);
			
			lv_label_set_text_fmt(myui.screen_1_label_co2_value,"%d",*(int *)&recv_data.buff[0]);
			lv_label_set_text_fmt(myui.screen_1_label_ch2o_value,"%.2f",*(float *)&recv_data.buff[4]);
			lv_label_set_text_fmt(myui.screen_1_label_voc_value,"%.2f",*(float *)&recv_data.buff[8]);
		}else if(recv_data.sensor_id==SENSOR_ID_BH1750){
			printf("lcd --- light:%d\r\n",*(int *)&recv_data.buff[0]);
			
			lv_label_set_text_fmt(myui.screen_1_label_light_value,"%d",*(int *)&recv_data.buff[0]);
		}else if(recv_data.sensor_id==SENSOR_ID_MQ2){
			printf("lcd --- smoke:%d\r\n",*(int *)&recv_data.buff[0]);
		}
		
	}
}


/* ========== WIFI任务-- 联网上传数据功能 ========== */
TaskHandle_t handle_wifi = NULL;
void Task_WIFI(void *p)
{
	p=p;
	WIFI_ReadInfoFormFlash();
	
	while(1)
	{
		WIFI_Run();		
		if(wifi_state == 1)
		{
			vTaskDelay(2000);  // 联网后每2秒循环一次
		}
		else
		{
			vTaskDelay(10);
		}
	}
}
/* ========== SU03 ========== */
TaskHandle_t handle_su03 = NULL;
void Task_SU03(void *p)
{
	p=p;
	while(1)
	{
		SU03_DataAnaly();
		vTaskDelay(500);
	}
}
/* ========== RTC ========== */
TaskHandle_t handle_rtc = NULL;
void Task_RTC(void *p)
{
	p=p;
	struct SENSOR_DATA rtc_data = {0};
	u8 *p1=NULL;
	while(1)
	{
		RTC_GetTime();
		lv_label_set_text(myui.screen_1_label_time_value, time_str);
		lv_label_set_text(myui.index_label_index_time,time_str);
		vTaskDelay(1000);
	}
}
//更新LVGL页面
TaskHandle_t handle_lvgl = NULL;
void Task_LVGL(void *p)
{
	p=p;
	while(1)
	{
		lv_task_handler();//更新LVGL页面
		vTaskDelay(5);
	}
}


/* ========== Start任务 ========== */
TaskHandle_t handle_start = NULL;
void Task_Start(void *p)
{
	p = p;
	// 创建相关的任务间通信
	kqm6600_sem = xSemaphoreCreateBinary();  // 用于KQM6600传感器数据的采集
	lcd_queue   = xQueueCreate(10, sizeof(struct SENSOR_DATA));

	// 创建其他任务
	xTaskCreate(Task_LED2,"LED2",128, NULL, configMAX_PRIORITIES - 4, &handle_led2);
	printf("Task_LED2创建成功\r\n");
	xTaskCreate(Task_KEY1,"KEY1", 128, NULL, configMAX_PRIORITIES - 3, &handle_key1);
	printf("Task_KEY1创建成功\r\n");
	res=xTaskCreate(Task_DHT11,"DHT11", 128, NULL, 3,&handle_dht11);
	if(res==1)
		printf("Task_DHT11创建成功\r\n");
	xTaskCreate(Task_KQM6600,"KQM6600",128, NULL,10,&handle_kqm6600);
	printf("Task_KQM6600创建成功\r\n");
	xTaskCreate(Task_BH1750,"BH1750",  128, NULL, 11,&handle_bh1750);
	printf("Task_BH1750创建成功\r\n");
	xTaskCreate(Task_MQ2,"MQ2", 128, NULL, 9, &handle_mq2);
	printf("Task_MQ2创建成功\r\n");
	xTaskCreate(Task_LCD,"LCD",     256, NULL, configMAX_PRIORITIES - 6, &handle_lcd);
	printf("Task_LCD创建成功\r\n");
	res=xTaskCreate(Task_WIFI,"WIFI",   256, NULL, configMAX_PRIORITIES - 5, &handle_wifi);
	printf("Task_WIFI创建成功\r\n");
	res=xTaskCreate(Task_RTC,"RTC",   256, NULL, configMAX_PRIORITIES - 7, &handle_rtc);
	printf("Task_RTC创建成功\r\n");
	res=xTaskCreate(Task_SU03,"SU03",   256, NULL, configMAX_PRIORITIES - 8, &handle_su03);
	if(res==1)printf("Task_SU03创建成功\r\n");
	vTaskDelete(handle_start);
}


/* ========== BSP初始化 ========== */
void BSP_Config(void)
{
	// 中断分组 占先+次级 = 2+2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	USART1_Config(115200);
	printf("串口初始化成功\r\n");
	LED_Config_Lib();
	Key_Config_Lib();
	// 相关的传感器
	DHT11_ModeConfig(DHT11_MODE_OUT);
	// LCD屏初始化
	sFLASH_Init();
	WIFI_Config();
	SU03_Config();
	RTC_Config();
	if(wifi_state == 0)
		WIFI_StateChange(3);
//	BEEP_Config_Lib();
}


/* ========== 主函数 ========== */
int main()
{
	BSP_Config(); // 底层硬件初始化
	lv_init();
	lv_port_disp_init();
	//LVGL测试
	
	setup_ui(&myui);
	// 创建任务
	res = xTaskCreate(Task_Start, "START", 256, NULL, configMAX_PRIORITIES - 2, &handle_start);
	if(res == pdPASS){
		printf("START任务创建完成\r\n");
	}else{
		printf("START任务创建失败\r\n");
	}
	xTaskCreate(Task_LVGL, "LVGL", 512, NULL, configMAX_PRIORITIES - 4, &handle_lvgl);
	vTaskStartScheduler(); // 启动RTOS

	while(1)
	{
		
	}
}



/* ========== 空闲钩子函数 ========== */
void vApplicationIdleHook(void)
{
	LEDx_Toggle(LED1_Port, LED1_Pin); // LED灯状态翻转
	Delay_nms_nop(500);
//	vTaskDelay(500);
}


/* ========== USART2中断处理(KQM6600) ========== */
void USART2_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	u8 data = 0;

	// 判断是否是接收中断
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET){
		// 清中断标志位
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		// 数据处理
		data = USART2->DR;
		if(kqm6600_msg.rx_count < KQM6600_RX_LEN){
			kqm6600_msg.rx_buff[kqm6600_msg.rx_count] = data;
			kqm6600_msg.rx_count++;
		}
	}else if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_IDLE);
		data = USART2->DR; // 清除空闲中断标志位
		kqm6600_msg.over_flag = 1;
		// 发送一个信号量
		xSemaphoreGiveFromISR(kqm6600_sem, &xHigherPriorityTaskWoken);
	}

	// 退出中断时，进行上下文切换
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
//时钟节拍的钩子函数--给lvgl提供基本时钟
void vApplicationTickHook( void )
{
	lv_tick_inc(1);
}
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	printf("StackOverflow:%s\r\n",pcTaskName);
}
