#ifndef _TGA_H_
#define _TGA_H_

struct ImageInfo{
	WORD	xOrigin;
	WORD	yOrigin;
	WORD	width;
	WORD	height;
	BYTE	depth;
	BYTE	param;
};

struct TGAHead{
	BYTE		ID;
	BYTE		colorMap;
	BYTE		imageType;
	BYTE		cmInfo[5];
	ImageInfo	imageInfo;
};

struct RGBA{
	BYTE	blue;
	BYTE	green;
	BYTE	red;
	BYTE	alpha;
};

struct RGB_{
	BYTE	blue;
	BYTE	green;
	BYTE	red;
};

extern void SaveTga( char *fname, BYTE *bmp, int srcw, int pitch, int srch, int depth, int hrev );
extern TGAHead *LoadTgaInMemory( BYTE *ptr, BYTE **bmp, BYTE **pal, int *palnum, int *srcw, int *srch, int *bbp, int *alpha, int *hrev, int rgb_pal_ptn=1 );


#endif	