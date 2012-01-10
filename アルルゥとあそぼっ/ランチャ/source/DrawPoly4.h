

#ifndef	DRAW_POLY4_H_
#define DRAW_POLY4_H_


typedef struct {
	BYTE	b;
	BYTE	g;
	BYTE	r;
}RGB24;

typedef struct {
	BYTE	b;
	BYTE	g;
	BYTE	r;
	BYTE	a;
}RGB32;

typedef struct{
	BYTE	*buf;
	BYTE	*alp;
	RGB32	*pal;
	int		sx,sy;
}BMP_BT;

typedef struct{
	RGB24	*buf;
	int		sx,sy;
}BMP_F;

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

extern BOOL DRW_DrawPOLY4_FB( DRAW_OBJECT dobj );


extern BOOL DRW_DrawPOLY4_FB_Simple( BMP_F *dest, BMP_BT *src,
							 int dx1, int dy1, int dx2, int dy2,
							 int dx3, int dy3, int dx4, int dy4, int r, int g, int b );


#endif	//DRAW_POLY4_H_
