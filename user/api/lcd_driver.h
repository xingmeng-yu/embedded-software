#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

#include "stm32f10x.h"
#define LCD_SPI  0
#define LCD_FSMC 1

//LCD参数
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;					//LCD ID
	u8  dir;				//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	wramcmd;		//开始写gram指令
	u16  setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令 
}_lcd_dev; 	 

extern _lcd_dev lcddev;

#if LCD_SPI
/*
			LCD屏                                     STM32         模式
M_LCD_RES  -- LCD屏复位管脚（低电平有效          PB12     通用推挽输出模式
M_LCD_CS   -- LCD屏片选管脚                      PB11     通用推挽输出模式
M_LCD_DC   -- 数据/命令选择端                    PB10     通用推挽输出模式
M_LCD_SCL  -- SPI1_CLK                           PA5     复用推挽输出模式（SPI1功能）
M_LCD_SDI   -- SPI1_DIN                          PA7     复用推挽输出模式（SPI1功能） 
M_LCD_SDO  -- SPI1_DOUT                          PA6     浮空输入模式
*/
#define LCD_RES_Port   GPIOB
#define LCD_RES_Pin    GPIO_Pin_12

#define LCD_CS_Port   GPIOB
#define LCD_CS_Pin    GPIO_Pin_11

#define LCD_DC_Port   GPIOB
#define LCD_DC_Pin    GPIO_Pin_10


#define LCD_RES_High (GPIO_SetBits(LCD_RES_Port,LCD_RES_Pin))
#define LCD_RES_Low  (GPIO_ResetBits(LCD_RES_Port,LCD_RES_Pin))

#define LCD_CS_High (GPIO_SetBits(LCD_CS_Port,LCD_CS_Pin))
#define LCD_CS_Low  (GPIO_ResetBits(LCD_CS_Port,LCD_CS_Pin))

#define LCD_DC_High (GPIO_SetBits(LCD_DC_Port,LCD_DC_Pin))
#define LCD_DC_Low  (GPIO_ResetBits(LCD_DC_Port,LCD_DC_Pin))

//底层固件函数
void LCD_GPIOConfig(void);
void LCD_SPI1Config(void);
uint8_t SPI1_SendAndRecvData(uint8_t data);
void LCD_ILI9341_CMD(uint8_t cmd);
void LCD_ILI9341_Parameter(uint8_t data);
void LCD_ILI9341_Parameter16bit(uint16_t data);
#endif
#if LCD_FSMC 
//LCD 背光控制
#define	LCD_LED(x)	(x?(GPIO_SetBits(GPIOB, GPIO_Pin_0)):(GPIO_ResetBits(GPIOB, GPIO_Pin_0)))	
//LCD的FSMC地址结构体
typedef struct {
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;
//使用NOR/SRAM的 Bank1.sector4,地址位HADDR 11位 -- FSMC_A10作为数据命令区分线 
//注意设置时STM32内部会右移一位对其! 			                0000 0000 0010    
#define LCD_BASE        ((u32)(0x6C000000 | 0x000007FE))//0111 1111 1110  >>1    DC=0
#define LCD             ((LCD_TypeDef *) LCD_BASE)			//1000 0000 0000  >>1		 DC=1
//                        //HADDR 7位 --- FSMC_A6         //
//#define LCD_BASE        ((u32)(0x6C000000 | 0x0000007E))//0000 0111 1110  >>1    DC=0
//#define LCD             ((LCD_TypeDef *) LCD_BASE)			//0000 1000 0000  >>1		 DC=1
#endif                                                    

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

//SSD LCD分辨率设置
#define SSD_HOR_RESOLUTION		800		//LCD水平分辨率
#define SSD_VER_RESOLUTION		480		//LCD垂直分辨率
//SSD LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH		1		//水平脉宽
#define SSD_HOR_BACK_PORCH		46		//水平前廊
#define SSD_HOR_FRONT_PORCH		210		//水平后廊

#define SSD_VER_PULSE_WIDTH		1		//垂直脉宽
#define SSD_VER_BACK_PORCH		23		//垂直前廊
#define SSD_VER_FRONT_PORCH		22		//垂直前廊
//SSD LCD的几个参数
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)


//LCD屏幕扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	 

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

//底层驱动函数
void LCD_DriverConfig(void);
void LCD_Scan_Dir(u8 dir);
void LCD_Display_Dir(u8 dir);
void LCD_Config(void);
void LCD_Address_Set(u16 xsc,u16 ysp,u16 xec,u16 yep);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
//上层应用函数
void LCD_Fill_Color(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 *color);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_DrawPoint(u16 x,u16 y,u16 color);
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);
void Draw_Circlemy(u16 x0,u16 y0,u8 r,u16 color);
void LCD_ShowxXx(u16 x,u16 y,u16 w,u16 h,u16 fc,u16 bc,u8 *data);
void LCD_ShowPicture(u16 x,u16 y,u16 w,u16 h,u8 *data);
void LCD_Clear(u16 color);
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);

//显示
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode);
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode);
void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);
void LCD_ShowAll(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);
#endif
