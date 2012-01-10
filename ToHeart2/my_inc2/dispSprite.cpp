





#include "mm_std.h"

#include "draw.h"

#include "dispSprite.h"



#define SPBMP_MAX	256

static BMP_SET			SpBmp[SPBMP_MAX];		




ANIME_STRUCT		AnimeStruct[ANIME_MAX];
ANIME_CONTROL		AnimeControl[ANIME_CONTROL_MAX];

static int	SprInterless=0;

static int	AntiFlag = ON;	
void SPR_SetAntiFlag( int flag )
{
	AntiFlag = flag;
}

void SPR_DrawParts( BMP_SET *vram, PARTS_STRUCT *ps, CHIP_STRUCT *cs, ANIME_CONTROL *ac, SPRITE_PARAM *sp )
{
	ANIME_STRUCT	*as = &AnimeStruct[ac->ano];
	SPRITE_STRUCT	*ss = &as->ss[ac->sp_no];
	int		cx,cy,dsx,dsy;
	DWORD	param;
	DWORD	rparam;
	int		r,g,b, rr,gg,bb;
	int		zoom=0, nuki;

	if( ps->flag ){	
		if(ps->nuki==-2){
			if(AntiFlag)nuki = CHK_ANTI;
			else		nuki = CHK_NOANTI;
		}else
			nuki = ps->nuki;

		if(cs->r<=128)	r =      ps->r * cs->r     /128;
		else			r = (255-ps->r)*(cs->r-128)/128+ps->r;
		if(cs->g<=128)	g =      ps->g * cs->g     /128;
		else			g = (255-ps->g)*(cs->g-128)/128+ps->g;
		if(cs->b<=128)	b =      ps->b * cs->b     /128;
		else			b = (255-ps->b)*(cs->b-128)/128+ps->b;

		if(sp->r<=128)	r =      r * sp->r     /128;
		else			r = (255-r)*(sp->r-128)/128+r;
		if(sp->g<=128)	g =      g * sp->g     /128;
		else			g = (255-g)*(sp->g-128)/128+g;
		if(sp->b<=128)	b =      b * sp->b     /128;
		else			b = (255-b)*(sp->b-128)/128+b;
		
		cx = ps->cx;	
		cy = ps->cy;
		dsx = ps->sx;	
		dsy = ps->sy;

		if(sp->param){
			if( (sp->param&0xffff)==cs->dparam ){
				if(ps->dparam==cs->dparam){
					int	work1 = cs->param1*ps->param1*((sp->param&  0xff0000)>>16)/(256*256);
					int	work2 = cs->param2*ps->param2*((sp->param&0xff000000)>>24)/(256*256);
					param = cs->dparam|((work1|(work2<<8))<<16);
				}else{
					int	work1 = cs->param1*((sp->param&  0xff0000)>>16)/256;
					int	work2 = cs->param2*((sp->param&0xff000000)>>24)/256;
					param = cs->dparam|((work1|(work2<<8))<<16);
				}
			}else{
				param = sp->param;

			}
		}else if(cs->dparam){
			if(ps->dparam==cs->dparam){
				int	work1 = cs->param1*ps->param1/256;
				int	work2 = cs->param2*ps->param2/256;
				param = cs->dparam|((work1|(work2<<8))<<16);
			}else{
				param = cs->dparam|((cs->param1|(cs->param2<<8))<<16);
			}

		}else{
			param = ps->dparam|((ps->param1|(ps->param2<<8))<<16);
		}
		rparam = sp->rparam^cs->rparam^ps->rparam;	

		if( cs->rparam&REV_W ){ cx = -(cx + dsx); }	
		if( sp->rparam&REV_W ){ cx = -(cx + dsx); }	
		if( cs->rparam&REV_H ){ cy = -(cy + dsy); }	
		if( sp->rparam&REV_H ){ cy = -(cy + dsy); }	

		if( ps->zflag && ps->zoom!=100 ){
			dsx = dsx*ps->zoom/100;
			dsy = dsy*ps->zoom/100;
		}
		if( cs->zflag && cs->zoom!=100 ){
			dsx = dsx*cs->zoom/100;
			dsy = dsy*cs->zoom/100;
			cx = cx*cs->zoom/100;
			cy = cy*cs->zoom/100;
		}
		cx = sp->dx + cx;
		cy = sp->dy + cy;
		{
			BMP_SET	*bs = &SpBmp[ as->code2bno[ps->bno] ];

			DRAW_OBJECT	dobj = DRW_SetDrawObject(	vram->bmp_bit, DSP_GetBmp2(vram), cx, cy, dsx, dsy,
													bs->bmp_bit,   DSP_GetBmp2(bs), ps->px, ps->py, ps->sx, ps->sy, 0, NULL, 0, 0, 0, 0,
													r, g, b, nuki, sp->clip, param, 0,0, rparam );

			if( ac->draw_rect.top > cy )        ac->draw_rect.top    = cy;
			if( ac->draw_rect.bottom < cy+dsy ) ac->draw_rect.bottom = cy+dsy;
			if( ac->draw_rect.left > cx )       ac->draw_rect.left   = cx;
			if( ac->draw_rect.right < cx+dsx )  ac->draw_rect.right  = cx+dsx;

			if( dsx==ps->sx && dsy==ps->sy ){
				
				if(ac->cfade && ss->code[ac->code_no].code==SPCODE_DRAW && dobj.dparam==DRW_NML && r==128){
					
					PARTS_STRUCT	*ps2 = &as->cs[ss->code[ac->next_code].data1].ps[0];
					BMP_SET			*bs2 = &SpBmp[ as->code2bno[ps2->bno] ];
					int				rate = 256*ac->count/ss->code[ac->code_no].data2;

					if(bs->bmp_bit==BMP_256P){
						rr = r-r*rate/256;
						gg = g-g*rate/256;
						bb = b-b*rate/256;
						dobj = DRW_SetDrawObject(	vram->bmp_bit, DSP_GetBmp2(vram), cx, cy, dsx, dsy,
													bs->bmp_bit,   DSP_GetBmp2(bs),  ps->px, ps->py, ps->sx, ps->sy, 0, NULL, 0, 0, 0, 0,
													rr, gg, bb, nuki, sp->clip, param, 0,0, rparam );
						switch(vram->bmp_bit){
							case BMP_256P:	break;
							case BMP_HIGH:	DRW_DrawBMP_HB( dobj );	break;
							case BMP_FULL:	DRW_DrawBMP_FB( dobj, SprInterless );	break;
							case BMP_TRUE:	DRW_DrawBMP_TB( dobj );	break;
						}

						rr = r*(rate)/256;
						gg = g*(rate)/256;
						bb = b*(rate)/256;
						dobj = DRW_SetDrawObject(	vram->bmp_bit, DSP_GetBmp2(vram), cx, cy, dsx, dsy,
													bs2->bmp_bit,  DSP_GetBmp2(bs2), ps2->px, ps2->py, ps2->sx, ps2->sy, 0, NULL, 0, 0, 0, 0,
													rr, gg, bb, nuki, sp->clip, DRW_ADD, 0,0, rparam );
						switch(vram->bmp_bit){
							case BMP_256P:	break;
							case BMP_HIGH:	DRW_DrawBMP_HB( dobj );	break;
							case BMP_FULL:	DRW_DrawBMP_FB( dobj, SprInterless );	break;
							case BMP_TRUE:	DRW_DrawBMP_TB( dobj );	break;
						}
					}else{
						dobj = DRW_SetDrawObject(	vram->bmp_bit, DSP_GetBmp2(vram), cx, cy, dsx, dsy,
													bs->bmp_bit,   DSP_GetBmp2(bs),  ps->px, ps->py, ps->sx, ps->sy, 0, NULL, 0, 0, 0, 0,
													r, g, b, nuki, sp->clip, DRW_NML, 0,0, rparam );
						switch(vram->bmp_bit){
							case BMP_256P:	break;
							case BMP_HIGH:	DRW_DrawBMP_HH( dobj );	break;
							case BMP_FULL:	DRW_DrawBMP_FF( dobj, SprInterless );	break;
							case BMP_TRUE:	DRW_DrawBMP_TT( dobj );	break;
						}

						dobj = DRW_SetDrawObject(	vram->bmp_bit, DSP_GetBmp2(vram), cx, cy, dsx, dsy,
													bs2->bmp_bit,  DSP_GetBmp2(bs2), ps2->px, ps2->py, ps2->sx, ps2->sy, 0, NULL, 0, 0, 0, 0,
													r, g, b, nuki, sp->clip, DRW_BLD(rate), 0,0, rparam );
						switch(vram->bmp_bit){
							case BMP_256P:	break;
							case BMP_HIGH:	DRW_DrawBMP_HH( dobj );	break;
							case BMP_FULL:	DRW_DrawBMP_FF( dobj, SprInterless );	break;
							case BMP_TRUE:	DRW_DrawBMP_TT( dobj );	break;
						}
					}
				}else{
					switch(vram->bmp_bit){
					case BMP_256P:	break;
					case BMP_HIGH:
						switch(bs->bmp_bit){
						case BMP_256P:	DRW_DrawBMP_HB( dobj );	break;
						case BMP_HIGH:	DRW_DrawBMP_HH( dobj );	break;
	
	
						}
						break;
					case BMP_FULL:
						switch(bs->bmp_bit){
						case BMP_256P:	DRW_DrawBMP_FB( dobj, SprInterless );	break;
	
						case BMP_FULL:	DRW_DrawBMP_FF( dobj, SprInterless );	break;
						case BMP_TRUE:	DRW_DrawBMP_FT( dobj, SprInterless );	break;
						}
						break;
					case BMP_TRUE:
						switch(bs->bmp_bit){
						case BMP_256P:	DRW_DrawBMP_TB( dobj );	break;
	
	
						case BMP_TRUE:	DRW_DrawBMP_TT( dobj );	break;
						}
						break;
					}
				}
			}else{
				switch(vram->bmp_bit){
				case BMP_256P:	break;
				case BMP_HIGH:
					switch(bs->bmp_bit){
					case BMP_256P:	DRW_DrawZOOM_HH( dobj );	break;
					case BMP_HIGH:	DRW_DrawZOOM_HH( dobj );	break;


					}
					break;
				case BMP_FULL:
					switch(bs->bmp_bit){
					case BMP_256P:	DRW_DrawZOOM_FB( dobj, SprInterless );	break;

					case BMP_FULL:	DRW_DrawZOOM_FF( dobj, SprInterless );	break;
					case BMP_TRUE:	DRW_DrawZOOM_FT( dobj, SprInterless );	break;
					}
					break;
				case BMP_TRUE:
					switch(bs->bmp_bit){
					case BMP_256P:	DRW_DrawZOOM_TB( dobj );	break;


					case BMP_TRUE:	DRW_DrawZOOM_TT( dobj );	break;
					}
					break;
				}
			}
		}
	}

}

void SPR_DrawChip( BMP_SET *vram, CHIP_STRUCT *cs, ANIME_CONTROL *ac, SPRITE_PARAM *sp )
{
	int				i;
	
	if( cs->flag ){	
		ac->draw_rect.top    = DISP_Y;
		ac->draw_rect.bottom = 0;
		ac->draw_rect.left   = DISP_Y;
		ac->draw_rect.right  = 0;
		for(i=0;i<cs->ps_max;i++){
			SPR_DrawParts( vram, &cs->ps[i], cs, ac, sp );
		}
	}
}
void SPR_DrawSprite( void *dest, int draw_mode, int ani_no,
					int dx, int dy, int r, int g, int b, RECT *clip, int param, int rparam, int zoom )
{
	BMP_SET			vram;
	SPRITE_STRUCT	*ss;
	ANIME_STRUCT	*as;
	ANIME_CONTROL	*ac = &AnimeControl[ani_no];
	SPRITE_PARAM	sp = { dx,dy, r,g,b, clip, param, rparam, zoom };

	if(!ac->flag)					{ DebugPrintf("AnimeControlに登録されていません[%d]\n",ani_no); return; }
	if(!AnimeStruct[ac->ano].flag)	{ DebugPrintf("AnimeStructに登録されていません[%d]\n",ac->ano); return; }
	as = &AnimeStruct[ac->ano];

	vram.flag    = 1;
	vram.bmp_bit = BPP(draw_mode);
	vram.alp_flag= 0;
	vram.bmp_t   = *(BMP_T*)dest;
	vram.bmp_f   = *(BMP_F*)dest;
	vram.bmp_h   = *(BMP_H*)dest;
	vram.bmp_b   = *(BMP_BT*)dest;

	if( !(ac->mode==SP_STOP && ac->end==0) ){
		if(as->cs && as->ss){
			ss = &as->ss[ac->sp_no];
			if(ss->code){
				SPR_DrawChip( &vram, &as->cs[ ss->code[ac->code_no].data1 ], ac, &sp );
			}
		}
	}
}

static void RenewSprite( SPRITE_OPR code[], long *count, long *code_no, long *draw_count, char *loop_no, char lcount[], ANIME_CONTROL *ac  )
{


	int		i;

	while( *draw_count > 0 ){
		switch( code[*code_no].code ){
		case SPCODE_NULL:
			if(*code_no==0)(*draw_count) -=100;

			if( ac->lnum==0 ){
				*code_no=0;
			}else if(ac->lnum==1){
				if(ac->end){
					while( code[*code_no].code!=SPCODE_DRAW ){
						(*code_no)--;
					}
					ac->mode = SP_STOP;
					*count = 0;
				}else{		
					ac->mode = SP_STOP;
					*draw_count = 0;
				}
			}else{
				ac->lnum--;
				*code_no=0;
			}
			break;
		case SPCODE_DRAW:
			(*count)++;
			if( *count > code[*code_no].data2 ){
				(*code_no)++;
				*count = 0;
			}else{
				if( *count==1 && code[*code_no].data3 ){
					ac->event = code[*code_no].data3;
				}
				if(ac->rep_wait==REPEAT_WAIT){
					if( *count+1 > code[*code_no].data2 && code[*code_no+1].code==SPCODE_REPEAT ){
						ac->rep_wait = REPEAT_FIND;
					}
				}
				(*draw_count) -= 100;
			}
			break;
		case SPCODE_LOOP:
			if(code[*code_no].data1)
				lcount[*loop_no] = code[*code_no].data1+1;
			else
				lcount[*loop_no] = 0;
			(*loop_no)++;
			(*code_no)++;
			break;
		case SPCODE_REPEAT:
			if(*loop_no==0){
				(*code_no)++;
				break;
			}
			if(ac->through){
				ac->through = OFF;
				lcount[*loop_no-1] = 0;
				(*loop_no)--;
				(*code_no)++;
			}else{
				switch(lcount[*loop_no-1]){
					default:
						lcount[*loop_no-1]--;
						
					case 0:
						
						i=*code_no;
						while( i>=0 && code[i].code!=SPCODE_LOOP ) i--;
						*code_no = i+1;
						break;
					case 1:
						lcount[*loop_no-1] = 0;
						(*loop_no)--;
						(*code_no)++;
						break;
				}
			}
			break;
		}
	}
}

static void GetNextSprite( SPRITE_OPR code[], long *count, long *code_no, char *loop_no, char lcount[] )
{
	int		i;
	int		next_flag=0;

	while( !next_flag  ){
		switch( code[*code_no].code ){
		case SPCODE_NULL:
			if(*code_no==0) next_flag = 1;
			*code_no = 0;
			break;
		case SPCODE_DRAW:
			if( *count==0 ){
				next_flag = 1;
			}else{
				(*count)++;
				if( *count > code[*code_no].data2 ){
					(*code_no)++;
					*count = 0;
				}
			}
			break;
		case SPCODE_LOOP:
			if(code[*code_no].data1)
				lcount[*loop_no] = code[*code_no].data1+1;
			else
				lcount[*loop_no] = 0;
			(*loop_no)++;
			(*code_no)++;
			break;
		case SPCODE_REPEAT:
			if(*loop_no==0) break;
			switch(lcount[*loop_no-1]){
				default:
					lcount[*loop_no-1]--;
					
				case 0:
					
					i=*code_no;
					while( i>=0 && code[i].code!=SPCODE_LOOP ) i--;
					*code_no = i+1;
					break;
				case 1:
					lcount[*loop_no-1] = 0;
					(*loop_no)--;
					(*code_no)++;
					break;
			}
			break;
		}
	}
}

void SPR_RenewSprite( int frame, int interless )
{
	int				j;

	SprInterless = interless;

	for(j=0;j<ANIME_CONTROL_MAX;j++){
		ANIME_CONTROL	*ac = &AnimeControl[j];
		if(ac->flag && ac->mode){
			ANIME_STRUCT	*as = &AnimeStruct[ac->ano];
			SPRITE_STRUCT	*ss = &as->ss[ac->sp_no];
			if(ss==NULL) continue;

			ac->draw_count += as->frame*ac->speed/frame;
			RenewSprite( ss->code, &ac->count, &ac->code_no, &ac->draw_count, &ac->loop_no, ac->lcount, ac );

			
			if(ac->cfade){
				long	count = ac->count;
				long	code_no = ac->code_no;
				char	loop_no = ac->loop_no;
				char	lcount[16];
				CopyMemory( lcount, ac->lcount, 16 );

				if( ac->lnum!=0 ){
					DebugPrintf("無限ﾙｰﾌﾟ専用なりよ\n");
				}

				GetNextSprite( ss->code, &count, &code_no, &loop_no, lcount );
				ac->next_code = code_no;
			}

		}
	}
}


static int SPR_LoadBmpSet( BMP_SET *bmp_set, char *fname, char *tfname, int viv )
{
	BOOL	ret=0;

	ret = LoadBmpSet( bmp_set, BMP_256P, fname, NULL );
	if( !ret ) return FALSE;
	BMP_SetTonecurve_BT(&bmp_set->bmp_b, tfname, viv );

	if(ret){
		return bmp_set->bmp_bit;
	}else{
		ZeroMemory( bmp_set, sizeof(BMP_SET) );
		return -1;
	}
}

static void SPR_ReleaseBmpSet( BMP_SET *bmp_set )
{
	switch( bmp_set->bmp_bit ){
		case BMP_256P:	BMP_ReleaseBMP_BT( &bmp_set->bmp_b );	break;
		case BMP_HIGH:	BMP_ReleaseBMP_H(  &bmp_set->bmp_h );	break;
		case BMP_FULL:	BMP_ReleaseBMP_F(  &bmp_set->bmp_f );	break;
		case BMP_TRUE:	BMP_ReleaseBMP_T(  &bmp_set->bmp_t ); 	break;
	}
	ZeroMemory( bmp_set, sizeof(BMP_SET) );
}


int SPR_LoadBmp( char *fname, char *tfname, int viv )
{
	int		i,ret;

	for( i=0; i<SPBMP_MAX ;i++ ){
		if( !SpBmp[i].flag ){
			break;
		}
	}
	if(i==SPBMP_MAX){
		DebugBox( NULL, "スプライトが多過ぎます" );
		return -1;
	}
	ret = SPR_LoadBmpSet( SpBmp+i, fname, tfname,viv );
	if(ret==-1) {
		DebugBox( NULL, "スプライトファイルの読み込みに失敗しました[%s]", fname );
		return -1;
	}
	return i;
}

int SPR_CopyBmp( int src_bno, char *tfname, int viv, int r, int g, int b )
{
	int	dst_bno,i;
	for( i=0; i<SPBMP_MAX ;i++ ){
		if( !SpBmp[i].flag ){
			break;
		}
	}
	dst_bno = i;
	
	DRAW_OBJECT	dobj;
	if(src_bno<0 || src_bno>SPBMP_MAX) { DebugPrintf("SPBMP領域エラー[SRC=%d]",src_bno); return -1; }
	if(dst_bno<0 || dst_bno>SPBMP_MAX) { DebugPrintf("SPBMP領域エラー[DST=%d]",dst_bno); return -1; }
	BMP_SET		*dst_bmp = &SpBmp[dst_bno];
	BMP_SET		*src_bmp = &SpBmp[src_bno];
	if(!src_bmp->flag) { DebugPrintf("ソースの無い画像コピーを実行しました"); return -1; }

	if( dst_bno != src_bno ){
		dst_bmp->flag     = 1;
		dst_bmp->bmp_bit  = src_bmp->bmp_bit;
		dst_bmp->alp_flag = src_bmp->alp_flag;
		dst_bmp->pos      = src_bmp->pos;
		switch( src_bmp->bmp_bit ){
			case 0:
				break;
			case 1:
				BMP_CreateBMP_H( &dst_bmp->bmp_h, src_bmp->bmp_h.sx, src_bmp->bmp_h.sy, 0 );
				dobj = DRW_SetDrawObject( BMP_HIGH, &dst_bmp->bmp_h, 0, 0, src_bmp->bmp_h.sx, src_bmp->bmp_h.sy,
										  BMP_HIGH, &src_bmp->bmp_h,     0, 0, src_bmp->bmp_h.sx, src_bmp->bmp_h.sy, 0,NULL, 0, 0, 0, 0,
										  r, g, b, CHK_NO, (RECT*)-1, DRW_NML, 0,0, 0 );
				DRW_DrawBMP_HH(  dobj );
				BMP_SetTonecurve_H( &dst_bmp->bmp_h, tfname, viv );
				break;
			case 2:
				BMP_CreateBMP_F( &dst_bmp->bmp_f, src_bmp->bmp_f.sx, src_bmp->bmp_f.sy );
				dobj = DRW_SetDrawObject( BMP_FULL, &dst_bmp->bmp_f, 0, 0, dst_bmp->bmp_f.sx, dst_bmp->bmp_f.sy,
										  BMP_FULL, &src_bmp->bmp_f, 0, 0, src_bmp->bmp_f.sx, src_bmp->bmp_f.sy, 0,(RECT*)NULL, 0, 0, 0, 0,
										  r, g, b, CHK_NO, (RECT*)-1, DRW_NML, 0,0, 0 );
				DRW_DrawBMP_FF( dobj );
				BMP_SetTonecurve_F( &dst_bmp->bmp_f, tfname, viv );
				break;
			case 3:
				BMP_CreateBMP_T( &dst_bmp->bmp_t, src_bmp->bmp_t.sx, src_bmp->bmp_t.sy );
				dobj = DRW_SetDrawObject( BMP_TRUE, &dst_bmp->bmp_t, 0, 0, src_bmp->bmp_t.sx, src_bmp->bmp_t.sy,
										  BMP_TRUE, &src_bmp->bmp_t,     0, 0, src_bmp->bmp_t.sx, src_bmp->bmp_t.sy, 0,(RECT*)NULL, 0, 0, 0, 0,
										  r, g, b, CHK_NO, (RECT*)-1, DRW_NML, 0,0, 0 );
				DRW_DrawBMP_TT(  dobj );
				BMP_SetTonecurve_T( &dst_bmp->bmp_t, tfname, viv );
				break;
		}
	}
	return dst_bno;
}


BOOL SPR_CopySprite( int ani_no1,  int ani_no2, char *tfname, int viv, int r, int g, int b )
{
	int	i;
	ANIME_STRUCT	*as = &AnimeStruct[ani_no1];
	ANIME_STRUCT	*as_src = &AnimeStruct[ani_no2];

	
	CopyMemory( as, as_src, sizeof(ANIME_STRUCT2) );
	
	as->cs = (CHIP_STRUCT *)GAlloc(sizeof(CHIP_STRUCT)*as->cs_max);
	for( i=0 ; i<as->cs_max ; i++ ){
		CopyMemory( &as->cs[i], &as_src->cs[i], sizeof(CHIP_STRUCT2) );
		
		as->cs[i].ps = (PARTS_STRUCT *)GAlloc(sizeof(PARTS_STRUCT)*as->cs[i].ps_max);
		CopyMemory( as->cs[i].ps, as_src->cs[i].ps, sizeof(PARTS_STRUCT)*as->cs[i].ps_max );
	}
	as->ss = (SPRITE_STRUCT *)GAlloc(sizeof(SPRITE_STRUCT)*as->ss_max);
	for( i=0 ; i<as->ss_max ; i++ ){
		CopyMemory( &as->ss[i], &as_src->ss[i], sizeof(SPRITE_STRUCT2) );

		
		as->ss[i].code = (SPRITE_OPR *)GAlloc(sizeof(SPRITE_OPR)*(as->ss[i].code_max+1));
		CopyMemory( as->ss[i].code, as_src->ss[i].code, sizeof(SPRITE_OPR)*(as->ss[i].code_max+1) );
	}

	for(i=0; i<as_src->bmp_max ;i++){
		as->code2bno[i] = SPR_CopyBmp( as_src->code2bno[i], tfname, viv, r, g, b );
	}
	as->flag = 1;

	return TRUE;
}

BOOL SPR_LoadSprite( int ani_no, char *fname, char *tfname, int viv )
{
	int				i;
	ANIME_STRUCT	*as = &AnimeStruct[ani_no];
	CHIP_STRUCT		*cs;
	SPRITE_STRUCT	*ss;
	DWORD			size;
	char			*buf=NULL, *ptr;
	SPANI_HEADER	shead;

	SPR_ReleaseSprite(ani_no);

	size = PAC_LoadFile( BMP_GetPackDir(), fname, &buf);
	if(!size) return FALSE;
	ptr = buf;
	CopyMemory( &shead, ptr, sizeof(SPANI_HEADER) );
	ptr += sizeof(SPANI_HEADER);

	if( shead.header!='SANI' ){
		GFree( buf );
		DebugBox(NULL,"未知のフォーマットです。[%s]",fname);
		return FALSE;
	}
	
	CopyMemory( as, ptr, sizeof(ANIME_STRUCT2) );
	ptr += sizeof(ANIME_STRUCT2);
	
	
	as->cs = (CHIP_STRUCT *)GAlloc(sizeof(CHIP_STRUCT)*as->cs_max);
	for( i=0 ; i<as->cs_max ; i++ ){
		cs = &as->cs[i];
		CopyMemory( cs, ptr, sizeof(CHIP_STRUCT2) );
		ptr += sizeof(CHIP_STRUCT2);
		
		cs->ps = (PARTS_STRUCT *)GAlloc(sizeof(PARTS_STRUCT)*cs->ps_max);
		CopyMemory( cs->ps, ptr, sizeof(PARTS_STRUCT)*cs->ps_max );
		ptr += sizeof(PARTS_STRUCT)*cs->ps_max;
	}
	as->ss = (SPRITE_STRUCT *)GAlloc(sizeof(SPRITE_STRUCT)*as->ss_max);
	for( i=0 ; i<as->ss_max ; i++ ){
		ss = &as->ss[i];
		CopyMemory( ss, ptr, sizeof(SPRITE_STRUCT2) );
		ptr += sizeof(SPRITE_STRUCT2);

		
		ss->code = (SPRITE_OPR *)GAlloc(sizeof(SPRITE_OPR)*(ss->code_max+1));
		CopyMemory( ss->code, ptr, sizeof(SPRITE_OPR)*(ss->code_max+1) );
		ptr += sizeof(SPRITE_OPR)*(ss->code_max+1);
	}

	
	for(i=0; i<as->bmp_max ;i++){
		as->code2bno[i] = SPR_LoadBmp( ptr, tfname, viv );
		ptr += strlen(ptr)+1;
	}
	GFree( buf );

	as->flag = 1;

	return TRUE;
}

BOOL SPR_LoadBmpSprite( int ani_no, char *fname, int nuki, char *tfname, int viv )
{

	return TRUE;
}




BOOL SPR_ReleaseSprite( int ani_no )
{
	int				i;
	ANIME_STRUCT	*as = &AnimeStruct[ani_no];

	if(as->flag){
		for( i=0 ; i<as->cs_max ; i++ ){
			GFree( as->cs[i].ps );
		}
		GFree( as->cs );

		for( i=0 ; i<as->ss_max ; i++ ){
			GFree( as->ss[i].code );
		}
		GFree( as->ss );
	
		
		for(i=0; i<as->bmp_max ;i++){
			int	j = as->code2bno[i];
			SPR_ReleaseBmpSet( &SpBmp[j] );
		}
	}
	ZeroMemory( as, sizeof(SPRITE_STRUCT) );

	return TRUE;
}


void SPR_ReleaseSpriteAll( void )
{
	int	i;

	for( i=0 ; i<SPRITE_MAX ; i++ ){
		SPR_ReleaseSprite( i );
	}
}




BOOL SPR_SetSprite( int ani_no, int sp_no, int mode, int end, int lnum, int cfade )
{
	int	ac_no;
	ANIME_CONTROL	*ac;
	
	for(ac_no=0;ac_no<ANIME_CONTROL_MAX;ac_no++){
		if(AnimeControl[ac_no].flag==0){
			ac = &AnimeControl[ac_no];
			break;
		}
	}
	if( ac_no==ANIME_CONTROL_MAX )    { DebugPrintf("スプライトの表示数が飽和しました[max=%d]",ANIME_CONTROL_MAX); return -1; }
	if(ani_no<0 || ANIME_MAX<=ani_no) { DebugPrintf("存在しないスプライト領域が指定されました[%d]\n",ani_no);  return -1; }
	if(AnimeStruct[ani_no].flag==0)   { DebugPrintf("存在しないスプライト領域が指定されました[%d]\n",ani_no);  return -1; }
	if(sp_no<0 || AnimeStruct[ani_no].ss_max<=sp_no) { DebugPrintf("指定されたスプライトパターンは存在しません[%d]\n",sp_no);  return -1; }

	ZeroMemory( ac, sizeof(ANIME_CONTROL) );
	ac->flag = 1;
	ac->ano  = ani_no;
	ac->mode = mode;	
	ac->end  = end;		
	ac->lnum = lnum;	
	ac->cfade = cfade;

	ac->sp_no = sp_no;

	ac->code_no = 0;
	ac->loop_no = 0;	
	ZeroMemory( ac->lcount, 16 );	

	ac->speed = 100;
	return ac_no;
}


BOOL SPR_ResetSprite( int ac_no )
{
	ANIME_CONTROL	*ac = &AnimeControl[ac_no];

	ZeroMemory( ac, sizeof(ANIME_CONTROL) );

	return TRUE;
}

BOOL SPR_SetSpriteSpeed( int ac_no, int speed )
{
	if(speed<0) return FALSE;
	return AnimeControl[ac_no].speed = speed;
}

BOOL SPR_SetSpritePlay( int ac_no, int ani_no, int sp_no, int mode=SP_PLAY, int end=ON, int lnum=0, int cfade=OFF )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります[%d]\n",ac_no); return FALSE; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました[%d]\n",ac_no);  return FALSE; }
	if(ani_no<0 || ANIME_MAX<=ani_no)                { DebugPrintf("存在しないスプライト領域が指定されました[%d]\n",ani_no);  return FALSE; }
	if(AnimeStruct[ani_no].flag==0)                  { DebugPrintf("存在しないスプライト領域が指定されました[%d]\n",ani_no);  return FALSE; }
	if(sp_no<0 || AnimeStruct[ani_no].ss_max<=sp_no) { DebugPrintf("指定されたスプライトパターンは存在しません[%d]\n",sp_no);  return FALSE; }

	ANIME_CONTROL	*ac = &AnimeControl[ac_no];

	if( !(ac->ano==ani_no && ac->sp_no==sp_no) ){
		ZeroMemory( ac, sizeof(ANIME_CONTROL) );
	}
	ac->flag = 1;
	ac->ano  = ani_no;
	ac->sp_no = sp_no;

	ac->mode = mode;	
	ac->end  = end;		
	ac->lnum = lnum;	
	ac->cfade = cfade;

	ac->speed = 100;

	return TRUE;
}

BOOL SPR_SetSpriteNo( int ac_no, int sp_no )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります1[%d]\n",ac_no); return FALSE; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました1[%d]\n",ac_no);  return FALSE; }
	ANIME_CONTROL	*ac = &AnimeControl[ac_no];
	if(sp_no<0 || AnimeStruct[ac->ano].ss_max<=sp_no) { DebugPrintf("指定されたスプライトパターンは存在しません1[%d]\n",sp_no);  return FALSE; }

	if( ac->sp_no!=sp_no ){
		ac->sp_no = sp_no;

		ac->draw_count = 0;
		ac->next_code  = 0;
		ac->rep_wait   = 0;
		ac->through    = 0;
		ac->count   = 0;
		ac->code_no = 0;
		ac->loop_no = 0;	
		ZeroMemory( ac->lcount, 16 );	
	}
	return TRUE;
}



BOOL SPR_SetSpriteCfade( int ac_no, int flag )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります2[%d]\n",ac_no); return FALSE; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました2[%d]\n",ac_no);  return FALSE; }

	AnimeControl[ac_no].cfade = flag;
	return TRUE;
}

BOOL SPR_SetSpriteLoop( int ac_no, int end, int lnum )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります3[%d]\n",ac_no); return FALSE; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました3[%d]\n",ac_no);  return FALSE; }

	AnimeControl[ac_no].end  = end;
	AnimeControl[ac_no].lnum = lnum;
	return TRUE;
}

BOOL SPR_SetSpriteLoopThrough( int ac_no )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります4[%d]\n",ac_no); return FALSE; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました4[%d]\n",ac_no);  return FALSE; }

	AnimeControl[ac_no].through = ON;
	return TRUE;
}

BOOL SPR_SetSpriteRepeatFind( int ac_no )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります5[%d]\n",ac_no); return FALSE; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました5[%d]\n",ac_no);  return FALSE; }

	AnimeControl[ac_no].rep_wait = REPEAT_WAIT;
	return TRUE;
}

int SPR_GetSpriteRepeatFind( int ac_no )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります6[%d]\n",ac_no); return REPEAT_NULL; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました6[%d]\n",ac_no);  return REPEAT_NULL; }

	return AnimeControl[ac_no].rep_wait;
}

RECT *SPR_GetSpriteRect( int ac_no )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります7[%d]\n",ac_no); return NULL; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました7[%d]\n",ac_no);  return NULL; }

	return &AnimeControl[ac_no].draw_rect;
}


BOOL SPR_WaitSpriteCondition( int ac_no )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります8[%d]\n",ac_no); return FALSE; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました8[%d]\n",ac_no);  return FALSE; }
	BOOL	ret = AnimeControl[ac_no].mode==SP_PLAY;

	if( !AnimeControl[ac_no].lnum ){
		AnimeControl[ac_no].lnum=1;
	}
	return TRUE;
}


int SPR_GetSpriteMode( int ac_no )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります9[%d]\n",ac_no); return SP_STOP; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました9[%d]\n",ac_no);  return SP_STOP; }

	return AnimeControl[ac_no].mode;
}

BOOL SPR_SetSpriteMode( int ac_no, int mode )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります10[%d]\n",ac_no); return SP_STOP; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました10[%d]\n",ac_no); return SP_STOP; }

	AnimeControl[ac_no].mode = mode;
	return TRUE;
}


int SPR_GetSpriteEvent( int ac_no )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります11[%d]\n",ac_no); return 0; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました11[%d]\n",ac_no);  return 0; }
	int				event=0;
	event = AnimeControl[ac_no].event;
	AnimeControl[ac_no].event = 0;
	return event;
}
