#ifndef	_MOVIE_H__
#define _MOVIE_H__

#include <vfw.h>

#define AVI_MAX		4

#define AVI_STOP	0
#define AVI_PLAY	1

extern int MOV_OpenFile( char *fname, int w, int h, int bit );
extern BOOL MOV_CloseFile( int mov_no );

extern BOOL MOV_Play( int mov_no );
extern BOOL MOV_RePlay( int mov_no );
extern BOOL MOV_Stop( int mov_no );

extern BOOL MOV_RenewMovie( int mov_no );
extern BOOL MOV_RenewMovieAudio( int ano, int audioSize, char *audioBuf );


extern int MOV_SetVideoBufFormat( int mov_no, int w, int h, int bit, BITMAPINFOHEADER *bi );

extern int MOV_GetVideoBufFormat( int mov_no, BITMAPINFOHEADER *bi );
extern DWORD MOV_GetVideoBuf( int mov_no, void **buf, int *w, int *h, int *bit );
extern int MOV_GetMode( int mov_no );
extern int MOV_GetNowFrameVideo( int mov_no );
extern int MOV_GetMaxFrameVideo( int mov_no );
extern int MOV_GetNowFrameAudio( int mov_no );
extern int MOV_GetMaxFrameAudio( int mov_no );


#endif	

