#ifndef _BMPPACMAIN_H_
#define _BMPPACMAIN_H_

#include "bmp.h"

typedef struct{
	DWORD	head;	
	char	bit;	
	char	frame;	
	char	sabun;	
	char	yoyaku;	
	long	sx;		
	long	sy;		
}BMP_MOVE_INFO;

#define MOVIE_MAX	32

enum{
	MOVE_COND_STOP,		
	MOVE_COND_PLAY,		
	MOVE_COND_PAUSE,	
	MOVE_COND_END,		
};

extern void PAC_PackFileMovie_Init( void );
extern void PAC_PackFileMovie_Term( void );


extern int  PAC_PackFileMovie_Open( char *PackFname );
extern BOOL PAC_PackFileMovie_Close( int move_no );
extern void PAC_PackFileMovie_GetSize( int move_no, int *sx, int *sy );
extern BOOL PAC_PackFileMovie_GetSabun( int move_no );
extern BOOL PAC_PackFileMovie_GetCondition( int move_no );
extern int  PAC_PackFileMovie_GetLoop( int move_no );
extern BOOL PAC_PackFileMovie_Start( int move_no, int cond, int loop );
extern BOOL PAC_PackFileMovie_Loop( int move_no, int loop );
extern BOOL PAC_PackFileMovie_Play( int move_no, int loop );
extern BOOL PAC_PackFileMovie_Pause( int move_no );
extern void PAC_PackFileMovie_Bright( int move_no, int r, int g, int b );
extern BOOL PAC_PackFileMovie_Read( int move_no, void *dest, int draw_mode, int bai, int frame );

extern int  LZS_DecodeMemoryBmpTbl_DWORD( RGB32 *dst, BYTE *src, int file_size, int sabun, int bai, int r, int g, int b );
extern int  LZS_DecodeMemoryBmpTbl_3BYTE( BYTE *dst, BYTE *src, int file_size, int sabun, int bai, int r, int g, int b );
extern int  LZS_DecodeMemoryBmpTbl_WORD(  WORD *dst, BYTE *src, int file_size, int sabun, int bai, int r, int g, int b );
extern int  LZS_DecodeMemoryBmpTbl_BYTE( BYTE *dst, RGB32 *tbl, BYTE *src, int file_size, int sabun );


#endif