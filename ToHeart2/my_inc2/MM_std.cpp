
 
  
   
    
     
      

#include "mm_std.h"
#include <shlobj.h>
#include <math.h>

#include <TCHAR.H>




static HWND	MainHwnd=NULL;

void STD_SetMainHwnd( HWND hwnd )
{
	MainHwnd = hwnd;
}







void memcpy2( void *dst, const void *src, int size )
{
	int	size2=size/16;



	if(SSE2_Flag && !((DWORD)dst&0xf) ){
		int	size2=size/32;
		if(size2){
			if((DWORD)src&0xf){
				_asm {
					mov edi,dst;
					mov esi,src;
					mov ecx,size2;
				lx_sse2_ups:
					movups xmm0,[esi];
					movups xmm1,[esi+16];
					lea esi,[esi+32];
					movntps [edi],xmm0;
					prefetcht0 [esi+2048];
					movntps [edi+16],xmm1;
					lea edi,[edi+32];
					dec ecx;
					jnz lx_sse2_ups;
				}
			}else{
				_asm {
					mov edi,dst;
					mov esi,src;
					mov ecx,size2;
				lx_sse2_aps:
					movaps xmm0,[esi];
					movaps xmm1,[esi+16];
					lea esi,[esi+32];
					movntps [edi],xmm0;
					prefetcht0 [esi+2048];
					movntps [edi+16],xmm1;
					lea edi,[edi+32];
					dec ecx;
					jnz lx_sse2_aps;
				}
			}
		}
		if(size-size2*32) CopyMemory( (char*)(dst)+size2*32, (char*)(src)+size2*32, size-size2*32 );
	}else if(SSE_FP_Flag ){
		if(size2){
			_asm{
				mov edi,dst;
				mov esi,src;
				mov ecx,size2;
			lx_sse:
				movq mm0,[esi];
				movq mm1,[esi+8];
				lea esi,[esi+16];
				movntq [edi],mm0;
				prefetcht0 [esi+2048];
				movntq [edi+8],mm1;
				lea edi,[edi+16];
				dec ecx;
				jnz lx_sse;
				emms
			}
		}
		if(size-size2*16) CopyMemory( (char*)(dst)+size2*16, (char*)(src)+size2*16, size-size2*16 );
	}else if(MMX_Flag){
		if(size2){
			_asm {
				mov edi,dst;
				mov esi,src;
				mov ecx,size2;
			lx_mmx:
				movq mm0,[esi];
				movq mm1,[esi+8];
				lea esi,[esi+16];
				movq [edi],mm0;
				movq [edi+8],mm1;
				lea edi,[edi+16];
				dec ecx;
				jnz lx_mmx;
				emms
			}
		}
		if(size-size2*16) CopyMemory( (char*)(dst)+size2*16, (char*)(src)+size2*16, size-size2*16 );
	}else{
		CopyMemory( dst, src, size );
	}
}




void DebugBox( HWND hwnd, char *mes, ... )
{
	char	buf[512];
	wvsprintf( buf, mes, (char *)(&mes+1) );
#ifdef _DEBUG
	MessageBox2( hwnd, MB_OK|MB_SETFOREGROUND, "警告", buf );
#else
	MessageBox2( hwnd, MB_OK|MB_SETFOREGROUND, "警告", buf );

#endif
}

int MessageBox2( HWND hwnd, DWORD flag, char *title, char *mes, ... )
{
	char	buf[512];
	if(hwnd==NULL) hwnd = MainHwnd;
	wvsprintf( buf, mes, (char *)(&mes+1) );
	return MessageBox( hwnd, buf, title, flag|MB_SETFOREGROUND );
}

int CheckBox( HWND hwnd, char *title, char *mes, ... )
{
	char	buf[512];
	wvsprintf( buf, mes, (char *)(&mes+1) );
	return MessageBox2( hwnd, MB_OKCANCEL, title, buf );
}





void DebugPrintf( char *mes, ...)
{
	char	buf[1024];

	wvsprintf( buf, mes, (char *)(&mes+1));
	OutputDebugString(buf);



}

void GetPrintErrMessage( int err_code )
{
	LPTSTR lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);
	DebugPrintf( "err:%s\n", lpMsgBuf );
	LocalFree(lpMsgBuf);
}








DWORD	RandTbl[0x10000];
DWORD	RandCount=0;

void STD_InitTable(void)
{
	int	i;






	for(i=0; i<0x10000 ;i++)
		RandTbl[i] = rand();
}



const WORD	SinTbl[256] = {										
	0x0000, 0x0064, 0x00c8, 0x012d, 0x0191, 0x01f5, 0x0259, 0x02bc,
	0x031f, 0x0381, 0x03e3, 0x0444, 0x04a5, 0x0504, 0x0563, 0x05c2,
	0x061f, 0x067b, 0x06d7, 0x0731, 0x078a, 0x07e2, 0x0839, 0x088f,
	0x08e3, 0x0936, 0x0987, 0x09d7, 0x0a26, 0x0a73, 0x0abe, 0x0b08,
	0x0b50, 0x0b96, 0x0bda, 0x0c1d, 0x0c5e, 0x0c9d, 0x0cd9, 0x0d14,
	0x0d4d, 0x0d84, 0x0db9, 0x0deb, 0x0e1c, 0x0e4a, 0x0e76, 0x0ea0,
	0x0ec8, 0x0eed, 0x0f10, 0x0f31, 0x0f4f, 0x0f6b, 0x0f85, 0x0f9c,
	0x0fb1, 0x0fc3, 0x0fd3, 0x0fe1, 0x0fec, 0x0ff4, 0x0ffb, 0x0ffe,
	0x1000, 0x0ffe, 0x0ffb, 0x0ff4, 0x0fec, 0x0fe1, 0x0fd3, 0x0fc3,
	0x0fb1, 0x0f9c, 0x0f85, 0x0f6b, 0x0f4f, 0x0f31, 0x0f10, 0x0eed,
	0x0ec8, 0x0ea0, 0x0e76, 0x0e4a, 0x0e1c, 0x0deb, 0x0db9, 0x0d84,
	0x0d4d, 0x0d14, 0x0cd9, 0x0c9d, 0x0c5e, 0x0c1d, 0x0bda, 0x0b96,
	0x0b50, 0x0b08, 0x0abe, 0x0a73, 0x0a26, 0x09d7, 0x0987, 0x0936,
	0x08e3, 0x088f, 0x0839, 0x07e2, 0x078a, 0x0731, 0x06d7, 0x067b,
	0x061f, 0x05c2, 0x0563, 0x0504, 0x04a5, 0x0444, 0x03e3, 0x0381,
	0x031f, 0x02bc, 0x0259, 0x01f5, 0x0191, 0x012d, 0x00c8, 0x0064,
	0x0000, 0xff9c, 0xff38, 0xfed3, 0xfe6f, 0xfe0b, 0xfda7, 0xfd44,
	0xfce1, 0xfc7f, 0xfc1d, 0xfbbc, 0xfb5b, 0xfafc, 0xfa9d, 0xfa3e,
	0xf9e1, 0xf985, 0xf929, 0xf8cf, 0xf876, 0xf81e, 0xf7c7, 0xf771,
	0xf71d, 0xf6ca, 0xf679, 0xf629, 0xf5da, 0xf58d, 0xf542, 0xf4f8,
	0xf4b0, 0xf46a, 0xf426, 0xf3e3, 0xf3a2, 0xf363, 0xf327, 0xf2ec,
	0xf2b3, 0xf27c, 0xf247, 0xf215, 0xf1e4, 0xf1b6, 0xf18a, 0xf160,
	0xf138, 0xf113, 0xf0f0, 0xf0cf, 0xf0b1, 0xf095, 0xf07b, 0xf064,
	0xf04f, 0xf03d, 0xf02d, 0xf01f, 0xf014, 0xf00c, 0xf005, 0xf002,
	0xf000, 0xf002, 0xf005, 0xf00c, 0xf014, 0xf01f, 0xf02d, 0xf03d,
	0xf04f, 0xf064, 0xf07b, 0xf095, 0xf0b1, 0xf0cf, 0xf0f0, 0xf113,
	0xf138, 0xf160, 0xf18a, 0xf1b6, 0xf1e4, 0xf215, 0xf247, 0xf27c,
	0xf2b3, 0xf2ec, 0xf327, 0xf363, 0xf3a2, 0xf3e3, 0xf426, 0xf46a,
	0xf4b0, 0xf4f8, 0xf542, 0xf58d, 0xf5da, 0xf629, 0xf679, 0xf6ca,
	0xf71d, 0xf771, 0xf7c7, 0xf81e, 0xf876, 0xf8cf, 0xf929, 0xf985,
	0xf9e1, 0xfa3e, 0xfa9d, 0xfafc, 0xfb5b, 0xfbbc, 0xfc1d, 0xfc7f,
	0xfce1, 0xfd44, 0xfda7, 0xfe0b, 0xfe6f, 0xfed3, 0xff38, 0xff9c,
};




int STD_Amari( int num1, int num2 )
{
	if(num2==0) return 0;

	if(num2<0) { num1*=-1; num2*=-1; }	
	if(num1<0){
		while(num1<0) { num1+=num2; }
	}else{
		while(num1>=num2) { num1-=num2; }
	}
	return num1;
}



typedef struct{
	int		type;	
	int		factor;	
	void	*next;
}CAL_LIST;

static char	*CalStr=NULL;
static int	CalP=0;

static CAL_LIST *NewList( int type, int factor )
{
	CAL_LIST	*list = (CAL_LIST *)GAlloc( sizeof(CAL_LIST) );

	list->type   = type;
	list->factor = factor;
	list->next   = NULL;

	return list;
}

static void AddList( CAL_LIST *list, CAL_LIST *add_list )
{
	while(list->next){list = (CAL_LIST *)list->next;}
	list->next     = add_list;

}

static void ReleaseList( CAL_LIST *list )
{
	CAL_LIST *next;

	if(list){
		next = (CAL_LIST *)list->next;
		GFree(list);
		ReleaseList( next );
	}
	return ;
}

static CAL_LIST *factor( void )
{
	BOOL		minus = FALSE;
	int			digit;
	char		buf[256], i=0;

	while( CalStr[CalP]=='-' ){
		minus = !minus;
		CalP++;
	}
	if( !strncmp(&CalStr[CalP],"0x",2) ){
		CalP+=2;
		while( isdigit(CalStr[CalP]) ||
				('a'<=CalStr[CalP] && CalStr[CalP]<='f') ||
				('A'<=CalStr[CalP] && CalStr[CalP]<='F') ){
			buf[i++]=CalStr[CalP++];
		}
		buf[i] = '\0';
		digit = Str2Hex(buf);
	}else{
		while( isdigit(CalStr[CalP]) ){
			buf[i++] = CalStr[CalP++];
		}
		buf[i]='\0';
		digit = atoi(buf);
	}
	if(minus) digit = -digit;

	return NewList( 0, digit );
}

static CAL_LIST *expr1( void )
{
	CAL_LIST	*list = factor();
	int			type;

	while( CalStr[CalP]=='*' || CalStr[CalP]=='/' || CalStr[CalP]=='%' ){
		type = CalStr[CalP];
		CalP++;
		AddList( list, factor() );
		AddList( list, NewList(1,type) );
	}
	return list;
}

static CAL_LIST *expr0( void )
{
	CAL_LIST	*list = expr1();
	int			type;

	while( CalStr[CalP]=='+' || CalStr[CalP]=='-' ){
		type = CalStr[CalP];
		CalP++;
		AddList( list, expr1() );
		AddList( list, NewList(1,type) );
	}
	return list;
}

static int StackDigit[32];
static int StackP=0;
static int	GoCal2Digit( char *cal )
{
	CAL_LIST	*start_list, *work_list;
	int			opr1, opr2;

	CalStr=cal;
	CalP=0;
	work_list = start_list = expr0();

	while(work_list){
		if(work_list->type){
			opr1 = StackDigit[--StackP];
			opr2 = StackDigit[--StackP];

			switch(work_list->factor){
				case '-':	opr1 = opr2-opr1;	break;
				case '+':	opr1 = opr2+opr1;	break;
				case '*':	opr1 = opr2*opr1;	break;
				case '/':	opr1 = opr2/opr1;	break;
				case '%':	opr1 = opr2%opr1;	break;
			}
			StackDigit[StackP] = opr1;
			StackP++;
		}else{
			StackDigit[StackP] = work_list->factor;
			StackP++;
		}
		work_list = (CAL_LIST *)work_list->next;
	}

	ReleaseList( start_list );
	StackP=0;
	return StackDigit[0];
}


int	Cal2Disgit( char *cal )
{
	char	buf[256];
	int		i=0, j=0;
	int		flag=0;

	while(1){
		buf[j] = cal[i];

		switch(cal[i]){
			case '\0':
				return GoCal2Digit( buf );
			case '[':
				j+=wsprintf( &buf[j], "%d", Cal2Disgit( &cal[i+1] ) )-1;
				while(cal[i]!=']'){ i++; }
				break;
			case ']':
				buf[i]='\0';
				return GoCal2Digit( buf );
		}
		i++;
		j++;
	}
	return 0;
}



HWND GetVolumeContorolHandle( void )
{
	HWND	hwnd;

	if( (hwnd=FindWindow(NULL,"Play Control"))==NULL ){
		if( (hwnd=FindWindow(NULL,"ボリューム コントロール"))==NULL ){
			if( (hwnd=FindWindow(NULL,"ﾎﾞﾘｭｰﾑ ｺﾝﾄﾛｰﾙ"))==NULL ){
				if( (hwnd=FindWindow(NULL,"デスクトップ スピーカー"))==NULL ){
					if( (hwnd=FindWindow(NULL,"ﾃﾞｽｸﾄｯﾌﾟ ｽﾋﾟｰｶｰ"))==NULL ){
						if( (hwnd=FindWindow(NULL,"Master Volume"))==NULL ){
							hwnd=FindWindow(NULL,"Volume Control");
						}
					}
				}
			}
		}
	}
	return hwnd;
}




void STD_SetForgroundMyWnd(HWND hWnd)
{
	DWORD dwID;
	dwID=GetWindowThreadProcessId(GetForegroundWindow(),NULL);

	AttachThreadInput(dwID,GetCurrentThreadId(),TRUE);

	SetForegroundWindow(hWnd);

	AttachThreadInput(dwID,GetCurrentThreadId(),FALSE);
}




void STD_SetDialogPos( HWND hd, int sx, int sy)
{
	STD_SetWindowPos( hd, sx, sy );
}

void STD_SetWindowPos( HWND hd, int sx, int sy )
{

	if(sx<0 || sy<0){
		RECT	rc1, rc2;
		GetWindowRect( hd, &rc1 );
		GetWindowRect( MainHwnd, &rc2 );

		SetWindowPos( hd, MainHwnd,
			rc2.left+((rc2.right-rc2.left)-(rc1.right-rc1.left))/2,
			rc2.top+((rc2.bottom-rc2.top)-(rc1.bottom-rc1.top))/2,
			0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER );
		



	}else{
		SetWindowPos( hd, MainHwnd, sx, sy, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER );
	}
}

void STD_GetWindowPos( HWND hd, int *sx, int *sy )
{
	RECT	rect;

	GetWindowRect( hd, &rect );
	*sx = rect.left;
	*sy = rect.top;
}
BOOL STD_GetWindowDisp( HWND hd )
{
	return IsWindowVisible( hd );
}




int	STD_GetScreenBpp( int *w, int *h )
{
	HDC		hDc = GetDC(MainHwnd);
	int		ret;

	ret = GetDeviceCaps(hDc,BITSPIXEL);
	*w = GetDeviceCaps(hDc,HORZRES);
	*h = GetDeviceCaps(hDc,VERTRES);

	ReleaseDC(NULL,hDc);
	return ret;
}





BYTE STD_GetBit( BYTE *buf, int pich, int num )
{
	BYTE	mask = 0xff>>(8-pich);
	if( !(pich==1 || pich==2 || pich==4) ) return 0;

	buf += num*pich/8;
	return (*buf>>(8-pich-num*pich%8)) & mask;

}



BOOL STD_SetBit( BYTE *buf, int pich, int num, int src )
{
	BYTE	mask = 0xff>>(8-pich);
	if( !(pich==1 || pich==2 || pich==4) ) return FALSE;

	buf += num*pich/8;

	*buf &= ~(mask<<(8-pich-num*pich%8));
	*buf |= (src & mask)<<(8-pich-num*pich%8);

	return TRUE;

}




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

	if(st==src)	return NULL;
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



BOOL STD_WriteFilePos( char *fname, char *src, DWORD pos, DWORD size )
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



BOOL STD_WriteFile( char *fname, char *src, DWORD size )
{
	HANDLE		fh;
	BOOL		dummy;

	fh = CREATE_WRITE_FILE( fname );
	if( fh == INVALID_HANDLE_VALUE ) return 0;

	WriteFile( fh, src, size, (unsigned long *)&dummy, NULL );
	
	CloseHandle( fh );
	
	return dummy;
}



BOOL STD_ApendFile( char *fname, char *src, DWORD size )
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



BOOL STD_ReadFilePos( char *fname, char *buf, DWORD pos, DWORD size )
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



BOOL STD_ReadFile( char *fname, char *buf, DWORD size )
{
	HANDLE		fh;
	DWORD		dummy;

	fh = CREATE_READ_FILE( fname );
	if ( fh == INVALID_HANDLE_VALUE ) return 0;

	ReadFile( fh, buf, size, &dummy, NULL );
	
	CloseHandle( fh );
	
	return dummy;
}




DWORD STD_ReadFileMax( char *fname, char **buf )
{
	HANDLE		fh;
	DWORD		dummy, size;

	fh = CREATE_READ_FILE( fname );
	if ( fh == INVALID_HANDLE_VALUE ) return 0;

	size = GetFileSize( fh, &dummy );	
	if(size && dummy==0){
		*buf = (char *)GAlloc( size );
		ReadFile( fh, *buf, size, &dummy, NULL );
	}
	
	CloseHandle( fh );

	if( dummy != size ){
		DebugPrintf( "ファイルサイズと読みこみサイズが違うなり、何ゆえ？" );
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
	char			src_buf[1024];
	WIN32_FIND_DATA	FindFileData;
	HANDLE			hFind;

	if( src==NULL ){ return FALSE; }

	wsprintf( src_buf, "%s\\*.*", src );
	hFind = FindFirstFile( src_buf, &FindFileData );
	if( hFind == INVALID_HANDLE_VALUE ){ return FALSE; }
	FindNextFile( hFind, &FindFileData );		

	while( FindNextFile( hFind, &FindFileData ) ){
		if(FindFileData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY){
			wsprintf( src_buf, "%s\\%s", src, FindFileData.cFileName );
			
			if( !STD_DeleteDir( src_buf ) ){
				DebugBox(NULL,"フォルダ[%s]が削除できません。",src_buf);
			}
		}else{
			wsprintf( src_buf, "%s\\%s", src, FindFileData.cFileName );
			if( !STD_DeleteFile( src_buf ) ){
				DebugBox(NULL,"ファイル[%s]が削除できません。",src_buf);
			}
		}
	}
	FindClose( hFind );

	return RemoveDirectory( src );
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
	SEARCH_DIR_INIT,
	SEARCH_DIR,
	SEARCH_FILE_INIT,
	SEARCH_FILE,
	SEARCH_END,
};
static BOOL SaerchNextFile2( char *src_dir, WIN32_FIND_DATA_LIST **list, SEARCH_DIR_DATA *sdd )
{
	char		src_buf[1024];

	if(*list==NULL){
		*list = (WIN32_FIND_DATA_LIST *)GAlloc( sizeof(WIN32_FIND_DATA_LIST) );
		if(sdd->subdir_chk){
			(*list)->step = SEARCH_DIR_INIT;
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
					FindClose( (*list)->handle );
					(*list)->step = SEARCH_FILE_INIT;
				}
				break;

			case SEARCH_FILE_INIT://直下ファイルを検索
				wsprintf( src_buf, "%s%s", src_dir, sdd->file );//全てのファイルを検出
				(*list)->handle = FindFirstFile( src_buf, &(*list)->data );//.
				if( (*list)->handle == INVALID_HANDLE_VALUE ){
					GFree( *list );
					return FALSE;
				}else{
					(*list)->step = SEARCH_FILE;
					if( (*list)->data.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY ){//ファイル検出
						strcpy( sdd->find_dir, src_dir );
						sdd->find_data = (*list)->data;
						return TRUE;
					}
				}
				break;
			case SEARCH_FILE:
				(*list)->fnf_ret = FindNextFile( (*list)->handle, &(*list)->data );
				if( (*list)->fnf_ret ){
					if( (*list)->data.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY ){//ファイル検出
						strcpy( sdd->find_dir, src_dir );
						sdd->find_data = (*list)->data;
						return TRUE;
					}
				}else{//ディレクトリ検索終了
					FindClose( (*list)->handle );
					GFree( *list );
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
//	ZeroMemory( cdd, sizeof(SEARCH_DIR_DATA) );
//	strcpy( cdd->src, src );
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
	ZeroMemory( &fname, sizeof(OPENFILENAME));
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
	ZeroMemory( &fname, sizeof(OPENFILENAME));
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
	DWORD	type=REG_BINARY;
	
	ret = RegOpenKeyEx( HKEY_CURRENT_USER, RegistryKey, 0, KEY_ALL_ACCESS, &key);

	if( ret==ERROR_SUCCESS){
//		ret = RegQueryValueEx( key, vname, NULL, &type, NULL, &size2 );	//文字数取得
//		if( ret==ERROR_SUCCESS){
			ret = RegQueryValueEx( key, vname, NULL, NULL, (unsigned char *)data, (unsigned long *)&size );
//		}
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


//-------------------------------------------------------
//	WORD単位でメモリを埋める
//-------------------------------------------------------
BOOL FiilMemory_Word( WORD *buf, int size, int code )
{
	WORD	code2[4]={code,code,code,code};
	WORD	*temp = code2;
	int		size_4 = size/4;
	int		size_a = size%4;
	int		i;

	if( MMX_Flag ){
		if(size_4){
			_asm {
				mov		ecx, size_4
				mov		esi, temp
				mov		edi, buf
				movq	mm0, [esi]

				mmloopx:
					add		edi, 8
					movq	[edi-8], mm0
				loop	mmloopx
			}
			_asm	emms
		}
		buf += size_4*4;

		if( size_a ){	//8BYTE転送であぶれたcharを描画
			for( i=0; i < size_a ; i++, buf++, temp++ )
				*buf = *temp;
		}
	}else{
		for( i=0; i < size_4 ; i++, buf+=8 ){
			*(double*)buf = *(double*)temp;
		}
		if( size_a ){	//8BYTE転送であぶれたcharを描画
			for( i=0; i < size_a ; i++, buf++, temp++ )
				*buf = *temp;
		}
	}
	return TRUE;
}
//-------------------------------------------------------
//	3BYTE単位でメモリを埋める
//-------------------------------------------------------
BOOL FiilMemory_3Byte( char *buf, int size, int code )
{
	char	code2[24]={	code, code>>8, code>>16,	code, code>>8, code>>16,
						code, code>>8, code>>16,	code, code>>8, code>>16,
						code, code>>8, code>>16,	code, code>>8, code>>16,
						code, code>>8, code>>16,	code, code>>8, code>>16 };
	char	*temp = code2;
	int	size24 = size*3/24;
	int	size_a = size*3%24;
	int	i;

	if( MMX_Flag ){
		if(size24){
			_asm {
				mov		ecx, size24
				mov		esi, temp
				mov		edi, buf
				movq	mm0,  [esi]
				movq	mm1,  [esi+8]
				movq	mm2,  [esi+16]

				mmloopx:
					movq	[edi],    mm0
					movq	[edi+8],  mm1
					movq	[edi+16], mm2
					add		edi,24
				loop	mmloopx
			}
			_asm	emms
		}
		buf += size24*24;

		if( size_a ){	//8*3BYTE転送であぶれたcharを描画
			for( i=0; i < size_a ; i++, buf++, temp++ )
				*buf = *temp;
		}
	}else{
		for( i=0; i < size24 ; i++, buf+=24 ){
			*(double*)(buf)    = *(double*)(temp);
			*(double*)(buf+8)  = *(double*)(temp+8);
			*(double*)(buf+16) = *(double*)(temp+16);
		}
		if( size_a ){	//8*3BYTE転送であぶれたcharを描画
			for( i=0; i < size_a ; i++, buf++, temp++ )
				*buf = *temp;
		}
	}
	return TRUE;
}
//-------------------------------------------------------
//	DWORD単位でメモリを埋める
//-------------------------------------------------------
BOOL FiilMemory_Dword( DWORD *buf, int size, int code )
{
	DWORD	code2[2]={code,code};
	DWORD	*temp = code2;
	int		size_2 = size/2;
	int		size_a = size%2;
	int		i;

	if( MMX_Flag ){
		if(size_2){
			_asm {
				mov		ecx, size_2
				mov		esi, temp
				mov		edi, buf
				movq	mm0, [esi]

				mmloopx:
					add		edi, 8
					movq	[edi-8], mm0
				loop	mmloopx
			}
			_asm	emms
		}
		buf += size_2*2;

		if( size_a ){	//8BYTE転送であぶれたcharを描画
			*buf = *temp;
		}
	}else{
		for( i=0; i < size_2 ; i++, buf+=2 ){
			*(double*)buf = *(double*)temp;
		}
		if( size_a ){	//8BYTE転送であぶれたcharを描画
			*buf = *temp;
		}
	}

	return TRUE;
}
//-------------------------------------------------------
//	指定単位でメモリを埋める
//-------------------------------------------------------
BOOL STD_FiilMemory( void *buf, int size, int width, int code )
{
	if(buf==NULL)	return FALSE;

	switch(width){
		case 1:	FillMemory( buf, size, code );			break;
		case 2:	FiilMemory_Word(  (u_short *)buf, size, code );		break;
		case 3:	FiilMemory_3Byte( (char *)buf, size, code );	break;
		case 4:	FiilMemory_Dword( (u_long *)buf, size, code );	break;
		default:	return FALSE;
	}
	return TRUE;
}

//-------------------------------------------------------
//	指定数のスペースの文字列を返す
//-------------------------------------------------------
char *Digit2Space( int dg )
{
	static char	str[16];
	int			i;

	ZeroMemory( str, 16 );
	for(i=0;i<dg;i++) str[i] = ' ';

	return str;
}


//-------------------------------------------------------
//	文字列を数字に変換(１０進記号非対応)
//-------------------------------------------------------
int Str2Digit( char *str )
{
	int		i=0;
	int		minus = 1, digit=0;

	if( str== NULL   ) return 0;							//NULLエラー
	if( str[0]=='\0' ) return 0;							//未入力エラー
	if( str[0]=='-' ){	str++;	minus = -1;	}				//マイナス判定
	while( str[i] )	if( !isdigit( str[i++] ) ) return 0;	//数字外エラー

	i=0;
	while( str[i] ){
		digit *= 10;
		digit += str[i]-'0';
		i++;
	}
	digit *= minus;
	return digit;
}
//-------------------------------------------------------
//	文字列を数字に変換(１６進記号非対応)
//-------------------------------------------------------
int Str2Hex( char *str )
{
	int		i=0;
	int		digit=0;
	char	str2[256];
	if( str== NULL   ) return 0;							//NULLエラー
	if( str[0]=='\0' ) return 0;							//未入力エラー

	strcpy(str2,str);
	while( str2[i] ){
		str2[i] = (char)tolower( str2[i] );//大文字があれば小文字に変換
		if( !isdigit(str2[i]) && !('a'<=str[i] && str[i]>='f') )
			return 0;	//１６進外エラー
		i++;
	}

	i=0;
	while( str2[i] ){
		digit *= 16;

		if( isdigit(str2[i]) )	digit += str[i]-'0';
		else					digit += str[i]-'a'+10;
		i++;
	}

	return digit;
}

//-------------------------------------------------------
//	数字を文字列に変換(１０進１６桁)
//-------------------------------------------------------
char * Digit2Str( int digit, int zero, int lr )
{
	int			dg=1;
	int			p=0;
	int			digit2=digit;
	static char	str[16];

	if(lr){
		if( zero )	wsprintf( str, "%016d", digit );
		else		wsprintf( str, "%16d", digit );
	}else{
		wsprintf( str, "%d", digit );
	}

	return str;
}
char * Digit2Str2( int digit, int zero, int lr )
{
	int			dg=1;
	int			p=0;
	int			digit2=digit;
	static char	str[16];

	if(lr){
		if( zero )	wsprintf( str, "%02d", digit );
		else		wsprintf( str, "%2d", digit );
	}else{
		wsprintf( str, "%d", digit );
	}

	return str;
}
//-------------------------------------------------------
//	全角数字を 半角数字へ変換(１０桁)
//-------------------------------------------------------
char *Wstr2Bstr( char *str )
{
	int			i=0;
	static char	buf[11];
	ZeroMemory( buf, 11 );

	while( *str==(char)0x82 && i<10 ){
		str++;
		buf[i] = '0' + (*str - (char)0x4f);
		str++;
		i++;
	}
	return buf;
}
//-------------------------------------------------------
//	全角数字を 半角数字へ変換(１０桁)
//-------------------------------------------------------
char *Bstr2Wstr( char *str )
{
	int			i=0;
	static char	buf[21];
	ZeroMemory( buf, 21 );

	while( *str>='0' && *str<'9' && i<20 ){
		buf[i++] = (char)0x82;
		buf[i++] = *str - '0' + (char)0x4f;
		str++;
	}
	return buf;
}

//-------------------------------------------------------
//	半角大文字を、小文字に変換
//-------------------------------------------------------
void STD_strlower( char *dst, char *src )
{
	while( *src ){
		*(dst++) = (char)tolower( *(src++) );
	}
	*dst='\0';
}
//-------------------------------------------------------
//	半角小文字を、大文字に変換
//-------------------------------------------------------
void STD_strupper( char *dst, char *src )
{
	while( *src ){
		*(dst++) = (char)toupper( *(src++) );
	}
	*dst='\0';
}
//-------------------------------------------------------
//	文字列からあるコードを検索し、そこまでの文字列を取得
//-------------------------------------------------------
int	STD_strtok( char *dst, char *src, unsigned long code )
{
	int			i=0;
	static u_char	*src2=NULL;
	if(src)	src2 = (u_char *)src;

	if(code>0xff){
		code = ( (code>>8)|(code<<8) )&0xffff;
		while( *src2 ){
			if( *(WORD*)src2==code ){
				break;
			}
			i++;
			if(dst)	*dst++ = *src2++;
			else	src2++;
		}
		if(dst)	*dst='\0';
	}else{
		while( *src2 ){
			if( *src2==code ){
				break;
			}
			i++;
			if(dst)	*dst++ = *src2++;
			else	src2++;
		}
		if(dst)	*dst='\0';
	}
	return i;
}

//-------------------------------------------------------
//	文字列比較（ＮＵＬＬ文字を無視）
//-------------------------------------------------------
int	STD_strcmp( char *str1, char *str2 )
{
	int	len = strlen( str2 );
	return strncmp( str1, str2, len );
}
//-------------------------------------------------------
//	文字列比較（文字列１内に文字列２が含まれていれば、真）
//-------------------------------------------------------
int	STD_strcmp2( char *str1, char *str2 )
{
	int	i;
	int	len1 = strlen( str1 );
	int	len2 = strlen( str2 );
	int	ret=1;

	i=0;
	while( strncmp( str1, str2, len2 ) ){
		str1++; i++;
		if(i+len2>len1){
			ret=0;
			break;
		}
	}
	
	return ret;
}
//-------------------------------------------------------
//	文字列比較（ＮＵＬＬ文字を無視）
//-------------------------------------------------------
int	STD_strcmpUL( char *str1, char *str2 )
{
	char	buf1[1024], buf2[1024];
	STD_strlower( buf1, str1 );
	STD_strlower( buf2, str2 );
	return strcmp( buf1, buf2 );
}
//-------------------------------------------------------
//	リミット判定
//-------------------------------------------------------
long STD_StepLimit( int src, int start, int end, int fade, int max, int type )
{
	int cnt;
	if(fade==0) return 0;
	if(max==0) return 0;

	cnt = LIM( max*(src-start)/fade, 0, max );
	if(cnt>=max){
		cnt = max - LIM( max*(src-(end-fade))/fade, 0, max );

		if(type==3) cnt = cnt*cnt/max;
		if(type==4) cnt = max - (max-cnt)*(max-cnt)/max;
	}else{
		if(type==3) cnt = max - (max-cnt)*(max-cnt)/max;
		if(type==4) cnt = cnt*cnt/max;
	}
	if(type==1) cnt = cnt*cnt/max;
	if(type==2) cnt = max - (max-cnt)*(max-cnt)/max;

	return cnt;
}
//-------------------------------------------------------
//	リミット判定
//-------------------------------------------------------
long STD_Limit( long src, long digit1, long digit2 )
{
	long	ret;
	if(digit1 < digit2){
		ret = max( src, digit1 );
		ret = min( ret, digit2 );
	}else{
		ret = max( src, digit2 );
		ret = min( ret, digit1 );
	}
	return ret;
}
//-------------------------------------------------------
//	リミット巡回関数
//-------------------------------------------------------
long STD_LimitLoop( long src, long max_digit )
{
	long	ret;

	ret = src%(max_digit*2);
	if( ret>=max_digit ){	ret = max_digit*2 - ret;	}
	return ret;
}
//-------------------------------------------------------
//	たらいまわし関数
//-------------------------------------------------------
int STD_Taraimawasi( int x, int y, int z )
{
	if(x <= y) return y;

	return STD_Taraimawasi( STD_Taraimawasi( x-1, y, z ),
							STD_Taraimawasi( y-1, z, x ),
							STD_Taraimawasi( z-1, x, y ) );
}
double STD_Lagrange( double x, int m, POINT *pt )
{
	double	retval = 0;
	double	Nk = 1;
	int		k, j;
	
	for( k=0; k<=m; k++ ){	// Σ Ｙk Ｎk(x)
		Nk=1;
		for ( j=0; j<=m; j++ ){	// このfor-loopはＮk(x)の計算。
			if (j==k) continue;	// 条件に j != k があるので除外
			Nk *= (x - pt[j].x) / (pt[k].x - pt[j].x);
		}
		retval += Nk * pt[k].y;
	}
	
	return retval;
}


DWORD CPUIDMax    = 0;
DWORD CPUIDExtMax = 0;

char szVenderID[13] = {0};
char szCpuName[49] = {0};

DWORD TypeID     = 0;
DWORD FamilyID   = 0;
DWORD ModelID    = 0;
DWORD SteppingID = 0;

DWORD CPUID_Flag  = 0;
DWORD FPU_Flag    = 0;
DWORD TSC_Flag    = 0;
DWORD MSR_Flag    = 0;
DWORD CMOV_Flag   = 0;
DWORD MMX_Flag    = 0;
DWORD _3DNow_Flag = 0;

DWORD SSE_INT_Flag   = 0;
DWORD SSE_FP_Flag    = 0;
DWORD _3DNowExt_Flag = 0;

DWORD SSE2_Flag    = 0;
DWORD SSE3_Flag    = 0;

DWORD fCpuClock = 0;

void GetCpuID(void)
{
	__asm{
		//Check CPUID
		pushfd
		pop eax
		mov ebx, eax
		xor eax, 1<<21
		push eax
		popfd
		pushfd
		pop eax
		cmp eax, ebx
		je exitasm
		mov [CPUID_Flag], 1

		//CPUID 0
		xor eax, eax
		cpuid
		mov [CPUIDMax], eax
		mov DWORD PTR [szVenderID+0], ebx
		mov DWORD PTR [szVenderID+4], edx
		mov DWORD PTR [szVenderID+8], ecx
		mov BYTE PTR [szVenderID+12], 0

		//CPUID 1
		cmp eax, 1
		jb exitasm
		mov eax, 1
		cpuid

		mov esi, eax
		and esi, 0xf
		mov [SteppingID], esi

		shr eax, 4
		mov esi, eax
		and esi, 0xf
		mov [ModelID], esi

		shr eax, 4
		mov esi, eax
		and esi, 0xf
		mov [FamilyID], esi

		shr eax, 4
		mov esi, eax
		and esi, 0x3
		mov [TypeID], esi


		xor eax, eax
		test edx, 1<<0
		setnz al
		mov [FPU_Flag], eax

		test edx, 1<<4
		setnz al
		mov [TSC_Flag], eax

		test edx, 1<<5
		setnz al
		mov [MSR_Flag], eax

		test edx, 1<<15
		setnz al
		mov [CMOV_Flag], eax

		test edx, 1<<23
		setnz al
		mov [MMX_Flag], eax

		test edx, 1<<25
		setnz al
		mov [SSE_INT_Flag], eax
		mov [SSE_FP_Flag], eax

		test edx, 1<<26
		setnz al
		mov [SSE2_Flag], eax
//
		test ecx, 1<<0
		setnz al
		mov [SSE3_Flag], eax

		//CPUID 0x80000000
		mov eax, 0x80000000
		cpuid
		mov [CPUIDExtMax], eax

		//CPUID 0x80000001
		cmp eax, 0x80000001
		jb exitasm
		mov eax, 0x80000001
		cpuid

		xor eax, eax
		test edx, 1<<22
		setnz al
		mov [SSE_INT_Flag], eax

		test edx, 1<<30
		setnz al
		mov [_3DNowExt_Flag], eax

		test edx, 1<<31
		setnz al
		mov [_3DNow_Flag], eax

		//CPUID 0x80000002-4
		cmp [CPUIDExtMax], 0x80000004
		jb exitasm
		mov eax, 0x80000002
		cpuid
		mov DWORD PTR [szCpuName+ 0], eax
		mov DWORD PTR [szCpuName+ 4], ebx
		mov DWORD PTR [szCpuName+ 8], ecx
		mov DWORD PTR [szCpuName+12], edx
		mov eax, 0x80000003
		cpuid
		mov DWORD PTR [szCpuName+16], eax
		mov DWORD PTR [szCpuName+20], ebx
		mov DWORD PTR [szCpuName+24], ecx
		mov DWORD PTR [szCpuName+28], edx
		mov eax, 0x80000004
		cpuid
		mov DWORD PTR [szCpuName+32], eax
		mov DWORD PTR [szCpuName+36], ebx
		mov DWORD PTR [szCpuName+40], ecx
		mov DWORD PTR [szCpuName+44], edx
		

	exitasm:
	}
}

void GetCpuClock(void)
{
	DWORD dwCpuClock;
	DWORD dwMsCount;

	if (TSC_Flag == 0)
		return;

	__asm{
		rdtsc
		mov [dwCpuClock], eax
	}

	dwMsCount = timeGetTime();
	while(timeGetTime() - dwMsCount < 500);

	__asm{
		rdtsc
		sub eax, [dwCpuClock]
		mov [dwCpuClock], eax
	}

	fCpuClock = (DWORD)(dwCpuClock/500000.0);
}

//------------------------------------------------------
//	CPU判定
//------------------------------------------------------
void STD_CheckCPU( void )
{
//	return ;
	GetCpuID();
	GetCpuClock();

	DebugPrintf("VenderID  %s\n", szVenderID);
	DebugPrintf("CpuName   %s\n", szCpuName);
	DebugPrintf("TypeID %d, FamilyID %d, ModelID %d, SteppingID %d\n", TypeID, FamilyID, ModelID, SteppingID);
	DebugPrintf("CpuClock %dMHz\n", fCpuClock);

	DebugPrintf("\n");
	DebugPrintf("CPUID       %d\n", CPUID_Flag);		//CPUIDサポートの有無
	DebugPrintf("CPUIDMax    %x\n", CPUIDMax);		//CPUID標準機能のサポート最大値
	DebugPrintf("CPUIDExtMax %x\n", CPUIDExtMax);	//CPUID拡張機能のサポート最大値

	DebugPrintf("FPU    %d\n", FPU_Flag);				//浮動小数点演算器の有無
	DebugPrintf("TSC    %d\n", TSC_Flag);				//RDTSC命令の有無
	DebugPrintf("MSR    %d\n", MSR_Flag);				//RDMSR,WRMSR命令の有無
	DebugPrintf("CMOV   %d\n", CMOV_Flag);				//CMOV命令の有無
	DebugPrintf("MMX    %d\n", MMX_Flag);				//MMX命令の有無
	DebugPrintf("3DNow! %d\n", _3DNow_Flag);			//3DNow!命令の有無

	DebugPrintf("SSE_INT    %d\n", SSE_INT_Flag);		//SSE integer(=MMX拡張)命令の有無
	DebugPrintf("SSE_FP     %d\n", SSE_FP_Flag);		//SSE floating-point命令の有無
	DebugPrintf("3DNow! Ext %d\n", _3DNowExt_Flag);		//3DNow!拡張命令の有無
	DebugPrintf("FCMOV      %d\n", FPU_Flag & CMOV_Flag);//FCMOV命令の有無
/*
	DWORD			cpu_id = 0;
	DWORD			cpu_model = 0;
	DWORD			cpu_family = 0;
	char			CpuName[5][17] = { "謎めいています…", "Pentium", "MMX Pentium", "Pentium Pro", "Pentium II以上" };
	char			*CpuRank;
	DWORD			reg_eax=0;
	DWORD			reg_ebx=0;
	DWORD			reg_ecx=0;
	DWORD			reg_edx=0;
	DWORD			reg_tmp=0;

	CPU_Type = 0;
	MMX_Flag = FALSE;

	//CPUID命令を使用できるCPUかどうか
	_asm {
		pushfd
		pop		eax
		mov		reg_tmp, eax
		xor		eax, 00200000h
		push	eax
		popfd
		pushfd
		pop		eax
		mov		cpu_id, eax
	}
	if( cpu_id!=reg_tmp ) {//ID使用可
		_asm {
			mov     eax, 1
			_emit	0xf
			_emit	0xa2
			mov		reg_eax, eax
			mov		reg_ebx, ebx
			mov		reg_ecx, ecx
			mov		reg_edx, edx
		}
	}
	if( reg_edx & 0x00800000 ) {	//MMX使用可能？
		MMX_Flag = TRUE;
	}
	cpu_family = (reg_eax&0x00000f00)>>8;
	cpu_model  = (reg_eax&0x000000f0)>>4;
	switch( cpu_family ) {//CPU ファミリ番号
		case 5:		CPU_Type = MMX_Flag ? CPU_PENTIUM_MMX : CPU_PENTIUM;		break;
		case 6:
			if( MMX_Flag ){
				if(      cpu_model<=5 )	{ CPU_Type = CPU_PENTIUM_II;	}
				else if( cpu_model<=7 )	{ CPU_Type = CPU_PENTIUM_II+1;	}
				else					{ CPU_Type = CPU_PENTIUM_II+2;	}
			}else{
				CPU_Type = CPU_PENTIUM_PRO;
			}
			break;
		default:
			CPU_Type = CPU_UNKNOWN;
			break;
	}
	// デバッグ用メッセージ
	DebugPrintf( "CPU ID - EAX: %08x\n", reg_eax );
	DebugPrintf( "CPU ID - EBX: %08x\n", reg_ebx );
	DebugPrintf( "CPU ID - ECX: %08x\n", reg_ecx );
	DebugPrintf( "CPU ID - EDX: %08x\n", reg_edx );
	switch(CPU_Type){
		case CPU_UNKNOWN:		CpuRank = "わかんない (fps30)";		break;
		case CPU_PENTIUM:		CpuRank = "ヤバイ (fps15/20)";		break;
		case CPU_PENTIUM_MMX:	CpuRank = "ふつー (fps20/30)";		break;
		case CPU_PENTIUM_PRO:	CpuRank = "せこい (fps20/30)";		break;
		case CPU_PENTIUM_II:	CpuRank = "いいかんじ (fps30/45)";	break;
		case CPU_PENTIUM_II+1:	CpuRank = "やるなっ (fps45/60)";	break;
		case CPU_PENTIUM_II+2:	CpuRank = "すげぇぜ (fps60/60)";	break;
	}
	DebugPrintf( "CPU判定：%s\nMMX判定：%s\n",	CpuName[min(CPU_Type,CPU_PENTIUM_II)],
															(MMX_Flag)? "有": "無" );
//	DebugBox( NULL, "CPU判定：%s\nMMX判定：%s\n性能評価：%s",	CpuName[min(CPU_Type,CPU_PENTIUM_II)],
//																(MMX_Flag)? "有": "無",
//																CpuRank );
*/
} 

OSVERSIONINFO	OSver;

OSVERSIONINFO STD_CheckOS( void )
{
	OSver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &OSver );
	return OSver;
}
BOOL STD_CheckOSw2k( void )
{
	STD_CheckOS();
	if( OSver.dwPlatformId==VER_PLATFORM_WIN32_NT ){
		if( OSver.dwMajorVersion>4 ){
			return TRUE;
		}
	}
	return FALSE;
}

int STD_GetOSVer( void )
{
	STD_CheckOS();
	if( OSver.dwPlatformId==VER_PLATFORM_WIN32_NT ){
		if( OSver.dwMajorVersion<=4 )		return 0;//NT4以前
		else if(OSver.dwMajorVersion==5)	return 1;//W2K
		else if(OSver.dwMajorVersion==6)	return 2;//XP
		else								return 3;//XP以降
	}else{
		if( OSver.dwMajorVersion<=2 )		return 256;//95以前
		else if(OSver.dwMajorVersion==3)	return 257;//win95
		else if(OSver.dwMajorVersion==4)	return 258;//win98,SE,ME
		else								return 260;//ME以降
	}
}
char *STD_GetOSName( void )
{
	static char	buf[128];

	STD_CheckOS();
	if( OSver.dwPlatformId==VER_PLATFORM_WIN32_NT ){
		
		if( OSver.dwMajorVersion<=4 ){//NT4以前
			wsprintf( buf, "Windows NT%d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}else if(OSver.dwMajorVersion==5){//W2K,XP
			if(OSver.dwMinorVersion==0){//W2K
				wsprintf( buf, "Windows 2000 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else if(OSver.dwMinorVersion==1){//XP
				wsprintf( buf, "Windows XP %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else{
				wsprintf( buf, "Windows XP以降 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}
		}else {//XP以降
			wsprintf( buf, "Windows XP以降 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}
	}else{
		if( OSver.dwMajorVersion<=3){//win95
			wsprintf( buf, "Windows %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}else if(OSver.dwMajorVersion==4){//win98,SE,ME
			if(OSver.dwMinorVersion>=90){//
				wsprintf( buf, "Windows ME %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else if(OSver.dwMinorVersion>=10){
				wsprintf( buf, "Windows 98 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else{
				wsprintf( buf, "Windows 95 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}
		}else{//ME以降
			wsprintf( buf, "Windows ME以降 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}
	}
	return buf;
}
//xp   =    5,     1,     2
//2000 = ma 5, mi  0, pid 2, sp3
//98se = ma 4, mi 10, pid 1,  a
//ME   = ma 4, mi 90, pid 1,  a


double	TimerCount=0;

int STD_timeGetTime(void)
{
	if(1){
		return timeGetTime();
	}else{
		return (int)TimerCount;
	}
}

// -----------------------------------------------------------------
//	ファイルを分割する。
// -----------------------------------------------------------------
FILE_PATH_INFO DivFile( FILE_PATH_INFO *fi, LPSTR str, ...)
{
	char		fname[1024];
	FILE_PATH_INFO	workFileInfo;

	wvsprintf( fname, str, (char*)( &str + 1) );

	if( fi == NULL) fi = &workFileInfo;

	_tsplitpath( fname, fi->Drive, fi->Dir, fi->Fname, fi->Ext );

	// szFilePathに実行モジュールのパスを作成
	_tcscpy( fi->FilePath, fi->Drive);
	_tcscat( fi->FilePath, fi->Dir);

	// szFileNameに実行モジュールのファイル名を作成
	_tcscpy( fi->FileName, fi->Fname);
	_tcscat( fi->FileName, fi->Ext);

	return workFileInfo;
}
