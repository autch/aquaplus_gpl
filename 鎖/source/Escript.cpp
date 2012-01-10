#include "AVGYMZ.h"
#include <io.h>
#include "escript.h"
#include "escr.h"
#include "graphic.h"
#include "backLog.h"
#include "msgWnd.h"
#include "saveWnd.h"
#include "soundDS.h"
#include "select.h"
#include "movie.h"


#define SCRIPT_MAX	1000
#define BLOCK_MAX	1024
#define CG_MAX		((5000+200)/8)	
BYTE ScenarioFlag[SCRIPT_MAX][BLOCK_MAX/8];
static int ScenarioNo = -1;
static int BlockNo    = 0;
BYTE	CgFlagBuf[ CG_MAX ];	

void InitReadFlag()
{
	DWORD	fSize;
	ZeroMemory(ScenarioFlag, SCRIPT_MAX*(BLOCK_MAX/8));
	ZeroMemory(CgFlagBuf, CG_MAX);
	ESC_InitGameFlag();
	if(0==_access("save_99.sav",0)){
		sysInf.configHF = CreateFile("save_99.sav",GENERIC_WRITE|GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
		ReadFile(sysInf.configHF,ScenarioFlag,SCRIPT_MAX*(BLOCK_MAX/8),&fSize,NULL);
		ReadFile(sysInf.configHF,CgFlagBuf,CG_MAX,&fSize,NULL);
		ReadFile(sysInf.configHF,ESC_GameFlagBuf,ESC_FLAG_MAX*4,&fSize,NULL);
	}else{
		sysInf.configHF = CreateFile("save_99.sav",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,NULL);
		WriteFile(sysInf.configHF,ScenarioFlag,SCRIPT_MAX*(BLOCK_MAX/8),&fSize,NULL);
		WriteFile(sysInf.configHF,CgFlagBuf,CG_MAX,&fSize,NULL);
		WriteFile(sysInf.configHF,ESC_GameFlagBuf,ESC_FLAG_MAX*4,&fSize,NULL);
	}
}

void ESC_SetGameFlag(int N, int D )
{
	ESC_GameFlagBuf[ N ] = D;
	
	DWORD	fSize;
	SetFilePointer(sysInf.configHF, SCRIPT_MAX*(BLOCK_MAX/8) +CG_MAX +N*4, NULL, FILE_BEGIN);
	WriteFile(sysInf.configHF,&ESC_GameFlagBuf[N],4,&fSize,NULL);
}



void AVG_SetCgFlag( int cg_no, int sub_no )
{
	DWORD	fSize;
	if(off==look1BitFlag(CgFlagBuf, 0)){
		set1BitFlag(CgFlagBuf, 0, on);	
		SetFilePointer(sysInf.configHF, SCRIPT_MAX*(BLOCK_MAX/8), NULL, FILE_BEGIN);
		WriteFile(sysInf.configHF,CgFlagBuf,1,&fSize,NULL);
	}
	set1BitFlag(CgFlagBuf, cg_no*20+sub_no+2, on);
	SetFilePointer(sysInf.configHF, SCRIPT_MAX*(BLOCK_MAX/8) +(cg_no*20+sub_no+2)/8, NULL, FILE_BEGIN);
	WriteFile(sysInf.configHF,&CgFlagBuf[(cg_no*20+sub_no+2)/8],1,&fSize,NULL);
}



BOOL AVG_GetCgFlag( int cg_no, int sub_no )
{
	int ret = look1BitFlag(CgFlagBuf, cg_no*20+sub_no+2 );
	return ret;

}



void AVG_SetSceneFlag( int scene_no )
{
	DWORD	fSize;
	if(off==look1BitFlag(CgFlagBuf, 1)){
		set1BitFlag(CgFlagBuf, 1, on);	
		SetFilePointer(sysInf.configHF, SCRIPT_MAX*(BLOCK_MAX/8), NULL, FILE_BEGIN);
		WriteFile(sysInf.configHF,CgFlagBuf,1,&fSize,NULL);
	}
	set1BitFlag(CgFlagBuf, 5000+scene_no, on);
	SetFilePointer(sysInf.configHF, SCRIPT_MAX*(BLOCK_MAX/8) +(5000+scene_no)/8, NULL, FILE_BEGIN);
	WriteFile(sysInf.configHF,&CgFlagBuf[(5000+scene_no)/8],1,&fSize,NULL);
}



BOOL AVG_GetSceneFlag( int scene_no )
{
	int ret = look1BitFlag(CgFlagBuf, 5000+scene_no );

	return TRUE;	
}

int GetScenarioNo( char *in_str )
{
	int	i, ret = 0, len = strlen(in_str);
	char	str[64];

	strcpy(str,in_str);
	for(i=0;i<len;i++){
		if(str[i]=='_'){
			str[i] ='\0';
			break;
		}
	}
	ret = atoi(str) *11;
	if(i < (len-1)){
		ret += (atoi(&str[i+1]) +1);
	}
	return ret;
}


void AVG_SetScenarioNo( int scenario_no )
{
	if(-1 != ScenarioNo){
		if( scenario_no >= SCRIPT_MAX ){
			myOutputDebugString("既読判定対象外スクリプトです。[%d]",scenario_no);
		}else if( ScenarioNo < SCRIPT_MAX ){
			DWORD fSize;
			set1BitFlag(ScenarioFlag[ScenarioNo], BlockNo, on);
			SetFilePointer(sysInf.configHF, ScenarioNo*BLOCK_MAX/8, NULL, FILE_BEGIN);
			WriteFile(sysInf.configHF,ScenarioFlag[ScenarioNo],BLOCK_MAX/8,&fSize,NULL);
		}
	}
	ScenarioNo = scenario_no;
	BlockNo = 0;
}


void AVG_SetScenarioFlag( int block_no )
{
	if( block_no < 0 )return;
	if( ScenarioNo >= SCRIPT_MAX )return;
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
	if( ScenarioNo >= SCRIPT_MAX )return 0;
	if( ESC_GetFlag( _回想モード中 ) ) return TRUE;
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
				j += wsprintf( &buf[j], "%d", EXEC_LangInfo->reg[ reg ] );
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

	mode = EXEC_LangBuf[ pc + (*start)++ ];						
	if(mode==2){
		int		i,len;
		char	buf[256];

		len   = EXEC_LangBuf[ pc + (*start)++ ];		
		for( i=0; i<len; i++ ) { buf[ i ] = EXEC_LangBuf[ pc + (*start)++ ]; }
		buf[ i ] = 0x00;
		ret = GetScriptParamCal( buf );
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



static int GetScriptParamByte( int pc, int *start )
{
	return EXEC_LangBuf[ pc + (*start)++ ];						
}



static int GetScriptParamWord( int pc, int *start )
{
	int	ret;

	ret  = EXEC_LangBuf[ pc + (*start)++ ];	
	ret |= EXEC_LangBuf[ pc + (*start)++ ]<<8;	
	return ret;
}



static int GetScriptParam_str( int pc, int *start, char *buf )
{
	int	i,len;

	len   = EXEC_LangBuf[ pc + (*start)++ ];		
	for( i=0; i<len; i++ ) { buf[ i ] = EXEC_LangBuf[ pc + (*start)++ ]; }
	buf[ i ] = 0x00;
	return *start;
}



static int GetScriptParam_str2( int pc, int *start, char *buf )
{
	int	i,len;

	len  = EXEC_LangBuf[ pc + (*start)++ ];	
	len |= EXEC_LangBuf[ pc + (*start)++ ]<<8;	
	for( i=0; i<len; i++ ) { buf[ i ] = EXEC_LangBuf[ pc + (*start)++ ]; }
	buf[ i ] = 0x00;
	return *start;
}




static int GetScriptParamControl( int opr )
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
			case ESC_ASC:	EscParam[i].num = GetScriptParamByte( EXEC_LangInfo->pc, &cnt );	break;	
			case ESC_CNT:	EscParam[i].num = GetScriptParamWord( EXEC_LangInfo->pc, &cnt );	break;	
			case ESC_VCNT:	EscParam[i].num = GetScriptParamWord( EXEC_LangInfo->pc, &cnt );	break;	
			case ESC_NUM:	EscParam[i].num = GetScriptParam( EXEC_LangInfo->pc, &cnt, 4 );	break;	
			case ESC_STR:	GetScriptParam_str( EXEC_LangInfo->pc, &cnt, EscParam[i].str );	break;	
			case ESC_STR2:	GetScriptParam_str2( EXEC_LangInfo->pc, &cnt, EscParam[i].str );	break;	
			case ESC_CMP:
				{
					int	param1, param2, param3;
					param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + cnt++ ];	
					param1 = EXEC_GetParam( param1 );						
					param2 = EXEC_LangBuf[ EXEC_LangInfo->pc + cnt++ ];	
					param3 = GetScriptParam( EXEC_LangInfo->pc, &cnt, 4 );						
					
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



static int GetScriptParamControlEx( int pc, int opr, char *str )
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
					param1 = EXEC_LangBuf[ pc + cnt++ ];	
					param1 = EXEC_GetParam( param1 );						
					param2 = EXEC_LangBuf[ pc + cnt++ ];	
					param3 = GetScriptParam( pc, &cnt, 4 );						
					
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
	
	ESC_SetGameFlag( EscParam[0].num, EscParam[1].num );
	if( EscParam[0].num>=30 && EscParam[0].num<100 && EscParam[1].num>0){
		ESC_SetGameFlag( 100, 1 );
	}
	
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
	
	int index = EXEC_SLoad( str );
	AVG_SetScenarioNo( index );

}
static void ESC_EOprLoadScript( void )
{
	LoadScript( EscParam[0].str );
}




static void LoadScriptNum( int	fno )
{
	char	str[256];
	wsprintf( str, "%05d.sdt", fno );
	
	int index = EXEC_SLoad( str );
	AVG_SetScenarioNo( index );


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

static void ESC_EOprSetNoiseScreen( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 1;	
	if(EscParam[1].num==-1) EscParam[1].num = 1200;	
	if(EscParam[2].num==-1) EscParam[2].num = 1800;	
	if(0==EscParam[0].num){
		saveInf.noiseType = 0;	
	}else{
		saveInf.noiseType = rndSelect(7)+1;	
	}
	saveInf.noiseFrameMin = EscParam[1].num;
	saveInf.noiseFrameMax = EscParam[2].num;
	saveInf.noiseTime = timeGetTime() +saveInf.noiseFrameMin*1000/60;
	saveInf.noiseTime += ((saveInf.noiseFrameMax -saveInf.noiseFrameMin)*rndSelect(100)*1000/60/100);
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprGetItem( void )
{
	if(NULL==itemList){
		itemList = new ItemList(EscParam[0].num);
		MessageMenuSet( off );
	}
	if(itemList->GetItem()){

		my_delete(itemList);
		EXEC_AddPC( EscCnt );
	}
}




static void ESC_EOprSetChapter( void )
{
	saveChapterTitle( EscParam[0].str );
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetRoom( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 0;	
	if(EscParam[1].num==-1) EscParam[1].num = 0;	

	SetRoomNo( EscParam[0].num, EscParam[1].num );
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
	if(EOprFlag[ESC_SETBACKFADERGB]==0){
		
		EOprFlag[ESC_SETBACKFADERGB] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if( FALSE==SetBackFade( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num ) ){	
		EOprFlag[ESC_SETBACKFADERGB] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}




static void ESC_EOprSetShake( void )
{
	
	if(!EOprFlag[ESC_SETSHAKE]){
		EOprFlag[ESC_SETSHAKE] = 1;
		if(EscParam[4].num==-1) EscParam[4].num=256;

	}
	if(bgInf.look_cnt==0){
		bgInf.in_type = EscParam[0].num +60;
		bgInf.shake_direct = (BYTE)EscParam[3].num;
		bgInf.shakeSize = EscParam[1].num;
		if(EscParam[3].num==1 || EscParam[3].num==3) bgInf.shakeSize *= -1;
		bgInf.look_cnt = timeGetTime();
		bgInf.look_max = EscParam[2].num *1000/60;
		if( bgInf.in_type==_BAK_ROLL && (2==EscParam[3].num || 3==EscParam[3].num) ){
			saveInf.shakeSize = bgInf.shakeSize;
		}
		if(EscParam[2].num==0){
			EOprFlag[ESC_SETSHAKE] = 0;
			EXEC_AddPC( EscCnt );
			return;
		}
	}
	if((bgInf.look_cnt+bgInf.look_max) <= timeGetTime() || EscParam[1].num==0 || G_SKIP){
		if( bgInf.in_type==_BAK_ROLL && (2==EscParam[3].num || 3==EscParam[3].num) ){
			
			backColorBuf.RollBlt(&bgInf.colorBuf, bgInf.shakeSize*180.0f);
			bgInf.colorBuf.BltFast(0,0,&backColorBuf,NULL,FALSE);
		}
		bgInf.look_cnt = bgInf.shakeSize = bgInf.shake_direct = 0;
		bgInf.in_type = _BAK_DIRECT;
		EOprFlag[ESC_SETSHAKE] = 0;
		EXEC_AddPC( EscCnt );
	}
}


static void ESC_EOprStopShake( void )
{
	
	bgInf.look_cnt = bgInf.shake_direct = 0;
	bgInf.in_type = _BAK_DIRECT;
	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetFlash( void )
{
	
	if(!EOprFlag[ESC_SETFLASH]){
		EOprFlag[ESC_SETFLASH] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
		return;
	}
	BOOL bIn = FALSE;
	if(1==EOprFlag[ESC_SETFLASH])bIn = TRUE;
	if(FALSE==SetFlash( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num ,bIn)){
		if(2==EOprFlag[ESC_SETFLASH]){
			EOprFlag[ESC_SETFLASH] = 0;
			EXEC_AddPC( EscCnt );
		}else{
			EOprFlag[ESC_SETFLASH] = 2;
		}
	}
	




}




static void ESC_EOprSetChar( void )
{
	
	if(EscParam[2].num==-1) EscParam[2].num = 1;
	if(EscParam[3].num==-1) EscParam[3].num = 0;
	if(EscParam[4].num==-1) EscParam[4].num = 0;
	if(EscParam[5].num==-1) EscParam[5].num = 128;
	if(EscParam[6].num==-1) EscParam[6].num = 255;
	if(EscParam[7].num==-1) EscParam[7].num = 30; 
	if(EscParam[8].num==-1) EscParam[8].num = _CHR_DEFAULTPOS; 
	if(EscParam[9].num==-1) EscParam[9].num = _CHR_DEFAULTPOS; 
	


	
	c_disp(EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[6].num, EscParam[7].num,EscParam[5].num,EscParam[8].num,EscParam[9].num);
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



static void ESC_EOprSetMessage2(BOOL bADD)
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
		if(FALSE==bADD)AVG_SetScenarioFlag( EscParam[2].num );

		BOOL tmp_AlreadyRead =  AVG_CheckScenarioFlag();
		if(tmp_AlreadyRead){
			saveInf.textColor[0] = 0x40;
		}else{
			saveInf.textColor[0] = 0xff;
		}
		saveInf.textColor[1] = saveInf.textColor[2] = 0xff;
		if(sysInf.bReadSkip){
			sysInf.bAlreadyRead = tmp_AlreadyRead;
			if(off==sysInf.bAlreadyRead){
				sysInf.bReadSkip = off;
				CheckMenuItem( sysInf.hMenu, ID_READSKIP,   MF_UNCHECKED );
				msgWnd.msgBtn[4].SetState(btn_normal);
			}
		}else{
			sysInf.bAlreadyRead = tmp_AlreadyRead;
			if(off==sysInf.bAlreadyRead && FALSE==sysInf.bAllSkip){
				keyState.data_skip = 0;
			}

		}
		saveMessage((LPBYTE)msgBuf);	
	}
	if(msgBuf){
		msgBuf = msgWnd.DisplayMessage((BYTE *)msgBuf);
	}
	if(NULL==msgBuf){
		switch(EOprFlag[ESC_SETMESSAGE2]){
		  case 1:
			backLog.SetBackLog(EscParam[0].str, msgWnd.logMsgLine, EscParam[1].num!=2);
			EOprFlag[ESC_SETMESSAGE2] = 2;
			if(EscParam[1].num==2){
				if(off==sysInf.bMsgAlreadyStop){
					sysInf.bMsgStop = on;

					sysInf.msgStopTime = timeGetTime() +(11-sysInf.autoMsg) *2000/11;
				}
			}
			break;
		  case 2:

			if(FALSE==msgWnd.msgStopCheck()){

				if(EscParam[1].num==2){
					sysInf.bMsgCLS = on;
				}
				EOprFlag[ESC_SETMESSAGE2] = 0;
				saveInf.RestartVoice[0] = 0;
				
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
	ESC_EOprSetMessage2(TRUE);

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
		
		SAV_Save( 98 );
		EnableMenuItem( sysInf.hMenu, ID_BACKSELECT, MF_ENABLED );

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
	if(EscParam[0].num==-1) EscParam[0].num = 0;
	if(EscParam[1].num==-1) EscParam[1].num = 30;

	lpSoundDS->SetVolumeFade(bgmHandle, EscParam[0].num, EscParam[1].num);
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSetVolumeBgmEx(void)
{






	EXEC_AddPC( EscCnt );
}




static void ESC_EOprPlaySe(void)
{
	if( EscParam[1].num==-1) EscParam[1].num = 0xff;

	PlayEffectNum(evtHandle[0],EscParam[0].num,EscParam[1].num);

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprPlaySeEx(void)
{
	if( EscParam[2].num==-1) EscParam[2].num = 0;	
	if( EscParam[3].num==-1) EscParam[3].num = 0;	
	if( EscParam[4].num==-1) EscParam[4].num = 0xff;	
	PlayEffectNum(evtHandle[EscParam[0].num],EscParam[1].num,EscParam[4].num,EscParam[3].num,EscParam[2].num);
	EXEC_AddPC( EscCnt );
}



static void ESC_EOprStopSeEx(void)
{
	if( EscParam[1].num==-1) EscParam[1].num = 0;
	if(0==EscParam[1].num){
		StopEffect(evtHandle[EscParam[0].num]);
	}else{
		FadeOutEffect(evtHandle[EscParam[0].num],EscParam[1].num);
	}

	EXEC_AddPC( EscCnt );
}



static void ESC_EOprSetVolumeSe(void)
{
	lpSoundDS->SetVolume(evtHandle[ EscParam[0].num ], EscParam[1].num, lpSoundDS->seVolume);

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
	if(EscParam[0].num==-1) EscParam[0].num = 30;
	if(EscParam[1].num==-1) EscParam[1].num = 0;
	static DWORD avg_waitTime = 0;

	if(0==EOprFlag[ESC_WAITFRAME]){
		EOprFlag[ESC_WAITFRAME] = 1;
		if(0==EscParam[0].num || 0==EscParam[1].num || 0==avg_waitTime){
			avg_waitTime = timeGetTime() +EscParam[0].num*1000/60;
		}else{
			avg_waitTime = avg_waitTime +EscParam[0].num*1000/60;
		}
	}
	if( timeGetTime() >= avg_waitTime || G_SKIP ){	
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
	if(EscParam[3].num==-1) EscParam[3].num=-1;
	if(EscParam[4].num==-1) EscParam[4].num=1;

	if(EscParam[6].num==-1) EscParam[6].num=1;
	

	setBmpInf[EscParam[0].num].LoadBmpAnime( EscParam[1].str,  EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num, EscParam[6].num);
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWaitBmpAnime( void )
{
	if( setBmpInf[EscParam[0].num].WaitBmpAnime() ){	



		EXEC_AddPC( EscCnt );	
	}
}
static void ESC_EOprResetBmpAnime( void )
{
	


	setBmpInf[EscParam[0].num].Release();
	
	EXEC_AddPC( EscCnt );
}




static void ESC_EOprSetTitle( void )
{
	ReturnTitile();
	EXEC_AddPC( EscCnt );
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

	setBmpInf[EscParam[0].num].SetAvi( EscParam[1].str, EscParam[2].num );
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprResetAvi( void )
{
	

	setBmpInf[EscParam[0].num].Release();
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprWaitAvi( void )
{
	if( setBmpInf[EscParam[0].num].WaitAvi()  || G_SKIP){
		EXEC_AddPC( EscCnt );
	}
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
	

	setBmpInf[EscParam[0].num].SetGraphParam( EscParam[1].num,  EscParam[2].num);
	
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
	

	setBmpInf[EscParam[0].num].SetBmpMove(EscParam[1].num, EscParam[2].num );
	
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
	


	setBmpInf[EscParam[0].num].SetBmpZoom2(EscParam[1].num, EscParam[2].num, EscParam[3].num);
	
	EXEC_AddPC( EscCnt );
}

static void ESC_EOprSetBmpAnimePlay(void)
{
	if(EscParam[1].num==-1) EscParam[1].num = 0;
	if(EscParam[2].num==-1) EscParam[2].num = 1;	
	if(EscParam[3].num==-1) EscParam[3].num = 1;	
	setBmpInf[EscParam[0].num].SetBmpAnimePlay(EscParam[1].num, EscParam[2].num, EscParam[3].num);
	
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

	if(0==scrWaitTime) scrWaitTime = timeGetTime()+(EscParam[0].num*1000/60);

	if( timeGetTime() >= scrWaitTime ){
		scrWaitTime = 0;
		EXEC_AddPC( EscCnt );
	}
}

static void ESC_EOprStandbyTime( void )
{
	if( timeGetTime() >= EscParam[0].num ){
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
	
	if(EscParam[1].num!=-1) bak_no = EscParam[1].num*100;					
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;	
	if(EscParam[3].num==-1) EscParam[3].num = 30;	
	if(EscParam[4].num==-1) EscParam[4].num = 0;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;	


	if(EOprFlag[ESC_V]==0 && _BAK_DIRECT!=EscParam[0].num){
		
		EOprFlag[ESC_V] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
		if(0<EscParam[1].num && 999>EscParam[1].num)SetRoomNo( EscParam[1].num, 0 );
	}else if(g_disp(bak_no, EscParam[0].num, 0, EscParam[3].num, EscParam[4].num, EscParam[5].num, TRUE)){
		EOprFlag[ESC_V] = 0;
		EXEC_AddPC( EscCnt );
	}

}
static void ESC_EOprBT( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*100;					
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;	
	if(EscParam[3].num==-1) EscParam[3].num = 30;	
	if(EscParam[4].num==-1) EscParam[4].num = 0;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;	

	if(0<EscParam[1].num && 999>EscParam[1].num)SetRoomNo( EscParam[1].num, 0 );
	if(g_disp(bak_no, EscParam[0].num, 0, EscParam[3].num, EscParam[4].num, EscParam[5].num, TRUE)){
		EOprFlag[ESC_V] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprBC( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*100;
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = 30;	
	if(EscParam[4].num==-1) EscParam[4].num = 0;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;	

	if(EOprFlag[ESC_V]==0 && _BAK_DIRECT!=EscParam[0].num){
		
		EOprFlag[ESC_V] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
		if(0<EscParam[1].num && 999>EscParam[1].num)SetRoomNo( EscParam[1].num, 0 );
	}else if(g_disp(bak_no, EscParam[0].num, 0, EscParam[3].num, EscParam[4].num, EscParam[5].num)){
		EOprFlag[ESC_V] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprBCT( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*100;
	if(EscParam[2].num!=-1) bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = 30;	
	if(EscParam[4].num==-1) EscParam[4].num = 0;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;	

	if(0<EscParam[1].num && 999>EscParam[1].num)SetRoomNo( EscParam[1].num, 0 );
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

	bak_no = EscParam[1].num*100;
	if(EscParam[2].num==-1)	EscParam[2].num = 0;
	bak_no += EscParam[2].num;
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
			AVG_SetCgFlag( EscParam[1].num, EscParam[2].num );
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

		bak_no = EscParam[1].num*100;
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
	bak_no = EscParam[1].num*100;
	if(EscParam[2].num==-1)	EscParam[2].num = 0;
	bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = 30;
	if(EscParam[4].num==-1) EscParam[4].num = off;
	if(EscParam[5].num==-1) EscParam[5].num = 0;
	if(EscParam[6].num==-1) EscParam[6].num = 0;

	if(EOprFlag[ESC_H]==0 && _BAK_DIRECT!=EscParam[0].num){
		
		EOprFlag[ESC_H] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if(g_disp(bak_no, EscParam[0].num, 2, EscParam[3].num, EscParam[5].num, EscParam[6].num, TRUE)){
		AVG_SetCgFlag( EscParam[1].num, EscParam[2].num );
		EOprFlag[ESC_H] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprHT( void )
{
	int	bak_no;

	bak_no = EscParam[1].num*100;
	if(EscParam[2].num==-1)	EscParam[2].num = 0;
	bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) EscParam[3].num = 30;
	if(EscParam[4].num==-1) EscParam[4].num = off;
	if(EscParam[5].num==-1) EscParam[5].num = 0;
	if(EscParam[6].num==-1) EscParam[6].num = 0;

	if(EOprFlag[ESC_HT]==0 && _BAK_DIRECT!=EscParam[0].num){
		
		EOprFlag[ESC_HT] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if(g_disp(bak_no, EscParam[0].num, 2, EscParam[3].num, EscParam[5].num, EscParam[6].num, TRUE)){
		AVG_SetCgFlag( EscParam[1].num, EscParam[2].num );
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
	if(EOprFlag[ESC_FI]==0){
		
		EOprFlag[ESC_FI] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if( FALSE==SetBackFade( 128, 128, 128, 30 ) ){	
		EOprFlag[ESC_FI] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprFIF( void )
{
	if(EOprFlag[ESC_FIF]==0){
		
		EOprFlag[ESC_FIF] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if( FALSE==SetBackFade( 128, 128, 128, EscParam[0].num ) ){	
		EOprFlag[ESC_FIF] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprFO( void )
{
	if(EOprFlag[ESC_FO]==0){
		
		EOprFlag[ESC_FO] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if( FALSE==SetBackFade( 0, 0, 0, 30 ) ){	
		EOprFlag[ESC_FO] = 0;
		
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprFOF( void )
{
	if(EOprFlag[ESC_FOF]==0){
		
		EOprFlag[ESC_FOF] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
	}else if( FALSE==SetBackFade( 0, 0, 0, EscParam[0].num ) ){	
		EOprFlag[ESC_FOF] = 0;
		
		EXEC_AddPC( EscCnt );
	}
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
		if(EscParam[4].num==-1) EscParam[4].num=256;
	}
	if(bgInf.look_cnt==0){
		bgInf.in_type = EscParam[0].num +60;
		bgInf.shake_direct = (BYTE)EscParam[3].num;
		bgInf.shakeSize = EscParam[1].num;
		if(EscParam[3].num==1 || EscParam[3].num==3) bgInf.shakeSize *= -1;
		bgInf.look_cnt = timeGetTime();
		bgInf.look_max = EscParam[2].num *1000/60;
		if( bgInf.in_type==_BAK_ROLL && (2==EscParam[3].num || 3==EscParam[3].num) ){
			saveInf.shakeSize = bgInf.shakeSize;
		}
		if(EscParam[2].num==0){
			EOprFlag[ESC_Q] = 0;
			EXEC_AddPC( EscCnt );
			return;
		}
	}
	if((bgInf.look_cnt+bgInf.look_max) <= timeGetTime() || EscParam[1].num==0 || G_SKIP){
		if( bgInf.in_type==_BAK_ROLL && (2==EscParam[3].num || 3==EscParam[3].num) ){
			
			backColorBuf.RollBlt(&bgInf.colorBuf, bgInf.shakeSize*180.0f);
			bgInf.colorBuf.BltFast(0,0,&backColorBuf,NULL,FALSE);
		}
		bgInf.look_cnt = bgInf.shakeSize = bgInf.shake_direct = 0;
		bgInf.in_type = _BAK_DIRECT;
		EOprFlag[ESC_Q] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprF( void )
{
	if(!EOprFlag[ESC_F]){
		EOprFlag[ESC_F] = 1;
		msgWnd.ForceHide();	
		sysInf.bForceRender = on;
		return;
	}
	BOOL bIn = FALSE;
	if(1==EOprFlag[ESC_F])bIn = TRUE;
	if(FALSE==SetFlash( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num ,bIn)){
		if(2==EOprFlag[ESC_F]){
			EOprFlag[ESC_F] = 0;
			EXEC_AddPC( EscCnt );
		}else{
			EOprFlag[ESC_F] = 2;
		}
	}




	




}


static void ESC_EOprC( void )
{
	
	if(EscParam[2].num==-1){

		int pos = CheckCharLocate( EscParam[0].num );
		EscParam[2].num = (pos==-1) ? 1 : pos ;
	}
	if(EscParam[3].num==-1) EscParam[3].num = 0;
	if(EscParam[3].num==-1) EscParam[3].num = -2;
	if(EscParam[4].num==-1) EscParam[4].num = 0;
	if(EscParam[5].num==-1) EscParam[5].num = 128;
	if(EscParam[6].num==-1) EscParam[6].num = 256;
	if(EscParam[7].num==-1) EscParam[7].num = 30;	
	if(EscParam[8].num==-1) EscParam[8].num = _CHR_DEFAULTPOS; 
	if(EscParam[9].num==-1) EscParam[9].num = _CHR_DEFAULTPOS; 
	if(c_disp(EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[6].num, EscParam[7].num,EscParam[5].num,EscParam[8].num,EscParam[9].num)){
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
	
	c_disp(EscParam[0].num, EscParam[1].num, EscParam[2].num, _CHR_WAIT, EscParam[3].num,255,0,EscParam[4].num);


	EXEC_AddPC( EscCnt );
}
static void ESC_EOprCRW( void )
{
	
	c_cls(EscParam[0].num, _CHR_WAIT, 0);

	
	EXEC_AddPC( EscCnt );
}


static void ESC_EOprW( void )
{
	

	if(EscParam[0].num==-1) EscParam[0].num = on;
	if(!EOprFlag[ESC_W]){
		EOprFlag[ESC_W] = 1;
		sysInf.bShowMsgwin = on;
		sysInf.bHideMsgwin = off;
		if(off==EscParam[0].num){
			EOprFlag[ESC_W] = 0;
			EXEC_AddPC( EscCnt );
		}
	}else if(msgWnd.GetAlpha()>=1.0f){
		EOprFlag[ESC_W] = 0;
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprWR( void )
{
	

	if(EscParam[0].num==-1) EscParam[0].num = on;
	if(!EOprFlag[ESC_WR]){
		EOprFlag[ESC_WR] = 1;
		sysInf.bShowMsgwin = off;
		sysInf.bHideMsgwin = off;
		if(off==EscParam[0].num){
			EOprFlag[ESC_WR] = 0;
			EXEC_AddPC( EscCnt );
		}
	}else if(msgWnd.GetAlpha()<=0){
		EOprFlag[ESC_WR] = 0;
		EXEC_AddPC( EscCnt );
	}
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
	if(EscParam[1].num==-1) EscParam[1].num = 30;
	if(EscParam[2].num==-1 || EscParam[2].num==1) EscParam[2].num = 0;
	else EscParam[2].num = 1;
	if(EscParam[3].num==-1) EscParam[3].num = 0xff; 
	lpSoundDS->ChangeMusic(bgmHandle, EscParam[0].num, TRUE, EscParam[2].num, EscParam[1].num, EscParam[3].num);
	ESC_SetGameFlag( EscParam[0].num+200, 1 );
	ESC_SetGameFlag( 230, 1 );	

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMS( void )
{
	int		fade = EscParam[0].num;
	if(EscParam[0].num==-1) fade = 25;

	if(fade>0){
		lpSoundDS->FadeOut(bgmHandle,fade);
	}else{
		lpSoundDS->Stop(bgmHandle);
	}
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMP( void )
{
	int		flag = EscParam[0].num;

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMV( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 0;
	if(EscParam[1].num==-1) EscParam[1].num = 30;

	lpSoundDS->SetVolumeFade(bgmHandle, EscParam[0].num, EscParam[1].num);
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprMW(void)
{
	if(Cl_OK==lpSoundDS->FadeOut(bgmHandle)){
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
	if( EscParam[2].num==-1) EscParam[2].num = 0;	
	if( EscParam[3].num==-1) EscParam[3].num = 0;	
	if( EscParam[4].num==-1) EscParam[4].num = 0xff;	
	
	PlayEffectNum(evtHandle[EscParam[0].num],EscParam[1].num,EscParam[4].num,EscParam[3].num,EscParam[2].num);

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSES( void )
{
	if( EscParam[1].num==-1) EscParam[1].num = 0;
	if(0==EscParam[1].num){
		StopEffect(evtHandle[EscParam[0].num]);
	}else{
		FadeOutEffect(evtHandle[EscParam[0].num],EscParam[1].num);
	}

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSEV( void )
{
	lpSoundDS->SetVolume(evtHandle[ EscParam[0].num ], EscParam[1].num, lpSoundDS->seVolume);

	EXEC_AddPC( EscCnt );
}
static void ESC_EOprSEW( void )
{
	if(PCM_STOP==lpSoundDS->GetStatus(evtHandle[ EscParam[0].num ])){
		
		EXEC_AddPC( EscCnt );
	}else if(keyState.data_skip || sysInf.bReadSkip){  
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
	if(EscParam[2].num==-1) EscParam[2].num = FALSE;	
	if(!EOprFlag[ESC_SETMOVIE]){
		EOprFlag[ESC_SETMOVIE] = 1;
		my_delete(lpMovie);
		lpMovie = new CMovie(sysInf.nouseOverlay);
		lpMovie->OpenMovie(EscParam[0].str,EscParam[1].num, EscParam[2].num);
	}
	if( NULL==lpMovie || !lpMovie->IsPlay() ){
		my_delete(lpMovie);
		EOprFlag[ESC_SETMOVIE] = 0;
		EXEC_AddPC( EscCnt );
	}
}


int checkPlayVoice(int charNo)
{
	if(charNo < 0)return -1;
	int		checkNo = charNo;
	if(99==checkNo)checkNo = 9;	
	if(checkNo < 10){	
		if( ESC_GetFlag( 254 ) && (sysInf.voiceFlag&(1<<10)) ){	
			if(charNo>=6 && charNo<=8) return -1; 
		}
		if(sysInf.voiceFlag & (1<<checkNo)){ 
			return charNo;
		}
		return -1;
	}
	int Kyosuke = -1;
	int Heroine = -1;
	if(sysInf.voiceFlag & (1<<8)) Kyosuke = 8;
	switch(charNo){
	  case 10:	
		if(sysInf.voiceFlag & (1<<3)) Heroine = 3;
		break;
	  case 11:	
		if(sysInf.voiceFlag & (1<<4)) Heroine = 4;
		break;
	  case 12:	
		if(sysInf.voiceFlag & (1<<0)) Heroine = 0;
		break;
	  default:
		return -1;
	}
	if(Kyosuke>=0){
		if(Heroine>=0) return charNo;
		return Kyosuke;
	}
	if(Heroine>=0) return Heroine;
	return -1;
}


static void ESC_EOprVV( void )
{
	if(EscParam[1].num==-1) EscParam[1].num = 255;		
	if(EscParam[2].num==-1) EscParam[2].num = FALSE;	
	

	backLog.SetVoiceNum(EscParam[0].num);
	if(!keyState.data_skip && !sysInf.bReadSkip){ 
		int charNo = checkPlayVoice(EscParam[0].num);
		if(charNo >= 0){ 
			PlayVoiceNum(charNo, EscParam[2].num, EscParam[1].num, EscParam[3].num);
		}
	}else{
#ifndef _MASTER
		int charNo = checkPlayVoice(EscParam[0].num);
		if(charNo >= 0){ 
			char workBuf[64];
			if(-1==EscParam[3].num)
				wsprintf(workBuf,"%05d_%03d_%03d.ogg",lpSoundDS->scriptNo,lpSoundDS->voiceCnt,charNo);
			else
				wsprintf(workBuf,"%05d_%03d_%03d.ogg",lpSoundDS->scriptNo,EscParam[3].num,charNo);
			int num = readFile->SearchFile(pack_voice,workBuf);
			if(num==-1){
				myOutputDebugString("Voice %s が見つかりませんでした",workBuf);
			}
		}
#endif
	}
	saveInf.RestartVoice[0] = EscParam[0].num +1;
	saveInf.RestartVoice[1] = lpSoundDS->voiceCnt;
	saveInf.RestartVoice[2] = lpSoundDS->scriptNo;
	if(-1==EscParam[3].num) lpSoundDS->voiceCnt ++;	
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprVA( void )
{
	ESC_EOprVV();
}
static void ESC_EOprVB( void )
{
	ESC_EOprVV();
}
static void ESC_EOprVC( void )
{
	ESC_EOprVV();
}
static void ESC_EOprVX( void )
{
	if(EscParam[3].num==-1) EscParam[3].num = 255;		
	if(EscParam[4].num==-1) EscParam[4].num = FALSE;	
	if(EscParam[5].num==-1) EscParam[5].num = 0;		

	char voicefile[32];

	int charNo = checkPlayVoice(EscParam[0].num);
	if(charNo >= 0){ 
		wsprintf(voicefile,"%05d_%03d_%03d",EscParam[1].num,EscParam[2].num,charNo);
		if(!keyState.data_skip && !sysInf.bReadSkip){ 
			PlayVoice(voicefile, EscParam[4].num, EscParam[3].num);
		}else{
#ifndef _MASTER
			char workBuf[64];
			wsprintf(workBuf,"%s.ogg",voicefile);
			int num = readFile->SearchFile(pack_voice,workBuf);
			if(num==-1){
				myOutputDebugString("Voice %s が見つかりませんでした",workBuf);
			}
#endif
		}
	}else{
		wsprintf(voicefile,"%05d_%03d_%03d",EscParam[1].num,EscParam[2].num,EscParam[0].num);
	}
	backLog.SetVoice(voicefile);
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprVW( void )
{
	if(PCM_STOP==lpSoundDS->GetStatus(voiceHandle)){
		
		EXEC_AddPC( EscCnt );
	}
}
static void ESC_EOprVS( void )
{
	if(EscParam[0].num==-1) EscParam[0].num = 0;		
	if(EscParam[1].num==-1) EscParam[1].num = 0;		
	if(EscParam[0].num==0){
		lpSoundDS->Stop(voiceHandle);
	}else{
		lpSoundDS->FadeOut(voiceHandle,EscParam[0].num);
	}
	
	EXEC_AddPC( EscCnt );
}
static void ESC_EOprVI( void )
{
	lpSoundDS->SetVoiceNo( EscParam[0].num, EscParam[1].num );
	myOutputDebugString("Voiceカウンタ更新[%05d_%03d_xxx]",lpSoundDS->scriptNo,lpSoundDS->voiceCnt);
	
	EXEC_AddPC( EscCnt );
}








int ESC_OprControl( int opr )
{
	int	i=ESC_LOADSCRIPT;
	
	EscCnt = GetScriptParamControl( opr );
	if(opr!=ESC_SETMESSAGE2) saveInf.RestartVoice[0] = 0;	

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

		case ESC_VV:				ESC_EOprVV();			break;	
		case ESC_VA:				ESC_EOprVA();			break;	
		case ESC_VB:				ESC_EOprVB();			break;	
		case ESC_VC:				ESC_EOprVC();			break;	
		case ESC_VX:				ESC_EOprVX();			break;	
		case ESC_VW:				ESC_EOprVW();			break;	
		case ESC_VS:				ESC_EOprVS();			break;	
		case ESC_VI:				ESC_EOprVI();			break;	

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
		case ESC_SETMESSAGE2:		ESC_EOprSetMessage2(FALSE);	break;	
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
		case ESC_SETBMPANIMEPLAY:	ESC_EOprSetBmpAnimePlay();	break;	

		case ESC_SETDEMOFLAG:		ESC_EOprSetDemoFlag();		break;	
		case ESC_SETSCENENO:		ESC_EOprSetSceneNo();		break;	
		case ESC_SETENDINGNO:		ESC_EOprSetEndingNo();		break;	
		case ESC_SETREPLAYNO:		ESC_EOprSetReplayNo();		break;	

		case ESC_SETSOUNDEVENT:			ESC_EOprSetSoundEvent();		break;	
		case ESC_SETSOUNDEVENTVOLUME:	ESC_EOprSetSoundEventVolume();	break;	
	
		case ESC_SETPOTAPOTA:		ESC_EOprSetPotaPota();		break;	

		case ESC_GETTIME:			ESC_EOprGetTime();			break;	
		case ESC_WAITTIME:			ESC_EOprWaitTime();			break;	
		case ESC_STANDBYTIME:		ESC_EOprStandbyTime();		break;	

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
		case ESC_SETNOISESCREEN:	ESC_EOprSetNoiseScreen();	break;
		case ESC_GETITEM:			ESC_EOprGetItem();			break;
		case ESC_SETCHAPTER:		ESC_EOprSetChapter();		break;
		case ESC_SETROOM:			ESC_EOprSetRoom();			break;
	}

	SetScriptRegisterControl( opr );

	if(ESC_C==opr){
		if(EscParam[3].num==_CHR_WAIT){
			return ESC_NOWAIT;	
		}
	}
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

static char	*PackLangDir=NULL;

void EXEC_SetPackDir( char *pac_dir )
{
	PackLangDir = pac_dir;
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







int EXEC_ReadLang( char *filename, EXEC_DATA *scr )
{
	SCRIPT_HEADER	*header;
	int				i, size,index;
	char			*buf=NULL;

	MessageMenuSet( off );	
	lpSoundDS->Stop(voiceHandle);	
	

	index = readFile->SearchFile(pack_script, filename);
	if(index<0){
		myOutputDebugString("packされたスクリプトではありません。\n" );
		index = 0;
	}
	size = readFile->ReadPackFile(pack_script,filename,(char **)&buf);
	
	lpSoundDS->scriptNo = atoi(filename);
	lpSoundDS->voiceCnt = 0;
	if(size==0){
		return -1;
	}
	
	header = (SCRIPT_HEADER*)buf;
	if( ( header->h1 != 'L' ) || ( header->h2 != 'F' ) ) {
		myOutputDebugString("スクリプトファイルではありません。\n" );
		cl_free( buf );
		return -1;
	}
#ifndef _MASTER
	mySetWindowText(sysInf.hWnd, "鎖－クサリ－ script: %s",filename);
#endif
	
	size = header->Fsize - sizeof( SCRIPT_HEADER );		
	cl_free(scr->LangBuf);
	scr->LangBuf = (char *)cl_malloc( size );
	CopyMemory( scr->LangBuf, buf + sizeof(SCRIPT_HEADER), size );

	
	for( i=0; i<SCRIPT_BLOCK_MAX; i++ ) { scr->BlockAddres[i] = header->BlockAddres[i]; }

	
	scr->BusyFlg  = SCCODE_NOOPR;								

	wsprintf( NowLangFileName, "%s", filename );

	cl_free( buf );
	return( index );
}







BOOL EXEC_StartLang( EXEC_DATA *scr, int scr_no )
{
	int		i;
	
	
	if( (scr->BusyFlg != SCCODE_NOOPR) && (scr->BusyFlg != SCCODE_WAIT_SLOAD) )    { return( FALSE ); }	
	if( scr->BlockAddres[ scr_no ] == 0 ) { return( FALSE ); }	


	
	scr->BusyFlg = SCCODE_RUN;									
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
	scr->BusyFlg   = SCCODE_NOOPR;
	scr->pc        = 0;

	scr->sp      = 0;										
	for( i=0; i<STACK_MAX; i++ ) { scr->sp_buf[ i ] = 0; }	
	for( i=0; i<REGISTER_MAX; i++ ) { scr->reg[ i ] = 0; }	
	
	RunLangBlockNo = -1;
}






void EXEC_EndLang( EXEC_DATA *scr, int init )
{
	
	scr->BusyFlg   = SCCODE_NOOPR;
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
	else														EXEC_LangInfo->BusyFlg = SCCODE_RUN;
}






static void EXEC_LangTWait( void )
{
	
	if( timeGetTime() >= EXEC_LangInfo->TWaitEnd )	EXEC_LangInfo->BusyFlg = SCCODE_RUN;
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
	int		param1;
	
	
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];
	
	
	EXEC_LangInfo->reg[ param1 ] = ( rand() % 65535 );

	
	EXEC_AddPC( 3 );
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
	EXEC_LangInfo->BusyFlg     = SCCODE_WAIT_WAIT;		
	
	
	EXEC_AddPC( 4 );
}






static void EXEC_OprTWait( void )
{
	int		param1;
	
	
	param1 = EXEC_GetShortValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ] );		
	
	
	EXEC_LangInfo->TWaitEnd   = timeGetTime()+(DWORD)param1;	
 	EXEC_LangInfo->BusyFlg    = SCCODE_WAIT_TWAIT;				

	
	EXEC_AddPC( 4 );
}






static void EXEC_OprRun( void )
{
	ESC_SetDrawFlag();
	
	EXEC_AddPC( 2 );
}

int EXEC_SLoad( char *str )
{
	int index;
	
	EXEC_ReleaseLang( EXEC_LangInfo );
	
	
	index = EXEC_ReadLang( str, EXEC_LangInfo );

	EXEC_LangInfo->BusyFlg = SCCODE_WAIT_SLOAD;

	EXEC_StartLang( EXEC_LangInfo, 0 );

	return index;
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
	
	if( EXEC_LangInfo->BusyFlg == SCCODE_WAIT_TWAIT )		EXEC_LangTWait();
	if( EXEC_LangInfo->BusyFlg == SCCODE_WAIT_WAIT )		EXEC_LangWait();

	
	if( EXEC_LangInfo->BusyFlg == SCCODE_RUN ){			
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
			i+=GetScriptParamControlEx( i, opr, buf );
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
