
#include "STD_Debug.h"
#include "STD_File.h"

#include "Comp_LZS.h"
#include "Comp_pac.h"

#define		PAC_FILE_ID		'PACK'

#define		PAC_LOAD		TRUE

DWORD PAC_CheckFile( char *pname, char *fname )
{
	int		ret;
	char	fname2[256];

	if(pname){
		if(PAC_LOAD){

			wsprintf( fname2, "%s.pak", pname );
			if( !(ret = PAC_CheckPackFile(fname2,fname)) ){
				wsprintf( fname2, "pak\\%s.pak", pname );
				if( !(ret = PAC_CheckPackFile(fname2,fname)) ){
					wsprintf( fname2, "%s\\%s", pname, fname );
					ret = STD_CheckFile( fname2 );
				}
			}
		}else{
			wsprintf( fname2, "%s\\%s", pname, fname );
			ret = STD_CheckFile( fname2 );
		}
	}else{
		ret = STD_CheckFile( fname );
	}
	return ret;
}


DWORD PAC_LoadFile( char *pname, char *fname, BYTE **buf )
{
	int		ret;
	char	fname2[256];
	
	if(pname){
		if(PAC_LOAD){
			char	fname3[256];
			wsprintf( fname2, "%s\\%s", pname, fname );
			wsprintf( fname3, "%s\\buf\\%s", pname, fname );
			CopyFile(fname2,fname3,TRUE);

			wsprintf( fname2, "%s\\%s", pname, fname );
			if( !(ret = STD_ReadFileMax(fname2,buf)) ){
				wsprintf( fname2, "patch.pak" );
				if( !(ret = PAC_OpenPackFile( fname2, fname, buf)) ){
					wsprintf( fname2, "%s.pak", pname );
					if( !(ret = PAC_OpenPackFile(fname2,fname, buf)) ){
						wsprintf( fname2, "pak\\%s.pak", pname );
						ret = PAC_OpenPackFile(fname2,fname,buf);	
					}
				}
			}
		}else{
			wsprintf( fname2, "%s\\%s", pname, fname );
			ret = STD_ReadFileMax( fname2, buf );
		}
		if( !ret ){
			DebugBox( NULL, "ファイルが開けません[%s,%s]", pname, fname );
		}
	}else{
		ret = STD_ReadFileMax( fname, buf );
		if( !ret ){
			DebugBox( NULL, "ファイルが開けません[%s]", fname );
		}
	}
	return ret;
}
DWORD PAC_AllFilecheck( char *pname )
{
	HANDLE			fh;
	int				i, ret = 0;
	UINT			size = 0;
	FILE_INFO		*fi = NULL;
	PAC_FILE_INFO	pfi;
	BYTE			*buf=NULL;
	char			PackFname[128];

	wsprintf( PackFname, "%s.pak", pname );

	fh = CREATE_READ_FILE( PackFname);
	if( fh!=INVALID_HANDLE_VALUE) {
		
		ReadFile( fh, &pfi, sizeof( PAC_FILE_INFO), (unsigned long *)&size, NULL );

		if( pfi.head==PAC_FILE_ID ) {
		
			
			fi = (FILE_INFO *)GlobalAlloc( GPTR, sizeof(FILE_INFO)*pfi.famount );
			ReadFile( fh, fi, sizeof(FILE_INFO)*pfi.famount, (unsigned long *)&size, NULL );

			
			for( i=0; i < (int)pfi.famount ; i++) {
				
				PAC_LoadFile( pname, fi[i].fname, &buf );
				if(buf) buf = (BYTE*)GlobalFree(buf);
				DebugPrintf("%s\n",fi[i].fname);
			}
			
			if( fi) fi = (FILE_INFO*)GlobalFree( fi);
		}
		CloseHandle( fh);
	}
	return ret;
}





int PAC_CheckPackFile( char *pname, char *fname )
{
	HANDLE			fh;
	int				i, ret = 0;
	UINT			size = 0;
	FILE_INFO		*fi = NULL;
	PAC_FILE_INFO	pfi;

	fh = CREATE_READ_FILE( pname);
	if( fh!=INVALID_HANDLE_VALUE) {
		
		ReadFile( fh, &pfi, sizeof( PAC_FILE_INFO), (unsigned long *)&size, NULL );

		if( pfi.head==PAC_FILE_ID ) {
		
			
			fi = (FILE_INFO *)GlobalAlloc( GPTR, sizeof(FILE_INFO)*pfi.famount );
			ReadFile( fh, fi, sizeof(FILE_INFO)*pfi.famount, (unsigned long *)&size, NULL );

			
			for( i=0; i < (int)pfi.famount ; i++) {
				
				if( !lstrcmpi( fi[i].fname, fname)) {
					ret = i+1;
					break;
				}
			}
			
			if( fi) fi = (FILE_INFO*)GlobalFree( fi);
		}
		CloseHandle( fh);
	}

	return ret;
}




int PAC_OpenPackFile( char *pname, char *fname, BYTE **buf )
{
	HANDLE			fh;
	int				i, size = 0, ret = 0;
	BYTE			*src = NULL;
	FILE_INFO		*fi = NULL;
	PAC_FILE_INFO	pfi;
	
	fh = CREATE_READ_FILE( pname);
	if( fh!=INVALID_HANDLE_VALUE) {
		
		ReadFile( fh, &pfi, sizeof( PAC_FILE_INFO), (unsigned long *)&size, NULL );

		if( pfi.head==PAC_FILE_ID ) {

			
			fi = (FILE_INFO *)GlobalAlloc( GPTR, sizeof(FILE_INFO)*pfi.famount );
			ReadFile( fh, fi, sizeof(FILE_INFO)*pfi.famount, (unsigned long *)&size, NULL );

			
			for( i=0; i < (int)pfi.famount ; i++) {
				
				if( !lstrcmpi( fi[i].fname, fname)) {
					SetFilePointer( fh, fi[i].offset, NULL, FILE_BEGIN );	
					if(fi[i].type==0){
						*buf = (BYTE *)GlobalAlloc( GPTR, fi[i].size );			
						ReadFile( fh, *buf, fi[i].size, (unsigned long *)&size, NULL);	
					}else{
						src = (BYTE *)GlobalAlloc( GPTR, fi[i].size );					
						ReadFile( fh, src, fi[i].size, (unsigned long *)&size, NULL);	

						*buf = (BYTE *)GlobalAlloc( GPTR, *(DWORD*)(src+4) );			
						
						size = LZS_DecodeMemory( *buf, &src[8], fi[i].size-8, *(DWORD*)(src+4) );	

						if( *(DWORD*)(src+4) != (DWORD)size ){
							
							DebugBox( NULL, "読み込みデータに異常が認められました。HDDまたはメモリ等にエラーがある可能性があります。\n\nオーバークロック等の改造を施している場合は、それを解除して下さい。\nまた、マシン環境に問題が無いと思われる場合は、HDDのエラーチェックをした上で、\nゲームの再インストールを試して下さい。\nファイル名：[%s]", fname );
						}
						
						if( src) src = (BYTE *)GlobalFree(src);
					}
					ret = i+1;
					break;
				}
			}

			
			if( !ret){
				size = 0;

			}
			
			if( fi) fi = (FILE_INFO *)GlobalFree( fi);
		}
		CloseHandle( fh);
	}

	return size;
}

DWORD PAC_OpenFileHandle( char *pname, char *fname, HANDLE *fh )
{
	char			fpath[256];
	PAC_FILE_INFO	fpi;
	FILE_INFO		fi;
	DWORD			count=0, size=0;

	ZeroMemory( &fpi, sizeof(PAC_FILE_INFO) );
	ZeroMemory( &fi, sizeof(FILE_INFO) );
	if(*fh) CloseHandle( *fh );
	*fh = NULL;

	wsprintf( fpath, "%s\\%s", pname, fname );
	*fh = CREATE_READ_FILE( fpath );
	if( *fh == INVALID_HANDLE_VALUE ) {
		wsprintf( fpath, "%s.pak", pname );
		*fh = CREATE_READ_FILE( fpath );
		if( *fh != INVALID_HANDLE_VALUE ) {
			ReadFile( *fh, &fpi, sizeof(PAC_FILE_INFO), &size, NULL );
			if( fpi.head!=PAC_FILE_ID ) {	
				return 0;
			}
			while( count<fpi.famount ){
				ReadFile( *fh, &fi, sizeof(FILE_INFO), &size, NULL );
				if( !lstrcmpi( fi.fname, fname ) ){
					SetFilePointer( *fh, fi.offset, NULL, FILE_BEGIN );
					size = fi.size;
					break;
				}
				count++;
			}
		}
	}else{
		size = GetFileSize( *fh, NULL );
	}
	if(size==0) { if(*fh)CloseHandle( *fh ); *fh=NULL; }
	return size;
}
