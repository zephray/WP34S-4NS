#define RGB(r,g,b) (((r>>3) & 0x1f) << 11)|(((g>>2) & 0x3f) << 5)|((b>>3) & 0x1f)

char * init_VRAM() ;
void AllClr_VRAM(char * VRAM);
void PutDisp_DDVRAM(char * VRAM);
void close_VRAM(char * VRAM);
int DrawPoint_VRAM( char * VRAM , int x , int y , char color );
int GetPoint_VRAM(char * VRAM,int x, int y) ;
void AreaClr_VRAM(char * VRAM , int x1 , int y1 , int x2 , int y2 );
void AreaRev_VRAM(char * VRAM , int x1 , int y1 , int x2 , int y2 );
void DrawGraph_VRAM(char * VRAM,int x,int y,int width,int height,char * pimage,char cl_fg,char cl_bg);
void AllFill_VRAM(char * VRAM,int color);
void Draw_Line_VRAM (char * VRAM,float x1 , float y1 , float x2 , float y2 , int color);
void DrawAsciiChar_VRAM(char *VRAM,int x,int y,char c,int cl_fg,int cl_bg);
void DrawAsciiChar_Gray_VRAM(char *VRAM , int x,int y,int width,int height,char c1,int is_rev);
void Draw_Mini_Char(char * VRAM,int x, int y, char ch, int textColor, int bgColor);
void DrawAsciiString_VRAM(char *VRAM,int x,int y,char *string,int cl_fg,int cl_bg);
void DrawAsciiStringGray_VRAM(char *VRAM,int x,int y,char *string,int cl_fg,int cl_bg);
void DrawMiniString_VRAM (char * VRAM,int x , int y , char * str , int cl_fg , int cl_bg );
void Draw_Rect_VRAM(char * VRAM, int x1 , int y1 ,int x2 ,int y2,int color);
void Fill_Rect_VRAM(char * VRAM, int x1 , int y1 ,int x2 ,int y2,int color);
void slide_up(char * VRAM_A , char * VRAM_B , int speed);
void slide_down(char * VRAM_A , char * VRAM_B , int speed);
void Draw_Region_VRAM(char * VRAM ,int x1 , int y1 , int x2 , int y2 , int color );
