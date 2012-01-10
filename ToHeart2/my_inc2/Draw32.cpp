
 
  
   
    
     
      
#include "mm_std.h"

#include "bmp.h"
#include "draw.h"
#include "drawprim.h"


#include <math.h>


extern int	DispWidth;
extern int	DispHeight;















BOOL DRW_DrawBMP_TTT_Bld( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src1 = (BMP_T*)dobj->src1;
	BMP_T	*src2 = (BMP_T*)dobj->src2;
	int		drev  = dest->sx - w;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src1_p = src1->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*src2_p = src2->buf + DRW_GetStartPointerSrc2( dobj );
	int		xinc = XINC(dobj);
	int		blnd = dobj->dnum2;
	int		brev = 256 - blnd;

	if(dobj->dparam!=DRW_BLD2){
		char	*blnd_tbl=BlendTable[blnd], *blnd_tbl2;
		char	*brev_tbl=BlendTable[brev], *brev_tbl2, *dest_tbl;

		if( dobj->nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					dest_p->b = blnd_tbl[ src1_p->b ] + brev_tbl[ src2_p->b ];
					dest_p->g = blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
					dest_p->r = blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
				}
			}
		}else if( dobj->nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( src1_p->a!=0 ){
						if( src2_p->a!=0 ){	
							if( src1_p->a==255 ){
								if( src2_p->a==255 ){
									dest_p->b = brev_tbl[ src2_p->b ] + blnd_tbl[ src1_p->b ];
									dest_p->g = brev_tbl[ src2_p->g ] + blnd_tbl[ src1_p->g ];
									dest_p->r = brev_tbl[ src2_p->r ] + blnd_tbl[ src1_p->r ];
								}else{
									blnd_tbl2 = BlendTable[ brev - brev_tbl[ src2_p->a ] ];
									dest_p->b = blnd_tbl2[ dest_p->b ] + brev_tbl[ src2_p->b ] + blnd_tbl[ src1_p->b ];
									dest_p->g = blnd_tbl2[ dest_p->g ] + brev_tbl[ src2_p->g ] + blnd_tbl[ src1_p->g ];
									dest_p->r = blnd_tbl2[ dest_p->r ] + brev_tbl[ src2_p->r ] + blnd_tbl[ src1_p->r ];
								}
							}else{
								if( src2_p->a==255 ){
									brev_tbl2 = BlendTable[ blnd - blnd_tbl[ src1_p->a ] ];
									dest_p->b = brev_tbl2[ dest_p->b ] + blnd_tbl[ src1_p->b ] + brev_tbl[ src2_p->b ];
									dest_p->g = brev_tbl2[ dest_p->g ] + blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
									dest_p->r = brev_tbl2[ dest_p->r ] + blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
								}else{
									dest_tbl  = BlendTable[ 256 - blnd_tbl[ src1_p->a ] - brev_tbl[ src2_p->a ] ];
									dest_p->b = dest_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ] + brev_tbl[ src2_p->b ];
									dest_p->g = dest_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
									dest_p->r = dest_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
								}
							}
						}else{							
							if( src1_p->a==255 ){	
								dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ];
								dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ];
								dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ];
							}else{						
								brev_tbl2 = BlendTable[ 255 - blnd_tbl[ src1_p->a ] ];
								dest_p->b = brev_tbl2[ dest_p->b ] + blnd_tbl[ src1_p->b ];
								dest_p->g = brev_tbl2[ dest_p->g ] + blnd_tbl[ src1_p->g ];
								dest_p->r = brev_tbl2[ dest_p->r ] + blnd_tbl[ src1_p->r ];
							}
						}
					}else{
						if( src2_p->a!=0 ){	
							if( src2_p->a==255 ){	
								dest_p->b = blnd_tbl[ dest_p->b ] + brev_tbl[ src2_p->b ];
								dest_p->g = blnd_tbl[ dest_p->g ] + brev_tbl[ src2_p->g ];
								dest_p->r = blnd_tbl[ dest_p->r ] + brev_tbl[ src2_p->r ];
							}else{						
								brev_tbl2 = BlendTable[ 255 - brev_tbl[ src2_p->a ] ];
								dest_p->b = brev_tbl2[ dest_p->b ] + brev_tbl[ src2_p->b ];
								dest_p->g = brev_tbl2[ dest_p->g ] + brev_tbl[ src2_p->g ];
								dest_p->r = brev_tbl2[ dest_p->r ] + brev_tbl[ src2_p->r ];
							}
						}
						
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){

					if( (*(long *)src1_p&0x00ffffff) != dobj->nuki ){
						if( (*(long *)src2_p&0x00ffffff) != dobj->nuki ){
							dest_p->b = brev_tbl[ src2_p->b ] + blnd_tbl[ src1_p->b ];
							dest_p->g = brev_tbl[ src2_p->g ] + blnd_tbl[ src1_p->g ];
							dest_p->r = brev_tbl[ src2_p->r ] + blnd_tbl[ src1_p->r ];
						}else{
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ];
						}
					}else{
						if( (*(long *)src2_p&0x00ffffff) != dobj->nuki ){
							dest_p->b = brev_tbl[ src2_p->b ] + blnd_tbl[ dest_p->b ];
							dest_p->g = brev_tbl[ src2_p->g ] + blnd_tbl[ dest_p->g ];
							dest_p->r = brev_tbl[ src2_p->r ] + blnd_tbl[ dest_p->r ];
						}
					}
				}
			}
		}
	}else{
		int		blnd3 = (blnd*dobj->dnum)>>8;
		int		brev3 = (brev*dobj->dnum)>>8;
		char	*blnd_tbl=BlendTable[blnd3], *blnd_tbl2;
		char	*brev_tbl=BlendTable[brev3], *brev_tbl2, *dest_tbl;
		char	*dbb1_tbl=BlendTable[256-blnd3-brev3];
		char	*dbb2_tbl=BlendTable[256-blnd3];
		char	*dbb3_tbl=BlendTable[256-brev3];

		if( dobj->nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++ ){
					dest_p->b = dbb1_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ] + brev_tbl[ src2_p->b ];
					dest_p->g = dbb1_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
					dest_p->r = dbb1_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
				}
			}
		}else if( dobj->nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++ ){
					if( src1_p->a!=0 ){
						if( src2_p->a!=0 ){	
							if( src1_p->a==255 ){
								if( src2_p->a==255 ){
									dest_p->b= dbb1_tbl[ dest_p->b] + blnd_tbl[ src1_p->b] + brev_tbl[ src2_p->b ];
									dest_p->g = dbb1_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
									dest_p->r = dbb1_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
								}else{
									blnd_tbl2 = BlendTable[ 256 - blnd3 - brev_tbl[ src2_p->a ] ];
									dest_p->b = blnd_tbl2[ dest_p->b ] + brev_tbl[ src2_p->b ] + blnd_tbl[ src1_p->b ];
									dest_p->g = blnd_tbl2[ dest_p->g ] + brev_tbl[ src2_p->g ] + blnd_tbl[ src1_p->g ];
									dest_p->r = blnd_tbl2[ dest_p->r ] + brev_tbl[ src2_p->r ] + blnd_tbl[ src1_p->r ];
								}
							}else{
								if( src2_p->a==255 ){
									brev_tbl2 = BlendTable[ 256 - brev3 - blnd_tbl[ src1_p->a ] ];
									dest_p->b = brev_tbl2[ dest_p->b ] + blnd_tbl[ src1_p->b ] + brev_tbl[ src2_p->b ];
									dest_p->g = brev_tbl2[ dest_p->g ] + blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
									dest_p->r = brev_tbl2[ dest_p->r ] + blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
								}else{
									dest_tbl  = BlendTable[ 256 - blnd_tbl[ src1_p->a ] - brev_tbl[ src2_p->a ] ];
									dest_p->b = dest_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ] + brev_tbl[ src2_p->b ];
									dest_p->g = dest_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
									dest_p->r = dest_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
								}
							}
						}else{							
							if( src1_p->a==255 ){	
								dest_p->b = dbb2_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ];
								dest_p->g = dbb2_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ];
								dest_p->r = dbb2_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ];
							}else{							
								brev_tbl2 = BlendTable[ 255-blnd_tbl[src1_p->a] ];
								dest_p->b = brev_tbl2[ dest_p->b ] + blnd_tbl[ src1_p->b ];
								dest_p->g = brev_tbl2[ dest_p->g ] + blnd_tbl[ src1_p->g ];
								dest_p->r = brev_tbl2[ dest_p->r ] + blnd_tbl[ src1_p->r ];
							}
						}
					}else{
						if( src2_p->a!=0 ){	
							if( src2_p->a==255 ){	
								dest_p->b = dbb3_tbl[ dest_p->b ] + brev_tbl[ src2_p->b ];
								dest_p->g = dbb3_tbl[ dest_p->g ] + brev_tbl[ src2_p->g ];
								dest_p->r = dbb3_tbl[ dest_p->r ] + brev_tbl[ src2_p->r ];
							}else{						
								brev_tbl2 = BlendTable[ 255 - brev_tbl[ src2_p->a ] ];
								dest_p->b = brev_tbl2[ dest_p->b ] + brev_tbl[ src2_p->b ];
								dest_p->g = brev_tbl2[ dest_p->g ] + brev_tbl[ src2_p->g ];
								dest_p->r = brev_tbl2[ dest_p->r ] + brev_tbl[ src2_p->r ];
							}
						}
						
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++ ){

					if( (*(long *)src1_p&0x00ffffff) != dobj->nuki ){
						
						if( (*(long *)src2_p&0x00ffffff) != dobj->nuki ){
							dest_p->b = dbb1_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ] + brev_tbl[ src2_p->b ];
							dest_p->g = dbb1_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
							dest_p->r = dbb1_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
						}else{
							dest_p->b = dbb2_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ];
							dest_p->g = dbb2_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ];
							dest_p->r = dbb2_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ];
						}
					}else{

						if( (*(long *)src2_p&0x00ffffff) != dobj->nuki ){
							dest_p->b = dbb3_tbl[ dest_p->b ] + brev_tbl[ src2_p->b ];
							dest_p->g = dbb3_tbl[ dest_p->g ] + brev_tbl[ src2_p->g ];
							dest_p->r = dbb3_tbl[ dest_p->r ] + brev_tbl[ src2_p->r ];
						}
					}
				}
			}
		}
	}
	return TRUE;
}







BOOL DRW_DrawBMP_TTT_Ami( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src1 = (BMP_T*)dobj->src1;
	BMP_T	*src2 = (BMP_T*)dobj->src2;
	int		drev  = dest->sx - dobj->dw;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src1_p = src1->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*src2_p = src2->buf + DRW_GetStartPointerSrc2( dobj );
	int		xinc = XINC(dobj);
	int		mesh = dobj->dnum2;
	char	*brev_tbl;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	mesh_pt = MeshFadePatern[mesh/16];

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	if( dobj->nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			linemask2 = linemask[y & 3];
			for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
				if( linemask2[x & 3] )	{ *dest_p = *src1_p; }
				else					{ *dest_p = *src2_p; }
			}
		}
	}else if( dobj->nuki==-2 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			linemask2 = linemask[y & 3];
			for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
				if( linemask2[x & 3] ){
					if( src1_p->a!=0 ){
						if( src1_p->a==255 ){
							*dest_p = *src1_p;
						}else{
							brev_tbl = BlendTable[ 256 - src1_p->a ];
							dest_p->b = brev_tbl[ dest_p->b ] + src1_p->b;
							dest_p->g = brev_tbl[ dest_p->g ] + src1_p->g;
							dest_p->r = brev_tbl[ dest_p->r ] + src1_p->r;
						}
					}
				}else{
					if( src2_p->a!=0 ){	
						if( src2_p->a==255 ){
							*dest_p = *src2_p;
						}else{
							brev_tbl = BlendTable[ 256 - src2_p->a ];
							dest_p->b = brev_tbl[ dest_p->b ] + src2_p->b;
							dest_p->g = brev_tbl[ dest_p->g ] + src2_p->g;
							dest_p->r = brev_tbl[ dest_p->r ] + src2_p->r;
						}
					}
				}
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			linemask2 = linemask[y & 3];
			for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
				if( linemask2[x & 3] ){
					if( (*(long *)src1_p&0x00ffffff) != dobj->nuki ) { *dest_p = *src1_p; }
				}else{
					if( (*(long *)src2_p&0x00ffffff) != dobj->nuki ) { *dest_p = *src2_p; }
				}
			}
		}
	}
	return TRUE;
}
BOOL DRW_DrawBMP_TTT_Lpp( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src1 = (BMP_T*)dobj->src1;
	BMP_T	*src2 = (BMP_T*)dobj->src2;
	int		drev  = dest->sx - dobj->dw;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src1_p = src1->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*src2_p = src2->buf + DRW_GetStartPointerSrc2( dobj );
	int		xinc = XINC(dobj);
	int		rate = dobj->dnum2;
	char	*brev_tbl;
	int		rate_max[2];

	rate = (h/16+16)*(256-rate)/256;
	rate_max[0] = STD_Limit( rate*16-240, 0, h );
	rate_max[1] = STD_Limit( rate*16,     0, h );

	if( dobj->nuki==-1 ){	
		for( y=0; y < rate_max[0] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
				*dest_p = *src2_p;
			}
		}
		for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if( (rate-(y>>4)) <= (y&0xf) ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					*dest_p = *src1_p;
				}
			}else{
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					*dest_p = *src2_p;
				}
			}
		}
		for( ; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
				*dest_p = *src1_p;
			}
		}
	}else if( dobj->nuki==-2 ){	
		for( y=0; y < rate_max[0] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
				if( src2_p->a!=0 ){	
					if( src2_p->a==255 ){
						*dest_p = *src2_p;
					}else{
						brev_tbl = BlendTable[ 256 - src2_p->a ];
						dest_p->b = brev_tbl[ dest_p->b ] + src2_p->b;
						dest_p->g = brev_tbl[ dest_p->g ] + src2_p->g;
						dest_p->r = brev_tbl[ dest_p->r ] + src2_p->r;
					}
				}
			}
		}
		for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if( (rate-(y>>4)) <= (y&0xf) ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( src1_p->a!=0 ){	
						if( src1_p->a==255 ){
							*dest_p = *src1_p;
						}else{
							brev_tbl = BlendTable[ 256 - src1_p->a ];
							dest_p->b = brev_tbl[ dest_p->b ] + src1_p->b;
							dest_p->g = brev_tbl[ dest_p->g ] + src1_p->g;
							dest_p->r = brev_tbl[ dest_p->r ] + src1_p->r;
						}
					}
				}
			}else{
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( src2_p->a!=0 ){	
						if( src2_p->a==255 ){
							*dest_p = *src2_p;
						}else{
							brev_tbl = BlendTable[ 256 - src2_p->a ];
							dest_p->b = brev_tbl[ dest_p->b ] + src2_p->b;
							dest_p->g = brev_tbl[ dest_p->g ] + src2_p->g;
							dest_p->r = brev_tbl[ dest_p->r ] + src2_p->r;
						}
					}
				}
			}
		}
		for( ; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
				if( src1_p->a!=0 ){	
					if( src1_p->a==255 ){
						*dest_p = *src1_p;
					}else{
						brev_tbl = BlendTable[ 256 - src1_p->a ];
						dest_p->b = brev_tbl[ dest_p->b ] + src1_p->b;
						dest_p->g = brev_tbl[ dest_p->g ] + src1_p->g;
						dest_p->r = brev_tbl[ dest_p->r ] + src1_p->r;
					}
				}
			}
		}
	}else{			
		for( y=0; y < rate_max[0] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
				if( (*(long*)src2_p&0x00ffffff) != dobj->nuki ){	
					*dest_p = *src2_p;
				}
			}
		}
		for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if( (rate-(y>>4)) <= (y&0xf) ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( (*(long*)src1_p&0x00ffffff) != dobj->nuki ){	
						*dest_p = *src1_p;
					}
				}
			}else{
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( (*(long*)src2_p&0x00ffffff) != dobj->nuki ){	
						*dest_p = *src2_p;
					}
				}
			}
		}
		for( ; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
				if( (*(long*)src1_p&0x00ffffff) != dobj->nuki ){	
					*dest_p = *src1_p;
				}
			}
		}
	}
	return TRUE;
}






static BOOL DRW_DrawBMP_TT_Bld( DRAW_OBJECT *dobj );
static BOOL DRW_DrawBMP_TT_Ami( DRAW_OBJECT *dobj );
static BOOL DRW_DrawBMP_TT_Lpp( DRAW_OBJECT *dobj );

BOOL DRW_DrawBMP_TTT( DRAW_OBJECT dobj )
{
	BOOL	ret=TRUE;
	BMP_T	*dest = (BMP_T*)dobj.dest;
	BMP_T	*src1 = (BMP_T*)dobj.src1;
	BMP_T	*src2 = (BMP_T*)dobj.src2;
	DRAW_OBJECT		dobj2, dobj3, dobj4, dobj5;
	
	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_TTT]\n"); return FALSE; }
	if( src1->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ1‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_TTT]\n"); return FALSE; }
	if( src2->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ2‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_TTT]\n"); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRectS2(&dobj.dx,  &dobj.dy,  dest->sx, dest->sy,
						&dobj.s1x, &dobj.s1y, src1->sx, src1->sy,  &dobj.s1w, &dobj.s1h,
						&dobj.s2x, &dobj.s2y, src2->sx, src2->sy,  &dobj.s2w, &dobj.s2h, dobj.clip ) ) return FALSE;
	}
	DRW_SetDrawObject2XXX( &dobj, &dobj2, &dobj3, &dobj4, &dobj5 );

	switch(dobj.dparam2){
		default:
		case DRW_BLD2:
			if(dobj2.dest) DRW_DrawBMP_TT_Bld(&dobj2);
			if(dobj3.dest) DRW_DrawBMP_TT_Bld(&dobj3);
			DRW_DrawBMP_TTT_Bld(&dobj);
			break;	
		case DRW_AMI2:
			if(dobj2.dest) DRW_DrawBMP_TT_Ami(&dobj2);
			if(dobj3.dest) DRW_DrawBMP_TT_Ami(&dobj3);
			DRW_DrawBMP_TTT_Ami(&dobj);
			break;	
		case DRW_LPP2:		case DRW_LPP2+1:	case DRW_LPP2+2:
		case DRW_LPP2+3:	case DRW_LPP2+4:	case DRW_LPP2+5:	
			if(dobj2.dest) DRW_DrawBMP_TT_Lpp(&dobj2);
			if(dobj3.dest) DRW_DrawBMP_TT_Lpp(&dobj3);
			DRW_DrawBMP_TTT_Lpp(&dobj);
			break;
	}

	return ret;
}









BOOL DRW_DrawBMP_TT_Std( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( dobj->nuki==-1 ){	

			if(dobj->rparam&REV_W){
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						*dest_p = *src_p;
					}
				}
			}else{
				if(dobj->rparam&REV_H){
					for( y=0; y < h ; y++, dest_p+=dest->sx, src_p-=src->sx )
						CopyMemory( dest_p, src_p, w * sizeof(RGB32) );	
				}else{
					if( w==dest->sx && w==src->sx ){
						CopyMemory( dest_p, src_p, w*h * sizeof(RGB32) );	
					}else{
						for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx )
							CopyMemory( dest_p, src_p, w * sizeof(RGB32) );	
					}
				}
			}
		}else if( dobj->nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( src_p->a != 0 ){	
						if( src_p->a == 255 ){	
							*dest_p = *src_p;
						}else{					
							alph_tbl = BlendTable[ 255 - src_p->a ];
							dest_p->b = alph_tbl[dest_p->b] + src_p->b;
							dest_p->g = alph_tbl[dest_p->g] + src_p->g;
							dest_p->r = alph_tbl[dest_p->r] + src_p->r;
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						*dest_p = *src_p;
					}
				}
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];
		char	*rtbl2 = BrightTable_TB[r];
		char	*gtbl2 = BrightTable_TB[g];

		if( dobj->nuki == -1 ){



			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b] | (src_p->a<<24);
				}
			}
		}else if( dobj->nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( src_p->a != 0 ){	
						if( src_p->a == 255 ){	
							*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
						}else{					
							alph_tbl = BlendTable[ 255 - src_p->a ];
							alp2_tbl = BlendTable[  src_p->a ];
							alp3_tbl = BlendTable2[ src_p->a ];
							dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[ btbl[alp3_tbl[src_p->b]]];
							dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl2[alp3_tbl[src_p->g]]];
							dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl2[alp3_tbl[src_p->r]]];
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						*(DWORD*)dest_p = rtbl[src_p->r] | gtbl[src_p->g] | btbl[src_p->b];
					}
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_TT_Bld( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		w2=w/2;
	int		w3=w%2;
	int		w4=w2*2;
	int		blnd = dobj->dnum;
	int		brev = 256-blnd;
	DWORD	alpha2 = brev * 0x01010101;
	DWORD	alpha1 = blnd * 0x01010101;
	char	*blnd_tbl = BlendTable[blnd];
	char	*brev_tbl = BlendTable[brev];

	if( blnd==256 )	return DRW_DrawBMP_TT_Std( dobj );	
	if( blnd==0   )	return 1;																			

	
	if( MMX_Flag && nuki!=-2 && (r==128 && g==128 && b==128) && !dobj->rparam ){
		if( nuki == -1 ){
			for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
				if( w2 ){
					_asm {
						mov			ecx, w2
						mov			eax, src_p	
						mov			ebx, dest_p	
						pxor		mm3, mm3	
						movd		mm4, alpha1	
						movd		mm5, alpha2	
						punpcklbw	mm4, mm3	
						punpcklbw	mm5, mm3	
				loopx01:
						
						movq		mm0, [eax]	
						movq		mm1, [ebx]	
						
						movq		mm6, mm0	
						movq		mm7, mm1	
						
						punpcklbw	mm0, mm3	
						punpcklbw	mm1, mm3	
						
						punpckhbw	mm6, mm3	
						punpckhbw	mm7, mm3	
						
						pmullw		mm0, mm4	
						pmullw		mm1, mm5	
						paddusw		mm0, mm1	
						pmullw		mm6, mm4	
						pmullw		mm7, mm5	
						psrlw		mm0, 8		
						paddusw		mm6, mm7	
						psrlw		mm6, 8		
						
						packuswb	mm0, mm6	
						movq		[ebx], mm0

						add			eax, 8		
						add			ebx, 8		
						loop		loopx01
					}
					_asm emms
				}
				if( w3 ){	
					(dest_p+w4)->b = brev_tbl[ (dest_p+w4)->b ] + blnd_tbl[ (src_p+w4)->b ];
					(dest_p+w4)->g = brev_tbl[ (dest_p+w4)->g ] + blnd_tbl[ (src_p+w4)->g ];
					(dest_p+w4)->r = brev_tbl[ (dest_p+w4)->r ] + blnd_tbl[ (src_p+w4)->r ];
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
				if( w2 ){
					_asm {
						mov			ecx, w2
						mov			eax, src_p	
						mov			ebx, dest_p	
						pxor		mm3, mm3	
						movd		mm4, alpha1	
						movd		mm5, alpha2	
						punpcklbw	mm4, mm3	
						punpcklbw	mm5, mm3	

						mov			edx, nuki
					loopx02:
							
							movq		mm0, [eax]	
							movq		mm1, [ebx]	
							
							movq		mm6, mm0	
							movq		mm7, mm1	
							
							punpcklbw	mm0, mm3	
							punpcklbw	mm1, mm3	
							
							punpckhbw	mm6, mm3	
							punpckhbw	mm7, mm3	
							
							pmullw		mm0, mm4	
							pmullw		mm1, mm5	
							paddusw		mm0, mm1	
							pmullw		mm6, mm4	
							pmullw		mm7, mm5	
							psrlw		mm0, 8		
							paddusw		mm6, mm7	
							psrlw		mm6, 8		
							
							packuswb	mm0, mm6	
	
							cmp		edx, [eax]	
							je	LOW01
							jmp	LOW02
							
							LOW01:
								cmp		edx, [eax+4]	
								
								je	NEXT01
								
									movd	[ebx], mm0
								jmp	NEXT01
							
							LOW02:
								cmp		edx, [eax+4]	
								jne	HIGH01
								
									psrlq	mm0, 32
									movd	[ebx+4], mm0
								jmp	NEXT01
								
								HIGH01:
									movq	[ebx], mm0
								jmp	NEXT01
							NEXT01:

							add			eax, 8		
							add			ebx, 8		
						loop		loopx02

						mov			src_p,  eax	
						mov			dest_p, ebx	
					}
					_asm emms
				}
				if( w3 ){	
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
					}
				}
			}
		}
	}else{
		char	*alph_tbl, *alp2_tbl, *alp3_tbl;

		if(r==128 && g==128 && b==128){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
					}
				}
			}else if( nuki==-2 ){
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){	
							if( src_p->a==255 ){	
								dest_p->b = brev_tbl[dest_p->b] + blnd_tbl[ src_p->b ];
								dest_p->g = brev_tbl[dest_p->g] + blnd_tbl[ src_p->g ];
								dest_p->r = brev_tbl[dest_p->r] + blnd_tbl[ src_p->r ];
							}else{						
								alph_tbl = BlendTable[ 255-blnd_tbl[src_p->a] ];

								dest_p->b = alph_tbl[dest_p->b] + blnd_tbl[ src_p->b ];
								dest_p->g = alph_tbl[dest_p->g] + blnd_tbl[ src_p->g ];
								dest_p->r = alph_tbl[dest_p->r] + blnd_tbl[ src_p->r ];
							}
						}
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
						}
					}
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
					}
				}
			}else if( nuki==-2 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){	
							if( src_p->a==255 ){	
								dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
								dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
								dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
							}else{							
								alph_tbl = BlendTable[ 255-blnd_tbl[src_p->a] ];
								alp2_tbl = BlendTable[ blnd_tbl[src_p->a] ];
								alp3_tbl = BlendTable2[ src_p->a ];

								dest_p->b = alph_tbl[ dest_p->b ] + alp2_tbl[ btbl[alp3_tbl[src_p->b]] ];
								dest_p->g = alph_tbl[ dest_p->g ] + alp2_tbl[ gtbl[alp3_tbl[src_p->g]] ];
								dest_p->r = alph_tbl[ dest_p->r ] + alp2_tbl[ rtbl[alp3_tbl[src_p->r]] ];
							}
						}
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
						}
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TT_Add( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		w2=w/2;
	int		w3=w%2;
	int		w4=w2*2;
	char	*add_tbl = AddTable;

	
	if( r==0 && g==0 && b==0 ){ return 1; }

	
	if( MMX_Flag && nuki==-1 && (r==128 && g==128 && b==128) && !dobj->rparam ){
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if(w2){
				_asm {
					mov		ecx, w2
					mov		esi, src_p
					mov		edi, dest_p
					loopx01:
						movq	mm0, [esi]
						movq	mm1, [edi]
						paddusb	mm0, mm1
						movq	[edi], mm0
						add		esi, 8
						add		edi, 8
					loop	loopx01

					mov		src_p,  esi
					mov		dest_p, edi
				}
				_asm	emms
			}
			if( w3 ){	
				dest_p->b = add_tbl[ dest_p->b + src_p->b ];
				dest_p->g = add_tbl[ dest_p->g + src_p->g ];
				dest_p->r = add_tbl[ dest_p->r + src_p->r ];
			}
		}
	}else{
		if( (r==128 && g==128 && b==128) ){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = add_tbl[ dest_p->b + src_p->b ];
						dest_p->g = add_tbl[ dest_p->g + src_p->g ];
						dest_p->r = add_tbl[ dest_p->r + src_p->r ];
					}
				}
			}else if( nuki==-2 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){
							dest_p->b = add_tbl[ dest_p->b + src_p->b ];
							dest_p->g = add_tbl[ dest_p->g + src_p->g ];
							dest_p->r = add_tbl[ dest_p->r + src_p->r ];
						}
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = add_tbl[ dest_p->b + src_p->b ];
							dest_p->g = add_tbl[ dest_p->g + src_p->g ];
							dest_p->r = add_tbl[ dest_p->r + src_p->r ];
						}
					}
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = add_tbl[ dest_p->b + btbl[src_p->b] ];
						dest_p->g = add_tbl[ dest_p->g + gtbl[src_p->g] ];
						dest_p->r = add_tbl[ dest_p->r + rtbl[src_p->r] ];
					}
				}
			}else if( nuki==-2 ){	
				char	*alph_tbl;
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){
							if( src_p->a==255 ){
								dest_p->b = add_tbl[ dest_p->b + btbl[src_p->b] ];
								dest_p->g = add_tbl[ dest_p->g + gtbl[src_p->g] ];
								dest_p->r = add_tbl[ dest_p->r + rtbl[src_p->r] ];
							}else{
								alph_tbl = BlendTable[ src_p->a ];

								dest_p->b = add_tbl[ dest_p->b + alph_tbl[btbl[src_p->b]] ];
								dest_p->g = add_tbl[ dest_p->g + alph_tbl[gtbl[src_p->g]] ];
								dest_p->r = add_tbl[ dest_p->r + alph_tbl[rtbl[src_p->r]] ];
							}
						}
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = add_tbl[ dest_p->b + btbl[src_p->b] ];
							dest_p->g = add_tbl[ dest_p->g + gtbl[src_p->g] ];
							dest_p->r = add_tbl[ dest_p->r + rtbl[src_p->r] ];
						}
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TT_Ooi( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	
	if( r==0 && g==0 && b==0 ){ return 1; }

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					dest_p->b = OoiTable[ dest_p->b ][ src_p->b ];
					dest_p->g = OoiTable[ dest_p->g ][ src_p->g ];
					dest_p->r = OoiTable[ dest_p->r ][ src_p->r ];
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( src_p->a!=0 ){
						dest_p->b = OoiTable[ dest_p->b ][ src_p->b ];
						dest_p->g = OoiTable[ dest_p->g ][ src_p->g ];
						dest_p->r = OoiTable[ dest_p->r ][ src_p->r ];
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						dest_p->b = OoiTable[ dest_p->b ][ src_p->b ];
						dest_p->g = OoiTable[ dest_p->g ][ src_p->g ];
						dest_p->r = OoiTable[ dest_p->r ][ src_p->r ];
					}
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					dest_p->b = OoiTable[ dest_p->b ][ btbl[src_p->b] ];
					dest_p->g = OoiTable[ dest_p->g ][ gtbl[src_p->g] ];
					dest_p->r = OoiTable[ dest_p->r ][ rtbl[src_p->r] ];
				}
			}
		}else if( nuki==-2 ){	
			char	*alph_tbl;
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( src_p->a!=0 ){
						if( src_p->a==255 ){
							dest_p->b = OoiTable[ dest_p->b ][ btbl[src_p->b] ];
							dest_p->g = OoiTable[ dest_p->g ][ gtbl[src_p->g] ];
							dest_p->r = OoiTable[ dest_p->r ][ rtbl[src_p->r] ];
						}else{
							alph_tbl = BlendTable[ src_p->a ];

							dest_p->b = OoiTable[ dest_p->b ][ alph_tbl[btbl[src_p->b]] ];
							dest_p->g = OoiTable[ dest_p->g ][ alph_tbl[gtbl[src_p->g]] ];
							dest_p->r = OoiTable[ dest_p->r ][ alph_tbl[rtbl[src_p->r]] ];
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						dest_p->b = OoiTable[ dest_p->b ][ btbl[src_p->b] ];
						dest_p->g = OoiTable[ dest_p->g ][ gtbl[src_p->g] ];
						dest_p->r = OoiTable[ dest_p->r ][ rtbl[src_p->r] ];
					}
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_TT_Sub( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		w2=w/2;
	int		w3=w%2;
	int		w4=w2*2;
	char	*sub_tbl = SubTable+255;

	
	if( r==0 && g==0 && b==0 ){ return 1; }

	
	if( MMX_Flag && nuki==-1 && (r==128 && g==128 && b==128) && !dobj->rparam ){
		for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
			if(w2){
				_asm {
					mov		ecx, w2
					mov		eax, src_p
					mov		ebx, dest_p

					loopx01:
						movq	mm0, [eax]
						movq	mm1, [ebx]
						psubusb	mm1, mm0
						movq	[ebx], mm1
						add		eax, 8
						add		ebx, 8
					loop	loopx01

					mov		src_p,  esi
					mov		dest_p, edi
				}
				_asm	emms
			}
			if( w3 ){	
				dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
				dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
				dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
			}
		}
	}else{
		if( (r==128 && g==128 && b==128) ){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
						dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
						dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
					}
				}
			}else if( nuki==-2 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){
							dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
							dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
							dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
						}
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
							dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
							dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
						}
					}
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = sub_tbl[ dest_p->b - btbl[src_p->b] ];
						dest_p->g = sub_tbl[ dest_p->g - gtbl[src_p->g] ];
						dest_p->r = sub_tbl[ dest_p->r - rtbl[src_p->r] ];
					}
				}
			}else if( nuki==-2 ){	
				char	*alph_tbl;
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){
							if( src_p->a==255 ){
								dest_p->b = sub_tbl[ dest_p->b - btbl[src_p->b] ];
								dest_p->g = sub_tbl[ dest_p->g - gtbl[src_p->g] ];
								dest_p->r = sub_tbl[ dest_p->r - rtbl[src_p->r] ];
							}else{
								alph_tbl = BlendTable[ src_p->a ];

								dest_p->b = sub_tbl[ dest_p->b - alph_tbl[btbl[src_p->b]] ];
								dest_p->g = sub_tbl[ dest_p->g - alph_tbl[gtbl[src_p->g]] ];
								dest_p->r = sub_tbl[ dest_p->r - alph_tbl[rtbl[src_p->r]] ];
							}
						}
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = sub_tbl[ dest_p->b - btbl[src_p->b] ];
							dest_p->g = sub_tbl[ dest_p->g - gtbl[src_p->g] ];
							dest_p->r = sub_tbl[ dest_p->r - rtbl[src_p->r] ];
						}
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TT_Mul( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*alph_tbl,*alp3_tbl,br;

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					dest_p->b = BrightTable_TB[ src_p->b ][ dest_p->b ];
					dest_p->g = BrightTable_TB[ src_p->g ][ dest_p->g ];
					dest_p->r = BrightTable_TB[ src_p->r ][ dest_p->r ];
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( src_p->a!=0 ){
						if( src_p->a==255 ){
							dest_p->b = BrightTable_TB[ src_p->b ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ src_p->g ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ src_p->r ][ dest_p->r ];
						}else{
							alph_tbl = BlendTable[ src_p->a ];
							br = (256-src_p->a)>>1;

							dest_p->b = BrightTable_TB[ alph_tbl[src_p->b]+br ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ alph_tbl[src_p->g]+br ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ alph_tbl[src_p->r]+br ][ dest_p->r ];
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						dest_p->b = BrightTable_TB[ src_p->b ][ dest_p->b ];
						dest_p->g = BrightTable_TB[ src_p->g ][ dest_p->g ];
						dest_p->r = BrightTable_TB[ src_p->r ][ dest_p->r ];
					}
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					dest_p->b = BrightTable_TB[ btbl[src_p->b] ][ dest_p->b ];
					dest_p->g = BrightTable_TB[ gtbl[src_p->g] ][ dest_p->g ];
					dest_p->r = BrightTable_TB[ rtbl[src_p->r] ][ dest_p->r ];
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( src_p->a!=0 ){
						if( src_p->a==255 ){
							dest_p->b = BrightTable_TB[ btbl[src_p->b] ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ gtbl[src_p->g] ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ rtbl[src_p->r] ][ dest_p->r ];
						}else{
							alph_tbl = BlendTable[ src_p->a ];
							alp3_tbl = BlendTable2[ src_p->a ];
							br = (256-src_p->a)>>1;

							dest_p->b = BrightTable_TB[ alph_tbl[btbl[alp3_tbl[src_p->b]]]+br ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ alph_tbl[gtbl[alp3_tbl[src_p->g]]]+br ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ alph_tbl[rtbl[alp3_tbl[src_p->r]]]+br ][ dest_p->r ];
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						dest_p->b = BrightTable_TB[ btbl[src_p->b] ][ dest_p->b ];
						dest_p->g = BrightTable_TB[ gtbl[src_p->g] ][ dest_p->g ];
						dest_p->r = BrightTable_TB[ rtbl[src_p->r] ][ dest_p->r ];
					}
				}
			}
		}
	}
	return TRUE;
}





BOOL DRW_DrawBMP_TT_Ami( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*linemask[4], *linemask2;
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;
	WORD	mesh_pt;
	int		mesh = dobj->dnum;
	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=0   )	return 1;							
	if( mesh>=256 )	return DRW_DrawBMP_TT_Std( dobj );	

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( linemask2[x & 3] )	*dest_p = *src_p;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( linemask2[x & 3] ){
						if( src_p->a != 0 ){
							if( src_p->a == 255 ){
								*dest_p = *src_p;
							}else{
								alph_tbl = BlendTable[ 255 - src_p->a ];
								dest_p->b = alph_tbl[dest_p->b] + src_p->b;
								dest_p->g = alph_tbl[dest_p->g] + src_p->g;
								dest_p->r = alph_tbl[dest_p->r] + src_p->r;
							}
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						if( linemask2[x & 3] )	*dest_p = *src_p;
					}
				}
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];
		char	*rtbl2 = BrightTable_TB[r];
		char	*gtbl2 = BrightTable_TB[g];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( linemask2[x & 3] )
						*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( linemask2[x & 3] ){
						if( src_p->a != 0 ){
							if( src_p->a == 255 ){
								*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
							}else{
								alph_tbl = BlendTable[ 255 - src_p->a ];
								alp2_tbl = BlendTable[ src_p->a ];
								alp3_tbl = BlendTable2[ src_p->a ];
								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[ btbl[alp3_tbl[src_p->b]]];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl2[alp3_tbl[src_p->g]]];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl2[alp3_tbl[src_p->r]]];
							}
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						if( linemask2[x & 3] )
							*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TT_Nis( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;

	int		mesh = dobj->dnum, m=mesh>>2;
	char	*mask;
	static int mfalg=0;

	if( mesh<=0   )	return 1;							
	if( mesh>=256 )	return DRW_DrawBMP_TT_Std( dobj );	

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				mask = MeshFadePatern64[m][y&63];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( mask[x&63] ){
						*dest_p = *src_p;
					}
				}
			}
		}else if( nuki==-2 ){
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				mask = MeshFadePatern64[m][y&63];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( mask[x&63] ){
						if( src_p->a!=0 ){	
							if( src_p->a==255 ){
								*dest_p = *src_p;
							}else{
								alph_tbl = BlendTable[ 255 - src_p->a ];
								dest_p->b = alph_tbl[dest_p->b] + src_p->b;
								dest_p->g = alph_tbl[dest_p->g] + src_p->g;
								dest_p->r = alph_tbl[dest_p->r] + src_p->r;
							}
						}
					}
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				mask = MeshFadePatern64[m][y&63];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( mask[x&63] ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							*dest_p = *src_p;
						}
					}
				}
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];
		char	*rtbl2 = BrightTable_TB[r];
		char	*gtbl2 = BrightTable_TB[g];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				mask = MeshFadePatern64[m][y&63];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( mask[x&63] ){
						*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
					}
				}
			}
		}else if( nuki==-2 ){
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				mask = MeshFadePatern64[m][y&63];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( mask[x&63] ){
						if( src_p->a != 0 ){
							if( src_p->a == 255 ){
								*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
							}else{
								alph_tbl = BlendTable[ 255 - src_p->a ];
								alp2_tbl = BlendTable[ src_p->a ];
								alp3_tbl = BlendTable2[ src_p->a ];
								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[ btbl[alp3_tbl[src_p->b]]];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl2[alp3_tbl[src_p->g]]];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl2[alp3_tbl[src_p->r]]];
							}
						}
					}
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				mask = MeshFadePatern64[m][y&63];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( mask[x&63] ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
						}
					}
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_TT_Neg( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;
	
	if(r==128 && g==128 && b==128){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*(DWORD*)dest_p = ~(*(DWORD*)src_p);
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( src_p->a!=0 ){
						if( src_p->a==255 ){
							*(DWORD*)dest_p = ~(*(DWORD*)src_p);
						}else{
							alph_tbl = BlendTable[ 255 - src_p->a ];
							alp2_tbl = BlendTable[ src_p->a ];
							alp3_tbl = BlendTable2[ src_p->a ];
							dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[~alp3_tbl[src_p->b]];
							dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[~alp3_tbl[src_p->g]];
							dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[~alp3_tbl[src_p->r]];
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						*(DWORD*)dest_p = ~(*(DWORD*)src_p);
					}
				}
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];
		char	*rtbl2 = BrightTable_TB[r];
		char	*gtbl2 = BrightTable_TB[g];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*(DWORD*)dest_p = ~(rtbl[src_p->r]+gtbl[src_p->g]+btbl[src_p->b]);
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( src_p->a!=0 ){
						if( src_p->a==255 ){
							*(DWORD*)dest_p = ~(rtbl[src_p->r]+gtbl[src_p->g]+btbl[src_p->b]);
						}else{
							alph_tbl = BlendTable[ 255 - src_p->a ];
							alp2_tbl = BlendTable[ src_p->a ];
							alp3_tbl = BlendTable2[ src_p->a ];
							dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[ ~btbl[alp3_tbl[src_p->b]]];
							dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[~gtbl2[alp3_tbl[src_p->g]]];
							dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[~rtbl2[alp3_tbl[src_p->r]]];
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						*(DWORD*)dest_p = ~(rtbl[src_p->r]+gtbl[src_p->g]+btbl[src_p->b]);
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TT_Viv( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		viv = dobj->dnum;
	int		glay;
	char	*viv_tbl=BlendTable[viv];
	char	*riv_tbl=BlendTable[256-viv];
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
	char	*btbl = BrightTable_TB[b];
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;

	if( viv==256 ){
		return DRW_DrawBMP_TT_Std( dobj );
	}
	if(viv==0){
		if(r==128 && g==128 && b==128){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
					}
				}
			}else if( nuki==-2 ){			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a != 0 ){
							if( src_p->a == 255 ){
								dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
							}else{
								alph_tbl = BlendTable[ 255-src_p->a ];
								glay = riv_tbl[(src_p->b+src_p->g+src_p->g+src_p->r)>>2];
								dest_p->b = alph_tbl[dest_p->b] + glay;
								dest_p->g = alph_tbl[dest_p->g] + glay;
								dest_p->r = alph_tbl[dest_p->r] + glay;
							}
						}
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
						}
					}
				}
			}
		}else{
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						glay = riv_tbl[(src_p->b+src_p->g+src_p->g+src_p->r)>>2];
						dest_p->b = btbl[glay];
						dest_p->g = gtbl[glay];
						dest_p->r = rtbl[glay];
					}
				}
			}else if( nuki==-2 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a != 0 ){
							if( src_p->a == 255 ){
								glay = riv_tbl[(src_p->b+src_p->g+src_p->g+src_p->r)>>2];
								dest_p->b = btbl[glay];
								dest_p->g = gtbl[glay];
								dest_p->r = rtbl[glay];
							}else{
								alph_tbl = BlendTable[255-src_p->a];
								alp2_tbl = BlendTable[src_p->a];
								glay = BlendTable2[src_p->a][ riv_tbl[(src_p->b+src_p->g+src_p->g+src_p->r)>>2] ];
								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[btbl[glay]];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl[glay]];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl[glay]];
							}
						}
					}
				}
			}else{					
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							glay = riv_tbl[(src_p->b+src_p->g+src_p->g+src_p->r)>>2];
							dest_p->b = btbl[glay];
							dest_p->g = gtbl[glay];
							dest_p->r = rtbl[glay];
						}
					}
				}
			}
		}
	}else{
		
		if(r==128 && g==128 && b==128){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						glay = riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
						dest_p->b = glay + viv_tbl[src_p->b];
						dest_p->g = glay + viv_tbl[src_p->g];
						dest_p->r = glay + viv_tbl[src_p->r];
					}
				}
			}else if( nuki==-2 ){			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a != 0 ){
							if( src_p->a == 255 ){
								glay = riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
								dest_p->b = glay + viv_tbl[src_p->b];
								dest_p->g = glay + viv_tbl[src_p->g];
								dest_p->r = glay + viv_tbl[src_p->r];
							}else{
								alph_tbl = BlendTable[ 255-src_p->a ];
								glay = riv_tbl[ (src_p->b + src_p->g + src_p->g + src_p->r)>>2 ];
								dest_p->b = alph_tbl[dest_p->b] + glay+viv_tbl[src_p->b];
								dest_p->g = alph_tbl[dest_p->g] + glay+viv_tbl[src_p->g];
								dest_p->r = alph_tbl[dest_p->r] + glay+viv_tbl[src_p->r];
							}
						}
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							glay = riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
							dest_p->b = glay + viv_tbl[src_p->b];
							dest_p->g = glay + viv_tbl[src_p->g];
							dest_p->r = glay + viv_tbl[src_p->r];
						}
					}
				}
			}
		}else{
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						glay = riv_tbl[ (src_p->b + src_p->g+src_p->g + src_p->r)>>2 ];
						dest_p->b = btbl[ glay + viv_tbl[src_p->b] ];
						dest_p->g = gtbl[ glay + viv_tbl[src_p->g] ];
						dest_p->r = rtbl[ glay + viv_tbl[src_p->r] ];
					}
				}
			}else if( nuki==-2 ){			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a != 0 ){
							if( src_p->a == 255 ){
								glay = riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
								dest_p->b = btbl[glay + viv_tbl[src_p->b]];
								dest_p->g = gtbl[glay + viv_tbl[src_p->g]];
								dest_p->r = rtbl[glay + viv_tbl[src_p->r]];
							}else{
								alph_tbl = BlendTable[ 255-src_p->a ];
								alp2_tbl = BlendTable[ src_p->a ];
								alp3_tbl = BlendTable2[ src_p->a ];
								glay = riv_tbl[ (src_p->b + src_p->g + src_p->g + src_p->r)>>2 ];
								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[btbl[alp3_tbl[glay+viv_tbl[src_p->b]]]];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl[alp3_tbl[glay+viv_tbl[src_p->g]]]];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl[alp3_tbl[glay+viv_tbl[src_p->r]]]];
							}
						}
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							glay = riv_tbl[ (src_p->b + src_p->g+src_p->g + src_p->r)>>2 ];
							dest_p->b = btbl[ glay + viv_tbl[src_p->b] ];
							dest_p->g = gtbl[ glay + viv_tbl[src_p->g] ];
							dest_p->r = rtbl[ glay + viv_tbl[src_p->r] ];
						}
					}
				}
			}
		}
	}

	return TRUE;
}





BOOL DRW_DrawBMP_TT_Bom( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		x2,y2;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		bom  = dobj->dnum;
	short	*bom_tbl = BomdTabl[bom];
	char	count=0;
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;

	if( bom<=0 ) return DRW_DrawBMP_TT_Std(dobj);

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
					x2 = bom_tbl[ count++ ]+x+dobj->dx;
					y2 = bom_tbl[ count ]+y+dobj->dy;

					if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
						dest_p = dest->buf + y2 * dest->sx + x2;
						*dest_p = *src_p;
					}
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
					if(src_p->a!=0){
						x2 = bom_tbl[ count++ ]+x+dobj->dx;
						y2 = bom_tbl[ count ]+y+dobj->dy;
						if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
							dest_p = dest->buf + y2 * dest->sx + x2;
							if(src_p->a==256){
								*dest_p = *src_p;
							}else{
								alph_tbl = BlendTable[ 255 - src_p->a ];
								dest_p->b = alph_tbl[dest_p->b] + src_p->b;
								dest_p->g = alph_tbl[dest_p->g] + src_p->g;
								dest_p->r = alph_tbl[dest_p->r] + src_p->r;
							}
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, src_p+=srev ){
				for( x=0 ; x < w ; x++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						x2 = bom_tbl[ count++ ]+x+dobj->dx;
						y2 = bom_tbl[ count ]+y+dobj->dy;

						if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
							dest_p = dest->buf + y2 * dest->sx + x2;
							*dest_p = *src_p;



						}
						count++;
					}
				}
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];
		char	*rtbl2 = BrightTable_TB[r];
		char	*gtbl2 = BrightTable_TB[g];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
					x2 = bom_tbl[ count++ ]+x+dobj->dx;
					y2 = bom_tbl[ count ]+y+dobj->dy;

					if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
						dest_p = dest->buf + y2 * dest->sx + x2;
						*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
					}
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
					if(src_p->a!=0){
						x2 = bom_tbl[ count++ ]+x+dobj->dx;
						y2 = bom_tbl[ count ]+y+dobj->dy;
						if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
							dest_p = dest->buf + y2 * dest->sx + x2;
							if(src_p->a==256){
								*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
							}else{
								alph_tbl = BlendTable[ 255 - src_p->a ];
								alp2_tbl = BlendTable[ src_p->a ];
								alp3_tbl = BlendTable2[ src_p->a ];
								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[ btbl[alp3_tbl[src_p->b]]];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl2[alp3_tbl[src_p->g]]];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl2[alp3_tbl[src_p->r]]];
							}
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, src_p+=srev ){
				for( x=0 ; x < w ; x++, src_p+=xinc ){
					if( ((*(long *)src_p)&0x00ffffff) != nuki ){
						x2 = bom_tbl[ count++ ]+x+dobj->dx;
						y2 = bom_tbl[ count ]+y+dobj->dy;

						if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
							dest_p = dest->buf + y2 * dest->sx + x2;
							*(DWORD*)dest_p = rtbl[ src_p->r ] | gtbl[ src_p->g ] | btbl[ src_p->b];
						}
					}
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_TT_Lcf( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		rate = dobj->dnum;
	int		rate2;
	int		line = dobj->dparam-DRW_LCF2;

	char	*blnd_tbl, *brev_tbl;
	int		rate_max[3];
	int		blnd, brev;

	switch(line){
	case UP:
		rate = (DispHeight+255)*(256-rate)/256;
		rate2 = STD_Limit( -(rate-255), 0, 255 );
		rate_max[0] = STD_Limit( rate-255, 0, h );
		rate_max[1] = STD_Limit( rate,     0, h );

		dest_p += dest->sx*rate_max[0];
		src_p  += src->sx *rate_max[0];

		blnd =   0 + rate2;
		brev = 256 - rate2;
		for( y=rate_max[0] ; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev, blnd++, brev-- ){
			blnd_tbl = BlendTable[blnd];
			brev_tbl = BlendTable[brev];
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
				dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
				dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
			}
		}
		for( ; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
			CopyMemory( dest_p, src_p, w*sizeof(RGB32) );
		}
		break;

	case DO:
		rate = (DispHeight+255)*rate/256;
		rate2 = STD_Limit( -(rate-255), 0, 255 );
		rate_max[0] = STD_Limit( rate-255, 0, h );
		rate_max[1] = STD_Limit( rate,     0, h );
		for( y=0; y < rate_max[0] ; y++, dest_p+=dest->sx, src_p+=src->sx ){
			CopyMemory( dest_p, src_p, w*sizeof(RGB32) );
		}
		blnd = 256 - rate2;
		brev =   0 + rate2;
		for( ; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev, blnd--, brev++ ){
			blnd_tbl = BlendTable[blnd];
			brev_tbl = BlendTable[brev];
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
				dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
				dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
			}
		}
		break;

	case RI:
		rate = (DispWidth+255)*(256-rate)/256;
		rate2 = STD_Limit( -(rate-255), 0, 255 );
		rate_max[0] = STD_Limit( rate-255, 0, w );
		rate_max[1] = STD_Limit( rate,     0, w );

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			blnd =   0 + rate2;
			brev = 256 - rate2;

			dest_p += rate_max[0];
			src_p  += rate_max[0];
			for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc, blnd++, brev-- ){
				dest_p->b = BlendTable[brev][ dest_p->b ] + BlendTable[blnd][ src_p->b ];
				dest_p->g = BlendTable[brev][ dest_p->g ] + BlendTable[blnd][ src_p->g ];
				dest_p->r = BlendTable[brev][ dest_p->r ] + BlendTable[blnd][ src_p->r ];
			}
			if(w>x){
				CopyMemory( dest_p, src_p, (w-x)*sizeof(RGB32) );
				dest_p += w-x;
				src_p  += w-x;
			}
		}
		break;

	case LE:
		rate = (DispWidth+255)*rate/256;
		rate2 = STD_Limit( -(rate-255), 0, 255 );
		rate_max[0] = STD_Limit( rate-255, 0, w );
		rate_max[1] = STD_Limit( rate,     0, w );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			blnd = 256 - rate2;
			brev =   0 + rate2;
			x=0;
			if(rate_max[0]>0){
				CopyMemory( dest_p, src_p, rate_max[0]*sizeof(RGB32) );
				dest_p += rate_max[0];
				src_p  += rate_max[0];
				x = rate_max[0];
			}
			for( ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc, blnd--, brev++ ){
				dest_p->b = BlendTable[brev][ dest_p->b ] + BlendTable[blnd][ src_p->b ];
				dest_p->g = BlendTable[brev][ dest_p->g ] + BlendTable[blnd][ src_p->g ];
				dest_p->r = BlendTable[brev][ dest_p->r ] + BlendTable[blnd][ src_p->r ];
			}
			dest_p += w-x;
			src_p  += w-x;
		}
		break;

	case CE:
		rate = (320+127)*(256-rate)/256;
		rate2 = STD_Limit( -(rate-127), 0, 127 )*2;
		rate_max[0] = STD_Limit( rate-127, 0, w/2 );
		rate_max[1] = STD_Limit( rate,     0, w/2 );

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			blnd =   0 + rate2;
			brev = 256 - rate2;

			dest_p += rate_max[0];
			src_p  += rate_max[0];
			for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc, blnd+=2, brev-=2 ){
				dest_p->b = BlendTable[brev][ dest_p->b ] + BlendTable[blnd][ src_p->b ];
				dest_p->g = BlendTable[brev][ dest_p->g ] + BlendTable[blnd][ src_p->g ];
				dest_p->r = BlendTable[brev][ dest_p->r ] + BlendTable[blnd][ src_p->r ];
			}
			if( w/2>x ){
				CopyMemory( dest_p, src_p, (w/2-x)*sizeof(RGB32) );
				dest_p += w/2-x;
				src_p  += w/2-x;
				x = w/2;
			}

			if( x>rate_max[1] ){
				CopyMemory( dest_p, src_p, (x-rate_max[1])*sizeof(RGB32) );
				dest_p += x-rate_max[1];
				src_p  += x-rate_max[1];
				x = rate_max[1];
			}
			for( ; x > rate_max[0] ; x--, dest_p++, src_p+=xinc, blnd-=2, brev+=2){
				dest_p->b = BlendTable[brev][ dest_p->b ] + BlendTable[blnd][ src_p->b ];
				dest_p->g = BlendTable[brev][ dest_p->g ] + BlendTable[blnd][ src_p->g ];
				dest_p->r = BlendTable[brev][ dest_p->r ] + BlendTable[blnd][ src_p->r ];
			}
			dest_p += rate_max[0];
			src_p  += rate_max[0];
		}
		break;

	case OU:
		rate = (320+127)*rate/256;
		rate2 = STD_Limit( -(rate-127), 0, 127 )*2;
		rate_max[0] = STD_Limit( rate-127, 0, w );
		rate_max[1] = STD_Limit( rate,     0, w );

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			blnd = 256 - rate2;
			brev =   0 + rate2;

			x=0;
			if(rate_max[0]>0){
				CopyMemory( dest_p, src_p, rate_max[0]*sizeof(RGB32) );
				dest_p += rate_max[0];
				src_p  += rate_max[0];
				x = rate_max[0];
			}
			for( ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc, blnd-=2, brev+=2 ){
				dest_p->b = BlendTable[brev][ dest_p->b ] + BlendTable[blnd][ src_p->b ];
				dest_p->g = BlendTable[brev][ dest_p->g ] + BlendTable[blnd][ src_p->g ];
				dest_p->r = BlendTable[brev][ dest_p->r ] + BlendTable[blnd][ src_p->r ];
			}
			dest_p += (w/2-x)*2;
			src_p  += (w/2-x)*2;

			for( ; x > rate_max[0] ; x--, dest_p++, src_p+=xinc, blnd+=2, brev-=2){
				dest_p->b = BlendTable[brev][ dest_p->b ] + BlendTable[blnd][ src_p->b ];
				dest_p->g = BlendTable[brev][ dest_p->g ] + BlendTable[blnd][ src_p->g ];
				dest_p->r = BlendTable[brev][ dest_p->r ] + BlendTable[blnd][ src_p->r ];
			}
			if(x>0){
				CopyMemory( dest_p, src_p, x*sizeof(RGB32) );
				dest_p += x;
				src_p  += x;
			}
		}
		break;
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TT_Lpp( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		rate = dobj->dnum;
	int		line = dobj->dparam-DRW_LPP2;
	int		rate_max[3];
	char	*alph_tbl;

	switch(line){
	case UP:
		if( nuki==-1 ){	
			rate = 46*(256-rate)/256;
			rate_max[0] = STD_Limit( rate*16-240, 0, h );
			rate_max[1] = STD_Limit( rate*16,     0, h );

			dest_p+=rate_max[0]*dest->sx;
			src_p +=rate_max[0]*src->sx;

			for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev ){
				if( (rate-(y>>4)) <= (y&0xf) ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						*dest_p = *src_p;
					}
				}else{
					dest_p+=w;
					src_p +=w;
				}
			}
			for( ; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *src_p;
				}
			}
		}else if( nuki==-2 ){	
			rate = 46*(256-rate)/256;
			rate_max[0] = STD_Limit( rate*16-240, 0, h );
			rate_max[1] = STD_Limit( rate*16,     0, h );

			dest_p+=rate_max[0]*dest->sx;
			src_p +=rate_max[0]*src->sx;

			for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev ){
				if( (rate-(y>>4)) <= (y&0xf) ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if(src_p->a!=0){
							if(src_p->a==255){
								*dest_p = *src_p;
							}else{
								alph_tbl = BlendTable[255-src_p->a];
								dest_p->b = alph_tbl[dest_p->b] + src_p->b;
								dest_p->g = alph_tbl[dest_p->g] + src_p->g;
								dest_p->r = alph_tbl[dest_p->r] + src_p->r;
							}
						}
					}
				}else{
					dest_p+=w;
					src_p +=w;
				}
			}
			for( ; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if(src_p->a!=0){
						if(src_p->a==255){
							*dest_p = *src_p;
						}else{
							alph_tbl = BlendTable[255-src_p->a];
							dest_p->b = alph_tbl[dest_p->b] + src_p->b;
							dest_p->g = alph_tbl[dest_p->g] + src_p->g;
							dest_p->r = alph_tbl[dest_p->r] + src_p->r;
						}
					}
				}
			}
		}else{
		}
		break;
	case DO:
		rate = 46*rate/256;
		rate_max[0] = min( h, rate*16-240 );
		rate_max[1] = min( h, rate*16 );
		for( y=0; y < rate_max[0] ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				*dest_p = *src_p;
			}
		}
		for( ; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev ){
			if( (rate-(y>>4)) > (y&0xf) ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *src_p;
				}
			}else{
				dest_p+=w;
				src_p +=w;
			}
		}
		break;
	case RI:
		rate = 56*(256-rate)/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w );
		rate_max[1] = STD_Limit( rate*16,     0, w );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			dest_p+=rate_max[0];
			src_p +=rate_max[0];
			for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc ){
				if( (rate-(x>>4)) <= (x&0xf) ){
					*dest_p = *src_p;
				}
			}
			for( ; x < w ; x++, dest_p++, src_p+=xinc ){
				*dest_p = *src_p;
			}
		}
		break;
	case LE:
		rate = 56*rate/256;
		rate_max[0] = min( w, rate*16-240 );
		rate_max[1] = min( w, rate*16 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < rate_max[0] ; x++, dest_p++, src_p+=xinc ){
				*dest_p = *src_p;
			}
			for( ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc ){
				if( (rate-(x>>4)) > (x&0xf) ){
					*dest_p = *src_p;
				}
			}
			dest_p+=w-x;
			src_p +=w-x;
		}
		break;
	case CE:
		rate = (20+16)*(256-rate)/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w/2 );
		rate_max[1] = STD_Limit( rate*16,     0, w/2 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			dest_p+=rate_max[0];
			src_p +=rate_max[0];
			for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc ){
				if( (rate-(x>>4)) <= (x&0xf) ){
					*dest_p = *src_p;
				}
			}
			for( ; x < w/2 ; x++, dest_p++, src_p+=xinc ){
				*dest_p = *src_p;
			}
			for( ; x > rate_max[1] ; x--, dest_p++, src_p+=xinc ){
				*dest_p = *src_p;
			}
			for( ; x > rate_max[0] ; x--, dest_p++, src_p+=xinc ){
				if( (rate-(x>>4)) <= ((x-1)&0xf) ){
					*dest_p = *src_p;
				}
			}
			dest_p+=rate_max[0];
			src_p +=rate_max[0];
		}
		break;
	case OU:
		rate = (20+16)*rate/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w/2 );
		rate_max[1] = STD_Limit( rate*16,     0, w/2 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < rate_max[0] ; x++, dest_p++, src_p+=xinc ){
				*dest_p = *src_p;
			}
			for( ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc ){
				if( (rate-(x>>4)) > (x&0xf) ){
					*dest_p = *src_p;
				}
			}
			dest_p+=(w/2-x)*2;
			src_p +=(w/2-x)*2;
			for( ; x > rate_max[0] ; x--, dest_p++, src_p+=xinc ){
				if( (rate-(x>>4)) > ((x-1)&0xf) ){
					*dest_p = *src_p;
				}
			}
			for( ; x > 0 ; x--, dest_p++, src_p+=xinc ){
				*dest_p = *src_p;
			}
		}
		break;
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TT_Dia( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	char	*blnd_tbl, *brev_tbl;
	int		x2, y2;
	int		mask=0x3f;
	int		harf=32;
	int		blnd;
	int		rate3= dobj->dnum;
	int		rate2;
	int		rate;
	int		type  = (dobj->dparam-DRW_DIA2)%3;
	int		bflag = (dobj->dparam-DRW_DIA2)/3;

	rate = (96>>type)*dobj->dnum/256;

	mask >>= type;
	harf >>= type;
	rate2 = rate-harf;

	if(bflag){
		blnd = 16+(rate3*rate3/256)/2;
		blnd_tbl = BlendTable[blnd];
		brev_tbl = BlendTable[256-blnd];

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			y2 = abs( (y&mask)-harf ) + rate2;
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				x2 = abs( (x&mask)-harf );
				if( x2 < y2 ){
					dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
					dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
					dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
				}
			}
		}
	}else{
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			y2 = abs( (y&mask)-harf ) + rate2;
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				x2 = abs( (x&mask)-harf );
				if( x2 < y2 ){
					*dest_p = *src_p;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_TT_Wav( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		yinc = YINC(dobj);
	int		wav = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		dy;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if(dobj->rparam&REV_W){
			for(y = 0; y < h; y++, dest_p+=drev ) {
				dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
				dy = (dy+y+h)%h * yinc;
				src_p = src->buf + DRW_GetStartPointerEx( dobj, 0, dy );

				for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *src_p;
				}
			}
		}else{
			for(y = 0; y < h; y++, dest_p+=dest->sx ) {
				dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
				dy = ((dy+y+h)%h) * yinc;
				src_p = src->buf + DRW_GetStartPointerEx( dobj, 0, dy );
				CopyMemory( dest_p, src_p, w*4 );
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];

		for(y = 0; y < h; y++, dest_p+=drev ) {
			dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
			dy = (dy+y+h)%h * yinc;
			src_p = src->buf + DRW_GetStartPointerEx( dobj, 0, dy );

			for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
				*(DWORD*)dest_p = rtbl[src_p->r]+ gtbl[src_p->g] + btbl[src_p->b];
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TT_Wbl( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		yinc = YINC(dobj);
	int		wav = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		dy;
	char	*bld_tbl = BlendTable[32];
	char	*rbl_tbl = BlendTable[256-32];

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		for(y = 0; y < h; y++, dest_p+=drev ) {
			dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
			dy = (dy+y+h)%h * yinc;
			src_p = src->buf + (dobj->s1x+dy+STY(dobj))*src->sx + dobj->s1x+STX(dobj);

			for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = rbl_tbl[dest_p->b] + bld_tbl[src_p->b];
				dest_p->g = rbl_tbl[dest_p->g] + bld_tbl[src_p->g];
				dest_p->r = rbl_tbl[dest_p->r] + bld_tbl[src_p->r];
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];

		for(y = 0; y < h; y++, dest_p+=drev ) {
			dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
			dy = (dy+y+h)%h * yinc;
			src_p = src->buf + (dobj->s1x+dy+STY(dobj))*src->sx + dobj->s1x+STX(dobj);

			for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
				*(DWORD*)dest_p = rtbl[src_p->r]+ gtbl[src_p->g] + btbl[src_p->b];
			}
		}
	}
	return TRUE;
}



static BOOL DRW_DrawZOOM_TT_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy );

BOOL DRW_DrawBMP_TT_Sui( DRAW_OBJECT *dobj )
{
	BMP_T		*dest = (BMP_T*)dobj->dest;
	BMP_T		*src = (BMP_T*)dobj->src1;
	int			wav = dobj->dnum&0x00ff;
	int			cnt = (dobj->dnum&0xff00)>>8;
	int			x,y, x1,x2, y1,y2;
	int			w2=dobj->dw/8, h2=dobj->dh/8;
	int			wavx = 40 * wav/256;
	int			wavy = 30 * wav/256;
	int			sx,sy;
	int			xx,yy,xxx,yyy;
	int			xinc = XINC(dobj);
	int			yinc = YINC(dobj);
	DRAW_OBJECT	dobj2;

	if( dobj->dnum==0 ){
		return DRW_DrawBMP_TT_Std( dobj );
	}

	xxx=0; yyy=0;
	if(dobj->rparam&REV_W) xxx = 7;
	if(dobj->rparam&REV_H) yyy = 7;

	sx = wavx*COS(cnt)/4096;
	sy = wavy*SIN(cnt)/4096;
	for( y=0, yy=yyy ; y<8 ; y++, yy+=yinc ){
		y1 = wavy*SIN(cnt+(yy  )*32)/4096-sy;
		y2 = wavy*SIN(cnt+(yy+1)*32)/4096-sy;
		for( x=0, xx=xxx ; x<8 ; x++, xx+=xinc ){
			x1 = wavx*COS(cnt+(xx  )*32)/4096-sx;
			x2 = wavx*COS(cnt+(xx+1)*32)/4096-sx;

			dobj2 = DRW_SetDrawObject(	3,dest, xx*w2,    yy*h2,    w2,       h2,
										3,src,  xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_NML, 0,0, dobj->rparam );
			DRW_DrawZOOM_TT_Std( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0 );
		}
	}

	return TRUE;
}




static BOOL DRW_DrawZOOM_TT_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy );

BOOL DRW_DrawBMP_TT_Sbl( DRAW_OBJECT *dobj )
{
	BMP_T		*dest = (BMP_T*)dobj->dest;
	BMP_T		*src = (BMP_T*)dobj->src1;
	int			wav = dobj->dnum&0x00ff;
	int			cnt = (dobj->dnum&0xff00)>>8;
	int			x,y, x1,x2, y1,y2;
	int			w2=dobj->dw/8, h2=dobj->dh/8;
	int			wavx = 40 * wav/256;
	int			wavy = 30 * wav/256;
	int			sx, sy;
	int			xx,yy,xxx,yyy;
	int			xinc = XINC(dobj);
	int			yinc = YINC(dobj);
	DRAW_OBJECT	dobj2;

	xxx=0; yyy=0;
	if(dobj->rparam&REV_W) xxx = 7;
	if(dobj->rparam&REV_H) yyy = 7;

	sx = wavx*COS(cnt)/4096;
	sy = wavy*SIN(cnt)/4096;
	for( y=0, yy=yyy ; y<8 ; y++, yy+=yinc ){
		y1 = wavy*SIN(cnt+(yy  )*32)/4096-sy;
		y2 = wavy*SIN(cnt+(yy+1)*32)/4096-sy;
		for( x=0, xx=xxx ; x<8 ; x++, xx+=xinc ){
			x1 = wavx*COS(cnt+(xx  )*32)/4096-sx;
			x2 = wavx*COS(cnt+(xx+1)*32)/4096-sx;

			dobj2 = DRW_SetDrawObject(	3,dest, xx*w2,    yy*h2,    w2,       h2,
										3,src,  xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_BLD(32), 0,0, dobj->rparam );
			DRW_DrawZOOM_TT_Bld( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0 );
		}
	}

	return TRUE;
}



BOOL DRW_DrawBMP_TT_Lst( DRAW_OBJECT *dobj )
{
	int		x, y, xx;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	int		lst = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);

	lst = lst*w/640;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){	
			if(dobj->rparam&REV_W){
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=yinc ){
					xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
					if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
						sss = src_p+xx;
						for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
							*dest_p = *sss;
						}
					}else{
						dest_p+=w;
					}
				}
			}else{
				for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=yinc ){
					xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
					if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
						sss = src_p+xx;
						CopyMemory( dest_p, sss, w*4 );	
					}
				}
			}
		}else if(nuki==-2){
			char	*alph_tbl;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=yinc ){
				xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
						if( sss->a!=0 ){	
							if( sss->a==255 ){	
								*dest_p = *sss;
							}else{						
								alph_tbl = BlendTable[255-sss->a];
								dest_p->b = alph_tbl[dest_p->b] + sss->b;
								dest_p->g = alph_tbl[dest_p->g] + sss->g;
								dest_p->r = alph_tbl[dest_p->r] + sss->r;
							}
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=yinc ){
				xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
						if( ((*((long *)sss))&0x00ffffff) != nuki ){
							*dest_p = *sss;
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=yinc ){
				xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
						*(DWORD*)dest_p = rtbl[ sss->r ] | gtbl[ sss->g ] | btbl[ sss->b];
					}
				}else{
					dest_p+=w;
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=yinc ){
				xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
						if(*(long*)sss!=nuki){
							*(DWORD*)dest_p = rtbl[ sss->r ] | gtbl[ sss->g ] | btbl[ sss->b];
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}
	}

	return TRUE;
}



BOOL DRW_DrawBMP_TT_Lbl( DRAW_OBJECT *dobj )
{
	int		x, y, xx;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	int		lst = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	char	*blnd_tbl = BlendTable[256-lst];
	char	*brev_tbl = BlendTable[lst];

	if( lst==0   )	return DRW_DrawBMP_TT_Std( dobj );	
	if( lst==255 )	return 1;							

	lst = lst*w/640;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
						dest_p->b = brev_tbl[dest_p->b] + blnd_tbl[sss->b];
						dest_p->g = brev_tbl[dest_p->g] + blnd_tbl[sss->g];
						dest_p->r = brev_tbl[dest_p->r] + blnd_tbl[sss->r];
					}
				}else{
					dest_p+=w;
				}
			}
		}else if(nuki==-2){
			char	*alph_tbl;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
						if( sss->a!=0 ){	
							if( sss->a==255 ){	
								dest_p->b = brev_tbl[dest_p->b] + blnd_tbl[ sss->b ];
								dest_p->g = brev_tbl[dest_p->g] + blnd_tbl[ sss->g ];
								dest_p->r = brev_tbl[dest_p->r] + blnd_tbl[ sss->r ];
							}else{						
								alph_tbl = BlendTable[ 255-blnd_tbl[sss->a] ];
								dest_p->b = alph_tbl[dest_p->b] + blnd_tbl[ sss->b ];
								dest_p->g = alph_tbl[dest_p->g] + blnd_tbl[ sss->g ];
								dest_p->r = alph_tbl[dest_p->r] + blnd_tbl[ sss->r ];
							}
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
						if( ((*((long *)sss))&0x00ffffff) != nuki ){
							dest_p->b = brev_tbl[dest_p->b] + blnd_tbl[sss->b];
							dest_p->g = brev_tbl[dest_p->g] + blnd_tbl[sss->g];
							dest_p->r = brev_tbl[dest_p->r] + blnd_tbl[sss->r];
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
						dest_p->b = brev_tbl[dest_p->b] + blnd_tbl[btbl[sss->b]];
						dest_p->g = brev_tbl[dest_p->g] + blnd_tbl[gtbl[sss->g]];
						dest_p->r = brev_tbl[dest_p->r] + blnd_tbl[rtbl[sss->r]];
					}
				}else{
					dest_p+=w;
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
						if(*(long*)sss!=nuki){
							dest_p->b = brev_tbl[dest_p->b] + blnd_tbl[btbl[sss->b]];
							dest_p->g = brev_tbl[dest_p->g] + blnd_tbl[gtbl[sss->g]];
							dest_p->r = brev_tbl[dest_p->r] + blnd_tbl[rtbl[sss->r]];
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}
	}

	return TRUE;
}





BOOL DRW_DrawBMP_TT( DRAW_OBJECT dobj )
{
	BOOL	ret=TRUE;
	BMP_T	*dest = (BMP_T*)dobj.dest;
	BMP_T	*src  = (BMP_T*)dobj.src1;

	
	if( dest->buf==NULL ) {	DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_TT]\n"); return FALSE;}
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_TT]\n"); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRect( &dobj.dx,  &dobj.dy,  dest->sx, dest->sy,
					   &dobj.s1x, &dobj.s1y, src->sx,  src->sy,  &dobj.dw, &dobj.dh, dobj.clip ) ) return FALSE;
	}
	
	if( dobj.dparam==DRW_AMI2 || dobj.dparam==DRW_BLD2 ){
		if( dobj.dnum==BLD_0P   ) return 1;			
		if( dobj.dnum==BLD_100P ){
			dobj.dparam = DRW_NML;	
		}
	}
	switch(dobj.dparam){
		default:

		case DRW_NML:	DRW_DrawBMP_TT_Std( &dobj );	break;	
		case DRW_BLD2:	DRW_DrawBMP_TT_Bld( &dobj );	break;	
		case DRW_ADD:	DRW_DrawBMP_TT_Add( &dobj );	break;	
		case DRW_OOI:	DRW_DrawBMP_TT_Ooi( &dobj );	break;	
		case DRW_SUB:	DRW_DrawBMP_TT_Sub( &dobj );	break;	
		case DRW_MUL:	DRW_DrawBMP_TT_Mul( &dobj );	break;	
		case DRW_AMI2:	DRW_DrawBMP_TT_Ami( &dobj );	break;	
		case DRW_NIS2:	DRW_DrawBMP_TT_Nis( &dobj );	break;	
		case DRW_NEG:	DRW_DrawBMP_TT_Neg(	&dobj );	break;	
		case DRW_VIV2:	DRW_DrawBMP_TT_Viv( &dobj );	break;	
		case DRW_BOM2:	DRW_DrawBMP_TT_Bom(	&dobj );	break;	



		case DRW_SUI2:	DRW_DrawBMP_TT_Sui(	&dobj );	break;	
		case DRW_SBL2:	DRW_DrawBMP_TT_Sbl(	&dobj );	break;	
		case DRW_WAV2:	DRW_DrawBMP_TT_Wav(	&dobj );	break;	
		case DRW_WBL2:	DRW_DrawBMP_TT_Wbl(	&dobj );	break;	
		case DRW_LST2:	DRW_DrawBMP_TT_Lst(	&dobj );	break;	
		case DRW_LBL2:	DRW_DrawBMP_TT_Lbl(	&dobj );	break;	

		case DRW_LCF2:		case DRW_LCF2+1:	case DRW_LCF2+2:	
		case DRW_LCF2+3:	case DRW_LCF2+4:	case DRW_LCF2+5:
			DRW_DrawBMP_TT_Lcf(	&dobj );
			break;

		case DRW_LPP2:		case DRW_LPP2+1:	case DRW_LPP2+2:
		case DRW_LPP2+3:	case DRW_LPP2+4:	case DRW_LPP2+5:	
			DRW_DrawBMP_TT_Lpp( &dobj );
			break;

		case DRW_DIA2:	case DRW_DIA2+1:	case DRW_DIA2+2:
		case DRW_DIO2:	case DRW_DIO2+1:	case DRW_DIO2+2:
			DRW_DrawBMP_TT_Dia( &dobj );
			break;
	}
	return ret;
}









BOOL DRW_DrawBMP_TB_Std( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);

	
	if( dobj->nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				*dest_p =  spal[ *src_p ];
			}
		}
	}else if( dobj->nuki==-2 && src->alp ){	
		char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );
		char	*alp_tbl, *al2_tbl;

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
				if( *alp_p != 0 ){
					if( *alp_p == 255 ){
						*dest_p =  spal[ *src_p ];
					}else{
						alp_tbl = BlendTable[*alp_p];
						al2_tbl = BlendTable[255-*alp_p];
						dest_p->b = al2_tbl[dest_p->b] + alp_tbl[spal[*src_p].b];
						dest_p->g = al2_tbl[dest_p->g] + alp_tbl[spal[*src_p].g];
						dest_p->r = al2_tbl[dest_p->r] + alp_tbl[spal[*src_p].r];
					}
				}
			}
		}
	}else if( dobj->nuki==-4 && src->alp ){	
		char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
				if( *alp_p > 128  ){
					*dest_p =  spal[ *src_p ];
				}
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if( *src_p != dobj->nuki ){
					*dest_p =  spal[ *src_p ];
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_TB_Bld( DRAW_OBJECT *dobj, BMP_BT *src, RGB32 *spal2 )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	int		blnd = dobj->dnum;
	int		brev = 256-blnd;
	char	*blend_tbl=BlendTable[brev];
	char	*blen2_tbl=BlendTable[blnd];

	if( blnd==BLD_0P   )	return 1;								
	if( blnd==BLD_100P )	return DRW_DrawBMP_TB_Std( dobj, src );	

	if( MMX_Flag && !dobj->rparam && dobj->nuki!=-2){
		int		w2=w/2;
		int		w3=w%2;
		int		w4=w2*2;
		DWORD	alpha2 = brev * 0x01010101;
		DWORD	alpha1 = blnd * 0x01010101;
		if( dobj->nuki==-1 ){
			for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
				if(w2){
					_asm {
						mov			ecx, w2
						mov			esi, src_p
						mov			edi, dest_p
						mov			eax, spal
						xor			edx, edx
						xor			ebx, ebx
						pxor		mm3, mm3	
						movd		mm5, alpha2	
						punpcklbw	mm5, mm3	

						loopx01:
							
							mov		dx,  [esi]
							mov		bl,  dh
							xor		dh,  dh
							xor		bh,  bh
							shl		bx,  2
							shl		dx,  2
							movd	mm0, [eax+edx]
							movd	mm1, [eax+ebx]
							psllq	mm1, 32
							por		mm0, mm1
							
							movq		mm1, [edi]

							
							movq		mm7, mm1	
							
							punpcklbw	mm1, mm3	
							
							punpckhbw	mm7, mm3	
							
							pmullw		mm1, mm5	
							psrlw		mm1, 8		
							pmullw		mm7, mm5	
							psrlw		mm7, 8		
							packuswb	mm1, mm7	
							paddusb		mm0, mm1	
							
							movq		[edi], mm0	

							add		esi, 2
							add		edi, 8
						loop	loopx01
					}
				}
				if(w3){
					(dest_p+w4)->b = blend_tbl[ (dest_p+w4)->b ] + spal[*(src_p+w4)].b;
					(dest_p+w4)->g = blend_tbl[ (dest_p+w4)->g ] + spal[*(src_p+w4)].g;
					(dest_p+w4)->r = blend_tbl[ (dest_p+w4)->r ] + spal[*(src_p+w4)].r;
				}
			}
		}else{
			char	nukib = dobj->nuki;
			for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
				if(w2){
					_asm {
						mov			ecx, w2
						mov			esi, src_p
						mov			edi, dest_p
						mov			eax, spal
						xor			edx, edx
						xor			ebx, ebx
						pxor		mm3, mm3	
						movd		mm5, alpha2	
						punpcklbw	mm5, mm3	

						mov			bl, nukib

						loopx02:
							
							mov		dl,  [esi]
							movd	mm0, [eax+edx*4]
							mov		dl,  [esi+1]
							movd	mm1, [eax+edx*4]
							psllq	mm0, 32
							por		mm0, mm1

							
							movq		mm1, [edi]

							
							movq		mm7, mm1	

							punpcklbw	mm1, mm3	
							punpckhbw	mm7, mm3	
							
							pmullw		mm1, mm5	
							psrlw		mm1, 8		
							pmullw		mm7, mm5	
							psrlw		mm7, 8		
							
							packuswb	mm1, mm7	
							paddusb		mm0, mm1	

							cmp		bl, [esi]	
							je	LOW01
							jmp	LOW02
							
							LOW01:
								cmp		bl, [esi+1]	
								
								je	NEXT01
								
									movd	[edi+4], mm0
								jmp	NEXT01
							
							LOW02:
								cmp		bl, [esi+1]	
								jne	HIGH01
								
									psrlq	mm0, 32
									movd	[edi], mm0
								jmp	NEXT01
								
								HIGH01:
									movq	[edi], mm0
								jmp	NEXT01
							NEXT01:
							
							add		esi, 2
							add		edi, 8
						loop	loopx02
					}
				}
				if(w3 && dobj->nuki!=*src_p ){
					(dest_p+w4)->b = blend_tbl[ (dest_p+w4)->b ] + spal[*(src_p+w4)].b;
					(dest_p+w4)->g = blend_tbl[ (dest_p+w4)->g ] + spal[*(src_p+w4)].g;
					(dest_p+w4)->r = blend_tbl[ (dest_p+w4)->r ] + spal[*(src_p+w4)].r;
				}
			}
		}
		_asm	emms
	}else{
		
		if( dobj->nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					dest_p->b = blend_tbl[ dest_p->b ]+ spal[*src_p].b;
					dest_p->g = blend_tbl[ dest_p->g ]+ spal[*src_p].g;
					dest_p->r = blend_tbl[ dest_p->r ]+ spal[*src_p].r;
				}
			}
		}else if( dobj->nuki==-2 && src->alp ){	
			char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );
			char	*alp_tbl, *al2_tbl;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
					if( *alp_p != 0 ){
						if( *alp_p == 255 ){
							dest_p->b = blend_tbl[ dest_p->b ]+ spal[*src_p].b;
							dest_p->g = blend_tbl[ dest_p->g ]+ spal[*src_p].g;
							dest_p->r = blend_tbl[ dest_p->r ]+ spal[*src_p].r;
						}else{
							alp_tbl = BlendTable[ blen2_tbl[*alp_p] ];
							al2_tbl = BlendTable[ 255-blen2_tbl[*alp_p] ];
							dest_p->b = al2_tbl[dest_p->b] + alp_tbl[spal2[*src_p].b];
							dest_p->g = al2_tbl[dest_p->g] + alp_tbl[spal2[*src_p].g];
							dest_p->r = al2_tbl[dest_p->r] + alp_tbl[spal2[*src_p].r];
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( *src_p != dobj->nuki ){
						dest_p->b = blend_tbl[ dest_p->b ]+ spal[*src_p].b;
						dest_p->g = blend_tbl[ dest_p->g ]+ spal[*src_p].g;
						dest_p->r = blend_tbl[ dest_p->r ]+ spal[*src_p].r;
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TB_Add( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	char	*add_tbl = AddTable;

	
	if( MMX_Flag && nuki==-1 && !dobj->rparam ){
		int		w2=w/2;
		int		w3=w%2;
		int		w4=w2*2;
		for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
			if(w2){
				_asm {
					mov			ecx, w2
					mov			esi, src_p
					mov			edi, dest_p
					mov			eax, spal
					xor			edx, edx

					loopx01:
	
						
						mov		dl, [esi]
						movd	mm0, [eax+edx*4]
						mov		dl, [esi+1]
						movd	mm1, [eax+edx*4]
						psllq	mm1, 32
						por		mm0, mm1
						
						movq		mm1, [edi]

						paddusb	mm0, mm1
						movq	[edi], mm0
						
						add		esi, 2
						add		edi, 8
					loop	loopx01
				}
				_asm	emms
			}
			if(w3){
				(dest_p+w4)->b = add_tbl[ (dest_p+w4)->b + spal[ *(src_p+w4) ].b ];
				(dest_p+w4)->g = add_tbl[ (dest_p+w4)->g + spal[ *(src_p+w4) ].g ];
				(dest_p+w4)->r = add_tbl[ (dest_p+w4)->r + spal[ *(src_p+w4) ].r ];
			}
		}
	}else{
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					dest_p->b = add_tbl[ dest_p->b + spal[*src_p].b ];
					dest_p->g = add_tbl[ dest_p->g + spal[*src_p].g ];
					dest_p->r = add_tbl[ dest_p->r + spal[*src_p].r ];
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( *src_p != nuki ){
						dest_p->b = add_tbl[ dest_p->b + spal[*src_p].b ];
						dest_p->g = add_tbl[ dest_p->g + spal[*src_p].g ];
						dest_p->r = add_tbl[ dest_p->r + spal[*src_p].r ];
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TB_Ooi( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	char	*add_tbl = AddTable;

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = OoiTable[ dest_p->b ][ spal[*src_p].b ];
				dest_p->g = OoiTable[ dest_p->g ][ spal[*src_p].g ];
				dest_p->r = OoiTable[ dest_p->r ][ spal[*src_p].r ];
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if( *src_p != nuki ){
					dest_p->b = OoiTable[ dest_p->b ][ spal[*src_p].b ];
					dest_p->g = OoiTable[ dest_p->g ][ spal[*src_p].g ];
					dest_p->r = OoiTable[ dest_p->r ][ spal[*src_p].r ];
				}
			}
		}
	}

	return TRUE;
}



BOOL DRW_DrawBMP_TB_Sub( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	char	*sub_tbl = SubTable+255;

	if( MMX_Flag && nuki==-1 && !dobj->rparam ){
		int		w2=w/2;
		int		w3=w%2;
		int		w4=w2*2;

		for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
			if(w2){
				_asm {
					mov			ecx, w2
					mov			esi, src_p
					mov			edi, dest_p
					mov			eax, spal
					xor			edx, edx

					loopx01:
	
						
						mov		dl, [esi]
						movd	mm0, [eax+edx*4]
						mov		dl, [esi+1]
						movd	mm1, [eax+edx*4]
						psllq	mm1, 32
						por		mm0, mm1
						
						movq		mm1, [edi]

						psubusb	mm1, mm0
						movq	[edi], mm1
						
						add		esi, 2
						add		edi, 8
					loop	loopx01
				}
				_asm	emms
			}
			if(w3){
				(dest_p+w4)->b = sub_tbl[ (dest_p+w4)->b - spal[ *(src_p+w4) ].b ];
				(dest_p+w4)->g = sub_tbl[ (dest_p+w4)->g - spal[ *(src_p+w4) ].g];
				(dest_p+w4)->r = sub_tbl[ (dest_p+w4)->r - spal[ *(src_p+w4) ].r  ];
			}
		}
	}else{
		
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					dest_p->b = sub_tbl[ dest_p->b - spal[*src_p].b  ];
					dest_p->g = sub_tbl[ dest_p->g - spal[*src_p].g ];
					dest_p->r = sub_tbl[ dest_p->r - spal[*src_p].r   ];
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if(*src_p!=nuki){
						dest_p->b = sub_tbl[ dest_p->b - spal[*src_p].b  ];
						dest_p->g = sub_tbl[ dest_p->g - spal[*src_p].g ];
						dest_p->r = sub_tbl[ dest_p->r - spal[*src_p].r   ];
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TB_Mul( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = BrightTable_TB[ spal[*src_p].b ][ dest_p->b ];
				dest_p->g = BrightTable_TB[ spal[*src_p].g ][ dest_p->g ];
				dest_p->r = BrightTable_TB[ spal[*src_p].r ][ dest_p->r ];
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if(*src_p!=nuki){
					dest_p->b = BrightTable_TB[ spal[*src_p].b ][ dest_p->b ];
					dest_p->g = BrightTable_TB[ spal[*src_p].g ][ dest_p->g ];
					dest_p->r = BrightTable_TB[ spal[*src_p].r ][ dest_p->r ];
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TB_Ami( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	int		mesh= dobj->dnum;
	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=0   )	return 1;								
	if( mesh>=256 )	return DRW_DrawBMP_TB_Std( dobj, src );	

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			linemask2 = linemask[y & 3];
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if( linemask2[x & 3] )	*dest_p = spal[*src_p];
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			linemask2 = linemask[y & 3];
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if( *src_p!=nuki && linemask2[x & 3] )
					*dest_p = spal[*src_p];
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_TB_Bom( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		x2,y2;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	int		bom = dobj->dnum;
	short	*bom_tbl = BomdTabl[bom];
	char	count=0;

	if( bom<=0 )	return DRW_DrawBMP_TB_Std( dobj, src );	

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, src_p+=srev ){
			for( x=0 ; x < w ; x++, src_p+=xinc, count+=(x+y+*src_p) ){
				x2 = bom_tbl[ count++ ]+x+dobj->dx;
				y2 = bom_tbl[ count ]+y+dobj->dy;

				if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
					dest_p = dest->buf + y2 * dest->sx + x2;
					*dest_p = spal[*src_p];
				}
			}
		}
	}else{			
		for( y=0; y < h ; y++, src_p+=srev ){
			for( x=0 ; x < w ; x++, src_p+=xinc ){
				if( *src_p!=nuki ){
					x2 = bom_tbl[ count++ ]+x+dobj->dx;
					y2 = bom_tbl[ count ]+y+dobj->dy;

					if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
						dest_p = dest->buf + y2 * dest->sx + x2;
						*dest_p = spal[*src_p];
					}
				}
			}
		}
	}
	return TRUE;
}



static BOOL DRW_DrawZOOM_TB_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src );

BOOL DRW_DrawBMP_TB_Sui( DRAW_OBJECT *dobj, BMP_BT *src )
{
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		wav = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		x,y, x1,x2, y1,y2;
	int		w2=dobj->dw/8, h2=dobj->dh/8;
	int		wavx = 40 * wav/256;
	int		wavy = 30 * wav/256;
	int		sx, sy;
	DRAW_OBJECT	dobj2;
	int			xinc = XINC(dobj);
	int			yinc = YINC(dobj);
	int			xx,yy,xxx,yyy;

	if( wav==0 ){
		return DRW_DrawBMP_TB_Std( dobj, src );
	}

	xxx=0; yyy=0;
	if(dobj->rparam&REV_W) xxx = 7;
	if(dobj->rparam&REV_H) yyy = 7;

	sx = wavx*COS(cnt)/4096;
	sy = wavy*SIN(cnt)/4096;
	for( y=0, yy=yyy ; y<8 ; y++, yy+=yinc ){
		y1 = wavy*SIN(cnt+(yy  )*32)/4096-sy;
		y2 = wavy*SIN(cnt+(yy+1)*32)/4096-sy;
		for( x=0, xx=xxx ; x<8 ; x++, xx+=xinc ){
			x1 = wavx*COS(cnt+(xx  )*32)/4096-sx;
			x2 = wavx*COS(cnt+(xx+1)*32)/4096-sx;

			dobj2 = DRW_SetDrawObject(	3,dest, xx*w2,    yy*h2,    w2,       h2,
										0,src,  xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_NML, 0,0, dobj->rparam );
			DRW_DrawZOOM_TB_Std( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0, src );
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_TB_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src );

BOOL DRW_DrawBMP_TB_Sbl( DRAW_OBJECT *dobj, BMP_BT *src )
{
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		wav = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		x,y, x1,x2, y1,y2;
	int		w2=dobj->dw/8, h2=dobj->dh/8;
	int		wavx = 40 * wav/256;
	int		wavy = 30 * wav/256;
	int		sx, sy;
	DRAW_OBJECT	dobj2;
	int			xinc = XINC(dobj);
	int			yinc = YINC(dobj);
	int			xx,yy,xxx,yyy;

	xxx=0; yyy=0;
	if(dobj->rparam&REV_W) xxx = 7;
	if(dobj->rparam&REV_H) yyy = 7;

	sx = wavx*COS(cnt)/4096;
	sy = wavy*SIN(cnt)/4096;
	for( y=0, yy=yyy ; y<8 ; y++, yy+=yinc ){
		y1 = wavy*SIN(cnt+(yy  )*32)/4096-sy;
		y2 = wavy*SIN(cnt+(yy+1)*32)/4096-sy;
		for( x=0, xx=xxx ; x<8 ; x++, xx+=xinc ){
			x1 = wavx*COS(cnt+(xx  )*32)/4096-sx;
			x2 = wavx*COS(cnt+(xx+1)*32)/4096-sx;

			dobj2 = DRW_SetDrawObject(	3,dest, xx*w2,    yy*h2,    w2,       h2,
										0,src,  xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_BLD(128), 0,0, dobj->rparam );
			DRW_DrawZOOM_TB_Bld( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0, src );
		}
	}
	return TRUE;
}







BOOL DRW_DrawBMP_TB( DRAW_OBJECT dobj )
{
	BOOL	ret=TRUE;
	BMP_T	*dest = (BMP_T*)dobj.dest;
	BMP_BT	*src  = (BMP_BT*)dobj.src1;
	RGB32	pal[256];
	BMP_BT	src2={ src->buf, src->alp, pal, src->sx, src->sy };

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_TB]\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_TB]\n"); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRect(  &dobj.dx,  &dobj.dy,  dest->sx, dest->sy,
						&dobj.s1x, &dobj.s1y, src->sx,  src->sy,  &dobj.dw, &dobj.dh, dobj.clip ) ) return 0;
	}
	
	if( dobj.dparam==DRW_AMI2 || dobj.dparam==DRW_BLD2 ){
		if( dobj.dnum==BLD_0P   ) return 1;	
		if( dobj.dnum==BLD_100P ){
			dobj.dparam = DRW_NML;	
		}
	}

	if( dobj.r==128 && dobj.g==128 && dobj.b==128 &&
		!(dobj.dparam==DRW_BLD2 || dobj.dparam==DRW_NEG || dobj.dparam==DRW_SBL2 || dobj.dparam==DRW_VIV2) ){
		src2.pal = src->pal;
	}else{
		CopyMemory( pal, src->pal, 256*4 );
		if( dobj.dparam==DRW_VIV2 ){
			BMP_ChangePalFine_Full( pal, 256-dobj.dnum );
			BMP_ChangePalBright_Full( pal, dobj.r, dobj.g, dobj.b );
		}else{						
			BMP_ChangePalBright_Full( pal, dobj.r, dobj.g, dobj.b );
			switch(dobj.dparam){
				case DRW_SBL2:	BMP_ChangePalBright_Full( pal, 128/2, 128/2, 128/2 );					break;
				case DRW_BLD2:	BMP_ChangePalBright_Full( pal, dobj.dnum/2, dobj.dnum/2, dobj.dnum/2 );	break;
				case DRW_NEG:	BMP_ChangePalNega_Full( pal );											break;	
			}
		}
	}

	switch(dobj.dparam){
		default:

		case DRW_NEG:
		case DRW_VIV2:
		case DRW_NML:	DRW_DrawBMP_TB_Std( &dobj, &src2 );	break;	
		case DRW_BLD2:	DRW_DrawBMP_TB_Bld( &dobj, &src2, src->pal );	break;	
		case DRW_ADD:	DRW_DrawBMP_TB_Add( &dobj, &src2 );	break;	
		case DRW_OOI:	DRW_DrawBMP_TB_Ooi( &dobj, &src2 );	break;	
		case DRW_SUB:	DRW_DrawBMP_TB_Sub( &dobj, &src2 );	break;	
		case DRW_MUL:	DRW_DrawBMP_TB_Mul( &dobj, &src2 );	break;	
		case DRW_AMI2:	DRW_DrawBMP_TB_Ami( &dobj, &src2 );	break;	
		case DRW_BOM2:	DRW_DrawBMP_TB_Bom( &dobj, &src2 );	break;	

		case DRW_SUI2:	DRW_DrawBMP_TB_Sui(	&dobj, &src2 );	break;	
		case DRW_SBL2:	DRW_DrawBMP_TB_Sbl(	&dobj, &src2 );	break;	
	}
	return ret;
}

































BOOL DRW_DrawZOOM2_TT_Std(	BMP_T *dest, int dx, int dy,
							BMP_T *src,  int sx, int sy, int sw, int sh,
							int nuki, int r, int g, int b )
{
	int			x, y;
	DWORD		*src_p  = (DWORD*)src->buf  + sy*src->sx  + sx;
	DWORD		*dest_p = (DWORD*)dest->buf + dy*dest->sx + dx;
	int			drev = (dest->sx-sw)*2;
	int			srev = (src->sx-sw);
	int			ddx = dest->sx;
	int			ssx = src->sx;

	if(nuki == -3){
		dy+=2;	sy+=1;	sh-=2;
		src_p  = (DWORD*)src->buf  + sy*src->sx  + sx;
		dest_p = (DWORD*)dest->buf + dy*dest->sx + dx;
	}

	if( r==0x80 && g==0x80 && b==0x80 ){	
		if(nuki == -3){
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					*(dest_p      ) = *src_p+*src_p + *(src_p-ssx) + *(src_p-1);
					*(dest_p    +1) = *src_p+*src_p + *(src_p-ssx) + *(src_p+1);
					*(dest_p+ddx  ) = *src_p+*src_p + *(src_p+ssx) + *(src_p-1);
					*(dest_p+ddx+1) = *src_p+*src_p + *(src_p+ssx) + *(src_p+1);
				}
			}
		}else{
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					*(dest_p+ddx+1) = *(dest_p+ddx) = *(dest_p+1) = *dest_p = *src_p;
				}
			}
		}
	}else{
		DWORD		*rtbl = BrightTable_TR[r];
		WORD		*gtbl = BrightTable_TG[g];
		char		*btbl = BrightTable_TB[b];

		if(nuki == -3){
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					*(dest_p      ) = *src_p+*src_p + *(src_p-ssx) + *(src_p-1);
					*(dest_p    +1) = *src_p+*src_p + *(src_p-ssx) + *(src_p+1);
					*(dest_p+ddx  ) = *src_p+*src_p + *(src_p+ssx) + *(src_p-1);
					*(dest_p+ddx+1) = *src_p+*src_p + *(src_p+ssx) + *(src_p+1);

					*(dest_p)       = rtbl[ ((RGB32*)dest_p)->r ]       + gtbl[ ((RGB32*)dest_p)->g ]       + btbl[ ((RGB32*)dest_p)->b];
					*(dest_p+1)     = rtbl[ ((RGB32*)dest_p+1)->r ]     + gtbl[ ((RGB32*)dest_p+1)->g ]     + btbl[ ((RGB32*)dest_p+1)->b];
					*(dest_p+ddx)   = rtbl[ ((RGB32*)dest_p+ddx)->r ]   + gtbl[ ((RGB32*)dest_p+ddx)->g ]   + btbl[ ((RGB32*)dest_p+ddx)->b];
					*(dest_p+ddx+1) = rtbl[ ((RGB32*)dest_p+ddx+1)->r ] + gtbl[ ((RGB32*)dest_p+ddx+1)->g ] + btbl[ ((RGB32*)dest_p+ddx+1)->b];
				}
			}
		}else{
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					*(dest_p+ddx+1) = *(dest_p+ddx) = *(dest_p+1) = *dest_p
						= rtbl[ ((RGB32*)src_p)->r ] + gtbl[ ((RGB32*)src_p)->g ] + btbl[ ((RGB32*)src_p)->b];
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_TT_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx,yy;
	char	*alph_tbl;

	if( sw*2==dw && sh*2==dh ){	
		if( nuki==-1 || nuki==-3 ){	
			return DRW_DrawZOOM2_TT_Std( dest, dobj->dx, dobj->dy, src, dobj->s1x, dobj->s1y, sw, sh, nuki, r, g, b );
		}
	}

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy = yyy;


	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					*dest_p = *sss;

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						if( sss->a == 255 ){	
							*dest_p = *sss;
						}else{					
							alph_tbl = BlendTable[ 255 - sss->a ];
							dest_p->b = alph_tbl[dest_p->b] + sss->b;
							dest_p->g = alph_tbl[dest_p->g] + sss->g;
							dest_p->r = alph_tbl[dest_p->r] + sss->r;
						}
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( *(long*)sss != nuki )
						*dest_p = *sss;

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];
		char	*rtbl2 = BrightTable_TB[r];
		char	*gtbl2 = BrightTable_TB[g];
		char	*alp2_tbl,*alp3_tbl;

		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					*(DWORD*)dest_p = rtbl[sss->r] | gtbl[sss->g] | btbl[sss->b];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						if( sss->a == 255 ){	
							*(DWORD*)dest_p = rtbl[sss->r] | gtbl[sss->g] | btbl[sss->b];
						}else{					
							alph_tbl = BlendTable[ 255 - sss->a ];
							alp2_tbl = BlendTable[ sss->a ];
							alp3_tbl = BlendTable2[ sss->a ];
							dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[ btbl[alp3_tbl[sss->b]]];
							dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl2[alp3_tbl[sss->g]]];
							dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl2[alp3_tbl[sss->r]]];
						}
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki )
						*(DWORD*)dest_p = rtbl[sss->r] | gtbl[sss->g] | btbl[sss->b];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawZOOM_TT_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		blnd = dobj->dnum;
	int		x,y;
	int		px,py,wx,wy,xx,yy;
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
	char	*btbl = BrightTable_TB[b];
	char	*blnd_tbl = BlendTable[blnd];
	char	*brev_tbl = BlendTable[256-blnd];
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;
	
	if( blnd==BLD_0P  )	return 1;															
	if( blnd==BLD_100P)	return DRW_DrawZOOM_TT_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy );	

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy=yyy;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			if(MMX_Flag && !dobj->rparam){
				DWORD	alpha1[2] = { (blnd) * 0x00010001, (blnd) * 0x00010001 };
				DWORD	alpha2[2] = { (256-blnd) * 0x00010001, (256-blnd) * 0x00010001 };
				int		dwa = dw%2;

				px = px*4;
				dw = dw/2;

				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					_asm {
						mov			ecx, dw		
						mov			esi, sss	
						mov			edi, dest_p	
						mov			eax, dw2
						mov			ebx, xxx
						mov			edx, px

						pxor		mm0, mm0	
						movq		mm1, alpha2	
						movq		mm2, alpha1	

						xloop_001:
							
							movd		mm3, [esi]	

							add			ebx, wx		
							cmp			ebx, eax	
							jb			next_001
								sub			ebx, eax	
								add			esi, 4		
							next_001:
							add			esi, edx	

							movd		mm4, [esi]	
							punpcklbw	mm3, mm0	
							punpcklbw	mm4, mm0	


							movq		mm5, [edi]	
							movq		mm6, mm5	

							punpcklbw	mm5, mm0	
							punpckhbw	mm6, mm0	
							

							pmullw		mm3, mm2	
							pmullw		mm4, mm2	
							psrlw		mm3, 8		
							psrlw		mm4, 8		
							
							pmullw		mm5, mm1	
							pmullw		mm6, mm1	
							psrlw		mm5, 8		
							psrlw		mm6, 8		

							
							packuswb	mm3, mm4	
							packuswb	mm5, mm6	
							
							paddusb		mm3, mm5	

							movq		[edi], mm3

							add			ebx, wx		
							cmp			ebx, eax	
							jb			next_002
								sub			ebx, eax	
								add			esi, 4		
							next_002:

							add			esi, edx	
							add			edi, 8
						loop xloop_001
						
						mov		sss,    esi
						mov		dest_p, edi
					}
					if(dwa){
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ sss->r ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ sss->g ];
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ sss->b ];
						dest_p++;
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
				}
				_asm emms
			}else{
				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ sss->r ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ sss->g ];
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ sss->b ];

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}else if(nuki==-2){		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( sss->a!=0 ){	
						if( sss->a==255 ){	
							dest_p->b = brev_tbl[dest_p->b] + blnd_tbl[ sss->b ];
							dest_p->g = brev_tbl[dest_p->g] + blnd_tbl[ sss->g ];
							dest_p->r = brev_tbl[dest_p->r] + blnd_tbl[ sss->r ];
						}else{						
							alph_tbl = BlendTable[ 255-blnd_tbl[sss->a] ];

							dest_p->b = alph_tbl[dest_p->b] + blnd_tbl[ sss->b ];
							dest_p->g = alph_tbl[dest_p->g] + blnd_tbl[ sss->g ];
							dest_p->r = alph_tbl[dest_p->r] + blnd_tbl[ sss->r ];
						}
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ sss->r ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ sss->g ];
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ sss->b ];
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}else{
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[sss->r] ];
					dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[sss->g] ];
					dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[sss->b] ];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else if(nuki==-2){		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( sss->a!=0 ){	
						if( sss->a==255 ){	
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[sss->b] ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[sss->g] ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[sss->r] ];
						}else{							
							alph_tbl = BlendTable[ 255-blnd_tbl[sss->a] ];
							alp2_tbl = BlendTable[ blnd_tbl[sss->a] ];
							alp3_tbl = BlendTable2[ sss->a ];

							dest_p->b = alph_tbl[ dest_p->b ] + alp2_tbl[ btbl[alp3_tbl[sss->b]] ];
							dest_p->g = alph_tbl[ dest_p->g ] + alp2_tbl[ gtbl[alp3_tbl[sss->g]] ];
							dest_p->r = alph_tbl[ dest_p->r ] + alp2_tbl[ rtbl[alp3_tbl[sss->r]] ];
						}
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[sss->r] ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[sss->g] ];
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[sss->b] ];
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_TT_Add( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=0;
	char	*add_tbl = AddTable;

	if( r==0 && g==0 && b==0 ){
		return 1;
	}
	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy=yyy;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					dest_p->r = add_tbl[ dest_p->r + sss->r ];
					dest_p->g = add_tbl[ dest_p->g + sss->g ];
					dest_p->b = add_tbl[ dest_p->b + sss->b ];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else if(nuki==-2){		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( sss->a!=0 ){
						dest_p->r = add_tbl[ dest_p->r + sss->r ];
						dest_p->g = add_tbl[ dest_p->g + sss->g ];
						dest_p->b = add_tbl[ dest_p->b + sss->b ];
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->r = add_tbl[ dest_p->r + sss->r ];
						dest_p->g = add_tbl[ dest_p->g + sss->g ];
						dest_p->b = add_tbl[ dest_p->b + sss->b ];
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}else{
		char		*rtbl = BrightTable_TB[r];
		char		*gtbl = BrightTable_TB[g];
		char		*btbl = BrightTable_TB[b];

		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					dest_p->r = add_tbl[ dest_p->r + rtbl[sss->r] ];
					dest_p->g = add_tbl[ dest_p->g + gtbl[sss->g] ];
					dest_p->b = add_tbl[ dest_p->b + btbl[sss->b] ];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else if( nuki==-2 ){		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( sss->a!=0 ){
						if( sss->a==256 ){
							dest_p->r = add_tbl[ dest_p->r + rtbl[sss->r] ];
							dest_p->g = add_tbl[ dest_p->g + gtbl[sss->g] ];
							dest_p->b = add_tbl[ dest_p->b + btbl[sss->b] ];
						}else{





						}
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->r = add_tbl[ dest_p->r + rtbl[sss->r] ];
						dest_p->g = add_tbl[ dest_p->g + gtbl[sss->g] ];
						dest_p->b = add_tbl[ dest_p->b + btbl[sss->b] ];
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_TT_Sub( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=0;
	char	*sub_tbl = SubTable+255;

	if( r==0 && g==0 && b==0 ){
		return 1;
	}

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy=yyy;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					dest_p->r = sub_tbl[ dest_p->r - sss->r];
					dest_p->g = sub_tbl[ dest_p->g - sss->g];
					dest_p->b = sub_tbl[ dest_p->b - sss->b];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->r = sub_tbl[ dest_p->r - sss->r];
						dest_p->g = sub_tbl[ dest_p->g - sss->g];
						dest_p->b = sub_tbl[ dest_p->b - sss->b];
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					dest_p->r = sub_tbl[ dest_p->r - rtbl[sss->r] ];
					dest_p->g = sub_tbl[ dest_p->g - gtbl[sss->g] ];
					dest_p->b = sub_tbl[ dest_p->b - btbl[sss->b] ];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->r = sub_tbl[ dest_p->r - rtbl[sss->r] ];
						dest_p->g = sub_tbl[ dest_p->g - gtbl[sss->g] ];
						dest_p->b = sub_tbl[ dest_p->b - btbl[sss->b] ];
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawZOOM_TT_Mul( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=0;

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy=yyy;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					dest_p->r = BrightTable_TB[ sss->r ][ dest_p->r ];
					dest_p->g = BrightTable_TB[ sss->g ][ dest_p->g ];
					dest_p->b = BrightTable_TB[ sss->b ][ dest_p->b ];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->r = BrightTable_TB[ sss->r ][ dest_p->r ];
						dest_p->g = BrightTable_TB[ sss->g ][ dest_p->g ];
						dest_p->b = BrightTable_TB[ sss->b ][ dest_p->b ];
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					dest_p->r = BrightTable_TB[ rtbl[sss->r] ][ dest_p->r ];
					dest_p->g = BrightTable_TB[ gtbl[sss->g] ][ dest_p->g ];
					dest_p->b = BrightTable_TB[ btbl[sss->b] ][ dest_p->b ];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->r = BrightTable_TB[ rtbl[sss->r] ][ dest_p->r ];
						dest_p->g = BrightTable_TB[ gtbl[sss->g] ][ dest_p->g ];
						dest_p->b = BrightTable_TB[ btbl[sss->b] ][ dest_p->b ];
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_TT_Ami( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=0;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	int		mesh = dobj->dnum;
	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=0   )	return 1;															
	if( mesh>=256 )	return DRW_DrawZOOM_TT_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy );	

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy=yyy;


	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] )
						*dest_p = *sss;

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] && *(long*)sss != nuki )
						*dest_p = *sss;

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}else{
		DWORD	*rtbl = BrightTable_TR[r];
		WORD	*gtbl = BrightTable_TG[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] )
						*(DWORD*)dest_p = rtbl[sss->r] | gtbl[sss->g] | btbl[sss->b];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] && *(long*)sss != nuki )
						*(DWORD*)dest_p = rtbl[sss->r] | gtbl[sss->g] | btbl[sss->b];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawZOOM_TT_Neg( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=0;

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy=yyy;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					*(DWORD*)dest_p = ~(*(DWORD*)sss);

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						*(DWORD*)dest_p = ~(*(DWORD*)sss);
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					dest_p->b = ~btbl[ sss->b ];
					dest_p->g = ~gtbl[ sss->g ];
					dest_p->r = ~rtbl[ sss->r ];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->b = ~btbl[ sss->b ];
						dest_p->g = ~gtbl[ sss->g ];
						dest_p->r = ~rtbl[ sss->r ];
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawZOOM_TT_Viv( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	BMP_T	*src = (BMP_T*)dobj->src1;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		viv = dobj->dnum, glay;
	char	*viv_tbl=BlendTable[viv];
	char	*riv_tbl=BlendTable[256-viv];
	int		x,y;
	int		px,py,wx,wy,xx,yy;

	if( viv==256 ) { return DRW_DrawZOOM_TT_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy ); }	

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy=yyy;

	if( viv==0 ){
		if( r==0x80 && g==0x80 && b==0x80 ){
			if( nuki==-1 ){
				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}else{		
				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						if( *(long*)sss != nuki ){
							dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
						}
						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			if( nuki==-1 ){
				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ){
						glay = riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
						dest_p->b = btbl[ glay ];
						dest_p->g = gtbl[ glay ];
						dest_p->r = rtbl[ glay ];

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}else{		
				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						if( *(long*)sss != nuki ){
							glay = riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
							dest_p->b = btbl[ glay ];
							dest_p->g = gtbl[ glay ];
							dest_p->r = rtbl[ glay ];
						}

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}
	}else{
		if( r==0x80 && g==0x80 && b==0x80 ){
			if( nuki==-1 ){
				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						glay = riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
						dest_p->b = glay + viv_tbl[sss->b];
						dest_p->g = glay + viv_tbl[sss->g];
						dest_p->r = glay + viv_tbl[sss->r];

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}else{		
				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						if( *(long*)sss != nuki ){
							glay = riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
							dest_p->b = glay + viv_tbl[sss->b];
							dest_p->g = glay + viv_tbl[sss->g];
							dest_p->r = glay + viv_tbl[sss->r];
						}
						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			if( nuki==-1 ){
				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ){
						glay = riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
						dest_p->b = btbl[ glay + viv_tbl[sss->b] ];
						dest_p->g = gtbl[ glay + viv_tbl[sss->g] ];
						dest_p->r = rtbl[ glay + viv_tbl[sss->r] ];

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}else{		
				for( y=0; y<dh; y++, dest_p+=drev ) {
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						if( *(long*)sss != nuki ){
							glay = riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
							dest_p->b = btbl[ glay + viv_tbl[sss->b] ];
							dest_p->g = gtbl[ glay + viv_tbl[sss->g] ];
							dest_p->r = rtbl[ glay + viv_tbl[sss->r] ];
						}

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}
	}
	return TRUE;
}






BOOL DRW_DrawZOOM_TT( DRAW_OBJECT dobj )
{
	BOOL	ret=TRUE;
	BMP_T	*dest = (BMP_T*)dobj.dest;
	BMP_T	*src  = (BMP_T*)dobj.src1;
	int		xx, yy, dw2=dobj.dw, dh2=dobj.dh, sw2=dobj.s1w, sh2=dobj.s1h;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_TT]\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_TT]\n"); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRectZ( &dobj.s1x, &dobj.s1y, &dobj.s1w, &dobj.s1h,
						&dobj.dx,  &dobj.dy,  &dobj.dw,  &dobj.dh, &xx, &yy, dest->sx, dest->sy, src->sx, src->sy, dobj.clip ) )	return 1;
	}
	
	if( dobj.dparam==DRW_AMI2 || dobj.dparam==DRW_BLD2 ){
		if( dobj.dnum==BLD_0P   ) return 1;	
		if( dobj.dnum==BLD_100P ){
			dobj.dparam = DRW_NML;	
		}
	}
	switch(dobj.dparam){
		default:
		case DRW_NML:	DRW_DrawZOOM_TT_Std( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_BLD2:	DRW_DrawZOOM_TT_Bld( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_ADD:	DRW_DrawZOOM_TT_Add( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_SUB:	DRW_DrawZOOM_TT_Sub( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_MUL:	DRW_DrawZOOM_TT_Mul( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_AMI2:	DRW_DrawZOOM_TT_Ami( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_NEG:	DRW_DrawZOOM_TT_Neg( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_VIV2:	DRW_DrawZOOM_TT_Viv( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
	}
	return TRUE;
}









BOOL DRW_DrawZOOM_TB_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	RGB32	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				*dest_p = spal[ *sss ];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki )	*dest_p = spal[ *sss ];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}

	return TRUE;
}



BOOL DRW_DrawZOOM_TB_Bld128( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	RGB32	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;
	char	*blend_tbl = BlendTable[128];

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				*(DWORD*)dest_p = ((*((DWORD*)dest_p)>>1)&0x7f7f7f7f) + *(DWORD*)&spal[*sss];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki ){
					*(DWORD*)dest_p = ((*((DWORD*)dest_p)>>1)&0x7f7f7f7f) + *(DWORD*)&spal[*sss];
				}

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_TB_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	RGB32	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;
	int		blnd=dobj->dnum;
	char	*blend_tbl = BlendTable[256-blnd];

	if( blnd==BLD_0P  )	return 1;																	
	if( blnd==BLD_50P )	return DRW_DrawZOOM_TB_Bld128( dobj, dw2, dh2, sw2, sh2, xxx, yyy, src );	
	if( blnd==BLD_100P)	return DRW_DrawZOOM_TB_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy, src );		

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				dest_p->r = blend_tbl[dest_p->r] + spal[ *sss ].r;
				dest_p->g = blend_tbl[dest_p->g] + spal[ *sss ].g;
				dest_p->b = blend_tbl[dest_p->b] + spal[ *sss ].b;

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki ){
					dest_p->r = blend_tbl[dest_p->r] + spal[ *sss ].r;
					dest_p->g = blend_tbl[dest_p->g] + spal[ *sss ].g;
					dest_p->b = blend_tbl[dest_p->b] + spal[ *sss ].b;
				}
				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}

	return TRUE;
}



BOOL DRW_DrawZOOM_TB_Add( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	RGB32	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;
	char	*blend_tbl = AddTable;

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				dest_p->r = blend_tbl[dest_p->r + spal[ *sss ].r];
				dest_p->g = blend_tbl[dest_p->g + spal[ *sss ].g];
				dest_p->b = blend_tbl[dest_p->b + spal[ *sss ].b];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki ){
					dest_p->r = blend_tbl[dest_p->r + spal[ *sss ].r];
					dest_p->g = blend_tbl[dest_p->g + spal[ *sss ].g];
					dest_p->b = blend_tbl[dest_p->b + spal[ *sss ].b];
				}

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}

	return TRUE;
}




BOOL DRW_DrawZOOM_TB_Sub( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	RGB32	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;
	char	*blend_tbl = SubTable+255;

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				dest_p->r = blend_tbl[dest_p->r - spal[ *sss ].r];
				dest_p->g = blend_tbl[dest_p->g - spal[ *sss ].g];
				dest_p->b = blend_tbl[dest_p->b - spal[ *sss ].b];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki ){
					dest_p->r = blend_tbl[dest_p->r - spal[ *sss ].r];
					dest_p->g = blend_tbl[dest_p->g - spal[ *sss ].g];
					dest_p->b = blend_tbl[dest_p->b - spal[ *sss ].b];
				}

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}

	return TRUE;
}



BOOL DRW_DrawZOOM_TB_Mul( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	RGB32	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				dest_p->r = BrightTable_TB[ spal[*sss].r ][ dest_p->r ];
				dest_p->g = BrightTable_TB[ spal[*sss].g ][ dest_p->g ];
				dest_p->b = BrightTable_TB[ spal[*sss].b ][ dest_p->b ];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki ){
					dest_p->r = BrightTable_TB[ spal[*sss].r ][ dest_p->r ];
					dest_p->g = BrightTable_TB[ spal[*sss].g ][ dest_p->g ];
					dest_p->b = BrightTable_TB[ spal[*sss].b ][ dest_p->b ];
				}
				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}

	return TRUE;
}



BOOL DRW_DrawZOOM_TB_Ami( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_T	*dest = (BMP_T*)dobj->dest;
	int		drev = dest->sx - dw;
	RGB32	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	RGB32	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;
	int		mesh=dobj->dnum;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=BLD_0P )	return 1;																
	if( mesh>=BLD_100P)	return DRW_DrawZOOM_TB_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy, src );	

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			linemask2 = linemask[y & 3];
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( linemask2[x & 3] )*dest_p = spal[ *sss ];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			linemask2 = linemask[y & 3];
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki ){
					if( linemask2[x & 3] )*dest_p = spal[ *sss ];
				}
				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}

	return TRUE;
}






BOOL DRW_DrawZOOM_TB( DRAW_OBJECT dobj )
{
	BOOL	ret=TRUE;
	BMP_T	*dest = (BMP_T*)dobj.dest;
	BMP_BT	*src  = (BMP_BT*)dobj.src1;
	int		xx, yy, dw2=dobj.dw, dh2=dobj.dh, sw2=dobj.s1w, sh2=dobj.s1h;
	RGB32	pal[256];
	BMP_BT	src2={ src->buf, src->alp, pal, src->sx, src->sy };

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_TB]\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_TB]\n"); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRectZ( &dobj.s1x, &dobj.s1y, &dobj.s1w, &dobj.s1h,
						&dobj.dx,  &dobj.dy,  &dobj.dw,  &dobj.dh, &xx, &yy, dest->sx, dest->sy, src->sx, src->sy, dobj.clip ) )	return 1;
	}
	
	if( dobj.dparam==DRW_AMI2 || dobj.dparam==DRW_BLD2 ){
		if( dobj.dnum==BLD_0P   ) return 1;	
		if( dobj.dnum==BLD_100P ){
			dobj.dparam = DRW_NML;	
		}
	}

	if( dobj.r==128 && dobj.g==128 && dobj.b==128 &&
		!(dobj.dparam==DRW_BLD2 || dobj.dparam==DRW_NEG || dobj.dparam==DRW_SBL2 || dobj.dparam==DRW_VIV2) ){
		src2.pal = src->pal;
	}else{
		CopyMemory( pal, src->pal, 256*4 );
		if( dobj.dparam==DRW_VIV2 ){
			BMP_ChangePalFine_Full( pal, 256-dobj.dnum );
			BMP_ChangePalBright_Full( pal, dobj.r, dobj.g, dobj.b );
		}else{						
			BMP_ChangePalBright_Full( pal, dobj.r, dobj.g, dobj.b );
			switch(dobj.dparam){
				case DRW_SBL2:	BMP_ChangePalBright_Full( pal, 128/2, 128/2, 128/2 );					break;
				case DRW_BLD2:	BMP_ChangePalBright_Full( pal, dobj.dnum/2, dobj.dnum/2, dobj.dnum/2 );	break;
				case DRW_NEG:	BMP_ChangePalNega_Full( pal );											break;	
			}
		}
	}

	switch(dobj.dparam){
		default:
		case DRW_NEG:	
		case DRW_VIV2:	
		case DRW_NML:	DRW_DrawZOOM_TB_Std( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_BLD2:	DRW_DrawZOOM_TB_Bld( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_ADD:	DRW_DrawZOOM_TB_Add( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_SUB:	DRW_DrawZOOM_TB_Sub( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_MUL:	DRW_DrawZOOM_TB_Mul( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_AMI2:	DRW_DrawZOOM_TB_Ami( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
	}
	return TRUE;
}














BOOL DRW_DrawXLine_TT_Std(	RGB32 *dest_p, BMP_T *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB32 *src_p = src->buf + sy*src->sx + sx;
	DWORD	*rtbl = BrightTable_TR[r];
	WORD	*gtbl = BrightTable_TG[g];
	char	*btbl = BrightTable_TB[b];

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				*dest_p = *src_p;

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					*dest_p = *src_p;
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}else{
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				*(DWORD*)dest_p = rtbl[src_p->r] | gtbl[src_p->g] | btbl[src_p->b];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					*(DWORD*)dest_p = rtbl[src_p->r] | gtbl[src_p->g] | btbl[src_p->b];
				}

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}

	return TRUE;
}



BOOL DRW_DrawXLine_TT_Bld(	RGB32 *dest_p, BMP_T *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b, int blnd )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB32 *src_p = src->buf + sy*src->sx + sx;
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
	char	*btbl = BrightTable_TB[b];
	char	*blnd_tbl = BlendTable[blnd];
	char	*brev_tbl = BlendTable[256-blnd];

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
				dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
				dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
					dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
					dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}else{
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
				dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
				dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
					dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
					dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}

	return TRUE;
}



BOOL DRW_DrawXLine_TT_Add(	RGB32 *dest_p, BMP_T *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB32 *src_p = src->buf + sy*src->sx + sx;
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
	char	*btbl = BrightTable_TB[b];
	char	*add_tbl = AddTable;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( MMX_Flag){
			if( nuki==-1 ){
				for( x=0; x < w ; x++, dest_p++ ){
					_asm {
						mov			edi, dest_p
						mov			esi, src_p

						movd		mm0, [edi]
						movd		mm1, [esi]
						paddusb		mm0, mm1
						movd		[edi], mm0
					}

					src_p += psy;
					ssx+=dsx;		ssy+=dsy;
					if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
					if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
				}
			}else{
				for( x=0; x < w ; x++, dest_p++ ){
					if( nuki!=*(long*)src_p ){
						_asm {
							mov			edi, dest_p
							mov			esi, src_p

							movd		mm0, [edi]
							movd		mm1, [esi]
							paddusb		mm0, mm1
							movd		[edi], mm0
						}
					}

					src_p += psy;
					ssx+=dsx;		ssy+=dsy;
					if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
					if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
				}
			}
			_asm	emms
		}else{
			if( nuki==-1 ){
				for( x=0; x < w ; x++, dest_p++ ){
					dest_p->b = add_tbl[ dest_p->b + src_p->b ];
					dest_p->g = add_tbl[ dest_p->g + src_p->g ];
					dest_p->r = add_tbl[ dest_p->r + src_p->r ];

					src_p += psy;
					ssx+=dsx;		ssy+=dsy;
					if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
					if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
				}
			}else{
				for( x=0; x < w ; x++, dest_p++ ){
					if( nuki!=*(long*)src_p ){
						dest_p->b = add_tbl[ dest_p->b + src_p->b ];
						dest_p->g = add_tbl[ dest_p->g + src_p->g ];
						dest_p->r = add_tbl[ dest_p->r + src_p->r ];
					}
					src_p += psy;
					ssx+=dsx;		ssy+=dsy;
					if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
					if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
				}
			}
		}
	}else{
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				dest_p->b = add_tbl[ dest_p->b + btbl[src_p->b] ];
				dest_p->g = add_tbl[ dest_p->g + gtbl[src_p->g] ];
				dest_p->r = add_tbl[ dest_p->r + rtbl[src_p->r] ];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					dest_p->b = add_tbl[ dest_p->b + btbl[src_p->b] ];
					dest_p->g = add_tbl[ dest_p->g + gtbl[src_p->g] ];
					dest_p->r = add_tbl[ dest_p->r + rtbl[src_p->r] ];
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawXLine_TT_Sub(	RGB32 *dest_p, BMP_T *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB32 *src_p = src->buf + sy*src->sx + sx;
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
	char	*btbl = BrightTable_TB[b];
	char	*sub_tbl = SubTable+255;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( MMX_Flag){
			if( nuki==-1 ){
				for( x=0; x < w ; x++, dest_p++ ){
					_asm {
						mov			edi, dest_p
						mov			esi, src_p

						movd		mm0, [edi]
						movd		mm1, [esi]
						psubusb		mm0, mm1
						movd		[edi], mm0
					}

					src_p += psy;
					ssx+=dsx;		ssy+=dsy;
					if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
					if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
				}
			}else{
				for( x=0; x < w ; x++, dest_p++ ){
					if( nuki!=*(long*)src_p ){
						_asm {
							mov			edi, dest_p
							mov			esi, src_p

							movd		mm0, [edi]
							movd		mm1, [esi]
							psubusb		mm0, mm1
							movd		[edi], mm0
						}
					}

					src_p += psy;
					ssx+=dsx;		ssy+=dsy;
					if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
					if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
				}
			}
			_asm	emms
		}else{
			if( nuki==-1 ){
				for( x=0; x < w ; x++, dest_p++ ){
					dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
					dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
					dest_p->r = sub_tbl[ dest_p->r - src_p->r ];

					src_p += psy;
					ssx+=dsx;		ssy+=dsy;
					if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
					if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
				}
			}else{
				for( x=0; x < w ; x++, dest_p++ ){
					if( nuki!=*(long*)src_p ){
						dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
						dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
						dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
					}
					src_p += psy;
					ssx+=dsx;		ssy+=dsy;
					if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
					if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
				}
			}
		}
	}else{
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				dest_p->b = sub_tbl[ dest_p->b - btbl[src_p->b] ];
				dest_p->g = sub_tbl[ dest_p->g - gtbl[src_p->g] ];
				dest_p->r = sub_tbl[ dest_p->r - rtbl[src_p->r] ];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					dest_p->b = sub_tbl[ dest_p->b - btbl[src_p->b] ];
					dest_p->g = sub_tbl[ dest_p->g - gtbl[src_p->g] ];
					dest_p->r = sub_tbl[ dest_p->r - rtbl[src_p->r] ];
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawXLine_TT_Mul(	RGB32 *dest_p, BMP_T *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB32 *src_p = src->buf + sy*src->sx + sx;
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
	char	*btbl = BrightTable_TB[b];

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				dest_p->b = BrightTable_TB[ src_p->b ][ dest_p->b ];
				dest_p->g = BrightTable_TB[ src_p->g ][ dest_p->g ];
				dest_p->r = BrightTable_TB[ src_p->r ][ dest_p->r ];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					dest_p->b = BrightTable_TB[ src_p->b ][ dest_p->b ];
					dest_p->g = BrightTable_TB[ src_p->g ][ dest_p->g ];
					dest_p->r = BrightTable_TB[ src_p->r ][ dest_p->r ];
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}else{
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				dest_p->b = BrightTable_TB[ src_p->b ][ btbl[dest_p->b] ];
				dest_p->g = BrightTable_TB[ src_p->g ][ gtbl[dest_p->g] ];
				dest_p->r = BrightTable_TB[ src_p->r ][ rtbl[dest_p->r] ];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					dest_p->b = BrightTable_TB[ src_p->b ][ btbl[dest_p->b] ];
					dest_p->g = BrightTable_TB[ src_p->g ][ gtbl[dest_p->g] ];
					dest_p->r = BrightTable_TB[ src_p->r ][ rtbl[dest_p->r] ];
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}

	return TRUE;
}



BOOL DRW_DrawXLine_TT_Ami(	RGB32 *dest_p, BMP_T *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b, int mesh,int dx,int dy )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB32 *src_p = src->buf + sy*src->sx + sx;
	DWORD	*rtbl = BrightTable_TR[r];
	WORD	*gtbl = BrightTable_TG[g];
	char	*btbl = BrightTable_TB[b];
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	mesh_pt = MeshFadePatern[mesh/16];

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	linemask2 = linemask[dy & 3];

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				if( linemask2[(dx+x) & 3] )	*dest_p = *src_p;

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					if( linemask2[(dx+x) & 3] )	*dest_p = *src_p;
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}else{
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				if( linemask2[(dx+x) & 3] )
					*(DWORD*)dest_p = rtbl[src_p->r] | gtbl[src_p->g] | btbl[src_p->b];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					if( linemask2[(dx+x) & 3] )
						*(DWORD*)dest_p = rtbl[src_p->r] | gtbl[src_p->g] | btbl[src_p->b];
				}

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}

	return TRUE;
}




BOOL DRW_DrawXLine_TT( RGB32 *dest_p,int dy, BMP_T *src, MIN_MAX *mi, MIN_MAX *ma, int nuki, RECT *clip,
					   int r, int g, int b, int param, int blnd )
{
	BOOL	ret = FALSE;
	int		dx1, dx2, w;
	int		xx, ww = ma->x - mi->x;
	int		sx,  sy,  sx2, sy2;
	int		dsx, dsy, fsx, fsy;

	if( mi->x == DispWidth )	return 1;
	if( ww==0 )			return 1;

	dx1 = max( mi->x, clip->left  );
	dx2 = min( ma->x, clip->right );

	w  = dx2-dx1;
	xx = dx1 - mi->x;
	dest_p += dx1;

	sx = ma->sx - mi->sx;
	sy = ma->sy - mi->sy;

	if(sx<0) { dsx = (-sx)%ww; fsx=-1; sx2 = sx*xx/ww + mi->sx-1; }
	else	 { dsx =   sx %ww; fsx= 1; sx2 = sx*xx/ww + mi->sx; }
	if(sy<0) { dsy = (-sy)%ww; fsy=-1; sy2 = sy*xx/ww + mi->sy-1; }
	else	 { dsy =   sy %ww; fsy= 1; sy2 = sy*xx/ww + mi->sy; }

	switch( param ){
		default:
		case DRW_NML:	DRW_DrawXLine_TT_Std( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b );				break;	
		case DRW_BLD2:	DRW_DrawXLine_TT_Bld( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b, blnd );		break;	
		case DRW_ADD:	DRW_DrawXLine_TT_Add( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b );				break;	
		case DRW_SUB:	DRW_DrawXLine_TT_Sub( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b );				break;	
		case DRW_MUL:	DRW_DrawXLine_TT_Mul( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b );				break;	
		case DRW_AMI2:	DRW_DrawXLine_TT_Ami( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b, blnd,dx1,dy );	break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}









BOOL DRW_DrawPOLY4_TT( DRAW_OBJECT dobj )
{
	BOOL		ret=TRUE;
	BMP_T		*dest = (BMP_T*)dobj.dest;
	BMP_T		*src  = (BMP_T*)dobj.src1;
	RGB32		*dest_p = dest->buf;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			y;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( dobj.clip==NULL ) {	
		dobj.clip = &ClipRectDef;
	}else if( dobj.clip==(void*)-1 ){
		dobj.clip = &ClipRectDef;
	}else{
		dobj.clip = &ClipRectDef;
	}

	for( y=0; y<DispHeight; y++ )	{	mi[y].x = DispWidth;	ma[y].x = 0;	}	
	DrawMinMaxTableSrc( dobj.dx,  dobj.dy,  dobj.s1x,  dobj.s1y,		dobj.dx2, dobj.dy2, dobj.s1x2, dobj.s1y2, dobj.clip, mi, ma );	
	DrawMinMaxTableSrc( dobj.dx2, dobj.dy2, dobj.s1x2, dobj.s1y2,		dobj.dx4, dobj.dy4, dobj.s1x4, dobj.s1y4, dobj.clip, mi, ma );	
	DrawMinMaxTableSrc( dobj.dx4, dobj.dy4, dobj.s1x4, dobj.s1y4,		dobj.dx3, dobj.dy3, dobj.s1x3, dobj.s1y3, dobj.clip, mi, ma );	
	DrawMinMaxTableSrc( dobj.dx3, dobj.dy3, dobj.s1x3, dobj.s1y3,		dobj.dx,  dobj.dy,  dobj.s1x,  dobj.s1y,  dobj.clip, mi, ma );	

	
	if( dobj.dparam==DRW_AMI2 || dobj.dparam==DRW_BLD2 ){
		if( dobj.dnum==BLD_0P   ) goto end;	
		if( dobj.dnum==BLD_100P ){
			dobj.dparam = DRW_NML;	
		}
	}

	for( y = dobj.clip->top ; y < dobj.clip->bottom ; y++, dest_p+=dest->sx )
		DRW_DrawXLine_TT( dest_p, y, src, &mi[y], &ma[y], dobj.nuki, dobj.clip, dobj.r, dobj.g, dobj.b, dobj.dparam, dobj.dnum );

end:
	GFree(mi);
	GFree(ma);

	return ret;
}









BOOL DRW_DrawXLine_TB_Std(	RGB32 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32 *spal = src->pal;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( nuki==-1 ){
		for( x=0; x < w ; x++, dest_p++ ){
			*dest_p = spal[*src_p];

			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}else{
		for( x=0; x < w ; x++, dest_p++ ){
			if( nuki != *src_p ){
				*dest_p = spal[*src_p];
			}
			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}

	return TRUE;
}



BOOL DRW_DrawXLine_TB_Bld(	RGB32 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int blnd )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32 *spal = src->pal;
	char	*bld_tbl = BlendTable[256-blnd];

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( nuki==-1 ){
		for( x=0; x < w ; x++, dest_p++ ){
			dest_p->b = bld_tbl[ dest_p->b ] + spal[*src_p].b;
			dest_p->g = bld_tbl[ dest_p->g ] + spal[*src_p].g;
			dest_p->r = bld_tbl[ dest_p->r ] + spal[*src_p].r;

			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}else{
		for( x=0; x < w ; x++, dest_p++ ){
			if( nuki!=*(long*)src_p ){
				dest_p->b = bld_tbl[ dest_p->b ] + spal[*src_p].b;
				dest_p->g = bld_tbl[ dest_p->g ] + spal[*src_p].g;
				dest_p->r = bld_tbl[ dest_p->r ] + spal[*src_p].r;
			}
			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}
	return TRUE;
}



BOOL DRW_DrawXLine_TB_Add(	RGB32 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32 *spal = src->pal;
	DWORD	wpal;
	char	*add_tbl = AddTable;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( MMX_Flag){
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				wpal = *((DWORD*)spal + *src_p);
				_asm {
					mov			edi, dest_p

					movd		mm0, [edi]
					movd		mm1, wpal
					paddusb		mm0, mm1
					movd		[edi], mm0
				}

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					wpal = *((DWORD*)spal + *src_p);
					_asm {
						mov			edi, dest_p
					
						movd		mm0, [edi]
						movd		mm1, wpal
						paddusb		mm0, mm1
						movd		[edi], mm0
					}
				}

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
		_asm	emms
	}else{
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				dest_p->b = add_tbl[ dest_p->b + spal[*src_p].b  ];
				dest_p->g = add_tbl[ dest_p->g + spal[*src_p].g ];
				dest_p->r = add_tbl[ dest_p->r + spal[*src_p].r   ];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					dest_p->b = add_tbl[ dest_p->b + spal[*src_p].b  ];
					dest_p->g = add_tbl[ dest_p->g + spal[*src_p].g ];
					dest_p->r = add_tbl[ dest_p->r + spal[*src_p].r   ];
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawXLine_TB_Sub(	RGB32 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32 *spal = src->pal;
	DWORD	wpal;
	char	*sub_tbl = SubTable+255;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( MMX_Flag){
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				wpal = *((DWORD*)spal + *src_p);
				_asm {
					mov			edi, dest_p

					movd		mm0, [edi]
					movd		mm1, wpal
					psubusb		mm0, mm1
					movd		[edi], mm0
				}

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					wpal = *((DWORD*)spal + *src_p);
					_asm {
						mov			edi, dest_p
					
						movd		mm0, [edi]
						movd		mm1, wpal
						psubusb		mm0, mm1
						movd		[edi], mm0
					}
				}

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
		_asm	emms
	}else{
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				dest_p->b = sub_tbl[ dest_p->b - spal[*src_p].b ];
				dest_p->g = sub_tbl[ dest_p->g - spal[*src_p].g ];
				dest_p->r = sub_tbl[ dest_p->r - spal[*src_p].r ];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					dest_p->b = sub_tbl[ dest_p->b - spal[*src_p].b ];
					dest_p->g = sub_tbl[ dest_p->g - spal[*src_p].g ];
					dest_p->r = sub_tbl[ dest_p->r - spal[*src_p].r ];
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawXLine_TB_Mul(	RGB32 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32 *spal = src->pal;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( nuki==-1 ){
		for( x=0; x < w ; x++, dest_p++ ){
			dest_p->b = BrightTable_TB[ spal[*src_p].b][dest_p->b];
			dest_p->g = BrightTable_TB[ spal[*src_p].g][dest_p->g];
			dest_p->r = BrightTable_TB[ spal[*src_p].r][dest_p->r];

			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}else{
		for( x=0; x < w ; x++, dest_p++ ){
			if( nuki!=*(long*)src_p ){
				dest_p->b = BrightTable_TB[ spal[*src_p].b][dest_p->b];
				dest_p->g = BrightTable_TB[ spal[*src_p].g][dest_p->g];
				dest_p->r = BrightTable_TB[ spal[*src_p].r][dest_p->r];
			}
			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}
	return TRUE;
}



BOOL DRW_DrawXLine_TB_Ami(	RGB32 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int mesh,int dx,int dy )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32 *spal = src->pal;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	mesh_pt = MeshFadePatern[mesh/16];

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	linemask2 = linemask[dy & 3];

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( nuki==-1 ){
		for( x=0; x < w ; x++, dest_p++ ){
			if( linemask2[(dx+x) & 3] )	*dest_p = spal[*src_p];

			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}else{
		for( x=0; x < w ; x++, dest_p++ ){
			if( nuki!=*(long*)src_p ){
				if( linemask2[(dx+x) & 3] )	*dest_p = spal[*src_p];
			}
			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}

	return TRUE;
}




BOOL DRW_DrawXLine_TB( RGB32 *dest_p,int dy, BMP_BT *src, MIN_MAX *mi, MIN_MAX *ma, int nuki, RECT *clip, int param, int blnd )
{
	BOOL	ret = FALSE;
	int		dx1, dx2, w;
	int		xx, ww = ma->x - mi->x;
	int		sx,  sy,  sx2, sy2;
	int		dsx, dsy, fsx, fsy;

	if( mi->x == DispWidth )	return 1;
	if( ww==0 )			return 1;

	dx1 = max( mi->x, clip->left  );
	dx2 = min( ma->x, clip->right );

	w  = dx2-dx1;
	xx = dx1 - mi->x;
	dest_p += dx1;

	sx = ma->sx - mi->sx;
	sy = ma->sy - mi->sy;

	if(sx<0) { dsx = (-sx)%ww; fsx=-1; sx2 = sx*xx/ww + mi->sx-1; }
	else	 { dsx =   sx %ww; fsx= 1; sx2 = sx*xx/ww + mi->sx;   }
	if(sy<0) { dsy = (-sy)%ww; fsy=-1; sy2 = sy*xx/ww + mi->sy-1; }
	else	 { dsy =   sy %ww; fsy= 1; sy2 = sy*xx/ww + mi->sy;   }

	switch( param ){
		default:
		case DRW_NML:	DRW_DrawXLine_TB_Std( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );				break;	
		case DRW_BLD2:	DRW_DrawXLine_TB_Bld( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, blnd );			break;	
		case DRW_ADD:	DRW_DrawXLine_TB_Add( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );				break;	
		case DRW_SUB:	DRW_DrawXLine_TB_Sub( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );				break;	
		case DRW_MUL:	DRW_DrawXLine_TB_Mul( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );				break;	
		case DRW_AMI2:	DRW_DrawXLine_TB_Ami( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, blnd,dx1,dy );	break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}






BOOL DRW_DrawPOLY4_TB( DRAW_OBJECT dobj )
{
	BOOL		ret=TRUE;
	BMP_T		*dest = (BMP_T*)dobj.dest;
	BMP_BT		*src  = (BMP_BT*)dobj.src1;
	RGB32		*dest_p = dest->buf;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			y;
	RGB32		pal[256];
	BMP_BT		src2={ src->buf, src->alp, pal, src->sx, src->sy };

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( dobj.clip==NULL ) {	
		dobj.clip = &ClipRectDef;
	}else if( dobj.clip==(void*)-1 ){
		dobj.clip = &ClipRectDef;
	}else{
		dobj.clip = &ClipRectDef;
	}

	for( y=0; y<DispHeight; y++ )	{	mi[y].x = DispWidth;	ma[y].x = 0;	}	
	DrawMinMaxTableSrc( dobj.dx,  dobj.dy,  dobj.s1x,  dobj.s1y,	dobj.dx2, dobj.dy2, dobj.s1x2, dobj.s1y2, dobj.clip, mi, ma );	
	DrawMinMaxTableSrc( dobj.dx2, dobj.dy2, dobj.s1x2, dobj.s1y2,	dobj.dx4, dobj.dy4, dobj.s1x4, dobj.s1y4, dobj.clip, mi, ma );	
	DrawMinMaxTableSrc( dobj.dx4, dobj.dy4, dobj.s1x4, dobj.s1y4,	dobj.dx3, dobj.dy3, dobj.s1x3, dobj.s1y3, dobj.clip, mi, ma );	
	DrawMinMaxTableSrc( dobj.dx3, dobj.dy3, dobj.s1x3, dobj.s1y3,	dobj.dx,  dobj.dy,  dobj.s1x,  dobj.s1y,  dobj.clip, mi, ma );	

	
	if( dobj.dparam==DRW_AMI2 || dobj.dparam==DRW_BLD2 ){
		if( dobj.dnum==BLD_0P   ) goto end;	
		if( dobj.dnum==BLD_100P ){
			dobj.dparam = DRW_NML;	
		}
	}

	if( dobj.r==128 && dobj.g==128 && dobj.b==128 &&
		!(dobj.dparam==DRW_BLD2 || dobj.dparam==DRW_NEG || dobj.dparam==DRW_SBL2 || dobj.dparam==DRW_VIV2) ){
		src2.pal = src->pal;
	}else{
		CopyMemory( pal, src->pal, 256*4 );
		if( dobj.dparam==DRW_VIV2 ){
			BMP_ChangePalFine_Full( pal, 256-dobj.dnum );
			BMP_ChangePalBright_Full( pal, dobj.r, dobj.g, dobj.b );
		}else{						
			BMP_ChangePalBright_Full( pal, dobj.r, dobj.g, dobj.b );
			switch(dobj.dparam){
				case DRW_SBL2:	BMP_ChangePalBright_Full( pal, 128/2, 128/2, 128/2 );					break;
				case DRW_BLD2:	BMP_ChangePalBright_Full( pal, dobj.dnum/2, dobj.dnum/2, dobj.dnum/2 );	break;
				case DRW_NEG:	BMP_ChangePalNega_Full( pal );											break;	
			}
		}
	}

	for( y = dobj.clip->top ; y < dobj.clip->bottom ; y++, dest_p+=dest->sx )
		DRW_DrawXLine_TB( dest_p, y, &src2, &mi[y], &ma[y], dobj.nuki, dobj.clip, dobj.dparam, dobj.dnum );

end:
	GFree(mi);
	GFree(ma);

	return ret;
}
