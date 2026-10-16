#include "adc.h"
#include "stdio.h"

void ADC1_Config(void)
{
	//1.PA5 PC1配置为模拟输入 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	//2.ADC1配置
	//配置ADC时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//初始化ADC1_IN5
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	ADC_InitTypeDef ADC_InitStruct = {0};
//	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;//不连续
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//连续
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//不使用外部触发，使用软件触发
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_InitStruct.ADC_NbrOfChannel = 2;//转换一个通道
//	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//不扫描
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//不扫描
	ADC_Init(ADC1,&ADC_InitStruct);
	//3.ADC1通道的配置 - ADC1_IN5 规则组的位置以及采样周期
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,1,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11,2,ADC_SampleTime_239Cycles5);
	//4.ADC1使能
	ADC_Cmd(ADC1,ENABLE);
	//使用DMA模式
	ADC1_DMAConfig();
	
	//启动ADC -- 连续模式下，启动1次
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}
//获取ADC1转换后的值
//单次模式 -- 启动1次，转换1次，转换完，停止。
void ADC1_GetValue1(void)
{
	u16 value = 0;
	//启动ADC
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	//等待ADC转换完成
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	//打印
	value = ADC_GetConversionValue(ADC1);
	printf("illu_r=%d,",value);
}

//获取ADC1转换后的值
//连续模式
void ADC1_GetValue2(void)
{
	u16 value = 0;
	//打印
	value = ADC_GetConversionValue(ADC1);
	printf("illu_r=%d,",value);
}

/***************************配置ADC1的DMA*****************************/
//ADC1数据源  adc_buff目标
#define ADC_COUNT 10
u16 adc_buff[20]={0};//2个通道IN5 IN11
void ADC1_DMAConfig(void)
{
	//1.打开ADC1的DMA功能
	ADC_DMACmd(ADC1,ENABLE);
	//2.配置DMA1_CH1
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_InitTypeDef DMA_InitStruct = {0};
	DMA_InitStruct.DMA_BufferSize = 2*ADC_COUNT;//要传输的数据次数
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;//外设->存储器
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//不是存储器到存储器
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&adc_buff[0];
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据宽度
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//地址增加
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;//循环模式
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//外设地址
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;//优先级很高
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);
	//使能DMA
	DMA_Cmd(DMA1_Channel1,ENABLE);
}
//平均滤波
u16 smoke_value = 0;
void ADC1_GetValue3(void)
{
	int i = 0;
	u32 sum1= 0,sum2 = 0;
	for(i=0;i<10;i++)
	{
		if(i%2 == 0){
			sum1 += adc_buff[i];//0 2 4 6 8 10 12 14 16 18 
		}else{
			sum2 += adc_buff[i];
		}
	}
	printf("illu_r=%d\r\n",sum1/10);
	smoke_value = sum2/10;
	printf("smoke=%d\r\n",smoke_value);
}
	











