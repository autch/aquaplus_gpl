
 
  
   
    
     
      

#include "STD_Debug.h"

static HWND	MainHwnd=NULL;

void STD_SetMainHwnd( HWND hwnd )
{
	MainHwnd = hwnd;
}

static int DebugFlag=0;
void SetDebug( int flag ){
	DebugFlag = flag;
}

BOOL GetDebug( void )
{
	return DebugFlag;
}




void DebugBox( HWND hwnd, char *mes, ... )
{
	char	buf[512];
	wvsprintf( buf, mes, (char *)(&mes+1) );
	MessageBox2( hwnd, MB_OK, "åxçê", buf );
}

int MessageBox2( HWND hwnd, DWORD flag, char *title, char *mes, ... )
{
	char	buf[512];
	if(hwnd==NULL) hwnd = MainHwnd;
	wvsprintf( buf, mes, (char *)(&mes+1) );
	return MessageBox( hwnd, buf, title, flag|MB_SETFOREGROUND );
}




void DebugPrintf( char *mes, ...)
{
	char	buf[512];

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

