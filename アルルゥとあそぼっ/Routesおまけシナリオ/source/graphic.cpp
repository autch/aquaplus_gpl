#include "Clcommon.h"
#include "msgWnd.h"
#include "graphic.h"

BYTE		AlphaTable[256][256];
ColorBuf	backColorBuf;
ColorBuf	bak_backColorBuf;
ColorBuf	ptnFadeBuf;
BGInf		bgInf;
StdCharInf	charInf[STCHAR_MAX];
SetBmpInf	setBmpInf[STBMP_MAX];
ToneConvert	toneConvert;

RECT		bgRect = {40,30,840,630};

void CopyEqualColor(BYTE *pDest,int destP_W, BYTE *pSrc, int srcP_W,int width,int height,int bpp);
void Copy24to24A(myRGB *pDest,int destP_W, myRGB *pSrc1,int src1P_W, myRGB *pSrc2, int src2P_W, int width,int height,float alpha);
void Copy24to24A(myRGB *pDest,int destP_W, myRGB *pSrc, int srcP_W, int width,int height,float alpha);
void Copy32to24(myRGB *pDest,int destP_W, myRGBA *pSrc, int srcP_W, int width,int height);
void Copy24to32(myRGBA *pDest,int destP_W, myRGB *pSrc, int srcP_W, int width,int height);
void Copy32to24A(myRGB *pDest,int destP_W, myRGBA *pSrc, int srcP_W, int width,int height,float alpha);
void Copy32_2to24A(ColorBuf *destBuf,ColorBuf *srcBuf1, int offsetX1, int offsetY1,ColorBuf *srcBuf2, int offsetX2, int offsetY2,float alpha);
void Copy8to24(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height);
void Copy8to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha);
void StretchCopy8to24(myRGB *pDest,int destP_W, int widthDest,int heightDest,BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,BOOL bAlpha=FALSE);
void StretchCopy24to24(myRGB *pDest,int destP_W, int widthDest,int heightDest, myRGB *pSrc, int srcP_W, int widthSrc,int heightSrc);

BOOL ColorBuf::loadLGF(pack_file_no arcFileNum,char *fname,CBufType ctype)
{
	char	lgfFile[256];
	LgfHead	*lgfHead;
	BYTE	*srcBuf;

	Release();
	type = ctype;
	strcpy(filename,fname);
	wsprintf(lgfFile,"%s.LGF",fname);
	int ret = readFile->ReadPackFile(arcFileNum,lgfFile,(char **)&lgfHead);
	if(0==ret)return FALSE;
	width = lgfHead->width;
	height = lgfHead->height;
	bpp = lgfHead->bpp;
	offset.x = lgfHead->offsetX;
	offset.y = lgfHead->offsetY;
	srcBuf = (BYTE *)lgfHead +sizeof(LgfHead);
	if(bpp==8){	
		palColor = new myRGBA[256];
		CopyMemory(palColor,srcBuf,sizeof(myRGBA)*256);
		srcBuf += sizeof(myRGBA)*256;
	}
	wPitch = width * (bpp>>3);
	if(wPitch & 3){
		wPitch = ((wPitch>>2)+1) <<2;
	}
	pBuf = (BYTE *)cl_malloc(wPitch * height);
	CopyMemory(pBuf,srcBuf,wPitch *height);
	if(toneConvert.bUse){
		ToneConvert();
	}
	alpha = 1.0f;
	cl_free(lgfHead);

	return TRUE;
} // ColorBuf::loadLGF

BOOL ColorBuf::createColorBuf(int sWidth,int sHeight,BYTE sBpp,BYTE bClear)
{
	cl_free(pBuf);
	width = sWidth;
	height = sHeight;
	bpp = sBpp;
	offset.x = offset.y = 0;
	wPitch = width * (bpp>>3);
	if(wPitch & 3){
		wPitch = ((wPitch>>2)+1) <<2;
	}
	pBuf = (BYTE *)cl_malloc(wPitch * height);
	if(bClear) clearColorBuf();
	alpha = 1.0f;
	return TRUE;
} // ColorBuf::createColorBuf

BOOL ColorBuf::BltFast(int xDest,int yDest,ColorBuf *srcCBuf,RECT *srcRect,BOOL bAlpha)
{
	int		x,y;
	int xSrc, ySrc, widthSrc, heightSrc;

	if(bAlpha && srcCBuf->alpha>=1.0f)bAlpha = FALSE;	

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
	if(width < (xDest+widthSrc)){
		widthSrc = width -xDest;
	}
	if(height < (yDest+heightSrc)){
		heightSrc = height -yDest;
	}
	if(FALSE==bAlpha){
		if(bpp==srcCBuf->bpp){	
			CopyEqualColor(pDest,wPitch, pSrc,srcCBuf->wPitch, widthSrc,heightSrc,bpp);
		}else switch(srcCBuf->bpp){
		  case 8:
			Copy8to24((myRGB *)pDest,wPitch, pSrc,srcCBuf->wPitch, srcCBuf->palColor, widthSrc,heightSrc);
			break;
		  case 24:
			Copy24to32((myRGBA *)pDest,wPitch, (myRGB *)pSrc,srcCBuf->wPitch, widthSrc,heightSrc);
			break;
		  case 32:
			Copy32to24((myRGB *)pDest,wPitch, (myRGBA *)pSrc,srcCBuf->wPitch, widthSrc,heightSrc);
			break;
		}
	}else{
		switch(srcCBuf->bpp){
		  case 8:
			Copy8to24A((myRGB *)pDest,wPitch, pSrc,srcCBuf->wPitch, srcCBuf->palColor, widthSrc,heightSrc,srcCBuf->alpha);
			break;
		  case 24:
			Copy24to24A((myRGB *)pDest,wPitch, (myRGB *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc,srcCBuf->alpha);
			break;
		  case 32:
			Copy32to24A((myRGB *)pDest,wPitch, (myRGBA *)pSrc,srcCBuf->wPitch, widthSrc,heightSrc,srcCBuf->alpha);
			break;
		}
	}
	return TRUE;
} // ColorBuf::BltFast

BOOL ColorBuf::Blt(RECT *dstRect,ColorBuf *srcCBuf,RECT *srcRect,BOOL bAlpha)
{
	int		x,y;
	int xDest, yDest, widthDest, heightDest;
	int	xSrc, ySrc, widthSrc, heightSrc;

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
	LPBYTE	pDest = pBuf +wPitch*yDest +xDest*(bpp>>3);
	LPBYTE	pSrc = srcCBuf->pBuf +(srcCBuf->wPitch)*ySrc +xSrc*(srcCBuf->bpp>>3);

	if(8==srcCBuf->bpp){
		StretchCopy8to24((myRGB *)pDest,wPitch,widthDest,heightDest,pSrc,srcCBuf->palColor,srcCBuf->wPitch,widthSrc,heightSrc,bAlpha);
	}else if(FALSE==bAlpha){
		if(bpp==srcCBuf->bpp){	
			StretchCopy24to24((myRGB *)pDest,wPitch,widthDest,heightDest,(myRGB *)pSrc,srcCBuf->wPitch,widthSrc,heightSrc);
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

	for(y=0;y<heightSrc;y++){
		for(x=0;x<widthSrc;x++){
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

BOOL ColorBuf::BrightBlt(int xDest,int yDest,ColorBuf *srcCBuf,RECT *srcRect,int r, int g, int b)
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
	myRGB	*pDest = (myRGB *)(pBuf +wPitch*yDest +xDest*(bpp>>3));
	myRGB	*pSrc = (myRGB *)(srcCBuf->pBuf +(srcCBuf->wPitch)*ySrc +xSrc*(srcCBuf->bpp>>3));
	if(width < (xDest+widthSrc)){
		widthSrc = width -xDest;
	}
	if(height < (yDest+heightSrc)){
		heightSrc = height -yDest;
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
	for(y=0;y<heightSrc;y++){
		for(x=0;x<widthSrc;x++){
			if(bR)pDest[x].bRed   = AlphaTable[pSrc[x].bRed][r];
			else pDest[x].bRed = pSrc[x].bRed +AlphaTable[0xff-pSrc[x].bRed][r];
			if(bG)pDest[x].bGreen = AlphaTable[pSrc[x].bGreen][g];
			else pDest[x].bGreen = pSrc[x].bGreen +AlphaTable[0xff-pSrc[x].bGreen][g];
			if(bB)pDest[x].bBlue  = AlphaTable[pSrc[x].bBlue][b];
			else pDest[x].bBlue = pSrc[x].bBlue +AlphaTable[0xff-pSrc[x].bBlue][b];
		}
		pDest = (myRGB *)((BYTE *)pDest +wPitch);
		pSrc = (myRGB *)((BYTE *)pSrc +srcCBuf->wPitch);
	}
	return TRUE;
}

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
	BYTE	*ptnBuf = ptnFadeBuf.pBuf;

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

	for(y=0;y<heightSrc;y++){
		for(x=0;x<widthSrc;x++){
			alpha = add +ptnBuf[x] -0xff;
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
		ptnBuf += ptnFadeBuf.wPitch;
	}
	return TRUE;
} // ColorBuf::PtnFade

BOOL ColorBuf::ShakeBlt(ColorBuf *srcCBuf,int s_size,int step,int stepMax)
{
	RECT	shakeRect = bgRect;
	int		add;
	float	ang;

	switch(bgInf.in_type){
	  case _BAK_SHAKE:
		if(stepMax>0){
			s_size = (s_size*(stepMax-step) /stepMax );
			add = int (s_size *sinf(DEGtoRAD(20*step)));
		}else{
			add = int (s_size *sinf(DEGtoRAD(20*bgInf.shake_cnt)));
		}
		shakeRect.top += add;
		shakeRect.bottom = shakeRect.top +WIN_SIZEY;
		if(shakeRect.top < 0) shakeRect.top = 0;
		if(shakeRect.bottom > BG_SIZEY)shakeRect.bottom = BG_SIZEY;

		BltFast(0,0,srcCBuf,&shakeRect,FALSE);
		break;
	  case _BAK_RAND:
		if(stepMax>0){
			s_size = (s_size*(stepMax-step) /stepMax );
		}
		ang = DEGtoRAD(rndSelect(360));
		shakeRect.top += int(sinf(ang)*s_size);
		shakeRect.bottom = shakeRect.top +WIN_SIZEY;
		if(shakeRect.top < 0) shakeRect.top = 0;
		if(shakeRect.bottom > BG_SIZEY)shakeRect.bottom = BG_SIZEY;
		shakeRect.left += int(cosf(ang)*s_size);
		shakeRect.right = shakeRect.left +WIN_SIZEX;
		if(shakeRect.left < 0) shakeRect.left = 0;
		if(shakeRect.right > BG_SIZEX)shakeRect.right = BG_SIZEX;
		BltFast(0,0,srcCBuf,&shakeRect,FALSE);
		break;
		break;
	  case _BAK_ZOOMSHAK:
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

	rect.left = 40;	rect.right = rect.left +width;
	rect.top = 30; rect.bottom = rect.top  +height;
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
	for(y=0;y<height;y++){
		st_p.x = pt[0].x +aw[0]*y;
		st_p.y = pt[0].y +ah[0]*y;
		bIn = FALSE;
		for(x=0;x<width;x++){
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

void drawChar()
{
	int		i,j;
	float	alpha;

	if(off==sysInf.charFlag)return;
	for(i=0;i<STCHAR_MAX;i++){
		for(j=0;j<STCHAR_MAX;j++){
			if(0==charInf[j].save.char_no || i!=charInf[j].save.layer) continue;
			if(_CHR_FADE==charInf[j].out_type){
				alpha = GetEffectStep(charInf[j].look_cnt,charInf[j].look_max,FALSE);
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
				alpha = GetEffectStep(charInf[j].look_cnt,charInf[j].look_max,TRUE);
				if(NULL==charInf[j].bak_colorBuf.pBuf){
					charInf[j].colorBuf.alpha = alpha;
					backColorBuf.BltFast(0,0,&charInf[j].colorBuf,NULL,TRUE);
				}else{	
					Copy32_2to24A(&backColorBuf,
						&charInf[j].colorBuf, charInf[j].colorBuf.offset.x, charInf[j].colorBuf.offset.y,
						&charInf[j].bak_colorBuf, charInf[j].bak_colorBuf.offset.x, charInf[j].bak_colorBuf.offset.y,
						alpha);
				}
				break;
			  case _CHR_DIRECT:	
				backColorBuf.BltFast(0,0,&charInf[j].colorBuf,NULL,FALSE);
				break;
			}
		}
	}
} 

BOOL bFadeExec = FALSE;
int bgFade;
void drawBG()
{
	if(off==sysInf.bgFlag)return;
	if(FALSE==bFadeExec)
		backColorBuf.BltFast(saveInf.btPoint.x, saveInf.btPoint.y, &bgInf.colorBuf, &saveInf.btRect, FALSE);
	else
		backColorBuf.BrightBlt(saveInf.btPoint.x, saveInf.btPoint.y, &bgInf.colorBuf,&saveInf.btRect,bgFade,bgFade,bgFade);	


	sysInf.charFlag = TRUE;	
} // drawBG

void drawBack()
{
	float	ratio;
	if(saveInf.bWave){
		g_DibInf.colorBuf.WaveBlt(0,0,&backColorBuf,&bgRect,saveInf.waveSize,saveInf.waveCnt);
		return;
	}
	if(0==bgInf.look_cnt && 0==bgInf.shake_cnt || _BAK_TXT_SHAKE==bgInf.in_type){
		backColorBuf.alpha = 1;
		g_DibInf.colorBuf.BltFast(0,0,&backColorBuf,&bgRect,FALSE);
	}else{
		switch(bgInf.in_type){
		  case _BAK_SHAKE:
		  case _BAK_ZOOMSHAK:
		  case _BAK_RAND:
			g_DibInf.colorBuf.ShakeBlt(&backColorBuf,bgInf.shakeSize,bgInf.look_max-bgInf.look_cnt,bgInf.look_max);
			break;
		  case _BAK_ROLL:
		  case _BAK_ROLL_2TI:
			{
				ratio = sinf(M_PI/2*(bgInf.look_max-bgInf.look_cnt)/bgInf.look_max);
				if(_BAK_ROLL==bgInf.in_type){	
					g_DibInf.colorBuf.RollBlt(&backColorBuf, bgInf.shakeSize*180.0f*ratio);
				}else{	
					ratio = sinf(bgInf.shakeSize*M_PI*ratio)*30*(1.0f-ratio);
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
		  default:
			g_DibInf.colorBuf.PtnFade(0,0,&bak_backColorBuf,&backColorBuf,&bgRect,
				bgInf.look_max-(bgInf.look_cnt-timeGetTime()),
				bgInf.look_max);
			break;
		}
	}
	sysInf.bgFlag = sysInf.charFlag = sysInf.drawFlag = off;
} // drawBack

int CheckCharLocate(int char_no)
{
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

BOOL c_disp(int char_no, int pose, int locate, int in_type, int layer,int frame)
{
	int		i;
	BOOL	bNoChangeLayer = FALSE;

	sysInf.drawFlag = on;
	sysInf.charFlag = on;
	if(_CHR_FADE==in_type){
		sysInf.bgFlag = on;
	}
	for(i=0;i<STCHAR_MAX;i++){
		if(charInf[i].save.char_no==char_no){	
			bNoChangeLayer = TRUE;
			if(_CHR_FADE==charInf[i].in_type){
				if(charInf[i].look_cnt<=timeGetTime() || G_SKIP){
					charInf[i].look_cnt = 0;
					charInf[i].in_type = _CHR_DIRECT;
					charInf[i].colorBuf.alpha = 1.0f;
					charInf[i].bak_colorBuf.Release();
					return TRUE;
				}
				return FALSE;
			}else{
				if(_CHR_WAIT!=in_type)locate = charInf[i].save.locate;	
				switch(in_type){
				  case _CHR_FADE:
					charInf[i].bak_colorBuf = charInf[i].colorBuf;
					charInf[i].colorBuf.pBuf = NULL;
					goto charLoad;
				  case _CHR_DIRECT: case _CHR_WAIT:
					if(pose==charInf[i].save.pose && locate==charInf[i].save.locate)return TRUE;
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
	char fname[32];
	wsprintf(fname,"c%02d%04d",char_no,pose);
	charInf[i].colorBuf.loadLGF(pack_gparts,fname,Type_char);
	switch(locate){
	  case _CHR_LL:
		charInf[i].colorBuf.offset.x += (-220 +40);
		break;
	  case _CHR_RR:
		charInf[i].colorBuf.offset.x += (220 +40);
		break;
	  default:
		charInf[i].colorBuf.offset.x += (locate-1)*180 +40;
		break;
	}
	if(charInf[i].colorBuf.offset.y<0)charInf[i].colorBuf.offset.y = 0;
	if(charInf[i].colorBuf.offset.x<0)charInf[i].colorBuf.offset.x = 0;
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
	char	fname[32];

	sysInf.drawFlag = on;
	if((-1)==bak_no) sysInf.bgFlag = off;
	if(0==bgInf.look_cnt){	
		bFadeExec = FALSE;
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
		sysInf.bgFlag = on;
		if(bak_no>=0)switch(cg_type){
		  case 0:
			wsprintf(fname,"s%03d%d%d",bak_no/10,saveInf.tone_type[0],bak_no%10);
			bgInf.colorBuf.loadLGF(pack_eventcg,fname,Type_back);
			toneConvert.CheckBGTable(fname);	
			toneConvert.SetCharTable();
			break;
		  case 1:
			wsprintf(fname,"v%05d",bak_no);
			bgInf.colorBuf.loadLGF(pack_eventcg,fname,Type_event);
			break;
		  case 2:
			wsprintf(fname,"h%05d",bak_no);
			bgInf.colorBuf.loadLGF(pack_eventcg,fname,Type_event);
			break;
		}
		bgInf.colorBuf.offset.x = 0; bgInf.colorBuf.offset.y = 0;
		saveInf.btRect.left = x; saveInf.btRect.top = y;
		saveInf.btRect.right = bgInf.colorBuf.width;
		saveInf.btRect.bottom = bgInf.colorBuf.height;
		saveInf.btPoint.x = saveInf.btPoint.y = 0;
		int	tmp_width  = saveInf.btRect.right -saveInf.btRect.left;
		int tmp_height = saveInf.btRect.bottom -saveInf.btRect.top;
		if(BG_SIZEX>tmp_width) saveInf.btPoint.x = (BG_SIZEX -tmp_width ) /2;
		if(BG_SIZEY>tmp_height)saveInf.btPoint.y = (BG_SIZEY -tmp_height) /2;
		if(cg_type==1 || cg_type==2){
			saveInf.btPoint.x = 40;
			saveInf.btPoint.y = 30;
		}
		if( (_BAK_DIRECT)==chg_type )return TRUE;
		if(chg_type>=_BAK_PATTERN0){	
			wsprintf( fname, "f00%02d", chg_type&0x7f );
			ptnFadeBuf.loadLGF(pack_gparts,fname);
			bgInf.in_type = chg_type;
		}
		bgInf.look_max = fade*1000/60;
		bgInf.look_cnt = timeGetTime() +bgInf.look_max;
		bak_backColorBuf.BltFast(bgRect.left,bgRect.top,&g_DibInf.colorBuf,NULL,FALSE); 
	}
	if(bgInf.look_cnt<=timeGetTime() || G_SKIP){
		bgInf.in_type = _BAK_DIRECT;
		bgInf.look_cnt = 0;
		return TRUE;
	}
	sysInf.bShowMsgwin = off; 
	return FALSE;
} // g_disp

BOOL SetBackFade(int r, int g, int b, int frame)
{
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
		bgInf.in_type = _BAK_CFADE;
	}
	if(bgInf.look_cnt<=timeGetTime() || G_SKIP){
		bgInf.look_cnt = 0;
		return FALSE;
	}
	sysInf.bShowMsgwin = off;
	return TRUE;
} // SetBackFade

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
		bgFade = 0xff;
		return FALSE;
	}
	float diff = GetEffectStep(look_cnt,look_max,TRUE);
	if(x==680){
		bFadeExec = TRUE;
		bgFade = 128 +int(128*diff);
	}
	saveInf.btRect.left = orgPt.x + int((x -orgPt.x)*diff);
	saveInf.btRect.top  = orgPt.y + int((y -orgPt.y)*diff);
	if(bgInf.colorBuf.type==Type_back){
		saveInf.btRect.right = saveInf.btRect.left+BG_SIZEX;
		saveInf.btRect.bottom = saveInf.btRect.top+BG_SIZEY;
	}else{
		saveInf.btRect.right = saveInf.btRect.left+WIN_SIZEX;
		saveInf.btRect.bottom = saveInf.btRect.top+WIN_SIZEY;
	}
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

void SetBmpInf::SetBmpGraph( char *fname, int i_layer, int i_nuki )
{
	char	file[_MAX_PATH];
	_splitpath(fname,NULL,NULL,file,NULL);
	colorBuf.loadLGF(pack_gparts,file,Type_setbmp);
	layer = i_layer;
	nuki = i_nuki;
	dstRect.left = colorBuf.offset.x;
	dstRect.top = colorBuf.offset.y;
	dstRect.right = dstRect.left +colorBuf.width;
	dstRect.bottom = dstRect.top +colorBuf.height;
	srcRect.left = srcRect.top = 0;
	srcRect.right = colorBuf.width;
	srcRect.bottom = colorBuf.height;
	colorBuf_Br.Release();
	bUseBright = FALSE;
	bDraw = TRUE;
} // SetBmpInf::SetBmpGraph

void SetBmpInf::Draw()
{
	if(FALSE==bDraw)return;
	BOOL bAlpha = TRUE;
	if(nuki==(-1))bAlpha = FALSE;
	if(FALSE==bUseBright){
		backColorBuf.Blt(&dstRect,&colorBuf,&srcRect,bAlpha);
	}else{
		backColorBuf.Blt(&dstRect,&colorBuf_Br,&srcRect,bAlpha);
	}
} // SetBmpInf::Draw

void SetBmpInf::SetBmpPos(int dx, int dy, int sx, int sy, int width, int height)
{
	dstRect.left = dx +40;
	dstRect.top = dy +30;
	dstRect.right = dstRect.left +width;
	dstRect.bottom = dstRect.top +height;

	srcRect.left = sx;
	srcRect.top = sy;
	srcRect.right = srcRect.left +width;
	srcRect.bottom = srcRect.top +height;
}

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

void SetBmpInf::SetBmpZoom(int x, int y, int width, int height)
{
	dstRect.left = x +40;
	dstRect.right = dstRect.left +width;
	dstRect.top = y +30;
	dstRect.bottom = dstRect.top +height;

	srcRect.top = 0;
	if(y<0){
		srcRect.top = int( ((-y)/(float)height)*colorBuf.height );
		dstRect.top = 30;
	}
	if((y+height)>WIN_SIZEY){
		srcRect.bottom = int( (1.0f -((y+height)-WIN_SIZEY)/(float)height)*colorBuf.height );
		dstRect.bottom = WIN_SIZEY +30;
	}
	srcRect.left = 0;
	if(x<0){
		srcRect.left = int( ((-x)/(float)width)*colorBuf.width );
		dstRect.left = 40;
	}
	if((x+width)>WIN_SIZEX){
		srcRect.right = int( (1.0f -((x+width)-WIN_SIZEX)/(float)width)*colorBuf.width );
		dstRect.right = WIN_SIZEX +40;
	}
} // SetBmpInf::SetBmpZoom

void SetBmpDraw()
{
	int		i,j;

	for(i=STBMP_MAX-1;i>=0;i--){
		for(j=0;j<STBMP_MAX;j++){
			if(setBmpInf[j].bDraw==FALSE || setBmpInf[j].layer!=i)continue;
			setBmpInf[j].Draw();
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
	int width,int height)
{
	int		x,y;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
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
	int width,int height)
{
	int		x,y;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
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
	int width,int height, float alpha)
{
	int		x,y;
	BYTE	a;

	alpha = MaxMin(1.0f, alpha);
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
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

void Copy32_2to24A(
	ColorBuf *destBuf,
	ColorBuf *srcBuf1, int offsetX1, int offsetY1,	
	ColorBuf *srcBuf2, int offsetX2, int offsetY2,	
	float alpha)
{
	RECT	allRect,srcRect1,srcRect2;
	BOOL	bSrc1,bSrc2;
	int		x,y;
	BYTE	aS1,aS2,aD;
	myRGBA	*pSrc1,*pSrc2;
	myRGB	*pDest;

	alpha = MaxMin(1.0f, alpha);
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

	int width  = allRect.right -allRect.left;
	int height = allRect.bottom -allRect.top;
	pDest = (myRGB *)(destBuf->pBuf +(destBuf->wPitch*allRect.top)) +allRect.left;
	pSrc1 = (myRGBA *)(srcBuf1->pBuf +(srcBuf1->wPitch*(allRect.top-srcRect1.top))) +(allRect.left-srcRect1.left);
	pSrc2 = (myRGBA *)(srcBuf2->pBuf +(srcBuf2->wPitch*(allRect.top-srcRect2.top))) +(allRect.left-srcRect2.left);
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			bSrc1 = bSrc2 = FALSE;
			point.x = x +allRect.left;
			point.y = y +allRect.top;
			if(PtInRect(&srcRect1,point))bSrc1 = TRUE;
			if(PtInRect(&srcRect2,point))bSrc2 = TRUE;
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

void Copy8to24(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height)
{
	int		x,y;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			if(pSrc[x]>0){
				pDest[x].bRed   = palColor[pSrc[x]].bRed;
				pDest[x].bGreen = palColor[pSrc[x]].bGreen;
				pDest[x].bBlue  = palColor[pSrc[x]].bBlue;
			}
		}
		pDest = (myRGB *)( (BYTE *)pDest +destP_W );
		pSrc += srcP_W;
	}
}

void Copy8to24A(myRGB *pDest,int destP_W, BYTE *pSrc, int srcP_W, myRGBA *palColor, int width,int height,float alpha)
{
	int		x,y;
	alpha = MaxMin(1.0f, alpha);
	BYTE	as = BYTE(alpha*0xFF);
	BYTE	ad = 0xFF -as;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			if(pSrc[x]>0){
				pDest[x].bRed   = AlphaTable[palColor[pSrc[x]].bRed][as]   +AlphaTable[pDest[x].bRed][ad];
				pDest[x].bGreen = AlphaTable[palColor[pSrc[x]].bGreen][as] +AlphaTable[pDest[x].bGreen][ad];
				pDest[x].bBlue  = AlphaTable[palColor[pSrc[x]].bBlue][as]  +AlphaTable[pDest[x].bBlue][ad];
			}
		}
		pDest = (myRGB *)( (BYTE *)pDest +destP_W );
		pSrc += srcP_W;
	}
}

void Copy24to24A(
	myRGB *pDest, int destP_W,	
	myRGB *pSrc1, int src1P_W,
	myRGB *pSrc2, int src2P_W,
	int width,int height,float alpha)
{
	BYTE a = MaxMin(0xFF, BYTE(alpha*0xFF));
	int		x,y;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
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
	int width,int height,float alpha)
{
	BYTE a = MaxMin(0xFF,BYTE(alpha*0xFF));
	int		x,y;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
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
}

void StretchCopy8to24(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	BYTE *pSrc, myRGBA *srcPal, int srcP_W,  int widthSrc, int heightSrc,BOOL bAlpha)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	BYTE	*pSrcPix;

	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = pSrc +srcP_W*ys;
		for(x=0;x<widthDest;x++){
			xs = (xrange*x) >>11;
			if(!bAlpha || pSrcPix[xs]>0){
				pDest[x].bRed   = srcPal[pSrcPix[xs]].bRed;
				pDest[x].bGreen = srcPal[pSrcPix[xs]].bGreen;
				pDest[x].bBlue  = srcPal[pSrcPix[xs]].bBlue;
			}
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
	}
}

void StretchCopy24to24(
	myRGB *pDest,int destP_W, int widthDest,int heightDest,
	myRGB *pSrc, int srcP_W,  int widthSrc, int heightSrc)
{
	int		x, y, xs, ys;
	DWORD	xrange = 2048 *widthSrc  / widthDest;
	DWORD	yrange = 2048 *heightSrc / heightDest;
	myRGB	*pSrcPix;

	for(y=0;y<heightDest;y++){
		ys = (yrange*y) >> 11;
		pSrcPix = (myRGB *)((BYTE *)pSrc +srcP_W*ys);
		for(x=0;x<widthDest;x++){
			xs = (xrange*x) >>11;
			pDest[x].bRed   = pSrcPix[xs].bRed;
			pDest[x].bGreen = pSrcPix[xs].bGreen;
			pDest[x].bBlue  = pSrcPix[xs].bBlue;
		}
		pDest = (myRGB *)((BYTE *)pDest +destP_W);
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
	for(i=0;i<(type+2);i++){
		partsRect[i] = p_rect[i];
	}
	destCBuf = dest;
	state = btn_normal;
} // MyButton::Create

BOOL MyButton::CheckState(POINT *msPoint)
{
	switch(state){
	  case btn_normal:
	  case btn_onCursor:
		if(PtInRect(&selectRect,*msPoint)){
			if(keyState.push_action){
				if(keyState.data_action){
					state = btn_push;
				}else{	
					if(0==type){
						state = btn_normal;
					}else{
						state = btn_onCursor;
					}
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
		if(PtInRect(&selectRect,*msPoint)){
			if(keyState.data_action){
				state = btn_push;
			}else{
				if(0==type){
					state = btn_normal;
				}else{
					state = btn_onCursor;
				}
				return TRUE;	
			}
		}else{
			state = btn_normal;
		}
		break;
	}
	return FALSE;
} // MyButton::CheckState

void MyButton::Draw()
{
	if(btn_nouse==state)return;
	if(btn_disable==state && type!=2)return;	
	destCBuf->BltFast(selectRect.left, selectRect.top, btnCBuf, &partsRect[state], TRUE);
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
	"evening.amp", 		
	"night.amp", 		
	"indoor.AMP",		
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
			charInf[i].colorBuf.loadLGF(pack_gparts,charInf[i].colorBuf.filename,Type_char);
			charInf[i].colorBuf.offset = pt;
			charInf[i].save.bWaitTone = FALSE;
		}
	}
} // ToneConvert::SetCharTable