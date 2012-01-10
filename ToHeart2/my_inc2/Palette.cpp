
 
  
   
    
     
      

#include "mm_std.h"




#if	DIRECT_DRAW
#include "ddlib.h"
#endif

#include "Palette.h"
#include "Draw.h"


PALETTEENTRY	*lpPalData = NULL;								
char			*lpUseTbl  = NULL;								
HPALETTE		hPalette   = NULL;								
HPALETTE		hPrevPal   = NULL;								


	#define USE_PAL		256

RGB32		GlobalPal[256];

char		*PaletteIndex = NULL;
char		(*BlendIndex)[32][256] = NULL;
char		(*AddIndex)[256] = NULL;
char		(*SubIndex)[256] = NULL;
char		(*MulIndex)[256] = NULL;








HPALETTE PAL_Initialize( void )
{
	int		count;
	HWND	hWnd = NULL;
	HDC		hDC;
	
	struct {
		WORD         palVersion;
		WORD         palNumEntries;
		PALETTEENTRY palPalEntry[256];
	}logpal = { 0x300, 256 };

	
	
	PAL_Terminate();

	
	lpPalData = (PALETTEENTRY *)GAlloc( sizeof(PALETTEENTRY)*256 );
	if( lpPalData == NULL )	return FALSE ;

	
	hDC = GetDC( hWnd );
	GetSystemPaletteEntries( hDC, 0, 256, lpPalData );
	

	ReleaseDC( hWnd, hDC );

	for( count=0; count<256; count++ )	lpPalData[count].peFlags = 0;
	ZeroMemory( &lpPalData[10], sizeof( PALETTEENTRY )*236 );

	
	lpUseTbl = (char *)GAlloc( sizeof(char)*256 );
	if( lpUseTbl == NULL )	return FALSE ;
	ZeroMemory( lpUseTbl, 256 );
	for( count=0; count<10; count++ ){	lpUseTbl[count] = 0xff;	lpUseTbl[246+count] = 0xff;	}
	lpUseTbl[10] = 0xff;


	
	CopyMemory( logpal.palPalEntry, lpPalData, sizeof(PALETTEENTRY)*256 );
	if( (hPalette = CreatePalette( (LOGPALETTE*)&logpal )) == NULL )	return FALSE;

	return hPalette;
}





void PAL_Terminate( void )
{
	
	if( hPalette ){		DeleteObject( hPalette );	hPalette  = NULL;	}
	GFree( lpUseTbl );
	GFree( lpPalData);
}









char PAL_GetPaletteIndex( int r, int g, int b )
{
	int		index=0;
	int		i, rate1, rate2=999999;
	int		rr,gg,bb;

#if	USE_PAL==236
	for( i=10; i<246 ; i++ ) {
		rr = GlobalPal[i].r   - r;
		gg = GlobalPal[i].g - g;
		bb = GlobalPal[i].b  - b;

		rr = rr*rr;
		gg = gg*gg;
		bb = bb*bb;

		rate1 = rr + gg + bb;

		if( rate1 < rate2 ){
			rate2 = rate1;
			index = i;
		}
	}
#else
	for( i=0; i<256 ; i++ ) {
		rr = GlobalPal[i].r   - r;
		gg = GlobalPal[i].g - g;
		bb = GlobalPal[i].b  - b;

		rr = rr*rr;
		gg = gg*gg;
		bb = bb*bb;

		rate1 = rr + gg + bb;

		if( rate1 < rate2 ){
			rate2 = rate1;
			index = i;
		}
	}
#endif
	return index;
}






BOOL PAL_SetPaletteTable( void )
{
	int		i,j,k;
	int		r,g,b;
	RGB32	pal;

	PAL_ResetPaletteTable();

	PaletteIndex = (char *)GAlloc( 0x10000 );
	BlendIndex   = (char (*)[32][256])GAlloc( 32*256*256 );
	AddIndex     = (char (*)[256])GAlloc( 256*256 );
	SubIndex     = (char (*)[256])GAlloc( 256*256 );
	MulIndex     = (char (*)[256])GAlloc( 256*256 );

	for( r=0; r<32 ; r++ ){
		for( g=0; g<32 ; g++ ){
			for( b=0; b<32 ; b++ ){
				PaletteIndex[(r<<10) + (g<<5) + b] = PAL_GetPaletteIndex( r<<3, g<<3, b<<3 );
			}
		}
	}
	 for( k=0; k<32 ; k++ ){
		for( i=0; i<256 ; i++ ){
			for( j=0; j<256 ; j++ ){
				r = (GlobalPal[i].r   * k + GlobalPal[j].r  *(32-k) )>>8;
				g = (GlobalPal[i].g * k + GlobalPal[j].g*(32-k) )>>8;
				b = (GlobalPal[i].b  * k + GlobalPal[j].b *(32-k) )>>8;

				BlendIndex[k][i][j] = PaletteIndex[(r<<10) + (g<<5) + b];
			}
		}
	}

	for( i=0; i<256 ; i++ ){
		for( j=0; j<256 ; j++ ){
			r = min( 0xff, GlobalPal[i].r   + GlobalPal[j].r   ) >> 3;
			g = min( 0xff, GlobalPal[i].g + GlobalPal[j].g ) >> 3;
			b = min( 0xff, GlobalPal[i].b  + GlobalPal[j].b  ) >> 3;

			AddIndex[i][j] = PaletteIndex[(r<<10) + (g<<5) + b];

			r = max(    0, GlobalPal[i].r   - GlobalPal[j].r   ) >> 3;
			g = max(    0, GlobalPal[i].g - GlobalPal[j].g ) >> 3;
			b = max(    0, GlobalPal[i].b  - GlobalPal[j].b  ) >> 3;
			SubIndex[i][j] = PaletteIndex[(r<<10) + (g<<5) + b];

			pal = BMP_ChangeBright_Full( GlobalPal[i], GlobalPal[j].r, GlobalPal[j].g, GlobalPal[j].b );
			MulIndex[i][j] = PaletteIndex[ ((pal.r>>3)<<10) + ((pal.g>>3)<<5) + (pal.b>>3)];
		}
	}
	return 1;
}

void PAL_ResetPaletteTable( void )
{
	GFree(PaletteIndex);
	GFree(BlendIndex);
	GFree(AddIndex);
	GFree(SubIndex);
	GFree(MulIndex);
}








#include "comp_pac.h"
BOOL PAL_SetEntry( RGB32* rgbPal, int top, int num )
{
	int		count, no;
	


	
	if( top < 0 ){	num	+= top;	top = 0;	}
	if( top + num > 256 ){	num -= top + num - 256;	}

	
	if( rgbPal==NULL ){
		char	*buf;
		no = 0;
		PAC_LoadFile( BMP_GetPackDir(), "236.act", &buf );



		for( count=0 ; count<256 ; count++ ){
			lpPalData[count].peRed   = GlobalPal[count].r = buf[no++];
			lpPalData[count].peGreen = GlobalPal[count].g = buf[no++];
			lpPalData[count].peBlue  = GlobalPal[count].b = buf[no++];
			lpPalData[count].peFlags = 0;
			lpUseTbl[count] = 0xff;
		}
		GFree( buf );
	}else{
		for( count=0; count<num; count++ ){
			no = top + count;
			lpPalData[no].peRed   = GlobalPal[count].r   = rgbPal[count].r;
			lpPalData[no].peGreen = GlobalPal[count].g = rgbPal[count].g;
			lpPalData[no].peBlue  = GlobalPal[count].b  = rgbPal[count].b;
			lpPalData[no].peFlags = 0;
			lpUseTbl[no] = 0xff;
		}
	}
	
	if( DirectDrawFlag ){

#if	DIRECT_DRAW
		DD_SetPalette( GlobalPal, 0, 256 );
#endif

	}else{
		if( !SetPaletteEntries( hPalette, top, num, &lpPalData[top] ) ){
			DebugBox(NULL,"ƒpƒŒƒbƒgÝ’è‚ÉŽ¸”s‚µ‚Ü‚µ‚½");
		}
	}

	return TRUE;
}








BOOL PAL_AddEntry( RGB32* rgbPal, int num, LPBYTE lpChgTbl )
{
	int		src, dst;
	char	tbl[256];

	
	if( (hPalette == NULL) || (lpPalData == NULL) )	return FALSE;

	for( src=0; src<256; src++ )	tbl[src] = 0;

	
	for( src=0; src<num; src++ ){
		if( lpChgTbl[src] ){
			for( dst=1; dst<256; dst++ ){
				if( lpUseTbl[dst] ){
					if( (rgbPal[src].r   == lpPalData[dst].peRed)   &&
						(rgbPal[src].g == lpPalData[dst].peGreen) &&
						(rgbPal[src].b  == lpPalData[dst].peBlue) ){
							tbl[src] = dst;
							lpChgTbl[src] = 0;
							break;
					}
				}
			}
		}
	}

	
	src = 0;
	for( dst=1; dst<256; dst++ ){
		if( lpUseTbl[dst] == 0 ){
			while( (lpChgTbl[src] == 0) && (src<num) )	src++;
			if( src >= num )	break;
			lpPalData[dst].peRed   = rgbPal[src].r;
			lpPalData[dst].peGreen = rgbPal[src].g;
			lpPalData[dst].peBlue  = rgbPal[src].b;
			lpPalData[dst].peFlags = 0;
			lpUseTbl[dst] = 1;
			tbl[src] = dst;
			lpChgTbl[src] = 0;
		}
	}

	
	if( (src<num) && (dst>=256) ){		
		int		min, val, no;
		for( ; src<num; src++ ){
			if( lpChgTbl[src] ){
				min = 32768;	no = 0;
				for( dst=1; dst<256; dst++ ){
					val = abs(rgbPal[src].r - lpPalData[dst].peRed) + abs(rgbPal[src].g - lpPalData[dst].peGreen) + abs(rgbPal[src].b - lpPalData[dst].peBlue);
					if( val < min ){	min = val;	no = dst;	}
				}
				tbl[src] = no;
			}
		}
	}

	
	SetPaletteEntries( hPalette, 0, 256, lpPalData );

	
	CopyMemory( lpChgTbl, tbl, 256 );

	return TRUE;
}








BOOL PAL_GetEntry( RGB32* rgbPal, int top, int num )
{
	int		count, no;
	
	if( lpPalData == NULL )	return FALSE;

	
	if( top < 0 ){	num	+= top;	top = 0;	}
	if( top + num > 256 ){	num -= top + num - 256;	}

	
	for( count=0; count<num; count++ ){
		no = top + count;
		rgbPal[count].r    = lpPalData[no].peRed;
		rgbPal[count].g    = lpPalData[no].peGreen;
		rgbPal[count].b    = lpPalData[no].peBlue;
		rgbPal[count].a = 0;
	}

	return TRUE;
}






HPALETTE PAL_GetPalette( void )
{
	return hPalette;
}






BOOL PAL_Select( HDC hDC )
{
	
	if( hPalette == NULL )	return FALSE;

	
	hPrevPal = SelectPalette( hDC, hPalette, FALSE );

	return( hPrevPal ? TRUE : FALSE );
}






BOOL PAL_Unselect( HDC hDC )
{
	HPALETTE	hPal;

	
	if( hPrevPal == NULL )	return FALSE;

	
	hPal = SelectPalette( hDC, hPrevPal, FALSE );
	hPrevPal = NULL;

	return( hPal ? TRUE : FALSE );
}






BOOL PAL_Realize( HDC hDC )
{
	
	if( hPalette == NULL )	return FALSE;

	
	RealizePalette( hDC );

	return TRUE;
}

