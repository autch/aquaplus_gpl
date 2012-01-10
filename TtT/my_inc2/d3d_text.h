
 
  
   
    
     
      

#ifndef	_D3DTEXT_H_
#define _D3DTEXT_H_

#include "d3d_font.h"

extern int D3DTXT_DrawText( D3DD_TEXTURE *d3dTex, int sx, int sy, int w, int h, int pixh_w, int pich_h, int font, char *str, int kage, int r=255, int g=255, int b=255, int kr=0, int kg=0, int kb=0 );
extern int D3DTXT_DrawTextEx( D3DD_TEXTURE *d3dTex, int sx, int sy, int w, int h, int pixh_w, int pich_h, int font, char *str, int r, int g, int b, int kage );

extern int D3DTXT_DispText( D3DD_DISP *d3dd, int sx, int sy, int w, int h, int pixh_w, int pich_h, int *px2, int *py2, RECT *clip,
					int font, char *str, int text_cnt, int step_cnt, int r, int g, int b, int alph, int kage, int cnt_flag, char *normal_str, int *step_wait );



#endif