#ifndef CONTROL_H
#define CONTROL_H

#include <CommCtrl.h>


#define ListReset(HWD,IDC)					SendDlgItemMessage(HWD,IDC,LB_RESETCONTENT,0,0)				
#define ListGetMaxCount(HWD,IDC)			SendDlgItemMessage(HWD,IDC,LB_GETCOUNT,0,0)					

#define ListAddFile(HWD,IDC,FILE)			SendDlgItemMessage(HWD,IDC,LB_ADDFILE,0,(int)FILE)			
#define ListAddDirFile(HWD,IDC,FLG,FILE)	SendDlgItemMessage(HWD,IDC,LB_DIR,FLG,FILE)					

#define ListSetString(HWD,IDC,INDEX,STR)	ListDelString(HWD,IDC,INDEX),ListInsertString(HWD,IDC,INDEX,STR)	
#define ListAddString(HWD,IDC,STR)			SendDlgItemMessage(HWD,IDC,LB_ADDSTRING,0,(int)STR)			
#define ListInsertString(HWD,IDC,INDEX,STR)	SendDlgItemMessage(HWD,IDC,LB_INSERTSTRING,INDEX,(int)STR)	
#define ListDelString(HWD,IDC,INDEX)		SendDlgItemMessage(HWD,IDC,LB_DELETESTRING,INDEX,0)			
#define ListFindString(HWD,IDC,STR)			SendDlgItemMessage(HWD,IDC,LB_FINDSTRING,-1,(int)STR)		
#define ListGetString(HWD,IDC,INDEX,BUF)	SendDlgItemMessage(HWD,IDC,LB_GETTEXT,INDEX,(int)BUF)		

#define ListGetSel(HWD,IDC,INDEX)			SendDlgItemMessage(HWD,IDC,LB_GETSEL,INDEX,0)				
#define ListSetSelString(HWD,IDC,STR)		SendDlgItemMessage(HWD,IDC,LB_SELECTSTRING,-1,(int)STR)		
#define ListGetSelLast(HWD,IDC)				SendDlgItemMessage(HWD,IDC,LB_GETANCHORINDEX,0,0)			

#define ListGetSelIndex(HWD,IDC)			SendDlgItemMessage(HWD,IDC,LB_GETCURSEL,0,0)				
#define ListSetSelIndex(HWD,IDC,INDEX)		SendDlgItemMessage(HWD,IDC,LB_SETCURSEL,INDEX,0)			

#define ListGetSelCount(HWD,IDC)			SendDlgItemMessage(HWD,IDC,LB_GETSELCOUNT,0,0)				
#define ListGetSelList(HWD,IDC,MAX,BUF)		SendDlgItemMessage(HWD,IDC,LB_GETSELITEMS,MAX,(int)BUF)		
#define ListSetSel(HWD,IDC,FLG,INDEX)		SendDlgItemMessage(HWD,IDC,LB_SETSEL,FLG,INDEX)				
#define ListSetSelRange(HWD,IDC,FLG,FI,LA)	SendDlgItemMessage(HWD,IDC,LB_SELITEMRANGE,FLG,MAKELPARAM(FI,LA))	
#define ListSetSelAll(HWD,IDC,FLG)			ListSetSel(HWD,IDC,FLG,-1)									

extern int ListGetSelListEx( HWND hwnd, int idc, int **buf );


#define ComboReset(HWD,IDC)						SendDlgItemMessage(HWD,IDC,CB_RESETCONTENT,0,0)				
#define ComboGetMaxCount(HWD,IDC)				SendDlgItemMessage(HWD,IDC,CB_GETCOUNT,0,0)					

#define ComboSetString(HWD,IDC,INDEX,STR)		ComboDelString(HWD,IDC,INDEX),ComboInsertString(HWD,IDC,INDEX,STR)	
#define ComboAddString(HWD,IDC,STR)				SendDlgItemMessage(HWD,IDC,CB_ADDSTRING,0,(int)STR)			
#define ComboInsertString(HWD,IDC,INDEX,STR)	SendDlgItemMessage(HWD,IDC,CB_INSERTSTRING,INDEX,(int)STR)	
#define ComboDelString(HWD,IDC,INDEX)			SendDlgItemMessage(HWD,IDC,CB_DELETESTRING,INDEX,0)			
#define ComboFindString(HWD,IDC,STR)			SendDlgItemMessage(HWD,IDC,CB_FINDSTRING,-1,(int)STR)		

#define ComboSelectString(HWD,IDC,STR)			SendDlgItemMessage(HWD,IDC,CB_SELECTSTRING,-1,(int)STR)		
#define ComboGetSelIndex(HWD,IDC)				SendDlgItemMessage(HWD,IDC,CB_GETCURSEL,0,0)				
#define ComboSelectIndex(HWD,IDC,INDEX)			SendDlgItemMessage(HWD,IDC,CB_SETCURSEL,INDEX,0)			


#define SliderSetRange(HWD,IDC,MIN,MAX)		SendDlgItemMessage(HWD,IDC,TBM_SETRANGE,0,MAKELONG(MIN,MAX) )	
#define SliderSetPos(HWD,IDC,POS)			SendDlgItemMessage(HWD,IDC,TBM_SETPOS,1,POS)					
#define SliderSetPage(HWD,IDC,PAGE)			SendDlgItemMessage(HWD,IDC,TBM_SETPAGESIZE,0,PAGE)				
#define SliderSetLine(HWD,IDC,LINE)			SendDlgItemMessage(HWD,IDC,TBM_SETLINESIZE,0,LINE)				
#define SliderSetTick(HWD,IDC,TIC)			SendDlgItemMessage(HWD,IDC,TBM_SETTICFREQ,TIC,0)				

#define SliderGetRangeMax(HWD,IDC)			SendDlgItemMessage(HWD,IDC,TBM_GETRANGEMAX,0,0)				
#define SliderGetRangeMin(HWD,IDC)			SendDlgItemMessage(HWD,IDC,TBM_GETRANGEMIN,0,0)				
#define SliderGetPos(HWD,IDC)				SendDlgItemMessage(HWD,IDC,TBM_GETPOS,0,0)					
#define SliderGetPage(HWD,IDC)				SendDlgItemMessage(HWD,IDC,TBM_GETPAGESIZE,0,0)				
#define SliderGetLine(HWD,IDC)				SendDlgItemMessage(HWD,IDC,TBM_GETLINESIZE,0,0)				
#define SliderGetTick(HWD,IDC)				SendDlgItemMessage(HWD,IDC,TBM_GETTICFREQ,0,0)				

extern void SetSliderParam( HWND hwnd, int idc_no, int max, int page, int start );
extern void SetSliderParamEx( HWND hwnd, int idc_no, int min, int max, int page, int line, int tick, int start );


#define SpinSetRange(HWD,IDC,MAX,MIN)		SendDlgItemMessage(HWD,IDC,UDM_SETRANGE,0,MAKELONG(MAX,MIN) )				
#define SpinSetPos(HWD,IDC,POS)				SendDlgItemMessage(HWD,IDC,UDM_SETPOS,0,MAKELONG(POS,0))					
#define SpinSetBuddy(HWD,IDCS,IDCE)			SendDlgItemMessage(HWD,IDCS,UDM_SETBUDDY,(UINT)GetDlgItem(HWD,IDCE),0)		
#define SpinSetPosB(HWD,IDC,POS)			SendMessage(SpinGetBuddy(HWD,IDC),WM_SETTEXT,0,(LPARAM)Digit2Str(POS,0,0))	

#define SpinSetIncB(HWD,IDC)				SpinSetPosB(HWD,IDC,min(SpinGetPos(HWD,IDC)+1,SpinGetRangeMax(HWD,IDC)))
#define SpinSetDecB(HWD,IDC)				SpinSetPosB(HWD,IDC,max(SpinGetPos(HWD,IDC)-1,SpinGetRangeMin(HWD,IDC)))

#define SpinGetRangeMax(HWD,IDC)			((short)LOWORD(SendDlgItemMessage(HWD,IDC,UDM_GETRANGE,0,0)))		
#define SpinGetRangeMin(HWD,IDC)			((short)HIWORD(SendDlgItemMessage(HWD,IDC,UDM_GETRANGE,0,0)))		
#define SpinGetPos(HWD,IDC)					((short)SendDlgItemMessage(HWD,IDC,UDM_GETPOS,0,0))					
#define SpinGetBuddy(HWD,IDC)				((HWND)SendDlgItemMessage(HWD,IDC,UDM_GETBUDDY,0,0))		


#define	MenuSetCheck(HWD,ID,FLG)		CheckMenuItem(GetMenu(HWD),ID,FLG)	
#define	MenuSetEnable(HWD,ID,FLG)		EnableMenuItem(GetMenu(HWD),ID,FLG)	
#define	MenuGetState(HWD,ID)			GetMenuState(GetMenu(HWD),ID)		
#define	MenuGetString(HWD,ID,STR,SIZE)	GetMenuString(GetMenu(HWD),ID,STR,SIZE,MF_BYCOMMAND)

extern void SetMenuStr( HWND hwnd, int id, char *str );
extern void SetNewMenuStr( HWND hwnd, int id, int new_id, char *str );
extern void InsertMenuStr( HWND hwnd, int ins_id, int id, char *str );
extern void ResetMenu( HWND hwnd, int id, char *str );


#define	ControlSetCheck(HWD,IDC,CHK)		SendDlgItemMessage(HWD,IDC,BM_SETCHECK,CHK,0)	
#define	ControlGetCheck(HWD,IDC)			SendDlgItemMessage(HWD,IDC,BM_GETCHECK,0,0)		

#define ControlSetEnable(HWD,IDC,FLG)		EnableWindow(GetDlgItem(HWD,IDC),FLG)			
#define ControlGetEnable(HWD,IDC)			IsWindowEnabled(GetDlgItem(HWD,IDC))			


#endif 