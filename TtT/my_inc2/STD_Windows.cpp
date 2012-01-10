
 
  
   
    
     
      

#include "STD_Windows.h"
#include "STD_Debug.h"

#include <shlobj.h>
#include <math.h>

#include <TCHAR.H>

static HWND	MainHwnd=NULL;

void STDWIN_SetMainHwnd( HWND hwnd )
{
	MainHwnd = hwnd;
}









void memcpy2( void *dst, const void *src, int size )
{
	CopyMemory( dst,src,size );
	return ;

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
	}else if( SSE_FP_Flag ){
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
	}else if( MMX_Flag ){
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
int	GetBitCountDWORD( DWORD flag )
{
	int	count = 0;

	while( flag ) {
		DWORD	event	= flag & ( ( ~flag) + 1);
		flag	^= event;	
		count++;
	}
	return count;
}




int GetShift(int n)
{ 
	n--; 
	n  = (n & 0x55555555) + (n >> 1 & 0x55555555); 
	n  = (n & 0x33333333) + (n >> 2 & 0x33333333); 
	n  = ((n >> 4) + n) & 0x0f0f0f0f;
	n += ( n >> 8); 
	n += ( n >>16) & 0xff;

	return (n&31); 
}



DWORD	Pow2Tbl[2048];
WORD	SqrtTbl[0x10000];


void STD_InitTable(void)
{
	int	i;
	for(i=0; i<2048 ;i++)
		Pow2Tbl[i] = (i-1024)*(i-1024);
	for(i=0; i<0x10000 ;i++)
		SqrtTbl[i] = (WORD)(sqrt((double)i)*256);



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
		while( _ismbcdigit(CalStr[CalP]) ||
				('a'<=CalStr[CalP] && CalStr[CalP]<='f') ||
				('A'<=CalStr[CalP] && CalStr[CalP]<='F') ){
			buf[i++]=CalStr[CalP++];
		}
		buf[i] = '\0';
		digit = Str2Hex(buf);
	}else{
		while( _ismbcdigit(CalStr[CalP]) ){
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




void STD_SetWindowPos( HWND hd, int sx, int sy )
{


	RECT	rc;
	GetClientRect( hd, &rc );

	if( sx<0 ) sx=0;
	if( sy<0 ) sy=0;
	if( sx>GetSystemMetrics(SM_CXFULLSCREEN)-rc.right  ) sx = GetSystemMetrics(SM_CXFULLSCREEN)-rc.right;
	if( sy>GetSystemMetrics(SM_CYFULLSCREEN)-rc.bottom ) sy = GetSystemMetrics(SM_CYFULLSCREEN)-rc.bottom;
	
	SetWindowPos(hd,NULL, sx, sy, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOOWNERZORDER);
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

		if( size_a ){	
			for( i=0; i < size_a ; i++, buf++, temp++ )
				*buf = *temp;
		}
	}else{
		for( i=0; i < size_4 ; i++, buf+=8 ){
			*(double*)buf = *(double*)temp;
		}
		if( size_a ){	
			for( i=0; i < size_a ; i++, buf++, temp++ )
				*buf = *temp;
		}
	}
	return TRUE;
}



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

		if( size_a ){	
			for( i=0; i < size_a ; i++, buf++, temp++ )
				*buf = *temp;
		}
	}else{
		for( i=0; i < size24 ; i++, buf+=24 ){
			*(double*)(buf)    = *(double*)(temp);
			*(double*)(buf+8)  = *(double*)(temp+8);
			*(double*)(buf+16) = *(double*)(temp+16);
		}
		if( size_a ){	
			for( i=0; i < size_a ; i++, buf++, temp++ )
				*buf = *temp;
		}
	}
	return TRUE;
}



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

		if( size_a ){	
			*buf = *temp;
		}
	}else{
		for( i=0; i < size_2 ; i++, buf+=2 ){
			*(double*)buf = *(double*)temp;
		}
		if( size_a ){	
			*buf = *temp;
		}
	}

	return TRUE;
}



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




char *Digit2Space( int dg )
{
	static char	str[16];
	int			i;

	ZeroMemory( str, 16 );
	for(i=0;i<dg;i++) str[i] = ' ';

	return str;
}





int Str2Digit( char *str )
{
	int		i=0;
	int		minus = 1, digit=0;

	if( str== NULL   ) return 0;							
	if( str[0]=='\0' ) return 0;							
	if( str[0]=='-' ){	str++;	minus = -1;	}				


	i=0;
	while( str[i] && _ismbcdigit(str[i]) ){
		digit *= 10;
		digit += str[i]-'0';
		i++;
	}
	digit *= minus;
	return digit;
}



int Str2Hex( char *str )
{
	int		i=0;
	int		digit=0;
	char	str2[256];
	if( str== NULL   ) return 0;							
	if( str[0]=='\0' ) return 0;							

	strcpy(str2,str);
	while( str2[i] ){


		if( !_ismbcalnum(str2[i]) )
			return 0;	
		i++;
	}

	i=0;
	while( str2[i] ){
		digit *= 16;

		if( _ismbcdigit(str2[i]) )	digit += str[i]-'0';
		else					digit += str[i]-'a'+10;
		i++;
	}

	return digit;
}




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
int	Digit4keta( int digit )
{
	int	ret=0;

	while(digit){
		digit /= 10;
		ret++;
	}
	return max(1,ret);
	
}




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




void STD_strlower( char *dst, char *src )
{
	while( *src ){
		*(dst++) = (char)tolower( *(src++) );
	}
	*dst='\0';
}



void STD_strupper( char *dst, char *src )
{
	while( *src ){
		*(dst++) = (char)toupper( *(src++) );
	}
	*dst='\0';
}



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




int	STD_strcmp( char *str1, char *str2 )
{
	int	len = strlen( str2 );
	return strncmp( str1, str2, len );
}



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



int	STD_strcmpUL( char *str1, char *str2 )
{
	char	buf1[1024], buf2[1024];
	STD_strlower( buf1, str1 );
	STD_strlower( buf2, str2 );
	return strcmp( buf1, buf2 );
}



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



long STD_LimitLoop( long src, long max_digit )
{
	long	ret;

	ret = src%(max_digit*2);
	if( ret>=max_digit ){	ret = max_digit*2 - ret;	}
	return ret;
}



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
	
	for( k=0; k<=m; k++ ){	
		Nk=1;
		for ( j=0; j<=m; j++ ){	
			if (j==k) continue;	
			Nk *= (x - pt[j].x) / (pt[k].x - pt[j].x);
		}
		retval += Nk * pt[k].y;
	}
	
	return retval;
}


DWORD CPUIDMax    = 0;
DWORD CPUIDExtMax = 0;

char szVenderID[32] = "";
char szCpuName[64] = "";

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

DWORD fCpuClock = 0;

void GetCpuID(void)
{
	__asm{
		
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

		
		xor eax, eax
		cpuid
		mov [CPUIDMax], eax
		mov DWORD PTR [szVenderID+0], ebx
		mov DWORD PTR [szVenderID+4], edx
		mov DWORD PTR [szVenderID+8], ecx
		mov BYTE PTR [szVenderID+12], 0

		
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

		
		mov eax, 0x80000000
		cpuid
		mov [CPUIDExtMax], eax

		
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




void STD_CheckCPU( void )
{
	GetCpuID();
	GetCpuClock();

	DebugPrintf("VenderID  %s\n", szVenderID);
	DebugPrintf("CpuName   %s\n", szCpuName);
	DebugPrintf("TypeID %d, FamilyID %d, ModelID %d, SteppingID %d\n", TypeID, FamilyID, ModelID, SteppingID);
	DebugPrintf("CpuClock %dMHz\n", fCpuClock);

	DebugPrintf("\n");
	DebugPrintf("CPUID       %d\n", CPUID_Flag);		
	DebugPrintf("CPUIDMax    %x\n", CPUIDMax);		
	DebugPrintf("CPUIDExtMax %x\n", CPUIDExtMax);	

	DebugPrintf("FPU    %d\n", FPU_Flag);				
	DebugPrintf("TSC    %d\n", TSC_Flag);				
	DebugPrintf("MSR    %d\n", MSR_Flag);				
	DebugPrintf("CMOV   %d\n", CMOV_Flag);				
	DebugPrintf("MMX    %d\n", MMX_Flag);				
	DebugPrintf("3DNow! %d\n", _3DNow_Flag);			

	DebugPrintf("SSE_INT    %d\n", SSE_INT_Flag);		
	DebugPrintf("SSE_FP     %d\n", SSE_FP_Flag);		
	DebugPrintf("3DNow! Ext %d\n", _3DNowExt_Flag);		
	DebugPrintf("FCMOV      %d\n", FPU_Flag & CMOV_Flag);

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
		if( OSver.dwMajorVersion<=4 )		return 0;
		else if(OSver.dwMajorVersion==5)	return 1;
		else if(OSver.dwMajorVersion==6)	return 2;
		else								return 3;
	}else{
		if( OSver.dwMajorVersion<=2 )		return 256;
		else if(OSver.dwMajorVersion==3)	return 257;
		else if(OSver.dwMajorVersion==4)	return 258;
		else								return 260;
	}
}
char *STD_GetOSName( void )
{
	static char	buf[128];

	STD_CheckOS();
	if( OSver.dwPlatformId==VER_PLATFORM_WIN32_NT ){
		
		if( OSver.dwMajorVersion<=4 ){
			wsprintf( buf, "Windows NT%d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}else if(OSver.dwMajorVersion==5){
			if(OSver.dwMinorVersion==0){
				wsprintf( buf, "Windows 2000 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else if(OSver.dwMinorVersion==1){
				wsprintf( buf, "Windows XP %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else{
				wsprintf( buf, "Windows XP以降 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}
		}else {
			wsprintf( buf, "Windows XP以降 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}
	}else{
		if( OSver.dwMajorVersion<=3){
			wsprintf( buf, "Windows %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}else if(OSver.dwMajorVersion==4){
			if(OSver.dwMinorVersion>=90){
				wsprintf( buf, "Windows ME %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else if(OSver.dwMinorVersion>=10){
				wsprintf( buf, "Windows 98 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else{
				wsprintf( buf, "Windows 95 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}
		}else{
			wsprintf( buf, "Windows ME以降 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}
	}
	return buf;
}






double	TimerCount=0;

int STD_timeGetTime(void)
{
	if(1){
		return timeGetTime();
	}else{
		return (int)TimerCount;
	}
}




FILE_PATH_INFO DivFile( FILE_PATH_INFO *fi, LPSTR str, ...)
{
	char		fname[1024];
	FILE_PATH_INFO	workFileInfo;

	wvsprintf( fname, str, (char*)( &str + 1) );

	if( fi == NULL) fi = &workFileInfo;

	_splitpath( fname, fi->Drive, fi->Dir, fi->Fname, fi->Ext );

	
	_tcscpy( fi->FilePath, fi->Drive);
	_tcscat( fi->FilePath, fi->Dir);

	
	_tcscpy( fi->FileName, fi->Fname);
	_tcscat( fi->FileName, fi->Ext);

	return workFileInfo;
}
