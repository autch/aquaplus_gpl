#ifndef _MY_YMZUTILS
#define _MY_YMZUTILS

extern DWORD		progFlag[5];
extern DEVMODE		*DevModeStruct;
extern int			DispFreq[16];
extern DWORD CPUIDMax;
extern DWORD CPUIDExtMax;
extern char szVenderID[13];
extern char szCpuName[49];
extern DWORD TypeID;
extern DWORD FamilyID;
extern DWORD ModelID;
extern DWORD SteppingID;
extern DWORD CPUID_Flag;
extern DWORD FPU_Flag;
extern DWORD TSC_Flag;
extern DWORD MSR_Flag;
extern DWORD CMOV_Flag;
extern DWORD MMX_Flag;
extern DWORD _3DNow_Flag;
extern DWORD SSE_INT_Flag;
extern DWORD SSE_FP_Flag;
extern DWORD _3DNowExt_Flag;
extern DWORD SSE2_Flag;
extern DWORD fCpuClock;


int rndSelect(int max);
void itoaL(int value,WORD *workBuf);
void maketable2(float p[], float x[], float y[],float a[], float b[],int N);
void spline2(float t, float *px, float *py,float p[], float x[], float  y[],float a[], float b[],int N);
void maketable3(float p[], float x[], float y[], float z[],float a[], float b[], float c[], int N);
void spline3(float t, float *px, float *py, float *pz, float p[], float x[], float y[], float z[], float a[], float b[], float c[], int N);
BOOL checkLineToCircle(float x1,float y1,float x2,float y2,float cx,float cy,float r);
BOOL checkTwoLine(float mx,float my,float nx,float ny,float px,float py,float qx,float qy,float *s=NULL);
int my_strcmpi(BYTE *str1,BYTE *str2);
int MaxMin(int max,int n);
float MaxMin(float max,float n);
float GetEffectStep(DWORD fadeTime,int fadeMax,BOOL bCntUp);
void mySetWindowText(const char *formatstring, ...);
void myOutputDebugString(const char *formatstring, ...);
void strcatfn( char *buf, char *mes, ...);
void CheckCPU( void );
int	EnumGraphicsDevice( void );
char *GetGraphicsDeviceName( int index );
int EnumSoundDevice( void );
char *GetSoundDeviceName( int index );
BOOL CheckOSw2k( void );
char *GetOSName( void );
DEVMODE *CheckScreenMode( void );
void CheckScreenMode_BPP24( void );

void SetWindowCursorPos(HWND hWnd,int x,int y);
void SetFont(HDC hDC,int sizeH,int sizeW);
BOOL WinExec2(LPCSTR exePath,LPCSTR fullPath);
void SetWallPaper(char *str);

inline int floatToInt(float val)
{
    val += 8388608.0f;
	return ( (* (int *) &val) & 0xffff );
}

inline BOOL CopyMemOffset(void *dst, char **src, size_t size)
{
	CopyMemory(dst,*src,size);
	(*src) += size;
	return TRUE;
}


inline BOOL look1BitFlag(BYTE *buf,int flagNo)
{
	WORD j = flagNo >> 3;
	BYTE i = flagNo & 0x7;
	BYTE k = 1;
	k = k << i;
	if( (buf[j] & k) )
		return(on);
	else
		return(off);
}

inline void set1BitFlag(BYTE *buf,int flagNo,OnOff sw)
{
	WORD j = flagNo >> 3;
	BYTE i = flagNo & 0x7;
	BYTE k = 1;
	k = k << i;
	if(sw==on){
		buf[j] |= k;
	}else{
		k = ~k;
		buf[j] &= k;
	}
}

#endif _MY_YMZUTILS
