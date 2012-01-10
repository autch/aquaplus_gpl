#ifndef  _MY_KEYINPUT
#define  _MY_KEYINPUT

BOOL InitDirectInput(HINSTANCE hInstApp);
void clearPadState(void);
void clearAllPushData(void);
void lookPadState(void);
void lookPadRepeat(void);

#define	MAX_DEV		6
#define	INPUT_LEFTRIGHT			1L
#define	INPUT_UPDOWN			2L
#define	INPUT_LEFT				3L
#define	INPUT_RIGHT				4L
#define	INPUT_UP				5L
#define	INPUT_DOWN				6L

#define	INPUT_ACTION			7L		
#define INPUT_CANCEL			8L	
#define	INPUT_CAMP				9L		
#define INPUT_SHIFT				10L	
#define	INPUT_LEFTTRIGGER		11L		
#define	INPUT_RIGHTTRIGGER		12L	
#define	INPUT_MAP				13L		
#define	INPUT_SKIP				14L	
#define	INPUT_DEBUG				15L		
#define	INPUT_END				16L	
#define	INPUT_LEFTUP			17L
#define	INPUT_RIGHTUP			18L
#define	INPUT_LEFTDOWN			19L
#define	INPUT_RIGHTDOWN			20L
#define INPUT_MOUSEA			21L
#define INPUT_MOUSEB			22L
#define INPUT_MOUSEC			23L

extern LPDIRECTINPUT8			lpDInput;
extern LPDIRECTINPUTDEVICE8		g_pDIDevice[MAX_DEV];
extern int g_iDIDeviceNum;

#endif _MY_KEYINPUT
