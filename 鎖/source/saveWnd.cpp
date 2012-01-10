#include "AVGYMZ.h"
#include "backLog.h"
#include "graphic.h"
#include "msgWnd.h"
#include "select.h"
#include "soundDS.h"

#include "saveWnd.h"
#include <io.h>
#include <sys/stat.h>
#include <time.h>

#define SaveVerMinor	0x0a
#define SaveVerMajor	0x00

#define thumbnailW		120
#define thumbnailH		90

SaveInfo			saveInf;		

SaveWnd			*saveWnd = NULL;
SAVE_STRUCT		SaveStruct;
SAVE_STRUCT		backSelectStruct;
BackSelectInf	backSelectInf;	
void AVG_SetScenarioNo( int scenario_no );


void InitRestartData(void)
{
	int			i;

	my_delete(lpMovie);
	lpSoundDS->FadeOut(voiceHandle);	
	selectWnd.selectEnd();
	msgWnd.MsgCLS();
	backColorBuf.clearColorBuf();
	bgInf.colorBuf.clearColorBuf();
	toneConvert.ClearTable();
	for(i=0;i<STCHAR_MAX;i++){
		charInf[i].Release();
	}
	for(i=0;i<STBMP_MAX;i++){
		setBmpInf[i].Release();
	}
	for(i=0;i<hEvtSndMax;i++) StopEffect(evtHandle[i]);
	cl_free(LangData.LangBuf);
	saveInf.Reset();
	SaveStruct.Reset();
	backLog.ClearLog();
}

void SAV_SaveScript(void)
{
	memcpy( SaveStruct.NowLangFileName, NowLangFileName, 16 );
	SaveStruct.EXEC_LangInfo       = *EXEC_LangInfo;
	SaveStruct.RunLangBlockNo     = RunLangBlockNo;
	SaveStruct.PrevRunLangBlockNo = PrevRunLangBlockNo;
} // SAV_SaveScript

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
	saveInf.voiceCnt = lpSoundDS->voiceCnt;
	saveInf.voiceScriptNo = lpSoundDS->scriptNo;
	saveInf.musicNum = lpSoundDS->GetMusicFileNo();
	saveInf.musicVol = lpSoundDS->GetMusicVolume();
	for(i=0;i<hEvtSndMax;i++){
		if(PCM_PLAY!=lpSoundDS->GetStatus(evtHandle[i])){
			saveInf.loopSeNum[i] = 0;
			continue;
		}
		if(0!=lpSoundDS->GetRepeatNum(evtHandle[i])){
			saveInf.loopSeNum[i] = 0;
			continue;
		}
		int seNum = lpSoundDS->GetMusicNo(evtHandle[i]);
		if(-1==seNum){
			saveInf.loopSeNum[i] = 0;
			continue;
		}
		saveInf.loopSeNum[i] = seNum +1;
		saveInf.loopSeVol[i] = lpSoundDS->GetVolume(evtHandle[i]);
	}
	if(save_no==98){
		saveInf.bBackSelect = on;
		CopyMemory(&backSelectStruct,&SaveStruct,sizeof(SaveStruct));
		CopyMemory(&backSelectInf.saveInf,&saveInf,sizeof(SaveInfo));
		CopyMemory(&bak_setBmpInf,&setBmpInf,sizeof(SetBmpInf)*STBMP_MAX);

		CopyMemory(backSelectInf.selectMsg,selectMsg,60*saveInf.selectNum);
		strcpy(backSelectInf.msgBuf,backLog.GetBottomLogBuffer());
		backSelectInf.msgLine = msgWnd.logMsgLine;
		cl_free(selectMsg);
		return;
	}
	strncpy(saveHead.head,"LSAV",4);
	saveHead.ver = MAKEWORD(SaveVerMajor,SaveVerMinor);
	HANDLE hFile = CreateFile(fname,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,NULL);
	WriteFile(hFile,saveWnd->saveBmp.pBuf,thumbnailW*thumbnailH*3,&rsize,NULL);
	WriteFile(hFile,&saveHead,sizeof(MySaveHead),&rsize,NULL);
	WriteFile(hFile,&SaveStruct, sizeof(SaveStruct), &rsize,NULL);
	WriteFile(hFile,&saveInf,sizeof(SaveInfo),&rsize,NULL);
	WriteFile(hFile,setBmpInf,sizeof(SetBmpInf)*STBMP_MAX,&rsize,NULL);
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

	int index = EXEC_ReadLang( NowLangFileName, EXEC_LangInfo );
	EXEC_LangInfo->BusyFlg  = SaveStruct.EXEC_LangInfo.BusyFlg;
	AVG_SetScenarioNo( index );
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
	exec_mode tmp_back_mode = sysInf.execMode;
	InitRestartData();
	if(sysInf.execMode!=tmp_back_mode)changeExecMode(tmp_back_mode);
	if(load_no!=98){
		HANDLE hFile = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0L ,NULL);
		if(INVALID_HANDLE_VALUE==hFile){
			return FALSE;
		}
		ReadFile(hFile,saveWnd->saveBmp.pBuf,thumbnailW*thumbnailH*3,&rsize,NULL);
		ReadFile(hFile,&saveHead,sizeof(MySaveHead),&rsize,NULL);

		if(saveHead.ver!=MAKEWORD(SaveVerMajor,SaveVerMinor) || 0!=strnicmp(saveHead.head,"LSAV",4)){
			MessageBox(sysInf.hWnd,"セーブデータ形式が異常です。このセーブデータは使用できません","データエラー",MB_OK|MB_ICONWARNING);
			return FALSE;
		}
		ReadFile(hFile,&SaveStruct, sizeof(SaveStruct), &rsize,NULL);
		ReadFile(hFile,&saveInf,sizeof(SaveInfo),&rsize,NULL);
		ReadFile(hFile,setBmpInf,sizeof(SetBmpInf)*STBMP_MAX,&rsize,NULL);
		ReadFile(hFile,&backSelectStruct, sizeof(SaveStruct), &rsize,NULL);
		ReadFile(hFile,&backSelectInf,sizeof(BackSelectInf),&rsize,NULL);
		CloseHandle(hFile);
	}else{
		CopyMemory(&SaveStruct,&backSelectStruct,sizeof(SaveStruct));
		CopyMemory(&saveInf,&backSelectInf.saveInf,sizeof(SaveInfo));
		CopyMemory(&setBmpInf,&bak_setBmpInf,sizeof(SetBmpInf)*STBMP_MAX);
	}
	SAV_LoadScript();
	memcpy( ESC_FlagBuf, SaveStruct.ESC_FlagBuf, sizeof(int)*ESC_FLAG_MAX ); 
	toneConvert.ChangeTable( saveInf.tone_type[0], saveInf.tone_type[1], saveInf.tone_backOnly);
	SHORT	tmp_shakeSize = saveInf.shakeSize;
	g_disp(saveInf.bak_no, _BAK_DIRECT, saveInf.cg_type, 0, saveInf.btRect.left, saveInf.btRect.top);
	saveInf.shakeSize = tmp_shakeSize; 
	if(saveInf.shakeSize){
		backColorBuf.RollBlt(&bgInf.colorBuf, saveInf.shakeSize*180.0f);
		bgInf.colorBuf.BltFast(0,0,&backColorBuf,NULL,FALSE);
	}
	for(i=0;i<STCHAR_MAX;i++){
		if(saveInf.charSave[i].char_no){
			c_disp(saveInf.charSave[i].char_no-1, saveInf.charSave[i].pose, saveInf.charSave[i].locate, _CHR_DIRECT, saveInf.charSave[i].layer,saveInf.charSave[i].alpha,0,saveInf.charSave[i].bright);
		}
	}
	for(i=0;i<STBMP_MAX;i++){
		setBmpInf[i].Reload();
	}
	if(saveInf.selectNum){
		for(i=0;i<saveInf.selectNum;i++){
			selectWnd.addSelect(&backSelectInf.selectMsg[i*60]);
		}
		sysInf.bMsgAtOnce = on;
		msgWnd.DisplayMessage((BYTE *)backSelectInf.msgBuf);
		backLog.SetBackLog(backSelectInf.msgBuf,backSelectInf.msgLine);
		sysInf.bMsgCLS = on;
	}
	if(saveInf.musicNum>=0){
		lpSoundDS->ChangeMusic(bgmHandle, saveInf.musicNum,TRUE,0, 30,saveInf.musicVol);
	}else{
		lpSoundDS->FadeOut( bgmHandle );
	}
	for(i=0;i<hEvtSndMax;i++){
		if(saveInf.loopSeNum[i]==0)continue;
		CreateEffect(evtHandle[i],saveInf.loopSeNum[i]-1);
		PlayEffect(evtHandle[i],saveInf.loopSeVol[i],TRUE);
	}
	if(backSelectInf.saveInf.selectNum){
		EnableMenuItem( sysInf.hMenu, ID_BACKSELECT, MF_ENABLED );
	}else{
		EnableMenuItem( sysInf.hMenu, ID_BACKSELECT, MF_GRAYED );
	}
	sysInf.FontSize = mainFontSize;	
	lpSoundDS->voiceCnt = saveInf.voiceCnt;
	lpSoundDS->scriptNo = saveInf.voiceScriptNo;
	BOOL bPlayVoice = FALSE;
	if(saveInf.RestartVoice[0]>0){
		char	voicefile[32];
		saveInf.RestartVoice[0] --;
		int charNo = checkPlayVoice(saveInf.RestartVoice[0]);
		if(charNo >= 0){
			wsprintf(voicefile,"%05d_%03d_%03d",saveInf.RestartVoice[2], saveInf.RestartVoice[1], charNo);
			if(!keyState.data_skip && !sysInf.bReadSkip){
				bPlayVoice = TRUE;
			}
		}else{
			wsprintf(voicefile,"%05d_%03d_%03d",saveInf.RestartVoice[2], saveInf.RestartVoice[1], saveInf.RestartVoice[0]);
		}
		backLog.SetVoice(voicefile);
		saveInf.RestartVoice[0] = 0;

		if(saveWnd){
			strcpy(saveWnd->voicefile,voicefile);
		}
	}
	if(FALSE==bPlayVoice){
		if(saveWnd)saveWnd->voicefile[0] = '\0';
	}
	return TRUE;
} // SAV_Load

SaveWnd::SaveWnd(exec_mode s_mode)
{
	int			i,j;
	ColorBuf	wallWnd;

	ZeroMemory(this,sizeof(SaveWnd));
	mode = s_mode;
	EnableMenuItem( sysInf.hMenu,ID_SAVEEXEC,   MF_GRAYED  );
	EnableMenuItem( sysInf.hMenu,ID_LOADEXEC,   MF_GRAYED  );
	bak_mode = sysInf.execMode;
	panel[0].loadLGF(pack_gparts,"database00");
	panel[1].loadLGF(pack_gparts,"database01");
	panel[2].loadLGF(pack_gparts,"database02");
	button.loadLGF(pack_gparts,"SavePageBtn");
	numParts.loadLGF(pack_gparts,"saveFont");
	newParts.loadLGF(pack_gparts,"saveNew");
	if(mode==save_mode){
		pagebutton.loadLGF(pack_gparts,"SavePageNoBtn1");
		wallWnd.loadLGF(pack_gparts,"savebg");
	}else{
		pagebutton.loadLGF(pack_gparts,"SavePageNoBtn2");
		wallWnd.loadLGF(pack_gparts,"loadbg");
	}
	retBtnPrt.loadLGF(pack_gparts,"returnBtn");
	confirmWnd.loadLGF(pack_gparts,"saveConfirm");
	yesNoWnd.loadLGF(pack_gparts,"saveYesNo");
	backupWnd.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	backupWnd.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
	backWnd.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	infoWnd.createColorBuf(686,496,32,FALSE);
	saveBmp.createColorBuf(thumbnailW,thumbnailH,24,FALSE);
	backWnd.BltFast(0,0,&backColorBuf,NULL,FALSE);
	backWnd.BltFast(0,0,&wallWnd,NULL,TRUE);

	maskWnd.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,TRUE);
	maskWnd.alpha = 0.5f;
	for(i=0;i<5;i++){
		backWnd.BltFast(20,18+100*i,&panel[0],NULL,TRUE);
	}
	saveBmp.Blt(NULL,&g_DibInf.colorBuf,NULL,FALSE);
	RECT	selectRect = {709,565,709+86,565+25};
	RECT	btnRect[7];
	for(i=0;i<5;i++){
		btnRect[i].left = 0; btnRect[i].right = 86;
		btnRect[i].top = 25*i; btnRect[i].bottom = btnRect[i].top +25;
	}
	btn[2].Create(3,&retBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);
	selectRect.left = 22;	selectRect.right = selectRect.left +40;
	selectRect.top = 528;	selectRect.bottom = selectRect.top +40;
	for(i=0;i<5;i++){
		btnRect[i].left = 0;		btnRect[i].right = btnRect[i].left +40;
		btnRect[i].top = i*40;	btnRect[i].bottom = btnRect[i].top +40;
	}
	btn[0].Create(3,&button,&selectRect,btnRect,&g_DibInf.colorBuf);
	selectRect.left = 190;	selectRect.right = selectRect.left +40;
	selectRect.top = 528;	selectRect.bottom = selectRect.top +40;
	for(i=0;i<5;i++){
		btnRect[i].left = 40;	btnRect[i].right = btnRect[i].left +40;
		btnRect[i].top = i*40;	btnRect[i].bottom = btnRect[i].top +40;
	}
	btn[1].Create(3,&button,&selectRect,btnRect,&g_DibInf.colorBuf);
	selectRect.top = 528;	selectRect.bottom = selectRect.top +40;
	for(j=0;j<10;j++){
		selectRect.left = 286 +j*42;	selectRect.right = selectRect.left +40;
		for(i=0;i<7;i++){
			btnRect[i].left = j*40;		btnRect[i].right = btnRect[i].left +40;
			btnRect[i].top = i*40;	btnRect[i].bottom = btnRect[i].top +40;
		}
		pageBtn[j].Create(5,&pagebutton,&selectRect,btnRect,&g_DibInf.colorBuf);
	}
	selectRect.top = 18+128; selectRect.bottom = selectRect.top +25;
	for(j=0;j<2;j++){
		if(j==0){
			selectRect.left = 20+289; selectRect.right = selectRect.left +86;
		}else{
			selectRect.left = 20+385; selectRect.right = selectRect.left +86;
		}
		for(i=0;i<5;i++){
			btnRect[i].left = j*86;	btnRect[i].right = btnRect[i].left +86;
			btnRect[i].top = i*25;	btnRect[i].bottom = btnRect[i].top +25;
		}
		yesnoBtn[j].Create(3,&yesNoWnd,&selectRect,btnRect,&g_DibInf.colorBuf);
	}
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
	selectNum = btnSelect = 0xff;
	if(new_time){
		offsetNum = ((new_time-1) /5)*5;
		SetWindowCursorPos(sysInf.hWnd, 100, 18+45+ 100* (new_time-1-offsetNum));
		selectNum = (new_time-1-offsetNum);
	}
	CreateInfo();
	wallWnd.Release();
	changeExecMode(mode);
	startTime = timeGetTime() +500;
	state = save_fadein;
} // SaveWnd::SaveWnd

void SaveWnd::time_info(int x,int y,exec_mode mode,char *tbuf)
{
	int		i=0;
	rect.top = 0;
	if(load_mode==mode)rect.top = 24;
	rect.bottom = rect.top +24;
	while(tbuf[i]){
		if(tbuf[i]>='0' && tbuf[i]<='9'){
			rect.left = (tbuf[i]-'0')*18;
			rect.right = rect.left +18;
		}else if(tbuf[i]==':'){
			rect.left = 180; rect.right = 186;
		}else{
			x += 6;
			i++;
			continue;
		}
		infoWnd.BltFast(x,y,&numParts,&rect,FALSE);
		if(rect.left == 180){
			x += 6;
		}else{
			x += 18;
		}
		i++;
	}
} // SaveWnd::time_info

void saveMessage(BYTE *msgBuf)
{
	if(*msgBuf=='\\')return;
	if(*msgBuf=='<' && (msgBuf[1]!='R' && msgBuf[1]!='F')){
		while(*msgBuf!='>')msgBuf++;
		msgBuf++;
	}
	BOOL	bADD = TRUE;
	int		i;

	for(i=0;i<19*2;i++){
		if( msgBuf[i]=='\0'){
			SaveStruct.save_head.message[i] = '\0';
			bADD = FALSE;
			break;
		}
		if( msgBuf[i]>0x80){
			if( i > 18*2)
				break;
			SaveStruct.save_head.message[i] = (char)msgBuf[i];
			i++;
			SaveStruct.save_head.message[i] = (char)msgBuf[i];
		}else{
			SaveStruct.save_head.message[i] = (char)msgBuf[i];
		}
	}
	if(bADD)strcpy(&SaveStruct.save_head.message[i],"…");

} // saveMessage

void saveChapterTitle(char *msg)
{
	strcpy(SaveStruct.save_head.chapter, msg);
} //saveChapterTitle

void SetRoomNo(int mainRoom, int subRoom)
{
	SaveStruct.sdata.mainRoomNo = mainRoom;
	SaveStruct.sdata.subRoomNo = subRoom;
}

void SaveWnd::TextOutSaveMsg(int x, int y, char *msg)
{
	int			i,len;
	BOOL		bRubi = FALSE, bBig = FALSE;

	len = strlen(msg);
	for(i=0;i<len;i+=2){
RetryTextOut:
		if(msg[i]=='<'){
			if(msg[i+1]=='R'){
				bRubi = TRUE;
				i += 2;
			}else if(msg[i+1]=='F'){
				bBig = TRUE;
				i += 2;
				while(msg[i]>='0' && msg[i]<='9' && i<len)i++;
			}else{
				while(msg[i]!='>' && i<len)i++;
				i++;
			}
			if(i>=len)return;
		}
		if(bRubi && msg[i]=='|'){
			bRubi = FALSE;
			while(msg[i]!='>' && i<len)i++;
			i++;
			if(i>=len)return;
			goto RetryTextOut;
		}
		if(bBig && msg[i]=='>'){
			bBig = FALSE;
			i--;
			continue;
		}
		if(msg[i]=='\\')break;	
		BOOL bHankaku = OutputOutlineFont(&infoWnd,(BYTE *)msg +i,x,y,mainFontSize,FALSE);
		if(FALSE==bHankaku){
			x+=mainFontSize;
		}else{
			x+=mainFontSize/2;
			i--;
		}
	}
} // SaveWnd::TextOutSaveMsg

void SaveWnd::CreateInfo()
{
	int				i,type;
	char			fname[32], timework[32];
	ColorBuf		tmpCBuf;
	SAVE_STRUCT		tmpSaveStruct;
	RECT			maskRect;

	tmpCBuf.createColorBuf(thumbnailW,thumbnailH,24,FALSE);
	infoWnd.clearColorBuf();

	sysInf.backupTextColor();
	saveInf.textColor[0] = saveInf.textColor[1] = saveInf.textColor[2] = 0xff;
	newMark = 0;
	for(i=0;i<5;i++){
		if(useTable[i+offsetNum]){
			DWORD rsize;
			wsprintf( fname, "save_%02d.sav", i+offsetNum+1 );
			HANDLE hFile = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0L ,NULL);
			ReadFile(hFile,tmpCBuf.pBuf,thumbnailW*thumbnailH*3,&rsize,NULL);
			SetFilePointer(hFile,sizeof(MySaveHead),NULL,FILE_CURRENT);
			ReadFile(hFile,&tmpSaveStruct, sizeof(SAVE_STRUCT), &rsize,NULL);
			CloseHandle(hFile);
			infoWnd.BltFast(44, 3+100*i, &tmpCBuf,NULL,FALSE);

			struct tm *newtime = localtime(&file_time[i+offsetNum]);
			newtime->tm_year += 1900;
			newtime->tm_mon += 1;
			wsprintf(timework,"%4d %02d%02d %02d:%02d", newtime->tm_year, newtime->tm_mon, newtime->tm_mday, newtime->tm_hour, newtime->tm_min);
			type = 1;
			if((i+offsetNum+1)==new_time){
				newMark = i+1;
				type = 2;
			}
			time_info(438,57+100*i, mode, timework);
			TextOutSaveMsg(179,17+100*i,tmpSaveStruct.save_head.message);
			TextOutSaveMsg(179,57+100*i,tmpSaveStruct.save_head.chapter);
			maskRect.left = 0; maskRect.right = mainFontSize*20; maskRect.top = 0; maskRect.bottom = 24;
			maskWnd.alpha = 0.2f;
			infoWnd.BltFast(179,17+100*i,&maskWnd,&maskRect,TRUE);
			maskWnd.alpha = 0.5f;
		}
		rect.top = 66; rect.bottom = 76;
		if((i+offsetNum+1) >= 10){
			rect.left = 8*((i+offsetNum+1)/10); rect.right = rect.left +8;
			infoWnd.BltFast(20,28+100*i,&numParts,&rect,FALSE);
		}
		rect.left = 8*((i+offsetNum+1)%10); rect.right = rect.left +8;
		infoWnd.BltFast(28,28+100*i,&numParts,&rect,FALSE);
	}
	for(i=0;i<10;i++){
		if(offsetNum/5 == i){
			pageBtn[i].SetState(btn_lock);
		}else{
			pageBtn[i].SetState(btn_normal);
		}
	}
	tmpCBuf.Release();
	sysInf.restoreTextColor();
} // SaveWnd::CreateInfo

BOOL SaveWnd::selectExec()
{
	int		i;
	BOOL	bSetCur = FALSE;
	int		oldSelect = selectNum, oldBtn = btnSelect;
	switch(state){
	  case save_fadein:
		if(timeGetTime() >= startTime){
			alpha = 1.0f;
			state = save_exec;
		}else{
			alpha = 1.0f - (startTime -timeGetTime()) /500.0f;
		}
		SetAlpha();
		btn[2].selectRect.top = 565; btn[2].selectRect.bottom = btn[2].selectRect.top+25;
		btn[2].selectRect.left = LONG(709 +(1.0f- alpha)*(47+40*4));
		btn[2].selectRect.right = btn[2].selectRect.left +86;
		return TRUE;
	  case save_fadeout:
		if(timeGetTime() >= startTime){
			alpha = 0;
			changeExecMode(bak_mode);	
			return FALSE;
		}
		alpha = (startTime -timeGetTime()) /500.0f;
		SetAlpha();
		btn[2].selectRect.top = 457+4*27;
		btn[2].selectRect.top += LONG((1.0f-alpha)*40);
		btn[2].selectRect.bottom = btn[2].selectRect.top +25;
		return TRUE;
	  case save_writeSure:
		if(keyState.push_left || keyState.push_right || keyState.push_up || keyState.push_down){
			bYesNo = !bYesNo;
			SetWindowCursorPos(sysInf.hWnd,yesnoBtn[bYesNo].selectRect.left+43, yesnoBtn[bYesNo].selectRect.top+12);
		}
		for(i=0;i<2;i++){
			if(yesnoBtn[i].CheckState(&sysInf.msPoint)){
				if(i==0){
					PlayEffectNum(evtHandle[11],SE_PUSH);
					state = save_writeExec;
					return TRUE;
				}else{
					PlayEffectNum(evtHandle[11],SE_CANCEL);
					state = save_exec;
				}
			}
		}
		if(keyState.push_cancel){
			PlayEffectNum(evtHandle[11],SE_CANCEL);
			state = save_exec;
		}
		return TRUE;
	  case save_loadSure:
		if(keyState.push_left || keyState.push_right || keyState.push_up || keyState.push_down){
			bYesNo = !bYesNo;
			SetWindowCursorPos(sysInf.hWnd,yesnoBtn[bYesNo].selectRect.left+43, yesnoBtn[bYesNo].selectRect.top+12);
		}
		for(i=0;i<2;i++){
			if(yesnoBtn[i].CheckState(&sysInf.msPoint)){
				if(i==0){
					if(SAV_Load( selectNum+offsetNum+1 )){
						PlayEffectNum(evtHandle[11],SE_PUSH);
						startTime = timeGetTime();
						state = save_loadEnd;
						return TRUE;
					}
					startTime = timeGetTime() +500;
					state = save_fadeout;
					return TRUE;
				}else{
					PlayEffectNum(evtHandle[11],SE_CANCEL);
					state = save_exec;
				}
			}
		}
		if(keyState.push_cancel){
			PlayEffectNum(evtHandle[11],SE_CANCEL);
			state = save_exec;
		}
		return TRUE;
	  case save_writeExec:
		new_time = selectNum+offsetNum+1;
		SAV_Save( new_time );
		useTable[selectNum+offsetNum] = TRUE;

		struct _stat	fstatus;
		char fname[32];
		wsprintf( fname, "save_%02d.sav", new_time );
		_stat(fname,&fstatus);
		file_time[selectNum+offsetNum] = fstatus.st_mtime;

		CreateInfo();
		startTime = timeGetTime();
		state = save_writeEnd;
	  case save_loadEnd:
	  case save_writeEnd:
		i = timeGetTime() -startTime;
		if(i>1000){
			if(state==save_writeEnd){
				changeExecMode(bak_mode);
			}else{
				changeExecMode(event_exec_mode);
			}
			startTime = timeGetTime();
			if(voicefile[0])PlayVoice(voicefile, FALSE, 255);
			voicefile[0] = '\0';
			bFadeOut = TRUE;
		}
		return TRUE;
	}
	btnSelect = 0xff;
	for(i=0;i<3;i++){
		if(btn[i].CheckState(&sysInf.msPoint)){
			btnSelect = i;
			switch(i){
			  case 0:	
				PlayEffectNum(evtHandle[11],SE_PUSH);		
				if(0==offsetNum)offsetNum = 45;
				else offsetNum -= 5;
				CreateInfo();
				return TRUE;
			  case 1:	
				PlayEffectNum(evtHandle[11],SE_PUSH);		
				offsetNum += 5;
				if(offsetNum>45)offsetNum = 0;
				CreateInfo();
				return TRUE;
			  default:	
				PlayEffectNum(evtHandle[11],SE_CANCEL);		
				startTime = timeGetTime() +500;
				state = save_fadeout;
				return TRUE;
			}
		}else if(btn_onCursor==btn[i].GetState()){
			btnSelect = i;
			if(oldBtn != btnSelect){
				PlayEffectNum(evtHandle[11],SE_SELECT);		
			}
		}
	}
	for(i=0;i<10;i++){
		if(pageBtn[i].CheckState(&sysInf.msPoint)){
			PlayEffectNum(evtHandle[11],SE_PUSH);
			btnSelect = i+3;
			offsetNum = i*5;
			CreateInfo();
			return TRUE;
		}else if(btn_onCursor==pageBtn[i].GetState()){
			btnSelect = i+3;
			if(oldBtn != btnSelect){
				PlayEffectNum(evtHandle[11],SE_SELECT);
			}
		}
	}
	if(keyState.push_action){
		if(selectNum<5){
			if(useTable[selectNum+offsetNum]){
				PlayEffectNum(evtHandle[11],SE_PUSH);
				SetWindowCursorPos(sysInf.hWnd,yesnoBtn[0].selectRect.left +43, yesnoBtn[0].selectRect.top +12);
				if(mode==save_mode) state = save_writeSure;
				else state = save_loadSure;
				return TRUE;
			}else if(mode==save_mode){
				PlayEffectNum(evtHandle[11],SE_PUSH);
				state = save_writeExec;
				return TRUE;
			}
		}
	}else if(keyState.push_cancel){
		PlayEffectNum(evtHandle[11],SE_CANCEL);		
		startTime = timeGetTime() +500;
		state = save_fadeout;
		return TRUE;
	}else if(keyState.push_up){
		for(i=0;i<5;i++){
			if(selectNum==0xff){
				selectNum = 0;
			}else if(selectNum==0){
				selectNum = 4;
				if(0==offsetNum)offsetNum = 45;
				else offsetNum -= 5;
				CreateInfo();
			}else{
				selectNum --;
			}
			if(mode==save_mode || useTable[selectNum+offsetNum])break;
		}
		if(i==5){
			selectNum = 0xff;
		}else{
			bSetCur = TRUE;
		}
	}else if(keyState.push_down){
		for(i=0;i<5;i++){
			if(selectNum>=4){
				selectNum = 0;
				offsetNum += 5;
				if(offsetNum>45)offsetNum = 0;
				CreateInfo();
			}else{
				selectNum ++;
			}
			if(mode==save_mode || useTable[selectNum+offsetNum])break;
		}
		if(i==5){
			selectNum = 0xff;
		}else{
			bSetCur = TRUE;
		}
	}else if(keyState.push_left){
		if(0==offsetNum)offsetNum = 45;
		else offsetNum -= 5;
		CreateInfo();
		return TRUE;
	}else if(keyState.push_right){
		offsetNum += 5;
		if(offsetNum>45)offsetNum = 0;
		CreateInfo();
		return TRUE;
	}
	if(bSetCur){
		if(sysInf.msPoint.x>=20 && sysInf.msPoint.x<706){
			point.x = sysInf.msPoint.x;
		}else{
			point.x = 20 +40;
		}
		point.y = 18 +40 +selectNum*100;
		SetWindowCursorPos(sysInf.hWnd, point.x, point.y);
	}else{
		selectNum = 0xff;
		for(i=0;i<5;i++){
			rect.left = 20;
			rect.right = rect.left +686;
			rect.top = 18 +100*i;
			rect.bottom = rect.top +96;
			if(myPtInRect(&rect, &sysInf.msPoint)){
				if(mode==save_mode || useTable[i+offsetNum]){
					selectNum = i;
					break;
				}
			}
		}
	}
	if(0xff != selectNum){
		yesnoBtn[0].selectRect.top = yesnoBtn[1].selectRect.top = 18+128+100*selectNum;
		yesnoBtn[0].selectRect.bottom = yesnoBtn[1].selectRect.bottom = yesnoBtn[0].selectRect.top +25;
		if(selectNum!=oldSelect){
			PlayEffectNum(evtHandle[11],SE_SELECT);
		}
	}
	return TRUE;
} // SaveWnd::selectExec

void SaveWnd::Blt()
{
	int		i;

	int flashTime = timeGetTime() % 1000;
	float flash = (flashTime -500) /500.0f;

	if(bFadeOut){
		alpha = 1.0f -(timeGetTime() -startTime) /800.0f;
		if(alpha <= 0){
			alpha = 0;
			bFadeOut = FALSE;
		}
		SetAlpha();
		btn[2].selectRect.top = 457+4*27;
		btn[2].selectRect.top += LONG((1.0f-alpha)*40);
		btn[2].selectRect.bottom = btn[2].selectRect.top +25;
	}
	if(save_fadein==state || save_fadeout==state){
		g_DibInf.colorBuf.BltFast(0,0, &backupWnd,NULL,FALSE);
	}
	g_DibInf.colorBuf.BltFast(0,0,&backWnd,NULL,TRUE);
	g_DibInf.colorBuf.BltFast(20,18,&infoWnd,NULL,TRUE);
	for(i=0;i<5;i++){
		if(useTable[i+offsetNum]) g_DibInf.colorBuf.BltFast(20,18+100*i,&panel[1],NULL,TRUE);
	}
	if(newMark){
		rect.left = 0; rect.right = 40; rect.top = 22; rect.bottom = 44;
		newParts.alpha = fabsf(flash) *alpha;
		g_DibInf.colorBuf.BltFast(20+4, 18+46 +100*(newMark-1), &newParts,&rect,TRUE);
		rect.top = 0; rect.bottom = 22;
		newParts.alpha = alpha;
		g_DibInf.colorBuf.BltFast(20+4, 18+46 +100*(newMark-1), &newParts,&rect,TRUE);
	}
	rect.top = 48; rect.bottom = rect.top +18;
	rect.left = (offsetNum/5)*20; rect.right = rect.left +20;
	g_DibInf.colorBuf.BltFast(110 ,540,&numParts,&rect,TRUE);
	for(i=0;i<3;i++){
		btn[i].Draw();
	}
	for(i=0;i<10;i++){
		pageBtn[i].Draw();
	}
	if(state >= save_writeSure){
		rect.left = 0; rect.right = WIN_SIZEX;
		rect.top = 0; rect.bottom = 18+100*selectNum;
		g_DibInf.colorBuf.BltFast(0,0,&maskWnd,&rect,TRUE);
		rect.top = 114+100*selectNum; rect.bottom = WIN_SIZEY;
		g_DibInf.colorBuf.BltFast(0,114+100*selectNum,&maskWnd,&rect,TRUE);
		rect.top = 18+100*selectNum; rect.bottom = 114+100*selectNum;
		rect.left = 0; rect.right = 20;
		g_DibInf.colorBuf.BltFast(0,18+100*selectNum,&maskWnd,&rect,TRUE);
		rect.left = 706; rect.right = WIN_SIZEX;
		g_DibInf.colorBuf.BltFast(706,18+100*selectNum,&maskWnd,&rect,TRUE);
	}
	if(selectNum < 0xff){
		panel[2].alpha = fabsf(flash) *alpha;
		g_DibInf.colorBuf.BltFast(20,18+100*selectNum,&panel[2],NULL,TRUE);
	}
	switch(state){
	  case save_writeSure:
	  case save_loadSure:
		for(i=0;i<2;i++){
			yesnoBtn[i].Draw();
		}
		rect.left = 0; rect.right = 530;
		if(save_loadSure==state){
			rect.top = 80; rect.bottom = 120;
			g_DibInf.colorBuf.BltFast(20+115,18+86+100*selectNum,&confirmWnd,&rect,TRUE);
		}else{
			rect.top = 0; rect.bottom = 40;
			g_DibInf.colorBuf.BltFast(20+115,18+86+100*selectNum,&confirmWnd,&rect,TRUE);
		}
		break;
	  case save_loadEnd:
	  case save_writeEnd:
		rect.left = 0; rect.right = 530;
		if(save_writeEnd==state){
			rect.top = 40; rect.bottom = 80;
		}else{
			rect.top = 120; rect.bottom = 160;
		}
		i = 18+86+100*selectNum +LONG((1.0f-alpha)*40);
		g_DibInf.colorBuf.BltFast(20+115,i,&confirmWnd,&rect,TRUE);
		break;
	}
} // SaveWnd::Blt
