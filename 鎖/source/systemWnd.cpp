#include "AVGYMZ.h"
#include "backLog.h"
#include "keyInput.h"
#include "saveWnd.h"
#include "soundDS.h"

#include "systemWnd.h"

OptionMode	*optionWnd = NULL;
MusicModeWnd *musicWnd = NULL;
CG_ModeWnd	*cgmodeWnd = NULL;
TitleWnd	*titleWnd = NULL;
RudderWnd	*rudderWnd = NULL;
MapWnd		*mapWnd = NULL;

extern BYTE floorConvert[28][2];
BYTE	floorMax[5] = {6,7,5,6,4};
BYTE	floorPlateMax[5] = {3,3,2,3,3};

MapWnd::MapWnd()
{
	ColorBuf	tmpCBuf;
	RECT		btnRect[5];
	char		fname[32];
	int			i;

	ZeroMemory(this,sizeof(MapWnd));
	bak_mode = sysInf.execMode;
	changeExecMode(map_mode);	

	floorNum = mainRoom = lookFloor = subRoom = -1;
	selectFloor = 0;
	if(SaveStruct.sdata.mainRoomNo>0 && SaveStruct.sdata.mainRoomNo<=28){
		selectFloor = floorNum = floorConvert[SaveStruct.sdata.mainRoomNo -1][0];
		mainRoom = floorConvert[SaveStruct.sdata.mainRoomNo -1][1];
	}
	if(SaveStruct.sdata.subRoomNo>0 && SaveStruct.sdata.subRoomNo<=28){
		lookFloor = floorConvert[SaveStruct.sdata.subRoomNo -1][0];
		subRoom = floorConvert[SaveStruct.sdata.subRoomNo -1][1];
	}
	backupWnd.createColorBuf(WIN_SIZEX, WIN_SIZEY,24,FALSE);
	backupWnd.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
	baseCBuf.createColorBuf(WIN_SIZEX, WIN_SIZEY,24,FALSE);
	baseCBuf.BltFast(0,0,&backColorBuf,NULL,FALSE);
	tmpCBuf.loadLGF(pack_gparts, "infoBG");
	baseCBuf.BltFast(0,0,&tmpCBuf,NULL,TRUE);
	tmpCBuf.Release();
	wsprintf(fname,"mapIntro%02d",rndSelect(5)+1);
	mapSelectBuf.loadLGF(pack_gparts,fname);
	mapFloorBuf.loadLGF(pack_gparts,"mapSelect");
	floorSelect.loadLGF(pack_gparts,"mapFloorMask");
	mapSelectRect.left = 0; mapSelectRect.right = 560;
	mapSelectRect.top = 0; mapSelectRect.bottom = 420;

	wsprintf(fname,"mapDeck%d_base",selectFloor+1);
	floorCBuf[0].loadLGF(pack_gparts,fname);
	for(i=0;i<floorMax[selectFloor];i++){
		wsprintf(fname,"mapDeck%d_%02d",selectFloor+1,i);
		floorCBuf[i+1].loadLGF(pack_gparts,fname);
	}
	for(i=0;i<floorPlateMax[selectFloor];i++){
		wsprintf(fname,"mapRname_%df%02d",selectFloor+1,i);
		floorNameBuf[i].loadLGF(pack_gparts,fname);
	}
	mapAllFloor.loadLGF(pack_gparts,"map_all");
	RoomNameBase.loadLGF(pack_gparts,"mapRoomName_base");
	RoomNameLine.loadLGF(pack_gparts,"mapLine");
	FloorNamePrt.loadLGF(pack_gparts,"mapFloorName");
	selectFloorPrt.loadLGF(pack_gparts,"mapSelectFloor");
	for(i=0;i<5;i++){
		wsprintf(fname,"mapRoom_%df",i+1);
		RoomName[i].loadLGF(pack_gparts,fname);
	}
	itemList_s.loadLGF(pack_gparts,"itemList_small");
	itemSelect.loadLGF(pack_gparts,"itemSelect");
	itemRet.loadLGF(pack_gparts,"itemret");
	itemInfoPtr.createColorBuf(26*mainFontSize,4*(mainFontSize+14),32,TRUE);
	retBtnPrt.loadLGF(pack_gparts,"returnBtn");
	RECT	selectRect = {709,565,709+86,565+25};
	for(i=0;i<5;i++){
		btnRect[i].left = 0; btnRect[i].right = 86;
		btnRect[i].top = 25*i; btnRect[i].bottom = btnRect[i].top +25;
	}
	retBtn.Create(3,&retBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);
	CreateItemList();

	btnSelect = 0xff;
	map_alpha = 1.0f;
	lastFloor = tmpfloorNum = -1;
	itemNum = tmpItemNum = -1;
	PlayEffectNum(evtHandle[11],SE_PUSH);
	state = map_fadein;
	alpha_time = timeGetTime();

} // MapWnd::MapWnd

char *itemName[24] ={ 
	"マスターキー","まち針","手錠","オイルライター","モップ",
"消火器","ロープ","瞬間接着剤","鉄やすりの破片","ペティナイフ",
"アイスピック","ブラシ","金の針","火炎瓶","拳銃（リボルバー）",
"拳銃（オートマチック）","クロスボウ","ペットボトル爆弾・赤","ペットボトル爆弾・青","信号弾",

"ジャンボでかでかぶりぶりクラッカー","プラチナリング","スタンガン","黒塗りの長剣",
};

char *itemInfo[24] ={
	"バシリスク号のマスターキー。岸田から志乃さんにことづかった。",
	"可憐に繕い物を頼んだら一緒についてきた。\\nただのかぎ裂きになぜまち針が必要なのか謎だ。",
	"悪人を拘束するためのもの。もしくは愛情の表現にも使われる。\\nいかにも頑丈そうだ。",
	"打ち上げ花火に点火するため、珠美が岸田から借りて\\nきた。それをさらに俺が取り上げたもの。\\nオイルが切れかかっている。",
	"モニタールームで見つけた何の変哲もないモップ。\\n武器として見るには頼りない。\\n意外と丈夫。",
	"火を消すための道具。油火災、電気火災にも活躍。\\n消防署の方からって消防署からって意味じゃないよね。",
	"モニタールームで見つけたロープ。\\n長くて丈夫なのが取り柄。といってもナニのことではない。",
	"決して真似はしないでください、という使用法をＣＭで流す不思議な商品。\\n大抵、使ってる途中でどこかにやってしまう。",
	"岸田が振り回す鉄やすりの破片。先端の長さ１０センチくらい。\\n刃物用なので爪研ぎにはちょっと痛い。",
	"フォルムが美しい小型のナイフ。\\nペティとペッティングは明らかに違う。",
	"氷を砕くための錐のような道具。\\nおしゃれな一般家庭でも稀に見かけるが、使われることはまずない。",
	"モップタイプ。先端が弁当箱みたいな形で緑の植毛がされている。",
	"滅菌処理されてビニールで個別密封された金色の針。\\n医療器具っぽい。\\nこれも何らかの愛情の表現に使われるのかもしれない。",
	"世界一アルコール度数の高い酒、スピリタスを使った火炎瓶。\\n口火には珠美のパンツの切れ端（天然繊維）",
	"ラウンジのガラス棚にあった回転式拳銃。\\nロシアンルーレットなど、色々と白黒つけるために使用する。",
	"貴賓室の隠し金庫にあった自動式拳銃。\\n「ジャムりやがった！」というセリフは一度リアルで言ってみたい。",
	"ピストルのような形をしている。\\n小型だが威力は抜群、のはず。",
	"炭酸系のペットボトルを使った爆弾。\\n中には水とドライアイスとガラス片が入っている。\\n真似しちゃダメですよ。",
	"非炭酸系のペットボトルを使った爆弾。\\n中には水とドライアイスとガラス片が入っている。\\n真似しちゃダメですよ。",
	"何らかの意思を伝えるため、補助的に使われる道具。\\n強い光を放ちながら飛んでいく。\\nどーしても真横に向けて撃ちたくなる代物。",
	"通常のクラッカー十本分の火薬をつめた巨大なクラッ\\nカー。珠美のお手製。命名も当然珠美。\\n決して真似しないように。",
	"可憐がまだ見ぬ婚約者から虫除けの意味で前渡しされた結婚指輪。ごくシンプルなデザイン。",
	"電気コンロのニクロム線をむしってコンセントのプラグにくっつけただけの、スタンガンと呼ぶのもおこがましい代物。珠美の珍発明。",
	"シュラスコで使う鍔なしの長剣。\\nこれに肉を刺して炙り焼きにする。使用後なので炭が付着して黒くなっている。",
};

void MapWnd::CreateItemInfo()
{
	int			i,x,y,len;

	itemInfoPtr.clearColorBuf();
	OutputOutlineFont(&itemInfoPtr,(BYTE *)"●",0,0,mainFontSize,FALSE);
	len = strlen(itemName[itemNum]);
	x = mainFontSize;	y = 0;
	for(i=0;i<len;i+=2){
		BOOL bHankaku = OutputOutlineFont(&itemInfoPtr,(BYTE *)itemName[itemNum] +i,x,y,mainFontSize,FALSE);
		if(FALSE==bHankaku){
			x+=mainFontSize;
		}else{
			x+=mainFontSize/2;
			i --;
		}
	}
	len = strlen(itemInfo[itemNum]);
	x = mainFontSize;	y = mainFontSize+14;
	for(i=0;i<len;i+=2){
		if(x>=26*mainFontSize){
			x = mainFontSize;
			y += (mainFontSize+14);
		}
		if(itemInfo[itemNum][i] == '\\'){
			x = mainFontSize;
			y += (mainFontSize+14);
			i += 2;
		}
		BOOL bHankaku = OutputOutlineFont(&itemInfoPtr,(BYTE *)itemInfo[itemNum] +i,x,y,mainFontSize,FALSE);
		if(FALSE==bHankaku){
			x+=mainFontSize;
		}else{
			x+=mainFontSize/2;
			i --;
		}
	}
}

void MapWnd::CreateItemList()
{
	int		i;
	itemMax = 0;
	for(i=0;i<24;i++){
		if(ESC_GetFlag( 200+i )){
			itemTable[itemMax++] = i;
		}
	}
}

void MapWnd::SelectItem()
{
	int		i;
	if(itemMax <= 0)return;
	short	oldItem = tmpItemNum;
	char	fname[64];

	tmpItemNum = -1;
	for(i=0;i<30;i++){
		if(i >= itemMax)break;
		rect.left = 12+52*(i%15);	rect.right = rect.left +48;
		rect.top = 462+(i/15)*50;	rect.bottom = rect.top +48;
		if(myPtInRect(&rect,&sysInf.msPoint)){
			tmpItemNum = i;
			break;
		}
	}
	if(-1!=tmpItemNum && keyState.push_action && itemNum!=itemTable[tmpItemNum]){
		if(state == map_itemView){
			oldItemNum = itemNum;
			itemNum = itemTable[tmpItemNum];
			itemList[1].Release();
			CopyMemory(&itemList[1],&itemList[0],sizeof(ColorBuf));
			ZeroMemory(&itemList[0],sizeof(ColorBuf));
			alpha_time = timeGetTime() +500;
			PlayEffectNum(evtHandle[11],SE_PUSH);
			map_alpha = 1.0f;
			state = map_itemchangein;
		}else{
			itemNum = itemTable[tmpItemNum];
			CreateItemInfo();
			alpha_time = timeGetTime() +500;
			PlayEffectNum(evtHandle[11],SE_PUSH);
			state = map_itemViewin;
		}
		wsprintf(fname, "item_%02d",itemNum);
		itemList[0].loadLGF(pack_gparts,fname);
		return;
	}
	if(oldItem!=tmpItemNum && tmpItemNum!=(-1)) PlayEffectNum(evtHandle[11],SE_SELECT);
}

BOOL MapWnd::Exec()
{
	int			i;
	BYTE		oldBtn;
	char		fname[32];

	switch(state){
	  case map_fadein:
		alpha = (timeGetTime() -alpha_time)/800.0f;
		if(alpha >= 1.0f){
			alpha = 1.0f;
			mapSelectBuf.Release();
			for(i=0;i<3;i++)fontColor[i] = saveInf.textColor[i];
			saveInf.textColor[0] = saveInf.textColor[1] = saveInf.textColor[2] = 0xff;
			alpha_time = timeGetTime();
			map_alpha = 0;
			state = map_selectin;
		}
		mapSelectCnt = BYTE(14 * alpha);
		mapSelectRect.top = mapSelectCnt*420; mapSelectRect.bottom = mapSelectRect.top +420;

		retBtn.selectRect.top = 565; retBtn.selectRect.bottom = retBtn.selectRect.top+25;
		retBtn.selectRect.left = LONG(709 +(1.0f- alpha)*(47+40*4));
		retBtn.selectRect.right = retBtn.selectRect.left +86;
		return TRUE;
	  case map_fadeout:
		alpha = (alpha_time -(int)timeGetTime())/500.0f;
		if(alpha < 0){
			alpha = 0;
			changeExecMode(bak_mode);
			return FALSE;	
		}
		retBtn.selectRect.top = 457+4*27;
		retBtn.selectRect.top += LONG((1.0f-alpha)*40);
		retBtn.selectRect.bottom = retBtn.selectRect.top +25;
		return TRUE;
	  case map_itemViewin:
		map_alpha = (alpha_time -(int)timeGetTime())/500.0f;
		if(map_alpha <= 0){
			map_alpha = 0;
			state = map_itemView;
		}
		return TRUE;
	  case map_itemView:
		SelectItem();
		oldBtn = btnSelect;
		btnSelect = 0xff;
		if(keyState.push_cancel || retBtn.CheckState(&sysInf.msPoint)){
			PlayEffectNum(evtHandle[11],SE_CANCEL);
			alpha_time = timeGetTime();
			state = map_itemViewout;
		}else if(retBtn.GetState()==btn_onCursor){
			btnSelect = 1;
			if(oldBtn != btnSelect) PlayEffectNum(evtHandle[11],SE_SELECT);
		}
		return TRUE;
	  case map_itemViewout:
		map_alpha = (timeGetTime() -alpha_time)/500.0f;
		if(map_alpha >= 1.0f){
			map_alpha = 1.0f;
			itemNum = -1;
			state = map_select;
		}
		return TRUE;
	  case map_itemchangein:
		map_alpha = (alpha_time -(int)timeGetTime())/500.0f;
		if(map_alpha <= 0){
			CreateItemInfo();
			map_alpha = 0;
			alpha_time = timeGetTime();
			state = map_itemchangeout;
		}
		return TRUE;
	  case map_itemchangeout:
		map_alpha = (timeGetTime() -alpha_time)/500.0f;
		if(map_alpha >= 1.0f){
			map_alpha = 0;
			state = map_itemView;
		}
		return TRUE;
	  case map_selectin:
		map_alpha = (timeGetTime() -alpha_time)/500.0f;
		if(map_alpha >= 1.0f){
			map_alpha = 1.0f;
			state = map_select;
		}
		return TRUE;
	}
	oldBtn = btnSelect;
	btnSelect = 0xff;
	if(keyState.push_cancel || retBtn.CheckState(&sysInf.msPoint)){ 
		for(i=0;i<3;i++)saveInf.textColor[i] = fontColor[i];
		PlayEffectNum(evtHandle[11],SE_CANCEL);		
		alpha_time = timeGetTime() +500;
		state = map_fadeout;
		return TRUE;
	}else if(retBtn.GetState()==btn_onCursor){
		btnSelect = 1;
		if(oldBtn != btnSelect) PlayEffectNum(evtHandle[11],SE_SELECT);
	}
	short oldFloor = tmpfloorNum;
	tmpfloorNum = SelectFloor();
	if(oldFloor!=tmpfloorNum && tmpfloorNum!=(-1)) PlayEffectNum(evtHandle[11],SE_SELECT);
	if(tmpfloorNum!=(-1) && keyState.push_action){	
		lastFloor = selectFloor;
		selectFloor = tmpfloorNum;
		PlayEffectNum(evtHandle[11],SE_PUSH);
		wsprintf(fname,"mapDeck%d_base",selectFloor+1);
		floorCBuf[0].loadLGF(pack_gparts,fname);
		for(i=0;i<floorMax[selectFloor];i++){
			wsprintf(fname,"mapDeck%d_%02d",selectFloor+1,i);
			floorCBuf[i+1].loadLGF(pack_gparts,fname);
		}
		for(i=0;i<floorPlateMax[selectFloor];i++){
			wsprintf(fname,"mapRname_%df%02d",selectFloor+1,i);
			floorNameBuf[i].loadLGF(pack_gparts,fname);
		}
		alpha_time = timeGetTime();
		map_alpha = 0;
		state = map_selectin;
		return TRUE;
	}
	SelectItem();

	return TRUE;
} // MapWnd::Exec

RECT	floorPos[5][8] ={
	{{ 46,135,337,215},{280,260,170,120},{124,176,146, 94},{200,180, 54, 40},{106,168,124, 74},{ 90,176, 80, 54},{148,160, 62, 40}},
	{{ 85,131,302,140},{283,233,168,110},{221,237,114, 80},{161,202,114, 74},{208,195,140, 86},{131,151, 96, 60},{169,144,100, 60},{ 39,107,152, 82}},

	{{  0,  0,  8,  8},{368,243, 90, 62},{284,199,145, 98},{ 78,101,313,178},{126,119,224,126},{ 31, 77,158, 78}},
	{{ 44, 54,381,193},{295,166,175,101},{ 19, 45,391,205},{217,133,170, 98},{172,113,135, 75},{111, 98,139, 58},{148, 85, 83, 44},{ 44, 54,130, 61}},
	{{ 67, 50,226,114},{250,127,123, 65},{155, 91, 88, 48},{105, 45,203, 98},{103, 67, 83, 43}},
};

POINT	floorNamePos[5][3] = {
	{{432,299},{223,125},{64,232}},
	{{325,229},{ 85,236},{179,142}},
	{{318,231},{167,154}},
	{{357,262},{119,270},{235,189}},
	{{358,285},{239,220},{109,268}},
};

RECT	mapRectBig =  {46, 20,46+560, 20+420};
RECT	mapRectBig2 = {46,-20,46+560,-20+420};
RECT	mapRectSmall[5] ={
	{412,94,412+290,94+220},
	{412,84,412+290,84+220},
	{412,74,412+290,74+220},
	{412,64,412+290,64+220},
	{412,54,412+290,54+220},
};

void MapWnd::Draw()
{
	int			i,j;
	int flashTime = timeGetTime() % 4000;
	float flash = (flashTime -2000) /2000.0f;
	float ratio;
	POINT	pOffset;

	SetAlpha();
	if(map_fadein==state || map_fadeout==state){
		g_DibInf.colorBuf.BltFast(0,0, &backupWnd,NULL,FALSE);
	}
	g_DibInf.colorBuf.BltFast(0,0, &baseCBuf, NULL, TRUE);
	if(itemMax > 0){
		for(i=0;i<30;i++){
			if(i >= itemMax)break;
			itemSelect.alpha = alpha;
			pOffset.x = 12+52*(i%15);
			pOffset.y = 462+(i/15)*50;
			rect.left = 0; rect.right = 48;
			rect.top = 0; rect.bottom = 48;
			g_DibInf.colorBuf.BltFast(pOffset.x, pOffset.y, &itemSelect,&rect,TRUE);
			rect.left = 0; rect.right = 48;
			rect.top = 48*itemTable[i]; rect.bottom = rect.top +48;
			g_DibInf.colorBuf.BltFast(pOffset.x, pOffset.y, &itemList_s,&rect,TRUE);
			if(itemTable[i]==itemNum){
				if(map_itemViewin<=state && state<=map_itemchangeout){
					rect.left = 0; rect.right = 48;
					rect.top = 48; rect.bottom = 96;
					itemSelect.alpha = 0.8f*(1-map_alpha)*(1.0f -fabsf(cosf(flash*M_PI*2)));
					g_DibInf.colorBuf.BltFast(pOffset.x, pOffset.y, &itemSelect,&rect,TRUE,TRUE);
				}
			}
			if(i==tmpItemNum){
				itemSelect.alpha = alpha;
				rect.left = 0; rect.right = 48;
				rect.top = 96; rect.bottom = 144;
				g_DibInf.colorBuf.BltFast(pOffset.x, pOffset.y, &itemSelect,&rect,TRUE);
			}
		}
	}
	switch(state){
	  case map_itemchangein:
	  case map_itemchangeout:
	  case map_itemView:
		break;
	  default:
		if(map_itemViewin==state || map_itemViewout==state){
			FloorNamePrt.alpha = RoomNameBase.alpha = RoomNameLine.alpha = alpha *map_alpha;
			for(i=0;i<5;i++)RoomName[i].alpha = alpha *map_alpha;
		}
		if(floorNum>=0){
			rect.left = 0; rect.right= 118;
			rect.top = 0; rect.bottom = rect.top +26;
			g_DibInf.colorBuf.BltFast( 10, 52, &RoomNameBase, &rect, TRUE);
			g_DibInf.colorBuf.BltFast( 10, 78, &RoomNameLine, NULL, TRUE);
			rect.left = 0; rect.right= 104;
			rect.top = 52*floorNum; rect.bottom = rect.top +26;
			g_DibInf.colorBuf.BltFast(128, 52, &FloorNamePrt, &rect, TRUE);
			rect.left = 0; rect.right = 140;
			rect.top = 52*mainRoom; rect.bottom = rect.top +26;
			g_DibInf.colorBuf.BltFast(232, 52, &RoomName[floorNum], &rect, TRUE);
		}
		if(lookFloor>=0){
			rect.left = 0; rect.right= 118;
			rect.top = 26; rect.bottom = 52;
			g_DibInf.colorBuf.BltFast( 10, 82, &RoomNameBase, &rect, TRUE);
			rect.left = 0; rect.right= 104;
			rect.top = 52*lookFloor +26; rect.bottom = rect.top +26;
			g_DibInf.colorBuf.BltFast(128, 82, &FloorNamePrt, &rect, TRUE);
			rect.left = 0; rect.right = 140;
			rect.top = 52*subRoom+26; rect.bottom = rect.top +26;
			g_DibInf.colorBuf.BltFast(232, 82, &RoomName[lookFloor], &rect, TRUE);
		}
		break;
	}
	switch(state){
	  case map_fadein:
		g_DibInf.colorBuf.BltFast(142, 64, &mapSelectBuf, &mapSelectRect, TRUE);
		break;
	  case map_selectin:
		if(lastFloor>=0){
			rect.left = 0; rect.right= 366;
			rect.top = 48*lastFloor; rect.bottom = rect.top +48;
			selectFloorPrt.alpha = alpha *(1.0f-map_alpha);
			g_DibInf.colorBuf.BltFast( 10, 400, &selectFloorPrt,&rect,TRUE);
		}
		rect.left = 0; rect.right= 366;
		rect.top = 48*selectFloor; rect.bottom = rect.top +48;
		selectFloorPrt.alpha = alpha *map_alpha;
		g_DibInf.colorBuf.BltFast( 10, 400, &selectFloorPrt,&rect,TRUE);

		mapFloorBuf.alpha = alpha;
		mapSelectRect.left = 0;	mapSelectRect.right = 290;
		for(i=0;i<5;i++){
			mapSelectRect.top = i*220*3;
			if(i==selectFloor || i==lastFloor) continue;
			mapSelectRect.bottom = mapSelectRect.top +220;
			g_DibInf.colorBuf.BltFast(412, 64, &mapFloorBuf, &mapSelectRect, TRUE);
		}
		mapAllFloor.alpha = 1.0f;
		ratio = sinf( M_PI/2 *map_alpha );
		if(lastFloor != (-1)){
			ratio = 1.0f-sinf(M_PI/2*sinf(M_PI/2*map_alpha));
			if(ratio > 0.4f){
				ratio = 1.0f -ratio;
				mapSelectRect.left = 0;	mapSelectRect.right = 560;
				mapSelectRect.top = lastFloor*420;
				mapSelectRect.bottom = mapSelectRect.top +420;
				rect.left = mapRectBig.left + int((mapRectSmall[lastFloor].left-mapRectBig.left)*ratio);
				rect.right = mapRectBig.right + int((mapRectSmall[lastFloor].right-mapRectBig.right)*ratio);
				rect.top = mapRectBig.top+40*lastFloor + int((mapRectSmall[lastFloor].top-(mapRectBig.top+40*lastFloor))*ratio);
				rect.bottom = mapRectBig.bottom+40*lastFloor + int((mapRectSmall[lastFloor].bottom-(mapRectBig.bottom+40*lastFloor))*ratio);
				g_DibInf.colorBuf.Blt(&rect, &mapAllFloor, &mapSelectRect, TRUE);
			}else{
				mapSelectRect.left = 0;	mapSelectRect.right = 290;
				mapSelectRect.top = lastFloor*220*3;
				mapSelectRect.bottom = mapSelectRect.top +220;
				rect.left = 412 + int((mapRectBig2.left-412)*ratio);
				rect.right = (412+290) +int((mapRectBig2.right-(412+290))*ratio);
				rect.top = 64 + int((mapRectBig2.top+50*lastFloor-64)*ratio);
				rect.bottom = (64+220) + int((mapRectBig2.bottom+50*lastFloor-(64+220))*ratio);
				g_DibInf.colorBuf.Blt(&rect, &mapFloorBuf, &mapSelectRect, TRUE);
			}
		}
		ratio = sinf(M_PI/2*sinf(M_PI/2*map_alpha));
		if(ratio > 0.4f){
			ratio = 1.0f -ratio;
			mapSelectRect.left = 0;	mapSelectRect.right = 560;
			mapSelectRect.top = selectFloor*420;
			mapSelectRect.bottom = mapSelectRect.top +420;
			rect.left = mapRectBig.left + int((mapRectSmall[selectFloor].left-mapRectBig.left)*ratio);
			rect.right = mapRectBig.right + int((mapRectSmall[selectFloor].right-mapRectBig.right)*ratio);
			rect.top = mapRectBig.top+40*selectFloor + int((mapRectSmall[selectFloor].top-(mapRectBig.top+40*selectFloor))*ratio);
			rect.bottom = mapRectBig.bottom+40*selectFloor + int((mapRectSmall[selectFloor].bottom-(mapRectBig.bottom+40*selectFloor))*ratio);
			g_DibInf.colorBuf.Blt(&rect, &mapAllFloor, &mapSelectRect, TRUE);
		}else{
			mapSelectRect.left = 0;	mapSelectRect.right = 290;
			mapSelectRect.top = selectFloor*220*3;
			mapSelectRect.bottom = mapSelectRect.top +220;
			rect.left = 412 + int((mapRectBig2.left-412)*ratio);
			rect.right = (412+290) +int((mapRectBig2.right-(412+290))*ratio);
			rect.top = 64 + int((mapRectBig2.top+50*selectFloor-64)*ratio);
			rect.bottom = (64+220) + int((mapRectBig2.bottom+50*selectFloor-(64+220))*ratio);
			g_DibInf.colorBuf.Blt(&rect, &mapFloorBuf, &mapSelectRect, TRUE);
		}
		break;
	  case map_itemchangein:
	  case map_itemchangeout:
	  case map_itemView:
		break;
	  default:
		if(0>=map_alpha)break;
		mapSelectRect.left = 0;	mapSelectRect.right = 290;
		for(i=0;i<5;i++){
			if(i==selectFloor)continue;
			mapSelectRect.top = i*220*3;
			if(i==tmpfloorNum) mapSelectRect.top += 220;
			mapSelectRect.bottom = mapSelectRect.top +220;
			mapFloorBuf.alpha = alpha *map_alpha;
			g_DibInf.colorBuf.BltFast(412, 64, &mapFloorBuf, &mapSelectRect, TRUE);
			if(i==tmpfloorNum){
				mapSelectRect.top += 220;
				mapSelectRect.bottom = mapSelectRect.top +220;
				mapFloorBuf.alpha = alpha*map_alpha*(1.0f -fabsf(cosf(flash*M_PI*2)));
				g_DibInf.colorBuf.BltFast(412, 64, &mapFloorBuf, &mapSelectRect, TRUE);
			}
		}
		rect.left = 0; rect.right= 366;
		rect.top = 48*selectFloor; rect.bottom = rect.top +48;
		g_DibInf.colorBuf.BltFast( 10, 400, &selectFloorPrt,&rect,TRUE);
		for(i=0;i<floorMax[selectFloor]+1;i++){
			rect.left = 0; rect.right = floorPos[selectFloor][i].right;
			rect.top = 0;
			if(i>0){
				if(selectFloor==floorNum && i==(mainRoom+1))continue;	
				if(selectFloor==lookFloor && i==(subRoom+1))continue;
			}
			rect.bottom = rect.top + floorPos[selectFloor][i].bottom;
			floorCBuf[i].alpha = alpha*map_alpha;
			g_DibInf.colorBuf.BltFast(floorPos[selectFloor][i].left+46,floorPos[selectFloor][i].top+20+40*selectFloor,&floorCBuf[i],&rect,TRUE);
		}
		for(j=0;j<2;j++){
			if(j==0){
				if(mainRoom==(-1) || selectFloor!=floorNum)continue;
				i = mainRoom +1;
				rect.top = floorPos[selectFloor][i].bottom;
			}else{
				if(subRoom==(-1) || selectFloor!=lookFloor)continue;
				i = subRoom +1;
				rect.top = floorPos[selectFloor][i].bottom*3;
			}
			rect.left = 0; rect.right = floorPos[selectFloor][i].right;
			rect.bottom = rect.top + floorPos[selectFloor][i].bottom;
			floorCBuf[i].alpha = alpha*map_alpha;
			g_DibInf.colorBuf.BltFast(floorPos[selectFloor][i].left+46,floorPos[selectFloor][i].top+20+40*selectFloor,&floorCBuf[i],&rect,TRUE);
			rect.top = rect.top + floorPos[selectFloor][i].bottom;
			rect.bottom = rect.top + floorPos[selectFloor][i].bottom;
			floorCBuf[i].alpha = alpha*map_alpha*(1.0f -fabsf(cosf(flash*M_PI*2)));
			g_DibInf.colorBuf.BltFast(floorPos[selectFloor][i].left+46,floorPos[selectFloor][i].top+20+40*selectFloor,&floorCBuf[i],&rect,TRUE);
		}
		for(i=0;i<floorPlateMax[selectFloor];i++){
			floorNameBuf[i].alpha = alpha*map_alpha;
			g_DibInf.colorBuf.BltFast(floorNamePos[selectFloor][i].x,floorNamePos[selectFloor][i].y,&floorNameBuf[i],NULL,TRUE);
		}
		break;
	}
	switch(state){
	  case map_itemView: case map_itemViewin: case map_itemViewout:
		g_DibInf.colorBuf.BltFast(272, 44, &itemList[0],NULL,TRUE);
		g_DibInf.colorBuf.BltFast(108,300,&itemInfoPtr,NULL,TRUE);
		if(map_itemViewin==state || map_itemViewout==state){
			itemRet.alpha = alpha *(1.0f-map_alpha);
		}
		g_DibInf.colorBuf.BltFast(580,440,&itemRet,NULL,TRUE);
		break;
	  case map_itemchangein:
	  case map_itemchangeout:
		itemInfoPtr.alpha = map_alpha *alpha;
		g_DibInf.colorBuf.BltFast(108,300,&itemInfoPtr,NULL,TRUE);
		g_DibInf.colorBuf.BltFast(580,440,&itemRet,NULL,TRUE);
		if(map_itemchangein==state){
			itemList[1].alpha = (map_alpha +1)*0.5f;
		}else{
			itemList[1].alpha = (1.0f-map_alpha)*0.5f;
		}
		g_DibInf.colorBuf.BltFast(272, 44, &itemList[1],NULL,TRUE);
		itemList[0].alpha = 1.0f -itemList[1].alpha;
		g_DibInf.colorBuf.BltFast(272, 44, &itemList[0],NULL,TRUE);
		break;
	}
	retBtn.Draw();
}


POINT	checkBtnOffset[16] = { {138,69},{138,93},{138,200},{138,224},
{100,419},{240,419},{370,419},{510,419},
{100,443},{240,443},
{100,467},{240,467},{370,467},{510,467},
{138,503},{138,527} };

OptionMode::OptionMode()
{
	ColorBuf	tmpCBuf;
	int			i,j;
	RECT	btnRect[5];

	ZeroMemory(this,sizeof(OptionMode));

	bak_execMode = sysInf.execMode;
	changeExecMode(option_mode);	
	backupWnd.createColorBuf(WIN_SIZEX, WIN_SIZEY,24,FALSE);
	backupWnd.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
	baseCBuf.createColorBuf(WIN_SIZEX, WIN_SIZEY,24,FALSE);
	baseCBuf.BltFast(0,0,&backColorBuf,NULL,FALSE);
	blackWnd.createColorBuf(WIN_SIZEX, WIN_SIZEY,24,TRUE);
	tmpCBuf.loadLGF(pack_gparts, "optionbg");
	baseCBuf.BltFast(0,0,&tmpCBuf,NULL,TRUE);
	tmpCBuf.Release();
	tmpCBuf.loadLGF(pack_gparts, "optionbase");
	baseCBuf.BltFast(20,9,&tmpCBuf,NULL,TRUE);
	tmpCBuf.Release();
	charnamePrt.loadLGF(pack_gparts,"optionchr");
	meterPrt.loadLGF(pack_gparts,"optionMeter");
	confirmWnd.loadLGF(pack_gparts,"optionConfirm");
	yesNoWnd.loadLGF(pack_gparts,"saveYesNo");
	retBtnPrt.loadLGF(pack_gparts,"returnBtn");
	RECT	selectRect = {709,565,709+86,565+25};
	for(i=0;i<5;i++){
		btnRect[i].left = 0; btnRect[i].right = 86;
		btnRect[i].top = 25*i; btnRect[i].bottom = btnRect[i].top +25;
	}
	retBtn.Create(3,&retBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);
	initBtnPrt.loadLGF(pack_gparts,"optionInit");
	selectRect.left = 19;	selectRect.right = selectRect.left +132;
	selectRect.top = 567;	selectRect.bottom = selectRect.top +25;
	for(i=0;i<5;i++){
		btnRect[i].left = 0; btnRect[i].right = 132;
		btnRect[i].top = 25*i; btnRect[i].bottom = btnRect[i].top +25;
	}
	initBtn.Create(3,&initBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);
	screenBtnPrt.loadLGF(pack_gparts, "optionScreen");
	for(j=0;j<2;j++){
		selectRect.left = 276+184*j;	selectRect.right = selectRect.left +182;
		selectRect.top = 38;	selectRect.bottom = selectRect.top +20;
		for(i=0;i<2;i++){
			btnRect[i].left = 0; btnRect[i].right = 182;
			if(j==0){
				btnRect[i].top = 20*i; btnRect[i].bottom = btnRect[i].top +20;
			}else{
				btnRect[i].top = 40*i; btnRect[i].bottom = btnRect[i].top +20;
			}
		}
		screenBtn[j].Create(255,&screenBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);
	}
	if(0==sysInf.full_screen){
		screenBtn[0].SetState(btn_push);
	}else{
		screenBtn[1].SetState(btn_push);
	}
	voiceSetBtnPrt.loadLGF(pack_gparts, "optionVoiceSet");
	for(j=0;j<4;j++){
		selectRect.left = 276+92*j;	selectRect.right = selectRect.left +90;
		selectRect.top = 382;	selectRect.bottom = selectRect.top +20;
		for(i=0;i<2;i++){
			btnRect[i].left = 0; btnRect[i].right = 90;
			btnRect[i].top = (20*(j+1))*i; btnRect[i].bottom = btnRect[i].top +20;
		}
		voiceSetBtn[j].Create(255,&voiceSetBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);
		if(j==sysInf.voiceSelect){
			voiceSetBtn[j].SetState(btn_push);
		}
	}
	checkBtnPrt.loadLGF(pack_gparts,"optionCheck");
	for(j=0;j<16;j++){
		selectRect.left = checkBtnOffset[j].x;	selectRect.right = selectRect.left +20;
		selectRect.top = checkBtnOffset[j].y;	selectRect.bottom = selectRect.top +20;
		for(i=0;i<2;i++){
			btnRect[i].left = 0; btnRect[i].right = 20;
			btnRect[i].top = 20*i; btnRect[i].bottom = btnRect[i].top +20;
		}
		checkBtn[j].Create(254,&checkBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);
	}
	if(sysInf.bBackStop)checkBtn[0].SetState(btn_push);
	if(sysInf.bMsgWndCls)checkBtn[1].SetState(btn_push);
	if(sysInf.bAllSkip)checkBtn[2].SetState(btn_push);
	if(sysInf.bGEffectSkip)checkBtn[3].SetState(btn_push);
	for(i=0;i<12;i++){
		if(sysInf.voiceFlag & (1<<i))checkBtn[i+4].SetState(btn_push);
	}
	selectRect.top = 562; selectRect.bottom = selectRect.top +25;
	for(j=0;j<2;j++){
		if(j==0){
			selectRect.left = 490; selectRect.right = selectRect.left +86;
		}else{
			selectRect.left = 586; selectRect.right = selectRect.left +86;
		}
		for(i=0;i<5;i++){
			btnRect[i].left = j*86;	btnRect[i].right = btnRect[i].left +86;
			btnRect[i].top = i*25;	btnRect[i].bottom = btnRect[i].top +25;
		}
		yesnoBtn[j].Create(3,&yesNoWnd,&selectRect,btnRect,&g_DibInf.colorBuf);
	}
	selectBtnNum = 0xff;
	alpha = 0;
	state = 0;
	fadeTime = 0;
}

void ScreenChange(BYTE full_screen, BOOL bUseRrate);
extern int	DispFreqMax;

BOOL OptionMode::Exec()
{
	int		i,j,k;
	WORD	tmpFlag;
	BYTE	oldSelect = selectBtnNum;

	switch(state){
	  case 0:	
		if(0==fadeTime){
			fadeTime = timeGetTime() +500;
		}else{
			alpha = 1.0f -(fadeTime -(int)timeGetTime())/500.0f;
			if(alpha >= 1.0f){
				alpha = 1.0f;
				fadeTime = 0;
				state = 1;
			}
			retBtn.selectRect.top = 565; retBtn.selectRect.bottom = retBtn.selectRect.top+25;
			retBtn.selectRect.left = LONG(709 +(1.0f- alpha)*(47+40*4));
			retBtn.selectRect.right = retBtn.selectRect.left +86;
		}
		return TRUE;
	  case 2:
		if(0==fadeTime){
			fadeTime = timeGetTime() +500;
		}else{
			alpha = (fadeTime -(int)timeGetTime())/500.0f;
			if(alpha <= 0){
				alpha = 0;
			}else{
				retBtn.selectRect.top = 457+4*27;
				retBtn.selectRect.top += LONG((1.0f-alpha)*40);
				retBtn.selectRect.bottom = retBtn.selectRect.top +25;
			}
			if(0==alpha){
				changeExecMode(bak_execMode);
				return FALSE;
			}
		}
		return TRUE;
	  case 3:
		alpha = (fadeTime -(int)timeGetTime())/500.0f;
		if(alpha <= 0.5f){
			alpha = 0.5f;
			state = 4;
		}
		return TRUE;
	  case 4:
		for(i=0;i<2;i++){
			if(yesnoBtn[i].CheckState(&sysInf.msPoint)){
				keyState.push_cancel = 1;
				if(i==0){
					PlayEffectNum(evtHandle[11],SE_PUSH);
					sysInf.bBackStop = 0;
					checkBtn[0].SetState(btn_normal);
					sysInf.bMsgWndCls = 1;
					checkBtn[1].SetState(btn_push);
					sysInf.msgSpeed = 5;
					sysInf.autoMsg = 5;
					sysInf.bAllSkip = 0;
					checkBtn[2].SetState(btn_normal);
					sysInf.bGEffectSkip = 1;
					checkBtn[3].SetState(btn_push);
					sysInf.bgmVolume = 7;
					if(lpSoundDS)lpSoundDS->bgmVolume = sysInf.bgmVolume;
					sysInf.voiceVolume = 8;
					if(lpSoundDS)lpSoundDS->voiceVolume = sysInf.voiceVolume;
					sysInf.seVolume = 8;
					if(lpSoundDS)lpSoundDS->seVolume = sysInf.seVolume;
					sysInf.voiceSelect = 0;
					voiceSetBtn[0].SetState(btn_push);
					for(j=1;j<4;j++){
						voiceSetBtn[j].SetState(btn_normal);
					}
					sysInf.voiceFlag = 0x3ff;
					for(j=0;j<10;j++){
						checkBtn[j+4].SetState(btn_push);
					}
					checkBtn[14].SetState(btn_normal);
					checkBtn[15].SetState(btn_normal);
					SetVolumeLevelG(bgmHandle,sysInf.bgmVolume);
					SetVolumeLevelG(voiceHandle,sysInf.voiceVolume);
					for(k=0;k<hEvtSndMax;k++)SetVolumeLevelG(evtHandle[k],sysInf.seVolume);
					fadeTime = timeGetTime() +1500;
					state = 6;
					return TRUE;
				}
			}
		}
		if(keyState.push_cancel){
			PlayEffectNum(evtHandle[11],SE_CANCEL);
			fadeTime = timeGetTime() +500;
			state = 5;
		}
		return TRUE;
	  case 6:	
		if(timeGetTime() > fadeTime){
			fadeTime = timeGetTime() +500;
			state = 7;
		}
		return TRUE;
	  case 5:	
	  case 7:
		alpha = 1.5f -(fadeTime -(int)timeGetTime())/500.0f;
		if(alpha >= 1.0f){
			alpha = 1.0f;
			state = 1;
		}
		return TRUE;
	}
	selectBtnNum = 0xff;
	meterCnt = 0;
	for(i=0;i<5;i++){
		rect.left = 326;	rect.right = rect.left + 260;
		if(i<2) rect.top = 141 +i*32;
		else rect.top = 273 +(i-2)*32;
		rect.bottom = rect.top +16;
		if(myPtInRect(&rect,&sysInf.msPoint)){
			meterCnt = i+1;
			if(0==keyState.data_action)break;
			j = ((sysInf.msPoint.x -326)/13 +1) /2;
			if(j>10)j=10;
			int		oldValue;
			switch(i){
			case 0:
				oldValue = sysInf.msgSpeed;
				sysInf.msgSpeed = j;
				break;
			case 1:
				oldValue = sysInf.autoMsg;
				sysInf.autoMsg = j;
				break;
			case 2:
				oldValue = sysInf.bgmVolume;
				sysInf.bgmVolume = j;
				if(lpSoundDS){
					lpSoundDS->bgmVolume = sysInf.bgmVolume;
					SetVolumeLevelG(bgmHandle,sysInf.bgmVolume);
				}
				break;
			case 3:
				oldValue = sysInf.voiceVolume;
				sysInf.voiceVolume = j;
				if(lpSoundDS){
					lpSoundDS->voiceVolume = sysInf.voiceVolume;
					SetVolumeLevelG(voiceHandle,sysInf.voiceVolume);
				}
				break;
			case 4:
				oldValue = sysInf.seVolume;
				sysInf.seVolume = j;
				if(lpSoundDS){
					lpSoundDS->seVolume = sysInf.seVolume;
					for(int k=0;k<hEvtSndMax;k++)SetVolumeLevelG(evtHandle[k],sysInf.seVolume);
				}
				break;
			}
			if(oldValue != j)PlayEffectNum(evtHandle[11],SE_SELECT);
			break;
		}
	}
	for(i=0;i<2;i++){
		if(screenBtn[i].CheckCheckBtnState(&sysInf.msPoint)){
			if(i==0)screenBtn[1].SetState(btn_normal);
			else screenBtn[0].SetState(btn_normal);
			if(screenBtn[i].GetState() & btn_push){
				PlayEffectNum(evtHandle[11],SE_PUSH);
				if(i==0){
					ScreenChange(0,0);
					if(CheckOSw2k()){
						for(i=0;i<DispFreqMax;i++){
							EnableMenuItem( sysInf.hMenu, ID_SUBMENU+i,  MF_ENABLED  );
							CheckMenuItem( sysInf.hMenu, ID_SUBMENU+i,   MF_UNCHECKED );
						}
					}else{
						EnableMenuItem( sysInf.hMenu, ID_DISPFULL,  MF_ENABLED );
						CheckMenuItem( sysInf.hMenu, ID_DISPFULL,   MF_UNCHECKED );	
					}
					EnableMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_GRAYED  );
					CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW,  MF_CHECKED   );
				}else{
					ScreenChange(1,0);
					EnableMenuItem( sysInf.hMenu,ID_DISPFULL,   MF_GRAYED  );
					CheckMenuItem( sysInf.hMenu, ID_DISPFULL,   MF_CHECKED );	
					EnableMenuItem( sysInf.hMenu,ID_DISPWINDOW, MF_ENABLED );
					CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_UNCHECKED );
				}
			}
			return TRUE;
		}
	}
	for(i=0;i<4;i++){
		if(voiceSetBtn[i].CheckCheckBtnState(&sysInf.msPoint)){
			for(j=0;j<4;j++){
				if(j!=i)voiceSetBtn[j].SetState(btn_normal);
			}
			sysInf.voiceSelect = i;
			switch(sysInf.voiceSelect){
			case 0:
				for(j=0;j<10;j++){
					sysInf.voiceFlag |= (1<<j);
					checkBtn[j+4].SetState(btn_push);
				}
				break;
			case 1:
				for(j=0;j<10;j++){
					tmpFlag = (1<<j);
					tmpFlag = ~tmpFlag;
					sysInf.voiceFlag &= tmpFlag;
					checkBtn[j+4].SetState(btn_normal);
				}
				break;
			case 2:
				for(j=0;j<6;j++){
					sysInf.voiceFlag |= (1<<j);
					checkBtn[j+4].SetState(btn_push);
				}
				for(j=6;j<10;j++){
					tmpFlag = (1<<j);
					tmpFlag = ~tmpFlag;
					sysInf.voiceFlag &= tmpFlag;
					checkBtn[j+4].SetState(btn_normal);
				}
				break;
			}
			PlayEffectNum(evtHandle[11],SE_PUSH);
			return TRUE;
		}
	}
	for(i=0;i<16;i++){
		if(checkBtn[i].CheckCheckBtnState(&sysInf.msPoint)){
			BOOL sw = off;
			if(checkBtn[i].GetState() & btn_push) sw = on;
			switch(i){
			case 0:
				sysInf.bBackStop = sw;
				break;
			case 1:
				sysInf.bMsgWndCls = sw;
				break;
			case 2:
				sysInf.bAllSkip = sw;
				break;
			case 3:
				sysInf.bGEffectSkip = sw;
				break;
			default:
				tmpFlag = 1<<(i-4);
				if(on==sw){
					sysInf.voiceFlag |= tmpFlag;
				}else{
					tmpFlag = ~tmpFlag;
					sysInf.voiceFlag &= tmpFlag;
				}
				if(i>=4 && i<=13){
					sysInf.voiceSelect = 3;
					voiceSetBtn[0].SetState(btn_normal);
					voiceSetBtn[1].SetState(btn_normal);
					voiceSetBtn[2].SetState(btn_normal);
					voiceSetBtn[3].SetState(btn_push);
				}
				break;
			}
			PlayEffectNum(evtHandle[11],SE_PUSH);
			return TRUE;
		}
	}
	if(initBtn.CheckState(&sysInf.msPoint)){
		state = 3;
		PlayEffectNum(evtHandle[11],SE_PUSH);
		fadeTime = timeGetTime() +500;
		return TRUE;
	}else if(initBtn.GetState()){
		selectBtnNum = 1;
	}
	if(keyState.push_cancel || retBtn.CheckState(&sysInf.msPoint)){ 
		PlayEffectNum(evtHandle[11],SE_CANCEL);		
		state = 2;
	}else if(retBtn.GetState()){
		selectBtnNum = 2;
	}
	if(selectBtnNum!=0xff && selectBtnNum!=oldSelect){
			PlayEffectNum(evtHandle[11],SE_SELECT);
	}
	return TRUE;
}

void OptionMode::Draw()
{
	int		i;
	POINT	offset;
	SetAlpha();
	if(0==state || 2==state){
		g_DibInf.colorBuf.BltFast(0,0, &backupWnd,NULL,FALSE);
	}else if(alpha < 1.0f){
		g_DibInf.colorBuf.BltFast(0,0, &blackWnd,NULL,FALSE);		
	}
	g_DibInf.colorBuf.BltFast(0,0, &baseCBuf,NULL,TRUE);
	if(meterCnt>0){
		rect.left = 0; rect.right = 260;
		rect.top = 0; rect.bottom = 16;
		if(meterCnt < 3){
			i = 141+32*(meterCnt-1);
		}else{
			i = 273+32*(meterCnt-3);
		}
		g_DibInf.colorBuf.BltFast(326,i, &meterPrt,&rect,TRUE);
	}
	rect.left = 0; rect.right = 260*sysInf.msgSpeed/10;
	rect.top = 16; rect.bottom = 32;
	g_DibInf.colorBuf.BltFast(326,141, &meterPrt,&rect,TRUE);
	rect.left = 0; rect.right = 260*sysInf.autoMsg/10;
	g_DibInf.colorBuf.BltFast(326,173, &meterPrt,&rect,TRUE);
	rect.left = 0; rect.right = 260*sysInf.bgmVolume/10;
	rect.top = 32; rect.bottom = 48;
	g_DibInf.colorBuf.BltFast(326,273, &meterPrt,&rect,TRUE);
	rect.left = 0; rect.right = 260*sysInf.voiceVolume/10;
	g_DibInf.colorBuf.BltFast(326,305, &meterPrt,&rect,TRUE);
	rect.left = 0; rect.right = 260*sysInf.seVolume/10;
	g_DibInf.colorBuf.BltFast(326,337, &meterPrt,&rect,TRUE);
	for(i=0;i<10;i++){
		if(i<6){
			offset.x = (i%4);
			offset.y = (i/4);
		}else{
			offset.x = i - 6;
			offset.y = 2;
		}
		rect.left = offset.x *128; rect.right = rect.left +128;
		rect.top = offset.y *24; rect.bottom = rect.top +24;
		charnamePrt.alpha = alpha*0.5f;
		if(sysInf.voiceFlag & (1<<i)){
			charnamePrt.alpha = alpha;
		}
		g_DibInf.colorBuf.BltFast(122+offset.x*128,417+offset.y*24, &charnamePrt,&rect,TRUE);
	}
	retBtn.Draw();
	initBtn.Draw();
	for(i=0;i<2;i++)screenBtn[i].Draw();
	for(i=0;i<4;i++)voiceSetBtn[i].Draw();
	for(i=0;i<16;i++)checkBtn[i].Draw();
	if(3<=state){
		rect.left = 0; rect.right = 532;
		rect.top = 0; rect.bottom = 40;
		if(state>=6){
			rect.top = 40; rect.bottom = 80;
		}
		g_DibInf.colorBuf.BltFast(156,554+int((alpha-0.5f)*92), &confirmWnd,&rect,TRUE);
		if(state<=5){
			for(i=0;i<2;i++){
				yesnoBtn[i].selectRect.top = 562 +int((alpha-0.5f)*92); yesnoBtn[i].selectRect.bottom = yesnoBtn[i].selectRect.top +25;
				yesnoBtn[i].Draw();
			}
		}
	}
}

BYTE musicConvert[21] = {
	20, 0, 1, 6, 7,
	 3,14, 8,10, 2,
	15,11,12,13,16,
	 5, 4,17,19,18,21};

MusicModeWnd::MusicModeWnd()
{
	int		i,j;

	lpSoundDS->FadeOut(bgmHandle);
	ZeroMemory(this,sizeof(MusicModeWnd));

	bak_mode = sysInf.execMode;
	baseCBuf.loadLGF(pack_gparts,"musicBase");
	musicTitlePrt.loadLGF(pack_gparts,"musicTitle");
	musicBtnPrt.loadLGF(pack_gparts,"musicButton");
	musicTitle2.loadLGF(pack_gparts,"musicTitle2");
	musicSelect.loadLGF(pack_gparts,"musicSelect");
	musicInfoPrt[0].loadLGF(pack_gparts,"musicInfo1");
	musicInfoPrt[1].loadLGF(pack_gparts,"musicInfo2");
	retBtnPrt.loadLGF(pack_gparts,"returnBtn");
	backupWnd.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	backupWnd.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
	RECT	selectRect = {709,565,709+86,565+25};
	RECT	btnRect[5];
	for(i=0;i<5;i++){
		btnRect[i].left = 0; btnRect[i].right = 86;
		btnRect[i].top = 25*i; btnRect[i].bottom = btnRect[i].top +25;
	}
	retBtn.Create(3,&retBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);
	for(i=0;i<21;i++){
		BYTE m = musicConvert[i];
		BOOL bOpen = ESC_GetGameFlag( 200+m );
		if(m > 9) m--;
		selectRect.left = 81 +336*(i/11);selectRect.right = selectRect.left +264;
		selectRect.top = 83 +42*(i%11); selectRect.bottom = selectRect.top +20;
		for(j=0;j<3;j++){
			btnRect[j].left = 0; btnRect[j].right = btnRect[j].left +264;
			btnRect[j].top = 20*(m*3) +20*j; btnRect[j].bottom = btnRect[j].top +20;
		}
		musicBtn[i].Create(1,&musicTitlePrt,&selectRect,btnRect,&g_DibInf.colorBuf);
		if(0==bOpen)musicBtn[i].SetState(btn_disable);
	}
	selectNum = stopNum = oldMusic = btnSelect = 0xff;
	alpha = 0;
	SetAlpha();
	alpha_time = timeGetTime();
	changeExecMode(	music_mode );
}

BOOL MusicModeWnd::Exec()
{
	int		i, now_time = (int)timeGetTime() - alpha_time;
	int		oldSelect = selectNum, oldBtn = btnSelect;;
	switch(state){
	  case 0:	
		alpha = now_time / 500.0f;
		if(alpha >= 1.0f){
			alpha = 1.0f;
			state = 1;
		}
		SetAlpha();
		retBtn.selectRect.top = 565; retBtn.selectRect.bottom = retBtn.selectRect.top+25;
		retBtn.selectRect.left = LONG(709 +(1.0f- alpha)*(47+40*4));
		retBtn.selectRect.right = retBtn.selectRect.left +86;
		return TRUE;
	  case 2:	
		alpha = 1.0f -now_time / 500.0f;
		if(alpha < 0){
			lpSoundDS->ChangeMusic(bgmHandle,2,TRUE,0);
			changeExecMode(bak_mode);	
			return FALSE;
		}
		SetAlpha();
		retBtnPrt.alpha = alpha;
		retBtn.selectRect.top = 457+4*27;
		retBtn.selectRect.top += LONG((1.0f-alpha)*40);
		retBtn.selectRect.bottom = retBtn.selectRect.top +25;
		return TRUE;
	}
	if(keyState.push_cancel){
		PlayEffectNum(evtHandle[11],SE_CANCEL);		
		alpha_time = timeGetTime();
		state = 2;
		return TRUE;
	}
	if(retBtn.CheckState(&sysInf.msPoint)){
		PlayEffectNum(evtHandle[11],SE_CANCEL);		
		alpha_time = timeGetTime();
		state = 2;
		return TRUE;
	}else if(retBtn.GetState()){
		btnSelect = 1;
		if(oldBtn != btnSelect) PlayEffectNum(evtHandle[11],SE_SELECT);
	}
	for(i=0;i<21;i++){
		if(musicBtn[i].CheckState(&sysInf.msPoint)){
			int m = musicConvert[i];
			if(PCM_STOP==lpSoundDS->GetStatus(bgmHandle) || m!=lpSoundDS->GetMusicFileNo()){
				oldMusic = lpSoundDS->GetMusicFileNo();
				btn_time = timeGetTime();
				PlayEffectNum(evtHandle[11],SE_PUSH);
				lpSoundDS->ChangeMusic(bgmHandle,m,TRUE,0);
			}else{
				stopNum = i;
				btn_time = timeGetTime() +300;
				PlayEffectNum(evtHandle[11],SE_CANCEL);		
				lpSoundDS->FadeOut(bgmHandle);
			}
		}else if(btn_onCursor==musicBtn[i].GetState()){
			selectNum = i;
			if(selectNum!=oldSelect){
				PlayEffectNum(evtHandle[11],SE_SELECT);		
			}
		}
	}
	return TRUE;
}

void MusicModeWnd::Draw()
{
	int flashTime = timeGetTime() % 4000;
	float flash = (flashTime -2000) /2000.0f;

	int		i,j,m,om;

	if(0==state || 2==state){
		g_DibInf.colorBuf.BltFast(0,0, &backupWnd,NULL,FALSE);
	}
	g_DibInf.colorBuf.BltFast(0,0,&baseCBuf,NULL,TRUE);
	retBtn.Draw();
	for(i=0;i<21;i++){
		musicBtn[i].Draw();
	}
	m = lpSoundDS->GetMusicFileNo();
	if(m>=0){
		for(i=0;i<21;i++){
			if(musicConvert[i]==m)break;
		}
		if(m > 9) m--;
		rect.left = 0; rect.right = 264;
		rect.top = 20*(m*3+1); rect.bottom = rect.top +20;
		g_DibInf.colorBuf.BltFast(81 +336*(i/11), 83 +42*(i%11), &musicTitlePrt,&rect,TRUE);

		float fade = (timeGetTime() -btn_time) /500.0f;
		if(fade > 1.0f){
			fade = 1.0f;
			oldMusic = 0xff;
		}
		fade *= alpha;

		if(oldMusic != 0xff){
			musicBtnPrt.alpha = musicTitle2.alpha = musicInfoPrt[0].alpha = musicInfoPrt[1].alpha = alpha -fade;
			for(j=0;j<21;j++){
				if(musicConvert[j]==oldMusic)break;
			}
			om = oldMusic;
			if(om > 9) om --;
			rect.left = 0; rect.right = 20;
			rect.top = 0; rect.bottom = 20;
			g_DibInf.colorBuf.BltFast(51 +336*(j/11), 83 +42*(j%11), &musicBtnPrt,&rect,TRUE);
			if(om<19){
				rect.left = 0; rect.right = 180;
				rect.top = 24*om; rect.bottom = rect.top +24;
				g_DibInf.colorBuf.BltFast(514,552,&musicInfoPrt[0],&rect,TRUE);
			}else{
				rect.left = 0; rect.right = 540;
				rect.top = 24*(om-19); rect.bottom = rect.top +24;
				g_DibInf.colorBuf.BltFast(154,552,&musicInfoPrt[1],&rect,TRUE);
			}
			rect.left = 0; rect.right = 230;
			rect.top = 24*om; rect.bottom = rect.top +24;
			g_DibInf.colorBuf.BltFast(26,24,&musicTitle2,&rect,TRUE);
		}
		musicBtnPrt.alpha = musicTitle2.alpha = musicInfoPrt[0].alpha = musicInfoPrt[1].alpha = fade;
		rect.left = 0; rect.right = 20;
		rect.top = 0; rect.bottom = 20;
		g_DibInf.colorBuf.BltFast(51 +336*(i/11), 83 +42*(i%11), &musicBtnPrt,&rect,TRUE);
		if(m<19){
			rect.left = 0; rect.right = 180;
			rect.top = 24*m; rect.bottom = rect.top +24;
			g_DibInf.colorBuf.BltFast(514,552,&musicInfoPrt[0],&rect,TRUE);
		}else{
			rect.left = 0; rect.right = 540;
			rect.top = 24*(m-19); rect.bottom = rect.top +24;
			g_DibInf.colorBuf.BltFast(154,552,&musicInfoPrt[1],&rect,TRUE);
		}
		rect.left = 0; rect.right = 230;
		rect.top = 24*m; rect.bottom = rect.top +24;
		g_DibInf.colorBuf.BltFast(26,24,&musicTitle2,&rect,TRUE);
	}else if(stopNum != 0xff){	
		rect.left = 0; rect.right = 20;
		rect.top = 20; rect.bottom = 40;
		musicBtnPrt.alpha = (btn_time -(int)timeGetTime())/300.0f *alpha;
		if(musicBtnPrt.alpha > 0){
			g_DibInf.colorBuf.BltFast(51 +336*(stopNum/11), 83 +42*(stopNum%11), &musicBtnPrt,&rect,TRUE);
		}else{
			stopNum = 0xff;
		}
	}
	if(selectNum!=0xff){
		musicSelect.alpha = (1.0f -fabsf(cosf(flash*M_PI*2))/2) *alpha;
		g_DibInf.colorBuf.BltFast(40 +336*(selectNum/11), 76 +42*(selectNum%11), &musicSelect,NULL,TRUE);

	}
}

extern BYTE	CgFlagBuf[ 5000/8 ];

BYTE	vMax[7] = {16, 18, 8, 10, 5, 5, 11};

BYTE	cgList[7][20*3] = {
{ 0,2,0,  1,3,0,  2,3,1,  3,6,0,  4,2,0,
  6,2,0, 48,2,0, 50,2,0, 22,1,0, 23,4,0,
 24,5,0, 25,3,0, 26,2,0, 27,3,0, 29,4,0,
 71,1,1 },
{17,3,0, 70,1,0, 72,1,0, 51,3,0, 43,4,0,
 60,2,0,  7,1,0,  8,2,1, 11,2,1, 12,4,0,
 13,3,0, 30,3,0, 61,1,1, 62,4,1, 53,4,1,
 54,1,1, 55,2,1, 68,2,1 }, 
{34,4,0, 35,1,0, 44,3,0, 64,1,0,  9,1,0,
 45,2,0, 46,2,0, 65,1,1,}, 
{18,1,0, 19,11,0, 20,4,0, 38,5,0, 41,3,0,
 42,3,0, 36, 4,0, 37,3,0, 47,2,0, 66,3,1, },
{31,6,0, 32,5,0, 33,4,0, 39,1,0, 67,1,1, },
{15,1,0,  5,2,0, 49,2,0, 21,5,0, 28,2,0,}, 
{69,1,1, 10,1,0, 40,5,0, 16,1,0, 63,1,1,
 14,2,1, 52,1,1, 56,1,1, 57,2,1, 58,1,1,
 59,3,1, }, 
};

BYTE sceneListMax[7] = {8,11,7,5,4,4,2};
BYTE sceneThumnail[7][12] = {
	{30,31,32,33,34,35,36,37},
	{40,41,42,43,44,45,46,47,34,48,63},
	{50,51,52,53,54,55,56,},
	{60,61,62,63,64,},
	{70,71,72,64,},
	{80,33,35,81,},
	{90,91} };
int sceneScript[7][12] = {
	{90000,90010,90020,90030,90040,90050,90060,90070,},
	{91000,91010,91020,91030,91040,91050,91060,91070,90040,91080,93030},
	{92000,92010,92020,92030,92040,92050,92060,},
	{93000,93010,93020,93030,93040,},
	{94000,94010,94020,93040,},
	{95000,90030,90050,95010,},
	{96000,96010,} };

CG_ModeWnd::CG_ModeWnd(exec_mode e_mode)
{
	int		i,j;
	ZeroMemory(this,sizeof(CG_ModeWnd));
	mode = e_mode;
	bak_mode = sysInf.execMode;
	state = cgmode_allfadein;
	baseCBuf.loadLGF(pack_gparts,"cgm_base");
	titlePrt.loadLGF(pack_gparts,"cgm_title");
	thumbnailPrt[0].loadLGF(pack_gparts,"cgm_thumbnail");
	thumbnailPrt[13].loadLGF(pack_gparts,"cgm_waku");
	numParts.loadLGF(pack_gparts,"saveFont");
	retBtnPrt.loadLGF(pack_gparts,"returnBtn");
	pagebutton.loadLGF(pack_gparts,"SavePageBtn");
	char fname[10] = "chrsel00";
	for(i=0;i<7;i++){
		charSelectPrt[i].loadLGF(pack_gparts,fname);
		fname[7] ++;
	}
	charnamePrt.loadLGF(pack_gparts,"cgm_charName");
	charSelect.loadLGF(pack_gparts,"cgm_charSelect");
	sabunNumPrt.loadLGF(pack_gparts,"cgm_sabunNum");
	mainWnd.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	cgWndOld.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	backupWnd.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	backupWnd.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
	RECT	selectRect = {709,565,709+86,565+25};
	RECT	btnRect[7];
	for(i=0;i<5;i++){
		btnRect[i].left = 0; btnRect[i].right = 86;
		btnRect[i].top = 25*i; btnRect[i].bottom = btnRect[i].top +25;
	}
	retBtn.Create(3,&retBtnPrt,&selectRect,btnRect,&g_DibInf.colorBuf);
	selectRect.left = 22;	selectRect.right = selectRect.left +40;
	selectRect.top = 528;	selectRect.bottom = selectRect.top +40;
	for(i=0;i<5;i++){
		btnRect[i].left = 0;		btnRect[i].right = btnRect[i].left +40;
		btnRect[i].top = i*40;	btnRect[i].bottom = btnRect[i].top +40;
	}
	btn[0].Create(3,&pagebutton,&selectRect,btnRect,&g_DibInf.colorBuf);
	selectRect.left = 190;	selectRect.right = selectRect.left +40;
	selectRect.top = 528;	selectRect.bottom = selectRect.top +40;
	for(i=0;i<5;i++){
		btnRect[i].left = 40;	btnRect[i].right = btnRect[i].left +40;
		btnRect[i].top = i*40;	btnRect[i].bottom = btnRect[i].top +40;
	}
	btn[1].Create(3,&pagebutton,&selectRect,btnRect,&g_DibInf.colorBuf);
	selectNum = btnSelect = 0xff;
	charNum = 0xff;
	prevChar = 0xfe;
	alpha = 0;
	SetAlpha();
	if(lpSoundDS)lpSoundDS->ChangeMusic(bgmHandle,2,TRUE,0);
	alpha_time = (int)timeGetTime();
	changeExecMode(mode);
} // CG_ModeWnd::CG_ModeWnd

void CG_ModeWnd::CreateThumbnail()
{
	char		workBuf[32];
	int			i, j, k;
	int thumbnailMax, thumbnailNum, thumbnailOffsetX;
;
	ColorBuf	tmpCBuf;

	ZeroMemory(bCgOpen,12);
	mainWnd.BltFast(0,0,&baseCBuf,NULL,FALSE);
	tmpCBuf.loadLGF(pack_gparts,"cgm_sideTitle");
	rect.top = 0; rect.bottom = 544;
	rect.left = 80; rect.right = 160;
	if(cg_look_mode==mode){
		rect.left = 0; rect.right = 80;
	}
	mainWnd.BltFast(712, 14, &tmpCBuf,&rect,FALSE);
	tmpCBuf.Release();
	rect.left = 0; rect.right = 334;
	rect.top = 76*charNum; rect.bottom = rect.top +76;
	g_DibInf.colorBuf.BltFast(22,12,&charnamePrt,&rect,FALSE);

	for(i=0;i<12;i++){
		if(offsetNum+i >= selectMax)break;
		rect.left = 0; rect.right = 166;
		rect.top = 126*i; rect.bottom = rect.top +126;
		mainWnd.BltFast(22+170*(i%4), 102+140*(i/4),&thumbnailPrt[0],&rect,FALSE);
		if(cg_look_mode==mode){
			thumbnailMax = cgList[charNum][(offsetNum+i)*3+1];
			thumbnailNum = 0;
			k = 0;
			for(j=0;j<thumbnailMax;j++){
				if(AVG_GetCgFlag( cgList[charNum][(offsetNum+i)*3], j)){
					if(0==thumbnailNum) k = j;
					thumbnailNum ++;
				}
			}
			if(0==thumbnailNum){
				bCgOpen[i] = FALSE;
				continue;
			}
			bCgOpen[i] = TRUE;
			if(cgList[charNum][(offsetNum+i)*3+2]==1){
				wsprintf(workBuf,"sv%04d%02d",cgList[charNum][(offsetNum+i)*3], k);
			}else{
				wsprintf(workBuf,"sh%04d%02d",cgList[charNum][(offsetNum+i)*3], k);
			}
			thumbnailOffsetX = 145;
		}else{
			if(FALSE==ESC_GetGameFlag( sceneThumnail[charNum][offsetNum+i] )){
				bCgOpen[i] = FALSE;
				continue;
			}
			bCgOpen[i] = TRUE;
			wsprintf(workBuf,"ss%03d",sceneThumnail[charNum][offsetNum+i]);
		}
		thumbnailPrt[i+1].loadLGF(pack_gparts,workBuf);
		if(cg_look_mode==mode && thumbnailNum>0){
			rect.top = 0; rect.bottom = 10;
			if(thumbnailNum==thumbnailMax){ rect.top = 10; rect.bottom = 20;}
			rect.left = 8*(thumbnailMax%10); rect.right = rect.left +8;
			thumbnailPrt[i+1].BltFast(thumbnailOffsetX, 103,&sabunNumPrt,&rect,TRUE);
			thumbnailOffsetX -= 8;
			if(thumbnailMax >= 10){
				rect.left = 8*(thumbnailMax/10); rect.right = rect.left +8;
				thumbnailPrt[i+1].BltFast(thumbnailOffsetX, 103,&sabunNumPrt,&rect,TRUE);
				thumbnailOffsetX -= 8;
			}
			rect.left = 80; rect.right = 88;
			thumbnailPrt[i+1].BltFast(thumbnailOffsetX, 103,&sabunNumPrt,&rect,TRUE);
			thumbnailOffsetX -= 8;
			rect.left = 8*(thumbnailNum%10); rect.right = rect.left +8;
			thumbnailPrt[i+1].BltFast(thumbnailOffsetX, 103,&sabunNumPrt,&rect,TRUE);
			thumbnailOffsetX -= 8;
			if(thumbnailNum >= 10){
				rect.left = 8*(thumbnailMax/10); rect.right = rect.left +8;
				thumbnailPrt[i+1].BltFast(thumbnailOffsetX, 103,&sabunNumPrt,&rect,TRUE);
				thumbnailOffsetX -= 8;
			}
		}
		thumbnailPrt[i+1].alpha = 0.5f;
		mainWnd.BrightBlt(25+170*(i%4),105+140*(i/4),&thumbnailPrt[i+1],NULL,64,64,64);
		mainWnd.BltFast(22+170*(i%4), 102+140*(i/4),&thumbnailPrt[13],NULL,FALSE);
	}
	if(pageMax > 1){	
		tmpCBuf.loadLGF(pack_gparts,"cgm_pagebase");
		mainWnd.BltFast(22, 518, &tmpCBuf,NULL,FALSE);
		tmpCBuf.Release();
		rect.top = 48; rect.bottom = rect.top +18;
		rect.left = (pageMax-1)*20; rect.right = rect.left +20;
		mainWnd.BltFast(148 ,540,&numParts,&rect,TRUE);
	}
} // CG_ModeWnd::CreateThumbnail

RECT charSelectRect[7] = {
	{ 278,  12, 278+522,  12+588},
	{   0, 312,   0+306, 312+288},
	{  82, 220,  82+374, 220+380},
	{ 126,   0, 126+590,   0+600},
	{  36,   0,  36+246,   0+278},
	{   0,  24,   0+210,  24+386},
	{ 632, 170, 632+168, 170+430} };
BOOL CG_ModeWnd::Exec()
{
	char	workBuf[32];
	int		i, now_time = (int)timeGetTime() - alpha_time;
	int		oldSelect = selectNum, oldBtn = btnSelect,nextChar;
	switch(state){
	  case cgmode_allfadein:	
		alpha = now_time / 500.0f;
		if(alpha >= 1.0f){
			alpha = 1.0f;
			charNum = SelectChar();
			if(charNum > 6)charNum = 0;
			char_alpha_org[0] = char_alpha_org[1] = 0;
			alpha_time = timeGetTime();
			state = cgmode_charSelect;
		}
		SetAlpha();
		retBtnPrt.alpha = alpha;
		retBtn.selectRect.top = 565; retBtn.selectRect.bottom = retBtn.selectRect.top+25;
		retBtn.selectRect.left = LONG(709 +(1.0f- alpha)*(47+40*4));
		retBtn.selectRect.right = retBtn.selectRect.left +86;
		return TRUE;
	  case cgmode_allfadeout:	
		alpha = 1.0f -now_time / 500.0f;
		if(alpha < 0){
			if(lpSoundDS)lpSoundDS->ChangeMusic(bgmHandle,2,TRUE,0);
			changeExecMode(bak_mode);	
			return FALSE;
		}
		SetAlpha();
		retBtnPrt.alpha = alpha;
		retBtn.selectRect.top = 457+4*27;
		retBtn.selectRect.top += LONG((1.0f-alpha)*40);
		retBtn.selectRect.bottom = retBtn.selectRect.top +25;
		return TRUE;
	  case cgmode_charSelect:	
		if(prevChar!=0xff){	
			char_alpha_org[0] = (timeGetTime()-alpha_time)/500.0f;
			if(prevChar<0xfe){
				charSelectPrt[prevChar].alpha = char_alpha_org[1] * (1.0f -char_alpha_org[0]);
			}
			if(char_alpha_org[0] >= 1.0f){
				char_alpha_org[0] = 1.0f;
				prevChar = 0xff;
			}
			return TRUE;
		}
		nextChar = SelectChar();
		if(nextChar<=6 && nextChar!=charNum){
			prevChar = charNum;
			charNum = nextChar;
			char_alpha_org[0] = 0;
			char_alpha_org[1] = charSelectPrt[prevChar].alpha;
			PlayEffectNum(evtHandle[11],SE_SELECT);
			alpha_time = timeGetTime();
			return TRUE;
		}
		if(keyState.push_cancel){
			charNum = 0xff;
			PlayEffectNum(evtHandle[11],SE_CANCEL);		
			alpha_time = timeGetTime();
			state = cgmode_allfadeout;
			return TRUE;
		}
		btnSelect = 0;
		if(retBtn.CheckState(&sysInf.msPoint)){
			charNum = 0xff;
			PlayEffectNum(evtHandle[11],SE_CANCEL);		
			alpha_time = timeGetTime();
			state = cgmode_allfadeout;
			return TRUE;
		}else if(btn_onCursor==retBtn.GetState() || btn_push==retBtn.GetState()){
			btnSelect = 2;
			if(oldBtn != btnSelect){
				PlayEffectNum(evtHandle[11],SE_SELECT);		
			}
			return TRUE;
		}
		if(keyState.push_action){
			selectNum = btnSelect = 0xff;
			selectTime = 0;
			if(cg_look_mode==mode){
				selectMax = vMax[charNum];
			}else{
				selectMax = sceneListMax[charNum];
			}
			pageMax = selectMax /12;
			if(selectMax % 12) pageMax ++;
			offsetNum = 0;
			CreateThumbnail();
			PlayEffectNum(evtHandle[11],SE_PUSH);
			alpha_time = timeGetTime()+500;
			state = cgmode_Thumbnail_fadein;
		}
		return TRUE;
	  case cgmode_Thumbnail_fadein:
		titlePrt.alpha = (alpha_time-(int)timeGetTime()) / 500.0f;
		if(titlePrt.alpha <= 0){
			titlePrt.alpha = 0;
			alpha = 0;
			SetAlpha();
			alpha_time = timeGetTime();
			state = cgmode_Thumbnail_fadein2;
		}
		char_alpha_org[0] = titlePrt.alpha;
		return TRUE;
	  case cgmode_Thumbnail_fadeout:
		titlePrt.alpha = (timeGetTime()-alpha_time)/500.0f;
		if(titlePrt.alpha >= 1.0f){
			titlePrt.alpha = 1.0f;
			state = cgmode_charSelect;
		}
		char_alpha_org[0] = titlePrt.alpha;
		return TRUE;
	  case cgmode_Thumbnail_fadein2:	
		alpha = (timeGetTime()-alpha_time)/500.0f;
		if(alpha >= 1.0f){
			alpha = 1.0f;
			state = cgmode_Thumbnail;
		}
		SetAlpha();
		return TRUE;
	  case cgmode_Thumbnail_fadeout2:
		alpha = (alpha_time-(int)timeGetTime())/500.0f;
		if(alpha < 0){
			alpha = 1.0f;
			SetAlpha();
			titlePrt.alpha = 0;
			alpha_time = timeGetTime();
			state = cgmode_Thumbnail_fadeout;
			return TRUE;
		}
		SetAlpha();
		return TRUE;
	  case cgmode_cgfadein: case cgmode_cgcrossfade:
		cgWnd.alpha = now_time / 500.0f;
		if(cgWnd.alpha >= 1.0f){
			cgWnd.alpha = 1.0f;
			state = cgmode_cgview;
		}
		return TRUE;
	  case cgmode_cgview:
		if(keyState.push_action){
			while(subNum < cgList[charNum][(offsetNum+selectNum)*3+1]-1){
				PlayEffectNum(evtHandle[11],SE_PUSH);		
				subNum ++;
				if(FALSE==AVG_GetCgFlag( cgList[charNum][(offsetNum+selectNum)*3],subNum))continue;
				cgWndOld.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
				if(cgList[charNum][(offsetNum+selectNum)*3+2]==1){
					wsprintf(workBuf,"v%04d%02d",cgList[charNum][(offsetNum+selectNum)*3],subNum);
				}else{
					wsprintf(workBuf,"h%04d%02d",cgList[charNum][(offsetNum+selectNum)*3],subNum);
				}
				cgWnd.loadLGF(pack_eventcg, workBuf);
				cgWnd.alpha = 0;
				alpha_time = timeGetTime();
				state = cgmode_cgcrossfade;
				return TRUE;
			}
			PlayEffectNum(evtHandle[11],SE_CANCEL);		
			state = cgmode_cgfadeout;
			alpha_time = timeGetTime();
			return TRUE;
		}
		if(keyState.push_cancel){
			PlayEffectNum(evtHandle[11],SE_CANCEL);		
			state = cgmode_cgfadeout;
			alpha_time = timeGetTime();
		}
		return TRUE;
	  case cgmode_cgfadeout:
		cgWnd.alpha = 1.0f -now_time / 500.0f;
		if(cgWnd.alpha < 0){
			cgWnd.Release();
			state = cgmode_Thumbnail;
		}
		return TRUE;

	}
	ESC_SetFlag(_セーブロード無効 , off);
	if(keyState.push_action){
		if(selectNum!=0xff && (offsetNum+selectNum < selectMax) ){
			PlayEffectNum(evtHandle[11],SE_PUSH);		
			if(cg_look_mode==mode){
				cgWndOld.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
				for(subNum=0;subNum<cgList[charNum][(offsetNum+selectNum)*3+1];subNum++){
					if(AVG_GetCgFlag( cgList[charNum][(offsetNum+selectNum)*3],subNum))break;
				}
				if(cgList[charNum][(offsetNum+selectNum)*3+2]==1){
					wsprintf(workBuf,"v%04d%02d",cgList[charNum][(offsetNum+selectNum)*3],subNum);
				}else{
					wsprintf(workBuf,"h%04d%02d",cgList[charNum][(offsetNum+selectNum)*3],subNum);
				}
				cgWnd.loadLGF(pack_eventcg, workBuf);
				cgWnd.alpha = 0;
				alpha_time = timeGetTime();
				state = cgmode_cgfadein;
				return TRUE;
			}else{
				ESC_InitFlag();
				backColorBuf.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
				wsprintf(workBuf,"%05d.sdt",sceneScript[charNum][offsetNum+selectNum]);
				int index = EXEC_ReadLang( workBuf, &LangData );
				AVG_SetScenarioNo( index );
				EXEC_StartLang( &LangData, MAIN_SCRIPT );
				c_cls_all();
				msgWnd.MsgCLS();
				msgWnd.ForceHide();
				lpSoundDS->FadeOut(bgmHandle);
				ESC_SetFlag( _セーブロード無効 , on);
				ESC_SetFlag( _回想モード中 , on);
				EnableMenuItem( sysInf.hMenu, ID_LOADEXEC, MF_GRAYED );
				EnableMenuItem( sysInf.hMenu, ID_SAVEEXEC, MF_GRAYED );
				changeExecMode( event_exec_mode );
				return TRUE;
			}
		}
	}else if(keyState.push_cancel){
		PlayEffectNum(evtHandle[11],SE_CANCEL);		
		alpha_time = timeGetTime() +500;
		state = cgmode_Thumbnail_fadeout2;
		return TRUE;
	}
	btnSelect = 0xff;
	if(pageMax > 1)for(i=0;i<2;i++){
		if(btn[i].CheckState(&sysInf.msPoint)){
			btnSelect = i;
			switch(i){ 
			  case 0:	
				PlayEffectNum(evtHandle[11],SE_PUSH);		
				if(0==offsetNum)offsetNum = (pageMax-1)*12;
				else offsetNum -= 12;
				CreateThumbnail();
				return TRUE;
			  case 1:	
				PlayEffectNum(evtHandle[11],SE_PUSH);		
				offsetNum += 12;
				if(offsetNum > (pageMax-1)*12)offsetNum = 0;
				CreateThumbnail();
				return TRUE;
			}
		}else if(btn_onCursor==btn[i].GetState()){
			btnSelect = i;
			if(oldBtn != btnSelect){
				PlayEffectNum(evtHandle[11],SE_SELECT);		
			}
		}
	}
	if(retBtn.CheckState(&sysInf.msPoint)){
		btnSelect = 2;
		PlayEffectNum(evtHandle[11],SE_CANCEL);		
		alpha_time = timeGetTime() +500;
		state = cgmode_Thumbnail_fadeout2;
	}else if(btn_onCursor==retBtn.GetState()){
		btnSelect = 2;
		if(oldBtn != btnSelect){
			PlayEffectNum(evtHandle[11],SE_SELECT);		
		}
	}
	selectNum = 0xff;
	for(i=0;i<12;i++){
		if(FALSE==bCgOpen[i])continue;
		rect.left = 22+170*(i%4); rect.right = rect.left +166;
		rect.top = 102+140*(i/4); rect.bottom = rect.top +126;
		if(myPtInRect(&rect,&sysInf.msPoint)){
			selectNum = i;
			if(oldSelect!=selectNum){
				PlayEffectNum(evtHandle[11],SE_SELECT);
				selectTime = timeGetTime() +500;
			}
			break;
		}
	}
	return TRUE;
} // CG_ModeWnd::Exec

void CG_ModeWnd::Draw()
{
	int flashTime = timeGetTime() % 3000;
	float flash = (flashTime -1500) /1500.0f;

	int		i;
	if(cgmode_allfadein==state || cgmode_allfadeout==state){
		g_DibInf.colorBuf.BltFast(0,0, &backupWnd,NULL,FALSE);
	}

	if(cgmode_Thumbnail_fadeout>=state){
		g_DibInf.colorBuf.BltFast(0,0,&baseCBuf,NULL,TRUE);
		if(prevChar < 0xfe){
			g_DibInf.colorBuf.BltFast(charSelectRect[prevChar].left,charSelectRect[prevChar].top,&charSelectPrt[prevChar],NULL,TRUE);
		}
		if(charNum != 0xff){
			charSelectPrt[charNum].alpha = char_alpha_org[0] *(1.0f -fabsf(cosf(flash*M_PI*2)));
			g_DibInf.colorBuf.BltFast(charSelectRect[charNum].left,charSelectRect[charNum].top,&charSelectPrt[charNum],NULL,TRUE);
			if(prevChar < 0xfe){
				rect.left = 0; rect.right = 334;
				rect.top = 76*prevChar; rect.bottom = rect.top +76;
				charnamePrt.alpha = (1.0f -char_alpha_org[0]);
				g_DibInf.colorBuf.BltFast(22-int(20*(1.0f-charnamePrt.alpha)),12,&charnamePrt,&rect,TRUE);
			}
			rect.left = 0; rect.right = 334;
			rect.top = 76*charNum; rect.bottom = rect.top +76;
			charnamePrt.alpha = char_alpha_org[0];
			if(cgmode_Thumbnail_fadein==state || cgmode_Thumbnail_fadeout==state) charnamePrt.alpha = 1.0f;
			g_DibInf.colorBuf.BltFast(22+int(20*(1.0f-charnamePrt.alpha)),12,&charnamePrt,&rect,TRUE);
		}
		rect.left = 0; rect.right = 348;
		rect.top = 26; rect.bottom = 52;
		if(cg_look_mode==mode){
			rect.top = 0; rect.bottom = 26;
		}
		g_DibInf.colorBuf.BltFast(6,550,&titlePrt,&rect,TRUE);
		rect.top = 52; rect.bottom = 78;
		g_DibInf.colorBuf.BltFast(478,22,&titlePrt,&rect,TRUE);
		retBtn.Draw();
		return;
	}
	if(mainWnd.alpha < 1.0f){
		g_DibInf.colorBuf.BltFast(0,0,&baseCBuf,NULL,FALSE);
	}
	g_DibInf.colorBuf.BltFast(0,0,&mainWnd,NULL,TRUE);

	rect.left = 0; rect.right = 334;
	rect.top = 76*charNum; rect.bottom = rect.top +76;
	g_DibInf.colorBuf.BltFast(22,12,&charnamePrt,&rect,TRUE);

	if(pageMax>1)for(i=0;i<2;i++)btn[i].Draw();
	retBtn.Draw();
	if(pageMax > 1){
		rect.top = 48; rect.bottom = rect.top +18;
		rect.left = (offsetNum/12)*20; rect.right = rect.left +20;
		g_DibInf.colorBuf.BltFast(110 ,540,&numParts,&rect,TRUE);
	}
	switch(state){
	  case cgmode_cgfadein: case cgmode_cgview: case cgmode_cgfadeout: case cgmode_cgcrossfade:	
		if(cgmode_cgfadein==state || cgmode_cgcrossfade==state) g_DibInf.colorBuf.BltFast(0,0,&cgWndOld,NULL,FALSE);
		if(cgWnd.alpha < 1.0f && (cgmode_cgfadein==state || cgmode_cgfadeout==state)){
			rect.left = int(200-cgWnd.alpha *200);
			rect.right = int(600+cgWnd.alpha *200);
			rect.top = int(150-cgWnd.alpha *150);
			rect.bottom = int(450+cgWnd.alpha *150);
			g_DibInf.colorBuf.Blt(&rect,&cgWnd,NULL,TRUE);
		}else{
			g_DibInf.colorBuf.BltFast(0,0,&cgWnd,NULL,TRUE);
		}
		break;
	  case cgmode_Thumbnail:
		if(selectNum!=0xff && (offsetNum+selectNum < selectMax) ){
			thumbnailPrt[selectNum+1].alpha = alpha;
			float selectFlash = 0;
			if(selectTime){
				selectFlash = (selectTime -(int)timeGetTime())/500.0f;
				if(selectFlash <= 0){
					selectFlash = 0;
					selectTime = 0;
				}
			}
			if(selectFlash > 0){
				g_DibInf.colorBuf.BrightBlt(25+170*(selectNum%4),105+140*(selectNum/4),&thumbnailPrt[selectNum+1],NULL,int(128+128*selectFlash),int(128+128*selectFlash),int(128+128*selectFlash));
				selectFlash -= 0.1f;
				if(selectFlash < 0)selectFlash = 0;
			}else{
				g_DibInf.colorBuf.BltFast(25+170*(selectNum%4), 105+140*(selectNum/4),&thumbnailPrt[selectNum+1],NULL,TRUE);
			}
			g_DibInf.colorBuf.BltFast(22+170*(selectNum%4), 102+140*(selectNum/4),&thumbnailPrt[13],NULL,TRUE);
		}
		break;
	}
} // CG_ModeWnd::Draw


BOOL CALLBACK ConfDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam );
TitleWnd::TitleWnd()
{
	int		i;
	RECT	ptRect[5];
	char	workBuf[32];

	ZeroMemory(this,sizeof(TitleWnd));
	ZeroMemory(&backSelectInf,sizeof(BackSelectInf));	
	msgWnd.MsgCLS();
	msgWnd.ForceHide();
	ESC_InitEOprFlag();
	ESC_SetGameFlag( 202, 1 );	
	ESC_SetGameFlag( 230, 1 );	
	baseCBuf.loadLGF(pack_gparts,"titleback");
	logoCBuf[0].loadLGF(pack_gparts,"logo_stop");
	logoCBuf[0].offset.x = 80; logoCBuf[0].offset.y = 24;
	logoCBuf[1].loadLGF(pack_gparts,"logo_move");
	logoCBuf[1].offset.x = 277; logoCBuf[1].offset.y = 38;
	charCBuf.loadLGF(pack_gparts,"character");
	crCBuf.loadLGF(pack_gparts,"copyright");
	for(i=0;i<7;i++){
		wsprintf(workBuf,"TmBtn%02d",i+1);
		btnCBuf[i].loadLGF( pack_gparts,workBuf);
	}
	rect.left = 520;	rect.right = rect.left +104;
	rect.top = 402;		rect.bottom = rect.top +22;
	for(i=0;i<5;i++){
		ptRect[i].left = 0;	ptRect[i].right = 104;
		ptRect[i].top = 22*i;
		ptRect[i].bottom = ptRect[i].top +22;
	}
	btn[0].Create(3,&btnCBuf[0],&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 628;	rect.right = rect.left +104;
	for(i=0;i<5;i++){
		ptRect[i].left = 0;	ptRect[i].right = 104;
		ptRect[i].top = 22*i;
		ptRect[i].bottom = ptRect[i].top +22;
	}
	btn[1].Create(3,&btnCBuf[1],&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 520;	rect.right = rect.left +212;
	for(i=0;i<5;i++){
		ptRect[i].left = 0;	ptRect[i].right = 212;
		ptRect[i].top = 22*i;
		ptRect[i].bottom = ptRect[i].top +22;
	}
	btn[2].Create(3,&btnCBuf[2],&rect,ptRect,&g_DibInf.colorBuf);
	if(off==look1BitFlag(CgFlagBuf, 0)){
		btn[2].SetState(btn_disable);	
	}
	rect.left = 520;	rect.right = rect.left +212;
	for(i=0;i<5;i++){
		ptRect[i].left = 0;	ptRect[i].right = 212;
		ptRect[i].top = 22*i;
		ptRect[i].bottom = ptRect[i].top +22;
	}
	btn[3].Create(3,&btnCBuf[3],&rect,ptRect,&g_DibInf.colorBuf);
	if(0==ESC_GetGameFlag( 100 )){
		btn[3].SetState(btn_disable);
	}
	rect.left = 520;	rect.right = rect.left +212;
	for(i=0;i<5;i++){
		ptRect[i].left = 0;	ptRect[i].right = 212;
		ptRect[i].top = 22*i;
		ptRect[i].bottom = ptRect[i].top +22;
	}
	btn[4].Create(3,&btnCBuf[4],&rect,ptRect,&g_DibInf.colorBuf);
	if(0==ESC_GetGameFlag( 230 )){
		btn[4].SetState(btn_disable);
	}
	rect.left = 520;	rect.right = rect.left +104;
	for(i=0;i<5;i++){
		ptRect[i].left = 0;	ptRect[i].right = 104;
		ptRect[i].top = 22*i;
		ptRect[i].bottom = ptRect[i].top +22;
	}
	btn[5].Create(3,&btnCBuf[5],&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 628;	rect.right = rect.left +104;
	for(i=0;i<5;i++){
		ptRect[i].left = 0;	ptRect[i].right = 104;
		ptRect[i].top = 22*i;
		ptRect[i].bottom = ptRect[i].top +22;
	}
	btn[6].Create(3,&btnCBuf[6],&rect,ptRect,&g_DibInf.colorBuf);
	baseCBuf.alpha = charCBuf.alpha = crCBuf.alpha = 0;
	logoCBuf[0].alpha = logoCBuf[1].alpha = 0;
	for(i=0;i<7;i++)btnCBuf[i].alpha = 0;
	timer = timeGetTime() +500;
	changeExecMode( opening_mode );	
} // TitleWnd::TitleWnd

void TitleWnd::SetStartCursor()
{
	char	workBuf[32];
	int		i;
	for(i=0;i<50;i++){
		wsprintf( workBuf, "save_%02d.sav", i );
		DWORD attr = GetFileAttributes(workBuf);
		if((-1)!=attr && (attr & FILE_ATTRIBUTE_ARCHIVE))break;	
	}
	if(i==50){	
		selectNum = 0;
		SetWindowCursorPos(sysInf.hWnd,btn[0].selectRect.left+50, btn[0].selectRect.top+10);
	}else{
		selectNum = 1;
		SetWindowCursorPos(sysInf.hWnd,btn[1].selectRect.left+50, btn[1].selectRect.top+10);
	}
} // TitleWnd::SetStartCursor

void AVG_SetScenarioNo( int scenario_no );
BOOL TitleWnd::Exec()
{
	int		i,index;
	float	pos;
	BOOL	bMoveCur = FALSE;
	int		oldSelect = selectNum;

	if(state<4 && keyState.push_action){ 
		baseCBuf.alpha = charCBuf.alpha = crCBuf.alpha = 1.0f;
		logoCBuf[0].alpha = logoCBuf[1].alpha = 1.0f;
		for(i=0;i<7;i++)btnCBuf[i].alpha = 1.0f;
		logoCBuf[1].offset.x = 257;
		logoCBuf[1].offset.y = 57;
		for(i=2;i<6;i++){
			btn[i].selectRect.top = 402 +27*(i-1);
			btn[i].selectRect.bottom = btn[i].selectRect.top +22;
			if(i==5){
				btn[6].selectRect.top = btn[5].selectRect.top;
				btn[6].selectRect.bottom = btn[5].selectRect.bottom;
			}
		}
		SetStartCursor();
		EnableMenuItem( sysInf.hMenu, ID_LOADEXEC, MF_ENABLED  );
		EnableMenuItem( sysInf.hMenu, ID_SAVEEXEC, MF_ENABLED  );
		EnableMenuItem( sysInf.hMenu, ID_CONFIG, MF_ENABLED );
		lpSoundDS->ChangeMusic(bgmHandle,2,TRUE,0);
		state = 4;
		return FALSE;
	}
	switch(state){
	  case 0:
		baseCBuf.alpha = 1.0f -(timer-(int)timeGetTime())/500.0f;
		if(baseCBuf.alpha >= 1.0f){
			backColorBuf.clearColorBuf();
			baseCBuf.alpha = 1.0f;
			timer = timeGetTime() +500;
			state = 1;
		}
		return FALSE;
	  case 1:
		logoCBuf[0].alpha = logoCBuf[1].alpha = 1.0f -(timer-(int)timeGetTime())/500.0f;
		if(logoCBuf[0].alpha >= 1.0f){
			logoCBuf[0].alpha = logoCBuf[1].alpha = 1.0f;
			timer = timeGetTime() +500;
			PlayEffectNum(evtHandle[10],8159);
			state = 2;
		}
		return FALSE;
	  case 2:
		pos = 1.0f -(timer-(int)timeGetTime())/500.0f;
		logoCBuf[1].offset.x = LONG(277 -20*pos);
		logoCBuf[1].offset.y =  LONG(38 +19*pos);
		if(logoCBuf[1].offset.x <= 257){
			logoCBuf[1].offset.x = 257;
			logoCBuf[1].offset.y = 57;
			timer = timeGetTime() +1000;
			state = 3;
		}
		return FALSE;
	  case 3:
		charCBuf.alpha = 1.0f -(timer-(int)timeGetTime())/1000.0f;
		if(charCBuf.alpha >= 1.0f){
			charCBuf.alpha = 1.0f;
			timer = timeGetTime() +500;
			SetStartCursor();
			EnableMenuItem( sysInf.hMenu, ID_LOADEXEC, MF_ENABLED  );
			EnableMenuItem( sysInf.hMenu, ID_SAVEEXEC, MF_ENABLED  );
			EnableMenuItem( sysInf.hMenu, ID_CONFIG, MF_ENABLED );
			lpSoundDS->ChangeMusic(bgmHandle,2,TRUE,0);
			state = 4;
		}
		crCBuf.alpha = charCBuf.alpha; 

		btnCBuf[0].alpha = 1.0f -((timer-500)-(int)timeGetTime())/500.0f;
		if(btnCBuf[0].alpha >= 1.0f) btnCBuf[0].alpha = 1.0f;
		for(i=2;i<6;i++){
			btn[i].selectRect.top = LONG(402 +27*(i-1)*btnCBuf[0].alpha);
			btn[i].selectRect.bottom = btn[i].selectRect.top +22;
			if(i==5){
				btn[6].selectRect.top = btn[5].selectRect.top;
				btn[6].selectRect.bottom = btn[5].selectRect.bottom;
			}
		}
		for(i=1;i<7;i++) btnCBuf[i].alpha = btnCBuf[0].alpha;

		return FALSE;
	}
	if(keyState.push_up || keyState.push_left){
		selectNum --;
		while(1){
			if(selectNum<0){
				selectNum = 6;
				break;
			}
			if(btn[selectNum].GetState()!=btn_disable)break;
			selectNum --;
		}
		bMoveCur = TRUE;
	}else if(keyState.push_down || keyState.push_right){
		selectNum ++;
		while(1){
			if(selectNum>6){
				selectNum = 0;
				break;
			}
			if(btn[selectNum].GetState()!=btn_disable)break;
			selectNum ++;
		}
		bMoveCur = TRUE;
	}
	if(bMoveCur){
		SetWindowCursorPos(sysInf.hWnd,btn[selectNum].selectRect.left+50, btn[selectNum].selectRect.top+10);
	}else{
		selectNum = -1;
	}
	for(i=0;i<7;i++){
		if(FALSE==btn[i].CheckState(&sysInf.msPoint)){
			if(btn[i].GetState()==btn_onCursor){
				selectNum = i;
				if(selectNum != oldSelect){
					PlayEffectNum(evtHandle[11],SE_SELECT);
				}
			}
			continue;
		}
		selectNum = i;
		PlayEffectNum(evtHandle[11],SE_PUSH);		
		switch(i){
		  case 0:
			index = EXEC_ReadLang( "00000.sdt", &LangData );
			AVG_SetScenarioNo(index);
			EXEC_StartLang( &LangData, MAIN_SCRIPT );
			c_cls_all();
			msgWnd.MsgCLS();
			sysInf.bAlreadyRead =  AVG_CheckScenarioFlag();
			lpSoundDS->FadeOut(bgmHandle);
			changeExecMode( event_exec_mode );
			return TRUE;
		  case 1:
			EXEC_LangBuf  = (char *)&LangData.LangBuf;
			EXEC_LangInfo = &LangData;
			saveWnd = new SaveWnd(load_mode);
			return FALSE;
		  case 2:	
			cgmodeWnd = new CG_ModeWnd(cg_look_mode);
			break;
		  case 3:	
			cgmodeWnd = new CG_ModeWnd(scene_play_mode);
			break;
		  case 4:	
			musicWnd = new MusicModeWnd;
			break;
		  case 5:	
			optionWnd = new OptionMode;
			break;
		  case 6:	
			PlayEffectNum(evtHandle[11],SE_CANCEL);		
			sysInf.bGameEnd = on;
			changeExecMode( game_end_mode );	
			return TRUE;
		}
	}
	return FALSE;
} // TitleWnd::Exec

void TitleWnd::Draw()
{
	int			i;

	if(opening_mode!=sysInf.execMode)return;
	g_DibInf.colorBuf.BltFast(0,0,&baseCBuf,NULL,TRUE);
	if(state>=3) g_DibInf.colorBuf.BltFast(0,0,&charCBuf,NULL,TRUE);
	if(state>=1){
		for(i=0;i<2;i++){
			g_DibInf.colorBuf.BltFast(0,0,&logoCBuf[i],NULL,TRUE);
		}
	}
	if(state<3)return;
	g_DibInf.colorBuf.BltFast(336,513,&crCBuf,NULL,TRUE);
	for(i=0;i<7;i++){
		btn[i].Draw();
	}
} // TitleWnd::Draw


BOOL RudderWnd::Exec()
{
	if(ESC_GetFlag(_インフォメーション無効)){
		animeCnt = 0;
		alpha = 0;
		state = 0;
		return TRUE;	
	}
	switch(state){
	  case 0:	
	  case 3:
		if(0==animeCnt && 0==alpha){
			if( (msgWnd.GetAlpha() >= 1.0f) &&
				lineLength2((float)sysInf.msPoint.x, (float)sysInf.msPoint.y, 799, 0) < 92*92 ){
				startTime = timeGetTime();
				state = 1;
			}
			return TRUE;
		}
		animeCnt = (timeGetTime() -startTime) / 33 +animeCnt_bak;
		alpha = 1.0f -(animeCnt-animeCnt_bak)/20.0f;
		if(alpha<0 || state==3) alpha = 0;
		if(animeCnt > 59)animeCnt = 0;
		rudderPrt.alpha = alpha*.5f +.5f;
		infoPrt.alpha = alpha*msgWnd.GetAlpha();
		break;
	  case 1:	
		if(0==startTime){
			startTime = timeGetTime();
			animeCnt = 0;
		}else{	
			animeCnt = (timeGetTime() -startTime) / 33;
			if(animeCnt > 39){
				animeCnt = 40;
				startTime = timeGetTime();
				state = 2;
			}
			alpha = 1.0f -(40-animeCnt)/40.0f;
			rudderPrt.alpha = alpha*.5f +.5f;
			infoPrt.alpha = alpha*msgWnd.GetAlpha();
		}
		if(keyState.push_action && 
			lineLength2((float)sysInf.msPoint.x, (float)sysInf.msPoint.y, 799, 0) < 92*92 ){
				mapWnd = new MapWnd;
				clearPadState();
		}
		break;
	  case 2:
		if( msgWnd.GetAlpha() < 1.0f ){	
			startTime = timeGetTime();
			animeCnt_bak = animeCnt;
			state = 3;
		}else if( lineLength2((float)sysInf.msPoint.x, (float)sysInf.msPoint.y, 799, 0) > 92*92 ){
			startTime = timeGetTime();
			animeCnt_bak = animeCnt;
			state = 0;
		}else{
			animeCnt = (timeGetTime() -startTime) / 33;
			animeCnt = (animeCnt % 20) +40;
			if(keyState.push_action){
				mapWnd = new MapWnd;
				clearPadState();
			}
		}
		break;
	}
	return TRUE;
}
void RudderWnd::Draw()
{
	if(ESC_GetFlag(_インフォメーション無効)) return;
	rect.left = 0; rect.right = 92;
	rect.top = 92*animeCnt; rect.bottom = rect.top +92;
	g_DibInf.colorBuf.BltFast(int(800-92*msgWnd.GetAlpha()),-int(92*(1.0f-msgWnd.GetAlpha())),&rudderPrt,&rect,TRUE);
	if(infoPrt.alpha > 0){
		rect.left = 0; rect.right = 96;
		rect.top = 0; rect.bottom = 18;
		g_DibInf.colorBuf.BltFast(700,24,&infoPrt,&rect,TRUE);
	}
}

BYTE floorConvert[28][2] = {
	{0, 5},	
	{0, 4},	
	{1, 5},	
	{1, 2},	
	{1, 4},	
	{1, 6},	
	{1, 3},	
	{1, 0},	
	{2, 4},	
	{2, 3},	
	{2, 2},	
	{2, 0},	
	{2, 1},	
	{3, 4},	
	{3, 3},	
	{3, 0},	
	{3, 1},	
	{3, 2},	
	{3, 5},	
	{4, 0},	
	{4, 1},	
	{4, 2},	
	{4, 3},	
	{0, 3},	
	{0, 1},	
	{0, 2},	
	{0, 0},	
	{1, 1},	
};
