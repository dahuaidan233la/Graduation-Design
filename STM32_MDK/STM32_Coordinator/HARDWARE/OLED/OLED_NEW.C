#include "OLED_NEW.H"
#include "OLED_TAB.H"

void ys(unsigned int i)
{
   while(i--);
}
/**********************************
作者：特纳斯电子
网站：https://www.mcude.com
联系方式：46580829(QQ)
淘宝店铺：特纳斯电子
  * 函数功能: OLED IIC  开始函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_IIC_Start()
{
   scl(1);
  ys(4);
   sda(1);
  ys(4);
   sda(0);
  ys(4);
   scl(0);

}

/**********************************************
//IIC Stop
**********************************************/
void OLED_IIC_Stop()
{
   sda(0);
  ys(4);
   scl(1);
  ys(4);
   sda(1);
}
/**********************************************
// IIC Write byte
**********************************************/
unsigned char OLED_Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char Ack_Bit;                    //应答信号
	for(i=0;i<8;i++)		
	{
		if(IIC_Byte & 0x80)		//1?0?
		{sda(1);}
		else
		{
		sda(0);
		}
		 ys(4);
		scl(1);
    	 ys(4);
		scl(0);
		ys(4);
		IIC_Byte<<=1;			//loop
	}
	 sda(1);		                //释放IIC SDA总线为主器件接收从器件产生应答信号	
	 ys(4);
	scl(1);                     //第9个时钟周期
	 ys(4);
	Ack_Bit = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13);		            //读取应答信号
	scl(0);
	return Ack_Bit;	
}  
void OLED_write_iic_com(unsigned char IIC_Command)
{
   OLED_IIC_Start();
   OLED_Write_IIC_Byte(0x78);            //Slave addre.ss,SA0=0
   OLED_Write_IIC_Byte(0x00);			//write command
   OLED_Write_IIC_Byte(IIC_Command); 
   OLED_IIC_Stop();
}


void OLED_write_iic_dat(unsigned char IIC_Data)
{
   OLED_IIC_Start();
   OLED_Write_IIC_Byte(0x78);			
   OLED_Write_IIC_Byte(0x40);			//write data
   OLED_Write_IIC_Byte(IIC_Data);
   OLED_IIC_Stop(); 
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{
	OLED_write_iic_com(0xb0+y);
	OLED_write_iic_com(((x&0xf0)>>4)|0x10);
	OLED_write_iic_com((x&0x0f)|0x01);
}
void OLED_Clear(void)  
{  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_write_iic_com (0xb0+i);
		OLED_write_iic_com (0x00);     
		OLED_write_iic_com (0x10);        
		for(n=0;n<128;n++)OLED_write_iic_dat(0); 
	}
} 
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_write_iic_com(0xb0+m);		//page0-page1
		OLED_write_iic_com(0x00);		//low column start address
		OLED_write_iic_com(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_write_iic_dat(fill_Data);
				ys(11110);
			}
	}
}
void OLED_Init(void) 
{
	//OLED_GPIO_Init();//引脚初始化

	OLED_write_iic_com(0xae);//--turn off oled panel
	OLED_write_iic_com(0x00);//---set low column address
	OLED_write_iic_com(0x10);//---set high column address
	OLED_write_iic_com(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_write_iic_com(0x81);//--set contrast control register
	OLED_write_iic_com(0xCF); // Set SEG Output Current Brightness
	OLED_write_iic_com(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_write_iic_com(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_write_iic_com(0xa6);//--set normal display
	OLED_write_iic_com(0xa8);//--set multiplex ratio(1 to 64)
	OLED_write_iic_com(0x3f);//--1/64 duty
	OLED_write_iic_com(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_write_iic_com(0x00);//-not offset
	OLED_write_iic_com(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_write_iic_com(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_write_iic_com(0xd9);//--set pre-charge period
	OLED_write_iic_com(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_write_iic_com(0xda);//--set com pins hardware configuration
	OLED_write_iic_com(0x12);
	OLED_write_iic_com(0xdb);//--set vcomh
	OLED_write_iic_com(0x40);//Set VCOM Deselect Level
	OLED_write_iic_com(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_write_iic_com(0x02);//
	OLED_write_iic_com(0x8d);//--set Charge Pump enable/disable
	OLED_write_iic_com(0x14);//--set(0x10) disable
	OLED_write_iic_com(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_write_iic_com(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_write_iic_com(0xaf);//--turn on oled panel
	OLED_Set_Pos(0,0);
} 

void OLED_Clear2(void)  
{  
	unsigned char i,n;		    
	for(i=4;i<8;i++)  
	{  
		OLED_write_iic_com (0xb0+i);
		OLED_write_iic_com (0x02);    
		OLED_write_iic_com (0x10);       
		for(n=0;n<128;n++)OLED_write_iic_dat(0); 
	} 
} 
//void OLED_ShowChar(unsigned char x,unsigned char y,unsigned int chr)
//{      	
//	unsigned int c=0,i=0;	
//	c=chr-' ';			
//		OLED_Set_Pos(x,y);	
//		for(i=0;i<8;i++)
//		OLED_write_iic_dat(F8X16[c*16+i]);
//		OLED_Set_Pos(x,y+1);
//		for(i=0;i<8;i++)
//		OLED_write_iic_dat(F8X16[c*16+i+8]);
//}
/****
*******OLED显示单个字符函数
*****/
void Oled_ShowChar(unsigned char x,unsigned char y,unsigned int chr)
{      	
	unsigned int c=0,i=0;	
	c=chr-' ';			
	OLED_Set_Pos(x,y);	
	for(i=0;i<8;i++)
		OLED_write_iic_dat(F8X16[c*16+i]);
	OLED_Set_Pos(x,y+1);
	for(i=0;i<8;i++)
		OLED_write_iic_dat(F8X16[c*16+i+8]);
}
/****
*******OLED显示字符串函数
*****/
void Oled_ShowString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned int j=0;
	while (chr[j]!='\0')
	{		
		Oled_ShowChar(x,y,chr[j]);
		x+=8;
		if(x>120){x=0;y+=2;}
		j++;
	}
}
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~65525);	 		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2)
{         	
	unsigned char t,temp;
	unsigned char enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				Oled_ShowChar(x+(size2/2)*t,y,'0');
				continue;
			}else enshow=1; 
		 	 
		}
	 	Oled_ShowChar(x+(size2/2)*t,y,temp+'0'); 
	}
} 
/****
*******OLED显示中文函数
*****/
void Oled_ShowCHinese(unsigned char x,unsigned char y,unsigned char *p)
{      			    
	unsigned char t,wordNum;
	while(*p != '\0')
	{
		for(wordNum=0;wordNum<(sizeof(Hzk)/34+1);wordNum++)    //sizeof(Hzk)/34+1个字
		{
			if(Hzk[wordNum].Char[0]== *p && Hzk[wordNum].Char[1]== *(p+1))
			{
				OLED_Set_Pos(x,y);
				for(t=0;t<16;t++)
				{
					OLED_write_iic_dat(Hzk[wordNum].Hex[t]);
				}
				OLED_Set_Pos(x,y+1);
				for(t=0;t<16;t++)
				{
					OLED_write_iic_dat(Hzk[wordNum].Hex[t+16]);
				}
				break;
			}
		}
		p += 2;
		x+=16;
	}
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char *p)
{
 	unsigned char x,y;
 	if(y1%8==0) y=y1/8;      
 	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
   	 	for(x=x0;x<x1;x++)
	    {      
	    	OLED_write_iic_dat(*p++);	    	
	    }
	}
}
void OLED_Drwa_QRCode(void)//二维码显示函数（64X64）
{
	uint16_t i, j;
	for(j=0; j<8; j++)
	{
		OLED_Set_Pos(32, j);
		for(i=0; i<64; i++)
		{
			OLED_write_iic_dat(*(unsigned char *)((QRCode_HEX)+i+j*64));
		}
	}
}
void OLED_Drwa_NUM_Plus(unsigned char x,unsigned char num)//数字显示函数（64X64）
{
	 unsigned char i, j;
	for(j=0; j<8; j++)
	{
		OLED_Set_Pos(x, j);
		for(i=0; i<32; i++)
			OLED_write_iic_dat(ZiMo_64X64[num][j*32 + i]);
	}
}
void point(unsigned char x,unsigned char y)
{
  unsigned char y1,y2;
   y1=y/8;
   y2=y%8;
	 OLED_Set_Pos(x+2,y1);
	 OLED_write_iic_dat(0x01<<y2);
}
