#ifndef _MY_GRAPHIC

#include "movie.h"

enum{
	_CHR_DIRECT	 = -2,		
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
};

enum{
	_DIR_D			= 0,	
	_DIR_DL			= 1,	
	_DIR_L			= 2,	
	_DIR_UL			= 3,	
	_DIR_U			= 4,	
	_DIR_UR			= 5,	
	_DIR_R			= 6,	
	_DIR_DR			= 7,	
};

struct LgfHead{
	char	head[3];
	char	bpp;
	WORD	width;
	WORD	height;
	WORD	offsetX;
	WORD	offsetY;
};

struct YMZAnimeHead{
	char		head[4];	
	WORD		allFrame;	
	WORD		loopCnt;	
	BYTE		skipFrame;	
	BYTE		bpp;		
	WORD		width;		
	WORD		height;		
	WORD		offsetX;	
	WORD		offsetY;	

	YMZAnimeHead(){
		ZeroMemory(this,sizeof(YMZAnimeHead));
		strncpy(head, "YANI", 4);
		loopCnt = 1;
	}
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
	myRGBA		*anime_pal;
	char		filename[32];

	ColorBuf(){
		ZeroMemory(this,sizeof(ColorBuf));
	}
	~ColorBuf(){
		Release();
	}
	void Release(){
		cl_free(pBuf);
		if(anime_pal){
			my_deletes(anime_pal);
			palColor = NULL;
		}else{
			my_deletes(palColor);
		}
	}
	BOOL loadLGF(pack_file_no arcFileNum,char *fname,CBufType ctype=Type_parts);
	int SearchFile(pack_file_no arcFileNum,char *org_fname,char *out_fname,BOOL bPack=FALSE);
	BOOL createColorBuf(int width,int height,BYTE bpp,BYTE bClear=TRUE);
	BOOL Blt(int xDest,int yDest,int widthDest,int heightDest,ColorBuf *srcCBuf,int xSrc,int ySrc,int widthSrc,int heightSrc,BOOL bAlpha);
	BOOL BltFast(int xDest,int yDest,ColorBuf *srcCBuf,RECT *srcRect,BOOL bAlpha,BOOL bAdd=FALSE,BOOL bFast=FALSE);
	BOOL BltFast2(int xDest,int yDest,ColorBuf *srcCBuf1,ColorBuf *srcCBuf2,RECT *srcRect,int ptnNum,int ptnMax);
	BOOL Blt(RECT *dstRect,ColorBuf *srcCBuf,RECT *srcRect,BOOL bAlpha,BOOL bAdd=FALSE,BOOL bFast=FALSE);
	BOOL BrightBlt(int xDest,int yDest,ColorBuf *srcCBuf,RECT *srcRect,int r, int g, int b, BOOL bFast=FALSE);
	BOOL WaveBlt(int xDest,int yDest,ColorBuf *srcCBuf1, RECT *srcRect, SHORT shakeSize,DWORD wave_cnt);
	BOOL PtnFade( int xDest,int yDest,ColorBuf *srcCBuf1,ColorBuf *srcCBuf2,RECT *srcRect,int ptnNum,int ptnMax);
	BOOL ShakeBlt(ColorBuf *srcCBuf,int s_width,int s_direct,int step,int stepMax);
	BOOL RollBlt(ColorBuf *srcCBuf, float deg);
	void SpreadBlt(ColorBuf *srcCBuf,int power);
	BOOL NoiseBlt(ColorBuf *srcCBuf);
	BOOL SpiralBlt(int centerX,int centerY, ColorBuf *srcCBuf,RECT *srcRect,float step,float size);
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
		ZeroMemory(&colorBuf,sizeof(ColorBuf));	
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
		save.alpha = 0xff;
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
	WORD		in_type;		
	BYTE		shake_direct;	
	SHORT		shakeSize;		
	DWORD		look_cnt;		
	DWORD		look_max;		
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
	_BAK_BFADE = 23,	
	_BAK_SPREAD = 24,	

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

	_BAK_SIN			= 60,
	_BAK_2TI			= 61,
	_BAK_ZOOM			= 62,
	_BAK_TXT_SIN		= 63,
	_BAK_TXT_2TI		= 64,
	_BAK_ALL_SIN		= 66,
	_BAK_ALL_2TI		= 67,
	_BAK_RAND			= 69,
	_BAK_TXT_RAND		= 70,
	_BAK_ALL_RAND		= 71,
	_BAK_ROLL			= 72,
	_BAK_ROLL_SIN		= 73,
	_BAK_ROLL_2TI		= 74,

};

#define		_CHR_DEFAULTPOS		10000

struct SetBmpInf
{
	char		filename[32];	
	BYTE		bDraw;			
	BYTE		bUseBright;		
	BYTE		layer;			

	SHORT		nuki;			
	RECT		srcRect;		
	RECT		dstRect;		
	RECT		zoomRect;		
	BYTE		bAnime;
	BYTE		bAnimePlay;
	BYTE		bRelease;
	BYTE		animeSkip;
	BYTE		alphaMode;
	WORD		alphaParam;
	WORD		animeCnt;
	WORD		animeMax;
	WORD		animeWidth;
	WORD		animeHeight;
	DWORD		startTime;
	int			avi_stream;
	LPBYTE		readBuf;
	int			buffer_size;
	int			read_size;
	int			rest;
	float		fps;
	SHORT		animeLoop;
	XviDDec		xvidDec;

	ColorBuf	colorBuf;		
	ColorBuf	colorBuf_Br;	

	SetBmpInf(){
		ZeroMemory(this,sizeof(SetBmpInf));
	}
	~SetBmpInf(){
		Release();
	}
	void Release(){
		if(2==bAnime){
			if(readFile)readFile->StreamCloseFile(pack_anime, avi_stream);
			xvidDec.Clear_XviD();
			cl_free(readBuf);
		}
		colorBuf.Release();
		colorBuf_Br.Release();
		ZeroMemory(&zoomRect,sizeof(RECT));
		bDraw = bUseBright = bAnime = FALSE;
		bAnimePlay = 0;
	}
	void SetBmpGraph( char *fname, int layer, int nuki );
	void LoadBmpAnime( char *fname, int i_layer, int i_nuki ,int loop, int skip,BOOL bReleaseAnime=TRUE);
	BOOL WaitBmpAnime();
	void SetBmpAnimePlay(int sp_no, int play, int loop);
	void SetGraphParam(int mode, int param);
	void Draw(ColorBuf *dstCBuf);
	void SetBmpPos(int dx, int dy, int sx, int sy, int width, int height);
	void SetBmpMove(int dx, int dy);
	void SetBmpBright(int r, int g, int b);
	void SetBmpZoom(int x, int y, int width, int height);
	void SetBmpZoom2(int x, int y, int ratio);
	void SetBmpInf::ResetSetBmpZoom(void);
	void DrawAvi(void);
	BOOL SetAvi(char *fname, int i_layer);
	BOOL WaitAvi(void);
	void Reload(void);
	void AnimePlay(void);
};

class ItemList
{
	BYTE		state;
	BYTE		itemNum;
	BYTE		bHighSpeed;
	int			time;
	float		showStep;
	ColorBuf	fadePrt;
	ColorBuf	itemPrt;
	ColorBuf	itemNamePrt;
public:
	ItemList(int num);
	~ItemList(){
		fadePrt.Release();
		itemPrt.Release();
		itemNamePrt.Release();
	}
	BOOL GetItem();
	void Draw();
};

enum ButtonState{
	btn_nouse	= -1,	
	btn_normal	= 0,	
	btn_onCursor,		
	btn_push,			
	btn_disable,		
	btn_dummy,			
	btn_lock,			
};

class MyButton
{
	BYTE			type;			
	ButtonState		state;			
	int				flashTime;
	ColorBuf		*destCBuf;		
	ColorBuf		*btnCBuf;		
	RECT			partsRect[7];	
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
	BOOL CheckCheckBtnState(POINT *msPoint);
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

void drawChar(int layer);
void drawBG();
void drawBack(void);
void SetBmpDraw(void);
int CheckCharLocate(int char_no);
BOOL c_disp(int char_no, int pose, int locate, int in_type,int layer=0,int alpha = 255,int frame=30,int bright=128,int offsetX=_CHR_DEFAULTPOS,int offsetY=_CHR_DEFAULTPOS);
BOOL waitChar(int char_no);
void c_cls_all(void);
BOOL c_cls(int char_no, int out_type,int frame=30);
BOOL g_disp(int bak_no,int chg_type,int cg_type,DWORD fade,int x=0,int y=0,BOOL bak_clear=FALSE);
void createInitColorBuf(void);
BOOL SetBackFade(int r, int g, int b, int frame);
BOOL SetFlash(int r, int g, int b, int in_frame, int out_frame, BOOL bIn);
BOOL SetBackScroll(int x,int y,int frame,int type);
BOOL SetWavEffect(int wave, int frame, int wave_no);
void CreateAlphaTable(void);

inline DWORD AddBlend(DWORD *destP, DWORD *srcP)
{
	DWORD tmp = ((*destP & *srcP) + (((*destP ^ *srcP) >> 1) & 0x7f7f7f7f)) & 0x80808080;
	DWORD mask = (tmp << 1) - (tmp >> 7);
	*destP = ((*destP + *srcP) - mask) | mask;
	return *destP;
}
extern DIBInfo		g_DibInf;
extern ColorBuf		backColorBuf;
extern ColorBuf		bak_backColorBuf;
extern ColorBuf		ptnFadeBuf;
extern ColorBuf		noisePatternBuf;
extern BGInf		bgInf;
extern StdCharInf	charInf[STCHAR_MAX];
extern SetBmpInf	setBmpInf[STBMP_MAX];
extern SetBmpInf	bak_setBmpInf[STBMP_MAX];
extern ItemList		*itemList;
extern RECT			bgRect;
extern ToneConvert	toneConvert;

#define _MY_GRAPHIC
#endif  _MY_GRAPHIC
