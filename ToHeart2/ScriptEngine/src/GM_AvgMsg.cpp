
 
  
   
    
     
      

#include "mm_std.h"
#include "main.h"
#include <mbstring.h>

#include "bmp.h"
#include "draw.h"
#include "disp.h"
#include "text.h"
#include "keybord.h"
#include "mouse.h"

#include "escript.h"

#include "main.h"
#include "GM_avg.h"
#include "GM_Save.h"
#include "GM_title.h"


/*

#nl(1-6)	苗字
#nlk(1-6)	みょうじ
#nf(1-6) 	名前
#nfk(1-6)	なまえ
#nn		ニックネーム

*/
#define NAME_L		"*nl"	
#define NAME_LK		"*nlk"	
#define NAME_F		"*nf"	
#define NAME_FK		"*nfk"	
#define NAME_NN		"*nn"	
#define NAME_NNK	"*nnk"	


char	NameL[13]  = DEF_NAME_L;
char	NameLK[13] = DEF_NAME_LK;
char	NameF[13]  = DEF_NAME_F;
char	NameFK[13] = DEF_NAME_FK;
char	NameNN[13] = DEF_NAME_NN;
char	NameNNK[13] = DEF_NAME_NNK;


void ReadRegistryName( void )
{
	ReadRegistryStr( "NameL", NameL );
	ReadRegistryStr( "NameF", NameF );
	ReadRegistryStr( "NameLK", NameLK );
	ReadRegistryStr( "NameFK", NameFK );
	ReadRegistryStr( "NameNN", NameNN );

	ReadRegistryAmount( "DefaultName", &DefaultCharName );
	ESC_SetFlag( _DEFAULT_NAME, DefaultCharName );
}

void WriteRegistryName( void )
{
	WriteRegistryStr( "NameL", NameL );
	WriteRegistryStr( "NameF", NameF );
	WriteRegistryStr( "NameLK", NameLK );
	WriteRegistryStr( "NameFK", NameFK );
	WriteRegistryStr( "NameNN", NameNN );

	WriteRegistryAmount( "DefaultName", DefaultCharName );
}





NOVEL_MESSEGE	NovelMessage;
NOVEL_BUF	NovelBuf;
HALF_TONE	HalfTone;

void InitNovelMessage(void)
{
	ZeroMemory( &NovelMessage, sizeof(NOVEL_MESSEGE) );
	ZeroMemory( &NovelBuf, sizeof(NOVEL_BUF) );
	ZeroMemory( &HalfTone, sizeof(HALF_TONE) );
}

BOOL AVG_GetMesCut(void)
{
	if(Avg.msg_cut_optin&1){
		return Avg.msg_cut || Avg.msg_cut_mode;
	}else{	
		return (Avg.msg_cut || Avg.msg_cut_mode) && AVG_CheckScenarioFlag();
	}
}


int AVG_GetHalfToneFade(void)
{
	return Avg.half_tone;
}

void AVG_SetHalfTone(void)
{
	switch( HalfTone.tstep){
		default:
		case TONE_NODISP:
			HalfTone.tstep  = TONE_FADEOUT;
			HalfTone.tcount = 0;
			DSP_CopyBmp2( BMP_BACKHALF, BMP_BACK, NULL, 256, BackStruct.r*Avg.half_tone/128, BackStruct.g*Avg.half_tone/128, BackStruct.b*Avg.half_tone/128  );
			DSP_SetGraph( GRP_BACK+1, BMP_BACKHALF, LAY_BACK+2, OFF, CHK_NO );
			DSP_SetGraphPos( GRP_BACK+1, 0,   0, BackStruct.x, BackStruct.y, DISP_X, DISP_Y );



			DSP_SetGraphDisp( GRP_BACK, ON );
			if(BackStruct.sp_flag==2){
				DSP_SetGraphDisp( GRP_SPBACK, ON );
				DSP_SetGraphDisp( GRP_SPBACKHALF, OFF );
			}
			break;
		case TONE_FADEOUT:
		case TONE_DISP:

			HalfTone.tstep  = TONE_DISP;
			DSP_SetGraph( GRP_BACK+1, BMP_BACKHALF, LAY_BACK+2, ON, CHK_NO );
			DSP_SetGraphPos( GRP_BACK+1, 0,   0, BackStruct.x, BackStruct.y, DISP_X, DISP_Y );



			DSP_SetGraphDisp( GRP_BACK, OFF );
			if(BackStruct.sp_flag==2){
				DSP_SetGraphDisp( GRP_SPBACK, OFF );
				DSP_SetGraphDisp( GRP_SPBACKHALF, ON );
			}
			break;
	}
}

void AVG_SetHalfToneDirect(void)
{

	MainWindow.draw_flag=1;

	HalfTone.tstep = TONE_DISP;
	HalfTone.tcount = 16;
	

	SetCharHalfTone( ON );
	
	DSP_CopyBmp2( BMP_BACKHALF, BMP_BACK, NULL, 256, BackStruct.r*Avg.half_tone/128, BackStruct.g*Avg.half_tone/128, BackStruct.b*Avg.half_tone/128  );
	DSP_SetGraph( GRP_BACK+1, BMP_BACKHALF, LAY_BACK+2, ON, CHK_NO );
	DSP_SetGraphPos( GRP_BACK+1, 0,   0, BackStruct.x, BackStruct.y, DISP_X, DISP_Y );
	DSP_SetGraphDisp( GRP_BACK, OFF );
	if(BackStruct.sp_flag==2){
		DSP_SetGraphDisp( GRP_SPBACK, OFF );
		DSP_SetGraphDisp( GRP_SPBACKHALF, ON );
	}
}


void AVG_ResetHalfTone(void)
{
	HalfTone.tstep  = TONE_NODISP;
	HalfTone.tcount = 0;
	DSP_ReleaseBmp(BMP_BACKHALF);
	DSP_ResetGraph( GRP_BACK+1 );

	DSP_SetGraphDisp( GRP_BACK, ON );
	DSP_SetGraphBright( GRP_BACK, BackStruct.r, BackStruct.g, BackStruct.b );
	if(BackStruct.sp_flag==2){
		DSP_SetGraphBright( GRP_SPBACK, BackStruct.r, BackStruct.g, BackStruct.b );
		SetCharBright( BackStruct.r, BackStruct.g, BackStruct.b );
		DSP_SetGraphDisp( GRP_SPBACK, ON );
		DSP_SetGraphDisp( GRP_SPBACKHALF, OFF );
	}
}
void AVG_SetHalfToneStep( int setp )
{
	HalfTone.tstep=setp;
}
int AVG_CheckHalfToneStep( void )
{
	return HalfTone.tstep;
}

void AVG_ControlHalfTone(void)
{
	int	r,g,b;
	switch(HalfTone.tstep){
		default:
		case TONE_NODISP:
			HalfTone.tstep  = TONE_NODISP;
			break;
		case TONE_DISP:
			HalfTone.tstep  = TONE_DISP;
			break;
		case TONE_FADEIN:
/*			HalfTone.tcount-=AVG_EffCntPuls();
			DSP_SetGraphDisp( GRP_BACK, ON );
			DSP_SetGraphDisp( GRP_SPBACK, ON );
			if(HalfTone.tcount<=0){
				HalfTone.tstep = TONE_NODISP;
				DSP_SetGraphParam( GRP_BACK+1, DRW_BLD(0) );
				DSP_SetGraphParam( GRP_SPBACKHALF, DRW_BLD(0) );
			}else{
				DSP_SetGraphParam( GRP_BACK+1, DRW_BLD(HalfTone.tcount*16) );
				DSP_SetGraphParam( GRP_SPBACKHALF, DRW_BLD(HalfTone.tcount*16) );
			}
*/
			break;
		case TONE_FADEOUT:
			HalfTone.tcount+=AVG_EffCntPuls();
			if(HalfTone.tcount>=16 || AVG_GetWavEffectFlag() ){
				HalfTone.tstep  = TONE_DISP;
				HalfTone.tcount = 16;

				DSP_SetGraphDisp( GRP_BACK+1, ON );
				DSP_SetGraphDisp( GRP_SPBACKHALF, ON );

				DSP_SetGraphDisp( GRP_BACK, OFF );
				DSP_SetGraphDisp( GRP_SPBACK, OFF );
			}else{
				r = (HalfTone.tcount*BackStruct.r*Avg.half_tone/128 + (16-HalfTone.tcount)*BackStruct.r)/16;
				g = (HalfTone.tcount*BackStruct.g*Avg.half_tone/128 + (16-HalfTone.tcount)*BackStruct.g)/16;
				b = (HalfTone.tcount*BackStruct.b*Avg.half_tone/128 + (16-HalfTone.tcount)*BackStruct.b)/16;
				DSP_SetGraphBright( GRP_BACK,   r, g, b );
				DSP_SetGraphBright( GRP_SPBACK, r, g, b );
				



				


			}
			break;
	}
}


int	AVG_GetNovelMessageFlag(void)
{
	return NovelMessage.flag;
}

int	AVG_GetNovelMessageConfig(void)
{
	return NovelMessage.step1==MSG_WAIT || NovelMessage.step1==MSG_STOP || (NovelMessage.step1==MSG_NEXT && AVG_GetSelectMessageFlag());
}



void SetNovelMessageHistory( char *str )
{
	int	i;
	ZeroMemory( &NovelBuf.nv[NovelBuf.bpoint], sizeof(NOVEL_VOICE) );

	strcpy( NovelBuf.buf[NovelBuf.bpoint], str );
	NovelBuf.bflag[NovelBuf.bpoint] = ON;
	NovelBuf.bpoint = (NovelBuf.bpoint+1)%NLOG_MAX;
	NovelBuf.bmax   = min(NLOG_MAX,NovelBuf.bmax+1);
}

void SetNovelMessageVoice1( int sno, int vno, int cno, int a_cut )
{
	NovelBuf.sno = sno;
	NovelBuf.vno = vno;
	NovelBuf.cno = cno;
	NovelBuf.a_cut = a_cut;
}
void SetNovelMessageVoice2( int top )
{
	int	px,py;
	int	cnt = (NovelBuf.bpoint-1+NLOG_MAX)%NLOG_MAX;

	if(NovelBuf.sno){
		NovelBuf.nv[cnt].sno[NovelBuf.nv[cnt].vcount] = NovelBuf.sno;
		NovelBuf.nv[cnt].vno[NovelBuf.nv[cnt].vcount] = NovelBuf.vno;
		NovelBuf.nv[cnt].cno[NovelBuf.nv[cnt].vcount] = NovelBuf.cno;
		NovelBuf.nv[cnt].a_cut[NovelBuf.nv[cnt].vcount] = NovelBuf.a_cut;

		NovelBuf.sno=0;
		NovelBuf.vno=0;
		NovelBuf.cno=0;
	}else{
		NovelBuf.sno=0;
		NovelBuf.vno=0;
		NovelBuf.cno=0;
		return ;
	}
	if(top){
		NovelBuf.nv[cnt].px[NovelBuf.nv[cnt].vcount] = MES_POS_X;
		NovelBuf.nv[cnt].py[NovelBuf.nv[cnt].vcount] = MES_POS_Y;
		NovelBuf.nv[cnt].vstcount[NovelBuf.nv[cnt].vcount] = 0;
	}else{
		TXT_DrawText( NULL, 0, MES_POS_X, MES_POS_Y, MES_POS_W, MES_POS_H, MES_PICH_W, MES_PICH_H, &px, &py, NULL, SYS_FONT, NovelBuf.buf[cnt], 0, -1, -1, 128,128,128, -1, 0, TRUE );
		NovelBuf.nv[cnt].px[NovelBuf.nv[cnt].vcount] = px;
		NovelBuf.nv[cnt].py[NovelBuf.nv[cnt].vcount] = py;

		NovelBuf.nv[cnt].vstcount[NovelBuf.nv[cnt].vcount] = strlen( NovelBuf.buf[cnt] );
	}

	NovelBuf.nv[cnt].vcount++;
}


void ResetHistoryVoiceMouseRect( void )
{
	int	i;
	for( i=0 ; i< NLOG_V_MAX; i++ ){
		MUS_ResetMouseRect( 0, 32+i );
	}
}
void SetHistoryVoiceMouseRect( void )
{
	int	i,j,k,m, n;
	int	cnt = (NovelBuf.bpoint-1-NovelBuf.bcount+NLOG_MAX)%NLOG_MAX;

	m=0;
	for( i=0 ; i<NovelBuf.nv[cnt].vcount ; i++ ){
		j=0;
		n=0;
		while(NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ]){
			if( _mbsncmp( (BYTE*)&NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ], (BYTE*)"」", 2 )==0 ){
				break;
			}
			if( _mbsncmp( (BYTE*)&NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ], (BYTE*)"\\k", 2 )==0 ){
				n+=2;
				if(j!=0)	break;
			}
			if( _mbsncmp( (BYTE*)&NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ], (BYTE*)"\\n", 2 )==0 ){
				n+=2;
				if(j!=0)	break;
			}
			j++;
		}
		int	vwork = NovelBuf.nv[cnt].px[i]+17*(j-n);
		int	vpx   = NovelBuf.nv[cnt].px[i];
		int	vpy   = NovelBuf.nv[cnt].py[i];

		MUS_SetMouseRectAdd( 0, 32+m++, 32+i, vpx, vpy-9, 17*j,34+18, ON );
		vwork -= 34*20;
		while( vwork>0 ){
			vpx=MES_POS_X;
			vpy+=MES_PICH_H+34;
			MUS_SetMouseRectAdd( 0, 32+m++, 32+i, vpx, vpy-9, vwork, 34+18, ON );
			vwork -= 34*20;
		}

		for( k=0; k<(NovelBuf.nv[cnt].px[i]-MES_POS_X)/17; k++ ){
			NovelBuf.vv_mes[i][k] = ' ';
		}
		j=0;
		while(NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ]){
			if( _mbsncmp( (BYTE*)&NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ], (BYTE*)"」", 2 )==0 ){
				NovelBuf.vv_mes[i][j+k] = NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ];
				j++;
				NovelBuf.vv_mes[i][j+k] = NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ];
				j++;
				break;
			}
			if( _mbsncmp( (BYTE*)&NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ], (BYTE*)"\\k", 2 )==0 ){
				if(j!=0)	break;
			}
			NovelBuf.vv_mes[i][j+k] = NovelBuf.buf[cnt][ NovelBuf.nv[cnt].vstcount[i]+j ];
			j++;
		}
		NovelBuf.vv_mes[i][j+k] = '\0';
	}
	for( ; i< NLOG_V_MAX; i++ ){
		NovelBuf.vv_mes[i][0] = '\0';
	}
	for( ; m< NLOG_V_MAX; m++ ){
		MUS_ResetMouseRect( 0, 32+m );
	}
}


static int	HistorySystemRectX[10]={776, 776, 776,  776,      776,      776,      776,      776,      776, 776 };
static int	HistorySystemRectY[10]={ 10, 271, 312,  353, 353+23*1, 353+23*2, 353+23*3, 353+23*4, 353+23*5, 492 };
static int	HistorySystemRectW[10]={ 22,  22,  22,   22,       22,       22,       22,       22,       22,  22 };
static int	HistorySystemRectH[10]={255,  36,  36,   20,       20,       20,       20,       20,       20,  98 };

static int	HistorySystemSrcX[10]={   0,   0,   0,    0,      0,        0,        0,        0,        0,    0 };
static int	HistorySystemSrcY[10]={   0,  36,  77,  118, 118+23, 118+23*2, 118+23*3, 118+23*4, 118+23*5,  257 };
static int	HistorySystemSrcW[10]={  22,  22,  22,   22,     22,       22,       22,       22,       22,   22 };
static int	HistorySystemSrcH[10]={  30,  36,  36,   20,     20,       20,       20,       20,       20,    6 };


void SetHistorySystemMouseRect( void )
{
	if(Avg.side_option!=3){
		MUS_SetMouseRect( 0, 0, HistorySystemRectX[0], HistorySystemRectY[0], HistorySystemRectW[0], HistorySystemRectH[0], 1 );
		MUS_SetMouseRect( 0, 1, HistorySystemRectX[1], HistorySystemRectY[1], HistorySystemRectW[1], HistorySystemRectH[1], 1 );
		MUS_SetMouseRect( 0, 2, HistorySystemRectX[2], HistorySystemRectY[2], HistorySystemRectW[2], HistorySystemRectH[2], 1 );

		
		if(Avg.omake){
			MUS_SetMouseRect( 0, 3, HistorySystemRectX[3], HistorySystemRectY[3], HistorySystemRectW[3], HistorySystemRectH[3], OFF );
			MUS_SetMouseRect( 0, 4, HistorySystemRectX[4], HistorySystemRectY[4], HistorySystemRectW[4], HistorySystemRectH[4], OFF );
		}else{
			MUS_SetMouseRect( 0, 3, HistorySystemRectX[3], HistorySystemRectY[3], HistorySystemRectW[3], HistorySystemRectH[3], ON );
			MUS_SetMouseRect( 0, 4, HistorySystemRectX[4], HistorySystemRectY[4], HistorySystemRectW[4], HistorySystemRectH[4], ON );
		}

		MUS_SetMouseRect( 0, 5, HistorySystemRectX[5], HistorySystemRectY[5], HistorySystemRectW[5], HistorySystemRectH[5], 1 );
		MUS_SetMouseRect( 0, 6, HistorySystemRectX[6], HistorySystemRectY[6], HistorySystemRectW[6], HistorySystemRectH[6], 1 );
		MUS_SetMouseRect( 0, 7, HistorySystemRectX[7], HistorySystemRectY[7], HistorySystemRectW[7], HistorySystemRectH[7], 1 );
		MUS_SetMouseRect( 0, 8, HistorySystemRectX[8], HistorySystemRectY[8], HistorySystemRectW[8], HistorySystemRectH[8], 1 );

		MUS_SetMouseRect( 0, 9, HistorySystemRectX[9], HistorySystemRectY[9], HistorySystemRectW[9], HistorySystemRectH[9], 1 );
	}
}

void ResetHistorySystemMouseRect( void )
{
	MUS_ResetMouseRect( 0, 0 );
	MUS_ResetMouseRect( 0, 1 );
	MUS_ResetMouseRect( 0, 2 );

	MUS_ResetMouseRect( 0, 3 );
	MUS_ResetMouseRect( 0, 4 );
	MUS_ResetMouseRect( 0, 5 );
	MUS_ResetMouseRect( 0, 6 );
	MUS_ResetMouseRect( 0, 7 );
	MUS_ResetMouseRect( 0, 8 );

	MUS_ResetMouseRect( 0, 9 );
}



int	MES_POS_X  = (96)/2;
int	MES_POS_Y  = 100/2;
int	MES_POS_W  = 20;
int	MES_POS_H  = 10;
int	MES_PICH_W = 0;	
int	MES_PICH_H = 18;
int	MES_KAGE   = 2;

void AVG_SetNovelParam( int fsize, int posx, int posy, int posw, int posh, int pichw, int pichh, int kage )
{

}

void AVG_SetNovelMessageDisp( int disp )
{
	NovelMessage.disp  = disp;
	DSP_SetTextDisp( TXT_WINDOW, disp );

	if(disp){
		SetHistorySystemMouseRect();
	}else{
		DSP_ResetGraph( GRP_KEYWAIT );
		ResetHistorySystemMouseRect();
	}
}

void AVG_SetName( char *dst, char *src )
{
	int		i;

	i=0;
	while( *src ){
		if( !_mbsncmp( (BYTE*)"*h2", (BYTE*)src, strlen("*h2") ) ){
			if( ESC_GetFlag(213) ){
				_mbscpy( (BYTE*)&dst[i], (BYTE*)"愛佳" );
				i += strlen("愛佳");
				src+=strlen("*h2");
			}else{
				_mbscpy( (BYTE*)&dst[i], (BYTE*)"小牧" );
				i += strlen("小牧");
				src+=strlen("*h2");
			}
		}
		if( !_mbsncmp( (BYTE*)NAME_NNK, (BYTE*)src, strlen(NAME_NNK) ) ){

			if(DefaultCharName){
				switch(src[strlen(NAME_NNK)]){
					case '1':	if(NameNNK[0]) {dst[i++] = NameNNK[0];	dst[i++] = NameNNK[1];}	src++;	break;
					case '2':	if(NameNNK[2]) {dst[i++] = NameNNK[2];	dst[i++] = NameNNK[3];}	src++;	break;
					case '3':	if(NameNNK[4]) {dst[i++] = NameNNK[4];	dst[i++] = NameNNK[5];}	src++;	break;
					case '4':	if(NameNNK[6]) {dst[i++] = NameNNK[6];	dst[i++] = NameNNK[7];}	src++;	break;
					case '5':	if(NameNNK[8]) {dst[i++] = NameNNK[8];	dst[i++] = NameNNK[9];}	src++;	break;
					case '6':	if(NameNNK[10]){dst[i++] = NameNNK[10];	dst[i++] = NameNNK[11];}src++;	break;
					default:
						_mbscpy( (BYTE*)&dst[i], (BYTE*)NameNNK );
						i += strlen(NameNNK);
						break;
				}
			}else{
				switch(src[strlen(NAME_NNK)]){
					case '1':	if(NameNN[0]) {dst[i++] = NameNN[0];	dst[i++] = NameNN[1];}	src++;	break;
					case '2':	if(NameNN[2]) {dst[i++] = NameNN[2];	dst[i++] = NameNN[3];}	src++;	break;
					case '3':	if(NameNN[4]) {dst[i++] = NameNN[4];	dst[i++] = NameNN[5];}	src++;	break;
					case '4':	if(NameNN[6]) {dst[i++] = NameNN[6];	dst[i++] = NameNN[7];}	src++;	break;
					case '5':	if(NameNN[8]) {dst[i++] = NameNN[8];	dst[i++] = NameNN[9];}	src++;	break;
					case '6':	if(NameNN[10]){dst[i++] = NameNN[10];	dst[i++] = NameNN[11];}	src++;	break;
					default:
						_mbscpy( (BYTE*)&dst[i], (BYTE*)NameNN );
						i += strlen(NameNN);
						break;
				}
			}
			src+=strlen(NAME_NNK);
		}else if( !_mbsncmp( (BYTE*)NAME_NN, (BYTE*)src, strlen(NAME_NN) ) ){
			switch(src[strlen(NAME_NN)]){
				case '1':	if(NameNN[0]) {dst[i++] = NameNN[0];	dst[i++] = NameNN[1];}	src++;	break;
				case '2':	if(NameNN[2]) {dst[i++] = NameNN[2];	dst[i++] = NameNN[3];}	src++;	break;
				case '3':	if(NameNN[4]) {dst[i++] = NameNN[4];	dst[i++] = NameNN[5];}	src++;	break;
				case '4':	if(NameNN[6]) {dst[i++] = NameNN[6];	dst[i++] = NameNN[7];}	src++;	break;
				case '5':	if(NameNN[8]) {dst[i++] = NameNN[8];	dst[i++] = NameNN[9];}	src++;	break;
				case '6':	if(NameNN[10]){dst[i++] = NameNN[10];	dst[i++] = NameNN[11];}	src++;	break;
				default:
					_mbscpy( (BYTE*)&dst[i], (BYTE*)NameNN );
					i += strlen(NameNN);
					break;
			}
			src+=strlen(NAME_NN);
		}else if( !_mbsncmp( (BYTE*)NAME_LK, (BYTE*)src, strlen(NAME_LK) ) ){
			switch(src[strlen(NAME_LK)]){
				case '1':	if(NameLK[0]) {dst[i++] = NameLK[0];	dst[i++] = NameLK[1];}	src++;	break;
				case '2':	if(NameLK[2]) {dst[i++] = NameLK[2];	dst[i++] = NameLK[3];}	src++;	break;
				case '3':	if(NameLK[4]) {dst[i++] = NameLK[4];	dst[i++] = NameLK[5];}	src++;	break;
				case '4':	if(NameLK[6]) {dst[i++] = NameLK[6];	dst[i++] = NameLK[7];}	src++;	break;
				case '5':	if(NameLK[8]) {dst[i++] = NameLK[8];	dst[i++] = NameLK[9];}	src++;	break;
				case '6':	if(NameLK[10]){dst[i++] = NameLK[10];	dst[i++] = NameLK[11];}	src++;	break;
				default:
					_mbscpy( (BYTE*)&dst[i], (BYTE*)NameLK );
					i += strlen(NameLK);
					break;
			}
			src+=strlen(NAME_LK);
		}else if( !_mbsncmp( (BYTE*)NAME_FK, (BYTE*)src, strlen(NAME_FK) ) ){
			switch(src[strlen(NAME_FK)]){
				case '1':	if(NameFK[0]) {dst[i++] = NameFK[0];	dst[i++] = NameFK[1];}	src++;	break;
				case '2':	if(NameFK[2]) {dst[i++] = NameFK[2];	dst[i++] = NameFK[3];}	src++;	break;
				case '3':	if(NameFK[4]) {dst[i++] = NameFK[4];	dst[i++] = NameFK[5];}	src++;	break;
				case '4':	if(NameFK[6]) {dst[i++] = NameFK[6];	dst[i++] = NameFK[7];}	src++;	break;
				case '5':	if(NameFK[8]) {dst[i++] = NameFK[8];	dst[i++] = NameFK[9];}	src++;	break;
				case '6':	if(NameFK[10]){dst[i++] = NameFK[10];	dst[i++] = NameFK[11];}	src++;	break;
				default:
					_mbscpy( (BYTE*)&dst[i], (BYTE*)NameFK );
					i += strlen(NameFK);
					break;
			}
			src+=strlen(NAME_FK);
		}else if( !_mbsncmp( (BYTE*)NAME_L, (BYTE*)src, strlen(NAME_L) ) ){
			switch(src[strlen(NAME_L)]){
				case '1':	if(NameL[0]) {dst[i++] = NameL[0];	dst[i++] = NameL[1];}	src++;	break;
				case '2':	if(NameL[2]) {dst[i++] = NameL[2];	dst[i++] = NameL[3];}	src++;	break;
				case '3':	if(NameL[4]) {dst[i++] = NameL[4];	dst[i++] = NameL[5];}	src++;	break;
				case '4':	if(NameL[6]) {dst[i++] = NameL[6];	dst[i++] = NameL[7];}	src++;	break;
				case '5':	if(NameL[8]) {dst[i++] = NameL[8];	dst[i++] = NameL[9];}	src++;	break;
				case '6':	if(NameL[10]){dst[i++] = NameL[10];	dst[i++] = NameL[11];}	src++;	break;
				default:
					_mbscpy( (BYTE*)&dst[i], (BYTE*)NameL );
					i += strlen(NameL);
					break;
			}
			src+=strlen(NAME_L);
		}else if( !_mbsncmp( (BYTE*)NAME_F, (BYTE*)src, strlen(NAME_F) ) ){
			switch(src[strlen(NAME_F)]){
				case '1':	if(NameF[0]) {dst[i++] = NameF[0];	dst[i++] = NameF[1];}	src++;	break;
				case '2':	if(NameF[2]) {dst[i++] = NameF[2];	dst[i++] = NameF[3];}	src++;	break;
				case '3':	if(NameF[4]) {dst[i++] = NameF[4];	dst[i++] = NameF[5];}	src++;	break;
				case '4':	if(NameF[6]) {dst[i++] = NameF[6];	dst[i++] = NameF[7];}	src++;	break;
				case '5':	if(NameF[8]) {dst[i++] = NameF[8];	dst[i++] = NameF[9];}	src++;	break;
				case '6':	if(NameF[10]){dst[i++] = NameF[10];	dst[i++] = NameF[11];}	src++;	break;
				default:
					_mbscpy( (BYTE*)&dst[i], (BYTE*)NameF );
					i += strlen(NameF);
					break;
			}
			src+=strlen(NAME_F);
		}else{
			dst[i++] = *src;
			src++;
		}
	}
	dst[i] = '\0';
}
void AVG_SetNovelMessage( char *str, int add_flag )
{
	char	buf[1024];
	AVG_SetName( buf, str );
	
	MainWindow.draw_flag=ON;
	ZeroMemory( &NovelMessage, sizeof(NOVEL_MESSEGE) );

	NovelMessage.flag   = ON;
	NovelMessage.add_flag   = add_flag;
	NovelMessage.disp   = ON;
	NovelMessage.step1  = MSG_DISP;
	NovelMessage.max    = TXT_GetTextCount( buf, NovelMessage.kstep )+8;


	DSP_SetText( TXT_WINDOW, LAY_WINDOW+1, SYS_FONT|0x100, ON, buf );
	DSP_SetTextPos( TXT_WINDOW, MES_POS_X, MES_POS_Y, MES_POS_W, MES_POS_H );
	DSP_SetTextPich( TXT_WINDOW, MES_PICH_W, MES_PICH_H );
	DSP_SetTextKage( TXT_WINDOW, MES_KAGE );
	DSP_SetTextAlph( TXT_WINDOW, -1 );

	DSP_SetTextCount( TXT_WINDOW, NovelMessage.count );
	DSP_SetTextStep( TXT_WINDOW, NovelMessage.kstep );


	AVG_SetHalfTone();
	SetNovelMessageHistory(buf);
	AVG_SetNovelMessageDisp( ON );

	SetNovelMessageVoice2(1);
}
void AVG_AddNovelMessage( char *str, int cr )
{
	SetNovelMessageVoice2(0);

	char	buf1[1024];
	char	buf2[1024];
	int		sp;
	AVG_SetName( buf1, str );

	MainWindow.draw_flag=ON;

	NovelMessage.flag   = ON;
	NovelMessage.disp   = ON;
	NovelMessage.step1  = MSG_DISP;
	NovelMessage.add_flag = cr;

	sp = (NovelBuf.bpoint-1+NLOG_MAX)%NLOG_MAX;
	strcpy( NovelMessage.str, NovelBuf.buf[sp] );
	wsprintf( buf2, "\\k%s", buf1 );
	strcat(NovelBuf.buf[sp], buf2 );
	if( !DSP_AddText( TXT_WINDOW, buf2) ){
		AVG_SetNovelMessage( str, cr );
		return ;
	}

	NovelMessage.kstep++;
	NovelMessage.count = NovelMessage.max;
	NovelMessage.max = TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), NovelMessage.kstep ) + 8;
	DSP_SetTextStep( TXT_WINDOW, NovelMessage.kstep );
	DSP_SetTextCount( TXT_WINDOW, NovelMessage.count );
	DSP_SetTextDisp( TXT_WINDOW, ON );


	AVG_SetHalfTone();
	AVG_SetNovelMessageDisp( ON );
}

BOOL AVG_WaitNovelMessage( void )
{
	return NovelMessage.step1 == MSG_NEXT;
}

BOOL AVG_NovelLogStart( void )
{
	if( NovelMessage.step1==MSG_NEXT || NovelMessage.step1==MSG_WAIT || NovelMessage.step1==MSG_STOP ){
		NovelMessage.step2 = NovelMessage.step1;
		NovelMessage.step1 = MSG_UP;

		DSP_SetTextDisp( TXT_WINDOW, OFF );

		DSP_SetText( TXT_WINDOW+1, LAY_WINDOW+1, SYS_FONT, ON, NovelBuf.buf[(NovelBuf.bpoint-2+NLOG_MAX)%NLOG_MAX] );
		DSP_SetTextPos( TXT_WINDOW+1, MES_POS_X, MES_POS_Y, MES_POS_W, MES_POS_H );
		DSP_SetTextPich( TXT_WINDOW+1, MES_PICH_W, MES_PICH_H );
		DSP_SetTextKage( TXT_WINDOW+1, MES_KAGE );
		DSP_SetTextAlph( TXT_WINDOW+1, -1 );
		DSP_SetTextColor( TXT_WINDOW+1, 10 );
	}
	return TRUE;
}

void ControlHistorySystem( void )
{
	int		command = MUS_GetMouseNoEx(-1,0);
	int		DragBarY, i;
	int		click = MUS_GetMouseTrigger(MOUSE_LBUTTON);
	static int	fade=0;

	if( NovelMessage.disp && !Avg.demo ){
		if(NovelBuf.bmax>=2){
			DragBarY = HistorySystemRectY[0]+(NovelBuf.bmax-1-NovelBuf.bcount)*(HistorySystemRectH[0]-(HistorySystemSrcH[0]+1))/(NovelBuf.bmax-1);
		}else{
			DragBarY = HistorySystemRectY[0]+HistorySystemRectH[0]-(HistorySystemSrcH[0]+1);
		}

		DSP_SetGraph( GRP_HISTORY+0, BMP_HISTORY+0, LAY_WINDOW, ON, CHK_ANTI );
		DSP_SetGraphMove( GRP_HISTORY+0, DISP_X-30, 0 );

		DSP_SetGraph( GRP_HISTORY+1, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );
		DSP_SetGraph( GRP_HISTORY+2, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );
		DSP_SetGraph( GRP_HISTORY+3, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );

		DSP_SetGraph( GRP_HISTORY+4, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );
		DSP_SetGraph( GRP_HISTORY+5, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );

		DSP_SetGraph( GRP_HISTORY+6, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );
		DSP_SetGraph( GRP_HISTORY+7, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );
		DSP_SetGraph( GRP_HISTORY+8, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );
		DSP_SetGraph( GRP_HISTORY+9, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );

		DSP_SetGraph( GRP_HISTORY+10, BMP_HISTORY+1, LAY_WINDOW+1, ON, CHK_ANTI );

		
		if(NovelMessage.step1==MSG_DRAG){

			DSP_SetGraphPos( GRP_HISTORY+1, HistorySystemRectX[0], DragBarY, HistorySystemSrcX[0]+HistorySystemSrcW[0]*3, HistorySystemSrcY[0], HistorySystemRectW[0], HistorySystemSrcH[0] );
		}else{
			if(NovelBuf.bmax>1){
				DSP_SetGraphPos( GRP_HISTORY+1, HistorySystemRectX[0], DragBarY, HistorySystemSrcX[0]+HistorySystemSrcW[0]*1, HistorySystemSrcY[0], HistorySystemRectW[0], HistorySystemSrcH[0] );
			}else{
				DSP_SetGraphPos( GRP_HISTORY+1, HistorySystemRectX[0], DragBarY, HistorySystemSrcX[0]+HistorySystemSrcW[0]*0, HistorySystemSrcY[0], HistorySystemRectW[0], HistorySystemSrcH[0] );
			}
		}
		
		DSP_SetGraphPos( GRP_HISTORY+2, HistorySystemRectX[1], HistorySystemRectY[1], HistorySystemSrcX[1]+HistorySystemSrcW[1]*1, HistorySystemSrcY[1], HistorySystemRectW[1], HistorySystemRectH[1] );
		
		DSP_SetGraphPos( GRP_HISTORY+3, HistorySystemRectX[2], HistorySystemRectY[2], HistorySystemSrcX[2]+HistorySystemSrcW[2]*1, HistorySystemSrcY[2], HistorySystemRectW[2], HistorySystemRectH[2] );

		if( NovelMessage.step1==MSG_WAIT || NovelMessage.step1==MSG_STOP || NovelMessage.step1==MSG_DISP || NovelMessage.step1==MSG_NEXT ){
			
			if(Avg.omake){
				DSP_SetGraphPos( GRP_HISTORY+4, HistorySystemRectX[3], HistorySystemRectY[3], HistorySystemSrcX[3]+HistorySystemSrcW[3]*0,  HistorySystemSrcY[3], HistorySystemRectW[3], HistorySystemRectH[3] );
				DSP_SetGraphPos( GRP_HISTORY+5, HistorySystemRectX[4], HistorySystemRectY[4], HistorySystemSrcX[4]+HistorySystemSrcW[4]*0,  HistorySystemSrcY[4], HistorySystemRectW[4], HistorySystemRectH[4] );
			}else{
				DSP_SetGraphPos( GRP_HISTORY+4, HistorySystemRectX[3], HistorySystemRectY[3], HistorySystemSrcX[3]+HistorySystemSrcW[3]*1,  HistorySystemSrcY[3], HistorySystemRectW[3], HistorySystemRectH[3] );
				DSP_SetGraphPos( GRP_HISTORY+5, HistorySystemRectX[4], HistorySystemRectY[4], HistorySystemSrcX[4]+HistorySystemSrcW[4]*1,  HistorySystemSrcY[4], HistorySystemRectW[4], HistorySystemRectH[4] );
			}
			
			DSP_SetGraphPos( GRP_HISTORY+6, HistorySystemRectX[5], HistorySystemRectY[5], HistorySystemSrcX[5]+HistorySystemSrcW[5]*1,  HistorySystemSrcY[5], HistorySystemRectW[5], HistorySystemRectH[5] );
			
			DSP_SetGraphPos( GRP_HISTORY+7, HistorySystemRectX[6], HistorySystemRectY[6], HistorySystemSrcX[6]+HistorySystemSrcW[6]*1,  HistorySystemSrcY[6], HistorySystemRectW[6], HistorySystemRectH[6] );
			
			DSP_SetGraphPos( GRP_HISTORY+8, HistorySystemRectX[7], HistorySystemRectY[7], HistorySystemSrcX[7]+HistorySystemSrcW[7]*1,  HistorySystemSrcY[7], HistorySystemRectW[7], HistorySystemRectH[7] );
			
			DSP_SetGraphPos( GRP_HISTORY+9, HistorySystemRectX[8], HistorySystemRectY[8], HistorySystemSrcX[8]+HistorySystemSrcW[8]*1,  HistorySystemSrcY[8], HistorySystemRectW[8], HistorySystemRectH[8] );

			if(Avg.auto_flag){
				DSP_SetGraphSMove( GRP_HISTORY+6, HistorySystemSrcX[5]+HistorySystemSrcW[5]*3, HistorySystemSrcY[5] );
			}else{
				DSP_SetGraphSMove( GRP_HISTORY+6, HistorySystemSrcX[5]+HistorySystemSrcW[5]*1, HistorySystemSrcY[5] );
			}
			if( (Avg.msg_cut_optin&1) || AVG_CheckScenarioFlag() ){
				if(Avg.msg_cut_mode){
					DSP_SetGraphSMove( GRP_HISTORY+7, HistorySystemSrcX[6]+HistorySystemSrcW[6]*3, HistorySystemSrcY[6] );
				}else{
					DSP_SetGraphSMove( GRP_HISTORY+7, HistorySystemSrcX[6]+HistorySystemSrcW[6]*1, HistorySystemSrcY[6] );
				}
			}else{	
				DSP_SetGraphSMove( GRP_HISTORY+7, HistorySystemSrcX[6]+HistorySystemSrcW[6]*0, HistorySystemSrcY[6] );
			}
		}else{
			DSP_SetGraphPos( GRP_HISTORY+4, HistorySystemRectX[3], HistorySystemRectY[3], HistorySystemSrcX[3]+HistorySystemSrcW[3]*0,  HistorySystemSrcY[3], HistorySystemRectW[3], HistorySystemRectH[3] );
			DSP_SetGraphPos( GRP_HISTORY+5, HistorySystemRectX[4], HistorySystemRectY[4], HistorySystemSrcX[4]+HistorySystemSrcW[4]*0,  HistorySystemSrcY[4], HistorySystemRectW[4], HistorySystemRectH[4] );
			DSP_SetGraphPos( GRP_HISTORY+6, HistorySystemRectX[5], HistorySystemRectY[5], HistorySystemSrcX[5]+HistorySystemSrcW[5]*0,  HistorySystemSrcY[5], HistorySystemRectW[5], HistorySystemRectH[5] );
			DSP_SetGraphPos( GRP_HISTORY+7, HistorySystemRectX[6], HistorySystemRectY[6], HistorySystemSrcX[6]+HistorySystemSrcW[6]*0,  HistorySystemSrcY[6], HistorySystemRectW[6], HistorySystemRectH[6] );
			DSP_SetGraphPos( GRP_HISTORY+8, HistorySystemRectX[7], HistorySystemRectY[7], HistorySystemSrcX[7]+HistorySystemSrcW[7]*0,  HistorySystemSrcY[7], HistorySystemRectW[7], HistorySystemRectH[7] );
			DSP_SetGraphPos( GRP_HISTORY+9, HistorySystemRectX[8], HistorySystemRectY[8], HistorySystemSrcX[8]+HistorySystemSrcW[8]*0,  HistorySystemSrcY[8], HistorySystemRectW[8], HistorySystemRectH[8] );
		}

		DSP_SetGraphPos( GRP_HISTORY+10, HistorySystemRectX[9], HistorySystemRectY[9] + (128-Avg.half_tone), HistorySystemSrcX[9]+HistorySystemSrcW[9]*1, HistorySystemSrcY[9], HistorySystemSrcW[9], HistorySystemSrcH[9] );
		switch(command){
			case 0:
				if(NovelMessage.step1!=MSG_DRAG && NovelBuf.bmax>1){
					DSP_SetGraphPos( GRP_HISTORY+1, HistorySystemRectX[0], DragBarY, HistorySystemSrcX[0]+HistorySystemSrcW[0]*2, HistorySystemSrcY[0], HistorySystemRectW[0], HistorySystemSrcH[0] );
				}
				break;
			case 1:
				if( click )	DSP_SetGraphSMove( GRP_HISTORY+2, HistorySystemSrcX[1]+HistorySystemSrcW[1]*3, HistorySystemSrcY[1] );
				else		DSP_SetGraphSMove( GRP_HISTORY+2, HistorySystemSrcX[1]+HistorySystemSrcW[1]*2, HistorySystemSrcY[1] );
				break;
			case 2:
				if( click )	DSP_SetGraphSMove( GRP_HISTORY+3, HistorySystemSrcX[2]+HistorySystemSrcW[2]*3, HistorySystemSrcY[2] );
				else		DSP_SetGraphSMove( GRP_HISTORY+3, HistorySystemSrcX[2]+HistorySystemSrcW[2]*2, HistorySystemSrcY[2] );
				break;
			case 3:
			case 4:
				if(Avg.omake) break;
			case 7:
			case 8:
				if( NovelMessage.step1==MSG_WAIT || NovelMessage.step1==MSG_STOP || NovelMessage.step1==MSG_DISP || NovelMessage.step1==MSG_NEXT  ){
					if( click )	DSP_SetGraphSMove( GRP_HISTORY+command+1, HistorySystemSrcX[command]+HistorySystemSrcW[command]*3, HistorySystemSrcY[command] );
					else		DSP_SetGraphSMove( GRP_HISTORY+command+1, HistorySystemSrcX[command]+HistorySystemSrcW[command]*2, HistorySystemSrcY[command] );
				}
				break;
			case 5:
				if( NovelMessage.step1==MSG_WAIT || NovelMessage.step1==MSG_STOP || NovelMessage.step1==MSG_DISP || NovelMessage.step1==MSG_NEXT  ){
					if( click )	DSP_SetGraphSMove( GRP_HISTORY+6, HistorySystemSrcX[5]+HistorySystemSrcW[5]*3, HistorySystemSrcY[5] );
					else		DSP_SetGraphSMove( GRP_HISTORY+6, HistorySystemSrcX[5]+HistorySystemSrcW[5]*2, HistorySystemSrcY[5] );
				}
				break;
			case 6:
				if( (Avg.msg_cut_optin&1) || AVG_CheckScenarioFlag() ){
					if( NovelMessage.step1==MSG_WAIT || NovelMessage.step1==MSG_STOP || NovelMessage.step1==MSG_DISP || NovelMessage.step1==MSG_NEXT  ){
						if( click )	DSP_SetGraphSMove( GRP_HISTORY+7, HistorySystemSrcX[6]+HistorySystemSrcW[6]*3, HistorySystemSrcY[6] );
						else		DSP_SetGraphSMove( GRP_HISTORY+7, HistorySystemSrcX[6]+HistorySystemSrcW[6]*2, HistorySystemSrcY[6] );
					}
				}
				break;
			case 9:
				if( MUS_GetMouseButton(MOUSE_LBUTTON) ){
					DSP_SetGraphSMove( GRP_HISTORY+10, HistorySystemSrcX[9]+HistorySystemSrcW[9]*3, HistorySystemSrcY[9] );
				}else{
					DSP_SetGraphSMove( GRP_HISTORY+10, HistorySystemSrcX[9]+HistorySystemSrcW[9]*2, HistorySystemSrcY[9] );

				}
				break;
			default:
				break;
		}
		if(NovelBuf.bmax-1<=NovelBuf.bcount){
			DSP_SetGraphSMove( GRP_HISTORY+2, HistorySystemSrcX[1]+HistorySystemSrcW[1]*0, HistorySystemSrcY[1] );
		}
		if(0>=NovelBuf.bcount){
			DSP_SetGraphSMove( GRP_HISTORY+3, HistorySystemSrcX[2]+HistorySystemSrcW[2]*0, HistorySystemSrcY[2] );
		}

		switch(Avg.side_option){
			case 0:
				if( MUS_GetMousePosX()<DISP_X-24 && NovelMessage.step1!=MSG_DRAG ){
					fade = LIM(fade-24,64,256);
				}else{
					fade = LIM(fade+24,64,256);
				}
				break;
			case 1:
				fade = 256;
				break;
			case 2:
				if( MUS_GetMousePosX()<DISP_X-24 && NovelMessage.step1!=MSG_DRAG ){
					fade = LIM(fade-32,0,256);
				}else{
					fade = LIM(fade+32,0,256);
				}
				break;
			case 3:
				fade = 0;
				break;
		}
		for(i=0;i<11;i++){
			DSP_SetGraphParam( GRP_HISTORY+i, DRW_BLD(fade) );
		}
	}else{
		for(i=0;i<11;i++){
			DSP_ResetGraph( GRP_HISTORY+i );
		}
	}
}

int	AutoCount=0;

BOOL AVG_WaitAutoMode( void )
{
	int	wv = AVG_WaitVoice(0);

	int	wtime=60;

	if(NovelMessage.step1==MSG_WAIT || NovelMessage.add_flag!=2){
		wtime=Avg.auto_key;
	}else{
		wtime=Avg.auto_page;
	}

	if( Avg.auto_flag){
		if(!wv){
			AutoCount=wtime;
		}
		AutoCount++;
		if(AutoCount>wtime){

			AutoCount=0;
			return wv;
		}else{
			return FALSE;
		}
	}else{
		return FALSE;
	}
}
void AVG_ResetAutoMode( void )
{
	AutoCount=0;
}
void AVG_ControlNovelMessage( void )
{
	int		command_btrg = MUS_GetMouseNoEx(MOUSE_LBTRIGGER,0);
	int		command_trg  = MUS_GetMouseNoEx(MOUSE_LBUTTON,0);
	int		my;
	static int	key_wait_count=0;
	static int	key_wait_count2=0;
	
	if( AVG_CheckGotoTitle() ){
		EnableMenuItem( GetMenu( MainWindow.hwnd ), ID_TITLE, MF_ENABLED );
	}else{
		EnableMenuItem( GetMenu( MainWindow.hwnd ), ID_TITLE, MF_GRAYED );
	}
	switch(NovelMessage.step1){
		case MSG_NODISP:
			break;
		case MSG_NEXT:
			if( AVG_GetSelectMessageFlag() ){
				if( GameKey.diswin || command_trg==7 ){
					AVG_PlaySE3( 9104, 255 );
					NovelMessage.step2 = NovelMessage.step1;
					NovelMessage.step1 = MSG_SYSTEM;
	
					AVG_ResetHalfTone();
					AVG_SetNovelMessageDisp( OFF );
					AVG_CloseSelectWindow();
				}else if( GameKey.pup || command_btrg==1 ){
					AVG_PlaySE3( 9012, 140 );
					AVG_CloseSelectWindow();
					if(NovelBuf.bmax-1>0){
						DSP_ResetGraph( GRP_KEYWAIT );
						AVG_NovelLogStart();
					}
				}else if(command_btrg==0){
					AVG_CloseSelectWindow();
					NovelMessage.step2 = NovelMessage.step1;
					NovelMessage.step1 = MSG_DRAG;

					DSP_ResetGraph( GRP_KEYWAIT );
					DSP_SetText( TXT_WINDOW+1, LAY_WINDOW+1, SYS_FONT, OFF, NovelBuf.buf[(NovelBuf.bpoint-2+NLOG_MAX)%NLOG_MAX] );
					DSP_SetTextPos( TXT_WINDOW+1, MES_POS_X, MES_POS_Y, MES_POS_W, MES_POS_H );
					DSP_SetTextPich( TXT_WINDOW+1, MES_PICH_W, MES_PICH_H );
					DSP_SetTextKage( TXT_WINDOW+1, MES_KAGE );
					DSP_SetTextAlph( TXT_WINDOW+1, -1 );
					DSP_SetTextColor( TXT_WINDOW+1, 10 );
				}else if(command_trg==3 && !Avg.omake){
					AVG_PlaySE3( 9104, 255 );
					AVG_GoConfig(1);
				}else if(command_trg==4 && !Avg.omake){
					AVG_PlaySE3( 9104, 255 );
					AVG_GoConfig(2);
				}else if(command_trg==5){
					AVG_PlaySE3( 9104, 255 );
					Avg.auto_flag = !Avg.auto_flag;
				}else if(command_trg==6){
					AVG_PlaySE3( 9104, 255 );
					if( (Avg.msg_cut_optin&1) || AVG_CheckScenarioFlag() ){
						Avg.msg_cut_mode = !Avg.msg_cut_mode;
					}else{
						Avg.msg_cut_mode = FALSE;
					}
				}else if(command_trg==8){
					AVG_PlaySE3( 9104, 255 );
					AVG_GoConfig(3);
				}else if(command_btrg==9){
					my = MUS_GetMousePosY()-HistorySystemRectY[9];
					Avg.half_tone = 128-LIM(my-HistorySystemSrcH[9]/2,0,HistorySystemRectH[9]-HistorySystemSrcH[9]);
					AVG_SetHalfToneDirect();
				}
			}
			break;
		case MSG_DISP:
			if( NovelMessage.count>=NovelMessage.max || AVG_GetHitKey() || AVG_GetMesCut() || Avg.msg_page ){
				NovelMessage.count=NovelMessage.max;
				if( (NovelMessage.count >= TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), -1 ) + 8) ){
					switch( NovelMessage.add_flag ){
						default:
						case 2:
							NovelMessage.step1 = MSG_STOP;
							NovelMessage.kstep = -1;
							DSP_SetTextCount( TXT_WINDOW, -1 );
							DSP_SetTextStep( TXT_WINDOW, -1 );
							break;
						case 1:	
							if( TXT_GetTextEndKeyWait( DSP_GetTextStr(TXT_WINDOW) ) && !Avg.msg_page ){
								NovelMessage.step1 = MSG_STOP;
								DSP_SetTextCount( TXT_WINDOW, -1 );
							}else{
								if(Avg.msg_page){
									if( AVG_WaitVoice(0) || AVG_GetHitKey() || AVG_GetMesCut() ){
										NovelMessage.step1 = MSG_NEXT;
										DSP_SetTextCount( TXT_WINDOW, -1 );
									}else{
										NovelMessage.step1 = MSG_DISP;
										DSP_SetTextCount( TXT_WINDOW, -1 );
									}
								}else{
									NovelMessage.step1 = MSG_NEXT;
									DSP_SetTextCount( TXT_WINDOW, -1 );
								}
							}
							break;
						case 3:	
							NovelMessage.step1 = MSG_STOP;
							DSP_SetTextCount( TXT_WINDOW, -1 );
							break;
					}
				}else{
					if(Avg.msg_page){
						NovelMessage.step1 = MSG_DISP;
						if(NovelMessage.kstep!=-1) NovelMessage.kstep++;
						NovelMessage.max   = TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), NovelMessage.kstep ) + 8;
						DSP_SetTextStep( TXT_WINDOW, NovelMessage.kstep );
					}else{
						NovelMessage.step1 = MSG_WAIT;
						NovelMessage.count = TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), NovelMessage.kstep );
						DSP_SetTextCount( TXT_WINDOW, -1 );
					}
				}
			}else{
				if( Avg.demo ){
					NovelMessage.count+=(short)(2*30/Avg.frame);
				}else{
					NovelMessage.count+=AVG_MsgCnt();
				}
				DSP_SetTextCount( TXT_WINDOW, NovelMessage.count );
			}
/*
			if( NovelMessage.count>=NovelMessage.max || AVG_GetHitKey() || AVG_GetMesCut() ){
				NovelMessage.count=NovelMessage.max;
				if( (NovelMessage.count >= TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), -1 ) + 8) ){
					switch( NovelMessage.add_flag ){
						default:
						case 2:
							NovelMessage.step1 = MSG_STOP;
							NovelMessage.kstep = -1;
							DSP_SetTextCount( TXT_WINDOW, -1 );
							DSP_SetTextStep( TXT_WINDOW, -1 );
							break;
						case 1:	
							if( TXT_GetTextEndKeyWait( DSP_GetTextStr(TXT_WINDOW) ) && !Avg.msg_page ){
								NovelMessage.step1 = MSG_STOP;
								DSP_SetTextCount( TXT_WINDOW, -1 );
							}else{
								NovelMessage.step1 = MSG_NEXT;
								DSP_SetTextCount( TXT_WINDOW, -1 );
							}
							break;
						case 3:	
							NovelMessage.step1 = MSG_STOP;
							DSP_SetTextCount( TXT_WINDOW, -1 );
							break;
					}
				}else{
					if(Avg.msg_page){
						NovelMessage.step1 = MSG_DISP;
						NovelMessage.kstep++;
						NovelMessage.max   = TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), NovelMessage.kstep ) + 8;
						DSP_SetTextStep( TXT_WINDOW, NovelMessage.kstep );
						DSP_SetTextCount( TXT_WINDOW, NovelMessage.count );
					}else{
						NovelMessage.step1 = MSG_WAIT;
						NovelMessage.count = TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), NovelMessage.kstep );
						DSP_SetTextCount( TXT_WINDOW, -1 );
					}
				}
			}else{
				if( Avg.demo ){
					NovelMessage.count+=(short)(2*30/Avg.frame);
				}else{
					NovelMessage.count+=AVG_MsgCnt();
				}
				DSP_SetTextCount( TXT_WINDOW, NovelMessage.count );
			}
*/
			break;
		case MSG_SYSTEM:




			if( GameKey.cansel || GameKey.diswin || GameKey.click ){
				AVG_SetHalfTone();
				AVG_SetNovelMessageDisp( ON );
				AVG_OpenSelectWindow();

				NovelMessage.step1 = NovelMessage.step2;

			}
			break;
		case MSG_WAIT:
		case MSG_STOP:
			if( Avg.demo ){
				Avg.demo_cnt++;
				if( Avg.demo_cnt>=AVG_EffCnt3(Avg.demo_max) ){
					Avg.demo_cnt=0;

					key_wait_count=0;
					key_wait_count2=0;
					DSP_ResetGraph( GRP_KEYWAIT );
					if(NovelMessage.step1==MSG_WAIT){
						NovelMessage.step1 = MSG_DISP;
						NovelMessage.kstep++;
						NovelMessage.max   = TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), NovelMessage.kstep ) + 8;
						DSP_SetTextStep( TXT_WINDOW, NovelMessage.kstep );
						DSP_SetTextCount( TXT_WINDOW, NovelMessage.count );
					}else{
						NovelMessage.step1 = MSG_NEXT;
						NovelMessage.kstep++;
					}
				}
			}else{
				if( !AVG_GetLaodFlag() ){
					int	px,py;

					if(NovelMessage.step1==MSG_WAIT || NovelMessage.add_flag!=2){
						DSP_SetGraph( GRP_KEYWAIT, BMP_KEYWAIT+1, LAY_WINDOW+3, ON, CHK_ANTI );
					}else{
						DSP_SetGraph( GRP_KEYWAIT, BMP_KEYWAIT+0, LAY_WINDOW+3, ON, CHK_ANTI );

					}
					DSP_GetTextDispPos( TXT_WINDOW, &px, &py );
					DSP_SetGraphSPos( GRP_KEYWAIT, KEYWAIT_SIZE*(GlobalCount/2%30), 0, KEYWAIT_SIZE, KEYWAIT_SIZE );
	
					DSP_SetGraphMove( GRP_KEYWAIT, px-2, py-2 );

				
					key_wait_count  = min(key_wait_count+1,10);
					key_wait_count2 = min(key_wait_count2+1,30);
					if( GlobalCount%30<15 ){

					}else{

					}
				}

				if( GameKey.diswin || command_trg==7 ){
					AVG_PlaySE3( 9104, 255 );
					NovelMessage.step2 = NovelMessage.step1;
					NovelMessage.step1 = MSG_SYSTEM;
	
					AVG_ResetHalfTone();
					AVG_SetNovelMessageDisp( OFF );
				}else if( GameKey.pup || command_btrg==1 ){
					AVG_PlaySE3( 9012, 140 );
					if(NovelBuf.bmax-1>0){
						key_wait_count=0;
						key_wait_count2=0;
						DSP_ResetGraph( GRP_KEYWAIT );
						AVG_NovelLogStart();
					}
				}else if( 
					   ( AVG_GetHitKey() && MUS_GetMousePosX()<800-32 )
					|| ( AVG_GetMesCut() )
					|| ( GameKey.pdown && Avg.tx_pdwon)
					|| ( (KeyCond.btrg.space || KeyCond.btrg.enter) && Avg.tx_lbtrg)
					|| ( MUS_GetMouseBTrigger(MOUSE_LBUTTON) && Avg.tx_lbtrg && MUS_GetMousePosX()<800-32 )
					|| ( MainWindow.save_disp && key_wait_count2==30)
					|| AVG_WaitAutoMode() ){

					AVG_ResetAutoMode();
					key_wait_count=0;
					key_wait_count2=0;
					DSP_ResetGraph( GRP_KEYWAIT );
					if(NovelMessage.step1==MSG_WAIT){
						NovelMessage.step1 = MSG_DISP;
						NovelMessage.kstep++;
						NovelMessage.max   = TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), NovelMessage.kstep ) + 8;
						DSP_SetTextStep( TXT_WINDOW, NovelMessage.kstep );
						DSP_SetTextCount( TXT_WINDOW, NovelMessage.count );
					}else{
						NovelMessage.step1 = MSG_NEXT;
						NovelMessage.kstep++;
					}
				}else if(command_btrg==0){
					NovelMessage.step2 = NovelMessage.step1;
					NovelMessage.step1 = MSG_DRAG;

					key_wait_count=0;
					key_wait_count2=0;
					DSP_ResetGraph( GRP_KEYWAIT );
					DSP_SetText( TXT_WINDOW+1, LAY_WINDOW+1, SYS_FONT, OFF, NovelBuf.buf[(NovelBuf.bpoint-2+NLOG_MAX)%NLOG_MAX] );
					DSP_SetTextPos( TXT_WINDOW+1, MES_POS_X, MES_POS_Y, MES_POS_W, MES_POS_H );
					DSP_SetTextPich( TXT_WINDOW+1, MES_PICH_W, MES_PICH_H );
					DSP_SetTextKage( TXT_WINDOW+1, MES_KAGE );
					DSP_SetTextAlph( TXT_WINDOW+1, -1 );
					DSP_SetTextColor( TXT_WINDOW+1, 10 );
				}else if(command_trg==3 && !Avg.omake){
					AVG_PlaySE3( 9104, 255 );
					AVG_GoConfig(1);
				}else if(command_trg==4 && !Avg.omake){
					AVG_PlaySE3( 9104, 255 );
					AVG_GoConfig(2);
				}else if(command_trg==5){
					AVG_PlaySE3( 9104, 255 );
					Avg.auto_flag = !Avg.auto_flag;
				}else if(command_trg==6){
					AVG_PlaySE3( 9104, 255 );
					if( (Avg.msg_cut_optin&1) || AVG_CheckScenarioFlag() ){
						Avg.msg_cut_mode = !Avg.msg_cut_mode;
					}else{
						Avg.msg_cut_mode = FALSE;
					}
				}else if(command_trg==8){
					AVG_PlaySE3( 9104, 255 );
					AVG_GoConfig(3);
				}else if(command_btrg==9){
					my = MUS_GetMousePosY()-HistorySystemRectY[9];
					Avg.half_tone = 128-LIM(my-HistorySystemSrcH[9]/2,0,HistorySystemRectH[9]-HistorySystemSrcH[9]);
					AVG_SetHalfToneDirect();
				}
			}
			break;
		case MSG_LOG:
			if(MUS_GetMouseNo(-1)>=32){
				int	mno = MUS_GetMouseNo(-1)-32;
				int	cnt = (NovelBuf.bpoint-1-NovelBuf.bcount+NLOG_MAX)%NLOG_MAX;

				int px = MES_POS_X;
				int py = NovelBuf.nv[cnt].py[mno];

				DSP_SetText( TXT_WINDOW+2, LAY_WINDOW+2, SYS_FONT, ON, NovelBuf.vv_mes[ mno ] );
				DSP_SetTextPos( TXT_WINDOW+2, px, py, MES_POS_W, MES_POS_H );
				DSP_SetTextPich( TXT_WINDOW+2, MES_PICH_W, MES_PICH_H );
				DSP_SetTextKage( TXT_WINDOW+2, MES_KAGE );
				DSP_SetTextAlph( TXT_WINDOW+2, -1 );
				DSP_SetTextColor( TXT_WINDOW+2, 11 );

				if( GameKey.click ){
					AVG_PlayVoice( 0, NovelBuf.nv[cnt].cno[mno], NovelBuf.nv[cnt].sno[mno], NovelBuf.nv[cnt].vno[mno], 255, 0, NovelBuf.nv[cnt].a_cut[mno], 1 );
				}
			}else{
				DSP_ResetText( TXT_WINDOW+2 );
			}
			if(AVG_GetHitKey() || GameKey.cansel){
				ResetHistoryVoiceMouseRect();

				DSP_ResetText( TXT_WINDOW+1 );
				DSP_ResetText( TXT_WINDOW+2 );
				DSP_SetTextDisp( TXT_WINDOW, ON );

				AVG_OpenSelectWindow();
				NovelBuf.bcount=0;
				NovelMessage.step1 = NovelMessage.step2;

			}else if( GameKey.pup || command_btrg==1 ){
				DSP_ResetText( TXT_WINDOW+2 );
				AVG_PlaySE3( 9012, 140 );
				NovelMessage.step1 = MSG_UP;
			}else if( GameKey.pdown || command_btrg==2 ){
				DSP_ResetText( TXT_WINDOW+2 );
				AVG_PlaySE3( 9012, 140 );
				NovelMessage.step1 = MSG_DOWN;
			}else if(command_btrg==0){
				key_wait_count=0;
				DSP_ResetGraph( GRP_KEYWAIT );
				NovelMessage.step1 = MSG_DRAG;
			}else if(command_btrg==9){
				my = MUS_GetMousePosY()-HistorySystemRectY[9];
				Avg.half_tone = 128-LIM(my-HistorySystemSrcH[9]/2,0,HistorySystemRectH[9]-HistorySystemSrcH[9]);
				AVG_SetHalfToneDirect();
			}
			break;
		case MSG_UP:
			NovelBuf.bcount = min(NovelBuf.bcount+1,NovelBuf.bmax-1);
			if(NovelBuf.bcount==0){
				ResetHistoryVoiceMouseRect();
				DSP_ResetText( TXT_WINDOW+1 );
				DSP_SetTextDisp( TXT_WINDOW, ON );
				AVG_OpenSelectWindow();
				NovelMessage.step1 = NovelMessage.step2;
			}else{
				SetHistoryVoiceMouseRect();
				DSP_SetTextStr( TXT_WINDOW+1, NovelBuf.buf[(NovelBuf.bpoint-1-NovelBuf.bcount+NLOG_MAX)%NLOG_MAX] );
				NovelMessage.step1 = MSG_LOG;
			}
			break;
		case MSG_DOWN:
			if(NovelBuf.bcount<=1){
				ResetHistoryVoiceMouseRect();
				DSP_ResetText( TXT_WINDOW+1 );
				DSP_SetTextDisp( TXT_WINDOW, ON );
				AVG_OpenSelectWindow();
				NovelBuf.bcount=0;
				NovelMessage.step1 = NovelMessage.step2;
			}else{
				NovelBuf.bcount--;
				SetHistoryVoiceMouseRect();
				DSP_SetTextStr( TXT_WINDOW+1, NovelBuf.buf[(NovelBuf.bpoint-1-NovelBuf.bcount+NLOG_MAX)%NLOG_MAX] );
				NovelMessage.step1 = MSG_LOG;
			}
			break;
		case MSG_DRAG:
			my = MUS_GetMousePosY();

			if(NovelBuf.bmax>1){
				NovelBuf.bcount = (NovelBuf.bmax-1)-(my-HistorySystemRectY[0]+((HistorySystemRectH[0]-40)/((NovelBuf.bmax-1)*2)))*(NovelBuf.bmax-1)/(HistorySystemRectH[0]-40);
				NovelBuf.bcount = LIM(NovelBuf.bcount, 0, NovelBuf.bmax-1 );
			}else{
				NovelBuf.bcount = 0;
			}

			if(NovelBuf.bcount>0){
				DSP_SetTextDisp( TXT_WINDOW,   OFF );
				DSP_SetTextDisp( TXT_WINDOW+1, ON );
				SetHistoryVoiceMouseRect();
				DSP_SetTextStr( TXT_WINDOW+1, NovelBuf.buf[(NovelBuf.bpoint-1-NovelBuf.bcount+NLOG_MAX)%NLOG_MAX] );
			}else{
				DSP_SetTextDisp( TXT_WINDOW,   ON );
				DSP_SetTextDisp( TXT_WINDOW+1, OFF );
			}
			if( !MUS_GetMouseButton2(MOUSE_LBUTTON) ){
				if(NovelBuf.bcount>0){
					NovelMessage.step1 = MSG_LOG;
				}else{
					ResetHistoryVoiceMouseRect();
					NovelBuf.bcount=0;

					DSP_ResetText( TXT_WINDOW+1 );
					DSP_SetTextDisp( TXT_WINDOW, ON );
					AVG_OpenSelectWindow();
					NovelMessage.step1 = NovelMessage.step2;
				}

			}
			break;
		case MSG_WAIT2:
			break;
	}
	ControlHistorySystem();
}

void AVG_SetSaveDataNovelMessage( AVG_SAVE_DATA *sdata )
{
/*
typedef struct{
	char		flag;			
	char		add_flag;			
	char		disp;			
	char		step1;			
	char		step2;			
	short		count;			
	short		kstep;			
	short		max;			
}NOVEL_MESSEGE;
NOVEL_MESSEGE	NovelMessage;

typedef struct{
	char		buf[NLOG_MAX][NLOG_BUF];	
	char		bflag[NLOG_MAX];			
	int			bmax;					
	int			bpoint;					
	int			bcount;					
}NOVEL_BUF;
NOVEL_BUF	NovelBuf;
*/
	sdata->ms_flag  = NovelMessage.flag;			
	sdata->ms_add   = NovelMessage.add_flag;			

	sdata->ms_disp  = ON;							
	sdata->ms_step1 = NovelMessage.step2;			
	sdata->ms_count = NovelMessage.count;			
	sdata->ms_kstep = NovelMessage.kstep;			
	sdata->ms_max   = NovelMessage.max;			
	{
		char	buf[1024];
		int		i,j,cnt;
		if( AVG_GetSelectMessageFlag() ){
			strncpy( buf, NovelBuf.buf[ (NovelBuf.bpoint-1+NLOG_MAX)%NLOG_MAX ], NLOG_BUF );
		}else{
			strncpy( buf, NovelMessage.str, NLOG_BUF );
		}
		ZeroMemory( sdata->ms_str, 1024 );

		i=0;
		j=0;
		cnt=0;
		while(buf[i]){
			switch( buf[i] ){
				case '\\':
					if(buf[i+1]=='k'){
						i++;
						i++;
						cnt++;
					}else{
						sdata->ms_str[j++] = buf[i++];
					}
					break;
				default:	sdata->ms_str[j++] = buf[i++];	break;
			}
		}
	}
}
void AVG_SetLoadDataNovelMessage( AVG_SAVE_DATA *sdata )
{
/*
typedef struct{
	char		flag;			
	char		add_flag;			
	char		disp;			
	char		step1;			
	char		step2;			
	short		count;			
	short		kstep;			
	short		max;			
}NOVEL_MESSEGE;
NOVEL_MESSEGE	NovelMessage;
*/
/*
	sdata.ms_add   = NovelMessage.add_flag;			
	sdata.ms_disp  = NovelMessage.disp;			
	sdata.ms_step1 = NovelMessage.step1;			
	sdata.ms_step2 = NovelMessage.step2;			
	sdata.ms_count = NovelMessage.count;			
	sdata.ms_kstep = NovelMessage.kstep;			
	sdata.ms_max   = NovelMessage.max;			
	strncpy( sdata.ms_str, NovelBuf.buf[ LOOP(NovelBuf.bpoint-1,NLOG_MAX) ], NLOG_BUF );	
*/
	ZeroMemory( &NovelBuf, sizeof(NOVEL_BUF) );
	ZeroMemory( &NovelMessage, sizeof(NOVEL_MESSEGE) );

	NovelMessage.flag     = sdata->ms_flag;
	NovelMessage.add_flag = sdata->ms_add;
	
	AVG_SetNovelMessageDisp( sdata->ms_disp );

	NovelMessage.step2  = NovelMessage.step1  = sdata->ms_step1;
	NovelMessage.max    = TXT_GetTextCount( sdata->ms_str, 0 )+8;
	NovelMessage.count  = NovelMessage.max;

	DSP_SetText( TXT_WINDOW, LAY_WINDOW+1, SYS_FONT, sdata->ms_disp, sdata->ms_str );
	DSP_SetTextPos( TXT_WINDOW, MES_POS_X, MES_POS_Y, MES_POS_W, MES_POS_H );
	DSP_SetTextPich( TXT_WINDOW, MES_PICH_W, MES_PICH_H );
	DSP_SetTextKage( TXT_WINDOW, MES_KAGE );
	DSP_SetTextAlph( TXT_WINDOW, -1 );

	DSP_SetTextCount( TXT_WINDOW, -1 );
	DSP_SetTextStep( TXT_WINDOW, 0 );



	if(sdata->ms_str[0]!='\0')
		SetNovelMessageHistory(sdata->ms_str);
	



}




#define MSG_WINODW_W	(DISP_X)
#define MSG_WINODW_H	(120)
#define MSG_WINODW_X	(0)
#define MSG_WINODW_Y	(DISP_Y-MSG_WINODW_H)
#define MSG_WINODW_CX	(MSG_WINODW_X+MSG_WINODW_W/2)
#define MSG_WINODW_CY	(MSG_WINODW_Y+MSG_WINODW_H/2)

#define WIN_ALPHA		192

enum{
	MWIN_NODISP,
	MWIN_OPEN,
	MWIN_STOP,
	MWIN_CLOSE,
};


#define LOG_MAX		8
#define LOG_BUF		8

typedef struct{
	char		mflag;			
	char		mstep;			
	char		mstep2;			
	short		mcount;			
	short		mkstep;			

	short		add_flag;		
	short		wait_flag;		

	short		wait;			
	int			wait_cnt;		
	short		wait_op;		

	short		rflag;			

	char		wflag;			
	char		wstep;			
	char		bstep;			
	char		bstep2;			
	short		wcount;			
	short		max;			
	signed char	cno;			

	char		buf[LOG_MAX][LOG_BUF];	
	char		bflag[LOG_MAX];		
	signed char	bcno[LOG_MAX];		
	int			hight[LOG_MAX];		
	int			bpoint;				
	int			bmax;				

	int			bcount;				
	int			rcount;				
	int			bar_y;				

	int			nowin;				
}MESSEGE_STRUCT;
MESSEGE_STRUCT	Message;

#define MES_POS(X)			((Message.bpoint+LOG_MAX*2+((X)+Message.bcount))%LOG_MAX)
#define MES_HIGHT(P,P2)		(Message.hight[MES_POS(P)]-Message.hight[MES_POS(P2)])

typedef struct{
	int		dx;
	int		dy;
	int		sx;
	int		sy;
	int		w;
	int		h;
}PARTS_RECT;

PARTS_RECT		MsgCons[11] = {
	{     50, 360,     50,   0,   640-100, 120 },	
	{      5, 360,      5,   0,      50-5, 120 },	
	{    588, 360,    588,   0,      50-2, 120 },	

	{     14, 370,      1, 122,     31,  76 },	
	{     14, 446,      1, 198,     31,  24 },	
	{    604, 370,     33, 122,     22,  50 },	
	{    604, 420,     33, 172,     22,  50 },	

	{    592, 451,    192, 120,      10,  20 },	
	{    592, 369,      0,   0,      10, 102 },	
	
	{      0,      0, 216, 120,      24,  24 },	
	{ 320-16, 480-16, 192, 144,      32,  16 },	
};

void SetDragBar(void)
{
	int		y;

	if(Message.bmax-1){
		y = MsgCons[7].dy + Message.bcount*82/(Message.bmax-1);
	}else{
		y = MsgCons[7].dy;
	}
	Message.bar_y = y;
	DSP_SetGraphMove( GRP_WINDOW+7, MsgCons[7].dx, y );
	MUS_SetMouseRect( 0, 4, MsgCons[7].dx, y, MsgCons[7].w, MsgCons[7].h, 1 );
}

void SetMessageWindow_MouseRect( void )
{
	MUS_SetMouseRect( 0, 0, MsgCons[3].dx, MsgCons[3].dy, MsgCons[3].w, MsgCons[3].h, 1 );
	MUS_SetMouseRect( 0, 1, MsgCons[4].dx, MsgCons[4].dy, MsgCons[4].w, MsgCons[4].h, 1 );
	MUS_SetMouseRect( 0, 2, MsgCons[5].dx, MsgCons[5].dy, MsgCons[5].w, MsgCons[5].h, 1 );

	MUS_SetMouseRect( 0, 3, MsgCons[6].dx, MsgCons[6].dy, MsgCons[6].w, MsgCons[6].h, 1 );
	MUS_SetMouseRect( 0, 4, MsgCons[7].dx, MsgCons[7].dy, MsgCons[7].w, MsgCons[7].h, 1 );
	MUS_SetMouseRect( 0, 5, MsgCons[8].dx, MsgCons[8].dy, MsgCons[8].w, MsgCons[8].h, 1 );

	SetDragBar();
}

void ResetMessageWindow_MouseRect( void )
{
	MUS_ResetMouseRect( 0, 0 );
	MUS_ResetMouseRect( 0, 1 );
	MUS_ResetMouseRect( 0, 2 );
	MUS_ResetMouseRect( 0, 3 );
	MUS_ResetMouseRect( 0, 4 );
	MUS_ResetMouseRect( 0, 5 );

}

void AVG_SetWindow( int direct )
{
	int		i;

	if( direct){
		Message.wflag  = 1;
		Message.wcount = 0;
		Message.wstep  = MWIN_STOP;



		if(Avg.level && Avg.win_bld==1){
			for(i=0;i<3;i++)DSP_SetGraph( GRP_WINDOW+i, BMP_WINDOW, LAY_WINDOW,   ON, CHK_ANTI );
		}else{
			for(i=0;i<3;i++)DSP_SetGraph( GRP_WINDOW+i, BMP_WINDOW, LAY_WINDOW,   ON, CHK_256 );
		}
		for(i=3;i<8;i++)DSP_SetGraph( GRP_WINDOW+i, BMP_WINDOW, LAY_WINDOW+1, ON, CHK_NO );
		
		switch(Avg.win_bld)	{
			case 0:	DSP_SetGraphParam( GRP_WINDOW, DRW_NML );				break;
			case 1:	DSP_SetGraphParam( GRP_WINDOW, DRW_BLD(WIN_ALPHA) );	break;
			case 2:	DSP_SetGraphParam( GRP_WINDOW, DRW_AMI(128) );			break;
		}
		for(i=0;i<8;i++)
			DSP_SetGraphPos( GRP_WINDOW+i, MsgCons[i].dx, MsgCons[i].dy, MsgCons[i].sx, MsgCons[i].sy, MsgCons[i].w, MsgCons[i].h );

		SetMessageWindow_MouseRect();
		MainWindow.draw_flag=ON;
	}else{
		Message.wflag  = 1;
		Message.wcount = 0;
		Message.wstep  = MWIN_OPEN;

		for(i=0;i<3;i++)DSP_SetGraph( GRP_WINDOW+i, BMP_WINDOW, LAY_WINDOW,   ON, CHK_256 );
		for(i=3;i<8;i++)DSP_SetGraph( GRP_WINDOW+i, BMP_WINDOW, LAY_WINDOW+1, ON, CHK_256 );
		
		switch(Avg.win_bld)	{
			case 0:	DSP_SetGraphParam( GRP_WINDOW, DRW_NML );				break;
			case 1:	DSP_SetGraphParam( GRP_WINDOW, DRW_BLD(WIN_ALPHA) );	break;
			case 2:	DSP_SetGraphParam( GRP_WINDOW, DRW_AMI(128) );			break;
		}
		for(i=0;i<8;i++)
			DSP_SetGraphPos( GRP_WINDOW+i, MsgCons[i].dx, MsgCons[i].dy, MsgCons[i].sx, MsgCons[i].sy, MsgCons[i].w, MsgCons[i].h );
	}
}

void AVG_ResetWindowGrph( void )
{
	int	i;
	for(i=0;i<9;i++)
		DSP_ResetGraph( GRP_WINDOW+i );
}
void AVG_ResetWindow( int direct )
{
	if(direct){
		Message.wflag  = 0;
		Message.wcount = 0;
		Message.wstep  = MWIN_NODISP;

		AVG_ResetMessage();
		AVG_ResetWindowGrph();

		ResetMessageWindow_MouseRect();
		MainWindow.draw_flag=ON;
	}else{
		Message.wflag  = 1;
		Message.wcount = 0;
		Message.wstep  = MWIN_CLOSE;

		AVG_ResetMessage();

		ResetMessageWindow_MouseRect();
	}
}

BOOL AVG_WaitWindow( void )
{
	return (Message.wstep==MWIN_STOP) || (Message.wstep==MWIN_NODISP);
}

static void SetMessageWindowEffect( int cnt )
{
	int	i;

	for(i=0;i<3;i++)DSP_SetGraphNuki( GRP_WINDOW+i, CHK_256 );
	if(Avg.level){
		if(Avg.win_bld)	{ DSP_SetGraphParam( GRP_WINDOW, DRW_BLD(WIN_ALPHA*cnt/256) );	}
		else			{ DSP_SetGraphParam( GRP_WINDOW, DRW_BLD(cnt) );				}
		for(i=1;i<8;i++)	DSP_SetGraphParam( GRP_WINDOW+i, DRW_BLD(cnt) );
		cnt = cnt*cnt/256;
		cnt = 256-cnt;

		for(i=0;i<8;i++)
			DSP_SetGraphZoom2( GRP_WINDOW+i, MSG_WINODW_CX, MSG_WINODW_CY, cnt );
	}else{
		if(Avg.win_bld)	{ DSP_SetGraphParam( GRP_WINDOW, DRW_AMI(WIN_ALPHA*cnt/256) );	}
		else			{ DSP_SetGraphParam( GRP_WINDOW, DRW_AMI(cnt) );				}
		for(i=1;i<8;i++)	DSP_SetGraphParam( GRP_WINDOW+i, DRW_AMI(cnt) );


	}
/*	x = -64*cnt/256;
	DSP_SetGraphMove( GRP_WINDOW+1, MsgCons[1].dx+x, MsgCons[1].dy );
	DSP_SetGraphMove( GRP_WINDOW+3, MsgCons[3].dx+x, MsgCons[3].dy );
	DSP_SetGraphMove( GRP_WINDOW+4, MsgCons[4].dx+x, MsgCons[4].dy );
	x = 64*cnt/256;
	DSP_SetGraphMove( GRP_WINDOW+2, MsgCons[2].dx+x, MsgCons[2].dy );
	DSP_SetGraphMove( GRP_WINDOW+5, MsgCons[5].dx+x, MsgCons[5].dy );
	DSP_SetGraphMove( GRP_WINDOW+6, MsgCons[6].dx+x, MsgCons[6].dy );
	DSP_SetGraphMove( GRP_WINDOW+7, MsgCons[7].dx+x, MsgCons[7].dy );
*/
}

BOOL AVG_GetWindowCond( void )
{
	BOOL	ret=FALSE;
	
	switch(Message.wstep){
		case MWIN_NODISP:	ret = FALSE;	break;
		case MWIN_STOP:		ret = 1;		break;
		case MWIN_OPEN:
		case MWIN_CLOSE:	ret = -1;		break;
	}
	return ret;
}

void AVG_OpenWindow( int tdisp, int flag )
{
	int	i;




		AVG_SetNovelMessageDisp( ON );
		NovelMessage.step1 = NovelMessage.step2;
		for(i=0;i<10;i++)
			DSP_SetGraphDisp( GRP_WINDOW+i, ON );
		if(tdisp){
			for(i=0;i<5;i++)
				DSP_SetTextDisp( TXT_WINDOW+i, ON );
		}
		for( i=0; i<MAX_SCRIPT_OBJ ; i++ ){
			if(SpriteBmp[i].disp){
				DSP_SetGraphDisp( GRP_SCRIPT+i, ON );
			}
		}


}
void AVG_CloseWindow( int flag )
{
	int	i;


		AVG_SetNovelMessageDisp( OFF );
		NovelMessage.step2 = NovelMessage.step1;
		NovelMessage.step1 = MSG_NODISP;
		for(i=0;i<10;i++)
			DSP_SetGraphDisp( GRP_WINDOW+i, OFF );

		for( i=0; i<MAX_SCRIPT_OBJ ; i++ ){
			if(SpriteBmp[i].disp){
				DSP_SetGraphDisp( GRP_SCRIPT+i, OFF );
			}
		}
		for(i=0;i<10;i++)
			DSP_SetGraphDisp( GRP_WINDOW+i, OFF );

		for(i=0;i<5;i++)
			DSP_SetTextDisp( TXT_WINDOW+i, OFF );


}

static char	MessageCut = 0;

void AVG_ControlWindow( void )
{
	int		wmax = AVG_EffCnt( Avg.wait*10 );
	int		cnt;
	int		i;

	if( Message.wflag ){
		switch(Message.wstep){
			case MWIN_NODISP:
				break;
			case MWIN_OPEN:
				Message.wcount++;

				if(Message.wcount>=wmax || (MessageCut=AVG_GetHitKey()) || !Avg.win_eff ){
					SetMessageWindow_MouseRect();
					Message.wstep = MWIN_STOP;
					cnt = 256;
				}else{
					cnt = 256*Message.wcount/wmax;
				}
				SetMessageWindowEffect( cnt );
				break;
			case MWIN_STOP:
				switch(Avg.win_bld){
					case 0:	DSP_SetGraphParam( GRP_WINDOW, DRW_NML );				break;
					case 1:	DSP_SetGraphParam( GRP_WINDOW, DRW_BLD(WIN_ALPHA) );	break;
					case 2:	DSP_SetGraphParam( GRP_WINDOW, DRW_AMI(128) );			break;
				}


				for(i=1;i<8;i++)	DSP_SetGraphParam( GRP_WINDOW+i, DRW_NML );
				if(Avg.level && Avg.win_bld==1){
					for(i=0;i<3;i++)DSP_SetGraphNuki( GRP_WINDOW+i, CHK_ANTI );
				}else{
					for(i=0;i<3;i++)DSP_SetGraphNuki( GRP_WINDOW+i, CHK_256 );
				}
				
				DSP_SetGraphPos( GRP_WINDOW, MsgCons[0].dx, MsgCons[0].dy, MsgCons[0].sx, MsgCons[0].sy, MsgCons[0].w, MsgCons[0].h );
				break;
			case MWIN_CLOSE:
				Message.wcount++;
				if( Message.wcount>=wmax || AVG_GetHitKey() || !Avg.win_eff ){
					Message.wflag  = 0;
					Message.wstep = MWIN_NODISP;
					AVG_ResetWindowGrph();
					cnt = 0;
				}else{
					cnt = 256-256*Message.wcount/wmax;
					SetMessageWindowEffect( cnt );
				}
				break;
		}
	}
}


#define MSG_CTEXT_X		(20)
#define MSG_CTEXT_Y		(32+352+20)
#define MSG_CTEXT_W		(25)
#define MSG_CTEXT_H		(4)

#define MSG_TEXT_X		(MSG_WINODW_X+70)
#define MSG_TEXT_Y		(MSG_WINODW_Y+14)
#define MSG_TEXT_W		(25)
#define MSG_TEXT_H		(4)

#define MSG_TEXT_CX		(MSG_WINODW_X+60)
#define MSG_TEXT_CY		(MSG_WINODW_Y+12)
#define MSG_TEXT_CW		(MSG_WINODW_W-110)
#define MSG_TEXT_CH		(MSG_WINODW_H-25)

#define MSG_TEXT_PW		(0)
#define MSG_TEXT_PH		(4)

int	mes_count=0;

BOOL AVG_GetMessageFlag( void )
{
	return Message.mflag;
}



void SetMessageHistory( char *str )
{
	int	i;

	DSP_SetText(     TXT_WORK, LAY_WINDOW, SYS_FONT, OFF, str );
	DSP_SetTextPos(  TXT_WORK, MSG_TEXT_X, MSG_TEXT_Y, MSG_TEXT_W, MSG_TEXT_H );
	DSP_SetTextPich( TXT_WORK, 0, MSG_TEXT_PH );
	DSP_SetTextAlph( TXT_WORK, -1 );
	DSP_SetTextCount(TXT_WORK, -1  );

	strcpy(Message.buf[MES_POS(0)], str );
	Message.bflag[MES_POS(0)] = 1;


	Message.hight[MES_POS(0)] = (DSP_GetTextDispH(TXT_WORK)+MSG_TEXT_PH)/24*24;
	for(i=1;i<LOG_MAX;i++){
		Message.hight[MES_POS(-i)] += Message.hight[MES_POS(0)];
	}
	Message.bpoint = MES_POS(1);
	Message.bmax   = min(256,Message.bmax+1);
	Message.bar_y  = MsgCons[7].dy;

	DSP_ResetText( TXT_WORK );
}

#define SCTEXT_MAX	32
typedef struct{
	int	flag;
	int	count;
	int	max;

	int	font;			
	int	x,y,w,h;		
	int	pichw,pichh;	
	int	kage;			
	int	alph;			
	int	wait;			
						
						

	int	dy;				
}SCRIPT_TEXT;
SCRIPT_TEXT		ScText[SCTEXT_MAX];

#define TEXT_WAIT_SYNCKEY	0
#define TEXT_WAIT_SYNC		1
#define TEXT_WAIT_NOSYNC	2


void AVG_SetTextFormat( int txt_no, int font, int x, int y, int w, int h, int pichw, int pichh, int kage, int alph, int wait )
{
	ScText[txt_no].font = font;
	ScText[txt_no].x = x;
	ScText[txt_no].y = y;
	ScText[txt_no].w = w;
	ScText[txt_no].h = h;
	ScText[txt_no].pichw = pichw;
	ScText[txt_no].pichh = pichh;
	ScText[txt_no].kage = kage;
	ScText[txt_no].alph = alph;
	ScText[txt_no].wait = wait;
	ScText[txt_no].dy = 0;
}

void AVG_SetTextSync( int txt_no1, int txt_no2 )
{
	ScText[txt_no1]    = ScText[txt_no2];
	ScText[txt_no1].y  = ScText[txt_no2].dy;
	ScText[txt_no1].dy = 0;
}

void AVG_SetText( int txt_no, char *str )
{
	DSP_SetText(     TXT_SCRIPT+txt_no, LAY_WINDOW-1, ScText[txt_no].font, ON, str );
	DSP_SetTextPos(  TXT_SCRIPT+txt_no, ScText[txt_no].x, ScText[txt_no].y, ScText[txt_no].w/ScText[txt_no].font, ScText[txt_no].h/ScText[txt_no].font );
	DSP_SetTextPich( TXT_SCRIPT+txt_no, ScText[txt_no].pichw, ScText[txt_no].pichh );
	DSP_SetTextAlph( TXT_SCRIPT+txt_no, ScText[txt_no].alph );
	DSP_SetTextCount(TXT_SCRIPT+txt_no, 0  );
	DSP_SetTextKage( TXT_SCRIPT+txt_no, ScText[txt_no].kage );

	ScText[txt_no].flag  = ON;
	ScText[txt_no].count = 0;
	ScText[txt_no].max   = TXT_GetTextCount(str,0);



	ScText[txt_no].dy = DSP_GetTextDispH(TXT_SCRIPT+txt_no)+ScText[txt_no].y+ScText[txt_no].pichh;
}

void AVG_ResetText( int txt_no )
{
	DSP_ResetText( TXT_SCRIPT+txt_no );

	ScText[txt_no].flag  = OFF;
	ScText[txt_no].count = 0;
	ScText[txt_no].max   = 0;
}

void AVG_ResetTextAll( void )
{
	int	i;
	for(i=0;i<SCTEXT_MAX;i++){
		AVG_ResetText( i );
	}
}

BOOL AVG_WaitText( int txt_no, int flag )
{
	BOOL	ret = TRUE;
	switch( ScText[txt_no].wait ){
		default:
		case TEXT_WAIT_SYNCKEY:
			if(ScText[txt_no].count < ScText[txt_no].max){
				if( AVG_WaitKeyMessage() ){
					ScText[txt_no].count = ScText[txt_no].max+8;
				}
				ret = FALSE;
			}else{
				ret = AVG_WaitKeyMessage();
			}
			break;
		case TEXT_WAIT_SYNC:
			if(ScText[txt_no].count < ScText[txt_no].max){
				if( AVG_WaitKeyMessage() ){
					ScText[txt_no].count = ScText[txt_no].max+8;
				}
				ret = FALSE;
			}else{
				ret = TRUE;
			}
			break;
		case TEXT_WAIT_NOSYNC:
			if( flag==OFF ) { ret = TRUE; break; }
			ret = ScText[txt_no].count >= ScText[txt_no].max;
			break;
	}
	return ret;
}

void AVG_ControlText( void )
{
	int	i;
	for(i=0;i<SCTEXT_MAX;i++){
		if(ScText[i].flag){
			ScText[i].count++;
			DSP_SetTextCount( TXT_SCRIPT+i, ScText[i].count );
		}
	}
}


void AVG_SetMessage( int char_no, char *str, int type, int add_flag, int dy, int font )
{
	int		i,x;

	if(Message.wflag){
		Message.nowin = OFF;
	}else{
		Message.nowin = ON;
	}
mes_count=GlobalCount;
Message.mstep2=MSG_STOP;
MainWindow.draw_flag=ON;




	Message.mflag    = 1;
	Message.mstep    = MSG_DISP;
	Message.cno      = char_no;
	Message.add_flag = add_flag;
	Message.wait_flag= 0;

	if( ChipBackStruct.flag && !Message.wflag ){
		int	f = (font==20)? 30*2 :25*2;
		i = min(f,TXT_GetTextCountEnter(str));
		x = (f-i)*CHIP_FONT/2/2;

		if(font==-1)DSP_SetText( TXT_WINDOW, LAY_WINDOW+1, CHIP_FONT, 1, str );
		else		DSP_SetText( TXT_WINDOW, LAY_WINDOW+1, font, 1, str );
		if(dy==-1)	DSP_SetTextPos( TXT_WINDOW, MSG_CTEXT_X+x, MSG_CTEXT_Y, MSG_CTEXT_W, MSG_CTEXT_H );
		else		DSP_SetTextPos( TXT_WINDOW, MSG_CTEXT_X+x, dy, MSG_CTEXT_W, MSG_CTEXT_H );
		DSP_SetTextPich( TXT_WINDOW, 0, 8 );

		DSP_SetTextAlph( TXT_WINDOW, -1 );
	}else{
		if(font==-1)DSP_SetText( TXT_WINDOW, LAY_WINDOW+1, SYS_FONT, 1, str );
		else		DSP_SetText( TXT_WINDOW, LAY_WINDOW+1, font, 1, str );
		if(dy==-1){
			DSP_SetTextPos( TXT_WINDOW, MSG_TEXT_X, MSG_TEXT_Y, MSG_TEXT_W, MSG_TEXT_H );
			DSP_SetTextClip( TXT_WINDOW, MSG_TEXT_CX, MSG_TEXT_CY, MSG_TEXT_CW, MSG_TEXT_CH );
		}else{
			int	f = (font==24)? 25*2 :30*2;
			i = min(f,TXT_GetTextCountEnter(str));
			x = (f-i)*CHIP_FONT/2/2;

			DSP_SetTextPos( TXT_WINDOW, MSG_CTEXT_X+x, dy, f, MSG_TEXT_CH );
		}
		DSP_SetTextPich( TXT_WINDOW, MSG_TEXT_PW, MSG_TEXT_PH );
		DSP_SetTextAlph( TXT_WINDOW, -1 );
		DSP_SetTextKage( TXT_WINDOW, 2 );
	}
	if(type){
		Message.max    = TXT_GetTextCount( str, -1 )+8;
		Message.mcount = Message.max;
		Message.mkstep  = -1;
		Message.rflag  = 0;
		DSP_SetTextCount( TXT_WINDOW, -1 );
		DSP_SetTextStep( TXT_WINDOW,  -1 );
	}else{
		Message.max    = TXT_GetTextCount( str, 0 )+8;
		Message.mcount = 0;
		Message.mkstep  = 0;
		DSP_SetTextCount( TXT_WINDOW, Message.mcount );
		DSP_SetTextStep( TXT_WINDOW, Message.mkstep );
	}
	Message.rflag  = type;

	SetMessageHistory( str );
}
void AVG_AddMessage( int char_no, char *str, int cr, int add_flag )
{
	char	buf[1024];
	int		i, size;





MainWindow.draw_flag=ON;

	Message.mflag    = 1;
	Message.mstep    = MSG_DISP;
	Message.mcount   = Message.max;
	Message.add_flag = add_flag;
	
	if(Message.wait_flag){
		Message.wait_flag= 0;
	}
	if(cr){
		wsprintf( buf, "\n\\k%s", str );
		strcat( Message.buf[MES_POS(-1)], buf );
		if( !DSP_AddText( TXT_WINDOW, buf) ){
			AVG_SetMessage( char_no, str, 0, 0x02, -1, SYS_FONT );
			return ;
		}

	}else{
		wsprintf( buf, "\\k%s", str );
		strcat( Message.buf[MES_POS(-1)], buf );

		if( !DSP_AddText( TXT_WINDOW, buf) ){
			while( 'n\\'==*(WORD*)str ){
				str+=2;
			}
			AVG_SetMessage( char_no, str, 0, 0x02, -1, SYS_FONT );
			return ;
		}
	}
	Message.max += TXT_GetTextCount( buf, 1 );
	DSP_SetTextCount( TXT_WINDOW, Message.mcount );
	DSP_SetTextDisp( TXT_WINDOW, ON );

	size = DSP_GetTextDispH( TXT_WINDOW )+2 - Message.hight[MES_POS(-1)];
	Message.hight[MES_POS(-1)] += size;
	for(i=1;i<LOG_MAX;i++){
		Message.hight[MES_POS(-i-1)] += size;
	}
}
void AVG_ResetMessage( void )
{
	Message.mflag  = 0;
	Message.mstep  = MSG_NODISP;
	Message.mcount = 0;
	DSP_ResetText( TXT_WINDOW );
	DSP_ResetGraph( GRP_KEYWAIT );
}
BOOL AVG_BackMessage( void )
{
	int	i;
	int	sp, index;
	BOOL	ret=FALSE;

	if( Message.bcount > -(LOG_MAX-1) && Message.bflag[MES_POS(-2)] ){
		for(i=0;i<5;i++){
			sp = MES_POS(-2+i);
			index = -2+Message.bcount+i;
			if( index<0 ){
				DSP_SetText( TXT_WINDOW+i, LAY_WINDOW+1, SYS_FONT, 1, Message.buf[sp] );
				DSP_SetTextPos( TXT_WINDOW+i, MSG_TEXT_X, MSG_TEXT_Y-MES_HIGHT(-2+i,-1), MSG_TEXT_W, MSG_TEXT_H );
				DSP_SetTextPich( TXT_WINDOW+i, MSG_TEXT_PW, MSG_TEXT_PH );
				DSP_SetTextClip( TXT_WINDOW+i, MSG_TEXT_CX, MSG_TEXT_CY, MSG_TEXT_CW, MSG_TEXT_CH );
				DSP_SetTextKage( TXT_WINDOW+i, 2 );
				DSP_SetTextCount( TXT_WINDOW+i, -1 );
				if(index==-1){
					DSP_SetTextStep( TXT_WINDOW+i, Message.mkstep );
					DSP_SetTextColor( TXT_WINDOW+i, 0 );
				}else{
					DSP_SetTextStep( TXT_WINDOW+i, -1 );
					DSP_SetTextColor( TXT_WINDOW+i, 10 );
				}
			}else{
				DSP_ResetText( TXT_WINDOW+i );
			}
		}
		Message.mcount = Message.max;
		Message.rcount = 0;
		ret = TRUE;
	}else{
		
		ret=FALSE;
	}
	return ret;
}
BOOL AVG_ControlBackMessage( void )
{
	int		rmax = AVG_EffCnt( Avg.wait*4 );
	int		i,y;
	int		sp, index;
	BOOL	ret=FALSE;

	Message.rcount++;
	if(Message.rcount>=rmax){
		for(i=0;i<5;i++){
			sp = MES_POS(-2+i);
			index = -2+Message.bcount+i;
			if(index<0){
				DSP_SetTextStr( TXT_WINDOW+i, Message.buf[sp] );
				DSP_SetTextMove( TXT_WINDOW+i, MSG_TEXT_X, MSG_TEXT_Y-MES_HIGHT(-2+i,-2) );
				if(index==-1)	DSP_SetTextColor( TXT_WINDOW+i, 0 );
				else			DSP_SetTextColor( TXT_WINDOW+i, 10 );
			}else{
				DSP_ResetText( TXT_WINDOW+i );
			}
		}
		Message.bcount--;
		Message.rcount=0;
		ret = TRUE;
		SetDragBar();
	}else{
		y = MSG_TEXT_Y+MES_HIGHT(-2,-1)*Message.rcount/rmax;
		for(i=0;i<5;i++){
			if(-2+Message.bcount+i<0){
				DSP_SetTextMove( TXT_WINDOW+i, MSG_TEXT_X, y-MES_HIGHT(-2+i,-1) );
			}
		}
	}
	return ret;
}
BOOL AVG_ForwardMessage( void )
{
	BOOL	ret = FALSE;
	int		sp;
	int		i, index;

	if( Message.bcount < 0 ){
		for(i=0;i<5;i++){
			sp = MES_POS(-2+i+1);
			index = -2+Message.bcount+i+1;
			if( index<0 ){
				DSP_SetText( TXT_WINDOW+i, LAY_WINDOW+1, SYS_FONT, 1, Message.buf[sp] );
				DSP_SetTextPos( TXT_WINDOW+i, MSG_TEXT_X, MSG_TEXT_Y-MES_HIGHT(-2+i+1,-1), MSG_TEXT_W, MSG_TEXT_H );
				DSP_SetTextPich( TXT_WINDOW+i, MSG_TEXT_PW, MSG_TEXT_PH );
				DSP_SetTextClip( TXT_WINDOW+i, MSG_TEXT_CX, MSG_TEXT_CY, MSG_TEXT_CW, MSG_TEXT_CH );
				DSP_SetTextKage( TXT_WINDOW+i, 2 );
				DSP_SetTextCount( TXT_WINDOW+i, -1 );
				if(index==-1){
					DSP_SetTextStep( TXT_WINDOW+i, Message.mkstep );
					DSP_SetTextColor( TXT_WINDOW+i, 0 );
				}else{
					DSP_SetTextStep( TXT_WINDOW+i, -1 );
					DSP_SetTextColor( TXT_WINDOW+i, 10 );
				}
			}else{
				DSP_ResetText( TXT_WINDOW+i );
			}
		}
		Message.mcount = Message.max;
		Message.rcount = 0;
		ret = TRUE;
	}else{
		
	}
	return ret;
}
BOOL AVG_ControlForwardMessage(void)
{
	int		rmax = AVG_EffCnt( Avg.wait*4 );
	int		i,y;
	int		sp, index;
	BOOL	ret=FALSE;

	Message.rcount++;
	if(Message.rcount>=rmax){
		for(i=0;i<5;i++){
			sp = MES_POS(-2+i+1);
			index = -2+Message.bcount+i+1;
			if(index<0){
				DSP_SetTextStr( TXT_WINDOW+i, Message.buf[sp] );
				DSP_SetTextMove( TXT_WINDOW+i, MSG_TEXT_X, MSG_TEXT_Y-MES_HIGHT(-2+i+1,0) );
				if(index==-1)	DSP_SetTextColor( TXT_WINDOW+i, 0 );
				else			DSP_SetTextColor( TXT_WINDOW+i, 10 );
			}else{
				DSP_ResetText( TXT_WINDOW+i );
			}
		}
		Message.bcount++;
		Message.rcount=0;
		if(Message.bcount==0){
			DSP_SetTextStr( TXT_WINDOW, Message.buf[Message.bpoint-1] );
			DSP_SetTextMove( TXT_WINDOW, MSG_TEXT_X, MSG_TEXT_Y );
			DSP_SetTextColor( TXT_WINDOW, 0 );
			DSP_SetTextStep( TXT_WINDOW, Message.mkstep );
			DSP_SetTextAlph( TXT_WINDOW, -1 );
			for(i=1;i<5;i++){
				DSP_ResetText( TXT_WINDOW+i );
			}
		}
		ret = TRUE;
		SetDragBar();
	}else{
		y = MSG_TEXT_Y-MES_HIGHT(-1,0) * Message.rcount/rmax;
		for(i=0;i<5;i++){
			index = -2+Message.bcount+i+1;
			if(index<0){
				DSP_SetTextMove( TXT_WINDOW+i, MSG_TEXT_X, y-MES_HIGHT(-2+i+1,-1) );
			}
		}
	}
	return ret;
}
BOOL AVG_SetLogMessage( int bcount )
{
	int			i, index;
	BOOL		ret=FALSE;

	if(Message.mflag){
		Message.bcount = LIM( bcount, -(Message.bmax-1), 0 );
		for(i=0;i<5;i++){
			index = -2+i+1;
			if( index+Message.bcount<0 ){
				DSP_SetText( TXT_WINDOW+i, LAY_WINDOW+1, SYS_FONT, 1, Message.buf[MES_POS(index)] );
				DSP_SetTextPos( TXT_WINDOW+i, MSG_TEXT_X, MSG_TEXT_Y-MES_HIGHT(index,-1), MSG_TEXT_W, MSG_TEXT_H );
				DSP_SetTextPich( TXT_WINDOW+i, MSG_TEXT_PW, MSG_TEXT_PH );
				DSP_SetTextClip( TXT_WINDOW+i, MSG_TEXT_CX, MSG_TEXT_CY, MSG_TEXT_CW, MSG_TEXT_CH );
				DSP_SetTextKage( TXT_WINDOW+i, 2 );
				DSP_SetTextCount( TXT_WINDOW+i, -1 );
				if(index+Message.bcount==-1){
					DSP_SetTextStep( TXT_WINDOW+i, Message.mkstep );
					DSP_SetTextColor( TXT_WINDOW+i, 0 );
					DSP_SetTextAlph( TXT_WINDOW+i, -1 );
				}else{
					DSP_SetTextStep( TXT_WINDOW+i, -1 );
					DSP_SetTextColor( TXT_WINDOW+i, 10 );
				}
			}else{
				DSP_ResetText( TXT_WINDOW+i );
			}
		}
	}
	return 1;
}

BOOL AVG_ControlDragBarMessage(void)
{
	int			mx,my;
	int			dx,dy;

	int			mvy;
	BOOL		ret=FALSE;

	MUS_GetMousePos( &mx, &my );
	MUS_GetMouseDragPos( MOUSE_LBUTTON, &dx, &dy );

	if( MUS_GetMouseButton2(MOUSE_LBUTTON) ){
		mvy = Message.bar_y-(dy-my);
		DSP_SetGraphMove( GRP_WINDOW+7, MsgCons[7].dx, LIM(mvy,369,451) );
		AVG_SetLogMessage( -(Message.bmax-1)*LIM( 451-mvy,0,82)/82 );
	}else{
		SetDragBar();
		ret = TRUE;
	}
	return ret;
}


void AVG_ControlMessage( void )
{
	int		command;
	int		click;
	int		wheel;
	int		px,py,i;
	int		mstep = Message.mstep;
	char	*str;


	if(Message.mflag ){

		if( ChipBackStruct.flag && !Message.wflag ){
			switch(Message.mstep){
				case MSG_DISP:
					if( Message.mcount>=Message.max || Avg.msg_wait==0 ){
						MessageCut = 0;
						Message.mcount = Message.max;
						str = DSP_GetTextStr(TXT_WINDOW);
						if( (Message.mcount >= TXT_GetTextCount( str, -1 ) + 8) ){
							Message.mstep = MSG_STOP;
							Message.mkstep = -1;
							DSP_SetTextCount( TXT_WINDOW, -1 );
							DSP_SetTextStep( TXT_WINDOW, -1 );
						}
					}else{
						Message.mcount += AVG_MsgCnt();
						DSP_SetTextCount( TXT_WINDOW, Message.mcount );
						DSP_SetTextStep( TXT_WINDOW, Message.mkstep );
					}
					break;
				case MSG_STOP:
					Message.wait_cnt++;
					break;
				default:
					DebugBox( NULL, "チップ劇モードでは、来ないはずのメッセージが来ています。" );
					break;
			}

		}else{
			static int count1=0;
			static int count2=-1;
			static int max1=60;
			static int max2=60;
			
			if( Message.wstep!=MWIN_NODISP ){
				switch(Message.mstep){
					case MSG_WAIT2:
					case MSG_WAIT:
						if( Avg.demo || Message.wait!=-1 || Message.wait_cnt<=1 ){
							count1=0;
							count2=-1;

							DSP_ResetGraph( GRP_WINDOW+8 );
							DSP_ResetGraph( GRP_WINDOW+9 );
						}else{
							int	fr=3;
							int	cnt1 = LIM(count1,-1,30*fr-1);
							int	cnt2 = LIM(count2,-1,30*fr-1);
							DSP_GetTextDispPos( TXT_WINDOW, &px, &py );

							if(cnt1!=-1){
								if(count1>=30*fr){
									DSP_ResetGraph( GRP_WINDOW+8 );
								}else{
									DSP_SetGraph( GRP_WINDOW+8, BMP_KEYWAIT+0, LAY_WINDOW+2, ON, CHK_NO );
									DSP_SetGraphPos( GRP_WINDOW+8, px-2, py-2, 40*(cnt1/fr%40), 0, 40, 40 );


								}
								if(count1==max1 && (count2>max2||count2==-1) ){
									count2=0;
									max2=(25*fr)+rand()%(40*fr);
								}
								count1++;
							}
							if(cnt2!=-1){
								if(count2>=30*fr){
									DSP_ResetGraph( GRP_WINDOW+9 );
								}else{
									DSP_SetGraph( GRP_WINDOW+9, BMP_KEYWAIT+1, LAY_WINDOW+2, ON, CHK_NO );
									DSP_SetGraphPos( GRP_WINDOW+9, px-2, py-2, 40*(cnt2/fr%40), 0, 40, 40 );


								}
								if(count2==max2 && (count1>max1||count1==-1) ){
									count1=0;
									max2=(25*fr)+rand()%(40*fr);
								}
								count2++;
							}
						}
						break;
					case MSG_STOP:
						count1=0;
						count2=-1;

						if( Avg.demo || Message.wait!=-1 || Message.wait_cnt<=1 ){
							DSP_ResetGraph( GRP_WINDOW+8 );
							DSP_ResetGraph( GRP_WINDOW+9 );
						}else{
							DSP_SetGraph( GRP_WINDOW+8, BMP_WINDOW, LAY_WINDOW+2, ON, CHK_256 );
							DSP_SetGraphParam( GRP_WINDOW+8, DRW_ADD );
							DSP_SetGraphPos( GRP_WINDOW+8, MsgCons[10].dx, MsgCons[10].dy, MsgCons[10].sx, MsgCons[10].sy+(GlobalCount2/6%4), MsgCons[10].w, MsgCons[10].h );
						}
						break;
					default:
						count1=0;
						count2=-1;
						DSP_ResetGraph( GRP_WINDOW+8 );
						DSP_ResetGraph( GRP_WINDOW+9 );
						break;
				}
			}
			switch(Message.mstep){
				case MSG_NODISP:
					if( GameKey.cansel || GameKey.click || GameKey.diswin ){
						Message.mstep = Message.bstep;
						AVG_OpenWindow( ON, ON );			


					}
					break;
				case MSG_DISP:
					for(i=1;i<5;i++)
						DSP_SetTextDisp(TXT_WINDOW+i,OFF);

					if( (Message.mcount>=Message.max) || AVG_GetHitKey() || MessageCut ||
						Avg.msg_wait==0 || AVG_GetMesCut() ){

						MessageCut = 0;
						Message.mcount = Message.max;
						str = DSP_GetTextStr(TXT_WINDOW);
						if( (Message.mcount >= TXT_GetTextCount( str, -1 ) + 8) ){
							if( Message.add_flag&0x01 ){
								Message.wait_flag = 0;
								Message.wait_cnt  = 0;
								Message.mstep = MSG_WAIT;
								Message.mcount = TXT_GetTextCount( str, Message.mkstep );
								DSP_SetTextCount( TXT_WINDOW, -1 );
							}else{
								Message.mstep = MSG_STOP;
								Message.mkstep = -1;
								DSP_SetTextCount( TXT_WINDOW, -1 );
								DSP_SetTextStep( TXT_WINDOW, -1 );
							}
						}else{
							Message.wait_flag = 1;
							Message.wait_cnt  = 0;
							Message.mstep = MSG_WAIT;
							Message.mcount = TXT_GetTextCount( str, Message.mkstep );
							DSP_SetTextCount( TXT_WINDOW, -1 );
						}
					}else{
						Message.mcount += AVG_MsgCnt();
	
						DSP_SetTextCount( TXT_WINDOW, Message.mcount );
						DSP_SetTextStep( TXT_WINDOW, Message.mkstep );
					}
					break;
				case MSG_LOG:

					if(Message.bcount==0){

						if(Message.bstep==MSG_LOG){
							if(Message.bstep2==MSG_WAIT){	Message.mstep = MSG_WAIT;	}
							else						{	Message.mstep = MSG_STOP;	}
							MAIN_SetScriptFlag( ON );	
						}else{
							Message.mstep = Message.bstep;
							MAIN_SetScriptFlag( ON );	
						}




						break;
					}
					
				case MSG_STOP:
				case MSG_WAIT:
				case MSG_WAIT2:
					if(Message.mstep==MSG_WAIT2){
						if(Message.mcount){
							Message.wait_cnt++;
						}
					}else{
						Message.wait_cnt++;
					}
					if(Message.mstep2==MSG_NODISP)
						MAIN_SetScriptFlag( ON );
					if( Message.mstep==MSG_WAIT ){
	
						if( AVG_WaitKeyMessage() || (Message.add_flag && !Message.wait_flag) || MessageCut ){
							if( !(Message.add_flag && !Message.wait_flag) ){
								Message.mstep  = MSG_DISP;
							}else{
								Message.mstep  = MSG_WAIT2;
							}
							Message.mcount = TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), Message.mkstep++ );
							Message.max    = TXT_GetTextCount( DSP_GetTextStr(TXT_WINDOW), Message.mkstep )+8;
							DSP_SetTextCount( TXT_WINDOW, Message.mcount );
							DSP_SetTextStep(  TXT_WINDOW, Message.mkstep );
							break;
						}
					}
	
					DSP_SetGraphSMove( GRP_WINDOW+3, MsgCons[3].sx, MsgCons[3].sy );
					if( AVG_CheckScenarioFlag() && !Avg.demo ){
						if( Avg.msg_cut ){
							DSP_SetGraphSMove( GRP_WINDOW+4, MsgCons[4].sx+128, MsgCons[4].sy );	
						}else{
							DSP_SetGraphSMove( GRP_WINDOW+4, MsgCons[4].sx+192, MsgCons[4].sy );	
						}
					}else{
						DSP_SetGraphSMove( GRP_WINDOW+4, MsgCons[4].sx,     MsgCons[4].sy );	
					}
					DSP_SetGraphSMove( GRP_WINDOW+5, MsgCons[5].sx, MsgCons[5].sy );
					DSP_SetGraphSMove( GRP_WINDOW+6, MsgCons[6].sx, MsgCons[6].sy );
					DSP_SetGraphSMove( GRP_WINDOW+7, MsgCons[7].sx, MsgCons[7].sy );
	
					command = MUS_GetMouseNoEx(-1,0);
					Debug.num[10] = command;
					click   = GameKey.click;
					wheel   = 0;

					if( !GameKey.mes_cut && !GameKey.cansel && Message.wflag && Message.wstep!=MWIN_NODISP && Message.wait_cnt>1 ){
						if( GameKey.diswin  ){	command = 0;	click = 1;	}

						wheel   = MUS_GetMouseWheel();
						if( GameKey.pup  )	wheel = 1;
						if( GameKey.pdown)	wheel = -1;

						if( GameKey.home ){
							Avg.msg_cut = OFF;
							if(Message.mstep!=MSG_LOG)
								Message.bstep = Message.mstep;
							Debug.num[10] = -Message.bmax;
							AVG_SetLogMessage( -Message.bmax );
							SetDragBar();
							Message.mstep = MSG_LOG;
							MAIN_SetScriptFlag( OFF );
							break;
						}
						if( Message.mstep==MSG_LOG && (GameKey.end || (click && command==-1)) ){
							Avg.msg_cut = OFF;


							AVG_SetLogMessage( 0 );
							SetDragBar();
							break;
						}
					}
					if( wheel ) {
						if( wheel>0 ){	command = 2; click = 1; }
						else		 {	command = 3; click = 1; }
					}
					switch( command ){
						case 0:	DSP_SetGraphSMove( GRP_WINDOW+3, MsgCons[3].sx+64, MsgCons[3].sy );	break;	
						case 1:	
							if( AVG_CheckScenarioFlag() && !Avg.msg_cut && !Avg.demo ){
								DSP_SetGraphSMove( GRP_WINDOW+4, MsgCons[4].sx+64, MsgCons[4].sy );
							}
							break;
						case 2:	
							if( Message.bcount > -(LOG_MAX-1) && Message.bflag[MES_POS(-2)] ){
								DSP_SetGraphSMove( GRP_WINDOW+5, MsgCons[5].sx+64, MsgCons[5].sy );
							}
							break;
						case 3:	
							if( Message.bcount < 0 ){
								DSP_SetGraphSMove( GRP_WINDOW+6, MsgCons[6].sx+64, MsgCons[6].sy );
							}
							break;
						case 4:	DSP_SetGraphSMove( GRP_WINDOW+7, MsgCons[7].sx+10, MsgCons[7].sy );	break;	
						case 5:	
	
							break;
					}
					if( click ){
						MainWindow.draw_flag=ON;
						switch( command ){
							case 0:	
								AVG_PlaySE3( 9104, 255 );

								Message.bstep2= Message.bstep;
								Message.bstep = Message.mstep;
								Message.mstep = MSG_NODISP;
								AVG_CloseWindow(ON);			
								MAIN_SetScriptFlag( OFF );	
								
								break;
							case 1:	
								if(AVG_CheckScenarioFlag())
									AVG_PlaySE3( 9104, 255 );

								Avg.msg_cut = ON;
								break;
							case 2:	
								if( AVG_BackMessage() ){
									AVG_PlaySE3( 9012, 140 );

									if(Message.mstep!=MSG_LOG)
										Message.bstep = Message.mstep;
									Message.mstep= MSG_DOWN;
									MAIN_SetScriptFlag( OFF );	
									DSP_SetGraphSMove( GRP_WINDOW+5, MsgCons[5].sx+128, MsgCons[5].sy );
								}
								break;
							case 3:	
								if( AVG_ForwardMessage() ){
									AVG_PlaySE3( 9012, 140 );
									
									if(Message.mstep!=MSG_LOG)
										Message.bstep = Message.mstep;
									Message.mstep= MSG_UP;
									DSP_SetGraphSMove( GRP_WINDOW+6, MsgCons[6].sx+128, MsgCons[6].sy );
								}
								break;
							case 4:	


								DSP_SetGraphSMove( GRP_WINDOW+7, MsgCons[7].sx+20, MsgCons[7].sy );
								if(Message.mstep!=MSG_LOG)
									Message.bstep = Message.mstep;
								Message.mstep = MSG_DRAG;
								break;
							case 5:	
								if(Message.bmax!=1){
									int	mx,my;

									MUS_GetMousePos( &mx, &my );
									if( my > Message.bar_y )AVG_SetLogMessage( Message.bcount+4 );
									else					AVG_SetLogMessage( Message.bcount-4 );
									SetDragBar();
									if(Message.mstep!=MSG_LOG)
										Message.bstep = Message.mstep;
									Message.mstep=MSG_LOG;
								}
								break;
						}
					}
					break;
				case MSG_UP:
					if( AVG_ControlForwardMessage() ){
						Message.mstep = MSG_LOG;
					}
					break;
				case MSG_DOWN:
					if( AVG_ControlBackMessage() ){
						Message.mstep = MSG_LOG;
					}
					break;
				case MSG_DRAG:
					if( AVG_ControlDragBarMessage() ){
						Message.mstep = MSG_LOG;
					}
					break;
			}
		}
	}
	Message.mstep2 = mstep;
}
void AVG_ControlMessageWindow( void )
{
	AVG_ControlWindow();
	AVG_ControlMessage();
}

void AVG_SetMessageWait( int wait, int wait_op )
{
	Message.wait    = wait;
	Message.wait_op = wait_op;
}

BOOL AVG_WaitKeyMessage( void )
{
	int		flag1,flag2,flag3,flag4;
	BOOL	ret;

	flag1 = AVG_GetMesCut();				
	flag2 = Message.wait==-1 && AVG_GetHitKey();				
	flag3 = Message.wait!=-1 && Message.wait<Message.wait_cnt;	

	flag4 = Avg.demo && (Avg.demo_cnt>=AVG_EffCnt3(Avg.demo_max));
	Avg.demo_cnt++;

	ret = flag1 || flag2 || flag3 || flag4;
	if( ret ) Avg.demo_cnt = 0;

	return ret;
}

BOOL AVG_WaitMessage( void )
{
	BOOL		ret=FALSE;
	int			flag;

	

	if( ChipBackStruct.flag && !Message.wflag ){
		ret=TRUE;
	}else{
		if( (Message.mstep==MSG_STOP && Message.mstep2==MSG_STOP)
		 || (Message.mstep==MSG_STOP && Message.mstep2==MSG_DISP && (Avg.msg_wait==0 || GameKey.mes_cut) )
		 || (Message.mstep==MSG_WAIT2 && Message.mstep2==MSG_WAIT2 && Message.add_flag && !Message.wait_flag) ){
			flag = AVG_WaitKeyMessage();

			if( flag || (!(Message.add_flag&0x02) && !Message.wait_flag) ){
				if(flag){
					if(Message.wait_op)
						Message.wait = -1;
				}
				Message.wait_cnt = 0;
				Message.mcount   = 0;
				ret = TRUE;
			}
		}
	}
	return ret;
}

BOOL AVG_WaitKey( void )
{
	return AVG_GetHitKey() || AVG_GetMesCut() || Avg.demo;
}



#define SELECT_WINODW_X			(150)
#define SELECT_WINODW_Y(N)		(75+(5-(N))*(SYS_FONT/2))
#define SELECT_WINODW_W			(500)
#define SELECT_WINODW_H(N)		(30+(N)*SYS_FONT)


typedef struct{
	int		flag;		
	int		ex;
	int		disp;
	int		cond;		
	int		cnt;		

	int		mnum;		

	char	mes[SELECT_MAX][SELECT_MES_MAX];	
	char	mes_sav[SELECT_MAX][SELECT_MES_MAX];	
	char	sno[SELECT_MAX][SELECT_SNO_MAX];	

	int		res;		
	int		select;		

	int		gf_no[SELECT_MAX];
	int		gf_value[SELECT_MAX];
}SELECT_WINDOW;
SELECT_WINDOW	SelectWindow;

void CreateSelectWindow( void )
{

	
/*	wsprintf( fname, "p%04d.bmp", 1 );
	DSP_LoadBmp( BMP_SELECT,   BMP_256P, fname );
*/






}

BOOL AVG_GetSelectMessageFlag( void )
{
	return SelectWindow.flag;
}

void AVG_SetSelectMessage( char *mes, int game_flag_no, int game_flag_value, int mes_wait )
{
	if(mes_wait)
		wsprintf( SelectWindow.mes[SelectWindow.mnum], "<w%d>%c%c．%s", mes_wait, '１'>>8, '１'+SelectWindow.mnum, mes );
	else
		wsprintf( SelectWindow.mes[SelectWindow.mnum], "%c%c．%s", '１'>>8, '１'+SelectWindow.mnum, mes );

	wsprintf( SelectWindow.mes_sav[SelectWindow.mnum], "%s", mes );

	SelectWindow.gf_no[SelectWindow.mnum]    = game_flag_no;
	SelectWindow.gf_value[SelectWindow.mnum] = game_flag_value;
	SelectWindow.mnum = min( SelectWindow.mnum+1, SELECT_MAX );
}

void AVG_SetSelectMessageEx( char *mes, char *sno, int game_flag_no, int game_flag_value )
{
	SelectWindow.ex = 1;
	wsprintf( SelectWindow.sno[SelectWindow.mnum], sno );
	AVG_SetSelectMessage( mes, game_flag_no, game_flag_value,0 );
}

void AVG_SetSelectWindow( void )
{

	SelectWindow.flag = 1;	
	SelectWindow.cond = 1;	
	SelectWindow.cnt  = 0;	
	SelectWindow.disp = 1;
	SelectWindow.res    =-1;
	SelectWindow.select =-1;

	CreateSelectWindow( );




	DSP_SetTextDisp( TXT_WINDOW, ON );






}
void AVG_ResetSelectWindow( void )
{
	int		i;
	for(i=0;i<SELECT_MAX;i++){
		DSP_ResetText( TXT_SELECT+i );
	}


	SelectWindow.cnt  = 0;	


	SelectWindow.res   = SelectWindow.select;
	SelectWindow.flag  = 0;
	SelectWindow.cond  = 0;
	SelectWindow.mnum  = 0;
	SelectWindow.ex    = 0;

	for(i=0;i<SELECT_MAX;i++){
		MUS_ResetMouseRect( 0, 16+i );
	}

}

void AVG_OpenSelectWindow( void )
{
	int		i;

	if(SelectWindow.flag){
		SelectWindow.disp = 1;



		for(i=0;i<SELECT_MAX;i++)
			DSP_SetTextDisp( TXT_SELECT+i, ON );
	}
}
void AVG_CloseSelectWindow( void )
{
	int		i;

	if(SelectWindow.flag){
		SelectWindow.disp = 0;



		for(i=0;i<SELECT_MAX;i++)
			DSP_SetTextDisp( TXT_SELECT+i, OFF );
	}
}

int AVG_WaitSelect( void )
{
	return SelectWindow.res;
}

char *AVG_WaitSelectEx( void )
{
	if( SelectWindow.res==-1 ){
		return NULL;
	}else{
		return SelectWindow.sno[SelectWindow.res];
	}
}

void AVG_ControlSelectWindow( void )
{
	int		i,j;
	int		cmax = AVG_EffCnt(-1);
	int		px,py;
	int		select = MUS_GetMouseNoEx( -1, 0 )-16;
	int		click = GameKey.click;
	int		h = 0, max_cnt=0;

	if( SelectWindow.flag && SelectWindow.disp && !AVG_GetLaodFlag() ){
		DSP_GetTextDispPos( TXT_WINDOW, &px, &py );

		switch(SelectWindow.cond){
			case 0:
				if(GameKey.u)	MUS_SetMousePosRect( MainWindow.hwnd, 0, (select<0)? 16 : select+16-1 );
				if(GameKey.d)	MUS_SetMousePosRect( MainWindow.hwnd, 0, (select<0)? 16 : select+16+1 );
				for(j=0;j<SelectWindow.mnum+1;j++){
					if(GameKey.num[j]){
						select = j-1;
						click = 1;
					}
				}
				for(j=0;j<SelectWindow.mnum;j++){
					DSP_SetTextColor( TXT_SELECT+j, FCT_GLAY );
				}
				if(select>=0) {
					DSP_SetTextColor( TXT_SELECT+select, FCT_NORMAL );
				}





				if( click && select>=0 ){
			
			
					SelectWindow.select = select;
					AVG_ResetSelectWindow();
					AVG_SetNovelMessageDisp(OFF);

			
					Avg.msg_cut = OFF;
			
				}
				break;
			case 1:
				MainWindow.draw_flag=1;
				SelectWindow.cnt+=AVG_MsgCnt();
				for(j=0;j<SelectWindow.mnum;j++){


					int	chk_flag = 1;
					if( chk_flag ){
						DSP_SetText( TXT_SELECT+j, LAY_WINDOW+1, SYS_FONT, ON, SelectWindow.mes[j] );

						DSP_SetTextPos( TXT_SELECT+j, 32, py + SYS_FONT*2 + 13*j+h, 20, 4 );
						DSP_SetTextKage( TXT_SELECT+j, 2 );
						DSP_SetTextBrightFlag( TXT_SELECT+j, ON );
						DSP_SetTextColor( TXT_SELECT+j, FCT_GLAY );
						DSP_SetTextAlph( TXT_SELECT+j, -1 );
						DSP_SetTextCount( TXT_SELECT+j, LIM(SelectWindow.cnt-j*4,0,1024) );
						h += DSP_GetTextDispH( TXT_SELECT+j );

						max_cnt = max( max_cnt, TXT_GetTextCount(SelectWindow.mes[j],-1)+8+j*4 );
					}
				}
				h=0;
				if(SelectWindow.cnt>=max_cnt){
					SelectWindow.cond=0;
					for(i=0;i<SelectWindow.mnum;i++){


						int	chk_flag = 1;
						if( chk_flag ){

							int	len = min( 26, TXT_GetTextCount(SelectWindow.mes[i],0) );
							DSP_SetTextAlph( TXT_SELECT+i, 256 );
							DSP_SetTextCount( TXT_SELECT+i, -1 );
							
							MUS_SetMouseRect( 0, 16+i, 32, py + SYS_FONT*2 + 13*i+h, len*SYS_FONT, DSP_GetTextDispH( TXT_SELECT+i ), ON );
							h += DSP_GetTextDispH( TXT_SELECT+i );
						}
					}
				}
				break;
			case 2:
				break;
		}
	}else if( SelectWindow.flag ){
		if( NovelMessage.step1==MSG_NEXT || NovelMessage.step1==MSG_WAIT || NovelMessage.step1==MSG_STOP ){
			AVG_OpenSelectWindow();
		}
	}
}



BOOL AVG_ConfigCheckMessage( void )
{
	return (Message.wflag && Message.wstep==MWIN_STOP) || !Message.wflag;
}

void AVG_SetSaveDataMessage( AVG_SAVE_DATA *sdata )
{
	sdata->ms_flag    = Message.mflag;		
	sdata->ms_wflag   = Message.wflag;		
	sdata->ms_cno     = Message.cno;		
	sdata->ms_wait    = Message.wait;		
	sdata->ms_wait_op = Message.wait_op;	
	strncpy( sdata->ms_str, Message.buf[ LOOP(Message.bpoint-1,256) ], 256 );	
}



BOOL AVG_ConfigCheckSelect(void)
{
	return !SelectWindow.cond;
}

void AVG_SetSaveDataSelect( AVG_SAVE_DATA *sdata )
{
	int		i;
	sdata->sw_flag = SelectWindow.flag;				
	sdata->sw_mnum = SelectWindow.mnum;				
	sdata->sw_ex   = SelectWindow.ex;

	for( i=0; i<SELECT_MAX ;i++ ){
		CopyMemory( sdata->sw_mes[i], SelectWindow.mes_sav[i], SELECT_MES_MAX );	
		CopyMemory( sdata->sw_sno[i], SelectWindow.sno[i], SELECT_SNO_MAX );	
		sdata->sw_no[i]    = SelectWindow.gf_no[i];
		sdata->sw_value[i] = SelectWindow.gf_value[i];
	}
}

void AVG_LoadWindow( void )
{
	DSP_LoadBmp( BMP_HISTORY+0, BMP_TRUE, "sys0000.tga" );
	DSP_LoadBmp( BMP_HISTORY+1, BMP_TRUE, "sys0001.tga" );
	DSP_LoadBmp( BMP_KEYWAIT+0, BMP_256P, "sys0011.tga" );
	DSP_LoadBmp( BMP_KEYWAIT+1, BMP_256P, "sys0010.tga" );
}
void AVG_InitWindow( void )
{
	AVG_LoadWindow();

	ZeroMemory( &Message, sizeof(MESSEGE_STRUCT) );
	Message.wait    = -1;

	ZeroMemory( &SelectWindow, sizeof(SELECT_WINDOW) );
	SelectWindow.res    =-1;
	SelectWindow.select =-1;
	SelectWindow.ex = 0;
}

void AVG_TermWindow( void )
{
}

