
 
  
   
    
     
      

#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include <shlobj.h>
#include <math.h>

#include <TCHAR.H>





char *STD_FileAttrChange( char *dst, char *src, char *attr )
{
	char	*ret = dst;

	if(src==NULL){
		while( *dst ){ *dst++; }
	}else{
		while( *src ){ *dst++ = *src++; }
		*dst = '\0';
	}

	while( *dst != '.' ){
		dst--;
		if( ret == dst )	return NULL;	
	}
	dst++;
	strcpy( dst, attr );

	return ret;
}




char *STD_GetAttr( char *src )
{
	char	*st = src;

	src+=strlen(src);

	while( *src!='.' && st<src ){ *src--; }

	if(st==src)	return NULL;
	else		return src+1;
}



char *STD_GetFileName( char *src )
{
	char	*st = src;

	while( *src ){ *src++; }
	while( *src!='\\' && st<src ){ *src--; }

	if(st==src)	return st;
	else		return src+1;
}



char *STD_GetDir( char *dst, char *src )
{
	char	*st = dst;

	if(dst==NULL) return NULL;
	if(src){
		strcpy(dst,src);
	}

	while( *dst ){ *dst++; }
	while( *dst!='\\' && st<dst ){ *dst--; }

	if(st==dst)	return NULL;
	*dst = '\0';
	return st;
}




DWORD STD_GetFileSize( char *fname )
{
	HANDLE		fh;
	DWORD		dummy, size;

	fh = CREATE_READ_FILE( fname );
	if ( fh == INVALID_HANDLE_VALUE ) return 0;

	size = GetFileSize( fh, &dummy );
	
	CloseHandle( fh );

	if( dummy != 0 )	size = 0;	

	return size;
}



BOOL STD_WriteFilePos( char *fname, BYTE *src, DWORD pos, DWORD size )
{
	char		*buf=NULL,*buf2=NULL;
	HANDLE		fh;
	BOOL		dummy;

	fh = CREATE_APEND_FILE( fname );
	if( fh == INVALID_HANDLE_VALUE ) return 0;

	SetFilePointer( fh, pos, NULL, FILE_BEGIN );
	WriteFile( fh, src, size, (unsigned long *)&dummy, NULL );

	CloseHandle( fh );

	return dummy;
}



BOOL STD_WriteFile( char *fname, BYTE *src, DWORD size )
{
	HANDLE		fh;
	BOOL		dummy;

	fh = CREATE_WRITE_FILE( fname );
	if( fh == INVALID_HANDLE_VALUE ) return 0;

	WriteFile( fh, src, size, (unsigned long *)&dummy, NULL );
	
	CloseHandle( fh );
	
	return dummy;
}



BOOL STD_ApendFile( char *fname, BYTE *src, DWORD size )
{
	HANDLE		fh;
	BOOL		dummy;

	fh = CREATE_APEND_FILE( fname );
	if( fh == INVALID_HANDLE_VALUE ) return 0;

	SetFilePointer( fh, 0, NULL, FILE_END );
	WriteFile( fh, src, size, (unsigned long *)&dummy, NULL );
	
	CloseHandle( fh );
	
	return dummy;
}



BOOL STD_ReadFilePos( char *fname, BYTE *buf, DWORD pos, DWORD size )
{
	HANDLE		fh;
	DWORD		dummy;

	fh = CREATE_READ_FILE( fname );
	if ( fh == INVALID_HANDLE_VALUE ) return 0;
	
	SetFilePointer( fh, pos, NULL, FILE_BEGIN );
	ReadFile( fh, buf, size, &dummy, NULL );
	
	CloseHandle( fh );
	
	return dummy;
}



BOOL STD_ReadFile( char *fname, BYTE *buf, DWORD size )
{
	HANDLE		fh;
	DWORD		dummy;

	fh = CREATE_READ_FILE( fname );
	if ( fh == INVALID_HANDLE_VALUE ) return 0;

	ReadFile( fh, buf, size, &dummy, NULL );
	
	CloseHandle( fh );
	
	return dummy;
}




DWORD STD_ReadFileMax( char *fname, BYTE **buf )
{
	HANDLE		fh;
	DWORD		dummy, size;

	fh = CREATE_READ_FILE( fname );
	if ( fh == INVALID_HANDLE_VALUE ) return 0;

	size = GetFileSize( fh, &dummy );	
	if(size && dummy==0){
		*buf = (BYTE *)GlobalAlloc( GPTR, size );
		ReadFile( fh, *buf, size, &dummy, NULL );
	}
	
	CloseHandle( fh );

	if( dummy != size ){

	}
	return size;
}



BOOL STD_DeleteFile( char *fname )
{
	HANDLE		fh;
	fh = CREATE_REWRITE_FILE2( fname, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_DELETE_ON_CLOSE );
	if( fh == INVALID_HANDLE_VALUE ) return FALSE;
	CloseHandle( fh );
	return TRUE;
}



BOOL STD_CheckFile( char *fname )
{
	HANDLE		fh;

	fh = CREATE_READ_FILE( fname );
	if( fh == INVALID_HANDLE_VALUE ) return FALSE;
	CloseHandle( fh );

	return TRUE;
}



BOOL STD_DeleteDir( char *src )
{
	char			src2[1024];
	char			src_buf[1024];
	WIN32_FIND_DATA	FindFileData;
	HANDLE			hFind;
	int				len;

	if( src==NULL ){ return FALSE; }
	if( src[0]=='\0' ){ return FALSE; }
	if( src[1]!=':' ){ return FALSE; }


	strcpy( src2, src );
	len = strlen(src2);
	if( *(src2+len-1)=='\\' ){
		*(src2+len-1) = '\0';
	}
	wsprintf( src_buf, "%s\\*.*", src2 );
	hFind = FindFirstFile( src_buf, &FindFileData );
	if( hFind == INVALID_HANDLE_VALUE ){ return FALSE; }
	FindNextFile( hFind, &FindFileData );		

	while( FindNextFile( hFind, &FindFileData ) ){
		if(FindFileData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY){
			wsprintf( src_buf, "%s\\%s", src2, FindFileData.cFileName );
			
			if( !STD_DeleteDir( src_buf ) ){

			}
		}else{
			wsprintf( src_buf, "%s\\%s", src2, FindFileData.cFileName );
			if( !STD_DeleteFile( src_buf ) ){

			}
		}
	}
	FindClose( hFind );

	return RemoveDirectory( src2 );
}







BOOL STD_CopyDir( char *src, char *dst, BOOL uwagaki )
{
	char			src_buf[1024];
	char			dst_buf[1024];
	WIN32_FIND_DATA	FindFileData;
	HANDLE			hFind;

	if( src==NULL || dst==NULL ){ return FALSE; }

	wsprintf( src_buf, "%s\\*.*", src );
	hFind = FindFirstFile( src_buf, &FindFileData );
	if( hFind == INVALID_HANDLE_VALUE ){ return FALSE; }
	FindNextFile( hFind, &FindFileData );		

	while( FindNextFile( hFind, &FindFileData ) ){
		if(FindFileData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY){
			wsprintf( src_buf, "%s\\%s", src, FindFileData.cFileName );
			wsprintf( dst_buf, "%s\\%s", dst, FindFileData.cFileName );
			
			CreateDirectory( dst_buf, NULL );
			STD_CopyDir( src_buf, dst_buf, uwagaki );
		}else{
			wsprintf( src_buf, "%s\\%s", src, FindFileData.cFileName );
			wsprintf( dst_buf, "%s\\%s", dst, FindFileData.cFileName );
			
			CopyFile( src_buf, dst_buf, uwagaki );
			DebugPrintf( "%s\n", dst_buf );
		}
	}
	return TRUE;
}

BOOL STD_CopyDir_First( char *src, char *dst, BOOL uwagaki, COPY_DIR_DATA *cdd )
{
	strcpy( cdd->src, src );
	strcpy( cdd->dst, dst );

	cdd->uwagaki = uwagaki;
	cdd->count    = 0;
	cdd->countmax = 0;
	cdd->err_count= 0;

	return TRUE;
}


static BOOL STD_CopyDir_Next2( char *src, char *dst, COPY_DIR_DATA *cdd )
{
	char			src_buf[1024];
	char			dst_buf[1024];
	WIN32_FIND_DATA	FindFileData;
	HANDLE			hFind;
	BOOL			ret;

	wsprintf( src_buf, "%s\\*.*", src );
	hFind = FindFirstFile( src_buf, &FindFileData );
	if( hFind == INVALID_HANDLE_VALUE ){ return FALSE; }
	FindNextFile( hFind, &FindFileData );		

	while( ret=FindNextFile( hFind, &FindFileData) ){
		if(FindFileData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY){
			wsprintf( src_buf, "%s\\%s", src, FindFileData.cFileName );
			wsprintf( dst_buf, "%s\\%s", dst, FindFileData.cFileName );
			
			CreateDirectory( dst_buf, NULL );
			STD_CopyDir_Next2( src_buf, dst_buf, cdd );
		}else{
			cdd->count++;
			if( cdd->count == cdd->countmax ){
				wsprintf( src_buf, "%s\\%s", src, FindFileData.cFileName );
				wsprintf( dst_buf, "%s\\%s", dst, FindFileData.cFileName );

				if(!CopyFile( src_buf, dst_buf, cdd->uwagaki ) ){
					cdd->err_count++;
				}
				wsprintf( cdd->fname, dst_buf+strlen(cdd->dst)+1 );
				DebugPrintf( "%s\n", dst_buf );
			}
		}
		if( cdd->count >= cdd->countmax ){
			break;
		}
	}
	FindClose( hFind );

	return ret;
}

BOOL STD_CopyDir_Next( COPY_DIR_DATA *cdd )
{
	BOOL	ret;

	cdd->count = 0;
	cdd->countmax++;
	ret = STD_CopyDir_Next2( cdd->src, cdd->dst, cdd );
	return ret;
}


enum{
	SEARCH_FILE_INIT,
	SEARCH_FILE,
	SEARCH_DIR_INIT,
	SEARCH_DIR,
	SEARCH_END,
};
static BOOL SaerchNextFile2( char *src_dir, WIN32_FIND_DATA_LIST **list, SEARCH_DIR_DATA *sdd )
{
	char		src_buf[1024];

	if(*list==NULL){
		*list = (WIN32_FIND_DATA_LIST *)GAlloc( sizeof(WIN32_FIND_DATA_LIST) );
		if(sdd->subdir_chk){
			(*list)->step = SEARCH_FILE_INIT;
		}else{
			(*list)->step = SEARCH_FILE_INIT;
		}
	}
	while( *list ){
		switch( (*list)->step ){
			case SEARCH_DIR_INIT:
				wsprintf( src_buf, "%s%s", src_dir, "*" );
				(*list)->handle = FindFirstFile( src_buf, &(*list)->data );
				if( (*list)->handle == INVALID_HANDLE_VALUE ){
					GFree( *list );
					return FALSE;
				}
				FindNextFile( (*list)->handle, &(*list)->data );		

				(*list)->step = SEARCH_DIR;
				
			case SEARCH_DIR:
				if( (*list)->next==NULL ){
					(*list)->fnf_ret = FindNextFile( (*list)->handle, &(*list)->data );
				}
				if( (*list)->fnf_ret ){
					if( (*list)->data.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY ){
						wsprintf( src_buf, "%s%s\\", src_dir, (*list)->data.cFileName );
						if( SaerchNextFile2( src_buf, (WIN32_FIND_DATA_LIST**)&(*list)->next, sdd ) ){
							return TRUE;
						}
					}
				}else{
//					FindClose( (*list)->handle );
//					(*list)->step = SEARCH_FILE_INIT;
					GFree( *list );
					return FALSE;
				}
				break;

			case SEARCH_FILE_INIT://直下ファイルを検索
				wsprintf( src_buf, "%s%s", src_dir, sdd->file );//ファイルを検出
				(*list)->handle = FindFirstFile( src_buf, &(*list)->data );
				if( (*list)->handle == INVALID_HANDLE_VALUE ){
					if(sdd->subdir_chk){
						(*list)->step = SEARCH_DIR_INIT;
					}else{
						GFree( *list );
					}
				}else{
					(*list)->step = SEARCH_FILE;
					if( (*list)->data.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY ){//ファイル検出
						strcpy( sdd->find_dir, src_dir + strlen(sdd->dir) );
						sdd->find_data = (*list)->data;
						return TRUE;
					}
				}
				break;
			case SEARCH_FILE:
				(*list)->fnf_ret = FindNextFile( (*list)->handle, &(*list)->data );
				if( (*list)->fnf_ret ){
					if( (*list)->data.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY ){//ファイル検出
						strcpy( sdd->find_dir, src_dir + strlen(sdd->dir) );
						sdd->find_data = (*list)->data;
						return TRUE;
					}
				}else{//ディレクトリ検索終了
					FindClose( (*list)->handle );
					if(sdd->subdir_chk){
						(*list)->step = SEARCH_DIR_INIT;
					}else{
						GFree( *list );
					}
				}
				break;
		}
	}
	return FALSE;
}

BOOL InitSaerchFile( char *src, BOOL subdir_chk, SEARCH_DIR_DATA *sdd )
{
	char	drive[256];
	char	dir[256];
	char	fname[256];
	char	ext[256];

	ZeroMemory( sdd, sizeof(SEARCH_DIR_DATA) );
	_splitpath( src, drive, dir, fname, ext );
	wsprintf( sdd->dir,  "%s%s", drive, dir );
	wsprintf( sdd->file, "%s%s", fname, ext );
	sdd->subdir_chk = subdir_chk;
	return TRUE;
}
BOOL SaerchNextFile( SEARCH_DIR_DATA *sdd )
{
	return SaerchNextFile2( sdd->dir, (WIN32_FIND_DATA_LIST**)&sdd->list, sdd );
}
BOOL TermSaerchFile( SEARCH_DIR_DATA *sdd )
{
	WIN32_FIND_DATA_LIST	*list;
	if( sdd==NULL ) return FALSE;

	list = sdd->list;
	while( list ){
		FindClose( list->handle );
		GFree( list );
		list = (WIN32_FIND_DATA_LIST*)list->next;
	}
	ZeroMemory( sdd, sizeof(SEARCH_DIR_DATA) );
	return TRUE;
}


static char DefaultDir[MAX_PATH];
static char DefaultExt[8];
// ---------------------------------------------------------------------------
//	デフォルトデレクトリ設定
// ---------------------------------------------------------------------------
void STD_SetDefaultDir( char *str, int flag )
{
	int	i;

	ZeroMemory( DefaultDir, MAX_PATH );
	strcpy( DefaultDir, str );
	if( flag ){
		for(i=0; i<MAX_PATH ;i++){
			if( DefaultDir[MAX_PATH-1-i] == '\\' ){
				DefaultDir[MAX_PATH-1-i] = '\0';
				break;
			}
		}
	}
}
// ---------------------------------------------------------------------------
//	デフォルト拡張子設定
// ---------------------------------------------------------------------------
void STD_SetDefaultExt( char *str)
{
	strcpy( DefaultExt, str);
}
// ---------------------------------------------------------------------------
//	デフォルトディレクトリ／拡張子、レジストリロード
// ---------------------------------------------------------------------------
void STD_LoadDefaultDir( char *regkey )
{
	SetRegistryKeyEx( regkey );

	ReadRegistryStr( "defdir", DefaultDir );
	ReadRegistryStr( "defext", DefaultExt );
}
// ---------------------------------------------------------------------------
//	デフォルトディレクトリ／拡張子、レジストリセーブ
// ---------------------------------------------------------------------------
void STD_SaveDefaultDir( char *regkey )
{
	SetRegistryKeyEx( regkey );

	WriteRegistryStr( "defdir", DefaultDir );
	WriteRegistryStr( "defext", DefaultExt );
}
// ---------------------------------------------------------------------------
//	オープンファイルダイアログ
// ---------------------------------------------------------------------------
char *STD_GetOpenFileName( HWND hwnd, char *ff)
{
	OPENFILENAME	fname;
	static char		fn[MAX_PATH];
	char			filename[64];

	fn[0] = '\0';
	memset( &fname, 0, sizeof(OPENFILENAME));
	fname.lStructSize		= sizeof(OPENFILENAME);
	fname.hwndOwner			= hwnd;
	fname.lpstrFilter		= ff;
	fname.nFilterIndex		= 1;
	fname.lpstrFile			= fn;
	fname.nMaxFile			= sizeof(fn);
	fname.lpstrFileTitle	= filename;
	fname.nMaxFileTitle		= sizeof(filename)-1;
	fname.lpstrInitialDir	= DefaultDir;
	fname.Flags				= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	if( !GetOpenFileName( &fname))	return NULL;

	return fn;
}

// ---------------------------------------------------------------------------
//	セーブファイルダイアログ
// ---------------------------------------------------------------------------
char *STD_GetSaveFileName( HWND hwnd, char *ff)
{
	OPENFILENAME	fname;
	static char		fn[MAX_PATH];
	char			filename[64];

	fn[0] = '\0';
	memset( &fname, 0, sizeof(OPENFILENAME));
	fname.lStructSize		= sizeof(OPENFILENAME);
	fname.hwndOwner			= hwnd;
	fname.lpstrFilter		= ff;
	fname.nFilterIndex		= 1;
	fname.lpstrFile			= fn;
	fname.nMaxFile			= sizeof(fn);
	fname.lpstrFileTitle	= filename;
	fname.nMaxFileTitle		= sizeof(filename)-1;
	fname.lpstrInitialDir	= DefaultDir;
	fname.Flags				= OFN_HIDEREADONLY;
	fname.lpstrDefExt		= DefaultExt;
	if( !GetSaveFileName( &fname))	return NULL;

	return fn;
}
// ---------------------------------------------------------------------------
//		よーわからんけど、解放らしい
// ---------------------------------------------------------------------------
static void _SHFree(void *p)
{
    IMalloc *pm;
    SHGetMalloc(&pm);

    if( pm ){
		pm->Free( p );
        pm->Release();
    }
}
// ---------------------------------------------------------------------------
//	参照ディレクトリ ダイアログ
// ---------------------------------------------------------------------------
static char	*BrowseDir=NULL;
int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	int iRetValue = 0;

	switch (uMsg){
	case BFFM_INITIALIZED:
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)BrowseDir );
		break;
	case BFFM_SELCHANGED:		break;
	case BFFM_VALIDATEFAILED:	iRetValue = 1;	break;
	}
	return iRetValue;
}
BOOL STD_LuckupDir( HWND hwnd, char *dir )
{
	BROWSEINFO	bi = { hwnd, NULL, NULL, NULL, BIF_RETURNONLYFSDIRS, NULL, 0, 0 };
	ITEMIDLIST	*pidl=NULL;

	BrowseDir =dir;
	bi.pszDisplayName = dir;
	bi.lpfn = BrowseCallbackProc;
	pidl = SHBrowseForFolder( &bi );
	if ( pidl ){
		SHGetPathFromIDList( pidl, dir );
		_SHFree(pidl);
	}
	return pidl!=NULL;
}




//++++++ iniファイル制御 +++++++++++++++++
static char	IniFileName[MAX_PATH];
void SetIniFileName( char *dir, char *ini_fname, BOOL *exist )
{
	if(dir){
		wsprintf( IniFileName, "%s\\%s.ini", dir, ini_fname );
	}else{
		wsprintf( IniFileName, ".\\%s.ini", ini_fname );
	}

	if(exist){
		HANDLE	fh = CreateFile( IniFileName, GENERIC_READ , FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( fh == INVALID_HANDLE_VALUE ){
			*exist = FALSE;
			return ;
		}else{
			*exist = TRUE;
			CloseHandle( fh );
			return ;
		}
	}
	return ;
}


// ---------------------------------------------------------------------------
//	iniファイルからKEYのリストを受け取る
// ---------------------------------------------------------------------------
int GetIniFileKeyList( char *key_list, int size, char *sec_name, char *ini_fname )
{
	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}

	GetPrivateProfileString( sec_name, NULL, "NO_KEY", key_list, size, ini_fname );

	return !strcmp( key_list, "NO_KEY" );
}

// ---------------------------------------------------------------------------
//	iniファイルからSECTIONのリストを受け取る
// ---------------------------------------------------------------------------
int GetIniFileSectionList( char *sec_list, int size, char *ini_fname )
{
	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}

	GetPrivateProfileString( NULL, NULL, "NO_SEC", sec_list, size, ini_fname );

	return !strcmp( sec_list, "NO_SEC" );
}


// ---------------------------------------------------------------------------
//	iniファイルから数値(long)を書き込む
// ---------------------------------------------------------------------------
int WriteIniFileAmount( char *vname, int amount, char *sec_name, char *ini_fname )
{
	int		ret;
	char	int_buf[16];

	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}
	wsprintf( int_buf, "%d", amount );
	ret = !WritePrivateProfileString( sec_name, vname, int_buf, ini_fname );
	return ret;
}

// ---------------------------------------------------------------------------
//	iniファイルから数値(long)を読み取る
// ---------------------------------------------------------------------------
int ReadIniFileAmount( char *vname, int def, char *sec_name, char *ini_fname )
{
	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}

	return GetPrivateProfileInt( sec_name, vname, def, ini_fname );
}
// ---------------------------------------------------------------------------
//	iniファイルにデータを書き込む
// ---------------------------------------------------------------------------
int WriteIniFileData( char *vname, char *data, int size, char *sec_name, char *ini_fname )
{
	int		ret;

	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}
	ret = !WritePrivateProfileStruct( sec_name, vname, data, size, ini_fname );
	return ret;
}

// ---------------------------------------------------------------------------
//	iniファイルからデータを読み取る
// ---------------------------------------------------------------------------
int ReadIniFileData( char *vname, char *data, int size, char *sec_name, char *ini_fname )
{
	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}
	return !GetPrivateProfileStruct( sec_name, vname, data, size, ini_fname );
}


// ---------------------------------------------------------------------------
//	iniファイルに文字を書き込む
// ---------------------------------------------------------------------------
int WriteIniFileStr( char *vname, char *str, char *sec_name, char *ini_fname )
{
	int		ret;
	char	buf[1024];

	strcpy( buf, "_" );
	_mbscat( (BYTE*)buf, (BYTE*)str );//空白回避
//	_mbscpy( (BYTE*)buf, (BYTE*)str );

	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}
	
	ret = !WritePrivateProfileString( sec_name, vname, buf, ini_fname );
	return ret;
}

// ---------------------------------------------------------------------------
//	iniファイルから文字を読み取る
// ---------------------------------------------------------------------------
int ReadIniFileStr( char *vname, char *str, char *def, int size, char *sec_name, char *ini_fname )
{
	char	*buf = (char*)GAlloc( size+1 );
//	char	*buf = (char*)GAlloc( size );
	
	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}

	GetPrivateProfileString( sec_name, vname, def, buf, size, ini_fname );
	if( _mbscmp( (BYTE*)def, (BYTE*)buf ) ){
		_mbscpy( (BYTE*)str, (BYTE*)buf+1 );
	}else{
		_mbscpy( (BYTE*)str, (BYTE*)buf );
	}
//	_mbscpy( (BYTE*)str, (BYTE*)buf );
	GFree(buf);

	return 0;
}

// ---------------------------------------------------------------------------
//	iniファイルから指定KEYを削除する
// ---------------------------------------------------------------------------
int DeleteIniFileKey( char *vname, char *sec_name, char *ini_fname )
{
	int		ret;

	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}

	ret = !WritePrivateProfileString( sec_name, vname, NULL, ini_fname );
	return ret;
}
// ---------------------------------------------------------------------------
//	iniファイルから指定SECTIONを削除する
// ---------------------------------------------------------------------------
int DeleteIniFileSection( char *sec_name, char *ini_fname )
{
	int		ret;

	if( ini_fname==NULL){
		if(IniFileName[0]==0){
			ini_fname = "default.ini";
		}else{
			ini_fname = IniFileName;
		}
	}

	ret = !WritePrivateProfileString( sec_name, NULL, NULL, ini_fname );
	return ret;
}




//++++++ レジストリ制御 +++++++++++++++++

static char	RegistryKey[MAX_PATH];
// ---------------------------------------------------------------------------
//	レジストリキーを設定する
// ---------------------------------------------------------------------------
void SetRegistryKey( char *key)
{
	wsprintf( RegistryKey, "%s", key);
}

void SetRegistryKeyEx( char *key)
{
	wsprintf( RegistryKey, "SoftWare\\%s", key);
}

// ---------------------------------------------------------------------------
//	レジストリに値を書き込む
// ---------------------------------------------------------------------------
int WriteRegistryAmount( char *vname, long amount )
{
	int   ret;
	HKEY  key;
	DWORD size;
	
	ret = RegCreateKeyEx( HKEY_CURRENT_USER, RegistryKey, 0, NULL,
							REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &size);
	if( ret==ERROR_SUCCESS) {
		ret = (RegSetValueEx( key, vname, 0, REG_DWORD, (const unsigned char *)&amount, 4)==ERROR_SUCCESS);
		RegCloseKey(key);
	} else
		ret = -1;

	return ret;
}

// ---------------------------------------------------------------------------
//	レジストリから値を読み取る
// ---------------------------------------------------------------------------
int ReadRegistryAmount( char *vname, long *amount )
{
	return ReadRegistryData( vname, (char *)amount, 4 );
}
// ---------------------------------------------------------------------------
//	レジストリに文字を書き込む
// ---------------------------------------------------------------------------
int WriteRegistryData( char *vname, char *data, int size )
{
	int   ret;
	HKEY  key;
	DWORD dmy;
	
	ret = RegCreateKeyEx( HKEY_CURRENT_USER, RegistryKey, 0, NULL,
							REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dmy);
	if( ret==ERROR_SUCCESS) {
		ret = RegSetValueEx( key, vname, 0, REG_BINARY, ( const unsigned char *)data, size );
		RegCloseKey(key);
	} else
		ret = -1;

	return ret;
}

// ---------------------------------------------------------------------------
//	レジストリから文字を読み取る
// ---------------------------------------------------------------------------
int ReadRegistryData( char *vname, char *data, int size )
{
	int   ret;
	HKEY  key;
	
	ret = RegOpenKeyEx( HKEY_CURRENT_USER, RegistryKey, 0, KEY_ALL_ACCESS, &key);
	if( ret==ERROR_SUCCESS){
		ret = RegQueryValueEx( key, vname, NULL, NULL, (unsigned char *)data, (unsigned long *)&size );
		RegCloseKey(key);
	} else
		ret = -1;

	return ret;
}


// ---------------------------------------------------------------------------
//	レジストリに文字を書き込む
// ---------------------------------------------------------------------------
int WriteRegistryStr( char *vname, char *str )
{
	int   ret;
	HKEY  key;
	DWORD dmy;
	
	ret = RegCreateKeyEx( HKEY_CURRENT_USER, RegistryKey, 0, NULL,
							REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dmy);
	if( ret==ERROR_SUCCESS) {
		ret = RegSetValueEx( key, vname, 0, REG_SZ, (unsigned char *)str, strlen(str)+1 );
		RegCloseKey(key);
	} else
		ret = -1;

	return ret;
}

// ---------------------------------------------------------------------------
//	レジストリから文字を読み取る
// ---------------------------------------------------------------------------
int ReadRegistryStr( char *vname, char *str )
{
	int		ret;
	DWORD	size, type=REG_SZ;
	HKEY	key;
	
	ret = RegOpenKeyEx( HKEY_CURRENT_USER, RegistryKey, 0, KEY_ALL_ACCESS, &key);
	if( ret==ERROR_SUCCESS) {
		ret = RegQueryValueEx( key, vname, NULL, &type, NULL, &size );	//文字数取得
		if( ret==ERROR_SUCCESS) {
			ret = RegQueryValueEx( key, vname, NULL, &type, (unsigned char *)str, &size );	//文字列取得
		}
		RegCloseKey(key);
	} else
		ret = -1;

	return ret;
}

// ---------------------------------------------------------------------------
//	レジストリを削除する
// ---------------------------------------------------------------------------
int DeleteRegistry( char *vname)
{
	int   ret;
	HKEY  key;
	
	ret = RegOpenKeyEx( HKEY_CURRENT_USER, RegistryKey, 0, KEY_ALL_ACCESS, &key);
	if( ret==ERROR_SUCCESS) {
		ret = RegDeleteValue( key, vname);
		RegCloseKey(key);
	} else
		ret = -1;

	return ret;
}
