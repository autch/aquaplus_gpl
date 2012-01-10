
 
  
   
    
     
      

#ifndef	_TEXT_H_
#define _TEXT_H_

#include "font.h"

enum{
	FCT_NORMAL=0,	
	FCT_GLAY,		
	FCT_DARK,		
	FCT_BLACK,		
	
	FCT_RED,		
	FCT_GREEN,		
	FCT_BLUE,		
	FCT_YELLOW,		

	FCT_CYAN,		
	FCT_PURPLE,		
	FCT_ORENGE,		
	FCT_PASTEL,		
	
	FCT_VIOLET,		
	FCT_OLIVE,		
	FCT_MARIN,		
	FCT_PINK,		

	FCT_USER01=16,		
	FCT_USER02,		
	FCT_USER03,		
	FCT_USER04,		
	FCT_USER05,		
	FCT_USER06,		
};

extern int TXT_GetTextCountEnter( char *str );

extern int TXT_GetTextEndKeyWait( char *str );

extern int TXT_GetTextDispStr( char *str, char *normal_str );
extern int TXT_GetTextCount( char *str, int step_cnt );

extern int TXT_DrawText(void *dest,int draw_mode2, int sx, int sy, int w, int h, int pixh_w, int pich_h, int *px2, int *py2, RECT *clip,
						int font, char *str, int color, int text_cnt, int step_cnt, int r, int g, int b, int alph, int kage, int kaigyou_musi=FALSE );
extern int TXT_DrawTextEx(	void *dest,int draw_mode2, int sx, int sy, int w, int h, int pixh_w, int pich_h, int *px2, int *py2, RECT *clip,
							int font, char *str, int color, int text_cnt, int step_cnt, int r, int g, int b, int alph, int kage, int cnt_flag, char *normal_str=NULL, int kaigyou_musi=FALSE );

#endif