#include "Clcommon.h"
#include "backLog.h"
#include "graphic.h"
#include "msgWnd.h"
#include "select.h"
#include "soundDS.h"

#include "saveWnd.h"
#include <io.h>
#include <sys/stat.h>
#include <time.h>

#define SaveVerMinor	0x01
#define SaveVerMajor	0x00

SaveInfo			saveInf;
SaveWnd		*saveWnd = NULL;
SAVE_STRUCT		SaveStruct;
SAVE_STRUCT		backSelectStruct;
BackSelectInf	backSelectInf;

void SAV_SaveScript(void)
{
	memcpy( SaveStruct.NowLangFileName, NowLangFileName, 16 );
	SaveStruct.EXEC_LangInfo       = *EXEC_LangInfo;
	SaveStruct.RunLangBlockNo     = RunLangBlockNo;
	SaveStruct.PrevRunLangBlockNo = PrevRunLangBlockNo;
}

void SAV_Save( int save_no )
{
	char			fname[256];
	DWORD			i,rsize;
	MySaveHead		saveHead;

	SAV_SaveScript();
	memcpy( SaveStruct.ESC_FlagBuf, ESC_FlagBuf, sizeof(int)*ESC_FLAG_MAX );

	if(save_no>=0)	{ wsprintf( fname, "save_%02d.sav", save_no );	}
	else			{
		wsprintf( fname, "save___.sav" );
	}
	char	*selectMsg = NULL;
	selectWnd.selectBackup(&selectMsg);
	for(i=0;i<STCHAR_MAX;i++){
		CopyMemory(&saveInf.charSave[i],&charInf[i].save,sizeof(CharSave));
	}
	if(save_no==98){
		saveInf.bBackSelect = on;
		CopyMemory(&backSelectStruct,&SaveStruct,sizeof(SaveStruct));
		CopyMemory(&backSelectInf.saveInf,&saveInf,sizeof(SaveInfo));

		CopyMemory(backSelectInf.selectMsg,selectMsg,60*saveInf.selectNum);
		strcpy(backSelectInf.msgBuf,backLog.GetBottomLogBuffer());
		backSelectInf.msgLine = msgWnd.logMsgLine;
		return;
	}

	strncpy(saveHead.head,"LSAV",4);
	saveHead.ver = MAKEWORD(SaveVerMajor,SaveVerMinor);
	HANDLE hFile = CreateFile(fname,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,NULL);
	WriteFile(hFile,saveWnd->saveBmp.pBuf,80*60*3,&rsize,NULL);
	WriteFile(hFile,&saveHead,sizeof(MySaveHead),&rsize,NULL);
	WriteFile(hFile,&SaveStruct, sizeof(SaveStruct), &rsize,NULL);
	WriteFile(hFile,&saveInf,sizeof(SaveInfo),&rsize,NULL);
	WriteFile(hFile,&backSelectStruct, sizeof(SaveStruct), &rsize,NULL);
	WriteFile(hFile,&backSelectInf,sizeof(BackSelectInf),&rsize,NULL);
	cl_free(selectMsg);
	CloseHandle(hFile);
} // SAV_Save

void SAV_LoadScript(void)
{
	int		digit=0;

	ESC_InitEOprFlag( );

	memcpy( NowLangFileName, SaveStruct.NowLangFileName, 16 );
	*EXEC_LangInfo      = SaveStruct.EXEC_LangInfo;
	EXEC_LangInfo->LangBuf=NULL;
	RunLangBlockNo     = SaveStruct.RunLangBlockNo;
	PrevRunLangBlockNo = SaveStruct.PrevRunLangBlockNo;

	EXEC_ReadLang( NowLangFileName, EXEC_LangInfo );
	EXEC_LangInfo->BusyFlg  = SaveStruct.EXEC_LangInfo.BusyFlg;

} // SAV_LoadScript

BOOL SAV_Load( int load_no )
{
	char		fname[256];
	DWORD		rsize;
	int			i;
	char		*msgBuf;
	MySaveHead	saveHead;

	if(load_no>=0) { wsprintf( fname, "save_%02d.sav", load_no ); }
	else		   { wsprintf( fname, "save___.sav" );			  }
	exec_mode back_mode = sysInf.execMode;
	selectWnd.selectEnd();
	msgWnd.MsgCLS();
	for(i=0;i<STCHAR_MAX;i++){
		charInf[i].Release();
	}
	for(i=0;i<hEvtSndMax;i++) StopEffect(evtHandle[i]);
	if(load_no!=98){
		HANDLE hFile = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0L ,NULL);
		if(INVALID_HANDLE_VALUE==hFile){
			changeExecMode(back_mode);
			return FALSE;
		}
		ReadFile(hFile,saveWnd->saveBmp.pBuf,80*60*3,&rsize,NULL);
		ReadFile(hFile,&saveHead,sizeof(MySaveHead),&rsize,NULL);

		if(saveHead.ver!=MAKEWORD(SaveVerMajor,SaveVerMinor) || 0!=strnicmp(saveHead.head,"LSAV",4)){
			MessageBox(sysInf.hWnd,"セーブデータ形式が異常です。このセーブデータは使用できません","データエラー",MB_OK|MB_ICONWARNING);
			changeExecMode(back_mode);
			return FALSE;
		}
		ReadFile(hFile,&SaveStruct, sizeof(SaveStruct), &rsize,NULL);
		ReadFile(hFile,&saveInf,sizeof(SaveInfo),&rsize,NULL);
		ReadFile(hFile,&backSelectStruct, sizeof(SaveStruct), &rsize,NULL);
		ReadFile(hFile,&backSelectInf,sizeof(BackSelectInf),&rsize,NULL);
		CloseHandle(hFile);
	}else{
		CopyMemory(&SaveStruct,&backSelectStruct,sizeof(SaveStruct));
		CopyMemory(&saveInf,&backSelectInf.saveInf,sizeof(SaveInfo));
	}
	SAV_LoadScript();
	memcpy( ESC_FlagBuf, SaveStruct.ESC_FlagBuf, sizeof(int)*ESC_FLAG_MAX ); 
	toneConvert.ChangeTable( saveInf.tone_type[0], saveInf.tone_type[1], saveInf.tone_backOnly);
	g_disp(saveInf.bak_no, _BAK_DIRECT, saveInf.cg_type, 0, saveInf.btRect.left, saveInf.btRect.top);
	for(i=0;i<STCHAR_MAX;i++){
		if(saveInf.charSave[i].char_no){
			c_disp(saveInf.charSave[i].char_no, saveInf.charSave[i].pose, saveInf.charSave[i].locate, _CHR_DIRECT, saveInf.charSave[i].layer,0);
		}
	}
	if(saveInf.selectNum){
		for(i=0;i<saveInf.selectNum;i++){
			selectWnd.addSelect(&backSelectInf.selectMsg[i*60]);
		}
		msgWnd.DisplayMessage((BYTE *)backSelectInf.msgBuf);
		backLog.SetBackLog(backSelectInf.msgBuf,backSelectInf.msgLine);
		sysInf.bMsgCLS = on;
	}
	if(saveInf.musicNum>=0){
		lpSoundDS->ChangeMusic(bgmHandle, pack_bgmfile, saveInf.musicNum,TRUE,0);
	}else{
		lpSoundDS->FadeOut( bgmHandle );
	}
	sysInf.FontSize = mainFontSize;
	changeExecMode(event_exec_mode);
	return TRUE;
} // SAV_Load

#define SWndLEFT	40
#define SWndTOP		60

SaveWnd::SaveWnd(exec_mode s_mode)
{
	int		i;
	ZeroMemory(this,sizeof(SaveWnd));
	mode = s_mode;
	bak_mode = sysInf.execMode;
	baseWnd.loadLGF(pack_gparts,"saveWnd");
	panel.loadLGF(pack_gparts,"saveParts");
	cursor.loadLGF(pack_gparts,"saveSelect");
	button.loadLGF(pack_gparts,"saveBtn");
	numParts.loadLGF(pack_gparts,"saveNumber");
	numPartsSm.loadLGF(pack_gparts,"saveNumberSm");
	typeParts.loadLGF(pack_gparts,"saveLoad");
	wallWnd.loadLGF(pack_gparts,"saveWall");
	backWnd.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	infoWnd.createColorBuf(348*2+6,76*5+6*4,32,FALSE);
	saveBmp.createColorBuf(80,60,24,FALSE);
	backWnd.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
	saveBmp.Blt(NULL,&g_DibInf.colorBuf,NULL,FALSE);
	RECT	ptRect[3];
	rect.left = SWndLEFT +410;	rect.right = rect.left +100;
	rect.top = SWndTOP +9;		rect.bottom = rect.top +20;
	for(i=0;i<3;i++){
		ptRect[i].left = 0;		ptRect[i].right = ptRect[i].left +100;
		ptRect[i].top = i*20;	ptRect[i].bottom = ptRect[i].top +20;
	}
	btn[0].Create(1,&button,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = SWndLEFT +611;	rect.right = rect.left +100;
	rect.top = SWndTOP +9;		rect.bottom = rect.top +20;
	for(i=0;i<3;i++){
		ptRect[i].left = 100;	ptRect[i].right = ptRect[i].left +100;
		ptRect[i].top = i*20;	ptRect[i].bottom = ptRect[i].top +20;
	}
	btn[1].Create(1,&button,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = SWndLEFT +611;	rect.right = rect.left +100;
	rect.top = SWndTOP +457;	rect.bottom = rect.top +20;
	for(i=0;i<3;i++){
		ptRect[i].left = 200;		ptRect[i].right = ptRect[i].left +100;
		ptRect[i].top = i*20;	ptRect[i].bottom = ptRect[i].top +20;
	}
	btn[2].Create(1,&button,&rect,ptRect,&g_DibInf.colorBuf);
	time_t		tmp_time = 0;
	for(i=0;i<50;i++){
		char	fname[32];
		struct _stat	fstatus;
		wsprintf( fname, "save_%02d.sav", i+1 );
		if(0==_access(fname,0)){
			useTable[i] = TRUE;
			_stat(fname,&fstatus);
			file_time[i] = fstatus.st_mtime;
			if(tmp_time < file_time[i]){
				tmp_time = file_time[i];
				new_time = i+1;
			}
		}
	}
	if(new_time) offsetNum = ((new_time-1) /10)*10;
	CreateInfo();
	selectNum = 0xff;
	changeExecMode(mode);
	startTime = timeGetTime() +500;
} // SaveWnd::SaveWnd

void SaveWnd::timeout(int x,int y,int type,char *tbuf,int num)
{
	int		i=0;
	rect.top = 14*type; rect.bottom = rect.top +14;
	while(tbuf[i]){
		if(tbuf[i]>='0' && tbuf[i]<='9'){
			rect.left = (tbuf[i]-'0')*10;
			rect.right = rect.left +10;
		}else if(tbuf[i]==':'){
			rect.left = 100; rect.right = 110;
		}else{
			i++;
			continue;
		}
		infoWnd.BltFast(170 +x +i*10,50 +y,&numParts,&rect,FALSE);
		i++;
	}
} // SaveWnd::timeout

void saveMessage(char *msgBuf)
{
	if(*msgBuf=='\\')return;

	if(*msgBuf=='<'){
		while(*msgBuf!='>')msgBuf++;
		msgBuf++;
	}
	if(strlen(msgBuf) <= 30){
		strcpy(SaveStruct.save_head.message,msgBuf);
	}else{
		strncpy(SaveStruct.save_head.message,msgBuf,30);
		SaveStruct.save_head.message[30] = '\0';
	}
}

void SaveWnd::TextOutSaveMsg(int x, int y, char *msg)
{
	int			i,len;

	len = strlen(msg);
	x+=116;
	y+=18;
	for(i=0;i<len;i+=2){
		if(msg[i]=='<'){
			while(msg[i]!='>')i++;
			i++;
		}
		if(msg[i]=='\\')break;
		OutputOutlineFont(&infoWnd,(BYTE *)msg +i,x,y,systemFontSize);
		x+=systemFontSize;
	}
} // SaveWnd::TextOutSaveMsg

void SaveWnd::CreateInfo()
{
	int				i,type;
	char			fname[32], timework[32];
	ColorBuf		tmpCBuf;
	SAVE_STRUCT		tmpSaveStruct;

	tmpCBuf.createColorBuf(80,60,24,FALSE);
	infoWnd.clearColorBuf();

	for(i=0;i<10;i++){
		if(useTable[i+offsetNum]){
			DWORD rsize;
			wsprintf( fname, "save_%02d.sav", i+offsetNum+1 );
			HANDLE hFile = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0L ,NULL);
			ReadFile(hFile,tmpCBuf.pBuf,80*60*3,&rsize,NULL);
			SetFilePointer(hFile,sizeof(MySaveHead),NULL,FILE_CURRENT);
			ReadFile(hFile,&tmpSaveStruct, sizeof(SAVE_STRUCT), &rsize,NULL);
			CloseHandle(hFile);
			infoWnd.BltFast(28  +354*(i/5), 8 +82*(i%5), &tmpCBuf,NULL,FALSE);

			struct tm *newtime = localtime(&file_time[i+offsetNum]);
			newtime->tm_year += 1900;
			newtime->tm_mon += 1;
			wsprintf(timework,"%4d %02d%02d %02d:%02d", newtime->tm_year, newtime->tm_mon, newtime->tm_mday, newtime->tm_hour, newtime->tm_min);
			type = 1;
			if((i+offsetNum+1)==new_time){
				rect.left = 0; rect.right = 48;
				rect.top = 72; rect.bottom = 98;
				infoWnd.BltFast(120 +354*(i/5), 44 +82*(i%5), &typeParts,&rect,FALSE);
				type = 2;
			}
			timeout(354*(i/5),82*(i%5), type, timework,i+offsetNum);
			TextOutSaveMsg(354*(i/5),82*(i%5),tmpSaveStruct.save_head.message);
		}
		rect.top = 0; rect.bottom = 6;
		if((i+offsetNum+1) >= 10){
			rect.left = 5*((i+offsetNum+1)/10); rect.right = rect.left +5;
			infoWnd.BltFast(354*(i/5)+10,82*(i%5)+39,&numPartsSm,&rect,FALSE);
		}
		rect.left = 5*((i+offsetNum+1)%10); rect.right = rect.left +5;
		infoWnd.BltFast(354*(i/5)+15,82*(i%5)+39,&numPartsSm,&rect,FALSE);
	}
	rect.top = 0; rect.bottom = rect.top +14;
	rect.left = ((offsetNum/10)+1) *10; rect.right = rect.left +10;
	baseWnd.BltFast(540 ,12,&numParts,&rect,FALSE);
	rect.left = 50; rect.right = rect.left +10;
	baseWnd.BltFast(572 ,12,&numParts,&rect,FALSE);

	tmpCBuf.Release();
} // SaveWnd::Create

BOOL SaveWnd::selectExec()
{
	int		i;
	BOOL	bSetCur = FALSE;
	if(state==0){
		if(timeGetTime() >= startTime){
			alpha = 1.0f;
			state = 1;
		}else{
			alpha = 1.0f - (startTime -timeGetTime()) /500.0f;
		}
		SetAlpha();
		return TRUE;
	}else if(state==2){
		if(timeGetTime() >= startTime){
			alpha = 0;
			changeExecMode(bak_mode);
			return FALSE;
		}
		alpha = (startTime -timeGetTime()) /500.0f;
		SetAlpha();
		return TRUE;
	}
	for(i=0;i<3;i++){
		if(btn[i].CheckState(&sysInf.msPoint)){
			switch(i){
			  case 0:
				if(0==offsetNum)offsetNum = 40;
				else offsetNum -= 10;
				CreateInfo();
				return TRUE;
			  case 1:
				offsetNum += 10;
				if(offsetNum>40)offsetNum = 0;
				CreateInfo();
				return TRUE;
			  default:
				startTime = timeGetTime() +500;
				state = 2;
				return TRUE;
			}
		}
	}
	if(keyState.push_action){
		if(selectNum<10){
			if(mode==save_mode || useTable[selectNum+offsetNum]){
				if(mode==save_mode){
					new_time = selectNum+offsetNum+1;
					SAV_Save( new_time );
					useTable[selectNum+offsetNum] = TRUE;

					struct _stat	fstatus;
					char fname[32];
					wsprintf( fname, "save_%02d.sav", new_time );
					_stat(fname,&fstatus);
					file_time[selectNum+offsetNum] = fstatus.st_mtime;

					CreateInfo();
				}else{
					if(SAV_Load( selectNum+offsetNum+1 )) return FALSE;
					startTime = timeGetTime() +500;
					state = 2;
				}
				return TRUE;
			}
		}
	}else if(keyState.push_cancel){
		startTime = timeGetTime() +500;
		state = 2;
		return TRUE;
	}else if(keyState.push_up){
		if(selectNum==0xff){
			selectNum = 0;
		}else if(selectNum==0){
			selectNum = 9;
		}else{
			selectNum --;
		}
		bSetCur = TRUE;
	}else if(keyState.push_down){
		if(selectNum>=9){
			selectNum = 0;
		}else{
			selectNum ++;
		}
		bSetCur = TRUE;
	}else if(keyState.push_left || keyState.push_right){
		if(selectNum==0xff){
			selectNum = 0;
		}else if(selectNum < 5){
			selectNum += 5;
		}else{
			selectNum -= 5;
		}
		bSetCur = TRUE;
	}
	if(bSetCur){
		point.x = SWndLEFT+70 +354*(selectNum/5);
		point.y = SWndTOP+90 +82*(selectNum%5);
		SetWindowCursorPos(sysInf.hWnd, point.x, point.y);
	}else{
		selectNum = 0xff;
		for(i=0;i<10;i++){
			rect.left = SWndLEFT+9 +354*(i/5);
			rect.right = rect.left +348;
			rect.top = SWndTOP+41 +82*(i%5);
			rect.bottom = rect.top +76;
			if(PtInRect(&rect, sysInf.msPoint)){
				selectNum = i;
				break;
			}
		}
	}
	return TRUE;
} // SaveWnd::selectExec

void SaveWnd::Blt()
{
	int		i;

	g_DibInf.colorBuf.BltFast(0,0,&backWnd,NULL,FALSE);
	g_DibInf.colorBuf.BltFast(SWndLEFT,SWndTOP,&baseWnd,NULL,TRUE);
	for(i=0;i<10;i++){
		rect.top = rect.left = 0; rect.right = rect.left +348;
		if(useTable[i+offsetNum])rect.top += 76;
		rect.bottom = rect.top +76;
		g_DibInf.colorBuf.BltFast(SWndLEFT+9 +354*(i/5),SWndTOP+41 +82*(i%5),&panel,&rect,TRUE);
	}
	g_DibInf.colorBuf.BltFast(SWndLEFT+9,SWndTOP+41,&infoWnd,NULL,TRUE);
	if(selectNum < 0xff){
		int num = selectNum % 10;
		g_DibInf.colorBuf.BltFast(SWndLEFT+6 +354*(num/5),SWndTOP+38 +82*(num%5),&cursor,NULL,TRUE);
	}
	rect.top = rect.left = 0; rect.right = 122;
	if(save_mode==mode)rect.top = 36;
	rect.bottom = rect.top +36;
	g_DibInf.colorBuf.BltFast(SWndLEFT+30,SWndTOP,&typeParts,&rect,TRUE);
	for(i=0;i<3;i++){
		btn[i].Draw();
	}
} // SaveWnd::Blt
