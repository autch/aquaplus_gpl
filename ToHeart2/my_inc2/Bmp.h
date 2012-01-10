
 
  
   
    
     
      

#ifndef	_BMP_H_
#define _BMP_H_



extern void BMP_SetPackDir( char *pac_dir );
extern char *BMP_GetPackDir( void );





typedef struct {
	unsigned char	b;
	unsigned char	g;
	unsigned char	r;
	unsigned char	a;
}RGB32;

typedef struct {
	unsigned char	b;
	unsigned char	g;
	unsigned char	r;
}RGB24;

typedef struct {
	unsigned	b	:5;
	unsigned	g	:5;
	unsigned	r	:5;
	unsigned	a	:1;
}RGB16_5551;

typedef struct {
	unsigned	b	:5;
	unsigned	g	:6;
	unsigned	r	:5;
}RGB16_565;

typedef struct {
	unsigned	b	:4;
	unsigned	g	:4;
	unsigned	r	:4;
	char		a	:4;
}RGB16_4444;

#define	BIT16_555		0
#define	BIT16_565		1
#define	BIT16_4444		2




typedef	struct	{
	BITMAPINFOHEADER	bmiHeader;
	DWORD				bmiColors[3];
} BITMAPINFO2;

typedef	struct	{
	BITMAPINFOHEADER	bmiHeader;
	RGB32				bmiColors[256];
} BITMAPINFO3;



typedef struct{	
	RGB32	*buf;	
	int		sx,sy;
}BMP_T;

typedef struct{	
	RGB24	*buf;	
	int		sx,sy;
}BMP_F;

typedef struct{
	WORD	*buf;	
	char	*alp;	
	int		sx,sy;
}BMP_H;

typedef struct{
	char	*buf;	
	char	*alp;	
	RGB32	*pal;	
	int		sx,sy;
}BMP_BT;

typedef struct{
	char	*buf;	
	char	*alp;	
	WORD	*pal;	
	int		sx,sy;
}BMP_BH;


typedef struct{
	int		flag;
	int		bmp_bit;
	int		alp_flag;

	POINT	pos;
	POINT	size;

	BMP_T	bmp_t;		
	BMP_F	bmp_f;
	BMP_H	bmp_h;
	BMP_BT	bmp_b;
}BMP_SET;



typedef struct{
	char	flag;		
	char	bmp_bit;	
	char	alp_flag;	
	int		sx,sy;		

	char	*buf;	
	char	*alp;	
	RGB32	*pal;	
}BMP_STRUCT;


typedef struct {
	char	dep_r;		
	char	dep_g;
	char	dep_b;

	char	num_r;		
	char	num_g;
	char	num_b;

	char	pos_r;		
	char	pos_g;
	char	pos_b;

	WORD	mask_r;		
	WORD	mask_g;
	WORD	mask_b;

	DWORD	mask_rr;	
	DWORD	mask_gg;
	DWORD	mask_bb;

	DWORD	mask_rb;	

	DWORD	mask_rbg;	
	DWORD	mask_grb;	
} RGB16;
extern RGB16		rgb16;								

extern BOOL BMP_CheckFile( char *fname );


extern BOOL BMP_PalLoad( RGB32 *pal, char *fname );
extern BOOL BMP_PalLoad_BT( BMP_BT *bmp_b, char *fname );

extern BOOL BMP_PalSave( RGB32 *pal, char *fname );
extern BOOL BMP_PalSave_BT( BMP_BT *bmp_b, char *fname );


extern BOOL BMP_SetTonecurve_T( BMP_T *bmp_t, char *fname, int viv );
extern BOOL BMP_SetTonecurve_F( BMP_F *bmp_f, char *fname, int viv );
extern BOOL BMP_SetTonecurve_H( BMP_H *bmp_h, char *fname, int viv );
extern BOOL BMP_SetTonecurve_BT( BMP_BT *bmp_bt, char *fname, int viv );

extern BOOL BMP_LoadBMP_X2X( char *fname, BMP_SET *bmp_set, char *pack=NULL );

extern BOOL BMP_LoadBMP_X2T( char *fname, BMP_T *bmp_t, POINT *pos, POINT *size, int *alp_flag, char *pack=NULL );
extern BOOL BMP_LoadBMP_X2F( char *fname, BMP_F *bmp_f, POINT *pos, POINT *size, char *pack=NULL );
extern BOOL BMP_LoadBMP_X2H( char *fname, BMP_H *bmp_h, POINT *pos, POINT *size, int *alp_flag, char *pack=NULL );
extern BOOL BMP_LoadBMP_X2B( char *fname, BMP_BT *bmp_bt, POINT *pos, POINT *size, int *alp_flag, char *pack=NULL );

extern BOOL BMP_CheckBMPBpp( char *fname );
extern BOOL BMP_CheckBMPBpp2( char *fname );

extern BOOL BMP_LoadBMP_T2T( char *fname, BMP_T *bmp_t );
extern BOOL BMP_LoadBMP_F2T( char *fname, BMP_T *bmp_t );
extern BOOL BMP_LoadBMP_B2T( char *fname, BMP_T *bmp_t );

extern BOOL BMP_LoadBMP_F2F( char *fname, BMP_F *bmp_f );
extern BOOL BMP_LoadBMP_B2F( char *fname, BMP_F *bmp_f );

extern BOOL BMP_LoadBMP_T2H( char *fname, BMP_H *bmp_h );
extern BOOL BMP_LoadBMP_F2H( char *fname, BMP_H *bmp_h );
extern BOOL BMP_LoadBMP_H2H( char *fname, BMP_H *bmp_h );
extern BOOL BMP_LoadBMP_B2H( char *fname, BMP_H *bmp_h );

extern BOOL BMP_LoadBMP_B2B( char *fname, BMP_BT *bmp_bt );
extern BOOL BMP_LoadBMP_B2B_Key( char *fname, BMP_BT *bmp_bt, int *key );


extern void BMP_CopyBMP_T( BMP_T *dest, BMP_T *src );
extern void BMP_CopyBMP_F( BMP_F *dest, BMP_F *src );
extern void BMP_CopyBMP_H( BMP_H *dest, BMP_H *src );
extern void BMP_CopyBMP_BT( BMP_BT *dest, BMP_BT *src );


extern void BMP_CreateBMP_T( BMP_T *bmp_t, int sx, int sy );
extern void BMP_CreateBMP_F( BMP_F *bmp_f, int sx, int sy );
extern void BMP_CreateBMP_H( BMP_H *bmp_h, int sx, int sy, int alph );
extern void BMP_CreateBMP_BT( BMP_BT *bmp_bt, int sx, int sy, int alph );

extern void BMP_ReleaseBMP_T( BMP_T *bmp_t );
extern void BMP_ReleaseBMP_F( BMP_F *bmp_f );
extern void BMP_ReleaseBMP_H( BMP_H *bmp_h );
extern void BMP_ReleaseBMP_BT( BMP_BT *bmp_bt );



extern void BMP_GetVramWH( HWND hwnd, int *sx, int *sy );

extern void BMP_GetImageVram8( HWND hwnd, HDC *MemDC, HBITMAP *hbmp, HBITMAP *hold, BMP_BT *vram, int w, int h );
extern void BMP_GetImageVram16( HWND hwnd, HBITMAP *hBmp, BMP_H *vram, int w, int h );
extern void BMP_GetImageVram24( HWND hwnd, HDC *MemDC, HBITMAP *hbmp, HBITMAP *hold, BMP_F *vram, int w, int h );
extern void BMP_GetImageVram32( HWND hwnd, HDC *MemDC, HBITMAP *hbmp, HBITMAP *hold, BMP_T *vram, int w, int h );

extern BOOL BMP_SetPixelFormat16( int type );



extern int BMP_SaveBmp_T( char *fname, BMP_T *bmp_t );
extern int BMP_SaveBmp_F( char *fname, BMP_F *bmp_f );
extern int BMP_SaveBmp_H( char *fname, BMP_H *bmp_h );
extern int BMP_SaveBmp_B( char *fname, BMP_BT *bmp_bt );


typedef struct{
	RECT	cut;
	int		w;
	int		h;
}UME_FORMAT;




extern int BMP_MakeAlphaFF2T( char *bfname, char *afname, char *tfname, int alp_flag, int alp_end, int alp250, int CutSave );
extern int BMP_MakeAlphaBB2BA( char *bfname, char *afname, char *tfname, int alp250, int CutSave );



extern RGB32 BMP_ChangeNeg_Full( RGB32 src );
extern RGB32 BMP_ChangeFine_Full( RGB32 src, int rate );
extern RGB32 BMP_ChangeBright_Full( RGB32 src, int r, int g, int b );

extern void BMP_ChangePalNega_Full( RGB32 *pal );
extern void BMP_ChangePalFine_Full( RGB32 *pal, int rate );
extern void BMP_ChangePalBright_Full( RGB32 *pal, int r, int g, int b );

extern WORD BMP_ChangeNeg_High( WORD src );
extern WORD BMP_ChangeFine_High( WORD src, int rate );
extern WORD BMP_ChangeBright_High( WORD src, int r, int g, int b );

extern void BMP_ChangePalNega_High( WORD *pal );
extern void BMP_ChangePalFine_High( WORD *pal, int rate );
extern void BMP_ChangePalBright_High( WORD *pal, int r, int g, int b );

extern void BMP_ChangePalBright_TH( WORD *dpal,RGB32 *spal, int r, int g, int b );
extern void BMP_ChangePal_TH( WORD *dpal,RGB32 *spal );




extern DWORD	BrightTable_TF[256][256];
extern DWORD	BrightTable_TR[256][256];
extern WORD		BrightTable_TG[256][256];
extern char		BrightTable_TB[256][256];


extern WORD	BrightTable_HR[256][32];	
extern WORD	BrightTable_HG[256][64];	
extern WORD	BrightTable_HB[256][32];	



extern RGB32		H2F_Tbl[0x10000];	


extern WORD		F2H_TblRW[256];
extern WORD		F2H_TblBW[256];
extern WORD		F2H_TblGW[256];
extern DWORD	F2H_TblRD[256];
extern DWORD	F2H_TblBD[256];
extern DWORD	F2H_TblGD[256];


extern char		BlendTable[257][257];	
extern char		BlendTable2[257][257];	
extern char		BlendTable16[16][257];

extern char		DimTable[256][256];

extern char		AddTable[512];			
extern char		SubTable[512];			
extern char		OoiTable[256][256];			

extern char		PtnTable[768];

extern char	MeshFadePatern64[64][64][64];

extern short BomdTabl[256][256];		



#define BMP_RGB_F(R,G,B)		(((R)<<16)|((G)<<8)|(B))

#define DWORD4B(DW)		(DW&0xff)
#define DWORD4G(DW)		((DW>>8)&0xff)
#define DWORD4R(DW)		((DW>>16)&0xff)
#define DWORD4A(DW)		((DW>>24)&0xff)



#define BMP_BLD_H(D,A)			(WORD)((((((D)&rgb16.mask_rb)*(A))>>5)&rgb16.mask_rb)|(((((D)&rgb16.mask_g)*(A))>>5)&rgb16.mask_g))
#define BMP_RGB_FH(R,G,B)		(F2H_TblRW[(R)]|F2H_TblGW[(G)]|F2H_TblBW[(B)])
#define BMP_R_HH(DEST)			(((DEST)&rgb16.mask_r)>>rgb16.pos_r)
#define BMP_G_HH(DEST)			(((DEST)&rgb16.mask_g)>>rgb16.pos_g)
#define BMP_B_HH(DEST)			(((DEST)&rgb16.mask_b)>>rgb16.pos_b)
#define BMP_R_HF(DEST)			(((DEST)&rgb16.mask_r)>>rgb16.pos_r<<rgb16.num_r)
#define BMP_G_HF(DEST)			(((DEST)&rgb16.mask_g)>>rgb16.pos_g<<rgb16.num_g)
#define BMP_B_HF(DEST)			(((DEST)&rgb16.mask_b)>>rgb16.pos_b<<rgb16.num_b)


extern WORD *BMP_CreateBrightTable_H( int r, int g, int b );

extern void BMP_InitBmp( int disp_w, int disp_h );


extern int BMP_EncodeVisual( char *dst_fname, char *src_fname, char *dir );

#endif