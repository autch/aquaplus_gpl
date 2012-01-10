#include <windows.h>
#include "mm_std.h"
#include "tga.h"

struct RGBA{
	BYTE	blue;
	BYTE	green;
	BYTE	red;
	BYTE	alpha;
};

struct RGB{
	BYTE	blue;
	BYTE	green;
	BYTE	red;
};

TGAHead *LoadTgaInMemory( BYTE *ptr, BYTE **bmp, BYTE **pal, int *palnum, int *srcw, int *srch, int *bbp, int *alpha, int *hrev, int rgb_pal_ptn )
{
	int	i;
	static TGAHead	tgaHead;
	tgaHead = *(TGAHead*)ptr; ptr+=sizeof(tgaHead);


	if(tgaHead.imageType==9 || tgaHead.imageType==10){
		DebugBox(NULL,"このライブラリは、圧縮TGAをサポートしていません");
		return NULL;
	}
	*srcw = tgaHead.imageInfo.width;
	*srch = tgaHead.imageInfo.height;
	*bbp  = tgaHead.imageInfo.depth;
	*hrev = tgaHead.imageInfo.param&0x20;
	*alpha = 0;
	*palnum = 0;
	*pal = NULL;
	switch(*bbp){
		case 8:
			*palnum = tgaHead.cmInfo[2] + tgaHead.cmInfo[3]*256;
			if(tgaHead.cmInfo[4]==24){
				RGB	*rgb=(RGB*)ptr;
				static RGBA	rgba[256];
				*pal = (BYTE*)rgba;
				ptr += sizeof(RGB) * *palnum;
				if(rgb_pal_ptn){
					for(i=0;i<*palnum;i++){
						rgba[i].blue  = rgb[i].red;
						rgba[i].green = rgb[i].green;
						rgba[i].red   = rgb[i].blue;
						rgba[i].alpha = 255;
					}
				}else{
					for(i=0;i<*palnum;i++){
						rgba[i].blue  = rgb[i].blue;
						rgba[i].green = rgb[i].green;
						rgba[i].red   = rgb[i].red;
						rgba[i].alpha = 255;
					}
				}
				*alpha=0;
			}else{
				RGBA	*rgb=(RGBA*)ptr;
				*pal = ptr;
				ptr += sizeof(RGBA) * *palnum;
				if(rgb_pal_ptn){
					for(i=0;i<*palnum;i++){
						BYTE	work=rgb[i].blue;
						rgb[i].blue  = rgb[i].red;
						rgb[i].green = rgb[i].green;
						rgb[i].red   = work;
						rgb[i].alpha = rgb[i].alpha;
					}
				}
				*alpha=1;
			}
			break;
		case 32:
			*alpha=1;
		case 24:
			break;
	}
	*bmp = ptr;
	return &tgaHead;
}
