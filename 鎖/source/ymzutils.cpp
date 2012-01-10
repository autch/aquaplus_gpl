#include "ClCommon.h"
#include <ddraw.h>
#include <dsound.h>

int rndSelect(int max)
{
	int ret;

	if(max == 0)return 0;

	ret = rand()*max / RAND_MAX;
	if(ret>=max)ret = max -1;
	return (ret);
} // rndSelect

void itoaL(int value,WORD *workBuf)
{
	SHORT	count=1,buf[2];
	int	i=10;
	
	while(value >= i){
		count ++;
		i *= 10;
	}
	lstrcpy((char *)buf,"ÇO");
	for(i=count-1;i>=0;i--){
		workBuf[i] = buf[0] + (value%10 << 8);
		value /= 10;
	}
	workBuf[count] = '\0';
} // itoaL

int my_strcmpi(BYTE *str1,BYTE *str2)
{
	int		i,len1,len2,min,diff,c[2];
	min = len1 = lstrlen((LPCSTR)str1);
	len2 = lstrlen((LPCSTR)str2);
	if(len2 < min)min = len2;
	for(i=0;i<min;i++){
		c[0] = str1[i];
		if(c[0]>=0x41 && c[0]<=0x5A)c[0] += 0x20;
		c[1] = str2[i];
		if(c[1]>=0x41 && c[1]<=0x5A)c[1] += 0x20;
		diff = c[0] - c[1];
		if(0==diff)continue;
		return (diff);
	}
	return (len1-len2);
} // my_strcmpi

int MaxMin(int max,int n)
{
	if(n < 0)
		return 0;
	if(n > max)
		return max;

	return n;
} //MaxMin

float MaxMin(float max,float n)
{
	if(n < 0)
		return 0;
	if(n > max)
		return max;

	return n;
} //MaxMin

BOOL IsKanji(char *str)
{
	if( BYTE(str[0])>=0x81 && BYTE(str[1])>=0x40 ) return TRUE;
	return FALSE;
}

BOOL myGetPath(char *in_str, char *out_str)
{
	int		i,lastPath = 0;

	for(i=0;i<(int)strlen(in_str);i++){
		if(in_str[i]=='\\'){
			lastPath = i;
		}
		if( IsKanji(&in_str[i]) ){
			i++;
		}
	}
	if(0==lastPath)return FALSE;
	strncpy(out_str,in_str,lastPath+1);
	out_str[lastPath+1] = 0;
	return TRUE;
}

float GetEffectStep(DWORD fadeTime,int fadeMax,BOOL bCntUp)
{
	float	alpha;
	DWORD	time = timeGetTime();

	if(time >= fadeTime){
		alpha = 0;
	}else{
		alpha = (fadeTime-time) /float(fadeMax);
	}
	if(bCntUp)alpha = 1.0f -alpha;
	return alpha;
} // GetEffectStep


void mySetWindowText(HWND hWnd, const char *formatstring, ...)
{
	char buff[255];
	va_list args;
	va_start(args, formatstring);
	_vsnprintf( buff, sizeof(buff), formatstring, args);
	SetWindowText(hWnd,buff);
	va_end(args);
} // mySetWindowText

void strcatfn( char *buf, char *mes, ...)
{
	char	buf2[256];
	int	len = wvsprintf( buf2, mes, (char *)(&mes+1));
	buf2[len]   = 0x0d;
	buf2[len+1] = 0x0a;
	buf2[len+2] = '\0';
	strcat(buf,buf2);
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

		test ecx, 1
		setnz al
		mov [SSE3_Flag], eax

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
} // GetCpuID

DWORD GetCpuClock(void)
{
	DWORD dwCpuClock;
	DWORD dwMsCount;

	if (TSC_Flag == 0)
		return 0;

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
	return fCpuClock;
} // GetCpuClock

void CheckCPU( void )
{
	GetCpuID();
	GetCpuClock();
} // CheckCPU


#define MAX_DEVICES     32

DDDEVICEIDENTIFIER2	DeviceInfo[MAX_DEVICES];
int					g_iMaxDevices = 0;

BOOL WINAPI DDEnumCallback( GUID *pGUID, LPSTR pDescription, LPSTR strName,
                              LPVOID pContext )
{
	LPDIRECTDRAW7 pDD = NULL;	
	HRESULT hr;

	if( FAILED( hr = DirectDrawCreateEx( pGUID, (VOID**)&pDD, 
										 IID_IDirectDraw7, NULL ) ) )
		return DDENUMRET_CANCEL;

	pDD->GetDeviceIdentifier( &DeviceInfo[g_iMaxDevices], 0 );
	if(pDD) {
		pDD->Release();
		pDD = NULL;
	}

	if( g_iMaxDevices < MAX_DEVICES )
		g_iMaxDevices++;
	else
		return DDENUMRET_CANCEL;

	return DDENUMRET_OK;
}

int	EnumGraphicsDevice( void )
{
	LPDIRECTDRAWENUMERATEEX pDirectDrawEnumerateEx=NULL;
	HINSTANCE               hDDrawDLL = NULL;

	hDDrawDLL = GetModuleHandle("DDRAW");
	if( NULL == hDDrawDLL ){
		MessageBox( NULL, "LoadLibrary() FAILED", 
					"DirectDraw", MB_OK | MB_ICONERROR );
		return NULL;
	}

	g_iMaxDevices=0;
	DirectDrawEnumerate( DDEnumCallback, NULL );

	if( 0 == g_iMaxDevices ){
		MessageBox( NULL, "No devices to enumerate.", 
					"DirectDraw Sample", MB_OK | MB_ICONERROR );
		return NULL;
	}
	FreeLibrary( hDDrawDLL );
	return g_iMaxDevices;
}

char *GetGraphicsDeviceName( int index )
{
	return DeviceInfo[index].szDescription;
}

static int	SoundDeviceNum=0;
static char	SoundDeviceName[MAX_DEVICES][256];

INT_PTR CALLBACK DSoundEnumCallback( GUID* pGUID, LPSTR strDesc, LPSTR strDrvName,
                                  VOID* pContext )
{
	if(SoundDeviceNum>=MAX_DEVICES) return FALSE;
	if(pGUID){
		strcpy( SoundDeviceName[SoundDeviceNum++], strDesc );
	}
	return TRUE;
}

int EnumSoundDevice( void )
{
	SoundDeviceNum=0;
	if( DS_OK != DirectSoundEnumerate( (LPDSENUMCALLBACK)DSoundEnumCallback, NULL ) ){
		return 0;
	}
	return SoundDeviceNum;
}

char *GetSoundDeviceName( int index )
{
	return SoundDeviceName[index];
}

OSVERSIONINFO	OSver;

OSVERSIONINFO CheckOS( void )
{
	OSver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &OSver );
	return OSver;
} // CheckOS

BOOL CheckOSw2k( void )
{
	CheckOS();
	if( OSver.dwPlatformId==VER_PLATFORM_WIN32_NT ){
		if( OSver.dwMajorVersion>4 ){
			return TRUE;
		}
	}
	return FALSE;
} // CheckOSw2k

int GetOSVer( void )
{
	CheckOS();
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
} // GetOSVer
char *GetOSName( void )
{
	static char	buf[128];

	CheckOS();
	if( OSver.dwPlatformId==VER_PLATFORM_WIN32_NT ){
		
		if( OSver.dwMajorVersion<=4 ){
			wsprintf( buf, "Windows NT%d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}else if(OSver.dwMajorVersion==5){
			if(OSver.dwMinorVersion==0){
				wsprintf( buf, "Windows 2000 %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else if(OSver.dwMinorVersion==1){
				wsprintf( buf, "Windows XP %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}else{
				wsprintf( buf, "Windows XPà»ç~ %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
			}
		}else {
			wsprintf( buf, "Windows XPà»ç~ %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
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
			wsprintf( buf, "Windows MEà»ç~ %d.%d (Build:%d) %s ", OSver.dwMajorVersion, OSver.dwMinorVersion, OSver.dwBuildNumber, OSver.szCSDVersion );
		}
	}
	return buf;
} // GetOSName

int			DevModeNum = 0;
DEVMODE		*DevModeStruct = NULL;
int			DispFreq[16];

DEVMODE *CheckScreenMode( void )
{
	DEVMODE		Dummy;
	
	DevModeNum=0;
	while( EnumDisplaySettings(NULL, DevModeNum, &Dummy ) ){
		DevModeNum++;
	}
	cl_free(DevModeStruct);
	DevModeStruct = (DEVMODE*)cl_malloc( sizeof(DEVMODE)*(DevModeNum+1) );
	ZeroMemory(DevModeStruct,sizeof(DEVMODE)*(DevModeNum+1));
	DevModeNum=0;
	while( EnumDisplaySettings(NULL, DevModeNum, &DevModeStruct[DevModeNum]) ){
		DevModeNum++;
	}
	return DevModeStruct;
} // CheckScreenMode

BOOL CheckScreenMode_BPP24( void )
{
	int			iModeNum = 0;
	DEVMODE		DevMode;
	BOOL		ret = FALSE;

	while( EnumDisplaySettings(NULL, iModeNum, &DevMode) )
	{
		iModeNum++;
		if(DevMode.dmBitsPerPel==24){
			ret = TRUE;
			return ret;
		}
	}
	return ret;
} // CheckScreenMode_BPP24
