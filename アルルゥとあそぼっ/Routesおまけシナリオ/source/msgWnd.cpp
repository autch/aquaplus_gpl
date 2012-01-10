
#include "Clcommon.h"
#include "graphic.h"
#include "msgWnd.h"
#include "saveWnd.h"
#include "soundDS.h"

ClMsgWnd	msgWnd;

BYTE		ForbidHeadW[] = "ÅAÅBÅCÅDÅcÅEÅHÅIÅJÅKÅRÅSÅTÅUÅXÅ[ÅjÅnÅpÅvÅxÅâÅä";
BYTE	fontMain[3149328];
BYTE	fontSystem[787332];
void LoadFontData(void);

BOOL ClMsgWnd::InitMessageParts()
{
	LoadFontData();
	msgFrame.loadLGF(pack_gparts,"msgWndMain");
	msgWait.loadLGF(pack_gparts,"msgWait");
	nameFrame.loadLGF(pack_gparts,"nameFrame");
	offset.x = 0;
	offset.y = 422;
	msgPlate.createColorBuf(msgWndWidth,msgWndHeight,32,FALSE);
	FillMemory(msgPlate.pBuf,msgPlate.wPitch*msgPlate.height,0xff);	
	namePlate.createColorBuf(168,32,32,FALSE);
	FillMemory(namePlate.pBuf,namePlate.wPitch*namePlate.height,0xff);	
	namePlate.alpha = 0;
	MsgCLS();
	msgPlate.offset.x = 20;	msgPlate.offset.y = 26;
	return TRUE;
} // ClMsgWnd::InitMessageParts

BOOL ClMsgWnd::BltMessage()
{
	BOOL	bAlphaUse;
	if(off==sysInf.bShowMsgwin){
		msgFrame.alpha -= 0.2f;
		if(msgFrame.alpha < 0) msgFrame.alpha = 0;
	}else{
		msgFrame.alpha += 0.2f;
		if(msgFrame.alpha > 1.0f) msgFrame.alpha = 1.0f;
	}
	msgPlate.alpha = msgFrame.alpha;
	if(0==msgPlate.alpha)return FALSE;
	bAlphaUse = FALSE;
	if(msgPlate.alpha < 1.0f)bAlphaUse = TRUE;
	POINT shakePt = {0,0};
	g_DibInf.colorBuf.BltFast(offset.x, offset.y+shakePt.y, &msgFrame,NULL,bAlphaUse);
	if(_BAK_TXT_SHAKE==bgInf.in_type){
		int s_size = bgInf.shakeSize;
		int step;
		if(0<bgInf.look_max){
			step = bgInf.look_max-bgInf.look_cnt;
			s_size = (s_size*(bgInf.look_max-step) /bgInf.look_max );
		}else{
			step = bgInf.shake_cnt;
		}
		shakePt.y = int (s_size *sin(DEGtoRAD(20*step)));
	}
	g_DibInf.colorBuf.BltFast(offset.x, offset.y+shakePt.y, &msgPlate,NULL,bAlphaUse);
	if(bName){
		namePlate.alpha += 0.25f;
		if(namePlate.alpha > 1.0f) namePlate.alpha = 1.0f;
	}else{
		namePlate.alpha -= 0.25f;
		if(namePlate.alpha < 0) namePlate.alpha = 0;
	}
	if(namePlate.alpha > msgFrame.alpha) namePlate.alpha = msgFrame.alpha;
	nameFrame.alpha = namePlate.alpha;
	if(namePlate.alpha){
		bAlphaUse = FALSE;
		if(namePlate.alpha < 1.0f)bAlphaUse = TRUE;
		g_DibInf.colorBuf.BltFast(offset.x+30, offset.y-38, &nameFrame,NULL,bAlphaUse);
		g_DibInf.colorBuf.BltFast(offset.x+30, offset.y-26, &namePlate,NULL,bAlphaUse);
	}
	if(off==sysInf.bMsgStop){
		msgWait.alpha -= 0.2f;
		if(msgWait.alpha < 0){
			msgWait.alpha = 0;
			return TRUE;
		}
	}else{
		msgWait.alpha += 0.1f;
		if(msgWait.alpha > 1.0f) msgWait.alpha = 1.0f;
	}
	if(msgWait.alpha > msgFrame.alpha) msgWait.alpha = msgFrame.alpha;
	waitStep ++;
	if(waitStep >= 30)waitStep = 0;
	rect.left = waitStep *34;
	rect.right = rect.left +34;
	rect.top = 0;	rect.bottom = 34;
	g_DibInf.colorBuf.BltFast(offset.x+738, offset.y+130, &msgWait, &rect, TRUE);
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
	}
	if( (heightOffset+sysInf.FontSize)>msgWndHeight){	
		if(off==sysInf.bMsgAlreadyStop){
			sysInf.bMsgStop = on;
		}else{
			MsgCLS();
		}
	}
	
	sysInf.bShowMsgwin = on;
	if(sysInf.bMsgStop) return;
	sysInf.bMsgAlreadyStop = off;	
	if(msgWidth==0 && msgHeight==0){
		if( (*((WORD *)msgBuf)==0x7581 || *((WORD *)msgBuf)==0x7781 || *((WORD *)msgBuf)==0x6981) ) bIndent = TRUE;
	}
	OutputOutlineFont(&msgPlate,(BYTE *)msgBuf ,widthOffset,heightOffset,sysInf.FontSize);
	if(*msgBuf >= 0x80){
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

	if(NULL == msgBuf)return(NULL);
	if( msgStopCheck() )return ((char *)msgBuf);
	while(bLoop){
		if(*msgBuf){
			switch(*msgBuf){
			  case '\\':
				switch(msgBuf[1]){
				  case 'n':	
					MsgNewline();
					if(on==sysInf.bMsgStop)bLoop = FALSE;
					break;
				  case 'k':	
					sysInf.bMsgStop = on;
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
						if((*msgBuf) >= 0x80){
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
						OutputOutlineFont(&msgPlate,(BYTE *)msgBuf ,rubiOffset.x,rubiOffset.y,systemFontSize);
						rubiOffset.x += rubiWidth[0];
						msgBuf += 2;
					}
					msgBuf ++;
					break;
				  case 'S':  case 's':	
					msgBuf += 3;
					break;
				  case 'W': case 'w':	
					for(i=0;;i++){
						if(*(msgBuf+i)=='>')break;
					}
					if(!keyState.data_skip && !sysInf.bAlreadyRead && !sysInf.bMsgAtOnce){
						*(msgBuf+i) = '\0';
						sysInf.msgWaitTime = timeGetTime() +atoi((char *)msgBuf+2)*1000/30;
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
					rubiOffset.x = (12-i)*(mainFontSize/2)/2 +1;
					while(*msgBuf != '>'){
						OutputOutlineFont(&namePlate,(BYTE *)msgBuf ,rubiOffset.x, 2, mainFontSize);
						if((*msgBuf) >= 0x80){
							rubiOffset.x += mainFontSize;
							msgBuf += 2;
						}else{
							rubiOffset.x += mainFontSize/2;
							msgBuf ++;
						}
					}
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
				MsgTypeChar(msgBuf);
				if(on==sysInf.bMsgStop) return (char *)msgBuf;
				if((*msgBuf) >= 0x80){
					msgBuf += 2;
				}else{
					msgBuf ++;
				}
				if(!keyState.data_skip && !sysInf.bAlreadyRead && !sysInf.bMsgAtOnce && sysInf.msgSpeed<10){
					sysInf.msgWaitTime = timeGetTime() +(10-sysInf.msgSpeed)*10;
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
	lineHeight = heightOffset = widthOffset = 0;
	heightOffset = rubiHeight;
	bIndent = FALSE;
	sysInf.bMsgStop = off;
	sysInf.bMsgAtOnce = off;
	for(y=0;y<msgPlate.height;y++){
		lpPix = (myRGBA *)(msgPlate.pBuf +msgPlate.wPitch*y);
		for(x=0;x<msgPlate.width;x++){
			lpPix[x].bAlpha = 0;
		}
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
	logMsgLine ++;

	heightOffset += lineHeight;
	if(lineHeight==mainFontSize) heightOffset += rubiHeight;
	lineHeight = 0;
	if( (heightOffset+sysInf.FontSize)>msgWndHeight){
		if(off==sysInf.bMsgAlreadyStop){
			sysInf.bMsgStop = on;
		}else{
			MsgCLS();
		}
	}
} // ClMsgWnd::MsgNewline

BOOL ClMsgWnd::msgStopCheck()
{
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
				goto AutoMsg;
			}
		}
	}
	if(saveInf.demo)goto AutoMsg;
	if(keyState.push_action || keyState.data_skip || sysInf.bAlreadyRead){
		if(off==sysInf.bShowMsgwin){
			sysInf.bShowMsgwin = on;
			return TRUE;
		}else{
AutoMsg:
			sysInf.bMsgAtOnce = off;
			if(msgHeight >= msgHmax) MsgCLS();
			sysInf.bMsgStop = off;
			sysInf.bMsgAlreadyStop = on;
			msgWnd.autoWaitStart = timeGetTime();
			msgWnd.autoWaitTime = 0;
			return FALSE;
		}
	}
	if(keyState.push_cancel){
		sysInf.bShowMsgwin = !sysInf.bShowMsgwin;
	}
	return TRUE;
} // ClMsgWnd::msgStopCheck

void ClMsgWnd::TextOut(int x,int y,char *msg)
{
	int			i,len;

	len = strlen(msg);
	for(i=0;i<len;i+=2){
		OutputOutlineFont(&msgPlate,(BYTE *)msg +i,x,y,mainFontSize);
		x+=mainFontSize;
	}
}

void OutputOutlineFont(ColorBuf *destCBuf,BYTE *str,int mx,int my,int size)
{
	int				offset,x,y,c[2],orgSize = mainFontSize;
	int				i,oX,sX,eX,oY,sY,eY;
	BYTE			val;
	myRGBA			*lpPix;

	if(my<0)my = 0;
	if(size==systemFontSize)orgSize = systemFontSize;
	if(*str<0x80){	
		offset = ((*str-0x20)+189*42)*orgSize*orgSize/2;
	}else{
		if(*str>=0xe0) (*str) -= 0x40;
		offset = ((*str-0x81)*189 +(*(str+1)-0x40))*orgSize*orgSize/2;
	}
	if(orgSize!=size)goto StretchFont;

	for(y=0;y<orgSize;y++){
		lpPix = (myRGBA *)(destCBuf->pBuf +destCBuf->wPitch*(my+y)) +mx;
		for(x=0;x<orgSize/2;x++){
			if(orgSize==mainFontSize){
				val = fontMain[offset++];
			}else{
 				val = fontSystem[offset++];
			}
			c[0] = val & 0x0f;
			c[1] = val>>4;
			for(i=0;i<2;i++){
				oX = x*2 +i;
				if(c[i]>=6){	
					if(size!=systemFontSize){
						lpPix[oX].bAlpha = 0xff;
						lpPix[oX].bRed = lpPix[oX].bGreen = lpPix[oX].bBlue = (c[i]-5)*0x7f/10 +0x7f;
					}else{
						lpPix[oX].bAlpha = (c[i]-5)*0xdf/10 +0x20;
						lpPix[oX].bRed = lpPix[oX].bGreen = lpPix[oX].bBlue = 0xff;
					}
				}else if(c[i]){			
					if(c[i]==5) lpPix[oX].bAlpha = 0xff;
					else lpPix[oX].bAlpha = c[i]*0xff/5;
					lpPix[oX].bRed = lpPix[oX].bGreen = lpPix[oX].bBlue = 0;
				}
			}
		}
	}
	return;
StretchFont:
	for(y=0;y<orgSize;y++){
		sY = y*size/orgSize;
		eY = (y+1)*size/orgSize;
		for(x=0;x<orgSize/2;x++){
			if(orgSize==mainFontSize){
				val = fontMain[offset++];
			}else{
 				val = fontSystem[offset++];
			}
			c[0] = val & 0x0f;
			c[1] = val>>4;
			for(i=0;i<2;i++){
				sX = (x*2+i)*size/orgSize;
				eX = (x*2+i+1)*size/orgSize;
				if(c[i]){
					c[i] = 17*c[i];
					for(oY=sY;oY<eY;oY++){
						lpPix = (myRGBA *)(destCBuf->pBuf +destCBuf->wPitch*(my+oY)) +mx;
						for(oX=sX;oX<eX;oX++){
							lpPix[oX].bAlpha = c[i];
							lpPix[oX].bRed = lpPix[oX].bGreen = lpPix[oX].bBlue = 0xff;
						}
					}
				}
			}
		}
	}
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
