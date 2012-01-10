
 
  
   
    
     
      

#ifndef	_D3DFONT_H_
#define _D3DFONT_H_

#include "d3d_draw.h"

extern void D3DFNT_SetPackDir( char *pac_dir );

#define FONT08		0x01
#define FONT12		0x02
#define FONT16		0x04
#define FONT20		0x08
#define FONT24		0x10
#define FONT32		0x20
#define FONT40		0x40
#define FONT_ALL	0xff


extern HFONT D3DFNT_SetFont( LPCTSTR face, int h, int w );
extern void D3DFNT_TextOut( HWND hwnd, HFONT hFont, HBITMAP hbmp, int x, int y, int no );
extern void D3DFNT_TextOutStr( HWND hwnd, HFONT hFont, HBITMAP hbmp, int x, int y, char *str );




extern BOOL D3DFNT_UpdateTextChar( D3DD_TEXTURE *d3dTex, int dest_x, int dest_y, WORD moji, int fsize, int r, int g, int b, int kage );
extern BOOL D3DFNT_UpdateText( D3DD_TEXTURE *d3dTex, int dest_x, int dest_y, int pitch_w, int pitch_h, BYTE *str, int fsize, int r, int g, int b, int kage );


extern BOOL D3DFNT_DrawChar( void *dest, int draw_mode, int dest_x, int dest_y, int moji, int fsize, RECT *clip, int r, int g, int b, int alph, int kage, int han );
extern void D3DFNT_DrawText( void *dest, int draw_mode, int dest_x, int dest_y, int pich_w, int pich_h, BYTE *str, int fsize, RECT *clip, int r, int g, int b, int alph, int kage );

extern void D3DFNT_ChangeFont( int ftype );
extern void D3DFNT_LoadFont( int fsize, int ftype );
extern void D3DFNT_ReleaseFont( int fsize );

extern void D3DFNT_Init( void );
extern void D3DFNT_Term( void );

#endif