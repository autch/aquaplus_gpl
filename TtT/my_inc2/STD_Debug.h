
 
  
   
    
     
      


#ifndef STD_DEBUG_H
#define STD_DEBUG_H

#include <windows.h>

extern void STD_SetMainHwnd( HWND hwnd );

extern void SetDebug(int tool);
extern BOOL GetDebug( void );

extern void DebugBox( HWND hwnd, char *mes, ...);
extern int  MessageBox2( HWND hwnd, DWORD flag, char *title, char *mes, ... );
extern int  CheckBox( HWND hwnd, char *title, char *mes, ... );
extern void DebugPrintf( char *mes, ...);
extern void GetPrintErrMessage( int err_code );

#endif 