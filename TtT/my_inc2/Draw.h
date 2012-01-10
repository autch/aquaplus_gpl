
 
  
   
    
     
      

#ifndef	_DRAW_H
#define _DRAW_H

#include "bmp.h"


extern int	AlphaFormat;
#define ALPHA_ALL	0	
#define ALPHA_HALF	1	



extern int	DirectDrawFlag;







extern int	InterlessLine;



typedef struct{
	int		x;		
	RGB32	rgb;	
	int		sx, sy;	
}MIN_MAX;



#define	UP		0
#define	DO		1
#define	RI		2
#define	LE		3
#define	CE		4
#define	OU		5

enum{
	DRW_NML=0x00,	
	DRW_ADD,	
	DRW_OOI,	
	DRW_SUB,	
	DRW_DIM,
	DRW_MUL,	
	DRW_OLY,	
	DRW_CML,	
	DRW_CMD,	
	DRW_NEG,	
	DRW_TNC,	

	DRW_BLD2,	
	DRW_VIV2,	
	DRW_AMI2,	
	DRW_NIS2,	
	DRW_MOZ2,	
	DRW_BOM2,	

	DRW_GNM2,	
	DRW_FLT2,	

	DRW_RPL2,	

	DRW_RP22,	
	DRW_RP32,	

	DRW_SUI2,	
	DRW_SBL2,	
	DRW_WAV2,	
	DRW_WBL2,	
	DRW_LST2,	
	DRW_LBL2,	

	DRW_LCF2,				
	DRW_LPP2=DRW_LCF2+6,	
	DRW_DIA2=DRW_LPP2+6,	
	DRW_DIO2=DRW_DIA2+3,	

	DRW_PAL=0x0100,	
};

#define DRW_PRM(PRM1,PRM2)	((PRM1)|((PRM2)<<16))

#define DRW_BLD(ALP)		(DRW_BLD2|((LIM(ALP,0,256))<<16))	
#define DRW_VIV(FLS)		(DRW_VIV2|((FLS)<<16))	
#define DRW_AMI(AMI)		(DRW_AMI2|((AMI)<<16))	
#define DRW_NIS(NIS)		(DRW_NIS2|((NIS)<<16))	
#define DRW_MOZ(MOZ)		(DRW_MOZ2|((MOZ)<<16))	
#define DRW_BOM(BOM)		(DRW_BOM2|((BOM)<<16))	

#define DRW_GNM(GNM)		(DRW_GNM2|((GNM)<<16))	
#define DRW_FLT(FLT)		(DRW_FLT2|((FLT)<<16))	


#define DRW_RPL(WAV_CNT)	(DRW_RPL2|((WAV_CNT)<<16))	

#define DRW_RP2(BLD,CNT)	(DRW_RP22|(((BLD)|((CNT)<<8))<<16))	
#define DRW_RP3(BLD,CNT)	(DRW_RP32|(((BLD)|((CNT)<<8))<<16))	

#define DRW_SUI(WAV,CNT)	(DRW_SUI2|(((WAV)|((CNT)<<8))<<16))	
#define DRW_SBL(WAV,CNT)	(DRW_SBL2|(((WAV)|((CNT)<<8))<<16))	
#define DRW_WAV(WAV,CNT)	(DRW_WAV2|(((WAV)|((CNT)<<8))<<16))	
#define DRW_WBL(WAV,CNT)	(DRW_WBL2|(((WAV)|((CNT)<<8))<<16))	
#define DRW_LST(LST,CNT)	(DRW_LST2|(((LST)|((CNT)<<8))<<16))	
#define DRW_LBL(LBL,CNT)	(DRW_LBL2|(((LBL)|((CNT)<<8))<<16))	



#define DRW_LCF(P,RAT)		((DRW_LCF2+(P))|((RAT)<<16))	
#define DRW_LPP(P,RAT)		((DRW_LPP2+(P))|((RAT)<<16))	
#define DRW_DIA(P,RAT)		((DRW_DIA2+(P))|((RAT)<<16))	
#define DRW_DIO(P,RAT)		((DRW_DIO2+(P))|((RAT)<<16))	

#define DRW_PRM4(PRM4,PRM3,PRM2,PRM1)	(((PRM4)<<24)|((PRM3)<<16)|((PRM2)<<8)|(PRM1))	


#define REV_NOT		0x00	
#define REV_W		0x10	
#define REV_H		0x20	

#define XINC(DOBJ)		(((DOBJ)->rparam&REV_W)?-1:1)
#define YINC(DOBJ)		(((DOBJ)->rparam&REV_H)?-1:1)
#define STX(DOBJ)		(((DOBJ)->rparam&REV_W)?((DOBJ)->s1w)-1:0)
#define STY(DOBJ)		(((DOBJ)->rparam&REV_H)?((DOBJ)->s1h)-1:0)
#define STX2(DOBJ,W)	(((DOBJ)->rparam&REV_W)?(W)-1:0)
#define STY2(DOBJ,H)	(((DOBJ)->rparam&REV_H)?(H)-1:0)


#define BRT_MIN		0x00	
#define BRT_NML		0x80	
#define BRT_MAX		0xff	

#define BLD_0P		0x00	
#define BLD_6P		0x10	
#define BLD_12P		0x20	
#define BLD_25P		0x40	
#define BLD_50P		0x80	
#define BLD_100P	0x100	

#define CHK_NO		-1		
#define CHK_ANTI	-2		
#define CHK_HOKAN	-3		
#define CHK_NOANTI	-4		
#define CHK_256		0		


#define DISP_256	8		
#define DISP_HIGH	16		
#define DISP_FULL	24		
#define DISP_TRUE	32		



extern RECT	ClipRectDef;	

extern void DRW_SetDispWH( int disp_w, int disp_h );

extern BOOL DRW_SetClipRectDefault( int l, int t, int r, int b );
extern BOOL ClipRect( int *dest_px, int *dest_py, int dest_sx, int dest_sy,
					  int *src_px,  int *src_py,  int src_sx,  int src_sy, int *w, int *h, RECT *clip );

extern BOOL ClipRectS2(int *dest_px, int *dest_py, int dest_sx, int dest_sy,
					   int *src1_px, int *src1_py, int src1_sx, int src1_sy, int *w1, int *h1,
					   int *src2_px, int *src2_py, int src2_sx, int src2_sy, int *w2, int *h2, RECT *clip );

extern BOOL ClipRectZ( int *sx, int *sy, int *sw, int *sh, int *dx, int *dy, int *dw, int *dh, int *xx, int *yy, int dest_sx, int dest_sy, int src_sx, int src_sy, RECT *clip );
extern BOOL ClipRect2( int *dest_px, int *dest_py, int *w, int *h, int dest_sx, int dest_sy, RECT *clip );
extern BOOL ClipRectLine( int *x1, int *y1, int *x2, int *y2, int dest_sx, int dest_sy, RECT *clip );
extern BOOL ClipRectLineRGB( int *x1, int *y1, int *r1, int *g1, int *b1, int *x2, int *y2, int *r2, int *g2, int *b2, int dest_sx, int dest_sy, RECT *clip );


extern void DrawMinMax4Table( int x1, int y1, int x2, int y2, RECT *clip, short (*mm)[4] );
extern void DrawMinMaxTable(    int x1, int y1, int x2, int y2, RECT *clip, MIN_MAX mi[], MIN_MAX ma[] );
extern void DrawMinMaxTableRGB( int x1, int y1, int r1, int g1, int b1, int x2, int y2, int r2, int g2, int b2, RECT *clip, MIN_MAX mi[], MIN_MAX ma[] );
extern void DrawMinMaxTableSrc( int x1, int y1, int sx1, int sy1, int x2, int y2, int sx2, int sy2, RECT *clip, MIN_MAX mi[], MIN_MAX ma[] );



extern void DRW_InitRipleMapping( int w, int h, int cx, int cy );



typedef struct{
	int		bppd;
	void	*dest;
	int		dx,  dy;	
	int		dx2, dy2;	
	int		dx3, dy3;	
	int		dx4, dy4;	
	int		dw,  dh;

	int		bpps1;
	void	*src1;
	int		s1x,  s1y;	
	int		s1x2, s1y2;	
	int		s1x3, s1y3;	
	int		s1x4, s1y4;	
	int		s1w,  s1h;

	int		bpps2;
	void	*src2;
	int		s2x,  s2y;
	int		s2x2, s2y2;
	int		s2x3, s2y3;
	int		s2x4, s2y4;
	int		s2w,  s2h;

	int		r,g,b;
	int		r2,g2,b2;
	int		r3,g3,b3;
	int		r4,g4,b4;

	void	*mask;

	int		nuki;
	RECT	*clip;
	
	WORD	dparam;
	WORD	dnum;
	WORD	dparam2;
	WORD	dnum2;
	DWORD	rparam;
}DRAW_OBJECT;

extern DRAW_OBJECT DRW_SetDrawObjectLocal(int bppd, void *dest, int dx,  int dy,  int dw,  int dh,
										int bpps1, void *src1, int sx1, int sy1, int sw1, int sh1,
										int r, int g, int b, int nuki, RECT *clip, int param1, int rparam );

extern DRAW_OBJECT DRW_SetDrawObject(int bppd, void *dest, int dx,  int dy,  int dw,  int dh,
									int bpps1, void *src1, int sx1, int sy1, int sw1, int sh1,
									int bpps2, void *src2, int sx2, int sy2, int sw2, int sh2,
									int r, int g, int b, int nuki, RECT *clip, int param1, int param2, int rparam );

extern void DRW_SetDrawObjectPoly( DRAW_OBJECT *dobj,
								int dx,  int dy,  int dx2,  int dy2,  int dx3,  int dy3,  int dx4,  int dy4,
								int sx1, int sy1, int sx12, int sy12, int sx13, int sy13, int sx14, int sy14 );

extern int DRW_GetStartPointerSrc1( DRAW_OBJECT *dobj );
extern int DRW_GetStartPointerSrc2( DRAW_OBJECT *dobj );
extern int DRW_GetStartPointerEx( DRAW_OBJECT *dobj, int dx, int dy );
extern void DRW_SetDrawObject2XXX( DRAW_OBJECT *dobj, DRAW_OBJECT *dobj2, DRAW_OBJECT *dobj3, DRAW_OBJECT *dobj4, DRAW_OBJECT *dobj5 );


extern BOOL DRW_DrawBMP_TTT( DRAW_OBJECT dobj );

extern BOOL DRW_DrawBMP_TT( DRAW_OBJECT dobj );
extern BOOL DRW_DrawBMP_TB( DRAW_OBJECT dobj );

extern BOOL DRW_DrawZOOM_TT( DRAW_OBJECT dobj );
extern BOOL DRW_DrawZOOM_TB( DRAW_OBJECT dobj );

extern BOOL DRW_DrawPOLY4_TT( DRAW_OBJECT dobj );
extern BOOL DRW_DrawPOLY4_TB( DRAW_OBJECT dobj );

extern void DRW_SetRippleTable( int w, int h );
extern void DRW_RenewRippleTable( int side_flag );
extern short *DRW_GetRippleTableAddr( void );
extern int DRW_GetRippleTableNwf( void );


extern BOOL DRW_DrawBMP_FTT( DRAW_OBJECT dobj, int interless=0 );
extern BOOL DRW_DrawBMP_FFF( DRAW_OBJECT dobj, int interless=0 );
extern BOOL DRW_DrawBMP_FFB( DRAW_OBJECT dobj, int interless=0 );
extern BOOL DRW_DrawBMP_FFBM( DRAW_OBJECT dobj, int interless=0 );

extern BOOL DRW_DrawBMP_FT( DRAW_OBJECT dobj, int interless=0 );
extern BOOL DRW_DrawBMP_FF( DRAW_OBJECT dobj, int interless=0 );
extern BOOL DRW_DrawBMP_FB( DRAW_OBJECT dobj, int interless=0 );

extern BOOL DRW_DrawZOOM_FT( DRAW_OBJECT dobj, int interless=0 );
extern BOOL DRW_DrawZOOM_FF( DRAW_OBJECT dobj, int interless=0 );
extern BOOL DRW_DrawZOOM_FB( DRAW_OBJECT dobj, int interless=0 );

extern BOOL DRW_DrawPOLY4_FT( DRAW_OBJECT dobj, int interless=0 );
extern BOOL DRW_DrawPOLY4_FF( DRAW_OBJECT dobj, int interless=0 );
extern BOOL DRW_DrawPOLY4_FB( DRAW_OBJECT dobj, int interless=0 );



extern BOOL DRW_DrawBMP_HHH( DRAW_OBJECT dobj );

extern BOOL DRW_DrawBMP_HH( DRAW_OBJECT dobj );
extern BOOL DRW_DrawBMP_HB( DRAW_OBJECT dobj );

extern BOOL DRW_DrawZOOM_HH( DRAW_OBJECT dobj );
extern BOOL DRW_DrawZOOM_HB( DRAW_OBJECT dobj );

extern BOOL DRW_DrawPOLY4_HH( DRAW_OBJECT dobj );
extern BOOL DRW_DrawPOLY4_HB( DRAW_OBJECT dobj );



extern BOOL DRW_DrawBMP_BB( BMP_BT *dest, int dest_x, int dest_y, BMP_BT *src,  int src_x,  int src_y, int w,int h, int nuki, RECT *clip, int r, int g, int b, DWORD wparam, DWORD rparam );






extern BOOL DRW_InitDigit( HINSTANCE hinst, char *bitmap );
extern BOOL DRW_TermDigit( void );
extern BOOL DRW_DrawDigit( void *dest, int x, int y, int num, DWORD wparam, int draw_mode );
extern BOOL DRW_DrawDmoji( void *dest, int x, int y, char *str, DWORD wparam, int draw_mode );

extern void DRW_DrawBMP( HWND hwnd, HDC MemDC, HBITMAP hbmp, int w, int h, int interless );
extern void DRW_DrawBMP2( HWND hwnd, HBITMAP hbmp, HDC hMem, int dx, int dy, int sx, int sy );
extern void DRW_DrawBMP3( HWND hwnd, HBITMAP hbmp, HDC hMem, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh );



#endif