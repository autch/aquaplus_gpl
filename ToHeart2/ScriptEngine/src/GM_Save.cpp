
#include "mm_std.h"
#include "main.h"


#include "Escript.h"
#include "GM_Avg.h"
#include "GM_Save.h"

#include "disp.h"



typedef struct{
	int			ver_no;		
	int			sin_no;		
	SYSTEMTIME	sys_time;	

	char		message[32];
	RGB24		thumbnail[80*60];
}SAVE_HEADER;

typedef struct{
	
	SAVE_HEADER		save_head;

	
	EXEC_DATA 		EXEC_ScriptInfo;					
	char			NowScriptFileName[64];			
	int				RunScriptBlockNo;
	int				PrevRunScriptBlockNo;

	
	int				ESC_FlagBuf[ ESC_FLAG_MAX ];
	AVG_SAVE_DATA	sdata;

}SAVE_STRUCT;
SAVE_STRUCT		SaveStruct;

void SAV_CreateSaveHead(void)
{
	int	_times = ESC_GetFlag( _TIME );
	int	_days  = ESC_GetFlag( _DAY  );
	int	_month = ESC_GetFlag( _MONTH );

	SaveStruct.save_head.ver_no = 1;		
	SaveStruct.save_head.sin_no = (_month<<16) | (_days<<8) | _times;	
	GetLocalTime( &SaveStruct.save_head.sys_time );

	DSP_SetGraph( GRP_WORK, BMP_CAP, 0, ON, CHK_HOKAN );
	DSP_SetGraphPosZoom( GRP_WORK, 0, 0, 80, 60, 0, 0, 800, 600 );
	DSP_CreateBmp( BMP_WORK, BMP_FULL, 80, 60 );
	DSP_SetGraphTarget( GRP_WORK, BMP_WORK, MainWindow.draw_mode2 );
	BMP_F	*bmp_f = (BMP_F *)DSP_GetBmp( BMP_WORK );

	CopyMemory( SaveStruct.save_head.thumbnail, bmp_f->buf, 80*60*sizeof(RGB24) );
	DSP_ResetGraph( GRP_WORK );
	DSP_ReleaseBmp( BMP_WORK );

	
	char	*buf = SaveStruct.save_head.message;
	char	buf2[1024];
	int		k=0;
	int		j=0;
	
	ZeroMemory( &SaveStruct.save_head.message, 32 );
	ZeroMemory( buf2, 1024 );
	DSP_GetTextDispStr( TXT_WINDOW, buf2 );
	if(MapStep){
		strcpy( buf, "　ＭＡＰ選択" );
	}else{
		strncpy( buf, buf2, 18 );
	}
	while( buf[k] && buf[k]!='\n' ){
		if( (0x00<=buf[k] && buf[k]<0x80) || (0xa0<=buf[k] && buf[k]<0xe0) ) {
			if(k>=28) { j=1; break; }
			else	  { k++; }
		}else{
			if(k>=27) { j=1; break; }
			else	  { k+=2; }
		}
	}
	if(j){
		buf[k  ]='･';
		buf[k+1]='･';
		buf[k+2]='\0';
	}else{
		buf[k]='\0';
	}
	
}

BOOL SAV_LoadHeadTime( int load_no, SYSTEMTIME *sys_time )
{
	char	fname[256];

	wsprintf( fname, "save_%02d.sav", load_no );
	if( !STD_ReadFile( fname, (char*)&SaveStruct.save_head, sizeof(SYSTEMTIME)+8 ) ){
		return FALSE;
	}else{
		*sys_time = SaveStruct.save_head.sys_time;
		return TRUE;
	}
}

BOOL SAV_LoadHead( int load_no, char *mes, SYSTEMTIME *sys_time, BMP_SET *bmp_set, int *game_time )
{
	char	fname[256];

	if(bmp_set){
		ZeroMemory( bmp_set, sizeof(BMP_SET) );
		bmp_set->flag=1;
		bmp_set->bmp_bit=BMP_FULL;
		bmp_set->alp_flag=0;

		bmp_set->pos.x=0;
		bmp_set->pos.y=0;
		bmp_set->size.x=80;
		bmp_set->size.y=60;
	}
	wsprintf( fname, "save_%02d.sav", load_no );
	if( !STD_ReadFile( fname, (char*)&SaveStruct.save_head, sizeof(SaveStruct.save_head)) ){
		return FALSE;
	}else{
		if(mes){
			wsprintf( mes, "%s", SaveStruct.save_head.message );
		}

		if(bmp_set){
			bmp_set->bmp_f.buf = SaveStruct.save_head.thumbnail;
			bmp_set->bmp_f.sx=80;
			bmp_set->bmp_f.sy=60;
		}
		
		if(sys_time) *sys_time = SaveStruct.save_head.sys_time;

		if(game_time) *game_time = SaveStruct.save_head.sin_no;
		return TRUE;
	}
}

void SAV_SaveScript(void)
{
	CopyMemory( SaveStruct.NowScriptFileName, NowLangFileName, 64 );
	SaveStruct.EXEC_ScriptInfo       = *EXEC_LangInfo;
	SaveStruct.RunScriptBlockNo     = RunLangBlockNo;
	SaveStruct.PrevRunScriptBlockNo = PrevRunLangBlockNo;
}

void SAV_LoadScript(void)
{

	int		digit=0;

	ESC_InitEOprFlag( );

	CopyMemory( NowLangFileName, SaveStruct.NowScriptFileName, 64 );
	*EXEC_LangInfo      = SaveStruct.EXEC_ScriptInfo;
	EXEC_LangInfo->LangBuf=NULL;
	RunLangBlockNo     = SaveStruct.RunScriptBlockNo;
	PrevRunLangBlockNo = SaveStruct.PrevRunScriptBlockNo;

	EXEC_ReadLang( NowLangFileName, EXEC_LangInfo );
	EXEC_LangInfo->BusyFlg  = SaveStruct.EXEC_ScriptInfo.BusyFlg;

	AVG_SetScenarioNo( GetScenarioNo(NowLangFileName) );
}

void SAV_ErrSave( char *buf )
{
	char	fname[256];

	SAV_CreateSaveHead();
	SAV_SaveScript();
	CopyMemory( SaveStruct.ESC_FlagBuf, ESC_FlagBuf, sizeof(int)*ESC_FLAG_MAX );
	AVG_SetSaveData( &SaveStruct.sdata );

	wsprintf( fname, "err\\%s.sav", buf );
	if( !STD_WriteFile( fname, (char*)&SaveStruct, sizeof(SaveStruct)) ){
		DebugPrintf( "ファイルが開けないにょ[%s]", fname );
	}
}

void SAV_Save( int save_no )
{
	char	fname[256];

	SAV_CreateSaveHead();
	SAV_SaveScript();
	CopyMemory( SaveStruct.ESC_FlagBuf, ESC_FlagBuf, sizeof(int)*ESC_FLAG_MAX );
	AVG_SetSaveData( &SaveStruct.sdata );

	if(save_no>=0)	{ wsprintf( fname, "save_%02d.sav", save_no );	}
	else			{
		wsprintf( fname, "save___.sav" );
	}
	if( !STD_WriteFile( fname, (char*)&SaveStruct, sizeof(SaveStruct)) ){
		DebugBox( NULL, "ファイルが開けないにょ[%s]", fname );
	}
}

void SAV_Load( int load_no )
{
	char	fname[256];

	if(load_no>=0) { wsprintf( fname, "save_%02d.sav", load_no ); }
	else		   { wsprintf( fname, "save___.sav" );			  }
	if( !STD_ReadFile( fname, (char*)&SaveStruct, sizeof(SaveStruct)) ){
		DebugBox( NULL, "ファイルが開けないにょ[%s]", fname );
	}

	SAV_LoadScript();
	CopyMemory( ESC_FlagBuf, SaveStruct.ESC_FlagBuf, sizeof(int)*ESC_FLAG_MAX ); 

	DefaultCharName = ESC_GetFlag( _DEFAULT_NAME );

			AVG_SetLoadData( SaveStruct.sdata );
	AVG_LoadWindow();
}


