#include "AVGYMZ.h"
#include "msgWnd.h"
#include "escript.h"
#include "graphic.h"

ColorBuf	backColorBuf;
ColorBuf	bak_backColorBuf;
ColorBuf	ptnFadeBuf;
ColorBuf	noisePatternBuf;
BGInf		bgInf;
StdCharInf	charInf[STCHAR_MAX];
SetBmpInf	setBmpInf[STBMP_MAX];
SetBmpInf	bak_setBmpInf[STBMP_MAX];
ItemList	*itemList = NULL;
ToneConvert	toneConvert;

BYTE		AlphaTable[256][256];
RECT		bgRect = {0,0,800,600};

void CopyEqualColor(BYTE *pDest,int destP_W, BYTE *pSrc, int srcP_W,int width,int height,int bpp);
void Copy24to24A(myRGB *pDest,int destP_W, myRGB *pSrc1,int src1P_W, myRGB *pSrc2, int src2P_W, int width,int height,float alpha,BOOL bFast=FALSE);
void Copy24to24A(myRGB *pDest,int destP_W, myRGB *pSrc, int srcP_W, int width,int height,float alpha,BOOL bFast=FALSE);
void Copy32to24(myRGB *pDest,int destP_W, myRGBA *pSrc, int srcP_W, int width,int height,BOOL bFast=FALSE);
void Copy24to32(myRGBA *pDest,int destP_W, myRGB *pSrc, int srcP_W, int width,int height,BOOL bFast=FALSE);
void Copy32to24A(myRGB *pDest,int destP_W, myRGBA *pSrc, int srcP_W, int width,int height,float alpha,BOOL bFast=FALSE);
void Copy32_2to24A(ColorBuf *destBuf,ColorBuf *srcBuf1, int offsetX1, int offsetY1,ColorBuf *srcBuf2, int offsetX2, int offsetY2,float alpha,BOOL bFast=FALSE);
void Copy8to24(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,BOOL bFast=FALSE);
void Copy8to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha,BOOL bFast=FALSE);
void Copy9to24(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,BOOL bFast=FALSE);
void Copy9to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha,BOOL bFast=FALSE);
void StretchCopy8to24(myRGB *pDest,int destP_W, int widthDest,int heightDest,BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,BOOL bFast=FALSE);
void StretchCopy8to24A(myRGB *pDest,int destP_W, int widthDest,int heightDest,BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast=FALSE);
void StretchCopy9to24(myRGB *pDest,int destP_W, int widthDest,int heightDest,BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,BOOL bFast=FALSE);
void StretchCopy24to24(myRGB *pDest,int destP_W, int widthDest,int heightDest, myRGB *pSrc, int srcP_W, int widthSrc,int heightSrc,BOOL bFast=FALSE);
void StretchCopy32to24(myRGB *pDest,int destP_W, int widthDest,int heightDest, myRGBA *pSrc, int srcP_W, int widthSrc,int heightSrc,BOOL bFast=FALSE);
void StretchCopy9to24A(myRGB *pDest,int destP_W, int widthDest,int heightDest,BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast=FALSE);
void StretchCopy24to24A(myRGB *pDest,int destP_W, int widthDest,int heightDest, myRGB *pSrc, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast=FALSE);
void StretchCopy32to24A(myRGB *pDest,int destP_W, int widthDest,int heightDest, myRGBA *pSrc, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast=FALSE);
void Add8to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha,BOOL bFast=FALSE);
void Add9to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha,BOOL bFast=FALSE);
void Add24to24A(myRGB *pDest, int destP_W, myRGB *pSrc, int srcP_W, int width,int height, float alpha,BOOL bFast=FALSE);
void Add32to24A(myRGB *pDest, int destP_W, myRGBA *pSrc, int srcP_W, int width,int height, float alpha,BOOL bFast=FALSE);
void StretchAdd8to24A(myRGB *pDest,int destP_W, int widthDest,int heightDest,BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast=FALSE);
void StretchAdd9to24A(myRGB *pDest,int destP_W, int widthDest,int heightDest,BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast=FALSE);
void StretchAdd24to24A(myRGB *pDest,int destP_W, int widthDest,int heightDest, myRGB *pSrc, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast=FALSE);
void StretchAdd32to24A(myRGB *pDest,int destP_W, int widthDest,int heightDest, myRGBA *pSrc, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast=FALSE);
void Copy8to32A(myRGBA *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha,BOOL bFast=FALSE);
void Copy24to32A(myRGBA *pDest,int destP_W, myRGB *pSrc, int srcP_W, int width,int height,float alpha,BOOL bFast=FALSE);
void Copy32to32A(myRGBA *pDest,int destP_W, myRGBA *pSrc, int srcP_W, int width,int height,float alpha,BOOL bFast=FALSE);
void BrightCopy24to24(myRGB *pDest, int destP_W, myRGB *pSrc, int srcP_W, int width, int height, int r, int g, int b, BOOL bFast);
void BrightCopy32to32(myRGBA *pDest, int destP_W, myRGBA *pSrc, int srcP_W, int width, int height, int r, int g, int b, BOOL bFast);

struct ImageInfo{
	WORD	xOrigin;
	WORD	yOrigin;
	WORD	width;
	WORD	height;
	BYTE	depth;
	BYTE	alphaInfo;
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

struct RGB{
	BYTE	blue;
	BYTE	green;
	BYTE	red;
};

BOOL ColorBuf::loadLGF(pack_file_no arcFileNum,char *fname,CBufType ctype)
{
	char	lgfFile[256];
	LgfHead	*lgfHead;
	BYTE	*srcBuf;

	Release();
	type = ctype;
	strcpy(filename,fname);
	alpha = 1.0f;

#ifndef _MASTER
	LONG	x,y;
	BOOL	bGparts = FALSE;
	TGAHead	tgaHead;
	FILE	*fpr;

	x = SearchFile(arcFileNum,fname,lgfFile);

	if(1 == x){
		fpr = fopen(lgfFile,"rb");
		if(NULL!=fpr){
			BOOL		bTurn,bUseAlpha,bAllZero;
			RGB			*rgb = NULL,  *lpSrcColor24 = NULL;
			RGBA		*rgba = NULL, *lpSrcColor32 = NULL;
			BYTE		*map = NULL, *lpSrc8 = NULL;
			RGB			trgb;
			RGBA		trgba;
			int			palNum = 0;
			fread(&tgaHead,sizeof(tgaHead),1,fpr);
			if(tgaHead.ID > 0) fseek(fpr,tgaHead.ID,SEEK_CUR);
			if(tgaHead.colorMap!=0) {
				palNum = tgaHead.cmInfo[2] + tgaHead.cmInfo[3]*256;
			}
			BOOL bCompress = FALSE;
			if(tgaHead.imageType==10)bCompress = TRUE;
			else if(tgaHead.imageType!=2 &&tgaHead.imageType!=1){ fclose(fpr); return FALSE; }
			bTurn = (tgaHead.imageInfo.alphaInfo & 0x20) ? TRUE:FALSE; 
			width = tgaHead.imageInfo.width;
			height = tgaHead.imageInfo.height;
			bpp = tgaHead.imageInfo.depth;
			if(bpp!=8 && bpp!=24 && bpp!=32) { fclose(fpr); return FALSE; }
			wPitch = width * (bpp>>3);
			if(wPitch & 3){
				wPitch = ((wPitch>>2)+1) <<2;
			}
			pBuf = (BYTE *)cl_malloc(wPitch * height);
			if(32==bpp){
				lpSrcColor32 = (RGBA *)pBuf;
			}else if(24==bpp){
				lpSrcColor24 = (RGB *)pBuf;
			}else{
				lpSrc8 = pBuf;
			}
			bUseAlpha = FALSE;
			bAllZero = TRUE;
			if(bCompress){
				int ptr = 0,offset;
				while(ptr<width*height){
					BYTE len = (BYTE)fgetc(fpr);
					if(len<0x81){	
						for(y=0;y<(len+1);y++){
							if(false==bTurn){
								offset = (height-1-ptr/width)*wPitch +(ptr % width)*(bpp>>3);
							}else{
								offset = (ptr/width)*wPitch +(ptr % width)*(bpp>>3);
							}
							if(bpp==32){
								fread(&trgba,sizeof(RGBA),1,fpr);
								if(trgba.alpha) bAllZero = FALSE;
								if(trgba.alpha>0 && trgba.alpha<0xff) bUseAlpha = TRUE;
								*((RGBA *)((LPBYTE)lpSrcColor32 +offset)) = trgba;
							}else{
								fread(&trgb,sizeof(RGB),1,fpr);
								if(trgb.green==0xff && trgb.red==0 && trgb.blue==0){
									trgb.green = 0;
								}
								*((RGB *)((LPBYTE)lpSrcColor24 +offset)) = trgb;
							}
							ptr ++;
						}
					}else{			
						if(bpp==32){
							fread(&trgba,sizeof(RGBA),1,fpr);
							if(trgba.alpha) bAllZero = FALSE;
							if(trgba.alpha>0 && trgba.alpha<0xff) bUseAlpha = TRUE;
							for(y=0;y<(len-0x7f);y++){
								if(false==bTurn){
									offset = (height-1-ptr/width)*width +(ptr % width);
								}else{
									offset = (ptr/width)*width +(ptr % width);
								}
								lpSrcColor32[offset] = trgba;
								ptr ++;
							}
						}else{
							fread(&trgb,sizeof(RGB),1,fpr);
							if(trgb.green==0xff && trgb.red==0 && trgb.blue==0){
								trgb.green = 0;
							}
							for(y=0;y<(len-0x7f);y++){
								if(false==bTurn){
									offset = (height-1-ptr/width)*wPitch +(ptr % width)*(bpp>>3);
								}else{
									offset = (ptr/width)*wPitch +(ptr % width)*(bpp>>3);
								}
								*((RGB *)((LPBYTE)lpSrcColor24 +offset)) = trgb;
								ptr ++;
							}
						}
					}
				}
			}else switch(bpp){
			  case 32:
				rgba = new RGBA[width*height];
				fread(rgba,sizeof(RGBA)*width*height,1,fpr);
				for(y=0;y<width*height;y++){
					if(rgba[y].alpha) bAllZero = FALSE;
					if(rgba[y].alpha>0 && rgba[y].alpha<0xff) bUseAlpha = TRUE;
				}
				for(y=0;y<height;y++){
					if(false==bTurn){
						memcpy(&lpSrcColor32[width*(height-y-1)],&rgba[width*y],sizeof(DWORD)*width);
					}else{
						memcpy(&lpSrcColor32[width*y],&rgba[width*y],sizeof(DWORD)*width);
					}
				}
				break;
			  case 24:
				rgb = new RGB[width*height];
				fread(rgb,sizeof(RGB)*width*height,1,fpr);
				for(y=0;y<height;y++){
					DWORD	offset2 = width*y;
					DWORD	offset = wPitch*y;
					if(false==bTurn){
						offset = wPitch*(height-y-1);
					}
					for(x=0;x<width;x++){
						trgb = rgb[offset2+x];
						if(trgb.green==0xff && trgb.red==0 && trgb.blue==0) trgb.green = 0;
						*((RGB *)((LPBYTE)lpSrcColor24 +offset+(x*3))) = rgb[offset2+x];
					}
				}
				break;
			  case 8:
				palColor = new myRGBA[palNum];
				if(tgaHead.cmInfo[4]==32){
					fread(palColor,sizeof(myRGBA)*palNum,1,fpr);
					bpp = 9;
				}else{
					myRGB *tmpPal = new myRGB[palNum];
					fread(tmpPal,sizeof(myRGB)*palNum,1,fpr);
					for(x=0;x<palNum;x++){
						palColor[x].bAlpha = 0;
						palColor[x].bRed   = tmpPal[x].bRed;
						palColor[x].bGreen = tmpPal[x].bGreen;
						palColor[x].bBlue  = tmpPal[x].bBlue;
					}
					delete[] tmpPal;
				}
				map = new BYTE[width*height];
				fread(map,width*height,1,fpr);
				for(y=0;y<height;y++){
					if(false==bTurn){
						memcpy(&lpSrc8[wPitch*(height-y-1)], &map[width*y], width);
					}else{
						memcpy(&lpSrc8[wPitch*y], &map[width*y], width);
					}
				}
				delete[] map;
			}
			if(bpp==32 && bAllZero){
				lpSrcColor24 = new RGB[width*height];
				for(y=0;y<width*height;y++){
					lpSrcColor24[y].red =   lpSrcColor32[y].red;
					lpSrcColor24[y].green = lpSrcColor32[y].green;
					lpSrcColor24[y].blue =  lpSrcColor32[y].blue;
				}
				bpp = 24;
				bUseAlpha = FALSE;
			}
			offset.x = tgaHead.imageInfo.xOrigin;
			offset.y = tgaHead.imageInfo.yOrigin;
			if(NULL==map) delete[] map;
			if(NULL!=rgb) delete[] rgb;
			if(NULL!=rgba)delete[] rgba;
			fclose(fpr);
			if(toneConvert.bUse){
				ToneConvert();
			}
			return TRUE;
		}
	}else if(2==x){  
		fpr = fopen(lgfFile,"rb");
		if(NULL!=fpr){
			BITMAPFILEHEADER	bmpFile;
			BITMAPINFOHEADER	bmpInfo;
			fread(&bmpFile,sizeof(bmpFile),1,fpr);
			fread(&bmpInfo,sizeof(bmpInfo),1,fpr);
			width = bmpInfo.biWidth;
			height = bmpInfo.biHeight;
			bpp = (char)bmpInfo.biBitCount;
			offset.x = LOWORD(bmpInfo.biXPelsPerMeter);
			offset.y = LOWORD(bmpInfo.biYPelsPerMeter);
			wPitch = width * (bpp>>3);
			if(wPitch & 3){
				wPitch = ((wPitch>>2)+1) <<2;
			}
			pBuf = (BYTE *)cl_malloc(wPitch * height);
			if(8==bmpInfo.biBitCount){	
				palColor = new myRGBA[256];
				ZeroMemory(palColor,sizeof(myRGBA)*256);
				if(bmpInfo.biClrUsed==0)bmpInfo.biClrUsed = 256;
				fread(palColor,sizeof(RGBQUAD)*bmpInfo.biClrUsed,1,fpr);
				BOOL bUseAlpha = FALSE;
				for(x=0;x<bmpInfo.biClrUsed;x++){
					if(palColor[x].bAlpha>0)bUseAlpha = TRUE;
				}
				if(FALSE==bUseAlpha){
					for(x=0;x<bmpInfo.biClrUsed;x++) palColor[x].bAlpha = 0xff;
				}
			}
			fseek(fpr,bmpFile.bfOffBits,SEEK_SET);

			BYTE *cgBuf = (BYTE *)cl_malloc(wPitch*height);
			fread(cgBuf,wPitch*height,1,fpr);
			for(x=0;x<height;x++){
				CopyMemory(&pBuf[wPitch*x],&cgBuf[wPitch*(height-1-x)],wPitch);
			}
			cl_free(cgBuf);
			fclose(fpr);
		}
		if(toneConvert.bUse){
			ToneConvert();
		}
		return TRUE;
	}
#endif
	wsprintf(lgfFile,"%s.LGF",fname);
	int ret = readFile->ReadPackFile(arcFileNum,lgfFile,(char **)&lgfHead);
	if(0==ret)return FALSE;
	width = lgfHead->width;
	height = lgfHead->height;
	bpp = lgfHead->bpp;
	offset.x = lgfHead->offsetX;
	offset.y = lgfHead->offsetY;
	srcBuf = (BYTE *)lgfHead +sizeof(LgfHead);
	if(bpp==8 || bpp==9){	
		palColor = new myRGBA[256];
		CopyMemory(palColor,srcBuf,sizeof(myRGBA)*256);
		srcBuf += sizeof(myRGBA)*256;
		wPitch = width;
	}else{
		wPitch = width * (bpp>>3);
	}
	if(wPitch & 3){
		wPitch = ((wPitch>>2)+1) <<2;
	}
	pBuf = (BYTE *)cl_malloc(wPitch * height);
	CopyMemory(pBuf,srcBuf,wPitch *height);
	if(toneConvert.bUse){
		ToneConvert();
	}
	cl_free(lgfHead);

	return TRUE;
} // ColorBuf::loadLGF

int ColorBuf::SearchFile(pack_file_no arcFileNum, char *org_fname, char *out_fname, BOOL bPack)
{
	BOOL bGparts = FALSE;
	if(arcFileNum==pack_gparts)bGparts = TRUE;
	if(bGparts){
		wsprintf(out_fname,"gparts\\%s.tga",org_fname);
	}else{
		wsprintf(out_fname,"eventcg\\%s.tga",org_fname);
	}
	DWORD attr = GetFileAttributes(out_fname);
	if((-1)!=attr && (attr & FILE_ATTRIBUTE_ARCHIVE))return 1;
	if(bGparts){
		wsprintf(out_fname,"gparts\\%s.bmp",org_fname);
	}else{
		wsprintf(out_fname,"eventcg\\%s.bmp",org_fname);
	}
	attr = GetFileAttributes(out_fname);
	if((-1)!=attr && (attr & FILE_ATTRIBUTE_ARCHIVE))return 2;

	if(FALSE==bPack) return 0;
	char lgfFile[32];
	wsprintf(lgfFile,"%s.LGF",org_fname);
	if((-1)!=readFile->SearchFile(arcFileNum,lgfFile,FALSE)) return 3;
	return 0;
} // ColorBuf::SearchFile

BOOL ColorBuf::createColorBuf(int sWidth,int sHeight,BYTE sBpp,BYTE bClear)
{
	cl_free(pBuf);
	width = sWidth;
	height = sHeight;
	bpp = sBpp;
	offset.x = offset.y = 0;
	if(bpp < 16){
		wPitch = width;
	}else{
		wPitch = width * (bpp>>3);
	}
	if(wPitch & 3){
		wPitch = ((wPitch>>2)+1) <<2;
	}
	pBuf = (BYTE *)cl_malloc(wPitch * height);
	if(bClear) clearColorBuf();
	alpha = 1.0f;
	return TRUE;
} // ColorBuf::createColorBuf


void ColorBuf::SpreadBlt(ColorBuf *srcCBuf,int power)
{
	if(0==power)return;
	int		x,y,x2,y2,offsetX,offsetY;

	int		red,green,blue,cnt;

	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			red = green = blue = cnt = 0;
			for(x2=-power; x2<=power; x2++){
				offsetX = x+x2;
				if(offsetX<0 || offsetX>=width)continue;
				for(y2=-power; y2<=power; y2++){
					offsetY = y+y2;
					if(offsetY<0 || offsetY>=height)continue;
					red   += srcCBuf->pBuf[ srcCBuf->wPitch*offsetY +offsetX*3];
					green += srcCBuf->pBuf[ srcCBuf->wPitch*offsetY +offsetX*3+1];
					blue  += srcCBuf->pBuf[ srcCBuf->wPitch*offsetY +offsetX*3+2];
					cnt ++;
				}
			}
			pBuf[ wPitch*y +x*3]   = red /cnt;
			pBuf[ wPitch*y +x*3+1] = green /cnt;
			pBuf[ wPitch*y +x*3+2] = blue /cnt;
		}
	}
} // ColorBuf::SpreadBlt

#define noiseWait	200
BOOL ColorBuf::NoiseBlt(ColorBuf *srcCBuf)
{
	DWORD	time = timeGetTime();
	int		x,y,offset,len,pSize = (bpp>>3);
	BYTE	*outBuf,*inBuf;
	BOOL	bRaster = FALSE;

	if(saveInf.noiseTime > time)return FALSE;
	if(saveInf.noiseTime+noiseWait < time){
		saveInf.noiseTime = time +saveInf.noiseFrameMin*1000/60;
		saveInf.noiseTime += ((saveInf.noiseFrameMax -saveInf.noiseFrameMin)*rndSelect(100)*1000/60/100);
		saveInf.noiseType = rndSelect(7)+1;
		sysInf.noiseCnt = 0;
		return FALSE;
	}

	float	noiseStep = ((noiseWait/2) -abs(int(time -saveInf.noiseTime) -(noiseWait/2))) / float(noiseWait/2);

	if(saveInf.noiseType & 2){
		offset = int(700 *noiseStep);
		outBuf = pBuf;
		inBuf = srcCBuf->pBuf;
		if(offset < height){
			CopyMemory(outBuf+wPitch*offset, inBuf, wPitch*(height-offset));
			ZeroMemory(outBuf, wPitch*offset);
		}
		offset -= 100;
		if(offset>0){
			CopyMemory(outBuf, inBuf+wPitch*(height-offset), wPitch*offset);
		}
		bRaster = TRUE;
	}
	if(saveInf.noiseType & 4){
		if(FALSE==bRaster){
			CopyMemory(pBuf,srcCBuf->pBuf,wPitch*height);
		}
		if(0==sysInf.noiseCnt){
			sysInf.noiseCnt = rndSelect(3) +1;
			for(x=0;x<sysInf.noiseCnt;x++){
				sysInf.noiseW[x] = rndSelect(3)+4;
				sysInf.noiseOffset[x] = rndSelect(500);
			}
		}
		for(x=0;x<sysInf.noiseCnt;x++){
			for(y=0;y<300;y++){
				if(sysInf.noiseOffset[x]+y >= WIN_SIZEY)continue;
				outBuf = pBuf+wPitch*(y+sysInf.noiseOffset[x]);
				float wide = 1+ sysInf.noiseW[x]*0.01f;
				offset = int(130*noiseStep*powf(wide,float(300-y))/powf(wide,float(300)));
				if(x==1)offset *= -1;
				if(offset >=0){
					MoveMemory(outBuf +offset*pSize, outBuf, (width-offset)*pSize);
					ZeroMemory(outBuf, offset*pSize);
				}else{
					MoveMemory(outBuf, outBuf-offset*pSize, (width+offset)*pSize);
					ZeroMemory(outBuf+((width+offset)*pSize), -offset*pSize);
				}
			}
		}
		bRaster = TRUE;
	}
	if(saveInf.noiseType){
		if(FALSE==bRaster){
			outBuf = pBuf;
			inBuf = srcCBuf->pBuf;
			CopyMemory(outBuf,inBuf,wPitch*height);
		}
		int step = (time - saveInf.noiseTime) /10;
		if(step >= 20) step = 19;
		rect.left = 0; rect.right = WIN_SIZEX;
		rect.top = step *WIN_SIZEY;	rect.bottom = rect.top +WIN_SIZEY;
		BltFast(0,0,&noisePatternBuf,&rect,FALSE,TRUE);
	}
	return TRUE;
} // ColorBuf::NoiseBlt

BOOL ColorBuf::SpiralBlt(int centerX,int centerY, ColorBuf *srcCBuf,RECT *srcRect,float step,float size)
{
	POINT	dPoint;
	float	tmpX,tmpY;
	BYTE	*destP,*srcP, as,ad;
	float	rad,ratio,len,maxLen,circleW;
	int		x,y;
	int		srcW = srcRect->right - srcRect->left;
	int		srcH = srcRect->bottom - srcRect->top;
	int		destW = int(srcW*size);
	int		destH = int(srcH*size);

	dPoint.x = LONG(centerX -destW/2);
	dPoint.y = LONG(centerY -destH/2);
	for(y=0;y<destH;y++){
		destP = pBuf+wPitch*(dPoint.y+y) +dPoint.x*3;
		for(x=0;x<destW;x++){
			tmpX = float(x -destW/2);
			tmpY = float(y -destH/2);
			len = lineLength(tmpX, tmpY, 0, 0);
			rad = atan2f(tmpX,-tmpY);
			circleW = destW*sqrtf(2)/2;
			tmpX = sinf(rad) *circleW;
			tmpY = -cosf(rad) *circleW;
			if(fabsf(tmpX) > destW/2){
				tmpY = tmpY *(destW/2) /fabsf(tmpX);
			}
			if(fabsf(tmpY) > destW/2){
				tmpX = tmpX *(destW/2) /fabsf(tmpY);
			}
			maxLen = lineLength(tmpX, tmpY, 0, 0);
			ratio = len / maxLen;
			if(ratio > 1.0f)ratio = 1.0f;

			rad += M_PI*2*(1.0f-sinf(step*(M_PI/2)))*(1.0f-ratio);

			circleW = srcW*sqrtf(2)/2;
			tmpX = sinf(rad) *circleW;
			tmpY = -cosf(rad) *circleW;
			if(fabsf(tmpX) > srcW/2){
				tmpY =tmpY *(srcW/2) /fabsf(tmpX);
			}
			if(fabsf(tmpY) > srcW/2){
				tmpX = tmpX *(srcW/2) /fabsf(tmpY);
			}
			tmpX = tmpX*ratio +srcW/2;
			tmpY = tmpY*ratio +srcW/2;

			if(tmpX<0 || tmpY<0 || tmpX>=srcW || tmpY>=srcW){
				if(srcCBuf->bpp==8) destP[x*3+0] = destP[x*3+1] = destP[x*3+2] = 0xff;
				continue;
			}
			switch(srcCBuf->bpp){
			  case 32:
				srcP = srcCBuf->pBuf +srcCBuf->wPitch*(int(tmpY)+srcRect->top) +int(tmpX)*4;
				as = srcP[3];
				ad = 0xff -as;
				if(as>0){
					destP[x*3+0] = AlphaTable[srcP[0]][as]
								+AlphaTable[destP[x*3+0]][ad];
					destP[x*3+1] = AlphaTable[srcP[1]][as]
								+AlphaTable[destP[x*3+1]][ad];
					destP[x*3+2] = AlphaTable[srcP[2]][as]
								+AlphaTable[destP[x*3+2]][ad];
				}
				break;
			  case 24:
				srcP = srcCBuf->pBuf +srcCBuf->wPitch*(int(tmpY)+srcRect->top) +int(tmpX)*3;
				destP[x*3+0] = srcP[0];
				destP[x*3+1] = srcP[1];
				destP[x*3+2] = srcP[2];
				break;
			  case 8:
				srcP = srcCBuf->pBuf +srcCBuf->wPitch*(int(tmpY)+srcRect->top) +int(tmpX);
				destP[x*3+0] = srcCBuf->palColor[srcP[0]].bBlue;
				destP[x*3+1] = srcCBuf->palColor[srcP[0]].bGreen;
				destP[x*3+2] = srcCBuf->palColor[srcP[0]].bRed;
				break;
			  case 9:
				srcP = srcCBuf->pBuf +srcCBuf->wPitch*(int(tmpY)+srcRect->top) +int(tmpX);
				as = srcCBuf->palColor[srcP[0]].bAlpha;
				ad = 0xff -as;
				destP[x*3+0] = AlphaTable[srcCBuf->palColor[srcP[0]].bBlue][as]
						+AlphaTable[destP[x*3+0]][ad];
				destP[x*3+1] = AlphaTable[srcCBuf->palColor[srcP[0]].bGreen][as]
						+AlphaTable[destP[x*3+1]][ad];
				destP[x*3+2] = AlphaTable[srcCBuf->palColor[srcP[0]].bRed][as]
						+AlphaTable[destP[x*3+2]][ad];
				break;
			}
		}
	}
	return TRUE;
}// ColorBuf::SpiralBlt

BOOL ColorBuf::BltFast(int xDest,int yDest,ColorBuf *srcCBuf,RECT *srcRect,BOOL bAlpha,BOOL bAdd,BOOL bFast)
{
	int		x,y;
	int xSrc, ySrc, widthSrc, heightSrc;

	if(srcCBuf->alpha>=1.0f)bAlpha = FALSE;

	if(NULL==srcRect){
		xSrc = ySrc = 0;
		widthSrc = srcCBuf->width;
		heightSrc = srcCBuf->height;
		xDest += srcCBuf->offset.x;
		yDest += srcCBuf->offset.y;
	}else{
		xSrc = srcRect->left;
		ySrc = srcRect->top;
		widthSrc = srcRect->right -srcRect->left;
		heightSrc = srcRect->bottom -srcRect->top;
	}
	if(xDest < 0){
		widthSrc += xDest;
		if(widthSrc < 0)return FALSE;
		xSrc = -xDest;
		xDest = 0;
	}
	if(yDest < 0){
		heightSrc += yDest;
		if(heightSrc < 0)return FALSE;
		ySrc = -yDest;
		yDest = 0;
	}
	if(width <= xDest)return FALSE;
	if(width < (xDest+widthSrc)){
		widthSrc = width -xDest;
	}
	if(height <= yDest)return FALSE;
	if(height < (yDest+heightSrc)){
		heightSrc = height -yDest;
	}
	if(0==widthSrc || 0==heightSrc)return TRUE;
	LPBYTE	pDest = pBuf +wPitch*yDest +xDest*(bpp>>3);
	LPBYTE	pSrc = srcCBuf->pBuf +(srcCBuf->wPitch)*ySrc +xSrc*(srcCBuf->bpp>>3);
	if(bAdd){
		if(bpp==24){
			switch(srcCBuf->bpp){
			  case 8:
				Add8to24A((myRGB *)pDest,wPitch, pSrc, srcCBuf->wPitch, srcCBuf->palColor, widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			  case 9:
				Add9to24A((myRGB *)pDest,wPitch, pSrc, srcCBuf->wPitch, srcCBuf->palColor, widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			  case 24:
				Add24to24A((myRGB *)pDest,wPitch, (myRGB *)pSrc,srcCBuf->wPitch, widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
				case 32:
				Add32to24A((myRGB *)pDest,wPitch, (myRGBA *)pSrc,srcCBuf->wPitch, widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			}
		}
		return TRUE;
	}
	if(FALSE==bAlpha){
		if(bpp==srcCBuf->bpp){
			CopyEqualColor(pDest,wPitch, pSrc,srcCBuf->wPitch, widthSrc,heightSrc,bpp);
		}else switch(srcCBuf->bpp){
		  case 8:
			Copy8to24((myRGB *)pDest,wPitch, pSrc,srcCBuf->wPitch, srcCBuf->palColor, widthSrc,heightSrc,bFast);
			break;
		  case 9:
			Copy9to24((myRGB *)pDest,wPitch, pSrc,srcCBuf->wPitch, srcCBuf->palColor, widthSrc,heightSrc,bFast);
			break;
		  case 24:
			Copy24to32((myRGBA *)pDest,wPitch, (myRGB *)pSrc,srcCBuf->wPitch, widthSrc,heightSrc,bFast);
			break;
		  case 32:
			Copy32to24((myRGB *)pDest,wPitch, (myRGBA *)pSrc,srcCBuf->wPitch, widthSrc,heightSrc,bFast);
			break;
		}
	}else{
		if(bpp==24){
			switch(srcCBuf->bpp){
			case 8:
				Copy8to24A((myRGB *)pDest,wPitch, pSrc,srcCBuf->wPitch, srcCBuf->palColor, widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			case 9:
				Copy9to24A((myRGB *)pDest,wPitch, pSrc,srcCBuf->wPitch, srcCBuf->palColor, widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			case 24:
				Copy24to24A((myRGB *)pDest,wPitch, (myRGB *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			case 32:
				Copy32to24A((myRGB *)pDest,wPitch, (myRGBA *)pSrc,srcCBuf->wPitch, widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			}
		}else if(bpp==32){
			switch(srcCBuf->bpp){
			case 8:
				Copy8to32A((myRGBA *)pDest,wPitch, pSrc,srcCBuf->wPitch, srcCBuf->palColor, widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			case 24:
				Copy24to32A((myRGBA *)pDest,wPitch, (myRGB *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			case 32:
				Copy32to32A((myRGBA *)pDest,wPitch, (myRGBA *)pSrc,srcCBuf->wPitch, widthSrc,heightSrc,srcCBuf->alpha,bFast);
				break;
			}
		}
	}
	return TRUE;
} // ColorBuf::BltFast

BOOL ColorBuf::Blt(RECT *dstRect,ColorBuf *srcCBuf,RECT *srcRect,BOOL bAlpha,BOOL bAdd,BOOL bFast)
{
	int		x,y;
	int xDest, yDest, widthDest, heightDest;
	int	xSrc, ySrc, widthSrc, heightSrc;

	if(srcCBuf->alpha>=1.0f)bAlpha = FALSE;
	if(NULL==srcRect){
		xSrc = ySrc = 0;
		widthSrc = srcCBuf->width;
		heightSrc = srcCBuf->height;
	}else{
		xSrc = srcRect->left;
		ySrc = srcRect->top;
		widthSrc = srcRect->right -srcRect->left;
		heightSrc = srcRect->bottom -srcRect->top;
	}
	if(NULL==dstRect){
		xDest = yDest = 0;
		widthDest = width;
		heightDest = height;
	}else{
		xDest = dstRect->left;
		yDest = dstRect->top;
		widthDest = dstRect->right -dstRect->left;
		heightDest = dstRect->bottom -dstRect->top;
	}
	if(xDest < 0){
		x = widthSrc*(-xDest)/widthDest;
		xSrc += x;
		widthSrc -= x;
		widthDest += xDest;
		xDest = 0;
	}
	if(dstRect && dstRect->right >= width){
		x = widthSrc*(dstRect->right - width)/widthDest;
		widthSrc -= x;
		widthDest -= (dstRect->right - width);
	}
	if(yDest < 0){
		y = heightSrc*(-yDest)/heightDest;
		ySrc += y;
		heightSrc -= y;
		heightDest += yDest;
		yDest = 0;
	}
	if(dstRect && dstRect->bottom >= height){
		y = heightSrc*(dstRect->bottom - height)/heightDest;
		heightSrc -= y;
		heightDest -= (dstRect->bottom - height);
	}
	if(widthSrc==widthDest && heightSrc==heightDest){
		RECT newSrcRect = {xSrc, ySrc, xSrc+widthSrc, ySrc+heightSrc};
		return (BltFast(xDest,yDest,srcCBuf,&newSrcRect,bAlpha,bAdd,bFast));
	}
	if(0==widthDest || 0==heightDest || 0==widthSrc || 0==heightSrc)return TRUE;
	LPBYTE	pDest = pBuf +wPitch*yDest +xDest*(bpp>>3);
	LPBYTE	pSrc = srcCBuf->pBuf +(srcCBuf->wPitch)*ySrc +xSrc*(srcCBuf->bpp>>3);

	if(bAdd){	
		switch(srcCBuf->bpp){
		  case 8:
			StretchAdd8to24A((myRGB *)pDest,wPitch,widthDest,heightDest,pSrc,srcCBuf->palColor,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha,bFast);
			break;
		  case 9:
			StretchAdd9to24A((myRGB *)pDest,wPitch,widthDest,heightDest,pSrc,srcCBuf->palColor,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha,bFast);
			break;
		  case 24:
			StretchAdd24to24A((myRGB *)pDest,wPitch,widthDest,heightDest,(myRGB *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha,bFast);
			break;
		  case 32:
			StretchAdd32to24A((myRGB *)pDest,wPitch,widthDest,heightDest,(myRGBA *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha,bFast);
			break;
		}
		return TRUE;
	}
	if(FALSE==bAlpha){
		switch(srcCBuf->bpp){
		  case 8:
			StretchCopy8to24((myRGB *)pDest,wPitch,widthDest,heightDest,pSrc,srcCBuf->palColor,srcCBuf->wPitch,widthSrc,heightSrc,bFast);
		  case 9:
			StretchCopy9to24((myRGB *)pDest,wPitch,widthDest,heightDest,pSrc,srcCBuf->palColor,srcCBuf->wPitch,widthSrc,heightSrc,bFast);
			break;
		  case 24:
			StretchCopy24to24((myRGB *)pDest,wPitch,widthDest,heightDest,(myRGB *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc,bFast);
			break;
		  case 32:
			StretchCopy32to24((myRGB *)pDest,wPitch,widthDest,heightDest,(myRGBA *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc,bFast);
			break;
		}
	}else{
		switch(srcCBuf->bpp){
		  case 8:
			StretchCopy8to24A((myRGB *)pDest,wPitch,widthDest,heightDest,pSrc,srcCBuf->palColor,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha,bFast);
		  case 9:
			StretchCopy9to24A((myRGB *)pDest,wPitch,widthDest,heightDest,pSrc,srcCBuf->palColor,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha, bFast);
			break;
		  case 24:
			StretchCopy24to24A((myRGB *)pDest,wPitch,widthDest,heightDest,(myRGB *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha,bFast);
			break;
		  case 32:
			StretchCopy32to24A((myRGB *)pDest,wPitch,widthDest,heightDest,(myRGBA *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha,bFast);
			break;
		}
	}
	return TRUE;
} // ColorBuf::Blt

BOOL ColorBuf::BltFast2(int xDest,int yDest,ColorBuf *srcCBuf1,ColorBuf *srcCBuf2,RECT *srcRect,int ptnNum,int ptnMax)
{
	int		x,y;
	int		xSrc, ySrc, widthSrc, heightSrc;
	int		alpha = MaxMin(0xFF, 0xffL *ptnNum/ptnMax);

	if(NULL==srcRect){
		xSrc = ySrc = 0;
		widthSrc = srcCBuf1->width;
		heightSrc = srcCBuf1->height;
		xDest += srcCBuf1->offset.x;
		yDest += srcCBuf1->offset.y;
	}else{
		xSrc = srcRect->left;
		ySrc = srcRect->top;
		widthSrc = srcRect->right -srcRect->left;
		heightSrc = srcRect->bottom -srcRect->top;
	}
	myRGB	*pDest = (myRGB *)(pBuf +wPitch*yDest +xDest*(bpp>>3));
	myRGB	*pSrc1 = (myRGB *)(srcCBuf1->pBuf +(srcCBuf1->wPitch)*ySrc +xSrc*(srcCBuf1->bpp>>3));
	myRGB	*pSrc2 = (myRGB *)(srcCBuf2->pBuf +(srcCBuf2->wPitch)*ySrc +xSrc*(srcCBuf2->bpp>>3));
	if(width < (xDest+widthSrc)){
		widthSrc = width -xDest;
	}
	if(height < (yDest+heightSrc)){
		heightSrc = height -yDest;
	}
	int	start = 0, step = 1;
	for(y=0;y<heightSrc;y++){
		if(sysInf.g_highSpeed){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthSrc;x+=step){
			pDest[x].bRed = AlphaTable[pSrc1[x].bRed][0xff-alpha]
				+AlphaTable[pSrc2[x].bRed][alpha];
			pDest[x].bGreen = AlphaTable[pSrc1[x].bGreen][0xff-alpha]
				+AlphaTable[pSrc2[x].bGreen][alpha];
			pDest[x].bBlue = AlphaTable[pSrc1[x].bBlue][0xff-alpha]
				+AlphaTable[pSrc2[x].bBlue][alpha];
		}
		pDest = (myRGB *)((BYTE *)pDest +wPitch);
		pSrc1 = (myRGB *)((BYTE *)pSrc1 +srcCBuf1->wPitch);
		pSrc2 = (myRGB *)((BYTE *)pSrc2 +srcCBuf2->wPitch);
	}
	return TRUE;
} // ColorBuf::BltFast2

BOOL ColorBuf::BrightBlt(int xDest,int yDest,ColorBuf *srcCBuf,RECT *srcRect,int r, int g, int b, BOOL bFast)
{
	int		x,y;
	int		xSrc, ySrc, widthSrc, heightSrc;
	BOOL	bR=FALSE,bG=FALSE,bB=FALSE;

	if(NULL==srcRect){	
		xSrc = ySrc = 0;
		widthSrc = srcCBuf->width;
		heightSrc = srcCBuf->height;
		xDest += srcCBuf->offset.x;
		yDest += srcCBuf->offset.y;
	}else{
		xSrc = srcRect->left;
		ySrc = srcRect->top;
		widthSrc = srcRect->right -srcRect->left;
		heightSrc = srcRect->bottom -srcRect->top;
	}
	BYTE	*pDest = pBuf +wPitch*yDest +xDest*(bpp>>3);
	BYTE	*pSrc  = srcCBuf->pBuf +(srcCBuf->wPitch)*ySrc +xSrc*(srcCBuf->bpp>>3);
	if(width < (xDest+widthSrc)){
		widthSrc = width -xDest;
	}
	if(height < (yDest+heightSrc)){
		heightSrc = height -yDest;
	}
	if(24==bpp){
		if(24==srcCBuf->bpp){
			BrightCopy24to24((myRGB *)pDest, wPitch, (myRGB *)pSrc, srcCBuf->wPitch, widthSrc, heightSrc, r, g, b, bFast);
		}
	}else if(32==bpp){
		if(32==srcCBuf->bpp){
			BrightCopy32to32((myRGBA *)pDest, wPitch, (myRGBA *)pSrc, srcCBuf->wPitch, widthSrc, heightSrc, r, g, b, bFast);
		}
	}
	return TRUE;
} // ColorBuf::BrightBlt

void BrightCopy24to24(myRGB *pDest, int destP_W, myRGB *pSrc, int srcP_W, int width, int height, int r, int g, int b, BOOL bFast)
{
	int		x, y, start = 0, step = 1;
	BOOL	bR=FALSE,bG=FALSE,bB=FALSE;

	if(0x80>=r){
		bR = TRUE;
		if(0x80>r)r *= 2;
		else r=0xff;
	}else{
		r = (r-0x80)*2;
	}
	if(0x80>=g){
		bG = TRUE;
		if(0x80>g)g *= 2;
		else g=0xff;
	}else{
		g = (g-0x80)*2;
	}
	if(0x80>=b){
		bB = TRUE;
		if(0x80>b)b *= 2;
		else b=0xff;
	}else{
		b = (b-0x80)*2;
	}
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			if(bR)pDest[x].bRed   = AlphaTable[pSrc[x].bRed][r];
			else pDest[x].bRed = pSrc[x].bRed +(0xff-pSrc[x].bRed)*r/0xff;
			if(bG)pDest[x].bGreen = AlphaTable[pSrc[x].bGreen][g];
			else pDest[x].bGreen = pSrc[x].bGreen +(0xff-pSrc[x].bGreen)*g/0xff;
			if(bB)pDest[x].bBlue  = AlphaTable[pSrc[x].bBlue][b];
			else pDest[x].bBlue = pSrc[x].bBlue +(0xff-pSrc[x].bBlue)*b/0xff;
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
		pSrc = (myRGB *)((BYTE *)pSrc +srcP_W);
	}
} // BrightCopy24to24

void BrightCopy32to32(myRGBA *pDest, int destP_W, myRGBA *pSrc, int srcP_W, int width, int height, int r, int g, int b, BOOL bFast)
{
	int		x, y, start = 0, step = 1;
	BOOL	bR=FALSE,bG=FALSE,bB=FALSE;

	if(0x80>=r){
		bR = TRUE;
		if(0x80>r)r *= 2;
		else r=0xff;
	}else{
		r = (r-0x80)*2;
	}
	if(0x80>=g){
		bG = TRUE;
		if(0x80>g)g *= 2;
		else g=0xff;
	}else{
		g = (g-0x80)*2;
	}
	if(0x80>=b){
		bB = TRUE;
		if(0x80>b)b *= 2;
		else b=0xff;
	}else{
		b = (b-0x80)*2;
	}
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			if(bR)pDest[x].bRed   = AlphaTable[pSrc[x].bRed][r];
			else pDest[x].bRed = pSrc[x].bRed +(0xff-pSrc[x].bRed)*r/0xff;
			if(bG)pDest[x].bGreen = AlphaTable[pSrc[x].bGreen][g];
			else pDest[x].bGreen = pSrc[x].bGreen +(0xff-pSrc[x].bGreen)*g/0xff;
			if(bB)pDest[x].bBlue  = AlphaTable[pSrc[x].bBlue][b];
			else pDest[x].bBlue = pSrc[x].bBlue +(0xff-pSrc[x].bBlue)*b/0xff;
			pDest[x].bAlpha = pSrc[x].bAlpha;
		}
		pDest = (myRGBA *)((BYTE *)pDest +destP_W);
		pSrc = (myRGBA *)((BYTE *)pSrc +srcP_W);
	}
} // BrightCopy32to32


BOOL ColorBuf::WaveBlt(int xDest,int yDest,ColorBuf *srcCBuf, RECT *srcRect, SHORT shakeSize,DWORD wave_cnt)
{
	int		x,y,line;
	int xSrc, ySrc, widthSrc, heightSrc;
	float cycle,top;

	shakeSize /= 2;
	wave_cnt *= 2;

	if(NULL==srcRect){	
		xSrc = ySrc = 0;
		widthSrc = srcCBuf->width;
		heightSrc = srcCBuf->height;
		xDest += srcCBuf->offset.x;
		yDest += srcCBuf->offset.y;
	}else{
		xSrc = srcRect->left;
		ySrc = srcRect->top;
		widthSrc = srcRect->right -srcRect->left;
		heightSrc = srcRect->bottom -srcRect->top;
	}
	LPBYTE	pDest = pBuf +wPitch*yDest +xDest*(bpp>>3);
	LPBYTE	pSrc = srcCBuf->pBuf +(srcCBuf->wPitch)*ySrc +xSrc*(srcCBuf->bpp>>3);
	myRGB	*pRGB;
	if(width < (xDest+widthSrc)){
		widthSrc = width -xDest;
	}
	if(height < (yDest+heightSrc)){
		heightSrc = height -yDest;
	}
	top = sqrtf(powf(shakeSize,2) +powf(90,2));
	for(y=0;y<heightSrc;y++){
		cycle = float((wave_cnt+y) % 360);
		cycle /= 2;
		if(cycle < 90){
			line = int((sqrtf(powf(sinf(DEGtoRAD(cycle))*shakeSize,2) + powf(cycle,2)) / top) *180);
		}else{
			line = int((sqrtf(powf( (1-sinf(DEGtoRAD(cycle))) *shakeSize,2) + powf(cycle-90,2)) / top) *180);
		}
		line = line + (wave_cnt+y) /180 *180 -wave_cnt;
		if(line<0 || line>(WIN_SIZEY-1)){
			ZeroMemory(pDest +wPitch*y, wPitch); 
			continue;
		}
		CopyMemory(pDest +wPitch*y, pSrc +(srcCBuf->wPitch)*line,wPitch);
	}
	return TRUE;
} // ColorBuf::WaveBlt

BOOL ColorBuf::PtnFade(int xDest,int yDest,ColorBuf *srcCBuf1,ColorBuf *srcCBuf2,RECT *srcRect,int ptnNum,int ptnMax)
{
	int		x,y;
	int		xSrc, ySrc, widthSrc, heightSrc;
	int		alpha,add = 0x1fe *ptnNum/ptnMax;
	BYTE	*ptnBuf = ptnFadeBuf.pBuf,outPt;

	BOOL bReverse = 0x0100 & bgInf.in_type;
	BOOL bRevW    = 0x0800 & bgInf.in_type;
	BOOL bRevH    = 0x1000 & bgInf.in_type;

	if(NULL==srcRect){	
		xSrc = ySrc = 0;
		widthSrc = srcCBuf1->width;
		heightSrc = srcCBuf1->height;
		xDest += srcCBuf1->offset.x;
		yDest += srcCBuf1->offset.y;
	}else{
		xSrc = srcRect->left;
		ySrc = srcRect->top;
		widthSrc = srcRect->right -srcRect->left;
		heightSrc = srcRect->bottom -srcRect->top;
	}
	myRGB	*pDest = (myRGB *)(pBuf +wPitch*yDest +xDest*(bpp>>3));
	myRGB	*pSrc1 = (myRGB *)(srcCBuf1->pBuf +(srcCBuf1->wPitch)*ySrc +xSrc*(srcCBuf1->bpp>>3));
	myRGB	*pSrc2 = (myRGB *)(srcCBuf2->pBuf +(srcCBuf2->wPitch)*ySrc +xSrc*(srcCBuf2->bpp>>3));
	if(width < (xDest+widthSrc)){
		widthSrc = width -xDest;
	}
	if(height < (yDest+heightSrc)){
		heightSrc = height -yDest;
	}
	int start = 0, step = 1;
	for(y=0;y<heightSrc;y++){
		if(sysInf.g_highSpeed){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		if(!bRevH){
			ptnBuf = ptnFadeBuf.pBuf + ptnFadeBuf.wPitch*y;
		}else{
			ptnBuf = ptnFadeBuf.pBuf + ptnFadeBuf.wPitch*(heightSrc-y-1);
		}
		for(x=start;x<widthSrc;x+=step){
			if(!bRevW)outPt = ptnBuf[x];
			else outPt = ptnBuf[widthSrc-x-1];
			if(FALSE==bReverse){
				alpha = add +outPt -0xff;
			}else{
				alpha = add -outPt;
			}
			if(alpha>0xff)alpha = 0xff;
			else if(alpha<0)alpha = 0;

			pDest[x].bRed = AlphaTable[pSrc1[x].bRed][0xff-alpha]
				+AlphaTable[pSrc2[x].bRed][alpha];
			pDest[x].bGreen = AlphaTable[pSrc1[x].bGreen][0xff-alpha]
				+AlphaTable[pSrc2[x].bGreen][alpha];
			pDest[x].bBlue = AlphaTable[pSrc1[x].bBlue][0xff-alpha]
				+AlphaTable[pSrc2[x].bBlue][alpha];
		}
		pDest = (myRGB *)((BYTE *)pDest +wPitch);
		pSrc1 = (myRGB *)((BYTE *)pSrc1 +srcCBuf1->wPitch);
		pSrc2 = (myRGB *)((BYTE *)pSrc2 +srcCBuf2->wPitch);
	}
	return TRUE;
} // ColorBuf::PtnFade

BOOL ColorBuf::ShakeBlt(ColorBuf *srcCBuf,int s_size,int s_direct,int step,int stepMax)
{
	RECT	shakeRect = bgRect;
	POINT	dstPt;
	int		add, add_x, add_y;
	float	ang;

	switch(bgInf.in_type){
	  case _BAK_SIN:
	  case _BAK_ALL_SIN:
	  case _BAK_2TI:
	  case _BAK_ALL_2TI:
		if(stepMax>0 && (_BAK_SIN==bgInf.in_type || _BAK_ALL_SIN==bgInf.in_type)){
			s_size = (s_size*(stepMax-step) /stepMax );
		}
		add = int (s_size *sinf(DEGtoRAD(3*step)));
		switch(s_direct){
		  case _DIR_U:  case _DIR_D:
			add_x = 0; add_y = add;
			break;
		  case _DIR_L:  case _DIR_R:
			add_x = add; add_y = 0;
			break;
		  case _DIR_DL: case _DIR_UR:
			add_x = -int(add *0.7f);
			add_y = int(add *0.7f);
			break;
		  case _DIR_DR: case _DIR_UL:
			add_x = int(add *0.7f);
			add_y = int(add *0.7f);
			break;
		}
		shakeRect.left += add_x;
		shakeRect.right = shakeRect.top +WIN_SIZEX;
		shakeRect.top += add_y;
		shakeRect.bottom = shakeRect.top +WIN_SIZEY;
		dstPt.x = dstPt.y = 0;
		if(shakeRect.left < 0){
			dstPt.x = -shakeRect.left;
			shakeRect.left = 0;
		}
		if(shakeRect.right > WIN_SIZEX){
			shakeRect.right = WIN_SIZEX;
		}
		if(shakeRect.top < 0){
			dstPt.y = -shakeRect.top;
			shakeRect.top = 0;
		}
		if(shakeRect.bottom > WIN_SIZEY){
			shakeRect.bottom = WIN_SIZEY;
		}
		BltFast(dstPt.x,dstPt.y,srcCBuf,&shakeRect,FALSE);
		break;
	  case _BAK_RAND:
	  case _BAK_ALL_RAND:
		if(stepMax>0){
			s_size = (s_size*(stepMax-step) /stepMax );
		}
		ang = DEGtoRAD(rndSelect(360));
		shakeRect.top += int(sinf(ang)*s_size);
		shakeRect.bottom = shakeRect.top +WIN_SIZEY;
		shakeRect.left += int(cosf(ang)*s_size);
		shakeRect.right = shakeRect.left +WIN_SIZEX;
		dstPt.x = dstPt.y = 0;
		if(shakeRect.left < 0){
			dstPt.x = -shakeRect.left;
			shakeRect.left = 0;
		}
		if(shakeRect.right > WIN_SIZEX){
			shakeRect.right = WIN_SIZEX;
		}
		if(shakeRect.top < 0){
			dstPt.y = -shakeRect.top;
			shakeRect.top = 0;
		}
		if(shakeRect.bottom > WIN_SIZEY){
			shakeRect.bottom = WIN_SIZEY;
		}
		BltFast(dstPt.x,dstPt.y,srcCBuf,&shakeRect,FALSE);
		break;
	  case _BAK_ZOOM:
		if(step <= stepMax/2){
			add = (s_size * step /stepMax);
		}else{
			add = (s_size * (stepMax-step) /stepMax);
		}
		shakeRect.left += add;
		shakeRect.right -= add;
		shakeRect.top += add;
		shakeRect.bottom -= add;
		Blt(NULL,srcCBuf,&shakeRect,FALSE);
		break;
	}
	return TRUE;
} // ColorBuf::ShakeBlt

BOOL ColorBuf::RollBlt(ColorBuf *srcCBuf, float deg)
{
	int		x,y;
	int		ah[2],aw[2];
	float	len,rad = DEGtoRAD(deg);
	POINT	pt[4],st_p;
	myRGB	*pDest,*pSrc;
	BOOL	bIn;

	len = sqrtf(float(width*width +height*height))/2;
	float angle = atanf(float(width)/height);

	rect.left = 0;	rect.right = rect.left +width;
	rect.top = 0; rect.bottom = rect.top  +height;
	pt[0].x = int(2048*(sinf(-angle-rad)*len +srcCBuf->width/2));
	pt[0].y = int(2048*(-cosf(-angle-rad)*len +srcCBuf->height/2));
	pt[1].x = int(2048*(sinf(angle-rad)*len +srcCBuf->width/2));
	pt[1].y = int(2048*(-cosf(angle-rad)*len +srcCBuf->height/2));
	pt[2].x = int(2048*(sinf(-(M_PI-angle)-rad)*len +srcCBuf->width/2));
	pt[2].y = int(2048*(-cosf(-(M_PI-angle)-rad)*len +srcCBuf->height/2));

	aw[0] = (pt[2].x-pt[0].x) /height;
	ah[0] = (pt[2].y-pt[0].y) /height;
	aw[1] = (pt[1].x-pt[0].x) /width;
	ah[1] = (pt[1].y-pt[0].y) /width;
	pDest = (myRGB *)pBuf;
	int start = 0, step = 1;
	for(y=0;y<height;y++){
		st_p.x = pt[0].x +aw[0]*y;
		st_p.y = pt[0].y +ah[0]*y;
		bIn = FALSE;
		if(sysInf.g_highSpeed){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			point.x = (st_p.x +aw[1]*x)>>11;
			point.y = (st_p.y +ah[1]*x)>>11;
			if(point.x<rect.left || point.y<rect.top || point.x>rect.right || point.y>=rect.bottom){
				if(bIn)break;
				continue;
			}
			bIn = TRUE;
			pSrc = (myRGB *)(srcCBuf->pBuf +srcCBuf->wPitch*point.y);
			pDest[x] = pSrc[point.x];

		}
		pDest = (myRGB *)((char *)pDest +wPitch);
	}
	return TRUE;
} // ColorBuf::RollBlt


BOOL ColorBuf::colorBlt24(myRGBA *color,RECT *lpRect)
{
	int	x,y;
	int dWidth  = lpRect->right -lpRect->left;
	int dHeight = lpRect->bottom -lpRect->top;
	myRGB	*ptr = (myRGB *)(pBuf +wPitch*lpRect->top +lpRect->left*3);
	for(y=0;y<dHeight;y++){
		for(x=0;x<dWidth;x++){
			ptr[x].bRed   = AlphaTable[color->bRed  ][color->bAlpha] +AlphaTable[ptr[x].bRed  ][0xFF-color->bAlpha];
			ptr[x].bGreen = AlphaTable[color->bGreen][color->bAlpha] +AlphaTable[ptr[x].bGreen][0xFF-color->bAlpha];
			ptr[x].bBlue  = AlphaTable[color->bBlue ][color->bAlpha] +AlphaTable[ptr[x].bBlue ][0xFF-color->bAlpha];
		}
		ptr = (myRGB *)((char *)ptr +wPitch);
	}
	return TRUE;
} // ColorBuf::colorBlt

void ColorBuf::ToneConvert()
{
	if(bpp<24 || (type!=Type_back && type!=Type_char))return;
	if(type==Type_char && saveInf.tone_backOnly)return;
	toneConvert.Convert(pBuf,width,height,wPitch,bpp,type);
} // ColorBuf::ToneConvert

void drawChar(int layer)
{
	int		j;
	float	alpha;

	if(off==sysInf.charFlag)return;
	for(j=0;j<STCHAR_MAX;j++){
		if(0==charInf[j].save.char_no || layer!=charInf[j].save.layer) continue;
		if(_CHR_FADE==charInf[j].out_type){
			alpha = GetEffectStep(charInf[j].look_cnt,charInf[j].look_max,FALSE) *charInf[j].save.alpha /255.0f;
			if(NULL==charInf[j].bak_colorBuf.pBuf){
				charInf[j].colorBuf.alpha = alpha;
				backColorBuf.BltFast(0,0,&charInf[j].colorBuf,NULL,TRUE);
			}
			continue;
		}
		switch(charInf[j].in_type){
		  case _CHR_WAIT:	
			break;
		  case _CHR_FADE:	
			alpha = GetEffectStep(charInf[j].look_cnt,charInf[j].look_max,TRUE) *charInf[j].save.alpha /255.0f;
			if(NULL==charInf[j].bak_colorBuf.pBuf){
				charInf[j].colorBuf.alpha = alpha;
				backColorBuf.BltFast(0,0,&charInf[j].colorBuf,NULL,TRUE);
			}else{	
				if(charInf[j].save.alpha == 0xff){
					Copy32_2to24A(&backColorBuf,
						&charInf[j].colorBuf, charInf[j].colorBuf.offset.x, charInf[j].colorBuf.offset.y,
						&charInf[j].bak_colorBuf, charInf[j].bak_colorBuf.offset.x, charInf[j].bak_colorBuf.offset.y,
						alpha);
				}else{
					charInf[j].bak_colorBuf.alpha = charInf[j].save.alpha/255.0f -alpha;
					backColorBuf.BltFast(0,0,&charInf[j].bak_colorBuf,NULL,TRUE);
					charInf[j].colorBuf.alpha = alpha;
					backColorBuf.BltFast(0,0,&charInf[j].colorBuf,NULL,TRUE);
				}
			}
			break;
		  case _CHR_DIRECT:	
			charInf[j].colorBuf.alpha = charInf[j].save.alpha /255.0f;
			backColorBuf.BltFast(0,0,&charInf[j].colorBuf,NULL,TRUE);
			break;
		}
	}
} // drawChar

void drawBG()
{
	if(off==sysInf.bUseBG || off==sysInf.bgFlag)return;
	backColorBuf.BltFast(saveInf.btPoint.x, saveInf.btPoint.y, &bgInf.colorBuf, &saveInf.btRect, FALSE);
	sysInf.charFlag = TRUE;	
	sysInf.bgFlag = off;
} // drawBG

void drawBack()
{
	float	ratio;
	RECT	destRect;
	if(saveInf.bWave){
		g_DibInf.colorBuf.WaveBlt(0,0,&backColorBuf,&bgRect,saveInf.waveSize,saveInf.waveCnt);
		return;
	}
	if(0==bgInf.look_cnt || _BAK_TXT_SIN==bgInf.in_type || _BAK_TXT_2TI==bgInf.in_type || _BAK_TXT_RAND==bgInf.in_type){
		BOOL bDraw = FALSE;
		if(saveInf.bFade){
			g_DibInf.colorBuf.BrightBlt(0,0,&backColorBuf,NULL,saveInf.f_r, saveInf.f_g, saveInf.f_b);	
			bDraw = TRUE;
		}else if(saveInf.noiseType){
			if(g_DibInf.colorBuf.NoiseBlt(&backColorBuf))bDraw = TRUE;
		}
		if(FALSE==bDraw){
			backColorBuf.alpha = 1;
			g_DibInf.colorBuf.BltFast(0,0,&backColorBuf,&bgRect,FALSE);
		}
	}else{
		switch(bgInf.in_type){
		  case _BAK_SIN:
		  case _BAK_2TI:
		  case _BAK_ZOOM:
		  case _BAK_ALL_SIN:
		  case _BAK_ALL_2TI:
		  case _BAK_RAND:
		  case _BAK_ALL_RAND:
			g_DibInf.colorBuf.ShakeBlt(&backColorBuf,bgInf.shakeSize,bgInf.shake_direct,timeGetTime()-bgInf.look_cnt,bgInf.look_max);
			break;
		  case _BAK_ROLL:
		  case _BAK_ROLL_SIN:
		  case _BAK_ROLL_2TI:
			{
				ratio = ((int)timeGetTime()-(int)bgInf.look_cnt)/(float)bgInf.look_max;
				if(_BAK_ROLL==bgInf.in_type){	
					ratio = sinf(M_PI/2*ratio);
					g_DibInf.colorBuf.RollBlt(&backColorBuf, bgInf.shakeSize*180.0f*ratio);
				}else{	
					if(_BAK_ROLL_SIN==bgInf.in_type){
						ratio = sinf(M_PI*15*ratio)*bgInf.shakeSize*(1.0f-sinf(M_PI/2*ratio));
					}else{
						ratio = sinf(M_PI*15*ratio)*bgInf.shakeSize;
					}
					g_DibInf.colorBuf.RollBlt(&backColorBuf, ratio);
				}
			}
			break;
		  case _BAK_FADE:
		  case _BAK_CFADE:
			g_DibInf.colorBuf.BltFast2(0,0,&bak_backColorBuf,&backColorBuf,&bgRect,
				bgInf.look_max-(bgInf.look_cnt-timeGetTime()),
				bgInf.look_max);
			break;
		  case _BAK_BFADE:
			ratio = (bgInf.look_max-(bgInf.look_cnt-timeGetTime())) / float(bgInf.look_max);
			ratio = MaxMin( 1.0f, ratio);
			g_DibInf.colorBuf.BrightBlt(0,0,&backColorBuf,NULL,
				int(saveInf.old_r+(saveInf.f_r-saveInf.old_r)*ratio),
				int(saveInf.old_g+(saveInf.f_g-saveInf.old_g)*ratio),
				int(saveInf.old_b+(saveInf.f_b-saveInf.old_b)*ratio) ,TRUE);
			break;
		  case _BAK_CFZOOM1:
			ratio = (bgInf.look_max-(bgInf.look_cnt-timeGetTime())) / float(bgInf.look_max);
			ratio = MaxMin( 1.0f, ratio);
			g_DibInf.colorBuf.BltFast(0,0,&bak_backColorBuf,NULL,FALSE,FALSE,TRUE);
			destRect.left = -int((WIN_SIZEX/4)*(1.0f-ratio));
			destRect.right = WIN_SIZEX+int((WIN_SIZEX/4)*(1.0f-ratio));
			destRect.top = -int((WIN_SIZEY/4)*(1.0f-ratio));
			destRect.bottom = WIN_SIZEY+int((WIN_SIZEY/4)*(1.0f-ratio));
			backColorBuf.alpha = ratio;
			g_DibInf.colorBuf.Blt(&destRect,&backColorBuf,NULL,TRUE,FALSE,TRUE);
			break;
		  case _BAK_CFZOOM2:
			ratio = (bgInf.look_max-(bgInf.look_cnt-timeGetTime())) / float(bgInf.look_max);
			ratio = MaxMin( 1.0f, ratio);
			g_DibInf.colorBuf.BltFast(0,0,&backColorBuf,NULL,FALSE,FALSE,TRUE);
			destRect.left = -int((WIN_SIZEX/4)*ratio);
			destRect.right = WIN_SIZEX+int((WIN_SIZEX/4)*ratio);
			destRect.top = -int((WIN_SIZEY/4)*ratio);
			destRect.bottom = WIN_SIZEY+int((WIN_SIZEY/4)*ratio);
			bak_backColorBuf.alpha =1.0f -ratio;
			g_DibInf.colorBuf.Blt(&destRect,&bak_backColorBuf,NULL,TRUE,FALSE,TRUE);
			break;
		  case _BAK_CFZOOM3:
			ratio = (bgInf.look_max-(bgInf.look_cnt-timeGetTime())) / float(bgInf.look_max);
			ratio = MaxMin( 1.0f, ratio);
			g_DibInf.colorBuf.BltFast(0,0,&backColorBuf,NULL,FALSE,FALSE,TRUE);
			destRect.left = int((WIN_SIZEX/4)*ratio);
			destRect.right = WIN_SIZEX-int((WIN_SIZEX/4)*ratio);
			destRect.top = int((WIN_SIZEY/4)*ratio);
			destRect.bottom = WIN_SIZEY-int((WIN_SIZEY/4)*ratio);
			bak_backColorBuf.alpha =1.0f -ratio;
			bak_backColorBuf.alpha =1.0f -ratio;
			g_DibInf.colorBuf.Blt(&destRect,&bak_backColorBuf,NULL,TRUE,FALSE,TRUE);
			break;
		  case _BAK_CFZOOM4:
			ratio = (bgInf.look_max-(bgInf.look_cnt-timeGetTime())) / float(bgInf.look_max);
			ratio = MaxMin( 1.0f, ratio);
			g_DibInf.colorBuf.BltFast(0,0,&bak_backColorBuf,NULL,FALSE,FALSE,TRUE);
			destRect.left = int((WIN_SIZEX/4)*(1.0f-ratio));
			destRect.right = WIN_SIZEX-int((WIN_SIZEX/4)*(1.0f-ratio));
			destRect.top = int((WIN_SIZEY/4)*(1.0f-ratio));
			destRect.bottom = WIN_SIZEY-int((WIN_SIZEY/4)*(1.0f-ratio));
			backColorBuf.alpha = ratio;
			g_DibInf.colorBuf.Blt(&destRect,&backColorBuf,NULL,TRUE,FALSE,TRUE);
			break;
		  case _BAK_SU:
			ratio = (bgInf.look_max-(bgInf.look_cnt-timeGetTime())) / float(bgInf.look_max);
			ratio = MaxMin( 1.0f, ratio);
			g_DibInf.colorBuf.BltFast(0,int(WIN_SIZEY*ratio),&bak_backColorBuf,NULL,FALSE,FALSE,TRUE);
			g_DibInf.colorBuf.BltFast(0,int(WIN_SIZEY*(ratio-1.0f)),&backColorBuf,NULL,FALSE,FALSE,TRUE);
			break;
		  case _BAK_SD:
			ratio = (bgInf.look_max-(bgInf.look_cnt-timeGetTime())) / float(bgInf.look_max);
			ratio = MaxMin( 1.0f, ratio);
			g_DibInf.colorBuf.BltFast(0,-int(WIN_SIZEY*ratio),&bak_backColorBuf,NULL,FALSE,FALSE,TRUE);
			g_DibInf.colorBuf.BltFast(0,int(WIN_SIZEY*(1.0f-ratio)),&backColorBuf,NULL,FALSE,FALSE,TRUE);
			break;
		  case _BAK_SR:
			ratio = (bgInf.look_max-(bgInf.look_cnt-timeGetTime())) / float(bgInf.look_max);
			ratio = MaxMin( 1.0f, ratio);
			g_DibInf.colorBuf.BltFast(-int(WIN_SIZEX*ratio),0,&bak_backColorBuf,NULL,FALSE,FALSE,TRUE);
			g_DibInf.colorBuf.BltFast(int(WIN_SIZEX*(1.0f-ratio)),0,&backColorBuf,NULL,FALSE,FALSE,TRUE);
			break;
		  case _BAK_SL:
			ratio = (bgInf.look_max-(bgInf.look_cnt-timeGetTime())) / float(bgInf.look_max);
			ratio = MaxMin( 1.0f, ratio);
			g_DibInf.colorBuf.BltFast(int(WIN_SIZEX*ratio),0,&bak_backColorBuf,NULL,FALSE,FALSE,TRUE);
			g_DibInf.colorBuf.BltFast(int(WIN_SIZEX*(ratio-1.0f)),0,&backColorBuf,NULL,FALSE,FALSE,TRUE);
			break;
		  default:
			g_DibInf.colorBuf.PtnFade(0,0,&bak_backColorBuf,&backColorBuf,&bgRect,
				bgInf.look_max-(bgInf.look_cnt-timeGetTime()), bgInf.look_max );
			break;
		}
		if(saveInf.bFade){
			g_DibInf.colorBuf.BrightBlt(0,0,&g_DibInf.colorBuf,NULL,saveInf.f_r, saveInf.f_g, saveInf.f_b);	
		}
	}
	sysInf.charFlag = sysInf.drawFlag = off;
} // drawBack

int CheckCharLocate(int char_no)
{
	char_no ++;
	for(int i=0;i<STCHAR_MAX;i++){
		if(charInf[i].save.char_no==char_no){
			return (charInf[i].save.locate);
		}
	}
	return -1;
} // CheckCharLocate

BOOL waitChar(int char_no)
{
	int		i;
	char_no ++;
	sysInf.drawFlag = on;
	sysInf.charFlag = on;
	sysInf.bgFlag = on;
	for(i=0;i<STCHAR_MAX;i++){
		if(charInf[i].save.char_no==char_no){	
			if(_CHR_FADE==charInf[i].in_type){
				if(charInf[i].look_cnt<=timeGetTime() || G_SKIP){
					charInf[i].look_cnt = 0;
					charInf[i].in_type = _CHR_DIRECT;
					charInf[i].colorBuf.alpha = 1.0f;
					charInf[i].bak_colorBuf.Release();
					return TRUE;
				}
				return FALSE;
			}
		}
	}
	return TRUE;
} // waitChar

BOOL c_disp(int char_no, int pose, int locate, int in_type, int layer,int alpha,int frame,int bright,int offsetX,int offsetY)
{
	int		i;
	BOOL	bNoChangeLayer = FALSE, bCharPosSet = FALSE;

	if(offsetX!=_CHR_DEFAULTPOS || offsetY!=_CHR_DEFAULTPOS) bCharPosSet = TRUE;
	char_no ++;
	sysInf.drawFlag = on;
	sysInf.charFlag = on;
	if(_CHR_FADE==in_type || _CHR_DIRECT==in_type){
		sysInf.bgFlag = on;
	}
	for(i=0;i<STCHAR_MAX;i++){
		if(charInf[i].save.char_no==char_no){	
			bNoChangeLayer = TRUE;
			if(_CHR_FADE==charInf[i].in_type){
				if(charInf[i].look_cnt<=timeGetTime() || G_SKIP){
					charInf[i].look_cnt = 0;
					charInf[i].in_type = _CHR_DIRECT;
					charInf[i].save.alpha = MaxMin(255,alpha);
					charInf[i].colorBuf.alpha = 1.0f;
					charInf[i].bak_colorBuf.Release();
					return TRUE;
				}
				return FALSE;
			}else{
				if(locate==(-1)) locate = charInf[i].save.locate;
				switch(in_type){
				  case _CHR_FADE:
					charInf[i].bak_colorBuf = charInf[i].colorBuf;
					charInf[i].colorBuf.pBuf = NULL;
					goto charLoad;
				  case _CHR_DIRECT: case _CHR_WAIT:
					if(pose==charInf[i].save.pose && locate==charInf[i].save.locate && !bCharPosSet)return TRUE;
					if(pose==charInf[i].save.pose) goto charResetPos;
					charInf[i].colorBuf.Release();
					goto charLoad;
				}
			}
		}
	}
	for(i=0;i<(STCHAR_MAX-1);i++){
		if(0==charInf[i].save.char_no)break;
	}
charLoad:
	if(locate==(-1)) locate = 1;
	char fname[32];
	wsprintf(fname,"c%02d%04d",char_no-1,pose);
	if(FALSE==charInf[i].colorBuf.loadLGF(pack_eventcg,fname,Type_char)){
		pose = (pose /100) *100;
		wsprintf(fname,"c%02d%04d",char_no-1,pose);
		if(FALSE==charInf[i].colorBuf.loadLGF(pack_eventcg,fname,Type_char)){
			charInf[i].Release();
			return TRUE;
		}
	}
	if(bright != 128){
		charInf[i].colorBuf.BrightBlt(0,0,&charInf[i].colorBuf,NULL,bright,bright,bright);
	}
charResetPos:
	charInf[i].colorBuf.offset.x = (WIN_SIZEX -charInf[i].colorBuf.width) /2;
	charInf[i].colorBuf.offset.y = 0;
	if(offsetX==_CHR_DEFAULTPOS){
		switch(locate){
		case _CHR_LL:
			charInf[i].colorBuf.offset.x += -280;
			break;
		case _CHR_RR:
			charInf[i].colorBuf.offset.x += 280;
			break;
		default:
			charInf[i].colorBuf.offset.x += (locate-1)*180;
			break;
		}
	}else{
		charInf[i].colorBuf.offset.x += offsetX;
	}
	if(offsetY!=_CHR_DEFAULTPOS){
		charInf[i].colorBuf.offset.y = (WIN_SIZEY -charInf[i].colorBuf.height) /2;
		charInf[i].colorBuf.offset.y += offsetY;
	}
	charInf[i].save.alpha = MaxMin(255,alpha);
	charInf[i].save.bright = bright;
	charInf[i].save.char_no = char_no;
	charInf[i].save.pose = pose;
	charInf[i].save.locate = locate;
	if(!bNoChangeLayer) charInf[i].save.layer = layer;
	charInf[i].save.bWaitTone = FALSE;
	charInf[i].in_type = in_type;
	switch(in_type){
	  case _CHR_DIRECT: case _CHR_WAIT: case _CHR_WAVE:
		charInf[i].look_cnt = 0;
		return TRUE;
	  default:
		charInf[i].look_max = frame*1000/60;
		charInf[i].look_cnt = timeGetTime() +charInf[i].look_max;
		break;
	}
	return FALSE;
} // c_disp

void c_cls_all()
{
	int		i;
	for(i=0;i<STCHAR_MAX;i++){
		charInf[i].Release();
	}
} // c_cls_all

BOOL c_cls(int char_no, int out_type, int frame)
{
	int		i;
	char_no ++;
	sysInf.drawFlag = on;
	sysInf.bgFlag = on;
	sysInf.charFlag = on;
	for(i=0;i<STCHAR_MAX;i++){
		if(charInf[i].save.char_no != char_no)continue;
		charInf[i].out_type = out_type;
		if(_CHR_WAIT==out_type)	return TRUE;
		if(_CHR_DIRECT==out_type){
			charInf[i].Release();
			return TRUE;
		}
		if(_CHR_FADE==out_type){
			if(0==charInf[i].look_cnt){
				charInf[i].look_max = frame*1000/60;
				charInf[i].look_cnt = timeGetTime() +charInf[i].look_max;
			}
			if(charInf[i].look_cnt<=timeGetTime() || G_SKIP){
				charInf[i].Release();
				return TRUE;
			}
			return FALSE;
		}
	}
	return TRUE;
}

BOOL g_disp(int bak_no,int chg_type,int cg_type,DWORD fade,int x,int y,BOOL bak_clear)
{
	int		i;
	BOOL	bRead;
	char	fname[32];

	sysInf.drawFlag = sysInf.bUseBG = on;
	if((-1)==bak_no){
		sysInf.bgFlag = sysInf.bUseBG = off;
	}
	if(0==bgInf.look_cnt){
		saveInf.bak_no = bak_no;
		saveInf.cg_type = cg_type;
		if( (_BAK_DIRECT)!=chg_type )bgInf.in_type = _BAK_CFADE;
		if(FALSE==bak_clear){
			for(i=0;i<STCHAR_MAX;i++){
				if(_CHR_WAIT==charInf[i].in_type) charInf[i].in_type = _CHR_DIRECT;
				if(_CHR_WAIT==charInf[i].out_type){
					charInf[i].Release();
				}
			}
		}else{
			for(i=0;i<STCHAR_MAX;i++){
				charInf[i].Release();
			}
		}
		saveInf.shakeSize = 0;
		sysInf.bgFlag = on;
		if(bak_no>=0){
			switch(cg_type){
			case 0:
				wsprintf(fname,"B%03d%d%02d", bak_no/100, saveInf.tone_type[0], bak_no%100);
				if(0!=saveInf.tone_type[0]){
					char	out_fname[64];
					if(0==bgInf.colorBuf.SearchFile(pack_eventcg, fname, out_fname, TRUE)){
						myOutputDebugString("%s ‚ðŠî–{Žž”wŒi‚É“Ç‚Ý‘Ö‚¦‚Ü‚·",fname);
						wsprintf(fname,"B%03d%d%02d", bak_no/100, 0, bak_no%100);
					}
				}
				bRead = bgInf.colorBuf.loadLGF(pack_eventcg,fname,Type_back);
				toneConvert.CheckBGTable(fname);
				toneConvert.SetCharTable();
				break;
			case 1:
				wsprintf(fname,"v%04d%02d", bak_no/100, bak_no%100);
				bRead = bgInf.colorBuf.loadLGF(pack_eventcg,fname,Type_event);
				break;
			case 2:
				wsprintf(fname,"h%04d%02d", bak_no/100, bak_no%100);
				bRead = bgInf.colorBuf.loadLGF(pack_eventcg,fname,Type_event);
				break;
			}
			if(FALSE==bRead){
				sysInf.bgFlag = sysInf.bUseBG = off;
				return TRUE;
			}
			if(_BAK_SPREAD==chg_type){
				ColorBuf	tmpCBuf;
				tmpCBuf.createColorBuf(bgInf.colorBuf.width,bgInf.colorBuf.height,bgInf.colorBuf.bpp);
				tmpCBuf.SpreadBlt(&bgInf.colorBuf,6);
				bgInf.colorBuf.BltFast(0,0,&tmpCBuf,NULL,FALSE);
				tmpCBuf.Release();
				chg_type = _BAK_CFADE;
			}
		}
		bgInf.colorBuf.offset.x = 0; bgInf.colorBuf.offset.y = 0;
		saveInf.btRect.left = x; saveInf.btRect.top = y;
		saveInf.btRect.right = bgInf.colorBuf.width;
		saveInf.btRect.bottom = bgInf.colorBuf.height;
		saveInf.btPoint.x = saveInf.btPoint.y = 0;
		int	tmp_width  = saveInf.btRect.right -saveInf.btRect.left;
		int tmp_height = saveInf.btRect.bottom -saveInf.btRect.top;
		if(WIN_SIZEX>tmp_width) saveInf.btPoint.x = (WIN_SIZEX -tmp_width ) /2;
		if(WIN_SIZEY>tmp_height)saveInf.btPoint.y = (WIN_SIZEY -tmp_height) /2;

		if( (_BAK_DIRECT)==chg_type )return TRUE;
		switch(chg_type){
		  case _BAK_CFZOOM1:
		  case _BAK_CFZOOM2:
		  case _BAK_CFZOOM3:
		  case _BAK_CFZOOM4:
		  case _BAK_SU:
		  case _BAK_SD:
		  case _BAK_SR:
		  case _BAK_SL:
			bgInf.in_type = chg_type;
			break;
		}
		if(chg_type>=_BAK_PATTERN0){
			wsprintf( fname, "f00%02d", chg_type&0x7f );
			ptnFadeBuf.loadLGF(pack_gparts,fname);
			bgInf.in_type = chg_type;
		}
		bgInf.look_max = fade*1000/60;
		bgInf.look_cnt = timeGetTime() +bgInf.look_max;
		bak_backColorBuf.BltFast(0,0,&backColorBuf,NULL,FALSE);
	}
	if(bgInf.look_cnt<=timeGetTime() || G_SKIP){
		bgInf.in_type = _BAK_DIRECT;
		bgInf.look_cnt = 0;
		return TRUE;
	}
	sysInf.bShowMsgwin = off;
	sysInf.bHideMsgwin = off;
	return FALSE;
} // g_disp

void createInitColorBuf()
{
	HDC		hDC;
	BITMAPINFOHEADER	bmpInfoHead;
	ZeroMemory(&bmpInfoHead,sizeof(BITMAPINFOHEADER));
	bmpInfoHead.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHead.biWidth = WIN_SIZEX;
	bmpInfoHead.biHeight = -WIN_SIZEY;
	bmpInfoHead.biPlanes = 1;
    bmpInfoHead.biBitCount = 24;
    bmpInfoHead.biCompression = BI_RGB;
	bmpInfoHead.biSizeImage = WIN_SIZEX * WIN_SIZEY *3;
	hDC = GetDC(sysInf.hWnd);
	g_DibInf.memDC = CreateCompatibleDC(hDC);
	g_DibInf.hBmp = CreateDIBSection(hDC,(BITMAPINFO *)&bmpInfoHead,DIB_RGB_COLORS,
					(void **)&g_DibInf.colorBuf.pBuf,NULL,NULL);
	g_DibInf.oldBmp = (HBITMAP)SelectObject(g_DibInf.memDC,g_DibInf.hBmp);
	ReleaseDC(sysInf.hWnd,hDC);
	ZeroMemory(g_DibInf.colorBuf.pBuf, WIN_SIZEX*WIN_SIZEY*3);
	g_DibInf.colorBuf.width = WIN_SIZEX;
	g_DibInf.colorBuf.height = WIN_SIZEY;
	g_DibInf.colorBuf.bpp = 24;
	g_DibInf.colorBuf.wPitch = WIN_SIZEX *3;

	backColorBuf.createColorBuf(WIN_SIZEX,WIN_SIZEY,24);
	bak_backColorBuf.createColorBuf(WIN_SIZEX,WIN_SIZEY,24);
	bgInf.colorBuf.createColorBuf(WIN_SIZEX,WIN_SIZEY,24);
	sysInf.drawFlag = sysInf.bUseBG = sysInf.bgFlag = on;
	saveInf.btRect.left = saveInf.btRect.top = 0;
	saveInf.btRect.right = WIN_SIZEX;
	saveInf.btRect.bottom = WIN_SIZEY;
	saveInf.btPoint.x = saveInf.btPoint.y = 0;
	noisePatternBuf.loadLGF(pack_gparts,"noisePattern");
}


BOOL SetBackFade(int r, int g, int b, int frame)
{
	if(0==bgInf.look_cnt){
		saveInf.old_r = saveInf.f_r;
		saveInf.old_g = saveInf.f_g;
		saveInf.old_b = saveInf.f_b;
		saveInf.f_r = r;
		saveInf.f_g = g;
		saveInf.f_b = b;
		bgInf.look_max = frame*1000/60;
		bgInf.look_cnt = timeGetTime() +bgInf.look_max;
		bgInf.in_type = _BAK_BFADE;
		saveInf.bFade = FALSE;
	}
	if(bgInf.look_cnt<=timeGetTime() || G_SKIP){
		bgInf.look_cnt = 0;
		bgInf.in_type = _BAK_DIRECT;
		saveInf.bFade = TRUE;
		if(r==128 && g==128 && b==128) saveInf.bFade = FALSE;
		return FALSE;
	}
	sysInf.bShowMsgwin = off; 
	sysInf.bHideMsgwin = off;
	return TRUE;


	if(0==bgInf.look_cnt){
		if(bgInf.in_type != _BAK_CFADE){
			bak_backColorBuf.BltFast(bgRect.left,bgRect.top,&g_DibInf.colorBuf,NULL,FALSE); 
			backColorBuf.BrightBlt(0,0,&bak_backColorBuf,NULL,r,g,b);	
		}else{
			backColorBuf.BrightBlt(0,0,&bak_backColorBuf,NULL,r,g,b);	
			bak_backColorBuf.BltFast(bgRect.left,bgRect.top,&g_DibInf.colorBuf,NULL,FALSE); 
		}
		bgInf.look_max = frame*1000/60;
		bgInf.look_cnt = timeGetTime() +bgInf.look_max;
		bgInf.in_type = _BAK_BFADE;
	}
	if(bgInf.look_cnt<=timeGetTime() || G_SKIP){
		bgInf.look_cnt = 0;
		return FALSE;
	}
	sysInf.bShowMsgwin = off; 
	sysInf.bHideMsgwin = off;
	return TRUE;
} // SetBackFade

BOOL SetFlash(int r, int g, int b, int in_frame, int out_frame,BOOL bIn)
{
	if(0==bgInf.look_cnt){
		if(bIn){
			bak_backColorBuf.BltFast(bgRect.left,bgRect.top,&g_DibInf.colorBuf,NULL,FALSE); 
			backColorBuf.BrightBlt(0,0,&bak_backColorBuf,NULL,r,g,b);	

			bgInf.look_max = in_frame*1000/60;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
			bgInf.in_type = _BAK_CFADE;
		}
	}
	if(bgInf.look_cnt<=timeGetTime() || G_SKIP){
		if(FALSE==bIn || G_SKIP){
			if(bIn)	backColorBuf.BltFast(0,0,&bak_backColorBuf,NULL,FALSE);	
			bgInf.look_cnt = 0;
			bgInf.in_type = _BAK_DIRECT;
		}else{
			backColorBuf.BltFast(0,0,&bak_backColorBuf,NULL,FALSE);	
			bak_backColorBuf.BrightBlt(0,0,&backColorBuf,NULL,r,g,b);	
			bgInf.look_max = out_frame*1000/60;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
		}
		return FALSE;
	}
	return TRUE;
}

BOOL SetBackScroll(int x,int y,int frame,int type)
{
	static POINT orgPt;
	static DWORD look_max=0,look_cnt = 0;
	if(0==look_cnt){	
		orgPt.x = saveInf.btRect.left;
		orgPt.y = saveInf.btRect.top;
		look_max = frame*1000/60;
		look_cnt = timeGetTime() +look_max;
		msgWnd.ForceHide();
	}
	sysInf.bgFlag = TRUE;
	if(look_cnt<=timeGetTime() || G_SKIP){
		saveInf.btRect.left = x;	saveInf.btRect.top = y;
		saveInf.btRect.right =  x+WIN_SIZEX;
		saveInf.btRect.bottom = y+WIN_SIZEY;
		look_cnt = 0;
		return FALSE;
	}
	float diff = GetEffectStep(look_cnt,look_max,TRUE);
	saveInf.btRect.left = orgPt.x + int((x -orgPt.x)*diff);
	saveInf.btRect.top  = orgPt.y + int((y -orgPt.y)*diff);
	saveInf.btRect.right = saveInf.btRect.left+WIN_SIZEX;
	saveInf.btRect.bottom = saveInf.btRect.top+WIN_SIZEY;
	return TRUE;
} // set_backScroll

BOOL SetWavEffect(int wave, int frame, int wave_no)
{
	frame = frame *1000 /30;
	if(FALSE==saveInf.bWave){	
		saveInf.bWave = TRUE;
		saveInf.waveSize = wave;
		saveInf.waveCnt = 0;
		if(frame==0)return FALSE;
		saveInf.waveCntMax = frame +timeGetTime();
	}
	if(timeGetTime()>=saveInf.waveCntMax || G_SKIP){
		saveInf.waveSize = wave;
		return FALSE;
	}else{
		saveInf.waveSize = wave *(frame-(saveInf.waveCntMax-timeGetTime())) /frame;
	}
	return TRUE;
} // SetWavEffect


#define Ns   4096
#define Fs     18
int decodeAnime(int read_size,BYTE *readFile, BYTE *fileBuf)
{
	int i, j, k, r, c;
	unsigned int flags;
	int out_size,write_size;
	unsigned char	*rfP,*ofP;
	unsigned char text[Ns+Fs-1];

	out_size  = *(int *)readFile;
	rfP = readFile + 4;
	ofP = (unsigned char *)fileBuf;
	write_size = 0;
	ZeroMemory(text, Ns - Fs);					
	r = Ns - Fs;  flags = 0;
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			c = *(rfP ++); read_size --;
			if (read_size < 0) break;
			flags = c | 0xff00;
		}
		if (flags & 1) {
			c = *(rfP ++); read_size --;
			if (read_size < 0) break;
			*(ofP ++) = c;  write_size ++;
			if(write_size>out_size){
				return 0;
			}
			text[r++] = c;  r &= (Ns - 1);
		} else {
			i = *(rfP ++); read_size --;
			if (read_size < 0) break;
			j = *(rfP ++); read_size --;
			if (read_size < 0) break;
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + 2;
			for (k = 0; k <= j; k++) {
				c = text[(i + k) & (Ns - 1)];
				*(ofP ++) = c; write_size ++;
				if(write_size>out_size){
					return 0;
				}
				text[r++] = c;  r &= (Ns - 1);
			}
		}
	}
	return (out_size);
}

struct AnimePackInfo{
	DWORD	seekOffset;
	DWORD	packSize;
};

#include <Aviriff.h>

void SetBmpInf::DrawAvi()
{
	BOOL	bSkip = FALSE,bLost = FALSE;
	int		ret = FALSE;
	DWORD	nowTime;
	static skipCnt;

	xvidDec.SetSkip(FALSE);
	if(0==animeCnt){
		startTime = timeGetTime();
		skipCnt = 0;
	}
	nowTime = timeGetTime()-startTime;
	if( nowTime < (1000/fps)*animeCnt ){ 
		return;		
	}else if( nowTime > (1000/fps)*(animeCnt +2) ){
		if(skipCnt < 5){
			xvidDec.SetSkip(TRUE);
			bSkip = TRUE;
			skipCnt ++;
		}
	}
	if(FALSE==bSkip)skipCnt = 0;
REDRAW:
	ret = xvidDec.DecodeXviD(readBuf,read_size,&rest,colorBuf.pBuf,animeWidth*3);
	switch(ret){
	  case XviD_DEC_NEEDMORE:
		read_size = readFile->StreamReadFile(pack_anime,avi_stream,(char *)readBuf+rest,buffer_size-rest);
		break;
	  case XviD_DEC_END:
		if(animeLoop){
			xvidDec.SetInit();
			readFile->StreamSeekFile(pack_anime,avi_stream,0,FILE_BEGIN);
			read_size = readFile->StreamReadFile(pack_anime,avi_stream,(char *)readBuf,buffer_size);
			if(animeLoop>0)animeLoop --;
			goto REDRAW;
		}else{
			bAnime = FALSE;
			readFile->StreamCloseFile(pack_anime, avi_stream);
			xvidDec.Clear_XviD();
			cl_free(readBuf);
			return;
		}
		break;
	}
	animeCnt ++;
} // SetBmpInf::DrawAvi


BOOL SetBmpInf::SetAvi(char *fname, int i_layer)
{
	int			size;
	Chunk		aviFileHead;
	Chunk		aviInfo;
	AVIMAINHEADER aviMainHead;

	sysInf.bShowMsgwin = off;
	sysInf.bHideMsgwin = off;
	Release();
	strcpy(filename,fname);
	avi_stream = readFile->StreamOpenFile(pack_anime, filename, size);
	if((-1)==avi_stream) return FALSE;
	readFile->StreamReadFile(pack_anime, avi_stream, (char *)&aviFileHead, sizeof(Chunk));
	if(MAKEFOURCC('R','I','F','F')!=aviFileHead.ckID){
		readFile->StreamCloseFile(pack_anime, avi_stream);
		avi_stream = -1;
		return FALSE;
	}
	readFile->StreamReadFile(pack_anime,avi_stream,(char *)&aviInfo,sizeof(Chunk));
	readFile->StreamReadFile(pack_anime,avi_stream,(char *)&aviMainHead,sizeof(AVIMAINHEADER));
	readFile->StreamSeekFile(pack_anime,avi_stream,0,FILE_BEGIN);
	fps = 1000000.0f/aviMainHead.dwMicroSecPerFrame;

	animeWidth  = aviMainHead.dwWidth;
	animeHeight = aviMainHead.dwHeight;
	colorBuf.createColorBuf(animeWidth, animeHeight, 24, FALSE);

	buffer_size = animeWidth*animeHeight*2;
	readBuf = (LPBYTE)cl_malloc(buffer_size);

	xvidDec.Start_XviD(animeWidth,animeHeight,XVID_CSP_BGR);
	read_size = readFile->StreamReadFile(pack_anime,avi_stream,(char *)readBuf,buffer_size);

	layer = i_layer;
	nuki = -1;
	dstRect.left = 0;
	dstRect.top = 0;
	dstRect.right = dstRect.left +animeWidth;
	dstRect.bottom = dstRect.top +animeHeight;
	srcRect.left = srcRect.top = 0;
	srcRect.right = animeWidth;
	srcRect.bottom = animeHeight;
	bUseBright = FALSE;
	bDraw = TRUE;
	alphaMode = 0;
	bAnime = 2;
	animeCnt = 0;
	animeLoop = 0;
	animeMax = aviMainHead.dwTotalFrames;
	startTime = 0;
	return TRUE;
} // SetBmpInf::SetAvi

BOOL SetBmpInf::WaitAvi()
{
	if(2!=bAnime){
		return TRUE;
	}
	return FALSE;
}

void SetBmpInf::SetGraphParam(int mode, int param)
{
	alphaMode = mode;
	alphaParam = param;
}

void SetBmpInf::LoadBmpAnime( char *fname, int i_layer, int i_nuki, int loop, int skip ,BOOL bReleaseAnime)
{
	YMZAnimeHead	*animeHead;
	AnimePackInfo	*packInfo;
	BYTE			bpp;
	int				i;

	Release();
	strcpy(filename,fname);
	int ret = readFile->ReadPackFile(pack_anime,filename,(char **)&animeHead);
	bpp = animeHead->bpp;
	if(animeHead->bpp==9)bpp = 8;
	colorBuf.createColorBuf(animeHead->width, animeHead->height*animeHead->allFrame, animeHead->bpp, FALSE);
	packInfo = new AnimePackInfo[animeHead->allFrame];
	CopyMemory( packInfo, (char *)animeHead +sizeof(YMZAnimeHead), sizeof(AnimePackInfo)*animeHead->allFrame);

	if(8==bpp){
		if(NULL==colorBuf_Br.anime_pal)colorBuf.anime_pal = new myRGBA[256*animeHead->allFrame];
		CopyMemory( colorBuf.anime_pal, (char *)animeHead +sizeof(YMZAnimeHead)+sizeof(AnimePackInfo)*animeHead->allFrame, sizeof(myRGBA)*256*animeHead->allFrame );
		colorBuf.palColor = colorBuf.anime_pal;
	}
	for(i=0;i<animeHead->allFrame;i++){
		if(0x80000000 & packInfo[i].packSize){
			CopyMemory(
				colorBuf.pBuf+i*animeHead->width*animeHead->height*(bpp/8),
				(BYTE *)animeHead +packInfo[i].seekOffset,
				(packInfo[i].packSize&0x7fffffff) );
		}else{
			decodeAnime(
				packInfo[i].packSize-4,
				(BYTE *)animeHead +packInfo[i].seekOffset,
				colorBuf.pBuf +i*animeHead->width*animeHead->height*(bpp/8) );
		}
	}
	layer = i_layer;
	nuki = i_nuki;
	dstRect.left = animeHead->offsetX;
	dstRect.top = animeHead->offsetY;
	animeWidth = animeHead->width;
	animeHeight = animeHead->height;
	dstRect.right = dstRect.left +animeHead->width;
	dstRect.bottom = dstRect.top +animeHead->height;
	srcRect.left = srcRect.top = 0;
	srcRect.right = animeHead->width;
	srcRect.bottom = animeHead->height;
	bUseBright = FALSE;
	bDraw = TRUE;
	bAnime = 1;
	bAnimePlay = 1;
	bRelease = bReleaseAnime;
	animeCnt = 0;
	animeLoop = loop;
	alphaMode = 0;
	animeMax = animeHead->allFrame;
	if(skip >= 0){
		animeSkip = skip;
	}else{
		animeSkip = animeHead->skipFrame;
	}
	startTime = 0;
	my_deletes(packInfo);
	cl_free(animeHead);
} // SetBmpInf::LoadBmpAnime

BOOL SetBmpInf::WaitBmpAnime()
{
	if(0==bAnimePlay || 1!=bAnime){
		return TRUE;	
	}else if(G_SKIP){
		if(animeCnt<(animeMax-1)){
			animeCnt = animeMax-1;
			startTime = timeGetTime() -animeCnt*(33*(animeSkip+1));
		}
		bAnimePlay = 1;
		animeLoop = 1;
		if(bRelease)bAnimePlay = 2;
	}
	return FALSE;
} // SetBmpInf::WaitBmpAnime

void SetBmpInf::SetBmpAnimePlay(int sp_no, int play, int loop)
{
	if(play==off){
		bAnimePlay = 4;	
	}else{
		bAnimePlay = 1;
		animeCnt = 0;
		startTime = 0;
	}
	animeLoop = loop;
} // SetBmpInf::SetBmpAnimePlay

void SetBmpInf::SetBmpGraph( char *fname, int i_layer, int i_nuki )
{
	char	file[_MAX_PATH];
	Release();	
	strcpy(filename,fname);
	_splitpath(filename,NULL,NULL,file,NULL);
	if(FALSE==colorBuf.loadLGF(pack_eventcg,file,Type_setbmp)){
		return;
	}
	layer = i_layer;
	nuki = i_nuki;
	dstRect.left = colorBuf.offset.x;
	dstRect.top = colorBuf.offset.y;
	dstRect.right = dstRect.left +colorBuf.width;
	dstRect.bottom = dstRect.top +colorBuf.height;
	srcRect.left = srcRect.top = 0;
	srcRect.right = colorBuf.width;
	srcRect.bottom = colorBuf.height;
	animeWidth = colorBuf.width;
	animeHeight = colorBuf.height;
	bUseBright = FALSE;
	bDraw = TRUE;
	animeCnt = 0;
	alphaMode = 0;
} // SetBmpInf::SetBmpGraph

void SetBmpInf::Reload()
{
	char	file[_MAX_PATH];
	YMZAnimeHead	*animeHead;
	AnimePackInfo	*packInfo;
	int				i;

	if(FALSE==bDraw)return;

	ZeroMemory(&xvidDec,sizeof(XviDDec));
	ZeroMemory(&colorBuf,sizeof(ColorBuf));
	ZeroMemory(&colorBuf_Br,sizeof(ColorBuf));
	switch(bAnime){
	  case 0:	
		_splitpath(filename,NULL,NULL,file,NULL);
		colorBuf.loadLGF(pack_eventcg,file,Type_setbmp);
		animeCnt = 0;
		break;
	  case 1:
		readFile->ReadPackFile(pack_anime,filename,(char **)&animeHead);
		colorBuf.createColorBuf(animeHead->width, animeHead->height*animeHead->allFrame, animeHead->bpp, FALSE);
		packInfo = new AnimePackInfo[animeHead->allFrame];
		CopyMemory( packInfo, (char *)animeHead +sizeof(YMZAnimeHead), sizeof(AnimePackInfo)*animeHead->allFrame);

		if(8==animeHead->bpp){
			if(NULL==colorBuf_Br.anime_pal)colorBuf.anime_pal = new myRGBA[256*animeHead->allFrame];
			CopyMemory( colorBuf.anime_pal, (char *)animeHead +sizeof(YMZAnimeHead)+sizeof(AnimePackInfo)*animeHead->allFrame, sizeof(myRGBA)*256*animeHead->allFrame );
			colorBuf.palColor = colorBuf.anime_pal;
		}
		for(i=0;i<animeHead->allFrame;i++){
			if(0x80000000 & packInfo[i].packSize){
				CopyMemory(
					colorBuf.pBuf+i*animeHead->width*animeHead->height*(animeHead->bpp/8),
					(BYTE *)animeHead +packInfo[i].seekOffset,
					(packInfo[i].packSize&0x7fffffff) );
			}else{
				decodeAnime(
					packInfo[i].packSize-4,
					(BYTE *)animeHead +packInfo[i].seekOffset,
					colorBuf.pBuf +i*animeHead->width*animeHead->height*(animeHead->bpp/8) );
			}
		}
		startTime = timeGetTime() - animeCnt* (33*(animeSkip+1));
		my_deletes(packInfo);
		cl_free(animeHead);
		break;
	  case 2:
		{
		int			size;
		Chunk		aviFileHead;
		Chunk		aviInfo;
		AVIMAINHEADER aviMainHead;
		sysInf.bShowMsgwin = off;	
		sysInf.bHideMsgwin = off;
		avi_stream = readFile->StreamOpenFile(pack_anime, filename, size);
		if((-1)==avi_stream) return;
		readFile->StreamReadFile(pack_anime, avi_stream, (char *)&aviFileHead, sizeof(Chunk));
		if(MAKEFOURCC('R','I','F','F')!=aviFileHead.ckID){
			readFile->StreamCloseFile(pack_anime, avi_stream);
			avi_stream = -1;
			return;
		}
		readFile->StreamReadFile(pack_anime,avi_stream,(char *)&aviInfo,sizeof(Chunk));
		readFile->StreamReadFile(pack_anime,avi_stream,(char *)&aviMainHead,sizeof(AVIMAINHEADER));
		readFile->StreamSeekFile(pack_anime,avi_stream,0,FILE_BEGIN);
		fps = 1000000.0f/aviMainHead.dwMicroSecPerFrame;

		animeWidth  = aviMainHead.dwWidth;
		animeHeight = aviMainHead.dwHeight;
		colorBuf.createColorBuf(animeWidth, animeHeight, 24, FALSE);

		buffer_size = animeWidth*animeHeight*2;
		readBuf = (LPBYTE)cl_malloc(buffer_size);

		xvidDec.Start_XviD(animeWidth,animeHeight,XVID_CSP_BGR);
		read_size = readFile->StreamReadFile(pack_anime,avi_stream,(char *)readBuf,buffer_size);
		animeCnt = 0;
		startTime = 0;
		}
		break;
	}
} // SetBmpInf::Reload

void SetBmpInf::Draw(ColorBuf *dstCBuf)
{
	if(FALSE==bDraw)return;
	BOOL bAlpha = TRUE, bAdd = FALSE;
	if(nuki==(-1))bAlpha = FALSE;
	if(0==alphaMode){
		colorBuf.alpha = 1.0f;
	}else{
		colorBuf.alpha = alphaParam /255.0f;
		bAlpha = TRUE;
		if(1==alphaMode)bAdd = TRUE;
	}
	if(FALSE==bUseBright){
		dstCBuf->Blt(&dstRect,&colorBuf,&srcRect,bAlpha,bAdd);
	}else{
		dstCBuf->Blt(&dstRect,&colorBuf_Br,&srcRect,bAlpha,bAdd);
	}
} // SetBmpInf::Draw

void SetBmpInf::SetBmpPos(int dx, int dy, int sx, int sy, int width, int height)
{
	dstRect.left = dx;
	dstRect.top = dy;
	dstRect.right = dstRect.left +width;
	dstRect.bottom = dstRect.top +height;

	srcRect.left = sx;
	srcRect.top = sy;
	srcRect.right = srcRect.left +width;
	srcRect.bottom = srcRect.top +height;
} // SetBmpInf::SetBmpPos

void SetBmpInf::SetBmpMove(int dx, int dy)
{
	int width = dstRect.right -dstRect.left;
	int height = dstRect.bottom -dstRect.top;
	dstRect.left = dx;
	dstRect.top = dy;
	dstRect.right = dstRect.left +width;
	dstRect.bottom = dstRect.top +height;
} // SetBmpInf::SetBmpMove

void SetBmpInf::SetBmpBright(int r, int g, int b)
{
	int		i,x,y;
	myRGB	*pSrc,*pDst;
	BOOL	bR=FALSE,bG=FALSE,bB=FALSE;
	if(!bDraw)return;
	if(!bUseBright){
		bUseBright = TRUE;
		CopyMemory(&colorBuf_Br,&colorBuf,sizeof(ColorBuf));
		colorBuf_Br.palColor = NULL;
		colorBuf_Br.pBuf = NULL;
	}
	if(0x80>=r){
		bR = TRUE;
		if(0x80>r)r *= 2;
		else r=0xff;
	}else{
		r = (r-0x80)*2;
	}
	if(0x80>=g){
		bG = TRUE;
		if(0x80>g)g *= 2;
		else g=0xff;
	}else{
		g = (g-0x80)*2;
	}
	if(0x80>=b){
		bB = TRUE;
		if(0x80>b)b *= 2;
		else b=0xff;
	}else{
		b = (b-0x80)*2;
	}
	switch(colorBuf.bpp){
	  case 8:
		if(NULL==colorBuf_Br.palColor)colorBuf_Br.palColor = new myRGBA[256];
		for(i=0;i<256;i++){
			if(bR)colorBuf_Br.palColor[i].bRed   = AlphaTable[colorBuf.palColor[i].bRed][r];
			else colorBuf_Br.palColor[i].bRed = colorBuf.palColor[i].bRed +(0xff-colorBuf.palColor[i].bRed)*r/0xff;
			if(bG)colorBuf_Br.palColor[i].bGreen = AlphaTable[colorBuf.palColor[i].bGreen][g];
			else colorBuf_Br.palColor[i].bGreen = colorBuf.palColor[i].bGreen +(0xff-colorBuf.palColor[i].bGreen)*g/0xff;
			if(bB)colorBuf_Br.palColor[i].bBlue  = AlphaTable[colorBuf.palColor[i].bBlue][b];
			else colorBuf_Br.palColor[i].bBlue = colorBuf.palColor[i].bBlue +(0xff-colorBuf.palColor[i].bBlue)*b/0xff;
			colorBuf_Br.palColor[i].bAlpha = colorBuf.palColor[i].bAlpha;
		}
		if(NULL==colorBuf_Br.pBuf){
			colorBuf_Br.pBuf = (BYTE *)cl_malloc(colorBuf.wPitch * colorBuf.height);
			CopyMemory(colorBuf_Br.pBuf, colorBuf.pBuf, colorBuf.wPitch * colorBuf.height);
		}
		break;
	  case 24:
		if(NULL==colorBuf_Br.pBuf)colorBuf_Br.pBuf = (BYTE *)cl_malloc(colorBuf.wPitch * colorBuf.height);
		for(y=0;y<colorBuf.height;y++){
			pDst = (myRGB *)(colorBuf_Br.pBuf +colorBuf_Br.wPitch*y);
			pSrc = (myRGB *)(colorBuf.pBuf +colorBuf.wPitch*y);
			for(x=0;x<colorBuf.width;x++){
				if(bR)pDst[x].bRed   = AlphaTable[pSrc[x].bRed][r];
				else pDst[x].bRed = pSrc[x].bRed +(0xff-pSrc[x].bRed)*r/0xff;
				if(bG)pDst[x].bGreen = AlphaTable[pSrc[x].bGreen][g];
				else pDst[x].bGreen = pSrc[x].bGreen +(0xff-pSrc[x].bGreen)*g/0xff;
				if(bB)pDst[x].bBlue  = AlphaTable[pSrc[x].bBlue][b];
				else pDst[x].bBlue = pSrc[x].bBlue +(0xff-pSrc[x].bBlue)*b/0xff;
			}
		}
		break;
	}
}

void SetBmpInf::ResetSetBmpZoom()
{
	if(0==zoomRect.right || 0==zoomRect.bottom){
		srcRect.top = animeCnt *animeHeight;
		srcRect.bottom = srcRect.top +animeHeight;
		srcRect.left = 0; srcRect.right = animeWidth;
		return;
	}
	SetBmpZoom(zoomRect.left, zoomRect.top, zoomRect.right-zoomRect.left, zoomRect.bottom-zoomRect.top);
}

void SetBmpInf::SetBmpZoom(int x, int y, int width, int height)
{
	zoomRect.left = x;	zoomRect.right = x +width;
	zoomRect.top = y;	zoomRect.bottom = y +height;

	dstRect.left = x;
	dstRect.right = dstRect.left +width;
	dstRect.top = y;
	dstRect.bottom = dstRect.top +height;

	switch(bAnime){
	case 0:
		srcRect.top = 0;
		srcRect.bottom = animeHeight;
		srcRect.left = 0;
		srcRect.right = animeWidth;
		break;
	default:
		srcRect.top = animeCnt *animeHeight;
		srcRect.bottom = srcRect.top +animeHeight;
		srcRect.left = 0; srcRect.right = animeWidth;
		break;
	}
	if(y<0){
		srcRect.top += (-y)*animeHeight/height;
		dstRect.top = 0;
	}
	if((y+height)>WIN_SIZEY){
		srcRect.bottom = animeCnt *animeHeight +animeHeight*(WIN_SIZEY-y)/height;
		dstRect.bottom = WIN_SIZEY;
	}
	if(x<0){
		srcRect.left = (-x)*animeWidth/width;
		dstRect.left = 0;
	}
	if((x+width)>WIN_SIZEX){
		srcRect.right = animeWidth*(WIN_SIZEX-x)/width;
		dstRect.right = WIN_SIZEX;
	}
} // SetBmpInf::SetBmpZoom

void SetBmpInf::SetBmpZoom2(int x, int y, int ratio)
{
	int width = animeWidth *ratio /4096;
	int height = animeHeight *ratio /4096;

	int offsetX = x -(x*width/WIN_SIZEX);
	int offsetY = y -(y*height/WIN_SIZEY);
	SetBmpZoom(offsetX,offsetY,width,height);

} // SetBmpInf::SetBmpZoom2

void SetBmpInf::AnimePlay()
{
	if(0==startTime){
		startTime = timeGetTime();
		animeCnt = 0;
	}else if(1 == bAnimePlay){	
		animeCnt = (timeGetTime() -startTime) / (33*(animeSkip+1));
		if(animeCnt > (animeMax-1)){
			animeCnt = animeMax-1;
			if(animeLoop==0 || animeLoop==(-1)){	
				animeLoop = 0;
				bAnimePlay = 3;
			}else if(animeLoop > 0){
				animeLoop --;
				if(animeLoop == 0){
					bAnimePlay = 0;
					if(bRelease)bAnimePlay = 2;
					animeLoop = -2; 
				}else{
					bAnimePlay = 3; 
				}
			}
		}
	}
	if(colorBuf.anime_pal){
		colorBuf.palColor = &colorBuf.anime_pal[256*animeCnt];
	}
	ResetSetBmpZoom();
} // SetBmpInf::AnimePlay

void SetBmpDraw()
{
	int		i,j;
	BOOL	bDraw = FALSE;

	for(i=0;i<STBMP_MAX;i++){
		if(i==1)drawBG();
		if(i>=3 && i<=7){
			drawChar( i-3 );
		}
		for(j=0;j<STBMP_MAX;j++){
			if(setBmpInf[j].bDraw==FALSE || setBmpInf[j].layer!=i)continue;
			switch(setBmpInf[j].bAnime){
			 case 0: 
				setBmpInf[j].Draw(&backColorBuf);
				break;
			 case 1:
				setBmpInf[j].AnimePlay();
				setBmpInf[j].Draw(&backColorBuf);
				if(3==setBmpInf[j].bAnimePlay){
					setBmpInf[j].startTime = 0;
					setBmpInf[j].bAnimePlay = 1;
				}
				if(2==setBmpInf[j].bAnimePlay){
					setBmpInf[j].Release();
				}
				break;
			 case 2:
				setBmpInf[j].DrawAvi();
				setBmpInf[j].Draw(&backColorBuf);
				break;
			}
			sysInf.charFlag = TRUE;
			sysInf.bgFlag = on;
		}
	}
} // SetBmpDraw

void CopyEqualColor(
	BYTE *pDest,int destP_W,
	BYTE *pSrc, int srcP_W,
	int width,int height,int bpp)
{
	int		y;

	for(y=0;y<height;y++){
		CopyMemory(pDest, pSrc, width*(bpp>>3));
		pDest += destP_W;
		pSrc  += srcP_W;
	}
} // CopyEqualColor

void Copy32to24(
	myRGB *pDest, int destP_W,
	myRGBA *pSrc, int srcP_W,
	int width,int height, BOOL bFast)
{
	int		x,y;
	int start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			if(pSrc[x].bAlpha==0xff){
				pDest[x].bRed   = pSrc[x].bRed;
				pDest[x].bGreen = pSrc[x].bGreen;
				pDest[x].bBlue  = pSrc[x].bBlue;
			}else if(pSrc[x].bAlpha>0){
				pDest[x].bRed = AlphaTable[pSrc[x].bRed][pSrc[x].bAlpha]
					+AlphaTable[pDest[x].bRed][0xFF-pSrc[x].bAlpha];
				pDest[x].bGreen = AlphaTable[pSrc[x].bGreen][pSrc[x].bAlpha]
					+AlphaTable[pDest[x].bGreen][0xFF-pSrc[x].bAlpha];
				pDest[x].bBlue = AlphaTable[pSrc[x].bBlue][pSrc[x].bAlpha]
					+AlphaTable[pDest[x].bBlue][0xFF-pSrc[x].bAlpha];
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
		pSrc = (myRGBA *)((BYTE *)pSrc +srcP_W);
	}
} // Copy32to24

void Copy24to32(
	myRGBA *pDest, int destP_W,
	myRGB *pSrc, int srcP_W,
	int width,int height, BOOL bFast)
{
	int		x,y;
	int	start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			pDest[x].bRed   = pSrc[x].bRed;
			pDest[x].bGreen = pSrc[x].bGreen;
			pDest[x].bBlue  = pSrc[x].bBlue;
			pDest[x].bAlpha = 0xff;
		}
		pDest = (myRGBA *)((BYTE *)pDest +destP_W);
		pSrc =  (myRGB *)((BYTE *)pSrc +srcP_W);
	}
} // Copy24to32

void Copy32to24A(
	myRGB *pDest, int destP_W,
	myRGBA *pSrc, int srcP_W,
	int width,int height, float alpha, BOOL bFast)
{
	int		x,y;
	int		start = 0, step = 1;
	BYTE	a;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			a = BYTE(pSrc[x].bAlpha*alpha);
			if(a==0xff){
				pDest[x].bRed   = pSrc[x].bRed;
				pDest[x].bGreen = pSrc[x].bGreen;
				pDest[x].bBlue  = pSrc[x].bBlue;
			}else if(a>0){
				pDest[x].bRed = AlphaTable[pSrc[x].bRed][a]
					+AlphaTable[pDest[x].bRed][0xFF-a];
				pDest[x].bGreen = AlphaTable[pSrc[x].bGreen][a]
					+AlphaTable[pDest[x].bGreen][0xFF-a];
				pDest[x].bBlue = AlphaTable[pSrc[x].bBlue][a]
					+AlphaTable[pDest[x].bBlue][0xFF-a];
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
		pSrc = (myRGBA *)((BYTE *)pSrc +srcP_W);
	}
} // Copy32to24A

void Copy32to32A(
	myRGBA *pDest, int destP_W,
	myRGBA *pSrc, int srcP_W,
	int width,int height, float alpha, BOOL bFast)
{
	int		x,y;
	int		start = 0, step = 1;
	BYTE	a;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			a = BYTE(pSrc[x].bAlpha*alpha);
			if(a==0xff){
				pDest[x].bRed   = pSrc[x].bRed;
				pDest[x].bGreen = pSrc[x].bGreen;
				pDest[x].bBlue  = pSrc[x].bBlue;
			}else if(a>0){
				pDest[x].bRed = AlphaTable[pSrc[x].bRed][a]
					+AlphaTable[pDest[x].bRed][0xFF-a];
				pDest[x].bGreen = AlphaTable[pSrc[x].bGreen][a]
					+AlphaTable[pDest[x].bGreen][0xFF-a];
				pDest[x].bBlue = AlphaTable[pSrc[x].bBlue][a]
					+AlphaTable[pDest[x].bBlue][0xFF-a];
			}
		}
		pDest = (myRGBA *)((BYTE *)pDest +destP_W);
		pSrc = (myRGBA *)((BYTE *)pSrc +srcP_W);
	}
} // Copy32to24A

void Copy32_2to24A(
	ColorBuf *destBuf,
	ColorBuf *srcBuf1, int offsetX1, int offsetY1,	
	ColorBuf *srcBuf2, int offsetX2, int offsetY2,
	float alpha, BOOL bFast)
{
	RECT	allRect,srcRect1,srcRect2;
	BOOL	bSrc1,bSrc2;
	int		x,y;
	BYTE	aS1,aS2,aD;
	myRGBA	*pSrc1,*pSrc2;
	myRGB	*pDest;

	srcRect1.left = offsetX1; srcRect1.top = offsetY1;
	srcRect1.right = offsetX1 +srcBuf1->width; srcRect1.bottom = offsetY1 +srcBuf1->height;
	if(srcRect1.right > destBuf->width) srcRect1.right  = destBuf->width;
	if(srcRect1.bottom> destBuf->height)srcRect1.bottom = destBuf->height;
	srcRect2.left = offsetX2; srcRect2.top = offsetY2;
	srcRect2.right = offsetX2 +srcBuf2->width; srcRect2.bottom = offsetY2 +srcBuf2->height;
	if(srcRect2.right > destBuf->width) srcRect2.right  = destBuf->width;
	if(srcRect2.bottom> destBuf->height)srcRect2.bottom = destBuf->height;

	allRect.left = offsetX1; allRect.top = offsetY1;
	if(allRect.left>offsetX2)allRect.left = offsetX2;
	if(allRect.top >offsetY2)allRect.top = offsetY2;
	allRect.right = srcRect1.right;
	allRect.bottom = srcRect1.bottom;
	if(allRect.right <srcRect2.right) allRect.right  = srcRect2.right;
	if(allRect.bottom<srcRect2.bottom)allRect.bottom = srcRect2.bottom;
	if(allRect.left < 0){
		allRect.left = 0;
	}
	int width  = allRect.right -allRect.left;
	int height = allRect.bottom -allRect.top;
	pDest = (myRGB *)(destBuf->pBuf +(destBuf->wPitch*allRect.top)) +allRect.left;
	pSrc1 = (myRGBA *)(srcBuf1->pBuf +(srcBuf1->wPitch*(allRect.top-srcRect1.top))) +(allRect.left-srcRect1.left);
	pSrc2 = (myRGBA *)(srcBuf2->pBuf +(srcBuf2->wPitch*(allRect.top-srcRect2.top))) +(allRect.left-srcRect2.left);
	int start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			bSrc1 = bSrc2 = FALSE;
			point.x = x +allRect.left;
			point.y = y +allRect.top;
			if(myPtInRect(&srcRect1,&point))bSrc1 = TRUE;
			if(myPtInRect(&srcRect2,&point))bSrc2 = TRUE;
			if(bSrc1 && bSrc2){
				aS1 = BYTE(pSrc1[x].bAlpha*alpha);
				aS2 = BYTE(pSrc2[x].bAlpha*(1-alpha));
				aD = 0xFF -(aS1+aS2);
				pDest[x].bRed = 
					AlphaTable[pDest[x].bRed][aD] +
					AlphaTable[pSrc1[x].bRed][aS1]+
					AlphaTable[pSrc2[x].bRed][aS2];
				pDest[x].bGreen = 
					AlphaTable[pDest[x].bGreen][aD] +
					AlphaTable[pSrc1[x].bGreen][aS1]+
					AlphaTable[pSrc2[x].bGreen][aS2];
				pDest[x].bBlue = 
					AlphaTable[pDest[x].bBlue][aD] +
					AlphaTable[pSrc1[x].bBlue][aS1]+
					AlphaTable[pSrc2[x].bBlue][aS2];
			}else if(bSrc1){
				aS1 = BYTE(pSrc1[x].bAlpha*alpha);
				aD = 0xFF -aS1;
				pDest[x].bRed = 
					AlphaTable[pDest[x].bRed][aD] +
					AlphaTable[pSrc1[x].bRed][aS1];
				pDest[x].bGreen = 
					AlphaTable[pDest[x].bGreen][aD] +
					AlphaTable[pSrc1[x].bGreen][aS1];
				pDest[x].bBlue = 
					AlphaTable[pDest[x].bBlue][aD] +
					AlphaTable[pSrc1[x].bBlue][aS1];
			}else if(bSrc2){
				aS2 = BYTE(pSrc2[x].bAlpha*(1-alpha));
				aD = 0xFF -aS2;
				pDest[x].bRed = 
					AlphaTable[pDest[x].bRed][aD] +
					AlphaTable[pSrc2[x].bRed][aS2];
				pDest[x].bGreen = 
					AlphaTable[pDest[x].bGreen][aD] +
					AlphaTable[pSrc2[x].bGreen][aS2];
				pDest[x].bBlue = 
					AlphaTable[pDest[x].bBlue][aD] +
					AlphaTable[pSrc2[x].bBlue][aS2];
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destBuf->wPitch);
		pSrc1 = (myRGBA *)((BYTE *)pSrc1 +srcBuf1->wPitch);
		pSrc2 = (myRGBA *)((BYTE *)pSrc2 +srcBuf2->wPitch);
	}
} // Copy32_2to24A

void Copy8to24(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height, BOOL bFast)
{
	int		x,y;
	int		start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
				pDest[x].bRed   = palColor[pSrc[x]].bRed;
				pDest[x].bGreen = palColor[pSrc[x]].bGreen;
				pDest[x].bBlue  = palColor[pSrc[x]].bBlue;
		}
		pDest = (myRGB *)( (BYTE *)pDest +destP_W );
		pSrc += srcP_W;
	}
}

void Copy8to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha, BOOL bFast)
{
	int		x,y;
	int		start = 0, step = 1;
	BYTE	as = BYTE(alpha*0xFF);
	BYTE	ad = 0xFF -as;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
				pDest[x].bRed   = AlphaTable[palColor[pSrc[x]].bRed][as]   +AlphaTable[pDest[x].bRed][ad];
				pDest[x].bGreen = AlphaTable[palColor[pSrc[x]].bGreen][as] +AlphaTable[pDest[x].bGreen][ad];
				pDest[x].bBlue  = AlphaTable[palColor[pSrc[x]].bBlue][as]  +AlphaTable[pDest[x].bBlue][ad];
		}
		pDest = (myRGB *)( (BYTE *)pDest +destP_W );
		pSrc += srcP_W;
	}
} // Copy8to24A

void Copy8to32A(myRGBA *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha, BOOL bFast)
{
	int		x,y;
	int		start = 0, step = 1;
	BYTE	as = BYTE(alpha*0xFF);
	BYTE	ad = 0xFF -as;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
				pDest[x].bRed   = AlphaTable[palColor[pSrc[x]].bRed][as]   +AlphaTable[pDest[x].bRed][ad];
				pDest[x].bGreen = AlphaTable[palColor[pSrc[x]].bGreen][as] +AlphaTable[pDest[x].bGreen][ad];
				pDest[x].bBlue  = AlphaTable[palColor[pSrc[x]].bBlue][as]  +AlphaTable[pDest[x].bBlue][ad];
		}
		pDest = (myRGBA *)( (BYTE *)pDest +destP_W );
		pSrc += srcP_W;
	}
}

void Copy9to24(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height, BOOL bFast)
{
	int		x,y;
	int		start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			if(0==palColor[pSrc[x]].bAlpha)continue;
			pDest[x].bRed   = AlphaTable[palColor[pSrc[x]].bRed][palColor[pSrc[x]].bAlpha]
					+AlphaTable[pDest[x].bRed][0xFF-palColor[pSrc[x]].bAlpha];
			pDest[x].bGreen = AlphaTable[palColor[pSrc[x]].bGreen][palColor[pSrc[x]].bAlpha]
					+AlphaTable[pDest[x].bGreen][0xFF-palColor[pSrc[x]].bAlpha];
			pDest[x].bBlue  = AlphaTable[palColor[pSrc[x]].bBlue][palColor[pSrc[x]].bAlpha]
					+AlphaTable[pDest[x].bBlue][0xFF-palColor[pSrc[x]].bAlpha];
		}
		pDest = (myRGB *)( (BYTE *)pDest +destP_W );
		pSrc += srcP_W;
	}
} // Copy9to24

void Copy9to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha, BOOL bFast)
{
	int		x,y;
	int		start = 0, step = 1;
	BYTE	as,ad;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			as = BYTE(palColor[pSrc[x]].bAlpha*alpha);
			ad = 0xFF -as;
			if(0==as)continue;
			pDest[x].bRed   = AlphaTable[palColor[pSrc[x]].bRed][as]   +AlphaTable[pDest[x].bRed][ad];
			pDest[x].bGreen = AlphaTable[palColor[pSrc[x]].bGreen][as] +AlphaTable[pDest[x].bGreen][ad];
			pDest[x].bBlue  = AlphaTable[palColor[pSrc[x]].bBlue][as]  +AlphaTable[pDest[x].bBlue][ad];
		}
		pDest = (myRGB *)( (BYTE *)pDest +destP_W );
		pSrc += srcP_W;
	}
} // Copy9to24A

void Copy24to24A(
	myRGB *pDest, int destP_W,	
	myRGB *pSrc1, int src1P_W,	
	myRGB *pSrc2, int src2P_W,	
	int width,int height,float alpha, BOOL bFast)
{
	BYTE a = BYTE(alpha*0xFF);
	int		x,y;
	int		start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			if(a==0xff){
				pDest[x].bRed   = pSrc2[x].bRed;
				pDest[x].bGreen = pSrc2[x].bGreen;
				pDest[x].bBlue  = pSrc2[x].bBlue;
			}else if(a==0){
				pDest[x].bRed   = pSrc1[x].bRed;
				pDest[x].bGreen = pSrc1[x].bGreen;
				pDest[x].bBlue  = pSrc1[x].bBlue;
			}else{
				pDest[x].bRed = AlphaTable[pSrc2[x].bRed][a]
					+AlphaTable[pSrc1[x].bRed][0xFF-a];
				pDest[x].bGreen = AlphaTable[pSrc2[x].bGreen][a]
					+AlphaTable[pSrc1[x].bGreen][0xFF-a];
				pDest[x].bBlue = AlphaTable[pSrc2[x].bBlue][a]
					+AlphaTable[pSrc1[x].bBlue][0xFF-a];
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
		pSrc1 = (myRGB *)((BYTE *)pSrc1 +src1P_W);
		pSrc2 = (myRGB *)((BYTE *)pSrc2 +src2P_W);
	}
}

void Copy24to24A(
	myRGB *pDest, int destP_W,	
	myRGB *pSrc, int srcP_W,	
	int width,int height,float alpha, BOOL bFast)
{
	BYTE a = BYTE(alpha*0xFF);
	int		x,y;
	int		start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			if(a==0xff){
				pDest[x].bRed   = pSrc[x].bRed;
				pDest[x].bGreen = pSrc[x].bGreen;
				pDest[x].bBlue  = pSrc[x].bBlue;
			}else if(a>0){
				pDest[x].bRed = AlphaTable[pSrc[x].bRed][a]
					+AlphaTable[pDest[x].bRed][0xFF-a];
				pDest[x].bGreen = AlphaTable[pSrc[x].bGreen][a]
					+AlphaTable[pDest[x].bGreen][0xFF-a];
				pDest[x].bBlue = AlphaTable[pSrc[x].bBlue][a]
					+AlphaTable[pDest[x].bBlue][0xFF-a];
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
		pSrc  = (myRGB *)((BYTE *)pSrc  +srcP_W);
	}
} // Copy24to24A

void Copy24to32A(
	myRGBA *pDest, int destP_W,	
	myRGB *pSrc, int srcP_W,	
	int width,int height,float alpha, BOOL bFast)
{
	BYTE a = BYTE(alpha*0xFF);
	int		x,y;
	int		start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			if(a==0xff){
				pDest[x].bRed   = pSrc[x].bRed;
				pDest[x].bGreen = pSrc[x].bGreen;
				pDest[x].bBlue  = pSrc[x].bBlue;
			}else if(a>0){
				pDest[x].bRed = AlphaTable[pSrc[x].bRed][a]
					+AlphaTable[pDest[x].bRed][0xFF-a];
				pDest[x].bGreen = AlphaTable[pSrc[x].bGreen][a]
					+AlphaTable[pDest[x].bGreen][0xFF-a];
				pDest[x].bBlue = AlphaTable[pSrc[x].bBlue][a]
					+AlphaTable[pDest[x].bBlue][0xFF-a];
			}
		}
		pDest = (myRGBA *)((BYTE *)pDest +destP_W);
		pSrc  = (myRGB *)((BYTE *)pSrc  +srcP_W);
	}
} // Copy24to32A

void StretchCopy8to24(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	BYTE	*pSrcPix;
	int		start = 0, step = 1;

	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = pSrc +srcP_W*ys;
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			pDest[x].bRed   = srcPal[pSrcPix[xs]].bRed;
			pDest[x].bGreen = srcPal[pSrcPix[xs]].bGreen;
			pDest[x].bBlue  = srcPal[pSrcPix[xs]].bBlue;
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchCopy8to24

void StretchCopy8to24A(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	BYTE	as = BYTE(0xFF *alpha);
	BYTE	ad = 0xFF -as;
	BYTE	*pSrcPix;
	int		start = 0, step = 1;

	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = pSrc +srcP_W*ys;
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			pDest[x].bRed   = AlphaTable[srcPal[pSrcPix[xs]].bRed][as] +AlphaTable[pDest[x].bRed][ad];
			pDest[x].bGreen = AlphaTable[srcPal[pSrcPix[xs]].bGreen][as] +AlphaTable[pDest[x].bGreen][ad];
			pDest[x].bBlue  = AlphaTable[srcPal[pSrcPix[xs]].bBlue][as] +AlphaTable[pDest[x].bBlue][ad];
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchCopy8to24A

void StretchCopy9to24(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	BYTE	*pSrcPix;
	int		start = 0, step = 1;

	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = pSrc +srcP_W*ys;
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			pDest[x].bRed   = AlphaTable[srcPal[pSrcPix[xs]].bRed][srcPal[pSrcPix[xs]].bAlpha]
					+AlphaTable[pDest[x].bRed][0xFF-srcPal[pSrcPix[xs]].bAlpha];
			pDest[x].bGreen = AlphaTable[srcPal[pSrcPix[xs]].bGreen][srcPal[pSrcPix[xs]].bAlpha]
					+AlphaTable[pDest[x].bGreen][0xFF-srcPal[pSrcPix[xs]].bAlpha];
			pDest[x].bBlue  = AlphaTable[srcPal[pSrcPix[xs]].bBlue][srcPal[pSrcPix[xs]].bAlpha]
					+AlphaTable[pDest[x].bBlue][0xFF-srcPal[pSrcPix[xs]].bAlpha];
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchCopy9to24

void StretchCopy9to24A(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,float alpha,BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	BYTE	*pSrcPix;
	int		start = 0, step = 1;

	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = pSrc +srcP_W*ys;
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			BYTE	as = BYTE(srcPal[pSrcPix[xs]].bAlpha *alpha);
			BYTE	ad = 0xFF -as;
			if(0==as)continue;
			pDest[x].bRed   = AlphaTable[srcPal[pSrcPix[xs]].bRed][as]  +AlphaTable[pDest[x].bRed][ad];
			pDest[x].bGreen = AlphaTable[srcPal[pSrcPix[xs]].bGreen][as]+AlphaTable[pDest[x].bGreen][ad];
			pDest[x].bBlue  = AlphaTable[srcPal[pSrcPix[xs]].bBlue][as] +AlphaTable[pDest[x].bBlue][ad];
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchCopy9to24A

void StretchCopy24to24(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	myRGB *pSrc, int srcP_W,  int widthSrc, int heightSrc, BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	myRGB	*pSrcPix;

	int		start = 0, step = 1;
	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = (myRGB *)((BYTE *)pSrc +srcP_W*ys);
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			pDest[x].bRed   = pSrcPix[xs].bRed;
			pDest[x].bGreen = pSrcPix[xs].bGreen;
			pDest[x].bBlue  = pSrcPix[xs].bBlue;
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchCopy24to24

void StretchCopy24to24A(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	myRGB *pSrc, int srcP_W,  int widthSrc, int heightSrc,float alpha, BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	myRGB	*pSrcPix;
	BYTE a = BYTE(alpha*0xFF);

	if(a==0)return;
	int		start = 0, step = 1;
	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = (myRGB *)((BYTE *)pSrc +srcP_W*ys);
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			if(a==0xff){
				pDest[x].bRed   = pSrcPix[xs].bRed;
				pDest[x].bGreen = pSrcPix[xs].bGreen;
				pDest[x].bBlue  = pSrcPix[xs].bBlue;
			}else{
				pDest[x].bRed = AlphaTable[pSrcPix[xs].bRed][a]
						+AlphaTable[pDest[x].bRed][0xFF-a];
				pDest[x].bGreen = AlphaTable[pSrcPix[xs].bGreen][a]
						+AlphaTable[pDest[x].bGreen][0xFF-a];
				pDest[x].bBlue = AlphaTable[pSrcPix[xs].bBlue][a]
						+AlphaTable[pDest[x].bBlue][0xFF-a];
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchCopy24to24A

void StretchCopy32to24(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	myRGBA *pSrc, int srcP_W,  int widthSrc, int heightSrc, BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	myRGBA	*pSrcPix;
	BYTE	a;

	int		start = 0, step = 1;
	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = (myRGBA *)((BYTE *)pSrc +srcP_W*ys);
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			a = pSrcPix[xs].bAlpha;
			if(0xff==a){
				pDest[x].bRed   = pSrcPix[xs].bRed;
				pDest[x].bGreen = pSrcPix[xs].bGreen;
				pDest[x].bBlue  = pSrcPix[xs].bBlue;
			}else if(a > 0){
				pDest[x].bRed = AlphaTable[pSrcPix[xs].bRed][a]
						+AlphaTable[pDest[x].bRed][0xFF-a];
				pDest[x].bGreen = AlphaTable[pSrcPix[xs].bGreen][a]
						+AlphaTable[pDest[x].bGreen][0xFF-a];
				pDest[x].bBlue = AlphaTable[pSrcPix[xs].bBlue][a]
						+AlphaTable[pDest[x].bBlue][0xFF-a];
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchCopy32to24

void StretchCopy32to24A(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	myRGBA *pSrc, int srcP_W,  int widthSrc, int heightSrc,float alpha, BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	myRGBA	*pSrcPix;
	BYTE	a;

	int		start = 0, step = 1;
	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = (myRGBA *)((BYTE *)pSrc +srcP_W*ys);
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			a = BYTE(pSrcPix[xs].bAlpha*alpha);
			if(0xff==a){
				pDest[x].bRed   = pSrcPix[xs].bRed;
				pDest[x].bGreen = pSrcPix[xs].bGreen;
				pDest[x].bBlue  = pSrcPix[xs].bBlue;
			}else if(a > 0){
				pDest[x].bRed = AlphaTable[pSrcPix[xs].bRed][a]
						+AlphaTable[pDest[x].bRed][0xFF-a];
				pDest[x].bGreen = AlphaTable[pSrcPix[xs].bGreen][a]
						+AlphaTable[pDest[x].bGreen][0xFF-a];
				pDest[x].bBlue = AlphaTable[pSrcPix[xs].bBlue][a]
						+AlphaTable[pDest[x].bBlue][0xFF-a];
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchCopy32to24A

void Add8to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha, BOOL bFast)
{
	int		x,y;
	BYTE	as = BYTE(alpha*0xFF);
	myRGB	tmpSrc;

	if(as==0)return;
	int		start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			if(0xff==as){
				tmpSrc.bRed   = palColor[pSrc[x]].bRed;
				tmpSrc.bGreen = palColor[pSrc[x]].bGreen;
				tmpSrc.bBlue  = palColor[pSrc[x]].bBlue;
			}else{
				tmpSrc.bRed   = AlphaTable[palColor[pSrc[x]].bRed][as];
				tmpSrc.bGreen = AlphaTable[palColor[pSrc[x]].bGreen][as];
				tmpSrc.bBlue  = AlphaTable[palColor[pSrc[x]].bBlue][as];
			}
			AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&tmpSrc );
		}
		pDest = (myRGB *)( (BYTE *)pDest +destP_W );
		pSrc += srcP_W;
	}
} // Add8to24A

void Add9to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha, BOOL bFast)
{
	int		x,y;
	BYTE	as;
	myRGB	tmpSrc;

	if(alpha==0)return;
	int		start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			as = BYTE(alpha*palColor[pSrc[x]].bAlpha);
			if(0xff==as){
				tmpSrc.bRed   = palColor[pSrc[x]].bRed;
				tmpSrc.bGreen = palColor[pSrc[x]].bGreen;
				tmpSrc.bBlue  = palColor[pSrc[x]].bBlue;
			}else{
				tmpSrc.bRed   = AlphaTable[palColor[pSrc[x]].bRed][as];
				tmpSrc.bGreen = AlphaTable[palColor[pSrc[x]].bGreen][as];
				tmpSrc.bBlue  = AlphaTable[palColor[pSrc[x]].bBlue][as];
			}
			AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&tmpSrc );
		}
		pDest = (myRGB *)( (BYTE *)pDest +destP_W );
		pSrc += srcP_W;
	}
} // Add9to24A

void Add32to24A(
	myRGB *pDest, int destP_W,
	myRGBA *pSrc, int srcP_W,
	int width,int height, float alpha, BOOL bFast)
{
	int		x,y;
	BYTE	a;
	myRGB	tmpSrc;

	int		start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			a = BYTE(pSrc[x].bAlpha*alpha);
			if(0==a)continue;
			if(a==0xff){
				tmpSrc.bRed   = pSrc[x].bRed;
				tmpSrc.bGreen = pSrc[x].bGreen;
				tmpSrc.bBlue  = pSrc[x].bBlue;
			}else{
				tmpSrc.bRed   = AlphaTable[pSrc[x].bRed][a];
				tmpSrc.bGreen = AlphaTable[pSrc[x].bGreen][a];
				tmpSrc.bBlue  = AlphaTable[pSrc[x].bBlue][a];
			}
			AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&tmpSrc );
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
		pSrc = (myRGBA *)((BYTE *)pSrc +srcP_W);
	}
} // Add32to24A

void Add24to24A(
	myRGB *pDest, int destP_W,	
	myRGB *pSrc, int srcP_W,	
	int width,int height,float alpha, BOOL bFast)
{
	BYTE a = BYTE(alpha*0xFF);
	myRGB	tmpSrc;
	int		x,y;

	if(0==a)return;

	int		start = 0, step = 1;
	for(y=0;y<height;y++){
		if(sysInf.g_highSpeed && bFast){	
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<width;x+=step){
			if(a==0xff){
				AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&pSrc[x] );
			}else{
				tmpSrc.bRed    = AlphaTable[pSrc[x].bRed][a];
				tmpSrc.bGreen  = AlphaTable[pSrc[x].bGreen][a];
				tmpSrc.bBlue   = AlphaTable[pSrc[x].bBlue][a];
				AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&tmpSrc );
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
		pSrc  = (myRGB *)((BYTE *)pSrc  +srcP_W);
	}
} // Add24to24A

void StretchAdd8to24A(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,float alpha, BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	myRGB	tmpSrc;
	BYTE	*pSrcPix;
	BYTE	a = BYTE( 0xff *alpha );

	if(0==a)return;
	int		start = 0, step = 1;
	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = pSrc +srcP_W*ys;
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			if(0xff==a){
				tmpSrc.bRed   = srcPal[pSrcPix[xs]].bRed;
				tmpSrc.bGreen = srcPal[pSrcPix[xs]].bGreen;
				tmpSrc.bBlue  = srcPal[pSrcPix[xs]].bBlue;
			}else{
				tmpSrc.bRed   = AlphaTable[srcPal[pSrcPix[xs]].bRed][a];
				tmpSrc.bGreen = AlphaTable[srcPal[pSrcPix[xs]].bGreen][a];
				tmpSrc.bBlue  = AlphaTable[srcPal[pSrcPix[xs]].bBlue][a];
			}
			AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&tmpSrc );

		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchAdd8to24A

void StretchAdd9to24A(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,float alpha, BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	myRGB	tmpSrc;
	BYTE	*pSrcPix , a;

	int		start = 0, step = 1;
	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = pSrc +srcP_W*ys;
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			a = BYTE( srcPal[pSrcPix[xs]].bAlpha *alpha );
			if(0==a)continue;
			if(0xff==a){
				tmpSrc.bRed   = srcPal[pSrcPix[xs]].bRed;
				tmpSrc.bGreen = srcPal[pSrcPix[xs]].bGreen;
				tmpSrc.bBlue  = srcPal[pSrcPix[xs]].bBlue;
			}else{
				tmpSrc.bRed   = AlphaTable[srcPal[pSrcPix[xs]].bRed][a];
				tmpSrc.bGreen = AlphaTable[srcPal[pSrcPix[xs]].bGreen][a];
				tmpSrc.bBlue  = AlphaTable[srcPal[pSrcPix[xs]].bBlue][a];
			}
			AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&tmpSrc );
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchAdd9to24A

void StretchAdd24to24A(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	myRGB *pSrc, int srcP_W,  int widthSrc, int heightSrc,float alpha, BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	myRGB	*pSrcPix, tmpSrc;
	BYTE a = BYTE(alpha*0xFF);

	int		start = 0, step = 1;
	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = (myRGB *)((BYTE *)pSrc +srcP_W*ys);
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			if(0xFF == a){
				AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&pSrcPix[xs] );
			}else if(a > 0){
				tmpSrc.bRed    = AlphaTable[pSrcPix[xs].bRed][a];
				tmpSrc.bGreen  = AlphaTable[pSrcPix[xs].bGreen][a];
				tmpSrc.bBlue   = AlphaTable[pSrcPix[xs].bBlue][a];
				AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&tmpSrc );
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchAdd24to24A

void StretchAdd32to24A(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	myRGBA *pSrc, int srcP_W,  int widthSrc, int heightSrc,float alpha, BOOL bFast)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	myRGBA	*pSrcPix;
	myRGB	tmpSrc;
	BYTE	a;

	int		start = 0, step = 1;
	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = (myRGBA *)((BYTE *)pSrc +srcP_W*ys);
		if(sysInf.g_highSpeed && bFast){
			start = (sysInf.g_skip+y)&1; step = 2;
		}
		for(x=start;x<widthDest;x+=step){
			xs = (xrange*x) >>11;
			a = BYTE(pSrcPix[xs].bAlpha*alpha);
			if(0xff == a){
				AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&pSrcPix[xs] );
			}else if(a > 0){
				tmpSrc.bRed    = AlphaTable[pSrcPix[xs].bRed][a];
				tmpSrc.bGreen  = AlphaTable[pSrcPix[xs].bGreen][a];
				tmpSrc.bBlue   = AlphaTable[pSrcPix[xs].bBlue][a];
				AddBlend( (LPDWORD)&pDest[x], (LPDWORD)&tmpSrc );
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
} // StretchAdd32to24A

extern char *itemName[20];

ItemList::ItemList(int num)
{
	char	fname[32];
	if(num < 200 || num>223)return;
	ESC_SetFlag(num,1);
	num -= 200;
	ZeroMemory(this,sizeof(ItemList));
	wsprintf(fname,"item_%02d",num);
	itemPrt.loadLGF(pack_gparts,fname);
	itemNamePrt.loadLGF(pack_gparts,"itemNameList");
	sysInf.backupTextColor();
	saveInf.textColor[0] = saveInf.textColor[1] = saveInf.textColor[2] = 0xff;
	int offset = (650 -lstrlen(itemName[num])*(mainFontSize/2)) /2;
	for(int i=0;i<(int)lstrlen(itemName[num]);i+=2){
		BOOL bHankaku = OutputOutlineFont(&itemNamePrt,(BYTE *)&itemName[num][i] ,i*(mainFontSize/2)+offset,13,mainFontSize);
		if(bHankaku) i--;	
	}
	fadePrt.createColorBuf(WIN_SIZEX, WIN_SIZEY, 24, TRUE);
	itemNamePrt.alpha = 0;
	fadePrt.alpha = 0;
	showStep = 0;
	time = timeGetTime();
	itemNum = num;
	sysInf.bShowMsgwin = off;
	sysInf.bHideMsgwin = off;
	if(G_SKIP)bHighSpeed = TRUE;
	state = 3;
}

BOOL ItemList::GetItem()
{
	if(itemNum>23) return TRUE;
	if(bHighSpeed){
		int atime = (timeGetTime() -time);
		if(atime < 100){
			itemPrt.alpha = (atime) /100.0f;
		}else if(atime < 400){
			itemPrt.alpha = 1.0f;
		}else if(atime < 500){
			itemPrt.alpha = (100-(atime-400)) /100.0f;
		}else{
			return TRUE;
		}
		itemNamePrt.alpha = itemPrt.alpha;
		fadePrt.alpha = itemPrt.alpha /2;
		return FALSE;
	}
	switch(state){
	case 3:
		fadePrt.alpha = (timeGetTime() -time) /400.0f;
		if(fadePrt.alpha >= 0.5f){
			fadePrt.alpha = 0.5f;
			time = timeGetTime();
			state = 0;
		}
		break;
	case 0:
		showStep = (timeGetTime() -time) /400.0f;
		if(showStep >= 1.0f){
			showStep = 1.0f;
			state = 1;
			time = timeGetTime();
		}
		break;
	case 1:
		if(timeGetTime() -time < 500){
			itemNamePrt.alpha = (timeGetTime() -time)/ 500.0f;
		}else{
			itemNamePrt.alpha = 1.0f;
			if(timeGetTime() -time > 1500){
				time = timeGetTime() +1500;
				state = 2;
			}
		}
		break;
	case 2:
		showStep = (time -(int)timeGetTime()) /1500.0f;
		if(showStep<0){
			sysInf.restoreTextColor();
			showStep = 0;
			return TRUE;
		}
		fadePrt.alpha = showStep /2;
		break;
	}
	return FALSE;
}

void ItemList::Draw()
{
	RECT	destRect;

	g_DibInf.colorBuf.BltFast(0,0,&fadePrt,NULL,TRUE);
	if(bHighSpeed){	
		g_DibInf.colorBuf.BltFast(400-128,240-128,&itemPrt,NULL,TRUE);
		g_DibInf.colorBuf.BltFast(400-325,396,&itemNamePrt,NULL,TRUE);
		return;
	}
	rect.left = 0;	rect.right = 256;
	rect.top = 0;	rect.bottom = 256;
	switch(state){
	case 0:
		g_DibInf.colorBuf.SpiralBlt(400,240, &itemPrt,&rect,showStep,showStep*0.9f);
		break;
	case 1:
		if(itemNamePrt.alpha < 1.0f){
			destRect.left = LONG(282 -10*itemNamePrt.alpha);	destRect.right  = LONG(518 +10*itemNamePrt.alpha);
			destRect.top  = LONG(122 -10*itemNamePrt.alpha);	destRect.bottom = LONG(358 +10*itemNamePrt.alpha);
			g_DibInf.colorBuf.Blt(&destRect,&itemPrt,NULL,TRUE);
		}else{
			g_DibInf.colorBuf.BltFast(400-128,240-128,&itemPrt,NULL,TRUE);
		}
		g_DibInf.colorBuf.BltFast(400-325,396,&itemNamePrt,NULL,TRUE);
		break;
	case 2:
		destRect.left = LONG(282 -10*showStep);	destRect.right  = LONG(518 +10*showStep);
		destRect.top  = LONG(122 -10*showStep);	destRect.bottom = LONG(358 +10*showStep);
		itemNamePrt.alpha = itemPrt.alpha = showStep;
		g_DibInf.colorBuf.Blt(&destRect,&itemPrt,NULL,TRUE);
		g_DibInf.colorBuf.BltFast(400-325,int(396+40*(1.0f-showStep)),&itemNamePrt,NULL,TRUE);
		break;
	}
}


void CreateAlphaTable()
{
	int		i,j;
	for(i=0;i<256;i++){
		for(j=0;j<256;j++){
			AlphaTable[i][j] = BYTE(i*j/255);
		}
	}
} // CreateAlphaTable

void MyButton::Create(BYTE bt_type, ColorBuf *partsCBuf, RECT *s_rect, RECT *p_rect, ColorBuf *dest)
{
	int		i;

	type = bt_type;
	btnCBuf = partsCBuf;
	selectRect = *s_rect;
	if(254<=type){	
		for(i=0;i<2;i++){
			partsRect[i] = p_rect[i];
		}
	}else{
		for(i=0;i<(type+2);i++){
			partsRect[i] = p_rect[i];
		}
	}
	destCBuf = dest;
	state = btn_normal;
} // MyButton::Create

BOOL MyButton::CheckState(POINT *msPoint)
{
	flashTime = timeGetTime() % 1000;
	switch(state){
	  case btn_normal:
	  case btn_onCursor:
		if(myPtInRect(&selectRect,msPoint)){
			if(keyState.push_action){
				if(keyState.data_action){
					state = btn_push;
				}else{	
					if(0==type){
						state = btn_normal;
					}else if(4==type || 5==type){
						state = btn_lock;
					}else{
						state = btn_onCursor;
					}
					state = btn_normal;
					return TRUE;	
				}
			}else if(0==type){
				state = btn_normal;
			}else{
				state = btn_onCursor;
			}
		}else{
			state = btn_normal;
		}
		break;
	  case btn_push:
		if(myPtInRect(&selectRect,msPoint)){
			if(keyState.data_action){
				state = btn_push;
			}else{
				if(0==type){
					state = btn_normal;
				}else if(4==type || 5==type){
					state = btn_lock;
				}else{
					state = btn_onCursor;
				}
				state = btn_normal;
				return TRUE;
			}
		}else{
			state = btn_normal;
		}
		break;
	  case btn_lock:
		if(myPtInRect(&selectRect,msPoint)){
			if(keyState.push_action){
				state = btn_onCursor;
				state = btn_normal;
				return TRUE;
			}
		}
	}
	return FALSE;	
} // MyButton::CheckState

BOOL MyButton::CheckCheckBtnState(POINT *msPoint)
{
	flashTime = timeGetTime() % 1000;

	if(myPtInRect(&selectRect,msPoint)){
		state = ButtonState(state | btn_onCursor);
		if(keyState.push_action){
			if(!(state & btn_push)){	
				state = ButtonState(state | btn_push);
			}else if(254==type){
				state = ButtonState(state & btn_onCursor);
			}else{
				return FALSE;
			}
			return TRUE;
		}
	}else{
		state = ButtonState(state & btn_push);
	}
	return FALSE;
}

void MyButton::Draw()
{
	if(btn_nouse==state)return;
	if(254>type){	
		if(btn_disable==state && type<2)return;
		destCBuf->BltFast(selectRect.left, selectRect.top, btnCBuf, &partsRect[state], TRUE);
		if( type>=3 && ((btn_onCursor==state)||(btn_lock==state)) ){
			float tmp_alpha = btnCBuf->alpha;
			float flash = (flashTime -500) /500.0f;
			btnCBuf->alpha = fabsf(flash) * tmp_alpha;
			if(btn_onCursor==state){
				destCBuf->BltFast(selectRect.left, selectRect.top, btnCBuf, &partsRect[4], TRUE);
			}else{
				destCBuf->BltFast(selectRect.left, selectRect.top, btnCBuf, &partsRect[6], TRUE);
			}
			btnCBuf->alpha = tmp_alpha;
		}
	}else{
		if(btn_normal==state)return;
		if(btn_push & state){
			destCBuf->BltFast(selectRect.left, selectRect.top, btnCBuf, &partsRect[1], TRUE);
		}
		if(btn_onCursor & state){
			destCBuf->BltFast(selectRect.left, selectRect.top, btnCBuf, &partsRect[0], TRUE);
		}
	}
} // MyButton::Draw

#define	_TIME_NORMAL		0	
#define	_TIME_EVENING		1
#define	_TIME_NIGHT			2
#define	_TIME_NT_ROOM		3
#define	_TIME_SEPIA2		1
#define	_TIME_NEGA			2
#define	_TIME_MONO			3
#define	_TIME_BLUE			4
#define	_TIME_RED			5
#define	_TIME_GREEN			6
#define	_TIME_PURE_BLUE		7
#define	_TIME_BROWN			8
#define	_TIME_SEPIA_HALF	9
#define	_TIME_BLACK			10
#define	_TIME_YORITOMO		11
#define _TIME_BG_FILE		12

char	*tone_fname[] = { 
	NULL, 				
	"indoor.AMP",		
	"night.AMP",		
	"night.amp", 		
	"night.AMP",		
	"night.AMP",		
};

char	*eff_fname_tbl[] = {
	NULL,				
	"sepia.amp",		
	"nega.amp",			
	NULL,				
	"blue.amp",			
	"red.amp",			
	"green.amp",		
	"blue2.amp",		
	"brown.amp",		
	"sepia_half.AMP",	
	"black.AMP",
	"yoritomo.AMP",
};

void ToneTable::Convert(myRGB *rgb)
{
	if(bUseRGB){
		rgb->bRed   = red[rgb->bRed];
		rgb->bGreen = green[rgb->bGreen];
		rgb->bBlue  = blue[rgb->bBlue];
	}
	if(bUseAll){
		rgb->bRed   = all[rgb->bRed];
		rgb->bGreen = all[rgb->bGreen];
		rgb->bBlue  = all[rgb->bBlue];
	}
} // ToneTable::Convert

void ToneTable::Convert(myRGBA *rgb)
{
	if(bUseRGB){
		rgb->bRed   = red[rgb->bRed];
		rgb->bGreen = green[rgb->bGreen];
		rgb->bBlue  = blue[rgb->bBlue];
	}
	if(bUseAll){
		rgb->bRed   = all[rgb->bRed];
		rgb->bGreen = all[rgb->bGreen];
		rgb->bBlue  = all[rgb->bBlue];
	}
} // ToneTable::Convert


void ToneConvert::Convert(BYTE *pBuf,int width,int height,int wPitch,BYTE bpp,CBufType type)
{
	int		i,x,y;
	if(bpp==24){
		for(y=0;y<height;y++){
			myRGB *rgb = (myRGB *)(pBuf +wPitch*y);
			for(x=0;x<width;x++){
				for(i=0;i<2;i++){
					if(i==0){
						if(type==Type_back ||(type==Type_char && _TIME_NT_ROOM==saveInf.tone_type[0]))continue;
					}
					if(bUseTable[i]){
						if(i==1 && (saveInf.tone_type[1]==_TIME_MONO || saveInf.tone_type[1]==_TIME_SEPIA2 || saveInf.tone_type[1]==_TIME_SEPIA_HALF)){
							BYTE col = BYTE(0.299f*rgb[x].bRed + 0.587f*rgb[x].bGreen + 0.114f*rgb[x].bBlue);
							if(saveInf.tone_type[1]!=_TIME_SEPIA_HALF){
								rgb[x].bBlue = rgb[x].bGreen = rgb[x].bRed = col;
							}else{
								rgb[x].bBlue  = (rgb[x].bBlue  +col)/2;
								rgb[x].bGreen = (rgb[x].bGreen +col)/2;
								rgb[x].bRed   = (rgb[x].bRed   +col)/2;
							}
							if(saveInf.tone_type[1]==_TIME_MONO)continue;
						}
						table[i].Convert(&rgb[x]);
					}
				}
			}
		}
	}
	if(bpp==32){
		for(y=0;y<height;y++){
			myRGBA *rgb = (myRGBA *)(pBuf +wPitch*y);
			for(x=0;x<width;x++){
				for(i=0;i<2;i++){
					if(i==0){
						if(type==Type_back ||(type==Type_char && _TIME_NT_ROOM==saveInf.tone_type[0]))continue;
					}
					if(bUseTable[i]){
						if(i==1 && (saveInf.tone_type[1]==_TIME_MONO || saveInf.tone_type[1]==_TIME_SEPIA2 || saveInf.tone_type[1]==_TIME_SEPIA_HALF)){
							BYTE col = BYTE(0.299f*rgb[x].bRed + 0.587f*rgb[x].bGreen + 0.114f*rgb[x].bBlue);
							if(saveInf.tone_type[1]!=_TIME_SEPIA_HALF){
								rgb[x].bBlue = rgb[x].bGreen = rgb[x].bRed = col;
							}else{
								rgb[x].bBlue  = (rgb[x].bBlue  +col)/2;
								rgb[x].bGreen = (rgb[x].bGreen +col)/2;
								rgb[x].bRed   = (rgb[x].bRed   +col)/2;
							}
							if(saveInf.tone_type[1]==_TIME_MONO)continue;
						}
						table[i].Convert(&rgb[x]);
					}
				}
			}
		}
	}
} // ToneConvert::Convert

void ToneConvert::ClearTable()
{
	saveInf.tone_type[0] = saveInf.tone_type[1] = 0;
	saveInf.tone_backOnly = FALSE;
	bUseTable[0] = bUseTable[1] = FALSE;
	bUse = FALSE;
	bSetChar = FALSE;
} // ToneConvert::ClearTable

void ToneConvert::ChangeTable(int mainType, int subType, BOOL bBack)
{
	char	*lpBuf;
	int		i,size;

	saveInf.tone_type[0] = mainType;
	saveInf.tone_type[1] = subType;
	saveInf.tone_backOnly = bBack;
	for(i=0;i<2;i++){
		bUseTable[i] = FALSE;
		table[i].bUseAll = table[i].bUseRGB = FALSE;
		if((i==0 && mainType>0) || (i==1&&subType>0)){
			bUseTable[i] = TRUE;
			if(i==0){
				size = readFile->ReadPackFile(pack_gparts,tone_fname[mainType],&lpBuf);
			}else{
				if(_TIME_MONO==subType)continue;
				size = readFile->ReadPackFile(pack_gparts,eff_fname_tbl[subType],&lpBuf);
			}
			if(size==256){
				CopyMemory(table[i].all,lpBuf,256);
				table[i].bUseAll = TRUE;
			}else if(size==256*3){
				CopyMemory(table[i].red,  lpBuf,256);
				CopyMemory(table[i].green,lpBuf+256,256);
				CopyMemory(table[i].blue, lpBuf+512,256);
				table[i].bUseRGB = TRUE;
			}else if(size>=256*4){
				CopyMemory(table[i].all,  lpBuf,256);
				CopyMemory(table[i].red,  lpBuf+256,256);
				CopyMemory(table[i].green,lpBuf+512,256);
				CopyMemory(table[i].blue, lpBuf+768,256);
				table[i].bUseAll = TRUE;
				table[i].bUseRGB = TRUE;
			}
			cl_free(lpBuf);
		}
	}
	bUse = FALSE;
	if(bUseTable[0] || bUseTable[1])bUse = TRUE;
	if(FALSE==bBack){
		for(i=0;i<STCHAR_MAX;i++){	
			if(charInf[i].save.char_no>0){
				charInf[i].save.bWaitTone = TRUE;
			}
		}
		bSetChar = TRUE;
	}
} // ToneConvert::ChangeTable

void ToneConvert::CheckBGTable(char *file)
{
	char	*lpBuf,fname[256];
	int		i,size;

	wsprintf(fname,"%s.AMP",file);
	if( (-1) == readFile->SearchFile(pack_gparts, fname))return;
	size = readFile->ReadPackFile(pack_gparts,fname,&lpBuf);
	if(0==size)return;

	table[0].bUseAll = table[0].bUseRGB = FALSE;
	bUseTable[0] = TRUE;
	bUse = TRUE;
	if(size==256){
		CopyMemory(table[0].all,lpBuf,256);
		table[0].bUseAll = TRUE;
	}else if(size==256*3){
		CopyMemory(table[0].red,  lpBuf,256);
		CopyMemory(table[0].green,lpBuf+256,256);
		CopyMemory(table[0].blue, lpBuf+512,256);
		table[0].bUseRGB = TRUE;
	}else if(size>=256*4){
		CopyMemory(table[0].all,  lpBuf,256);
		CopyMemory(table[0].red,  lpBuf+256,256);
		CopyMemory(table[0].green,lpBuf+512,256);
		CopyMemory(table[0].blue, lpBuf+768,256);
		table[0].bUseAll = TRUE;
		table[0].bUseRGB = TRUE;
	}
	cl_free(lpBuf);
	if(FALSE==saveInf.tone_backOnly){
		for(i=0;i<STCHAR_MAX;i++){	
			if(charInf[i].save.char_no>0){
				charInf[i].save.bWaitTone = TRUE;
			}
		}
		bSetChar = TRUE;
	}
} // ToneConvert::CheckBGTable

void ToneConvert::SetCharTable()
{
	int		i;
	if(FALSE==bSetChar || saveInf.tone_backOnly)return;
	bSetChar = FALSE;
	for(i=0;i<STCHAR_MAX;i++){	
		if(charInf[i].save.char_no>0 && _CHR_DIRECT==charInf[i].in_type && charInf[i].save.bWaitTone){
			POINT pt = charInf[i].colorBuf.offset;
			charInf[i].colorBuf.loadLGF(pack_eventcg,charInf[i].colorBuf.filename,Type_char);
			charInf[i].colorBuf.offset = pt;
			charInf[i].save.bWaitTone = FALSE;
		}
	}
} // ToneConvert::SetCharTable

