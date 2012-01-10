#ifndef _DSHOW_CONFIG_H_
#define _DSHOW_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* registry stuff */
#define XVID_REG_KEY	HKEY_CURRENT_USER
#define XVID_REG_SUBKEY	"Software\\GNU\\XviD"
#define XVID_REG_CLASS	"config"

#define REG_GET_N(X, Y, Z) size=sizeof(int);if(RegQueryValueEx(hKey, X, 0, 0, (LPBYTE)&Y, &size) != ERROR_SUCCESS) {Y=Z;}
#define REG_GET_S(X, Y, Z) size=MAX_PATH;if(RegQueryValueEx(hKey, X, 0, 0, Y, &size) != ERROR_SUCCESS) {lstrcpy(Y, Z);}
#define REG_SET_N(X, Y) RegSetValueEx(hKey, X, 0, REG_DWORD, (LPBYTE)&Y, sizeof(int))
#define REG_SET_S(X, Y) RegSetValueEx(hKey, X, 0, REG_SZ, Y, lstrlen(Y)+1)


/* config struct */
#define SUPPORT_DX50		(1<<0)
#define SUPPORT_DIVX		(1<<1)
#define SUPPORT_MP4V		(1<<2)

#define FORCE_NONE  0
#define FORCE_YV12  1
#define FORCE_YUY2  2
#define FORCE_RGB24 3
#define FORCE_RGB32 4

typedef struct 
{
	int nBrightness;
	int nDeblock_Y;
	int nDeblock_UV;
	int nDering;
	int nFilmEffect;
	int nFlipVideo;
	int nForceColorspace;
	unsigned int supported_4cc;
	int videoinfo_compat;
} CONFIG;


/* global */
extern CONFIG g_config;


/* functions */
void LoadRegistryInfo();
void SaveRegistryInfo();
BOOL CALLBACK adv_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


#ifdef __cplusplus
}
#endif


#endif