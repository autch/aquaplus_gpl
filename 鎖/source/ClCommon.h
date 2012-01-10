#ifndef _LEAFLIB_COMMON
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <crtdbg.h>
#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>
#include <d3dx9.h>
#include <ddraw.h>

#define Uint8		unsigned char
#define	Sint8		signed char
#define	Uint16		unsigned short
#define	Sint16		signed short
#define	Uint32		unsigned long
#define	Sint32		signed long

enum OnOff{
	off = 0,
	on,
};

enum UpDown{
	down = 0,
	up,
};

#include "ymzutils.h"

#define FULLMODE_STYLE		(WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU | WS_VISIBLE)
#define WINDOWMODE_STYLE	(FULLMODE_STYLE | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)

#define M_PI  3.14159265359f
#define M_PIR 0.0174532925199f
#define		my_delete(a)	if(NULL!=(a)){delete (a); (a)=NULL;}
#define		my_deletes(a)	if(NULL!=(a)){delete[] (a); (a)=NULL;}
#define		RELEASE(x)		if((x) != NULL){(x)->Release(); (x) = NULL;}
#define		DEGtoRAD(x)		float((x)*M_PIR)
#define		RADtoDEG(x)		float((x)/M_PI*180)

#define		MY_FILE_SCAN	FILE_FLAG_SEQUENTIAL_SCAN

#define	ClResult			WORD
#define	Cl_OK				0		
#define	Cl_CANNOT_CREATFILE	0x0100	
#define	Cl_FILE_NOTFOUND	0x0101
#define	Cl_FILE_SEEK_ERROR	0x0102
#define	Cl_PLAY_CONTINUE	0x0200
#define Cl_CD_NOTREADY		0x0201
#define	Cl_CD_SEEK_ERROR	0x0202
#define	Cl_PCM_NOTFOUND		0x0300

#include "readfile.h"

#ifndef _DEBUG
#define cl_malloc(a)	HeapAlloc(GetProcessHeap(),0,a)
#define cl_realloc(a,b)	HeapReAlloc(GetProcessHeap(),0,a,b)
#define cl_free(a)		if(NULL!=(a)){HeapFree(GetProcessHeap(),0,a);(a)=NULL;}
#else
#define cl_malloc(a)	_malloc_dbg(a,_NORMAL_BLOCK,__FILE__,__LINE__)
#define cl_realloc(a,b)	_realloc_dbg(a,b,_NORMAL_BLOCK,__FILE__,__LINE__)
#define cl_free(a)		if(NULL!=(a)){ free(a);(a)=NULL;}
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__) 
#define malloc(p1) _malloc_dbg(p1,_NORMAL_BLOCK,__FILE__,__LINE__) 
#endif

#define _LEAFLIB_COMMON
#endif  _LEAFLIB_COMMON
