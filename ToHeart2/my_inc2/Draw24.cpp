
 
  
   
    
     
      
#include "mm_std.h"

#include "bmp.h"
#include "draw.h"
#include "drawprim.h"


#include <math.h>

#define INTERLESS_CHECK		((dobj->dy+y)%(interless+1)==InterlessLine||interless==0)
#define INTERLESS_CHECK2	((dobj.dy+y)%(interless+1)==InterlessLine||interless==0)

extern int	DispWidth;
extern int	DispHeight;
















BOOL DRW_DrawBMP_FTT_Bld( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src1 = (BMP_T *)dobj->src1;
	BMP_T	*src2 = (BMP_T *)dobj->src2;
	int		drev  = dest->sx - w;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src1_p = src1->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*src2_p = src2->buf + DRW_GetStartPointerSrc2( dobj );
	int		xinc = XINC(dobj);
	int		blnd = dobj->dnum2;
	int		brev = 256 - blnd;




	if(dobj->dparam!=DRW_BLD2){
		char	*blnd_tbl=BlendTable[blnd], *blnd_tbl2;
		char	*brev_tbl=BlendTable[brev], *brev_tbl2, *dest_tbl;

		if(dobj->r==128 && dobj->g==128 && dobj->b==128){
			if( dobj->nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if(INTERLESS_CHECK){
						for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
							dest_p->b = blnd_tbl[ src1_p->b ] + brev_tbl[ src2_p->b ];
							dest_p->g = blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
							dest_p->r = blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
						}
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}else if( dobj->nuki==-2 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if(INTERLESS_CHECK){
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
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}else if( dobj->nuki==-4 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if(INTERLESS_CHECK){
						for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
							if( src1_p->a > 128 ){
								if( src2_p->a > 128 ){	
									dest_p->b = brev_tbl[ src2_p->b ] + blnd_tbl[ src1_p->b ];
									dest_p->g = brev_tbl[ src2_p->g ] + blnd_tbl[ src1_p->g ];
									dest_p->r = brev_tbl[ src2_p->r ] + blnd_tbl[ src1_p->r ];
								}else{							
									dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ];
									dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ];
									dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ];
								}
							}else{
								if( src2_p->a > 128 ){	
									dest_p->b = blnd_tbl[ dest_p->b ] + brev_tbl[ src2_p->b ];
									dest_p->g = blnd_tbl[ dest_p->g ] + brev_tbl[ src2_p->g ];
									dest_p->r = blnd_tbl[ dest_p->r ] + brev_tbl[ src2_p->r ];
								}
							}
						}
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if(INTERLESS_CHECK){
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
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}
		}else{
			char	*rtbl1 = BrightTable_TB[dobj->r];
			char	*gtbl1 = BrightTable_TB[dobj->g];
			char	*btbl1 = BrightTable_TB[dobj->b];

			char	*rtbl2 = BrightTable_TB[dobj->r2];
			char	*gtbl2 = BrightTable_TB[dobj->g2];
			char	*btbl2 = BrightTable_TB[dobj->b2];

			if( dobj->nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if(INTERLESS_CHECK){
						for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
							dest_p->b = blnd_tbl[ btbl1[src1_p->b] ] + brev_tbl[ btbl2[src2_p->b] ];
							dest_p->g = blnd_tbl[ gtbl1[src1_p->g] ] + brev_tbl[ gtbl2[src2_p->g] ];
							dest_p->r = blnd_tbl[ rtbl1[src1_p->r] ] + brev_tbl[ rtbl2[src2_p->r] ];
						}
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}else if( dobj->nuki==-2 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if(INTERLESS_CHECK){
						for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
							if( src1_p->a!=0 ){
								if( src2_p->a!=0 ){	
									if( src1_p->a==255 ){
										if( src2_p->a==255 ){
											dest_p->b = brev_tbl[ btbl2[src2_p->b] ] + blnd_tbl[ btbl1[src1_p->b] ];
											dest_p->g = brev_tbl[ gtbl2[src2_p->g] ] + blnd_tbl[ gtbl1[src1_p->g] ];
											dest_p->r = brev_tbl[ rtbl2[src2_p->r] ] + blnd_tbl[ rtbl1[src1_p->r] ];
										}else{
											blnd_tbl2 = BlendTable[ brev - brev_tbl[ src2_p->a ] ];
											dest_p->b = blnd_tbl2[ dest_p->b ] + brev_tbl[ btbl2[src2_p->b] ] + blnd_tbl[ btbl1[src1_p->b] ];
											dest_p->g = blnd_tbl2[ dest_p->g ] + brev_tbl[ gtbl2[src2_p->g] ] + blnd_tbl[ gtbl1[src1_p->g] ];
											dest_p->r = blnd_tbl2[ dest_p->r ] + brev_tbl[ rtbl2[src2_p->r] ] + blnd_tbl[ rtbl1[src1_p->r] ];
										}
									}else{
										if( src2_p->a==255 ){
											brev_tbl2 = BlendTable[ blnd - blnd_tbl[ src1_p->a ] ];
											dest_p->b = brev_tbl2[ dest_p->b ] + blnd_tbl[ btbl1[src1_p->b] ] + brev_tbl[ btbl2[src2_p->b] ];
											dest_p->g = brev_tbl2[ dest_p->g ] + blnd_tbl[ gtbl1[src1_p->g] ] + brev_tbl[ gtbl2[src2_p->g] ];
											dest_p->r = brev_tbl2[ dest_p->r ] + blnd_tbl[ rtbl1[src1_p->r] ] + brev_tbl[ rtbl2[src2_p->r] ];
										}else{
											dest_tbl  = BlendTable[ 256 - blnd_tbl[ src1_p->a ] - brev_tbl[ src2_p->a ] ];
											dest_p->b = dest_tbl[ dest_p->b ] + blnd_tbl[ btbl1[src1_p->b] ] + brev_tbl[ btbl2[src2_p->b] ];
											dest_p->g = dest_tbl[ dest_p->g ] + blnd_tbl[ gtbl1[src1_p->g] ] + brev_tbl[ gtbl2[src2_p->g] ];
											dest_p->r = dest_tbl[ dest_p->r ] + blnd_tbl[ rtbl1[src1_p->r] ] + brev_tbl[ rtbl2[src2_p->r] ];
										}
									}
								}else{							
									if( src1_p->a==255 ){	
										dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl1[src1_p->b] ];
										dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl1[src1_p->g] ];
										dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl1[src1_p->r] ];
									}else{						
										brev_tbl2 = BlendTable[ 255 - blnd_tbl[ src1_p->a ] ];
										dest_p->b = brev_tbl2[ dest_p->b ] + blnd_tbl[ btbl1[src1_p->b] ];
										dest_p->g = brev_tbl2[ dest_p->g ] + blnd_tbl[ gtbl1[src1_p->g] ];
										dest_p->r = brev_tbl2[ dest_p->r ] + blnd_tbl[ rtbl1[src1_p->r] ];
									}
								}
							}else{
								if( src2_p->a!=0 ){	
									if( src2_p->a==255 ){	
										dest_p->b = blnd_tbl[ dest_p->b ] + brev_tbl[ btbl2[src2_p->b] ];
										dest_p->g = blnd_tbl[ dest_p->g ] + brev_tbl[ gtbl2[src2_p->g] ];
										dest_p->r = blnd_tbl[ dest_p->r ] + brev_tbl[ rtbl2[src2_p->r] ];
									}else{						
										brev_tbl2 = BlendTable[ 255 - brev_tbl[ src2_p->a ] ];
										dest_p->b = brev_tbl2[ dest_p->b ] + brev_tbl[ btbl2[src2_p->b] ];
										dest_p->g = brev_tbl2[ dest_p->g ] + brev_tbl[ gtbl2[src2_p->g] ];
										dest_p->r = brev_tbl2[ dest_p->r ] + brev_tbl[ rtbl2[src2_p->r] ];
									}
								}
								
							}
						}
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}else if( dobj->nuki==-4 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if(INTERLESS_CHECK){
						for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
							if( src1_p->a > 128 ){
								if( src2_p->a > 128 ){	
									dest_p->b = brev_tbl[ btbl2[src2_p->b] ] + blnd_tbl[ btbl1[src1_p->b] ];
									dest_p->g = brev_tbl[ gtbl2[src2_p->g] ] + blnd_tbl[ gtbl1[src1_p->g] ];
									dest_p->r = brev_tbl[ rtbl2[src2_p->r] ] + blnd_tbl[ rtbl1[src1_p->r] ];
								}else{							
									dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl1[src1_p->b] ];
									dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl1[src1_p->g] ];
									dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl1[src1_p->r] ];
								}
							}else{
								if( src2_p->a > 128 ){	
									dest_p->b = blnd_tbl[ dest_p->b ] + brev_tbl[ btbl2[src2_p->b] ];
									dest_p->g = blnd_tbl[ dest_p->g ] + brev_tbl[ gtbl2[src2_p->g] ];
									dest_p->r = blnd_tbl[ dest_p->r ] + brev_tbl[ rtbl2[src2_p->r] ];
								}
							}
						}
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if(INTERLESS_CHECK){
						for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){

							if( (*(long *)src1_p&0x00ffffff) != dobj->nuki ){
								if( (*(long *)src2_p&0x00ffffff) != dobj->nuki ){
									dest_p->b = brev_tbl[ btbl2[src2_p->b] ] + blnd_tbl[ btbl1[src1_p->b] ];
									dest_p->g = brev_tbl[ gtbl2[src2_p->g] ] + blnd_tbl[ gtbl1[src1_p->g] ];
									dest_p->r = brev_tbl[ rtbl2[src2_p->r] ] + blnd_tbl[ rtbl1[src1_p->r] ];
								}else{
									dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl1[src1_p->b] ];
									dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl1[src1_p->g] ];
									dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl1[src1_p->r] ];
								}
							}else{
								if( (*(long *)src2_p&0x00ffffff) != dobj->nuki ){
									dest_p->b = brev_tbl[ btbl2[src2_p->b] ] + blnd_tbl[ dest_p->b ];
									dest_p->g = brev_tbl[ gtbl2[src2_p->g] ] + blnd_tbl[ dest_p->g ];
									dest_p->r = brev_tbl[ rtbl2[src2_p->r] ] + blnd_tbl[ dest_p->r ];
								}
							}
						}
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
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
				if(INTERLESS_CHECK){
					for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++ ){
						dest_p->b = dbb1_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ] + brev_tbl[ src2_p->b ];
						dest_p->g = dbb1_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
						dest_p->r = dbb1_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
					}
				}else{
					dest_p+=w;
					src1_p+=xinc*w;
					src2_p+=xinc*w;
				}
			}
		}else if( dobj->nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
				if(INTERLESS_CHECK){
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
				}else{
					dest_p+=w;
					src1_p+=xinc*w;
					src2_p+=xinc*w;
				}
			}
		}else if( dobj->nuki==-4 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
				if(INTERLESS_CHECK){
					for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++ ){
						if( src1_p->a > 128 ){
							if( src2_p->a > 128 ){	
								dest_p->b= dbb1_tbl[ dest_p->b] + blnd_tbl[ src1_p->b] + brev_tbl[ src2_p->b ];
								dest_p->g = dbb1_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ] + brev_tbl[ src2_p->g ];
								dest_p->r = dbb1_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ] + brev_tbl[ src2_p->r ];
							}else{					
								dest_p->b = dbb2_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ];
								dest_p->g = dbb2_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ];
								dest_p->r = dbb2_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ];
							}
						}else{
							if( src2_p->a > 128 ){	
								dest_p->b = dbb3_tbl[ dest_p->b ] + brev_tbl[ src2_p->b ];
								dest_p->g = dbb3_tbl[ dest_p->g ] + brev_tbl[ src2_p->g ];
								dest_p->r = dbb3_tbl[ dest_p->r ] + brev_tbl[ src2_p->r ];
							}
							
						}
					}
				}else{
					dest_p+=w;
					src1_p+=xinc*w;
					src2_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
				if(INTERLESS_CHECK){
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
				}else{
					dest_p+=w;
					src1_p+=xinc*w;
					src2_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FTT_Ami( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src1 = (BMP_T *)dobj->src1;
	BMP_T	*src2 = (BMP_T *)dobj->src2;
	int		drev  = dest->sx - dobj->dw;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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
			if(INTERLESS_CHECK){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( linemask2[x & 3] )	{ *dest_p = *(RGB24*)src1_p; }
					else					{ *dest_p = *(RGB24*)src2_p; }
				}
			}else{
				dest_p+=w;
				src1_p+=xinc*w;
				src2_p+=xinc*w;
			}
		}
	}else if( dobj->nuki==-2 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( linemask2[x & 3] ){
						if( src1_p->a!=0 ){
							if( src1_p->a==255 ){
								*dest_p = *(RGB24*)src1_p;
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
								*dest_p = *(RGB24*)src2_p;
							}else{
								brev_tbl = BlendTable[ 256 - src2_p->a ];
								dest_p->b = brev_tbl[ dest_p->b ] + src2_p->b;
								dest_p->g = brev_tbl[ dest_p->g ] + src2_p->g;
								dest_p->r = brev_tbl[ dest_p->r ] + src2_p->r;
							}
						}
					}
				}
			}else{
				dest_p+=w;
				src1_p+=xinc*w;
				src2_p+=xinc*w;
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( linemask2[x & 3] ){
						if( (*(long *)src1_p&0x00ffffff) != dobj->nuki ) { *dest_p = *(RGB24*)src1_p; }
					}else{
						if( (*(long *)src2_p&0x00ffffff) != dobj->nuki ) { *dest_p = *(RGB24*)src2_p; }
					}
				}
			}else{
				dest_p+=w;
				src1_p+=xinc*w;
				src2_p+=xinc*w;
			}
		}
	}
	return TRUE;
}
BOOL DRW_DrawBMP_FTT_Lpp( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src1 = (BMP_T *)dobj->src1;
	BMP_T	*src2 = (BMP_T *)dobj->src2;
	int		drev  = dest->sx - dobj->dw;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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
			if(INTERLESS_CHECK){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					*dest_p = *(RGB24*)src2_p;
				}
			}else{
				dest_p+=drev*w;
				src1_p+=srev1*w;
				src2_p+=srev2*w;
			}
		}
		for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				if( (rate-(y>>4)) <= (y&0xf) ){
					for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
						*dest_p = *(RGB24*)src1_p;
					}
				}else{
					for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
						*dest_p = *(RGB24*)src2_p;
					}
				}
			}else{
				dest_p+=drev*w;
				src1_p+=srev1*w;
				src2_p+=srev2*w;
			}
		}
		for( ; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					*dest_p = *(RGB24*)src1_p;
				}
			}else{
				dest_p+=drev*w;
				src1_p+=srev1*w;
				src2_p+=srev2*w;
			}
		}
	}else if( dobj->nuki==-2 ){	
		for( y=0; y < rate_max[0] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( src2_p->a!=0 ){	
						if( src2_p->a==255 ){
							*dest_p = *(RGB24*)src2_p;
						}else{
							brev_tbl = BlendTable[ 256 - src2_p->a ];
							dest_p->b = brev_tbl[ dest_p->b ] + src2_p->b;
							dest_p->g = brev_tbl[ dest_p->g ] + src2_p->g;
							dest_p->r = brev_tbl[ dest_p->r ] + src2_p->r;
						}
					}
				}
			}else{
				dest_p+=drev*w;
				src1_p+=srev1*w;
				src2_p+=srev2*w;
			}
		}
		for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				if( (rate-(y>>4)) <= (y&0xf) ){
					for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
						if( src1_p->a!=0 ){	
							if( src1_p->a==255 ){
								*dest_p = *(RGB24*)src1_p;
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
								*dest_p = *(RGB24*)src2_p;
							}else{
								brev_tbl = BlendTable[ 256 - src2_p->a ];
								dest_p->b = brev_tbl[ dest_p->b ] + src2_p->b;
								dest_p->g = brev_tbl[ dest_p->g ] + src2_p->g;
								dest_p->r = brev_tbl[ dest_p->r ] + src2_p->r;
							}
						}
					}
				}
			}else{
				dest_p+=drev*w;
				src1_p+=srev1*w;
				src2_p+=srev2*w;
			}
		}
		for( ; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( src1_p->a!=0 ){	
						if( src1_p->a==255 ){
							*dest_p = *(RGB24*)src1_p;
						}else{
							brev_tbl = BlendTable[ 256 - src1_p->a ];
							dest_p->b = brev_tbl[ dest_p->b ] + src1_p->b;
							dest_p->g = brev_tbl[ dest_p->g ] + src1_p->g;
							dest_p->r = brev_tbl[ dest_p->r ] + src1_p->r;
						}
					}
				}
			}else{
				dest_p+=drev*w;
				src1_p+=srev1*w;
				src2_p+=srev2*w;
			}
		}
	}else{			
		for( y=0; y < rate_max[0] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( (*(long*)src2_p&0x00ffffff) != dobj->nuki ){	
						*dest_p = *(RGB24*)src2_p;
					}
				}
			}else{
				dest_p+=drev*w;
				src1_p+=srev1*w;
				src2_p+=srev2*w;
			}
		}
		for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				if( (rate-(y>>4)) <= (y&0xf) ){
					for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
						if( (*(long*)src1_p&0x00ffffff) != dobj->nuki ){	
							*dest_p = *(RGB24*)src1_p;
						}
					}
				}else{
					for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
						if( (*(long*)src2_p&0x00ffffff) != dobj->nuki ){	
							*dest_p = *(RGB24*)src2_p;
						}
					}
				}
			}else{
				dest_p+=drev*w;
				src1_p+=srev1*w;
				src2_p+=srev2*w;
			}
		}
		for( ; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
			if(INTERLESS_CHECK){
				for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
					if( (*(long*)src1_p&0x00ffffff) != dobj->nuki ){	
						*dest_p = *(RGB24*)src1_p;
					}
				}
			}else{
				dest_p+=drev*w;
				src1_p+=srev1*w;
				src2_p+=srev2*w;
			}
		}
	}
	return TRUE;
}






static BOOL DRW_DrawBMP_FT_Bld( DRAW_OBJECT *dobj, int interless );
static BOOL DRW_DrawBMP_FT_Ami( DRAW_OBJECT *dobj, int interless );
static BOOL DRW_DrawBMP_FT_Lpp( DRAW_OBJECT *dobj, int interless );

BOOL DRW_DrawBMP_FTT( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_T	*src1 = (BMP_T *)dobj.src1;
	BMP_T	*src2 = (BMP_T *)dobj.src2;
	DRAW_OBJECT		dobj2, dobj3, dobj4, dobj5;
	
	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FTT]\n"); return FALSE; }
	if( src1->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ1‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FTT]\n"); return FALSE; }
	if( src2->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ2‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FTT]\n"); return FALSE; }

	
	DRW_SetDrawObject2XXX( &dobj, &dobj2, &dobj3, &dobj4, &dobj5 );
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRectS2(&dobj.dx,  &dobj.dy,  dest->sx, dest->sy,
						&dobj.s1x, &dobj.s1y, src1->sx, src1->sy,  &dobj.s1w, &dobj.s1h,
						&dobj.s2x, &dobj.s2y, src2->sx, src2->sy,  &dobj.s2w, &dobj.s2h, dobj.clip ) ) return FALSE;
	}


	if( dobj2.clip!=(void*)-1 ){
		src1 = (BMP_T *)dobj2.src1;
		if( !ClipRect(&dobj2.dx,  &dobj2.dy,  dest->sx, dest->sy,
						&dobj2.s1x, &dobj2.s1y, src1->sx, src1->sy,  &dobj2.dw, &dobj2.dh, dobj2.clip ) ) dobj2.dest=NULL;
	}
	if( dobj3.clip!=(void*)-1 ){
		src1 = (BMP_T *)dobj3.src1;
		if( !ClipRect(&dobj3.dx,  &dobj3.dy,  dest->sx, dest->sy,
						&dobj3.s1x, &dobj3.s1y, src1->sx, src1->sy,  &dobj3.dw, &dobj3.dh, dobj3.clip ) ) dobj3.dest=NULL;
	}
	if( dobj4.clip!=(void*)-1 ){
		src1 = (BMP_T *)dobj4.src1;
		if( !ClipRect(&dobj4.dx,  &dobj4.dy,  dest->sx, dest->sy,
						&dobj4.s1x, &dobj4.s1y, src1->sx, src1->sy,  &dobj4.dw, &dobj4.dh, dobj4.clip ) ) dobj4.dest=NULL;
	}
	if( dobj5.clip!=(void*)-1 ){
		src1 = (BMP_T *)dobj5.src1;
		if( !ClipRect(&dobj5.dx,  &dobj5.dy,  dest->sx, dest->sy,
						&dobj5.s1x, &dobj5.s1y, src1->sx, src1->sy,  &dobj5.dw, &dobj5.dh, dobj5.clip ) ) dobj5.dest=NULL;
	}
	dobj.dw = min(dobj.s1w,dobj.s2w);
	dobj.dh = min(dobj.s1h,dobj.s2h);
	switch(dobj.dparam2){
		default:
		case DRW_BLD2:
			if(dobj2.dest) DRW_DrawBMP_FT_Bld(&dobj2,interless);
			if(dobj3.dest) DRW_DrawBMP_FT_Bld(&dobj3,interless);
			if(dobj4.dest) DRW_DrawBMP_FT_Bld(&dobj4,interless);
			if(dobj5.dest) DRW_DrawBMP_FT_Bld(&dobj5,interless);
			DRW_DrawBMP_FTT_Bld(&dobj,interless);
			break;	
		case DRW_AMI2:
			if(dobj2.dest) DRW_DrawBMP_FT_Ami(&dobj2,interless);
			if(dobj3.dest) DRW_DrawBMP_FT_Ami(&dobj3,interless);
			if(dobj4.dest) DRW_DrawBMP_FT_Ami(&dobj4,interless);
			if(dobj5.dest) DRW_DrawBMP_FT_Ami(&dobj5,interless);
			DRW_DrawBMP_FTT_Ami(&dobj,interless);
			break;	
		case DRW_LPP2:		case DRW_LPP2+1:	case DRW_LPP2+2:
		case DRW_LPP2+3:	case DRW_LPP2+4:	case DRW_LPP2+5:	
			if(dobj2.dest) DRW_DrawBMP_FT_Lpp(&dobj2,interless);
			if(dobj3.dest) DRW_DrawBMP_FT_Lpp(&dobj3,interless);
			if(dobj4.dest) DRW_DrawBMP_FT_Lpp(&dobj4,interless);
			if(dobj5.dest) DRW_DrawBMP_FT_Lpp(&dobj5,interless);
			DRW_DrawBMP_FTT_Lpp(&dobj,interless);
			break;
	}

	return ret;
}









BOOL DRW_DrawBMP_FFB_Ptn( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src1 = (BMP_F *)dobj->src1;
	BMP_BT	*src2 = (BMP_BT *)dobj->src2;
	int		drev  = dest->sx - w;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src1_p = src1->buf + DRW_GetStartPointerSrc1( dobj );
	char	*src2_p = src2->buf + DRW_GetStartPointerSrc2( dobj );
	RGB32	*pal = src2->pal;

	int		xinc = XINC(dobj);
	int		blnd2;
	int		blnd3,i;

	blnd3 = LIM(dobj->dnum3, 1, 256 );

	blnd2 = dobj->dnum2*(256+blnd3)/256;
	for( i=0 ; i<256+blnd3+256 ; i++ ){
		PtnTable[i] = LIM( (i-256)*256/blnd3 ,0,255);
	}

	if(dobj->dparam!=DRW_BLD2){
		char	*blnd_tbl, *brev_tbl;
		
		
		if( dobj->r==128 && dobj->g==128 && dobj->b==128 ){
			if( dobj->nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
							blnd_tbl = BlendTable[     PtnTable[pal[*src2_p].b+blnd2] ];
							brev_tbl = BlendTable[ 255-PtnTable[pal[*src2_p].b+blnd2] ];
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src1_p->b ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src1_p->g ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src1_p->r ];
						}
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[dobj->r];
			char	*gtbl = BrightTable_TB[dobj->g];
			char	*btbl = BrightTable_TB[dobj->b];

			if( dobj->nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc ){
							blnd_tbl = BlendTable[     PtnTable[*src2_p+blnd2] ];
							brev_tbl = BlendTable[ 255-PtnTable[*src2_p+blnd2] ];
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src1_p->b] ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src1_p->g] ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src1_p->r] ];
						}
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}
		}
	}

	return TRUE;
}






BOOL DRW_DrawBMP_FFB( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_F	*src1 = (BMP_F *)dobj.src1;
	BMP_BT	*src2 = (BMP_BT *)dobj.src2;
	
	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FTT]\n"); return FALSE; }
	if( src1->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ1‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FTT]\n"); return FALSE; }
	if( src2->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ2‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FTT]\n"); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRectS2(&dobj.dx,  &dobj.dy,  dest->sx, dest->sy,
						&dobj.s1x, &dobj.s1y, src1->sx, src1->sy,  &dobj.s1w, &dobj.s1h,
						&dobj.s2x, &dobj.s2y, src2->sx, src2->sy,  &dobj.s2w, &dobj.s2h, dobj.clip ) ) return FALSE;
		dobj.dw=dobj.s1w;
		dobj.dh=dobj.s1h;
	}

	switch(dobj.dparam2){
		default:
		case DRW_BLD2:	DRW_DrawBMP_FFB_Ptn(&dobj,interless);	break;	
	}

	return ret;
}



BOOL DRW_DrawBMP_FFBM_Ptn( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src1 = (BMP_F *)dobj->src1;
	BMP_BT	*src2 = (BMP_BT *)dobj->src2;
	BMP_BT	*mask = (BMP_BT *)dobj->mask;
	int		drev  = dest->sx - w;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src1_p = src1->buf + DRW_GetStartPointerSrc1( dobj );
	char	*src2_p = src2->buf + DRW_GetStartPointerSrc2( dobj );
	char	*mask_p = mask->buf;
	RGB32	*pal = src2->pal;

	int		xinc = XINC(dobj);
	int		blnd2 = dobj->dnum2*(256+128)/256,blnd;
	char	*blnd_tbl, *brev_tbl;

	if(mask->sx*2==w){
		if( dobj->nuki==-1 ){	
			xinc *= 2;
			h/=2;
			w/=2;
			for( y=0; y < h ; y++, dest_p+=drev+dest->sx, src1_p+=srev1+src1->sx, src2_p+=srev2+src2->sx ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p+=2, src1_p+=xinc, src2_p+=xinc, mask_p++ ){
						int	mask = PtnTable[*mask_p+blnd2];
						if(mask){
							if(mask<=128){
								if(mask==128){
									*dest_p = *(RGB24*)&pal[*src2_p];
									*(dest_p+1) = *(RGB24*)&pal[*(src2_p+1)];
									*(dest_p+dest->sx) = *(RGB24*)&pal[*(src2_p+src2->sx)];
									*(dest_p+dest->sx+1) = *(RGB24*)&pal[*(src2_p+src2->sx+1)];
								}else{
									blnd = mask<<1;

									blnd_tbl = BlendTable[     blnd ];
									brev_tbl = BlendTable[ 255-blnd ];
									dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ pal[*src2_p].b ];
									dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ pal[*src2_p].g ];
									dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ pal[*src2_p].r ];

									(dest_p+1)->b = brev_tbl[ (dest_p+1)->b ] + blnd_tbl[ pal[*(src2_p+1)].b ];
									(dest_p+1)->g = brev_tbl[ (dest_p+1)->g ] + blnd_tbl[ pal[*(src2_p+1)].g ];
									(dest_p+1)->r = brev_tbl[ (dest_p+1)->r ] + blnd_tbl[ pal[*(src2_p+1)].r ];

									(dest_p+dest->sx)->b = brev_tbl[ (dest_p+dest->sx)->b ] + blnd_tbl[ pal[*(src2_p+src2->sx)].b ];
									(dest_p+dest->sx)->g = brev_tbl[ (dest_p+dest->sx)->g ] + blnd_tbl[ pal[*(src2_p+src2->sx)].g ];
									(dest_p+dest->sx)->r = brev_tbl[ (dest_p+dest->sx)->r ] + blnd_tbl[ pal[*(src2_p+src2->sx)].r ];

									(dest_p+dest->sx+1)->b = brev_tbl[ (dest_p+dest->sx+1)->b ] + blnd_tbl[ pal[*(src2_p+src2->sx+1)].b ];
									(dest_p+dest->sx+1)->g = brev_tbl[ (dest_p+dest->sx+1)->g ] + blnd_tbl[ pal[*(src2_p+src2->sx+1)].g ];
									(dest_p+dest->sx+1)->r = brev_tbl[ (dest_p+dest->sx+1)->r ] + blnd_tbl[ pal[*(src2_p+src2->sx+1)].r ];
								}
							}else{
								if(mask==255){
									*dest_p = *(RGB24*)src1_p;
									*(dest_p+1) = *(RGB24*)(src1_p+1);
									*(dest_p+dest->sx) = *(RGB24*)(src1_p+src1->sx);
									*(dest_p+dest->sx+1) = *(RGB24*)(src1_p+src1->sx+1);
								}else{
									blnd = (mask-128)<<1;
									blnd_tbl = BlendTable[     blnd ];
									brev_tbl = BlendTable[ 255-blnd ];
									dest_p->b = brev_tbl[ pal[*src2_p].b ] + blnd_tbl[ src1_p->b ];
									dest_p->g = brev_tbl[ pal[*src2_p].g ] + blnd_tbl[ src1_p->g ];
									dest_p->r = brev_tbl[ pal[*src2_p].r ] + blnd_tbl[ src1_p->r ];

									(dest_p+1)->b = brev_tbl[ pal[*(src2_p+1)].b ] + blnd_tbl[ (src1_p+1)->b ];
									(dest_p+1)->g = brev_tbl[ pal[*(src2_p+1)].g ] + blnd_tbl[ (src1_p+1)->g ];
									(dest_p+1)->r = brev_tbl[ pal[*(src2_p+1)].r ] + blnd_tbl[ (src1_p+1)->r ];

									(dest_p+dest->sx)->b = brev_tbl[ pal[*(src2_p+src2->sx)].b ] + blnd_tbl[ (src1_p+src1->sx)->b ];
									(dest_p+dest->sx)->g = brev_tbl[ pal[*(src2_p+src2->sx)].g ] + blnd_tbl[ (src1_p+src1->sx)->g ];
									(dest_p+dest->sx)->r = brev_tbl[ pal[*(src2_p+src2->sx)].r ] + blnd_tbl[ (src1_p+src1->sx)->r ];

									(dest_p+dest->sx+1)->b = brev_tbl[ pal[*(src2_p+src2->sx+1)].b ] + blnd_tbl[ (src1_p+src1->sx+1)->b ];
									(dest_p+dest->sx+1)->g = brev_tbl[ pal[*(src2_p+src2->sx+1)].g ] + blnd_tbl[ (src1_p+src1->sx+1)->g ];
									(dest_p+dest->sx+1)->r = brev_tbl[ pal[*(src2_p+src2->sx+1)].r ] + blnd_tbl[ (src1_p+src1->sx+1)->r ];

								}
							}
						}
					}
				}else{
					dest_p+=w;
					src1_p+=xinc*w;
					src2_p+=xinc*w;
				}
			}
		}
	}else{
		if( dobj->r==128 && dobj->g==128 && dobj->b==128 ){
			if( dobj->nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2 ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src1_p+=xinc, src2_p+=xinc, mask_p++ ){
							int	mask = PtnTable[*mask_p+blnd2];
							if(mask){
								if(mask<=128){
									if(mask==128){
										*dest_p = *(RGB24*)&pal[*src2_p];
									}else{
										blnd = mask<<1;

										blnd_tbl = BlendTable[     blnd ];
										brev_tbl = BlendTable[ 255-blnd ];
										dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ pal[*src2_p].b ];
										dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ pal[*src2_p].g ];
										dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ pal[*src2_p].r ];
									}
								}else{
									if(mask==255){
										*dest_p = *(RGB24*)src1_p;
									}else{
										blnd = (mask-128)<<1;
										blnd_tbl = BlendTable[     blnd ];
										brev_tbl = BlendTable[ 255-blnd ];
										dest_p->b = brev_tbl[ pal[*src2_p].b ] + blnd_tbl[ src1_p->b ];
										dest_p->g = brev_tbl[ pal[*src2_p].g ] + blnd_tbl[ src1_p->g ];
										dest_p->r = brev_tbl[ pal[*src2_p].r ] + blnd_tbl[ src1_p->r ];
									}
								}
							}
						}
					}else{
						dest_p+=w;
						src1_p+=xinc*w;
						src2_p+=xinc*w;
					}
				}
			}
		}else{
		}
	}

	return TRUE;
}






BOOL DRW_DrawBMP_FFBM( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_F	*src1 = (BMP_F *)dobj.src1;
	BMP_BT	*src2 = (BMP_BT *)dobj.src2;
	BMP_BT	*mask = (BMP_BT *)dobj.mask;
	
	
	if( dest->buf==NULL ) { DebugPrintf( "•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FFBM]\n" ); return FALSE; }
	if( src1->buf==NULL ) { DebugPrintf( "‰æ‘œ—Ìˆæ1‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FFBM]\n" ); return FALSE; }
	if( src2->buf==NULL ) { DebugPrintf( "‰æ‘œ—Ìˆæ2‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FFBM]\n" ); return FALSE; }
	if( mask->buf==NULL ) { DebugPrintf( "ƒ}ƒXƒN—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FFBM]\n" ); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRectS2(&dobj.dx,  &dobj.dy,  dest->sx, dest->sy,
						&dobj.s1x, &dobj.s1y, src1->sx, src1->sy,  &dobj.s1w, &dobj.s1h,
						&dobj.s2x, &dobj.s2y, src2->sx, src2->sy,  &dobj.s2w, &dobj.s2h, dobj.clip ) ) return FALSE;
		dobj.dw=dobj.s1w;
		dobj.dh=dobj.s1h;
	}

	switch(dobj.dparam2){
		default:
		case DRW_BLD2:	DRW_DrawBMP_FFBM_Ptn(&dobj,interless);	break;	
	}

	return ret;
}







BOOL DRW_DrawBMP_FFF( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_F	*src1 = (BMP_F *)dobj.src1;
	BMP_F	*src2 = (BMP_F *)dobj.src2;
	
	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FTT]\n"); return FALSE; }
	if( src1->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ1‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FTT]\n"); return FALSE; }
	if( src2->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ2‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FTT]\n"); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRectS2(&dobj.dx,  &dobj.dy,  dest->sx, dest->sy,
						&dobj.s1x, &dobj.s1y, src1->sx, src1->sy,  &dobj.s1w, &dobj.s1h,
						&dobj.s2x, &dobj.s2y, src2->sx, src2->sy,  &dobj.s2w, &dobj.s2h, dobj.clip ) ) return FALSE;
	}

	switch(dobj.dparam2){
		default:
		case DRW_BLD2:	break;	
	}

	return ret;
}









BOOL DRW_DrawBMP_FT_Std( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( dobj->nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						*dest_p = *(RGB24*)src_p;
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( dobj->nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a != 0 ){	
							if( src_p->a == 255 ){	
								*dest_p = *(RGB24*)src_p;
							}else{					
								alph_tbl = BlendTable[ 255 - src_p->a ];

								dest_p->b = alph_tbl[dest_p->b] + src_p->b;
								dest_p->g = alph_tbl[dest_p->g] + src_p->g;
								dest_p->r = alph_tbl[dest_p->r] + src_p->r;
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							*dest_p = *(RGB24*)src_p;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( dobj->nuki == -1 ){
			if( (r==0xff || r==0x00) && (g==0xff || g==0x00) && (b==0xff || b==0x00) )
				return PRM_DrawRect_F( dest, dobj->dx, dobj->dy, dobj->dw, dobj->dh, NULL, r, g, b, DRW_NML );

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = btbl[ src_p->b ];
						dest_p->g = gtbl[ src_p->g ];
						dest_p->r = rtbl[ src_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( dobj->nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a != 0 ){	
							if( src_p->a == 255 ){	
								dest_p->b = btbl[ src_p->b ];
								dest_p->g = gtbl[ src_p->g ];
								dest_p->r = rtbl[ src_p->r ];
							}else{					
								alph_tbl = BlendTable[ 255 - src_p->a ];
								alp2_tbl = BlendTable[  src_p->a ];
								alp3_tbl = BlendTable2[ src_p->a ];
								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[btbl[alp3_tbl[src_p->b]]];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl[alp3_tbl[src_p->g]]];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl[alp3_tbl[src_p->r]]];
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = btbl[ src_p->b ];
							dest_p->g = gtbl[ src_p->g ];
							dest_p->r = rtbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FT_Bld( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		blnd = dobj->dnum;
	int		brev = 256-blnd;
	char	*blnd_tbl = BlendTable[blnd];
	char	*brev_tbl = BlendTable[brev];
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;

	if( blnd==256 )	return DRW_DrawBMP_FT_Std( dobj,interless );	
	if( blnd==0   )	return 1;																			

	
	if(r==128 && g==128 && b==128){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FT_Add( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*add_tbl = AddTable;

	
	if( r==0 && g==0 && b==0 ){ return 1; }

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = add_tbl[ dest_p->b + src_p->b ];
						dest_p->g = add_tbl[ dest_p->g + src_p->g ];
						dest_p->r = add_tbl[ dest_p->r + src_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){
							dest_p->b = add_tbl[ dest_p->b + src_p->b ];
							dest_p->g = add_tbl[ dest_p->g + src_p->g ];
							dest_p->r = add_tbl[ dest_p->r + src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = add_tbl[ dest_p->b + src_p->b ];
							dest_p->g = add_tbl[ dest_p->g + src_p->g ];
							dest_p->r = add_tbl[ dest_p->r + src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = add_tbl[ dest_p->b + btbl[src_p->b] ];
						dest_p->g = add_tbl[ dest_p->g + gtbl[src_p->g] ];
						dest_p->r = add_tbl[ dest_p->r + rtbl[src_p->r] ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			char	*alph_tbl;
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = add_tbl[ dest_p->b + btbl[src_p->b] ];
							dest_p->g = add_tbl[ dest_p->g + gtbl[src_p->g] ];
							dest_p->r = add_tbl[ dest_p->r + rtbl[src_p->r] ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FT_Ooi( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	
	if( r==0 && g==0 && b==0 ){ return 1; }

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = OoiTable[ dest_p->b ][ src_p->b ];
						dest_p->g = OoiTable[ dest_p->g ][ src_p->g ];
						dest_p->r = OoiTable[ dest_p->r ][ src_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){
							dest_p->b = OoiTable[ dest_p->b ][ src_p->b ];
							dest_p->g = OoiTable[ dest_p->g ][ src_p->g ];
							dest_p->r = OoiTable[ dest_p->r ][ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = OoiTable[ dest_p->b ][ src_p->b ];
							dest_p->g = OoiTable[ dest_p->g ][ src_p->g ];
							dest_p->r = OoiTable[ dest_p->r ][ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = OoiTable[ dest_p->b ][ btbl[src_p->b] ];
						dest_p->g = OoiTable[ dest_p->g ][ gtbl[src_p->g] ];
						dest_p->r = OoiTable[ dest_p->r ][ rtbl[src_p->r] ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			char	*alph_tbl;
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = OoiTable[ dest_p->b ][ btbl[src_p->b] ];
							dest_p->g = OoiTable[ dest_p->g ][ gtbl[src_p->g] ];
							dest_p->r = OoiTable[ dest_p->r ][ rtbl[src_p->r] ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FT_Sub( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*sub_tbl = SubTable+255;

	
	if( r==0 && g==0 && b==0 ){ return 1; }

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
						dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
						dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){
							dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
							dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
							dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
							dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
							dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = sub_tbl[ dest_p->b - btbl[src_p->b] ];
						dest_p->g = sub_tbl[ dest_p->g - gtbl[src_p->g] ];
						dest_p->r = sub_tbl[ dest_p->r - rtbl[src_p->r] ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			char	*alph_tbl;
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = sub_tbl[ dest_p->b - btbl[src_p->b] ];
							dest_p->g = sub_tbl[ dest_p->g - gtbl[src_p->g] ];
							dest_p->r = sub_tbl[ dest_p->r - rtbl[src_p->r] ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FT_Mul( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*alph_tbl,*alp3_tbl,br;

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = BrightTable_TB[ src_p->b ][ dest_p->b ];
						dest_p->g = BrightTable_TB[ src_p->g ][ dest_p->g ];
						dest_p->r = BrightTable_TB[ src_p->r ][ dest_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = BrightTable_TB[ src_p->b ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ src_p->g ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ src_p->r ][ dest_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = BrightTable_TB[ btbl[src_p->b] ][ dest_p->b ];
						dest_p->g = BrightTable_TB[ gtbl[src_p->g] ][ dest_p->g ];
						dest_p->r = BrightTable_TB[ rtbl[src_p->r] ][ dest_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = BrightTable_TB[ btbl[src_p->b] ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ gtbl[src_p->g] ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ rtbl[src_p->r] ][ dest_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}





BOOL DRW_DrawBMP_FT_Ami( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*linemask[4], *linemask2;
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;
	WORD	mesh_pt;
	int		mesh = dobj->dnum;
	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=0   )	return 1;							
	if( mesh>=256 )	return DRW_DrawBMP_FT_Std( dobj,interless );	

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( linemask2[x & 3] )	*dest_p = *(RGB24*)src_p;
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( linemask2[x & 3] ){
							if( src_p->a != 0 ){
								if( src_p->a == 255 ){
									*dest_p = *(RGB24*)src_p;
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
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							if( linemask2[x & 3] )	*dest_p = *(RGB24*)src_p;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( linemask2[x & 3] ){
							dest_p->b = btbl[ src_p->b ];
							dest_p->g = gtbl[ src_p->g ];
							dest_p->r = rtbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( linemask2[x & 3] ){
							if( src_p->a != 0 ){
								if( src_p->a == 255 ){
									dest_p->b = btbl[ src_p->b ];
									dest_p->g = gtbl[ src_p->g ];
									dest_p->r = rtbl[ src_p->r ];
								}else{
									alph_tbl = BlendTable[ 255 - src_p->a ];
									alp2_tbl = BlendTable[ src_p->a ];
									alp3_tbl = BlendTable2[ src_p->a ];
									dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[btbl[alp3_tbl[src_p->b]]];
									dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl[alp3_tbl[src_p->g]]];
									dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl[alp3_tbl[src_p->r]]];
								}
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							if( linemask2[x & 3] ){
								dest_p->b = btbl[ src_p->b ];
								dest_p->g = gtbl[ src_p->g ];
								dest_p->r = rtbl[ src_p->r ];
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FT_Nis( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;

	int		mesh = dobj->dnum, m=mesh>>2;
	char	*mask;
	static int mfalg=0;

	if( mesh<=0   )	return 1;							
	if( mesh>=256 )	return DRW_DrawBMP_FT_Std( dobj,interless );	

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					mask = MeshFadePatern64[m][y&63];
					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
						if( mask[x&63] ){
							*dest_p = *(RGB24*)src_p;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					mask = MeshFadePatern64[m][y&63];
					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
						if( mask[x&63] ){
							if( src_p->a!=0 ){	
								if( src_p->a==255 ){
									*dest_p = *(RGB24*)src_p;
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
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					mask = MeshFadePatern64[m][y&63];
					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
						if( mask[x&63] ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								*dest_p = *(RGB24*)src_p;
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					mask = MeshFadePatern64[m][y&63];
					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
						if( mask[x&63] ){
							dest_p->b = btbl[ src_p->b ];
							dest_p->g = gtbl[ src_p->g ];
							dest_p->r = rtbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					mask = MeshFadePatern64[m][y&63];
					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
						if( mask[x&63] ){
							if( src_p->a != 0 ){
								if( src_p->a == 255 ){
									dest_p->b = btbl[ src_p->b ];
									dest_p->g = gtbl[ src_p->g ];
									dest_p->r = rtbl[ src_p->r ];
								}else{
									alph_tbl = BlendTable[ 255 - src_p->a ];
									alp2_tbl = BlendTable[ src_p->a ];
									alp3_tbl = BlendTable2[ src_p->a ];
									dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[btbl[alp3_tbl[src_p->b]]];
									dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[gtbl[alp3_tbl[src_p->g]]];
									dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[rtbl[alp3_tbl[src_p->r]]];
								}
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					mask = MeshFadePatern64[m][y&63];
					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
						if( mask[x&63] ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = btbl[ src_p->b ];
								dest_p->g = gtbl[ src_p->g ];
								dest_p->r = rtbl[ src_p->r ];
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FT_Neg( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;
	
	if(r==128 && g==128 && b==128){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = ~src_p->b;
						dest_p->g = ~src_p->g;
						dest_p->r = ~src_p->r;
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){
							if( src_p->a==255 ){
								dest_p->b = ~src_p->b;
								dest_p->g = ~src_p->g;
								dest_p->r = ~src_p->r;
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
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = ~src_p->b;
							dest_p->g = ~src_p->g;
							dest_p->r = ~src_p->r;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = ~btbl[ src_p->b ];
						dest_p->g = ~gtbl[ src_p->g ];
						dest_p->r = ~rtbl[ src_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( src_p->a!=0 ){
							if( src_p->a==255 ){
								dest_p->b = ~btbl[ src_p->b ];
								dest_p->g = ~gtbl[ src_p->g ];
								dest_p->r = ~rtbl[ src_p->r ];
							}else{
								alph_tbl = BlendTable[ 255 - src_p->a ];
								alp2_tbl = BlendTable[ src_p->a ];
								alp3_tbl = BlendTable2[ src_p->a ];
								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[~btbl[alp3_tbl[src_p->b]]];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[~gtbl[alp3_tbl[src_p->g]]];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[~rtbl[alp3_tbl[src_p->r]]];
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = ~btbl[ src_p->b ];
							dest_p->g = ~gtbl[ src_p->g ];
							dest_p->r = ~rtbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FT_Viv( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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
		return DRW_DrawBMP_FT_Std( dobj,interless );
	}
	if(viv==0){
		if(r==128 && g==128 && b==128){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else if( nuki==-2 ){			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
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
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}else{
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							glay = riv_tbl[(src_p->b+src_p->g+src_p->g+src_p->r)>>2];
							dest_p->b = btbl[glay];
							dest_p->g = gtbl[glay];
							dest_p->r = rtbl[glay];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else if( nuki==-2 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
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
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{					
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								glay = riv_tbl[(src_p->b+src_p->g+src_p->g+src_p->r)>>2];
								dest_p->b = btbl[glay];
								dest_p->g = gtbl[glay];
								dest_p->r = rtbl[glay];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}
	}else{
		
		if(r==128 && g==128 && b==128){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							glay = riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
							dest_p->b = glay + viv_tbl[src_p->b];
							dest_p->g = glay + viv_tbl[src_p->g];
							dest_p->r = glay + viv_tbl[src_p->r];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else if( nuki==-2 ){			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
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
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								glay = riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
								dest_p->b = glay + viv_tbl[src_p->b];
								dest_p->g = glay + viv_tbl[src_p->g];
								dest_p->r = glay + viv_tbl[src_p->r];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}else{
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							glay = riv_tbl[ (src_p->b + src_p->g+src_p->g + src_p->r)>>2 ];
							dest_p->b = btbl[ glay + viv_tbl[src_p->b] ];
							dest_p->g = gtbl[ glay + viv_tbl[src_p->g] ];
							dest_p->r = rtbl[ glay + viv_tbl[src_p->r] ];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else if( nuki==-2 ){			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
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
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								glay = riv_tbl[ (src_p->b + src_p->g+src_p->g + src_p->r)>>2 ];
								dest_p->b = btbl[ glay + viv_tbl[src_p->b] ];
								dest_p->g = gtbl[ glay + viv_tbl[src_p->g] ];
								dest_p->r = rtbl[ glay + viv_tbl[src_p->r] ];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}
	}

	return TRUE;
}





BOOL DRW_DrawBMP_FT_Bom( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		x2,y2;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		bom  = dobj->dnum;
	short	*bom_tbl = BomdTabl[bom];
	char	count=0;
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;
	char	*blnd_tbl = BlendTable[256-bom];
	char	*brev_tbl = BlendTable[bom];

	if( bom<=0 ) return DRW_DrawBMP_FT_Std(dobj,interless);
	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
						x2 = bom_tbl[ count++ ]+x+dobj->dx;
						y2 = bom_tbl[ count ]+y+dobj->dy;

						if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
							dest_p = dest->buf + y2 * dest->sx + x2;
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
						if(src_p->a!=0){
							x2 = bom_tbl[ count++ ]+x+dobj->dx;
							y2 = bom_tbl[ count ]+y+dobj->dy;
							if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
								dest_p = dest->buf + y2 * dest->sx + x2;

								if( src_p->a==255 ){	
									dest_p->b = brev_tbl[dest_p->b] + blnd_tbl[src_p->b];
									dest_p->g = brev_tbl[dest_p->g] + blnd_tbl[src_p->g];
									dest_p->r = brev_tbl[dest_p->r] + blnd_tbl[src_p->r];
								}else{						
									alph_tbl = BlendTable[ 255-blnd_tbl[src_p->a] ];

									dest_p->b = alph_tbl[dest_p->b] + blnd_tbl[src_p->b];
									dest_p->g = alph_tbl[dest_p->g] + blnd_tbl[src_p->g];
									dest_p->r = alph_tbl[dest_p->r] + blnd_tbl[src_p->r];
								}
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							x2 = bom_tbl[ count++ ]+x+dobj->dx;
							y2 = bom_tbl[ count ]+y+dobj->dy;

							if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
								dest_p = dest->buf + y2 * dest->sx + x2;
								dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
								dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
								dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
							}
							count++;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
						x2 = bom_tbl[ count++ ]+x+dobj->dx;
						y2 = bom_tbl[ count ]+y+dobj->dy;

						if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
							dest_p = dest->buf + y2 * dest->sx + x2;
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
						if(src_p->a!=0){
							x2 = bom_tbl[ count++ ]+x+dobj->dx;
							y2 = bom_tbl[ count ]+y+dobj->dy;
							if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
								dest_p = dest->buf + y2 * dest->sx + x2;
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
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							x2 = bom_tbl[ count++ ]+x+dobj->dx;
							y2 = bom_tbl[ count ]+y+dobj->dy;

							if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
								dest_p = dest->buf + y2 * dest->sx + x2;
								dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
								dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
								dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FT_Lpp( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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
				if( INTERLESS_CHECK && (rate-(y>>4)) <= (y&0xf) ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						*dest_p = *(RGB24*)src_p;
					}
				}else{
					dest_p+=w;
					src_p +=xinc*w;
				}
			}
			for( ; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						*dest_p = *(RGB24*)src_p;
					}
				}else{
					dest_p+=w;
					src_p +=xinc*w;
				}
			}
		}else if( nuki==-2 ){	
			rate = 46*(256-rate)/256;
			rate_max[0] = STD_Limit( rate*16-240, 0, h );
			rate_max[1] = STD_Limit( rate*16,     0, h );

			dest_p+=rate_max[0]*dest->sx;
			src_p +=rate_max[0]*src->sx;

			for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK && (rate-(y>>4)) <= (y&0xf) ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if(src_p->a!=0){
							if(src_p->a==255){
								*dest_p = *(RGB24*)src_p;
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
					src_p +=xinc*w;
				}
			}
			for( ; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if(src_p->a!=0){
							if(src_p->a==255){
								*dest_p = *(RGB24*)src_p;
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
					src_p +=xinc*w;
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
			if( INTERLESS_CHECK ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *(RGB24*)src_p;
				}
			}else{
				dest_p+=w;
				src_p +=xinc*w;
			}
		}
		for( ; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK && (rate-(y>>4)) > (y&0xf) ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *(RGB24*)src_p;
				}
			}else{
				dest_p+=w;
				src_p +=xinc*w;
			}
		}
		break;
	case RI:
		rate = 56*(256-rate)/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w );
		rate_max[1] = STD_Limit( rate*16,     0, w );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				dest_p+=rate_max[0];
				src_p +=rate_max[0];
				for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc ){
					if( (rate-(x>>4)) <= (x&0xf) ){
						*dest_p = *(RGB24*)src_p;
					}
				}
				for( ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *(RGB24*)src_p;
				}
			}else{
				dest_p += w;
				src_p += xinc*w;
			}
		}
		break;
	case LE:
		rate = 56*rate/256;
		rate_max[0] = min( w, rate*16-240 );
		rate_max[1] = min( w, rate*16 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				for( x=0 ; x < rate_max[0] ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *(RGB24*)src_p;
				}
				for( ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc ){
					if( (rate-(x>>4)) > (x&0xf) ){
						*dest_p = *(RGB24*)src_p;
					}
				}
				dest_p+=w-x;
				src_p +=w-x;
			}else{
				dest_p += w;
				src_p += xinc*w;
			}
		}
		break;
	case CE:
		rate = (20+16)*(256-rate)/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w/2 );
		rate_max[1] = STD_Limit( rate*16,     0, w/2 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				dest_p+=rate_max[0];
				src_p +=rate_max[0];
				for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc ){
					if( (rate-(x>>4)) <= (x&0xf) ){
						*dest_p = *(RGB24*)src_p;
					}
				}
				for( ; x < w/2 ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *(RGB24*)src_p;
				}
				for( ; x > rate_max[1] ; x--, dest_p++, src_p+=xinc ){
					*dest_p = *(RGB24*)src_p;
				}
				for( ; x > rate_max[0] ; x--, dest_p++, src_p+=xinc ){
					if( (rate-(x>>4)) <= ((x-1)&0xf) ){
						*dest_p = *(RGB24*)src_p;
					}
				}
				dest_p+=rate_max[0];
				src_p +=rate_max[0];
			}else{
				dest_p += w;
				src_p += xinc*w;
			}
		}
		break;
	case OU:
		rate = (20+16)*rate/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w/2 );
		rate_max[1] = STD_Limit( rate*16,     0, w/2 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				for( x=0 ; x < rate_max[0] ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *(RGB24*)src_p;
				}
				for( ; x < rate_max[1] ; x++, dest_p++, src_p+=xinc ){
					if( (rate-(x>>4)) > (x&0xf) ){
						*dest_p = *(RGB24*)src_p;
					}
				}
				dest_p+=(w/2-x)*2;
				src_p +=(w/2-x)*2;
				for( ; x > rate_max[0] ; x--, dest_p++, src_p+=xinc ){
					if( (rate-(x>>4)) > ((x-1)&0xf) ){
						*dest_p = *(RGB24*)src_p;
					}
				}
				for( ; x > 0 ; x--, dest_p++, src_p+=xinc ){
					*dest_p = *(RGB24*)src_p;
				}
			}else{
				dest_p += w;
				src_p += xinc*w;
			}
		}
		break;
	}
	return TRUE;
}






BOOL DRW_DrawBMP_FT( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_T	*src  = (BMP_T *)dobj.src1;

	
	if( dest->buf==NULL ) {	DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FT]\n"); return FALSE;}
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FT]\n"); return FALSE; }

	
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

		case DRW_NML:	DRW_DrawBMP_FT_Std( &dobj,interless );	break;	
		case DRW_BLD2:	DRW_DrawBMP_FT_Bld( &dobj,interless );	break;	
		case DRW_ADD:	DRW_DrawBMP_FT_Add( &dobj,interless );	break;	
		case DRW_OOI:	DRW_DrawBMP_FT_Ooi( &dobj,interless );	break;	
		case DRW_SUB:	DRW_DrawBMP_FT_Sub( &dobj,interless );	break;	
		case DRW_MUL:	DRW_DrawBMP_FT_Mul( &dobj,interless );	break;	
		case DRW_AMI2:	DRW_DrawBMP_FT_Ami( &dobj,interless );	break;	
		case DRW_NIS2:	DRW_DrawBMP_FT_Nis( &dobj,interless );	break;	
		case DRW_NEG:	DRW_DrawBMP_FT_Neg(	&dobj,interless );	break;	
		case DRW_VIV2:	DRW_DrawBMP_FT_Viv( &dobj,interless );	break;	
		case DRW_BOM2:	DRW_DrawBMP_FT_Bom(	&dobj,interless );	break;	


		case DRW_LPP2:		case DRW_LPP2+1:	case DRW_LPP2+2:
		case DRW_LPP2+3:	case DRW_LPP2+4:	case DRW_LPP2+5:	
			DRW_DrawBMP_FT_Lpp( &dobj,interless );
			break;

	}
	return ret;
}





typedef struct{
	short	w,h;
	char	nwf1, nwf2;
	char	interless;

	short	depth[2][300][400];
	signed char	refraction[514][514];
	signed char	refraction2[258][258];
	char	bright[514][514];
	

	short	addx[400+256];
	short	addy[300+256];
	short	rimit[256+512+256+512];
}RIPPLE_STRUCT;
RIPPLE_STRUCT	Repple;



typedef struct{
	DWORD	d1;
	WORD	d2;
}RGB48;


void DRW_SetRippleTable( int w, int h )
{
	int	x,y;
	Repple.w=w;
	Repple.h=h;
	Repple.nwf1=0;
	Repple.nwf2=1;

	ZeroMemory( Repple.depth, 2*300*400*sizeof(short) );
	for(y = 0; y<514; y++ ){
		for(x = 0; x<514; x++ ){
			Repple.refraction[x][y] = LIM( ((x-256) - (y-256)) / 4, -128, 127 );
		    Repple.bright[x][y] = LIM( ((x-256) - (y-256))/2, 0, 255 );
		}
	}
	for(x = 0; x<257; x++ ){
		for(y = 0; y<257; y++ ){
			Repple.refraction2[x][y] = min( abs((x-128)+(y-128)), 127 );
		}
	}
	for(x = 0; x<w/2+256; x++ ){
		Repple.addx[x] = LIM(x-128,0,w/2-1)*2;
	}
	for(y = 0; y<h/2+256; y++ ){
		Repple.addy[y] = LIM(y-128,0,h/2-1)*2;
	}
	for(y = 0; y<1024+512; y++ ){
		Repple.rimit[y] = LIM(y-768,-128,128);
	}
}
void DRW_RenewRippleTable( int side_flag )
{
	short	*depth_p1 = &Repple.depth[Repple.nwf1][1][1];
	short	*depth_p2 = &Repple.depth[Repple.nwf2][1][1];
	short	x,y;
	short	w = Repple.w/2-1;
	short	h = Repple.h/2-1;
	short	revy = 400-(w-1);
	int		ck1=0,ck2=0;

int	now1 = timeGetTime();

	if(side_flag){
		for(y = 0 ; y<=h ; y++ ){
			Repple.depth[Repple.nwf1][y][0] = Repple.depth[Repple.nwf1][y][1];
			Repple.depth[Repple.nwf1][y][w] = Repple.depth[Repple.nwf1][y][w-1];
		}
		for(x = 0 ; x<=w ; x++ ){
			Repple.depth[Repple.nwf1][0][x] = Repple.depth[Repple.nwf1][1][x];
			Repple.depth[Repple.nwf1][h][x] = Repple.depth[Repple.nwf1][h-1][x];
		}
	}

	for(y = 1 ; y<h ; y++, depth_p2+=revy, depth_p1+=revy ){
if( (Repple.nwf1+y)%2 ){
		for(x = 1 ; x<w ; x++, depth_p2++, depth_p1++ ){






			


			*depth_p2 = Repple.rimit[ ((*(depth_p1-400+1) + *(depth_p1+400+1) + *(depth_p1-400-1) + *(depth_p1+400-1) - *depth_p2 - *depth_p2)>>1)+768 ];






		}
}else{
	CopyMemory(depth_p2,depth_p1,(w-1)*2);
	depth_p2+=w-1;
	depth_p1+=w-1;
}
	}

	Repple.nwf1 = (Repple.nwf1 + 1) & 1;
	Repple.nwf2 = 1 - Repple.nwf1;

int	now2 = timeGetTime();
DebugPrintf("%d %d %d\n",now2-now1,((short)-256)>>1,((short)256)>>1);
	
}
short	*DRW_GetRippleTableAddr( void )
{
	return Repple.depth[Repple.nwf1][0];
}
int DRW_GetRippleTableNwf( void )
{
	return Repple.nwf2;
}

BOOL DRW_DrawBMP_FF_Rp2( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw/2-1;
	int		h = dobj->dh/2-1;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx*2-(dobj->dw/2-2)*2;
	int		prev = 400-(dobj->dw/2-2);
	RGB24	*dest_p = dest->buf + (dobj->dy+2)*dest->sx + (dobj->dx+2);
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	short	*depth_p = &Repple.depth[Repple.nwf1][1][1];

	Repple.interless = !Repple.interless;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		for(y = 1 ; y<h ; y++, dest_p+=drev, depth_p+=prev ){
			if( (Repple.interless+y)%2 ){
				for(x = 1 ; x<w ; x++, dest_p+=2, depth_p++ ){
	
	
	
	
	

	
	
	
					
		
		

	
	
	

					
		
		

	
	
	

	
	
	
	
	
	
	
	
	
	
	

	
	
	
	
	
	
	
	
	
	
	
	
	
							src_p = src->buf+ src->sx*(Repple.addy[ Repple.refraction[ *(depth_p-400)+256 ][ *(depth_p+400)+256 ] + y+128 ]+dobj->s1y)
													+ (Repple.addx[ Repple.refraction[ *(depth_p-1)+256 ][ *(depth_p+1)+256 ] + x+128 ]+dobj->s1x);
	
	

	
	
	
							*(RGB48*)dest_p  = *(RGB48*)src_p;
	
							*(RGB48*)(dest_p+dest->sx)  = *(RGB48*)(src_p+src->sx);
	

	
				}
			}else{
				dest_p += (w-1)<<1;
				depth_p+= w-1;
			}
		}
	}else if(r==g && g==b ){
		char	*rtbl = BrightTable_TB[r];

		for(y = 1 ; y<h ; y++, dest_p+=drev, depth_p+=prev ){
			if( (Repple.interless+y)%2 ){
				for(x = 1 ; x<w ; x++, dest_p+=2, depth_p++ ){
					src_p = src->buf+ src->sx*(Repple.addy[ Repple.refraction[ *(depth_p-400)+256 ][ *(depth_p+400)+256 ] + y+128 ]+dobj->s1y)
											+ (Repple.addx[ Repple.refraction[ *(depth_p-1)+256 ][ *(depth_p+1)+256 ] + x+128 ]+dobj->s1x);

					dest_p->b = rtbl[ src_p->b ];
					dest_p->g = rtbl[ src_p->g ];
					dest_p->r = rtbl[ src_p->r ];
					(dest_p+1)->b = rtbl[ (src_p+1)->b ];
					(dest_p+1)->g = rtbl[ (src_p+1)->g ];
					(dest_p+1)->r = rtbl[ (src_p+1)->r ];
					(dest_p+dest->sx)->b   = rtbl[ (src_p+src->sx)->b ];
					(dest_p+dest->sx)->g   = rtbl[ (src_p+src->sx)->g ];
					(dest_p+dest->sx)->r   = rtbl[ (src_p+src->sx)->r ];
					(dest_p+dest->sx+1)->b = rtbl[ (src_p+src->sx+1)->b ];
					(dest_p+dest->sx+1)->g = rtbl[ (src_p+src->sx+1)->g ];
					(dest_p+dest->sx+1)->r = rtbl[ (src_p+src->sx+1)->r ];
				}
			}else{
				dest_p += (w-1)<<1;
				depth_p+= w-1;
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		for(y = 1 ; y<h ; y++, dest_p+=drev, depth_p+=prev ){
			if( (Repple.interless+y)%2 ){
				for(x = 1 ; x<w ; x++, dest_p+=2, depth_p++ ){
					src_p = src->buf+ src->sx*(Repple.addy[ Repple.refraction[ *(depth_p-400)+256 ][ *(depth_p+400)+256 ] + y+128 ]+dobj->s1y)
											+ (Repple.addx[ Repple.refraction[ *(depth_p-1)+256 ][ *(depth_p+1)+256 ] + x+128 ]+dobj->s1x);

					dest_p->b = btbl[ src_p->b ];
					dest_p->g = gtbl[ src_p->g ];
					dest_p->r = rtbl[ src_p->r ];
					(dest_p+1)->b = btbl[ (src_p+1)->b ];
					(dest_p+1)->g = gtbl[ (src_p+1)->g ];
					(dest_p+1)->r = rtbl[ (src_p+1)->r ];

					(dest_p+dest->sx)->b   = btbl[ (src_p+src->sx)->b ];
					(dest_p+dest->sx)->g   = gtbl[ (src_p+src->sx)->g ];
					(dest_p+dest->sx)->r   = rtbl[ (src_p+src->sx)->r ];
					(dest_p+dest->sx+1)->b = btbl[ (src_p+src->sx+1)->b ];
					(dest_p+dest->sx+1)->g = gtbl[ (src_p+src->sx+1)->g ];
					(dest_p+dest->sx+1)->r = rtbl[ (src_p+src->sx+1)->r ];
				}
			}else{
				dest_p += (w-1)<<1;
				depth_p+= w-1;
			}
		}
	}
	return 1;
}



BOOL DRW_DrawBMP_FF_RpM( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw/2-1;
	int		h = dobj->dh/2-1;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx*2-(dobj->dw/2-2)*2;
	int		prev = 400-(dobj->dw/2-2);
	RGB24	*dest_p = dest->buf + (dobj->dy+1)*dest->sx + (dobj->dx+1);
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	short	*depth_p = &Repple.depth[Repple.nwf1][1][1];

	Repple.interless = !Repple.interless;
	
	for(y = 1 ; y<h ; y++, dest_p+=drev, depth_p+=prev ){

			for(x = 1 ; x<w ; x++, dest_p+=2, depth_p++ ){
				int	rx;
				int	x1 = *(depth_p-1)+256;
				int	x2 = *(depth_p+1)+256;
				int	y1 = *(depth_p-400)+256;
				int	y2 = *(depth_p+400)+256;





				
	
	





				
	
	







				rx = Repple.refraction2[ (*(depth_p-400)-*(depth_p+400)+512)>>2 ][ (*(depth_p-1)-*(depth_p+1)+512)>>2 ];









				if(rx){
					(dest_p)->r = (dest_p+1)->r = (dest_p+dest->sx)->r = (dest_p+dest->sx+1)->r = 
					(dest_p)->g = (dest_p+1)->g = (dest_p+dest->sx)->g = (dest_p+dest->sx+1)->g = rx;
					(dest_p)->b = (dest_p+1)->b = (dest_p+dest->sx)->b = (dest_p+dest->sx+1)->b = (rx)<<1;
				}
			}




	}
return 1;
}



BOOL DRW_DrawBMP_FF_Std( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( dobj->nuki==-1 ){	
			if(dobj->rparam&REV_W){
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							*dest_p = *src_p;
						}
					}else{
						dest_p += w;
						src_p  += xinc*w;
					}
				}
			}else{
				if(dobj->rparam&REV_H){
					for( y=0; y < h ; y++, dest_p+=dest->sx, src_p-=src->sx )
						if( INTERLESS_CHECK ){
							CopyMemory( dest_p, src_p, w * sizeof(RGB24) );	
						}
				}else{
					if( w==dest->sx && w==src->sx && interless==0 ){
						CopyMemory( dest_p, src_p, w*h * sizeof(RGB24) );	
					}else{
						for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
							if( INTERLESS_CHECK ){
								CopyMemory( dest_p, src_p, w * sizeof(RGB24) );	
							}
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							*dest_p = *src_p;
						}
					}
				}else{
					dest_p += w;
					src_p  += xinc*w;
				}
			}
		}
	}else{
		if(r==g && g==b && dobj->nuki == -1 ){
			char	*rtbl = BrightTable_TB[r];




			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = rtbl[ src_p->b ];
						dest_p->g = rtbl[ src_p->g ];
						dest_p->r = rtbl[ src_p->r ];
					}
				}else{
					dest_p += w;
					src_p  += xinc*w;
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			if( dobj->nuki == -1 ){
				if( (r==0xff || r==0x00) && (g==0xff || g==0x00) && (b==0xff || b==0x00) )
					return PRM_DrawRect_F( dest, dobj->dx, dobj->dy, dobj->dw, dobj->dh, NULL, r, g, b, DRW_NML );

				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = btbl[ src_p->b ];
							dest_p->g = gtbl[ src_p->g ];
							dest_p->r = rtbl[ src_p->r ];
						}
					}else{
						dest_p += w;
						src_p  += xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = btbl[ src_p->b ];
								dest_p->g = gtbl[ src_p->g ];
								dest_p->r = rtbl[ src_p->r ];
							}
						}
					}else{
						dest_p += w;
						src_p  += xinc*w;
					}
				}
			}
		}
	}

	return TRUE;
}




BOOL DRW_DrawBMP_FF_Bld( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		blnd = dobj->dnum;
	int		brev = 256-blnd;
	char	*blnd_tbl = BlendTable[blnd];
	char	*brev_tbl = BlendTable[brev];



	if( blnd==256 )	return DRW_DrawBMP_FF_Std( dobj, interless );	
	if( blnd==0   )	return 1;																			

	
	if( MMX_Flag && nuki==-1 && (r==128 && g==128 && b==128) && dobj->rparam==0 ){
		DWORD	alpha2 = brev * 0x01010101;
		DWORD	alpha1 = blnd * 0x01010101;
		int		w2=w*3;
		int		w3=w2/8;
		int		wa=w2%8;
		char	*dest_p2, *src_p2;

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				if(w3){
					_asm {
						mov			ecx, w3
						mov			eax, src_p	
						mov			ebx, dest_p	
						pxor		mm3, mm3	
						movd		mm4, alpha1	
						movd		mm5, alpha2	
						punpcklbw	mm4, mm3	
						punpcklbw	mm5, mm3	

						loopx:

						
							movq		mm0, [eax]	
							movq		mm1, [ebx]	
							add		eax,8			

							
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
							paddusw		mm6, mm7	
							
							psrlw		mm0, 8		
							psrlw		mm6, 8		
							
							packuswb	mm0, mm6	
							movq		[ebx], mm0	


							add		ebx,8			

						dec ecx;
						jnz loopx;

						mov			src_p,  eax	
						mov			dest_p, ebx	
					}
				}
				dest_p2 = (char*)dest_p;
				src_p2  = (char*)src_p;
				for( x=0; x<wa ;x++, dest_p2++, src_p2++ ){	
					*dest_p2 = brev_tbl[ *dest_p2 ] + blnd_tbl[ *src_p2 ];
				}
				dest_p = (RGB24*)dest_p2;
				src_p  = (RGB24*)src_p2;
			}else{
				dest_p+=w;
				src_p+=w;
			}
		}
		_asm	emms
	}else{
		if(r==128 && g==128 && b==128){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
								dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
								dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
								dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
								dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FF_Add( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		w2=w/2;
	int		w3=w%2;
	int		w4=w2*2;
	char	*add_tbl = AddTable;

	
	if( r==0 && g==0 && b==0 ){ return 1; }

	
	if( MMX_Flag && nuki==-1 && (r==128 && g==128 && b==128) && !dobj->rparam ){
		int		w2=w*3;
		int		w3=w2/8;
		int		wa=w2%8;
		char	*dest_p2, *src_p2;

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				if(w3){
					_asm {
						mov		eax, src_p
						mov		ebx, dest_p
						mov		ecx, w3

						loopx:
							movq	mm0, [eax]
							movq	mm1, [ebx]
							paddusb	mm0, mm1
							movq	[ebx], mm0
							add		eax, 8
							add		ebx, 8
						loop	loopx
						mov		src_p,  eax
						mov		dest_p, ebx
					}
				}
				dest_p2 = (char*)dest_p;
				src_p2  = (char*)src_p;
				for( x=0; x<wa ;x++, dest_p2++, src_p2++ ){	
					*dest_p2 = add_tbl[ *dest_p2 + *src_p2 ];
				}
				dest_p = (RGB24*)dest_p2;
				src_p  = (RGB24*)src_p2;
			}else{
				dest_p+=w;
				src_p+=w;
			}
		}
		_asm	emms
	}else{
		if( (r==128 && g==128 && b==128) ){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = add_tbl[ dest_p->b + src_p->b ];
							dest_p->g = add_tbl[ dest_p->g + src_p->g ];
							dest_p->r = add_tbl[ dest_p->r + src_p->r ];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = add_tbl[ dest_p->b + src_p->b ];
								dest_p->g = add_tbl[ dest_p->g + src_p->g ];
								dest_p->r = add_tbl[ dest_p->r + src_p->r ];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = add_tbl[ dest_p->b + btbl[src_p->b] ];
							dest_p->g = add_tbl[ dest_p->g + gtbl[src_p->g] ];
							dest_p->r = add_tbl[ dest_p->r + rtbl[src_p->r] ];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = add_tbl[ dest_p->b + btbl[src_p->b] ];
								dest_p->g = add_tbl[ dest_p->g + gtbl[src_p->g] ];
								dest_p->r = add_tbl[ dest_p->r + rtbl[src_p->r] ];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FF_Ooi( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	
	if( r==0 && g==0 && b==0 ){ return 1; }

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = OoiTable[ dest_p->b ][ src_p->b ];
						dest_p->g = OoiTable[ dest_p->g ][ src_p->g ];
						dest_p->r = OoiTable[ dest_p->r ][ src_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = OoiTable[ dest_p->b ][ src_p->b ];
							dest_p->g = OoiTable[ dest_p->g ][ src_p->g ];
							dest_p->r = OoiTable[ dest_p->r ][ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = OoiTable[ dest_p->b ][ btbl[src_p->b] ];
						dest_p->g = OoiTable[ dest_p->g ][ gtbl[src_p->g] ];
						dest_p->r = OoiTable[ dest_p->r ][ rtbl[src_p->r] ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = OoiTable[ dest_p->b ][ btbl[src_p->b] ];
							dest_p->g = OoiTable[ dest_p->g ][ gtbl[src_p->g] ];
							dest_p->r = OoiTable[ dest_p->r ][ rtbl[src_p->r] ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FF_Sub( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		w2=w/2;
	int		w3=w%2;
	int		w4=w2*2;
	char	*sub_tbl = SubTable+255;

	
	if( r==0 && g==0 && b==0 ){ return 1; }

	
	if( MMX_Flag && (r==128 && g==128 && b==128) && !dobj->rparam ){
		int		w2=w*3;
		int		w3=w2/8;
		int		wa=w2%8;
		char	*dest_p2, *src_p2;

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				if(w3){
					_asm {
						mov		eax, src_p
						mov		ebx, dest_p
						mov		ecx, w3

						loopx:
							movq	mm1, [eax]
							movq	mm0, [ebx]
							psubusb	mm0, mm1
							movq	[ebx], mm0
							add		eax, 8
							add		ebx, 8
						loop	loopx
						mov		src_p,  eax
						mov		dest_p, ebx
					}
				}
				dest_p2 = (char*)dest_p;
				src_p2  = (char*)src_p;
				for( x=0; x<wa ;x++, dest_p2++, src_p2++ ){	
					*dest_p2 = sub_tbl[ *dest_p2 - *src_p2 ];
				}
				dest_p = (RGB24*)dest_p2;
				src_p  = (RGB24*)src_p2;
			}else{
				dest_p += w;
				src_p  += w;
			}
		}
		_asm	emms
	}else{
		if( (r==128 && g==128 && b==128) ){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
							dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
							dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
						}
					}else{
						dest_p += w;
						src_p  += xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = sub_tbl[ dest_p->b - src_p->b ];
								dest_p->g = sub_tbl[ dest_p->g - src_p->g ];
								dest_p->r = sub_tbl[ dest_p->r - src_p->r ];
							}
						}
					}else{
						dest_p += w;
						src_p  += xinc*w;
					}
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = sub_tbl[ dest_p->b - btbl[src_p->b] ];
							dest_p->g = sub_tbl[ dest_p->g - gtbl[src_p->g] ];
							dest_p->r = sub_tbl[ dest_p->r - rtbl[src_p->r] ];
						}
					}else{
						dest_p += w;
						src_p  += xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = sub_tbl[ dest_p->b - btbl[src_p->b] ];
								dest_p->g = sub_tbl[ dest_p->g - gtbl[src_p->g] ];
								dest_p->r = sub_tbl[ dest_p->r - rtbl[src_p->r] ];
							}
						}
					}else{
						dest_p += w;
						src_p  += xinc*w;
					}
				}
			}
		}
	}
	return TRUE;
}
#define RIPLE_SEIDO	128
#define X_SEIDO	4
#define Y_SEIDO	4
signed char	RipleTable[RIPLE_SEIDO][600/Y_SEIDO][800/X_SEIDO];		
static int	RipleW;
static int	RipleH;
static int	RipleCX;
static int	RipleCY;

void DRW_InitRipleMapping( int w, int h, int cx, int cy)
{
	int i, j, k, x, y;

	if( RipleW==w && RipleH==h && RipleCX==cx && RipleCY==cy ) return;
	RipleW=w;
	RipleH=h;
	RipleCX=cx;
	RipleCY=cy;


	for( j=0; j<h/Y_SEIDO; j++) {
		for( i=0; i<w/X_SEIDO; i++) {
			x = (i - (cx/X_SEIDO))*X_SEIDO;
			y = (j - (cy/Y_SEIDO))*Y_SEIDO;
			int dist = (int)sqrt( (double)(x*x+y*y) )<<1;
			for( k=0; k<RIPLE_SEIDO; k++) {
				int	work = (dist - (k<<2))&127;
				RipleTable[k][j][i] = ((signed char)((((127 * sin(PAI*2*work/128)+127) - 128)*(k+1))/512));
			}
		}
	}

}

BOOL DRW_DrawBMP_FF_RPL( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		time = LIM( dobj->dnum/2, 0, RIPLE_SEIDO-1 );

	int		sx;
	
	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev ){
				if( INTERLESS_CHECK ){
					signed char	*sintbl = RipleTable[time][y/Y_SEIDO];
					int	sy = LIM( sintbl[ y>>2 ]+y, 1, src->sy-2 );
					RGB24	*sss = src_p + sy*src->sx;
					for( x=0 ; x < w ; x++, dest_p++ ){
						sx = sintbl[ x>>2 ]+x;
		
		
		
		
		
						
						*(DWORD*)dest_p = *(DWORD*)(sss+sx);


					}
				}else{
					dest_p+=w;
				}
			}
		}
	}else if( (r==g && g==b) ){
		char	*rgb_tbl = BrightTable_TB[r];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev ){
				if( INTERLESS_CHECK ){
					signed char	*sintbl = RipleTable[time][y/Y_SEIDO];
					RGB24	*sss = src_p + y*src->sx;
					for( x=0 ; x < w ; x++, dest_p++ ){
						sx = sintbl[ x>>2 ]+x;
						dest_p->b = rgb_tbl[ (sss+sx)->b ];
						dest_p->g = rgb_tbl[ (sss+sx)->g ];
						dest_p->r = rgb_tbl[ (sss+sx)->r ];
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
			for( y=0; y < h ; y++, dest_p+=drev ){
				if( INTERLESS_CHECK ){
					signed char	*sintbl = RipleTable[time][y/Y_SEIDO];
					RGB24	*sss = src_p + y*src->sx;
					for( x=0 ; x < w ; x++, dest_p++ ){
						sx = sintbl[ x>>2 ]+x;
						dest_p->b = rtbl[ (sss+sx)->b ];
						dest_p->g = gtbl[ (sss+sx)->g ];
						dest_p->r = btbl[ (sss+sx)->r ];
					}
				}else{
					dest_p+=w;
				}
			}
		}
	}
	return TRUE;
}


BOOL DRW_DrawBMP_FF_Dim( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	

	if( MMX_Flag && nuki==-1 && (r==128 && g==128 && b==128) && dobj->rparam==0 ){
		DWORD	alpha1 = 0xffffffff;
		int		w2=w*3;
		int		w3=w2/8;
		int		wa=w2%8;
		char	*dest_p2, *src_p2;

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				if(w3){
					_asm {
						mov			ecx, w3
						mov			eax, src_p	
						mov			ebx, dest_p	
						pxor		mm3, mm3	
						movd		mm4, alpha1	
						punpcklbw	mm4, mm3	

						loopx:

						
							movq		mm0, [eax]	
							movq		mm1, [ebx]	
							
							movq		mm6, mm0	
							movq		mm7, mm1	
							
							punpcklbw	mm0, mm3	
							punpcklbw	mm1, mm3	
							
							punpckhbw	mm6, mm3	
							punpckhbw	mm7, mm3	
							
							movq		mm5, mm1	
							pxor		mm5, mm4
							pmullw		mm1, mm1	
							pmullw		mm0, mm5	
							paddusw		mm0, mm1	

							movq		mm5, mm7	
							pxor		mm5, mm4
							pmullw		mm7, mm7	
							pmullw		mm6, mm5	
							paddusw		mm6, mm7	
							
							psrlw		mm0, 8		
							psrlw		mm6, 8		
							
							packuswb	mm0, mm6	
							movq		[ebx], mm0	

							add		eax,8			
							add		ebx,8			
						loop	loopx
						mov			src_p,  eax	
						mov			dest_p, ebx	
					}
				}
				dest_p2 = (char*)dest_p;
				src_p2  = (char*)src_p;
				for( x=0; x<wa ;x++, dest_p2++, src_p2++ ){	
					*dest_p2 = DimTable[*dest_p2][*src_p2];
				}
				dest_p = (RGB24*)dest_p2;
				src_p  = (RGB24*)src_p2;
			}else{
				dest_p+=w;
				src_p+=w;
			}
		}
		_asm	emms
	}else{
		if( (r==128 && g==128 && b==128) ){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = DimTable[dest_p->b][src_p->b];
							dest_p->g = DimTable[dest_p->g][src_p->g];
							dest_p->r = DimTable[dest_p->r][src_p->r];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = DimTable[dest_p->b][src_p->b];
								dest_p->g = DimTable[dest_p->g][src_p->g];
								dest_p->r = DimTable[dest_p->r][src_p->r];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = DimTable[dest_p->b][btbl[src_p->b]];
							dest_p->g = DimTable[dest_p->g][gtbl[src_p->g]];
							dest_p->r = DimTable[dest_p->r][rtbl[src_p->r]];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = DimTable[dest_p->b][btbl[src_p->b]];
								dest_p->g = DimTable[dest_p->g][gtbl[src_p->g]];
								dest_p->r = DimTable[dest_p->r][rtbl[src_p->r]];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FF_Mul( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = BrightTable_TB[ src_p->b ][ dest_p->b ];
						dest_p->g = BrightTable_TB[ src_p->g ][ dest_p->g ];
						dest_p->r = BrightTable_TB[ src_p->r ][ dest_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = BrightTable_TB[ src_p->b ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ src_p->g ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ src_p->r ][ dest_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = BrightTable_TB[ btbl[src_p->b] ][ dest_p->b ];
						dest_p->g = BrightTable_TB[ gtbl[src_p->g] ][ dest_p->g ];
						dest_p->r = BrightTable_TB[ rtbl[src_p->r] ][ dest_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = BrightTable_TB[ btbl[src_p->b] ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ gtbl[src_p->g] ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ rtbl[src_p->r] ][ dest_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}





BOOL DRW_DrawBMP_FF_Ami( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	int		mesh = dobj->dnum;
	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=0   )	return 1;							
	if( mesh>=256 )	return DRW_DrawBMP_FF_Std( dobj, interless );	

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( linemask2[x & 3] ){
							*dest_p = *src_p;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							if( linemask2[x & 3] ){
								*dest_p = *src_p;
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( linemask2[x & 3] ){
							dest_p->b = btbl[ src_p->b ];
							dest_p->g = gtbl[ src_p->g ];
							dest_p->r = rtbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					linemask2 = linemask[y & 3];
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							if( linemask2[x & 3] ){
								dest_p->b = btbl[ src_p->b ];
								dest_p->g = gtbl[ src_p->g ];
								dest_p->r = rtbl[ src_p->r ];
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FF_Nis( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		mesh = dobj->dnum, m=mesh>>2;

	static int mfalg=0;

	if( mesh<=0   )	return 1;							
	if( mesh>=256 )	return DRW_DrawBMP_FF_Std( dobj, interless );	

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){

					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){

						if( (RAND_TBL&0xff) < (unsigned)mesh ){
							*dest_p = *src_p;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){

					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){

						if( (RAND_TBL&0xff) < (unsigned)mesh ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								*dest_p = *src_p;
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){

					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
						if( (RAND_TBL&0xff) < (unsigned)mesh ){

							dest_p->b = btbl[ src_p->b ];
							dest_p->g = gtbl[ src_p->g ];
							dest_p->r = rtbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){

					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
						if( (RAND_TBL&0xff) < (unsigned)mesh ){

							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->b = btbl[ src_p->b ];
								dest_p->g = gtbl[ src_p->g ];
								dest_p->r = rtbl[ src_p->r ];
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FF_Neg( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	
	if(r==128 && g==128 && b==128){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = ~src_p->b;
						dest_p->g = ~src_p->g;
						dest_p->r = ~src_p->r;
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = ~src_p->b;
							dest_p->g = ~src_p->g;
							dest_p->r = ~src_p->r;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = ~btbl[ src_p->b ];
						dest_p->g = ~gtbl[ src_p->g ];
						dest_p->r = ~rtbl[ src_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = ~btbl[ src_p->b ];
							dest_p->g = ~gtbl[ src_p->g ];
							dest_p->r = ~rtbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FF_Viv( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		viv = dobj->dnum;
	int		glay;
	char	*viv_tbl=BlendTable[viv];
	char	*riv_tbl=BlendTable[256-viv];
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
	char	*btbl = BrightTable_TB[b];

	if( viv==256 ){
		return DRW_DrawBMP_FF_Std( dobj, interless );
	}
	if(viv==0){
		if(r==128 && g==128 && b==128){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}else{
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							glay = riv_tbl[(src_p->b+src_p->g+src_p->g+src_p->r)>>2];
							dest_p->b = btbl[glay];
							dest_p->g = gtbl[glay];
							dest_p->r = rtbl[glay];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{					
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								glay = riv_tbl[(src_p->b+src_p->g+src_p->g+src_p->r)>>2];
								dest_p->b = btbl[glay];
								dest_p->g = gtbl[glay];
								dest_p->r = rtbl[glay];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}
	}else{
		
		if(r==128 && g==128 && b==128){
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							glay = riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
							dest_p->b = glay + viv_tbl[src_p->b];
							dest_p->g = glay + viv_tbl[src_p->g];
							dest_p->r = glay + viv_tbl[src_p->r];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								glay = riv_tbl[ ( src_p->b+ src_p->g + src_p->g + src_p->r )>>2 ];
								dest_p->b = glay + viv_tbl[src_p->b];
								dest_p->g = glay + viv_tbl[src_p->g];
								dest_p->r = glay + viv_tbl[src_p->r];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}else{
			if( nuki==-1 ){	
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							glay = riv_tbl[ (src_p->b + src_p->g+src_p->g + src_p->r)>>2 ];
							dest_p->b = btbl[ glay + viv_tbl[src_p->b] ];
							dest_p->g = gtbl[ glay + viv_tbl[src_p->g] ];
							dest_p->r = rtbl[ glay + viv_tbl[src_p->r] ];
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}else{			
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					if( INTERLESS_CHECK ){
						for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
							if( ((*(long *)src_p)&0x00ffffff) != nuki ){
								glay = riv_tbl[ (src_p->b + src_p->g+src_p->g + src_p->r)>>2 ];
								dest_p->b = btbl[ glay + viv_tbl[src_p->b] ];
								dest_p->g = gtbl[ glay + viv_tbl[src_p->g] ];
								dest_p->r = rtbl[ glay + viv_tbl[src_p->r] ];
							}
						}
					}else{
						dest_p+=w;
						src_p+=xinc*w;
					}
				}
			}
		}
	}

	return TRUE;
}




BOOL DRW_DrawBMP_FF_Moz( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *src_p2;
	int		xinc = XINC(dobj);
	int		sx, sy, moz_x, moz_y, moz=dobj->dnum+1;
	int		moz_w = moz-(w%moz)/2;
	int		moz_h = moz-(h%moz)/2;

	for( moz_y=moz_h, sy=max(0,-moz_h+moz/2), y=0; y < h ; y++, dest_p+=drev ){
		if( INTERLESS_CHECK ){
			src_p2 = src->buf + src->sx*(dobj->s1y+sy);

			for( moz_x=moz_w, sx=max(0,-moz_w+moz/2), x=0 ; x < w ; x++, dest_p++ ){
				src_p = src_p2 + dobj->s1x+sx;
				
				*dest_p = *src_p;
				moz_x++;
				if(moz_x>=moz){ sx += moz; sx = min(sx,src->sx-1); moz_x=0; }
			}
		}else{
			dest_p+=w;
		}
		moz_y++;
		if(moz_y>=moz){ sy += moz; sy = min(sy,src->sy-1); moz_y=0; }
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FF_Bom( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		x2,y2;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		bom  = dobj->dnum;
	short	*bom_tbl = BomdTabl[bom];
	char	count=0;
	char	*blnd_tbl = BlendTable[256-bom];
	char	*brev_tbl = BlendTable[bom];

	if( bom<=0 ) return DRW_DrawBMP_FF_Std(dobj, interless);

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
						x2 = bom_tbl[ count++ ]+x+dobj->dx;
						y2 = bom_tbl[ count ]+y+dobj->dy;

						if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
							dest_p = dest->buf + y2 * dest->sx + x2;
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							x2 = bom_tbl[ count++ ]+x+dobj->dx;
							y2 = bom_tbl[ count ]+y+dobj->dy;

							if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
								dest_p = dest->buf + y2 * dest->sx + x2;
								dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
								dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
								dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
							}
							count+=src_p->r+1;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc, count+=(y+x+src_p->r) ){
						x2 = bom_tbl[ count++ ]+x+dobj->dx;
						y2 = bom_tbl[ count ]+y+dobj->dy;

						if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
							dest_p = dest->buf + y2 * dest->sx + x2;
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							x2 = bom_tbl[ count++ ]+x+dobj->dx;
							y2 = bom_tbl[ count ]+y+dobj->dy;

							if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
								dest_p = dest->buf + y2 * dest->sx + x2;
								dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[src_p->b] ];
								dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[src_p->g] ];
								dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[src_p->r] ];
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}






BOOL DRW_DrawBMP_FF_Flt( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		rnd  = dobj->dnum;
	int		rnd2 = rnd/2;

	
	if( (r==128 && g==128 && b==128) ){

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){

					char	*tbl = BrightTable_TB[ 128 - rnd2 + RAND_TBL%(rnd2+1) ];
					dest_p->b = tbl[src_p->b];
					dest_p->g = tbl[src_p->g];
					dest_p->r = tbl[src_p->r];
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}


	}else{

		if( (r==g && g==b) ){
			char	*rbg_tbl = BrightTable_TB[r];

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){

						char	*tbl = BrightTable_TB[ 128 - rnd2 + RAND_TBL%(rnd2+1) ];
						dest_p->b = tbl[src_p->b];
						dest_p->g = tbl[src_p->g];
						dest_p->r = tbl[src_p->r];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						int	blt = 128 - rnd2 + RAND_TBL%(rnd2+1);
						dest_p->b = BrightTable_TB[ btbl[blt] ][src_p->b];
						dest_p->g = BrightTable_TB[ gtbl[blt] ][src_p->g];
						dest_p->r = BrightTable_TB[ rtbl[blt] ][src_p->r];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FF_Oly( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = BrightTable_TB[ dest_p->b ][ src_p->b ];
						dest_p->g = BrightTable_TB[ dest_p->g ][ src_p->g ];
						dest_p->r = BrightTable_TB[ dest_p->r ][ src_p->r ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = BrightTable_TB[ dest_p->b ][ src_p->b ];
							dest_p->g = BrightTable_TB[ dest_p->g ][ src_p->g ];
							dest_p->r = BrightTable_TB[ dest_p->r ][ src_p->r ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						dest_p->b = BrightTable_TB[ dest_p->b ][ btbl[src_p->b] ];
						dest_p->g = BrightTable_TB[ dest_p->g ][ gtbl[src_p->g] ];
						dest_p->r = BrightTable_TB[ dest_p->r ][ rtbl[src_p->r] ];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							dest_p->b = BrightTable_TB[ dest_p->b ][ btbl[src_p->b] ];
							dest_p->g = BrightTable_TB[ dest_p->g ][ gtbl[src_p->g] ];
							dest_p->r = BrightTable_TB[ dest_p->r ][ rtbl[src_p->r] ];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FF_Cml( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if(dest_p->b < src_p->b) dest_p->b = src_p->b;
						if(dest_p->g < src_p->g) dest_p->g = src_p->g;
						if(dest_p->r < src_p->r) dest_p->r = src_p->r;
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							if(dest_p->b < src_p->b) dest_p->b = src_p->b;
							if(dest_p->g < src_p->g) dest_p->g = src_p->g;
							if(dest_p->r < src_p->r) dest_p->r = src_p->r;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if(dest_p->b < btbl[src_p->b]) dest_p->b = btbl[src_p->b];
						if(dest_p->g < gtbl[src_p->g]) dest_p->g = gtbl[src_p->g];
						if(dest_p->r < rtbl[src_p->r]) dest_p->r = rtbl[src_p->r];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							if(dest_p->b < btbl[src_p->b]) dest_p->b = btbl[src_p->b];
							if(dest_p->g < gtbl[src_p->g]) dest_p->g = gtbl[src_p->g];
							if(dest_p->r < rtbl[src_p->r]) dest_p->r = rtbl[src_p->r];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FF_Cmd( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if(dest_p->b > src_p->b) dest_p->b = src_p->b;
						if(dest_p->g > src_p->g) dest_p->g = src_p->g;
						if(dest_p->r > src_p->r) dest_p->r = src_p->r;
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							if(dest_p->b > src_p->b) dest_p->b = src_p->b;
							if(dest_p->g > src_p->g) dest_p->g = src_p->g;
							if(dest_p->r > src_p->r) dest_p->r = src_p->r;
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if(dest_p->b > btbl[src_p->b]) dest_p->b = btbl[src_p->b];
						if(dest_p->g > gtbl[src_p->g]) dest_p->g = gtbl[src_p->g];
						if(dest_p->r > rtbl[src_p->r]) dest_p->r = rtbl[src_p->r];
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				if( INTERLESS_CHECK ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
						if( ((*(long *)src_p)&0x00ffffff) != nuki ){
							if(dest_p->b > btbl[src_p->b]) dest_p->b = btbl[src_p->b];
							if(dest_p->g > gtbl[src_p->g]) dest_p->g = gtbl[src_p->g];
							if(dest_p->r > rtbl[src_p->r]) dest_p->r = rtbl[src_p->r];
						}
					}
				}else{
					dest_p+=w;
					src_p+=xinc*w;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FF_Lcf( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
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
			if( INTERLESS_CHECK ){
				blnd_tbl = BlendTable[blnd];
				brev_tbl = BlendTable[brev];
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
					dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
					dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		for( ; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
			if( INTERLESS_CHECK ){
				CopyMemory( dest_p, src_p, w*sizeof(RGB24) );
			}
		}
		break;

	case DO:
		rate = (DispHeight+255)*rate/256;
		rate2 = STD_Limit( -(rate-255), 0, 255 );
		rate_max[0] = STD_Limit( rate-255, 0, h );
		rate_max[1] = STD_Limit( rate,     0, h );
		for( y=0; y < rate_max[0] ; y++, dest_p+=dest->sx, src_p+=src->sx ){
			if( INTERLESS_CHECK ){
				CopyMemory( dest_p, src_p, w*sizeof(RGB24) );
			}
		}
		blnd = 256 - rate2;
		brev =   0 + rate2;
		for( ; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev, blnd--, brev++ ){
			if( INTERLESS_CHECK ){
				blnd_tbl = BlendTable[blnd];
				brev_tbl = BlendTable[brev];
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
					dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
					dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;

	case RI:
		rate = (DispWidth+255)*(256-rate)/256;
		rate2 = STD_Limit( -(rate-255), 0, 255 );
		rate_max[0] = STD_Limit( rate-255, 0, w );
		rate_max[1] = STD_Limit( rate,     0, w );

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
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
					CopyMemory( dest_p, src_p, (w-x)*sizeof(RGB24) );
					dest_p += w-x;
					src_p  += w-x;
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;

	case LE:
		rate = (DispWidth+255)*rate/256;
		rate2 = STD_Limit( -(rate-255), 0, 255 );
		rate_max[0] = STD_Limit( rate-255, 0, w );
		rate_max[1] = STD_Limit( rate,     0, w );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				blnd = 256 - rate2;
				brev =   0 + rate2;
				x=0;
				if(rate_max[0]>0){
					CopyMemory( dest_p, src_p, rate_max[0]*sizeof(RGB24) );
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
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;

	case CE:
		rate = (DispWidth/2+127)*(256-rate)/256;
		rate2 = STD_Limit( -(rate-127), 0, 127 )*2;
		rate_max[0] = STD_Limit( rate-127, 0, w/2 );
		rate_max[1] = STD_Limit( rate,     0, w/2 );

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
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
					CopyMemory( dest_p, src_p, (w/2-x)*sizeof(RGB24) );
					dest_p += w/2-x;
					src_p  += w/2-x;
					x = w/2;
				}
	
				if( x>rate_max[1] ){
					CopyMemory( dest_p, src_p, (x-rate_max[1])*sizeof(RGB24) );
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
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;

	case OU:
		rate = (DispWidth/2+127)*rate/256;
		rate2 = STD_Limit( -(rate-127), 0, 127 )*2;
		rate_max[0] = STD_Limit( rate-127, 0, w );
		rate_max[1] = STD_Limit( rate,     0, w );

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				blnd = 256 - rate2;
				brev =   0 + rate2;
	
				x=0;
				if(rate_max[0]>0){
					CopyMemory( dest_p, src_p, rate_max[0]*sizeof(RGB24) );
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
					CopyMemory( dest_p, src_p, x*sizeof(RGB24) );
					dest_p += x;
					src_p  += x;
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FF_Lpp( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		nuki = dobj->nuki;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		rate = dobj->dnum;
	int		line = dobj->dparam-DRW_LPP2;
	int		rate_max[3];

	switch(line){
	case UP:
		rate = 46*(256-rate)/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, h );
		rate_max[1] = STD_Limit( rate*16,     0, h );

		dest_p+=rate_max[0]*dest->sx;
		src_p +=rate_max[0]*src->sx;

		for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK && (rate-(y>>4)) <= (y&0xf) ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *src_p;
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		for( ; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *src_p;
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;
	case DO:
		rate = 46*rate/256;
		rate_max[0] = min( h, rate*16-240 );
		rate_max[1] = min( h, rate*16 );
		for( y=0; y < rate_max[0] ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *src_p;
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		for( ; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK && (rate-(y>>4)) > (y&0xf) ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *src_p;
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;
	case RI:
		rate = 56*(256-rate)/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w );
		rate_max[1] = STD_Limit( rate*16,     0, w );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
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
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;
	case LE:
		rate = 56*rate/256;
		rate_max[0] = min( w, rate*16-240 );
		rate_max[1] = min( w, rate*16 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
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
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;
	case CE:
		rate = (20+16)*(256-rate)/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w/2 );
		rate_max[1] = STD_Limit( rate*16,     0, w/2 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
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
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;
	case OU:
		rate = (20+16)*rate/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w/2 );
		rate_max[1] = STD_Limit( rate*16,     0, w/2 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
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
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
		break;
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FF_Dia( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
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
			if( INTERLESS_CHECK ){
				y2 = abs( (y&mask)-harf ) + rate2;
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					x2 = abs( (x&mask)-harf );
					if( x2 < y2 ){
						dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ src_p->b ];
						dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ src_p->g ];
						dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ src_p->r ];
					}
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
	}else{
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				y2 = abs( (y&mask)-harf ) + rate2;
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					x2 = abs( (x&mask)-harf );
					if( x2 < y2 ){
						*dest_p = *src_p;
					}
				}
			}else{
				dest_p+=w;
				src_p+=xinc*w;
			}
		}
	}
	return TRUE;
}





BOOL DRW_DrawBMP_FF_Wav( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		yinc = YINC(dobj);
	int		wav = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		dy;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if(dobj->rparam&REV_W){
			for(y = 0; y < h; y++, dest_p+=drev ) {
				if( INTERLESS_CHECK ){
					dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
					if( dy+y>=h || dy+y<0 ){
						FillMemory( dest_p, w*3, 0 );
						dest_p+=w;
					}else{
						dy = (dy+y+h)%h * yinc;
						src_p = src->buf + DRW_GetStartPointerEx( dobj, 0, dy );
						for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
							*dest_p = *src_p;
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}else{
			for(y = 0; y < h; y++, dest_p+=dest->sx ) {
				if( INTERLESS_CHECK ){
					dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
					if( dy+y>=h || dy+y<0 ){
						FillMemory( dest_p, w*3, 0 );
					}else{
						dy = ((dy+y+h)%h) * yinc;
						src_p = src->buf + DRW_GetStartPointerEx( dobj, 0, dy );
						CopyMemory( dest_p, src_p, w*3 );
					}
				}
			}
		}
	}else{
		if(r==g && g==b){
			char	*fade_tbl = BrightTable_TB[r];

			for(y = 0; y < h; y++, dest_p+=drev ) {
				if( INTERLESS_CHECK ){
					dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
					if( dy+y>=h || dy+y<0 ){
						FillMemory( dest_p, w*3, LIM((r-128)*2,0,255) );
						dest_p+=w;
					}else{
						dy = (dy+y+h)%h * yinc;
						src_p = src->buf + DRW_GetStartPointerEx( dobj, 0, dy );

						for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = fade_tbl[src_p->b];
							dest_p->g = fade_tbl[src_p->g];
							dest_p->r = fade_tbl[src_p->r];
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			for(y = 0; y < h; y++, dest_p+=drev ) {
				if( INTERLESS_CHECK ){
					dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
					if( dy+y>=h || dy+y<0 ){
						for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = (b-127)<<1;
							dest_p->g = (g-127)<<1;
							dest_p->r = (r-127)<<1;
						}


					}else{
						dy = (dy+y+h)%h * yinc;
						src_p = src->buf + DRW_GetStartPointerEx( dobj, 0, dy );
						for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = btbl[src_p->b];
							dest_p->g = gtbl[src_p->g];
							dest_p->r = rtbl[src_p->r];
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



BOOL DRW_DrawBMP_FF_Wbl( DRAW_OBJECT *dobj, int interless )
{
	int		x,y;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		yinc = YINC(dobj);
	int		wav = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		dy, dy_tbl[600];
	char	*bld_tbl = BlendTable[32];
	char	*rbl_tbl = BlendTable[256-32];

	
	if( MMX_Flag && (r==128 && g==128 && b==128) && dobj->rparam==0 ){
		DWORD	alpha1 = 0xffffffff;
		int		w2=w*3;
		int		w3=w2/8;
		int		wa=w2%8;
		char	*dest_p2, *src_p2;

		for( y=0; y < h ; y++ ){
			dy_tbl[y] = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10.0);
		}

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			if( INTERLESS_CHECK ){
				dy = dy_tbl[y];
				if( dy+y>=h || dy+y<0 ){
					FillMemory( dest_p, w*3, 0 );
					dest_p+=w;
				}else{
					dy = (dy+y+h)%h * yinc;
					src_p = src->buf + (dobj->s1x+dy+STY(dobj))*src->sx + dobj->s1x+STX(dobj);

					if(w3){
						_asm {
							mov			ecx, w3
							mov			eax, src_p	
							mov			ebx, dest_p	
							pxor		mm3, mm3	
							movd		mm4, alpha1	
							punpcklbw	mm4, mm3	

							loopx:

							
								movq		mm0, [eax]	
								movq		mm1, [ebx]	
								
								movq		mm6, mm0	
								movq		mm7, mm1	
								
								punpcklbw	mm0, mm3	
								punpcklbw	mm1, mm3	
								
								punpckhbw	mm6, mm3	
								punpckhbw	mm7, mm3	
								
								movq		mm5, mm1	
								pxor		mm5, mm4
								pmullw		mm1, mm1	
								pmullw		mm0, mm5	
								paddusw		mm0, mm1	

								movq		mm5, mm7	
								pxor		mm5, mm4
								pmullw		mm7, mm7	
								pmullw		mm6, mm5	
								paddusw		mm6, mm7	
								
								psrlw		mm0, 8		
								psrlw		mm6, 8		
								
								packuswb	mm0, mm6	
								movq		[ebx], mm0	

								add		eax,8			
								add		ebx,8			
							loop	loopx
							mov			src_p,  eax	
							mov			dest_p, ebx	
						}
					}
					dest_p2 = (char*)dest_p;
					src_p2  = (char*)src_p;
					for( x=0; x<wa ;x++, dest_p2++, src_p2++ ){	
						*dest_p2 = DimTable[*dest_p2][*src_p2];
					}
					dest_p = (RGB24*)dest_p2;
				}
			}else{
				dest_p+=w;
			}
		}
		_asm	emms
	}else{
		if( r==0x80 && g==0x80 && b==0x80 ){
			for(y = 0; y < h; y++, dest_p+=drev ) {
				if( INTERLESS_CHECK ){
					dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
					if( dy+y>=h || dy+y<0 ){
						FillMemory( dest_p, w*3, 0 );
						dest_p+=w;
					}else{

						dy = (dy+y+h)%h * yinc;
						src_p = src->buf + (dobj->s1x+dy+STY(dobj))*src->sx + dobj->s1x+STX(dobj);

						for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = DimTable[dest_p->b][src_p->b];
							dest_p->g = DimTable[dest_p->g][src_p->g];
							dest_p->r = DimTable[dest_p->r][src_p->r];
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}else{
			char	*rtbl = BrightTable_TB[r];
			char	*gtbl = BrightTable_TB[g];
			char	*btbl = BrightTable_TB[b];

			for(y = 0; y < h; y++, dest_p+=drev ) {
				if( INTERLESS_CHECK ){
					dy = (int)(sin(PAI / 128.0 * (y + cnt)) * wav/10);
					if( dy+y>=h || dy+y<0 ){
						FillMemory( dest_p, w*3, 0 );
						dest_p+=w;
					}else{
						dy = (dy+y+h)%h * yinc;
						src_p = src->buf + (dobj->s1x+dy+STY(dobj))*src->sx + dobj->s1x+STX(dobj);

						for( x=0 ; x<w ; x++, dest_p++, src_p+=xinc ){
							dest_p->b = DimTable[dest_p->b][btbl[src_p->b]];
							dest_p->g = DimTable[dest_p->g][gtbl[src_p->g]];
							dest_p->r = DimTable[dest_p->r][rtbl[src_p->r]];
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



static BOOL DRW_DrawZOOM_FF_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, int interless );

BOOL DRW_DrawBMP_FF_Sui( DRAW_OBJECT *dobj, int interless )
{
	BMP_F		*dest = (BMP_F *)dobj->dest;
	BMP_F		*src = (BMP_F *)dobj->src1;
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
		return DRW_DrawBMP_FF_Std( dobj, interless );
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

			dobj2 = DRW_SetDrawObject(	2,dest, xx*w2,    yy*h2,    w2,       h2,
										2,src,  xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_NML, 0,0, dobj->rparam );
			DRW_DrawZOOM_FF_Std( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0, interless );
		}
	}

	return TRUE;
}




static BOOL DRW_DrawZOOM_FF_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, int interless );
static BOOL DRW_DrawZOOM_FF_Dim( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, int interless );

BOOL DRW_DrawBMP_FF_Sbl( DRAW_OBJECT *dobj, int interless )
{
	BMP_F		*dest = (BMP_F *)dobj->dest;
	BMP_F		*src = (BMP_F *)dobj->src1;
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

			dobj2 = DRW_SetDrawObject(	2,dest, dobj->dx+xx*w2,     dobj->dy+yy*h2,    w2,       h2,
										2,src,  dobj->s1x+xx*w2+x1, dobj->s1y+yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_BLD(64), 0,0, dobj->rparam );

			DRW_DrawZOOM_FF_Bld( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0, interless );
		}
	}

	return TRUE;
}

BOOL DRW_DrawBMP_FF_Wap( DRAW_OBJECT *dobj, int interless )
{
	DRAW_OBJECT	dobj2;
	BMP_F		*dest = (BMP_F *)dobj->dest;
	BMP_F		*src = (BMP_F *)dobj->src1;
	float	xxx[36];
	float	yyy[36];
	int	xx[6];
	int	yy[6];
	int	x,y,sw,sh;
	int	i,j;

	int		wap = 255-(dobj->dnum&0x00ff);
	float	r = COS((dobj->dnum&0xff00)>>8)   /  (64.0f+wap);
	float	ry= COS((dobj->dnum&0xff00)>>8)*3 / ((64.0f+wap)*4);
	float	rmax = 4096/(64.0f+wap);
	x = dobj->dx2;
	y = dobj->dy2;
	sw = dobj->s1x;
	sh = dobj->s1y;
	xx[0] = -sw;
	xx[1] = x*1/5;
	xx[2] = x*4/5;
	xx[3] = x+(800-x)/5;
	xx[4] = x+(800-x)*4/5;
	xx[5] = 800+sw;

	yy[0] = -sh;
	yy[1] = y*1/5;
	yy[2] = y*4/5;
	yy[3] = y+(600-y)/5;
	yy[4] = y+(600-y)*4/5;
	yy[5] = 600+sh;

	switch(dobj->dparam2){
	default:
	case 0:
		for(i=0;i<5;i++){
			float	y2,h2;
			switch(i){

				case 0: y2=sh*r/rmax; h2=ry-sh*r/rmax;  break;
				case 1: y2=ry;   h2=-ry*2; break;
				case 2: y2=-ry;  h2=ry*2;  break;
				case 3: y2=ry;   h2=-ry*2; break;
				case 4: y2=-ry;  h2=ry-sh*r/rmax;  break;
			}
			for(j=0;j<5;j++){
				float	x2,w2;
				switch(j){

					case 0: x2=sw*r/rmax;   w2 = r-sw*r/rmax;   break;
					case 1: x2=r;   w2=-r*2; break;
					case 2: x2=-r;  w2=r*2;  break;
					case 3: x2=r;   w2=-r*2; break;
					case 4: x2=-r;  w2=r-sw*r/rmax;   break;
				}
				ZeroMemory( &dobj2, sizeof(DRAW_OBJECT) );
				dobj2 = DRW_SetDrawObject(	2,dest, (int)(xx[j]+x2), (int)(yy[i]+y2), (int)(xx[j+1]-xx[j]+w2), (int)(yy[i+1]-yy[i]+h2),
											2,src,  (int)(xx[j]+sw), (int)(yy[i]+sh), (int)(xx[j+1]-xx[j]),    (int)(yy[i+1]-yy[i]),
											0, NULL, 0, 0, 0, 0,
											dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_BLD(dobj->dnum2), 0,0, dobj->rparam );
				DRW_DrawZOOM_FF( dobj2, interless );
			}
		}
		break;
	case 1:
		xxx[0*6+0] = 3*sw*r/rmax/3;	yyy[0*6+0] = 3*sh*r/rmax/3;
		xxx[0*6+1] = r*2;	yyy[0*6+1] = 2*sh*r/rmax/3;
		xxx[0*6+2] = r;		yyy[0*6+2] = 1*sh*r/rmax/3;
		xxx[0*6+3] = -r;	yyy[0*6+3] = 1*sh*r/rmax/3;
		xxx[0*6+4] = -r*2;	yyy[0*6+4] = 2*sh*r/rmax/3;
		xxx[0*6+5] = -3*sw*r/rmax/3;	yyy[0*6+5] = 3*sh*r/rmax/3;

		xxx[1*6+0] = 2*sw*r/rmax/3;	yyy[1*6+0] = r*2;
		xxx[1*6+1] = r;		yyy[1*6+1] = r;
		xxx[1*6+2] = r/2;	yyy[1*6+2] = r/2;
		xxx[1*6+3] = -r/2;	yyy[1*6+3] = r/2;
		xxx[1*6+4] = -r;	yyy[1*6+4] = r;
		xxx[1*6+5] = -2*sw*r/rmax/3;	yyy[1*6+5] = r*2;

		xxx[2*6+0] = 1*sw*r/rmax/3;		yyy[2*6+0] = r;
		xxx[2*6+1] = r/2;	yyy[2*6+1] = r/2;
		xxx[2*6+2] = -r;	yyy[2*6+2] = -r;
		xxx[2*6+3] = r;		yyy[2*6+3] = -r;
		xxx[2*6+4] = -r/2;	yyy[2*6+4] = r/2;
		xxx[2*6+5] = -1*sw*r/rmax/3;	yyy[2*6+5] = r;

		xxx[3*6+0] = 1*sw*r/rmax/3;		yyy[3*6+0] = -r;
		xxx[3*6+1] = r/2;	yyy[3*6+1] = -r/2;
		xxx[3*6+2] = -r;	yyy[3*6+2] = r;
		xxx[3*6+3] = r;		yyy[3*6+3] = r;
		xxx[3*6+4] = -r/2;	yyy[3*6+4] = -r/2;
		xxx[3*6+5] = -1*sw*r/rmax/3;	yyy[3*6+5] = -r;

		xxx[4*6+0] = 2*sw*r/rmax/3;	yyy[4*6+0] = -r*2;
		xxx[4*6+1] = r;		yyy[4*6+1] = -r;
		xxx[4*6+2] = r/2;	yyy[4*6+2] = -r/2;
		xxx[4*6+3] = -r/2;	yyy[4*6+3] = -r/2;
		xxx[4*6+4] = -r;	yyy[4*6+4] = -r;
		xxx[4*6+5] = -2*sw*r/rmax/3;	yyy[4*6+5] = -r*2;

		xxx[5*6+0] = 3*sw*r/rmax/3;	yyy[5*6+0] = -3*sh*r/rmax/3;
		xxx[5*6+1] = r*2;	yyy[5*6+1] = -2*sh*r/rmax/3;
		xxx[5*6+2] = r*1;	yyy[5*6+2] = -1*sh*r/rmax/3;
		xxx[5*6+3] = -r*1;	yyy[5*6+3] = -1*sh*r/rmax/3;
		xxx[5*6+4] = -r*2;	yyy[5*6+4] = -2*sh*r/rmax/3;
		xxx[5*6+5] = -3*sw*r/rmax/3;	yyy[5*6+5] = -3*sh*r/rmax/3;

		for(i=0;i<5;i++){
			for(j=0;j<5;j++){
				dobj2=*dobj;
				dobj2.dparam = DRW_BLD2;
				dobj2.dnum   = dobj->dnum2;
				DRW_SetDrawObjectPoly( &dobj2,
								xx[j]+(int)xxx[ i   *6+j],   yy[i  ]+(int)yyy[ i   *6+j],   xx[j+1]+(int)xxx[ i   *6+j+1], yy[i  ]+(int)yyy[ i   *6+j+1],
								xx[j]+(int)xxx[(i+1)*6+j],   yy[i+1]+(int)yyy[(i+1)*6+j],   xx[j+1]+(int)xxx[(i+1)*6+j+1], yy[i+1]+(int)yyy[(i+1)*6+j+1],
								xx[j]+sw, yy[i]+sh,   xx[j+1]+sw, yy[i]+sh,   xx[j]+sw, yy[i+1]+sh,   xx[j+1]+sw, yy[i+1]+sh );
				DRW_DrawPOLY4_FF( dobj2, 0 );
			}
		}
		break;
	}
	return TRUE;
}





BOOL DRW_DrawBMP_FF_Lst( DRAW_OBJECT *dobj, int interless )
{
	int		x, y, xx;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	int		lst = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);

	lst = lst*w/640;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){	
			if(dobj->rparam&REV_W){
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=yinc ){
					if( INTERLESS_CHECK ){
						xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
						if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
							sss = src_p+xx;
							for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
								*dest_p = *sss;
							}
						}else{
							dest_p+=w;
						}
					}else{
						dest_p+=w;
					}
				}
			}else{
				for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=yinc ){
					if( INTERLESS_CHECK ){
						xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
						if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
							sss = src_p+xx;
							CopyMemory( dest_p, sss, w*3 );	
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=yinc ){
				if( INTERLESS_CHECK ){
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
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=yinc ){
				if( INTERLESS_CHECK ){
					xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
					if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
						sss = src_p+xx;
						for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
							dest_p->b = btbl[ src_p->b ];
							dest_p->g = gtbl[ src_p->g ];
							dest_p->r = rtbl[ src_p->r ];
						}
					}else{
						dest_p+=w;
					}
				}else{
					dest_p+=w;
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=yinc ){
				if( INTERLESS_CHECK ){
					xx = lst*SIN( y*480/h +cnt)/4096 * xinc;
					if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
						sss = src_p+xx;
						for( x=0 ; x < w ; x++, dest_p++, sss+=xinc ){
							if(*(long*)sss!=nuki){
								dest_p->b = btbl[ src_p->b ];
								dest_p->g = gtbl[ src_p->g ];
								dest_p->r = rtbl[ src_p->r ];
							}
						}
					}else{
						dest_p+=w;
					}
				}else{
					dest_p+=w;
				}
			}
		}
	}

	return TRUE;
}



BOOL DRW_DrawBMP_FF_Lbl( DRAW_OBJECT *dobj, int interless )
{
	int		x, y, xx;
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	int		lst = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	char	*blnd_tbl = BlendTable[256-lst];
	char	*brev_tbl = BlendTable[lst];

	if( lst==0   )	return DRW_DrawBMP_FF_Std( dobj, interless );	
	if( lst==255 )	return 1;							

	lst = lst*w/640;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				if( INTERLESS_CHECK ){
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
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				if( INTERLESS_CHECK ){
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
				}else{
					dest_p+=w;
				}
			}
		}
	}

	return TRUE;
}





BOOL DRW_DrawBMP_FF( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_F	*src  = (BMP_F *)dobj.src1;

	
	if( dest->buf==NULL ) {	DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FF]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FF]\n"); return FALSE; }

	
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

		case DRW_NML:	DRW_DrawBMP_FF_Std( &dobj, interless );	break;	
		case DRW_BLD2:	DRW_DrawBMP_FF_Bld( &dobj, interless );	break;	
		case DRW_ADD:	DRW_DrawBMP_FF_Add( &dobj, interless );	break;	
		case DRW_OOI:	DRW_DrawBMP_FF_Ooi( &dobj, interless );	break;	
		case DRW_SUB:	DRW_DrawBMP_FF_Sub( &dobj, interless );	break;	
		case DRW_DIM:	DRW_DrawBMP_FF_Dim( &dobj, interless );	break;	
		case DRW_MUL:	DRW_DrawBMP_FF_Mul( &dobj, interless );	break;	
		case DRW_AMI2:	DRW_DrawBMP_FF_Ami( &dobj, interless );	break;	
		case DRW_NIS2:	DRW_DrawBMP_FF_Nis( &dobj, interless );	break;	
		case DRW_NEG:	DRW_DrawBMP_FF_Neg(	&dobj, interless );	break;	
		case DRW_VIV2:	DRW_DrawBMP_FF_Viv( &dobj, interless );	break;	
		case DRW_MOZ2:	DRW_DrawBMP_FF_Moz(	&dobj, interless );	break;	
		case DRW_BOM2:	DRW_DrawBMP_FF_Bom(	&dobj, interless );	break;	
		case DRW_FLT2:	DRW_DrawBMP_FF_Flt( &dobj, interless );	break;	



		case DRW_RP22:	DRW_DrawBMP_FF_Rp2( &dobj );	break;	

		case DRW_RPL2:	DRW_DrawBMP_FF_RPL( &dobj, interless );	break;	
		case DRW_SUI2:	DRW_DrawBMP_FF_Sui(	&dobj, interless );	break;	
		case DRW_SBL2:	DRW_DrawBMP_FF_Sbl(	&dobj, interless );	break;	
		case DRW_WAV2:	DRW_DrawBMP_FF_Wav(	&dobj, interless );	break;	
		case DRW_WBL2:	DRW_DrawBMP_FF_Wbl(	&dobj, interless );	break;	
		case DRW_LST2:	DRW_DrawBMP_FF_Lst(	&dobj, interless );	break;	
		case DRW_LBL2:	DRW_DrawBMP_FF_Lbl(	&dobj, interless );	break;	

		case DRW_WAP2:	DRW_DrawBMP_FF_Wap(	&dobj, interless );	break;	

		case DRW_OLY:	DRW_DrawBMP_FF_Oly(	&dobj, interless );	break;	
		case DRW_CML:	DRW_DrawBMP_FF_Cml(	&dobj, interless );	break;	
		case DRW_CMD:	DRW_DrawBMP_FF_Cmd(	&dobj, interless );	break;	

		case DRW_LCF2:		case DRW_LCF2+1:	case DRW_LCF2+2:	
		case DRW_LCF2+3:	case DRW_LCF2+4:	case DRW_LCF2+5:
			DRW_DrawBMP_FF_Lcf(	&dobj, interless );
			break;

		case DRW_LPP2:		case DRW_LPP2+1:	case DRW_LPP2+2:
		case DRW_LPP2+3:	case DRW_LPP2+4:	case DRW_LPP2+5:	
			DRW_DrawBMP_FF_Lpp( &dobj, interless );
			break;

		case DRW_DIA2:	case DRW_DIA2+1:	case DRW_DIA2+2:
		case DRW_DIO2:	case DRW_DIO2+1:	case DRW_DIO2+2:
			DRW_DrawBMP_FF_Dia( &dobj, interless );
			break;
	}
	return ret;
}










BOOL DRW_DrawBMP_FB_Std( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);

	
	if( dobj->nuki==-1 ){	

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					*dest_p = *(RGB24*)&spal[ *src_p ];
				}
			}





	}else if( dobj->nuki==-2 ){	
		char	*alp_tbl, *al2_tbl;
		if(src->alp){
			char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
					if( *alp_p != 0 ){
						if( *alp_p == 255 ){
							*dest_p =  *(RGB24*)&spal[ *src_p ];
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
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( spal[ *src_p ].a != 0 ){
						if( spal[ *src_p ].a == 255 ){
							*dest_p =  *(RGB24*)&spal[ *src_p ];
						}else{
							alp_tbl = BlendTable[ spal[ *src_p ].a ];
							al2_tbl = BlendTable[ 255-spal[ *src_p ].a ];
							dest_p->b = al2_tbl[dest_p->b] + alp_tbl[spal[*src_p].b];
							dest_p->g = al2_tbl[dest_p->g] + alp_tbl[spal[*src_p].g];
							dest_p->r = al2_tbl[dest_p->r] + alp_tbl[spal[*src_p].r];
						}
					}
				}
			}
		}
	}else if( dobj->nuki==-4 ){	
		if(src->alp){
			char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
					if( *alp_p > 128  ){
						*dest_p =  *(RGB24*)&spal[ *src_p ];
					}
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( spal[ *src_p ].a > 128  ){
						*dest_p =  *(RGB24*)&spal[ *src_p ];
					}
				}
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if( *src_p != dobj->nuki ){
					*dest_p =  *(RGB24*)&spal[ *src_p ];
				}
			}
		}
	}

	return TRUE;
}




BOOL DRW_DrawBMP_FB_Bld( DRAW_OBJECT *dobj, BMP_BT *src, RGB32 *spal2 )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	int		blnd = dobj->dnum;
	int		brev = 256-blnd;
	char	*blend_tbl=BlendTable[brev];
	char	*blen2_tbl=BlendTable[blnd];

	if( blnd==BLD_0P   )	return 1;								
	if( blnd==BLD_100P )	return DRW_DrawBMP_FB_Std( dobj, src );	

	
	if( dobj->nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = blend_tbl[ dest_p->b ]+ spal[*src_p].b;
				dest_p->g = blend_tbl[ dest_p->g ]+ spal[*src_p].g;
				dest_p->r = blend_tbl[ dest_p->r ]+ spal[*src_p].r;
			}
		}
	}else if( dobj->nuki==-2 ){	
		char	*alp_tbl, *al2_tbl;
		if( src->alp ){
			char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );

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
					if( spal2[ *src_p ].a != 0 ){
						if( spal2[ *src_p ].a == 255 ){
							dest_p->b = blend_tbl[ dest_p->b ]+ spal[*src_p].b;
							dest_p->g = blend_tbl[ dest_p->g ]+ spal[*src_p].g;
							dest_p->r = blend_tbl[ dest_p->r ]+ spal[*src_p].r;
						}else{
							alp_tbl = BlendTable[ blen2_tbl[spal[ *src_p ].a] ];
							al2_tbl = BlendTable[ 255-blen2_tbl[spal[ *src_p ].a] ];
							dest_p->b = al2_tbl[dest_p->b] + alp_tbl[spal2[*src_p].b];
							dest_p->g = al2_tbl[dest_p->g] + alp_tbl[spal2[*src_p].g];
							dest_p->r = al2_tbl[dest_p->r] + alp_tbl[spal2[*src_p].r];
						}
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

	return TRUE;
}



BOOL DRW_DrawBMP_FB_Add( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	char	*add_tbl = AddTable;

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = add_tbl[ dest_p->b + spal[*src_p].b ];
				dest_p->g = add_tbl[ dest_p->g + spal[*src_p].g ];
				dest_p->r = add_tbl[ dest_p->r + spal[*src_p].r ];
			}
		}
	}else if( dobj->nuki==-2 || dobj->nuki==-4 ){	
		if(src->alp){
			char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );
			char	*alp_tbl;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
					if( *alp_p != 0 ){
						if( *alp_p == 255 ){
							dest_p->b = add_tbl[ dest_p->b + spal[*src_p].b ];
							dest_p->g = add_tbl[ dest_p->g + spal[*src_p].g ];
							dest_p->r = add_tbl[ dest_p->r + spal[*src_p].r ];
	
						}else{
							alp_tbl = BlendTable[*alp_p];
							dest_p->b = add_tbl[ dest_p->b + alp_tbl[spal[*src_p].b] ];
							dest_p->g = add_tbl[ dest_p->g + alp_tbl[spal[*src_p].g] ];
							dest_p->r = add_tbl[ dest_p->r + alp_tbl[spal[*src_p].r] ];
	
	
	
						}
					}
				}
			}
		}else{
			char	*alp_tbl;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if(  spal[*src_p].a != 0 ){
						if( spal[*src_p].a == 255 ){
							dest_p->b = add_tbl[ dest_p->b + spal[*src_p].b ];
							dest_p->g = add_tbl[ dest_p->g + spal[*src_p].g ];
							dest_p->r = add_tbl[ dest_p->r + spal[*src_p].r ];
						}else{
							alp_tbl = BlendTable[spal[*src_p].a];
							dest_p->b = add_tbl[ dest_p->b + alp_tbl[spal[*src_p].b] ];
							dest_p->g = add_tbl[ dest_p->g + alp_tbl[spal[*src_p].g] ];
							dest_p->r = add_tbl[ dest_p->r + alp_tbl[spal[*src_p].r] ];
						}
					}
				}
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
	return TRUE;
}



BOOL DRW_DrawBMP_FB_Ooi( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = OoiTable[ dest_p->b ][ spal[*src_p].b ];
				dest_p->g = OoiTable[ dest_p->g ][ spal[*src_p].g ];
				dest_p->r = OoiTable[ dest_p->r ][ spal[*src_p].r ];
			}
		}
	}else if(nuki==-2 || nuki==-4){			
		char	*alp_tbl;

		if(src->alp){
			char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
					if( *alp_p != 0 ){
						if( *alp_p == 255 ){
							dest_p->b = OoiTable[ dest_p->b ][ spal[*src_p].b ];
							dest_p->g = OoiTable[ dest_p->g ][ spal[*src_p].g ];
							dest_p->r = OoiTable[ dest_p->r ][ spal[*src_p].r ];
						}else{
							alp_tbl = BlendTable[*alp_p];
							dest_p->b = OoiTable[ dest_p->b ][ alp_tbl[spal[*src_p].b] ];
							dest_p->g = OoiTable[ dest_p->g ][ alp_tbl[spal[*src_p].g] ];
							dest_p->r = OoiTable[ dest_p->r ][ alp_tbl[spal[*src_p].r] ];
						}
					}
				}
			}
		}else{
			char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if( spal[*src_p].a != 0 ){
						if( spal[*src_p].a == 255 ){
							dest_p->b = OoiTable[ dest_p->b ][ spal[*src_p].b ];
							dest_p->g = OoiTable[ dest_p->g ][ spal[*src_p].g ];
							dest_p->r = OoiTable[ dest_p->r ][ spal[*src_p].r ];
						}else{
							alp_tbl = BlendTable[spal[*src_p].a];
							dest_p->b = OoiTable[ dest_p->b ][ alp_tbl[spal[*src_p].b] ];
							dest_p->g = OoiTable[ dest_p->g ][ alp_tbl[spal[*src_p].g] ];
							dest_p->r = OoiTable[ dest_p->r ][ alp_tbl[spal[*src_p].r] ];
						}
					}
				}
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



BOOL DRW_DrawBMP_FB_Sub( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	char	*sub_tbl = SubTable+255;

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = sub_tbl[ dest_p->b - spal[*src_p].b ];
				dest_p->g = sub_tbl[ dest_p->g - spal[*src_p].g ];
				dest_p->r = sub_tbl[ dest_p->r - spal[*src_p].r ];
			}
		}
	}else if(nuki==-2 || nuki==-4){
			char	*alp_tbl;
		if(src->alp){
			char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
					if( *alp_p != 0 ){
						if( *alp_p == 255 ){
							dest_p->b = sub_tbl[ dest_p->b - spal[*src_p].b ];
							dest_p->g = sub_tbl[ dest_p->g - spal[*src_p].g ];
							dest_p->r = sub_tbl[ dest_p->r - spal[*src_p].r ];
						}else{
							alp_tbl = BlendTable[*alp_p];
							dest_p->b = sub_tbl[ dest_p->b - alp_tbl[spal[*src_p].b] ];
							dest_p->g = sub_tbl[ dest_p->g - alp_tbl[spal[*src_p].g] ];
							dest_p->r = sub_tbl[ dest_p->r - alp_tbl[spal[*src_p].r] ];
						}
					}
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if(  spal[*src_p].a != 0 ){
						if( spal[*src_p].a == 255 ){
							dest_p->b = sub_tbl[ dest_p->b - spal[*src_p].b ];
							dest_p->g = sub_tbl[ dest_p->g - spal[*src_p].g ];
							dest_p->r = sub_tbl[ dest_p->r - spal[*src_p].r ];
						}else{
							alp_tbl = BlendTable[spal[*src_p].a];
							dest_p->b = sub_tbl[ dest_p->b - alp_tbl[spal[*src_p].b] ];
							dest_p->g = sub_tbl[ dest_p->g - alp_tbl[spal[*src_p].g] ];
							dest_p->r = sub_tbl[ dest_p->r - alp_tbl[spal[*src_p].r] ];
						}
					}
				}
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if(*src_p!=nuki){
					dest_p->b = sub_tbl[ dest_p->b - spal[*src_p].b ];
					dest_p->g = sub_tbl[ dest_p->g - spal[*src_p].g ];
					dest_p->r = sub_tbl[ dest_p->r - spal[*src_p].r ];
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FB_Mul( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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
	}else if(nuki==-2 || nuki==-4){
		char	*alp_tbl;
		if(src->alp){
			char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
					if( *alp_p != 0 ){
						if( *alp_p == 255 ){
							dest_p->b = BrightTable_TB[ spal[*src_p].b ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ spal[*src_p].g ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ spal[*src_p].r ][ dest_p->r ];
						}else{
							alp_tbl = BlendTable[*alp_p];
							dest_p->b = BrightTable_TB[ BrightTable_TB[*alp_p][spal[*src_p].b] ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ BrightTable_TB[*alp_p][spal[*src_p].g] ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ BrightTable_TB[*alp_p][spal[*src_p].r] ][ dest_p->r ];
						}
					}
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
					if(  spal[*src_p].a != 0 ){
						if( spal[*src_p].a == 255 ){
							dest_p->b = BrightTable_TB[ spal[*src_p].b ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ spal[*src_p].g ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ spal[*src_p].r ][ dest_p->r ];
						}else{
							alp_tbl = BlendTable[ spal[*src_p].a ];
							dest_p->b = BrightTable_TB[ BrightTable_TB[spal[*src_p].a][spal[*src_p].b] ][ alp_tbl[dest_p->b] ];
							dest_p->g = BrightTable_TB[ BrightTable_TB[spal[*src_p].a][spal[*src_p].g] ][ alp_tbl[dest_p->g] ];
							dest_p->r = BrightTable_TB[ BrightTable_TB[spal[*src_p].a][spal[*src_p].r] ][ alp_tbl[dest_p->r] ];
						}
					}
				}
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



BOOL DRW_DrawBMP_FB_Ami( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	int		mesh= dobj->dnum;
	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=0   )	return 1;								
	if( mesh>=256 )	return DRW_DrawBMP_FB_Std( dobj, src );	

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			linemask2 = linemask[y & 3];
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if( linemask2[x & 3] )
					*dest_p = *(RGB24*)&spal[*src_p];
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			linemask2 = linemask[y & 3];
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if( *src_p!=nuki && linemask2[x & 3] )
					*dest_p = *(RGB24*)&spal[*src_p];
			}
		}
	}
	return TRUE;
}
BOOL DRW_DrawBMP_FB_Nis( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);

	int		mesh = dobj->dnum, m=mesh>>2;

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				if( (RAND_TBL&0xff) < (unsigned)mesh ){
					*dest_p = *(RGB24*)&spal[*src_p];
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FB_Bom( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		x2,y2;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	int		bom = dobj->dnum;
	short	*bom_tbl = BomdTabl[bom];
	char	count=0;
	char	*blnd_tbl = BlendTable[256-bom];
	char	*brev_tbl = BlendTable[bom];

	if( bom<=0 )	return DRW_DrawBMP_FB_Std( dobj, src );	

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, src_p+=srev ){
			for( x=0 ; x < w ; x++, src_p+=xinc, count+=(x+y+*src_p) ){
				x2 = bom_tbl[ count++ ]+x+dobj->dx;
				y2 = bom_tbl[ count ]+y+dobj->dy;

				if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
					dest_p = dest->buf + y2 * dest->sx + x2;
					*dest_p = *(RGB24*)&spal[*src_p];
				}
			}
		}
	}else{			
		for( y=0; y < h ; y++, src_p+=srev ){
			for( x=0 ; x < w ; x++, src_p+=xinc, count+=*src_p ){
				if( *src_p!=nuki ){
					x2 = bom_tbl[ count++ ]+x+dobj->dx;
					y2 = bom_tbl[ count ]+y+dobj->dy;

					if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
						dest_p = dest->buf + y2 * dest->sx + x2;
						*dest_p = *(RGB24*)&spal[*src_p];
					}
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FB_Gnm( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		x2;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);
	int		gnm = LIM(dobj->dnum,0,256);
	int		work, work2;
	char	*sub_tbl = SubTable+255;

	if( gnm>=256 ) return 1;
	if( gnm<=0   ) return DRW_DrawBMP_FB_Add( dobj, src );	

	
	for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
		
		work=0;
		for( x2=0 ; x2 < gnm ; x2++ ){
			work += sub_tbl[ spal[*(src_p+x2)].b - gnm ];
		}

		for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
			work2=work/(gnm<<1);
			dest_p->b = AddTable[ dest_p->b + work2 ];
			dest_p->g = AddTable[ dest_p->g + work2 ];
			dest_p->r = AddTable[ dest_p->r + work2 ];

			if(x-gnm>=0){
				work -= sub_tbl[ spal[*(src_p-gnm)].b - gnm ];
			}
			if(w>x+gnm ){
				work += sub_tbl[ spal[*(src_p+gnm)].b - gnm ];
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FB_Flt( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;

	int		xinc = XINC(dobj);
	int		rnd  = dobj->dnum;
	int		rnd2 = rnd/2;


	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){

				char	*tbl = BrightTable_TB[ 128 - rnd2 + RAND_TBL%(rnd2+1) ];
				
				dest_p->b = tbl[spal[*src_p].b];
				dest_p->g = tbl[spal[*src_p].g];
				dest_p->r = tbl[spal[*src_p].r];
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_FB_Oly( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				dest_p->b = BrightTable_TB[ dest_p->b ][ spal[*src_p].b ];
				dest_p->g = BrightTable_TB[ dest_p->g ][ spal[*src_p].g ];
				dest_p->r = BrightTable_TB[ dest_p->r ][ spal[*src_p].r ];
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if(*src_p!=nuki){
					dest_p->b = BrightTable_TB[ dest_p->b ][ spal[*src_p].b ];
					dest_p->g = BrightTable_TB[ dest_p->g ][ spal[*src_p].g ];
					dest_p->r = BrightTable_TB[ dest_p->r ][ spal[*src_p].r ];
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FB_Cml( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if(dest_p->b < spal[*src_p].b) dest_p->b = spal[*src_p].b;
				if(dest_p->g < spal[*src_p].g) dest_p->g = spal[*src_p].g;
				if(dest_p->r < spal[*src_p].r) dest_p->r = spal[*src_p].r;
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if(*src_p!=nuki){
					if(dest_p->b < spal[*src_p].b) dest_p->b = spal[*src_p].b;
					if(dest_p->g < spal[*src_p].g) dest_p->g = spal[*src_p].g;
					if(dest_p->r < spal[*src_p].r) dest_p->r = spal[*src_p].r;
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_FB_Cmd( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dobj->dw;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	RGB32	*spal = src->pal;
	int		xinc = XINC(dobj);

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if(dest_p->b > spal[*src_p].b) dest_p->b = spal[*src_p].b;
				if(dest_p->g > spal[*src_p].g) dest_p->g = spal[*src_p].g;
				if(dest_p->r > spal[*src_p].r) dest_p->r = spal[*src_p].r;
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc ){
				if(*src_p!=nuki){
					if(dest_p->b > spal[*src_p].b) dest_p->b = spal[*src_p].b;
					if(dest_p->g > spal[*src_p].g) dest_p->g = spal[*src_p].g;
					if(dest_p->r > spal[*src_p].r) dest_p->r = spal[*src_p].r;
				}
			}
		}
	}
	return TRUE;
}




static BOOL DRW_DrawZOOM_FB_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src );

BOOL DRW_DrawBMP_FB_Sui( DRAW_OBJECT *dobj, BMP_BT *src )
{
	BMP_F	*dest = (BMP_F *)dobj->dest;
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
		return DRW_DrawBMP_FB_Std( dobj, src );
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

			dobj2 = DRW_SetDrawObject(	2,dest, xx*w2,    yy*h2,    w2,       h2,
										0,src,  xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_NML, 0,0, dobj->rparam );
			DRW_DrawZOOM_FB_Std( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0, src );
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_FB_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src );

BOOL DRW_DrawBMP_FB_Sbl( DRAW_OBJECT *dobj, BMP_BT *src )
{
	BMP_F	*dest = (BMP_F *)dobj->dest;
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

			dobj2 = DRW_SetDrawObject(	2,dest, xx*w2,    yy*h2,    w2,       h2,
										0,src,  xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_BLD(128), 0,0, dobj->rparam );

		}
	}
	return TRUE;
}

BOOL DRW_DrawBMP_FB_Rp2( DRAW_OBJECT *dobj, BMP_BT *src )
{
	int		x,y;
	int		w = dobj->dw/2-1;
	int		h = dobj->dh/2-1;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx*2-(dobj->dw/2-2)*2;
	int		prev = 400-(dobj->dw/2-2);
	RGB24	*dest_p = dest->buf + (dobj->dy+2)*dest->sx + (dobj->dx+2);
	char	*src_p;
	short	*depth_p = &Repple.depth[Repple.nwf1][1][1];
	RGB32	*spal = src->pal;

	Repple.interless = !Repple.interless;


	
	for(y = 1 ; y<h ; y++, dest_p+=drev, depth_p+=prev ){
		if( (Repple.interless+y)%2 ){
			for(x = 1 ; x<w ; x++, dest_p+=2, depth_p++ ){
				src_p = src->buf+ src->sx*Repple.addy[ Repple.refraction[ *(depth_p-400)+256 ][ *(depth_p+400)+256 ] + y+128 ]
										+ Repple.addx[ Repple.refraction[ *(depth_p-1)+256 ][ *(depth_p+1)+256 ] + x+128 ];

				*dest_p              = *(RGB24*)&spal[*src_p];
				*(dest_p+1)          = *(RGB24*)&spal[*(src_p+1)];
				*(dest_p+dest->sx)   = *(RGB24*)&spal[*(src_p+src->sx)];
				*(dest_p+dest->sx+1) = *(RGB24*)&spal[*(src_p+src->sx+1)];
			}
		}else{
			dest_p += (w-1)<<1;
			depth_p+= w-1;
		}
	}


	return 1;
}








BOOL DRW_DrawBMP_FB( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_BT	*src  = (BMP_BT *)dobj.src1;
	RGB32	pal[256];
	RGB32	pal2[256];
	BMP_BT	src2={ src->buf, src->alp, pal, src->sx, src->sy };

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FB]\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawBMP_FB]\n"); return FALSE; }

	
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
		CopyMemory( pal2, src->pal, 256*4 );
		if( dobj.dparam==DRW_VIV2 ){
			BMP_ChangePalFine_Full( pal, 256-dobj.dnum );
			BMP_ChangePalBright_Full( pal, dobj.r, dobj.g, dobj.b );
		}else{						
			BMP_ChangePalBright_Full( pal, dobj.r, dobj.g, dobj.b );
			BMP_ChangePalBright_Full( pal2, dobj.r, dobj.g, dobj.b );
			switch(dobj.dparam){
				case DRW_SBL2:	BMP_ChangePalBright_Full( pal, 128/2, 128/2, 128/2 );					break;
				case DRW_BLD2:	BMP_ChangePalBright_Full( pal, dobj.dnum/2, dobj.dnum/2, dobj.dnum/2 );	break;
				case DRW_NEG:	BMP_ChangePalNega_Full( pal );											break;	
			}
		}
	}

	switch(dobj.dparam){
		default:

		case DRW_TNC:	
		case DRW_NEG:
		case DRW_VIV2:
		case DRW_NML:	DRW_DrawBMP_FB_Std( &dobj, &src2 );	break;	
		case DRW_BLD2:	DRW_DrawBMP_FB_Bld( &dobj, &src2, pal2 );	break;	
		case DRW_ADD:	DRW_DrawBMP_FB_Add( &dobj, &src2 );	break;	
		case DRW_OOI:	DRW_DrawBMP_FB_Ooi( &dobj, &src2 );	break;	
		case DRW_SUB:	DRW_DrawBMP_FB_Sub( &dobj, &src2 );	break;	
		case DRW_MUL:	DRW_DrawBMP_FB_Mul( &dobj, &src2 );	break;	
		case DRW_AMI2:	DRW_DrawBMP_FB_Ami( &dobj, &src2 );	break;	
		case DRW_NIS2:	DRW_DrawBMP_FB_Nis( &dobj, &src2 );	break;	
		case DRW_BOM2:	DRW_DrawBMP_FB_Bom( &dobj, &src2 );	break;	
		case DRW_GNM2:	DRW_DrawBMP_FB_Gnm( &dobj, &src2 );	break;	
		case DRW_FLT2:	DRW_DrawBMP_FB_Flt( &dobj, &src2 );	break;	

		case DRW_OLY:	DRW_DrawBMP_FB_Oly( &dobj, &src2 );	break;	
		case DRW_CML:	DRW_DrawBMP_FB_Cml( &dobj, &src2 );	break;	
		case DRW_CMD:	DRW_DrawBMP_FB_Cmd( &dobj, &src2 );	break;	

		case DRW_SUI2:	DRW_DrawBMP_FB_Sui(	&dobj, &src2 );	break;	
		case DRW_SBL2:	DRW_DrawBMP_FB_Sbl(	&dobj, &src2 );	break;	

		case DRW_RP22:	DRW_DrawBMP_FB_Rp2(	&dobj, &src2 );	break;	
	}
	return ret;
}










BOOL DRW_DrawZOOM2_FT_Std(	BMP_F *dest, int dx, int dy,
							BMP_F *src,  int sx, int sy, int sw, int sh,
							int nuki, int r, int g, int b )
{
	int			x, y;
	RGB24		*src_p  = src->buf  + sy*src->sx  + sx;
	RGB24		*dest_p = dest->buf + dy*dest->sx + dx;
	int			drev = (dest->sx-sw)*2;
	int			srev = (src->sx-sw);
	int			ddx = dest->sx;
	int			ssx = src->sx;

	if(nuki == -3){
		dy+=2;	sy+=1;	sh-=2;
		src_p  = src->buf  + sy*src->sx  + sx;
		dest_p = dest->buf + dy*dest->sx + dx;
	}

	if( r==0x80 && g==0x80 && b==0x80 ){	
		if(nuki == -3){
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					*(DWORD*)(dest_p      ) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p-ssx) + *(DWORD*)(src_p-1);	
					*(DWORD*)(dest_p    +1) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p-ssx) + *(DWORD*)(src_p+1);	
					*(DWORD*)(dest_p+ddx  ) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p+ssx) + *(DWORD*)(src_p-1);	
					*(DWORD*)(dest_p+ddx+1) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p+ssx) + *(DWORD*)(src_p+1);	
				}
			}
		}else{
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw-1; x++, dest_p+=2, src_p++ ) {
					*(DWORD*)(dest_p+ddx+1) = *(DWORD*)(dest_p+ddx) = *(DWORD*)(dest_p+1) = *(DWORD*)dest_p = *(DWORD*)src_p;
				}
				if(sw>0){
					*(dest_p+ddx+1) = *(dest_p+ddx) = *(dest_p+1) = *dest_p = *src_p;
					dest_p+=2, src_p++;
				}
			}
		}
	}else{
		char		*rtbl = BrightTable_TB[r];
		char		*gtbl = BrightTable_TB[g];
		char		*btbl = BrightTable_TB[b];

		if(nuki == -3){
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					*(DWORD*)(dest_p      ) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p-ssx) + *(DWORD*)(src_p-1);	
					*(DWORD*)(dest_p    +1) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p-ssx) + *(DWORD*)(src_p+1);	
					*(DWORD*)(dest_p+ddx  ) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p+ssx) + *(DWORD*)(src_p-1);	
					*(DWORD*)(dest_p+ddx+1) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p+ssx) + *(DWORD*)(src_p+1);	

					dest_p->b         = btbl[ dest_p->b         ];	dest_p->g         = gtbl[ dest_p->g         ];	dest_p->r         = rtbl[ dest_p->r         ];
					(dest_p+1)->b     = btbl[ (dest_p+1)->b     ];	(dest_p+1)->g     = gtbl[ (dest_p+1)->g     ];	(dest_p+1)->r     = rtbl[ (dest_p)->r       ];
					(dest_p+ddx)->b   = btbl[ (dest_p+ddx)->b   ];	(dest_p+ddx)->g   = gtbl[ (dest_p+ddx)->g   ];	(dest_p+ddx)->r   = rtbl[ (dest_p+ddx)->r   ];
					(dest_p+ddx+1)->b = btbl[ (dest_p+ddx+1)->b ];	(dest_p+ddx+1)->g = gtbl[ (dest_p+ddx+1)->g ];	(dest_p+ddx+1)->r = rtbl[ (dest_p+ddx+1)->r ];
				}
			}
		}else{
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					(dest_p+ddx+1)->b = (dest_p+ddx)->b = (dest_p+1)->b = dest_p->b = btbl[ src_p->b ];
					(dest_p+ddx+1)->g = (dest_p+ddx)->g = (dest_p+1)->g = dest_p->g = gtbl[ src_p->g ];
					(dest_p+ddx+1)->r = (dest_p+ddx)->r = (dest_p+1)->r = dest_p->r = rtbl[ src_p->r ];
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_FT_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx,yy;
	char	*alph_tbl,*alp2_tbl,*alp3_tbl;

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
				for( x=0; x<dw-1; x++, dest_p++ ){
					*(RGB32*)dest_p = *sss;

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				if(dw-1>=0){
					*dest_p = *(RGB24*)sss;
					dest_p++;
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
							*dest_p = *(RGB24*)sss;
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
						*dest_p = *(RGB24*)sss;

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

		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw-1; x++, dest_p++ ) {
					*(DWORD*)dest_p = rtbl[sss->r] | gtbl[sss->g] | btbl[sss->b];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
				}
				if(dw-1>=0){
					dest_p->b = btbl[sss->b];
					dest_p->g = gtbl2[sss->g];
					dest_p->r = rtbl2[sss->r];
					dest_p++;
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
							dest_p->b = btbl[sss->b];
							dest_p->g = gtbl2[sss->g];
							dest_p->r = rtbl2[sss->r];
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
				for( x=0; x<dw-1; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						dest_p->b = btbl[sss->b];
						dest_p->g = gtbl2[sss->g];
						dest_p->r = rtbl2[sss->r];
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



BOOL DRW_DrawZOOM_FT_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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
	if( blnd==BLD_100P)	return DRW_DrawZOOM_FT_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy );	

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
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						if( sss->a == 255 ){	
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ sss->r ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ sss->g ];
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ sss->b ];
						}else{					
							alph_tbl = BlendTable[ 255 - blnd_tbl[sss->a] ];
							dest_p->b = alph_tbl[dest_p->b] + blnd_tbl[sss->b];
							dest_p->g = alph_tbl[dest_p->g] + blnd_tbl[sss->g];
							dest_p->r = alph_tbl[dest_p->r] + blnd_tbl[sss->r];
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
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						if( sss->a == 255 ){	
							dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ rtbl[sss->r] ];
							dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ gtbl[sss->g] ];
							dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ btbl[sss->b] ];
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




BOOL DRW_DrawZOOM_FT_Add( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src  = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						
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
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						
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




BOOL DRW_DrawZOOM_FT_Sub( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src  = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						
						dest_p->r = sub_tbl[ dest_p->r - sss->r ];
						dest_p->g = sub_tbl[ dest_p->g - sss->g ];
						dest_p->b = sub_tbl[ dest_p->b - sss->b ];
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
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						
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



BOOL DRW_DrawZOOM_FT_Mul( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y,br;
	int		px,py,wx,wy,xx=0,yy=0;
	char	*alph_tbl, *alp3_tbl;

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
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						
						if( sss->a == 255 ){	
							dest_p->r = BrightTable_TB[ sss->r ][ dest_p->r ];
							dest_p->g = BrightTable_TB[ sss->g ][ dest_p->g ];
							dest_p->b = BrightTable_TB[ sss->b ][ dest_p->b ];
						}else{
							alph_tbl = BlendTable[ sss->a ];
							br = (256-sss->a)>>1;

							dest_p->b = BrightTable_TB[ alph_tbl[sss->b]+br ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ alph_tbl[sss->g]+br ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ alph_tbl[sss->r]+br ][ dest_p->r ];
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
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						
						if( sss->a == 255 ){	
							dest_p->r = BrightTable_TB[ sss->r ][ dest_p->r ];
							dest_p->g = BrightTable_TB[ sss->g ][ dest_p->g ];
							dest_p->b = BrightTable_TB[ sss->b ][ dest_p->b ];
						}else{
							alph_tbl = BlendTable[ sss->a ];
							alp3_tbl = BlendTable2[ sss->a ];
							br = (256-sss->a)>>1;

							dest_p->b = BrightTable_TB[ alph_tbl[btbl[alp3_tbl[sss->b]]]+br ][ dest_p->b ];
							dest_p->g = BrightTable_TB[ alph_tbl[gtbl[alp3_tbl[sss->g]]]+br ][ dest_p->g ];
							dest_p->r = BrightTable_TB[ alph_tbl[rtbl[alp3_tbl[sss->r]]]+br ][ dest_p->r ];
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




BOOL DRW_DrawZOOM_FT_Ami( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=0;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	int		mesh = dobj->dnum;
	char	*alph_tbl;

	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=0   )	return 1;															
	if( mesh>=256 )	return DRW_DrawZOOM_FT_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy );	

	
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
					if( linemask2[x & 3] ){

						dest_p->b = sss->b;
						dest_p->g = sss->g;
						dest_p->r = sss->r;
					}

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
							dest_p->b = sss->b;
							dest_p->g = sss->g;
							dest_p->r = sss->r;
						}else{
							alph_tbl = BlendTable[ 255-sss->a ];

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
			for( y=0; y<dh; y++, dest_p+=drev ) {
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] && *(long*)sss != nuki ){

						dest_p->b = sss->b;
						dest_p->g = sss->g;
						dest_p->r = sss->r;
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
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] ){
						dest_p->r = rtbl[sss->r];
						dest_p->g = gtbl[sss->g];
						dest_p->b = btbl[sss->b];
					}

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
							dest_p->b = rtbl[sss->b];
							dest_p->g = gtbl[sss->g];
							dest_p->r = btbl[sss->r];
						}else{
							alph_tbl = BlendTable[ 255-sss->a ];

							dest_p->b = alph_tbl[dest_p->b] + rtbl[sss->b];
							dest_p->g = alph_tbl[dest_p->g] + gtbl[sss->g];
							dest_p->r = alph_tbl[dest_p->r] + btbl[sss->r];
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
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] && *(long*)sss != nuki ){

						dest_p->r = rtbl[sss->r];
						dest_p->g = gtbl[sss->g];
						dest_p->b = btbl[sss->b];
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



BOOL DRW_DrawZOOM_FT_Neg( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=0;
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;

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
					dest_p->b = ~sss->b;
					dest_p->g = ~sss->g;
					dest_p->r = ~sss->r;

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
							dest_p->b = ~sss->b;
							dest_p->g = ~sss->g;
							dest_p->r = ~sss->r;
						}else{
							alph_tbl = BlendTable[ 255-sss->a ];
							alp2_tbl = BlendTable[  sss->a ];
							alp3_tbl = BlendTable2[ sss->a ];

							dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[~alp3_tbl[sss->b]];
							dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[~alp3_tbl[sss->g]];
							dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[~alp3_tbl[sss->r]];
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
						dest_p->b = sss->b;
						dest_p->g = sss->g;
						dest_p->r = sss->r;
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
		}else if( nuki==-2 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					if( sss->a != 0 ){	
						if( sss->a == 255 ){	
							dest_p->b = ~btbl[ sss->b ];
							dest_p->g = ~gtbl[ sss->g ];
							dest_p->r = ~rtbl[ sss->r ];
						}else{
							alph_tbl = BlendTable[ 255-sss->a ];
							alp2_tbl = BlendTable[  sss->a ];
							alp3_tbl = BlendTable2[ sss->a ];

							dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[~btbl[alp3_tbl[sss->b]]];
							dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[~gtbl[alp3_tbl[sss->g]]];
							dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[~rtbl[alp3_tbl[sss->r]]];
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



BOOL DRW_DrawZOOM_FT_Viv( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_T	*src = (BMP_T *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB32	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		viv = dobj->dnum, glay;
	char	*viv_tbl=BlendTable[viv];
	char	*riv_tbl=BlendTable[256-viv];
	int		x,y;
	int		px,py,wx,wy,xx,yy;
	char	*alph_tbl, *alp2_tbl, *alp3_tbl;

	if( viv==256 ) { return DRW_DrawZOOM_FT_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy ); }	

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
			}else if( nuki==-2 ){
				for( y=0; y<dh; y++, dest_p+=drev ){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ){
						if( sss->a != 0 ){	
							if( sss->a == 255 ){	
								dest_p->r = dest_p->g = dest_p->b= riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
							}else{
								alph_tbl = BlendTable[ 255-sss->a ];
								alp2_tbl = BlendTable[  sss->a ];
								alp3_tbl = BlendTable2[ sss->a ];

								glay = alp3_tbl[ riv_tbl[ (sss->b+sss->g+sss->g+sss->r)>>2 ] ];

								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[glay];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[glay];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[glay];
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
			}else if( nuki==-2 ){
				for( y=0; y<dh; y++, dest_p+=drev ){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ){
						if( sss->a != 0 ){	
							if( sss->a == 255 ){	
								glay = riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
								dest_p->b = btbl[ glay ];
								dest_p->g = gtbl[ glay ];
								dest_p->r = rtbl[ glay ];
							}else{
								alph_tbl = BlendTable[ 255-sss->a ];
								alp2_tbl = BlendTable[  sss->a ];
								alp3_tbl = BlendTable2[ sss->a ];

								glay = alp3_tbl[ riv_tbl[ (sss->b+sss->g+sss->g+sss->r)>>2 ] ];

								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[ btbl[glay] ];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[ gtbl[glay] ];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[ rtbl[glay] ];
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
			}else if( nuki==-2 ){
				for( y=0; y<dh; y++, dest_p+=drev ){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ){
						if( sss->a != 0 ){	
							if( sss->a == 255 ){	
								glay = riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
								dest_p->b = glay + viv_tbl[sss->b];
								dest_p->g = glay + viv_tbl[sss->g];
								dest_p->r = glay + viv_tbl[sss->r];
							}else{
								alph_tbl = BlendTable[ 255-sss->a ];
								alp2_tbl = BlendTable[  sss->a ];
								alp3_tbl = BlendTable2[ sss->a ];

								glay = alp3_tbl[ riv_tbl[ (sss->b+sss->g+sss->g+sss->r)>>2 ] ];

								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[glay + viv_tbl[sss->b]];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[glay + viv_tbl[sss->g]];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[glay + viv_tbl[sss->r]];
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
			}else if( nuki==-2 ){
				for( y=0; y<dh; y++, dest_p+=drev ){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ){
						if( sss->a != 0 ){	
							if( sss->a == 255 ){	
								glay = riv_tbl[ ( sss->b+ sss->g + sss->g + sss->r )>>2 ];
								dest_p->b = btbl[ glay + viv_tbl[sss->b] ];
								dest_p->g = gtbl[ glay + viv_tbl[sss->g] ];
								dest_p->r = rtbl[ glay + viv_tbl[sss->r] ];
							}else{
								alph_tbl = BlendTable[ 255-sss->a ];
								alp2_tbl = BlendTable[  sss->a ];
								alp3_tbl = BlendTable2[ sss->a ];

								glay = alp3_tbl[ riv_tbl[ (sss->b+sss->g+sss->g+sss->r)>>2 ] ];

								dest_p->b = alph_tbl[dest_p->b] + alp2_tbl[ btbl[glay + viv_tbl[sss->b]] ];
								dest_p->g = alph_tbl[dest_p->g] + alp2_tbl[ gtbl[glay + viv_tbl[sss->g]] ];
								dest_p->r = alph_tbl[dest_p->r] + alp2_tbl[ rtbl[glay + viv_tbl[sss->r]] ];
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






BOOL DRW_DrawZOOM_FT( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_T	*src  = (BMP_T *)dobj.src1;
	int		xx, yy, dw2=dobj.dw, dh2=dobj.dh, sw2=dobj.s1w, sh2=dobj.s1h;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_FT]\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_FT]\n"); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRectZ( &dobj.s1x, &dobj.s1y, &dobj.s1w, &dobj.s1h,
						&dobj.dx,  &dobj.dy,  &dobj.dw,  &dobj.dh,
						&xx, &yy, dest->sx, dest->sy, src->sx, src->sy, dobj.clip ) )	return 1;
	}
	
	if( dobj.dparam==DRW_AMI2 || dobj.dparam==DRW_BLD2 ){
		if( dobj.dnum==BLD_0P   ) return 1;	
		if( dobj.dnum==BLD_100P ){
			dobj.dparam = DRW_NML;	
		}
	}
	switch(dobj.dparam){
		default:
		case DRW_NML:	DRW_DrawZOOM_FT_Std( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_BLD2:	DRW_DrawZOOM_FT_Bld( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_ADD:	DRW_DrawZOOM_FT_Add( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_SUB:	DRW_DrawZOOM_FT_Sub( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_MUL:	DRW_DrawZOOM_FT_Mul( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_AMI2:	DRW_DrawZOOM_FT_Ami( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_NEG:	DRW_DrawZOOM_FT_Neg( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_VIV2:	DRW_DrawZOOM_FT_Viv( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
	}
	return TRUE;
}










BOOL DRW_DrawZOOM2_FF_Std(	BMP_F *dest, int dx, int dy,
							BMP_F *src,  int sx, int sy, int sw, int sh,
							int nuki, int r, int g, int b )
{
	int			x, y;
	RGB24		*src_p  = src->buf  + sy*src->sx  + sx;
	RGB24		*dest_p = dest->buf + dy*dest->sx + dx;
	int			drev = (dest->sx-sw)*2;
	int			srev = (src->sx-sw);
	int			ddx = dest->sx;
	int			ssx = src->sx;

	if(nuki == -3){
		dy+=2;	sy+=1;	sh-=2;
		src_p  = src->buf  + sy*src->sx  + sx;
		dest_p = dest->buf + dy*dest->sx + dx;
	}

	if( r==0x80 && g==0x80 && b==0x80 ){	
		if(nuki == -3){
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					*(DWORD*)(dest_p      ) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p-ssx) + *(DWORD*)(src_p-1);	
					*(DWORD*)(dest_p    +1) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p-ssx) + *(DWORD*)(src_p+1);	
					*(DWORD*)(dest_p+ddx  ) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p+ssx) + *(DWORD*)(src_p-1);	
					*(DWORD*)(dest_p+ddx+1) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p+ssx) + *(DWORD*)(src_p+1);	
				}
			}
		}else{
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw-1; x++, dest_p+=2, src_p++ ) {
					*(DWORD*)(dest_p+ddx+1) = *(DWORD*)(dest_p+ddx) = *(DWORD*)(dest_p+1) = *(DWORD*)dest_p = *(DWORD*)src_p;
				}
				if(sw>0){
					*(dest_p+ddx+1) = *(dest_p+ddx) = *(dest_p+1) = *dest_p = *src_p;
					dest_p+=2, src_p++;
				}
			}
		}
	}else{
		char		*rtbl = BrightTable_TB[r];
		char		*gtbl = BrightTable_TB[g];
		char		*btbl = BrightTable_TB[b];

		if(nuki == -3){
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					*(DWORD*)(dest_p      ) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p-ssx) + *(DWORD*)(src_p-1);	
					*(DWORD*)(dest_p    +1) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p-ssx) + *(DWORD*)(src_p+1);	
					*(DWORD*)(dest_p+ddx  ) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p+ssx) + *(DWORD*)(src_p-1);	
					*(DWORD*)(dest_p+ddx+1) = *(DWORD*)src_p+*(DWORD*)src_p + *(DWORD*)(src_p+ssx) + *(DWORD*)(src_p+1);	

					dest_p->b         = btbl[ dest_p->b         ];	dest_p->g         = gtbl[ dest_p->g         ];	dest_p->r         = rtbl[ dest_p->r         ];
					(dest_p+1)->b     = btbl[ (dest_p+1)->b     ];	(dest_p+1)->g     = gtbl[ (dest_p+1)->g     ];	(dest_p+1)->r     = rtbl[ (dest_p)->r       ];
					(dest_p+ddx)->b   = btbl[ (dest_p+ddx)->b   ];	(dest_p+ddx)->g   = gtbl[ (dest_p+ddx)->g   ];	(dest_p+ddx)->r   = rtbl[ (dest_p+ddx)->r   ];
					(dest_p+ddx+1)->b = btbl[ (dest_p+ddx+1)->b ];	(dest_p+ddx+1)->g = gtbl[ (dest_p+ddx+1)->g ];	(dest_p+ddx+1)->r = rtbl[ (dest_p+ddx+1)->r ];
				}
			}
		}else{
			for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
				for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {
					(dest_p+ddx+1)->b = (dest_p+ddx)->b = (dest_p+1)->b = dest_p->b = btbl[ src_p->b ];
					(dest_p+ddx+1)->g = (dest_p+ddx)->g = (dest_p+1)->g = dest_p->g = gtbl[ src_p->g ];
					(dest_p+ddx+1)->r = (dest_p+ddx)->r = (dest_p+1)->r = dest_p->r = rtbl[ src_p->r ];
				}
			}
		}
	}
	return TRUE;
}

BOOL DRW_DrawZOOMX_FF_Std(	BMP_F *dest, int dx, int dy, int dw, int dh,
							BMP_F *src,  int sx, int sy, int sw, int sh,
							int nuki, int r, int g, int b )
{
	int			x, y;
	int			xx, yy;
	RGB24		*src_p  = src->buf  + sy*src->sx  + sx, *src_p2;
	RGB24		*dest_p = dest->buf + dy*dest->sx + dx;
	int			drev,srev;
	int			ddx = dest->sx;
	int			ssx = src->sx;
	int			zoomx;
	int			rr,gg,bb;
	
	if(dw)	zoomx = sw/dw;
	else	return 0;
	if(zoomx<=0) return 0;

	drev = (dest->sx-dw);
	srev = src->sx*zoomx-sw;

	if( r==0x80 && g==0x80 && b==0x80 ){	
		for( y=0; y<sh/zoomx; y++, dest_p+=drev, src_p+=srev ) {
			for( x=0; x<sw/zoomx; x++, dest_p++, src_p+=zoomx ) {
				rr=0,gg=0,bb=0;
				src_p2 = src_p;

				for( yy=0; yy<zoomx; yy++, src_p2+=(src->sx-zoomx) ) {
					for( xx=0; xx<zoomx; xx++, src_p2++ ) {
						rr+=src_p2->r;
						gg+=src_p2->g;
						bb+=src_p2->b;
					}
				}
				dest_p->r = rr/(zoomx*zoomx);
				dest_p->g = gg/(zoomx*zoomx);
				dest_p->b = bb/(zoomx*zoomx);
			}
		}
	}else{
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_FF_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, int interless )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx,yy;

	if( nuki==-3 ){	
		return DRW_DrawZOOMX_FF_Std( dest, dobj->dx, dobj->dy, dobj->dw, dobj->dh, src, dobj->s1x, dobj->s1y, sw, sh, nuki, r, g, b );
	}

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy = yyy;


	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ){
				if(INTERLESS_CHECK){
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
				}else{
					dest_p+=dw;
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ){
				if(INTERLESS_CHECK){
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
				}else{
					dest_p+=dw;
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
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
			for( y=0; y<dh; y++, dest_p+=drev ) {
				if(INTERLESS_CHECK){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw-1; x++, dest_p++ ) {
						*(DWORD*)dest_p = rtbl[sss->r] | gtbl[sss->g] | btbl[sss->b];

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					if(dw-1>=0){
						dest_p->b = btbl[sss->b];
						dest_p->g = gtbl2[sss->g];
						dest_p->r = rtbl2[sss->r];
						dest_p++;
					}

					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}else{
					dest_p+=dw;
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				if(INTERLESS_CHECK){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw-1; x++, dest_p++ ) {
						if( *(long*)sss != nuki ){
							dest_p->b = btbl[sss->b];
							dest_p->g = gtbl2[sss->g];
							dest_p->r = rtbl2[sss->r];
						}

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}

					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}else{
					dest_p+=dw;
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawZOOM_FF_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, int interless )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
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
	
	if( blnd==BLD_0P  )	return 1;															
	if( blnd==BLD_100P)	return DRW_DrawZOOM_FF_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy, interless );	

	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;
	yy=yyy;


	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				if(INTERLESS_CHECK){
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
				}else{
					dest_p+=dw;
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
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




BOOL DRW_DrawZOOM_FF_Add( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
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




BOOL DRW_DrawZOOM_FF_Sub( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
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



BOOL DRW_DrawZOOM_FF_Dim( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, int interless )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
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
				if(INTERLESS_CHECK){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						dest_p->b = DimTable[dest_p->b][sss->b];
						dest_p->g = DimTable[dest_p->g][sss->g];
						dest_p->r = DimTable[dest_p->r][sss->r];

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}else{
					dest_p+=dw;
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				if(INTERLESS_CHECK){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						if( *(long*)sss != nuki ){
							dest_p->b = DimTable[dest_p->b][sss->b];
							dest_p->g = DimTable[dest_p->g][sss->g];
							dest_p->r = DimTable[dest_p->r][sss->r];
						}
						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}else{
					dest_p+=dw;
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}
	}else{
		char	*rtbl = BrightTable_TB[r];
		char	*gtbl = BrightTable_TB[g];
		char	*btbl = BrightTable_TB[b];

		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				if(INTERLESS_CHECK){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						dest_p->b = DimTable[dest_p->b][btbl[sss->b]];
						dest_p->g = DimTable[dest_p->g][gtbl[sss->g]];
						dest_p->r = DimTable[dest_p->r][rtbl[sss->r]];

						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}else{
					dest_p+=dw;
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}else{		
			for( y=0; y<dh; y++, dest_p+=drev ) {
				if(INTERLESS_CHECK){
					sss = src_p;
					xx=xxx;
					for( x=0; x<dw; x++, dest_p++ ) {
						if( *(long*)sss != nuki ){
							dest_p->b = DimTable[dest_p->b][btbl[sss->b]];
							dest_p->g = DimTable[dest_p->g][gtbl[sss->g]];
							dest_p->r = DimTable[dest_p->r][rtbl[sss->r]];
						}
						sss+=px;
						xx += wx;
						if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
					}
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}else{
					dest_p+=dw;
					src_p+=py;
					yy += wy;
					if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_FF_Mul( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
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




BOOL DRW_DrawZOOM_FF_Ami( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy , int interless )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=0;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	int		mesh = dobj->dnum;
	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=0   )	return 1;															
	if( mesh>=256 )	return DRW_DrawZOOM_FF_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy, interless );	

	
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



BOOL DRW_DrawZOOM_FF_Neg( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
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



BOOL DRW_DrawZOOM_FF_Viv( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, int interless )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	BMP_F	*src = (BMP_F *)dobj->src1;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	RGB24	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		viv = dobj->dnum, glay;
	char	*viv_tbl=BlendTable[viv];
	char	*riv_tbl=BlendTable[256-viv];
	int		x,y;
	int		px,py,wx,wy,xx,yy;

	if( viv==256 ) { return DRW_DrawZOOM_FF_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy, interless ); }	

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






BOOL DRW_DrawZOOM_FF( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_F	*src  = (BMP_F *)dobj.src1;
	int		xx, yy, dw2=dobj.dw, dh2=dobj.dh, sw2=dobj.s1w, sh2=dobj.s1h;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_FF]\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_FF]\n"); return FALSE; }

	
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
		case DRW_NML:	DRW_DrawZOOM_FF_Std( &dobj, dw2, dh2, sw2, sh2, xx, yy, interless );	break;	
		case DRW_BLD2:	DRW_DrawZOOM_FF_Bld( &dobj, dw2, dh2, sw2, sh2, xx, yy, interless );	break;	
		case DRW_ADD:	DRW_DrawZOOM_FF_Add( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_SUB:	DRW_DrawZOOM_FF_Sub( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_DIM:	DRW_DrawZOOM_FF_Dim( &dobj, dw2, dh2, sw2, sh2, xx, yy, interless );	break;	
		case DRW_MUL:	DRW_DrawZOOM_FF_Mul( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_AMI2:	DRW_DrawZOOM_FF_Ami( &dobj, dw2, dh2, sw2, sh2, xx, yy, interless );	break;	
		case DRW_NEG:	DRW_DrawZOOM_FF_Neg( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_VIV2:	DRW_DrawZOOM_FF_Viv( &dobj, dw2, dh2, sw2, sh2, xx, yy, interless );	break;	
	}
	return TRUE;
}











BOOL DRW_DrawZOOM2_FB_Std(	BMP_F *dest, int dx, int dy,
							BMP_BT *src, int sx, int sy, int sw, int sh,
							int nuki )
{
	int			x, y;
	char		*src_p  = src->buf  + sy*src->sx  + sx;
	RGB32		*pal = src->pal;
	RGB24		*dest_p = dest->buf + dy*dest->sx + dx;
	int			drev = (dest->sx-sw)*2;
	int			srev = (src->sx-sw);
	int			ddx = dest->sx;
	int			ssx = src->sx;

	if(nuki == -3){
		dy+=2;	sy+=1;	sh-=2;
		src_p  = src->buf  + sy*src->sx  + sx;
		dest_p = dest->buf + dy*dest->sx + dx;
	}

	if(nuki == -3){
		RGB32	pal32[256];

		CopyMemory( pal32, pal, 1024 );

		for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
			for( x=0; x<sw; x++, dest_p+=2, src_p++ ) {




				dest_p->r = (pal[*src_p].r + pal[*src_p].r + pal[*(src_p-ssx)].r + pal[*(src_p-1)].r)>>2;	
				dest_p->g = (pal[*src_p].g + pal[*src_p].g + pal[*(src_p-ssx)].g + pal[*(src_p-1)].g)>>2;	
				dest_p->b = (pal[*src_p].b + pal[*src_p].b + pal[*(src_p-ssx)].b + pal[*(src_p-1)].b)>>2;	

				(dest_p    +1)->r = (pal[*src_p].r+pal[*src_p].r + pal[*(src_p-ssx)].r + pal[*(src_p+1)].r)>>2;	
				(dest_p    +1)->g = (pal[*src_p].g+pal[*src_p].g + pal[*(src_p-ssx)].g + pal[*(src_p+1)].g)>>2;	
				(dest_p    +1)->b = (pal[*src_p].b+pal[*src_p].b + pal[*(src_p-ssx)].b + pal[*(src_p+1)].b)>>2;	

				(dest_p+ddx  )->r = (pal[*src_p].r+pal[*src_p].r + pal[*(src_p+ssx)].r + pal[*(src_p-1)].r)>>2;	
				(dest_p+ddx  )->g = (pal[*src_p].g+pal[*src_p].g + pal[*(src_p+ssx)].g + pal[*(src_p-1)].g)>>2;	
				(dest_p+ddx  )->b = (pal[*src_p].b+pal[*src_p].b + pal[*(src_p+ssx)].b + pal[*(src_p-1)].b)>>2;	

				(dest_p+ddx+1)->r = (pal[*src_p].r+pal[*src_p].r + pal[*(src_p+ssx)].r + pal[*(src_p+1)].r)>>2;	
				(dest_p+ddx+1)->g = (pal[*src_p].g+pal[*src_p].g + pal[*(src_p+ssx)].g + pal[*(src_p+1)].g)>>2;	
				(dest_p+ddx+1)->b = (pal[*src_p].b+pal[*src_p].b + pal[*(src_p+ssx)].b + pal[*(src_p+1)].b)>>2;	
			}
		}
	}else{
		for( y=0; y<sh; y++, dest_p+=drev, src_p+=srev ) {
			for( x=0; x<sw-1; x++, dest_p+=2, src_p++ ) {
				*(DWORD*)(dest_p+ddx+1) = *(DWORD*)(dest_p+ddx) = *(DWORD*)(dest_p+1) = *(DWORD*)dest_p = *(DWORD*)&pal[*src_p];
			}
			if(sw>0){
				*(dest_p+ddx+1) = *(dest_p+ddx) = *(dest_p+1) = *dest_p = *(RGB24*)&pal[*src_p];
				dest_p+=2, src_p++;
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_FB_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	RGB32	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;

	if( sw*2==dw && sh*2==dh ){	
		if( nuki==-1 || nuki==-3 ){	
			return DRW_DrawZOOM2_FB_Std( dest, dobj->dx, dobj->dy, src, dobj->s1x, dobj->s1y, sw, sh, nuki );
		}
	}


	px = sw2/dw2 * xinc;
	wx = sw2%dw2;
	py = sh2/dh2 * yinc;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				*dest_p = *(RGB24*)&spal[ *sss ];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; }
		}
	}else if(nuki==-2){
		char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj ), *aaa;
		char	*alp_tbl, *al2_tbl;

		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			aaa = alp_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *aaa != 0 ){
					if( *aaa == 255 ){
						*dest_p =  *(RGB24*)&spal[ *sss ];
					}else{
						alp_tbl = BlendTable[*aaa];
						al2_tbl = BlendTable[255-*aaa];
						dest_p->b = al2_tbl[dest_p->b] + alp_tbl[spal[*sss].b];
						dest_p->g = al2_tbl[dest_p->g] + alp_tbl[spal[*sss].g];
						dest_p->r = al2_tbl[dest_p->r] + alp_tbl[spal[*sss].r];
					}
				}
				sss+=px;
				aaa+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; aaa+=xinc; }
			}
			src_p+=py;
			alp_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; alp_p+=yinc; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki )	*dest_p = *(RGB24*)&spal[ *sss ];

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




BOOL DRW_DrawZOOM_FB_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	RGB32	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;
	int		blnd=dobj->dnum;
	char	*blend_tbl = BlendTable[256-blnd];
	char	*blen2_tbl = BlendTable[blnd];

	if( blnd==BLD_0P  )	return 1;																	
	if( blnd==BLD_100P)	return DRW_DrawZOOM_FB_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy, src );		

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
	}else if( nuki==-2){
		char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj ), *aaa;
		char	*alp_tbl, *al2_tbl;

		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			aaa = alp_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *aaa != 0 ){
					if( *aaa == 255 ){
						dest_p->b = blend_tbl[ dest_p->b ]+ spal[*sss].b;
						dest_p->g = blend_tbl[ dest_p->g ]+ spal[*sss].g;
						dest_p->r = blend_tbl[ dest_p->r ]+ spal[*sss].r;
					}else{
						alp_tbl = BlendTable[ blen2_tbl[*aaa] ];
						al2_tbl = BlendTable[ 255-blen2_tbl[*aaa] ];
						dest_p->b = al2_tbl[dest_p->b] + alp_tbl[spal[*sss].b];
						dest_p->g = al2_tbl[dest_p->g] + alp_tbl[spal[*sss].g];
						dest_p->r = al2_tbl[dest_p->r] + alp_tbl[spal[*sss].r];
					}
				}
				sss+=px;
				aaa+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss+=xinc; aaa+=xinc; }
			}
			src_p+=py;
			alp_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=yinc; alp_p+=yinc; }
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



BOOL DRW_DrawZOOM_FB_Add( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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




BOOL DRW_DrawZOOM_FB_Sub( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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



BOOL DRW_DrawZOOM_FB_Mul( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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



BOOL DRW_DrawZOOM_FB_Ami( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BT *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_F	*dest = (BMP_F *)dobj->dest;
	int		drev = dest->sx - dw;
	RGB24	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
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
	if( mesh>=BLD_100P)	return DRW_DrawZOOM_FB_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy, src );	

	
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
				if( linemask2[x & 3] )
					*dest_p = *(RGB24*)&spal[ *sss ];

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
					if( linemask2[x & 3] )
						*dest_p = *(RGB24*)&spal[ *sss ];
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






BOOL DRW_DrawZOOM_FB( DRAW_OBJECT dobj, int interless )
{
	BOOL	ret=TRUE;
	BMP_F	*dest = (BMP_F *)dobj.dest;
	BMP_BT	*src  = (BMP_BT *)dobj.src1;
	int		xx, yy, dw2=dobj.dw, dh2=dobj.dh, sw2=dobj.s1w, sh2=dobj.s1h;
	RGB32	pal[256];
	BMP_BT	src2={ src->buf, src->alp, pal, src->sx, src->sy };

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_FB]\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[DRW_DrawZOOM_FB]\n"); return FALSE; }

	
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
		case DRW_NML:	DRW_DrawZOOM_FB_Std( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_BLD2:	DRW_DrawZOOM_FB_Bld( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_ADD:	DRW_DrawZOOM_FB_Add( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_SUB:	DRW_DrawZOOM_FB_Sub( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_MUL:	DRW_DrawZOOM_FB_Mul( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_AMI2:	DRW_DrawZOOM_FB_Ami( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
	}
	return TRUE;
}









BOOL DRW_DrawXLine_FF_Std(	RGB24 *dest_p, BMP_F *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB24	*src_p = src->buf + sy*src->sx + sx;
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
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
				dest_p->b = btbl[src_p->b];
				dest_p->g = gtbl[src_p->g];
				dest_p->r = rtbl[src_p->r];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					dest_p->b = btbl[src_p->b];
					dest_p->g = gtbl[src_p->g];
					dest_p->r = rtbl[src_p->r];
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



BOOL DRW_DrawXLine_FF_Bld(	RGB24 *dest_p, BMP_F *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b, int blnd )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB24	*src_p = src->buf + sy*src->sx + sx;
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



BOOL DRW_DrawXLine_FF_Add(	RGB24 *dest_p, BMP_F *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB24	*src_p = src->buf + sy*src->sx + sx;
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
	char	*btbl = BrightTable_TB[b];
	char	*add_tbl = AddTable;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( r==0x80 && g==0x80 && b==0x80 ){
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



BOOL DRW_DrawXLine_FF_Sub(	RGB24 *dest_p, BMP_F *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB24	*src_p = src->buf + sy*src->sx + sx;
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
	char	*btbl = BrightTable_TB[b];
	char	*sub_tbl = SubTable+255;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	if( r==0x80 && g==0x80 && b==0x80 ){
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



BOOL DRW_DrawXLine_FF_Mul(	RGB24 *dest_p, BMP_F *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB24	*src_p = src->buf + sy*src->sx + sx;
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



BOOL DRW_DrawXLine_FF_Ami(	RGB24 *dest_p, BMP_F *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b, int mesh,int dx,int dy )
{
	int		ssx=0,ssy=0;
	int		x;
	RGB24	*src_p = src->buf + sy*src->sx + sx;
	char	*rtbl = BrightTable_TB[r];
	char	*gtbl = BrightTable_TB[g];
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
				if( linemask2[(dx+x) & 3] ){
					dest_p->b = btbl[src_p->b];
					dest_p->g = gtbl[src_p->g];
					dest_p->r = rtbl[src_p->r];
				}

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki!=*(long*)src_p ){
					if( linemask2[(dx+x) & 3] ){
						dest_p->b = btbl[src_p->b];
						dest_p->g = gtbl[src_p->g];
						dest_p->r = rtbl[src_p->r];
					}
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




BOOL DRW_DrawXLine_FF( RGB24 *dest_p,int dy, BMP_F *src, MIN_MAX *mi, MIN_MAX *ma, int nuki, RECT *clip,
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
		case DRW_NML:	DRW_DrawXLine_FF_Std( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b );				break;	
		case DRW_BLD2:	DRW_DrawXLine_FF_Bld( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b, blnd );		break;	
		case DRW_ADD:	DRW_DrawXLine_FF_Add( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b );				break;	
		case DRW_SUB:	DRW_DrawXLine_FF_Sub( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b );				break;	
		case DRW_MUL:	DRW_DrawXLine_FF_Mul( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b );				break;	
		case DRW_AMI2:	DRW_DrawXLine_FF_Ami( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b, blnd,dx1,dy );	break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}






BOOL DRW_DrawPOLY4_FF( DRAW_OBJECT dobj, int interless )
{
	BOOL		ret=TRUE;
	BMP_F		*dest = (BMP_F *)dobj.dest;
	BMP_F		*src  = (BMP_F *)dobj.src1;
	RGB24		*dest_p = dest->buf;
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

	for( y = dobj.clip->top ; y < dobj.clip->bottom ; y++, dest_p+=dest->sx ){
		if(INTERLESS_CHECK2){
			DRW_DrawXLine_FF( dest_p, y, src, &mi[y], &ma[y], dobj.nuki, dobj.clip, dobj.r, dobj.g, dobj.b, dobj.dparam, dobj.dnum );
		}
	}

end:
	GFree(mi);
	GFree(ma);

	return ret;
}









BOOL DRW_DrawXLine_FB_Std(	RGB24 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32	*spal = src->pal;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	

	if( nuki==-1 ){
		for( x=0; x < w ; x++, dest_p++ ){
			*dest_p = *(RGB24*)&spal[*src_p];

			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}else if(nuki==-2){
		char	*alp_tbl, *al2_tbl;

		for( x=0; x < w ; x++, dest_p++ ){




			if( spal[*src_p].a != 0 ){
				if( spal[*src_p].a == 255 ){
					*dest_p =  *(RGB24*)&spal[*src_p];
				}else{
					alp_tbl = BlendTable[spal[*src_p].a];
					al2_tbl = BlendTable[255-spal[*src_p].a];
					dest_p->b = al2_tbl[dest_p->b] + alp_tbl[spal[*src_p].b];
					dest_p->g = al2_tbl[dest_p->g] + alp_tbl[spal[*src_p].g];
					dest_p->r = al2_tbl[dest_p->r] + alp_tbl[spal[*src_p].r];
				}
			}

			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}else{
		for( x=0; x < w ; x++, dest_p++ ){
			if( nuki != *src_p ){
				*dest_p = *(RGB24*)&spal[*src_p];
			}
			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}

	return TRUE;
}



BOOL DRW_DrawXLine_FB_Bld(	RGB24 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int blnd )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32	*spal = src->pal;
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



BOOL DRW_DrawXLine_FB_Add(	RGB24 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32	*spal = src->pal;
	char	*add_tbl = AddTable;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

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
	return TRUE;
}



BOOL DRW_DrawXLine_FB_Sub(	RGB24 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32	*spal = src->pal;
	char	*sub_tbl = SubTable+255;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

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
	return TRUE;
}



BOOL DRW_DrawXLine_FB_Mul(	RGB24 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32	*spal = src->pal;

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



BOOL DRW_DrawXLine_FB_Ami(	RGB24 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int mesh,int dx,int dy )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	RGB32	*spal = src->pal;
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
			if( linemask2[(dx+x) & 3] )
				*dest_p = *(RGB24*)&spal[*src_p];

			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}else{
		for( x=0; x < w ; x++, dest_p++ ){
			if( nuki!=*(long*)src_p ){
				if( linemask2[(dx+x) & 3] )
					*dest_p = *(RGB24*)&spal[*src_p];
			}
			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}

	return TRUE;
}




BOOL DRW_DrawXLine_FB( RGB24 *dest_p,int dy, BMP_BT *src, MIN_MAX *mi, MIN_MAX *ma, int nuki, RECT *clip, int param, int blnd )
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

	if(sx<0) { dsx = (-sx)%ww; fsx=-1; sx2 = LIM(sx*xx/ww + mi->sx-1,0,src->sx-1); }
	else	 { dsx =   sx %ww; fsx= 1; sx2 = LIM(sx*xx/ww + mi->sx,  0,src->sx-1); }

	if(sy<0) { dsy = (-sy)%ww; fsy=-1; sy2 = LIM(sy*xx/ww + mi->sy-1,0,src->sy-1); }
	else	 { dsy =   sy %ww; fsy= 1; sy2 = LIM(sy*xx/ww + mi->sy,  0,src->sy-1); }

	switch( param ){
		default:
		case DRW_NML:	DRW_DrawXLine_FB_Std( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );				break;	
		case DRW_BLD2:	DRW_DrawXLine_FB_Bld( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, blnd );			break;	
		case DRW_ADD:	DRW_DrawXLine_FB_Add( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );				break;	
		case DRW_SUB:	DRW_DrawXLine_FB_Sub( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );				break;	
		case DRW_MUL:	DRW_DrawXLine_FB_Mul( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );				break;	
		case DRW_AMI2:	DRW_DrawXLine_FB_Ami( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, blnd,dx1,dy );	break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}






BOOL DRW_DrawPOLY4_FB( DRAW_OBJECT dobj, int interless )
{
	BOOL		ret=TRUE;
	BMP_F		*dest = (BMP_F *)dobj.dest;
	BMP_BT		*src  = (BMP_BT *)dobj.src1;
	RGB24		*dest_p = dest->buf;
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

	for( y = dobj.clip->top ; y < dobj.clip->bottom ; y++, dest_p+=dest->sx ){
		if(INTERLESS_CHECK2){
			DRW_DrawXLine_FB( dest_p, y, &src2, &mi[y], &ma[y], dobj.nuki, dobj.clip, dobj.dparam, dobj.dnum );
		}
	}

end:
	GFree(mi);
	GFree(ma);

	return ret;
}
