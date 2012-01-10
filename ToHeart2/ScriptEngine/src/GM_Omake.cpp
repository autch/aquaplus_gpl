
#include <math.h>

#include "mm_std.h"
#include "main.h"

#include "disp.h"
#include "draw.h"

#include "GM_Avg.h"
#include "GM_Title.h"
#include "GM_Window.h"
#include "GM_Omake.h"

#include "ctrl.h"
#include "font.h"
#include "mouse.h"
#include "escript.h"
#include "keybord.h"

static int	CgModeBmp=0;
static int	CgModeGrp=0;
static int	CgModeTxt=0;
static int	CgModeMouse=0;

static int	Game4Replay=0;


static int	CgSamMode=0;


#define CG_PAGE_MAX		16


static int	CgList[12*CG_PAGE_MAX][10];

const int	CgList2[12*CG_PAGE_MAX][10]=
{

	{  1010,  1011,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1020,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1030,  1031,  1032,  1033,  1034,  1035,  1036,     0,     0,     0},
	{  1040,  1041,  1042,     0,     0,     0,     0,     0,     0,     0},

	{  1050,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1060,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1070,  1071,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1080,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  1090,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1100,  1101,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1110,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1120,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  1130,  1131,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1140,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1150,  1151,  1152,  1153,     0,     0,     0,     0,     0,     0},
	{  1160,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  1170,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  1990,  1991,     0,     0,     0,     0,     0,     0,     0,     0},
	{  2010,  2011,  2012,  2013,  2014,  2015,     0,     0,     0,     0},
	{  2020,  2021,  2022,  2023,  2024,     0,     0,     0,     0,     0},

	{  2030,  2031,  2032,     0,     0,     0,     0,     0,     0,     0},
	{  2040,  2041,  2042,     0,     0,     0,     0,     0,     0,     0},
	{  2050,  2051,  2052,  2053,  2054,  2055,  2056,  2057,  2058,     0},
	{  2060,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  2070,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  2080,  2081,  2082,     0,     0,     0,     0,     0,     0,     0},
	{  2090,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  2100,  2101,     0,     0,     0,     0,     0,     0,     0,     0},

	{  2110,  2111,     0,     0,     0,     0,     0,     0,     0,     0},
	{  2120,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  2140,  2141,     0,     0,     0,     0,     0,     0,     0,     0},
	{  2150,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  2160,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  2170,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  2180,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  2190,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  3010,  3011,  3012,     0,     0,     0,     0,     0,     0,     0},
	{  3020,  3021,     0,     0,     0,     0,     0,     0,     0,     0},
	{  3030,  3031,  3032,     0,     0,     0,     0,     0,     0,     0},
	{  3040,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  3050,  3051,     0,     0,     0,     0,     0,     0,     0,     0},
	{  3060,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  3070,  3071,  3072,     0,     0,     0,     0,     0,     0,     0},
	{  3080,  3081,  3082,  3083,  3084,  3085,     0,     0,     0,     0},

	{  3090,  3086,     0,     0,     0,     0,     0,     0,     0,     0},
	{  3100,  3101,  3102,  3103,  3104,  3105,     0,     0,     0,     0},
	{  3110,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  3120,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  3130,  3131,  3132,  3133,  3134,  3135,     0,     0,     0,     0},
	{  3140,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  3150,  3151,     0,     0,     0,     0,     0,     0,     0,     0},
	{  4010,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  4020,  4021,  4022,     0,     0,     0,     0,     0,     0,     0},
	{  4030,  4031,  4032,  4033,     0,     0,     0,     0,     0,     0},
	{  4040,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  4050,  4051,  4052,     0,     0,     0,     0,     0,     0,     0},

	{  4060,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  4070,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  4080,  4081,     0,     0,     0,     0,     0,     0,     0,     0},
	{  4090,  4091,  4092,  4093,  4094,     0,     0,     0,     0,     0},

	{  4100,  4101,  4102,  4103,     0,     0,     0,     0,     0,     0},
	{  4110,  4111,  4112,     0,     0,     0,     0,     0,     0,     0},
	{  4120,  4121,     0,     0,     0,     0,     0,     0,     0,     0},
	{  4130,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  4140,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  4150,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  4160,  4161,  4162,     0,     0,     0,     0,     0,     0,     0},
	{  5010,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  5020,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5030,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5040,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5050,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  5060,  5061,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5070,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5080,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5090,  5091,     0,     0,     0,     0,     0,     0,     0,     0},

	{  5100,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5110,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5120,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5130,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  5140,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5150,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  5160,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7010,  7011,  7012,     0,     0,     0,     0,     0,     0,     0},

	{  7020,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7030,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7040,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7050,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7060,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7070,  7071,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7080,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7090,  7091,  7092,  7093,     0,     0,     0,     0,     0,     0},
	{  7100,  7101,  7102,     0,     0,     0,     0,     0,     0,     0},
	{  7110,  7111,  7112,     0,     0,     0,     0,     0,     0,     0},
	{  7120,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7130,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  7140,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7150,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  7160,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8010,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8020,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8030,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8040,  8041,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8050,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8060,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8070,  8071,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8080,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8090,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  8100,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8110,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8120,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8130,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8140,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  8150,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  9010,  9011,     0,     0,     0,     0,     0,     0,     0,     0},
	{  9020,  9021,  9022,     0,     0,     0,     0,     0,     0,     0},
	{  9030,  9031,  9032,     0,     0,     0,     0,     0,     0,     0},
	{  9050,  9051,     0,     0,     0,     0,     0,     0,     0,     0},
	{  9060,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  9070,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{  9080,  9081,     0,     0,     0,     0,     0,     0,     0,     0},
	{  9090,  9091,  9092,     0,     0,     0,     0,     0,     0,     0},
	{  9100,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  9110,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{  9120,  9121,  9122,  9123,  9124,     0,     0,     0,     0,     0},
	{ 10010, 10011,     0,     0,     0,     0,     0,     0,     0,     0},
	{ 15000,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{ 28010,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{ 28020, 28021, 28022,     0,     0,     0,     0,     0,     0,     0},
	{ 28030, 28031, 28032,     0,     0,     0,     0,     0,     0,     0},
	{ 28040,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{ 28050,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{ 28060, 28061, 28062, 28063, 28064, 28065, 28066, 28067,     0,     0},
	{ 28070, 28071,     0,     0,     0,     0,     0,     0,     0,     0},
	{ 28080, 28081,     0,     0,     0,     0,     0,     0,     0,     0},

	{ 28090,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{ 28100,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{ 28110, 28111,     0,     0,     0,     0,     0,     0,     0,     0},
	{ 28120,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{ 28130, 28131,     0,     0,     0,     0,     0,     0,     0,     0},
	{101000,101001,     0,     0,     0,     0,     0,     0,     0,     0},
	{101010,101011,101012,101013,101014,101015,     0,     0,     0,     0},
	{101020,101021,101022,101023,101024,     0,     0,     0,     0,     0},
	{101030,101031,101032,101033,101034,101035,     0,     0,     0,     0},

	{101040,101041,101042,101043,101044,     0,     0,     0,     0,     0},
	{102000,102001,     0,     0,     0,     0,     0,     0,     0,     0},
	{102010,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{102020,102021,102022,     0,     0,     0,     0,     0,     0,     0},

	{102030,102031,     0,     0,     0,     0,     0,     0,     0,     0},
	{102040,102041,     0,     0,     0,     0,     0,     0,     0,     0},
	{103000,103001,103002,103003,103004,103005,103006,103007,103008,     0},
	{103010,103011,103012,103013,103014,     0,     0,     0,     0,     0},

	{103020,103021,103022,103023,103024,103025,103026,103027,103028,103029},
	{103030,103031,103032,103033,     0,     0,     0,     0,     0,     0},
	{103040,103041,103042,103043,103044,     0,     0,     0,     0,     0},
	{103120,103121,103122,     0,     0,     0,     0,     0,     0,     0},


	{103103,103104,103105,103106,103107,103108,     0,     0,     0,     0},
	{103130,103131,103132,103133,     0,     0,     0,     0,     0,     0},
	{103140,103141,103142,103143,103144,     0,     0,     0,     0,     0},
	{103203,103204,103205,103206,103207,103208,     0,     0,     0,     0},

	{103220,103221,103222,     0,     0,     0,     0,     0,     0,     0},
	{103230,103231,103232,103233,     0,     0,     0,     0,     0,     0},
	{103240,103241,103242,103243,103244,     0,     0,     0,     0,     0},
	{104000,104001,104002,104003,     0,     0,     0,     0,     0,     0},

	{104010,104011,104012,104013,     0,     0,     0,     0,     0,     0},
	{104020,104021,     0,     0,     0,     0,     0,     0,     0,     0},
	{104030,104031,104032,104033,104034,104035,     0,     0,     0,     0},
	{105000,     0,     0,     0,     0,     0,     0,     0,     0,     0},

	{105010,105011,105012,105013,     0,     0,     0,     0,     0,     0},
	{105020,105021,105022,     0,     0,     0,     0,     0,     0,     0},
	{105030,105031,105032,105033,105034,105035,     0,     0,     0,     0},
	{105040,105041,     0,     0,     0,     0,     0,     0,     0,     0},

	{107000,107001,107002,107003,     0,     0,     0,     0,     0,     0},
	{107010,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{107020,107021,107022,     0,     0,     0,     0,     0,     0,     0},
	{107030,107031,     0,     0,     0,     0,     0,     0,     0,     0},

	{107040,107041,     0,     0,     0,     0,     0,     0,     0,     0},
	{108000,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{108010,108011,108012,108013,     0,     0,     0,     0,     0,     0},
	{108020,108021,108022,     0,     0,     0,     0,     0,     0,     0},

	{108030,     0,     0,     0,     0,     0,     0,     0,     0,     0},
	{109000,109001,109002,109003,109004,109005,109006,109007,     0,     0},
	{109010,109011,109012,109013,     0,     0,     0,     0,     0,     0},
	{109020,109021,109022,109023,109024,109025,109026,109027,     0,     0},

	{109030,109031,109032,     0,     0,     0,     0,     0,     0,     0},
	{128000,128001,     0,     0,     0,     0,     0,     0,     0,     0},
	{128010,128011,     0,     0,     0,     0,     0,     0,     0,     0},
	{128020,128021,128022,128023,     0,     0,     0,     0,     0,     0},
};




static char		SamAmount[12];



#define CGGP_TITLE	CgModeGrp		
#define CGGP_BACK	(CGGP_TITLE+1)	
#define CGGP_SAM	(CGGP_BACK+2)	
#define CGGP_SYS	(CGGP_SAM+24)	

#define CGGP_WORK	(CGGP_SYS+50)	
#define CGGP_INDI	(CGGP_WORK+10)	
#define CGGP_MAX	(CGGP_INDI+100)


#define CGBM_TITLE	CgModeBmp		
#define CGBM_BACK	(CGBM_TITLE+1)	
#define CGBM_SAM	(CGBM_BACK+10)	
#define CGBM_SYS	(CGBM_SAM+36)	

#define CGBM_WORK	(CGBM_SYS+20)	
#define CGBM_INDI	(CGBM_WORK+10)	
#define CGBM_MAX	(CGBM_INDI+100)




static void CreateCgList(void)
{
	int	i,j,k;
	int	cnt=0;
	int	pat=0;

	if(0){
		ZeroMemory( (char *)CgList, sizeof(short)*12*6*10 );
		for(k=0;k<2;k++){
			for(i=0;i<1000;i++){
				for(j=0;j<10;j++){
					if( AVG_GetCgFlag(k,i*10+j) ){
						CgList[cnt][pat++] = i*10+j + k*100000;
					}
				}
				if(pat)cnt++;
				pat=0;
			}
		}
	}else{
		ZeroMemory( (char *)CgList, sizeof(short)*12*6*10 );
		
		for( i=0; i<12*CG_PAGE_MAX ;i++ ){
			for(j=0;j<10;j++){
				if( AVG_GetCgFlag(CgList2[i][j]/100000,CgList2[i][j]%100000) ){
					CgList[i][pat++] = CgList2[i][j];
				}
			}
			pat=0;
		}
	}


}

void OMK_SetCgModeMouse( void )
{
	int	x,y;
	MUS_SetMouseLayer( CgModeMouse );

	for( y=0; y<3 ;y++ )
		for( x=0; x<4 ;x++ )
			MUS_SetMouseRect( CgModeMouse, y*4+x, 55+177*x, 114+128*y, 160, 120, ON );

	MUS_SetMouseRect( CgModeMouse, 12, 190,72, 130, 32 , ON );	
	MUS_SetMouseRect( CgModeMouse, 13, 482,72, 130, 32 , ON );	
	MUS_SetMouseRect( CgModeMouse, 14, 306,496,188, 32 , ON );	
}
void OMK_ResetCgModeMouse( void )
{
	MUS_ResetMouseRect_Layer( CgModeMouse );
}


void OMK_SetCgMode( int bmp_set, int grp_set, int txt_set, int mus_lay )
{
	CgModeBmp = bmp_set;
	CgModeGrp = grp_set;
	CgModeTxt = txt_set;
	CgModeMouse = mus_lay;
}
void ReleaseCgWindow( void )
{
	int	i;
	for(i=0;i<3*4;i++){
		DSP_ResetGraph( CGGP_SAM+i );
		DSP_ResetGraph( CGGP_SAM+12+i );
		DSP_ReleaseBmp( CGBM_SAM+i );
		DSP_ReleaseBmp( CGBM_SAM+12+i );
	}
}

int CheckCgWindowNum( int page_no )
{
	int			i;
	int			cnt=0;
	for(i=0;i<3*4;i++){
		if(CgList[page_no*12+i][0]){
			cnt++;
		}
	}
	return cnt;
}

static void LoadCgWindow( int page_no )
{
	int			bmp_bpp = BPP(MainWindow.draw_mode2);
	char		buf[256];
	int			i,j;

	static int	bak_page=0;

	CgSamMode=(CgSamMode)? 0: 1;

	for(i=0;i<3*4;i++){
		DSP_ResetGraph( CGGP_SAM+i );
		DSP_ReleaseBmp( CGBM_SAM+CgSamMode*12+i );
	}

	for(i=0;i<3*4;i++){
		int	cgno = CgList[page_no*12+i][0];
		SamAmount[i] = 0;

		if(CgList2[page_no*12+i][0]){

			if(cgno){
				for(j=0;j<10;j++){
					if(!CgList[page_no*12+i][j])break;
				}
				SamAmount[i] = j;
				if(cgno>=100000)wsprintf( buf, "th0%04d%d.tga", (cgno-100000)/10, CgList2[page_no*12+i][0]%10 );
				else			wsprintf( buf, "tv0%04d%d.tga", cgno/10, CgList2[page_no*12+i][0]%10 );
				DSP_LoadBmp( CGBM_SAM+CgSamMode*12+i, BMP_FULL, buf );	



				DSP_SetGraph( CGGP_SAM+i,    CGBM_SAM+CgSamMode*12+i, 32, ON, CHK_NO );
				DSP_SetGraph( CGGP_SAM+12+i, CGBM_SAM+(!CgSamMode)*12+i, 31, ON, CHK_NO );
				DSP_SetGraphFade( CGGP_SAM+i, 96 );
				DSP_SetGraphFade( CGGP_SAM+12+i, 96 );

				DSP_SetGraphMove( CGGP_SAM+i,    55+177*(i%4), 114+128*(i/4) );
				DSP_SetGraphMove( CGGP_SAM+12+i, 55+177*(i%4), 114+128*(i/4) );
			}else{
				DSP_LoadBmp( CGBM_SAM+CgSamMode*12+i, BMP_TRUE, "t1100.tga" );	

				DSP_SetGraph( CGGP_SAM+i,    CGBM_SAM+CgSamMode*12+i, 32, ON, CHK_ANTI );
				DSP_SetGraph( CGGP_SAM+12+i, CGBM_SAM+(!CgSamMode)*12+i, 31, ON, CHK_NO );

				DSP_SetGraphFade( CGGP_SAM+12+i, 96 );

				DSP_SetGraphMove( CGGP_SAM+i,    55+177*(i%4), 114+128*(i/4) );
				DSP_SetGraphMove( CGGP_SAM+12+i, 55+177*(i%4), 114+128*(i/4) );
			}
		}else{
			DSP_ResetGraph( CGGP_SAM+i );
			DSP_ReleaseBmp( CGBM_SAM+CgSamMode*12+i );
			DSP_SetGraph( CGGP_SAM+12+i, CGBM_SAM+(!CgSamMode)*12+i, 31, ON, CHK_NO );
			DSP_SetGraphFade( CGGP_SAM+12+i, 96 );
			DSP_SetGraphMove( CGGP_SAM+12+i, 55+177*(i%4), 114+128*(i/4) );
		}
	}
}

int OMK_ControlCgMode( void )
{
	enum{
		CG_INIT,
		CG_TERM,
		CG_FADEIN,
		CG_FADEOUT,
		CG_MAIN,
		CG_PAGE_FADE,
		CG_VIEW_OPEN,
		CG_VIEW_CLOSE,
		CG_VIEW_MAIN,
		CG_VIEW_PAGE,
		CG_VIEW_SCROOL,
	};
	BOOL		ret=0;
	int			bmp_bpp = BPP(MainWindow.draw_mode2);
	static int	step=CG_INIT;
	int			step_bak=step;
	static int	count=0;
	int			fmax = AVG_EffCnt(-1);
	int			rate,i;
	static int	page=0;
	static int	cg=0, cg_page=0;
	static int	bak_page=0;

	int			select2=-1;
	int			select = MUS_GetMouseNo(-1);
	int			key_flag=0;
	int			click  = GameKey.click;
	int			cansel = GameKey.cansel;
	char		buf2[256];

	static int	select_fade = 0;

	if(GameKey.l) {
		if(select<12){
			select = (select-1< 0)? 0: select-1; key_flag=1;
		}else if(select ==13 ){
			select = 12;	key_flag=1;
		}
	}
	if(GameKey.r) {
		if(select<12){
			select = (select+1>=3*4)? select: select+1;	key_flag=1;
		}else if(select ==12 ){
			select = 13;	key_flag=1;
		}
	}
	if(GameKey.u) {
		if(select==14){
			select = 11; key_flag=2;
		}else if(select==12||select==13){
		}else if(select<3){
			select = 12; key_flag=2;
		}else if(select==3){
			select = 13; key_flag=2;
		}else{
			select = (select-4< 0)? select: select-4; key_flag=2;
		}
	}
	if(GameKey.d) {
		if(select==12){
			select = 2; key_flag=2;
		}else if(select==13){
			select = 3; key_flag=2;
		}else if(select==14){
		}else if(select>=8){
			select = 14; key_flag=2;
		}else{
			select = (select+4>=3*4)? select: select+4; key_flag=2;
		}
	}

	if(key_flag){
		if(select==-1)select=0;
		if( MUS_GetMouseRectFlag(CgModeMouse,select) ){
			MUS_SetMousePosRect( MainWindow.hwnd, CgModeMouse, select );
		}
	}



	wsprintf( buf2, "%02d/%02d", page+1, CG_PAGE_MAX );
	DSP_SetGraphStrDirect( CGGP_SYS+3, buf2 );
	switch(step){
		case CG_INIT:
			ret=-1;
			MainWindow.draw_flag=1;
			CreateCgList();
			AVG_PlayBGM( 10, 0, ON, 255, 0 );
			DSP_LoadBmp( CGBM_BACK+0, BMP_FULL, "t1000.tga" );
			DSP_LoadBmp( CGBM_SYS,   BMP_TRUE, "sys0203.tga" );

			DSP_LoadBmp( CGBM_SYS+1, BMP_TRUE, "sys0230.tga" );
			DSP_LoadBmp( CGBM_SYS+2, BMP_FULL, "tH001000.tga" );
			DSP_LoadBmp( CGBM_SYS+3,  BMP_TRUE, "sys0111.tga" );
			DSP_LoadBmp( CGBM_SYS+4,  BMP_TRUE, "sys0203.tga" );

			DSP_SetGraph( CGGP_BACK,  CGBM_BACK, 30, ON, CHK_NO );	

			DSP_SetGraph( CGGP_SYS+0, CGBM_SYS+4, 31, OFF, CHK_ANTI );
			DSP_SetGraph( CGGP_SYS+1, CGBM_SYS+4, 31, OFF, CHK_ANTI );
			DSP_SetGraph( CGGP_SYS+2, CGBM_SYS+3, 31, OFF, CHK_ANTI );
			DSP_SetGraphPos( CGGP_SYS+0, 190,72,      0,    64*0, 130, 32 );	
			DSP_SetGraphPos( CGGP_SYS+1, 482,72,      0, 32+64*0, 130, 32 );	
			DSP_SetGraphPos( CGGP_SYS+2, 306,496, 188*0,       0, 188, 32 );	

			wsprintf( buf2, "%02d/%02d", page+1, CG_PAGE_MAX );
			DSP_SetGraphStr( CGGP_SYS+3, CGBM_SYS+1, 31, ON, CHK_ANTI, buf2 );
			DSP_SetGraphMove( CGGP_SYS+3, 364, 78 );






			LoadCgWindow( page );		

			step = CG_FADEIN;
		case CG_FADEIN:
			ret=-1;
			if( count<fmax ){
				rate = count*256/fmax;
				DSP_SetGraphParam( CGGP_BACK, DRW_BLD(rate) );
				DSP_SetGraphParam( CGGP_SYS+3, DRW_BLD(rate) );

				for(i=0;i<3*4;i++){
					DSP_SetGraphParam( CGGP_SAM+i, DRW_BLD(rate) );
					if(CgList2[page*12+i][0] ){
						if(CgList[page*12+i][0]){
							DSP_SetGraphFade( CGGP_SAM+i, 96 );
						}
					}

				}
			}else{
				DSP_SetGraphParam( CGGP_BACK, DRW_NML );
				DSP_SetGraphParam( CGGP_SYS+3, DRW_NML );
				for(i=0;i<3;i++){
					DSP_SetGraphDisp( CGGP_SYS+i, ON );
				}
				for(i=0;i<3*4;i++){
					DSP_SetGraphParam( CGGP_SAM+i, DRW_NML );
				}
				OMK_SetCgModeMouse();
				step = CG_MAIN;
			}
			break;
		case CG_TERM:
			ReleaseCgWindow();	

			for(i=0;i<5;i++)
				DSP_ReleaseBmp( CGBM_BACK+i );
			DSP_ReleaseBmp( CGBM_SYS+0 );

			for(i=0;i<2;i++)
				DSP_ResetGraph( CGGP_BACK+i );
			for(i=0;i<12;i++)
				DSP_ResetGraph( CGGP_SYS+i );

			OMK_ResetCgModeMouse();

			step  = CG_INIT;
			count = 0;
			ret=1;
			break;
		case CG_FADEOUT:
			ret=-1;
			if( count<fmax ){
				rate = 256-count*256/fmax;
				DSP_SetGraphParam( CGGP_BACK, DRW_BLD(rate) );
				DSP_SetGraphParam( CGGP_SYS, DRW_BLD(rate) );
				DSP_SetGraphParam( CGGP_SYS+1, DRW_BLD(rate) );
				DSP_SetGraphParam( CGGP_SYS+2, DRW_BLD(rate) );
				DSP_SetGraphParam( CGGP_SYS+3, DRW_BLD(rate) );

				for(i=0;i<3*4;i++){
					DSP_SetGraphParam( CGGP_SAM+i, DRW_BLD(rate) );
				}
			}else{
				step = CG_TERM;
			}
			break;
		case CG_PAGE_FADE:

			fmax = AVG_EffCnt(-1)/2;
			if(count==0){
				LoadCgWindow( page );

			}

			if(count<fmax){
				rate = count*256/fmax;
				for(i=0;i<12;i++){
					DSP_SetGraphParam( CGGP_SAM+i,    DRW_BLD(rate) );
					DSP_SetGraphParam( CGGP_SAM+12+i, DRW_BLD(256-rate) );


				}

			}else{
				for(i=0;i<12;i++){
					DSP_SetGraphParam( CGGP_SAM+i, DRW_NML );
					DSP_SetGraphDisp( CGGP_SAM+12+i, OFF );
				}


				step = CG_MAIN;
			}
			break;
		case CG_VIEW_CLOSE:

			fmax = AVG_EffCnt(-1);
			if(count<fmax){
				rate = 256-count*256/fmax;
				DSP_SetGraphParam( CGGP_SYS+10, DRW_BLD(rate) );
			}else{
				DSP_ResetGraph( CGGP_SYS+10 );
				DSP_ReleaseBmp( CGBM_SYS+10 );
				step = CG_MAIN;
			}
			break;
		case CG_VIEW_OPEN:

			fmax = AVG_EffCnt(-1);
			if(count==0){
				int		cgno = CgList[page*12+cg][0];
				char	buf[256];

				if(cgno>=100000)	wsprintf( buf, "h0%05d.tga", cgno%100000 );

				else				wsprintf( buf, "v0%05d.tga", cgno%100000 );
				DSP_LoadBmp( CGBM_SYS+10, bmp_bpp, buf );
				DSP_SetGraph( CGGP_SYS+10, CGBM_SYS+10, 33, ON, CHK_NO );
				MainWindow.draw_flag=ON;
			}

			if(count<fmax){
				rate = count*256/fmax;
				DSP_SetGraphParam( CGGP_SYS+10, DRW_BLD(rate) );
				rate = 256-rate;
				rate = rate*rate/256;
				rate = 256-rate;

				int	sx,sy;
				DSP_GetGraphBmpSize( CGGP_SYS+10, &sx, &sy );
				if(sy==600){
					DSP_SetGraphZoom( CGGP_SYS+10, (54+177*(cg%4))*(256-rate)/256, (77+130*(cg/4))*(256-rate)/256,
												160*(256-rate)/256+800*rate/256, 120*(256-rate)/256+600*rate/256 );
				}else{
					DSP_SetGraphPosZoom( CGGP_SYS+10, (54+177*(cg%4))*(256-rate)/256, (77+130*(cg/4))*(256-rate)/256,
													160*(256-rate)/256+800*rate/256, 120*(256-rate)/256+600*rate/256, 0, 600, 800, 600 );
				}
			}else{
				DSP_SetGraphParam( CGGP_SYS+10, DRW_NML );
				int	sx,sy;
				DSP_GetGraphBmpSize( CGGP_SYS+10, &sx, &sy );
				if(sy==600){
					DSP_SetGraphPos( CGGP_SYS+10, 0, 0, 0,0, 800, 600 );
				}else{
					DSP_SetGraphPos( CGGP_SYS+10, 0, 0, 0,600, 800, 600 );
				}
				DSP_SetGraphDisp( CGGP_BACK, OFF );
				step = CG_VIEW_MAIN;
			}
			break;
		case CG_VIEW_PAGE:

			if(count==0){
				int		cgno = CgList[page*12+cg][cg_page];
				char	buf[256];

				DSP_CopyBmp( CGBM_SYS+11, CGBM_SYS+10 );

				if(cgno>=100000)	wsprintf( buf, "h0%05d.tga", cgno%100000 );
				else{
					if(cgno<0) cgno=0;
					wsprintf( buf, "v0%05d.tga", cgno%100000 );
				}
				DSP_LoadBmp( CGBM_SYS+10, bmp_bpp, buf );
				DSP_SetGraph( CGGP_SYS+10, CGBM_SYS+10, 33, ON, CHK_NO );
				DSP_SetGraph( CGGP_SYS+11, CGBM_SYS+11, 34, ON, CHK_NO );
				MainWindow.draw_flag=ON;
			}
			fmax = AVG_EffCnt(-1);
			if(count<fmax){
				rate = 256-count*256/fmax;
				DSP_SetGraphParam( CGGP_SYS+11, DRW_BLD(rate) );
			}else{
				DSP_ResetGraph( CGGP_SYS+11 );
				DSP_ReleaseBmp( CGBM_SYS+11 );
				step = CG_VIEW_MAIN;
			}
			break;
		case CG_VIEW_SCROOL:

			fmax = AVG_EffCnt2(90);

			if(count<fmax){
				rate = count*600/fmax;
				DSP_SetGraphSMove( CGGP_SYS+10, 0, 600-rate );
			}else{
				step = CG_VIEW_MAIN;
			}
			break;
		case CG_VIEW_MAIN:
			if(click){
				int	sx,sy;
				DSP_GetGraphBmpSize( CGGP_SYS+10, &sx, &sy );
				if(sy==600){
					if( SamAmount[cg]>cg_page+1 ){
						AVG_PlaySE3( 9104, 255 );
						cg_page++;
						step = CG_VIEW_PAGE;
					}else{
						AVG_PlaySE3( 9107, 255 );
						step = CG_VIEW_CLOSE;
						DSP_SetGraphDisp( CGGP_BACK, ON );
					}
				}else{
					if( cg_page==0 ){
						AVG_PlaySE3( 9104, 255 );
						cg_page++;
						step = CG_VIEW_SCROOL;
					}else{
						AVG_PlaySE3( 9107, 255 );
						step = CG_VIEW_CLOSE;
						DSP_SetGraphDisp( CGGP_BACK, ON );
					}
				}
			}
			if(cansel){
				AVG_PlaySE3( 9107, 255 );
				step = CG_VIEW_CLOSE;
				DSP_SetGraphDisp( CGGP_BACK, ON );
			}
			break;
		case CG_MAIN:


			for(i=0;i<12;i++){
				if(CgList2[page*12+i][0] ){
					if(CgList[page*12+i][0]){
						DSP_SetGraphFade( CGGP_SAM+i, 96 );

					}
				}
			}






			DSP_SetGraphSMove( CGGP_SYS+0,  0, 0 );
			DSP_SetGraphSMove( CGGP_SYS+1,  0, 32 );
			DSP_SetGraphSMove( CGGP_SYS+2,  188*0, 0 );

			
			if(GameKey.pdown) {
				AVG_PlaySE3( 9104, 255 );
				page = (page+1)%CG_PAGE_MAX;
				step = CG_PAGE_FADE;
			}
			if(GameKey.pup) {
				AVG_PlaySE3( 9104, 255 );
				page = (page+CG_PAGE_MAX-1)%CG_PAGE_MAX;
				step = CG_PAGE_FADE;
			}
			if(select!=-1){
				if(select<12){
					DSP_SetGraphFade( CGGP_SAM+select, 128 );
					DSP_SetGraphParam( CGGP_SAM+select, DRW_NML );
				}

				int	fade = 256-STD_LimitLoop( select_fade, 128 );
				select_fade+=8;
				if(select==12 ){

					DSP_SetGraphSMove( CGGP_SYS,  0, 64 );
				}
				if(select==13 ){

					DSP_SetGraphSMove( CGGP_SYS+1, 0, 32+64 );
				}
				if(select==14) {

					DSP_SetGraphSMove( CGGP_SYS+2, 188, 0 );
				}
				if( click ){
					switch(select){
					default:
						if(SamAmount[select]){
							AVG_PlaySE3( 9104, 255 );
							cg_page=0;
							cg=select;
							step = CG_VIEW_OPEN;
						}
						break;
					case 12:
						page = (page+CG_PAGE_MAX-1)%CG_PAGE_MAX;
						DSP_SetGraphSMove( CGGP_SYS+0, 0, 64*2 );
						AVG_PlaySE3( 9104, 255 );
						step = CG_PAGE_FADE;
						break;
					case 13:
						page = (page+1)%CG_PAGE_MAX;
						DSP_SetGraphSMove( CGGP_SYS+1, 0, 32+64*2 );
						AVG_PlaySE3( 9104, 255 );
						step = CG_PAGE_FADE;
						break;
					case 14:
						DSP_SetGraphSMove( CGGP_SYS+2, 188*2, 0 );
						AVG_StopBGM( 60 );
						AVG_PlaySE3( 9104, 255 );
						step = CG_FADEOUT;
						break;
					}
				}
			}
			if( cansel ){
				AVG_StopBGM( 60 );
				AVG_PlaySE3( 9107, 255 );
				step = CG_FADEOUT;
			}
			break;
	}
	if(step_bak==step)	count++;
	else				count=0;
	return ret;
}




#define REPLAY_PAGE_MAX		1


static int	EroTbl[12]={
	 1000,
	 2010,
	 3000,
	 4000,
			 
	 5000,
	 7010,
	 8000,
	 9000,
			 
	28000,
	0,
	0,
	0,
};
static int	EroTbl2[12]={
	10,
	20,
	30,
	40,

	50,
	70,
	80,
	90,

	110,
	0,	
	0,
	0,
};

void OMK_SetReplayModeMouse( void )
{
	int	x,y;
	MUS_SetMouseLayer( CgModeMouse );

	for( y=0; y<3 ;y++ )
		for( x=0; x<4 ;x++ )
			MUS_SetMouseRect( CgModeMouse, y*4+x, 55+177*x, 114+128*y, 160, 120, ON );

	MUS_SetMouseRect( CgModeMouse, 14, 306,496,188, 32 , ON );	
}
void OMK_ResetReplayModeMouse( void )
{
	MUS_ResetMouseRect_Layer( CgModeMouse );
}

void OMK_SetReplayMode( int bmp_set, int grp_set, int txt_set, int mus_lay, int flag )
{
	CgModeBmp = bmp_set;
	CgModeGrp = grp_set;
	CgModeTxt = txt_set;
	CgModeMouse = mus_lay;

	OMK_SetReplayModeMouse();

	Game4Replay = flag;
	if(flag){
		DSP_GetDispBmp( CGBM_TITLE, MainWindow.draw_mode2 );
		DSP_SetGraph( CGGP_TITLE, CGBM_TITLE, 0, ON, CHK_NO );
	}
}

void LoadReplayWindow( void )
{
	int			bmp_bpp = BPP(MainWindow.draw_mode2);
	char		buf[256];
	int			i;

	for(i=0;i<3*4;i++){
		DSP_ResetGraph( CGGP_SAM+i );
		DSP_ReleaseBmp( CGBM_SAM+i );
	}



	for(i=0;i<9;i++){
		int	check = 0;

		switch(i){
		case 0:	case 1:	case 2:	case 3: case 4:	check = AVG_GetReplayFlag(i+1);	break;
		case 5:	case 6:	case 7:	check = AVG_GetReplayFlag(i+2);	break;
		case 8:	check = AVG_GetReplayFlag(i+3);	break;
		}
		if(check && EroTbl2[i]){

			wsprintf( buf, "th0%05d.tga", EroTbl[i] );



			DSP_LoadBmp( CGBM_SAM+i, BMP_FULL, buf );	



			DSP_SetGraph( CGGP_SAM+i,    CGBM_SAM+i, 32, ON, CHK_NO );
			DSP_SetGraphFade( CGGP_SAM+i, 96 );
			DSP_SetGraphMove( CGGP_SAM+i, 55+177*(i%4), 114+128*(i/4) );
		}else{
			if(EroTbl2[i]){
				wsprintf( buf, "t1100.tga" );
				DSP_LoadBmp( CGBM_SAM+i, BMP_TRUE, buf );	
				DSP_SetGraph( CGGP_SAM+i,    CGBM_SAM+i, 32, ON, CHK_ANTI );
				DSP_SetGraphMove( CGGP_SAM+i, 55+177*(i%4), 114+128*(i/4) );
			}
		}
	}
}

int OMK_ControlReplayMode( void )
{
	enum{
		REPLAY_INIT,
		REPLAY_TERM,
		REPLAY_FADEIN,
		REPLAY_FADEOUT,
		REPLAY_MAIN,
	};
	BOOL		ret=0;
	int			bmp_bpp = BPP(MainWindow.draw_mode2);
	static int	step=REPLAY_INIT;
	int			step_bak=step;
	static int	count=0;
	int			fmax = AVG_EffCnt(-2);
	int			rate,i;

	int			select = MUS_GetMouseNo(-1);
	int			key_flag=0;
	int			click  = GameKey.click;
	int			cansel = GameKey.cansel;

	static int	select_fade = 0;

	if(GameKey.l) {	select = (select-1<0)? 0: select-1; key_flag=1;}
	if(GameKey.r) {	select = (select+1>7)? select: select+1;	key_flag=1; }
	if(GameKey.u) {
		if(select<=7){
			select = (select-4< 0)? select: select-4; key_flag=2;
		}else if(select==14){
			select = 7; key_flag=2;
		}
	}
	if(GameKey.d) {
		if(select==7 || select==6 || select==5 || select==4){
			select = 14; key_flag=2;
		}else{
			select = (select+4>=9)? select: select+4; key_flag=2;
		}
	}

	if(key_flag){
		if(select==-1)select=0;
		if( MUS_GetMouseRectFlag(CgModeMouse,select) ){
			MUS_SetMousePosRect( MainWindow.hwnd, CgModeMouse, select );
		}
	}

	switch(step){
		case REPLAY_INIT:
			MainWindow.draw_flag=1;
			LoadReplayWindow();


			DSP_LoadBmp( CGBM_BACK,   bmp_bpp, "t3000.tga" );

			DSP_SetGraph( CGGP_BACK,  CGBM_BACK, 30, ON, CHK_NO );	

			DSP_LoadBmp( CGBM_SYS,  BMP_TRUE, "sys0111.tga" );
			DSP_SetGraph( CGGP_SYS, CGBM_SYS, 32, ON, CHK_ANTI );
			DSP_SetGraphPos( CGGP_SYS, 306,496, 188*0,       0, 188, 32 );	

			step = REPLAY_FADEIN;

		case REPLAY_FADEIN:
			ret=-1;
			if( count<fmax && !Avg.omake ){
				rate = count*256/fmax;
				DSP_SetGraphParam( CGGP_BACK, DRW_BLD(rate) );
				DSP_SetGraphParam( CGGP_SYS, DRW_BLD(rate) );

				for(i=0;i<3*4;i++){
					int	check = 0;

					switch(i){
					case 0:	case 1:	case 2:	case 3: case 4:	check = AVG_GetReplayFlag(i+1);	break;
					case 5:	case 6:	case 7:	check = AVG_GetReplayFlag(i+2);	break;
					case 8:	check = AVG_GetReplayFlag(i+3);	break;
					}
					if(check && EroTbl2[i]){
						DSP_SetGraphFade( CGGP_SAM+i, 96 );
					}
					DSP_SetGraphParam( CGGP_SAM+i, DRW_BLD(rate) );
				}
			}else{

				DSP_SetGraphParam( CGGP_BACK, DRW_NML );
				DSP_SetGraphParam( CGGP_SYS, DRW_NML );
				for(i=0;i<3*4;i++){
					DSP_SetGraphParam( CGGP_SAM+i, DRW_NML );
				}
				OMK_SetReplayModeMouse();
				step = REPLAY_MAIN;
				
				SetMenuStr( MainWindow.hwnd, ID_TITLE, "タイトルへ戻る" );
				EnableMenuItem( GetMenu( MainWindow.hwnd ), ID_TITLE, MF_GRAYED );
				Avg.omake=0;
			}
			break;
		case REPLAY_MAIN:


			for(i=0;i<12;i++){
				int	check = 0;

				switch(i){
				case 0:	case 1:	case 2:	case 3: case 4:	check = AVG_GetReplayFlag(i+1);	break;
				case 5:	case 6:	case 7:	check = AVG_GetReplayFlag(i+2);	break;
				case 8:	check = AVG_GetReplayFlag(i+3);	break;
				}
				if(check && EroTbl2[i]){
					DSP_SetGraphFade( CGGP_SAM+i, 96 );
				}
			}

			DSP_SetGraphSMove( CGGP_SYS, 0,0 );
			if(select!=-1){
				if(select<12){
					DSP_SetGraphFade( CGGP_SAM+select, 128 );
					DSP_SetGraphParam( CGGP_SAM+select, DRW_NML );
				}

				if(select==14) {
					DSP_SetGraphSMove( CGGP_SYS, 188, 0 );
				}
				if( click ){
					int	check=0;
					switch(select){
					default:
						switch(select){
						case 0:	case 1:	case 2:	case 3: case 4:	check = AVG_GetReplayFlag(select+1);	break;
						case 5:	case 6:	case 7:	check = AVG_GetReplayFlag(select+2);	break;
						case 8:	check = AVG_GetReplayFlag(select+3);	break;
						}
						if( check ){

							AVG_PlaySE3( 9104, 255 );


							ReleaseCgWindow();	

							DSP_ReleaseBmp( CGBM_BACK );
							DSP_ReleaseBmp( CGBM_SYS+1 );
							DSP_ReleaseBmp( CGBM_SYS+2 );

							DSP_ResetGraph( CGGP_BACK );
							DSP_ResetGraph( CGGP_SYS+2 );
							DSP_ResetGraph( CGGP_SYS+3 );

							AVG_StopBGM( 30 );

							OMK_ResetReplayModeMouse();
							step = REPLAY_INIT;
							ret = EroTbl2[select]+2;
							SetMenuStr( MainWindow.hwnd, ID_TITLE, "リプレイ選択画面に戻る" );
						}
						break;
					case 14:
						DSP_SetGraphSMove( CGGP_SYS, 188*2, 0 );
						DSP_SetGraphParam( CGGP_SYS, DRW_NML );

						AVG_StopBGM( 60 );
						AVG_PlaySE3( 9104, 255 );
						step = REPLAY_FADEOUT;
						break;
					}
				}
			}
			if( cansel ){
				AVG_StopBGM( 60 );
				AVG_PlaySE3( 9107, 255 );
				step = REPLAY_FADEOUT;
			}
			break;
		case REPLAY_FADEOUT:
			ret=-1;
			DSP_SetGraphDisp( CGGP_SYS, OFF );

			if( count<fmax ){
				rate = 256-count*256/fmax;
				DSP_SetGraphParam( CGGP_BACK, DRW_BLD(rate) );
				DSP_ResetGraph( CGGP_SYS );

				for(i=0;i<3*4;i++){
					DSP_SetGraphParam( CGGP_SAM+i, DRW_BLD(rate) );
				}
			}else{
				step = REPLAY_TERM;
			}
			break;
		case REPLAY_TERM:
			ReleaseCgWindow();	

			DSP_ReleaseBmp( CGBM_BACK );
			DSP_ReleaseBmp( CGBM_SYS );
			DSP_ReleaseBmp( CGBM_SYS+1 );
			DSP_ReleaseBmp( CGBM_SYS+2 );

			DSP_ResetGraph( CGGP_BACK );
			DSP_ResetGraph( CGGP_SYS+0 );
			DSP_ResetGraph( CGGP_SYS+1 );
			DSP_ResetGraph( CGGP_SYS+2 );
			DSP_ResetGraph( CGGP_SYS+3 );

			OMK_ResetReplayModeMouse();

			step  = REPLAY_INIT;
			count = 0;
			ret=1;
			break;
	}
	if(step_bak==step)	count++;
	else				count=0;
	return ret;

}


int	MusTable[MUSIC_MODE_MAX]={
	 0,
	10,
	29,
	11,
	12,
	13,
	14,
	30,
	27,
	 1,

	 2,
	 4,
	 3,
	 5,
	 6,
	 8,
	 7,
	 9,
	18,
	37,

	38,
	41,
	42,
	39,
	40,
	15,
	16,
	17,
	19,
	20,

	22,
	32,
	21,
	23,
	26,
	31,
	25,
	24,
	28,
	50,
};
int	MusArtistTable[MUSIC_MODE_MAX][2]={
	{ 3, 5 },
	{ 3, 3 },
	{ 4, 4 },
	{ 2, 2 },
	{ 1, 1 },
	{ 2, 2 },
	{ 1, 1 },
	{ 0, 0 },
	{ 2, 2 },
	{ 2, 2 },

	{ 0, 0 },
	{ 3, 3 },
	{ 0, 0 },
	{ 4, 4 },
	{ 4, 4 },
	{ 2, 2 },
	{ 1, 1 },
	{ 1, 1 },
	{ 2, 2 },
	{ 6, 4 },

	{ 6, 4 },
	{ 1, 1 },
	{ 0, 0 },
	{ 5, 5 },
	{ 0, 0 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 2, 2 },
	{ 2, 2 },

	{ 1, 1 },
	{ 4, 4 },
	{ 2, 2 },
	{ 1, 1 },
	{ 2, 2 },
	{ 3, 3 },
	{ 1, 1 },
	{ 1, 1 },
	{ 3, 3 },
	{ 2, 5 }
};


void OMK_SetSoundModeMouse( void )
{
	int	x,y;
	MUS_SetMouseLayer( CgModeMouse );

	for( x=0; x<4 ;x++ )
		for( y=0; y<10 ;y++ )
			MUS_SetMouseRect( CgModeMouse, x*10+y, 20+x*192, 160+y*32, 184, 28, ON );

	MUS_SetMouseRect( CgModeMouse, 40, 306, 496, 188, 32, ON );
}



void OMK_SetSoundMode( int bmp_set, int grp_set, int txt_set, int mus_lay )
{
	CgModeBmp   = bmp_set;
	CgModeGrp   = grp_set;
	CgModeTxt   = txt_set;
	CgModeMouse = mus_lay;

	OMK_SetSoundModeMouse();
}


int OMK_ControlSoundMode( void )
{
	enum{
		SOUND_INIT,
		SOUND_TERM,
		SOUND_FADEIN,
		SOUND_FADEOUT,
		SOUND_MAIN,
	};
	BOOL		ret=0;
	int			bmp_bpp = BPP(MainWindow.draw_mode2);
	static int	step=SOUND_INIT;
	int			step_bak=step;
	static int	count=0;
	int			fmax = AVG_EffCnt(-2);
	int			rate,i, flag ,x,y;

	static int	mus_cnt=0;
	static int	bak_mus=-1;
	static int	sel_mus=-1;
	int			select = MUS_GetMouseNo(-1);
	int			key_flag=0;
	int			click  = GameKey.click;
	int			cansel = GameKey.cansel;

	static int	select_fade = 0;


	if(GameKey.u) {	select = (select-1< 0)? 0: select-1; key_flag=1;}
	if(GameKey.d) {	select = (select+1>=40)? select: select+1;	key_flag=1; }
	if(GameKey.l) {
		if(select<40){
			select = (select-10< 0)? select: select-10; key_flag=2;
		}else if(select==40){
			select = 39; key_flag=2;
		}
	}
	if(GameKey.r) {

		select = (select+10>=40)? 40 : select+10; key_flag=2;
	}

	if(key_flag){
		if(select==-1)select=0;
		if( MUS_GetMouseRectFlag(CgModeMouse,select) ){
			MUS_SetMousePosRect( MainWindow.hwnd, CgModeMouse, select );
		}
	}


	switch(step){
		case SOUND_INIT:
			AVG_StopBGM( 30 );
			ret=-1;
			MainWindow.draw_flag=1;
			bak_mus = -1;
			sel_mus = -1;
			flag=0;
			for(i=0;i<40;i++){
				if( !AVG_GetGameFlag(i+128) ){
					flag=1;
				}
			}
			DSP_LoadBmp( CGBM_BACK+0,   BMP_FULL, "t2000.tga" );
			DSP_LoadBmp( CGBM_BACK+1,   BMP_FULL, "t2001.tga" );

			DSP_LoadBmp( CGBM_SYS+0,   BMP_TRUE, "t2010.tga" );

			DSP_LoadBmp( CGBM_SYS+1,   BMP_TRUE, "t2020.tga" );
			DSP_LoadBmp( CGBM_SYS+2,   BMP_TRUE, "t2021.tga" );
			DSP_LoadBmp( CGBM_SYS+3,   BMP_TRUE, "t2100.tga" );

			DSP_LoadBmp( CGBM_SYS+4,  BMP_TRUE, "sys0111.tga" );


			DSP_SetGraph( CGGP_BACK,    CGBM_BACK,   30, OFF, CHK_NO );	
			i=0;
			for( x=0; x<4 ;x++ ){
				for( y=0; y<10 ;y++,i++ ){
					if( AVG_GetGameFlag(i+128) ){
						DSP_SetGraph( CGGP_SYS+i, CGBM_SYS+0, 32, OFF, CHK_ANTI );
						DSP_SetGraphPos( CGGP_SYS+i, 20+20+x*192, 160+4+y*32, x*160, y*20, 160, 20 );
					}
				}
			}

			DSP_SetGraph( CGGP_SYS+40, CGBM_SYS+4, 32, ON, CHK_ANTI );
			DSP_SetGraphPos( CGGP_SYS+40, 306, 496, 188*0, 0, 188, 32 );

			DSP_SetGraph( CGGP_SYS+41, CGBM_BACK+1, 31, OFF, CHK_NO );
			DSP_SetGraph( CGGP_SYS+42, CGBM_SYS+3,  32, OFF, CHK_ANTI );

			DSP_SetGraph( CGGP_SYS+43, CGBM_SYS+1, 31, OFF, CHK_ANTI );
			DSP_SetGraphPos( CGGP_SYS+43, 281, 86, 0, 0, 282, 24 );

			DSP_SetGraph( CGGP_SYS+44, CGBM_SYS+2, 32, OFF, CHK_ANTI );
			DSP_SetGraphPos( CGGP_SYS+44, 291, 122, 0, 0, 282, 24 );

			DSP_SetGraph( CGGP_SYS+45, CGBM_SYS+2, 32, OFF, CHK_ANTI );
			DSP_SetGraphPos( CGGP_SYS+45, 440, 122, 0, 0, 282, 24 );

			step = SOUND_FADEIN;

		case SOUND_FADEIN:
			if( count<fmax ){
				rate = count*256/fmax;
				DSP_SetGraphDisp( CGGP_BACK, ON );
				DSP_SetGraphParam( CGGP_BACK, DRW_BLD(rate) );

				for( i=0; i<40 ; i++ ){
					if( AVG_GetGameFlag(i+128) ){
						DSP_SetGraphDisp( CGGP_SYS+i, ON );
						DSP_SetGraphParam( CGGP_SYS+i, DRW_BLD(rate) );
					}
				}
				DSP_SetGraphDisp( CGGP_SYS+40, ON );
				DSP_SetGraphParam( CGGP_SYS+40, DRW_BLD(rate) );
			}else{
				DSP_SetGraphDisp( CGGP_BACK, ON );
				DSP_SetGraphParam( CGGP_BACK, DRW_NML );
				for( i=0; i<40 ; i++ ){
					if( AVG_GetGameFlag(i+128) ){
						DSP_SetGraphDisp( CGGP_SYS+i, ON );
						DSP_SetGraphParam( CGGP_SYS+i, DRW_NML );
					}
				}
				DSP_SetGraphDisp( CGGP_SYS+40, ON );
				DSP_SetGraphParam( CGGP_SYS+40, DRW_NML );

				step = SOUND_MAIN;
				mus_cnt=0;
				sel_mus=-1;
				bak_mus=-1;
			}
			break;
		case SOUND_FADEOUT:
			ret=-1;
			DSP_ResetGraph( CGGP_SYS+41);
			DSP_ResetGraph( CGGP_SYS+42);
			DSP_ResetGraph( CGGP_SYS+43);
			DSP_ResetGraph( CGGP_SYS+44);
			DSP_ResetGraph( CGGP_SYS+45);

			if( count<fmax ){
				rate = 256-count*256/fmax;

				DSP_SetGraphDisp( CGGP_BACK, ON );
				DSP_SetGraphParam( CGGP_BACK, DRW_BLD(rate) );

				for( i=0; i<40 ; i++ ){
					if( AVG_GetGameFlag(i+128) ){
						DSP_SetGraphDisp( CGGP_SYS+i, ON );
						DSP_SetGraphParam( CGGP_SYS+i, DRW_BLD(rate) );
					}
				}
				DSP_SetGraphDisp( CGGP_SYS+40, ON );
				DSP_SetGraphParam( CGGP_SYS+40, DRW_BLD(rate) );

			}else{
				step = SOUND_TERM;
			}
			break;
		case SOUND_TERM:
			ret=1;
			DSP_ReleaseBmp( CGBM_BACK+0);
			DSP_ReleaseBmp( CGBM_BACK+1 );

			DSP_ReleaseBmp( CGBM_SYS+0 );
			DSP_ReleaseBmp( CGBM_SYS+1 );
			DSP_ReleaseBmp( CGBM_SYS+2 );
			DSP_ReleaseBmp( CGBM_SYS+3 );
			DSP_ReleaseBmp( CGBM_SYS+4 );

			DSP_ResetGraph( CGGP_BACK );	
			for(i=0;i<43;i++){
				DSP_ResetGraph( CGGP_SYS+i );
			}


			OMK_ResetReplayModeMouse();

			step = SOUND_INIT;
			break;
		case SOUND_MAIN:

			if( !AVG_GetGameFlag(select+128) && select!=40 ){
				select=-1;
			}
			for( i=0; i<40 ; i++ ){
				if( AVG_GetGameFlag(i+128) ){
					DSP_SetGraphFade( CGGP_SYS+i, 128 );
				}
			}
			if(sel_mus!=-1){
				i=sel_mus;
				x=i/10;
				y=i%10;
				DSP_SetGraphBright( CGGP_SYS+i, 127,121,57 );

				DSP_SetGraphDisp( CGGP_SYS+42, ON );
				DSP_SetGraphMove( CGGP_SYS+42, 26+x*192, 160+4+y*32 );

				DSP_SetGraphDisp( CGGP_SYS+43, ON );
				DSP_SetGraphSMove( CGGP_SYS+43, 0, 24*i );

				DSP_SetGraphDisp( CGGP_SYS+44, ON );
				DSP_SetGraphSMove( CGGP_SYS+44, 0, 24*MusArtistTable[i][0] );

				DSP_SetGraphDisp( CGGP_SYS+45, ON );
				DSP_SetGraphSMove( CGGP_SYS+45, 0, 24*MusArtistTable[i][1] );
			}else{
				DSP_SetGraphDisp( CGGP_SYS+42, OFF );

				DSP_SetGraphDisp( CGGP_SYS+43, OFF );
				DSP_SetGraphDisp( CGGP_SYS+44, OFF );
				DSP_SetGraphDisp( CGGP_SYS+45, OFF );
			}
			
			DSP_SetGraphSMove( CGGP_SYS+40, 0, 0 );
			DSP_SetGraphDisp( CGGP_SYS+41, OFF );
			if(select!=-1){
				if(select==40) {
					DSP_SetGraphSMove( CGGP_SYS+40, 188, 0 );
				}else{
					i=select;
					x=i/10;
					y=i%10;
					DSP_SetGraphDisp( CGGP_SYS+41, ON );
					DSP_SetGraphPos( CGGP_SYS+41, 20+x*192, 160+y*32, 20+x*192, 160+y*32, 184, 28 );
				}
				if( click ){
					switch(select){
					default:

						if(select==sel_mus){
							bak_mus = sel_mus;
							sel_mus = -1;
							mus_cnt = 0;

							AVG_StopBGM( 60 );
							DSP_SetGraphDisp( CGGP_SYS+42, OFF );
						}else{
							bak_mus = sel_mus;
							sel_mus = select;
							mus_cnt = 0;

							AVG_PlayBGM( MusTable[select], -2, ON, 255, 0 );

							i=sel_mus;
						}
						MainWindow.draw_flag=1;
						break;
					case 40:
						DSP_SetGraphSMove( CGGP_SYS+40, 188*2, 0 );

						AVG_StopBGM( 60 );
						AVG_PlaySE3( 9104, 255 );
						step = SOUND_FADEOUT;
						break;
					}
				}
			}
			if( cansel ){
				AVG_StopBGM( 60 );
				AVG_PlaySE3( 9107, 255 );
				step = SOUND_FADEOUT;
			}
			break;
	}
	if(step_bak==step)	count++;
	else				count=0;

	return ret;
}

void OMK_SetSiyuModeMouse( void )
{
	int	y;
	MUS_SetMouseLayer( CgModeMouse );

	for( y=0; y<8 ;y++ )
		MUS_SetMouseRect( CgModeMouse, y, 532, 107+y*35, 184, 35, ON );

	MUS_SetMouseRect( CgModeMouse, 40, 628, 553, 124,28, ON );
}



void OMK_SetSiyuMode( int bmp_set, int grp_set, int txt_set, int mus_lay )
{
	CgModeBmp   = bmp_set;
	CgModeGrp   = grp_set;
	CgModeTxt   = txt_set;
	CgModeMouse = mus_lay;

	OMK_SetSiyuModeMouse();
}


int OMK_ControlSiyuMode( void )
{
	enum{
		SOUND_INIT,
		SOUND_TERM,
		SOUND_FADEIN,
		SOUND_FADEOUT,
		SOUND_MAIN,
	};
	BOOL		ret=0;
	int			bmp_bpp = BPP(MainWindow.draw_mode2);
	static int	step=SOUND_INIT;
	int			step_bak=step;
	static int	count=0;
	int			fmax = AVG_EffCnt(-2);
	int			rate,i, flag;

	static int	mus_cnt2=0;
	static int	bak_mus=-1;
	static int	sel_mus=-1;
	int			select = MUS_GetMouseNo(-1);
	int			key_flag=0;
	int			click  = GameKey.click;
	int			cansel = GameKey.cansel;

	static int	select_fade = 0;
	char		fname[256];

	if(GameKey.u) {
		if(select==40){
			select = 7; key_flag=2;
		}else{
			select = LIM(select-1,0,7); key_flag=2;
		}
	}
	if(GameKey.d) {
		if(select==7){
			select = 40; key_flag=2;
		}else if(select==40){
		}else{
			select = LIM(select+1,0,7); key_flag=2;
		}
	}

	if(key_flag){
		if(select==-1)select=0;
		if( MUS_GetMouseRectFlag(CgModeMouse,select) ){
			MUS_SetMousePosRect( MainWindow.hwnd, CgModeMouse, select );
		}
	}


	switch(step){
		case SOUND_INIT:
			AVG_StopBGM( 30 );
			ret=-1;
			MainWindow.draw_flag=1;
			bak_mus = -1;
			sel_mus = -1;
			flag=0;
			for(i=0;i<24;i++){
				if( !AVG_GetGameFlag(i+128) ){
					flag=1;
				}
			}
			DSP_LoadBmp( CGBM_BACK+0,   BMP_FULL, "t2000.bmp" );
			DSP_LoadBmp( CGBM_BACK+1,   BMP_FULL, "t2001.bmp" );
			DSP_LoadBmp( CGBM_BACK+2,   BMP_FULL, "t2002.bmp" );

			DSP_LoadBmp( CGBM_SYS,  BMP_FULL, "sys0420.bmp" );

			DSP_SetGraph( CGGP_BACK,    CGBM_BACK,   30, ON, CHK_NO );	

			DSP_SetGraph( CGGP_SYS+0, CGBM_BACK+1, 32, OFF, CHK_NO );
			DSP_SetGraph( CGGP_SYS+1, CGBM_BACK+2, 31, OFF, CHK_NO );
			DSP_SetGraph( CGGP_SYS+40, CGBM_SYS, 32, OFF, CHK_NO );
			DSP_SetGraphPos( CGGP_SYS+40, 628, 553, 102, 28, 124,28 );

			step = SOUND_FADEIN;

		case SOUND_FADEIN:
			if( count<fmax ){
				rate = count*256/fmax;
				DSP_SetGraphParam( CGGP_BACK, DRW_BLD(rate) );
			}else{
				DSP_SetGraphParam( CGGP_BACK, DRW_NML );
				step = SOUND_MAIN;
				mus_cnt2=0;
				sel_mus=-1;
				bak_mus=-1;
			}
			break;
		case SOUND_FADEOUT:
			ret=-1;

			DSP_ResetGraph( CGGP_SYS+0 );
			DSP_ResetGraph( CGGP_SYS+1 );
			DSP_ResetGraph( CGGP_SYS+40 );

			mus_cnt2 = LIM(mus_cnt2,0,32);
			if( count<fmax ){
				rate = 256-count*256/fmax;
				DSP_SetGraphParam( CGGP_BACK, DRW_BLD(rate) );
				rate = (256-mus_cnt2*8) - (256-mus_cnt2*8)*count/fmax;
				DSP_SetGraphParam( CGGP_SYS+2, DRW_BLD(rate) );
				rate = (    mus_cnt2*8) - (    mus_cnt2*8)*count/fmax;
				DSP_SetGraphParam( CGGP_SYS+3, DRW_BLD(rate) );
			}else{
				step = SOUND_TERM;
			}
			break;
		case SOUND_TERM:
			ret=1;
			DSP_ReleaseBmp( CGBM_BACK+0);
			DSP_ReleaseBmp( CGBM_BACK+1 );
			DSP_ReleaseBmp( CGBM_BACK+2 );

			DSP_ReleaseBmp( CGBM_SYS );
			DSP_ReleaseBmp( CGBM_SYS+1 );
			DSP_ReleaseBmp( CGBM_SYS+2 );

			DSP_ResetGraph( CGGP_BACK );	
			DSP_ResetGraph( CGGP_SYS+0 );
			DSP_ResetGraph( CGGP_SYS+1 );
			DSP_ResetGraph( CGGP_SYS+2 );
			DSP_ResetGraph( CGGP_SYS+3 );

			DSP_ResetGraph( CGGP_SYS+40 );

			OMK_ResetReplayModeMouse();

			step = SOUND_INIT;
			break;
		case SOUND_MAIN:

			if(sel_mus!=-1){
				DSP_SetGraphDisp( CGGP_SYS+1, ON );
				DSP_SetGraphPos( CGGP_SYS+1, 532, 107+sel_mus*35, 532, 107+sel_mus*35, 184, 35 );
			}else{
				DSP_SetGraphDisp( CGGP_SYS+1, OFF );
			}
			
			DSP_SetGraphDisp( CGGP_SYS+0, OFF );

			DSP_SetGraphDisp( CGGP_SYS+40, OFF );

			
			mus_cnt2++;
			DSP_SetGraphDisp( CGGP_SYS+2, ON );
			DSP_SetGraphDisp( CGGP_SYS+3, ON );
			DSP_SetGraphParam( CGGP_SYS+2, DRW_BLD(256-mus_cnt2*8) );
			DSP_SetGraphParam( CGGP_SYS+3, DRW_BLD(mus_cnt2*8) );
			
			if(select!=-1){
				if(select==40) {
					DSP_SetGraphDisp( CGGP_SYS+40, ON );
					DSP_SetGraphParam( CGGP_SYS+40, DRW_NML );
					DSP_SetGraphSMove( CGGP_SYS+40, 0, 28*2 );
				}else{
					DSP_SetGraphDisp( CGGP_SYS+0, ON );
					DSP_SetGraphPos( CGGP_SYS+0, 532, 107+select*35, 532, 107+select*35, 184, 35 );
				}
				if( click ){
					switch(select){
					default:
						AVG_PlaySE3( 9104, 255 );
						if(select==sel_mus){
							wsprintf( fname, "t201%d.bmp", sel_mus );	
							DSP_LoadBmp( CGBM_SYS+1,   BMP_TRUE, fname );
							DSP_SetGraph( CGGP_SYS+2, CGBM_SYS+1, 31, ON, CHK_ANTI );

							DSP_ReleaseBmp( CGBM_SYS+2 );

							bak_mus = sel_mus;
							sel_mus = -1;
							mus_cnt2 = 0;

							AVG_FadeOutVoice( 0, 60 );
							DSP_SetGraphDisp( CGGP_SYS+1, OFF );
						}else{
							if(sel_mus!=-1){
								wsprintf( fname, "t201%d.bmp", sel_mus );	
								DSP_LoadBmp( CGBM_SYS+1,   BMP_TRUE, fname );
								DSP_SetGraph( CGGP_SYS+2, CGBM_SYS+1, 31, ON, CHK_ANTI );
							}else{
								DSP_ReleaseBmp( CGBM_SYS+1 );
							}
							wsprintf( fname, "t201%d.bmp", select );	
							DSP_LoadBmp( CGBM_SYS+2,   BMP_TRUE, fname );
							DSP_SetGraph( CGGP_SYS+3, CGBM_SYS+2, 31, OFF, CHK_ANTI );
							
							bak_mus = sel_mus;
							sel_mus = select;
							mus_cnt2 = 0;

							switch(select){
							default:AVG_SizukuPlayVoice( 0, 6900+select, -1, -1, 256, FALSE,TRUE );	break;
							case 6:	AVG_SizukuPlayVoice( 0, 6900+7, -1, -1, 256, FALSE,TRUE );	break;
							case 7:	AVG_SizukuPlayVoice( 0, 6900+6, -1, -1, 256, FALSE,TRUE );	break;
							}

							DSP_SetGraphDisp( CGGP_SYS+1, ON );
							DSP_SetGraphPos( CGGP_SYS+1, 532, 107+sel_mus*35, 532, 107+sel_mus*35, 184, 35 );
						}
						MainWindow.draw_flag=1;
						DSP_SetGraphDisp( CGGP_SYS+0, OFF );
						break;
					case 40:
						DSP_SetGraphSMove( CGGP_SYS+40, 0, 28*3 );

						AVG_FadeOutVoice( 0, 60 );
						AVG_PlaySE3( 9104, 255 );
						step = SOUND_FADEOUT;
						break;
					}
				}
			}
			if( cansel ){
				AVG_FadeOutVoice( 0, 60 );
				AVG_PlaySE3( 9107, 255 );
				step = SOUND_FADEOUT;
			}
			break;
	}
	if(step_bak==step)	count++;
	else				count=0;

	return ret;
}

