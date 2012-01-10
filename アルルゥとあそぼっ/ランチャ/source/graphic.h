#ifndef _MY_GRAPHIC

enum{
	_CHR_DIRECT	 = -1,		
	_CHR_FADE	 = 0,		
	_CHR_SLIDE_L = 1,		
	_CHR_SLIDE_R = 2,		
	_CHR_WAIT	 = 3,		
	_CHR_WAVE	 = 4,		
};

enum{
	_CHR_LEFT   = 0,	
	_CHR_CENTER	= 1,	
	_CHR_RIGHT  = 2,	
	_CHR_LL		= 3,	
	_CHR_RR		= 4,	
	_CHR_CL		= 5,	
	_CHR_CR		= 6,	
};

struct LgfHead{
	char	head[3];
	char	bpp;
	WORD	width;
	WORD	height;
	WORD	offsetX;
	WORD	offsetY;
};

struct myRGB
{
  BYTE bBlue;
  BYTE bGreen;
  BYTE bRed;
};

struct myRGBA
{
  BYTE bBlue;
  BYTE bGreen;
  BYTE bRed;
  BYTE bAlpha;
};

enum CBufType{
	Type_parts,
	Type_back,
	Type_char,
	Type_event,
	Type_setbmp,
};

struct ColorBuf
{
	BYTE		bpp;
	CBufType	type;
	int			width;
	int			height;
	int			wPitch;
	float		alpha;
	POINT		offset;
	BYTE		*pBuf;
	myRGBA		*palColor;
	char		filename[32];

	ColorBuf(){
		ZeroMemory(this,sizeof(ColorBuf));
	}
	void Release(){
		cl_free(pBuf);
		my_deletes(palColor);
	}
	BOOL loadLGF(pack_file_no arcFileNum,char *fname,CBufType ctype=Type_parts);
	BOOL createColorBuf(int width,int height,BYTE bpp,BYTE bClear=TRUE);
	BOOL Blt(int xDest,int yDest,int widthDest,int heightDest,ColorBuf *srcCBuf,int xSrc,int ySrc,int widthSrc,int heightSrc,BOOL bAlpha);
	BOOL BltFast(int xDest,int yDest,ColorBuf *srcCBuf,RECT *srcRect,BOOL bAlpha);
	BOOL BltFast2(int xDest,int yDest,ColorBuf *srcCBuf1,ColorBuf *srcCBuf2,RECT *srcRect,int ptnNum,int ptnMax);
	BOOL Blt(RECT *dstRect,ColorBuf *srcCBuf,RECT *srcRect,BOOL bAlpha);
	BOOL BrightBlt(int xDest,int yDest,ColorBuf *srcCBuf,RECT *srcRect,int r, int g, int b);
	BOOL WaveBlt(int xDest,int yDest,ColorBuf *srcCBuf1, RECT *srcRect, SHORT shakeSize,DWORD wave_cnt);
	BOOL PtnFade( int xDest,int yDest,ColorBuf *srcCBuf1,ColorBuf *srcCBuf2,RECT *srcRect,int ptnNum,int ptnMax);
	BOOL ShakeBlt(ColorBuf *srcCBuf,int s_width,int step,int stepMax);
	BOOL RollBlt(ColorBuf *srcCBuf, float deg);
	BOOL colorBlt24(myRGBA *color,RECT *lpRect);
	inline void clearColorBuf(){
		if(pBuf==NULL)return;
		ZeroMemory(pBuf,wPitch *height);
	}
	void ToneConvert(void);
};

struct DIBInfo
{
	HDC			memDC;
	HBITMAP		hBmp;
	HBITMAP		oldBmp;
	ColorBuf	colorBuf;

	DIBInfo(){
		ZeroMemory(this,sizeof(DIBInfo));
	}
	void Release(){
		if(memDC){
			SelectObject(memDC,oldBmp);
			DeleteObject(hBmp);
			DeleteDC(memDC);
			memDC = NULL;
			hBmp = oldBmp = NULL;
		}
	}
};

struct StdCharInf
{
	CharSave	save;
	char		in_type;	
	char		out_type;	
	DWORD		look_cnt;	
	DWORD		look_max;	
	ColorBuf	colorBuf;	
	ColorBuf	bak_colorBuf;

	StdCharInf(){
		ZeroMemory(this,sizeof(StdCharInf));
		in_type = out_type = _CHR_DIRECT;
	}
	~StdCharInf(){
		Release();
	}
	void Release(){
		colorBuf.Release();
		bak_colorBuf.Release();
		ZeroMemory(this,sizeof(StdCharInf));
		in_type = out_type = _CHR_DIRECT;
		save.locate = _CHR_CENTER;
	}
};

struct BGInf
{
	BYTE		in_type;		
	SHORT		shakeSize;		
	DWORD		look_cnt;		
	DWORD		look_max;		
	DWORD		shake_cnt;		
	ColorBuf	colorBuf;		

	BGInf(){
		ZeroMemory(this,sizeof(StdCharInf));
	}
	~BGInf(){
		Release();
	}
	void Release(){
		colorBuf.Release();
		ZeroMemory(this,sizeof(BGInf));
	}
};

enum{
	_BAK_DIRECT = -1,	
	_BAK_FADE = 0,		
	_BAK_CFADE = 1,		
	_BAK_U = 2,			
	_BAK_D = 3,			
	_BAK_R = 4,			
	_BAK_L = 5,			
	_BAK_C = 6,			
	_BAK_O = 7,			
	_BAK_DIA1 = 8,		
	_BAK_DIA2 = 9,		
	_BAK_DIA3 = 10,		
	_BAK_CFZOOM1 = 11,	
	_BAK_CFZOOM2 = 12,	
	_BAK_CFZOOM3 = 13,	
	_BAK_CFZOOM4 = 14,	
	_BAK_KAMI = 15,		
	_BAK_SU = 16,		
	_BAK_SD = 17,		
	_BAK_SR = 18,		
	_BAK_SL = 19,		
	_BAK_LST = 20,		
	_BAK_NOISE = 21,	
	_BAK_PIPLE = 22,	
	_BAK_PATTERN0 = 40,	
	_BAK_PATTERN1 = 41,	
	_BAK_PATTERN2 = 42,	
	_BAK_PATTERN3 = 43,	
	_BAK_PATTERN4 = 44,	
	_BAK_PATTERN5 = 45,	
	_BAK_PATTERN6 = 46,	
	_BAK_PATTERN7 = 47,	
	_BAK_PATTERN8 = 48,	
	_BAK_PATTERN9 = 49,	

	_BAK_SHAKE = 60,	
	_BAK_TXT_SHAKE = 61,
	_BAK_RAND = 62,		
	_BAK_ZOOMSHAK = 63,	
	_BAK_ROLL = 64,		
	_BAK_ROLL_2TI = 65,	
};

struct SetBmpInf
{
	BYTE		bDraw;			
	BYTE		bUseBright;		
	BYTE		layer;			
	SHORT		nuki;			
	RECT		srcRect;		
	RECT		dstRect;		
	ColorBuf	colorBuf;		
	ColorBuf	colorBuf_Br;	

	SetBmpInf(){
		ZeroMemory(this,sizeof(SetBmpInf));
	}
	~SetBmpInf(){
		Release();
	}
	void Release(){
		colorBuf.Release();
		colorBuf_Br.Release();
		bDraw = bUseBright = FALSE;
	}
	void SetBmpGraph( char *fname, int layer, int nuki );
	void Draw(void);
	void SetBmpPos(int dx, int dy, int sx, int sy, int width, int height);
	void SetBmpBright(int r, int g, int b);
	void SetBmpZoom(int x, int y, int width, int height);
};

enum ButtonState{
	btn_nouse	= -1,	
	btn_normal	= 0,	
	btn_onCursor,		
	btn_push,			
	btn_disable,		
};


class MyButton
{
	BYTE			type;			
	ButtonState		state;			
	ColorBuf		*destCBuf;		
	ColorBuf		*btnCBuf;		
	RECT			partsRect[4];	
public:
	RECT			selectRect;
	MyButton(){
		ZeroMemory(this,sizeof(MyButton));
		state = btn_nouse;
	}
	~MyButton(){
		Release();
	}
	void Release(){
		state = btn_nouse;
	}
	void Create(BYTE type, ColorBuf *partsCBuf, RECT *s_rect, RECT *p_rect, ColorBuf *dest);
	BOOL CheckState(POINT *msPoint);
	void Draw(void);
	void SetState(ButtonState s_state){ if(btn_nouse!=state)state = s_state; }
	void SetEnable(BOOL set){
		if(set)state = btn_normal;
		else state = btn_disable;
	}
	ButtonState GetState(){ return state; }
};

struct ToneTable
{
	BYTE		bUseRGB;
	BYTE		bUseAll;

	BYTE		all[256];
	BYTE		red[256];
	BYTE		green[256];
	BYTE		blue[256];
	inline void Convert(myRGB *rgb);
	inline void Convert(myRGBA *rgba);
};

class ToneConvert
{
	BYTE		bUseTable[2];
	BYTE		bSetChar;
	ToneTable	table[2];
public:
	BYTE		bUse;
	ToneConvert(){
		ZeroMemory(this,sizeof(ToneConvert));
	}
	void ClearTable(void);
	void ChangeTable(int mainType, int subType,BOOL bBack=FALSE);
	void Convert(BYTE *pBuf,int width,int height,int wPitch,BYTE bpp,CBufType type);
	void CheckBGTable(char *fname);
	void SetCharTable(void);
};

void drawChar(void);
void drawBG(void);
void drawBack(void);
void SetBmpDraw(void);
int CheckCharLocate(int char_no);
BOOL c_disp(int char_no, int pose, int locate, int in_type,int layer=0,int frame=30);
BOOL waitChar(int char_no);
void c_cls_all(void);
BOOL c_cls(int char_no, int out_type,int frame=30);
BOOL g_disp(int bak_no,int chg_type,int cg_type,DWORD fade,int x=0,int y=0,BOOL bak_clear=FALSE);
BOOL SetBackFade(int r, int g, int b, int frame);
BOOL SetBackScroll(int x,int y,int frame,int type);
BOOL SetWavEffect(int wave, int frame, int wave_no);
void CreateAlphaTable(void);

#include <d3dx8.h>
void InitMaze();
void GetMazeVec(D3DXVECTOR3 *vec, float deg);

extern DIBInfo		g_DibInf;
extern ColorBuf		backColorBuf;
extern ColorBuf		bak_backColorBuf;
extern ColorBuf		ptnFadeBuf;
extern BGInf		bgInf;
extern StdCharInf	charInf[STCHAR_MAX];
extern SetBmpInf	setBmpInf[STBMP_MAX];
extern RECT			bgRect;
extern ToneConvert	toneConvert;

#define _MY_GRAPHIC
#endif  _MY_GRAPHIC
