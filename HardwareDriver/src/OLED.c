#include "OLED.h"


static uint8_t OLED_GRAM[128][8] = {0};


static void OLED_WrDat(unsigned char IIC_Data);
static void OLED_WrCmd(unsigned char IIC_Command);

extern const unsigned char F6x8[92][6];
extern const unsigned char Pic6x8[1][6];


void OLED_InitConfig(void)
{
	OLED_WrCmd(0xAE); //display off
    OLED_WrCmd(0x20); //Set Memory Addressing Mode    
    OLED_WrCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    OLED_WrCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WrCmd(0xC8); //Set COM Output Scan Direction
    OLED_WrCmd(0x00); //---set low column address
    OLED_WrCmd(0x10); //---set high column address
    OLED_WrCmd(0x40); //--set start line address
    OLED_WrCmd(0x81); //--set contrast control register
    OLED_WrCmd(0xFF); //亮度调节 0x00~0xff
    OLED_WrCmd(0xA1); //--set segment re-map 0 to 127
    OLED_WrCmd(0xA6); //--set normal display
    OLED_WrCmd(0xA8); //--set multiplex ratio(1 to 64)
    OLED_WrCmd(0x3F); //
    OLED_WrCmd(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    OLED_WrCmd(0xD3); //-set display offset
    OLED_WrCmd(0x00); //-not offset
    OLED_WrCmd(0xD5); //--set display clock divide ratio/oscillator frequency
    OLED_WrCmd(0xF0); //--set divide ratio
    OLED_WrCmd(0xD9); //--set pre-charge period
    OLED_WrCmd(0x22); //
    OLED_WrCmd(0xDA); //--set com pins hardware configuration
    OLED_WrCmd(0x12);
    OLED_WrCmd(0xDB); //--set vcomh
    OLED_WrCmd(0x20); //0x20,0.77xVcc
    OLED_WrCmd(0x8D); //--set DC-DC enable
    OLED_WrCmd(0x14); //
    OLED_WrCmd(0xAF); //--turn on oled panel
	
	OLED_ClearGRAM(0);
	OLED_ClearDisplay(0);
	OLED_RefreshGRAM();
}


void OLED_ON(void)
{
    OLED_WrCmd(0x8D);  //设置电荷泵
    OLED_WrCmd(0x14);  //开启电荷泵
    OLED_WrCmd(0xAF);  //OLED唤醒
}


void OLED_SetPos(uint8_t x, uint8_t y)
{ 
	OLED_WrCmd(0xB0 + y);
	OLED_WrCmd(((x & 0xF0) >> 4) | 0x10);
	OLED_WrCmd((x & 0x0f) | 0x01);
}


void OLED_RefreshGRAM(void)
{
	uint8_t i, n;		    
	for(i = 0;i < 8; i++)  
	{  
		OLED_WrCmd(0xB0 + i);    //设置页地址（0~7）
		OLED_WrCmd(0x04);      	//设置显示位置—列低地址
		OLED_WrCmd(0x10);      	//设置显示位置—列高地址   
		for(n = 0 ;n < 128; n++)
		{
			OLED_WrDat(OLED_GRAM[n][i]); 
		}
	}
}


void OLED_ClearDisplay(uint8_t color)
{
	uint8_t i, n, temp;

	temp = color ? 0xFF : 0x00;	
	for(i = 0;i < 8; i++)  
	{  
		OLED_WrCmd(0xB0 + i);    //设置页地址（0~7）
		OLED_WrCmd(0x04);      	//设置显示位置—列低地址
		OLED_WrCmd(0x10);      	//设置显示位置—列高地址   
		for(n = 0 ;n < 128; n++)
		{
			OLED_WrDat(temp); 
		}
	}
}


void OLED_ClearGRAM(uint8_t color)
{  
	uint8_t i, n, temp;
	
	temp = color ? 0xFF : 0x00;
	for(i = 0; i < 8; i++)
	{
		for(n = 0;n < 128; n++)
		{
			OLED_GRAM[n][i] = temp;
		}
	}
}


void OLED_DrawF6x8Pic(uint8_t x, uint8_t y, char chr)
{
	uint8_t i;
	uint8_t m, n, k;		   
    for(i = 0 ;i < 6; ++i)
    {
		m = y / 8;
		n = y % 8;
		k = 8 - n;
		if(n == 0)
		{
			OLED_GRAM[x + i][m] = Pic6x8[chr][i];
		}
		else
		{
			OLED_GRAM[x + i][m] = (OLED_GRAM[x + i][m] & (0xFF >> k)) | (Pic6x8[chr][i] << n);
			OLED_GRAM[x + i][m + 1] = (OLED_GRAM[x + i][m + 1] & (0xFF << n)) | (Pic6x8[chr][i] >> k);
		}
	}         
}


void OLED_DrawF6x8Char(uint8_t x, uint8_t y, char chr)
{
	uint8_t i;
	uint8_t m, n, k;
	chr -= ' ';//得到偏移后的值				   
    for(i = 0 ;i < 6; ++i)
    {
		m = y / 8;
		n = y % 8;
		k = 8 - n;
		if(n == 0)
		{
			OLED_GRAM[x + i][m] = F6x8[chr][i];
		}
		else
		{
			OLED_GRAM[x + i][m] = (OLED_GRAM[x + i][m] & (0xFF >> k)) | (F6x8[chr][i] << n);
			OLED_GRAM[x + i][m + 1] = (OLED_GRAM[x + i][m + 1] & (0xFF << n)) | (F6x8[chr][i] >> k);
		}
	}          
}


void OLED_DrawF6x8String(uint8_t x, uint8_t y, char *chr)
{
	while(*chr != 0)
	{
		OLED_DrawF6x8Char(x, y, *chr++);
		if(x > 110)
		{
			x = 0;
			y += 9;
		}
		else
		{
			x += 7;
		}
		
		if(y > 50)
		{
			break;
		}
	}
}






#define IIC_SCL_L		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)
#define IIC_SCL_H		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)
#define IIC_SDA_L		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)
#define IIC_SDA_H		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)


#define IIC_DELAY


static void IIC_Delay(uint32_t time)
{
	while(time--);
}


static void IIC_Start()
{
   IIC_SCL_H;		
   IIC_SDA_H;
   IIC_SDA_L;
   IIC_SCL_L;
}

/**********************************************
//IIC Stop
**********************************************/
static void IIC_Stop()
{
   IIC_SCL_L;
   IIC_SDA_L;
   IIC_SCL_H;
   IIC_SDA_H;
}

/**********************************************
// 通过I2C总线写一个字节
**********************************************/
static void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
  for(i=0;i<8;i++)
	{
		if(IIC_Byte & 0x80)
			IIC_SDA_H;
		else
			IIC_SDA_L;
		IIC_SCL_H;
        IIC_DELAY;  //必须有保持SCL脉冲的延时
		IIC_SCL_L;
		IIC_Byte<<=1;
	}
    //原程序这里有一个拉高SDA，根据OLED的DATASHEET，此句必须去掉。
	IIC_SCL_H;
    IIC_DELAY;
	IIC_SCL_L;
}

/*********************OLED写数据************************************/ 
static void OLED_WrDat(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);
	Write_IIC_Byte(0x40);			//write data
	Write_IIC_Byte(IIC_Data);
	IIC_Stop();
}
/*********************OLED写命令************************************/
static void OLED_WrCmd(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);            //Slave address,SA0=0
	Write_IIC_Byte(0x00);			//write command
	Write_IIC_Byte(IIC_Command);
	IIC_Stop();
} 








