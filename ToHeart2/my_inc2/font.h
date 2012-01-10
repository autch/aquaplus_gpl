
 
  
   
    
     
      

#ifndef	_FONT_H_
#define _FONT_H_

extern void FNT_SetPackDir( char *pac_dir );

#define FONT08		0x01
#define FONT12		0x02
#define FONT16		0x04
#define FONT20		0x08
#define FONT24		0x10
#define FONT32		0x20
#define FONT40		0x40
#define FONT_ALL	0xff


extern HFONT FNT_SetFont( LPCTSTR face, int h, int w );
extern void FNT_TextOut( HWND hwnd, HFONT hFont, HBITMAP hbmp, int x, int y, int no );
extern void FNT_TextOutStr( HWND hwnd, HFONT hFont, HBITMAP hbmp, int x, int y, char *str );


extern void FNT_CreateFont( HWND hwnd, HBITMAP hbmp, BMP_T *vram, int font_no );


extern BOOL FNT_DrawChar( void *dest, int draw_mode, int dest_x, int dest_y, int moji, int fsize, RECT *clip, int r, int g, int b, int alph, int kage, int han );
extern void FNT_DrawText( void *dest, int draw_mode, int dest_x, int dest_y, int pich_w, int pich_h, char *str, int fsize, RECT *clip, int r, int g, int b, int alph, int kage );

extern void FNT_ChangeFont( int ftype );
extern void FNT_LoadFont( int fsize, int ftype );
extern void FNT_ReleaseFont( int fsize );

extern void FNT_Init( void );
extern void FNT_Term( void );

#endif