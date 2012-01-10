
 
  
   
    
     
      

#ifndef	_PALETTE_H_
#define	_PALETTE_H_

#include <windows.h>
#include <bmp.h>

extern RGB32	GlobalPal[256];


extern BYTE		*PaletteIndex;
extern BYTE		(*BlendIndex)[32][256];
extern BYTE		(*AddIndex)[256];
extern BYTE		(*SubIndex)[256];
extern BYTE		(*MulIndex)[256];

extern HPALETTE	PAL_Initialize( void );										
extern void	PAL_Terminate( void );										

extern BYTE PAL_GetPaletteIndex( int r, int g, int b );
extern BOOL PAL_SetPaletteTable( void );
extern void PAL_ResetPaletteTable( void );

		
extern BOOL	PAL_SetEntry( RGB32* rgbPal, int top, int num );			
extern BOOL	PAL_AddEntry( RGB32* rgbPal, int num, LPBYTE lpChgTbl );	
extern BOOL PAL_GetEntry( RGB32* rgbPal, int top, int num );			

extern HPALETTE PAL_GetPalette( void );

		
extern BOOL	PAL_Select( HDC hDC );										
extern BOOL	PAL_Unselect( HDC hDC );									
extern BOOL	PAL_Realize( HDC hDC );										

#endif	