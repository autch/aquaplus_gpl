



#include "movPlayer.h"
#include "mm_std.h"
#include <dshow.h>








#pragma comment ( lib, "strmiids.lib" )

MovPlayerWnd	*movPlayerWnd = NULL;
BOOL			MovPlaying=FALSE;

int				DecodeFlag=0;
int				OverLay=1;





void MovPlayerWnd::Release()
{
	if(lpBA){ 
		lpBA->put_Volume( -10000);
		Sleep(100);	
	}
	if(lpMC){
		lpMC->Stop();
	}
	if ( lpBA != NULL ) { lpBA->Release(); lpBA = NULL; }
	if ( lpVW != NULL ) { lpVW->Release(); lpVW = NULL; }
	if ( lpME != NULL ) { lpME->Release(); lpME = NULL; }
	if ( lpMC != NULL ) { lpMC->Release(); lpMC = NULL; }
	if ( lpGB != NULL ) { lpGB->Release(); lpGB = NULL; }
	MovPlaying=FALSE;
}

BOOL MovPlayerWnd::PlayCheck()
{
	long lEventCode;
	long lParam1;
	long lParam2;
	
	lpME->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
	if (EC_COMPLETE==lEventCode) {
		MovPlaying=FALSE;
		return TRUE;
	}
	return FALSE;
}
#define WM_GRAPHNOTIFY WM_APP+1





BOOL MovPlayerWnd::PlayMediaFile( char *file, HWND hWnd )
{
	HRESULT hr;

	
	Release();

	
	
	
	hr = CoCreateInstance(
				 CLSID_FilterGraph,
				 NULL,
				 CLSCTX_INPROC_SERVER,
				 IID_IGraphBuilder,
				 (void **)&lpGB
			 );
	if FAILED( hr ) return false;

	
	
	
	WCHAR wFile[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, file, -1, wFile, MAX_PATH );  

	hr = lpGB->RenderFile( wFile, NULL );
	if FAILED( hr ) return false;

	
	
	
	hr = lpGB->QueryInterface( IID_IMediaControl, (void **)&lpMC );
	hr = lpGB->QueryInterface( IID_IMediaEventEx, (void **)&lpME );
	hr = lpGB->QueryInterface( IID_IBasicAudio, (void **)&lpBA );

	
	
	
	hr = lpGB->QueryInterface( IID_IVideoWindow, (void **)&lpVW );
	if FAILED( hr ) return false;

	
	
	
hr = (lpME->SetNotifyWindow( (OAHWND)hWnd, WM_GRAPHNOTIFY, 0) < 0);
if FAILED( hr ) return false;

	hr = lpVW->put_Owner( (long)hWnd );
	if FAILED( hr ) return false;
	hr = lpVW->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );
	if FAILED( hr ) return false;

	
	RECT client;
	GetClientRect( hWnd, &client );
	hr = lpVW->SetWindowPosition( client.left, client.top+76, client.right, client.bottom-76*2 );
	if FAILED( hr ) return false;

	
	
	
	hr = lpMC->Run();
	MovPlaying=TRUE;

	
	lpVW->put_AutoShow( OATRUE );
	lpVW->put_Visible( OATRUE );


	if FAILED( hr ) return false;

	return true;
}





#include <mmsystem.h>
#include <Aviriff.h>
#include <io.h>


CMovPlayerFrm	*movPlayerFrm = NULL;
BOOL			MovPlaying2=FALSE;


#define INIT_DIRECTDRAW_STRUCT(x) (ZeroMemory(&x, sizeof(x)), x.dwSize=sizeof(x))
DWORD DDColorMatch(IDirectDrawSurface7 * pdds, COLORREF rgb);

DDPIXELFORMAT g_ddpfOverlayFormats[] = {
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','U','Y','2'),0,0,0,0,0},	
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('U','Y','V','Y'),0,0,0,0,0},	
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','V','1','2'),0,0,0,0,0},	
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 32,  0xff0000, 0x0ff00, 0x00ff, 0},	
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 24,  0xff0000, 0x0ff00, 0x00ff, 0},	
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0x7C00, 0x03e0, 0x001F, 0},		
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0xF800, 0x07e0, 0x001F, 0},		
};
int	overlay_forcc[] = {
	
	XVID_CSP_YUY2,
	XVID_CSP_UYVY,
	XVID_CSP_YV12,
	
	XVID_CSP_BGRA,
	XVID_CSP_BGR,
	XVID_CSP_RGB555,
	XVID_CSP_RGB565,
	
	
	XVID_CSP_BGR
};

CMovPlayerFrm::CMovPlayerFrm()
{
	lpdd = NULL;
	lpClipper=NULL;
	lpPrimary = NULL;
	lpSrcSurface = NULL;
	readBuf = NULL;
	fh = NULL;
	srcWidth = -1;
	srcHeight = -1;
	wWidth = 800;
	wHeight = 600;
	wX = 0;
	wY = 0;
	DecodeFlag=0;
}
CMovPlayerFrm::~CMovPlayerFrm()
{
}

void CMovPlayerFrm::Release()
{
	DecodeFlag=0;
	PAC_OpenFileHandle( &fh );
	if(readBuf){
		free(readBuf);
		readBuf = NULL;
	}
	if(memDC){
		SelectObject(memDC,oldBmp);
		DeleteObject(hBmp);
		DeleteDC(memDC);
		memDC = NULL;
		hBmp = NULL;
	}

	if(lpSrcSurface){
		if(forccNum<3)lpSrcSurface->UpdateOverlay(NULL, lpPrimary, NULL, DDOVER_HIDE, NULL);
		lpSrcSurface->Release();
	}
	if(lpClipper) lpClipper->Release();
	if(lpPrimary)lpPrimary->Release();
	if(lpdd)lpdd->Release();
} 


void CMovPlayerFrm::OnPaint()
{


	if(NULL==lpPrimary)return;
	if(DD_OK == lpPrimary->IsLost()){
		
		if(forccNum<3){
			HDC hDC = ::GetDC(m_hWnd);
			HBRUSH brush = CreateSolidBrush(RGB(0, 0, 8));
		
			HBRUSH old_brush = (HBRUSH)SelectObject(hDC,brush);
			PatBlt(hDC,0,0, 800, 600, PATCOPY);
			SelectObject(hDC,old_brush);

			DrawSurface(GetDeviceCaps(hDC,BITSPIXEL));

			DeleteObject(brush);
			::ReleaseDC(m_hWnd,hDC);
		}
	}else{
		lpPrimary->Restore();
		if(lpSrcSurface)lpSrcSurface->Restore();
	}
} 


struct Chunk{
	FOURCC	ckID;
	DWORD	size;
	FOURCC	type;
};

BOOL CMovPlayerFrm::CreateSurface( )
{
	HRESULT		hr;
	DDSURFACEDESC2			ddsd;

	if(NULL==lpdd){
		DirectDrawCreateEx( NULL, (void **)&lpdd, IID_IDirectDraw7, NULL);
		
		hr = lpdd->SetCooperativeLevel( m_hWnd, DDSCL_NORMAL );
	}
	if(NULL==lpPrimary){
		INIT_DIRECTDRAW_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		hr = lpdd->CreateSurface(&ddsd, &lpPrimary, NULL );
	
		lpdd->CreateClipper(0,&lpClipper,NULL);
		lpClipper->SetHWnd(0,m_hWnd);
		lpPrimary->SetClipper(lpClipper);
	}
	colorMatch = DDColorMatch(lpPrimary,RGB(0, 0, 8));


	if(lpSrcSurface){
		if(forccNum<3)lpSrcSurface->UpdateOverlay(NULL, lpPrimary, NULL, DDOVER_HIDE, NULL);
		lpSrcSurface->Release();
	}
	INIT_DIRECTDRAW_STRUCT(ddsd);
	ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY;
	ddsd.dwFlags= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	
	ddsd.dwWidth=srcWidth;
	ddsd.dwHeight=srcHeight;
	ddsd.dwBackBufferCount=0;


	
	if(OverLay){
		forccNum=0;
	}else{
		forccNum=3;
	}

	for(;forccNum<7;forccNum++){
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
		if(hr==DD_OK){
			











			break;
		}
	}
	if(forccNum>=3){
		DebugPrintf( "オーバーレイサーフェスの作成に失敗しました。ムービーの再生速度が低下します[%d]", forccNum );
	}
	if(forccNum==7)return FALSE;
	return TRUE;
} 



BOOL CMovPlayerFrm::OpenMovie( HWND hWnd, char *fname, int x, int y, int w, int h )
{

	m_hWnd = hWnd;

	wX = x;
	wY = y;

	wWidth  = w;
	wHeight = h;

	
	ofi = PAC_OpenFileHandle( "mov", fname, &fh );
	if(fh==NULL)DebugBox( NULL, "ファイルが見つかりませんでした。(%s)", fname);







	Chunk		aviFileHead;
	Chunk		aviInfo;
	AVIMAINHEADER aviMainHead;

	PAC_ReedFileHandle( &fh, &ofi, (BYTE*)&aviFileHead, sizeof(Chunk) );			
	if(MAKEFOURCC('R','I','F','F')!=aviFileHead.ckID){
		PAC_OpenFileHandle( &fh );												
		return FALSE;
	}
	PAC_ReedFileHandle( &fh, &ofi, (BYTE*)&aviInfo, sizeof(Chunk) );				
	PAC_ReedFileHandle( &fh, &ofi, (BYTE*)&aviMainHead, sizeof(AVIMAINHEADER) );	



	fpms = aviMainHead.dwMicroSecPerFrame;

	srcWidth = aviMainHead.dwWidth;
	srcHeight = aviMainHead.dwHeight;

	
	HDC hDC = ::GetDC(m_hWnd);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 8));

	HBRUSH old_brush = (HBRUSH)SelectObject(hDC,brush);
	PatBlt(hDC,0,0, 800, 600, PATCOPY);
	SelectObject(hDC,old_brush);
	DeleteObject(brush);
	::ReleaseDC(m_hWnd,hDC);

	if(FALSE == CreateSurface() ){
		BITMAPINFOHEADER	bmpInfo;
		ZeroMemory(&bmpInfo,sizeof(BITMAPINFOHEADER));
		bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.biPlanes = 1;
		bmpInfo.biBitCount = 24;
		bmpInfo.biCompression = BI_RGB;
		bmpInfo.biWidth = srcWidth;
		bmpInfo.biHeight = -srcHeight;
		bmpInfo.biSizeImage = srcWidth * srcHeight *3;
		hDC = ::GetDC(m_hWnd);
		memDC = CreateCompatibleDC(hDC);
		hBmp = CreateDIBSection(hDC,(BITMAPINFO *)&bmpInfo,DIB_RGB_COLORS,(void **)&pBuffer,NULL,NULL);
		oldBmp = (HBITMAP)SelectObject(memDC,hBmp);
		::ReleaseDC(m_hWnd,hDC);
	}
	buffer_size = srcWidth*srcHeight*2;
	readBuf = (LPBYTE)malloc(buffer_size);

	xvidDec.Start_XviD(srcWidth,srcHeight,overlay_forcc[forccNum]);
	read_size = PAC_ReedFileHandle( &fh, &ofi, readBuf, buffer_size );
	frame_cnt = 0;
	old_frame = 0;
	start_time = 0;
	old_time = 0;
	skip_cnt = 0;
	total_skip_cnt = 0;
	bPause = FALSE;
	bEnd = FALSE;

	MovPlaying2 = TRUE;

	return TRUE;
} 


BOOL CMovPlayerFrm::DecodeMovie()
{
	BOOL	bSkip = FALSE,bLost = FALSE;
	HRESULT hr;
	int		ret = FALSE;


	if(bPause)return FALSE;
	if(bEnd)return FALSE;

	if(skip_cnt){
		xvidDec.SetSkip(TRUE);
		bSkip = TRUE;
	}
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
		DecodeFlag=1;
		ret = xvidDec.DecodeXviD(readBuf,read_size,&rest,(LPBYTE)ddsd.lpSurface,ddsd.lPitch);


		if(DD_OK==hr)
			lpSrcSurface->Unlock(NULL);




	}else{
		ret = xvidDec.DecodeXviD(readBuf,read_size,&rest,pBuffer,srcWidth*3);
	}
	xvidDec.SetSkip(FALSE);
	switch(ret){
	  case XviD_DEC_NEEDMORE:

		read_size = PAC_ReedFileHandle( &fh, &ofi, readBuf +rest, buffer_size-rest );

		break;
	  case XviD_DEC_END:
		xvidDec.Clear_XviD();
		bEnd=TRUE;
		return FALSE;
	}

	DWORD now_time = timeGetTime();
	DWORD play_time = DWORD(start_time+frame_cnt*fpms/1000);
	if(now_time<play_time && 0==skip_cnt){
		
		Sleep(play_time- now_time);
	}else if(frame_cnt>0 && now_time>play_time+fpms/1000){
		DebugPrintf( "skip%d\n", total_skip_cnt++ );
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
	return TRUE;
} 


BOOL CMovPlayerFrm::DrawMovie()
{
	char	time_str[16];
	HDC		hDC = NULL;




	hDC = ::GetDC(m_hWnd);
	if(lpSrcSurface){	
		DrawSurface(GetDeviceCaps(hDC,BITSPIXEL));
	}else{
		if(srcWidth ==wWidth && srcHeight==wHeight){
			BitBlt(hDC,0,0,srcWidth,srcHeight,memDC,0,0,SRCCOPY);
		}else{
			StretchBlt(hDC,0,0,wWidth,wHeight,memDC,0,0,srcWidth,srcHeight,SRCCOPY);
		}
	}
	if(0==frame_cnt){
		start_time = timeGetTime();
	}
	frame_cnt ++;

	DWORD now_time = timeGetTime();
	if(now_time > old_time){
		int fps = 1000/(now_time-old_time)/(frame_cnt-old_frame);


	}
	::ReleaseDC(m_hWnd,hDC);
	old_time = now_time;
	old_frame = frame_cnt;
	return TRUE;
} 



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
} 








BOOL CMovPlayerFrm::DrawSurface(BYTE bpp)
{
	HRESULT         ddrval;
	RECT            rs, rd;
	POINT			pos;
	DDOVERLAYFX     ovfx;
	DDCAPS          capsDrv;
	unsigned int    uDestSizeAlign, uSrcSizeAlign;
	DWORD           dwUpdateFlags;

	if(!lpdd || !lpPrimary || !lpSrcSurface)
		return FALSE;
	if(DecodeFlag==0){
		return FALSE;
	}
	

	int			mx = GetSystemMetrics(SM_CXDLGFRAME);
	int			my = GetSystemMetrics(SM_CYDLGFRAME)+GetSystemMetrics(SM_CYSIZE)
					+ GetSystemMetrics(SM_CYBORDER)*2+GetSystemMetrics(SM_CYMENUSIZE);

	GetWindowRect( m_hWnd, &rd );
	rd.top  += wY+my;
	rd.left += wX+mx;

	rd.bottom = rd.top  + wHeight;
	rd.right  = rd.left + wWidth;


	rs.left=0; rs.top=0;
	rs.right = srcWidth;
	rs.bottom = srcHeight;

	int maxX = GetSystemMetrics(SM_CXSCREEN);
	int maxY = GetSystemMetrics(SM_CYSCREEN);
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
} 

