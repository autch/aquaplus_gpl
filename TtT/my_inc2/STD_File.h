
 
  
   
    
     
      


#ifndef STD_FILE_H
#define STD_FILE_H

#include <windows.h>
#include <mmsystem.h>
#include <winnls32.h>
 

#define	CREATE_READ_FILE(FNAME)		CreateFile( FNAME, GENERIC_READ , FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)
#define	CREATE_WRITE_FILE(FNAME)	CreateFile( FNAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)
#define	CREATE_APEND_FILE(FNAME)	CreateFile( FNAME, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,   FILE_ATTRIBUTE_NORMAL, NULL)

#define	CREATE_READ_FILE2(FNAME,FLAG)		CreateFile( FNAME, GENERIC_READ , FILE_SHARE_READ, NULL, OPEN_EXISTING, FLAG, NULL)
#define	CREATE_WRITE_FILE2(FNAME,FLAG)		CreateFile( FNAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FLAG, NULL)
#define	CREATE_REWRITE_FILE2(FNAME,FLAG)	CreateFile( FNAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FLAG, NULL)


extern char *STD_FileAttrChange( char *dst, char *src, char *attr );
extern char *STD_GetAttr( char *src );
extern char *STD_GetFileName( char *src );
extern char *STD_GetDir( char *dst, char *src=NULL );

extern DWORD STD_GetFileSize( char *fname );
extern BOOL STD_WriteFilePos( char *fname, BYTE *src, DWORD pos, DWORD size );
extern BOOL STD_WriteFile( char *fname, BYTE *src, DWORD size );
extern BOOL STD_ApendFile( char *fname, BYTE *src, DWORD size );
extern BOOL STD_ReadFilePos( char *fname, BYTE *buf, DWORD pos, DWORD size );
extern BOOL STD_ReadFile( char *fname, BYTE *buf, DWORD size );
extern DWORD STD_ReadFileMax( char *fname, BYTE **buf );
extern BOOL STD_DeleteFile( char *fname );
extern BOOL STD_CheckFile( char *fname );

extern BOOL STD_CopyDir( char *src, char *dst, BOOL uwagaki );

typedef struct{
	char	src[1024];
	char	dst[1024];
	BOOL	uwagaki;

	int		count;
	int		countmax;
	int		err_count;

	char	fname[1024];
}COPY_DIR_DATA;

extern BOOL STD_CopyDir_First( char *src, char *dst, BOOL uwagaki, COPY_DIR_DATA *cdd );
extern BOOL STD_CopyDir_Next( COPY_DIR_DATA *cdd );

typedef struct{
	WIN32_FIND_DATA	data;
	HANDLE			handle;
	BOOL			fnf_ret;
	int				step;
	void			*next;
}WIN32_FIND_DATA_LIST;

typedef struct{
	char					dir[1024];
	char					file[256];
	BOOL					subdir_chk;
	char					find_dir[1024];
	WIN32_FIND_DATA			find_data;
	WIN32_FIND_DATA_LIST	*list;
}SEARCH_DIR_DATA;

extern BOOL InitSaerchFile( char *src, BOOL subdir_chk, SEARCH_DIR_DATA *sdd );
extern BOOL SaerchNextFile( SEARCH_DIR_DATA *sdd );
extern BOOL TermSaerchFile( SEARCH_DIR_DATA *sdd );


extern void STD_SetDefaultDir( char *str, int flag );
extern void STD_SetDefaultExt( char *str );
extern void STD_LoadDefaultDir( char *regkey );
extern void STD_SaveDefaultDir( char *regkey );
extern char *STD_GetOpenFileName( HWND hwnd, char *ff );
extern char *STD_GetSaveFileName( HWND hwnd, char *ff );

extern BOOL STD_LuckupDir( HWND hwnd, char *dir );



extern void SetIniFileName( char *dir, char *ini_fname, BOOL *exist=NULL );

extern int GetIniFileKeyList( char *key_list, int size, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int GetIniFileSectionList( char *sec_list, int size, char *ini_fname=NULL );
extern int WriteIniFileAmount( char *vname, int amount, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int ReadIniFileAmount( char *vname, int def, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int WriteIniFileData( char *vname, char *data, int size, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int ReadIniFileData( char *vname, char *data, int size, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int WriteIniFileStr( char *vname, char *str, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int ReadIniFileStr( char *vname, char *str, char *def, int size, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int DeleteIniFileKey( char *vname, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int DeleteIniFileSection( char *sec_name="DEFAULT", char *ini_fname=NULL );



extern void SetRegistryKey( char *key);
extern void SetRegistryKeyEx( char *key);
extern int WriteRegistryAmount( char *vname, long amount );
extern int ReadRegistryAmount( char *vname, long *amount );

extern int WriteRegistryData( char *vname, char *data, int size );
extern int ReadRegistryData( char *vname, char *data, int size );
extern int WriteRegistryStr( char *vname, char *str );
extern int ReadRegistryStr( char *vname, char *str );
extern int DeleteRegistry( char *vname);


#endif 