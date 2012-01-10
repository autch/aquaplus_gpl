
 
  
   
    
     
      

#include <STD_Windows.h>
#include <STD_Debug.h>
#include <STD_File.h>

#include <comp_pac.h>

#include "resource.h"

#include "main.h"


#include "escript.h"

#include "..\\mes\\escr.h"


#include "GM_Battle.h"
#include "GM_Avg.h"
#include "GM_Char.h"
#include "GM_CharThink.h"
#include "GM_SetBmp.h"
#include "GM_Main.h"
#include "GM_Item.h"


void ESC_SetDrawFlag(void)
{
	appData.draw_flag=ON;
}



int		ESC_FlagBuf[ ESC_FLAG_MAX ];				
int		ESC_GameFlagBuf[ ESC_FLAG_MAX ];			



typedef struct{
	char	str[1024];	
	long	num;		
	long	reg;		
}ESC_PARAM;
ESC_PARAM		EscParam[15];
int				EscCnt = 0;

static int GetScriptParamCal( char *cal )
{
	int		i=0,j=0,reg;
	char	buf[256];
	while( cal[i] ){
		if( !strncmp( &cal[i], "reg", 3 ) ){
			i+=3;
			if( isdigit(cal[i]) ){
				reg = cal[i]-'0';
				i++;
				if( isdigit(cal[i]) ){
					reg*=10;
					reg += cal[i]-'0';
					i++;
				}
				j += wsprintf( &buf[j], "%d", EXEC_ScriptInfo->reg[ reg ] );
			}
		}else{
			buf[j++] = cal[i++];
		}
	}
	buf[j]='\0';

	return Cal2Disgit( buf );
}




static int GetScriptParam( int pc, int *start,int size )
{
	int mode;
	int	ret;

	mode = EXEC_ScriptBuf[ pc + (*start)++ ];						
	if(mode==2){
		int		i,len;
		char	buf[256];

		len   = EXEC_ScriptBuf[ pc + (*start)++ ];		
		for( i=0; i<len; i++ ) { buf[ i ] = EXEC_ScriptBuf[ pc + (*start)++ ]; }
		buf[ i ] = 0x00;
		ret = GetScriptParamCal( buf );
	}else{		
		switch( size ){
			default:
			case 1:	ret  = (unsigned char)EXEC_ScriptBuf[ pc + *start ];			(*start)+=1;	break;	
			case 2: ret  = EXEC_GetShortValue( &EXEC_ScriptBuf[ pc + *start ] );	(*start)+=2;	break;	
			case 4: ret  = EXEC_GetLongValue(  &EXEC_ScriptBuf[ pc + *start ] );	(*start)+=4;	break;	
		}
		if( mode == 0 ) { ret = EXEC_ScriptInfo->reg[ ret ]; }					
	}
	return ret;
}



static int GetScriptParamByte( int pc, int *start )
{
	return EXEC_ScriptBuf[ pc + (*start)++ ];						
}



static int GetScriptParamWord( int pc, int *start )
{
	int	ret;

	ret  = EXEC_ScriptBuf[ pc + (*start)++ ];	
	ret |= EXEC_ScriptBuf[ pc + (*start)++ ]<<8;	
	return ret;
}



static int GetScriptParam_str( int pc, int *start, char *buf )
{
	int	i,len;

	len   = (unsigned)EXEC_ScriptBuf[ pc + (*start)++ ];		
	for( i=0; i<len; i++ ) { buf[ i ] = EXEC_ScriptBuf[ pc + (*start)++ ]; }
	buf[ i ] = 0x00;
	return *start;
}



static int GetScriptParam_str2( int pc, int *start, char *buf )
{
	int	i,len;

	len  = (unsigned)EXEC_ScriptBuf[ pc + (*start)++ ];	
	len |= ((unsigned)EXEC_ScriptBuf[ pc + (*start)++ ])<<8;	
	for( i=0; i<len; i++ ) { buf[ i ] = EXEC_ScriptBuf[ pc + (*start)++ ]; }
	buf[ i ] = 0x00;
	return *start;
}




static int GetScriptParamControl( int opr )
{
	int		i;
	int		cnt = 2;

	if(opr<64)	DebugPrintf("イベントスクリプトの命令コードとして不適です[%d]",opr);
	opr-=64;

	for(i=0;i<15;i++){
		switch( EScroptOpr[opr].type[i] ){
			default:	DebugPrintf("引数タイプが不適です[%d]",EScroptOpr[opr].type[i]);	
			case ESC_NOT:	return cnt;												
			case ESC_REG:	
			case ESC_ADD:	
			case ESC_ASC:	EscParam[i].num = GetScriptParamByte( EXEC_ScriptInfo->pc, &cnt );	break;	
			case ESC_CNT:	EscParam[i].num = GetScriptParamWord( EXEC_ScriptInfo->pc, &cnt );	break;	

			case ESC_VCNT:	EscParam[i].num = GetScriptParamWord( EXEC_ScriptInfo->pc, &cnt );	break;	

			case ESC_NUM:	EscParam[i].num = GetScriptParam( EXEC_ScriptInfo->pc, &cnt, 4 );	break;	
			case ESC_STR:	GetScriptParam_str( EXEC_ScriptInfo->pc, &cnt, EscParam[i].str );	break;	
			case ESC_STR2:	GetScriptParam_str2( EXEC_ScriptInfo->pc, &cnt, EscParam[i].str );	break;	
			case ESC_CMP:
				{
					int	param1, param2, param3;
					param1 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + cnt++ ];	
					param1 = EXEC_GetParam( param1 );						
					param2 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + cnt++ ];	
					param3 = GetScriptParam( EXEC_ScriptInfo->pc, &cnt, 4 );						
					
					switch( param2 ) {
						case CALC_SMALL:	EscParam[i].num = ( param1 <  param3 ) ? 1 : 0;		break;		
						case CALC_ESMALL:	EscParam[i].num = ( param1 <= param3 ) ? 1 : 0;		break;		
						case CALC_LARGE:	EscParam[i].num = ( param1 >  param3 ) ? 1 : 0;		break;		
						case CALC_ELARGE:	EscParam[i].num = ( param1 >= param3 ) ? 1 : 0;		break;		
						case CALC_EQUAL:	EscParam[i].num = ( param1 == param3 ) ? 1 : 0;		break;		
						case CALC_NEQUAL:	EscParam[i].num = ( param1 != param3 ) ? 1 : 0;		break;		
					}
				}
				break;	
		}
	}
	return cnt;
}



static int GetScriptParamControlEx( int pc, int opr, char *str )
{
	int		i;
	int		cnt = 2,flag=0;
	char	buf[32];

	if(opr<64)	DebugPrintf("イベントスクリプトの命令コードとして不適です[%d]",opr);
	opr-=64;

	for(i=0;i<15;i++){
		flag = 1;
		switch( EScroptOpr[opr].type[i] ){
			default:	DebugPrintf("引数タイプが不適です[%d]",EScroptOpr[opr].type[i]);	
			case ESC_NOT:	return cnt;												
			case ESC_REG:	EscParam[i].num = GetScriptParamByte( pc, &cnt );	wsprintf( buf, "REG%d, ", EscParam[i].num); 	break;	
			case ESC_ADD:	EscParam[i].num = GetScriptParamByte( pc, &cnt );	wsprintf( buf, "ADD(%d), ", EscParam[i].num); 	break;	

			case ESC_ASC:	EscParam[i].num = GetScriptParamByte( pc, &cnt );	wsprintf( buf, "%c, ", EscParam[i].num);	break;	
			case ESC_CNT:	EscParam[i].num = GetScriptParamWord( pc, &cnt );	wsprintf( buf, "CNT(%d), ", EscParam[i].num);	break;	

			case ESC_VCNT:	EscParam[i].num = GetScriptParamWord( pc, &cnt );	wsprintf( buf, "VCNT(%d), ", EscParam[i].num);		break;	

			case ESC_NUM:	EscParam[i].num = GetScriptParam( pc, &cnt, 4 );	wsprintf( buf, "%d, ", EscParam[i].num); 	break;	
			case ESC_STR:	GetScriptParam_str( pc, &cnt, EscParam[i].str );	strcat(str,EscParam[i].str ); strcat(str,", " ); flag=0;	break;	
			case ESC_STR2:	GetScriptParam_str2( pc, &cnt, EscParam[i].str );	strcat(str,EscParam[i].str ); strcat(str,", " ); flag=0;	break;	
			case ESC_CMP:
				{
					int	param1, param2, param3;
					param1 = EXEC_ScriptBuf[ pc + cnt++ ];	
					param1 = EXEC_GetParam( param1 );						
					param2 = EXEC_ScriptBuf[ pc + cnt++ ];	
					param3 = GetScriptParam( pc, &cnt, 4 );						
					
					switch( param2 ) {
						case CALC_SMALL:	EscParam[i].num = ( param1 <  param3 ) ? 1 : 0;	wsprintf( buf, "REG%d < %d, ", param1, param3 );	break;		
						case CALC_ESMALL:	EscParam[i].num = ( param1 <= param3 ) ? 1 : 0;	wsprintf( buf, "REG%d <= %d, ", param1, param3 );	break;		
						case CALC_LARGE:	EscParam[i].num = ( param1 >  param3 ) ? 1 : 0;	wsprintf( buf, "REG%d > %d, ", param1, param3 );	break;		
						case CALC_ELARGE:	EscParam[i].num = ( param1 >= param3 ) ? 1 : 0;	wsprintf( buf, "REG%d >= %d, ", param1, param3 );	break;		
						case CALC_EQUAL:	EscParam[i].num = ( param1 == param3 ) ? 1 : 0;	wsprintf( buf, "REG%d == %d, ", param1, param3 );	break;		
						case CALC_NEQUAL:	EscParam[i].num = ( param1 != param3 ) ? 1 : 0;	wsprintf( buf, "REG%d != %d, ", param1, param3 );	break;		
					}
				}
				break;	
		}
		if(flag)
			strcat( str, buf );
	}
	return cnt;
}



static void SetScriptRegisterControl( int opr )
{
	int		i;
	int		cnt = 2;

	if(opr<64)	DebugPrintf("イベントスクリプトの命令コードとして不適です[%d]",opr);
	opr-=64;

	for(i=0;i<15;i++){
		switch( EScroptOpr[opr].type[i] ){
			default:	DebugPrintf("引数タイプが不適です[%d]",EScroptOpr[opr].type[i]);	
			case ESC_NOT:	return ;														
			case ESC_ASC:	break;															
			case ESC_NUM:	break;															
			case ESC_STR2:	break;
			case ESC_STR:	break;															
			case ESC_REG:	EXEC_SetParam( EscParam[i].num, EscParam[i].reg );	break;		
			case ESC_ADD:	break;															
			case ESC_CNT:	break;															
			case ESC_VCNT:	break;															
		}
	}
	return ;
}





void ESC_InitFlag( void )
{
	int		i;
	for( i=0; i<ESC_FLAG_MAX ;i++ )
		ESC_FlagBuf[ i ] = 0;
}
void ESC_InitGameFlag( void )
{
	int		i;
	for( i=0; i<ESC_FLAG_MAX ;i++ )
		ESC_GameFlagBuf[ i ] = 0;
}



static char	EOprFlag[ESC_OPR_MAX];
void ESC_InitEOprFlag( void )
{
	int	i;
	for(i=0; i<ESC_OPR_MAX ;i++)
		EOprFlag[i] = 0;
}



static void ESC_EOprSetFlag( void )
{
	
	ESC_SetFlag( EscParam[0].num, EscParam[1].num  );
	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprGetFlag( void )
{
	
	EscParam[1].reg = ESC_GetFlag( EscParam[0].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetGameFlag( void )
{
	
	AVG_SetGameFlag( EscParam[0].num, EscParam[1].num );
	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprGetGameFlag( void )
{
	
	EscParam[1].reg = ESC_GetGameFlag( EscParam[0].num );
	
	EXEC_AddPC( EscCnt );
}




static void LoadScript( char *fno )
{
	char	str[256];
	wsprintf( str, "%s.sdt", fno );

	
	AVG_SetScenario( str );
}
static void ESC_EOprLoadScript( void )
{

	LoadScript( EscParam[0].str );
}




static void LoadScriptNum( int	fno )
{
	char	str[256];
	wsprintf( str, "%05d.sdt", fno );
	
	AVG_SetScenario( str );

}
static void ESC_EOprLoadScriptNum( void )
{

	LoadScriptNum( EscParam[0].num );
}




static void ESC_EOprCallFunc( void )
{
	long	num[14],i;

	for(i=0;i<14;i++) num[i] = EscParam[i+1].num;
	EXEC_OprCallFunc( EscParam[0].num, EscCnt, num );
}

static void ESC_EOprNextGameStep( void )
{
	BTL_SetNextGameStep( EscParam[0].num, EscParam[1].str );
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetTimeMode(void)
{
	if(EscParam[0].num==-1) EscParam[0].num=0;
	AVG_SetTimeMode( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetChromaMode(void)
{
	if(EscParam[0].num==-1) EscParam[0].num=255;
	if(EscParam[1].num==-1) EscParam[1].num=0;
	AVG_SetChromaMode( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetEffctMode(void)
{
	if(EscParam[1].num==-1) EscParam[1].num=0;
	AVG_SetEffctMode( EscParam[0].str, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprLoadMap( void )
{
	BTL_LoadBattleMap( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprReleaseMap( void )
{
	BTL_ReleaseBattleMap( );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetMapObj(void)
{



	if(EscParam[5].num==-1) EscParam[5].num=0;
	if(EscParam[6].num==-1) EscParam[6].num=0;

	SetMapObj(EscParam[0].num,EscParam[1].num,EscParam[2].num,EscParam[3].num,EscParam[4].num, EscParam[5].num,EscParam[6].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetMapObjEx(void)
{
	if(EscParam[5].num==-1) EscParam[5].num=0;
	if(EscParam[6].num==-1) EscParam[6].num=0;

	SetMapObjEx(EscParam[0].num,EscParam[1].str,EscParam[2].str,EscParam[3].num,EscParam[4].num, EscParam[5].num,EscParam[6].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetMapObjNoLoad( void )
{
	if(EscParam[5].num==-1) EscParam[5].num=0;
	if(EscParam[6].num==-1) EscParam[6].num=0;

	SetMapObjNoLoad( EscParam[0].num,EscParam[1].num,EscParam[2].num,EscParam[3].num,EscParam[4].num, EscParam[5].num,EscParam[6].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetMapObjRevParam( void)
{
	SetMapObjRevParam( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetMapObjLayer(void)
{
	SetMapObjLayer( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetMapObjMove(void)
{
	SetMapObjMove( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	EXEC_AddPC( EscCnt );
}


static void ESC_EOprSetMapObjZoom(void)
{
	SetMapObjZoom( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetMapObjParam(void)
{
	SetMapObjParam( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprPlayMapObj(void)
{



	PlayMapObj( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprResetMapObj(void)
{
	if(EscParam[1].num==-1) EscParam[1].num=0;

	ResetMapObj( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}


static void ESC_EOprSetBrightMap(void)
{
	if(EscParam[3].num==-1) EscParam[3].num=30;
	if(EscParam[4].num==-1) EscParam[4].num=0;

	SetBrightBattleMap( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetScrollMap(void)
{

	if(EscParam[3].num==-1) EscParam[3].num=30;
	if(EscParam[4].num==-1) EscParam[4].num=0;

	SetScrolBattleMap( (float)EscParam[0].num, (float)EscParam[1].num,  EscParam[2].num/4096.0f, EscParam[3].num, EscParam[4].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetScrollMapChar(void)
{

	if(EscParam[2].num==-1) EscParam[2].num=30;
	if(EscParam[3].num==-1) EscParam[3].num=0;

	SetScrolBattleMapChar( EscParam[0].num+10, EscParam[1].num/4096.0f, EscParam[2].num, EscParam[3].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprWaitMap(void)
{
	if(EscParam[0].num==-1) EscParam[0].num=0xff;

	if(EOprFlag[ESC_WAIT_MAP]==0){
		EOprFlag[ESC_WAIT_MAP] = 1;
	}
	if( !WaitScrolBattleMap(EscParam[0].num) ){
		EOprFlag[ESC_WAIT_MAP] = 0;
		EXEC_AddPC( EscCnt );
	}
}

static void ESC_EOprStartBattle( void )
{
	if(EscParam[2].num==-1) EscParam[2].num=0;
	if(EscParam[3].num==-1) EscParam[3].num=0;
	if(EscParam[4].num==-1) EscParam[4].num=0;
	if(EscParam[5].num==-1) EscParam[5].num=-1;

	BTL_SetStartBattle( EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num, EscParam[6].num );
	BTL_SetStartPoint( (float)EscParam[0].num, (float)EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprWaitBattleEnd( void )
{
	if(EOprFlag[ESC_WAIT_BATTLE_END]==0){
		EOprFlag[ESC_WAIT_BATTLE_END] = 1;
	}
	int	win;
	if( BTL_WaitBattleEnd(&win) ){
		EOprFlag[ESC_WAIT_BATTLE_END] = 0;
		EXEC_AddPC( EscCnt );
	}
	
}

static void ESC_EOprWaitBattleEndEx( void )
{
	if(EOprFlag[ESC_WAIT_BATTLE_END_EX]==0){
		EOprFlag[ESC_WAIT_BATTLE_END_EX] = 1;
	}
	int	win;
	if( BTL_WaitBattleEnd(&win) ){
		EscParam[0].reg = win;
		EOprFlag[ESC_WAIT_BATTLE_END_EX] = 0;
		EXEC_AddPC( EscCnt );
	}
	
}


static void ESC_EOprSetMapCharPlayer( void )
{
	int	pno  = EscParam[0].num;
	int	cpno = EscParam[1].num;
	int	px   = EscParam[2].num;
	int	py   = EscParam[3].num;
	int	dir  = (EscParam[4].num==-1)?0:EscParam[4].num;

	SetBackUpMapCharPlayer( pno, cpno, px, py, dir );

	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetMapChar( void )
{
	int	no   = EscParam[0].num;
	int	job  = EscParam[1].num;
	int	buki = EscParam[2].num;

	int	px   = EscParam[3].num;
	int	py   = EscParam[4].num;
	int	dir  = (EscParam[5].num==-1)?0:EscParam[5].num;
	int	lv   = EscParam[6].num;
	int	attr = EscParam[7].num;
	int	unit = (EscParam[8].num==-1)?1:EscParam[8].num;

	SetBackUpMapChar( no, no+32, job, buki, px, py, dir, lv, attr, unit );

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetMapCharName( void )
{
	int	no   = EscParam[0].num;

	SetBackUpMapCharName( no, no+32, EscParam[1].str );

	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetMapCharEngun( void )
{
	int	no = EscParam[0].num;
	int	etime = EscParam[1].num;	
	int	loop  = EscParam[2].num;	
	int	inter  = EscParam[3].num;	
	
	SetBackUpMapCharEngun( no, etime, loop, inter );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetMapCharItem( void )
{
	int	no = EscParam[0].num;
	int	itype = EscParam[1].num;	
	int	ino   = EscParam[2].num;	
	
	SetBackUpMapCharItem( no, itype, ino );
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetMapCharThink( void )
{
	int	no   = EscParam[0].num;
	int	move_think   = EscParam[1].num;
	int	action_think = EscParam[2].num;
	int	tx   = EscParam[3].num;
	int	ty   = EscParam[4].num;
	int	range  = EscParam[5].num;

	SetBackUpMapCharTnink( no, move_think, action_think, tx, ty, range );

	EXEC_AddPC( EscCnt );
}

static void ESC_EOprResetMapChar( void )
{
	int	no  = EscParam[0].num+10;

	ResetMapChar( no );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprResetMapCharAll( void )
{
	ResetMapCharAll( );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetMapCharEvent( void )
{
	int	no  = EscParam[0].num+10;
	int	dir = EscParam[1].num;
	int	act = EscParam[2].num;
	int	loop = (EscParam[3].num==-1)?1:EscParam[3].num;



		SetMapCharEvent( no, dir, act, loop );




		EXEC_AddPC( EscCnt );

}
static void ESC_EOprWaitMapCharEvent( void )
{
	int	no  = EscParam[0].num+10;

	if(	WaitMapCharEvent(no) ){
		EXEC_AddPC( EscCnt );
	}
}

static void ESC_EOprSetMapCharMove( void )
{
	int	no  = EscParam[0].num+10;
	int	tx  = EscParam[1].num;
	int	ty  = EscParam[2].num;
	int	sinkin = (EscParam[3].num==-1)?0:EscParam[3].num;
	int	dir    = (EscParam[4].num==-1)?-1:EscParam[4].num;
	int	mov    = EscParam[5].num;

	SetMapCharTartgetMove( no, tx, ty, sinkin, dir, mov, 0 );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprWaitMapCharMove( void )
{
	int	no  = EscParam[0].num+10;

	if(	WaitMapCharTartget2(no) ){

		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprSetMapCharDisp( void )
{
	int	no   = EscParam[0].num+10;
	int	disp = EscParam[1].num;

	SetMapCharDisp(no,disp);
	EXEC_AddPC( EscCnt );
}





static void ESC_EOprSetMessage2(void)
{
	
	if(!EOprFlag[ESC_SETMESSAGE2]){
		EOprFlag[ESC_SETMESSAGE2] = 1;

		AVG_SetMessageWindow(-1,0,-2);
		AVG_SetMessage( EscParam[0].str, EscParam[1].num );
		AVG_SetScenarioFlag( EscParam[2].num );
	}
	
	
	if( AVG_WaitMessage() ){
		EOprFlag[ESC_SETMESSAGE2] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}



static void ESC_EOprAddMessage2(void)
{
	
	if(!EOprFlag[ESC_ADDMESSAGE2]){
		EOprFlag[ESC_ADDMESSAGE2] = 1;
		AVG_AddMessage( EscParam[0].str, EscParam[1].num );
	}
	
	if( AVG_WaitMessage() ){
		EOprFlag[ESC_ADDMESSAGE2] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}


static void ESC_EOprB( void )
{
	int	bak_no=-1;
	
	if(EscParam[1].num!=-1){
		bak_no = EscParam[1].num*10;					
		if(EscParam[2].num!=-1) bak_no += EscParam[2].num;	
	}
	if(EscParam[3].num==-1) EscParam[3].num = -2;	
	if(EscParam[4].num==-1) EscParam[4].num = 128;	

	if(EscParam[5].num==-1) EscParam[5].num = 0;	
	if(EscParam[6].num==-1) EscParam[6].num = 0;	
	if(EscParam[7].num==-1) EscParam[7].num = 800;	
	if(EscParam[8].num==-1) EscParam[8].num = 600;	

	if(EOprFlag[ESC_B]==0){
		EOprFlag[ESC_B] = 1;
		AVG_SetBack( bak_no, EscParam[0].num, OFF, 0, EscParam[3].num, EscParam[4].num,
			EscParam[5].num, EscParam[6].num, EscParam[7].num, EscParam[8].num );
	}
	
	if( !AVG_WaitBack() ){
		EOprFlag[ESC_B] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprBT( void )
{
}

static void ESC_EOprBC( void )
{
	int	bak_no=-1;
	
	if(EscParam[1].num!=-1){
		bak_no = EscParam[1].num*10;					
		if(EscParam[2].num!=-1) bak_no += EscParam[2].num;	
	}
	if(EscParam[3].num==-1) EscParam[3].num = -2;	
	if(EscParam[4].num==-1) EscParam[4].num = 128;	

	if(EscParam[5].num==-1) EscParam[5].num = 0;	
	if(EscParam[6].num==-1) EscParam[6].num = 0;	
	if(EscParam[7].num==-1) EscParam[7].num = 800;	
	if(EscParam[8].num==-1) EscParam[8].num = 600;	

	if(EOprFlag[ESC_BC]==0){
		EOprFlag[ESC_BC] = 1;
		AVG_SetBack( bak_no, EscParam[0].num, ON, 0, EscParam[3].num, EscParam[4].num,
			EscParam[5].num, EscParam[6].num, EscParam[7].num, EscParam[8].num );
	}
	
	if( !AVG_WaitBack() ){
		EOprFlag[ESC_BC] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprBCT( void )
{
}
static void ESC_EOprV( void )
{
	int	bak_no=-1;

	if(EscParam[1].num!=-1){
		bak_no = EscParam[1].num*10;					
		if(EscParam[2].num!=-1) bak_no += EscParam[2].num;	
	}
	if(EscParam[3].num==-1) EscParam[3].num = -2;	
	if(EscParam[4].num==-1) EscParam[4].num = 128;	

	if(EscParam[5].num==-1) EscParam[5].num = 0;	
	if(EscParam[6].num==-1) EscParam[6].num = 0;	
	if(EscParam[7].num==-1) EscParam[7].num = 800;	
	if(EscParam[8].num==-1) EscParam[8].num = 600;	


	if(EOprFlag[ESC_V]==0){
		EOprFlag[ESC_V] = 1;
		AVG_SetBack( bak_no, EscParam[0].num, OFF, 1, EscParam[3].num, EscParam[4].num,
			EscParam[5].num, EscParam[6].num, EscParam[7].num, EscParam[8].num );
	}
	
	if( !AVG_WaitBack() ){
		EOprFlag[ESC_V] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprVT( void )
{
}
static void ESC_EOprH( void )
{
	int	bak_no=-1;

	if(EscParam[1].num!=-1){
		bak_no = EscParam[1].num*10;					
		if(EscParam[2].num!=-1) bak_no += EscParam[2].num;	
	}
	if(EscParam[3].num==-1) EscParam[3].num = -2;	
	if(EscParam[4].num==-1) EscParam[4].num = 128;	

	if(EscParam[5].num==-1) EscParam[5].num = 0;	
	if(EscParam[6].num==-1) EscParam[6].num = 0;	
	if(EscParam[7].num==-1) EscParam[7].num = 800;	
	if(EscParam[8].num==-1) EscParam[8].num = 600;	


	if(EOprFlag[ESC_H]==0){
		EOprFlag[ESC_H] = 1;
		AVG_SetBack( bak_no, EscParam[0].num, OFF, 2, EscParam[3].num, EscParam[4].num,
			EscParam[5].num, EscParam[6].num, EscParam[7].num, EscParam[8].num );
	}
	
	if( !AVG_WaitBack() ){
		EOprFlag[ESC_H] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprHT( void )
{
}

static void ESC_EOprVHFlag( void )
{

	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprS( void )
{
	

}
static void ESC_EOprZ( void )
{

}


static void ESC_EOprQ( void )
{
	
	if(!EOprFlag[ESC_Q]){
		EOprFlag[ESC_Q] = 1;
		AVG_SetShake( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	}
	
	if( !AVG_WaitShake() ){	
		EOprFlag[ESC_Q] = 0;
		EXEC_AddPC( EscCnt );
	}
}




static void ESC_EOprC( void )
{
	

	if(EscParam[3].num==-1)	EscParam[3].num = 0;	

	if(EscParam[3].num==-2){
		EscParam[3].num = 0;	
		EscParam[7].num = 0;
	}else{
		if(EscParam[7].num==-1) EscParam[7].num = -1;	
	}



	
	if(!EOprFlag[ESC_C]){
		EOprFlag[ESC_C] = 1;

		AVG_SetChar( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[4].num, EscParam[3].num&0xff,
			EscParam[5].num, EscParam[5].num, EscParam[5].num, EscParam[6].num, EscParam[7].num );
	}
	
	if( !AVG_WaitChar( EscParam[0].num ) || (EscParam[3].num&CHAR_TYPE_NOWAIT) ){	
		EOprFlag[ESC_C] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprCR( void )
{
	
	if(EscParam[1].num==-1) EscParam[1].num = -2;
	if(EscParam[2].num==-1) {
		EscParam[2].num = 0;
	}
	if(EscParam[2].num==-2) {
		EscParam[2].num = 0;
		EscParam[1].num = 0;
	}
	
	if(!EOprFlag[ESC_CR]){
		EOprFlag[ESC_CR] = 1;
		AVG_ResetChar( EscParam[0].num, EscParam[2].num&0xff, EscParam[1].num );
	}
	
	if( !AVG_WaitChar( EscParam[0].num ) || (EscParam[2].num&CHAR_TYPE_NOWAIT)  ){	
		EOprFlag[ESC_CR] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprCP( void )
{
	if(EscParam[2].num==-1) EscParam[2].num = -1;
	
	if(!EOprFlag[ESC_CP]){
		EOprFlag[ESC_CP] = 1;
		AVG_SetCharPose( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	
	if( !AVG_WaitChar( EscParam[0].num ) ){	
		EOprFlag[ESC_CP] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprCL( void )
{
	
	if(!EOprFlag[ESC_CL]){
		EOprFlag[ESC_CL] = 1;
		if( EscParam[2].num==-1 ) EscParam[2].num = -1;
		AVG_SetCharLocate( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	
	if( !AVG_WaitChar( EscParam[0].num ) ){	
		EOprFlag[ESC_CL] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprCY( void )
{
	
	if(!EOprFlag[ESC_CY]){
		EOprFlag[ESC_CY] = 1;
		AVG_SetCharLayer( EscParam[0].num, EscParam[1].num );
	}
	
	if( !AVG_WaitChar( EscParam[0].num ) ){	
		EOprFlag[ESC_CY] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprCB( void )
{
	
	if(!EOprFlag[ESC_CB]){
		EOprFlag[ESC_CB] = 1;
		AVG_SetCharBright( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	}
	
	if( !AVG_WaitChar( EscParam[0].num ) ){	
		EOprFlag[ESC_CB] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprCA( void )
{
	
	if(!EOprFlag[ESC_CA]){
		EOprFlag[ESC_CA] = 1;
		AVG_SetCharAlph( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	
	if( !AVG_WaitChar( EscParam[0].num ) ){	
		EOprFlag[ESC_CA] = 0;
		EXEC_AddPC( EscCnt );
	}
}

static void ESC_EOprCW( void )
{
	if(!EOprFlag[ESC_CW]){
		EOprFlag[ESC_CW] = 1;
	}
	if( !AVG_WaitChar( EscParam[0].num ) ){	
		EOprFlag[ESC_CW] = 0;
		EXEC_AddPC( EscCnt );
	}
}



static void ESC_EOprW( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 0;
	if(EscParam[2].num==-1) EscParam[2].num = 0;
	if(EscParam[1].num==-1) EscParam[1].num = -2;

	
	if(!EOprFlag[ESC_W]){
		EOprFlag[ESC_W] = 1;
		AVG_SetMessageWindow( EscParam[0].num, EscParam[2].num, EscParam[1].num );
	}
	

		EOprFlag[ESC_W] = 0;
		
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprWR( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = -2;
	if(EscParam[1].num==-1) EscParam[1].num = 0;
	
	if(!EOprFlag[ESC_WR]){
		EOprFlag[ESC_WR] = 1;
		AVG_ResetMessageWindow( EscParam[1].num, EscParam[0].num );
	}
	

		EOprFlag[ESC_WR] = 0;
		
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprWN( void )
{
	
	AVG_SetNameWindowFree( EscParam[0].str );
	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprK( void )
{
	
	if( AVG_WaitKey() ){
		
		EXEC_AddPC( EscCnt );
	}
}




static void ESC_EOprM( void )
{
	if(EscParam[1].num==-1) EscParam[1].num = -2;
	if(EscParam[2].num==-1) EscParam[2].num = ON;
	if(EscParam[3].num==-1) EscParam[3].num = 0xff;
	AVG_PlayBGM( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, 0 );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMS( void )
{
	int		fade = EscParam[0].num;
	if(EscParam[0].num==-1) fade = 0;
	AVG_StopBGM( fade );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMP( void )
{
	int		flag = EscParam[0].num;
	AVG_PauseBGM( flag );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMV( void )
{
	AVG_SetVolumeBGM( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMW(void)
{
	if( AVG_WaitBGM() ){
		
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprMLW(void)
{
	if( AVG_WaitLoopBGM() ){
		
		EXEC_AddPC( EscCnt );
	}
}


static void ESC_EOprSE( void )
{
	if( EscParam[1].num==-1) EscParam[1].num = 0xff;
	AVG_PlaySE3( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSEP( void )
{
	if( EscParam[2].num==-1) EscParam[2].num = 0;
	if( EscParam[3].num==-1) EscParam[3].num = TRUE;
	if( EscParam[4].num==-1) EscParam[4].num = 0xff;

	AVG_PlaySE2( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSES( void )
{
	AVG_StopSE2( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSEV( void )
{
	AVG_SetVolumeSE( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSEW( void )
{
	if( !AVG_WaitSe(EscParam[0].num) ){
		
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprSEVW( void )
{
	if( !AVG_WaitFadeSe(EscParam[0].num) ){
		
		EXEC_AddPC( EscCnt );
	}
}


static void ESC_EOprVV( void )
{
	if(EscParam[1].num==-1) EscParam[1].num = 256;		
	if(EscParam[2].num==-1) EscParam[2].num = FALSE;	
	if(EscParam[3].num==-1) EscParam[3].num = 0;		
	AVG_PlayVoice( EscParam[3].num, EscParam[0].num, -1, EscParam[4].num, EscParam[1].num, EscParam[2].num );
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprVA( void )
{
	if(EscParam[1].num==-1) EscParam[1].num = 255;		
	if(EscParam[2].num==-1) EscParam[2].num = FALSE;	
	if(EscParam[3].num==-1) EscParam[3].num = 0;		
	AVG_PlayVoice( EscParam[3].num, EscParam[0].num, -1, EscParam[4].num, EscParam[1].num, EscParam[2].num );
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprVB( void )
{
	if(EscParam[1].num==-1) EscParam[1].num = 255;		
	if(EscParam[2].num==-1) EscParam[2].num = FALSE;	
	if(EscParam[3].num==-1) EscParam[3].num = 0;		
	AVG_PlayVoice( EscParam[3].num, EscParam[0].num, -1, EscParam[4].num, EscParam[1].num, EscParam[2].num );
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprVC( void )
{
	if(EscParam[1].num==-1) EscParam[1].num = 255;		
	if(EscParam[2].num==-1) EscParam[2].num = FALSE;	
	if(EscParam[3].num==-1) EscParam[3].num = 0;		
	AVG_PlayVoice( EscParam[3].num, EscParam[0].num, -1, EscParam[4].num, EscParam[1].num, EscParam[2].num );
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprVX( void )
{
	if(EscParam[3].num==-1) EscParam[3].num = 255;		
	if(EscParam[4].num==-1) EscParam[4].num = FALSE;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;		
	AVG_PlayVoice( EscParam[5].num, EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprVW( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 0;		
	if( AVG_WaitVoice(EscParam[0].num) ){
		
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprVS( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 0;		
	if(EscParam[1].num==-1) EscParam[1].num = 0;		
	if(EscParam[0].num==0){
		AVG_StopVoice(EscParam[1].num);
	}else{
		AVG_FadeOutVoice( EscParam[1].num, EscParam[0].num );
	}
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprVI( void )
{
	if(EscParam[1].num==-1){
		AddVoiceDialogMes( "カウンター更新[xxxxx_%03d]", EscParam[0].num );
	}else{
		AddVoiceDialogMes( "カウンター更新[%05d_%03d]", EscParam[1].num, EscParam[0].num );
	}

	AVG_SetVI_EventVoiceNo( EscParam[1].num );
	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetWeather(void)
{
	if(EscParam[4].num==-1) EscParam[4].num = 0;
	
	AVG_SetWeather( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprChangeWeather(void)
{
	if(EscParam[3].num==-1) EscParam[3].num = 0;
	
	AVG_SetWeatherWind( EscParam[0].num );
	AVG_SetWeatherSpeed( EscParam[1].num );
	AVG_SetWeatherAmount( EscParam[2].num );
	AVG_SetWeatherLayer( EscParam[3].num );
	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprResetWeather(void)
{
	
	if(!EOprFlag[ESC_RESETWEATHER]){
		EOprFlag[ESC_RESETWEATHER] = 1;
		AVG_ResetWeather( );
	}
	
	if( AVG_WaitWeather() ){
		EOprFlag[ESC_RESETWEATHER] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}







static void ESC_EOprSetSelectMes(void)
{
	if(EscParam[1].num==-1) EscParam[1].num = 0;
	if(EscParam[2].num==-1) EscParam[2].num = 0;

	AVG_SetSelectMessage( EscParam[0].str, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetSelect(void)
{
	if(EscParam[1].num==-1) EscParam[1].num = 0;
	
	if(!EOprFlag[ESC_SETSELECT]){
		EOprFlag[ESC_SETSELECT] = 1;
		AVG_SetSelectWindow();
	}
	
	EscParam[0].reg = AVG_WaitSelect(EscParam[1].num);
	if( EscParam[0].reg!=-1 ){
		EOprFlag[ESC_SETSELECT] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}





static void ESC_EOprWaitFrame(void)
{
	if(!EOprFlag[ESC_WAITFRAME]){
		EOprFlag[ESC_WAITFRAME] = 1;
		AVG_SetWaitFrame( -1, EscParam[0].num );
	}
	if( AVG_WaitFrame() ){	
		EOprFlag[ESC_WAITFRAME] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}







static void ESC_EOprSetBmp( void )
{
	
	if( EscParam[3].num==-1 )EscParam[3].num=2;		
	if( EscParam[4].num==-1 )EscParam[4].num=-1;	

	if( EscParam[6].num==-1 )EscParam[6].num=256;	


	AVG_LoadBmp( EscParam[0].num, EscParam[1].str, EscParam[2].num, EscParam[3].num, EscParam[4].num,
		EscParam[5].str, EscParam[6].num, EscParam[7].str );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprResetBmp( void )
{
	
	AVG_ReleaseBmp(EscParam[0].num);
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprResetBmpAll( void )
{
	int	i;

	for(i=0;i<MAX_SCRIPT_OBJ;i++){
		AVG_ReleaseBmp( i );
	}
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpAnime( void )
{

	if(EscParam[4].num==-1) EscParam[4].num=256;
	if(EscParam[5].str[0]=='\0') _mbscpy( (BYTE*)EscParam[5].str, (BYTE*)"grp" );
	
	AVG_LoadBmpAnime( EscParam[0].num, EscParam[1].str, EscParam[2].num, EscParam[3].str, EscParam[4].num, EscParam[5].str );
	

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWaitBmpAnime( void )
{
	if( AVG_WaitBmpAnime( EscParam[0].num ) ){	
		EXEC_AddPC( EscCnt );	
	}
}
static void ESC_EOprResetBmpAnime( void )
{
	

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetBmpAnimePlay( void )
{
	if(EscParam[1].num==-1) EscParam[1].num=0;
	if(EscParam[2].num==-1) EscParam[2].num=1;
	if(EscParam[3].num==-1) EscParam[3].num=1;

	AVG_SetBmpAnimePlay( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpDisp( void )
{
	
	AVG_SetBmpDisp( EscParam[0].num, EscParam[1].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpLayer( void )
{
	
	AVG_SetBmpLayer( EscParam[0].num, EscParam[1].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpParam( void )
{
	if(EscParam[2].num==-1) EscParam[2].num=255;
	
	AVG_SetBmpParam( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpRevParam(void)
{
	
	AVG_SetBmpRevParam( EscParam[0].num, EscParam[1].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpBright( void )
{
	EscParam[1].num=LIM(EscParam[1].num,0,255);
	if(EscParam[2].num==-1) EscParam[2].num=EscParam[1].num;
	if(EscParam[3].num==-1) EscParam[3].num=EscParam[1].num;
	
	AVG_SetBmpBright( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpMove( void )
{
	
	AVG_SetBmpMove( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpPos( void )
{
	
	AVG_SetBmpPos( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num, EscParam[6].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpZoom( void )
{
	
	AVG_SetBmpZoom( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpZoom2( void )
{
	
	AVG_SetBmpZoom2( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetBmpRoll( void )
{
	
	AVG_SetBmpRoll( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetAvi( void )
{
	AVG_SetBmpAvi( EscParam[0].num, EscParam[1].str, EscParam[2].num );
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprResetAvi( void )
{
	

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWaitAvi( void )
{
	if( AVG_WaitBmpAvi(EscParam[0].num) ){
		EXEC_AddPC( EscCnt );
	}
}

static void ESC_EOprSetAviFull( void )
{
	if(EscParam[0].num==-1)EscParam[0].num=0;
	AVG_SetAviFull( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprWaitAviFull( void )
{
	
	if( AVG_WaitAviFull() ){
		EXEC_AddPC( EscCnt );
	}
}







static void ESC_EOprSetDemoFlag(void)
{
	
	AVG_SetDemoFlag( EscParam[0].num, EscParam[1].num );
	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprGetTime( void )
{
	EscParam[0].reg = timeGetTime();
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprWaitTime( void )
{
	if( timeGetTime() >= (unsigned long)EscParam[0].num ){
		EXEC_AddPC( EscCnt );
	}
}



static void ESC_EOprMov2( void )
{
	EscParam[0].reg = EscParam[1].num;
	EXEC_AddPC( EscCnt );
}

#include "math.h"
static void ESC_EOprSin( void )
{
	if(EscParam[2].num==-1) EscParam[2].num = 4096;
	EscParam[0].reg = (int)( sin(( LOOP(EscParam[1].num,3600) * PAI)/1800.0)*EscParam[2].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprCos( void )
{
	if(EscParam[2].num==-1) EscParam[2].num = 4096;
	EscParam[0].reg = (int)( cos(( LOOP(EscParam[1].num,3600) * PAI)/1800.0)*EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprAbs( void )
{
	EscParam[0].reg = abs(EscParam[1].num);
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprDebugBox( void )
{
	DebugBox(NULL,"[%d][%s]", EscParam[0].num, EscParam[1].str );
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprTestSetParam( void )
{
	TestSetParam( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetPartyChar( void )
{
	if(EscParam[2].num==-1) EscParam[2].num = 1;

	SetPartyChar( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num);
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprGetPartyLevel( void )
{
	EscParam[1].reg = GetPartyLevel( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}


static void ESC_EOprSetMapBox( void )
{

}

static void ESC_GetItem( void )
{
	ITM_GetItem( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}
static void ESC_CheckItem( void )
{
	EscParam[1].reg = ITM_GetItemAmount( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}









int ESC_OprControl( int opr )
{
	int	i=ESC_LOADSCRIPT;
	
	EscCnt = GetScriptParamControl( opr );
	

	switch( opr ) {
		case ESC_LOAD_MAP:				ESC_EOprLoadMap();			break;	
		case ESC_RELEASE_MAP:			ESC_EOprReleaseMap();			break;	

		case ESC_SET_MAP_OBJ:			ESC_EOprSetMapObj();			break;	
		case ESC_SET_MAP_OBJ_EX:		ESC_EOprSetMapObjEx();			break;	
		case ESC_SET_MAP_OBJ_NOLOAD:	ESC_EOprSetMapObjNoLoad();		break;

		case ESC_SET_MAP_OBJ_REV:		ESC_EOprSetMapObjRevParam();	break;
		case ESC_SET_MAP_OBJ_LAYER:		ESC_EOprSetMapObjLayer();		break;	
		case ESC_SET_MAP_OBJ_MOVE:		ESC_EOprSetMapObjMove();		break;	

		case ESC_SET_MAP_OBJ_ZOOM:		ESC_EOprSetMapObjZoom();		break;	
		case ESC_SET_MAP_OBJ_PARAM:		ESC_EOprSetMapObjParam();		break;	
		case ESC_PLAY_MAP_OBJ:			ESC_EOprPlayMapObj();			break;	
		case ESC_RESET_MAP_OBJ:			ESC_EOprResetMapObj();			break;	

		case ESC_SET_BRIGHT_MAP:		ESC_EOprSetBrightMap();			break;	
		case ESC_SET_SCROLL_MAP:		ESC_EOprSetScrollMap();			break;	
		case ESC_SET_SCROLL_MAP_CHAR:	ESC_EOprSetScrollMapChar();		break;	

		case ESC_WAIT_MAP:				ESC_EOprWaitMap();		break;	
			

		case ESC_START_BATTLE:			ESC_EOprStartBattle();		break;	
		case ESC_WAIT_BATTLE_END:		ESC_EOprWaitBattleEnd();	break;	
		case ESC_WAIT_BATTLE_END_EX:	ESC_EOprWaitBattleEndEx();	break;	

		case ESC_SET_MAP_CHAR_PLAER:	ESC_EOprSetMapCharPlayer();	break;
		case ESC_SET_MAP_CHAR:			ESC_EOprSetMapChar();		break;	
		case ESC_SET_MAP_CHAR_ENGUN:	ESC_EOprSetMapCharEngun();	break;
		case ESC_SET_MAP_CHAR_ITEM:		ESC_EOprSetMapCharItem();	break;	
		case ESC_SET_MAP_CHAR_THINK:	ESC_EOprSetMapCharThink();	break;
		case ESC_RESET_MAP_CHAR:		ESC_EOprResetMapChar();		break;	
		case ESC_RESET_MAP_CHAR_ALL:	ESC_EOprResetMapCharAll();		break;	

		case ESC_SET_MAP_CHAR_EVENT:	ESC_EOprSetMapCharEvent();	break;	
		case ESC_WAIT_MAP_CHAR_EVENT:	ESC_EOprWaitMapCharEvent();	break;	

		case ESC_SET_MAP_CHAR_MOVE:		ESC_EOprSetMapCharMove();	break;	
		case ESC_WAIT_MAP_CHAR_MOVE:	ESC_EOprWaitMapCharMove();	break;	
		case ESC_SET_MAP_CHAR_DISP:		ESC_EOprSetMapCharDisp();	break;	



		case ESC_B:					ESC_EOprB();			break;	
		case ESC_BT:				ESC_EOprBT();			break;	
		case ESC_BC:				ESC_EOprBC();			break;	
		case ESC_BCT:				ESC_EOprBCT();			break;	
		case ESC_V:					ESC_EOprV();			break;	
		case ESC_VT:				ESC_EOprVT();			break;	
		case ESC_H:					ESC_EOprH();			break;	
		case ESC_HT:				ESC_EOprHT();			break;	

		case ESC_VHFLAG:			ESC_EOprVHFlag();		break;	

		case ESC_S:					ESC_EOprS();			break;	
		case ESC_Z:					ESC_EOprZ();			break;	


		case ESC_Q:					ESC_EOprQ();			break;	



		case ESC_C:					ESC_EOprC();			break;	
		case ESC_CR:				ESC_EOprCR();			break;	
		case ESC_CP:				ESC_EOprCP();			break;	
		case ESC_CL:				ESC_EOprCL();			break;	
		case ESC_CY:				ESC_EOprCY();			break;	
		case ESC_CB:				ESC_EOprCB();			break;	
		case ESC_CA:				ESC_EOprCA();			break;	
		case ESC_CW:				ESC_EOprCW();			break;	


		case ESC_W:					ESC_EOprW();			break;	
		case ESC_WR:				ESC_EOprWR();			break;	
		case ESC_WN:				ESC_EOprWN();			break;	

		case ESC_K:					ESC_EOprK();			break;	


		case ESC_M:					ESC_EOprM();			break;	
		case ESC_MS:				ESC_EOprMS();			break;	
		case ESC_MP:				ESC_EOprMP();			break;	
		case ESC_MV:				ESC_EOprMV();			break;	
		case ESC_MW:				ESC_EOprMW();			break;	
		case ESC_MLW:				ESC_EOprMLW();			break;	

		case ESC_SE:				ESC_EOprSE();			break;	
		case ESC_SEP:				ESC_EOprSEP();			break;	
		case ESC_SES:				ESC_EOprSES();			break;	
		case ESC_SEW:				ESC_EOprSEW();			break;	
		case ESC_SEV:				ESC_EOprSEV();			break;	
		case ESC_SEVW:				ESC_EOprSEVW();			break;	

		case ESC_VV:				ESC_EOprVV();			break;	
		case ESC_VA:				ESC_EOprVA();			break;	
		case ESC_VB:				ESC_EOprVB();			break;	
		case ESC_VC:				ESC_EOprVC();			break;	
		case ESC_VX:				ESC_EOprVX();			break;	
		case ESC_VW:				ESC_EOprVW();			break;	
		case ESC_VS:				ESC_EOprVS();			break;	
		case ESC_VI:				ESC_EOprVI();			break;	


		case ESC_SETWEATHER:		ESC_EOprSetWeather();		break;
		case ESC_CHANGEWEATHER:		ESC_EOprChangeWeather();	break;
		case ESC_RESETWEATHER:		ESC_EOprResetWeather();		break;




		case ESC_SETFLAG:			ESC_EOprSetFlag();			break;	
		case ESC_GETFLAG:			ESC_EOprGetFlag();			break;	

		case ESC_SETGAMEFLAG:		ESC_EOprSetGameFlag();		break;	
		case ESC_GETGAMEFLAG:		ESC_EOprGetGameFlag();		break;	

		case ESC_LOADSCRIPT:		ESC_EOprLoadScript();		break;	
		case ESC_LOADSCRIPTNUM:		ESC_EOprLoadScriptNum();	break;	
		case ESC_CALLFUNC:			ESC_EOprCallFunc();			break;	

		case ESC_SETTIMEMODE:		ESC_EOprSetTimeMode();		break;	
		case ESC_SETCHROMAMODE:		ESC_EOprSetChromaMode();	break;	
		case ESC_SETEFFCTMODE:		ESC_EOprSetEffctMode();		break;	


		case ESC_SETMESSAGE2:		ESC_EOprSetMessage2();		break;	


		case ESC_ADDMESSAGE2:		ESC_EOprAddMessage2();		break;	







		case ESC_SETSELECTMES:		ESC_EOprSetSelectMes();		break;	
		case ESC_SETSELECT:			ESC_EOprSetSelect();		break;	



		case ESC_WAITFRAME:			ESC_EOprWaitFrame();		break;	

		case ESC_SETBMP:			ESC_EOprSetBmp();			break;	
		case ESC_RESETBMP:			ESC_EOprResetBmp();			break;	
		case ESC_RESETBMPALL:		ESC_EOprResetBmpAll();		break;	

		case ESC_SETBMPANIME:		ESC_EOprSetBmpAnime();		break;	
		case ESC_RESETBMPANIME:		ESC_EOprResetBmpAnime();	break;	
		case ESC_WAITBMPANIME:		ESC_EOprWaitBmpAnime();		break;	
		case ESC_SETBMPANIMEPLAY:	ESC_EOprSetBmpAnimePlay();	break;

		case ESC_SETBMPDISP:		ESC_EOprSetBmpDisp();		break;	
		case ESC_SETBMPLAYER:		ESC_EOprSetBmpLayer();		break;	
		case ESC_SETBMPPARAM:		ESC_EOprSetBmpParam();		break;	
		case ESC_SETBMPREVPARAM:	ESC_EOprSetBmpRevParam();	break;	
		case ESC_SETBMPBRIGHT:		ESC_EOprSetBmpBright();		break;	
		case ESC_SETBMPMOVE:		ESC_EOprSetBmpMove();		break;	
		case ESC_SETBMPPOS:			ESC_EOprSetBmpPos();		break;	
		case ESC_SETBMPZOOM:		ESC_EOprSetBmpZoom();		break;	
		case ESC_SETBMPZOOM2:		ESC_EOprSetBmpZoom2();		break;	
		case ESC_SETBMPROLL:		ESC_EOprSetBmpRoll();		break;	

		case ESC_SETAVI:			ESC_EOprSetAvi();			break;	
		case ESC_WAITAVI:			ESC_EOprWaitAvi();			break;	

		case ESC_SETAVIFULL:		ESC_EOprSetAviFull();		break;	
		case ESC_WAITAVIFULL:		ESC_EOprWaitAviFull();		break;	


		case ESC_NEXTGAMESTEP:		ESC_EOprNextGameStep();		break;

		case ESC_SETDEMOFLAG:		ESC_EOprSetDemoFlag();		break;	


		case ESC_GETTIME:			ESC_EOprGetTime();			break;	
		case ESC_WAITTIME:			ESC_EOprWaitTime();			break;	

		
		case ESC_MOV2:				ESC_EOprMov2();				break;	
		case ESC_SIN:				ESC_EOprSin();				break;	
		case ESC_COS:				ESC_EOprCos();				break;	
		case ESC_ABS:				ESC_EOprAbs();				break;	

		case ESC_TESTSETPARAM:		ESC_EOprTestSetParam();		break;	
		case ESC_SETPARTYCHAR:		ESC_EOprSetPartyChar();		break;	

		case ESC_GETPARTYLEVEL:		ESC_EOprGetPartyLevel();	break;
		case ESC_SETMAPBOX:			ESC_EOprSetMapBox();		break;

		case ESC_GET_ITEM:			ESC_GetItem();				break;
		case ESC_CHECK_ITEM:		ESC_CheckItem();			break;

		case ESC_SET_MAP_CHAR_NAME:	ESC_EOprSetMapCharName();		break;

		case ESC_DEBUGBOX:			ESC_EOprDebugBox();			break;	
	}

	SetScriptRegisterControl( opr );

	return EScroptOpr[opr-64].ret;
}

#include <ctrl.h>


static HWND	VoiceDialog;

static BOOL CALLBACK VoiceDialogProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{

	long		px=232,py=0;
	switch( wmes ){
		case WM_INITDIALOG:



			SetWindowPos( hwnd, NULL, px-232, py, 0, 0, SWP_NOSIZE|SWP_NOZORDER );
			break;
		case WM_HSCROLL:
		case WM_VSCROLL:
			px=0;
			break;
		case WM_COMMAND:



			break;
		case WM_DESTROY:
			VoiceDialog = NULL;
			break;
		case WM_ACTIVATE:
			break;
		case WM_CLOSE:
			DestroyWindow( hwnd );
			break;
	}
	return 0;
}

void SetVoiceDialog( int flag )
{
	if(flag){
		if(!VoiceDialog){
			VoiceDialog = CreateDialog( appData.Inst, MAKEINTRESOURCE(IDD_DIALOG10), appData.Hwnd, VoiceDialogProc );
			ShowWindow(VoiceDialog,SW_SHOW);
		}
	}else{
		if(VoiceDialog){
			DestroyWindow( VoiceDialog );
		}
	}
	return ;
}
void AddVoiceDialogMes( char *mes, ...)
{
	if( appData.debug_mouse==-1 ) return ;
	if(VoiceDialog){
		char	buf[1024];
		wvsprintf( buf, mes, (char *)(&mes+1));
		ListAddString( VoiceDialog, IDC_LIST2, buf );
		int	index=ListGetMaxCount(VoiceDialog, IDC_LIST2);
		SendDlgItemMessage(VoiceDialog, IDC_LIST2,LB_SETTOPINDEX, index-24, 0);

		index = SendDlgItemMessage(VoiceDialog, IDC_LIST2,LB_GETTOPINDEX, 0, 0);

	}
}




static char	*PackScriptDir=NULL;

void EXEC_SetPackDir( char *pac_dir )
{
	PackScriptDir = pac_dir;
}





char ScriptOprList[64][32] = 
{
	"Start",	
	"End",		
	"MovR",		
	"MovV",		
	"Swap",		
	"Rand",		
	"IfR",		
	"IfV",		
	"IfElseR",	
	"IfElseV",	
	"Loop",		
	"Goto",		
	"Inc",		
	"Dec",		
	"Not",		
	"Neg",		
	"AddR",		
	"AddV",		
	"SubR",		
	"SubV",		
	"MulR",		
	"MulV",		
	"DivR",		
	"DivV",		
	"ModR",		
	"ModV",		
	"AndR",		
	"AndV",		
	"OrR",		
	"OrV",		
	"XorR",		
	"XorV",		
	"Calc",		
	"Pusha",	
	"Popa",		
	"Call",		
	"Ret",		
	"Wait",		
	"TWait",	
	"Run",		
	"SLoad",	
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
	"",			
};


BYTE		*EXEC_ScriptBuf;						
EXEC_DATA 	*EXEC_ScriptInfo;					
char		NowScriptFileName[16] = "";			
int			RunScriptBlockNo = -1;
int			PrevRunScriptBlockNo = -1;







BOOL EXEC_ReadScript( char *filename, EXEC_DATA *scr )
{
	SCRIPT_HEADER	*header;
	int				i, size;
	char			*buf=NULL;
	
	
	size = PAC_LoadFile( PackScriptDir, filename, (BYTE**)&buf );
	if(size==0){
		return FALSE;
	}
	
	header = (SCRIPT_HEADER*)buf;
	if( ( header->h1 != 'L' ) || ( header->h2 != 'F' ) ) {
		DebugBox( NULL, "スクリプトファイルではありません。" );
		GlobalFree( buf );
		return FALSE;
	}

	
	size = header->Fsize - sizeof( SCRIPT_HEADER );		
	if( scr->ScriptBuf ) scr->ScriptBuf = (char *)GlobalFree(scr->ScriptBuf);
	scr->ScriptBuf = (char *)GlobalAlloc( GPTR, size );
	CopyMemory( scr->ScriptBuf, buf + sizeof(SCRIPT_HEADER), size );

	
	for( i=0; i<SCRIPT_BLOCK_MAX; i++ ) { scr->BlockAddres[i] = header->BlockAddres[i]; }

	
	scr->BusyFlg  = SCCODE_NOOPR;								

	wsprintf( NowScriptFileName, "%s", filename );

	GlobalFree( buf );

	return( TRUE );
}







BOOL EXEC_StartScript( EXEC_DATA *scr, int scr_no )
{
	int		i;
	
	
	if( (scr->BusyFlg != SCCODE_NOOPR) && (scr->BusyFlg != SCCODE_WAIT_SLOAD) )    { return( FALSE ); }	
	if( scr->BlockAddres[ scr_no ] == 0 ) { return( FALSE ); }	


	
	scr->BusyFlg = SCCODE_RUN;									
	scr->pc      = scr->BlockAddres[ scr_no ] - 1;				
	scr->sp      = 0;											
	for( i=0; i<REGISTER_MAX; i++ ) { scr->reg[ i ] = 0; }		
	RunScriptBlockNo = scr_no;

	return( TRUE );
}






void EXEC_ReleaseScript( EXEC_DATA *scr )
{
	int	i;
	
	if(scr==NULL) return ;
	if( scr->ScriptBuf )	GlobalFree( scr->ScriptBuf );

	
	scr->ScriptBuf = NULL;
	scr->BusyFlg   = SCCODE_NOOPR;
	scr->pc        = 0;

	scr->sp      = 0;										
	for( i=0; i<STACK_MAX; i++ ) { scr->sp_buf[ i ] = 0; }	
	for( i=0; i<REGISTER_MAX; i++ ) { scr->reg[ i ] = 0; }	
	
	RunScriptBlockNo = -1;
}






void EXEC_EndScript( EXEC_DATA *scr, int init )
{
	
	scr->BusyFlg   = SCCODE_NOOPR;
	scr->pc        = 0;

	if(!init){
		if( scr->sp != 0 )
			DebugBox( NULL, "Script Error : Stack Pointer Abnormal" );
	}
	RunScriptBlockNo = -1;
}






short EXEC_GetShortValue( BYTE *buf )
{
	int		i;
	union {
		char	byte[ 2 ];
		WORD	word;
	}val;

	
	for( i=0; i<2; i++ ) { val.byte[ i ] = buf[ i ]; }
	
	return(	val.word );
}






long EXEC_GetLongValue( BYTE *buf )
{
	long	i;
	union {
		char byte[ 4 ];
		long dword;
	}val;

	
	for( i=0; i<4; i++ ) { val.byte[ i ] = buf[ i ]; }

	return(	val.dword );
}







int EXEC_GetValue( BYTE *buf, int mode )
{
	int 	value;
	
	
	if( mode == 0 )	value = EXEC_ScriptInfo->reg[ *buf ];	
	else			value = EXEC_GetLongValue( buf );		

	return( value );
}






static void EXEC_ScriptWait( void )
{
	
	if( EXEC_ScriptInfo->WaitCounter < EXEC_ScriptInfo->WaitEnd )	EXEC_ScriptInfo->WaitCounter++;
	else														EXEC_ScriptInfo->BusyFlg = SCCODE_RUN;
}






static void EXEC_ScriptTWait( void )
{
	
	if( timeGetTime() >= EXEC_ScriptInfo->TWaitEnd )	EXEC_ScriptInfo->BusyFlg = SCCODE_RUN;
}






static void EXEC_SpPush( int num )
{
	
	EXEC_ScriptInfo->sp_buf[ EXEC_ScriptInfo->sp ] = num;
	EXEC_ScriptInfo->sp++;
}






static int EXEC_SpPop( void )
{
	
	EXEC_ScriptInfo->sp--;
	return( EXEC_ScriptInfo->sp_buf[ EXEC_ScriptInfo->sp ] );
}






static void EXEC_OprMov( char mode )
{
	int		param1, param2;
	
	
	param1 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];							
	param2 = EXEC_GetValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 3 ], mode );	
	
	
	EXEC_ScriptInfo->reg[ param1 ] = param2;

	
	if( mode == 0 )	EXEC_AddPC( 4 );	
	else			EXEC_AddPC( 7 );	
}






static void EXEC_OprSwap( void )
{
	int		param1, param2, work;

	
	param1 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];		
	param2 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 3 ];		

	
	work                          = EXEC_ScriptInfo->reg[ param1 ];
	EXEC_ScriptInfo->reg[ param1 ] = EXEC_ScriptInfo->reg[ param2 ];
	EXEC_ScriptInfo->reg[ param2 ] = work;
	
	
	EXEC_AddPC( 4 );
}





static void EXEC_OprRand( void )
{
	int		param1;
	
	
	param1 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];
	
	
	EXEC_ScriptInfo->reg[ param1 ] = ( rand() % 65535 );

	
	EXEC_AddPC( 3 );
}






static void EXEC_OprIf( char mode )
{
	int		param1, param2, param3, p, flg;
	
	
	param1 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];							
	param1 = EXEC_GetParam( param1 );											
	param2 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 3 ];							
	param3 = EXEC_GetValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 4 ], mode );	

	
	switch( param2 ) {
		case CALC_SMALL:	flg = ( param1 <  param3 ) ? 1 : 0;		break;		
		case CALC_ESMALL:	flg = ( param1 <= param3 ) ? 1 : 0;		break;		
		case CALC_LARGE:	flg = ( param1 >  param3 ) ? 1 : 0;		break;		
		case CALC_ELARGE:	flg = ( param1 >= param3 ) ? 1 : 0;		break;		
		case CALC_EQUAL:	flg = ( param1 == param3 ) ? 1 : 0;		break;		
		case CALC_NEQUAL:	flg = ( param1 != param3 ) ? 1 : 0;		break;		
	}

	
	if( flg ){	
		p = ( mode == 0 ) ? 5 : 8;
		EXEC_ScriptInfo->pc = EXEC_GetLongValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + p ] );
	}
	else{		
		EXEC_AddPC( ( mode == 0 ) ? 9 : 12 );
	}
}






static void EXEC_OprIfElse( char mode )
{
	int		param1, param2, param3, p;
	char	flg;
	
	
	param1 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];							
	param1 = EXEC_GetParam( param1 );											
	param2 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 3 ];							
	param3 = EXEC_GetValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 4 ], mode );	

	
	switch( param2 ) {
		case CALC_SMALL:	flg = ( param1 <  param3 ) ? 1 : 0;		break;		
		case CALC_ESMALL:	flg = ( param1 <= param3 ) ? 1 : 0;		break;		
		case CALC_LARGE:	flg = ( param1 >  param3 ) ? 1 : 0;		break;		
		case CALC_ELARGE:	flg = ( param1 >= param3 ) ? 1 : 0;		break;		
		case CALC_EQUAL:	flg = ( param1 == param3 ) ? 1 : 0;		break;		
		case CALC_NEQUAL:	flg = ( param1 != param3 ) ? 1 : 0;		break;		
	}

	
	if( flg )	p = ( mode == 0 ) ? 5 : 8;
	else		p = (( mode == 0 ) ? 5 : 8) + 4;
	EXEC_ScriptInfo->pc = EXEC_GetLongValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + p ] );
}






static void EXEC_OprLoop( void )
{
	int		param1;

	
	param1 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];	

	
	if( EXEC_ScriptInfo->reg[ param1 ] > 0 ) {			
		EXEC_ScriptInfo->reg[ param1 ]--;
		EXEC_ScriptInfo->pc = EXEC_GetLongValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 3 ] );
	}else{
		EXEC_AddPC( 7 );
	}
}






static void EXEC_OprGoto( void  )
{
	
	EXEC_ScriptInfo->pc = EXEC_GetLongValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ] );
}






static void EXEC_OprArithmetic1( int opr )
{
	int		param1;

	
	param1 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];		

	
	switch( opr ) {
		case EXEC_INC:	EXEC_ScriptInfo->reg[ param1 ] ++;								break;
		case EXEC_DEC:	EXEC_ScriptInfo->reg[ param1 ] --;								break;
		case EXEC_NOT:	EXEC_ScriptInfo->reg[ param1 ] = ~EXEC_ScriptInfo->reg[ param1 ];	break;
		case EXEC_NEG:	EXEC_ScriptInfo->reg[ param1 ] *= -1;							break;
	}

	
	EXEC_AddPC( 3 );
}






static void EXEC_OprArithmetic2( int opr )
{
	int		param1, val1, val2 = 0, ans = 0, mode;

	
	param1 = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];		
	
	val1 = EXEC_GetValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ], 0 );
	switch( opr ) {
		case EXEC_ADDR:	case EXEC_SUBR:	case EXEC_MULR:	case EXEC_DIVR:
		case EXEC_MODR:	case EXEC_ANDR:	case EXEC_ORR:	case EXEC_XORR:
						val2 = EXEC_GetValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 3 ], 0 );
						mode = 0;
						break;
		case EXEC_ADDV:	case EXEC_SUBV:	case EXEC_MULV:	case EXEC_DIVV:
		case EXEC_MODV:	case EXEC_ANDV:	case EXEC_ORV:	case EXEC_XORV:
						val2 = EXEC_GetValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 3 ], 1 );
						mode = 1;
						break;
	}

	
	switch( opr ) {
		case EXEC_ADDR:	case EXEC_ADDV:	ans = val1 + val2;							break;
		case EXEC_SUBR:	case EXEC_SUBV:	ans = val1 - val2;							break;
		case EXEC_MULR:	case EXEC_MULV:	ans = val1 * val2;							break;
		case EXEC_DIVR:	case EXEC_DIVV:	ans = ( val2 != 0 ) ? ( val1 / val2 ) : 0;	break;
		case EXEC_MODR:	case EXEC_MODV:	ans = ( val2 != 0 ) ? ( val1 % val2 ) : 0;	break;
		case EXEC_ANDR:	case EXEC_ANDV:	ans = val1 & val2;							break;
		case EXEC_ORR:	case EXEC_ORV:	ans = val1 | val2;							break;
		case EXEC_XORR:	case EXEC_XORV:	ans = val1 ^ val2;							break;
	}
	EXEC_ScriptInfo->reg[ param1 ] = ans;

	
	EXEC_AddPC( ( mode == 0 ) ? 4 : 7 );
}






static void EXEC_OprCalc( void )
{
	short	size;
	int		reg, bp, sbp, s1, s2, ans, cnt;
	struct CALC{
		char	opr;
		long	data;
	} Calc[ 250 ];
	
	
	reg  = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];
	size = EXEC_GetShortValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 3 ] );

	
	bp = sbp = 0;
	while( 1 ) {
		if( sbp >= size ) { break; }
		if( EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 5 + sbp ] == FACT_VAL ) {		
			Calc[ bp ].opr = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 5 + sbp ];
			Calc[ bp ].data = EXEC_GetLongValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc+3+sbp+1 ] );
			bp++;	sbp += 5;
		}
		else {																	
			Calc[ bp ].opr = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 5 + sbp ];
			Calc[ bp ].data = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 5 + sbp + 1 ];
			bp++;	sbp += 2;
		}
	}
	Calc[ bp ].opr = FACT_END;

	
	bp = 0;
	while( Calc[ bp ].opr != FACT_END ) {
		if( Calc[ bp ].opr != FACT_CLC ) { bp++; continue; }
		if( Calc[ bp - 2 ].opr == FACT_VAL ) { s1 = Calc[ bp - 2 ].data;                        }
		else                                  { s1 = EXEC_ScriptInfo->reg[ Calc[ bp - 2 ].data ]; }
		if( Calc[ bp - 1 ].opr == FACT_VAL ) { s2 = Calc[ bp - 1 ].data;                        }
		else                                  { s2 = EXEC_ScriptInfo->reg[ Calc[ bp - 2 ].data ]; }
		switch( Calc[ bp ].data ) {
			case CALC_ADD:	ans = s1 + s2;							break;
			case CALC_SUB:	ans = s1 - s2;							break;
			case CALC_MUL:	ans = s1 * s2;							break;
			case CALC_DIV:	ans = ( s2 != 0 ) ? ( s1 / s2 ) : 0;	break;
			case CALC_MOD:	ans = ( s2 != 0 ) ? ( s1 % s2 ) : 0;	break;
		}
		Calc[ bp ].opr = FACT_VAL;
		Calc[ bp ].data = ans;
		for( cnt=0; Calc[ bp + cnt ].opr != FACT_END; cnt++ ) {
			Calc[ bp + cnt - 2 ].opr = Calc[ bp + cnt ].opr;
			Calc[ bp + cnt - 2 ].data = Calc[ bp + cnt ].data;
		}
		Calc[ bp + cnt - 2 ].opr = FACT_END;
		bp -= 2;
	}
	EXEC_SetParam( reg, Calc[ 0 ].data );

	
	EXEC_AddPC( 5 + size );
}






static void EXEC_Pusha( void )
{
	int		i;
	
	for( i=0; i<REGISTER_MAX; i++ ) { EXEC_SpPush( EXEC_ScriptInfo->reg[ i ] ); }
}
static void EXEC_OprPusha( void )
{
	EXEC_Pusha();
	
	EXEC_AddPC( 2 );
}







static void EXEC_Popa( void )
{
	int		i;
	
	for( i=REGISTER_MAX-1; i>=0; i-- ) { EXEC_ScriptInfo->reg[ i ] = EXEC_SpPop(); }
}
static void EXEC_OprPopa( void )
{
	EXEC_Popa();
	
	EXEC_AddPC( 2 );
}






static void EXEC_OprCall( void )
{
	int			callno;
	DWORD		addres;

	
	EXEC_SpPush( EXEC_ScriptInfo->pc + 3 );
	
	
	callno = (char)EXEC_ScriptBuf[ EXEC_ScriptInfo->pc+2 ];
	if( EXEC_ScriptInfo->BlockAddres[ callno ] == 0 ){
		DebugBox( NULL, "Script Call Error : ScriptBlock(%d) Code None", callno );
		return;
	}
	addres = EXEC_ScriptInfo->BlockAddres[ callno ] - 1;
	PrevRunScriptBlockNo = RunScriptBlockNo;
	RunScriptBlockNo = callno;

	
	EXEC_ScriptInfo->pc = addres;
}
void EXEC_OprCallFunc( int call_no, int next_pc, long *num )
{
	DWORD	addres;
	int		i;

	
	EXEC_SpPush( EXEC_ScriptInfo->pc + next_pc );
	
	for( i=0; i<14 ; i++ ){
		EXEC_ScriptInfo->reg[i] = num[i];
	}

	if( EXEC_ScriptInfo->BlockAddres[ call_no ] == 0 ){
		DebugBox( NULL, "Script Call Error : ScriptBlock(%d) Opr None", call_no );
		return;
	}
	addres = EXEC_ScriptInfo->BlockAddres[ call_no ] - 1;
	PrevRunScriptBlockNo = RunScriptBlockNo;
	RunScriptBlockNo = call_no;

	
	EXEC_ScriptInfo->pc = addres;
}







static void EXEC_OprRet( void )
{
	
	EXEC_ScriptInfo->pc = EXEC_SpPop();
	RunScriptBlockNo = PrevRunScriptBlockNo;
}






static void EXEC_OprWait( void )
{
	int		param1;
	
	
	param1 = EXEC_GetShortValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ] );		
	
	
	EXEC_ScriptInfo->WaitCounter = 0;					
	EXEC_ScriptInfo->WaitEnd     = param1;				
	EXEC_ScriptInfo->BusyFlg     = SCCODE_WAIT_WAIT;		
	
	
	EXEC_AddPC( 4 );
}






static void EXEC_OprTWait( void )
{
	int		param1;
	
	
	param1 = EXEC_GetShortValue( &EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ] );		
	
	
	EXEC_ScriptInfo->TWaitEnd   = timeGetTime()+(DWORD)param1;	
 	EXEC_ScriptInfo->BusyFlg    = SCCODE_WAIT_TWAIT;				

	
	EXEC_AddPC( 4 );
}






static void EXEC_OprRun( void )
{
	ESC_SetDrawFlag();
	
	EXEC_AddPC( 2 );
}

void EXEC_SLoad( char *str )
{
	
	EXEC_ReleaseScript( EXEC_ScriptInfo );
	
	
	EXEC_ReadScript( str, EXEC_ScriptInfo );

	EXEC_ScriptInfo->BusyFlg = SCCODE_WAIT_SLOAD;

	EXEC_StartScript( EXEC_ScriptInfo, 0 );
}





void EXEC_OprSLoad( void )
{
	int		i, len;
	char	str[256];

	
	len = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 2 ];						
	for( i=0; i<len; i++ ) { str[ i ] = EXEC_ScriptBuf[ EXEC_ScriptInfo->pc + 3 + i ]; }
	str[i] = '\0';	str[i+1] = '\0';

	EXEC_SLoad( str );
}






static BOOL EXEC_OprControl( int opr )
{
	BOOL	ret;

	
	switch( opr ) {
		case EXEC_END:		EXEC_EndScript( EXEC_ScriptInfo,OFF);	ret = FALSE;	break;	
		case EXEC_MOVR:		EXEC_OprMov( 0 );					ret = TRUE;		break;	
		case EXEC_MOVV:		EXEC_OprMov( 1 );					ret = TRUE;		break;	
		case EXEC_SWAP:		EXEC_OprSwap();						ret = TRUE;		break;	
		case EXEC_RAND:		EXEC_OprRand();						ret = TRUE;		break;	
		case EXEC_IFR:		EXEC_OprIf( 0 );						ret = TRUE;		break;	
		case EXEC_IFV:		EXEC_OprIf( 1 );						ret = TRUE;		break;	
		case EXEC_IFELSER:	EXEC_OprIfElse( 0 );					ret = TRUE;		break;	
		case EXEC_IFELSEV:	EXEC_OprIfElse( 1 );					ret = TRUE;		break;	
		case EXEC_LOOP:		EXEC_OprLoop();						ret = TRUE;		break;	
		case EXEC_GOTO:		EXEC_OprGoto();						ret = TRUE;		break;	

		case EXEC_INC:     																
		case EXEC_DEC:     																
		case EXEC_NOT:     																
		case EXEC_NEG:		EXEC_OprArithmetic1( opr );			ret = TRUE;		break;	

		case EXEC_ADDR:	case EXEC_ADDV:													
		case EXEC_SUBR:	case EXEC_SUBV:													
		case EXEC_MULR:	case EXEC_MULV:													
		case EXEC_DIVR:	case EXEC_DIVV:													
		case EXEC_MODR:	case EXEC_MODV:													
		case EXEC_ANDR:	case EXEC_ANDV:													
		case EXEC_ORR:	case EXEC_ORV:													
		case EXEC_XORR:	case EXEC_XORV:													
							EXEC_OprArithmetic2( opr );			ret = TRUE;		break;	
		case EXEC_CALC:		EXEC_OprCalc();						ret = TRUE;		break;	
		case EXEC_PUSHA:		EXEC_OprPusha();						ret = TRUE;		break;	
		case EXEC_POPA:		EXEC_OprPopa();						ret = TRUE;		break;	
		case EXEC_CALL:		EXEC_OprCall();						ret = TRUE;		break;	

		case EXEC_RET:		EXEC_OprRet();						ret = TRUE;		break;	
		case EXEC_WAIT:		EXEC_OprWait();						ret = FALSE;	break;	
		case EXEC_TWAIT:		EXEC_OprTWait();						ret = FALSE;	break;	
		case EXEC_RUN:		EXEC_OprRun();						ret = FALSE;	break;	
		case EXEC_SLOAD:		EXEC_OprSLoad();						ret = FALSE;	break;	
	}

	return(	ret );
}






static BOOL EXEC_CallOprControl( int mode )
{
	int		opr;
	BOOL	ret;
	
	
	opr = *(WORD*)&EXEC_ScriptBuf[ EXEC_ScriptInfo->pc ];							




	if( opr < EXEC_OPREND ){	ret = EXEC_OprControl( opr );	}			
	else{																
		switch( mode ){													
			case SCRMODE_EVENT:	ret = ESC_OprControl( opr );	break;	
		}
	}

	return( ret );
}






static int EXEC_ControlTask( int mode )
{
	
	if( EXEC_ScriptInfo->BusyFlg == SCCODE_WAIT_TWAIT )		EXEC_ScriptTWait();
	if( EXEC_ScriptInfo->BusyFlg == SCCODE_WAIT_WAIT )		EXEC_ScriptWait();

	
	if( EXEC_ScriptInfo->BusyFlg == SCCODE_RUN ){			
		while( EXEC_CallOprControl( mode ) );
	}

	
	return( EXEC_ScriptInfo->BusyFlg );
}






int EXEC_ControlScript( EXEC_DATA *scr )
{
	int		ret;

	
	EXEC_ScriptBuf  = (BYTE *)scr->ScriptBuf;
	EXEC_ScriptInfo = scr;
	
	
	ret = EXEC_ControlTask( SCRMODE_EVENT );		

	return( ret );
}



