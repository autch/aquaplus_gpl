#include "ClCommon.h"
#include <io.h>

#include "escr.h"
#include "escript.h"

#include "graphic.h"
#include "backLog.h"
#include "msgWnd.h"
#include "saveWnd.h"
#include "soundDS.h"
#include "select.h"

BYTE ScenarioFlag[SCRIPT_MAX][BLOCK_MAX/8];
static int ScenarioNo = -1;
static int BlockNo    = 0;

void InitReadFlag()
{
	DWORD	fSize;
	ZeroMemory(ScenarioFlag,SCRIPT_MAX*(BLOCK_MAX/8));
	if(0==_access("save_99.sav",0)){
		sysInf.configHF = CreateFile("save_99.sav",GENERIC_WRITE|GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
		ReadFile(sysInf.configHF,ScenarioFlag,SCRIPT_MAX*(BLOCK_MAX/8),&fSize,NULL);
	}else{
		sysInf.configHF = CreateFile("save_99.sav",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,NULL);
		WriteFile(sysInf.configHF,ScenarioFlag,SCRIPT_MAX*(BLOCK_MAX/8),&fSize,NULL);
	}
}


void AVG_SetScenarioNo( int scenario_no )
{
	if(-1 != ScenarioNo){
		DWORD fSize;
		set1BitFlag(ScenarioFlag[ScenarioNo], BlockNo, on);
		SetFilePointer(sysInf.configHF, ScenarioNo*BLOCK_MAX/8, NULL, FILE_BEGIN);
		WriteFile(sysInf.configHF,ScenarioFlag[ScenarioNo],BLOCK_MAX/8,&fSize,NULL);
	}
	ScenarioNo = scenario_no;
	BlockNo = 0;
}


void AVG_SetScenarioFlag( int block_no )
{
	if(block_no < 0)return;
	if( block_no >= BLOCK_MAX ){
		myOutputDebugString("ちょぉいと文章が多過ぎやしませんか？[%d]",block_no);
	}else{
		DWORD fSize;
		set1BitFlag(ScenarioFlag[ScenarioNo], BlockNo, on);
		SetFilePointer(sysInf.configHF, ScenarioNo*BLOCK_MAX/8, NULL, FILE_BEGIN);
		WriteFile(sysInf.configHF,ScenarioFlag[ScenarioNo],BLOCK_MAX/8,&fSize,NULL);
	}
	BlockNo = block_no;
}


BOOL AVG_CheckScenarioFlag()
{
	BOOL sw = look1BitFlag(ScenarioFlag[ScenarioNo], BlockNo);
	return sw;
}




int Str2Hex( char *str )
{
	int		i=0;
	int		digit=0;
	char	str2[256];
	if( str== NULL   ) return 0;							
	if( str[0]=='\0' ) return 0;							

	strcpy(str2,str);
	while( str2[i] ){
		str2[i] = (char)tolower( str2[i] );
		if( !isdigit(str2[i]) && !('a'<=str[i] && str[i]>='f') )
			return 0;	
		i++;
	}

	i=0;
	while( str2[i] ){
		digit *= 16;

		if( isdigit(str2[i]) )	digit += str[i]-'0';
		else					digit += str[i]-'a'+10;
		i++;
	}

	return digit;
}



typedef struct{
	int		type;	
	int		factor;	
	void	*next;
}CAL_LIST;

static char	*CalStr=NULL;
static int	CalP=0;

static CAL_LIST *NewList( int type, int factor )
{
	CAL_LIST	*list = (CAL_LIST *)cl_malloc( sizeof(CAL_LIST) );

	list->type   = type;
	list->factor = factor;
	list->next   = NULL;

	return list;
}

static void AddList( CAL_LIST *list, CAL_LIST *add_list )
{
	while(list->next){list = (CAL_LIST *)list->next;}
	list->next     = add_list;

}

static void ReleaseList( CAL_LIST *list )
{
	CAL_LIST *next;

	if(list){
		next = (CAL_LIST *)list->next;
		cl_free(list);
		ReleaseList( next );
	}
	return ;
}

static CAL_LIST *factor( void )
{
	BOOL		minus = FALSE;
	int			digit;
	char		buf[256], i=0;

	while( CalStr[CalP]=='-' ){
		minus = !minus;
		CalP++;
	}
	if( !strncmp(&CalStr[CalP],"0x",2) ){
		CalP+=2;
		while( isdigit(CalStr[CalP]) ||
				('a'<=CalStr[CalP] && CalStr[CalP]<='f') ||
				('A'<=CalStr[CalP] && CalStr[CalP]<='F') ){
			buf[i++]=CalStr[CalP++];
		}
		buf[i] = '\0';
		digit = Str2Hex(buf);
	}else{
		while( isdigit(CalStr[CalP]) ){
			buf[i++] = CalStr[CalP++];
		}
		buf[i]='\0';
		digit = atoi(buf);
	}
	if(minus) digit = -digit;

	return NewList( 0, digit );
}

static CAL_LIST *expr1( void )
{
	CAL_LIST	*list = factor();
	int			type;

	while( CalStr[CalP]=='*' || CalStr[CalP]=='/' || CalStr[CalP]=='%' ){
		type = CalStr[CalP];
		CalP++;
		AddList( list, factor() );
		AddList( list, NewList(1,type) );
	}
	return list;
}

static CAL_LIST *expr0( void )
{
	CAL_LIST	*list = expr1();
	int			type;

	while( CalStr[CalP]=='+' || CalStr[CalP]=='-' ){
		type = CalStr[CalP];
		CalP++;
		AddList( list, expr1() );
		AddList( list, NewList(1,type) );
	}
	return list;
}

static int StackDigit[32];
static int StackP=0;
static int	GoCal2Digit( char *cal )
{
	CAL_LIST	*start_list, *work_list;
	int			opr1, opr2;

	CalStr=cal;
	CalP=0;
	work_list = start_list = expr0();

	while(work_list){
		if(work_list->type){
			opr1 = StackDigit[--StackP];
			opr2 = StackDigit[--StackP];

			switch(work_list->factor){
				case '-':	opr1 = opr2-opr1;	break;
				case '+':	opr1 = opr2+opr1;	break;
				case '*':	opr1 = opr2*opr1;	break;
				case '/':	opr1 = opr2/opr1;	break;
				case '%':	opr1 = opr2%opr1;	break;
			}
			StackDigit[StackP] = opr1;
			StackP++;
		}else{
			StackDigit[StackP] = work_list->factor;
			StackP++;
		}
		work_list = (CAL_LIST *)work_list->next;
	}

	ReleaseList( start_list );
	StackP=0;
	return StackDigit[0];
}


int	Cal2Disgit( char *cal )
{
	char	buf[256];
	int		i=0, j=0;
	int		flag=0;

	while(1){
		buf[j] = cal[i];

		switch(cal[i]){
			case '\0':
				return GoCal2Digit( buf );
			case '[':
				j+=wsprintf( &buf[j], "%d", Cal2Disgit( &cal[i+1] ) )-1;
				while(cal[i]!=']'){ i++; }
				break;
			case ']':
				buf[i]='\0';
				return GoCal2Digit( buf );
		}
		i++;
		j++;
	}
	return 0;
}

void ESC_SetDrawFlag(void)
{
	sysInf.drawFlag=on;
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

static int GetLangParamCal( char *cal )
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
				j += wsprintf( &buf[j], "%d", EXEC_LangInfo->reg[ reg ] );
			}
		}else{
			buf[j++] = cal[i++];
		}
	}
	buf[j]='\0';

	return Cal2Disgit( buf );
}




static int GetLangParam( int pc, int *start,int size )
{
	int mode;
	int	ret;

	mode = EXEC_LangBuf[ pc + (*start)++ ];						
	if(mode==2){
		int		i,len;
		char	buf[256];

		len   = EXEC_LangBuf[ pc + (*start)++ ];		
		for( i=0; i<len; i++ ) { buf[ i ] = EXEC_LangBuf[ pc + (*start)++ ]; }
		buf[ i ] = 0x00;
		ret = GetLangParamCal( buf );
	}else{		
		switch( size ){
			default:
			case 1:	ret  = (unsigned char)EXEC_LangBuf[ pc + *start ];			(*start)+=1;	break;	
			case 2: ret  = EXEC_GetShortValue( &EXEC_LangBuf[ pc + *start ] );	(*start)+=2;	break;	
			case 4: ret  = EXEC_GetLongValue(  &EXEC_LangBuf[ pc + *start ] );	(*start)+=4;	break;	
		}
		if( mode == 0 ) { ret = EXEC_LangInfo->reg[ ret ]; }					
	}
	return ret;
}



static int GetLangParamByte( int pc, int *start )
{
	return EXEC_LangBuf[ pc + (*start)++ ];						
}



static int GetLangParamWord( int pc, int *start )
{
	int	ret;

	ret  = EXEC_LangBuf[ pc + (*start)++ ];	
	ret |= EXEC_LangBuf[ pc + (*start)++ ]<<8;	
	return ret;
}



static int GetLangParam_str( int pc, int *start, char *buf )
{
	int	i,len;

	len   = EXEC_LangBuf[ pc + (*start)++ ];		
	for( i=0; i<len; i++ ) { buf[ i ] = EXEC_LangBuf[ pc + (*start)++ ]; }
	buf[ i ] = 0x00;
	return *start;
}



static int GetLangParam_str2( int pc, int *start, char *buf )
{
	int	i,len;

	len  = EXEC_LangBuf[ pc + (*start)++ ];	
	len |= EXEC_LangBuf[ pc + (*start)++ ]<<8;	
	for( i=0; i<len; i++ ) { buf[ i ] = EXEC_LangBuf[ pc + (*start)++ ]; }
	buf[ i ] = 0x00;
	return *start;
}




static int GetLangParamControl( int opr )
{
	int		i;
	int		cnt = 2;

	if(opr<64)	myOutputDebugString("イベントスクリプトの命令コードとして不適です[%d]",opr);
	opr-=64;

	for(i=0;i<15;i++){
		switch( EScroptOpr[opr].type[i] ){
			default:	myOutputDebugString("引数タイプが不適です[%d]",EScroptOpr[opr].type[i]);	
			case ESC_NOT:	return cnt;												
			case ESC_REG:	
			case ESC_ADD:	
			case ESC_ASC:	EscParam[i].num = GetLangParamByte( EXEC_LangInfo->pc, &cnt );	break;	
			case ESC_CNT:	EscParam[i].num = GetLangParamWord( EXEC_LangInfo->pc, &cnt );	break;	
			case ESC_NUM:	EscParam[i].num = GetLangParam( EXEC_LangInfo->pc, &cnt, 4 );	break;	
			case ESC_STR:	GetLangParam_str( EXEC_LangInfo->pc, &cnt, EscParam[i].str );	break;	
			case ESC_STR2:	GetLangParam_str2( EXEC_LangInfo->pc, &cnt, EscParam[i].str );	break;	
			case ESC_CMP:
				{
					int	param1, param2, param3;
					param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + cnt++ ];	
					param1 = EXEC_GetParam( param1 );						
					param2 = EXEC_LangBuf[ EXEC_LangInfo->pc + cnt++ ];	
					param3 = GetLangParam( EXEC_LangInfo->pc, &cnt, 4 );						
					
					switch( param2 ) {
						case CALL_SMALL:	EscParam[i].num = ( param1 <  param3 ) ? 1 : 0;		break;		
						case CALL_ESMALL:	EscParam[i].num = ( param1 <= param3 ) ? 1 : 0;		break;		
						case CALL_LARGE:	EscParam[i].num = ( param1 >  param3 ) ? 1 : 0;		break;		
						case CALL_ELARGE:	EscParam[i].num = ( param1 >= param3 ) ? 1 : 0;		break;		
						case CALL_EQUAL:	EscParam[i].num = ( param1 == param3 ) ? 1 : 0;		break;		
						case CALL_NEQUAL:	EscParam[i].num = ( param1 != param3 ) ? 1 : 0;		break;		
					}
				}
				break;	
		}
	}
	return cnt;
}



static int GetLangParamControlEx( int pc, int opr, char *str )
{
	int		i;
	int		cnt = 2,flag=0;
	char	buf[32];

	if(opr<64)	myOutputDebugString("イベントスクリプトの命令コードとして不適です[%d]",opr);
	opr-=64;

	for(i=0;i<15;i++){
		flag = 1;
		switch( EScroptOpr[opr].type[i] ){
			default:	myOutputDebugString("引数タイプが不適です[%d]",EScroptOpr[opr].type[i]);	
			case ESC_NOT:	return cnt;												
			case ESC_REG:	EscParam[i].num = GetLangParamByte( pc, &cnt );	wsprintf( buf, "REG%d, ", EscParam[i].num); 	break;	
			case ESC_ADD:	EscParam[i].num = GetLangParamByte( pc, &cnt );	wsprintf( buf, "ADD(%d), ", EscParam[i].num); 	break;	

			case ESC_ASC:	EscParam[i].num = GetLangParamByte( pc, &cnt );	wsprintf( buf, "%c, ", EscParam[i].num);	break;	
			case ESC_CNT:	EscParam[i].num = GetLangParamWord( pc, &cnt );	wsprintf( buf, "CNT(%d), ", EscParam[i].num);	break;	
			case ESC_NUM:	EscParam[i].num = GetLangParam( pc, &cnt, 4 );	wsprintf( buf, "%d, ", EscParam[i].num); 	break;	
			case ESC_STR:	GetLangParam_str( pc, &cnt, EscParam[i].str );	strcat(str,EscParam[i].str ); strcat(str,", " ); flag=0;	break;	
			case ESC_STR2:	GetLangParam_str2( pc, &cnt, EscParam[i].str );	strcat(str,EscParam[i].str ); strcat(str,", " ); flag=0;	break;	
			case ESC_CMP:
				{
					int	param1, param2, param3;
					param1 = EXEC_LangBuf[ pc + cnt++ ];	
					param1 = EXEC_GetParam( param1 );						
					param2 = EXEC_LangBuf[ pc + cnt++ ];	
					param3 = GetLangParam( pc, &cnt, 4 );						
					
					switch( param2 ) {
						case CALL_SMALL:	EscParam[i].num = ( param1 <  param3 ) ? 1 : 0;	wsprintf( buf, "REG%d < %d, ", param1, param3 );	break;		
						case CALL_ESMALL:	EscParam[i].num = ( param1 <= param3 ) ? 1 : 0;	wsprintf( buf, "REG%d <= %d, ", param1, param3 );	break;		
						case CALL_LARGE:	EscParam[i].num = ( param1 >  param3 ) ? 1 : 0;	wsprintf( buf, "REG%d > %d, ", param1, param3 );	break;		
						case CALL_ELARGE:	EscParam[i].num = ( param1 >= param3 ) ? 1 : 0;	wsprintf( buf, "REG%d >= %d, ", param1, param3 );	break;		
						case CALL_EQUAL:	EscParam[i].num = ( param1 == param3 ) ? 1 : 0;	wsprintf( buf, "REG%d == %d, ", param1, param3 );	break;		
						case CALL_NEQUAL:	EscParam[i].num = ( param1 != param3 ) ? 1 : 0;	wsprintf( buf, "REG%d != %d, ", param1, param3 );	break;		
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

	if(opr<64)	myOutputDebugString("イベントスクリプトの命令コードとして不適です[%d]",opr);
	opr-=64;

	for(i=0;i<15;i++){
		switch( EScroptOpr[opr].type[i] ){
			default:	myOutputDebugString("引数タイプが不適です[%d]",EScroptOpr[opr].type[i]);	
			case ESC_NOT:	return ;														
			case ESC_ASC:	break;															
			case ESC_NUM:	break;															
			case ESC_STR2:	break;
			case ESC_STR:	break;															
			case ESC_REG:	EXEC_SetParam( EscParam[i].num, EscParam[i].reg );	break;		
			case ESC_ADD:	break;															
			case ESC_CNT:	break;															
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
	if(1==EscParam[0].num && 1==EscParam[1].num){
		DWORD fSize;
		ScenarioFlag[SCRIPT_MAX-1][0] = 1;
		SetFilePointer(sysInf.configHF, (SCRIPT_MAX-1)*BLOCK_MAX/8, NULL, FILE_BEGIN);
		WriteFile(sysInf.configHF,ScenarioFlag[SCRIPT_MAX-1],1,&fSize,NULL);
	}
	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprGetFlag( void )
{
	
	EscParam[1].reg = ESC_GetFlag( EscParam[0].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetGameFlag( void )
{
	

	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprGetGameFlag( void )
{
	

	
	EXEC_AddPC( EscCnt );
}




static void LoadScript( char *fno )
{
	char	str[256];
	wsprintf( str, "%s.sdt", fno );
	
	toneConvert.ClearTable();
	EXEC_SLoad( str );
	AVG_SetScenarioNo( atoi(str) );
}
static void ESC_EOprLoadScript( void )
{
	LoadScript( EscParam[0].str );
}




static void LoadScriptNum( int	fno )
{
	char	str[256];
	wsprintf( str, "%05d.sdt", fno );
	
	toneConvert.ClearTable();
	EXEC_SLoad( str );
	AVG_SetScenarioNo( fno );

}
static void ESC_EOprLoadScriptNum( void )
{

	LoadScriptNum( EscParam[0].num );
}




static void ESC_EOprGameEnd( void )
{
	if( EscParam[0].num==-1 ) EscParam[0].num = 0;
	if(!EOprFlag[ESC_GAMEEND]){
		EOprFlag[ESC_GAMEEND] = 1;

	}
}



static void ESC_EOprCallFunc( void )
{
	long	num[14],i;

	for(i=0;i<14;i++) num[i] = EscParam[i+1].num;
	EXEC_OprCallFunc( EscParam[0].num, EscCnt, num );
}




static void ESC_EOprSetTimeMode( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 0;
	if(EscParam[1].num==-1) EscParam[1].num = 0;
	toneConvert.ChangeTable( EscParam[0].num, EscParam[1].num );

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetTimeBack( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 0;
	if(EscParam[1].num==-1) EscParam[1].num = 0;
	toneConvert.ChangeTable( EscParam[0].num, EscParam[1].num, TRUE );

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetTimeChar( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 0;
	if(EscParam[1].num==-1) EscParam[1].num = 0;


	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetMatrix( void )
{

	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprShowChipBack( void )
{
	if(!EOprFlag[ESC_SHOWCHIPBACK]){
		EOprFlag[ESC_SHOWCHIPBACK] = 1;
		
		if(EscParam[0].num==-1) EscParam[0].num = 0;	
		if(EscParam[1].num==-1) EscParam[1].num = -2;	


	}
	

		EOprFlag[ESC_SHOWCHIPBACK] = 0;
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprPoseChipBack( void )
{
	

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprPlayChipBack( void )
{
	

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetChipBack( void )
{
	

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetChipParts( void )
{

	if(EscParam[12].num==-1) EscParam[12].num = 0;
	




	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetChipScroll( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetChipScroll2( void )
{
	

	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetChipScrollSpeed( void )
{
	


	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprWaitChipScroll( void )
{
	

		
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprWaitChipScrollSpeed( void )
{
	

		
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprSetChipCharCash( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprResetChipCharCash( void )
{
	

	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetChipChar( void )
{
	


	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetChipCharAni( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprResetChipChar( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetChipCharParam( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetChipCharBright( void )
{
	

	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetChipCharMove( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetChipCharMove2( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetChipCharMoveSpeed( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprGetChipCharMove( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprGetChipCharMoveSpeed( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprCopyChipCharPos( void )
{
	

	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetChipCharRev( void )
{
	


	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprThroughChipCharAni( void )
{
	

	
	EXEC_AddPC( EscCnt );
}








static void ESC_EOprWaitChipCharRepeat( void )
{
	BOOL	through=FALSE;
	if(!EOprFlag[ESC_WAITCHIPCHARREPEAT]){
		EOprFlag[ESC_WAITCHIPCHARREPEAT] = 1;
		
		if(EscParam[1].num!=-1) through = EscParam[1].num;


	}
	
	

		EOprFlag[ESC_WAITCHIPCHARREPEAT] = 0;
		EXEC_AddPC( EscCnt );

}










static void ESC_EOprWaitChipCharAni( void )
{
	BOOL	through=FALSE;
	if(!EOprFlag[ESC_WAITCHIPCHARANI]){
		EOprFlag[ESC_WAITCHIPCHARANI] = 1;
		
		through = EscParam[1].num;
		if(EscParam[1].num==-1) through = on;

	}
	

		EOprFlag[ESC_WAITCHIPCHARANI] = 0;
		EXEC_AddPC( EscCnt );

}









static void ESC_EOprWaitChipCharAniLoop( void )
{
	BOOL	through=FALSE;
	if(!EOprFlag[ESC_WAITCHIPCHARANILOOP]){
		EOprFlag[ESC_WAITCHIPCHARANILOOP] = 1;

		through = EscParam[1].num;
		if(EscParam[1].num==-1) through = on;


	}
	

		EOprFlag[ESC_WAITCHIPCHARANILOOP] = 0;
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprWaitChipCharMove( void )
{
	

		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetBack( void )
{
	
	if(!EOprFlag[ESC_SETBACK]){
		EOprFlag[ESC_SETBACK] = 1;

	}
	

		EOprFlag[ESC_SETBACK] = 0;
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprSetBack2( void )
{
	
	if(!EOprFlag[ESC_SETBACK2]){
		EOprFlag[ESC_SETBACK2] = 1;

	}
	

		EOprFlag[ESC_SETBACK2] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprSetNoise( void )
{
	
	if(!EOprFlag[ESC_SETNOISE]){
		EOprFlag[ESC_SETNOISE] = 1;
		if(EscParam[1].num==-1) EscParam[1].num=32;
		if(EscParam[2].num==-1) EscParam[2].num=30;

	}
	

		EOprFlag[ESC_SETNOISE] = 0;
		EXEC_AddPC( EscCnt );

}




static void ESC_EOprGetBack( void )
{
	
	switch(EOprFlag[ESC_GETBACK]){
		case 0:
			EOprFlag[ESC_GETBACK] = 1;
			sysInf.drawFlag = 1;
			break;
		case 1:
			EOprFlag[ESC_GETBACK] = 0;

			
			EXEC_AddPC( EscCnt );
			break;
	}
}




static void ESC_EOprSetCg( void )
{
	
	if(!EOprFlag[ESC_SETCG]){
		EOprFlag[ESC_SETCG] = 1;

	}
	

		EOprFlag[ESC_SETCG] = 0;
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprSetCg2( void )
{
	
	if(!EOprFlag[ESC_SETCG2]){
		EOprFlag[ESC_SETCG2] = 1;

	}
	

		EOprFlag[ESC_SETCG2] = 0;
		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetBackPos( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBackPosZ( void )
{
	

	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetBackScroll( void )
{
	
	if(!EOprFlag[ESC_SETBACKSCROLL]){
		EOprFlag[ESC_SETBACKSCROLL] = 1;

	}
	

		EOprFlag[ESC_SETBACKSCROLL] = 0;
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprSetBackScrollZ( void )
{
	
	if(!EOprFlag[ESC_SETBACKSCROLLZ]){
		EOprFlag[ESC_SETBACKSCROLLZ] = 1;

	}
	

		EOprFlag[ESC_SETBACKSCROLLZ] = 0;
		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetBackCScope( void )
{
	

	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprWaitBackCScope( void )
{
	

		
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprLockBackCScope( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBackFadeIn( void )
{
	
	if(EscParam[1].num==-1) EscParam[1].num=off;
	if(!EOprFlag[ESC_SETBACKFADEIN]){
		EOprFlag[ESC_SETBACKFADEIN] = 1;

	}
	

		EOprFlag[ESC_SETBACKFADEIN] = 0;
		EXEC_AddPC( EscCnt );

}
 



static void ESC_EOprSetBackFadeOut( void )
{
	
	if(EscParam[1].num==-1) EscParam[1].num=off;
	if(!EOprFlag[ESC_SETBACKFADEOUT]){
		EOprFlag[ESC_SETBACKFADEOUT] = 1;

	}
	

		EOprFlag[ESC_SETBACKFADEOUT] = 0;
		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetBackFadeRGB( void )
{
	
	if(EscParam[4].num==-1) EscParam[4].num=off;
	if(!EOprFlag[ESC_SETBACKFADERGB]){
		EOprFlag[ESC_SETBACKFADERGB] = 1;

	}
	

		EOprFlag[ESC_SETBACKFADERGB] = 0;
		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetShake( void )
{
	
	if(!EOprFlag[ESC_SETSHAKE]){
		EOprFlag[ESC_SETSHAKE] = 1;
		if(EscParam[4].num==-1) EscParam[4].num=256;


	}
	if(bgInf.look_cnt==0){
		switch(EscParam[0].num){
		  case 2:
			bgInf.in_type = _BAK_ZOOMSHAK;
			break;
		  case 3:
			bgInf.in_type = _BAK_TXT_SHAKE;
			break;
		  case 9:
			bgInf.in_type = _BAK_RAND;
			break;
		  case 12:
			bgInf.in_type = _BAK_ROLL;
			break;
		  case 14:
			bgInf.in_type = _BAK_ROLL_2TI;
			break;
		  default:
			bgInf.in_type = _BAK_SHAKE;
			break;
		}
		bgInf.shakeSize = EscParam[1].num;
		if(EscParam[3].num==2 && bgInf.in_type==_BAK_ROLL) bgInf.shakeSize *= -1;
		bgInf.look_cnt = bgInf.look_max = EscParam[2].num *2;
		if(bgInf.look_max==0){
			bgInf.shake_cnt = 1;
			EOprFlag[ESC_SETSHAKE] = 0;
			EXEC_AddPC( EscCnt );
			return;
		}
	}
	bgInf.look_cnt --;
	if(bgInf.look_cnt<=0){
		bgInf.look_cnt = bgInf.shake_cnt = 0;
		bgInf.in_type = _BAK_DIRECT;
		EOprFlag[ESC_SETSHAKE] = 0;
		EXEC_AddPC( EscCnt );
	}
}



static void ESC_EOprStopShake( void )
{
	
	bgInf.look_cnt = bgInf.shake_cnt = 0;
	bgInf.in_type = _BAK_DIRECT;
	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetFlash( void )
{
	
	if(!EOprFlag[ESC_SETFLASH]){
		EOprFlag[ESC_SETFLASH] = 1;

	}
	

		EOprFlag[ESC_SETFLASH] = 0;
		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetChar( void )
{
	
	if(EscParam[2].num==-1) EscParam[2].num = 1;
	if(EscParam[3].num==-1) EscParam[3].num = 0;
	if(EscParam[4].num==-1) EscParam[4].num = 0;
	if(EscParam[5].num==-1) EscParam[5].num = 128;
	if(EscParam[6].num==-1) EscParam[6].num = 256;
	if(EscParam[7].num==-1) EscParam[7].num = 30; 
	


	
	c_disp(EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[7].num);
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprResetChar( void )
{
	
	if(EscParam[1].num==-1) EscParam[1].num = 0;
	if(EscParam[2].num==-1) EscParam[2].num = -1;
	

	c_cls(EscParam[0].num, EscParam[1].num);
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetCharPose( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetCharLocate( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetCharLayer( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprWaitChar( void )
{

		
	if(waitChar(EscParam[0].num)){
		EXEC_AddPC( EscCnt );
	}
}



static void ESC_EOprSetBlock(void)
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetWindow(void)
{
	
	if(!EOprFlag[ESC_SETWINDOW]){
		EOprFlag[ESC_SETWINDOW] = 1;
		if(EscParam[0].num==-1) EscParam[0].num = off;

	}
	

		EOprFlag[ESC_SETWINDOW] = 0;
		
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprResetWindow(void)
{
	
	if(!EOprFlag[ESC_RESETWINDOW]){
		EOprFlag[ESC_RESETWINDOW] = 1;
		if(EscParam[0].num==-1) EscParam[0].num = off;

	}
	

		EOprFlag[ESC_RESETWINDOW] = 0;
		
		
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprSetMessage(void)
{
	
	if(!EOprFlag[ESC_SETMESSAGE]){
		EOprFlag[ESC_SETMESSAGE] = 1;


	}
	

		EOprFlag[ESC_SETMESSAGE] = 0;
		
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprSetMessage2(void)
{
	static char *msgBuf = NULL;
	
	if(0==EOprFlag[ESC_SETMESSAGE2]){
		if(sysInf.bMsgCLS){
			msgWnd.MsgCLS();
			sysInf.bMsgCLS = off;
		}
		EOprFlag[ESC_SETMESSAGE2] = 1;
		msgWnd.logMsgLine = 1;
		msgBuf = EscParam[0].str;
		msgWnd.autoWaitStart = timeGetTime();
		msgWnd.autoWaitTime = 0;
		AVG_SetScenarioFlag( EscParam[2].num );
		if(sysInf.bReadSkip){
			sysInf.bAlreadyRead = AVG_CheckScenarioFlag();
		}else{
			sysInf.bAlreadyRead = off;
		}
		saveMessage(msgBuf);	
	}
	if(msgBuf){
		msgBuf = msgWnd.DisplayMessage((BYTE *)msgBuf);
	}
	if(NULL==msgBuf){
		switch(EOprFlag[ESC_SETMESSAGE2]){
		  case 1:
			backLog.SetBackLog(EscParam[0].str, msgWnd.logMsgLine);
			EOprFlag[ESC_SETMESSAGE2] = 2;
			if(EscParam[1].num==2){
				if(off==sysInf.bMsgAlreadyStop) sysInf.bMsgStop = on;
			}
			break;
		  case 2:

			if(FALSE==msgWnd.msgStopCheck()){

				if(EscParam[1].num==2){
					sysInf.bMsgCLS = on;
				}
				EOprFlag[ESC_SETMESSAGE2] = 0;
				
				EXEC_AddPC( EscCnt );
			}
			break;
		}
	}
}



static void ESC_EOprSetMessageEx(void)
{
	
	if(!EOprFlag[ESC_SETMESSAGEEX]){
		EOprFlag[ESC_SETMESSAGEEX] = 1;


	}
	

		EOprFlag[ESC_SETMESSAGEEX] = 0;
		
		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetChipMessage(void)
{
	


	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprAddMessage(void)
{
	
	if(!EOprFlag[ESC_ADDMESSAGE]){
		EOprFlag[ESC_ADDMESSAGE] = 1;

	}
	

		EOprFlag[ESC_ADDMESSAGE] = 0;
		
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprAddMessage2(void)
{
	ESC_EOprSetMessage2();

}



static void ESC_EOprSetMessageWait(void)
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprResetMessage(void)
{
	


	sysInf.drawFlag = on;

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetTextDisp(void)
{
	
	if(EscParam[1].num==-1) EscParam[1].num = off;

	if(EscParam[0].num){
		if(EscParam[1].num){

		}

	}else{
		if(EscParam[1].num){

		}

	}
	sysInf.drawFlag = on;
	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprWaitKey(void)
{
	

		
		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetSelectMes(void)
{
	if(EscParam[1].num==-1) EscParam[1].num = -1;
	if(EscParam[2].num==-1) EscParam[2].num = 0;

	BOOL bAdd = TRUE;
	if(EscParam[1].num>=0){
		if(ESC_GameFlagBuf[EscParam[1].num]!=EscParam[2].num)bAdd = FALSE;
	}
	if(bAdd)selectWnd.addSelect(EscParam[0].str);

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetSelect(void)
{
	
	if(!EOprFlag[ESC_SETSELECT]){
		EOprFlag[ESC_SETSELECT] = 1;
		selectWnd.SelectStart();
		



	}
	
	EscParam[0].reg = selectWnd.selectExec();

	if( EscParam[0].reg!=-1 ){
		EOprFlag[ESC_SETSELECT] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}
void ESC_SelectEnd(void)
{
	EOprFlag[ESC_SETSELECT] = 0;
}



static void ESC_EOprSetSelectMesEx(void)
{
	if(EscParam[2].num==-1) EscParam[2].num = -1;
	if(EscParam[3].num==-1) EscParam[3].num = 0;


	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetSelectEx(void)
{
	char	*ret;
	
	if(!EOprFlag[ESC_SETSELECT]){
		EOprFlag[ESC_SETSELECT] = 1;

	}
	


		EOprFlag[ESC_SETSELECT] = 0;


}



static void ESC_EOprPlayBgm(void)
{

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprPlayBgmEx(void)
{






	EXEC_AddPC( EscCnt );
}



static void ESC_EOprStopBgm(void)
{

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprStopBgmEx(void)
{






	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWaitBgmFade(void)
{
	

		
		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetVolumeBgm(void)
{

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetVolumeBgmEx(void)
{






	EXEC_AddPC( EscCnt );
}



static void ESC_EOprPlaySe(void)
{

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprPlaySeEx(void)
{

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprStopSeEx(void)
{

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetVolumeSe(void)
{

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetWeather(void)
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprChangeWeather(void)
{
	



	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprResetWeather(void)
{
	
	if(!EOprFlag[ESC_RESETWEATHER]){
		EOprFlag[ESC_RESETWEATHER] = 1;

	}
	

		EOprFlag[ESC_RESETWEATHER] = 0;
		
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprSetLensFrea(void)
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetWavEffect(void)
{
	if(EscParam[0].num) EscParam[0].num = 128;
	if(EscParam[1].num) EscParam[1].num = 30;
	if(EscParam[2].num) EscParam[2].num = 0;
	
	if(!EOprFlag[ESC_SETWAVEFFECT]){
		EOprFlag[ESC_SETWAVEFFECT] = 1;

	}
	

		EOprFlag[ESC_SETWAVEFFECT] = 0;
		
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprResetWavEffect(void)
{

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprWaitFrame(void)
{
	static DWORD avg_waitTime = 0;
	if(0==EOprFlag[ESC_WAITFRAME]){
		EOprFlag[ESC_WAITFRAME] = 1;
		avg_waitTime = EscParam[0].num;
		if(0==avg_waitTime)avg_waitTime = 30;
		avg_waitTime = timeGetTime() +avg_waitTime*1000/60;
	}
	if( timeGetTime() >= avg_waitTime || (keyState.data_skip || sysInf.bAlreadyRead || keyState.push_action) ){	
		EOprFlag[ESC_WAITFRAME] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}




static void ESC_EOprSetBmp( void )
{
	setBmpInf[EscParam[0].num].SetBmpGraph( EscParam[2].str,  EscParam[3].num, EscParam[4].num );
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmp4Bmp( void )
{



	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpPrim( void )
{


	sysInf.drawFlag=on;
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprResetBmp( void )
{
	
	setBmpInf[EscParam[0].num].Release();


	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprResetBmpAll( void )
{
	int	i;

	for(i=0;i<STBMP_MAX;i++) setBmpInf[i].Release();



	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpAnime( void )
{

	if(EscParam[2].num==-1) EscParam[2].num=0;
	if(EscParam[4].num==-1) EscParam[4].num=1;
	if(EscParam[3].num==-1) EscParam[3].num=-1;
	

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWaitBmpAnime( void )
{




		EXEC_AddPC( EscCnt );	

}
static void ESC_EOprResetBmpAnime( void )
{
	


	
	EXEC_AddPC( EscCnt );
}

#include "systemWnd.h"



static void ESC_EOprSetTitle( void )
{

	if( menuMode() ){
		EXEC_AddPC( EscCnt );
	}
}



static void ESC_EOprSetEnding( void )
{
	
	if(!EOprFlag[ESC_SETENDING]){
		EOprFlag[ESC_SETENDING] = 1;

	}

		EOprFlag[ESC_SETENDING] = 0;
		EXEC_AddPC( EscCnt );

}



static void ESC_EOprSetAvi( void )
{

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprResetAvi( void )
{
	

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWaitAvi( void )
{

		EXEC_AddPC( EscCnt );

}

static void ESC_EOprSetAviFull( void )
{

	EXEC_AddPC( EscCnt );
}

static void ESC_EOprWaitAviFull( void )
{
	

		EXEC_AddPC( EscCnt );

}




static void ESC_EOprSetBmpDisp( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpLayer( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpParam( void )
{
	if(EscParam[2].num==-1) EscParam[2].num=0;
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpRevParam(void)
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpBright( void )
{

	if(EscParam[2].num==-1) EscParam[2].num=EscParam[1].num;
	if(EscParam[3].num==-1) EscParam[3].num=EscParam[1].num;
	

	setBmpInf[EscParam[0].num].SetBmpBright(EscParam[1].num, EscParam[2].num, EscParam[3].num);
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpMove( void )
{
	

	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpPos( void )
{
	

	
	setBmpInf[EscParam[0].num].SetBmpPos(EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num, EscParam[6].num);
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpZoom( void )
{
	


	setBmpInf[EscParam[0].num].SetBmpZoom(EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num);
	
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetBmpZoom2( void )
{
	


	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetDemoFlag(void)
{
	SetDemoFlag( EscParam[0].num, EscParam[1].num );
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetSceneNo(void)
{
	

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetEndingNo( void )
{
	

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetReplayNo( void )
{
	

	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetSoundEvent( void )
{



	if(EscParam[6].num==-1) EscParam[6].num = off;
	


	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetSoundEventVolume( void )
{
	if(EscParam[2].num==-1) EscParam[2].num = -1;	
	if(EscParam[3].num==-1) EscParam[3].num = 0xff;	
	

	
	EXEC_AddPC( EscCnt );
}


static void ESC_EOprSetPotaPota( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 2;
	if(EscParam[1].num==-1) EscParam[1].num = 5;
	if(EscParam[2].num==-1) EscParam[2].num = 128;
	

		EXEC_AddPC( EscCnt );

}

static void ESC_EOprGetTime( void )
{

	EscParam[0].reg = timeGetTime();
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprWaitTime( void )
{
	static DWORD scrWaitTime = 0;

	if(0==scrWaitTime) scrWaitTime = EscParam[0].num;

	if( timeGetTime() >= scrWaitTime ){
		scrWaitTime = 0;
		EXEC_AddPC( EscCnt );
	}
}

static int	TextExCounter=0;
static void ESC_EOprSetTextFormat( void )
{
	if(EscParam[1].num==-1) EscParam[1].num = 20;
	if(EscParam[2].num==-1) EscParam[2].num = 0;
	if(EscParam[3].num==-1) EscParam[3].num = 0;


	if(EscParam[6].num==-1) EscParam[6].num = 0;
	if(EscParam[7].num==-1) EscParam[7].num = 0;
	if(EscParam[8].num==-1) EscParam[8].num = 0;
	if(EscParam[9].num==-1) EscParam[9].num = -1;
	if(EscParam[10].num==-1) EscParam[10].num = 0;



	EXEC_AddPC( EscCnt );

	if(EscParam[0].num==0){
		TextExCounter=0;
	}
}
static void ESC_EOprSetTextSync( void )
{

	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetText( void )
{
	if(!EOprFlag[ESC_SETTEXT]){
		EOprFlag[ESC_SETTEXT] = 1;

	}

		EOprFlag[ESC_SETTEXT] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprSetTextEx( void )
{
	if(!EOprFlag[ESC_SETTEXTEX]){
		EOprFlag[ESC_SETTEXTEX] = 1;


	}

		EOprFlag[ESC_SETTEXTEX] = 0;
		TextExCounter++;
		EXEC_AddPC( EscCnt );

}

static void ESC_EOprResetText( void )
{

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWaitText( void )
{

		EXEC_AddPC( EscCnt );

}
static void ESC_EOprResetTextAll( void )
{
	TextExCounter=0;

	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetDemoFedeFlag( void )
{

	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetCutCut( void )
{

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprMov2( void )
{
	EscParam[0].reg = EscParam[1].num;
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprBD( void )
{

	EXEC_AddPC( EscCnt );
}


static void ESC_EOprB( void )
{
	int	bak_no=-1;
	
	if(EscParam[1].num!=-1) bak_no = EscParam[1].num*10;					
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;	
	if(EscParam[3].num==-1) EscParam[3].num = 30;	
	if(EscParam[4].num==-1) EscParam[4].num = 0;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;	


	if(EOprFlag[ESC_V]==0 && _BAK_DIRECT!=EscParam[0].num){
		
		EOprFlag[ESC_V] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if(g_disp(bak_no, EscParam[0].num, 0, EscParam[3].num, EscParam[4].num, EscParam[5].num, TRUE)){
		EOprFlag[ESC_V] = 0;
		EXEC_AddPC( EscCnt );
	}
}

static void ESC_EOprBT( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*10;					
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;	
	if(EscParam[3].num==-1) EscParam[3].num = 30;	
	if(EscParam[4].num==-1) EscParam[4].num = 0;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;	

	if(g_disp(bak_no, EscParam[0].num, 0, EscParam[3].num, EscParam[4].num, EscParam[5].num, TRUE)){
		EOprFlag[ESC_V] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprBC( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = 30;	
	if(EscParam[4].num==-1) EscParam[4].num = 0;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;	

	if(EOprFlag[ESC_V]==0 && _BAK_DIRECT!=EscParam[0].num){
		
		EOprFlag[ESC_V] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if(g_disp(bak_no, EscParam[0].num, 0, EscParam[3].num, EscParam[4].num, EscParam[5].num)){
		EOprFlag[ESC_V] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprBCT( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = 30;	
	if(EscParam[4].num==-1) EscParam[4].num = 0;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;	

	if(g_disp(bak_no, EscParam[0].num, 0, EscParam[3].num, EscParam[4].num, EscParam[5].num)){
		EOprFlag[ESC_V] = 0;
		EXEC_AddPC( EscCnt );
	}
}


static void ESC_EOprBR( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;

	
	if(EOprFlag[ESC_BR]==0){
		EOprFlag[ESC_BR] = 1;

	}else if(EOprFlag[ESC_BR]==1){
		EOprFlag[ESC_BR] = 2;
		

	}
	

		EOprFlag[ESC_BR] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprBF( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = -2;
	
	if(EOprFlag[ESC_BF]==0){
		EOprFlag[ESC_BF] = 1;

	}else if(EOprFlag[ESC_BF]==1){
		EOprFlag[ESC_BF] = 2;


	}
	

		EOprFlag[ESC_BF] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprV( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = 30;	
	if(EscParam[4].num==-1) EscParam[4].num = off;
	if(EscParam[5].num==-1) EscParam[5].num = 0;
	if(EscParam[6].num==-1) EscParam[6].num = 0;

	if(EOprFlag[ESC_V]==0 && _BAK_DIRECT!=EscParam[0].num){
		
		EOprFlag[ESC_V] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else{
		if(g_disp(bak_no, EscParam[0].num, 1, EscParam[3].num, EscParam[5].num, EscParam[6].num, TRUE)){
			EOprFlag[ESC_V] = 0;
			EXEC_AddPC( EscCnt );
		}
	}	
}
static void ESC_EOprVT( void )
{
	int	bak_no;

	
	if(EOprFlag[ESC_VT]==0){
		EOprFlag[ESC_VT] = 1;

		bak_no = EscParam[1].num*10;
		if(EscParam[2].num!=-1) bak_no += EscParam[2].num;
		if(EscParam[3].num==-1) EscParam[3].num = -2;
		if(EscParam[4].num==-1) EscParam[4].num = off;
		if(EscParam[5].num==-1) EscParam[5].num = 0;
		if(EscParam[6].num==-1) EscParam[6].num = 0;



	}
	

		EOprFlag[ESC_VT] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprH( void )
{
	int	bak_no;
	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = 30;
	if(EscParam[4].num==-1) EscParam[4].num = off;
	if(EscParam[5].num==-1) EscParam[5].num = 0;
	if(EscParam[6].num==-1) EscParam[6].num = 0;

	if(EOprFlag[ESC_H]==0 && _BAK_DIRECT!=EscParam[0].num){
		
		EOprFlag[ESC_H] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if(g_disp(bak_no, EscParam[0].num, 2, EscParam[3].num, EscParam[5].num, EscParam[6].num, TRUE)){
		EOprFlag[ESC_H] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprHT( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = 30;
	if(EscParam[4].num==-1) EscParam[4].num = off;
	if(EscParam[5].num==-1) EscParam[5].num = 0;
	if(EscParam[6].num==-1) EscParam[6].num = 0;

	if(EOprFlag[ESC_HT]==0 && _BAK_DIRECT!=EscParam[0].num){
		
		EOprFlag[ESC_HT] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if(g_disp(bak_no, EscParam[0].num, 2, EscParam[3].num, EscParam[5].num, EscParam[6].num, TRUE)){
		EOprFlag[ESC_HT] = 0;
		EXEC_AddPC( EscCnt );
	}
}

static void ESC_EOprS( void )
{
	if(!SetBackScroll(EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num)){
		EXEC_AddPC( EscCnt );
	}	
}
static void ESC_EOprZ( void )
{
	if(!EOprFlag[ESC_Z]){
		EOprFlag[ESC_Z] = 1;
	}
		EOprFlag[ESC_Z] = 0;
		EXEC_AddPC( EscCnt );
}
static void ESC_EOprFI( void )
{
	if(!EOprFlag[ESC_FI]){
		EOprFlag[ESC_FI] = 1;
	}
		EOprFlag[ESC_FI] = 0;
		EXEC_AddPC( EscCnt );
}
static void ESC_EOprFIF( void )
{
	
	if(!EOprFlag[ESC_FIF]){
		EOprFlag[ESC_FIF] = 1;

	}
	

		EOprFlag[ESC_FIF] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprFO( void )
{
	
	if(!EOprFlag[ESC_FO]){
		EOprFlag[ESC_FO] = 1;

	}
	

		EOprFlag[ESC_FO] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprFOF( void )
{
	
	if(!EOprFlag[ESC_FOF]){
		EOprFlag[ESC_FOF] = 1;

	}
	

		EOprFlag[ESC_FOF] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprFB( void )
{
	if(EOprFlag[ESC_FB]==0){
		
		EOprFlag[ESC_FB] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if( FALSE==SetBackFade( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num ) ){	
		EOprFlag[ESC_FB] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprPFI( void )
{
	
	if(!EOprFlag[ESC_PFI]){
		EOprFlag[ESC_PFI] = 1;
		if( EscParam[2].num==-1 ) EscParam[2].num = 0;
		if( EscParam[3].num==-1 ) EscParam[3].num = 0;
		if( EscParam[4].num==-1 ) EscParam[4].num = 400;
		if( EscParam[5].num==-1 ) EscParam[5].num = 300;

	}
	

		EOprFlag[ESC_PFI] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprPFO( void )
{
	
	if(!EOprFlag[ESC_PFO]){
		EOprFlag[ESC_PFO] = 1;
		if( EscParam[2].num==-1 ) EscParam[2].num = 0;
		if( EscParam[3].num==-1 ) EscParam[3].num = 0;
		if( EscParam[4].num==-1 ) EscParam[4].num = 400;
		if( EscParam[5].num==-1 ) EscParam[5].num = 300;

	}
	

		EOprFlag[ESC_PFO] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprPWI( void )
{
	
	if(!EOprFlag[ESC_PWI]){
		EOprFlag[ESC_PWI] = 1;
		if( EscParam[2].num==-1 ) EscParam[2].num = 0;
		if( EscParam[3].num==-1 ) EscParam[3].num = 0;
		if( EscParam[4].num==-1 ) EscParam[4].num = 400;
		if( EscParam[5].num==-1 ) EscParam[5].num = 300;

	}
	

		EOprFlag[ESC_PWI] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprPWO( void )
{
	
	if(!EOprFlag[ESC_PWO]){
		EOprFlag[ESC_PWO] = 1;
		if( EscParam[2].num==-1 ) EscParam[2].num = 0;
		if( EscParam[3].num==-1 ) EscParam[3].num = 0;
		if( EscParam[4].num==-1 ) EscParam[4].num = 400;
		if( EscParam[5].num==-1 ) EscParam[5].num = 300;

	}
	

		EOprFlag[ESC_PWO] = 0;
		EXEC_AddPC( EscCnt );

}


static void ESC_EOprQ( void )
{
	
	if(!EOprFlag[ESC_Q]){
		EOprFlag[ESC_Q] = 1;

	}
	

		EOprFlag[ESC_Q] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprF( void )
{
	
	if(!EOprFlag[ESC_F]){
		EOprFlag[ESC_F] = 1;

	}
	

		EOprFlag[ESC_F] = 0;
		EXEC_AddPC( EscCnt );

}


static void ESC_EOprC( void )
{
	
	if(EscParam[2].num==-1){

		int pos = CheckCharLocate( EscParam[0].num );
		EscParam[2].num = (pos==-1) ? 1 : pos ;
	}
	if(EscParam[3].num==-1) EscParam[3].num = 0;
	if(EscParam[3].num==-2) EscParam[3].num = -1;
	if(EscParam[4].num==-1) EscParam[4].num = 0;
	if(EscParam[5].num==-1) EscParam[5].num = 128;
	if(EscParam[6].num==-1) EscParam[6].num = 256;
	if(EscParam[7].num==-1) EscParam[7].num = 30;	
	if(c_disp(EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[7].num)){
		EOprFlag[ESC_C] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprCR( void )
{
	
	if(EscParam[1].num==-1) EscParam[1].num = 0;
	if(EscParam[2].num==-1) EscParam[2].num = 30;
	




	

	if(c_cls(EscParam[0].num, EscParam[1].num, EscParam[2].num)){
		EOprFlag[ESC_CR] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprCP( void )
{
	
	if(!EOprFlag[ESC_CP]){
		EOprFlag[ESC_CP] = 1;

		if(EscParam[2].num==-1) EscParam[2].num = 0;

	}
	

		EOprFlag[ESC_CP] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprCL( void )
{
	
	if(!EOprFlag[ESC_CL]){
		EOprFlag[ESC_CL] = 1;
		if( EscParam[2].num==-1 ) EscParam[2].num = -1;

	}
	

		EOprFlag[ESC_CL] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprCY( void )
{
	
	if(!EOprFlag[ESC_CY]){
		EOprFlag[ESC_CY] = 1;

	}
	

		EOprFlag[ESC_CY] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprCB( void )
{
	
	if(!EOprFlag[ESC_CB]){
		EOprFlag[ESC_CB] = 1;

	}
	

		EOprFlag[ESC_CB] = 0;
		EXEC_AddPC( EscCnt );

}
static void ESC_EOprCA( void )
{
	
	if(!EOprFlag[ESC_CA]){
		EOprFlag[ESC_CA] = 1;

	}
	

		EOprFlag[ESC_CA] = 0;
		EXEC_AddPC( EscCnt );

}

static void ESC_EOprCW( void )
{
	
	if(EscParam[2].num==-1){
		int pos = CheckCharLocate( EscParam[0].num );
		EscParam[2].num = (pos==-1) ? 1 : pos ;


	}
	if(EscParam[3].num==-1) EscParam[3].num = 0;
	if(EscParam[4].num==-1) EscParam[4].num = 128;
	if(EscParam[5].num==-1) EscParam[5].num = 256;
	
	c_disp(EscParam[0].num, EscParam[1].num, EscParam[2].num, _CHR_WAIT, EscParam[3].num, 0);


	EXEC_AddPC( EscCnt );
}
static void ESC_EOprCRW( void )
{
	if(EscParam[1].num==-1) EscParam[1].num = -1;
	
	c_cls(EscParam[0].num, EscParam[1].num);

	
	EXEC_AddPC( EscCnt );
}


static void ESC_EOprW( void )
{
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWR( void )
{
		EXEC_AddPC( EscCnt );
}


static void ESC_EOprKW( void )
{
	

	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprK( void )
{
	

		
		EXEC_AddPC( EscCnt );

}


static void ESC_EOprM( void )
{



	lpSoundDS->ChangeMusic(bgmHandle,pack_bgmfile,EscParam[0].num,TRUE,0);

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMS( void )
{
	int		fade = EscParam[0].num;


	if(fade>0){
		lpSoundDS->FadeOut(bgmHandle);
	}else{
		lpSoundDS->Stop(bgmHandle);
	}
	saveInf.musicNum = -1;
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMP( void )
{
	int		flag = EscParam[0].num;

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMV( void )
{

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMW(void)
{
	if(Cl_OK==lpSoundDS->FadeOut(bgmHandle)){
		saveInf.musicNum = -1;
		EXEC_AddPC( EscCnt );
	}

		


}
static void ESC_EOprMLW(void)
{

		
		EXEC_AddPC( EscCnt );

}

static void ESC_EOprSE( void )
{
	if( EscParam[1].num==-1) EscParam[1].num = 0xff;

	PlayEffectNum(evtHandle[0],EscParam[0].num,EscParam[1].num);

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSEP( void )
{
	
	PlayEffectNum(evtHandle[EscParam[0].num],EscParam[1].num,EscParam[4].num,EscParam[3].num);

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSES( void )
{
	StopEffect(evtHandle[EscParam[0].num]);

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSEV( void )
{
	lpSoundDS->SetVolume(evtHandle[ EscParam[0].num ], EscParam[1].num);

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSEW( void )
{
	if(PCM_STOP==lpSoundDS->GetStatus(evtHandle[ EscParam[0].num ])){
		
		EXEC_AddPC( EscCnt );
	}else if(keyState.data_skip || sysInf.bAlreadyRead){  
		EXEC_AddPC( EscCnt );
	}



}
static void ESC_EOprSEVW( void )
{
	if(PCM_FADEOUT!=lpSoundDS->GetStatus(evtHandle[ EscParam[0].num ])){
		
		EXEC_AddPC( EscCnt );
	}



}


static void ESC_EOprR( void )
{
	

	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprRC( void )
{
	



	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprRR( void )
{
	
	if(!EOprFlag[ESC_RR] ){
		EOprFlag[ESC_RR] = 1;

	}
	

		EOprFlag[ESC_RR] = 0;
		
		EXEC_AddPC( EscCnt );

}


static void ESC_EOprLF( void )
{
	

	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprWE( void )
{
	if( EscParam[2].num==-1) EscParam[2].num = 0;
	if(!SetWavEffect( EscParam[0].num, EscParam[1].num, EscParam[2].num )){
		EOprFlag[ESC_WE] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprWER( void )
{

	saveInf.waveCnt = 0;
	saveInf.bWave = FALSE;
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetUsoErr( void )
{

	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetRipple( void )
{

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetRippleSet( void )
{
	if( EscParam[2].num == -1) EscParam[2].num = 0;

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWaitRipple( void )
{

		EXEC_AddPC( EscCnt );

}
static void ESC_EOprSetRippleLost( void )
{
	if(!EOprFlag[ESC_SETRIPPLELOST]){
		EOprFlag[ESC_SETRIPPLELOST] = 1;

	}

		EOprFlag[ESC_SETRIPPLELOST] = 0;
		EXEC_AddPC( EscCnt );

}

static void ESC_EOprSetMovie( void )
{
	if(!EOprFlag[ESC_SETMOVIE]){
		EOprFlag[ESC_SETMOVIE] = 1;

	}

		EOprFlag[ESC_SETMOVIE] = 0;
		EXEC_AddPC( EscCnt );

}


static void ESC_Voice(void)
{
	char	str[16];
	int		charNum,voiceNum;

	strncpy(str,EscParam[0].str,2);
	str[2] = '\0';
	charNum = atoi(str) -1;
	strncpy(str,EscParam[0].str+2,4);
	str[4] = '\0';
	voiceNum = atoi(str);
	lpSoundDS->playVoice(voiceHandle,pack_voice,EscParam[0].str);
	backLog.SetVoice(EscParam[0].str);
	EXEC_AddPC( EscCnt );
}

static void ESC_StaffRoll(void)
{
	if(NULL==lpStaffRoll){
		lpStaffRoll = new StaffRoll;
	}
	if(sysInf.execMode != staffRoll_mode){
		my_delete(lpStaffRoll);
		EXEC_AddPC( EscCnt );
	}
}

static void ESC_Html(void)
{
	ShellExecute( sysInf.hWnd, NULL, EscParam[0].str, NULL, NULL, SW_SHOWNORMAL );
	EXEC_AddPC( EscCnt );
}

char	str_winexec[_MAX_PATH];
char	str_winpath[_MAX_PATH];


BOOL CheckWinExec()
{
	return EOprFlag[ESC_WINEXEC];
}

static void ESC_WinExec(void)
{
	if(!EOprFlag[ESC_WINEXEC]){
		EOprFlag[ESC_WINEXEC] = 1;
		wsprintf(str_winexec,"%s\\%s",EscParam[0].str, EscParam[1].str );
		strcpy(str_winpath,EscParam[0].str);

		if(sysInf.full_screen){
			ChangeDisplaySettings(NULL,CDS_FULLSCREEN);
		}
		sysInf.bGameEnd = on;
	}

}

static void ESC_Kabegami(void)
{
	if(NULL==lpKabeSetWnd){
		lpKabeSetWnd = new KabeSetWnd;
	}
	if(lpKabeSetWnd->state==9){
		my_delete(lpKabeSetWnd);
		EXEC_AddPC( EscCnt );
	}
}

static void ESC_Staffmode(void)
{
	if(NULL==lpStaffmodeWnd){
		lpStaffmodeWnd = new StaffmodeWnd;
	}
	if(lpStaffmodeWnd->state==7){
		my_delete(lpStaffmodeWnd);
		EXEC_AddPC( EscCnt );
	}
}








int ESC_OprControl( int opr )
{
	int	i=ESC_LOADSCRIPT;
	
	EscCnt = GetLangParamControl( opr );
	

	switch( opr ) {

		case ESC_B:					ESC_EOprB();			break;	
		case ESC_BT:				ESC_EOprBT();			break;	
		case ESC_BCT:				ESC_EOprBCT();			break;	

		case ESC_BD:				ESC_EOprBD();			break;	
		case ESC_BC:				ESC_EOprBC();			break;	
		case ESC_BR:				ESC_EOprBR();			break;	
		case ESC_BF:				ESC_EOprBF();			break;	
		case ESC_V:					ESC_EOprV();			break;	
		case ESC_H:					ESC_EOprH();			break;	
		case ESC_VT:				ESC_EOprVT();			break;	
		case ESC_HT:				ESC_EOprHT();			break;	

		case ESC_S:					ESC_EOprS();			break;	
		case ESC_Z:					ESC_EOprZ();			break;	
		case ESC_FI:				ESC_EOprFI();			break;	
		case ESC_FIF:				ESC_EOprFIF();			break;	
		case ESC_FO:				ESC_EOprFO();			break;	
		case ESC_FOF:				ESC_EOprFOF();			break;	
		case ESC_FB:				ESC_EOprFB();			break;	
		case ESC_PFI:				ESC_EOprPFI();			break;	
		case ESC_PFO:				ESC_EOprPFO();			break;	
		case ESC_PWI:				ESC_EOprPWI();			break;	
		case ESC_PWO:				ESC_EOprPWO();			break;	


		case ESC_Q:					ESC_EOprQ();			break;	
		case ESC_F:					ESC_EOprF();			break;	


		case ESC_C:					ESC_EOprC();			break;	


		case ESC_CR:				ESC_EOprCR();			break;	

		case ESC_CP:				ESC_EOprCP();			break;	
		case ESC_CL:				ESC_EOprCL();			break;	
		case ESC_CY:				ESC_EOprCY();			break;	
		case ESC_CB:				ESC_EOprCB();			break;	
		case ESC_CA:				ESC_EOprCA();			break;	
		case ESC_CW:				ESC_EOprCW();			break;	
		case ESC_CRW:				ESC_EOprCRW();			break;	


		case ESC_W:					ESC_EOprW();			break;	
		case ESC_WR:				ESC_EOprWR();			break;	


		case ESC_KW:				ESC_EOprKW();			break;	
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


		case ESC_R:					ESC_EOprR();			break;	
		case ESC_RC:				ESC_EOprRC();			break;	
		case ESC_RR:				ESC_EOprRR();			break;	


		case ESC_LF:				ESC_EOprLF();			break;	

		case ESC_WE:				ESC_EOprWE();			break;	
		case ESC_WER:				ESC_EOprWER();			break;	


		case ESC_SETFLAG:			ESC_EOprSetFlag();			break;	
		case ESC_GETFLAG:			ESC_EOprGetFlag();			break;	

		case ESC_SETGAMEFLAG:		ESC_EOprSetGameFlag();		break;	
		case ESC_GETGAMEFLAG:		ESC_EOprGetGameFlag();		break;	

		case ESC_LOADSCRIPT:		ESC_EOprLoadScript();		break;	
		case ESC_GAMEEND:			ESC_EOprGameEnd();			break;	
		case ESC_CALLFUNC:			ESC_EOprCallFunc();			break;	

		case ESC_SETTIMEMODE:		ESC_EOprSetTimeMode();		break;	
		case ESC_SETTIMEBACK:		ESC_EOprSetTimeBack();		break;	
		case ESC_SETTIMECHAR:		ESC_EOprSetTimeChar();		break;	

		case ESC_SETMATRIX:				ESC_EOprSetMatrix();			break;	
		case ESC_SHOWCHIPBACK:			ESC_EOprShowChipBack();			break;	
		case ESC_POSECHIPBACK:			ESC_EOprPoseChipBack();			break;	
		case ESC_PLAYCHIPBACK:			ESC_EOprPlayChipBack();			break;	

		case ESC_SETCHIPBACK:			ESC_EOprSetChipBack();			break;	
		case ESC_SETCHIPPARTS:			ESC_EOprSetChipParts();			break;	
		case ESC_SETCHIPSCROLL:			ESC_EOprSetChipScroll();		break;	
		case ESC_SETCHIPSCROLL2:		ESC_EOprSetChipScroll2();		break;	
		case ESC_SETCHIPSCROLLSPEED:	ESC_EOprSetChipScrollSpeed();	break;	
		case ESC_WAITCHIPSCROLL:		ESC_EOprWaitChipScroll();		break;	
		case ESC_WAITCHIPSCROLLSPEED:	ESC_EOprWaitChipScrollSpeed();	break;	

		case ESC_SETCHIPCHARCASH:		ESC_EOprSetChipCharCash();		break;	
		case ESC_RESETCHIPCHARCASH:		ESC_EOprResetChipCharCash();	break;	

		case ESC_SETCHIPCHAR:			ESC_EOprSetChipChar();			break;	
		case ESC_SETCHIPCHARANI:		ESC_EOprSetChipCharAni();		break;	
		case ESC_RESETCHIPCHAR:			ESC_EOprResetChipChar();		break;	
		case ESC_SETCHIPCHARPARAM:		ESC_EOprSetChipCharParam();		break;	
		case ESC_SETCHIPCHARBRIGHT:		ESC_EOprSetChipCharBright();	break;	
		case ESC_SETCHIPCHARMOVE:		ESC_EOprSetChipCharMove();		break;	
		case ESC_SETCHIPCHARMOVE2:		ESC_EOprSetChipCharMove2();		break;	
		case ESC_SETCHIPCHARMOVESPEED:	ESC_EOprSetChipCharMoveSpeed();	break;	
		case ESC_GETCHIPCHARMOVE:		ESC_EOprGetChipCharMove();		break;	
		case ESC_GETCHIPCHARMOVESPEED:	ESC_EOprGetChipCharMoveSpeed();	break;	
		case ESC_COPYCHIPCHARPOS:		ESC_EOprCopyChipCharPos();		break;	
		case ESC_SETCHIPCHARREV:		ESC_EOprSetChipCharRev();		break;	
		case ESC_THROUGHCHIPCHARANI:	ESC_EOprThroughChipCharAni();	break;	
		case ESC_WAITCHIPCHARREPEAT:	ESC_EOprWaitChipCharRepeat();	break;	
		case ESC_WAITCHIPCHARANI:		ESC_EOprWaitChipCharAni();		break;	
		case ESC_WAITCHIPCHARANILOOP:	ESC_EOprWaitChipCharAniLoop();	break;	
		case ESC_WAITCHIPCHARAMOVE:		ESC_EOprWaitChipCharMove();		break;	

		case ESC_GETBACK:			ESC_EOprGetBack();			break;	
		case ESC_SETBACK:			ESC_EOprSetBack();			break;	
		case ESC_SETBACK2:			ESC_EOprSetBack2();			break;	

		case ESC_SETNOISE:			ESC_EOprSetNoise();			break;	

		case ESC_SETCG:				ESC_EOprSetCg();			break;	
		case ESC_SETCG2:			ESC_EOprSetCg2();			break;	
		case ESC_SETBACKPOS:		ESC_EOprSetBackPos();		break;	
		case ESC_SETBACKPOSZ:		ESC_EOprSetBackPosZ();		break;	
		case ESC_SETBACKSCROLL:		ESC_EOprSetBackScroll();	break;	
		case ESC_SETBACKSCROLLZ:	ESC_EOprSetBackScrollZ();	break;	
		case ESC_SETBACKCSCOPE:		ESC_EOprSetBackCScope();	break;	
		case ESC_WAITBACKCSCOPE:	ESC_EOprWaitBackCScope();	break;	
		case ESC_LOCKBACKCSCOPE:	ESC_EOprLockBackCScope();	break;	

		case ESC_SETBACKFADEIN:		ESC_EOprSetBackFadeIn();	break;	
		case ESC_SETBACKFADEOUT:	ESC_EOprSetBackFadeOut();	break;	
		case ESC_SETBACKFADERGB:	ESC_EOprSetBackFadeRGB();	break;	

		case ESC_SETSHAKE:			ESC_EOprSetShake();			break;	
		case ESC_STOPSHAKE:			ESC_EOprStopShake();		break;	
		case ESC_SETFLASH:			ESC_EOprSetFlash();			break;	

		case ESC_SETCHAR:			ESC_EOprSetChar();			break;	
		case ESC_RESETCHAR:			ESC_EOprResetChar();		break;	
		case ESC_SETCHARPOSE:		ESC_EOprSetCharPose();		break;	
		case ESC_SETCHARLOCATE:		ESC_EOprSetCharLocate();	break;	
		case ESC_SETCHARLAYER:		ESC_EOprSetCharLayer();		break;	
		case ESC_WAITCHAR:			ESC_EOprWaitChar();			break;	

		case ESC_SETBLOCK:			ESC_EOprSetBlock();			break;	
		case ESC_SETWINDOW:			ESC_EOprSetWindow();		break;	
		case ESC_RESETWINDOW:		ESC_EOprResetWindow();		break;	
		case ESC_SETMESSAGE:		ESC_EOprSetMessage();		break;	
		case ESC_SETMESSAGE2:		ESC_EOprSetMessage2();		break;	
		case ESC_SETMESSAGEEX:		ESC_EOprSetMessageEx();		break;	
		case ESC_SETCHIPMESSAGE:	ESC_EOprSetChipMessage();	break;	
		case ESC_ADDMESSAGE:		ESC_EOprAddMessage();		break;	
		case ESC_ADDMESSAGE2:		ESC_EOprAddMessage2();		break;	
		case ESC_SETMESSAGEWAIT:	ESC_EOprSetMessageWait();	break;	
		case ESC_RESETMESSAGE:		ESC_EOprResetMessage();		break;	

		case ESC_T:					ESC_EOprSetTextDisp();		break;	


		case ESC_WAITKEY:			ESC_EOprWaitKey();			break;	

		case ESC_SETSELECTMES:		ESC_EOprSetSelectMes();		break;	
		case ESC_SETSELECTMESEX:	ESC_EOprSetSelectMesEx();	break;	
		case ESC_SETSELECT:			ESC_EOprSetSelect();		break;	
		case ESC_SETSELECTEX:		ESC_EOprSetSelectEx();		break;	

		case ESC_PLAYBGM:			ESC_EOprPlayBgm();			break;	
		case ESC_PLAYBGMEX:			ESC_EOprPlayBgmEx();		break;	
		case ESC_STOPBGM:			ESC_EOprStopBgm();			break;	
		case ESC_STOPBGMEX:			ESC_EOprStopBgmEx();		break;	
		case ESC_SETVOLUMEBGM:		ESC_EOprSetVolumeBgm();		break;	
		case ESC_SETVOLUMEBGMEX:	ESC_EOprSetVolumeBgmEx();	break;	

		case ESC_PLAYSE:			ESC_EOprPlaySe();			break;	
		case ESC_PLAYSEEX:			ESC_EOprPlaySeEx();			break;	
		case ESC_STOPSEEX:			ESC_EOprStopSeEx();			break;	
		case ESC_SETVOLUMESE:		ESC_EOprSetVolumeSe();		break;	

		case ESC_SETWEATHER:		ESC_EOprSetWeather();		break;	
		case ESC_CHANGEWEATHER:		ESC_EOprChangeWeather();	break;	
		case ESC_RESETWEATHER:		ESC_EOprResetWeather();		break;	
		case ESC_SETLENSFREA:		ESC_EOprSetLensFrea();		break;	
		case ESC_SETWAVEFFECT:		ESC_EOprSetWavEffect();		break;	
		case ESC_RESETWAVEFFECT:	ESC_EOprResetWavEffect();	break;	

		case ESC_WAITFRAME:			ESC_EOprWaitFrame();		break;	

		case ESC_SETBMP:			ESC_EOprSetBmp();			break;	
		case ESC_SETBMP4BMP:		ESC_EOprSetBmp4Bmp();		break;	
		case ESC_SETBMPPRIM:		ESC_EOprSetBmpPrim();		break;	
		case ESC_RESETBMP:			ESC_EOprResetBmp();			break;	
		case ESC_RESETBMPALL:		ESC_EOprResetBmpAll();		break;	
		case ESC_SETBMPANIME:		ESC_EOprSetBmpAnime();		break;	
		case ESC_RESETBMPANIME:		ESC_EOprResetBmpAnime();	break;	
		case ESC_WAITBMPANIME:		ESC_EOprWaitBmpAnime();		break;	
		case ESC_SETTITLE:			ESC_EOprSetTitle();			break;	
		case ESC_SETENDING:			ESC_EOprSetEnding();		break;	

		case ESC_SETAVI:			ESC_EOprSetAvi();			break;	
		case ESC_RESETAVI:			ESC_EOprResetAvi();			break;	
		case ESC_WAITAVI:			ESC_EOprWaitAvi();			break;	
		case ESC_SETAVIFULL:		ESC_EOprSetAviFull();		break;	
		case ESC_WAITAVIFULL:		ESC_EOprWaitAviFull();		break;	

		case ESC_SETBMPDISP:		ESC_EOprSetBmpDisp();		break;	
		case ESC_SETBMPLAYER:		ESC_EOprSetBmpLayer();		break;	
		case ESC_SETBMPPARAM:		ESC_EOprSetBmpParam();		break;	
		case ESC_SETBMPREVPARAM:	ESC_EOprSetBmpRevParam();	break;	
		case ESC_SETBMPBRIGHT:		ESC_EOprSetBmpBright();		break;	
		case ESC_SETBMPMOVE:		ESC_EOprSetBmpMove();		break;	
		case ESC_SETBMPPOS:			ESC_EOprSetBmpPos();		break;	
		case ESC_SETBMPZOOM:		ESC_EOprSetBmpZoom();		break;	
		case ESC_SETBMPZOOM2:		ESC_EOprSetBmpZoom2();		break;	

		case ESC_SETDEMOFLAG:		ESC_EOprSetDemoFlag();		break;	
		case ESC_SETSCENENO:		ESC_EOprSetSceneNo();		break;	
		case ESC_SETENDINGNO:		ESC_EOprSetEndingNo();		break;	
		case ESC_SETREPLAYNO:		ESC_EOprSetReplayNo();		break;	

		case ESC_SETSOUNDEVENT:			ESC_EOprSetSoundEvent();		break;	
		case ESC_SETSOUNDEVENTVOLUME:	ESC_EOprSetSoundEventVolume();	break;	
	
		case ESC_SETPOTAPOTA:		ESC_EOprSetPotaPota();		break;	

		case ESC_GETTIME:			ESC_EOprGetTime();			break;	
		case ESC_WAITTIME:			ESC_EOprWaitTime();			break;	

		case ESC_SETTEXTFORMAT:		ESC_EOprSetTextFormat();	break;	
		case ESC_SETTEXTSYNC:		ESC_EOprSetTextSync();		break;	
			
		case ESC_SETTEXT:			ESC_EOprSetText();			break;	
		case ESC_SETTEXTEX:			ESC_EOprSetTextEx();		break;	
		case ESC_RESETTEXT:			ESC_EOprResetText();		break;	
		case ESC_WAITTEXT:			ESC_EOprWaitText();			break;	
		case ESC_RESETTEXTALL:		ESC_EOprResetTextAll();		break;	

		case ESC_SETDEMOFADEFLAG:	ESC_EOprSetDemoFedeFlag();	break;	
		
		case ESC_MOV2:				ESC_EOprMov2();				break;	

		case ESC_SETCUTCUT:			ESC_EOprSetCutCut();		break;	

		case ESC_SETUSOERR:			ESC_EOprSetUsoErr();		break;	

		case ESC_LOADSCRIPTNUM:		ESC_EOprLoadScriptNum();	break;	

		case ESC_SETRIPPLE:			ESC_EOprSetRipple();		break;	
		case ESC_SETRIPPLESET:		ESC_EOprSetRippleSet();		break;	
		case ESC_WAITRIPPLE:		ESC_EOprWaitRipple();		break;	
		case ESC_SETRIPPLELOST:		ESC_EOprSetRippleLost();	break;	

		case ESC_SETMOVIE:			ESC_EOprSetMovie();			break;	
		case ESC_VC:				ESC_Voice();				break;	
		case ESC_HTML:				ESC_Html();					break;	
		case ESC_WINEXEC:			ESC_WinExec();				break;	
		case ESC_KABEGAMI:			ESC_Kabegami();				break;	
		case ESC_STAFFMODE:			ESC_Staffmode();			break;	
		case ESC_STAFFROLL:			ESC_StaffRoll();			break;	
	}

	SetScriptRegisterControl( opr );

	return EScroptOpr[opr-64].ret;
}



static HWND	ScriptDialog;

static BOOL CALLBACK ScriptDialogProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	char	buf[256];
	int		index;
	switch( wmes ){
		case WM_INITDIALOG:
			break;
		case WM_VSCROLL:
			break;
		case WM_COMMAND:



			break;
		case WM_DESTROY:
			ScriptDialog = NULL;
			break;
		case WM_ACTIVATE:
			break;
		case WM_CLOSE:
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}

EXEC_DATA 	LangData;		

static char	*PackScriptDir=NULL;

void EXEC_SetPackDir( char *pac_dir )
{
	PackScriptDir = pac_dir;
}



char LangOprList[64][32] = 
{
	"Start",	"End",		"MovR",		"MovV",		
	"Swap",		"Rand",		"IfR",		"IfV",		
	"IfElseR",	"IfElseV",	"Loop",		"Goto",		
	"Inc",		"Dec",		"Not",		"Neg",		
	"AddR",		"AddV",		"SubR",		"SubV",		
	"MulR",		"MulV",		"DivR",		"DivV",		
	"ModR",		"ModV",		"AndR",		"AndV",		
	"OrR",		"OrV",		"XorR",		"XorV",		
	"Calc",		"Pusha",	"Popa",		"Call",		
	"Ret",		"Wait",		"TWait",	"Run",		
	"SLoad",	"",			"",			"",			
	"",			"",			"",			"",			
	"",			"",			"",			"",			
	"",			"",			"",			"",			
	"",			"",			"",			"",			
	"",			"",			"",			"",			
};


char		*EXEC_LangBuf;						
EXEC_DATA 	*EXEC_LangInfo;					
char		NowLangFileName[16] = "";			
int			RunLangBlockNo = -1;
int			PrevRunLangBlockNo = -1;







BOOL EXEC_ReadLang( char *filename, EXEC_DATA *scr )
{
	SCRIPT_HEADER	*header;
	int				i, size;
	char			*buf=NULL;

	lpSoundDS->Stop(voiceHandle);	
	

	size = readFile->ReadPackFile(pack_script,filename,(char **)&buf);
	if(size==0){
		return FALSE;
	}
	
	header = (SCRIPT_HEADER*)buf;
	if( ( header->h1 != 'L' ) || ( header->h2 != 'F' ) ) {
		myOutputDebugString("スクリプトファイルではありません。\n" );
		cl_free( buf );
		return FALSE;
	}



	
	size = header->Fsize - sizeof( SCRIPT_HEADER );		
	cl_free(scr->LangBuf);
	scr->LangBuf = (char *)cl_malloc( size );
	CopyMemory( scr->LangBuf, buf + sizeof(SCRIPT_HEADER), size );

	
	for( i=0; i<SCRIPT_BLOCK_MAX; i++ ) { scr->BlockAddres[i] = header->BlockAddres[i]; }

	
	scr->BusyFlg  = SCCODE_OPRS_NOOPR;								

	wsprintf( NowLangFileName, "%s", filename );

	cl_free( buf );
	return( TRUE );
}







BOOL EXEC_StartLang( EXEC_DATA *scr, int scr_no )
{
	int		i;
	
	
	if( (scr->BusyFlg != SCCODE_OPRS_NOOPR) && (scr->BusyFlg != SCCODE_OPRS_WAIT_SLOAD) )    { return( FALSE ); }	
	if( scr->BlockAddres[ scr_no ] == 0 ) { return( FALSE ); }	


	
	scr->BusyFlg = SCCODE_OPRS_RUN;									
	scr->pc      = scr->BlockAddres[ scr_no ] - 1;				
	scr->sp      = 0;											
	for( i=0; i<REGISTER_MAX; i++ ) { scr->reg[ i ] = 0; }		
	RunLangBlockNo = scr_no;

	return( TRUE );
}






void EXEC_ReleaseLang( EXEC_DATA *scr )
{
	int	i;
	
	cl_free( scr->LangBuf );

	
	scr->LangBuf = NULL;
	scr->BusyFlg   = SCCODE_OPRS_NOOPR;
	scr->pc        = 0;

	scr->sp      = 0;										
	for( i=0; i<STACK_MAX; i++ ) { scr->sp_buf[ i ] = 0; }	
	for( i=0; i<REGISTER_MAX; i++ ) { scr->reg[ i ] = 0; }	
	
	RunLangBlockNo = -1;
}






void EXEC_EndLang( EXEC_DATA *scr, int init )
{
	
	scr->BusyFlg   = SCCODE_OPRS_NOOPR;
	scr->pc        = 0;

	if(!init){
		if( scr->sp != 0 )
			myOutputDebugString("Script Error : Stack Pointer Abnormal\n" );
	}
	RunLangBlockNo = -1;
}






short EXEC_GetShortValue( char *buf )
{
	int		i;
	union {
		char	byte[ 2 ];
		WORD	word;
	}val;

	
	for( i=0; i<2; i++ ) { val.byte[ i ] = buf[ i ]; }
	
	return(	val.word );
}






long EXEC_GetLongValue( char *buf )
{
	long	i;
	union {
		char byte[ 4 ];
		long dword;
	}val;

	
	for( i=0; i<4; i++ ) { val.byte[ i ] = buf[ i ]; }

	return(	val.dword );
}







int EXEC_GetValue( char *buf, int mode )
{
	int 	value;
	
	
	if( mode == 0 )	value = EXEC_LangInfo->reg[ *buf ];	
	else			value = EXEC_GetLongValue( buf );		

	return( value );
}






static void EXEC_LangWait( void )
{
	
	if( EXEC_LangInfo->WaitCounter < EXEC_LangInfo->WaitEnd )	EXEC_LangInfo->WaitCounter++;
	else														EXEC_LangInfo->BusyFlg = SCCODE_OPRS_RUN;
}






static void EXEC_LangTWait( void )
{
	
	if( timeGetTime() >= EXEC_LangInfo->TWaitEnd )	EXEC_LangInfo->BusyFlg = SCCODE_OPRS_RUN;
}






static void EXEC_SpPush( int num )
{
	
	EXEC_LangInfo->sp_buf[ EXEC_LangInfo->sp ] = num;
	EXEC_LangInfo->sp++;
}






static int EXEC_SpPop( void )
{
	
	EXEC_LangInfo->sp--;
	return( EXEC_LangInfo->sp_buf[ EXEC_LangInfo->sp ] );
}






static void EXEC_OprMov( char mode )
{
	int		param1, param2;
	
	
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];							
	param2 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ], mode );	
	
	
	EXEC_LangInfo->reg[ param1 ] = param2;

	
	if( mode == 0 )	EXEC_AddPC( 4 );	
	else			EXEC_AddPC( 7 );	
}






static void EXEC_OprSwap( void )
{
	int		param1, param2, work;

	
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];		
	param2 = EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ];		

	
	work                          = EXEC_LangInfo->reg[ param1 ];
	EXEC_LangInfo->reg[ param1 ] = EXEC_LangInfo->reg[ param2 ];
	EXEC_LangInfo->reg[ param2 ] = work;
	
	
	EXEC_AddPC( 4 );
}






static void EXEC_OprRand( void )
{
	int		param1,param2;
	
	
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];
	param2 = EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ];
	
	

	EXEC_LangInfo->reg[ param1 ] = rndSelect(param2);

	
	EXEC_AddPC( 4 );
}






static void EXEC_OprIf( char mode )
{
	int		param1, param2, param3, p, flg;
	
	
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];							
	param1 = EXEC_GetParam( param1 );											
	param2 = EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ];							
	param3 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 4 ], mode );	

	
	switch( param2 ) {
		case CALL_SMALL:	flg = ( param1 <  param3 ) ? 1 : 0;		break;		
		case CALL_ESMALL:	flg = ( param1 <= param3 ) ? 1 : 0;		break;		
		case CALL_LARGE:	flg = ( param1 >  param3 ) ? 1 : 0;		break;		
		case CALL_ELARGE:	flg = ( param1 >= param3 ) ? 1 : 0;		break;		
		case CALL_EQUAL:	flg = ( param1 == param3 ) ? 1 : 0;		break;		
		case CALL_NEQUAL:	flg = ( param1 != param3 ) ? 1 : 0;		break;		
	}

	
	if( flg ){	
		p = ( mode == 0 ) ? 5 : 8;
		EXEC_LangInfo->pc = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + p ] );
	}
	else{		
		EXEC_AddPC( ( mode == 0 ) ? 9 : 12 );
	}
}






static void EXEC_OprIfElse( char mode )
{
	int		param1, param2, param3, p;
	char	flg;
	
	
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];							
	param1 = EXEC_GetParam( param1 );											
	param2 = EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ];							
	param3 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 4 ], mode );	

	
	switch( param2 ) {
		case CALL_SMALL:	flg = ( param1 <  param3 ) ? 1 : 0;		break;		
		case CALL_ESMALL:	flg = ( param1 <= param3 ) ? 1 : 0;		break;		
		case CALL_LARGE:	flg = ( param1 >  param3 ) ? 1 : 0;		break;		
		case CALL_ELARGE:	flg = ( param1 >= param3 ) ? 1 : 0;		break;		
		case CALL_EQUAL:	flg = ( param1 == param3 ) ? 1 : 0;		break;		
		case CALL_NEQUAL:	flg = ( param1 != param3 ) ? 1 : 0;		break;		
	}

	
	if( flg )	p = ( mode == 0 ) ? 5 : 8;
	else		p = (( mode == 0 ) ? 5 : 8) + 4;
	EXEC_LangInfo->pc = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + p ] );
}






static void EXEC_OprLoop( void )
{
	int		param1;

	
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];	

	
	if( EXEC_LangInfo->reg[ param1 ] > 0 ) {			
		EXEC_LangInfo->reg[ param1 ]--;
		EXEC_LangInfo->pc = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ] );
	}else{
		EXEC_AddPC( 7 );
	}
}






static void EXEC_OprGoto( void  )
{
	
	EXEC_LangInfo->pc = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ] );
}






static void EXEC_OprArithmetic1( int opr )
{
	int		param1;

	
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];		

	
	switch( opr ) {
		case OPRS_INC:	EXEC_LangInfo->reg[ param1 ] ++;								break;
		case OPRS_DEC:	EXEC_LangInfo->reg[ param1 ] --;								break;
		case OPRS_NOT:	EXEC_LangInfo->reg[ param1 ] = ~EXEC_LangInfo->reg[ param1 ];	break;
		case OPRS_NEG:	EXEC_LangInfo->reg[ param1 ] *= -1;							break;
	}

	
	EXEC_AddPC( 3 );
}






static void EXEC_OprArithmetic2( int opr )
{
	int		param1, val1, val2 = 0, ans = 0, mode;

	
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];		
	
	val1 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ], 0 );
	switch( opr ) {
		case OPRS_ADDR:	case OPRS_SUBR:	case OPRS_MULR:	case OPRS_DIVR:
		case OPRS_MODR:	case OPRS_ANDR:	case OPRS_ORR:	case OPRS_XORR:
						val2 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ], 0 );
						mode = 0;
						break;
		case OPRS_ADDV:	case OPRS_SUBV:	case OPRS_MULV:	case OPRS_DIVV:
		case OPRS_MODV:	case OPRS_ANDV:	case OPRS_ORV:	case OPRS_XORV:
						val2 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ], 1 );
						mode = 1;
						break;
	}

	
	switch( opr ) {
		case OPRS_ADDR:	case OPRS_ADDV:	ans = val1 + val2;							break;
		case OPRS_SUBR:	case OPRS_SUBV:	ans = val1 - val2;							break;
		case OPRS_MULR:	case OPRS_MULV:	ans = val1 * val2;							break;
		case OPRS_DIVR:	case OPRS_DIVV:	ans = ( val2 != 0 ) ? ( val1 / val2 ) : 0;	break;
		case OPRS_MODR:	case OPRS_MODV:	ans = ( val2 != 0 ) ? ( val1 % val2 ) : 0;	break;
		case OPRS_ANDR:	case OPRS_ANDV:	ans = val1 & val2;							break;
		case OPRS_ORR:	case OPRS_ORV:	ans = val1 | val2;							break;
		case OPRS_XORR:	case OPRS_XORV:	ans = val1 ^ val2;							break;
	}
	EXEC_LangInfo->reg[ param1 ] = ans;

	
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
	
	
	reg  = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];
	size = EXEC_GetShortValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ] );

	
	bp = sbp = 0;
	while( 1 ) {
		if( sbp >= size ) { break; }
		if( EXEC_LangBuf[ EXEC_LangInfo->pc + 5 + sbp ] == FACT_VAL ) {		
			Calc[ bp ].opr = EXEC_LangBuf[ EXEC_LangInfo->pc + 5 + sbp ];
			Calc[ bp ].data = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc+3+sbp+1 ] );
			bp++;	sbp += 5;
		}
		else {																	
			Calc[ bp ].opr = EXEC_LangBuf[ EXEC_LangInfo->pc + 5 + sbp ];
			Calc[ bp ].data = EXEC_LangBuf[ EXEC_LangInfo->pc + 5 + sbp + 1 ];
			bp++;	sbp += 2;
		}
	}
	Calc[ bp ].opr = FACT_END;

	
	bp = 0;
	while( Calc[ bp ].opr != FACT_END ) {
		if( Calc[ bp ].opr != FACT_CLC ) { bp++; continue; }
		if( Calc[ bp - 2 ].opr == FACT_VAL ) { s1 = Calc[ bp - 2 ].data;                        }
		else                                  { s1 = EXEC_LangInfo->reg[ Calc[ bp - 2 ].data ]; }
		if( Calc[ bp - 1 ].opr == FACT_VAL ) { s2 = Calc[ bp - 1 ].data;                        }
		else                                  { s2 = EXEC_LangInfo->reg[ Calc[ bp - 2 ].data ]; }
		switch( Calc[ bp ].data ) {
			case CALL_ADD:	ans = s1 + s2;							break;
			case CALL_SUB:	ans = s1 - s2;							break;
			case CALL_MUL:	ans = s1 * s2;							break;
			case CALL_DIV:	ans = ( s2 != 0 ) ? ( s1 / s2 ) : 0;	break;
			case CALL_MOD:	ans = ( s2 != 0 ) ? ( s1 % s2 ) : 0;	break;
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
	
	for( i=0; i<REGISTER_MAX; i++ ) { EXEC_SpPush( EXEC_LangInfo->reg[ i ] ); }
}
static void EXEC_OprPusha( void )
{
	EXEC_Pusha();
	
	EXEC_AddPC( 2 );
}







static void EXEC_Popa( void )
{
	int		i;
	
	for( i=REGISTER_MAX-1; i>=0; i-- ) { EXEC_LangInfo->reg[ i ] = EXEC_SpPop(); }
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

	
	EXEC_SpPush( EXEC_LangInfo->pc + 3 );
	
	
	callno = (char)EXEC_LangBuf[ EXEC_LangInfo->pc+2 ];
	if( EXEC_LangInfo->BlockAddres[ callno ] == 0 ){
		myOutputDebugString( "Script Call Error : ScriptBlock(%d) Code None\n", callno );
		return;
	}
	addres = EXEC_LangInfo->BlockAddres[ callno ] - 1;
	PrevRunLangBlockNo = RunLangBlockNo;
	RunLangBlockNo = callno;

	
	EXEC_LangInfo->pc = addres;
}
void EXEC_OprCallFunc( int call_no, int next_pc, long *num )
{
	DWORD	addres;
	int		i;

	
	EXEC_SpPush( EXEC_LangInfo->pc + next_pc );
	
	for( i=0; i<14 ; i++ ){
		EXEC_LangInfo->reg[i] = num[i];
	}

	if( EXEC_LangInfo->BlockAddres[ call_no ] == 0 ){
		myOutputDebugString("Script Call Error : ScriptBlock(%d) Code None", call_no );
		return;
	}
	addres = EXEC_LangInfo->BlockAddres[ call_no ] - 1;
	PrevRunLangBlockNo = RunLangBlockNo;
	RunLangBlockNo = call_no;

	
	EXEC_LangInfo->pc = addres;
}







static void EXEC_OprRet( void )
{
	
	EXEC_LangInfo->pc = EXEC_SpPop();
	RunLangBlockNo = PrevRunLangBlockNo;
}






static void EXEC_OprWait( void )
{
	int		param1;
	
	
	param1 = EXEC_GetShortValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ] );		
	
	
	EXEC_LangInfo->WaitCounter = 0;					
	EXEC_LangInfo->WaitEnd     = param1;				
	EXEC_LangInfo->BusyFlg     = SCCODE_OPRS_WAIT_WAIT;		
	
	
	EXEC_AddPC( 4 );
}






static void EXEC_OprTWait( void )
{
	int		param1;
	
	
	param1 = EXEC_GetShortValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ] );		
	
	
	EXEC_LangInfo->TWaitEnd   = timeGetTime()+(DWORD)param1;	
 	EXEC_LangInfo->BusyFlg    = SCCODE_OPRS_WAIT_TWAIT;				

	
	EXEC_AddPC( 4 );
}






static void EXEC_OprRun( void )
{
	ESC_SetDrawFlag();
	
	EXEC_AddPC( 2 );
}

void EXEC_SLoad( char *str )
{
	
	EXEC_ReleaseLang( EXEC_LangInfo );
	
	
	EXEC_ReadLang( str, EXEC_LangInfo );

	EXEC_LangInfo->BusyFlg = SCCODE_OPRS_WAIT_SLOAD;

	EXEC_StartLang( EXEC_LangInfo, 0 );
}





void EXEC_OprSLoad( void )
{
	int		i, len;
	char	str[256];

	
	len = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];						
	for( i=0; i<len; i++ ) { str[ i ] = EXEC_LangBuf[ EXEC_LangInfo->pc + 3 + i ]; }
	str[i] = '\0';	str[i+1] = '\0';

	EXEC_SLoad( str );
}






static BOOL EXEC_OprControl( int opr )
{
	BOOL	ret;

	
	switch( opr ) {
		case OPRS_END:		EXEC_EndLang( EXEC_LangInfo,off);	ret = FALSE;	break;	
		case OPRS_MOVR:		EXEC_OprMov( 0 );					ret = TRUE;		break;	
		case OPRS_MOVV:		EXEC_OprMov( 1 );					ret = TRUE;		break;	
		case OPRS_SWAP:		EXEC_OprSwap();						ret = TRUE;		break;	
		case OPRS_RAND:		EXEC_OprRand();						ret = TRUE;		break;	
		case OPRS_IFR:		EXEC_OprIf( 0 );						ret = TRUE;		break;	
		case OPRS_IFV:		EXEC_OprIf( 1 );						ret = TRUE;		break;	
		case OPRS_IFELSER:	EXEC_OprIfElse( 0 );					ret = TRUE;		break;	
		case OPRS_IFELSEV:	EXEC_OprIfElse( 1 );					ret = TRUE;		break;	
		case OPRS_LOOP:		EXEC_OprLoop();						ret = TRUE;		break;	
		case OPRS_GOTO:		EXEC_OprGoto();						ret = TRUE;		break;	

		case OPRS_INC:     																
		case OPRS_DEC:     																
		case OPRS_NOT:     																
		case OPRS_NEG:		EXEC_OprArithmetic1( opr );			ret = TRUE;		break;	

		case OPRS_ADDR:	case OPRS_ADDV:													
		case OPRS_SUBR:	case OPRS_SUBV:													
		case OPRS_MULR:	case OPRS_MULV:													
		case OPRS_DIVR:	case OPRS_DIVV:													
		case OPRS_MODR:	case OPRS_MODV:													
		case OPRS_ANDR:	case OPRS_ANDV:													
		case OPRS_ORR:	case OPRS_ORV:													
		case OPRS_XORR:	case OPRS_XORV:													
							EXEC_OprArithmetic2( opr );			ret = TRUE;		break;	
		case OPRS_CALC:		EXEC_OprCalc();						ret = TRUE;		break;	
		case OPRS_PUSHA:		EXEC_OprPusha();						ret = TRUE;		break;	
		case OPRS_POPA:		EXEC_OprPopa();						ret = TRUE;		break;	
		case OPRS_CALL:		EXEC_OprCall();						ret = TRUE;		break;	

		case OPRS_RET:		EXEC_OprRet();						ret = TRUE;		break;	
		case OPRS_WAIT:		EXEC_OprWait();						ret = FALSE;	break;	
		case OPRS_TWAIT:		EXEC_OprTWait();						ret = FALSE;	break;	
		case OPRS_RUN:		EXEC_OprRun();						ret = FALSE;	break;	
		case OPRS_SLOAD:		EXEC_OprSLoad();						ret = FALSE;	break;	
	}

	return(	ret );
}






static BOOL EXEC_CallOprControl( int mode )
{
	int		opr;
	BOOL	ret;
	
	
	opr = *(WORD*)&EXEC_LangBuf[ EXEC_LangInfo->pc ];							




	if( opr < OPRS_OPREND ){	ret = EXEC_OprControl( opr );	}			
	else{																
		switch( mode ){													
			case SCRMODE_EVENT:	ret = ESC_OprControl( opr );	break;	
		}
	}

	return( ret );
}






static int EXEC_ControlTask( int mode )
{
	
	if( EXEC_LangInfo->BusyFlg == SCCODE_OPRS_WAIT_TWAIT )		EXEC_LangTWait();
	if( EXEC_LangInfo->BusyFlg == SCCODE_OPRS_WAIT_WAIT )		EXEC_LangWait();

	
	if( EXEC_LangInfo->BusyFlg == SCCODE_OPRS_RUN ){			
		while( EXEC_CallOprControl( mode ) );
	}

	
	return( EXEC_LangInfo->BusyFlg );
}






int EXEC_ControlLang( EXEC_DATA *scr )
{
	int		ret;

	
	EXEC_LangBuf  = (char *)scr->LangBuf;
	EXEC_LangInfo = scr;
	
	
	ret = EXEC_ControlTask( SCRMODE_EVENT );		

	return( ret );
}

void ESC_CodeCheck(void)
{
	return ;
	char	buf[1024];
	int	opr,i;
	i=0;

	if(!ScriptDialog){

		ShowWindow(ScriptDialog,SW_SHOW);
	}
	
	
	while( EXEC_LangBuf[ i ]!=0 ) {
		opr = EXEC_LangBuf[ i ];
		if( opr < OPRS_OPREND ){
			switch( opr ) {
				case OPRS_END:		i+=2;			return;			
				case OPRS_MOVR:		i+=4;			break;	
				case OPRS_MOVV:		i+=7;			break;	
				case OPRS_SWAP:		i+=4;			break;	
				case OPRS_RAND:		i+=3;			break;	
				case OPRS_IFR:		i+=9;			break;	
				case OPRS_IFV:		i+=12;			break;	
				case OPRS_IFELSER:	i+=9;			break;	
				case OPRS_IFELSEV:	i+=12;			break;	
				case OPRS_LOOP:		i+=7;			break;	
				case OPRS_GOTO:		i+=6;			break;	

				case OPRS_INC:     	i+=3;			break;		
				case OPRS_DEC:     	i+=3;			break;		
				case OPRS_NOT:     	i+=3;			break;		
				case OPRS_NEG:		i+=3;			break;	

				case OPRS_ADDR:										
				case OPRS_SUBR:										
				case OPRS_MULR:										
				case OPRS_DIVR:										
				case OPRS_MODR:										
				case OPRS_ANDR:										
				case OPRS_ORR:										
				case OPRS_XORR:	i=4;	break;						

				case OPRS_ADDV:
				case OPRS_SUBV:
				case OPRS_MULV:
				case OPRS_DIVV:
				case OPRS_MODV:
				case OPRS_ANDV:
				case OPRS_ORV:
				case OPRS_XORV:	i+=7;	break;

				case OPRS_CALC:	i=2;						break;	
				case OPRS_PUSHA:	i=2;						break;	
				case OPRS_POPA:	i=2;						break;	
				case OPRS_CALL:	i=7;						break;	
				case OPRS_RET:	i=2;					break;	
				case OPRS_WAIT:	i=4;					break;	
				case OPRS_TWAIT:	i=4;					break;	
				case OPRS_RUN:	i=2;					break;	
				case OPRS_SLOAD:					break;	
			}
			myOutputDebugString( "%s()\n", LangOprList[opr] );
		}else{
			wsprintf( buf, "%s( ", EventOprList[opr-64] );
			i+=GetLangParamControlEx( i, opr, buf );
			int len2 = strlen(buf)-4;
			while( !strncmp( buf+len2, "-1, ", 4 ) ){
				len2-=4;
			}
			int len = len2+2;

			if( buf[len]==',' ){
				buf[len]=' ';
				buf[len+1]=')';
				buf[len+2]='\0';
			}
			myOutputDebugString( "%s\n", buf );
		}
	}
}
