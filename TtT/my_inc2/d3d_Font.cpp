
 
  
   
    
     
      


#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "d3d_draw.h"
#include "comp_pac.h"
#include <math.h>
#include "d3d_font.h"


static char	*PackFontDir="fnt";

void D3DFNT_SetPackDir( char *pac_dir )
{
	PackFontDir = pac_dir;
}

typedef struct{
	BYTE	*buf;
	int		ftype;
	int		fsize;

	int		width;
	BYTE	*kage;
}D3DFONT_STRUCT;
D3DFONT_STRUCT		FontStruct[8];


#define ANTI_BIT	4
#define ANTI_CNT	(8/ANTI_BIT)



typedef struct{
	int		pointer;
	WORD	st_word;
	WORD	ed_word;
}FONT_POINT;

#define MAX_FONT_GROUP	53

static FONT_POINT	FontPoint[MAX_FONT_GROUP] = {
	{    0, 'A', '¬' },	{  108, '¸', '¿' },	{  116, 'È', 'Î' },	{  123, 'Ú', 'ü' },	{  158, '‚O', '‚X' },
	{  168, '‚`', '‚y' },	{  194, '‚', '‚š' },	{  220, '‚Ÿ', '‚ñ' },	{  303, 'ƒ@', 'ƒ–' },	{  390, 'ƒŸ', 'ƒ¶' },
	{  414, 'ƒ¿', 'ƒÖ' },	{  438, '„@', '„`' },	{  471, '„p', '„‘' },	{  505, '„Ÿ', '„¾' },	{  537, '‡@', '‡™' },
	{  627, 'ˆŸ', 'ˆü' },	{  721, '‰@', '‰ü' },	{  910, 'Š@', 'Šü' },	{ 1099, '‹@', '‹ü' },	{ 1288, 'Œ@', 'Œü' },
	{ 1477, '@', 'ü' },	{ 1666, 'Ž@', 'Žü' },	{ 1855, '@', 'ü' },	{ 2044, '@', 'ü' },	{ 2233, '‘@', '‘ü' },
	{ 2422, '’@', '’ü' },	{ 2611, '“@', '“ü' },	{ 2800, '”@', '”ü' },	{ 2989, '•@', '•ü' },	{ 3178, '–@', '–ü' },
	{ 3367, '—@', '—ü' },	{ 3556, '˜@', '˜r' },	{ 3607, '˜Ÿ', '˜ü' },	{ 3701, '™@', '™ü' },	{ 3890, 'š@', 'šü' },
	{ 4079, '›@', '›ü' },	{ 4268, 'œ@', 'œü' },	{ 4457, '@', 'ü' },	{ 4646, 'ž@', 'žü' },	{ 4835, 'Ÿ@', 'Ÿü' },
	{ 5024, 'à@', 'àü' },	{ 5213, 'á@', 'áü' },	{ 5402, 'â@', 'âü' },	{ 5591, 'ã@', 'ãü' },	{ 5780, 'ä@', 'äü' },
	{ 5969, 'å@', 'åü' },	{ 6158, 'æ@', 'æü' },	{ 6347, 'ç@', 'çü' },	{ 6536, 'è@', 'èü' },	{ 6725, 'é@', 'éü' },
	{ 6914, 'ê@', 'ê¤' },	{ 7015, 'ð@', 'ðE'+2 },	{ 7023, '@', '@' }
};

typedef struct{
	int		pointer;
	BYTE	st_byte;
	BYTE	ed_byte;
}FONT_POINT2;
static FONT_POINT2	FontPoint2[3] = {
	{    0, '!', '~' },	{  94, '¡', 'ß' },	{ 157, ' ', ' ' }
};










static void UpdateTextChar4444( LPDIRECT3DTEXTURE8 texture, int lx, int ly, int mx, int my, int sx, int sy, BYTE *fbuf, int pitch_x, int r, int g, int b, int kage )
{
	int				x,y;
	D3DLOCKED_RECT	d3dlr;
	BYTE			code;
	D3DD_ARGB_4444	argb[16];
	RECT			srect = { lx, ly, mx, my };
	static int		kr,kg,kb;

	pitch_x=(pitch_x+1)/2*2;

	if(kage>=0){
		for(x=0;x<16;x++){
			argb[x].a = x;
			argb[x].r = r/16;
			argb[x].g = g/16;
			argb[x].b = b/16;
		}
		kr = r;	kg = g;	kb = b;
	}else if(kage<0){
		for(x=0;x<16;x++){
			argb[x].a = 0;
			argb[x].r = ( r*x + kr*(15-x) )/15/16;
			argb[x].g = ( g*x + kg*(15-x) )/15/16;
			argb[x].b = ( b*x + kb*(15-x) )/15/16;
		}
	}

	if( texture->LockRect( 0, &d3dlr, &srect, D3DLOCK_NOSYSLOCK )==D3D_OK ){
		WORD	*pixel = (WORD*)d3dlr.pBits;
		int		pitchx = d3dlr.Pitch/2;

			for(y=0;y<my-ly;y++){
				for(x=0;x<mx-lx;x++){
					if( (sx+x)%ANTI_CNT ){
						code = (fbuf[(sy+y)*(pitch_x/ANTI_CNT) + (sx+x)/ANTI_CNT] & 0xf0)>>4;
					}else{
						code =  fbuf[(sy+y)*(pitch_x/ANTI_CNT) + (sx+x)/ANTI_CNT] & 0x0f;
					}

					if(kage>=0){
						if( pixel[y*pitchx+x] && *(WORD*)&argb[code]){
							((D3DD_ARGB_4444*)&pixel[y*pitchx+x])->a = max( ((D3DD_ARGB_4444*)&pixel[y*pitchx+x])->a, argb[code].a );
						}else{
							pixel[y*pitchx+x] |= *(WORD*)&argb[code];
						}
					}else{
						pixel[y*pitchx+x] = (pixel[y*pitchx+x]&0xf000) | ((*(WORD*)&argb[code])&0x0fff);
					}


				}
			}

		texture->UnlockRect(0);
	}

}
static void UpdateTextChar8888( LPDIRECT3DTEXTURE8 texture, int lx, int ly, int mx, int my, int sx, int sy, BYTE *fbuf, int pitch_x, int r, int g, int b, int kage )
{
	int				x,y;
	D3DLOCKED_RECT	d3dlr;
	BYTE			code;
	D3DD_ARGB		argb[16];
	RECT			srect = { lx, ly, mx, my };
	static int		kr,kg,kb;

	if(kage>=0){
		for(x=0;x<16;x++){
			argb[x].a = 255*x/15;
			argb[x].r = r;
			argb[x].g = g;
			argb[x].b = b;
		}
		kr = r;	kg = g;	kb = b;
	}else{
		for(x=0;x<16;x++){
			argb[x].a = 0;
			argb[x].r = ( r*x + kr*(15-x) )/15;
			argb[x].g = ( g*x + kr*(15-x) )/15;
			argb[x].b = ( b*x + kr*(15-x) )/15;
		}
	}

	if( texture->LockRect( 0, &d3dlr, &srect, D3DLOCK_NOSYSLOCK )==D3D_OK ){
		DWORD	*pixel = (DWORD*)d3dlr.pBits;
		int		pitchx = d3dlr.Pitch/4;

		for(y=0;y<my-ly;y++){
			for(x=0;x<mx-lx;x++){
				if( (sx+x)%ANTI_CNT ){
					code = (fbuf[(sy+y)*pitch_x/ANTI_CNT + (sx+x)/ANTI_CNT] & 0xf0)>>4;
				}else{
					code =  fbuf[(sy+y)*pitch_x/ANTI_CNT + (sx+x)/ANTI_CNT] & 0x0f;
				}
				if(kage>=0){
					pixel[y*pitchx+x] |= *(DWORD*)&argb[code];
				}else{

					pixel[y*pitchx+x] = (pixel[y*pitchx+x]&0xff000000) | ((*(DWORD*)&argb[code])&0x00ffffff);
				}
			}
		}
		texture->UnlockRect(0);
	}
}


BOOL D3DFNT_UpdateTextChar( D3DD_TEXTURE *d3dTex, int dest_x, int dest_y, WORD moji, int fsize, int r, int g, int b, int kage )
{
	BYTE	*buf, *kbuf;
	int		size1,ksize1;
	int		size2,ksize2;
	int		no=-1,i, fno, bai=0;
	int		width;
	int		han;

	if(moji>0xff){
		han = 0;
	}else{
		han = 1;
	}

	for( i=0 ; i<8 ; i++ ){
		if( FontStruct[i].fsize==fsize ){
			width = FontStruct[i].width;
			size1 = fsize*fsize/ANTI_CNT;
			ksize1 = (fsize+width*2+1)/ANTI_CNT*(fsize+width*2);
			fno=i;
			break;
		}
	}
	if(i==8){
		DebugBox( NULL, "‚»‚ÌƒTƒCƒY‚ÌƒtƒHƒ“ƒg‚Í“Ç‚Ý‚±‚Ü‚ê‚Ä‚¢‚Ü‚¹‚ñ[%d]\n", fsize );
		return FALSE;
	}

	if(han){
		for( i=0 ; i<3 ; i++ ){
			if( FontPoint2[i].st_byte <= moji && moji <= FontPoint2[i].ed_byte ){
				no = FontPoint2[i].pointer + moji-FontPoint2[i].st_byte;
				break;
			}
		}
		if( i==2 )	return TRUE;

		size2 = FontPoint[MAX_FONT_GROUP-1].pointer * ((fsize+1)/ANTI_CNT*fsize);
		size1 = (fsize/2+1)/ANTI_CNT*fsize;

		ksize2 = FontPoint[MAX_FONT_GROUP-1].pointer * ((fsize+width*2+1)/ANTI_CNT*(fsize+width*2));
		ksize1 = (fsize/2+width*2+1)/ANTI_CNT*(fsize+width*2);
	}else{
		moji = (moji>>8)|((moji<<8)&0xff00);
		for( i=0 ; i<MAX_FONT_GROUP ; i++ ){
			if( FontPoint[i].st_word <= moji && moji <= FontPoint[i].ed_word ){
				no = FontPoint[i].pointer + moji-FontPoint[i].st_word;
				break;
			}
		}
		if( i==MAX_FONT_GROUP-1 )	return TRUE;
		size2 = 0;
		ksize2 = 0;
	}
	if( no==-1 ){
		DebugPrintf( "•`‰æ‚Å‚«‚È‚¢•¶Žš‚ªŠÜ‚Ü‚ê‚Ä‚¢‚Ü‚·B\n" );
		return FALSE;
	}
	buf = FontStruct[fno].buf + no * size1 + size2;
	kbuf = FontStruct[fno].kage+4 + no * ksize1 + ksize2;
	if(!FontStruct[fno].kage) kage=OFF;

	if(kage>0){
		dest_x-=1;
		dest_y-=1;
		int	ppx = dest_x/d3dTex->grid_w;
		int	ppy = dest_y/d3dTex->grid_h;
		
		for(int py=ppy;py<ppy+2;py++){
			for(int px=ppx;px<ppx+2;px++){
				int	lx = max( dest_x-px*d3dTex->grid_w, 0 );
				int	ly = max( dest_y-py*d3dTex->grid_h, 0 );
				int mx = min( dest_x+fsize/(han+1)+width*2-px*d3dTex->grid_w, d3dTex->grid_w );
				int my = min( dest_y+fsize        +width*2-py*d3dTex->grid_h, d3dTex->grid_h );
				int sx = (px*d3dTex->grid_w+lx)-dest_x;
				int sy = (py*d3dTex->grid_h+ly)-dest_y;

				if(lx<mx && ly<my && px<d3dTex->pitch_w && py<d3dTex->pitch_h){
					if( d3dTex->format==D3DFMT_A4R4G4B4 ){
						UpdateTextChar4444( d3dTex->texture[py*d3dTex->pitch_w+px], lx, ly, mx, my, sx, sy, kbuf, fsize/(han+1)+width*2,r,g,b,kage );
					}else{
						UpdateTextChar8888( d3dTex->texture[py*d3dTex->pitch_w+px], lx, ly, mx, my, sx, sy, kbuf, fsize/(han+1)+width*2,r,g,b,kage );
					}
				}
			}
		}
	}else{
		int	ppx = dest_x/d3dTex->grid_w;
		int	ppy = dest_y/d3dTex->grid_h;
		
		for(int py=ppy;py<ppy+2;py++){
			for(int px=ppx;px<ppx+2;px++){
				int	lx = max( dest_x-px*d3dTex->grid_w, 0 );
				int	ly = max( dest_y-py*d3dTex->grid_h, 0 );
				int mx = min( dest_x+fsize/(han+1)-px*d3dTex->grid_w, d3dTex->grid_w );
				int my = min( dest_y+fsize        -py*d3dTex->grid_h, d3dTex->grid_h );
				int sx = (px*d3dTex->grid_w+lx)-dest_x;
				int sy = (py*d3dTex->grid_h+ly)-dest_y;

				if(lx<mx && ly<my && px<d3dTex->pitch_w && py<d3dTex->pitch_h){
					if( d3dTex->format==D3DFMT_A4R4G4B4 ){
						UpdateTextChar4444( d3dTex->texture[py*d3dTex->pitch_w+px], lx, ly, mx, my, sx, sy, buf, fsize/(han+1),r,g,b,kage );
					}else{
						UpdateTextChar8888( d3dTex->texture[py*d3dTex->pitch_w+px], lx, ly, mx, my, sx, sy, buf, fsize/(han+1),r,g,b,kage );
					}
				}
			}
		}
	}
	return TRUE;
}
BOOL D3DFNT_UpdateText( D3DD_TEXTURE *d3dTex, int dest_x, int dest_y, int pitch_w, int pitch_h, BYTE *str, int fsize, int r, int g, int b, int kage )
{
	int		px=dest_x, py=dest_y;
	int		code;

	if( d3dTex==NULL ) { return FALSE; }

	while( *str ){
		if( (0x81 <= *str && *str <= 0x9f) || (0xe0 <= *str && *str <= 0xea) || (0xf0==*str) ){
			if( D3DFNT_UpdateTextChar( d3dTex, px, py, *(WORD*)str, fsize, r, g, b, kage ) ){
				str+=2;
				px += fsize+pitch_w;
			}else{
				str++;
			}
			continue;
		}else if( (0x21 <= *str && *str <= 0x7e) || (0xa1 <= *str && *str <= 0xdf) ){
			code = *str;
			if( D3DFNT_UpdateTextChar( d3dTex, px, py, code, fsize, r, g, b, kage ) ){
				str++;
				px += fsize/2+pitch_w;
			}else{
				str++;
			}
			continue;
		}
		if( *str=='\n' ){	px = dest_x;	py += fsize+pitch_h;	}	
		str++;
	}
	return TRUE;
}


BOOL D3DFNT_DrawChar( void *dest, int draw_mode, int dest_x, int dest_y, int moji, int fsize, RECT *clip, int r, int g, int b, int alph, int kage, int han )
{

	return TRUE;
}




void D3DFNT_DrawText( void *dest, int draw_mode, int dest_x, int dest_y, int pich_w, int pich_h, BYTE *str, int fsize, RECT *clip, int r, int g, int b, int alph, int kage )
{

}



void D3DFNT_ChangeFont( int ftype )
{
	int	i, fsize;

	for(i=0;i<8;i++){
		if(FontStruct[i].fsize && FontStruct[i].ftype!=ftype){
			fsize = FontStruct[i].fsize;

			D3DFNT_ReleaseFont( FontStruct[i].fsize );
			D3DFNT_LoadFont( fsize, ftype );
		}
	}
}


void D3DFNT_LoadFont( int fsize, int ftype )
{
	char	buf[256];
	int	i;

	for(i=0;i<8;i++){
		if(FontStruct[i].fsize==fsize || FontStruct[i].fsize==0){
			wsprintf( buf, "font%02d.fd%d", fsize, ftype );
			if( PAC_LoadFile( PackFontDir, buf, &FontStruct[i].buf ) ){
				wsprintf( buf, "font%02d.fk%d", fsize, ftype );
				if( PAC_CheckFile( PackFontDir, buf ) ){
					if( PAC_LoadFile( PackFontDir, buf, &FontStruct[i].kage ) ){
						FontStruct[i].width = *(int*)FontStruct[i].kage;
					}else{
						DebugPrintf("‘¾ŽšƒtƒHƒ“ƒgƒtƒ@ƒCƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s‚µ‚Ü‚µ‚½\n");
					}
				}
				FontStruct[i].fsize = fsize;
				FontStruct[i].ftype = ftype;
			}else{
				DebugPrintf("ƒtƒHƒ“ƒgƒtƒ@ƒCƒ‹‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s‚µ‚Ü‚µ‚½\n");
			}
			break;
		}
	}
}



void D3DFNT_ReleaseFont( int fsize )
{
	int	i;

	for(i=0;i<8;i++){
		if(FontStruct[i].fsize==fsize){
			if(FontStruct[i].buf) GlobalFree(FontStruct[i].buf);
			ZeroMemory( &FontStruct[i], sizeof(D3DFONT_STRUCT) );
		}
	}
}



void D3DFNT_Init( void )
{
	D3DFNT_Term();
}


void D3DFNT_Term( void )
{
	int		i;
	for(i=0;i<8;i++){
		if(FontStruct[i].buf) GlobalFree(FontStruct[i].buf);
	}
	ZeroMemory( &FontStruct, sizeof(D3DFONT_STRUCT)*8 );
}
