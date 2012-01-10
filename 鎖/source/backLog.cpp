
#include "AVGYMZ.h"
#include "keyInput.h"
#include "graphic.h"
#include "soundDS.h"
#include "systemWnd.h"

#include "backLog.h"

BackLog		backLog;

void BackLog::Exit()
{
	state = 0;
	fadeTime = 0;
	dispLogLine = allLogLine = 0;
	changeExecMode(bak_execMode);
	sysInf.restoreTextColor();
	Release();
} // BackLog::Exit

void BackLog::Exec()
{
	int			i,j;
	switch(state){
	  case 0:
		if(0==fadeTime){
			lpSoundDS->FadeOut(voiceHandle);
			fadeTime = timeGetTime() +500;
		}else{
            backWndCBuf.alpha = 1.0f -(fadeTime -(int)timeGetTime())/500.0f;
			if(backWndCBuf.alpha >= 1.0f){
				backWndCBuf.alpha = 1.0f;
				fadeTime = 0;
				state = 1;
			}
			retBtn.selectRect.top = 565; retBtn.selectRect.bottom = retBtn.selectRect.top+25;
			retBtn.selectRect.left = LONG(709 +(1.0f- backWndCBuf.alpha)*(47+40*4));
			retBtn.selectRect.right = retBtn.selectRect.left +86;
		}
		return;
	  case 2:
		if(0==fadeTime){
			lpSoundDS->FadeOut(voiceHandle);
			fadeTime = timeGetTime() +500;
		}else{
            backWndCBuf.alpha = (fadeTime -(int)timeGetTime())/500.0f;
			if(backWndCBuf.alpha <= 0){
				backWndCBuf.alpha = 0;
				Exit();
			}else{
				retBtn.selectRect.top = 457+4*27;
				retBtn.selectRect.top += LONG((1.0f-backWndCBuf.alpha)*40);
				retBtn.selectRect.bottom = retBtn.selectRect.top +25;
			}
		}
		return;
	}
	RECT	scrollBarRect = {693,12,705,580};
	if(retBtn.CheckState(&sysInf.msPoint)){ 
		PlayEffectNum(evtHandle[11],SE_CANCEL);		
		state = 2;
		return;
	}
	if(btn_normal!=retBtn.GetState())keyState.push_action = 0;
	if(myPtInRect(&scrollBarRect,&sysInf.msPoint) || (bOnScrBar&&keyState.data_action)){
		bOnScrBar = TRUE;
		if(keyState.push_action || keyState.data_action){
			float ptr;
			if(sysInf.msPoint.y <= 52)ptr = 0;
			else if(sysInf.msPoint.y >= 541)ptr = 1.0f;
			else{
				ptr = float(sysInf.msPoint.y -52)/489;
			}
			SetScrollPtr(ptr);
			keyState.push_action = 0;
			return;
		}
	}else{
		bOnScrBar = FALSE;
	}
	if(keyState.push_cancel){
		PlayEffectNum(evtHandle[11],SE_CANCEL);		
		state = 2;
		return;
	}else if(keyState.push_up){
		if(logTop!=dispTop || blockOffset){
			if(blockOffset){
				blockOffset --;
			}else if(dispTop>0){
				dispTop --;
				blockOffset = logLine[dispTop] -1;
			}else{
				dispTop = 99;
				blockOffset = logLine[dispTop] -1;
			}
			ScrBarFlash = 15;
			TextDraw();
		}
	}else if(keyState.push_down){
		if(dispBottom==dispTop && bottomBlockOffset==blockOffset){
			state = 2;
			return;
		}
		blockOffset ++;
		if(blockOffset>=logLine[dispTop]){
			blockOffset = 0;
			dispTop ++;
			if(dispTop > 99)dispTop = 0;
		}
		ScrBarFlash = 15;
		TextDraw();
	}else{
		BYTE oldSelectVoice = bSelectVoice;
		bSelectVoice = FALSE;
		for(i=0;i<voiceUse;i++){
			for(j=0;j<voiceCheck[i].useLine;j++){
				if(myPtInRect(&voiceCheck[i].Rect[j],&sysInf.msPoint)){
					bSelectVoice = i +1;
					if(bSelectVoice != oldSelectVoice){
						saveInf.textColor[0] = 0xff;
						saveInf.textColor[1] = saveInf.textColor[2] = 0x40;
						msgWidth = voiceCheck[i].org_msgWidth;
						msgHeight = voiceCheck[i].org_msgHeight;
						tmpBlockOffset = voiceCheck[i].org_blockOffset;
						DisplayMessage((BYTE *)&logWork[logOffset[voiceCheck[i].voiceLine]]);
					}else{
						oldSelectVoice = 0;	
					}
					if(keyState.push_action){	
						voiceNum = voiceCheck[i].logLine;
						if(PCM_PLAY==lpSoundDS->GetStatus(voiceHandle) && 0==strcmp(logVoice[voiceCheck[i].voiceLine],playVoice)){
							lpSoundDS->Stop(voiceHandle);
						}else{
							strcpy(playVoice,logVoice[voiceCheck[i].voiceLine]);
							PlayVoice(playVoice, FALSE, 255);
						}
					}
					i = voiceUse;	
					break;
				}
			}
		}
		if(oldSelectVoice > 0){
			saveInf.textColor[2] = 0x80;
			saveInf.textColor[0] = saveInf.textColor[1] = 0xff;
			msgWidth = voiceCheck[oldSelectVoice-1].org_msgWidth;
			msgHeight = voiceCheck[oldSelectVoice-1].org_msgHeight;
			tmpBlockOffset = voiceCheck[oldSelectVoice-1].org_blockOffset;
			DisplayMessage((BYTE *)&logWork[logOffset[voiceCheck[oldSelectVoice-1].voiceLine]]);
		}
		saveInf.textColor[0] = saveInf.textColor[1] = saveInf.textColor[2] = 0xff;
	}
} // BackLog::Exec

void BackLog::SetBackLog(char *msg, int line, char bContinue)
{
	int		len;
	WORD	oldOffset;

	if(0==strnicmp(&msg[strlen(msg)-2],"\\n",2)){
		line --;
		msg[strlen(msg)-2] = '\0';
	}
	len = strlen(msg)+1;
	if((logOffset[logBottom] +len) >= LOG_SIZE){
		logOffset[logBottom] = 0;	
	}
	oldOffset = logOffset[logBottom];
	logWork[oldOffset] = bContinue;
	strcpy(&logWork[oldOffset+1],msg);
	logLine[logBottom] = line;
	logBottom ++;
	if(logBottom >= 100) logBottom = 0;
	if(logTop == logBottom){
		logTop ++;
		if(logTop >= 100) logTop = 0;
	}
	logOffset[logBottom] = oldOffset +strlen(msg) +1+1;
	logVoice[logBottom][0] = '\0';
} // BackLog::SetBackLog

void BackLog::SetVoiceNum(int charNo,int voice_set)
{
	char	voicefile[32];
	if((-1)==voice_set){
		wsprintf(voicefile,"%05d_%03d_%03d",lpSoundDS->scriptNo,lpSoundDS->voiceCnt,charNo);
	}else{
		wsprintf(voicefile,"%05d_%03d_%03d",lpSoundDS->scriptNo,voice_set,charNo);
	}
	SetVoice(voicefile);
} // BackLog::SetVoice

void BackLog::SetVoice(char *voicefile)
{
	strcpy(logVoice[logBottom], voicefile);
} // BackLog::SetVoice

void BackLog::DispInit()
{
	int		i,line = backlogHeight;
	WORD	tmp_top;

	if(saveInf.demo)return;	
	clearAllPushData();
	bak_execMode = sysInf.execMode;
	changeExecMode(backlog_mode);
	sysInf.backupTextColor();
	saveInf.textColor[0] = saveInf.textColor[1] = saveInf.textColor[2] = 0xff;
	if(logBottom > 0){
		tmp_top = logBottom -1;
	}else{
		tmp_top = 99;
	}
	blockOffset = 0;
	while(1){
		line -= logLine[tmp_top];
		dispTop = tmp_top;
		if(tmp_top==logTop || line==0)
			break;
		if(line<0){
			blockOffset = -line;
			break;
		}
		if(tmp_top > 0){
			tmp_top --;
		}else{
			tmp_top = 99;
		}
	}
	dispBottom = dispTop;
	bottomBlockOffset = blockOffset;
	backupWnd.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	backupWnd.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
	backWndCBuf.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	backColorBuf.alpha = 0.9f;
	backWndCBuf.BltFast(0,0,&backColorBuf,&bgRect,TRUE);
	backColorBuf.alpha = 1.0f;
	ColorBuf	tmpCBuf;
	tmpCBuf.loadLGF(pack_gparts,"backlogbase");
	backWndCBuf.BltFast(0,0,&tmpCBuf,NULL,FALSE);
	tmpCBuf.Release();
	voiceMark.loadLGF(pack_gparts,"backlog_voice");
	scrollBar.loadLGF(pack_gparts,"backlog_slide");
	retBtnPrt.loadLGF(pack_gparts,"returnBtn");
	RECT	selectRect = {709,565,709+86,565+25};
	RECT	btnRect[5];
	for(i=0;i<5;i++){
		btnRect[i].left = 0; btnRect[i].right = 86;
		btnRect[i].top = 25*i; btnRect[i].bottom = btnRect[i].top +25;
	}
	retBtn.Create(3,&retBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);

	msgPlate.createColorBuf(WIN_SIZEX,WIN_SIZEY,32,FALSE);
	FillMemory(msgPlate.pBuf,msgPlate.wPitch*msgPlate.height,0xff);
	TextDraw();
	allLogLine = 0;
	tmp_top = logTop;
	while(tmp_top!=dispTop){
		allLogLine += logLine[tmp_top];
		tmp_top ++;
		if(tmp_top>99) tmp_top = 0;
	}
	allLogLine += blockOffset;
	backWndCBuf.alpha = 0;
	state = 0;
} // BackLog::DispInit

BOOL BackLog::TextDraw()
{
	voiceNum = 0;
	char	bContinue = FALSE;
	WORD	i,tmp_top = dispTop, org_msgWidth, org_msgHeight, org_blockOffset;
	for(int y=0;y<msgPlate.height;y++){
		myRGBA *lpPix = (myRGBA *)(msgPlate.pBuf +msgPlate.wPitch*y);
		for(int x=0;x<msgPlate.width;x++){
			lpPix[x].bAlpha = 0;
		}
	}
	bIndent = FALSE;
	msgWidth = msgHeight = 0;
	tmpBlockOffset = blockOffset;
	voiceUse = 0;
	while(tmp_top!=logBottom){
		org_msgWidth = msgWidth;
		org_msgHeight = msgHeight;
		org_blockOffset = tmpBlockOffset;
		BOOL bUseName = FALSE;
		if(0==strnicmp(&logWork[logOffset[tmp_top]+1],"<N",2) && 0==tmpBlockOffset){
			bUseName = TRUE;
			org_msgHeight ++;
		}
		if(logVoice[tmp_top][0]!='\0'){
			saveInf.textColor[2] = 0x80;
		}else{
			saveInf.textColor[2] = 0xff;
		}
		saveInf.textColor[0] = saveInf.textColor[1] = 0xff;
		DisplayMessage((BYTE *)&logWork[logOffset[tmp_top]]);
		if(logVoice[tmp_top][0]!='\0'){
			voiceCheck[voiceUse].bContinue = bContinue;
			voiceCheck[voiceUse].org_msgWidth = org_msgWidth;
			voiceCheck[voiceUse].org_msgHeight = org_msgHeight;
			voiceCheck[voiceUse].org_blockOffset = org_blockOffset;
			if(bUseName){
				voiceCheck[voiceUse].org_msgHeight = org_msgHeight -1;
			}
			if(FALSE==bContinue){
				voiceCheck[voiceUse].logLine = org_msgHeight +1;
			}else{
				voiceCheck[voiceUse].logLine = voiceCheck[voiceUse-1].logLine;
			}
			voiceCheck[voiceUse].useLine = 0;
			for(i=org_msgHeight;i<=msgHeight;i++){
				voiceCheck[voiceUse].useLine ++;
				if(i==org_msgHeight){
					voiceCheck[voiceUse].Rect[i-org_msgHeight].left  = org_msgWidth*(mainFontSize/2)+36;
					voiceCheck[voiceUse].Rect[i-org_msgHeight].top   = org_msgHeight*(mainFontSize+rubiHeight);
				}else{
					voiceCheck[voiceUse].Rect[i-org_msgHeight].left = 36;
					voiceCheck[voiceUse].Rect[i-org_msgHeight].top = voiceCheck[voiceUse].Rect[i-org_msgHeight-1].bottom;
				}
				voiceCheck[voiceUse].Rect[i-org_msgHeight].bottom = voiceCheck[voiceUse].Rect[i-org_msgHeight].top +mainFontSize+rubiHeight;
				if(i==msgHeight){
					voiceCheck[voiceUse].Rect[i-org_msgHeight].right = msgWidth*(mainFontSize/2)+36;
				}else{
					voiceCheck[voiceUse].Rect[i-org_msgHeight].right = 26*mainFontSize+36;
				}
				if( (i+1)==msgHeight  && 0==msgWidth )
					break;
			}
			voiceCheck[voiceUse].voiceLine = tmp_top;
			voiceUse++;
		}
		bContinue = logWork[logOffset[tmp_top]];
		if(msgHeight>=backlogHeight)break;
		tmp_top ++;
		if(tmp_top > 99)tmp_top = 0;
	}
	dispLogLine = 0;
	tmp_top = logTop;
	while(tmp_top!=dispTop){
		dispLogLine += logLine[tmp_top];
		tmp_top ++;
		if(tmp_top>99) tmp_top = 0;
	}
	dispLogLine += blockOffset;

	bSelectVoice = FALSE;
	return TRUE;
} // BackLog::TextDraw

extern BYTE		ForbidHeadW[];
void BackLog::MsgTypeChar(BYTE *msgBuf)
{
	BYTE	i;
	BYTE	tailCheck = FALSE;
	BYTE	msgWMAX = msgWmax,msgHMAX = msgHmax;

	switch(*msgBuf){
		case '^':
		*msgBuf = ' ';
		break;
		case '~':
		*msgBuf = ',';
		break;
	}
	if( msgWidth>=msgWMAX){
		for( i=0; ForbidHeadW[i]!=0; i+=2){
			if( msgBuf[0]==ForbidHeadW[i] && msgBuf[1]==ForbidHeadW[i+1]){
				tailCheck = TRUE;
				break;
			}
		}
	}
	if( (tailCheck && (msgWidth>=(msgWMAX+1))) ||
		( FALSE==tailCheck && msgWidth>=(msgWMAX-1) ) ){
		msgWidth = 0;
		if(0==tmpBlockOffset){
			msgHeight ++;
		}else{
			tmpBlockOffset --;
		}
		if(msgHeight>=backlogHeight)return;
	}
	if(0==tmpBlockOffset){
		if(bIndent){
			if( (*((WORD *)msgBuf)==0x7681 || *((WORD *)msgBuf)==0x7881 || *((WORD *)msgBuf)==0x6a81) ) bIndent = FALSE;
		}else if(msgWidth==0){
			if( (*((WORD *)msgBuf)==0x7581 || *((WORD *)msgBuf)==0x7781 || *((WORD *)msgBuf)==0x6981) ) bIndent = TRUE;
		}
		OutputOutlineFont(&msgPlate,(BYTE *)msgBuf ,msgWidth*(mainFontSize/2)+36,msgHeight*(mainFontSize+rubiHeight)+rubiHeight+1,mainFontSize);
	}
	if( IsHankaku(msgBuf) ){
		msgWidth ++;	
	}else{
		msgWidth += 2;
	}
} // BackLog::MsgTypeChar

char *BackLog::DisplayMessage(BYTE *msgBuf)
{
	DWORD		bLoop = TRUE;
	char        valueBuf[12];
	POINT		rubiOffset;
	int			i,rubiWidth[2];
	BYTE		orgTextColor[3];

	if(NULL == msgBuf)return(NULL);
	BYTE bContinue = *msgBuf ++;

	while(bLoop){
		if(*msgBuf){
			switch(*msgBuf){
			  case '\\':
				switch(msgBuf[1]){
				  case 'n':
					msgWidth = 0;
					if(0==tmpBlockOffset){
						msgHeight ++;
					}else{
						tmpBlockOffset --;
					}
					break;
				  case 'k':
					break;
				}
				msgBuf += 2;
				break;
			  case '<':
				switch(*(msgBuf+1)){
				  case 'R':  case 'r':	
					rubiOffset.x = msgWidth*(mainFontSize/2)+36;
					rubiOffset.y = msgHeight*(mainFontSize+rubiHeight);
					if(msgWidth>=msgWmax){
						rubiOffset.x = 36;
						rubiOffset.y = (msgHeight+1)*(mainFontSize+rubiHeight);
					}
					msgBuf += 2;
					rubiWidth[0] = 0;
					do{
						MsgTypeChar(msgBuf);
						if(FALSE==IsHankaku(msgBuf)){
							msgBuf += 2;
							rubiWidth[0] += 2;
						}else{
							msgBuf ++;
							rubiWidth[0] ++;
						}
					}while(*msgBuf != '|');
					msgBuf ++;
					rubiWidth[0] = rubiWidth[0]*(mainFontSize/2);
					for(i=0;;i++){
						if(*(msgBuf+i)=='>')break;
					}
					rubiWidth[1] = i*(systemFontSize/2);
					if(rubiWidth[1]<rubiWidth[0] && i>=2){
						rubiWidth[0] = (rubiWidth[0] -rubiWidth[1]) / (i/2);
						rubiOffset.x += (rubiWidth[0]/2);
						rubiWidth[0] += systemFontSize;
					}else{
						rubiOffset.x -= (rubiWidth[1] -rubiWidth[0])/2;
						if(rubiOffset.x<0)rubiOffset.x = 0;
						rubiWidth[0] = systemFontSize;
					}
					while(*msgBuf != '>'){
						if(0==tmpBlockOffset) OutputOutlineFont(&msgPlate,(BYTE *)msgBuf ,rubiOffset.x,rubiOffset.y,systemFontSize);
						if(IsHankaku(msgBuf)){
							rubiOffset.x += rubiWidth[0]/2;
							msgBuf ++;
						}else{
							rubiOffset.x += rubiWidth[0];
							msgBuf += 2;
						}
					}
					msgBuf ++;
					break;
				  case 'S':  case 's':
					for(i=0;;i++){
						if(*(msgBuf+i)=='>')break;
					}
					msgBuf += (i+1);
					break;
				  case 'W':  case 'w':
					for(i=0;;i++){
						if(*(msgBuf+i)=='>')break;
					}
					msgBuf += (i+1);
					break;
				  case 'N': case 'n':	
					msgBuf += 2;
					for(i=0;i<3;i++)orgTextColor[i] = saveInf.textColor[i];
					saveInf.textColor[0] = 0xff; saveInf.textColor[1] = 0x80; saveInf.textColor[2] = 0x40;
					while(*msgBuf != '>'){
						MsgTypeChar(msgBuf);
						if(FALSE==IsHankaku(msgBuf)){
							msgBuf += 2;
						}else{
							msgBuf ++;
						}
					}
					for(i=0;i<3;i++)saveInf.textColor[i] = orgTextColor[i];
					msgWidth = 0;
					if(0==tmpBlockOffset){
						msgHeight ++;
					}else{
						tmpBlockOffset --;
					}
					msgBuf ++;
					break;
				  case 'F': case 'f':
					msgBuf += 2;
					while((*msgBuf)>='0' && (*msgBuf)<='9'){
						msgBuf ++;
					}
					break;
				}
				break;
			  case '>':
				msgBuf ++;
				break;
			  default:
				MsgTypeChar(msgBuf);
				if(FALSE==IsHankaku(msgBuf)){
					msgBuf += 2;
				}else{
					msgBuf ++;
				}
				break;
			}
			if(msgHeight>=backlogHeight)
				break;
		}else{
			if(!bContinue){
				msgWidth = 0;
				if(0==tmpBlockOffset){
					msgHeight ++;
				}else{
					tmpBlockOffset --;
				}
			}
			msgBuf = NULL;
			break;
		}
	}
    return ((char *)msgBuf);
} // BackLog::DisplayMessage

void BackLog::SetScrollPtr(float ptr)
{
	WORD	old_dispTop = dispTop;
	WORD	old_blockOffset = blockOffset;
	WORD	old_dispLogLine = dispLogLine;

	if(allLogLine==0)return;
	dispLogLine = WORD(ptr*allLogLine);

	WORD	line = dispLogLine;
	dispTop = logTop;
	while(1){
		if(line <= logLine[dispTop]){
			blockOffset = line;
			break;
		}
		line -= logLine[dispTop];
		dispTop ++;
		if(dispTop > 99)dispTop = 0;
	}
	if(old_dispTop!=dispTop || old_blockOffset!=blockOffset || old_dispLogLine!=dispLogLine){
		TextDraw();
	}
} // BackLog::SetScrollPtr

void BackLog::DrawBackLog()
{
	int			i;
	if(sysInf.execMode != backlog_mode)return;
	if(state!=1){
		g_DibInf.colorBuf.BltFast(0,0, &backupWnd,NULL,FALSE);
	}
	retBtnPrt.alpha = scrollBar.alpha = msgPlate.alpha = backWndCBuf.alpha;
	g_DibInf.colorBuf.BltFast(0,0, &backWndCBuf,NULL,TRUE);
	g_DibInf.colorBuf.BltFast(0,0, &msgPlate,NULL,TRUE);
	for(i=0;i<voiceUse;i++){
		if(voiceCheck[i].bContinue)continue;
		rect.left = 24;	rect.right = 48; rect.top = 0; rect.bottom = 24;
		voiceMark.alpha = backWndCBuf.alpha;
		if(voiceNum==voiceCheck[i].logLine && PCM_PLAY==lpSoundDS->GetStatus(voiceHandle)){
			rect.left = 48; rect.right = 72;
			int flashTime = timeGetTime() % 1000;
			float flash = (flashTime -500) /500.0f;
			voiceMark.alpha = (fabsf(flash)/2+0.5f) *backWndCBuf.alpha;
		}
		g_DibInf.colorBuf.BltFast(12,(voiceCheck[i].logLine-1)*(mainFontSize+rubiHeight)+rubiHeight,&voiceMark,&rect,TRUE);
	}
	retBtn.Draw();

	int scBar = int(489 *backLog.GetScrollPtr()) +12;
	rect.left = 0; rect.right = 13; rect.top = 0; rect.bottom = 80;
	if(0==allLogLine) scrollBar.alpha *= 0.4f;
	else if(bOnScrBar || ScrBarFlash){
		if(ScrBarFlash) ScrBarFlash --;
		rect.left = 13; rect.right = 26;
	}
	g_DibInf.colorBuf.BltFast(693,scBar,&scrollBar,&rect,TRUE);
} // BackLog::DrawBackLog
