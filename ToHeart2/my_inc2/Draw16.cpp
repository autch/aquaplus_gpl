
 
  
   
    
     
      

#include "mm_std.h"


#include "bmp.h"
#include "draw.h"
#include "drawprim.h"




extern int	DispWidth;
extern int	DispHeight;












BOOL DRW_DrawBMP_HHH_Bld( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src1 = (BMP_H *)dobj->src1;
	BMP_H	*src2 = (BMP_H *)dobj->src2;
	int		drev  = dest->sx - w;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src1_p = src1->buf + DRW_GetStartPointerSrc1( dobj );
	WORD	*src2_p = src2->buf + DRW_GetStartPointerSrc2( dobj );
	int		xinc = XINC(dobj);
	int		blnd = dobj->dnum;
	int		blnd2 = dobj->dnum2;
	int		brev = 256 - blnd;

	if( dobj->dparam!=DRW_BLD2 ){
		int		brev = 256-blnd2;
		int		blnd3 = blnd2>>3, brev3 = 32-blnd3;

		if( nuki==-1 ){	
		}else if( nuki==-2 ){	
			char	*alp1_p = src1->alp + dobj->s1y *src1->sx + dobj->s1x;
			char	*alp2_p = src2->alp + dobj->s2y *src2->sx + dobj->s2x;
			int		brev2;

			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2, alp1_p+=srev1, alp2_p+=srev2 ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++, alp1_p++, alp2_p++ ){
					if( *alp1_p!=0 ){
						if( *alp2_p!=0 ){
							if( *alp1_p==255 ){
								if( *alp2_p==255 ){
									*dest_p = (WORD)( (( ( (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
													| (( ( (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
								}else{
									brev2 = (brev3)-((*alp2_p*brev3)>>8);
									*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
													| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
								}
							}else{
								if( *alp2_p==255 ){
									brev2 = (blnd3)-((*alp1_p*blnd3)>>8);
									*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
													| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
								}else{
									brev2  = 31 -((*alp2_p*brev3)>>8)-((*alp1_p*blnd3)>>8);
									*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
													| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
								}
							}
						}else{				
							if( *alp1_p==255 ){	
								*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
												| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
							}else{			
								brev2 = 31-((*alp1_p*blnd3)>>8);
								*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
												| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
							}
						}
					}else{
						if( *alp2_p!=0 ){	
							if( *alp2_p==255 ){	
								*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*blnd3 + (*src2_p&rgb16.mask_rb)*brev3 )>>5 )&rgb16.mask_rb)
												| (( ( (*dest_p&rgb16.mask_g )*blnd3 + (*src2_p&rgb16.mask_g )*brev3 )>>5 )&rgb16.mask_g ) );
							}else{							
								brev2 = 31-((*alp2_p*brev3)>>8);
								*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
												| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
							}
						}

					}
				}
			}
		}else{			
		}
	}else{
		int	blnd32 = blnd2>>3;
		int	brev32 = 32-blnd32;
		int	blnd3 = (blnd32*blnd)>>8;
		int	brev3 = (blnd>>3)-blnd3;
		int	dbb13 = 32-blnd3-brev3;
		int	dbb23 = 32-blnd3;
		int	dbb33 = 32-brev3;

		if( nuki==-1 ){	
		}else if( nuki==-2 ){	
			char	*alp1_p = src1->alp + DRW_GetStartPointerSrc1( dobj );
			char	*alp2_p = src2->alp + DRW_GetStartPointerSrc2( dobj );
			int		brev2;

			for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2, alp1_p+=srev1, alp2_p+=srev2 ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++, alp1_p++, alp2_p++ ){
					if( *alp1_p!=0 ){
						if( *alp2_p!=0 ){
							if( *alp1_p==255 ){
								if( *alp2_p==255 ){
									*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*dbb13 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
													| (( ( (*dest_p&rgb16.mask_g )*dbb13 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
								}else{
									brev2 = (31-blnd3)-((*alp2_p*brev3)>>8);
									*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
													| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
								}
							}else{
								if( *alp2_p==255 ){
									brev2 = (31-brev3)-((*alp1_p*blnd3)>>8);
									*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
													| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
								}else{
									brev2  = 31 -((*alp2_p*brev3)>>8)-((*alp1_p*blnd3)>>8);
									*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
													| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
								}
							}
						}else{				
							if( *alp1_p==255 ){	
								*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*dbb23 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
												| (( ( (*dest_p&rgb16.mask_g )*dbb23 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
							}else{			
								brev2 = 31-((*alp1_p*blnd3)>>8);
								*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
												| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
							}
						}
					}else{
						if( *alp2_p!=0 ){	
							if( *alp2_p==255 ){	
								*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*dbb33 + (*src2_p&rgb16.mask_rb)*brev3 )>>5 )&rgb16.mask_rb)
												| (( ( (*dest_p&rgb16.mask_g )*dbb33 + (*src2_p&rgb16.mask_g )*brev3 )>>5 )&rgb16.mask_g ) );
							}else{							
								brev2 = 31-((*alp2_p*brev3)>>8);
								*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src2_p&rgb16.mask_rb)*brev3 + (*src1_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
												| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src2_p&rgb16.mask_g )*brev3 + (*src1_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
							}
						}
					}
				}
			}
		}else{			
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HHH_Ami( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src1 = (BMP_H *)dobj->src1;
	BMP_H	*src2 = (BMP_H *)dobj->src2;
	int		drev  = dest->sx - w;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src1_p = src1->buf + DRW_GetStartPointerSrc1( dobj );
	WORD	*src2_p = src2->buf + DRW_GetStartPointerSrc2( dobj );
	char	*alp1_p = src1->alp + DRW_GetStartPointerSrc1( dobj );
	char	*alp2_p = src2->alp + DRW_GetStartPointerSrc2( dobj );
	int		brev2;
	int		xinc = XINC(dobj);

	int		mesh = dobj->dnum2;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;

	mesh_pt = MeshFadePatern[mesh/16];

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	for( y=0; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2, alp1_p+=srev1, alp2_p+=srev2 ){
		linemask2 = linemask[y & 3];
		for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++, alp1_p++, alp2_p++ ){
			if( linemask2[x & 3] ){
				if( *alp1_p!=0 ){	
					if( *alp1_p==255 ){
						*dest_p = *src1_p;
					}else{
						brev2 = 31-(*alp1_p>>3);
						*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
										| ((((*dest_p&rgb16.mask_g )*brev2)>>5)&rgb16.mask_g ) ) + *src1_p;
					}
				}
			}else{
				if( *alp2_p!=0 ){	
					if( *alp2_p==255 ){
						*dest_p = *src2_p;
					}else{
						brev2 = 31-((*alp2_p)>>3);
						*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
										| ((((*dest_p&rgb16.mask_g )*brev2)>>5)&rgb16.mask_g ) ) + *src2_p;
					}
				}
			}
		}
	}

	return TRUE;
}



BOOL DRW_DrawBMP_HHH_Lpp( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src1 = (BMP_H *)dobj->src1;
	BMP_H	*src2 = (BMP_H *)dobj->src2;
	int		drev  = dest->sx - w;
	int		srev1 = src1->sx * YINC(dobj) - w * XINC(dobj);
	int		srev2 = src2->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src1_p = src1->buf + DRW_GetStartPointerSrc1( dobj );
	WORD	*src2_p = src2->buf + DRW_GetStartPointerSrc2( dobj );
	int		xinc = XINC(dobj);
	int		rate = dobj->dnum2;
	int		rate_max[2];

	rate = (h/16+16)*(256-rate)/256;
	rate_max[0] = STD_Limit( rate*16-240, 0, h );
	rate_max[1] = STD_Limit( rate*16,     0, h );

	if( nuki==-1 ){	
	}else if( nuki==-2 ){	
		char	*alp1_p = src1->alp + DRW_GetStartPointerSrc1( dobj );
		char	*alp2_p = src2->alp + DRW_GetStartPointerSrc2( dobj );
		int		brev2;

		for( y=0; y < rate_max[0] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2, alp1_p+=srev1, alp2_p+=srev2 ){
			for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++, alp1_p++, alp2_p++ ){
				if( *alp2_p!=0 ){	
					if( *alp2_p==255 ){
						*dest_p = *src2_p;
					}else{
						brev2 = 31-((*alp2_p)>>3);
						*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
										| ((((*dest_p&rgb16.mask_g )*brev2)>>5)&rgb16.mask_g ) ) + *src2_p;
					}
				}
			}
		}
		for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2, alp1_p+=srev1, alp2_p+=srev2 ){
			if( (rate-(y>>4)) <= (y&0xf) ){
				for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++, alp1_p++, alp2_p++ ){
					if( *alp1_p!=0 ){	
						if( *alp1_p==255 ){
							*dest_p = *src1_p;
						}else{
							brev2 = 31-((*alp1_p)>>3);
							*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
											| ((((*dest_p&rgb16.mask_g )*brev2)>>5)&rgb16.mask_g ) ) + *src1_p;
						}
					}
				}
			}else{
				for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++, alp1_p++, alp2_p++ ){
					if( *alp2_p!=0 ){	
						if( *alp2_p==255 ){
							*dest_p = *src2_p;
						}else{
							brev2 = 31-((*alp2_p)>>3);
							*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
											| ((((*dest_p&rgb16.mask_g )*brev2)>>5)&rgb16.mask_g ) ) + *src2_p;
						}
					}
				}
			}
		}
		for( y=rate_max[1]; y < h ; y++, dest_p+=drev, src1_p+=srev1, src2_p+=srev2, alp1_p+=srev1, alp2_p+=srev2 ){
			for( x=0 ; x < w ; x++, dest_p++, src1_p++, src2_p++, alp1_p++, alp2_p++ ){
				if( *alp1_p!=0 ){	
					if( *alp1_p==255 ){
						*dest_p = *src1_p;
					}else{
						brev2 = 31-((*alp1_p)>>3);
						*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
										| ((((*dest_p&rgb16.mask_g )*brev2)>>5)&rgb16.mask_g ) ) + *src1_p;
					}
				}
			}
		}
	}else{			
	}
	return TRUE;
}





static BOOL DRW_DrawBMP_HH_Bld( DRAW_OBJECT *dobj );



BOOL DRW_DrawBMP_HHH( DRAW_OBJECT dobj )
{
	BOOL			ret=TRUE;
	BMP_H			*dest= (BMP_H *)dobj.dest;
	BMP_H			*src1= (BMP_H *)dobj.src1;
	BMP_H			*src2= (BMP_H *)dobj.src2;
	DRAW_OBJECT		dobj2, dobj3, dobj4, dobj5;
	
	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src1->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ1‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src2->buf==NULL ) { DebugPrintf("‰æ‘œ—Ìˆæ2‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }

	
	if( dobj.clip!=(void*)-1 ){
		if( !ClipRectS2(&dobj.dx,  &dobj.dy,  dest->sx, dest->sy,
						&dobj.s1x, &dobj.s1y, src1->sx, src1->sy,  &dobj.s1w, &dobj.s1h,
						&dobj.s2x, &dobj.s2y, src2->sx, src2->sy,  &dobj.s2w, &dobj.s2h, dobj.clip ) ) return FALSE;
	}
	DRW_SetDrawObject2XXX( &dobj, &dobj2, &dobj3, &dobj4, &dobj5 );

	switch(dobj.dparam2){
		default:
		case DRW_BLD2:
			if(dobj2.dest) DRW_DrawBMP_HH_Bld(&dobj2);
			if(dobj3.dest) DRW_DrawBMP_HH_Bld(&dobj3);
			DRW_DrawBMP_HHH_Bld(&dobj);
			break;	
		case DRW_AMI2:


			DRW_DrawBMP_HHH_Ami(&dobj);
			break;	
		case DRW_LPP2:		case DRW_LPP2+1:	case DRW_LPP2+2:
		case DRW_LPP2+3:	case DRW_LPP2+4:	case DRW_LPP2+5:	


			DRW_DrawBMP_HHH_Lpp(&dobj);
			break;
	}
	return ret;
}




BOOL DRW_DrawBMP_HH_Std( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	
	if( r==128 && g==128 && b==128 ){
		if( nuki==-1 ){	
			if( w==dest->sx && w==src->sx ){
				CopyMemory( dest_p, src_p, w*2*h );
			}else{
				for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx )
					CopyMemory( dest_p, src_p, w*2 );
			}
		}else if( nuki==-2 && src->alp ){
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			int		brev2;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w/4 ; x++, dest_p+=4, src_p+=4, alp_p+=4 ){
					if( *alp_p != 0 || *(alp_p+1) != 0 || *(alp_p+2) != 0 || *(alp_p+3) != 0 ){
						if( *alp_p == 255 && *(alp_p+1) == 255 && *(alp_p+2) == 255 && *(alp_p+3) == 255 ){
							*(double*)dest_p = *(double*)src_p;
						}else{
							brev2 = 31-(*alp_p>>3);
							*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
											| ((((*dest_p&rgb16.mask_g )>>5)*brev2)&rgb16.mask_g ) ) + *src_p;
							brev2 = 31-(*(alp_p+1)>>3);
							*(dest_p+1) = (WORD)( ((((*(dest_p+1)&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
												| ((((*(dest_p+1)&rgb16.mask_g )>>5)*brev2)&rgb16.mask_g ) ) + *(src_p+1);
							brev2 = 31-(*(alp_p+2)>>3);
							*(dest_p+2) = (WORD)( ((((*(dest_p+2)&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
												| ((((*(dest_p+2)&rgb16.mask_g )>>5)*brev2)&rgb16.mask_g ) ) + *(src_p+2);
							brev2 = 31-(*(alp_p+3)>>3);
							*(dest_p+3) = (WORD)( ((((*(dest_p+3)&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
												| ((((*(dest_p+3)&rgb16.mask_g )>>5)*brev2)&rgb16.mask_g ) ) + *(src_p+3);
						}
					}
				}
				for( x=0 ; x < w%4 ; x++, dest_p++, src_p++, alp_p++ ){
					if( *alp_p != 0 ){
						if( *alp_p == 255 ){
							*dest_p = *src_p;
						}else{
							brev2 = 31-(*alp_p>>3);
							*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
											| ((((*dest_p&rgb16.mask_g )>>5)*brev2)&rgb16.mask_g ) ) + *src_p;
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w/4 ; x++, dest_p+=4, src_p+=4 ){
					if( *src_p!=nuki && *(src_p+1)!=nuki && *(src_p+2)!=nuki && *(src_p+3)!=nuki ){
						*(double*)dest_p = *(double*)src_p;
					}else{
						if( *src_p!=nuki || *(src_p+1)!=nuki || *(src_p+2)!=nuki || *(src_p+3)!=nuki ){
							if( *src_p     != nuki )	*dest_p     = *src_p;
							if( *(src_p+1) != nuki )	*(dest_p+1) = *(src_p+1);
							if( *(src_p+2) != nuki )	*(dest_p+2) = *(src_p+2);
							if( *(src_p+3) != nuki )	*(dest_p+3) = *(src_p+3);
						}
					}
				}
				for( x=0 ; x < w%4 ; x++, dest_p++, src_p++ ){
					if( *src_p != nuki )	*dest_p = *src_p;
				}
			}
		}
	}else{		
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );
		if( nuki==-1 ){	
			if( (r==0xff && g==0xff && b==0xff) || (r==0x00 && g==0x00 && b==0x00) )
				return PRM_DrawRect_H( dest, dobj->dx, dobj->dy, w, h, NULL, r, g, b, DRW_NML );

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w/2 ; x++, dest_p+=2, src_p+=2 ){
					*(DWORD*)dest_p = bright_tbl[ *src_p ] | (bright_tbl[ *(src_p+1) ]<<16);
				}
				if(w%2){
					*dest_p = bright_tbl[*src_p];
					dest_p++;	src_p++;
				}
			}
		}else if( nuki==-2 && src->alp ){
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			int		brev2,blnd2;
			WORD	work;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w/2 ; x++, dest_p+=2, src_p+=2, alp_p+=2 ){
					if( *alp_p != 0 || *(alp_p+1) != 0 ){
						if( *alp_p == 255 && *(alp_p+1) == 255 ){
							*(DWORD*)dest_p = bright_tbl[*src_p] | (bright_tbl[*(src_p+1)]<<16);
						}else{
							blnd2 = (*alp_p>>3);
							brev2 = 31-blnd2;
							work = bright_tbl[ ((((*src_p&rgb16.mask_rb)<<5)/(blnd2+1))&rgb16.mask_rb) | ((((*src_p&rgb16.mask_g )<<5)/(blnd2+1))&rgb16.mask_g ) ];
							*dest_p = (WORD)( ((( (*dest_p&rgb16.mask_rb)*brev2 + (work&rgb16.mask_rb)*blnd2 )>>5)&rgb16.mask_rb)
											| ((( (*dest_p&rgb16.mask_g )*brev2 + (work&rgb16.mask_g )*blnd2 )>>5)&rgb16.mask_g ) );
							blnd2 = (*(alp_p+1)>>3);
							brev2 = 31-blnd2;
							work = bright_tbl[ ((((*(src_p+1)&rgb16.mask_rb)<<5)/(blnd2+1))&rgb16.mask_rb) | ((((*(src_p+1)&rgb16.mask_g )<<5)/(blnd2+1))&rgb16.mask_g ) ];
							*(dest_p+1) = (WORD)( ((( (*(dest_p+1)&rgb16.mask_rb)*brev2 + (work&rgb16.mask_rb)*blnd2 )>>5)&rgb16.mask_rb)
												| ((( (*(dest_p+1)&rgb16.mask_g )*brev2 + (work&rgb16.mask_g )*blnd2 )>>5)&rgb16.mask_g ) );
						}
					}
				}
				if(w%2){
					if( *alp_p == 255 ){
						*dest_p = bright_tbl[*src_p];
					}else{
						blnd2 = (*alp_p>>3);
						brev2 = 31-blnd2;
						work = bright_tbl[ ((((*src_p&rgb16.mask_rb)<<5)/(blnd2+1))&rgb16.mask_rb) | ((((*src_p&rgb16.mask_g )<<5)/(blnd2+1))&rgb16.mask_g ) ];
						*dest_p = (WORD)( ((( (*dest_p&rgb16.mask_rb)*brev2 + (work&rgb16.mask_rb)*blnd2 )>>5)&rgb16.mask_rb)
										| ((( (*dest_p&rgb16.mask_g )*brev2 + (work&rgb16.mask_g )*blnd2 )>>5)&rgb16.mask_g ) );
					}
					dest_p++;	src_p++;	alp_p++;
				}
			}
		}else{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w/2 ; x++, dest_p+=2, src_p+=2 ){
					if( *src_p!=nuki && *(src_p+1)!=nuki ){
						*(DWORD*)dest_p = bright_tbl[*src_p] | (bright_tbl[*(src_p+1)]<<16);
					}else if( *src_p != nuki ){
						*dest_p = bright_tbl[*src_p];
					}else if( *(src_p+1) != nuki ){
						*(dest_p+1) = bright_tbl[*(src_p+1)];
					}
				}
				if(w%2){
					if( *src_p != nuki )
						*dest_p = bright_tbl[*src_p];
					dest_p++;
					src_p++;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HH_Bld( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		w2 = w%2;
	int		w3 = w/2;
	int		blnd = dobj->dnum;
	int		blnd3 = blnd>>3, brev3 = 32-blnd3;

	if( blnd3==0  ){return 1;	}							
	if( blnd3==32 ){return DRW_DrawBMP_HH_Std( dobj ); }	


	
	if( r==128 && g==128 && b==128 ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w3 ; x++, dest_p+=2, src_p+=2 ){
					*(DWORD*)dest_p = (DWORD)( ((  ((*(DWORD*)dest_p&rgb16.mask_rbg)>>5)*brev3 + ((*(DWORD*)src_p&rgb16.mask_rbg)>>5)*blnd3 )&rgb16.mask_rbg)
											 | (( ( (*(DWORD*)dest_p&rgb16.mask_grb)    *brev3 +  (*(DWORD*)src_p&rgb16.mask_grb)*blnd3 )>>5)&rgb16.mask_grb) );
				}
				if(w2){
					*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*src_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
									| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*src_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
					dest_p++;
					src_p++;
				}
			}
		}else if( nuki==-2 && src->alp ){
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			int		brev2;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w3 ; x++, dest_p+=2, src_p+=2, alp_p+=2 ){
					if( *alp_p != 0 || *(alp_p+1) != 0 ){
						if( *alp_p == 255 && *(alp_p+1) == 255 ){
							*(DWORD*)dest_p = (DWORD)( ((  ((*(DWORD*)dest_p&rgb16.mask_rbg)>>5)*brev3 + ((*(DWORD*)src_p&rgb16.mask_rbg)>>5)*blnd3 )&rgb16.mask_rbg)
													 | (( ( (*(DWORD*)dest_p&rgb16.mask_grb)    *brev3 +  (*(DWORD*)src_p&rgb16.mask_grb)*blnd3 )>>5)&rgb16.mask_grb) );
						}else{
							brev2 = 31-((*alp_p*blnd3)>>8);
							*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
											| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
							brev2 = 31-((*(alp_p+1)*blnd3)>>8);
							*(dest_p+1) = (WORD)( (( ( (*(dest_p+1)&rgb16.mask_rb)*brev2 + (*(src_p+1)&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
												| (( ( (*(dest_p+1)&rgb16.mask_g )*brev2 + (*(src_p+1)&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
						}
					}
				}
				if(w2){
					if( *alp_p != 0 ){
						if( *alp_p == 255 ){
							*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*src_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
											| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*src_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
						}else{
							brev2 = 31-((*alp_p*blnd3)>>8);
							*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
											| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
						}
					}
					dest_p++;
					src_p++;
					alp_p++;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w3 ; x++, dest_p+=2, src_p+=2 ){
					if(*src_p!=nuki && *(src_p+1)!=nuki){
						*(DWORD*)dest_p = (DWORD)( ((  ((*(DWORD*)dest_p&rgb16.mask_rbg)>>5)*brev3 + ((*(DWORD*)src_p&rgb16.mask_rbg)>>5)*blnd3 )&rgb16.mask_rbg)
												 | (( ( (*(DWORD*)dest_p&rgb16.mask_grb)    *brev3 +  (*(DWORD*)src_p&rgb16.mask_grb)*blnd3 )>>5)&rgb16.mask_grb) );
					}else if( *src_p != nuki ){
						*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*src_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
										| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*src_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
					}else if( *(src_p+1) != nuki ){
						*(dest_p+1) = (WORD)( (( ( (*(dest_p+1)&rgb16.mask_rb)*brev3 + (*(src_p+1)&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
											| (( ( (*(dest_p+1)&rgb16.mask_g )*brev3 + (*(src_p+1)&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
					}
				}
				if(w2){
					if( *src_p != nuki ){
						*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*src_p&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
										| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*src_p&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
					}
					dest_p++;
					src_p++;
				}
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b ),work;
		
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (bright_tbl[*src_p]&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
									| (( ( (*dest_p&rgb16.mask_g )*brev3 + (bright_tbl[*src_p]&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
				}
			}
		}else if( nuki==-2 && src->alp ){
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			int		brev2,blnd2;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
					if( *alp_p != 0 ){
						if( *alp_p == 255 ){
							*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (bright_tbl[*src_p]&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
											| (( ( (*dest_p&rgb16.mask_g )*brev3 + (bright_tbl[*src_p]&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
						}else{
							blnd2 = *alp_p>>3;
							work = bright_tbl[ ((((*src_p&rgb16.mask_rb)<<5)/(blnd2+1))&rgb16.mask_rb) | ((((*src_p&rgb16.mask_g )<<5)/(blnd2+1))&rgb16.mask_g ) ];
							blnd2 = (blnd2*blnd)>>8;
							brev2 = 32-blnd2;
							*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (work&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
											| (( ( (*dest_p&rgb16.mask_g )*brev2 + (work&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p != nuki ){
						*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (bright_tbl[*src_p]&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
										| (( ( (*dest_p&rgb16.mask_g )*brev3 + (bright_tbl[*src_p]&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
					}
				}
			}
		}
	}

	return TRUE;
}



BOOL DRW_DrawBMP_HH_Add( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	DWORD	dcol,scol;
	DWORD	mask1=0, mask2=0;

	mask2 |= 0x00000001 << (rgb16.pos_r+5);
	mask2 |= 0x00000001 << (rgb16.pos_r);
	mask2 |= 0x00000001 << (rgb16.pos_g);
	mask1 = ~mask2;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					dcol = (*dest_p&mask1)+(*src_p&mask1);
					scol = dcol & mask2;
					*dest_p = (WORD)((scol-(scol>>5)) | dcol);
				}
			}
		}else if(nuki==-2){		
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
					if( *alp_p!=0 ){
						dcol = (*dest_p&mask1)+(*src_p&mask1);
						scol = dcol & mask2;
						*dest_p = (WORD)((scol-(scol>>5)) | dcol);
					}
				}
			}
		}else{		
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p!=nuki ){
						dcol = (*dest_p&mask1)+(*src_p&mask1);
						scol = dcol & mask2;
						*dest_p = (WORD)((scol-(scol>>5)) | dcol);
					}
				}
			}
		}
	}else{	
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );
		
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					dcol = (*dest_p&mask1)+(bright_tbl[*src_p]&mask1);
					scol = dcol & mask2;
					*dest_p = (WORD)((scol-(scol>>5)) | dcol);
				}
			}
		}else if(nuki==-2){		
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			int		blnd2;
			WORD	work;
			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
					if( *alp_p!=0 ){
						if( *alp_p==255 ){
							dcol = (*dest_p&mask1)+(bright_tbl[*src_p]&mask1);
							scol = dcol & mask2;
							*dest_p = (WORD)((scol-(scol>>5)) | dcol);
						}else{
							blnd2 = *alp_p>>3;
							work = bright_tbl[ ((((*src_p&rgb16.mask_rb)<<5)/(blnd2+1))&rgb16.mask_rb) | ((((*src_p&rgb16.mask_g )<<5)/(blnd2+1))&rgb16.mask_g ) ];
							work = (WORD)((((work&rgb16.mask_rb)*blnd2)>>5)&rgb16.mask_rb) | ((((work&rgb16.mask_g )*blnd2)>>5)&rgb16.mask_g );

							dcol = (*dest_p&mask1)+(work&mask1);
							scol = dcol & mask2;
							*dest_p = (WORD)((scol-(scol>>5)) | dcol);
						}
					}
				}
			}
		}else{		
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p!=nuki ){
						dcol = (*dest_p&mask1)+(bright_tbl[*src_p]&mask1);
						scol = dcol & mask2;
						*dest_p = (WORD)((scol-(scol>>5)) | dcol);
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HH_Sub( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		rr,gg,bb;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					rr = (*dest_p&rgb16.mask_r) - (*src_p&rgb16.mask_r);
					gg = (*dest_p&rgb16.mask_g) - (*src_p&rgb16.mask_g);
					bb = (*dest_p&rgb16.mask_b) - (*src_p&rgb16.mask_b);
					if( rr<0 ) rr = 0;
					if( gg<0 ) gg = 0;
					if( bb<0 ) bb = 0;
					*dest_p = (rr|gg|bb);
				}
			}
		}else if(nuki==-2){		
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			char	*sub_tbl = SubTable+255;
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
					if( *alp_p!=0 ){
						rr = (*dest_p&rgb16.mask_r) - (*src_p&rgb16.mask_r);
						gg = (*dest_p&rgb16.mask_g) - (*src_p&rgb16.mask_g);
						bb = (*dest_p&rgb16.mask_b) - (*src_p&rgb16.mask_b);
						if( rr<0 ) rr = 0;
						if( gg<0 ) gg = 0;
						if( bb<0 ) bb = 0;
						*dest_p = (rr|gg|bb);
					}
				}
			}
		}else{		
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p!=nuki ){
						rr = (*dest_p&rgb16.mask_r) - (*src_p&rgb16.mask_r);
						gg = (*dest_p&rgb16.mask_g) - (*src_p&rgb16.mask_g);
						bb = (*dest_p&rgb16.mask_b) - (*src_p&rgb16.mask_b);
						if( rr<0 ) rr = 0;
						if( gg<0 ) gg = 0;
						if( bb<0 ) bb = 0;
						*dest_p = (rr|gg|bb);
					}
				}
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );
		
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					rr = (*dest_p&rgb16.mask_r) - (bright_tbl[*src_p]&rgb16.mask_r);
					gg = (*dest_p&rgb16.mask_g) - (bright_tbl[*src_p]&rgb16.mask_g);
					bb = (*dest_p&rgb16.mask_b) - (bright_tbl[*src_p]&rgb16.mask_b);
					if( rr<0 ) rr = 0;
					if( gg<0 ) gg = 0;
					if( bb<0 ) bb = 0;
					*dest_p = (rr|gg|bb);
				}
			}
		}else if(nuki==-2){		
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			int		blnd2;
			WORD	work;
			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
					if( *alp_p!=0 ){
						if( *alp_p==255 ){
							rr = (*dest_p&rgb16.mask_r) - (bright_tbl[*src_p]&rgb16.mask_r);
							gg = (*dest_p&rgb16.mask_g) - (bright_tbl[*src_p]&rgb16.mask_g);
							bb = (*dest_p&rgb16.mask_b) - (bright_tbl[*src_p]&rgb16.mask_b);
							if( rr<0 ) rr = 0;
							if( gg<0 ) gg = 0;
							if( bb<0 ) bb = 0;
							*dest_p = (rr|gg|bb);
						}else{
							blnd2 = *alp_p>>3;
							work = bright_tbl[ ((((*src_p&rgb16.mask_rb)<<5)/(blnd2+1))&rgb16.mask_rb) | ((((*src_p&rgb16.mask_g )<<5)/(blnd2+1))&rgb16.mask_g ) ];
							work = (WORD)((((work&rgb16.mask_rb)*blnd2)>>5)&rgb16.mask_rb) | ((((work&rgb16.mask_g )*blnd2)>>5)&rgb16.mask_g );

							rr = (*dest_p&rgb16.mask_r) - (work&rgb16.mask_r);
							gg = (*dest_p&rgb16.mask_g) - (work&rgb16.mask_g);
							bb = (*dest_p&rgb16.mask_b) - (work&rgb16.mask_b);
							if( rr<0 ) rr = 0;
							if( gg<0 ) gg = 0;
							if( bb<0 ) bb = 0;
							*dest_p = (rr|gg|bb);
						}
					}
				}
			}
		}else{		
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p!=nuki ){
						rr = (*dest_p&rgb16.mask_r) - (bright_tbl[*src_p]&rgb16.mask_r);
						gg = (*dest_p&rgb16.mask_g) - (bright_tbl[*src_p]&rgb16.mask_g);
						bb = (*dest_p&rgb16.mask_b) - (bright_tbl[*src_p]&rgb16.mask_b);
						if( rr<0 ) rr = 0;
						if( gg<0 ) gg = 0;
						if( bb<0 ) bb = 0;
						*dest_p = (rr|gg|bb);
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HH_Mul( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = F2H_TblRW[ BrightTable_TB[ H2F_Tbl[*src_p].r ][ H2F_Tbl[*dest_p].r ] ]
							| F2H_TblGW[ BrightTable_TB[ H2F_Tbl[*src_p].g ][ H2F_Tbl[*dest_p].g ] ]
							| F2H_TblBW[ BrightTable_TB[ H2F_Tbl[*src_p].b ][ H2F_Tbl[*dest_p].b ] ];
				}
			}
		}else if(nuki==-2){		
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			char	*sub_tbl = SubTable+255;
			char	*alp_tbl;
			WORD	br;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
					if( *alp_p!=0 ){
						if( *alp_p==255 ){
							*dest_p = F2H_TblRW[ BrightTable_TB[ H2F_Tbl[*src_p].r ][ H2F_Tbl[*dest_p].r ] ]
									| F2H_TblGW[ BrightTable_TB[ H2F_Tbl[*src_p].g ][ H2F_Tbl[*dest_p].g ] ]
									| F2H_TblBW[ BrightTable_TB[ H2F_Tbl[*src_p].b ][ H2F_Tbl[*dest_p].b ] ];
						}else{
							alp_tbl = BlendTable[ *alp_p ];
							br = (256-*alp_p)>>1;

							*dest_p = F2H_TblRW[ BrightTable_TB[ alp_tbl[H2F_Tbl[*src_p].r]+br ][ H2F_Tbl[*dest_p].r ] ]
									| F2H_TblGW[ BrightTable_TB[ alp_tbl[H2F_Tbl[*src_p].g]+br ][ H2F_Tbl[*dest_p].g ] ]
									| F2H_TblBW[ BrightTable_TB[ alp_tbl[H2F_Tbl[*src_p].b]+br ][ H2F_Tbl[*dest_p].b ] ];
						}
					}
				}
			}
		}else{		
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p!=nuki ){
						*dest_p = F2H_TblRW[ BrightTable_TB[ H2F_Tbl[*src_p].r ][ H2F_Tbl[*dest_p].r ] ]
								| F2H_TblGW[ BrightTable_TB[ H2F_Tbl[*src_p].g ][ H2F_Tbl[*dest_p].g ] ]
								| F2H_TblBW[ BrightTable_TB[ H2F_Tbl[*src_p].b ][ H2F_Tbl[*dest_p].b ] ];
					}
				}
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );
		
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = F2H_TblRW[ BrightTable_TB[ H2F_Tbl[bright_tbl[*src_p]].r ][ H2F_Tbl[*dest_p].r ] ]
							| F2H_TblGW[ BrightTable_TB[ H2F_Tbl[bright_tbl[*src_p]].g ][ H2F_Tbl[*dest_p].g ] ]
							| F2H_TblBW[ BrightTable_TB[ H2F_Tbl[bright_tbl[*src_p]].b ][ H2F_Tbl[*dest_p].b ] ];
				}
			}
		}else if(nuki==-2){		
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			int		blnd2;
			WORD	work;
			char	*alp_tbl;
			WORD	br;
			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
					if( *alp_p!=0 ){
						if( *alp_p==255 ){
							*dest_p = F2H_TblRW[ BrightTable_TB[ H2F_Tbl[bright_tbl[*src_p]].r ][ H2F_Tbl[*dest_p].r ] ]
									| F2H_TblGW[ BrightTable_TB[ H2F_Tbl[bright_tbl[*src_p]].g ][ H2F_Tbl[*dest_p].g ] ]
									| F2H_TblBW[ BrightTable_TB[ H2F_Tbl[bright_tbl[*src_p]].b ][ H2F_Tbl[*dest_p].b ] ];
						}else{
							blnd2 = *alp_p>>3;
							work = bright_tbl[ ((((*src_p&rgb16.mask_rb)<<5)/(blnd2+1))&rgb16.mask_rb) | ((((*src_p&rgb16.mask_g )<<5)/(blnd2+1))&rgb16.mask_g ) ];
							work = (WORD)((((work&rgb16.mask_rb)*blnd2)>>5)&rgb16.mask_rb) | ((((work&rgb16.mask_g )*blnd2)>>5)&rgb16.mask_g );

							alp_tbl = BlendTable[ *alp_p ];
							br = (256-*alp_p)>>1;

							*dest_p = F2H_TblRW[ BrightTable_TB[ alp_tbl[H2F_Tbl[work].r]+br ][ H2F_Tbl[*dest_p].r ] ]
									| F2H_TblGW[ BrightTable_TB[ alp_tbl[H2F_Tbl[work].g]+br ][ H2F_Tbl[*dest_p].g ] ]
									| F2H_TblBW[ BrightTable_TB[ alp_tbl[H2F_Tbl[work].b]+br ][ H2F_Tbl[*dest_p].b ] ];
						}
					}
				}
			}
		}else{		
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p!=nuki ){
						*dest_p = F2H_TblRW[ BrightTable_TB[ H2F_Tbl[bright_tbl[*src_p]].r ][ H2F_Tbl[*dest_p].r ] ]
								| F2H_TblGW[ BrightTable_TB[ H2F_Tbl[bright_tbl[*src_p]].g ][ H2F_Tbl[*dest_p].g ] ]
								| F2H_TblBW[ BrightTable_TB[ H2F_Tbl[bright_tbl[*src_p]].b ][ H2F_Tbl[*dest_p].b ] ];
					}
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_HH_Ami( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		mesh = dobj->dnum;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt;
	mesh_pt = MeshFadePatern[mesh/16];

	if( mesh<=0   )	return 1;							
	if( mesh>=256 )	return DRW_DrawBMP_HH_Std( dobj );	

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( linemask2[x & 3] ){
						*dest_p = *src_p;
					}
				}
			}
		}else if( nuki==-2 ){		
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			int		brev2;

			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
					if( linemask2[x & 3] ){
						if( *alp_p != 0 ){
							if( *alp_p == 255 ){
								*dest_p = *src_p;
							}else{
								brev2 = 31-(*alp_p>>3);
								*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2)>>5 )&rgb16.mask_rb)
												| (( ( (*dest_p&rgb16.mask_g )*brev2)>>5 )&rgb16.mask_g ) ) + *src_p;
							}
						}
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p != nuki && linemask2[x & 3] ){
						*dest_p = *src_p;
					}
				}
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( linemask2[x & 3] ){
						*dest_p = bright_tbl[*src_p];
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				linemask2 = linemask[y & 3];
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p != nuki && linemask2[x & 3] ){
						*dest_p = bright_tbl[*src_p];
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HH_Neg( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);

	
	if( (r==128 && g==128 && b==128) ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = ~*src_p;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p != nuki ){
						*dest_p = ~*src_p;
					}
				}
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = ~bright_tbl[*src_p];
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p != nuki ){
						*dest_p = ~bright_tbl[*src_p];
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HH_Lcf( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		rate = dobj->dnum;
	int		line = dobj->dparam-DRW_LCF2;
	int		rate_max[3];
	int		blnd, brev;
	int		blnd2, brev2;
	int		rate2;

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
			blnd2 = blnd>>3;
			brev2 = 32-blnd2;
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
								| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
			}
		}
		for( ; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
		}
		break;

	case DO:
		rate = (DispHeight+255)*rate/256;
		rate2 = STD_Limit( -(rate-255), 0, 255 );
		rate_max[0] = STD_Limit( rate-255, 0, h );
		rate_max[1] = STD_Limit( rate,     0, h );
		for( y=0; y < rate_max[0] ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
		}
		blnd = 256 - rate2;
		brev =   0 + rate2;
		for( ; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev, blnd--, brev++ ){
			blnd2 = blnd>>3;
			brev2 = 32-blnd2;
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
								| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
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
			for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p++, blnd++, brev-- ){
				blnd2 = blnd>>3;
				brev2 = 32-blnd2;
				*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
								| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
			}
			for( ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
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
			for( x=0 ; x < rate_max[0] ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
			for( ; x < rate_max[1] ; x++, dest_p++, src_p++, blnd--, brev++ ){
				blnd2 = blnd>>3;
				brev2 = 32-blnd2;
				*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
								| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
			}
			dest_p += (w-x);
			src_p  += (w-x);
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
			for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p++, blnd+=2, brev-=2 ){
				blnd2 = blnd>>3;
				brev2 = 32-blnd2;
				*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
								| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
			}
			for( ; x < w/2 ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}

			for( ; x > rate_max[1] ; x--, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
			for( ; x > rate_max[0] ; x--, dest_p++, src_p++, blnd-=2, brev+=2){
				blnd2 = blnd>>3;
				brev2 = 32-blnd2;
				*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
								| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
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

			for( x=0 ; x < rate_max[0] ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
			for( ; x < rate_max[1] ; x++, dest_p++, src_p++, blnd-=2, brev+=2 ){
				blnd2 = blnd>>3;
				brev2 = 32-blnd2;
				*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
								| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
			}
			dest_p += (w/2-x)*2;
			src_p  += (w/2-x)*2;

			for( ; x > rate_max[0] ; x--, dest_p++, src_p++, blnd+=2, brev-=2){
				blnd2 = blnd>>3;
				brev2 = 32-blnd2;
				*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
								| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
			}
			for( ; x > 0 ; x--, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
		}
		break;
	}
	return TRUE;
}


BOOL DRW_DrawBMP_HH_Lpp( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		rate = dobj->dnum;
	int		line = dobj->dparam-DRW_LPP2;
	int		rate_max[3];

	switch(line){
	case UP:
		if(nuki==-1){
			rate = 46*(256-rate)/256;
			rate_max[0] = STD_Limit( rate*16-240, 0, h );
			rate_max[1] = STD_Limit( rate*16,     0, h );

			dest_p+=rate_max[0]*dest->sx;
			src_p +=rate_max[0]*src->sx;

			for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev ){
				if( (rate-(y>>4)) <= (y&0xf) ){
					for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
						*dest_p = *src_p;
					}
				}else{
					dest_p+=w;
					src_p+=w;
				}
			}
			for( ; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = *src_p;
				}
			}
		}else if( nuki==-2 ){
			char	*alp_p = src->alp + DRW_GetStartPointerSrc1( dobj );
			int		brev2;

			rate = 46*(256-rate)/256;
			rate_max[0] = STD_Limit( rate*16-240, 0, h );
			rate_max[1] = STD_Limit( rate*16,     0, h );

			dest_p+=rate_max[0]*dest->sx;
			src_p +=rate_max[0]*src->sx;
			alp_p +=rate_max[0]*src->sx;

			for( y=rate_max[0]; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				if( (rate-(y>>4)) <= (y&0xf) ){
					for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
						if(*alp_p){
							if( *alp_p == 255 ){
								*dest_p = *src_p;
							}else{
								brev2 = 31-((*alp_p)>>3);
								*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
												| ((((*dest_p&rgb16.mask_g )>>5)*brev2)&rgb16.mask_g ) ) + *src_p;
							}
						}
					}
				}else{
					dest_p+=w;
					src_p+=w;
					alp_p+=w;
				}
			}
			for( ; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
					if(*alp_p){
						if( *alp_p == 255 ){
							*dest_p = *src_p;
						}else{
							brev2 = 31-((*alp_p)>>3);
							*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev2)>>5)&rgb16.mask_rb)
											| ((((*dest_p&rgb16.mask_g )>>5)*brev2)&rgb16.mask_g ) ) + *src_p;
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
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
		}
		for( ; y < rate_max[1] ; y++, dest_p+=drev, src_p+=srev ){
			if( (rate-(y>>4)) > (y&0xf) ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = *src_p;
				}
			}else{
				dest_p+=w;
				src_p+=w;
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
			for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p++ ){
				if( (rate-(x>>4)) <= (x&0xf) ){
					*dest_p = *src_p;
				}
			}
			for( ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
		}
		break;
	case LE:
		rate = 56*rate/256;
		rate_max[0] = min( w, rate*16-240 );
		rate_max[1] = min( w, rate*16 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < rate_max[0] ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
			for( ; x < rate_max[1] ; x++, dest_p++, src_p++ ){
				if( (rate-(x>>4)) > (x&0xf) ){
					*dest_p = *src_p;
				}
			}
			dest_p+=(w-x);
			src_p +=(w-x);
		}
		break;
	case CE:
		rate = (20+16)*(256-rate)/256;
		rate_max[0] = STD_Limit( rate*16-240, 0, w/2 );
		rate_max[1] = STD_Limit( rate*16,     0, w/2 );
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			dest_p+=rate_max[0];
			src_p +=rate_max[0];
			for( x=rate_max[0] ; x < rate_max[1] ; x++, dest_p++, src_p++ ){
				if( (rate-(x>>4)) <= (x&0xf) ){
					*dest_p = *src_p;
				}
			}
			for( ; x < w/2 ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
			for( ; x > rate_max[1] ; x--, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
			for( ; x > rate_max[0] ; x--, dest_p++, src_p++ ){
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
			for( x=0 ; x < rate_max[0] ; x++, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
			for( ; x < rate_max[1] ; x++, dest_p++, src_p++ ){
				if( (rate-(x>>4)) > (x&0xf) ){
					*dest_p = *src_p;
				}
			}
			dest_p+=(w/2-x)*2;
			src_p +=(w/2-x)*2;
			for( ; x > rate_max[0] ; x--, dest_p++, src_p++ ){
				if( (rate-(x>>4)) > ((x-1)&0xf) ){
					*dest_p = *src_p;
				}
			}
			for( ; x > 0 ; x--, dest_p++, src_p++ ){
				*dest_p = *src_p;
			}
		}
		break;
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HH_Dia( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	int		xinc = XINC(dobj);
	int		rate = dobj->dnum;
	int		bflag = (dobj->dparam-DRW_DIA2)/3;
	int		type = (dobj->dparam-DRW_DIA2)%3;
	int		x2, y2;
	int		mask=0x3f;
	int		harf=32;
	int		rate2;
	int		rate3=rate;

	rate = (96>>type)*rate/256;

	mask >>= type;
	harf >>= type;
	rate2 = rate-harf;

	if(bflag){
		int		blnd2,brev2;

		blnd2 = 2 + 29*rate3*rate3/(256*256);
		brev2 = 32-blnd2;

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			y2 = abs( (y&mask)-harf ) + rate2;
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				x2 = abs( (x&mask)-harf );
				if( x2 < y2 ){





					*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev2 + (*src_p&rgb16.mask_rb)*blnd2 )>>5 )&rgb16.mask_rb)
									| (( ( (*dest_p&rgb16.mask_g )*brev2 + (*src_p&rgb16.mask_g )*blnd2 )>>5 )&rgb16.mask_g ) );
				}
			}
		}
	}else{
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			y2 = abs( (y&mask)-harf ) + rate2;
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				x2 = abs( (x&mask)-harf );
				if( x2 < y2 ){
					*dest_p = *src_p;
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HH_Lst( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);

	int		lst = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		xx;

	lst = lst*w/640;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					CopyMemory( dest_p, sss, w*2 );	
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss++ ){
						if( *sss != nuki ){
							*dest_p = *sss;
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss++ ){
						*dest_p = bright_tbl[ *sss ];
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
					for( x=0 ; x < w ; x++, dest_p++, sss++ ){
						if(*(long*)sss!=nuki){
							*dest_p = bright_tbl[ *sss ];
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



BOOL DRW_DrawBMP_HH_Lbl( DRAW_OBJECT *dobj )
{
	int		x,y;
	int		r = dobj->r, g = dobj->g, b = dobj->b;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		lst = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		xx;
	int		brev3 = lst>>3, blnd3 = 32-brev3;
		
	lst = lst*w/640;

	
	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss++ ){
						*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*sss&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
										| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*sss&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );

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
					for( x=0 ; x < w ; x++, dest_p++, sss++ ){
						if( *sss != nuki ){
							*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*sss&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
											| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*sss&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
						}
					}
				}else{
					dest_p+=w;
				}
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=src->sx ){
				xx = lst*SIN( y*480/h +cnt)/4096;
				if( (xx + y*src->sx >= 0) && (xx+w + y*src->sx < src->sx*h) ) {
					sss = src_p+xx;
					for( x=0 ; x < w ; x++, dest_p++, sss++ ){
						*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*sss&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
										| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*sss&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
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
					for( x=0 ; x < w ; x++, dest_p++, sss++ ){
						if(*(long*)sss!=nuki){
							*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*sss&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
											| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*sss&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
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



static BOOL DRW_DrawZOOM_HH_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy );

BOOL DRW_DrawBMP_HH_Sui( DRAW_OBJECT *dobj )
{
	int			w = dobj->dw;
	int			h = dobj->dh;
	int			wav = dobj->dnum&0x00ff;
	int			cnt = (dobj->dnum&0xff00)>>8;
	int			x,y, x1,x2, y1,y2;
	int			w2=w/8, h2=h/8;
	int			wavx = 40 * wav/256;
	int			wavy = 30 * wav/256;
	int			sx, sy;
	int			xx,yy,xxx,yyy;
	int			xinc = XINC(dobj);
	int			yinc = YINC(dobj);
	DRAW_OBJECT	dobj2;

	if( dobj->dnum==0 ){
		return DRW_DrawBMP_HH_Std( dobj );
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

			dobj2 = DRW_SetDrawObject(	1,dobj->dest, xx*w2,    yy*h2,    w2,       h2,
										1,dobj->src1, xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_NML, 0,0, dobj->rparam );
			DRW_DrawZOOM_HH_Std( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0 );
		}
	}

	return TRUE;
}




static BOOL DRW_DrawZOOM_HH_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy );

BOOL DRW_DrawBMP_HH_Sbl( DRAW_OBJECT *dobj )
{
	int			w = dobj->dw;
	int			h = dobj->dh;
	int			wav = dobj->dnum&0x00ff;
	int			cnt = (dobj->dnum&0xff00)>>8;
	int			x,y, x1,x2, y1,y2;
	int			w2=w/8, h2=h/8;
	int			wavx = 40 * wav/256;
	int			wavy = 30 * wav/256;
	int			sx, sy;
	int			xx,yy,xxx,yyy;
	int			xinc = XINC(dobj);
	int			yinc = YINC(dobj);
	DRAW_OBJECT	dobj2;

	if( dobj->dnum==0 ){
		return DRW_DrawBMP_HH_Std( dobj );
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

			dobj2 = DRW_SetDrawObject(	1,dobj->dest, xx*w2,    yy*h2,    w2,       h2,
										1,dobj->src1, xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_BLD(32), 0,0, dobj->rparam );
			DRW_DrawZOOM_HH_Bld( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0 );
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_HH( DRAW_OBJECT dobj )
{
	BOOL	ret=TRUE;
	BMP_H	*dest= (BMP_H *)dobj.dest;
	BMP_H	*src = (BMP_H *)dobj.src1;

	
	if( dest->buf==NULL ) {
		DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE;
	}
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }

	
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

		case DRW_NML:	DRW_DrawBMP_HH_Std( &dobj );	break;	
		case DRW_BLD2:	DRW_DrawBMP_HH_Bld( &dobj );	break;	
		case DRW_ADD:	DRW_DrawBMP_HH_Add( &dobj );	break;	
		case DRW_SUB:	DRW_DrawBMP_HH_Sub( &dobj );	break;	
		case DRW_MUL:	DRW_DrawBMP_HH_Mul( &dobj );	break;	
		case DRW_AMI2:	DRW_DrawBMP_HH_Ami( &dobj );	break;	
		case DRW_NEG:	DRW_DrawBMP_HH_Neg(	&dobj );	break;	


		case DRW_SUI2:	DRW_DrawBMP_HH_Sui(	&dobj );	break;	
		case DRW_SBL2:	DRW_DrawBMP_HH_Sbl(	&dobj );	break;	


		case DRW_LST2:	DRW_DrawBMP_HH_Lst(	&dobj );	break;	
		case DRW_LBL2:	DRW_DrawBMP_HH_Lbl(	&dobj );	break;	

		case DRW_LCF2:		case DRW_LCF2+1:	case DRW_LCF2+2:	
		case DRW_LCF2+3:	case DRW_LCF2+4:	case DRW_LCF2+5:
			DRW_DrawBMP_HH_Lcf(	&dobj );
			break;

		case DRW_LPP2:		case DRW_LPP2+1:	case DRW_LPP2+2:	
		case DRW_LPP2+3:	case DRW_LPP2+4:	case DRW_LPP2+5:
			DRW_DrawBMP_HH_Lpp( &dobj );
			break;

		case DRW_DIA2:	case DRW_DIA2+1:	case DRW_DIA2+2:
		case DRW_DIO2:	case DRW_DIO2+1:	case DRW_DIO2+2:
			DRW_DrawBMP_HH_Dia( &dobj );
			break;
	}
	return ret;
}



BOOL DRW_DrawBMP_HB_Std( DRAW_OBJECT *dobj, BMP_BH *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	WORD	*spal = src->pal;
	int		xinc = XINC(dobj);
	int		w2 = w/2;
	int		wa = w%2;

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w2 ; x++, dest_p+=2, src_p+=2 ){
				*(DWORD*)dest_p = spal[*src_p] | (spal[*(src_p+1)]<<16);	
			}
			if(wa){
				*dest_p = spal[*src_p];
				dest_p++, src_p++;
			}
		}
	}else if( dobj->nuki==-2 ){	
		char	*alp_p  = src->alp + DRW_GetStartPointerSrc1( dobj );
		int		alp,rev;

		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev, alp_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p+=xinc, alp_p+=xinc ){
				if( *alp_p != 0 ){
					if( *alp_p == 255 ){
						*dest_p =  spal[ *src_p ];
					}else{
						alp = *alp_p>>3;
						rev = 32-alp;
						*dest_p = (WORD)( ((( (*dest_p&rgb16.mask_rb)*rev + (spal[*src_p]&rgb16.mask_rb)*alp )>>5)&rgb16.mask_rb)
										| ((( (*dest_p&rgb16.mask_g )*rev + (spal[*src_p]&rgb16.mask_g )*alp )>>5)&rgb16.mask_g ) );
					}
				}
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w2 ; x++, dest_p+=2, src_p+=2 ){
				if(*src_p!=nuki && *(src_p+1)!=nuki){	
					*(DWORD*)dest_p = (spal[*(src_p+1)]<<16) + spal[*src_p];	
				}else if( *src_p!=nuki ){			
					*(dest_p) = spal[*src_p];		
				}else if( *(src_p+1)!=nuki ){		
					*(dest_p+1) = spal[*(src_p+1)];	
				}
			}
			if(wa){
				if(*src_p!=nuki){
					*dest_p = spal[*src_p];	
				}
				dest_p++, src_p++;
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HB_Bld( DRAW_OBJECT *dobj, BMP_BH *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	WORD	*spal = src->pal;
	int		xinc = XINC(dobj);
	int		blnd = dobj->dnum;

	if( blnd==BLD_0P  )return 1;								
	if( blnd==BLD_100P)return DRW_DrawBMP_HB_Std( dobj, src );	

	if( blnd==BLD_50P ){
		int		w2 = w/2;
		int		w3 = w%2;
		int		shift=1;
		DWORD	dmask;
		WORD	mask=0, mask2=0x1;

		mask |= mask2<<rgb16.pos_r;
		mask |= mask2<<rgb16.pos_g;
		mask |= mask2<<rgb16.pos_b;
		mask = ~mask;
		dmask = (mask<<16) | mask;

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w2 ; x++, dest_p+=2, src_p+=2 ){
					*(DWORD*)dest_p = ((*(DWORD*)dest_p&dmask)>>shift) + ((spal[*(src_p+1)]<<16)|spal[*src_p]);
				}
				if(w3){
					*dest_p = ((*dest_p&mask)>>shift) + spal[*src_p];
					dest_p++;	src_p++;
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w2 ; x++, dest_p+=2, src_p+=2 ){
					if(*src_p!=nuki && *(src_p+1)!=nuki){	
						*(DWORD*)dest_p = ((*(DWORD*)dest_p&dmask)>>shift) + ((spal[*(src_p+1)]<<16)|spal[*src_p]);
					}else if(*src_p !=nuki){	
						*(dest_p  ) = ((*(dest_p  )&mask)>>shift) + spal[*src_p];
					}else if(*(src_p+1)!=nuki){	
						*(dest_p+1) = ((*(dest_p+1)&mask)>>shift) + spal[*(src_p+1)];
					}
				}
				if(w3){
					*dest_p = ((*dest_p&mask)>>shift) + spal[*src_p];
					dest_p++;	src_p++;
				}
			}
		}
	}else{
		int		brev = 256-blnd;
		int		blnd3 = blnd>>3, brev3 = 32-blnd3;

		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3)>>5 )&rgb16.mask_rb)
									| (( ( (*dest_p&rgb16.mask_g )*brev3)>>5 )&rgb16.mask_g ) ) + spal[ *src_p ];
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					if( *src_p!=nuki ){
						*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3)>>5 )&rgb16.mask_rb)
										| (( ( (*dest_p&rgb16.mask_g )*brev3)>>5 )&rgb16.mask_g ) ) + spal[ *src_p ];
					}
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HB_Add( DRAW_OBJECT *dobj, BMP_BH *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	WORD	*spal = src->pal;
	int		xinc = XINC(dobj);

	int		w2 = w/2;
	int		w2a = w%2;
	DWORD	scol1,dcol1;
	DWORD	scol2,dcol2;
	DWORD	mask1, mask2=0;

	mask2 |= 0x00000001 << (rgb16.pos_r+5);
	mask2 |= 0x00000001 << (rgb16.pos_r);
	mask2 |= 0x00000001 << (rgb16.pos_g);
	mask1 = ~mask2;

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w2 ; x++, dest_p+=2, src_p+=2 ){
				dcol1 = (*(dest_p  )&mask1) + (spal[*(src_p  )]&mask1);
				dcol2 = (*(dest_p+1)&mask1) + (spal[*(src_p+1)]&mask1);
				scol1 = dcol1 & mask2;
				scol2 = dcol2 & mask2;
				*(DWORD*)dest_p = ((scol1-(scol1>>5)) | dcol1) | (((scol2-(scol2>>5)) | dcol2)<<16);
			}
			if(w2a){
				dcol1 = (*(dest_p  )&mask1) + (spal[*(src_p  )]&mask1);
				scol1 = dcol1 & mask2;
				*dest_p = (WORD)( (scol1-(scol1>>5)) | dcol1 );

				dest_p++, src_p++;
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w2 ; x++, dest_p+=2, src_p+=2 ){
				if(*(src_p)!=nuki && *(src_p+1)!=nuki){	
					dcol1 = (*(dest_p  )&mask1) + (spal[*(src_p  )]&mask1);
					dcol2 = (*(dest_p+1)&mask1) + (spal[*(src_p+1)]&mask1);
					scol1 = dcol1 & mask2;
					scol2 = dcol2 & mask2;
					*(DWORD*)dest_p = ((scol1-(scol1>>5)) | dcol1) | (((scol2-(scol2>>5)) | dcol2)<<16);
				}else if(*(src_p) !=nuki){			
					dcol1 = (*(dest_p  )&mask1) + (spal[*(src_p  )]&mask1);
					scol1 = dcol1 & mask2;
					*dest_p = (WORD)( (scol1-(scol1>>5)) | dcol1 );
				}else if(*(src_p+1)!=nuki){			
					dcol2 = (*(dest_p+1)&mask1) + (spal[*(src_p+1)]&mask1);
					scol2 = dcol2 & mask2;
					*(dest_p+1) = (WORD)( (scol2-(scol2>>5)) | dcol2 );
				}
			}
			if(w2a){
				if(*src_p !=nuki){			
					dcol1 = (*dest_p&mask1) + (spal[*src_p]&mask1);
					scol1 = dcol1 & mask2;
					*dest_p = (WORD)( (scol1-(scol1>>5)) | dcol1 );
				}

				dest_p++, src_p++;
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HB_Sub( DRAW_OBJECT *dobj, BMP_BH *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	WORD	*spal = src->pal;
	int		xinc = XINC(dobj);
	int		r1,g1,b1;

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				r1 = (*dest_p&rgb16.mask_r) - (spal[*src_p]&rgb16.mask_r);
				g1 = (*dest_p&rgb16.mask_g) - (spal[*src_p]&rgb16.mask_g);
				b1 = (*dest_p&rgb16.mask_b) - (spal[*src_p]&rgb16.mask_b);
				if( r1<0 ) r1 = 0;
				if( g1<0 ) g1 = 0;
				if( b1<0 ) b1 = 0;
				*dest_p = (r1|g1|b1);
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				if( *src_p!=nuki ){
					r1 = (*dest_p&rgb16.mask_r) - (spal[*src_p]&rgb16.mask_r);
					g1 = (*dest_p&rgb16.mask_g) - (spal[*src_p]&rgb16.mask_g);
					b1 = (*dest_p&rgb16.mask_b) - (spal[*src_p]&rgb16.mask_b);
					if( r1<0 ) r1 = 0;
					if( g1<0 ) g1 = 0;
					if( b1<0 ) b1 = 0;
					*dest_p = (r1|g1|b1);
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_HB_Mul( DRAW_OBJECT *dobj, BMP_BH *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	WORD	*spal = src->pal;
	int		xinc = XINC(dobj);

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = F2H_TblRW[ BrightTable_TB[ H2F_Tbl[spal[*src_p]].r ][ H2F_Tbl[*dest_p].r ] ]
						| F2H_TblGW[ BrightTable_TB[ H2F_Tbl[spal[*src_p]].g ][ H2F_Tbl[*dest_p].g ] ]
						| F2H_TblBW[ BrightTable_TB[ H2F_Tbl[spal[*src_p]].b ][ H2F_Tbl[*dest_p].b ] ];
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				if( *src_p!=nuki ){
					*dest_p = F2H_TblRW[ BrightTable_TB[ H2F_Tbl[spal[*src_p]].r ][ H2F_Tbl[*dest_p].r ] ]
							| F2H_TblGW[ BrightTable_TB[ H2F_Tbl[spal[*src_p]].g ][ H2F_Tbl[*dest_p].g ] ]
							| F2H_TblBW[ BrightTable_TB[ H2F_Tbl[spal[*src_p]].b ][ H2F_Tbl[*dest_p].b ] ];
				}
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawBMP_HB_Ami( DRAW_OBJECT *dobj, BMP_BH *src )
{
	int		x,y;
	int		w = dobj->dw;
	int		h = dobj->dh;
	int		nuki = dobj->nuki;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		drev = dest->sx - w;
	int		srev = src->sx * YINC(dobj) - w * XINC(dobj);
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj );
	WORD	*spal = src->pal;
	int		xinc = XINC(dobj);
	int		mesh = dobj->dnum;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt = MeshFadePatern[mesh/16];

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			linemask2 = linemask[y & 3];
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				if( linemask2[x & 3] ){
					*dest_p = spal[*src_p];
				}
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			linemask2 = linemask[y & 3];
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				if( linemask2[x & 3] && *src_p!=nuki ){
					*dest_p = spal[*src_p];
				}
			}
		}
	}
	return TRUE;
}



static BOOL DRW_DrawZOOM_HB_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BH *src );

static BOOL DRW_DrawBMP_HB_Sui( DRAW_OBJECT *dobj, BMP_BH *src )
{
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		wav = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		x,y, x1,x2, y1,y2;
	int		w2=dobj->dw/8, h2=dobj->dh/8;
	int		wavx = 40 * wav/256;
	int		wavy = 30 * wav/256;
	int		sx, sy;
	int		xinc = XINC(dobj);
	int		yinc = YINC(dobj);
	int		xx,yy,xxx,yyy;
	DRAW_OBJECT	dobj2;

	if( wav==0 ){
		return DRW_DrawBMP_HB_Std( dobj, src );
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

			dobj2 = DRW_SetDrawObject(	1,dest, xx*w2,    yy*h2,    w2,       h2,
										0,src,  xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_NML, 0,0, dobj->rparam );
			DRW_DrawZOOM_HB_Std( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0, src );
		}
	}
	return TRUE;
}




static BOOL DRW_DrawZOOM_HB_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BH *src );

BOOL DRW_DrawBMP_HB_Sbl( DRAW_OBJECT *dobj, BMP_BH *src )
{
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		wav = dobj->dnum&0x00ff;
	int		cnt = (dobj->dnum&0xff00)>>8;
	int		x,y, x1,x2, y1,y2;
	int		w2=dobj->dw/8, h2=dobj->dh/8;
	int		wavx = 40 * wav/256;
	int		wavy = 30 * wav/256;
	int		sx, sy;
	int		xinc = XINC(dobj);
	int		yinc = YINC(dobj);
	int		xx,yy,xxx,yyy;
	DRAW_OBJECT	dobj2;

	if( wav==0 ){
		return DRW_DrawBMP_HB_Std( dobj, src );
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

			dobj2 = DRW_SetDrawObject(	1,dest, xx*w2,    yy*h2,    w2,       h2,
										0,src,  xx*w2+x1, yy*h2+y1, w2+x2-x1, h2+y2-y1, 0,NULL, 0, 0, 0, 0,
										dobj->r, dobj->g, dobj->b, dobj->nuki, dobj->clip, DRW_BLD(128), 0,0, dobj->rparam );
			DRW_DrawZOOM_HB_Bld( &dobj2, w2, h2, w2+x2-x1, h2+y2-y1, 0, 0, src );
		}
	}

	return TRUE;
}




BOOL DRW_DrawBMP_HB( DRAW_OBJECT dobj )
{
	BOOL	ret=TRUE;
	BMP_H	*dest= (BMP_H *)dobj.dest;
	BMP_BT	*src = (BMP_BT *)dobj.src1;
	WORD	pal[256];
	RGB32	pal32[256];
	BMP_BH	src2={ src->buf, src->alp, pal, src->sx, src->sy };

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }

	
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
		BMP_ChangePal_TH( pal, src->pal );
	}else{
		if( dobj.dparam==DRW_VIV2 ){
			CopyMemory( pal32, src->pal, 256*4 );
			BMP_ChangePalFine_Full( pal32, 256-dobj.dnum );
			BMP_ChangePalBright_TH( pal, pal32, dobj.r, dobj.g, dobj.b );
		}else{						
			BMP_ChangePalBright_TH( pal, src->pal, dobj.r, dobj.g, dobj.b );
			switch(dobj.dparam){
				case DRW_SBL2:	BMP_ChangePalBright_High( pal, 128/2, 128/2, 128/2 );					break;
				case DRW_BLD2:	BMP_ChangePalBright_High( pal, dobj.dnum/2, dobj.dnum/2, dobj.dnum/2 );	break;
				case DRW_NEG:	BMP_ChangePalNega_High( pal );											break;	
			}
		}
	}

	switch(dobj.dparam){
		default:

		case DRW_NEG:
		case DRW_VIV2:
		case DRW_NML:	DRW_DrawBMP_HB_Std( &dobj, &src2 );	break;	
		case DRW_BLD2:	DRW_DrawBMP_HB_Bld( &dobj, &src2 );	break;	
		case DRW_ADD:	DRW_DrawBMP_HB_Add( &dobj, &src2 );	break;	
		case DRW_SUB:	DRW_DrawBMP_HB_Sub( &dobj, &src2 );	break;	
		case DRW_MUL:	DRW_DrawBMP_HB_Mul( &dobj, &src2 );	break;	
		case DRW_AMI2:	DRW_DrawBMP_HB_Ami( &dobj, &src2 );	break;	

		case DRW_SUI2:	DRW_DrawBMP_HB_Sui(	&dobj, &src2 );	break;	
		case DRW_SBL2:	DRW_DrawBMP_HB_Sbl(	&dobj, &src2 );	break;	
	}
	return ret;
}








BOOL DRW_DrawZOOM_HH_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - dw;
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx,yy = yyy;;

	px = sw2/dw2;
	wx = sw2%dw2;
	py = sh2/dh2 * src->sx;
	wy = sh2%dh2;


	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					*dest_p = *sss;

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki )
						*dest_p = *sss;

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					*dest_p =  bright_tbl[ *sss ];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki )
						*dest_p =  bright_tbl[ *sss ];

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}
	}
	return TRUE;
}




BOOL DRW_DrawZOOM_HH_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - dw;
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=yyy;
	int		blnd = dobj->dnum, brev = 256-blnd;
	int		blnd3 = blnd>>3, brev3 = 32-blnd3;

	if( blnd==0   ){ return 1;															}	
	if( blnd==256 ){ return DRW_DrawZOOM_HH_Std( dobj, dw2, dh2, sw2, sh2, xxx, yyy );	}	

	px = sw2/dw2;
	wx = sw2%dw2;
	py = sh2/dh2 * src->sx;
	wy = sh2%dh2;


	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*sss&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
									| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*sss&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (*sss&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
										| (( ( (*dest_p&rgb16.mask_g )*brev3 + (*sss&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (bright_tbl[*sss]&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
									| (( ( (*dest_p&rgb16.mask_g )*brev3 + (bright_tbl[*sss]&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( *(long*)sss != nuki ){
						*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 + (bright_tbl[*sss]&rgb16.mask_rb)*blnd3 )>>5 )&rgb16.mask_rb)
										| (( ( (*dest_p&rgb16.mask_g )*brev3 + (bright_tbl[*sss]&rgb16.mask_g )*blnd3 )>>5 )&rgb16.mask_g ) );
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawZOOM_HH_Ami( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy )
{
	int		r=dobj->r, g=dobj->g, b=dobj->b;
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	BMP_H	*src = (BMP_H *)dobj->src1;
	int		drev = dest->sx - dw;
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	WORD	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x,y;
	int		px,py,wx,wy,xx=0,yy=yyy;
	int		mesh = dobj->dnum;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt = MeshFadePatern[mesh/16];

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	px = sw2/dw2;
	wx = sw2%dw2;
	py = sh2/dh2 * src->sx;
	wy = sh2%dh2;


	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] ){
						*dest_p = *sss;
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] && *(long*)sss != nuki ){
						*dest_p = *sss;
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}
	}else{
		WORD	*bright_tbl=BMP_CreateBrightTable_H( r, g, b );
		if( nuki==-1 ){
			for( y=0; y<dh; y++, dest_p+=drev ) {
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] ){
						*dest_p =  bright_tbl[ *sss ];
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}else{
			for( y=0; y<dh; y++, dest_p+=drev ) {
				linemask2 = linemask[y & 3];
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( linemask2[x & 3] && *(long*)sss != nuki ){
						*dest_p =  bright_tbl[ *sss ];
					}

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}
	}
	return TRUE;
}






BOOL DRW_DrawZOOM_HH( DRAW_OBJECT dobj )
{
	BOOL	ret=TRUE;
	BMP_H	*dest= (BMP_H *)dobj.dest;
	BMP_H	*src = (BMP_H *)dobj.src1;
	int		xx, yy, dw2=dobj.dw, dh2=dobj.dh, sw2=dobj.s1w, sh2=dobj.s1h;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }

	
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
		case DRW_NML:	DRW_DrawZOOM_HH_Std( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	
		case DRW_BLD2:	DRW_DrawZOOM_HH_Bld( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	



		case DRW_AMI2:	DRW_DrawZOOM_HH_Ami( &dobj, dw2, dh2, sw2, sh2, xx, yy );	break;	


	}
	return TRUE;
}









BOOL DRW_DrawZOOM_HB_Std( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BH *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		drev = dest->sx - dw;
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	WORD	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;

	px = sw2/dw2;
	wx = sw2%dw2;
	py = sh2/dh2 * src->sx;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				*dest_p = spal[ *sss ];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss++; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki )	*dest_p = spal[ *sss ];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss++; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
		}
	}

	return TRUE;
}



BOOL DRW_DrawZOOM_HB_Bld( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BH *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		drev = dest->sx - dw;
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	WORD	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;
	int		blnd = dobj->dnum;
	int		brev = 256-blnd;
	int		blnd3 = blnd>>3, brev3 = 32-blnd3;

	px = sw2/dw2;
	wx = sw2%dw2;
	py = sh2/dh2 * src->sx;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 )>>5 )&rgb16.mask_rb)
								| (( ( (*dest_p&rgb16.mask_g )*brev3 )>>5 )&rgb16.mask_g ) ) + spal[ *sss ];

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss++; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki ){
					*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3 )>>5 )&rgb16.mask_rb)
									| (( ( (*dest_p&rgb16.mask_g )*brev3 )>>5 )&rgb16.mask_g ) ) + spal[ *sss ];
				}
				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss++; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
		}
	}

	return TRUE;
}



BOOL DRW_DrawZOOM_HB_Add( DRAW_OBJECT *dobj, int dw2, int dh2, int sw2, int sh2, int xxx, int yyy, BMP_BH *src )
{
	int		nuki=dobj->nuki;
	int		dw=dobj->dw, dh=dobj->dh, sw=dobj->s1w, sh=dobj->s1h;
	BMP_H	*dest = (BMP_H *)dobj->dest;
	int		drev = dest->sx - dw;
	WORD	*dest_p = dest->buf + dobj->dy *dest->sx + dobj->dx;
	char	*src_p  = src->buf + DRW_GetStartPointerSrc1( dobj ), *sss;
	WORD	*spal   = src->pal;
	int		xinc = XINC(dobj);
	int		yinc = src->sx*YINC(dobj);
	int		x, y;
	int		px,py,wx,wy,xx,yy=yyy;
	DWORD	scol, dcol;
	DWORD	mask1, mask2=0;

	mask2 |= 0x00000001 << (rgb16.pos_r+5);
	mask2 |= 0x00000001 << (rgb16.pos_r);
	mask2 |= 0x00000001 << (rgb16.pos_g);
	mask1 = ~mask2;

	px = sw2/dw2;
	wx = sw2%dw2;
	py = sh2/dh2 * src->sx;
	wy = sh2%dh2;

	if( nuki==-1 ){
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				dcol = (*dest_p&mask1) + (spal[*sss]&mask1);
				scol = dcol & mask2;
				*dest_p = (WORD)( (scol-(scol>>5)) | dcol );

				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss++; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
		}
	}else{
		for( y=0; y<dh; y++, dest_p+=drev ) {
			sss = src_p;
			xx=xxx;
			for( x=0; x<dw; x++, dest_p++ ) {
				if( *sss!=nuki ){
					dcol = (*dest_p&mask1) + (spal[*sss]&mask1);
					scol = dcol & mask2;
					*dest_p = (WORD)( (scol-(scol>>5)) | dcol );
				}
				sss+=px;
				xx += wx;
				if( xx>=dw2 ) { xx -= dw2; sss++; }
			}
			src_p+=py;
			yy += wy;
			if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
		}
	}

	return TRUE;
}






BOOL DRW_DrawZOOM_HB( DRAW_OBJECT dobj )
{
	BOOL	ret=TRUE;
	BMP_H	*dest= (BMP_H *)dobj.dest;
	BMP_BT	*src = (BMP_BT *)dobj.src1;
	int		xx, yy, dw2=dobj.dw, dh2=dobj.dh, sw2=dobj.s1w, sh2=dobj.s1h;
	RGB32	pal32[256];
	WORD	pal[256];
	BMP_BH	src2={ src->buf, src->alp, pal, src->sx, src->sy };

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }

	
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
		BMP_ChangePal_TH( pal, src->pal );
	}else{
		if( dobj.dparam==DRW_VIV2 ){
			CopyMemory( pal32, src->pal, 256*4 );
			BMP_ChangePalFine_Full( pal32, 256-dobj.dnum );
			BMP_ChangePalBright_TH( pal, pal32, dobj.r, dobj.g, dobj.b );
		}else{						
			BMP_ChangePalBright_TH( pal, src->pal, dobj.r, dobj.g, dobj.b );
			switch(dobj.dparam){
				case DRW_SBL2:	BMP_ChangePalBright_High( pal, 128/2, 128/2, 128/2 );					break;
				case DRW_BLD2:	BMP_ChangePalBright_High( pal, dobj.dnum/2, dobj.dnum/2, dobj.dnum/2 );	break;
				case DRW_NEG:	BMP_ChangePalNega_High( pal );											break;	
			}
		}
	}

	switch(dobj.dparam){
		default:
		case DRW_NEG:	
		case DRW_VIV2:	
		case DRW_NML:	DRW_DrawZOOM_HB_Std( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_BLD2:	DRW_DrawZOOM_HB_Bld( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	
		case DRW_ADD:	DRW_DrawZOOM_HB_Add( &dobj, dw2, dh2, sw2, sh2, xx, yy, &src2 );	break;	



	}
	return TRUE;
}








BOOL DRW_DrawXLine_HH_Std(	WORD *dest_p, BMP_H *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki, int r, int g, int b )
{
	int		ssx=0,ssy=0;
	int		x;
	WORD	*src_p = src->buf + sy*src->sx + sx;

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
				if( nuki != *src_p ){
					*dest_p = *src_p;
				}
				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}
	}else{
		WORD	*bright_tbl = BMP_CreateBrightTable_H( r, g, b );
		if( nuki==-1 ){
			for( x=0; x < w ; x++, dest_p++ ){
				*dest_p =  bright_tbl[ *src_p ];

				src_p += psy;
				ssx+=dsx;		ssy+=dsy;
				if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
				if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
			}
		}else{
			for( x=0; x < w ; x++, dest_p++ ){
				if( nuki != *src_p ){
					*dest_p =  bright_tbl[ *src_p ];
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



BOOL DRW_DrawXLine_HH( WORD *dest_p,int dy, BMP_H *src, MIN_MAX *mi, MIN_MAX *ma, int nuki, RECT *clip,
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
		case DRW_NML:	DRW_DrawXLine_HH_Std( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki, r, g, b );				break;	






		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}





BOOL DRW_DrawPOLY4_HH( DRAW_OBJECT dobj )
{
	BOOL		ret=TRUE;
	BMP_H		*dest= (BMP_H *)dobj.dest;
	BMP_H		*src = (BMP_H *)dobj.src1;
	WORD		*dest_p = dest->buf;
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
		DRW_DrawXLine_HH( dest_p, y, src, &mi[y], &ma[y], dobj.nuki, dobj.clip, dobj.r, dobj.g, dobj.b, dobj.dparam, dobj.dnum );

end:
	GFree(mi);
	GFree(ma);
	
	return ret;
}








BOOL DRW_DrawXLine_HB_Std(	WORD *dest_p, BMP_BH *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	char	*src_p = src->buf + sy*src->sx + sx;
	WORD	*spal  = src->pal;

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




BOOL DRW_DrawXLine_HB( WORD *dest_p,int dy, BMP_BH *src, MIN_MAX *mi, MIN_MAX *ma, int nuki, RECT *clip, int param, int blnd )
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
		case DRW_NEG:	
		case DRW_VIV2:	
		case DRW_NML:	DRW_DrawXLine_HB_Std( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );				break;	






	}
	return ret;
}






BOOL DRW_DrawPOLY4_HB( DRAW_OBJECT dobj )
{
	BOOL		ret=TRUE;
	BMP_H		*dest= (BMP_H *)dobj.dest;
	BMP_BT		*src = (BMP_BT *)dobj.src1;
	WORD		*dest_p = dest->buf;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			y;
	RGB32		pal32[256];
	WORD		pal[256];
	BMP_BH		src2 = { src->buf, src->alp, pal, src->sx, src->sy };

	
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
		BMP_ChangePal_TH( pal, src->pal );
	}else{
		if( dobj.dparam==DRW_VIV2 ){
			CopyMemory( pal32, src->pal, 256*4 );
			BMP_ChangePalFine_Full( pal32, 256-dobj.dnum );
			BMP_ChangePalBright_TH( pal, pal32, dobj.r, dobj.g, dobj.b );
		}else{						
			BMP_ChangePalBright_TH( pal, src->pal, dobj.r, dobj.g, dobj.b );
			switch(dobj.dparam){
				case DRW_SBL2:	BMP_ChangePalBright_High( pal, 128/2, 128/2, 128/2 );					break;
				case DRW_BLD2:	BMP_ChangePalBright_High( pal, dobj.dnum/2, dobj.dnum/2, dobj.dnum/2 );	break;
				case DRW_NEG:	BMP_ChangePalNega_High( pal );											break;	
			}
		}
	}

	for( y = dobj.clip->top ; y < dobj.clip->bottom ; y++, dest_p+=dest->sx )
		DRW_DrawXLine_HB( dest_p, y, &src2, &mi[y], &ma[y], dobj.nuki, dobj.clip, dobj.dparam, dobj.dnum );

end:
	GFree(mi);
	GFree(ma);

	return ret;
}

