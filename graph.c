#include <os.h>
#include "char.h"
#include "math.h"
#include "graph.h"
#include "image.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define GetR(c) (((c) & 0xF800) >> 11 )
#define GetG(c) (((c) & 0x07E0) >> 5 )
#define GetB(c) (((c) & 0x001F) )

const char RedLUT[16] =
{
	0,0,0,0,0,0,0,0,1,3,5,7,9,11,13,15
};

void Delay(unsigned int x)
{
	volatile unsigned int i;
	volatile unsigned int j;
	for (i=0;i<1000;i++)
	{
		for (j=0;j<x;j++)
		{
			asm("nop");
		}
	}
}

char * init_VRAM() 
{
	return malloc( SCREEN_BYTES_SIZE );
}

void AllClr_VRAM(char * VRAM)
{
	if (is_cx)
		memset(VRAM,0x00,SCREEN_BYTES_SIZE);
	else
		memset(VRAM,0xff,SCREEN_BYTES_SIZE);
}


void PutDisp_DDVRAM(char * VRAM)
{	memcpy(SCREEN_BASE_ADDRESS,VRAM,SCREEN_BYTES_SIZE);}

void close_VRAM(char * VRAM){free(VRAM);}

unsigned int ColorConverter_CX2CLASSIC(unsigned int color)
{
	unsigned int c;
	c=(RedLUT[GetR(color)>>1])|(GetB(color)>>1);
	c=15-c;
	return c;
}

int LCD_Point( char * VRAM , int x , int y , unsigned short int color )
{
	if( x < 0 || x >= 320 || y < 0 || y >= 240 )
		return -1;
	if (is_cx)
	{
		unsigned short int * p = VRAM;
		p[x+y*320] = color;
	}else
	{
		unsigned char * p = (unsigned char *)VRAM + (x >> 1) + ( y << 7 ) + (y << 5 );
		color = ColorConverter_CX2CLASSIC(color);
		*p = ( x & 1) ? ((*p & 0xf0 ) |color ) : (( *p & 0x0f ) | (color << 4 ));
	}
	return 1;
}

int LCD_Point_Classic( char * VRAM , int x , int y , char color )
{
	if( x < 0 || x >= 320 || y < 0 || y >= 240 )
		return -1;
	unsigned char * p = VRAM + (x >> 1) + ( y << 7 ) + (y << 5 );
	*p = ( x & 1) ? ((*p & 0xf0 ) | color ) : (( *p & 0x0f ) | ( color << 4 ));
	return 1;
}

int GetPoint_VRAM(char * VRAM,int x, int y) 
{
	unsigned short int * p = VRAM;
	if(x >= 0 && y >= 0 &&x < 320 && y < 240 )return (p[x+y*320]&0x1F)>>1;
	return 0 ;
}

unsigned short int LCD_GetPoint(char * VRAM,int x, int y) 
{
	unsigned short int * p = VRAM;
	if(x >= 0 && y >= 0 &&x < 320 && y < 240 )return (p[x+y*320]);
	return 0 ;
}

void AreaClr_VRAM(char * VRAM , int x1 , int y1 , int x2 , int y2 )
{
	int y ;
	for( ; x1 <= x2 ; x1 ++ )
		for( y = y1; y <= y2 ; y++ )
			LCD_Point( VRAM , x1 , y , 0xFFFF  );
}

void AreaRev_VRAM(char * VRAM , int x1 , int y1 , int x2 , int y2 )
{
	int y ;
	for( ; x1 <= x2 ; x1 ++ )
		for( y = y1; y <= y2 ; y++ )
			LCD_Point( VRAM , x1 , y , 0xFFFF - LCD_GetPoint( VRAM, x1 , y)  );
}

void LCD_FillAll(char * VRAM,unsigned int color)
{
	int x , y ;
	for( x = 0 ; x < 320 ; x ++)
		for(y = 0; y < 240 ; y ++)
			LCD_Point(VRAM, x , y , color );
}

void LCD_XLine(char * VRAM,unsigned int x0,unsigned int y0,unsigned int x1,unsigned int c)
{
		unsigned int i,xx0,xx1;
	
		xx0=MIN(x0,x1);
		xx1=MAX(x0,x1);
		for (i=xx0;i<=xx1;i++)
		{
				LCD_Point(VRAM,i,y0,c);
		}
}

void LCD_YLine(char * VRAM,unsigned int x0,unsigned int y0,unsigned int y1,unsigned int c)
{
		unsigned int i,yy0,yy1;
	
		yy0=MIN(y0,y1);
		yy1=MAX(y0,y1);
		for (i=yy0;i<=yy1;i++)
		{
				LCD_Point(VRAM,x0,i,c);
		}
}

void LCD_Line(char * VRAM,unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned int color)
{
  int temp;
  int dx,dy;               //定义起点到终点横向纵向的差值
  int s1,s2,status,i;
  int Dx,Dy,sub;

  dx=x1-x0;
  if(dx>=0)                 //X方向增加
    s1=1;
  else                     //X方向减少
    s1=-1;     
  dy=y1-y0;                 //同理
  if(dy>=0)
    s2=1;
  else
    s2=-1;

  Dx=abs(x1-x0);             //计算增加的绝对值
  Dy=abs(y1-y0);
  if(Dy>Dx)                              
  {                     //以45度为分界线,靠近Y轴status=1,靠近X轴status=0 
    temp=Dx;
    Dx=Dy;
    Dy=temp;
    status=1;
  } 
  else
    status=0;

/********如果是直线，直接画线********/
  if(dx==0)                  
    LCD_YLine(VRAM,x0,y0,y1,color);
  if(dy==0)                   
    LCD_XLine(VRAM,x0,y0,x1,color);


/*********Bresenham算法画线********/ 
  sub=2*Dy-Dx;                 //第一次判断
  for(i=0;i<Dx;i++)
  { 
    LCD_Point(VRAM,x0,y0,color);           //画点
    if(sub>=0)                               
    { 
      if(status==1)             
        x0+=s1; 
      else                          
        y0+=s2; 
      sub-=2*Dx;                
    } 
    if(status==1)
      y0+=s2; 
    else       
      x0+=s1; 
    sub+=2*Dy;   
  } 
}

void LCD_Display_MiniChar(char * VRAM,int x, int y, char ch, unsigned int color)
{
	int i, j, pixelOn;
	for(i = 0; i < 6; i++)
	{
		for(j = 8; j > 0; j--)
		{
			pixelOn = charMap_ascii_mini[(unsigned char)ch][i] << j ;
			pixelOn = pixelOn & 0x80 ;
			if (pixelOn) 		LCD_Point(VRAM,x+i,y+8-j,color);
			//else 			 	DrawPoint_VRAM(VRAM,x+i,y+8-j,bgColor);
		}
	}
}

void LCD_Display_6X12_Chr(unsigned char *VRAM,unsigned int left,unsigned int top,unsigned char chr,unsigned int color)
{
  unsigned int x,y;
  unsigned int ptr;
  
  ptr=(chr-0x20)*12;
  for (y=0;y<12;y++)
  {
    for (x=0;x<6;x++)
    {
      if (((Font_Ascii_6X12E[ptr]<<x)&0x80)==0x80)
        LCD_Point(VRAM,left+x,top+y,color); 
    }
    ptr++;
  }
}

void LCD_Display_12X20_Chr(unsigned char *VRAM,unsigned int left,unsigned int top,unsigned char chr)
{
  unsigned int x,y;
  unsigned int ptr;
	uint16_t c;
  
	if ((chr>=42)&&(chr<=59))
	{
		ptr=(chr-42)*480;
		for (y=0;y<20;y++)
		{
			for (x=0;x<12;x++)
			{
				c=gImage_niexie[ptr++];
				//c=c<<8;
				c=c|(gImage_niexie[ptr++]<<8);
				LCD_Point(VRAM,left+x,top+y,c); 
			}
		}
	}else{
	if ((chr>64)&&(chr<91))
	{
		ptr=(chr-65+18)*480;
		for (y=0;y<20;y++)
		{
			for (x=0;x<12;x++)
			{
				c=gImage_niexie[ptr++];
				c=c|(gImage_niexie[ptr++]<<8);
				LCD_Point(VRAM,left+x,top+y,c); 
			}
		}
	}
	}
}

void LCD_Display_8X16_Chr(char *VRAM,unsigned int left,unsigned int top,unsigned char chr)
{
  unsigned int x,y;
  unsigned int ptr;
  
  if (left<320)
  {
  ptr=(chr-0x20)*16;//整体下移1个像素
  for (y=0;y<15;y++)
  {
    for (x=0;x<8;x++)
    {
      if (((Font_Ascii_8X16E[ptr]>>x)&0x01)==0x01)
        LCD_Point_Classic(VRAM,left+x,top+y,0); 
    }
    ptr++;
  }
  }
}

void LCD_String(char* VRAM,unsigned int left,unsigned int top,unsigned char *s,unsigned int color)
{
  unsigned int x;
  
  x=0;
  while(*s)
  {
    if (*s<128)
    {
      //LCD_Display_ASCII_8X16_Chr(left+x,top,*s++,color);
		LCD_Display_6X12_Chr(VRAM,left+x,top,*s++,color);
		x+=7;
    }
    else
    {
		//LCD_Display_Chn_Chr(VRAM,left+x,top,s,color);
		s+=2;
		x+=12;
    }
  }
}

void LCD_Str(unsigned int left,unsigned int top,unsigned char *s)
{
  unsigned int x;
  
  x=0;
  //printf("Display X=%d,Y=%d",left,top);
  while(*s)
  {
    if (*s<128)
    {
		//if (is_cx)
		//{
			LCD_Display_12X20_Chr(VRAM_A,left+x,top,*s++);
			x+=12;
		//}else
		//{
		//	LCD_Display_8X16_Chr(VRAM_A,left+x,top,*s++);
		//	x+=9;
		//}
    }
    else
    {
		s+=2;
		x+=12;
    }
  }
}

void slide_up(char * VRAM_A , char * VRAM_B , int speed)//说明：A原来在屏幕中，B在下方，然后B滑道上方
{
	int j ;
	char * temp = init_VRAM();

	if (is_cx)
	{
	for( j = 0 ; j <= 240 ; j += 12 )
	{
		memcpy( temp ,VRAM_A , SCREEN_BYTES_SIZE  );
		memcpy( temp + j*640 , VRAM_B , SCREEN_BYTES_SIZE - ( j  ) *640  );
		PutDisp_DDVRAM(temp);
		//sleep(speed);
	}
	}else{
	for( j = 0 ; j <= 240 ; j += 6 )
	{
		memcpy( temp ,VRAM_A , SCREEN_BYTES_SIZE  );
		memcpy( temp + j*160 , VRAM_B , SCREEN_BYTES_SIZE - ( j  ) *160  );
		PutDisp_DDVRAM(temp);
		sleep(speed);
	}	
	}
	close_VRAM( temp );
}

void slide_down(char * VRAM_A , char * VRAM_B , int speed)
{
	int j  ;
	char * temp = init_VRAM();

	if (is_cx)
	{
	for( j = 240 ; j >= 0 ; j -= 12 )
	{
		memcpy( temp ,VRAM_A , SCREEN_BYTES_SIZE  );
		memcpy( temp + j*640 , VRAM_B , SCREEN_BYTES_SIZE - ( j  ) *640  );
		PutDisp_DDVRAM(temp);
		//sleep(speed);
	}
	}else{
	for( j = 240 ; j >= 0 ; j -= 6 )
	{
		memcpy( temp ,VRAM_A , SCREEN_BYTES_SIZE  );
		memcpy( temp + j*160 , VRAM_B , SCREEN_BYTES_SIZE - ( j  ) *160  );
		PutDisp_DDVRAM(temp);
		sleep(speed);
	}
	}
	close_VRAM( temp );
}

void LCD_FillRect(char * VRAM, int x1 , int y1 ,int x2 ,int y2,unsigned int color)
{
	int i ;
	if( x1 > x2 )
	{
		i = x1 ; x1 = x2 ; x2 = i ;
	}
	if( y1 > y2 )
	{
		i = y1 ; y1 = y2 ; y2 = i ;
	}
	for(;y1 <= y2 ; y1 ++)
		LCD_XLine(VRAM, x1 , y1 , x2 ,color );
}

void LCD_EmuPx(int x1 , int y1)
{
	//printf("Draw Dot at X=%d Y=%d\r\n",x1,y1);
	LCD_FillRect(VRAM_A,8+x1*4,150+y1*4,8+x1*4+2,150+y1*4+2,0x03FA);
}

void LCD_DispBmp(char *VRAM,uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t *pic,uint16_t key)
{
  uint16_t i,j,c;

  for(i=0;i<h;i++)
  {
    for(j=0;j<w;j++)
    {
      c=(*pic++);
	  if (c!=key)
		LCD_Point(VRAM,x+j,y+i,c);
    }
  }
}

void LCD_GradientFillH(char * VRAM,unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,unsigned int c0,unsigned int c1)//横向渐变填充
{
	unsigned char R0,G0,B0,R1,G1,B1;
	float DR,DG,DB;
	unsigned int R,G,B,c,i;
	R0=(c0>>8)&0xF8;         //RGB565->RGB888
	R0=R0|(R0>>5);           //校正操作
	G0=(c0>>3)&0xFC;
	G0=G0|(G0>>6);
	B0=(unsigned int)(c0&0x1F)<<3;
	B0=B0|(B0>>5);
	R1=(c1>>8)&0xF8;
	R1=R1|(R1>>5);
	G1=(c1>>3)&0xFC;
	G1=G1|(G1>>5);
	B1=(unsigned int)(c1&0x1F)<<3;
	B1=B1|(B1>>5);       
	DR=(float)(R1-R0)/(float)(x1-x0);  //计算R通道的斜率
	DG=(float)(G1-G0)/(float)(x1-x0);  //计算G通道的斜率
	DB=(float)(B1-B0)/(float)(x1-x0);  //计算B通道的斜率
	for (i=0;i<=(x1-x0);i++)
	{
		R=DR*i+R0;                   //计算R通道的值
		G=DG*i+G0;                   //计算G通道的值
		B=DB*i+B0;                   //计算B通道的值
		if (R>255)	R=255;
		if (G>255)	G=255;
		if (B>255)	B=255;
		c=(unsigned int)((B>>3)+(G&0xFC)*8+(R&0xF8)*256);  
		LCD_YLine(VRAM,i+x0,y0,y1,c);         //画出这个X上的线
	}
} 

void UI_DrawWindow(char * VRAM,unsigned int x,unsigned int y,unsigned int w,unsigned int h)
{
	LCD_FillRect( VRAM , x , y , x + w, y + h, RGB(212,208,200));
	LCD_Line( VRAM , x , y , x + w -1 , y , RGB(212,208,200)); 
	LCD_Line( VRAM , x , y , x , y + h -1 , RGB(212,208,200)); 
	LCD_Line( VRAM , x , y + h , x + w, y + h , RGB(64,64,64));
	LCD_Line( VRAM , x + w , y , x + w, y + h , RGB(64,64,64)); 
	LCD_Line( VRAM , x + 1, y + 1, x + w -2 , y + 1 , RGB(255,255,255)); 
	LCD_Line( VRAM , x + 1, y + 1, x + 1 , y + h -3 , RGB(255,255,255)); 
	LCD_Line( VRAM , x + 1, y + h -1 , x + w - 1, y + h -1 , RGB(128,128,128));
	LCD_Line( VRAM , x + w -1, y + 1, x + w - 1, y + h -1, RGB(128,128,128)); 
	LCD_GradientFillH(VRAM,x+4,y+4,x+w-4,y+22,RGB(20,38,78),RGB(178,200,221));
}