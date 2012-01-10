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
extern DWORD SSE3_Flag;
extern DWORD fCpuClock;


int rndSelect(int max);
void itoaL(int value,WORD *workBuf);
int my_strcmpi(BYTE *str1,BYTE *str2);
int MaxMin(int max,int n);
float MaxMin(float max,float n);
float GetEffectStep(DWORD fadeTime,int fadeMax,BOOL bCntUp);
void mySetWindowText(HWND hWnd, const char *formatstring, ...);
void myOutputDebugString(const char *formatstring, ...);
void strcatfn( char *buf, char *mes, ...);
BOOL IsKanji(char *str);
BOOL myGetPath(char *in_str, char *out_str);
void CheckCPU( void );
int	EnumGraphicsDevice( void );
char *GetGraphicsDeviceName( int index );
int EnumSoundDevice( void );
char *GetSoundDeviceName( int index );
void GetCpuID(void);
DWORD GetCpuClock(void);
BOOL CheckOSw2k( void );
char *GetOSName( void );
DEVMODE *CheckScreenMode( void );
BOOL CheckScreenMode_BPP24( void );

#define ListAddString(HWD,IDC,STR)		SendDlgItemMessage(HWD,IDC,LB_ADDSTRING,0,(int)STR)	
inline float square(float x){ return (x*x); };
inline float lineLength(float x1,float y1,float x2,float y2){
	return sqrtf(square(x1-x2)+square(y1-y2));
}
inline float lineLength2(float x1,float y1,float x2,float y2){
	return float(square(x1-x2)+square(y1-y2));
}
inline float lineLength3(float x1,float y1,float z1,float x2,float y2,float z2){
	return sqrtf(square(x1-x2)+square(y1-y2)+square(z1-z2));
}
inline float nearLength(float x1,float y1,float x2,float y2){
	float x = fabsf(x1-x2);
	float y = fabsf(y1-y2);
	if(x < y)return(x+y-x/2);
	return (x+y-y/2);
}
inline float Length3D(float x1,float y1,float z1,float x2,float y2,float z2){
	return sqrtf(square(x1-x2)+square(y1-y2)+square(z1-z2));
}

inline int floatToInt(float val)
{
    val += 8388608.0f;
	return ( (* (int *) &val) & 0xffff );
}

inline BOOL IsHankaku(BYTE *str)
{
	if(*str<0x80 || (*str>=0xa0 && *str<=0xdf) )return TRUE;
	return FALSE;
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

inline myPtInRect(RECT *rect,POINT *point)
{
	if( (rect->left > point->x) || (rect->right <= point->x) ||
		(rect->top > point->y) || (rect->bottom <= point->y) )return FALSE;
	return TRUE;
}

#endif _MY_YMZUTILS
