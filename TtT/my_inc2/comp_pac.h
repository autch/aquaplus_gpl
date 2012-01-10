#ifndef _PACMAIN_H_
#define _PACMAIN_H_

extern DWORD PAC_CheckFile( char *pname, char *fname );
extern DWORD PAC_LoadFile( char *pname, char *fname, BYTE **buf );

extern DWORD PAC_AllFilecheck( char *pname );

extern int PAC_CheckPackFile( char *pname, char *fname );
extern int PAC_OpenPackFile( char *pname, char *fname, BYTE **buf );
extern DWORD PAC_OpenFileHandle( char *pname, char *fname, HANDLE *fh );



typedef struct {
	DWORD	type;		
	char	fname[24];	
	DWORD	offset;		
	DWORD	size;		
}FILE_INFO;

typedef struct {
	DWORD	head;		
	DWORD	famount;	
}PAC_FILE_INFO;



#endif
