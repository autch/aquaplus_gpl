#include "ClCommon.h"
#include "movie.h"
#include <mmsystem.h>
#include <Aviriff.h>
#include <io.h>
#include "graphic.h"
#include "soundDS.h"
#include "keyInput.h"

CMovie	*lpMovie = NULL;

#define INIT_DIRECTDRAW_STRUCT(x) (ZeroMemory(&x, sizeof(x)), x.dwSize=sizeof(x))
DWORD DDColorMatch(IDirectDrawSurface7 * pdds, COLORREF rgb);
DWORD WINAPI movieThread(LPVOID param);

DDPIXELFORMAT g_ddpfOverlayFormats[] = {
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','V','1','2'),0,0,0,0,0},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','U','Y','2'),0,0,0,0,0},
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('U','Y','V','Y'),0,0,0,0,0},
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 32,  0xff0000, 0x0ff00, 0x00ff, 0},
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 24,  0xff0000, 0x0ff00, 0x00ff, 0},
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0x7C00, 0x03e0, 0x001F, 0},
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0xF800, 0x07e0, 0x001F, 0},
};
int	overlay_forcc[] = {
	XVID_CSP_YV12,XVID_CSP_YUY2,XVID_CSP_UYVY,
	XVID_CSP_BGRA,XVID_CSP_BGR,XVID_CSP_RGB555,XVID_CSP_RGB565,		
	XVID_CSP_BGR};										

CMovie::CMovie()
{
	ZeroMemory(this,sizeof(CMovie));
	lpdd = NULL;
	lpPrimary = NULL;
	lpSrcSurface = NULL;
	readBuf = NULL;
	stream = -1;
	srcWidth = -1;
	srcHeight = -1;
	wWidth = 800;
	wHeight = 600;
}

CMovie::~CMovie()
{
	WaitThreadEnd();
	Release();
}

void CMovie::Release()
{
	if(stream>=0){
		readFile->StreamCloseFile(pack_sound,stream);
		stream = -1;
	}
	if(readBuf){
		free(readBuf);
		readBuf = NULL;
	}
	if(lpSrcSurface){
		if(forccNum<3)lpSrcSurface->UpdateOverlay(NULL, lpPrimary, NULL, DDOVER_HIDE, NULL);
		RELEASE(lpSrcSurface);
	}
	RELEASE(lpClipper);
	RELEASE(lpPrimary);
	RELEASE(lpdd);
} // CMovie::Release

struct Chunk{
	FOURCC	ckID;
	DWORD	size;
	FOURCC	type;
};

BOOL CMovie::CreateSurface()
{
	HRESULT		hr;
	DDSURFACEDESC2			ddsd;

	if(NULL==lpdd){
		DirectDrawCreateEx( NULL, (void **)&lpdd, IID_IDirectDraw7, NULL);
		hr = lpdd->SetCooperativeLevel( sysInf.hWnd, DDSCL_NORMAL );
	}
	if(NULL==lpPrimary){
		INIT_DIRECTDRAW_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		hr = lpdd->CreateSurface(&ddsd, &lpPrimary, NULL );
		lpdd->CreateClipper(0,&lpClipper,NULL);
		lpClipper->SetHWnd(0,sysInf.hWnd);
		lpPrimary->SetClipper(lpClipper);
	}
	colorMatch = DDColorMatch(lpPrimary,RGB(0, 0, 8));
	if(lpSrcSurface){
		if(forccNum<3)lpSrcSurface->UpdateOverlay(NULL, lpPrimary, NULL, DDOVER_HIDE, NULL);
		RELEASE(lpSrcSurface);
	}
	INIT_DIRECTDRAW_STRUCT(ddsd);
    ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY;
    ddsd.dwFlags= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	ddsd.dwWidth=srcWidth;
	ddsd.dwHeight=srcHeight;
	ddsd.dwBackBufferCount=0;
	for(forccNum=0;forccNum<7;forccNum++){
		if(forccNum==3){
			ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
		}
		ddsd.ddpfPixelFormat = g_ddpfOverlayFormats[forccNum];
		if(forccNum>=3){
			DDSURFACEDESC2 desc;
			desc.dwSize = sizeof(desc);
			desc.dwFlags = DDSD_PIXELFORMAT;
			hr = lpPrimary->GetSurfaceDesc(&desc);
			if(memcmp(&desc.ddpfPixelFormat, &ddsd.ddpfPixelFormat, sizeof(DDPIXELFORMAT))!=0)continue;
		}
		hr = lpdd->CreateSurface(&ddsd, &lpSrcSurface, NULL);
		if(hr==DD_OK)break;
	}
	if(forccNum==7)return FALSE;
	return TRUE;
} // CMovie::CreateSurface

BOOL CMovie::OpenMovie(char *fname,int music,BOOL loop)
{
	int			size;
	Chunk		aviFileHead;
	Chunk		aviInfo;
	AVIMAINHEADER aviMainHead;

	WaitThreadEnd();
	stream = readFile->StreamOpenFile(pack_sound,fname,size);
	if((-1)==stream) return FALSE;
	readFile->StreamReadFile(pack_sound,stream,(char *)&aviFileHead,sizeof(Chunk));
	if(MAKEFOURCC('R','I','F','F')!=aviFileHead.ckID){
		readFile->StreamCloseFile(pack_sound,stream);
		stream = -1;
		return FALSE;
	}
	readFile->StreamReadFile(pack_sound,stream,(char *)&aviInfo,sizeof(Chunk));
	readFile->StreamReadFile(pack_sound,stream,(char *)&aviMainHead,sizeof(AVIMAINHEADER));
	readFile->StreamSeekFile(pack_sound,stream,0,FILE_BEGIN);
	fpms = aviMainHead.dwMicroSecPerFrame;

	srcWidth = aviMainHead.dwWidth;
	srcHeight = aviMainHead.dwHeight;

	HDC hDC = ::GetDC(sysInf.hWnd);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 8));
	HBRUSH old_brush = (HBRUSH)SelectObject(hDC,brush);
	PatBlt(hDC,0,0,wWidth,wHeight,PATCOPY);
	SelectObject(hDC,old_brush);
	DeleteObject(brush);
	::ReleaseDC(sysInf.hWnd,hDC);

	if(FALSE == CreateSurface() ){
	}
	buffer_size = srcWidth*srcHeight*2;
	readBuf = (LPBYTE)malloc(buffer_size);

	xvidDec.Start_XviD(srcWidth,srcHeight,overlay_forcc[forccNum]);

	read_size = readFile->StreamReadFile(pack_sound,stream,(char *)readBuf,buffer_size);
	frame_cnt = 0;
	old_frame = 0;
	start_time = 0;
	old_time = 0;
	skip_cnt = 0;
	total_skip_cnt = 0;
	bPause = FALSE;
	if(NULL==lpSoundDS)lpSoundDS = new ClSoundDS(sysInf.hWnd,readFile,FALSE);
	bPlay = TRUE;
	bLoop = loop;
	changeExecMode( movie_mode );
	musicNum = music;

	DWORD	dwThreadID;
	hMovieThread = CreateThread(NULL,0,movieThread,this,0,&dwThreadID);
	bExitMovieThread = FALSE;

	return TRUE;
} // CMovie::OpenMovie


BOOL CMovie::DecodeMovie()
{
	BOOL	bSkip = FALSE,bLost = FALSE;
	HRESULT hr;
	int		ret = FALSE;

	if((-1)==stream)return FALSE;
	if(bPause)return FALSE;

	if(skip_cnt){
		xvidDec.SetSkip(TRUE);
		bSkip = TRUE;
	}
REDRAW:
	if(lpSrcSurface){	
		DDSURFACEDESC2      ddsd;
		INIT_DIRECTDRAW_STRUCT(ddsd);

		if(DD_OK != (hr=lpPrimary->IsLost())){
			bLost = TRUE;
		}else{
			hr =lpSrcSurface->Lock( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL);
		}
		if(DD_OK!=hr || bLost){
			xvidDec.SetSkip(TRUE);
			bSkip = TRUE;
		}
		ret = xvidDec.DecodeXviD(readBuf,read_size,&rest,(LPBYTE)ddsd.lpSurface,ddsd.lPitch);
		if(DD_OK==hr) lpSrcSurface->Unlock(NULL);
	}else{
		ret = xvidDec.DecodeXviD(readBuf,read_size,&rest,g_DibInf.colorBuf.pBuf,WIN_SIZEX*3);
	}

	xvidDec.SetSkip(FALSE);
	switch(ret){
	  case XviD_DEC_NEEDMORE:
		read_size = readFile->StreamReadFile(pack_sound,stream,(char *)readBuf+rest,buffer_size-rest);
		break;
	  case XviD_DEC_END:
		if(bLoop){
			xvidDec.SetInit();
			readFile->StreamSeekFile(pack_sound,stream,0,FILE_BEGIN);
			read_size = readFile->StreamReadFile(pack_sound,stream,(char *)readBuf,buffer_size);
			goto REDRAW;
		}else{
			xvidDec.Clear_XviD();
			bPlay = FALSE;
			bExitMovieThread = TRUE;
			return TRUE;
		}
		break;
	}
	DWORD now_time = timeGetTime();
	DWORD play_time = DWORD(start_time+frame_cnt*fpms/1000);
	if(now_time<play_time && 0==skip_cnt){
		Sleep(play_time- now_time);
	}else if(frame_cnt>0 && now_time>play_time+fpms/1000){
		myOutputDebugString("skip%d\n",total_skip_cnt++);
		skip_cnt ++;
		if(skip_cnt >= 20){
			skip_cnt = 0;
		}
	}else{
		skip_cnt = 0;
	}
	if(bSkip){
		frame_cnt ++;
		return FALSE;
	}
	DrawMovie();
	return TRUE;
} // CMovie::DecodeMovie

BOOL CMovie::DrawMovie()
{
	char	time_str[16];
	HDC		hDC = NULL;

	if(0==frame_cnt && musicNum>=0){
		int repeat = 1;
		if(bLoop)repeat = 0;
		lpSoundDS->ChangeMusic(bgmHandle,pack_bgmfile,musicNum,FALSE,repeat);
	}
	hDC = ::GetDC(sysInf.hWnd);
	if(lpSrcSurface){	
		DrawSurface(GetDeviceCaps(hDC,BITSPIXEL));
	}else{
		if(srcWidth ==wWidth && srcHeight==wHeight){
			BitBlt(hDC,0,0,srcWidth,srcHeight,g_DibInf.memDC,0,0,SRCCOPY);
		}else{
			StretchBlt(hDC,0,0,wWidth,wHeight,g_DibInf.memDC,0,0,srcWidth,srcHeight,SRCCOPY);
		}
	}
	if(0==frame_cnt){
		start_time = timeGetTime();
	}
	frame_cnt ++;

	::ReleaseDC(sysInf.hWnd,hDC);
	return TRUE;
} // CMovie::DrawMovie()

DWORD DDColorMatch(IDirectDrawSurface7 * pdds, COLORREF rgb)
{
    COLORREF                rgbT;
    HDC                     hdc;
    DWORD                   dw = CLR_INVALID;
    DDSURFACEDESC2          ddsd;
    HRESULT                 hres;

    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);
        SetPixel(hdc, 0, 0, rgb);
        pdds->ReleaseDC(hdc);
    }
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
        ;
    if (hres == DD_OK)
    {
        dw = *(DWORD *) ddsd.lpSurface;

        if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
            dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;
        pdds->Unlock(NULL);
    }
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }
    return dw;
} // DDColorMatch

BOOL CMovie::DrawSurface(BYTE bpp)
{
	HRESULT         ddrval;
	RECT            rs, rd;
	DDOVERLAYFX     ovfx;
	DDCAPS          capsDrv;
	unsigned int    uStretchFactor1000;
	unsigned int    uDestSizeAlign, uSrcSizeAlign;
	DWORD           dwUpdateFlags;
	POINT			myPoint;

	if(!lpdd || !lpPrimary || !lpSrcSurface)
		return FALSE;

	rd.left = rd.top = 0;
	rd.right = WIN_SIZEX;
	rd.bottom = WIN_SIZEY;
	myPoint.x = myPoint.y = 0;
	ClientToScreen(sysInf.hWnd,&myPoint);
	rs.left=0; rs.top=0;
	rs.right = srcWidth;
	rs.bottom = srcHeight;

	int maxX = GetSystemMetrics(SM_CXSCREEN);
	int maxY = GetSystemMetrics(SM_CYSCREEN);

	rd.right  += (myPoint.x - rd.left);
	rd.bottom += (myPoint.y - rd.top);
	rd.left = myPoint.x;
	rd.top = myPoint.y;

	if(rd.left < 0)rd.left = 0;
	if(rd.top < 0) rd.top = 0;
	if(rd.right > maxX)  rd.right = maxX;
	if(rd.bottom > maxY) rd.bottom = maxY;

	INIT_DIRECTDRAW_STRUCT(capsDrv);
	ddrval = lpdd->GetCaps(&capsDrv, NULL);
	if (FAILED(ddrval))
		return FALSE;

	uDestSizeAlign = capsDrv.dwAlignSizeDest;
	uSrcSizeAlign =  capsDrv.dwAlignSizeSrc;
	if (capsDrv.dwCaps & DDCAPS_ALIGNSIZESRC && uSrcSizeAlign)
		rs.right -= rs.right % uSrcSizeAlign;

	if (capsDrv.dwCaps & DDCAPS_ALIGNSIZEDEST && uDestSizeAlign)
		rd.right = (int)((rd.right+uDestSizeAlign-1)/uDestSizeAlign)*uDestSizeAlign;

	if(forccNum<3){
		dwUpdateFlags = DDOVER_SHOW | DDOVER_DDFX;
		INIT_DIRECTDRAW_STRUCT(ovfx);
		if (bpp>=16 && (capsDrv.dwCKeyCaps & DDCKEYCAPS_DESTOVERLAY)){
			dwUpdateFlags |= DDOVER_KEYDESTOVERRIDE;
			ovfx.dckDestColorkey.dwColorSpaceLowValue  = colorMatch;
			ovfx.dckDestColorkey.dwColorSpaceHighValue = colorMatch;
		}
		ddrval = lpSrcSurface->UpdateOverlay(&rs, lpPrimary, &rd, dwUpdateFlags, &ovfx);
	}else{
		ddrval = lpPrimary->Blt(&rd,lpSrcSurface,&rs,0,NULL);
	}
	if(ddrval==DDERR_SURFACELOST){
		lpPrimary->Restore();
		lpSrcSurface->Restore();
	}
	if(FAILED(ddrval))
		return FALSE;
	return TRUE;
} // CMovie::DrawSurface

void CMovie::PauseMovie()
{
	if((-1)==stream)return;
	if(!bPause){
		bPause = TRUE;
		pauseTime = timeGetTime();
	}else{
		bPause = FALSE;
		DWORD time = timeGetTime() -pauseTime;
		start_time += time;
		old_time += time;
	}
}

BOOL CMovie::IsPlay()
{
	if(keyState.push_action || keyState.push_cancel){
		if(musicNum>=0)lpSoundDS->FadeOut(bgmHandle);
		WaitThreadEnd();
		xvidDec.Clear_XviD();
		return FALSE;
	}
	return bPlay;
}

void CMovie::StopMovie()
{
	if(musicNum>=0)lpSoundDS->FadeOut(bgmHandle);
	WaitThreadEnd();
	xvidDec.Clear_XviD();
	bPlay = FALSE;
}

void CMovie::WaitThreadEnd()
{
	if(FALSE==hMovieThread)return;
	bExitMovieThread = TRUE;
	while(hMovieThread){
		Sleep(1);
	}
} // CMovie::WaitThreadEnd


DWORD WINAPI movieThread(LPVOID param)
{
	CMovie *lpMov = (CMovie *)param;

	while(FALSE==lpMov->bExitMovieThread){
		lpMov->DecodeMovie();
	}
	lpMov->Release();
	CloseHandle(lpMov->hMovieThread);
	lpMov->hMovieThread = NULL;
	ExitThread(TRUE);
	return 0L;
} // movieThread
