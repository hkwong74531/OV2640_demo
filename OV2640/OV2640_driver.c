#include "OV2640_driver.h"

uint8_t image_framebuf[40*1024];

uint8_t OV2640_Init(void)
{
	uint16_t reg;

	OV2640_power(1);
    OV2640_reset();
	OV2640_write_register(OV2640_DSP_RA_DLMT, 0x01);
 	OV2640_write_register(OV2640_SENSOR_COM7, 0x80);
	delay_ms(50); 

	reg = OV2640_read_register(OV2640_SENSOR_MIDH);
	reg<<=8;
	reg |= OV2640_read_register(OV2640_SENSOR_MIDL);
	if(reg != OV2640_MID)
	{
		PRINTF("MID:%d\r\n",reg);
		return 1;
	}
	reg = OV2640_read_register(OV2640_SENSOR_PIDH);
	reg<<=8;
	reg|= OV2640_read_register(OV2640_SENSOR_PIDL);
	if(reg != OV2640_PID)
	{
		PRINTF("HID:%d\r\n",reg);
		return 2;
	}   

	for(uint16_t i=0;i<sizeof(ov2640_uxga_init_reg_tbl)/2;i++)
	{
	   	OV2640_write_register(ov2640_uxga_init_reg_tbl[i][0],ov2640_uxga_init_reg_tbl[i][1]);
 	}

  	return 0; 	//ok
}

void OV2640_JPEG_Mode(void) 
{
	uint16_t i = 0;
	//YUV422
	for(i=0;i<(sizeof(ov2640_yuv422_reg_tbl)/2);i++)
	{
		OV2640_write_register(ov2640_yuv422_reg_tbl[i][0],ov2640_yuv422_reg_tbl[i][1]); 
	} 
	//JPEG
	for(i=0;i<(sizeof(ov2640_jpeg_reg_tbl)/2);i++)
	{
		OV2640_write_register(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);  
	}  
}

void OV2640_RGB565_Mode(void) 
{
	uint16_t i = 0;
	//RGB565
	for(i=0;i<(sizeof(ov2640_rgb565_reg_tbl)/2);i++)
	{
		OV2640_write_register(ov2640_rgb565_reg_tbl[i][0],ov2640_rgb565_reg_tbl[i][1]); 
	} 
}

//AE level:0~4
void OV2640_Auto_Exposure(uint8_t level)
{  
	uint8_t i;
	uint8_t *p=(uint8_t*)OV2640_AUTOEXPOSURE_LEVEL[level];
	for(i=0;i<4;i++)
	{ 
		OV2640_write_register(p[i*2],p[i*2+1]); 
	} 
}

//0:auto
//1:sunny
//2,cloudy
//3,office
//4,home
void OV2640_Light_Mode(uint8_t mode)
{
	uint8_t regccval=0X5E;//Sunny 
	uint8_t regcdval=0X41;
	uint8_t regceval=0X54;
	switch(mode)
	{ 
		case 0://auto 
			OV2640_write_register(0XFF,0X00);	 
			OV2640_write_register(0XC7,0X00);//AWB ON 
			return;  	
		case 2://cloudy
			regccval=0X65;
			regcdval=0X41;
			regceval=0X4F;
			break;	
		case 3://office
			regccval=0X52;
			regcdval=0X41;
			regceval=0X66;
			break;	
		case 4://home
			regccval=0X42;
			regcdval=0X3F;
			regceval=0X71;
			break;	
	}
	OV2640_write_register(0XFF,0X00);	 
	OV2640_write_register(0XC7,0X40);	//AWB OFF 
	OV2640_write_register(0XCC,regccval); 
	OV2640_write_register(0XCD,regcdval); 
	OV2640_write_register(0XCE,regceval);  
}

//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Color_Saturation(uint8_t sat)
{ 
	uint8_t reg7dval=((sat+2)<<4)|0X08;
	OV2640_write_register(0XFF,0X00);		
	OV2640_write_register(0X7C,0X00);		
	OV2640_write_register(0X7D,0X02);				
	OV2640_write_register(0X7C,0X03);			
	OV2640_write_register(0X7D,reg7dval);			
	OV2640_write_register(0X7D,reg7dval); 		
}

//0:(0X00)-2
//1:(0X10)-1
//2,(0X20) 0
//3,(0X30)+1
//4,(0X40)+2
void OV2640_Brightness(uint8_t bright)
{
	OV2640_write_register(0xff, 0x00);
	OV2640_write_register(0x7c, 0x00);
	OV2640_write_register(0x7d, 0x04);
	OV2640_write_register(0x7c, 0x09);
	OV2640_write_register(0x7d, bright<<4); 
	OV2640_write_register(0x7d, 0x00); 
}

//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Contrast(uint8_t contrast)
{
	uint8_t reg7d0val=0X20;
	uint8_t reg7d1val=0X20;
  	switch(contrast)
	{
		case 0://-2
			reg7d0val=0X18;	 	 
			reg7d1val=0X34;	 	 
			break;	
		case 1://-1
			reg7d0val=0X1C;	 	 
			reg7d1val=0X2A;	 	 
			break;	
		case 3://1
			reg7d0val=0X24;	 	 
			reg7d1val=0X16;	 	 
			break;	
		case 4://2
			reg7d0val=0X28;	 	 
			reg7d1val=0X0C;	 	 
			break;	
	}
	OV2640_write_register(0xff,0x00);
	OV2640_write_register(0x7c,0x00);
	OV2640_write_register(0x7d,0x04);
	OV2640_write_register(0x7c,0x07);
	OV2640_write_register(0x7d,0x20);
	OV2640_write_register(0x7d,reg7d0val);
	OV2640_write_register(0x7d,reg7d1val);
	OV2640_write_register(0x7d,0x06);
}

//0,Normal
//1,Negative
//2,B&W 
//3,Reddish
//4,Greenish
//5,Bluish
//6,Antique    
void OV2640_Special_Effects(uint8_t eft)
{
	uint8_t reg7d0val=0X00;
	uint8_t reg7d1val=0X80;
	uint8_t reg7d2val=0X80; 
	switch(eft)
	{
		case 1://Negative
			reg7d0val=0X40; 
			break;	
		case 2://B&W
			reg7d0val=0X18; 
			break;	 
		case 3://Reddish
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XC0; 
			break;	
		case 4://Greenish
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0X40; 
			break;	
		case 5://Bluish
			reg7d0val=0X18; 
			reg7d1val=0XA0;
			reg7d2val=0X40; 
			break;	
		case 6://Antique
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XA6; 
			break;	 
	}
	OV2640_write_register(0xff,0x00);
	OV2640_write_register(0x7c,0x00);
	OV2640_write_register(0x7d,reg7d0val);
	OV2640_write_register(0x7c,0x05);
	OV2640_write_register(0x7d,reg7d1val);
	OV2640_write_register(0x7d,reg7d2val); 
}

//sw:0,
//   1,
void OV2640_Color_Bar(uint8_t sw)
{
	uint8_t reg;
	OV2640_write_register(0XFF,0X01);
	reg=OV2640_read_register(0X12);
	reg&=~(1<<1);
	if(sw)reg|=1<<1; 
	OV2640_write_register(0X12,reg);
}

void OV2640_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t endx;
	uint16_t endy;
	uint8_t temp; 
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;
	
	OV2640_write_register(0XFF,0X01);			
	temp=OV2640_read_register(0X03);
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	OV2640_write_register(0X03,temp);
	OV2640_write_register(0X19,sy>>2);
	OV2640_write_register(0X1A,endy>>2);
	
	temp=OV2640_read_register(0X32);
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	OV2640_write_register(0X32,temp);
	OV2640_write_register(0X17,sx>>3);
	OV2640_write_register(0X18,endx>>3);
}

uint8_t OV2640_OutSize_Set(uint16_t width,uint16_t height)
{
	uint16_t outh;
	uint16_t outw;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	OV2640_write_register(0XFF,0X00);	
	OV2640_write_register(0XE0,0X04);			
	OV2640_write_register(0X5A,outw&0XFF);
	OV2640_write_register(0X5B,outh&0XFF);
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	OV2640_write_register(0X5C,temp);
	OV2640_write_register(0XE0,0X00);	
	return 0;
}

uint8_t OV2640_ImageWin_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{
	uint16_t hsize;
	uint16_t vsize;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	OV2640_write_register(0XFF,0X00);	
	OV2640_write_register(0XE0,0X04);					
	OV2640_write_register(0X51,hsize&0XFF);
	OV2640_write_register(0X52,vsize&0XFF);
	OV2640_write_register(0X53,offx&0XFF);
	OV2640_write_register(0X54,offy&0XFF);
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07; 
	OV2640_write_register(0X55,temp);
	OV2640_write_register(0X57,(hsize>>2)&0X80);
	OV2640_write_register(0XE0,0X00);	
	return 0;
} 

uint8_t OV2640_ImageSize_Set(uint16_t width,uint16_t height)
{ 
	uint8_t temp; 
	OV2640_write_register(0XFF,0X00);			
	OV2640_write_register(0XE0,0X04);			
	OV2640_write_register(0XC0,(width)>>3&0XFF);
	OV2640_write_register(0XC1,(height)>>3&0XFF);
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80; 
	OV2640_write_register(0X8C,temp);	
	OV2640_write_register(0XE0,0X00);				 
	return 0;
}

void OV2640_FrameRate_Ctrl(void)
{
	// for horizontal resolution 320 pixels
	OV2640_write_register(0XFF,0X00);		
	OV2640_write_register(0XD3,15);
	OV2640_write_register(0XFF,0X01);
	OV2640_write_register(0X11,3);
}

void OV2640_Preview_Loop(void)
{
	uint8_t linetmp;
	while(href_loop)	// VSYNC high
    {
		uint32_t pixtmp;

		pixtmp = pixcnt;
        // skip pixels for lower vertical resolution
        if(!(linecnt%2))
        {
			linetmp = 0;
			while(OV2640_HREF_read())
			{
				while(!OV2640_PCLK_read());
                if(!OV2640_HREF_read())
				{
					break;
                }
				image_framebuf[pixcnt] = OV2640_DATA;
				pixcnt++;
				while(OV2640_PCLK_read());

				while(!OV2640_PCLK_read());
				image_framebuf[pixcnt] = OV2640_DATA;
				pixcnt++;
				while(OV2640_PCLK_read());

				// skip pixels for lower horizontal resolution
				while(!OV2640_PCLK_read());
                linetmp = 1;
				while(OV2640_PCLK_read());
				while(!OV2640_PCLK_read());
				while(OV2640_PCLK_read());
			}
		}
        else
        {
			linetmp = 0;
			while(OV2640_HREF_read())
			{
				linetmp = 1; 
			}			
        }
		if(linetmp)
        {
			linetmp = 0;
			linecnt++;
        }
    }
}

void OV2640_JPG_Data(void)
{
	uint16_t jpeglen;

    jpeglen = 0;

	OV2640_VSYNC_int_disable();
	while(!OV2640_VSYNC_read());

	OV2640_JPEG_Mode();
  	OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT); 

	OV2640_write_register(0XFF,0X00);
	OV2640_write_register(0XD3,30);
	OV2640_write_register(0XFF,0X01);
	OV2640_write_register(0X11,0X1); 

	for(int8_t i=0; i<10; i++)
	{
		while(OV2640_VSYNC_read());	 
		while(!OV2640_VSYNC_read());	  
	}  

	while(OV2640_VSYNC_read())
	{
		while(OV2640_HREF_read())
        {
			while(!OV2640_PCLK_read());
			image_framebuf[jpeglen] = OV2640_DATA;
			while(OV2640_PCLK_read());
			jpeglen++;
        }
	}
    OV2640_VSYNC_int_enable();
}  


