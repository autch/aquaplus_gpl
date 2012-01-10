
 
  
   
    
     
      

#ifndef _MOUSE_H_
#define _MOUSE_H_

#define MOUSE_LAYER_MAX		48
#define MOUSE_REST_MAX		64

#define MOUSE_FREE			-1
#define MOUSE_LBUTTON		0
#define MOUSE_RBUTTON		1
#define MOUSE_MBUTTON		2

#define MOUSE_LBTRIGGER		8
#define MOUSE_RBTRIGGER		9
#define MOUSE_MBTRIGGER		10

extern void MUS_SetClickThrough( void );

extern void MUS_InitMouse( int disp_w, int disp_h );
extern void MUS_SetFlag( int flag );
extern void MUS_RenewWheel( short wheel );
extern void MUS_RenewMouseButton( int button, int flag );
extern void MUS_RenewMouse( HWND hwnd, int active, int Flag_MenuSelected );


extern int MUS_GetMouseWheel( void );
extern BOOL MUS_GetMouseButton( int button );
extern BOOL MUS_GetMouseButton2( int button );
extern BOOL MUS_GetMouseTrigger( int button );
extern BOOL MUS_GetMouseTrigger2( int button );
extern BOOL MUS_GetMouseBTrigger( int button );
extern BOOL MUS_GetMouseBTrigger2( int button );
extern BOOL MUS_GetMouseOffTrigger( int button );
extern BOOL MUS_GetMouseOffTrigger2( int button );

extern int	MUS_GetMouseNo( int buttom );
extern int	MUS_GetMouseNoEx( int buttom, int lno );
extern int	MUS_GetMouseDir( int buttom );
extern int	MUS_GetMouseDragPos( int button, int *mx, int *my );
extern int	MUS_GetMouseDragMove( int button, int *mx, int *my );
extern void MUS_GetMousePos( int *mx, int *my );
extern int MUS_GetMousePosX( void );
extern int MUS_GetMousePosY( void );

extern int	MUS_GetMousePosCustom( int *mx, int *my,int sx,int sy,int pit_w,int pit_h,int w,int h );
extern int	MUS_GetMousePosCustom2( int *mx, int *my,int sx,int sy,int pit_w,int pit_h,int pit2_w,int pit2_h,int w,int h );

extern int	MUS_GetMouseInterval( void );
extern int	MUS_GetMouseRectFlag( int lno, int no );

extern void MUS_SetMousePos( HWND hwnd, int x, int y );
extern void MUS_SetMousePosRect( HWND hwnd, int lno, int no );
extern void MUS_SetSomeTimeClickFlag( int flag );
extern void MUS_SetMouseLayer( int lno );
extern int MUS_GetMouseLayer( void );

extern void MUS_SetMouseRect( int lno,int no, int sx, int sy, int w, int h,int flag );
extern BOOL MUS_GetMouseRect( int lno,int no, int *sx=NULL, int *sy=NULL, int *w=NULL, int *h=NULL );

extern void MUS_SetMouseRectFlag( int lno, int no, int flag );
extern void MUS_ResetMouseRect( int lno,int no );
extern void MUS_ResetMouseRect_Layer( int lno );
extern void MUS_ResetMouseRect_All( void );

extern void MUS_SetMouseRect_Disp( int lno );
extern void MUS_ResetMouseRect_Disp( void );
extern void MUS_SetMouseRect_DispActive( int lno );

extern void MUS_ShowCursor( int disp );

#endif