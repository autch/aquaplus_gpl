





#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"
#include "Comp_pac.h"


#include "d3d_draw.h"

#include "dispSprite.h"













ANIME_STRUCT		AnimeStruct[ANIME_MAX];
ANIME_CONTROL		AnimeControl[ANIME_CONTROL_MAX];

static int	SprInterless=0;

static int	AntiFlag = ON;	
void SPR_SetAntiFlag( int flag )
{
	AntiFlag = flag;
}

enum{
	DRW_NML=0x00,	
	DRW_ADD,	
	DRW_OOI,	
	DRW_SUB,	
	DRW_DIM,
	DRW_MUL,	
	DRW_OLY,	
	DRW_CML,	
	DRW_CMD,	
	DRW_NEG,	
	DRW_TNC,	

	DRW_BLD2,	
	DRW_VIV2,	
	DRW_AMI2,	
	DRW_NIS2,	
	DRW_MOZ2,	
	DRW_BOM2,	

	DRW_GNM2,	
	DRW_FLT2,	

	DRW_RPL2,	

	DRW_RP22,	
	DRW_RP32,	

	DRW_SUI2,	
	DRW_SBL2,	
	DRW_WAV2,	
	DRW_WBL2,	
	DRW_LST2,	
	DRW_LBL2,	

	DRW_LCF2,				
	DRW_LPP2=DRW_LCF2+6,	
	DRW_DIA2=DRW_LPP2+6,	
	DRW_DIO2=DRW_DIA2+3,	

	DRW_PAL=0x0100,	
};

void SPR_DrawSpriteFrame( int ani_no, D3DD_DISP disp )
{
	D3DD_DISP *sp_disp = &disp;
	
}



void SPR_DrawParts( PARTS_STRUCT *ps, CHIP_STRUCT *cs, ANIME_CONTROL *ac, D3DD_DISP *sp_disp )
{
	ANIME_STRUCT	*as = &AnimeStruct[ac->ano];
	SPRITE_STRUCT	*ss = &as->ss[ac->sp_no];
	float	dx,dy,dsx,dsy, cx,cy;
	DWORD	rparam;
	int		zoom=0;

	if( ps->flag ){	
		

		rparam = sp_disp->rev_param^cs->rparam^ps->rparam;	

		{
			D3DD_DISP	disp=*sp_disp;

			disp.flag=1;	
			disp.disp=1;	
			disp.type=1;	
			disp.vnum=4;	
		
			disp.ps_flag=0;
			
			if(disp.blend_param==BLEND_NONE){
				switch( cs->dparam ){
					case DRW_NML:
						if(ps->nuki==-2 && disp.blend_param==BLEND_NONE){
							disp.blend_param=BLEND_ALPHA;
						}
						break;
					case DRW_ADD:
						disp.blend_param=BLEND_ADD;
						break;
					case DRW_SUB:
						disp.blend_param=BLEND_REVSUB;
						break;
					case DRW_MUL:
						disp.blend_param=BLEND_MUL;
						break;
					case DRW_OLY:

						break;
					case DRW_NEG:

						break;
					case DRW_BLD2:
						disp.blend_param=BLEND_ALPHA;
						break;
				}
			}

			if(rparam&0x10)	disp.rev_param |= REVERSE_W;	
			if(rparam&0x20)	disp.rev_param |= REVERSE_H;

			disp.rander_no  = -1;	
			disp.textue_no  = as->code2bno[ps->bno];	
			disp.textue_no2 = -1;	

			disp.bright_no = sp_disp->bright_no;	


			cx  = (float)sp_disp->cx;
			cy  = (float)sp_disp->cy;
			disp.cx = cx;
			disp.cy = cy;
			disp.zoom_x = sp_disp->zoom_x;
			disp.zoom_y = sp_disp->zoom_y;

			dx  = (float)(sp_disp->dx[0] + ps->cx);	
			dy  = (float)(sp_disp->dy[0] + ps->cy);
			dsx = ps->sx;	
			dsy = ps->sy;

			disp.dx[0] = dx;	
			disp.dx[1] = dx+dsx;
			disp.dx[2] = dx;
			disp.dx[3] = dx+dsx;

			disp.dy[0] = dy;
			disp.dy[1] = dy;
			disp.dy[2] = dy+dsy;
			disp.dy[3] = dy+dsy;

			disp.rhw[0] = 1.0f;
			disp.rhw[1] = 1.0f;
			disp.rhw[2] = 1.0f;
			disp.rhw[3] = 1.0f;

			disp.sx1[0] = (float) ps->px;	
			disp.sx1[1] = (float)(ps->px+ps->sx);
			disp.sy1[0] = (float) ps->py;
			disp.sy1[1] = (float)(ps->py+ps->sy);






			D3DD_DrawTexture( &disp );

			if(ac->cfade && ss->code[ac->code_no].code==SPCODE_DRAW ){
				
				PARTS_STRUCT	*ps2 = &as->cs[ss->code[ac->next_code].data1].ps[0];
				int				rate = 255*ac->count/ss->code[ac->code_no].data2;

				disp.blend_param=BLEND_ALPHA;	


				rparam = sp_disp->rev_param^cs->rparam^ps2->rparam;	
				if(rparam&0x10)	disp.rev_param |= REVERSE_W;	
				if(rparam&0x20)	disp.rev_param |= REVERSE_H;

				disp.rander_no  = -1;	
				disp.textue_no  = as->code2bno[ps2->bno];	
				disp.textue_no2 = -1;	

				disp.bright_no = sp_disp->bright_no;
				
				*(DWORD*)&disp.color[0] = D3DCOLOR_ARGB(rate, disp.color[0].r, disp.color[0].g, disp.color[0].b );	
				*(DWORD*)&disp.color[1] = D3DCOLOR_ARGB(rate, disp.color[1].r, disp.color[1].g, disp.color[1].b );
				*(DWORD*)&disp.color[2] = D3DCOLOR_ARGB(rate, disp.color[2].r, disp.color[2].g, disp.color[2].b );
				*(DWORD*)&disp.color[3] = D3DCOLOR_ARGB(rate, disp.color[3].r, disp.color[3].g, disp.color[3].b );





				dx  = (float)(sp_disp->dx[0] + ps2->cx);	
				dy  = (float)(sp_disp->dy[0] + ps2->cy);
				dsx = ps2->sx;	
				dsy = ps2->sy;
				disp.dx[0] = dx;	
				disp.dx[1] = dx+dsx;
				disp.dx[2] = dx;
				disp.dx[3] = dx+dsx;

				disp.dy[0] = dy;
				disp.dy[1] = dy;
				disp.dy[2] = dy+dsy;
				disp.dy[3] = dy+dsy;

				disp.rhw[0] = 1.0f;
				disp.rhw[1] = 1.0f;
				disp.rhw[2] = 1.0f;
				disp.rhw[3] = 1.0f;

				disp.sx1[0] = (float) ps2->px;	
				disp.sx1[1] = (float)(ps2->px+ps2->sx);
				disp.sy1[0] = (float) ps2->py;
				disp.sy1[1] = (float)(ps2->py+ps2->sy);

				D3DD_DrawTexture( &disp );
			}
		}
	}
}

void SPR_DrawChip( CHIP_STRUCT *cs, ANIME_CONTROL *ac, D3DD_DISP *sp_disp )
{
	int				i;
	
	if( cs->flag ){	
		ac->draw_rect.top    = 0;
		ac->draw_rect.bottom = 600;
		ac->draw_rect.left   = 0;
		ac->draw_rect.right  = 800;
		for(i=0;i<cs->ps_max;i++){
			SPR_DrawParts( &cs->ps[i], cs, ac, sp_disp );
		}
	}
}

void SPR_DrawSprite( int ac_no, D3DD_DISP *sp_disp )
{
	SPRITE_STRUCT	*ss;
	ANIME_STRUCT	*as;
	ANIME_CONTROL	*ac = &AnimeControl[ac_no];



	if(!ac->flag)					{ DebugPrintf("AnimeControlに登録されていません[%d]\n",ac_no); return; }
	if(!AnimeStruct[ac->ano].flag)	{ DebugPrintf("AnimeStructに登録されていません[%d]\n",ac->ano); return; }
	as = &AnimeStruct[ac->ano];

	if( !(ac->mode==SP_STOP && ac->end==0) ){
		if(as->cs && as->ss){
			ss = &as->ss[ac->sp_no];
			if(ss->code){
				SPR_DrawChip( &as->cs[ ss->code[ac->code_no].data1 ], ac, sp_disp );
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
		if(ac->flag && ac->mode && ac->pause==0){
			ANIME_STRUCT	*as = &AnimeStruct[ac->ano];
			SPRITE_STRUCT	*ss = &as->ss[ac->sp_no];

			ac->draw_count += as->frame*ac->speed/frame;
			RenewSprite( ss->code, &ac->count, &ac->code_no, &ac->draw_count, (char *)&ac->loop_no, (char*)ac->lcount, ac );

			
			if(ac->cfade){
				long	count = ac->count;
				long	code_no = ac->code_no;
				char	loop_no = ac->loop_no;
				char	lcount[16];
				CopyMemory( lcount, ac->lcount, 16 );





				GetNextSprite( ss->code, &count, &code_no, &loop_no, lcount );
				ac->next_code = code_no;
			}

		}
	}
}



static int SPR_LoadTexturSet( int tno, char *fname, char *tfname, char *tfname2, int chroma, int alpha, int low_level, int mip, char *pak )
{
	if( !_mbsicmp( (BYTE *)STD_GetAttr(fname), (BYTE *)"bbm") ){
		if( D3DD_LoadTextureBBM( tno, fname, alpha, low_level, mip, tfname, tfname2, chroma, pak ) ){
			return 1;
		}else{
			return -1;
		}
	}else{
		if( D3DD_LoadTexture( tno, fname, -1, -1, alpha, FALSE, low_level, mip, tfname, tfname2, chroma, pak ) ){
			return 1;
		}else{
			return -1;
		}
	}
}
static int SPR_ReleaseTexturSet( int tno )
{
	return D3DD_ReleaseTexture( tno );
}



int SPR_LoadBmp( char *fname, char *tfname, char *tfname2, int chroma, int alpha, int low_level, int mip, char *pak )
{
	int		i,ret;

	for( i=D3DD_SPRITE_TEXTURE_START ; i<D3DD_MAX_TEXTURE_AMOUNT ; i++ ){
		D3DD_TEXTURE *d3dt = D3DD_GetTexture( i );

		if( !d3dt->flag ){
			break;
		}
	}
	if(i==D3DD_MAX_TEXTURE_AMOUNT){
		DebugBox( NULL, "スプライトが多過ぎます。テクスチャー領域が足りません" );
		return -1;
	}

	ret = SPR_LoadTexturSet( i, fname, tfname, tfname2, chroma, alpha, low_level, mip, pak );

	if(ret==-1) {
		DebugBox( NULL, "スプライトファイルの読み込みに失敗しました[%s]", fname );
		return -1;
	}
	return i;
}

int SPR_CopyBmp( int src_bno, char *tfname, int chroma, int r, int g, int b )
{

	return 0;
}


BOOL SPR_CopySprite( int ani_no1,  int ani_no2, char *tfname, int chroma, int r, int g, int b )
{

	return TRUE;
}

BOOL SPR_LoadSprite( int ani_no, char *fname, char *tfname, char *tfname2, int chroma, int alpha, int low_level, int mip, char *pak )
{
	int				i;
	ANIME_STRUCT	*as = &AnimeStruct[ani_no];
	CHIP_STRUCT		*cs;
	SPRITE_STRUCT	*ss;
	DWORD			size;
	BYTE			*buf=NULL, *ptr;
	SPANI_HEADER	shead;


	SPR_ReleaseSprite(ani_no);

	if(pak){
		size = PAC_LoadFile( pak, fname, &buf);
	}else{
		size = PAC_LoadFile( D3DD_GetPackDir(), fname, &buf);
	}
	if(!size) return FALSE;
	ptr = buf;
	CopyMemory( &shead, ptr, sizeof(SPANI_HEADER) );
	ptr += sizeof(SPANI_HEADER);

	if( shead.header!='SANI' ){
		if( buf ) buf = (BYTE *)GlobalFree( buf );
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

		
		ss->code = (SPRITE_OPR *)GAlloc(sizeof(SPRITE_OPR)*(ss->code_max+1) );
		CopyMemory( ss->code, ptr, sizeof(SPRITE_OPR)*(ss->code_max+1) );
		ptr += sizeof(SPRITE_OPR)*(ss->code_max+1);
	}

	


	for(i=0; i<as->bmp_max ;i++){

		as->code2bno[i] = SPR_LoadBmp( (char *)ptr, tfname, tfname2, chroma, alpha, low_level, mip, pak );
		ptr += strlen((char*)ptr)+1;
	}
	if( buf ) buf = (BYTE *)GlobalFree( buf );

	as->flag = 1;




	return TRUE;
}

BOOL SPR_LoadBmpSprite( int ani_no, char *fname, int nuki, char *tfname, int chroma )
{

	return TRUE;
}




BOOL SPR_ReleaseSprite( int ani_no )
{
	int				i;
	if(ani_no<0) return FALSE;
	ANIME_STRUCT	*as = &AnimeStruct[ani_no];

	if(as->flag){
		for( i=0 ; i<as->cs_max ; i++ ){
			if( as->cs[i].ps ) as->cs[i].ps = (PARTS_STRUCT *)GlobalFree( as->cs[i].ps );
		}
		if( as->cs   ) as->cs   = (CHIP_STRUCT *)GlobalFree( as->cs );

		for( i=0 ; i<as->ss_max ; i++ ){
			if( as->ss[i].code ) as->ss[i].code = (SPRITE_OPR  *)GlobalFree( as->ss[i].code );
		}
		if( as->ss ) as->ss = (SPRITE_STRUCT *)GlobalFree( as->ss );

		
		
		for(i=0; i<as->bmp_max ;i++){
			int	j = as->code2bno[i];
			SPR_ReleaseTexturSet( j );
		}
	}
	ZeroMemory( as, sizeof(SPRITE_STRUCT) );

	return TRUE;
}


void SPR_ReleaseSpriteAll( void )
{
	int	i;

	for( i=0 ; i<ANIME_MAX ; i++ ){
		SPR_ReleaseSprite( i );
	}
	for( i=0 ; i<ANIME_CONTROL_MAX ; i++ ){
		SPR_ResetSprite( i );
	}
}


int  SPR_GetSpriteNoMax( int ani_no )
{
	ANIME_STRUCT	*as = &AnimeStruct[ani_no];
	int	num=0;
	if(as->flag){
		num = as->ss_max;
	}
	return num;
}


int SPR_SetSprite( int ani_no, int sp_no, int mode, int end, int lnum, int cfade )
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
	if(sp_no<0 || AnimeStruct[ani_no].ss_max<=sp_no) {
		DebugPrintf("指定されたスプライトパターンは存在しません[%d]\n",sp_no);  return -1;
	}

	ZeroMemory( ac, sizeof(ANIME_CONTROL) );
	ac->flag = 1;
	ac->ano  = ani_no;
	ac->mode = mode;	
	ac->end  = end;		
	ac->lnum = lnum;	
	ac->cfade = cfade;

	ac->sp_no = sp_no;

	ac->pause = 0;

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

BOOL SPR_SetSpritePlay( int ac_no, int ani_no, int sp_no, int mode, int end, int lnum, int cfade )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります[%d]\n",ac_no); return FALSE; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました[%d]\n",ac_no);  return FALSE; }
	if(ani_no<0 || ANIME_MAX<=ani_no)                { DebugPrintf("存在しないスプライト領域が指定されました[%d]\n",ani_no);  return FALSE; }
	if(AnimeStruct[ani_no].flag==0)                  { DebugPrintf("存在しないスプライト領域が指定されました[%d]\n",ani_no);  return FALSE; }
	if(sp_no<0 || AnimeStruct[ani_no].ss_max<=sp_no) {
		DebugPrintf("指定されたスプライトパターンは存在しません[%d]\n",sp_no);  return FALSE;
	}

	ANIME_CONTROL	*ac = &AnimeControl[ac_no];

	if( !(ac->ano==ani_no && ac->sp_no==sp_no) || lnum==0 ){
		ZeroMemory( ac, sizeof(ANIME_CONTROL) );
	}
	if( ac->lnum==1 && ac->mode==SP_STOP ){
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
	if(sp_no<0 || AnimeStruct[ac->ano].ss_max<=sp_no) {
		DebugPrintf("指定されたスプライトパターンは存在しません1[%d]\n",sp_no);  return FALSE;
	}

	if( ac->sp_no!=sp_no ){
		ac->sp_no = sp_no;
		ac->mode = SP_PLAY;

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
		ret=0;
	}
	return ret;
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


BOOL SPR_SetSpritePause( int ac_no, int pause )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります10[%d]\n",ac_no); return FALSE; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました10[%d]\n",ac_no); return FALSE; }

	AnimeControl[ac_no].pause = pause;
	return TRUE;
}



int SPR_GetSpriteEvent( int ac_no )
{
	if( ac_no<0 || ANIME_CONTROL_MAX<=ac_no )        { DebugPrintf("スプライト制御バッファの指定に誤りがあります11[%d]\n",ac_no); return 0; }
	if(AnimeControl[ac_no].flag==0)                  { DebugPrintf("存在しないスプライト制御バッファが指定されました11[%d]\n",ac_no);  return 0; }
	int				event;
	event = AnimeControl[ac_no].event;
	AnimeControl[ac_no].event = 0;
	return event;
}
