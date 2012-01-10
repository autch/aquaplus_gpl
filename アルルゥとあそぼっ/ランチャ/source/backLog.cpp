#include "Clcommon.h"
#include "keyInput.h"
#include "graphic.h"
#include "soundDS.h"
#include "systemWnd.h"

#include "backLog.h"

BackLog		backLog;

void BackLog::Exit()
{
	dispLogLine = allLogLine = 0;
	changeExecMode(bak_execMode);
} // BackLog::Exit

void BackLog::Exec()
{
	if(keyState.push_cancel){
		Exit();
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
			TextDraw();
		}
	}else if(keyState.push_down){
		if(dispBottom==dispTop && bottomBlockOffset==blockOffset){
			Exit();
			return;
		}
		blockOffset ++;
		if(blockOffset>=logLine[dispTop]){
			blockOffset = 0;
			dispTop ++;
			if(dispTop > 99)dispTop = 0;
		}
		TextDraw();
	}else{
		int		i,line=BACKLOGLINE,lineHeight;
		bSelectVoice = FALSE;
		for(i=dispTop;(i!=logBottom && line>0);i++){
			if(i>=100)i=0;
			lineHeight = logLine[i];
			if(i==dispTop)lineHeight -= blockOffset;
			if(lineHeight > line)lineHeight = line;
			if(logVoice[i][0]!='\0'){
				voiceRect.left = 0;	voiceRect.right = 800-30;
				voiceRect.top = (BACKLOGLINE-line)*(mainFontSize+rubiHeight)+5;
				voiceRect.bottom = voiceRect.top +lineHeight*(mainFontSize+rubiHeight);
				if(PtInRect(&voiceRect,sysInf.msPoint)){
					bSelectVoice = TRUE;
					if(keyState.push_action){
						lpSoundDS->playVoice(voiceHandle,pack_voice,logVoice[i]);
					}
					break;
				}
			}
			line -= lineHeight;
		}
	}
} // BackLog::Exec

void BackLog::SetBackLog(char *msg, int line)
{
	int		len;
	WORD	oldOffset;

	if(0==strnicmp(&msg[strlen(msg)-2],"\\n",2)){
		line --;
		msg[strlen(msg)-2] = '\0';
	}
	len = strlen(msg);
	if((logOffset[logBottom] +len) >= LOG_SIZE){
		logOffset[logBottom] = 0;
	}
	oldOffset = logOffset[logBottom];
	strcpy(&logWork[oldOffset],msg);
	logLine[logBottom] = line;
	logBottom ++;
	if(logBottom >= 100) logBottom = 0;
	if(logTop == logBottom){
		logTop ++;
		if(logTop >= 100) logTop = 0;
	}
	logOffset[logBottom] = oldOffset +strlen(msg) +1;
	logVoice[logBottom][0] = '\0';
} // BackLog::SetBackLog

void BackLog::SetVoice(char *voicefile)
{
	strcpy(logVoice[logBottom], voicefile);
} // BackLog::SetVoice

void BackLog::DispInit()
{
	int		line = BACKLOGLINE;
	WORD	tmp_top;

	clearAllPushData();
	bak_execMode = sysInf.execMode;
	changeExecMode(backlog_mode);
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
	backWndCBuf.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	backColorBuf.alpha = 0.4f;
	backWndCBuf.BltFast(0,0,&backColorBuf,&bgRect,TRUE);
	backColorBuf.alpha = 1.0f;

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
} // BackLog::DispInit

BOOL BackLog::TextDraw()
{
	WORD	tmp_top = dispTop;
	for(int y=0;y<msgPlate.height;y++){
		myRGBA *lpPix = (myRGBA *)(msgPlate.pBuf +msgPlate.wPitch*y);
		for(int x=0;x<msgPlate.width;x++){
			lpPix[x].bAlpha = 0;
		}
	}
	bIndent = FALSE;
	msgWidth = msgHeight = 0;
	tmpBlockOffset = blockOffset;
	while(tmp_top!=logBottom){
		if(logVoice[tmp_top][0] != '\0'){
		}
		DisplayMessage((BYTE *)&logWork[logOffset[tmp_top]]);
		if(msgHeight>=BACKLOGLINE)break;
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

	if(logTop==dispTop && 0==blockOffset){
	}else{
	}
	bSelectVoice = FALSE;
	return TRUE;
} // BackLog::TextDraw

void BackLog::DrawBackLog()
{
	if(sysInf.execMode != backlog_mode)return;
	g_DibInf.colorBuf.BltFast(0,0, &backWndCBuf,NULL,FALSE);
	if(bSelectVoice){
		myRGBA	backColor = {0,0,0xff,128};
		g_DibInf.colorBuf.colorBlt24(&backColor,&voiceRect);
	}
	g_DibInf.colorBuf.BltFast(0,0, &msgPlate,NULL,FALSE);
} // BackLog::DrawBackLog

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
		if(msgHeight>=BACKLOGLINE)return;
	}
	if(0==tmpBlockOffset){
		if(bIndent){
			if( (*((WORD *)msgBuf)==0x7681 || *((WORD *)msgBuf)==0x7881 || *((WORD *)msgBuf)==0x6a81) ) bIndent = FALSE;
		}else if(msgWidth==0){
			if( (*((WORD *)msgBuf)==0x7581 || *((WORD *)msgBuf)==0x7781 || *((WORD *)msgBuf)==0x6981) ) bIndent = TRUE;
		}
		OutputOutlineFont(&msgPlate,(BYTE *)msgBuf ,msgWidth*(mainFontSize/2+1)+mainFontSize,msgHeight*(mainFontSize+rubiHeight)+rubiHeight,mainFontSize);
	}
	if(*msgBuf >= 0x80){
		msgWidth += 2;
	}else{
		msgWidth ++;
	}
} // BackLog::MsgTypeChar

char *BackLog::DisplayMessage(BYTE *msgBuf)
{
	DWORD      bLoop = TRUE;
	char        valueBuf[12];
	POINT		rubiOffset;
	int			i,rubiWidth[2];

	if(NULL == msgBuf)return(NULL);

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
					rubiOffset.x = msgWidth*(mainFontSize/2+1)+mainFontSize;
					rubiOffset.y = msgHeight*(mainFontSize+rubiHeight)+rubiHeight-13;
					if(msgWidth>=msgWmax){
						rubiOffset.x = mainFontSize;
						rubiOffset.y = (msgHeight+1)*(mainFontSize+rubiHeight)+rubiHeight-13;
					}
					msgBuf += 2;
					rubiWidth[0] = 0;
					do{
						MsgTypeChar(msgBuf);
						if((*msgBuf) >= 0x80){
							msgBuf += 2;
							rubiWidth[0] += 2;
						}else{
							msgBuf ++;
							rubiWidth[0] ++;
						}
					}while(*msgBuf != '|');
					msgBuf ++;
					rubiWidth[0] = rubiWidth[0]*(mainFontSize/2+1);
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
						rubiOffset.x += rubiWidth[0];
						msgBuf += 2;
					}
					msgBuf ++;
					break;
				  case 'S':  case 's':
					msgBuf += 3;
					break;
				  case 'W':  case 'w':
					for(i=0;;i++){
						if(*(msgBuf+i)=='>')break;
					}
					msgBuf += (i+1);
					break;
				  case 'N': case 'n':
					msgBuf += 2;
					while(*msgBuf != '>'){
						MsgTypeChar(msgBuf);
						if((*msgBuf) >= 0x80){
							msgBuf += 2;
						}else{
							msgBuf ++;
						}
					}
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
				  case '@':
					msgBuf += 2;
					OutputOutlineFontWidth(&msgPlate,(BYTE *)msgBuf ,msgWidth*(mainFontSize/2)+mainFontSize,msgHeight*(mainFontSize+rubiHeight)+rubiHeight,mainFontSize,2);
					msgBuf += 3;
					msgWidth += 4;
					break;
				}
				break;
			  case '>':
				msgBuf ++;
				break;
			  default:
				MsgTypeChar(msgBuf);
				if((*msgBuf) >= 0x80){
					msgBuf += 2;
				}else{
					msgBuf ++;
				}
				break;
			}
			if(msgHeight>=BACKLOGLINE)
				break;
		}else{
			msgWidth = 0;
			if(0==tmpBlockOffset){
				msgHeight ++;
			}else{
				tmpBlockOffset --;
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
