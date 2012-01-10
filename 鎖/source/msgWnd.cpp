#include "AVGYMZ.h"
#include "graphic.h"
#include "msgWnd.h"
#include "saveWnd.h"
#include "soundDS.h"
#include "backLog.h"
#include "systemWnd.h"

ClMsgWnd	msgWnd;

BYTE		ForbidHeadW[] = "、。，．…・？！゛゜ヽヾゝゞ々ー）］｝」』♂♀"; 
BYTE	fontMain[4790016];	
BYTE	fontSystem[1197504];
void LoadFontData(void);

void MessageMenuSet(BOOL sw, BOOL bForce)
{
	if(select_exec_mode==sysInf.execMode)sw = on;
	if(on==sw){
		if(off==sysInf.bMenuEnable || bForce){
			EnableMenuItem( sysInf.hMenu, ID_BACKTITLE, MF_ENABLED );
			if(0==ESC_GetFlag(_セーブロード無効)){
				EnableMenuItem( sysInf.hMenu, ID_LOADEXEC, MF_ENABLED );
				EnableMenuItem( sysInf.hMenu, ID_SAVEEXEC, MF_ENABLED );
			}
			EnableMenuItem( sysInf.hMenu, ID_CONFIG, MF_ENABLED );
			if(saveInf.bBackSelect)EnableMenuItem( sysInf.hMenu, ID_BACKSELECT, MF_ENABLED );
		}
	}else{
		if(on==sysInf.bMenuEnable || bForce){
			EnableMenuItem( sysInf.hMenu, ID_BACKTITLE, MF_GRAYED );
			EnableMenuItem( sysInf.hMenu, ID_LOADEXEC, MF_GRAYED );
			EnableMenuItem( sysInf.hMenu, ID_SAVEEXEC, MF_GRAYED );
			EnableMenuItem( sysInf.hMenu, ID_CONFIG, MF_GRAYED );
			EnableMenuItem( sysInf.hMenu, ID_BACKSELECT, MF_GRAYED );
		}
	}
	sysInf.bMenuEnable = sw;
}

BOOL ClMsgWnd::InitMessageParts()
{
	int			i;

	LoadFontData();
	msgBase[0].loadLGF(pack_gparts,"msgWndBase01");
	msgBase[1].loadLGF(pack_gparts,"msgWndBase02");
	msgBase[2].loadLGF(pack_gparts,"msgWndBase03");
	msgFrame.loadLGF(pack_gparts,"textwindow");
	msgWait[0].loadLGF(pack_gparts,"msgCursor");
	msgWait[1].loadLGF(pack_gparts,"msgCursor2");
	nameFrame.loadLGF(pack_gparts,"namewindow");
	for(i=0;i<3;i++)msgBase[i].alpha = 0;
	offset.x = 0;
	offset.y = 340;
	msgPlate.createColorBuf(msgWndWidth,msgWndHeight,32,FALSE);
	FillMemory(msgPlate.pBuf,msgPlate.wPitch*msgPlate.height,0xff);
	namePlate.createColorBuf(144,24,32,FALSE);
	FillMemory(namePlate.pBuf,namePlate.wPitch*namePlate.height,0xff);
	namePlate.alpha = 0;
	MsgCLS();
	msgPlate.offset.x = 36;	msgPlate.offset.y = 138;
	savePrt.loadLGF(pack_gparts,"msgSave");
	loadPrt.loadLGF(pack_gparts,"msgLoad");
	configPrt.loadLGF(pack_gparts,"msgConfig");
	autoskipPrt.loadLGF(pack_gparts,"msgAutoskip");
	skipPrt.loadLGF(pack_gparts,"msgSkip");
	backlogPrt.loadLGF(pack_gparts,"msgBacklog");
	slidePrt.loadLGF(pack_gparts,"msgSlide");

	RECT	selectRect, partsRect[7];
	for(i=0;i<7;i++){
		partsRect[i].left = 0; partsRect[i].right = partsRect[i].left +86;
		partsRect[i].top = 25*i; partsRect[i].bottom = partsRect[i].top +25;
	}
	selectRect.left = 709; selectRect.right = selectRect.left +86;
	selectRect.top = 462; selectRect.bottom = selectRect.top +25;
	msgBtn[0].Create(3, &savePrt, &selectRect, partsRect, &g_DibInf.colorBuf);
	selectRect.top = 489; selectRect.bottom = selectRect.top +25;
	msgBtn[1].Create(3, &loadPrt, &selectRect, partsRect, &g_DibInf.colorBuf);
	selectRect.top = 538; selectRect.bottom = selectRect.top +25;
	msgBtn[3].Create(5, &autoskipPrt, &selectRect, partsRect, &g_DibInf.colorBuf);
	selectRect.top = 565; selectRect.bottom = selectRect.top +25;
	msgBtn[4].Create(5, &skipPrt, &selectRect, partsRect, &g_DibInf.colorBuf);
	for(i=0;i<5;i++){
		partsRect[i].top = 20*i; partsRect[i].bottom = partsRect[i].top +20;
	}
	selectRect.top = 516; selectRect.bottom = selectRect.top +20;
	msgBtn[2].Create(3, &configPrt, &selectRect, partsRect, &g_DibInf.colorBuf);
	selectRect.left = 690;	selectRect.right = selectRect.left +18;
	selectRect.top = 462;	selectRect.bottom = selectRect.top +130;
	for(i=0;i<5;i++){
		partsRect[i].left = 18*i; partsRect[i].right = partsRect[i].left +18;
		partsRect[i].top = 0; partsRect[i].bottom = partsRect[i].top +130;
	}
	msgBtn[5].Create(3, &backlogPrt, &selectRect, partsRect, &g_DibInf.colorBuf);
	waitStep = timeGetTime();
	return TRUE;
} // ClMsgWnd::InitMessageParts

WORD	msgBaseOffset[3*2] = {0,0, 686,0, 0,242};
BOOL ClMsgWnd::BltMessage()
{
	int		i;

	if(off==sysInf.bShowMsgwin){
		MessageMenuSet(off);
		if(alpha > 0){
			if(FALSE==bFade)fadeTime = timeGetTime()+250;
			if(2==bFade)fadeTime = int(alpha *250) +timeGetTime();
			bFade = 1;
			alpha = (fadeTime -(int)timeGetTime()) / 250.0f;
			if(alpha <= 0){
				alpha = 0;
				bFade = FALSE;
			}
			for(i=0;i<5;i++){
				msgBtn[i].selectRect.top = 462+i*27;
				if(i>2)msgBtn[i].selectRect.top -= 5;
				if( alpha < 0.15f*i+0.4f ){
					msgBtn[i].selectRect.top += LONG((0.15f*i+0.4f-alpha)*40);
				}
				msgBtn[i].selectRect.bottom = msgBtn[i].selectRect.top +25;
			}
			msgBtn[5].selectRect.top = msgBtn[0].selectRect.top;
		}
	}else{
		MessageMenuSet(on);
		if(alpha < 1.0f){
			if(FALSE==bFade)fadeTime = timeGetTime()+250;
			if(1==bFade)fadeTime = int((1.0f -alpha)*250) +timeGetTime();
			bFade = 2;
			alpha = 1.0f -(fadeTime -(int)timeGetTime()) / 250.0f;
			if(alpha >= 1.0f){
				alpha = 1.0f;
				bFade = FALSE;
			}
			for(i=0;i<5;i++){
				msgBtn[i].selectRect.top = 462+i*27;
				if(i>2)msgBtn[i].selectRect.top -= 5;
				msgBtn[i].selectRect.bottom = msgBtn[i].selectRect.top +25;
				msgBtn[i].selectRect.left = LONG(709 +(1.0f-alpha)*(47+40*i));
				msgBtn[i].selectRect.right = msgBtn[i].selectRect.left +86;
			}
			msgBtn[5].selectRect.top = LONG(462 +(1.0f-alpha)*50);
		}
	}
	SetAlpha();
	msgBtn[2].selectRect.bottom = msgBtn[2].selectRect.top +20;
	if(0==alpha) return FALSE;
	if(sysInf.msgWndAlpha)msgFrame.alpha = alpha *(100-sysInf.msgWndAlpha)/100.0f;
	if(alpha < 1.0f){
		rect.left =  offset.x +10-LONG(160*(1.0f-alpha));
		rect.right = offset.x +10+678+LONG(160*(1.0f-alpha));
		rect.top =    offset.y+116 -LONG(60*(1.0f-alpha));
		rect.bottom = offset.y+116 +140 +LONG(60*(1.0f-alpha));
		g_DibInf.colorBuf.Blt(&rect, &msgFrame,NULL,TRUE);
	}else{
		g_DibInf.colorBuf.BltFast(offset.x+10, offset.y+116, &msgFrame,NULL,TRUE);
	}
	for(i=0;i<3;i++){
		g_DibInf.colorBuf.BltFast(offset.x+msgBaseOffset[i*2], offset.y+msgBaseOffset[i*2+1], &msgBase[i],NULL,TRUE);
	}
	POINT shakePt = {0,0};
	int s_size, step, add;
	float ang;
	switch(bgInf.in_type){
	  case _BAK_TXT_SIN:
	  case _BAK_ALL_SIN:
		s_size = bgInf.shakeSize;
		step = ((int)timeGetTime()-(int)bgInf.look_cnt);
		if(bgInf.look_max>0){
			if(step > bgInf.look_max) step = bgInf.look_max;
			s_size = (s_size*(bgInf.look_max-step) /bgInf.look_max );
		}
		add = int(s_size *sinf(DEGtoRAD(3*step)));
		switch(bgInf.shake_direct){
		  case _DIR_U:  case _DIR_D:
			shakePt.x = 0; shakePt.y = add;
			break;
		  case _DIR_L:  case _DIR_R:
			shakePt.x = add; shakePt.y = 0;
			break;
		  case _DIR_DL: case _DIR_UR:
			shakePt.x = -int(add *0.7f);
			shakePt.y = int(add *0.7f);
			break;
		  case _DIR_DR: case _DIR_UL:
			shakePt.x = int(add *0.7f);
			shakePt.y = int(add *0.7f);
			break;
		}
		break;
	  case _BAK_TXT_RAND:
	  case _BAK_ALL_RAND:
		s_size = bgInf.shakeSize;
		step = ((int)timeGetTime()-(int)bgInf.look_cnt);
		if(step > bgInf.look_max) step = bgInf.look_max;
		if(bgInf.look_max>0){
			s_size = (s_size*(bgInf.look_max-step) /bgInf.look_max );
		}
		ang = DEGtoRAD(rndSelect(360));
		shakePt.y = int(sinf(ang)*s_size);
		shakePt.x = int(cosf(ang)*s_size);
		break;
	}
	g_DibInf.colorBuf.BltFast(offset.x+shakePt.x, offset.y+shakePt.y, &msgPlate,NULL,TRUE);
	for(i=0;i<6;i++) msgBtn[i].Draw();
	rect.left = 0;rect.right = 16; rect.top = 0; rect.bottom = 107;
	g_DibInf.colorBuf.BltFast(668,476,&slidePrt,&rect,TRUE);
	if(FALSE==bSlideUse)slidePrt.alpha /= 2;
	rect.top = 107; rect.bottom = 214;
	g_DibInf.colorBuf.BltFast(668,476,&slidePrt,&rect,TRUE);
	rect.top = 214;
	if(bSlideUse)rect.top = 220;
	rect.bottom = rect.top +6;
	g_DibInf.colorBuf.BltFast(667,577-sysInf.msgWndAlpha,&slidePrt,&rect,TRUE);
	if(bName){
		namePlate.alpha += 0.25f;
		if(namePlate.alpha > 1.0f) namePlate.alpha = 1.0f;
	}else{
		namePlate.alpha -= 0.25f;
		if(namePlate.alpha < 0) namePlate.alpha = 0;
	}
	nameFrame.alpha = namePlate.alpha;
	if(namePlate.alpha > alpha) namePlate.alpha = nameFrame.alpha = alpha;
	if(sysInf.msgWndAlpha)nameFrame.alpha = namePlate.alpha *(100-sysInf.msgWndAlpha)/100.0f;
	if(namePlate.alpha){
		namePlate.offset.x = nameFrame.offset.x = LONG(12 -30*(1.0f-namePlate.alpha));
		g_DibInf.colorBuf.BltFast(0, 444, &nameFrame,NULL,TRUE);
		g_DibInf.colorBuf.BltFast(0, 448, &namePlate,NULL,TRUE);
	}
	BOOL bDrawCur = FALSE;
	if(sysInf.bAutoRead || sysInf.bReadSkip || keyState.data_skip){	
		float tmp_alpha = msgWait[0].alpha;
		msgWait[0].alpha = alpha;
		int curStep = (timeGetTime() -waitStep) /33;
		rect.left = 0; rect.right = 48;
		int	top = 546;
		if(FALSE==sysInf.bReadSkip && FALSE==keyState.data_skip){
			rect.top = 62; rect.bottom = rect.top+48;
			g_DibInf.colorBuf.BltFast(624, 546, &msgWait[0], &rect, TRUE);
			rect.top = 110; rect.bottom = rect.top +14;
		}else{
			rect.top = 124; rect.bottom = rect.top +14;
			top = 566;
		}
		msgWait[0].alpha *= (1.0f -abs((curStep %40)-20) / 20.0f);
		g_DibInf.colorBuf.BltFast(624, top, &msgWait[0], &rect, TRUE);
		msgWait[0].alpha = tmp_alpha;
		bDrawCur = TRUE;
	}
	if(off==sysInf.bMsgStop){
		msgWait[0].alpha = msgWait[1].alpha = msgWait[0].alpha -0.2f;
		if(msgWait[0].alpha < 0){
			msgWait[0].alpha = msgWait[1].alpha = 0;
			return TRUE;
		}
	}else{
		msgWait[0].alpha = msgWait[0].alpha +0.1f;
		if(msgWait[0].alpha > 1.0f) msgWait[0].alpha = 1.0f;
		msgWait[1].alpha = msgWait[0].alpha;
	}
	if(msgWait[0].alpha > alpha){
		msgWait[0].alpha = msgWait[1].alpha = alpha;
	}
	if(FALSE==bDrawCur){
		int curStep = (timeGetTime() -waitStep) /33;
		rect.left = 0; rect.right = 48;
		rect.top = (curStep%72) *48;
		rect.bottom = rect.top +48;
		g_DibInf.colorBuf.BltFast(offset.x+624, offset.y+206, &msgWait[1], &rect, TRUE,TRUE);
		rect.top = 0; rect.bottom = 48;
		g_DibInf.colorBuf.BltFast(offset.x+624, offset.y+206, &msgWait[0], &rect, TRUE);
		rect.top = 48; rect.bottom = 62;
		float tmp_alpha = msgWait[0].alpha;
		msgWait[0].alpha *= (1.0f -abs((curStep %40)-20) / 20.0f);
		g_DibInf.colorBuf.BltFast(offset.x+624, offset.y+206, &msgWait[0], &rect, TRUE);
		msgWait[0].alpha = tmp_alpha;
	}
	return TRUE;
} // ClMsgWnd::BltMessage

void ClMsgWnd::MsgTypeChar(BYTE *msgBuf)
{
	BYTE	i;
	BYTE	tailCheck = FALSE;
	BYTE	msgWMAX = msgWmax,msgHMAX = msgHmax;

	msgWMAX = (msgWndWidth/sysInf.FontSize -1)*2;
	switch(*msgBuf){
		case '^':
		*msgBuf = ' ';
		break;
		case '~':
		*msgBuf = ',';
		break;
	}
	if( (widthOffset+sysInf.FontSize*2) > msgWndWidth){
		for( i=0; ForbidHeadW[i]!=0; i+=2){
			if( msgBuf[0]==ForbidHeadW[i] && msgBuf[1]==ForbidHeadW[i+1]){
				tailCheck = TRUE;
				break;
			}
		}
	}
	if( (tailCheck && (msgWndWidth<(widthOffset+sysInf.FontSize))) ||
		( FALSE==tailCheck && (msgWndWidth<(widthOffset+sysInf.FontSize*2)) ) )
	{
		MsgNewline();
		logMsgLine ++;
	}
	if( (heightOffset+sysInf.FontSize)>msgWndHeight){	
		if(off==sysInf.bMsgAlreadyStop){
			sysInf.bMsgStop = on;
		}else{
			MsgCLS();
		}
	}

	sysInf.bShowMsgwin = on; 
	sysInf.bHideMsgwin = off;
	if(sysInf.bMsgStop) return;
	sysInf.bMsgAlreadyStop = off;	
	if(msgWidth==0 && msgHeight==0){
		if( (*((WORD *)msgBuf)==0x7581 || *((WORD *)msgBuf)==0x7781 || *((WORD *)msgBuf)==0x6981) ) bIndent = TRUE;
	}
	BOOL bHankaku = OutputOutlineFont(&msgPlate,(BYTE *)msgBuf ,widthOffset,heightOffset,sysInf.FontSize);
	if(FALSE==bHankaku){
		msgWidth += 2;
		widthOffset += sysInf.FontSize;
		autoWaitTime += (11-sysInf.autoMsg) *WaitOneMsg*2/11;
	}else{
		msgWidth ++;
		widthOffset += (sysInf.FontSize /2);
		autoWaitTime += (11-sysInf.autoMsg) *WaitOneMsg/11;
	}
	if(sysInf.FontSize > lineHeight)lineHeight = sysInf.FontSize;
} // ClMsgWnd::MsgTypeChar

char *ClMsgWnd::DisplayMessage(BYTE *msgBuf)
{
	DWORD      bLoop = TRUE;
	char        valueBuf[12];
	POINT		rubiOffset;
	int			i,rubiWidth[2];
	BOOL		bKuten;

	if(NULL == msgBuf)return(NULL);
	if( msgStopCheck() )return ((char *)msgBuf);
	while(bLoop){
		if(*msgBuf){
			switch(*msgBuf){
			  case '\\':
				switch(msgBuf[1]){
				  case 'n':	
					MsgNewline();
					logMsgLine ++;
					if(on==sysInf.bMsgStop)bLoop = FALSE;
					break;
				  case 'k':	
					sysInf.bMsgStop = on;
					sysInf.msgStopTime = timeGetTime() +(11-sysInf.autoMsg) *2000/11;
					bLoop = FALSE;
					break;
				}
				msgBuf += 2;
				break;
			  case '<':
				switch(*(msgBuf+1)){
				  case 'R':  case 'r':	
					rubiOffset.x = widthOffset;
					rubiOffset.y = heightOffset-13;
					if(msgWidth>=msgWmax && (msgHeight+1)<msgHmax){
						rubiOffset.x = 0;
						rubiOffset.y = heightOffset+(mainFontSize+rubiHeight)-13;
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
						BOOL bHankaku = OutputOutlineFont(&msgPlate,(BYTE *)msgBuf ,rubiOffset.x,rubiOffset.y,systemFontSize);
						if(FALSE==bHankaku){
							rubiOffset.x += rubiWidth[0];
							msgBuf += 2;
						}else{
							rubiOffset.x += rubiWidth[0]/2;
							msgBuf ++;
						}
					}
					msgBuf ++;
					break;
				  case 'S':  case 's':	
					if(sysInf.org_msgSpeed==0xff){
						sysInf.org_msgSpeed = sysInf.msgSpeed;
					}
					for(i=0;;i++){
						if(*(msgBuf+i)=='>')break;
					}
					strncpy(valueBuf,(char *)msgBuf+2,i-2);
					valueBuf[i-2] = '\0';
					sysInf.msgSpeed = atoi(valueBuf);
					msgBuf += (i+1);
					break;
				  case 'W': case 'w':
					for(i=0;;i++){
						if(*(msgBuf+i)=='>')break;
					}
					if(!keyState.data_skip && !sysInf.bReadSkip && !sysInf.bMsgAtOnce){
						strncpy(valueBuf,(char *)msgBuf+2,i-2);
						valueBuf[i-2] = '\0';
						sysInf.msgWaitTime = timeGetTime() +atoi(valueBuf)*1000/30;
						bLoop = FALSE;
					}
					msgBuf += (i+1);
					break;
				  case 'N': case 'n':
					NameCLS();
					msgBuf += 2;
					for(i=0;;i++){
						if(*(msgBuf+i)=='>')break;
					}
					rubiOffset.x = (12-i)*(mainFontSize/2)/2;
					sysInf.backupTextColor();
					saveInf.textColor[0] = saveInf.textColor[1] = saveInf.textColor[2] = 0xff;
					while(*msgBuf != '>'){
						BOOL bHankaku = OutputOutlineFont(&namePlate,(BYTE *)msgBuf ,rubiOffset.x, 0, mainFontSize);
						if(FALSE==bHankaku){
							rubiOffset.x += mainFontSize;
							msgBuf += 2;
						}else{
							rubiOffset.x += mainFontSize/2;
							msgBuf ++;
						}
					}
					sysInf.restoreTextColor();
					msgBuf ++;
					logMsgLine ++;
					bName = TRUE;
					break;
				  case 'F': case 'f':
					msgBuf += 2;
					sysInf.FontSize = 0;
					while((*msgBuf)>='0' && (*msgBuf)<='9'){
						sysInf.FontSize *= 10;
						sysInf.FontSize += (*msgBuf) -'0';
						msgBuf ++;
					}
					break;
				}
				break;
			  case '>':
				sysInf.FontSize = mainFontSize;
				msgBuf ++;
				break;
			  default: 
				bKuten = FALSE;
				if(strncmp((char *)msgBuf,"。",2)==0)bKuten = TRUE;
				MsgTypeChar(msgBuf);
				if(on==sysInf.bMsgStop){
					return (char *)msgBuf;
				}
				if(FALSE==IsHankaku(msgBuf)){
					msgBuf += 2;
				}else{
					msgBuf ++;
				}
				if(!keyState.data_skip && !sysInf.bReadSkip && !sysInf.bMsgAtOnce && sysInf.msgSpeed<10){
					if('\0'!=*msgBuf && 0!=strncmp((char *)msgBuf,"\\k",2)){
						sysInf.msgWaitTime = timeGetTime() +(10-sysInf.msgSpeed)*10;
						if(bKuten){
							sysInf.msgWaitTime += (10-sysInf.msgSpeed)*100;
						}
						if(sysInf.setFPS==0){
							sysInf.msgWaitTime -= 33;
						}else if(sysInf.setFPS==1){
							sysInf.msgWaitTime -= 17;
						}
					}
					bLoop = FALSE;
				}
				break;
			}
		}else{
			msgBuf = NULL;
			break;
		}
	}
    return ((char *)msgBuf);
} // ClMsgWnd::DisplayMessage

void ClMsgWnd::MsgCLS()
{
	int		x,y;
	myRGBA	*lpPix;
	msgWidth = msgHeight = 0;
	lineHeight = widthOffset = 0;
	heightOffset = rubiHeight+1;
	bIndent = FALSE;
	sysInf.bMsgStop = off;
	MessageMenuSet( off );
	sysInf.bMsgAtOnce = off;
	for(y=0;y<msgPlate.height;y++){
		lpPix = (myRGBA *)(msgPlate.pBuf +msgPlate.wPitch*y);
		for(x=0;x<msgPlate.width;x++){
			lpPix[x].bAlpha = 0;
		}
	}
	if(sysInf.org_msgSpeed != 0xff){
		sysInf.msgSpeed = sysInf.org_msgSpeed;
		sysInf.org_msgSpeed = 0xff;
	}
	NameCLS();
} // ClMsgWnd::MsgCLS

void ClMsgWnd::NameCLS()
{
	int		x,y;
	myRGBA	*lpPix;
	for(y=0;y<namePlate.height;y++){
		lpPix = (myRGBA *)(namePlate.pBuf +namePlate.wPitch*y);
		for(x=0;x<namePlate.width;x++){
			lpPix[x].bAlpha = 0;
		}
	}
	bName = FALSE;
} // ClMsgWnd::NameCLS

void ClMsgWnd::MsgNewline()
{
	msgWidth = 0;
	widthOffset = 0;
	msgHeight ++;

	heightOffset += lineHeight;
	if(lineHeight==mainFontSize) heightOffset += rubiHeight;	
	lineHeight = 0;
	if( (heightOffset+sysInf.FontSize)>msgWndHeight){
#ifndef _MASTER
		MessageBox(sysInf.hWnd, "行数が表示領域をオーバーしています。","メッセージエラー",MB_OK|MB_ICONEXCLAMATION);
#endif
		if(off==sysInf.bMsgAlreadyStop){
			sysInf.bMsgStop = on;
		}else{
			MsgCLS();
		}
	}
} // ClMsgWnd::MsgNewline


BOOL CALLBACK ConfDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam );
BOOL ClMsgWnd::CheckButton()
{
	int		i;
	BOOL	ret,bOnButton = FALSE;

	if(FALSE==sysInf.bShowMsgwin)return FALSE;
	if(sysInf.execMode!=event_exec_mode && sysInf.execMode!=select_exec_mode)return FALSE;
	int btnStart = 0;
	if(ESC_GetFlag(_セーブロード無効)) btnStart = 2;
	for(i=btnStart;i<6;i++){
		ret = msgBtn[i].CheckState(&sysInf.msPoint);
		if(btn_onCursor==msgBtn[i].GetState() || btn_push==msgBtn[i].GetState())bOnButton = TRUE;
		if(FALSE==ret)continue;
		PlayEffectNum(evtHandle[11],SE_PUSH);
		switch(i){
		  case 0:
			if(NULL==saveWnd && 0==ESC_GetFlag(_セーブロード無効))saveWnd = new SaveWnd(save_mode);
			return TRUE;
		  case 1:	
			if(NULL==saveWnd && 0==ESC_GetFlag(_セーブロード無効))saveWnd = new SaveWnd(load_mode);
			return TRUE;
		  case 2:
			if(NULL==optionWnd)optionWnd = new OptionMode;
			return TRUE;
		  case 3:
			toggleAutoRead();
			break;
		  case 4:
			toggleMessageSkip();
			break;
		  case 5:
			backLog.DispInit();
			break;
		}
	}
	rect.left = 667; rect.right = rect.left +18;
	rect.top = 477;	rect.bottom = rect.top +105;
	bSlideUse = FALSE;
	if(myPtInRect(&rect,&sysInf.msPoint)){
		bOnButton = TRUE;
		bSlideUse = TRUE;
		if(keyState.data_action){
			sysInf.msgWndAlpha = sysInf.msPoint.y -rect.top;
			if(sysInf.msgWndAlpha > 100)sysInf.msgWndAlpha = 100;
			sysInf.msgWndAlpha = 100- sysInf.msgWndAlpha;
		}
	}
	return bOnButton;
} // ClMsgWnd::CheckButton

void ClMsgWnd::toggleAutoRead()
{
	sysInf.bAutoRead = !sysInf.bAutoRead;
	if(sysInf.bAutoRead){
		CheckMenuItem( sysInf.hMenu, ID_AUTOREAD,   MF_CHECKED );
		msgBtn[3].SetState(btn_lock);
	}else{
		CheckMenuItem( sysInf.hMenu, ID_AUTOREAD,   MF_UNCHECKED );
		msgBtn[3].SetState(btn_normal);
	}
} // ClMsgWnd::toggleAutoRead

void ClMsgWnd::toggleMessageSkip()
{
	sysInf.bReadSkip = !sysInf.bReadSkip;
	if(sysInf.bReadSkip){
		sysInf.bAlreadyRead = AVG_CheckScenarioFlag();
		if(sysInf.bAlreadyRead){
			CheckMenuItem( sysInf.hMenu, ID_READSKIP,   MF_CHECKED );
			msgBtn[4].SetState(btn_lock);
		}else{
			sysInf.bReadSkip = off;
		}
	}else{
		CheckMenuItem( sysInf.hMenu, ID_READSKIP,   MF_UNCHECKED );
		msgBtn[4].SetState(btn_normal);
	}
} // ClMsgWnd::toggleMessageSkip

BOOL ClMsgWnd::msgStopCheck()
{
	if(sysInf.bMsgWndCls){
		if(FALSE==sysInf.bAutoRead && FALSE==sysInf.bReadSkip && FALSE==keyState.data_skip && sysInf.bMsgStop){
			if(sysInf.bShowMsgwin && sysInf.msPoint.y >= (WIN_SIZEY-1)){
				sysInf.bShowMsgwin = off;
				sysInf.bHideMsgwin = on;
			}
		}
	}
	if(sysInf.bHideMsgwin && sysInf.msPoint.y<(WIN_SIZEY-1)){
		sysInf.bShowMsgwin = on;
		sysInf.bHideMsgwin = off;
	}
	if(off==sysInf.bMsgStop){
		if(keyState.push_action && !saveInf.demo){
			sysInf.bMsgAtOnce = on;
			return FALSE;
		}
		if(timeGetTime() < sysInf.msgWaitTime){
			return TRUE;
		}
		return FALSE;
	}
	if(sysInf.bAutoRead){
		if(autoWaitTime<(20*(11-sysInf.autoMsg) *WaitOneMsg/11))
			autoWaitTime = 20*(11-sysInf.autoMsg) *WaitOneMsg/11;
		if(PCM_PLAY!=lpSoundDS->GetStatus(voiceHandle)){
			if( (autoWaitStart+autoWaitTime)<timeGetTime() ){
				if(sysInf.msgStopTime < timeGetTime())
					goto AutoMsg;
			}
		}
	}
	if(saveInf.demo)goto AutoMsg;
	if(keyState.push_action || keyState.data_skip || sysInf.bReadSkip){
		if(off==sysInf.bShowMsgwin){
			sysInf.bShowMsgwin = on;
			sysInf.bHideMsgwin = off;
			return TRUE;
		}else{
AutoMsg:
			sysInf.bMsgAtOnce = off;
			if(sysInf.voiceFlag & 0x0800)lpSoundDS->Stop(voiceHandle);
			if(msgHeight >= msgHmax) MsgCLS();
			sysInf.bMsgStop = off;
			MessageMenuSet( off );
			sysInf.bMsgAlreadyStop = on;
			msgWnd.autoWaitStart = timeGetTime();
			msgWnd.autoWaitTime = 0;
			return FALSE;
		}
	}
	if(keyState.push_cancel && !sysInf.bAutoRead){
		sysInf.bShowMsgwin = !sysInf.bShowMsgwin;
		sysInf.bHideMsgwin = off;
	}
	return TRUE;
} // ClMsgWnd::msgStopCheck

void ClMsgWnd::TextOut(int x,int y,char *msg)
{
	int			i,len;

	len = strlen(msg);
	for(i=0;i<len;i+=2){
		BOOL bHankaku = OutputOutlineFont(&msgPlate,(BYTE *)msg +i,x,y,mainFontSize);
		if(FALSE==bHankaku){
			x+=mainFontSize;
		}else{
			x+=mainFontSize/2;
			i --;
		}
	}
}

BOOL OutputOutlineFont(ColorBuf *destCBuf,BYTE *str,int mx,int my,int size, BOOL useKage)
{
	int				offset,x,y,c[2],orgSize = mainFontSize;
	int				i,oX,sX,eX,oY,sY,eY;
	BYTE			val;
	myRGBA			*lpPix;
	BOOL			bHankaku = FALSE;
	BYTE			str1 = *str, str2 = *(str+1);

	if(size==systemFontSize)orgSize = systemFontSize;
	if(str1<0x80){	
		offset = ((str1-0x20)+189*43)*orgSize*orgSize;
		bHankaku = TRUE;
	}else if(str1>=0xA0 && str1<=0xDF ){	
		offset = ((str1-0x40)+189*43)*orgSize*orgSize;
		bHankaku = TRUE;
	}else{
		if(str1>=0xf0){	
			str1 -= 0x45;
		}else if(str1>=0xe0){
			str1 -= 0x40;
		}
		offset = ((str1-0x81)*189 +(str2-0x40))*orgSize*orgSize;
	}
	if(orgSize!=size)goto StretchFont;

	for(y=0;y<orgSize;y++){
		lpPix = (myRGBA *)(destCBuf->pBuf +destCBuf->wPitch*(my+y)) +mx;
		for(x=0;x<orgSize;x++){
			if(orgSize==mainFontSize){
				val = fontMain[offset++];
			}else{
 				val = fontSystem[offset++];
			}
			if(size!=systemFontSize){
				if(0==val)continue;
				if(val<=64){	
					lpPix[x].bAlpha = 0xff;
					DWORD cBase = (val-1)*0xff/63;
					lpPix[x].bRed   = BYTE(cBase *saveInf.textColor[0] /0xff);
					lpPix[x].bGreen = BYTE(cBase *saveInf.textColor[1] /0xff);
					lpPix[x].bBlue  = BYTE(cBase *saveInf.textColor[2] /0xff);
				}else if(useKage){	
					lpPix[x].bAlpha = (val-65)*0xdb/63 +0x24;
					lpPix[x].bRed = lpPix[x].bGreen = lpPix[x].bBlue = 0;
				}
			}else if(val>0){
				lpPix[x].bAlpha = 0xff;
				DWORD cBase = (val-1)*0xbf/63 +0x40;
				lpPix[x].bRed   = BYTE(cBase *saveInf.textColor[0] /0xff);
				lpPix[x].bGreen = BYTE(cBase *saveInf.textColor[1] /0xff);
				lpPix[x].bBlue  = BYTE(cBase *saveInf.textColor[2] /0xff);
			}
		}
	}
	return bHankaku;
StretchFont:
	for(y=0;y<orgSize;y++){
		sY = y*size/orgSize;
		eY = (y+1)*size/orgSize;
		for(x=0;x<orgSize;x++){
			if(orgSize==mainFontSize){
				val = fontMain[offset++];
			}else{
 				val = fontSystem[offset++];
			}
			if(0==val)continue;
			sX = x*size/orgSize;
			eX = (x+1)*size/orgSize;
			if(val<=64){	
				for(oY=sY;oY<eY;oY++){
					lpPix = (myRGBA *)(destCBuf->pBuf +destCBuf->wPitch*(my+oY)) +mx;
					for(oX=sX;oX<eX;oX++){
						lpPix[oX].bAlpha = 0xff;
						DWORD cBase = (val-1)*0xff/63;
						lpPix[oX].bRed   = BYTE(cBase *saveInf.textColor[0] /0xff);
						lpPix[oX].bGreen = BYTE(cBase *saveInf.textColor[1] /0xff);
						lpPix[oX].bBlue  = BYTE(cBase *saveInf.textColor[2] /0xff);
					}
				}
			}else if(useKage){ 
				for(oY=sY;oY<eY;oY++){
					lpPix = (myRGBA *)(destCBuf->pBuf +destCBuf->wPitch*(my+oY)) +mx;
					for(oX=sX;oX<eX;oX++){
						lpPix[oX].bAlpha = (val-65)*0xdb/63 +0x24;
						lpPix[oX].bRed = lpPix[oX].bGreen = lpPix[oX].bBlue = 0;
					}
				}
			}
		}
	}
	return bHankaku;
}

void LoadFontData()
{
	int		size;
	char	*lpMemory;
	size = readFile->ReadPackFile(pack_gparts,"mainFont.fnt",(char **)&lpMemory);
	CopyMemory(fontMain,lpMemory,size);
	cl_free(lpMemory);
	size = readFile->ReadPackFile(pack_gparts,"systemFont.fnt",(char **)&lpMemory);
	CopyMemory(fontSystem,lpMemory,size);
	cl_free(lpMemory);
} // LoadFontData

void SetDemoFlag( int flag, int demo_count )
{
	saveInf.demo = flag;
	saveInf.demo_cnt = 0;
	saveInf.demo_max = demo_count;
}
