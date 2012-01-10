
#include "mm_std.h"
#include "main.h"

#include "bmp.h"
#include "draw.h"
#include "escript.h"
#include "disp.h"
#include "text.h"
#include "keybord.h"
#include "mouse.h"

#include "main.h"
#include "GM_avg.h"
#include "GM_Save.h"
#include "GM_title.h"


#define CHAR_COND_NOMAL		0
#define CHAR_COND_IN		1
#define CHAR_COND_OUT		2
#define CHAR_COND_POSE		3	
#define CHAR_COND_LOCATE	4	
#define CHAR_COND_BRIGHT	5	
#define CHAR_COND_ALPHA		6	
#define CHAR_COND_WAIT		7	

#define CHAR_TYPE_DIRECT	-1	
#define CHAR_TYPE_CFADE		0	
#define CHAR_TYPE_MOVE		1	
#define CHAR_TYPE_WAIT		3	
#define CHAR_TYPE_WAIT2		4	
#define CHAR_TYPE_WAVE		5	


static int	CharLocateTable[9]={ -160*80/64, 0, 160*80/64, -192*80/64, 192*80/64, -300*80/64, 300*80/64, -400*80/64, 400*80/64 };
#define CHAR_DY	0

#define CHAR_LOCATE(L)		(CharLocateTable[L])	

typedef struct{
	int		flag;	
	int		grp;	
	int		cond;	
	int		type;	
	int		cno;	
	int		disp;	

	int		cnt;	
	int		max;	

	int		pose;	
	int		loc1;	
	int		loc2;	
	int		layer;	

	int		fade1;	
	int		fade2;	

	int		alph1;	
	int		alph2;	

	int		cut_mode;	
}CHAR_STRUCT;
CHAR_STRUCT		CharStruct[MAX_CHAR];




static int GetCharIndex( int char_no )
{
	int	i;
	for(i=0;i<MAX_CHAR;i++){
		if( CharStruct[i].flag && CharStruct[i].cno==char_no ){
			break;
		}
	}
	return i; 
}
static int GetSpaceIndex( void )
{
	int	i;
	for(i=0;i<MAX_CHAR;i++){
		if( !CharStruct[i].flag ){
			break;
		}
	}
	return i; 
}
static BOOL CheckCharFlag( void )
{
	int		i;
	BOOL	ret=FALSE;
	for(i=0;i<MAX_CHAR;i++){
		ret |= (CharStruct[i].cond!=CHAR_COND_OUT && CharStruct[i].flag);
	}
	return ret; 
}



static int CharPosTable[12]={0,0,0,0,0,0,0,0,0,0,0,0};
static void SetCharPos( int index, int x )
{
	if(CharStruct[index].cno<10){
		if( !( (90<=CharStruct[index].pose && CharStruct[index].pose<100) ||
			(190<=CharStruct[index].pose && CharStruct[index].pose<200) ) ){
			x += CharPosTable[ CharStruct[index].cno ];
		}
	}

	DSP_SetGraphPos( GRP_CHAR+index, x,-CHAR_DY,  0,0,  DISP_X, DISP_Y+CHAR_DY*2 );
}
void SetCharPosShake( int shx, int shy, int disp )
{
	int	i,x=0;

	for(i=0;i<MAX_CHAR;i++){
		if(CharStruct[i].flag){
			if(CharStruct[i].cno<10){
				if( !( (90<=CharStruct[i].pose && CharStruct[i].pose<100) ||
					(190<=CharStruct[i].pose && CharStruct[i].pose<200) ) ){
					x = CharPosTable[ CharStruct[i].cno ];
				}
			}
			DSP_SetGraphMove( GRP_CHAR+i, CHAR_LOCATE(CharStruct[i].loc1)+shx+x, shy-CHAR_DY );
			DSP_SetGraphMove( GRP_SPBACK+i+1, CHAR_LOCATE(CharStruct[i].loc1)+shx+x, shy-CHAR_DY );
			
			if( CharStruct[i].cond==CHAR_COND_NOMAL && disp!=-1 ){
				if(disp)CharStruct[i].cut_mode=2;
				else	CharStruct[i].cut_mode=0;
			}
		}
	}
}
void SetCharBright( int r, int g, int b )
{
	int	i;
	for(i=0;i<MAX_CHAR;i++){
		if(CharStruct[i].flag){
			DSP_SetGraphBright( GRP_SPBACK+i+1, r,g,b );
		}
	}
}



void SetCharHalfTone( int flag )
{
	int	i;
/*
	if(!flag){
		for( i=0 ; i<MAX_CHAR ; i++ ){
			if(CharStruct[i].flag)
				DSP_SetGraphBmp( GRP_CHAR+i, BMP_CHARHALF+i*2+CharStruct[i].grp );
		}
	}else{
		for( i=0 ; i<MAX_CHAR ; i++ ){
			if(CharStruct[i].flag)
				DSP_SetGraphBmp( GRP_CHAR+i, BMP_CHAR+i*2+CharStruct[i].grp );
		}
	}
*/
	return ; 
}

void AVG_LoadChar( int index, int char_no, int pose, int tone_no, int in_type )
{
	char	fname[256];
	char	*tname;
	int		bmp_mode = (MainWindow.draw_mode2==16)? BMP_HIGH: BMP_TRUE;

	MainWindow.draw_flag=ON;

	if( !LIM_CHK(char_no,0,100) )	char_no = 0;
	if( !LIM_CHK(tone_no,0,10) ){
		tone_no = BackStruct.tone_no;
	}


	wsprintf( fname, "c%03d%05d.tga", char_no, pose );

	if( BackStruct.tone_char==-1 ){	
		if( PAC_CheckFile( BMP_GetPackDir(), tone_fname2 ) && in_type!=CHAR_TYPE_WAIT ){
			tname=tone_fname2;
		}else{
			tname=tone_fname[tone_no%4];
		}
	}else{
		tone_no = BackStruct.tone_char;
		tname=tone_fname[tone_no%4];	
	}
	

	DSP_LoadBmp( BMP_CHAR+index*2+CharStruct[index].grp, bmp_mode, fname, tname, 256 );

	if(BackStruct.wether){
		DSP_SetTonecurve( BMP_CHAR+index*2+CharStruct[index].grp, "rain.AMP", 256 );
	}
	char	*eff_fname=NULL;
	int		viv=256;
	switch( tone_no/TONE_MAX ){
		case 1: eff_fname = "sepia.amp";		viv=0;		break;	
		case 2: eff_fname = "nega.amp";			viv=256;	break;	
		case 3: eff_fname = NULL;				viv=0;		break;	
		case 4: eff_fname = "blue.amp";			viv=128;	break;	
		case 5: eff_fname = "red.amp";			viv=128;	break;	
		case 6: eff_fname = "green.amp";		viv=128;	break;	
		case 7: eff_fname = "blue2.amp";		viv=128;	break;	
		case 8: eff_fname = "brown.amp";		viv=128;	break;	
		case 9:	eff_fname = "sepia_half.AMP";	viv = 128;	break;	

		case 10:eff_fname = "black.AMP";		viv = 0;	break;	
		case 11:eff_fname = "yoritomo.AMP";		viv = 0;	break;	
	}

	if(tone_no/TONE_MAX){
		DSP_SetTonecurve( BMP_CHAR+index*2+CharStruct[index].grp, eff_fname, viv );
	}
	DSP_CopyBmp2( BMP_CHARHALF+index*2+CharStruct[index].grp, BMP_CHAR+index*2+CharStruct[index].grp, NULL, 256,
			BackStruct.r*AVG_GetHalfToneFade()/128,
			BackStruct.g*AVG_GetHalfToneFade()/128,
			BackStruct.b*AVG_GetHalfToneFade()/128 );

	DSP_SetGraph( GRP_CHAR+index, BMP_CHAR+index*2+CharStruct[index].grp, LAY_CHAR+CharStruct[index].layer, ON, CHK_ANTI );
	SetCharPos( index, CHAR_LOCATE(CharStruct[index].loc1)  );
}

int AVG_CheckCharLocate( int char_no )
{
	int index  = GetCharIndex(char_no);
	if( index!=MAX_CHAR ){
		return CharStruct[index].loc1;
	}
	return -1;
}

void AVG_SetChar( int char_no, int pose, int locate, int layer, int in_type, int bright, int alph, int frame )
{
	int	index, page=0;

	index  = GetCharIndex(char_no);






	if( index!=MAX_CHAR ){

		if( CharStruct[index].cno==char_no && CharStruct[index].pose==pose && CharStruct[index].loc1==locate &&
			CharStruct[index].fade1==bright && CharStruct[index].layer==layer && CharStruct[index].alph1==alph )
		{
			return ;
		}
	}

	if(in_type!=CHAR_TYPE_WAIT){
		AVG_ResetHalfTone();
		AVG_SetNovelMessageDisp(OFF);
	}
/*	if(!Avg.msg_page){
	}else{
		
		if(in_type!=CHAR_TYPE_WAIT){
			DSP_CopyBmp( BMP_BACK, BMP_BACK2 );
			in_type=CHAR_TYPE_WAIT;
		}else{
			page=1;
		}
	}
*/

	if( index!=MAX_CHAR ){

		if( CharStruct[index].cno==char_no && CharStruct[index].pose==pose && CharStruct[index].loc1==locate &&
			CharStruct[index].fade1==bright && CharStruct[index].layer==layer && CharStruct[index].alph1==alph )
		{
			return ;
		}

		if( in_type==CHAR_TYPE_WAIT || in_type==CHAR_TYPE_DIRECT ){
			CharStruct[index].flag = 1;
			if(in_type==CHAR_TYPE_WAIT){
				CharStruct[index].cond = CHAR_COND_WAIT;


				CharStruct[index].cut_mode = 0;
			}else{
				CharStruct[index].cond = CHAR_COND_NOMAL;
				AVG_LoadChar( index, char_no, pose, BackStruct.tone_no, in_type );
				DSP_SetGraphParam( GRP_CHAR+index, DRW_BLD(alph) );
				CharStruct[index].cut_mode = 0;
			}
			CharStruct[index].type = in_type;
			CharStruct[index].cno  = char_no;
			CharStruct[index].pose = pose;
			CharStruct[index].loc1 = locate;
			CharStruct[index].fade1= bright;
			CharStruct[index].cnt  = 0;
			CharStruct[index].max  = frame;
			CharStruct[index].grp  = 0;
			CharStruct[index].layer= layer;
			CharStruct[index].alph1= alph;

		}else{
			if( CharStruct[index].pose!=pose )	AVG_SetCharPose( char_no, pose, CHAR_TYPE_CFADE, frame );

		}
	}else{
		index = GetSpaceIndex();

		if( index!=MAX_CHAR ){
			if( in_type==CHAR_TYPE_WAIT || in_type==CHAR_TYPE_DIRECT ){
				CharStruct[index].flag = 1;
				if(in_type==CHAR_TYPE_WAIT){
					CharStruct[index].cond = CHAR_COND_WAIT;
					CharStruct[index].cut_mode = 0;
				}else{
					CharStruct[index].cut_mode = 0;
					CharStruct[index].cond = CHAR_COND_NOMAL;

					AVG_LoadChar( index, char_no, pose, BackStruct.tone_no, in_type );
					DSP_SetGraphParam( GRP_CHAR+index, DRW_BLD(alph) );
				}
				CharStruct[index].type = in_type;
				CharStruct[index].cno  = char_no;
				CharStruct[index].pose = pose;
				CharStruct[index].loc1 = locate;
				CharStruct[index].fade1= bright;
				CharStruct[index].cnt  = 0;
				CharStruct[index].max  = frame;
				CharStruct[index].grp  = 0;
				CharStruct[index].layer= layer;
				CharStruct[index].alph1= alph;
			}else{
				CharStruct[index].flag  = 1;
				CharStruct[index].cond  = CHAR_COND_IN;
				CharStruct[index].type  = in_type;
				CharStruct[index].cno   = char_no;
				CharStruct[index].pose  = pose;
				CharStruct[index].loc1  = locate;
				CharStruct[index].fade1 = bright;
				CharStruct[index].cnt   = 0;
				CharStruct[index].max   = frame;
				CharStruct[index].grp   = 0;
				CharStruct[index].layer = layer;
				CharStruct[index].alph1 = alph;
				CharStruct[index].cut_mode = 1;

				AVG_LoadChar( index, char_no, pose, BackStruct.tone_no, in_type );
			}
		}else{
			DebugBox( NULL, "これ以上、キャラクターを登録出来ません" );
		}
	}
/*	if(Avg.msg_page && !page){
		
		AVG_SetBackChar( 0, 0, 1 );
		AVG_SetHalfToneDirect();
	}
*/
}
void AVG_ReleaseChar( int char_index )
{
	DSP_ResetGraph( GRP_CHAR+char_index );
	DSP_ReleaseBmp( BMP_CHAR+char_index*2   );
	DSP_ReleaseBmp( BMP_CHAR+char_index*2+1 );
	ZeroMemory( &CharStruct[char_index], sizeof(CHAR_STRUCT) );
}
void AVG_ResetChar( int char_no, int out_type, int frame )
{
	int	index = GetCharIndex(char_no);

	
	if( index!=MAX_CHAR ){
		if(out_type==CHAR_TYPE_WAIT){
			CharStruct[index].type = CHAR_TYPE_WAIT2;
			return;
		}

		MainWindow.draw_flag=ON;

			AVG_ResetHalfTone();
			AVG_SetNovelMessageDisp(OFF);

			CharStruct[index].cond = CHAR_COND_OUT;
			CharStruct[index].type = out_type;
			CharStruct[index].cnt  = 0;
			CharStruct[index].max  = frame;
/*		}else{
			
			DSP_CopyBmp( BMP_BACK, BMP_BACK2 );
			CharStruct[index].type = CHAR_TYPE_WAIT2;
			
			AVG_SetBackReleaseChar();
			AVG_SetBackChar( 0, 0, 1 );
			DSP_CopyBmp2( BMP_BACKHALF, BMP_BACK, NULL, 256, BackStruct.r*MessageFade/128, BackStruct.g*MessageFade/128, BackStruct.b*MessageFade/128  );
		}
*/
	}else{

	}
/*	if( !CheckCharFlag() ){
		if( BackStruct.flag ){
			if(!BackStruct.cs_lock){
				if(out_type==-1)	AVG_SetBackCScope( OFF, 0 );
				else				AVG_SetBackCScope( OFF, frame );
			}
		}
	}
*/
}
void AVG_OpenChar( void )
{
	int	i;
	for(i=0;i<MAX_CHAR;i++){
		DSP_SetGraphDisp( GRP_CHAR+i, ON );
	}
}
void AVG_CloseChar( void )
{
	int	i;
	for(i=0;i<MAX_CHAR;i++){
		DSP_SetGraphDisp( GRP_CHAR+i, OFF );
	}
}


void AVG_SetCharPose( int char_no, int pose, int in_type, int frame )
{
	int		index = GetCharIndex(char_no);
	
	if( index!=MAX_CHAR ){
		if(in_type==CHAR_TYPE_WAIT){
			AVG_SetChar( char_no, pose, CharStruct[index].loc1, CharStruct[index].layer,
				CHAR_TYPE_WAIT, CharStruct[index].fade1, CharStruct[index].alph1, frame );
		}else{
			if( CharStruct[index].pose == pose ) return;

				CharStruct[index].cond = CHAR_COND_POSE;
				CharStruct[index].pose = pose;
				CharStruct[index].cnt  = 0;
				CharStruct[index].max  = frame;
				CharStruct[index].grp  = (CharStruct[index].grp)?0:1;

				AVG_ResetHalfTone();
				AVG_SetNovelMessageDisp(OFF);

				AVG_LoadChar( index, char_no, pose, BackStruct.tone_no, CHAR_TYPE_CFADE );
				DSP_SetGraphBSet( GRP_CHAR+index, BMP_CHAR+index*2+!CharStruct[index].grp, 0 );




		}
	}else{
		AVG_SetChar( char_no, pose, 1, 0, in_type, 128, 256, frame );
		DebugPrintf( "登録していないキャラクターです" );
	}
}
void AVG_SetCharLocate( int char_no, int locate, int frame )
{
	int	index = GetCharIndex(char_no);

	if( index!=MAX_CHAR ){
		CharStruct[index].cond = CHAR_COND_LOCATE;
		CharStruct[index].loc2 = CharStruct[index].loc1;
		CharStruct[index].loc1 = locate;
		CharStruct[index].cnt  = 0;
		CharStruct[index].max  = frame;


			AVG_ResetHalfTone();
			AVG_SetNovelMessageDisp(OFF);



	}else{

	}
}
void AVG_SetCharLayer( int char_no, int layer )
{
	int	index = GetCharIndex(char_no);
	if( index!=MAX_CHAR ){
		if( layer<-1 || layer>5 ){
			DebugBox(NULL,"キャラクターのレイヤは、0～4の間に収めてください[%d]",layer);
			layer = LIM(layer,0,4);
		}
		CharStruct[index].layer  = layer;
		DSP_SetGraphLayer( GRP_CHAR+index, LAY_CHAR+CharStruct[index].layer );


			AVG_ResetHalfTone();
			AVG_SetNovelMessageDisp(OFF);



	}else{

	}
}

void AVG_SetCharBright( int char_no, int fade, int fade_count )
{
	int	index = GetCharIndex(char_no);

	if( index!=MAX_CHAR ){
		if(fade_count<=0){
			CharStruct[index].cond  = CHAR_COND_NOMAL;
			CharStruct[index].fade2 = CharStruct[index].fade1;
			CharStruct[index].fade1 = fade;
			CharStruct[index].cnt   = 0;
			CharStruct[index].max   = fade_count;
			CharStruct[index].cut_mode = 0;
			DSP_SetGraphBright( GRP_CHAR+index, fade, fade, fade );
		}else{
			CharStruct[index].cond  = CHAR_COND_BRIGHT;
			CharStruct[index].fade2 = CharStruct[index].fade1;
			CharStruct[index].fade1 = fade;
			CharStruct[index].cnt   = 0;
			CharStruct[index].max   = fade_count;
		}


			AVG_ResetHalfTone();
			AVG_SetNovelMessageDisp(OFF);



	}else{

	}
}

void AVG_SetCharAlph( int char_no, int alph, int fade_count )
{
	int	index = GetCharIndex(char_no);

	if( index!=MAX_CHAR ){
		if(fade_count<=0){
			CharStruct[index].cond  = CHAR_COND_NOMAL;
			CharStruct[index].alph2 = CharStruct[index].alph1;
			CharStruct[index].alph1 = alph;
			CharStruct[index].cnt   = 0;
			CharStruct[index].max   = fade_count;
			CharStruct[index].cut_mode = 0;
			DSP_SetGraphParam( GRP_CHAR+index, DRW_BLD(alph) );
		}else{
			CharStruct[index].cond  = CHAR_COND_ALPHA;
			CharStruct[index].alph2 = CharStruct[index].alph1;
			CharStruct[index].alph1 = alph;
			CharStruct[index].cnt   = 0;
			CharStruct[index].max   = fade_count;
		}


			AVG_ResetHalfTone();
			AVG_SetNovelMessageDisp(OFF);



	}else{

	}
}

void AVG_SetBackReleaseChar( void )
{
	int	i;
	for(i=0;i<MAX_CHAR;i++){
		if(CharStruct[i].type==CHAR_TYPE_WAIT2){
			AVG_ReleaseChar( i );
		}
	}
}

void AVG_SetBackChar( int x, int y, int char_disp )
{
	int		l,i,mx,my,sx,sy;

	for( l=0; l<5 ;l++ ){
		for(i=0;i<MAX_CHAR;i++){
			if(CharStruct[i].layer==l){
				if( CharStruct[i].flag && (CharStruct[i].cond==CHAR_COND_NOMAL||CharStruct[i].cond==CHAR_COND_WAIT) ){
					CharStruct[i].cond     = CHAR_COND_NOMAL;
					AVG_LoadChar( i, CharStruct[i].cno, CharStruct[i].pose, BackStruct.tone_no, CHAR_TYPE_DIRECT );

					if(char_disp==1)CharStruct[i].cut_mode = 1;
					else			CharStruct[i].cut_mode = 0;
					DSP_SetGraphParam(  GRP_CHAR+i, DRW_BLD(CharStruct[i].alph1) );
					DSP_SetGraphFade(   GRP_CHAR+i, CharStruct[i].fade1 );

					DSP_GetGraphMove(   GRP_CHAR+i, &mx, &my );
					DSP_SetGraphMove(   GRP_CHAR+i, mx+x, my+y );
					DSP_GetBmpSize( BMP_BACK, &sx, &sy );
					DSP_SetGraphClip(   GRP_CHAR+i, 0, 0, sx, sy );
					DSP_SetGraphTarget( GRP_CHAR+i, BMP_BACK, MainWindow.draw_mode2 );
					DSP_SetGraphMove(   GRP_CHAR+i, mx, my );

				}
			}
		}
	}
}
void AVG_ControlSpChar( void )
{
	int	i,l,mx,my,r,g,b;

	for(i=0;i<MAX_CHAR;i++){
		DSP_ResetGraph( GRP_SPBACK+i+1 );
	}
	if(BackStruct.sp_flag==2){
		for( l=0; l<5 ;l++ ){
			for(i=0;i<MAX_CHAR;i++){
				if(CharStruct[i].flag && CharStruct[i].layer==l){
					if( CharStruct[i].cond==CHAR_COND_NOMAL || CharStruct[i].cond==CHAR_COND_WAIT ){
						
						if( DSP_GetGraphDisp(GRP_SPBACKHALF) ){
							DSP_SetGraph( GRP_SPBACK+i+1, BMP_CHARHALF+i*2+CharStruct[i].grp, LAY_CHAR+CharStruct[i].layer, ON, CHK_ANTI  );
							DSP_SetGraphClipSprit( GRP_SPBACK+i+1, GRP_SPBACKHALF );
						}else{
							DSP_SetGraph( GRP_SPBACK+i+1, BMP_CHAR+i*2+CharStruct[i].grp, LAY_CHAR+CharStruct[i].layer, ON, CHK_ANTI  );
							DSP_SetGraphClipSprit( GRP_SPBACK+i+1, GRP_SPBACK );
							DSP_GetGraphBright( GRP_SPBACK, &r, &g, &b );
							SetCharBright( r, g, b );
						}
						DSP_SetGraphParam(  GRP_SPBACK+i+1, DRW_BLD(CharStruct[i].alph1) );
						DSP_GetGraphMove(   GRP_CHAR+i, &mx, &my );
						DSP_SetGraphMove(   GRP_SPBACK+i+1, mx, my );
					}
				}
			}
		}
	}
}

BOOL AVG_WaitChar( int char_no )
{
	int	index = GetCharIndex(char_no);

	if( index!=MAX_CHAR ){
		return CharStruct[index].cond!=CHAR_COND_NOMAL && CharStruct[index].cond!=CHAR_COND_WAIT;
	}else{
		return FALSE;
	}
}

void AVG_ControlChar( void )
{
	int	i,j,l;
	int	max2;
	int	sp  = Avg.level;
	int	ami = Avg.ami;
	int	rate;
	int	cnt,max,locate,disp=0;;
	int	sx,sy;

	if( BackStruct.flag ){
		
		if( !BackStruct.sc_flag && !BackStruct.zoom ){
			for( i=0; i<MAX_CHAR ;i++ ){
				if(CharStruct[i].flag){
					
					if( CharStruct[i].cond!=CHAR_COND_NOMAL && CharStruct[i].cut_mode ){
						disp = 1;
						break;
					}
				}
			}
		}

		
		if(BackStruct.redraw){
			BackStruct.redraw = 0;
			disp = 1;
		}

		
		if(disp){
			
			AVG_CopyBack(OFF);
			AVG_CreateBackCScope();
			for( i=0; i<MAX_CHAR ;i++ ){
				CharStruct[i].cut_mode = 0;
			}
		}

		for( i=0; i<MAX_CHAR ;i++ ){
			if(CharStruct[i].flag && CharStruct[i].cond!=CHAR_COND_NOMAL && CharStruct[i].cond!=CHAR_COND_WAIT ){
				for( j=0; j<MAX_CHAR ;j++ ){
					if(CharStruct[j].flag && CharStruct[j].cond==CHAR_COND_NOMAL && CharStruct[i].cond!=CHAR_COND_WAIT ){
						if( CharStruct[j].layer*MAX_CHAR+j > CharStruct[i].layer*MAX_CHAR+i){
							CharStruct[j].disp = 1;
						}
					}
				}
			}
		}
		for( l=0; l<5 ;l++ ){
			for( i=0; i<MAX_CHAR ;i++ ){
				if(CharStruct[i].flag && CharStruct[i].layer==l){
					if( CharStruct[i].cond==CHAR_COND_NOMAL && !CharStruct[i].disp ){
						switch( CharStruct[i].cut_mode ){
							case 0:	
								DSP_SetGraphDisp(   GRP_CHAR+i, ON );

								if(1){
									DSP_GetBmpSize( BMP_BACK, &sx, &sy );
									DSP_SetGraphClip(   GRP_CHAR+i, 0, 0, sx, sy );

									DSP_GetGraphMove(   GRP_CHAR+i, &sx, &sy );
									DSP_SetGraphMove(   GRP_CHAR+i, sx+BackStruct.x, sy+BackStruct.y );
								}
								DSP_SetGraphTarget( GRP_CHAR+i, BMP_BACK, MainWindow.draw_mode2 );
								DSP_SetGraphMove(   GRP_CHAR+i, sx, sy );

								CharStruct[i].cut_mode = 1;
								break;
							case 1:	
								DSP_SetGraphDisp( GRP_CHAR+i, OFF );
								break;
							case 2:
								DSP_SetGraphDisp( GRP_CHAR+i, ON );
								break;
						}
					}else{
						if(CharStruct[i].cond!=CHAR_COND_WAIT){
							CharStruct[i].disp = 0;
							CharStruct[i].cut_mode = 0;
							DSP_SetGraphDisp( GRP_CHAR+i, ON );
						}else{
							CharStruct[i].cond=CHAR_COND_WAIT;
						}
					}
				}
			}
		}
	}
	for( i=0; i<MAX_CHAR ;i++ ){
		int	param =DRW_BLD(CharStruct[i].alph1);
		int	bright=CharStruct[i].fade1;

		max = max2 = AVG_EffCnt( CharStruct[i].max );
		cnt = CharStruct[i].cnt;
		if(CharStruct[i].flag){
			switch( CharStruct[i].cond ){
				case CHAR_COND_NOMAL:	
					break;
				case CHAR_COND_WAIT:	
					DSP_SetGraphDisp( GRP_CHAR+i, OFF );
					break;
				case CHAR_COND_IN:		
					if(max){
						switch(CharStruct[i].type){
						case CHAR_TYPE_MOVE:
						case CHAR_TYPE_MOVE+1:
							cnt = max-cnt;
							cnt = cnt*cnt*cnt;
							max = max*max*max;
							cnt = max-cnt;
							if(CharStruct[i].type==CHAR_TYPE_MOVE) {	locate = -600;	}
							else									{	locate = 600;	}
							rate = INT64(CHAR_LOCATE(CharStruct[i].loc1)-locate)*cnt/max;
							SetCharPos( i, locate+rate );
							break;
						case CHAR_TYPE_CFADE:
						case CHAR_TYPE_WAVE:
							if(CharStruct[i].alph1!=256){
								rate = CharStruct[i].alph1*cnt/max;
								param = DRW_BLD(rate);
							}else{
								rate = 256*cnt/max;
								if(sp){
									param = DRW_BLD(rate);
								}else{
									if(ami)	param = DRW_AMI(rate);
									else	param = DRW_LPP(UP,rate);
								}
							}
							break;
						}
					}
					break;
				case CHAR_COND_OUT:		
					if(max){
						switch(CharStruct[i].type){
						case CHAR_TYPE_MOVE:
						case CHAR_TYPE_MOVE+1:
							cnt = max-cnt;
							cnt = cnt*cnt*cnt;
							max = max*max*max;
							if(CharStruct[i].type==CHAR_TYPE_MOVE){	locate = -600;	}
							else								{	locate = 600;	}
							rate = INT64(CHAR_LOCATE(CharStruct[i].loc1)-locate)*cnt/max;
							SetCharPos( i, locate+rate );
							break;
						case CHAR_TYPE_CFADE:
						case CHAR_TYPE_WAVE:
							if(CharStruct[i].alph1!=256){
								rate = CharStruct[i].alph1-CharStruct[i].alph1*cnt/max;
								param = DRW_BLD(rate);
							}else{
								rate = 256-256*cnt/max;
								if(sp){
									param = DRW_BLD(rate);
								}else{
									if(ami)	param = DRW_AMI(rate);
									else	param = DRW_LPP(UP,rate);
								}
							}
							break;
						}
					}
					break;
				case CHAR_COND_POSE:	
					if(max){
						if(CharStruct[i].alph1!=256){
							rate = 256*cnt/max;
							DSP_SetGraphParam2( GRP_CHAR+i, DRW_BLD(rate) );
						}else{
							rate = 256*cnt/max;

							if(sp)	DSP_SetGraphParam2( GRP_CHAR+i, DRW_BLD(rate) );
							else{
								if(ami)	DSP_SetGraphParam2( GRP_CHAR+i, DRW_AMI(rate) );
								else	DSP_SetGraphParam2( GRP_CHAR+i, DRW_LPP(UP,rate) );
							}
						}
					}
					break;
				case CHAR_COND_LOCATE:	
					if(max){
						cnt = max-cnt;
						cnt = cnt*cnt*cnt;
						max = max*max*max;
						cnt = max-cnt;
						rate = INT64(CHAR_LOCATE(CharStruct[i].loc1)-CHAR_LOCATE(CharStruct[i].loc2))*cnt/max;
						SetCharPos( i, CHAR_LOCATE(CharStruct[i].loc2)+rate );
					}else{
						SetCharPos( i, CHAR_LOCATE(CharStruct[i].loc1) );
					}
					break;
				case CHAR_COND_BRIGHT:	
					if(max)	{	rate = (CharStruct[i].fade2*(max-cnt)+CharStruct[i].fade1*cnt)/max;	}
					else	{	rate =  CharStruct[i].fade1;										}
					bright = rate;
					break;
				case CHAR_COND_ALPHA:	
					if(max)	{	rate = (CharStruct[i].alph2*(max-cnt)+CharStruct[i].alph1*cnt)/max;	}
					else	{	rate =  CharStruct[i].alph1;										}
					param = DRW_BLD(rate);
					break;
			}
			DSP_SetGraphParam( GRP_CHAR+i, param );
			DSP_SetGraphFade( GRP_CHAR+i, bright );
			if(CharStruct[i].type==CHAR_TYPE_WAVE && CharStruct[i].cond!=CHAR_COND_NOMAL){
				DSP_SetGraphTarget( GRP_CHAR+i, BMP_BACK, MainWindow.draw_mode2 );
			}
		
			if(CharStruct[i].cond!=CHAR_COND_NOMAL && CharStruct[i].cond!=CHAR_COND_WAIT){	
				
				static int win_flag=0;
				if( AVG_GetWindowCond() ){
					win_flag = 1;
					AVG_CloseWindow(OFF);


				}
				if( CharStruct[i].cnt >= max2 ){
					
					if(win_flag){
						win_flag = 0;
						AVG_OpenWindow(OFF,OFF);
					}

					if(CharStruct[i].cond==CHAR_COND_OUT){	
						AVG_ReleaseChar( i );

						if(ChipBackStruct.flag){
							AVG_PlayChipBack();
							DSP_ResetGraph( GRP_BACK2 );
							DSP_ReleaseBmp( BMP_BACK2 );
						}
						break;
					}
					if(CharStruct[i].type==CHAR_TYPE_WAVE){
						DSP_SetGraphDisp( GRP_CHAR+i, OFF );
					}
					if(CharStruct[i].cond==CHAR_COND_POSE){	
						DSP_ResetGraphBSet( GRP_CHAR+i );
						DSP_ReleaseBmp( BMP_CHAR+i*2+!CharStruct[i].grp );
					}
					CharStruct[i].cond = CHAR_COND_NOMAL;
				}
				CharStruct[i].cnt++;
			}
		}
	}
}

BOOL AVG_ConfigCheckChar(void)
{
	int		i;
	BOOL	char_flag=FALSE;

	for( i=0; i<MAX_CHAR ;i++ ){
		char_flag |= CharStruct[i].flag && CharStruct[i].cond!=CHAR_COND_NOMAL;
	}
	return !char_flag;
}

void AVG_SetSaveDataChar( AVG_SAVE_DATA	*sdata )
{
	int		i;

	for( i=0; i<MAX_CHAR ;i++ ){
		sdata->cs_flag[i]  = CharStruct[i].flag;	
		sdata->cs_cno[i]   = CharStruct[i].cno;		
		sdata->cs_pose[i]  = CharStruct[i].pose;	
		sdata->cs_loc1[i]  = CharStruct[i].loc1;	
		sdata->cs_layer[i] = CharStruct[i].layer;	
		sdata->cs_fade[i]  = CharStruct[i].fade1;	
		sdata->cs_alph[i]  = CharStruct[i].alph1;	
	}
}

void AVG_InitChar(void)
{
	ZeroMemory( CharStruct, sizeof(CHAR_STRUCT)*MAX_CHAR );
}
