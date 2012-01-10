//------------------------------------------------------------------------
 //  Project Name   :  未定（仮）
  //  File Name      :  GM_Window.h
   //  Copyright      :  AQUA (Leaf)  since 2000 - 
    //                                 Programd by.  Miyakusa Masakazu
     //------------------------------------------------------------------------
      // ★アドベンチャーシステム管理（ゲーム寄り） ヘッダ

#ifndef	_GM_WINDOW_H_
#define _GM_WINDOW_H_


typedef struct{
	int		flag;
	int		type;

	int		cnt;
	int		mode;

	int		page;
	int		select;

	int		load_flag;
	int		save_flag;

	BOOL	file[10];
	int		new_no;
}SAVE_WINDOW;

extern SAVE_WINDOW		SaveWindow;

#define SW_SAVE_MODE	0
#define SW_LOAD_MODE	1
#define SW_LOAD_MODE2	2

extern void GWIN_SetSaveLoadWindow( int type, int bmp_set, int grp_set, int txt_set, int grp_bak, int txt_bak, int mus_lay, int mus_bak );
extern int  GWIN_ControlSaveLoadWindow(void);

extern void GWIN_SetSideBarWindow( int bmp_set, int grp_set, int txt_set, int mus_lay, int mus_bak );
extern int GWIN_ControlSideBarWindow(void);


extern void GWIN_SetSettingWindow( int bmp_set, int grp_set, int txt_set, int grp_bak, int txt_bak, int mus_lay, int mus_bak, int title );
extern BOOL GWIN_ControlSettingWindow(void);

#endif//_GM_WINDOW_H_
