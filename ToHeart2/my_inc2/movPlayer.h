#ifndef  _MY_MOVPLAYER
#define  _MY_MOVPLAYER

#include <dshow.h>

class MovPlayerWnd
{
	IGraphBuilder *lpGB;
	IMediaControl *lpMC;
	IMediaEventEx *lpME;
	IVideoWindow  *lpVW;
	IBasicAudio   *lpBA;
public:
	MovPlayerWnd(){
		lpGB = NULL;
		lpMC = NULL;
		lpME = NULL;
		lpVW = NULL;
		lpBA = NULL;
	}
	~MovPlayerWnd(){
		Release();
	}
	BOOL PlayCheck(void);
	BOOL PlayMediaFile( char *file, HWND hWnd );
	void Release(void);
};

extern MovPlayerWnd	*movPlayerWnd;
extern BOOL			MovPlaying;













#include "XviD_Dec.h"
#include <ddraw.h>

#include <stdio.h>

#include "Comp_pac.h"

class CMovPlayerFrm
{

public:
	CMovPlayerFrm();
	virtual ~CMovPlayerFrm();

	
	LPDIRECTDRAW7			lpdd;
	LPDIRECTDRAWSURFACE7	lpPrimary;
	LPDIRECTDRAWSURFACE7	lpSrcSurface;
	LPDIRECTDRAWCLIPPER		lpClipper;
	DWORD					colorMatch;
	
	HDC			memDC;
	HBITMAP		hBmp;
	HBITMAP		oldBmp;
	LPBYTE		pBuffer;

	HWND		m_hWnd;

	LPBYTE		readBuf;

	int			wX;
	int			wY;
	int			wWidth;
	int			wHeight;
	int			srcWidth;
	int			srcHeight;
	int			buffer_size;
	int			read_size;
	int			rest;
	int			forccNum;
	int			frame_cnt;
	int			old_frame;
	BOOL		bPause;
	BOOL		bEnd;
	BYTE		bpp;
	DWORD		fpms;

	DWORD		start_time;
	DWORD		old_time;
	DWORD		pauseTime;
	int			skip_cnt;
	int			total_skip_cnt;

	HANDLE			fh;
	OPEN_FILE_INFO	ofi;

	XviDDec		xvidDec;

	BOOL CreateSurface();
	BOOL OpenMovie( HWND hWnd, char *fname, int x, int y, int w, int h);
	BOOL DecodeMovie();
	BOOL DrawMovie();
	BOOL DrawSurface(BYTE bpp);
	void Release();

	void OnPaint();
};

extern CMovPlayerFrm	*movPlayerFrm;
extern BOOL			MovPlaying2;


extern int		OverLay;

#define		RELEASE(x)		if((x) != NULL){(x)->Release(); (x) = NULL;}





#endif