#ifndef _MY_MOVIE
#define _MY_MOVIE


#include <ddraw.h>
#include "XviD_dec.h"

struct Chunk{
	FOURCC	ckID;
	DWORD	size;
	FOURCC	type;
};

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
	int			frame_cnt;
	int			old_frame;
	RECT		dstRect;
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
	BOOL		bBreak;	
	BOOL		bNoUseOverlay;

public:
	HANDLE		hMovieThread;
	BOOL		bExitMovieThread;
	HBRUSH		brush;
	int			forccNum;

	CMovie(BOOL bNoOverlay=FALSE);
	~CMovie();

	BOOL CreateSurface();
	BOOL OpenMovie(char *fname,int music,BOOL bBreak=FALSE,BOOL bLoop=FALSE);
	BOOL DecodeMovie();
	BOOL DrawMovie();
	BOOL DrawSurface(BYTE bpp);
	void SetDestRect(RECT *lpRect){ dstRect = *lpRect; };
	void Release();
	void PauseMovie();
	void StopMovie();
	BOOL IsPlay();
	void WaitThreadEnd();
	void RestoreDevice();
};

extern CMovie	*lpMovie;

#endif _MY_MOVIE
