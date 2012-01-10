#ifndef _MY_MOVIE
#define _MY_MOVIE

#include <ddraw.h>
#include "F:\XVid_Proj\XVidDec\bin\XviD_dec.h"

class CMovie
{
	LPDIRECTDRAW7			lpdd;
	LPDIRECTDRAWSURFACE7	lpPrimary;
	LPDIRECTDRAWSURFACE7	lpSrcSurface;
	LPDIRECTDRAWCLIPPER		lpClipper;
	DWORD					colorMatch;

	LPBYTE		readBuf;
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
	BYTE		bpp;
	DWORD		fpms;
	int			musicNum;

	DWORD		start_time;
	DWORD		old_time;
	DWORD		pauseTime;
	int			skip_cnt;
	int			total_skip_cnt;
	int			stream;
	XviDDec		xvidDec;
	BOOL		bPlay;
	BOOL		bLoop;

public:
	HANDLE		hMovieThread;
	BOOL		bExitMovieThread;

	CMovie();
	~CMovie();

	BOOL CreateSurface();
	BOOL OpenMovie(char *fname,int music,BOOL bLoop=FALSE);
	BOOL DecodeMovie();
	BOOL DrawMovie();
	BOOL DrawSurface(BYTE bpp);
	void Release();
	void PauseMovie();
	void StopMovie();
	BOOL IsPlay();
	void WaitThreadEnd();
};

extern CMovie	*lpMovie;

#endif _MY_MOVIE
