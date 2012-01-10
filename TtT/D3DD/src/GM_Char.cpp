#include <windows.h>
#include <mouse.h>

#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "comp_pac.h"

#include "main.h"
#include "escript.h"

#include "GlobalDraw.h"

#include "GM_Main.h"
#include "GM_Battle.h"
#include "GM_Avg.h"
#include "GM_Char.h"
#include "GM_CharThink.h"

#include "GM_Magic.h"
#include "GM_Item.h"
#include <math.h>





#if (TAIKEN_FLAG==2)
PLAYER_PARAM	PlayerParam = {
	1000,0
};
#else
PLAYER_PARAM	PlayerParam = {
	0,0
};
#endif



#include "DAT_CharName.h"

char *CHR_GetCharName( int sex )
{
	srand( timeGetTime() );

	int	r;
	
	while(1){
		r = rand()%128;
		int	i, itti=0;
		for(i=0;i<CHAR_PARAM_MAX;i++){
			if(CharParam[i].flag){
				if(CharParam[i].unit==0||CharParam[i].unit==3){
					if( _mbscmp( (BYTE*)CharParam[i].name, (BYTE*)CharName[sex][r])==0 ){
						itti=1;
						break;
					}
				}
			}
		}
		if(itti==0){
			break;
		}
	}

	return CharName[sex][r];
}


CHAR_CONTROL	CharControl[CHAR_CONTROL_MAX];
int				StopCount=0;

int				CharMarkerSelect[CHAR_CONTROL_MAX];		
int				CharMarkerAtack[CHAR_CONTROL_MAX];		

int				CharMarkerPlayer = FALSE;	
int				CharMarkerEnemy  = FALSE;	

int				CharMarkerBoss = -1;	




JOB_PARAM2	JobParam2[32+1]={
	{0, 0, 0, 0, 0, 0, 0, 0 },
};

CHAR_PARAM	InitCharParam[10]={
	{1,0,0,"アロウン　　", 12,0, ATTR_YAMI, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"アルサル　　", 13,0, ATTR_TI,   1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"リアンノン　", 16,0, ATTR_HOSI, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"オガム　　　", 14,0, ATTR_KA,   1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"モルガン　　", 17,0, ATTR_HUU,  1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"オクタヴィア", 18,0, ATTR_KA,   1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"タリエシン　", 15,0, ATTR_SUI,  1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"スィール　　", 19,0, ATTR_SUI,  1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"ラスティ　　", 20,0, ATTR_TI,   1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
};

CHAR_PARAM	CharParam[CHAR_PARAM_MAX]={
	{1,0,0,"アロウン　　", 12,0, ATTR_YAMI, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"アルサル　　", 13,0, ATTR_TI,   1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"リアンノン　", 16,0, ATTR_HOSI, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"オガム　　　", 14,0, ATTR_KA,   1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"モルガン　　", 17,0, ATTR_HUU,  1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"オクタヴィア", 18,0, ATTR_KA,   1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"タリエシン　", 15,0, ATTR_SUI,  1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"スィール　　", 19,0, ATTR_SUI,  1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{1,0,0,"ラスティ　　", 20,0, ATTR_TI,   1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },

	{0,0,0,"剣士　　　　",  0,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"魔戦士　　　",  1,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"巫女　　　　",  2,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"占星術師　　",  3,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"弓使い　　　",  4,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"楽士　　　　",  5,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },

	{0,0,0,"戦士　　　　",  6,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"騎士　　　　",  7,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"魔法使い　　",  8,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"ドルイド　　",  9,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"狩人　　　　", 10,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"吟遊詩人　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },

	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
	{0,0,0,"未登録　　　", 11,0,  1, 1,0, 50, 20, 10, 10, 10, 10, 10, 10, 300, 0,0,0,0 },
};




JOB_PARAM	JobParam[JOB_MAX]={

	{ "剣士　　　　",100, EX_HITO|EX_AVG_LIM, ATTR_UNKNOWN, 450, 20, 80, 50, 60, 70, 20, 30, 350, ITEM_BUKI_SWORD,             255,  51,  0,  0,  0,  0 },
	{ "魔戦士　　　",101, EX_HITO|EX_MGK_SPD, ATTR_UNKNOWN, 400, 30, 55, 45, 50, 50, 60, 40, 250, ITEM_BUKI_SWORD, ITEM_BUKI_SPEAR,  51, 53,  0,  0,  0 },
	{ "巫女　　　　",102, EX_HITO,            ATTR_UNKNOWN, 300, 70, 35, 45, 40, 40, 60, 80, 270, ITEM_BUKI_WAND,              255,   0,  0,  0,  0, 55 },
	{ "占星術師　　",103, EX_HITO,            ATTR_UNKNOWN, 300, 60, 30, 30, 45, 45, 65, 75, 260, ITEM_BUKI_WAND,              255,   0,  0,  0,  0, 55 },
	{ "弓使い　　　",104, EX_HITO|EX_KISYU,   ATTR_UNKNOWN, 400, 20, 65, 40, 80, 70, 20, 25, 290, ITEM_BUKI_BOW,               255,   0,  0,  0, 57,  0 },
	{ "楽士　　　　",105, EX_HITO|EX_GAKUSI,  ATTR_UNKNOWN, 350, 40, 45, 40, 70, 45, 50, 50, 280, ITEM_BUKI_BOW,               255,   0,  0,  0, 57,  0 },

	{ "戦士　　　　",110, EX_HITO|EX_ATR_DEF, ATTR_UNKNOWN, 450, 20, 65, 65, 60, 60, 20, 30, 240, ITEM_BUKI_SWORD,             255,  50,  0,  0,  0,  0 },
	{ "騎士　　　　",111, EX_HITO|EX_SNG_DEF, ATTR_UNKNOWN, 400, 30, 60, 70, 50, 55, 20, 45, 230, ITEM_BUKI_SWORD, ITEM_BUKI_SPEAR,  50, 53,  0,  0 , 0 },
	{ "魔法使い　　",112, EX_HITO,            ATTR_UNKNOWN, 250, 75, 30, 40, 50, 45, 80, 55, 260, ITEM_BUKI_WAND,              255,   0,  0,  0,  0, 54 },
	{ "ドルイド　　",113, EX_HITO,            ATTR_UNKNOWN, 250, 65, 30, 35, 50, 45, 70, 70, 250, ITEM_BUKI_WAND,              255,   0,  0,  0,  0, 54 },
	{ "狩人　　　　",114, EX_HITO|EX_KARI,    ATTR_UNKNOWN, 400, 20, 55, 50, 70, 80, 20, 25, 300, ITEM_BUKI_BOW,               255,   0,  0,  0, 57,  0 },
	{ "吟遊詩人　　",115, EX_HITO|EX_GINYU,   ATTR_UNKNOWN, 350, 40, 35, 35, 65, 40, 60, 65, 280, ITEM_BUKI_BOW,               255,   0,  0,  0, 57,  0 },

	{ "魔王さま　　",  0, EX_SNG_DEF,         ATTR_UNKNOWN, 500, 50, 75, 75, 65, 60, 75, 60, 250, ITEM_BUKI_SWORD,             255,   0,  0,  0,  0,  0 },
	{ "一の戦士　　", 10, EX_HITO|EX_AVG_LIM, ATTR_UNKNOWN, 550, 20, 80, 80, 60, 60, 25, 35, 230, ITEM_BUKI_SWORD, ITEM_BUKI_SPEAR,  50, 53,  0,  0,  0 },
	{ "大賢者　　　", 11, 0,                  ATTR_UNKNOWN, 300, 90, 40, 45, 55, 60, 85, 70, 270, ITEM_BUKI_WAND,              255,   0,  0,  0,  0, 54 },
	{ "ヘボ詩人　　", 12, EX_HITO|EX_GINYU,   ATTR_UNKNOWN, 400, 40, 55, 55, 75, 50, 50, 75, 290, ITEM_BUKI_SWORD,   ITEM_BUKI_BOW,  50,  0,  0, 57,  0 },

	{ "託宣の巫女　",  1, EX_HITO,            ATTR_UNKNOWN, 350, 80, 40, 55, 55, 65, 70, 90, 280, ITEM_BUKI_WAND,              255,   0,  0,  0,  0, 55 },
	{ "女戦士　　　",  3, EX_HITO|EX_KISYU,   ATTR_UNKNOWN, 450, 20, 65, 50, 90, 80, 20, 30, 300, ITEM_BUKI_SWORD,   ITEM_BUKI_BOW,  51,  0,  0, 57,  0 },
	{ "帝国騎士　　",  2, EX_HITO|EX_SNG_DEF, ATTR_UNKNOWN, 450, 40, 65, 70, 65, 60, 50, 70, 240, ITEM_BUKI_SWORD, ITEM_BUKI_SPEAR,  52, 53,  0,  0,  0 },
	{ "アザラシ妖精",  5, EX_MGK_SPD,         ATTR_UNKNOWN, 400, 55, 60, 60, 60, 40, 85, 50, 260, ITEM_BUKI_SPEAR,             255,   0,  1,  0,  0,  0 },
	{ "鉱山妖精　　",  8, EX_ATR_DEF,         ATTR_UNKNOWN, 600, 20, 100, 90, 65, 65, 20, 40, 220, ITEM_BUKI_HAMMER,            255,   0,  0,  2,  0,  0 },

	{ "帝国兵士　　",120, EX_HITO, ATTR_UNKNOWN, 500, 20, 75, 65, 55, 65, 20, 30, 260, ITEM_BUKI_SWORD,255,  52,  0, 0, 0,  0 },
	{ "帝国騎士　　",121, EX_HITO|EX_SNG_DEF, ATTR_UNKNOWN, 450, 30, 70, 70, 60, 55, 20, 40, 240, ITEM_BUKI_SWORD,255,  52, 53, 0, 0,  0 },
	{ "帝国騎士　　",121, EX_HITO|EX_SNG_DEF, ATTR_UNKNOWN, 450, 30, 60, 70, 65, 55, 20, 55, 240, ITEM_BUKI_SPEAR,255,  52, 53, 0, 0,  0 },
	{ "聖堂騎士　　",122, EX_HITO|EX_SNG_DEF, ATTR_UNKNOWN, 450, 40, 70, 60, 60, 50, 50, 40, 250, ITEM_BUKI_SWORD,255,  52, 53, 0, 0,  0 },
	{ "聖堂騎士　　",122, EX_HITO|EX_SNG_DEF, ATTR_UNKNOWN, 450, 40, 60, 60, 65, 50, 70, 30, 250, ITEM_BUKI_SPEAR,255,  52, 53, 0, 0,  0 },

	{ "帝国神官　　",123, EX_HITO, ATTR_UNKNOWN, 300, 70, 30, 45, 45, 50, 70, 80, 270, ITEM_BUKI_WAND,255,   0,  0, 0, 0, 56 },
	{ "帝国司教　　",124, EX_HITO, ATTR_UNKNOWN, 300, 80, 30, 30, 45, 45, 80, 70, 280, ITEM_BUKI_WAND,255,   0,  0, 0, 0, 56 },

	{ "剣闘士　　　",116, EX_HITO|EX_AVG_LIM, ATTR_UNKNOWN, 550, 10, 80, 40, 70, 70, 10, 60, 350, ITEM_BUKI_SWORD,255,  52,  0, 0, 0, 0 },

	{ "ガイウス　　", 21, EX_BOSS|EX_GAKUSI|EX_SNG_DEF,            ATTR_KA,  1200, 30, 80, 70, 60, 60, 60, 65, 300, ITEM_BUKI_SWORD,255,  52, 0, 0, 0,  0 },
	{ "クレオン　　", 23, EX_BOSS|EX_GAKUSI|EX_SNG_DEF|EX_MGK_SPD, ATTR_TI,  1000,200, 70, 65, 70, 65, 70, 80, 300, ITEM_BUKI_WAND, 255,   0, 0, 0, 0, 56 },
	{ "レクトール　", 24, EX_BOSS|EX_GAKUSI|EX_SNG_DEF|EX_MGK_SPD, ATTR_SEI, 2000,200, 90, 90, 80, 50, 80, 80, 300, ITEM_BUKI_SWORD,255,   3, 0, 0, 0,  0 },



	{ "オルクス　　",  300, 0, ATTR_UNKNOWN, 500,20, 80, 60, 60, 40, 40, 40, 250, ITEM_BUKI_SWORD,255,   88, 0, 0, 0,  0 },


	{ "シオマネキ　", 301, 0, ATTR_SUI, 450,10, 70,  90, 50, 50, 10, 40, 150, ITEM_BUKI_SWORD,255,   88, 0, 0, 0,  0 },
  
	{ "キャンサー　", 301, 0, ATTR_SUI, 500,10, 75,  95, 55, 55, 15, 45, 150, ITEM_BUKI_SWORD,255,    188, 0, 0, 0,  0 },

	{ "カルキノス　", 301, 0, ATTR_SUI, 550,10, 80, 100, 60, 50, 20, 50, 150, ITEM_BUKI_SWORD,255,    288, 0, 0, 0,  0 },



	{ "ネクロム　　", 302, 0, ATTR_YAMI, 500,10, 60, 60, 60, 60, 60, 60, 200, ITEM_BUKI_SWORD,255,     50, 0, 0, 0,  0 },	
	{ "ネクロム　　", 302, 0, ATTR_YAMI, 500,10, 70, 70, 70, 70, 70, 70, 200, ITEM_BUKI_SWORD,255,    150, 0, 0, 0,  0 },	
	{ "ネクロム　　", 303, 0, ATTR_YAMI, 500,10, 60, 60, 60, 60, 60, 60, 200, ITEM_BUKI_SWORD,255,     50, 0, 0, 0,  0 },	
	{ "ネクロム　　", 303, 0, ATTR_YAMI, 500,10, 70, 70, 70, 70, 70, 70, 200, ITEM_BUKI_SWORD,255,    150, 0, 0, 0,  0 },	

	{ "スケルトン　", 304, 0, ATTR_YAMI, 450,10, 70, 50, 70, 70, 30, 30, 270, ITEM_BUKI_SWORD,255,    50, 0, 0, 0,  0 },	



	{ "エッヘウシュカ", 305, EX_SNG_DEF, ATTR_SUI, 350,30, 60, 40, 80, 70, 90, 20, 270, ITEM_BUKI_SWORD,255,    88, 0, 0, 0,  0 }, 





	{ "アッハウシュカ", 305, EX_SNG_DEF, ATTR_TI, 350,30, 65, 45, 80, 70, 95, 20, 270, ITEM_BUKI_SWORD,255,    188, 0, 0, 0,  0 },	






	{ "トルフトルイス", 306, EX_SNG_DEF,  ATTR_TI, 1000,30, 100, 100, 70, 70, 50, 50, 500, ITEM_BUKI_SWORD,255,   188, 0, 0, 0,  0 },



	{ "ヘンウェン", 306, EX_SNG_DEF, ATTR_TI, 1200,30, 100, 120, 70, 70, 50, 50, 500, ITEM_BUKI_SWORD,255,   388, 0, 0, 0,  0 },




	{ "豚男",  300, EX_MGK_SPD, ATTR_UNKNOWN, 550,30, 85, 65, 65, 45, 45, 45, 250, ITEM_BUKI_SWORD,255,  188, 0, 0, 0,  0 },





	{ "イノシシ", 306, 0,  ATTR_TI, 500,10, 80, 80, 55, 55, 30, 30, 500, ITEM_BUKI_SWORD,255,   88, 0, 0, 0,  0 },




	{ "Ｅ・ゴーレム", 307, 0, ATTR_NOT, 500,100, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,   88, 0, 0, 0,  0 },




	{ "ゴーレム", 307, 0, ATTR_NOT, 900, 30, 90, 50, 60, 40, 30, 90, 200, ITEM_BUKI_SWORD,255,  188, 0, 0, 0,  0 },




	{ "ギガース", 308, 0, ATTR_UNKNOWN, 600,10, 100, 100, 60, 50, 30, 30, 200, ITEM_BUKI_SWORD,255,   88, 0, 0, 0,  0 },




	{ "ゴルメス", 309, EX_SNG_DEF, ATTR_SEI, 600,30, 80, 80, 65, 65, 85, 30, 300, ITEM_BUKI_SWORD,255,   88, 0, 0, 0,  0 },




	{ "狂木　　　　", 315, EX_GINYU, ATTR_HUU, 500,100, 60, 80, 50, 50, 90, 20, 200, ITEM_BUKI_SWORD,255,   88, 0, 0, 0, 0 },


	{ "食人樹　　　", 315, EX_GINYU,  ATTR_HUU, 500,100, 65, 85, 55, 55, 95, 25, 200, ITEM_BUKI_SWORD,255,  188, 0, 0, 0, 0 },


	{ "森の爺　　　", 315, EX_GINYU, ATTR_HUU, 500,100, 70, 90, 60, 60, 100, 30, 200, ITEM_BUKI_SWORD,255,  288, 0, 0, 0, 0 },




	{ "黒妖犬", 316, EX_AVG_LIM, ATTR_HUU, 350,100, 65, 40, 70, 80, 20, 30, 400, ITEM_BUKI_SWORD,255,   88, 0, 0, 0, 0 },


	{ "ガルム", 316, EX_AVG_LIM, ATTR_HUU, 400,100, 70, 45, 70, 80, 30, 30, 400, ITEM_BUKI_SWORD,255,  188, 0, 0, 0, 0 },




	{ "ムカシトカゲ", 317, 0, ATTR_TI, 500,10, 80, 60, 60, 60, 50, 50, 350, ITEM_BUKI_SWORD,255,    88, 0, 0, 0, 0 },	






	{ "雪男", 318, 0, ATTR_SUI, 600,50, 80, 50, 60, 50, 70, 50, 300, ITEM_BUKI_SWORD,255,    88, 0, 0, 0, 0 },




	{ "ベヒモス", 318, 0,ATTR_TI, 700,60, 85, 55, 60, 50, 80, 50, 300, ITEM_BUKI_SWORD,255,   188, 0, 0, 0, 0 },



	{ "タロス", 319, EX_KISYU,ATTR_NOT, 450,10, 60, 65, 100, 60, 20, 30, 250, ITEM_BUKI_BOW,255,   0, 0, 0, 88, 0 },




	{ "オーガ", 320, 0,ATTR_UNKNOWN, 800,10, 100, 60, 60, 60, 30, 30, 300, ITEM_BUKI_SWORD,255,    88, 0, 0, 0, 0 },




	{ "竜族の子ども", 312,  0,ATTR_UNKNOWN, 500,100, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,   88, 0, 0, 0, 0 },





	{ "Ｅ・ドラゴン　",313, 0,ATTR_KA, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,   88, 0, 0, 0, 0 },


	{ "ドラゴン　　　",311, 0,ATTR_KA, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,   88, 0, 0, 0, 0 },



	{ "屍竜　　　　　",310, EX_BOSS2|EX_BOSS|EX_GAKUSI|EX_SNG_DEF,ATTR_YAMI, 1200, 1000, 100, 70, 70, 70, 60, 60, 300, ITEM_BUKI_SWORD,255,   88, 0, 0, 0, 0 },

	{ "アークオルクス",300, EX_MGK_SPD,ATTR_UNKNOWN, 600, 40, 70, 40, 60, 50, 70, 40, 250, ITEM_BUKI_SWORD,255,  288, 0, 0, 0, 0 },
	{ "骸骨騎士　　　",304, 0,ATTR_YAMI, 500,20, 75, 55, 70, 70, 65, 35, 300, ITEM_BUKI_SWORD,255,  150, 0, 0, 0, 0 },
	{ "デュラハン　　",304, 0,ATTR_YAMI, 550,30, 80, 60, 70, 70, 70, 40, 300, ITEM_BUKI_SWORD,255,  250, 0, 0, 0, 0 },
	{ "シルバータロス",319, EX_KISYU,ATTR_NOT, 500,10, 60, 65, 105, 60, 30, 40, 250, ITEM_BUKI_BOW,  255,  0, 0, 0,188, 0 },

	{ "サウルス　　　",317, 0,ATTR_TI, 550,10, 80, 60, 60,60, 50, 50, 350, ITEM_BUKI_SWORD,255,   188, 0, 0, 0, 0 },	



	{ "ケルトじいさん",200, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ケルトばあさん",201, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ケルトオヤジ１",202, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ケルトオヤジ２",202, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255, 199, 0, 0, 0, 0 },
	{ "ケルトおばさん",203, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ケルト青年　　",204, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ケルト姉さん　",205, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ケルト子供男　",206, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ケルト子供女　",207, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },

	{ "ローマじじい１",220, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ローマじじい２",220, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255, 199, 0, 0, 0, 0 },
	{ "ローマばあさん",221, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ローマおっさん",222, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ローマおばさん",223, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ローマ青年　　",224, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },
	{ "ローマ姉さん　",225, 0,ATTR_UNKNOWN, 100, 10, 60, 60, 60, 60, 60, 60, 300, ITEM_BUKI_SWORD,255,  99, 0, 0, 0, 0 },

	{ "ドルウク　　　", 20, EX_GINYU|EX_MGK_SPD, ATTR_TI, 2000, 200,  50,  50,  70, 80,  80, 120, 250, ITEM_BUKI_WAND, 255,  0, 0, 0, 0, 56 },
	{ "メルカディス　",314, EX_BOSS2|EX_BOSS|EX_SNG_DEF|EX_GAKUSI|EX_MGK_SPD,ATTR_SEI, 2500,1000,  90,  90, 70, 60, 60, 60, 200, ITEM_BUKI_SWORD,255,   88, 0, 0, 0,  0 },
	{ "メルカディス　",314, EX_BOSS2|EX_BOSS|EX_SNG_DEF|EX_GAKUSI|EX_MGK_SPD,ATTR_SEI, 3000,1000, 100, 100, 70, 60, 70, 70, 200, ITEM_BUKI_SWORD,255,  188, 0, 0, 0,  0 },

	{ "ギガース　　　",308, EX_HAKO,ATTR_UNKNOWN, 1000,100, 75, 75, 75, 75, 75, 75, 220, ITEM_BUKI_SWORD,255,  188, 0, 0, 0,  0 },

	{ "はこ　　　　　",400,EX_HAKO|EX_BOSS2, ATTR_NOT, 100, 0, 0, 60, 0, 60, 0, 200, 300, ITEM_BUKI_SWORD,255,  0, 0, 0, 0, 0 },
	{ "はこ　　　　　",401,EX_HAKO|EX_BOSS2, ATTR_NOT, 100, 0, 0, 60, 0, 60, 0, 200, 300, ITEM_BUKI_SWORD,255,  0, 0, 0, 0, 0 },
	{ "はこ　　　　　",402,EX_HAKO|EX_BOSS2, ATTR_NOT, 100, 0, 0, 60, 0, 60, 0, 200, 300, ITEM_BUKI_SWORD,255,  0, 0, 0, 0, 0 },
	{ "つぼ　　　　　",403,EX_HAKO|EX_BOSS2, ATTR_NOT, 100, 0, 0, 60, 0, 60, 0, 200, 300, ITEM_BUKI_SWORD,255,  0, 0, 0, 0, 0 },
	{ "たる　　　　　",404,EX_HAKO|EX_BOSS2, ATTR_NOT, 100, 0, 0, 60, 0, 60, 0, 200, 300, ITEM_BUKI_SWORD,255,  0, 0, 0, 0, 0 },
	{ "宝箱　　　　　",405,EX_HAKO|EX_BOSS2, ATTR_NOT, 100, 0, 0, 60, 0, 60, 0, 200, 300, ITEM_BUKI_SWORD,255,  0, 0, 0, 0, 0 },
	{ "宝箱　　　　　",406,EX_HAKO|EX_BOSS2, ATTR_NOT, 100, 0, 0, 60, 0, 60, 0, 200, 300, ITEM_BUKI_SWORD,255,  0, 0, 0, 0, 0 },

	{ "エイジ　　　　",500, EX_GINYU|EX_SNG_DEF, ATTR_SEI,  3000, 200,  70,  70,  60,  50, 120,  60,  500, ITEM_BUKI_SWORD,255,  0, 0, 0, 0, 0 },
	{ "メカシャチョー",501, EX_GINYU|EX_AVG_LIM, ATTR_YAMI, 5000, 200, 100,  90,  60,  70,  50,  50, 1000, ITEM_BUKI_SWORD,255,  0, 0, 0, 0, 0 },
	{ "ゾンビドルウク", 20, EX_GINYU|EX_MGK_SPD, ATTR_TI,   2000, 200,  50,  70,  50,  50,  80, 100,  250, ITEM_BUKI_WAND, 255,  0, 0, 0, 0,156 },
	{ "オベリスク　　",410, EX_HAKO|EX_BOSS2, ATTR_NOT, 300, 0,  0, 10,  0,  0, 0, 10, 250, ITEM_BUKI_SWORD,255,  0, 0, 0, 0,0 },
};


void CHAR_PARAM::LearningMagic( int type, int mno )
{
	int		i,j;
	short	magic_no = GetMagicNo( type, mno );
	if(magic_no==-1) return ;

	if( MagicParam[magic_no].type==MAGIC_SKILL && (unit==0 || unit==3) ){
		for(i=0;i<MAGIC_CHAR_MAX;i++){
			if( skill[i]==magic_no ) return ;
			if( skill[i]>magic_no ){
				for(j=MAGIC_CHAR_MAX-1;j>i;j--){
					skill[j] = skill[j-1];
				}
				break;
			}
			if( skill[i]==-1 ) break;
		}
		if(i==MAGIC_CHAR_MAX){
			DebugBox(NULL,"もう魔法覚えきれませんエラー");
			return ;
		}

		skill[i] = magic_no;
	}else{
		if(unit==1){
			if(MagicParam[magic_no].type==MAGIC_SKILL){
				if(mno>=100){
					if(job!=29){
						return ;
					}
				}
				
			}
		}
		for(i=0;i<MAGIC_CHAR_MAX;i++){
			if( magic[i]==magic_no ) return ;
			if( magic[i]>magic_no ){
				for(j=MAGIC_CHAR_MAX-1;j>i;j--){
					magic[j] = magic[j-1];
				}
				break;
			}
			if( magic[i]==-1 ) break;
		}
		if(i==MAGIC_CHAR_MAX){
			DebugBox(NULL,"もう魔法覚えきれませんエラー");
			return ;
		}
		magic[i] = magic_no;
	}
}

void CHAR_PARAM::LuUpMagic( void )
{
#if TAIKEN_FLAG==2
	if(lv>=30)return ;
#endif

	switch(job){

		case 34:
		case 35:
			LearningMagic(  MAGIC_MONSTAR,  1 );
			LearningMagic(  MAGIC_MONSTAR,  2 );
			break;

		case 41:
			LearningMagic(  MAGIC_MONSTAR,  1 );
			break;
		case 42:
			LearningMagic(  MAGIC_MONSTAR,  2 );
			break;

		case 51:
		case 52:
		case 53:
			if(lv>=15){
				LearningMagic(  MAGIC_ZOKUSEI, 50 );
			}
		case 45:
		case 65:
			if(job==45 || job==65){
				LearningMagic(  MAGIC_SENSEI,  1 );
			}
				LearningMagic(  MAGIC_ZOKUSEI, 51 );

		case 57:
		case 58:
			if(lv>=4){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10 );
				}
			}
			if(lv>=16){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+1 );
				}
			}
			if(lv>=38){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+2 );
				}
			}
			break;
		case 97:
			LearningMagic(  MAGIC_HOLY,  0 );
			break;
		case 98:
			LearningMagic(  MAGIC_SKILL, 10 );
			break;
		case 86:
		case 99:
			LearningMagic(  MAGIC_SENSEI,  10 );
			
			LearningMagic(  MAGIC_ZOKUSEI, 12 );
			LearningMagic(  MAGIC_ZOKUSEI, 40 );
			
			LearningMagic(  MAGIC_DARK, 10 );
			break;

		case 50:
				LearningMagic(  MAGIC_SENSEI,  1 );

				LearningMagic(  MAGIC_HOLY, 10 );
				LearningMagic(  MAGIC_HOLY, 11 );
				LearningMagic(  MAGIC_DARK, 10 );	

				LearningMagic(  MAGIC_ZOKUSEI, 51 );
				break;
		case 40:
		case 67:
				LearningMagic(  MAGIC_ZOKUSEI, 51 );
		case 66:
				LearningMagic(  MAGIC_DARK, 0 );	
				LearningMagic(  MAGIC_DARK, 1 );	
				LearningMagic(  MAGIC_DARK, 10 );	
			break;

		case 48:
			LearningMagic(  MAGIC_SKILL, 10 );
			break;
		case 64:
			LearningMagic(  MAGIC_SKILL, 0 );
			LearningMagic(  MAGIC_ZOKUSEI, 12 );
			break;
		case 31:
			LearningMagic(  MAGIC_HOLY, 11 );
			LearningMagic(  MAGIC_ZOKUSEI,  2 );
			LearningMagic(  MAGIC_ZOKUSEI, 12 );
			LearningMagic(  MAGIC_ZOKUSEI, 22 );
			LearningMagic(  MAGIC_ZOKUSEI, 32 );
			break;
		case 87:
			LearningMagic(  MAGIC_SKILL, 13 );
			LearningMagic(  MAGIC_ZOKUSEI, 40 );
			break;
		case 88:
			LearningMagic(  MAGIC_SKILL, 12 );
			LearningMagic(  MAGIC_HOLY, 0 );

			break;
		case 43:	
		case 44:	
			LearningMagic(  MAGIC_SENSEI,  2 );
			break;
		case 29:
			LearningMagic(  MAGIC_SKILL, 900 );
			break;

		case 12:
			if(lv>=4){
				LearningMagic(  MAGIC_SENSEI,  1 );
			}
#if !(TAIKEN_FLAG==1)
			if(lv>=15){
				LearningMagic(  MAGIC_DARK, 1 );	
			}
			if(lv>=20){
				LearningMagic(  MAGIC_DARK, 0 );	
			}
			if(lv>=25){
				LearningMagic(  MAGIC_DARK, 10 );	
			}
			if(lv>=30){
				LearningMagic(  MAGIC_HOLY, 40 );	
			}
			if(lv>=35){
				LearningMagic(  MAGIC_ZOKUSEI, 51 );
			}

			if(0){
				LearningMagic(  MAGIC_SKILL, 10 );

				LearningMagic(  MAGIC_HOLY, 10 );
				LearningMagic(  MAGIC_HOLY, 11 );
				LearningMagic(  MAGIC_HOLY, 0 );

				LearningMagic(  MAGIC_HOLY, 20 );
				LearningMagic(  MAGIC_HOLY, 21 );
				LearningMagic(  MAGIC_HOLY, 22 );
				LearningMagic(  MAGIC_HOLY, 23 );
			}
#endif
	
			if(lv>=1)	LearningMagic(  MAGIC_SKILL, 100 );
			if(lv>=7)	LearningMagic(  MAGIC_SKILL, 101 );
#if !(TAIKEN_FLAG==1)
			if(lv>=15)	LearningMagic(  MAGIC_SKILL, 102 );
			if(lv>=22)	LearningMagic(  MAGIC_SKILL, 103 );
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 900 );
#endif
			break;

		case 13:
#if !(TAIKEN_FLAG==1)
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 901 );
#endif
		case 0:
	
			if(lv>=1)	LearningMagic(  MAGIC_SKILL, 200 );
#if !(TAIKEN_FLAG==1)
			if(lv>=7)	LearningMagic(  MAGIC_SKILL, 201 );
			if(lv>=15)	LearningMagic(  MAGIC_SKILL, 202 );
			if(lv>=22)	LearningMagic(  MAGIC_SKILL, 203 );
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 900 );
#endif
			break;
		case 6:
	
			if(lv>=1)	LearningMagic(  MAGIC_SKILL, 100 );
			if(lv>=7)	LearningMagic(  MAGIC_SKILL, 101 );
			if(lv>=15)	LearningMagic(  MAGIC_SKILL, 102 );
			if(lv>=22)	LearningMagic(  MAGIC_SKILL, 103 );
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 900 );
			break;
		case 21:
		case 28:
			break;

		case 17:
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 900 );
		case 4:
	
			if(lv>=1)	LearningMagic(  MAGIC_SKILL, 300 );
			if(lv>=10)	LearningMagic(  MAGIC_SKILL, 301 );
			if(lv>=20)	LearningMagic(  MAGIC_SKILL, 302 );
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 903 );
			break;
		case 10:
	
			if(lv>=1)	LearningMagic(  MAGIC_SKILL, 400 );
			if(lv>=10)	LearningMagic(  MAGIC_SKILL, 401 );
			if(lv>=20)	LearningMagic(  MAGIC_SKILL, 402 );
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 903 );
			break;

		case 15:
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 900 );
		case 5:
		case 11:
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 903 );
			if(lv>=20){
				LearningMagic(  MAGIC_SENSEI,  10 );
			}

			if(lv>= 1)	LearningMagic(  MAGIC_BARD, 0 );
			if(lv>= 5)	LearningMagic(  MAGIC_BARD, 1 );
			if(lv>=10)	LearningMagic(  MAGIC_BARD, 2 );
			if(lv>=15)	LearningMagic(  MAGIC_BARD, 3 );
			if(lv>=20)	LearningMagic(  MAGIC_BARD, 4 );
			if(lv>=25)	LearningMagic(  MAGIC_BARD, 5 );
			if(lv>=30)	LearningMagic(  MAGIC_BARD, 6 );
			if(lv>=35)	LearningMagic(  MAGIC_BARD, 7 );
			if(lv>=40)	LearningMagic(  MAGIC_BARD, 8 );
			if(lv>=45)	LearningMagic(  MAGIC_BARD, 9 );
			if(lv>=50)	LearningMagic(  MAGIC_BARD, 10 );

			break;

		case 7:
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 900 );
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 901 );

		case 22:
		case 23:
			if(lv>=5){
				LearningMagic(  MAGIC_SENSEI,  0 );
			}
			if(lv>=20){
				LearningMagic(  MAGIC_SENSEI,  1 );
			}
			if(lv>=50){
				LearningMagic(  MAGIC_SENSEI,  2 );
			}
			break;
			

		case 3:
		case 16:
#if !(TAIKEN_FLAG==1)
			if(lv>=4){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10 );
				}
			}
			if(lv>=16){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+1 );
				}
			}
			if(lv>=38){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+2 );
				}
			}
			if(lv>=29){
				LearningMagic(  MAGIC_SENSEI,  20 );
			}
			if(lv>=50){
				LearningMagic(  MAGIC_SENSEI,  21 );
			}
#endif
		case 2:
			if(lv>=1){
				LearningMagic(  MAGIC_SENSEI,  0 );
			}
#if !(TAIKEN_FLAG==1)
			if(lv>=7){
				LearningMagic(  MAGIC_SENSEI,  1 );
			}
			if(lv>=21){
				LearningMagic(  MAGIC_SENSEI,  2 );
			}
			if(lv>=30){
				LearningMagic(  MAGIC_SENSEI,  10 );
			}
			if(lv>=42){
				LearningMagic(  MAGIC_SENSEI,  20 );
			}

			if(lv>=5){
				LearningMagic(  MAGIC_SENSEI,  42 );
			}
			if(lv>=5){
				LearningMagic(  MAGIC_SENSEI,  43 );
			}
			if(lv>=15){
				LearningMagic(  MAGIC_SENSEI,  40 );
			}
			if(lv>=15){
				LearningMagic(  MAGIC_SENSEI,  41 );
			}
			if(lv>=25){
				LearningMagic(  MAGIC_SENSEI,  30 );
			}
#endif
			break;

		case 20:
			if(lv>=10){
				LearningMagic(  MAGIC_MONSTAR,  3 );
			}
			if(lv>=10){
				LearningMagic(  MAGIC_MONSTAR,  2 );
			}
			if(lv>=4){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10 );
				}
			}
			if(lv>=12){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+1 );
				}
			}
			if(lv>=35){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+2 );
				}
			}
			if(lv>=15){
				LearningMagic(  MAGIC_ZOKUSEI, 50 );
			}
			if(lv>=20){
				LearningMagic(  MAGIC_ZOKUSEI, 51 );
			}
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 902 );
			break;

		case 19:
			if(lv>=10){
				LearningMagic(  MAGIC_MONSTAR,  1 );
			}
			if(lv>=10){
				LearningMagic(  MAGIC_MONSTAR,  3 );
			}
		case 1:
			if(lv>=4){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10 );
				}
			}
			if(lv>=12){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+1 );
				}
			}
			if(lv>=35){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+2 );
				}
			}
			if(lv>=8){
				LearningMagic(  MAGIC_ZOKUSEI,  62 );
			}
			if(lv>=8){
				LearningMagic(  MAGIC_ZOKUSEI,  63 );
			}
			if(lv>=23){
				LearningMagic(  MAGIC_ZOKUSEI,  60 );
			}
			if(lv>=23){
				LearningMagic(  MAGIC_ZOKUSEI,  61 );
			}

			if(lv>=15){
				LearningMagic(  MAGIC_ZOKUSEI, 50 );
			}
			if(lv>=20){
				LearningMagic(  MAGIC_ZOKUSEI, 51 );
			}
			if(job==1){
				if(lv>=30)	LearningMagic(  MAGIC_SKILL, 900 );
			}
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 901 );
			break;
		case 14:
			if(lv>=1){
				LearningMagic(  MAGIC_SENSEI,  0 );
			}
		case 9:
#if !(TAIKEN_FLAG==1)
			if(lv>=20){
				LearningMagic(  MAGIC_ZOKUSEI, 50 );
			}
			if(lv>=30){
				LearningMagic(  MAGIC_ZOKUSEI, 51 );
			}
			if(lv>=45){
				LearningMagic(  MAGIC_ZOKUSEI, 40 );
			}
			if(lv>=25){
				LearningMagic(  MAGIC_DARK, 10 );	
			}
#endif
		case 8:
			if(lv>=1){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10 );
				}
			}
#if (TAIKEN_FLAG==1)
			if(lv>=3){
				LearningMagic(  MAGIC_ZOKUSEI,  0 );
				LearningMagic(  MAGIC_ZOKUSEI, 10 );
				LearningMagic(  MAGIC_ZOKUSEI, 20 );
				LearningMagic(  MAGIC_ZOKUSEI, 30 );
			}
#else
			if(lv>=10){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+1 );
				}
			}
			if(lv>=30){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+2 );
				}
			}

			if(lv>=5){
				LearningMagic(  MAGIC_ZOKUSEI,  0 );
				LearningMagic(  MAGIC_ZOKUSEI, 10 );
				LearningMagic(  MAGIC_ZOKUSEI, 20 );
				LearningMagic(  MAGIC_ZOKUSEI, 30 );
			}
			if(lv>=15){
				LearningMagic(  MAGIC_ZOKUSEI,  1 );
				LearningMagic(  MAGIC_ZOKUSEI, 11 );
				LearningMagic(  MAGIC_ZOKUSEI, 21 );
				LearningMagic(  MAGIC_ZOKUSEI, 31 );
			}
			if(lv>=35){
				LearningMagic(  MAGIC_ZOKUSEI,  2 );
				LearningMagic(  MAGIC_ZOKUSEI, 12 );
				LearningMagic(  MAGIC_ZOKUSEI, 22 );
				LearningMagic(  MAGIC_ZOKUSEI, 32 );
			}

			if(lv>=6){
				LearningMagic(  MAGIC_ZOKUSEI,  62 );
			}
			if(lv>=6){
				LearningMagic(  MAGIC_ZOKUSEI,  63 );
			}
			if(lv>=17){
				LearningMagic(  MAGIC_ZOKUSEI,  60 );
			}
			if(lv>=17){
				LearningMagic(  MAGIC_ZOKUSEI,  61 );
			}
#endif
			break;
		case 18:
			if(lv>=15){
				LearningMagic(  MAGIC_HOLY, 10 );
			}
			if(lv>=25){
				LearningMagic(  MAGIC_HOLY, 11 );
			}
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 900 );
	
			if(lv>=30)	LearningMagic(  MAGIC_SKILL, 901 );

		case 24:
		case 25:
			if(lv>=7){
				LearningMagic(  MAGIC_SENSEI,  1 );
			}

			if(lv>=8){
				LearningMagic(  MAGIC_ZOKUSEI,  62 );
			}
			if(lv>=8){
				LearningMagic(  MAGIC_ZOKUSEI,  63 );
			}
			if(lv>=23){
				LearningMagic(  MAGIC_ZOKUSEI,  60 );
			}
			if(lv>=23){
				LearningMagic(  MAGIC_ZOKUSEI,  61 );
			}

			if(lv>=10){
				LearningMagic(  MAGIC_SENSEI,  42 );
			}
			if(lv>=10){
				LearningMagic(  MAGIC_SENSEI,  43 );
			}
			if(lv>=20){
				LearningMagic(  MAGIC_SENSEI,  40 );
			}
			if(lv>=20){
				LearningMagic(  MAGIC_SENSEI,  41 );
			}
			if(lv>=30){
				LearningMagic(  MAGIC_SENSEI,  30 );
			}

			break;


		case 27:
		case 30:
			if(lv>=4){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10 );
				}
			}
			if(lv>=16){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+1 );
				}
			}
			if(lv>=42){
				if( ATTR_KA<=attr && attr<=ATTR_HUU ){
					LearningMagic(  MAGIC_ZOKUSEI, (attr-1)*10+2 );
				}
			}
		case 26:
			if(lv>=1){
				LearningMagic(  MAGIC_SENSEI,  0 );
			}
			if(lv>=7){
				LearningMagic(  MAGIC_SENSEI,  1 );
			}
			if(lv>=21){
				LearningMagic(  MAGIC_SENSEI,  2 );
			}
			if(lv>=10){
				LearningMagic(  MAGIC_HOLY, 10 );
			}
			if(lv>=25){
				LearningMagic(  MAGIC_HOLY, 11 );
			}
			break;
	}
}
void SetCharDir( int i, double radian )
{
	if( JobParam[CharParam[CharControl[i].cno].job].ex&EX_BOSS2 ){
	}else{
		CharControl[i].dir = ( (int)((radian/3.141592+1.0)*8+1)/2 + 2)%8;
	}
}


void NewYouhei( void )
{
	int	j;
	for(j=9;j<32;j++){
		if(!CharParam[j].flag){
			memcpy( &CharParam[j], &CharParam[32] , sizeof(CHAR_PARAM) );
			ZeroMemory( &JobParam2[j], sizeof(JOB_PARAM2) );
			break;
		}
	}
}
int NokoriYouhei( void )
{
	int	j,i=0;
	for(j=9;j<32;j++){
		if(!CharParam[j].flag){
			i++;
		}
	}
	return i;
}

void ChangeCharJob32( int cno, int job_no )
{
	int	j;
	ResetCharParam( 32 );
	for(j=0;j<MAGIC_CHAR_MAX;j++){
		CharParam[32].skill[j] = CharParam[cno].skill[j];
	}
	for(j=0;j<MAGIC_CHAR_MAX;j++){
		CharParam[32].magic[j] = CharParam[cno].magic[j];
	}

	JobParam2[32].hp  = max( JobParam2[cno].hp,  sqrt( (float)CharParam[cno].hp )-3 );
	JobParam2[32].mp  = max( JobParam2[cno].mp,  sqrt( (float)CharParam[cno].mp )-3 );
	JobParam2[32].str = max( JobParam2[cno].str, sqrt( (float)CharParam[cno].str)-3 );
	JobParam2[32].vit = max( JobParam2[cno].vit, sqrt( (float)CharParam[cno].vit)-3 );
	JobParam2[32].dex = max( JobParam2[cno].dex, sqrt( (float)CharParam[cno].dex)-3 );
	JobParam2[32].spd = max( JobParam2[cno].spd, sqrt( (float)CharParam[cno].spd)-3 );
	JobParam2[32].wiz = max( JobParam2[cno].wiz, sqrt( (float)CharParam[cno].wiz)-3 );
	JobParam2[32].mnd = max( JobParam2[cno].mnd, sqrt( (float)CharParam[cno].mnd)-3 );

	CharParam[32].job = job_no;
	CharParam[32].job2 = 32;

	CharParam[32].flag=TRUE;	
	CharParam[32].unit=0;		

	_mbscpy( CharParam[32].name, CharParam[cno].name );
	CharParam[32].attr = CharParam[cno].attr;

	int buki=1;
	if(JobParam[job_no].buki_type[buki]==255)	buki = 0;
	switch( JobParam[job_no].buki_type[buki] ){
	default:				CharParam[32].buki = 0;	break;	
	case ITEM_BUKI_SWORD:	CharParam[32].buki = 1;	break;	
	case ITEM_BUKI_SPEAR:	CharParam[32].buki = 2;	break;	
	case ITEM_BUKI_HAMMER:	CharParam[32].buki = 3;	break;	
	case ITEM_BUKI_BOW:		CharParam[32].buki = 4;	break;	
	case ITEM_BUKI_WAND:	CharParam[32].buki = 5;	break;	
	}
	
	CharParam[32].SetLvUp(FALSE);
	CharParam[32].SetLvUp(FALSE);
	for(j=0;j<1;j++){
		CharParam[32].SetLvUp(TRUE);
	}
	CharParam[32].mov = JobParam[job_no].mov;	


	CharParam[32].php = CharParam[32].GetHp();
	CharParam[32].pmp = CharParam[32].GetMp();
}
void ChangeCharJob( int cno )
{
	SetCharEquip( cno, 0, 0 );
	SetCharEquip( cno, 1, 0 );
	SetCharEquip( cno, 2, 0 );
	SetCharEquip( cno, 3, 0 );

	ZeroMemory( &CharThink[cno], sizeof(CHAR_THINK_PARAM) );

	memcpy( &CharParam[cno], &CharParam[32] , sizeof(CHAR_PARAM) );
	memcpy( &JobParam2[cno], &JobParam2[32] , sizeof(JOB_PARAM2) );
	CharParam[cno].job2 = cno;
}

void ResetCharParam( int cno )
{
	ZeroMemory( &CharParam[cno], sizeof(CHAR_PARAM) );
	if(cno<32+1){
		ZeroMemory( &JobParam2[cno], sizeof(JOB_PARAM2) );
	}
}

void SetCharParam( int cno, int job_no, int lv, int buki, int unit, char *name )
{
	int	j;
	ResetCharParam( cno );
	for(j=0;j<MAGIC_CHAR_MAX;j++){
		CharParam[cno].magic[j] = -1;
	}
	for(j=0;j<MAGIC_CHAR_MAX;j++){
		CharParam[cno].skill[j] = -1;
	}

	if(lv==-1){
		lv=1;
		for(j=0;j<32;j++){
			if(CharParam[j].flag){
				if(CharParam[j].unit==0){
					if(CharParam[j].lv>lv){
						lv=CharParam[j].lv;
					}
				}
			}
		}
		if( ESC_GetFlag(5) ){
		}else{
			lv = LIM( lv-1, 1, max(1,ESC_GetFlag(4))*5 );
		}
	}else if(lv==-2){
		lv=1;
		for(j=0;j<32;j++){
			if(CharParam[j].flag){
				if(CharParam[j].lv>lv){
					lv=CharParam[j].lv;
				}
			}
		}
	}

	if(job_no==-1){
		job_no = 21+rand()%40;
	}
	CharParam[cno].job = job_no;

	CharParam[cno].flag=TRUE;	
	CharParam[cno].unit=unit;		

	if(name){
		_mbscpy( CharParam[cno].name, (BYTE*)name );
	}else{
		_mbscpy( CharParam[cno].name, JobParam[job_no].name );
	}


	if(JobParam[job_no].attr==ATTR_UNKNOWN){
		CharParam[cno].attr = ATTR_KA+rand()%4;
	}else{
		CharParam[cno].attr = JobParam[job_no].attr;
	}

	if(buki<=-1 || buki>=2){
		CharParam[cno].buki = 0;
	}else{
		if(JobParam[job_no].buki_type[buki]==255 && buki==1){
			buki = 0;
		}
		switch( JobParam[job_no].buki_type[buki] ){
		default:				CharParam[cno].buki = 0;	break;	
		case ITEM_BUKI_SWORD:	CharParam[cno].buki = 1;	break;	
		case ITEM_BUKI_SPEAR:	CharParam[cno].buki = 2;	break;	
		case ITEM_BUKI_HAMMER:	CharParam[cno].buki = 3;	break;	
		case ITEM_BUKI_BOW:		CharParam[cno].buki = 4;	break;	
		case ITEM_BUKI_WAND:	CharParam[cno].buki = 5;	break;	
		}
	}
	
	CharParam[cno].SetLvUp(FALSE);
	CharParam[cno].SetLvUp(FALSE);
	for(j=0;j<lv;j++){
		CharParam[cno].SetLvUp(TRUE);
	}

	CharParam[cno].mov = JobParam[job_no].mov;	


	CharParam[cno].php = CharParam[cno].GetHp();
	CharParam[cno].pmp = CharParam[cno].GetMp();
}
void SetCharParamName( int cno, char *name )
{
	_mbscpy( CharParam[cno].name, (BYTE*)name );
}

void GetKunrenExp( int lv )
{
	int	i, j, exp;

	for(j=0,i=9;i<32;i++){
		if(CharParam[i].flag && CharParam[i].unit==3){
			j++;
		}
	}
	if(j){
		for(i=9;i<32;i++){
			if(CharParam[i].flag && CharParam[i].unit==3){
				float exp_rate = (lv-CharParam[i].lv+5)/5.0f;
				exp_rate = LIM( exp_rate, 0.1, 10); 
				exp = (lv+10)*(lv)*exp_rate;
				exp = max( 1, exp/j/30 );

				CharParam[i].AddExp(exp, 0 );
			}
		}
	}
}
int CHAR_PARAM::GetNextExp(void)
{
	int	next_exp = 0, i;
	
	for(i=0;i<lv;i++){
		next_exp += (i+2)*10 + (i+1)*10;
	}

	return next_exp;
}

int CHAR_PARAM::AddExp( int add, int tlv )
{
	if( ESC_GetFlag(82) ){
		return 0;
	}

	int	lvup=0;
	int	next_exp = GetNextExp();

	if(tlv){
		GetKunrenExp( tlv );
	}

	add = max(1,add);
	while( exp+add >= next_exp ){
		if(lv==99){
			return lvup;
		}

		add -= next_exp-exp;

		exp = 0;
		SetLvUp();
		lvup++;
		next_exp = GetNextExp();
	}

	exp = exp+add;
	return lvup;
}
int CHAR_PARAM::AddExpAll( int tlv, int tdid )
{
	int	lvup=0;
	if(tlv){
		if(unit==0){
			float exp_rate;
			if(tlv<=lv) exp_rate = (tlv-lv+5)/5.0f;
			else		exp_rate = (pow(tlv-lv,2)+5)/5.0f;

			exp_rate = LIM( exp_rate, 0.1f, 10.0f); 
			int	add_exp = (int)( (tlv+10)*(tlv+1)*exp_rate );

			if( tdid ){
				lvup = AddExp( add_exp, tlv );
			}else{
				lvup = AddExp( add_exp/5, tlv );
			}
			GetBardExp( no, tlv );
		}
	}
	return lvup;
}

#define	LVUP_PARAM(WRK,LV,NAN)	(((WRK)+rand()%((WRK)/6+8))*(NAN)/8)


void CHAR_PARAM::SetLvUp( int flag )
{
	int		work;
	float	nanido=1.0f;
	float	nanido2=1.0f;

	if(lv>=99) return ;
	if(unit==1){
		if( lv<5){
			switch( ESC_GetFlag(3) ){
				case 1:	nanido=0.9f;	nanido2=0.8f;	break;
				case 2:	nanido=1.0f;	nanido2=0.9f;	break;
				case 0:
				case 3:	nanido=1.0f;	nanido2=1.0f;	break;
				case 4:	nanido=1.1f;	nanido2=1.1f;	break;
				case 5:	nanido=1.2f;	nanido2=1.1f;	break;
			}
		}else if(lv<20 || lv>50 ){
			switch( ESC_GetFlag(3) ){
				case 1:	nanido=1.0f;	nanido2=1.0f;	break;
				case 2:	nanido=1.1f;	nanido2=1.1f;	break;
				case 0:
				case 3:	nanido=1.3f;	nanido2=1.2f;	break;
				case 4:	nanido=1.5f;	nanido2=1.3f;	break;
				case 5:	nanido=1.7f;	nanido2=1.4f;	break;
			}
		}else{
			switch( ESC_GetFlag(3) ){
				case 1:	nanido=1.1f;	nanido2=1.1f;	break;
				case 2:	nanido=1.3f;	nanido2=1.2f;	break;
				case 0:
				case 3:	nanido=1.6f;	nanido2=1.3f;	break;
				case 4:	nanido=1.8f;	nanido2=1.5f;	break;
				case 5:	nanido=2.0f;	nanido2=1.7f;	break;
			}
		}
	}
	int	ihp=0;
	int	imp=0;
	int	istr=0;
	int	ivit=0;
	int	idex=0;
	int	ispd=0;
	int	iwiz=0;
	int	imnd=0;

	if(ItemParam[accessary2].type==ITEM_ACCE_NECKLACE){
		ihp  = ItemParam[accessary2].hp;
		imp  = ItemParam[accessary2].mp;

		istr = ItemParam[accessary2].str;
		ivit = ItemParam[accessary2].vit;

		idex = ItemParam[accessary2].dex;
		ispd = ItemParam[accessary2].spd;

		iwiz = ItemParam[accessary2].wiz;
		imnd = ItemParam[accessary2].mnd;
	}

	work = JobParam2[job2].hp+JobParam[job].hp+ihp;
	work = LVUP_PARAM(work,lv,1.0f);		
	work = max(work,1);
	hp += work;
	php += work;
	hp = min(hp,99999);
	php = min(php,99999);

	work = JobParam2[job2].mp+JobParam[job].mp+imp;
	work = LVUP_PARAM(work,lv,1.0f);		
	work = max(work,1);
	mp += work;
	pmp += work;
	mp = min(mp,99999);
	pmp = min(pmp,99999);

	work = JobParam2[job2].str+JobParam[job].str+istr;	
	work = LVUP_PARAM(work,lv,nanido);
	work = max(work,1);
	str += work;
	str = min(str,99999);

	work = JobParam2[job2].vit+JobParam[job].vit+ivit;	
	work = LVUP_PARAM(work,lv,nanido);
	work = max(work,1);
	vit += work;
	vit = min(vit,99999);

	work = JobParam2[job2].dex+JobParam[job].dex+idex;	
	work = LVUP_PARAM(work,lv,nanido2);
	work = max(work,1);
	dex += work;
	dex = min(dex,99999);

	work = JobParam2[job2].spd+JobParam[job].spd+ispd;	
	work = LVUP_PARAM(work,lv,1.0f);
	work = max(work,1);
	spd += work;
	spd = min(spd,99999);

	work = JobParam2[job2].wiz+JobParam[job].wiz+iwiz;	
	work = LVUP_PARAM(work,lv,nanido2);
	work = max(work,1);
	wiz += work;
	wiz = min(wiz,99999);

	work = JobParam2[job2].mnd+JobParam[job].mnd+imnd;	
	work = LVUP_PARAM(work,lv,nanido);
	work = max(work,1);
	mnd += work;
	mnd = min(mnd,99999);

	if(flag){
		lv++;

		if( 50<=lv ){
			switch(buki){
			case 284:
			case 285:
			case 286:
			case 287:
			case 288:
			case 289:
			case 290:
			case 291:
			case 292:
				ITM_GetItem( buki, 1 );
				switch(ItemParam[buki].type){
				case ITEM_BUKI_SWORD:	buki = 1;	break;	
				case ITEM_BUKI_SPEAR:	buki = 2;	break;	
				case ITEM_BUKI_HAMMER:	buki = 3;	break;	
				case ITEM_BUKI_BOW:		buki = 4;	break;	
				case ITEM_BUKI_WAND:	buki = 5;	break;	
				}
				break;
			}
		}
		LuUpMagic();
	}
}


void TestSetParam( int lv )
{
	int	i,j;


	for(i=0;i<32;i++){
		CharParam[i].party = 0;
		CharParam[i].hp = 0;
		CharParam[i].mp = 0;

		CharParam[i].str = 0;
		CharParam[i].vit = 0;
		CharParam[i].dex = 0;
		CharParam[i].spd = 0;
		CharParam[i].wiz = 0;
		CharParam[i].mnd = 0;

		CharParam[i].did    = 0;
		CharParam[i].hujimi = 0;

		CharParam[i].job2 = 0;	
		CharParam[i].exp  = 0;	

		for(j=0;j<MAGIC_CHAR_MAX;j++){
			CharParam[i].magic[j] = -1;
		}
		for(j=0;j<MAGIC_CHAR_MAX;j++){
			CharParam[i].skill[j] = -1;
		}


		CharParam[i].lv=0;		

		CharParam[i].SetLvUp(FALSE);
		CharParam[i].SetLvUp(FALSE);
		for(j=0;j<lv;j++){
			CharParam[i].SetLvUp(TRUE);
		}
		CharParam[i].LuUpMagic();
		CharParam[i].mov = JobParam[CharParam[i].job].mov;	


		CharParam[i].php = CharParam[i].GetHp();
		CharParam[i].pmp = CharParam[i].GetMp();

		CharParam[i].ResetStatusEffct();

		CharParam[i].buki=0;		
		CharParam[i].yoroi=0;		
		CharParam[i].accessary1=0;	
		CharParam[i].accessary2=0;	
	}
	CharParam[0].buki=42;
	CharParam[1].buki=1;
	CharParam[2].buki=5;
	CharParam[3].buki=5;
	CharParam[4].buki=4;
	CharParam[5].buki=2;
	CharParam[6].buki=4;
	CharParam[7].buki=2;
	CharParam[8].buki=3;

	CharParam[ 9].buki=1;
	CharParam[10].buki=2;
	CharParam[11].buki=5;
	CharParam[12].buki=5;
	CharParam[13].buki=4;
	CharParam[14].buki=4;

	CharParam[15].buki=1;
	CharParam[16].buki=2;
	CharParam[17].buki=5;
	CharParam[18].buki=5;
	CharParam[19].buki=4;
	CharParam[20].buki=4;

}

void AllPartyHeal( void )
{
	int	i,j;

	for(i=0;i<32;i++){
		if(!CharParam[i].flag) continue;

		CharParam[i].php = CharParam[i].GetHp();
		CharParam[i].pmp = CharParam[i].GetMp();

		CharParam[i].did = 0;

		CharParam[i].ResetStatusEffct();
	}
	
}
int Change_Pno2Cno( int cpno )
{
	int	cno=-1;
	switch(cpno){
	case 0:		cno=0;	break;
	case 1:		cno=2;	break;
	case 2:		cno=5;	break;
	case 3:		cno=4;	break;
	case 5:		cno=7;	break;
	case 8:		cno=8;	break;
	case 10:	cno=1;	break;
	case 11:	cno=3;	break;
	case 12:	cno=6;	break;
	}
	return cno;
}
int Change_Jno2Pno( int jno )
{
	int	cpno=-1;
	switch(jno){
	case 12:		cpno=0;		break;
	case 16:		cpno=1;		break;
	case 18:		cpno=2;		break;
	case 17:		cpno=3;		break;
	case 19:		cpno=5;		break;
	case 20:		cpno=8;		break;
	case 13:		cpno=10;	break;
	case 14:		cpno=11;	break;
	case 15:		cpno=12;	break;
	case 29:		cpno=21;	break;
	}
	return cpno;
}


int GetPartyLevel( int cpno )
{
	int	cno=Change_Pno2Cno( cpno );
	if(cno==-1){
		return 0;
	}
	return CharParam[cno].lv;
}

void SetPartyChar( int cpno, int disp, int lv, int equip )
{
	if(cpno==-1){
		SetPartyChar( 0, disp, lv );
		SetPartyChar( 1, disp, lv );
		SetPartyChar( 2, disp, lv );
		SetPartyChar( 3, disp, lv );
		SetPartyChar( 5, disp, lv );
		SetPartyChar( 8, disp, lv );
		SetPartyChar( 10, disp, lv );
		SetPartyChar( 11, disp, lv );
		SetPartyChar( 12, disp, lv );
		return ;
	}

	int	cno=Change_Pno2Cno( cpno );
	if(cno==-1){
		DebugBox(NULL,"%d", cpno );
		return ;
	}

	if(disp){
		if(CharParam[cno].party==0){
			CharParam[cno].party= 1;

			while(CharParam[cno].lv<lv){
				CharParam[cno].SetLvUp();
			}
		}
		CharParam[cno].flag = disp;
	}else{
		if(equip==0){
			SetCharEquip( cno, 0, 0 );
			SetCharEquip( cno, 1, 0 );
			SetCharEquip( cno, 2, 0 );
			SetCharEquip( cno, 3, 0 );
		}
		CharParam[cno].flag = disp;
	}
	
}



typedef struct{
	BYTE	flag;
	short	job_no;
	short	buki_no;
}CHAR_LOAD_TABLE;
CHAR_LOAD_TABLE		CharLoadTable[CHAR_PARAM_MAX];

short LoadMapChar( int cno )
{
	char	fname[MAX_PATH];
	short	i;

	for(i=0;i<CHAR_PARAM_MAX;i++){
		if(CharLoadTable[i].flag){
			if( CharLoadTable[i].job_no == CharParam[cno].GetJobFileNo() && CharLoadTable[i].buki_no == CharParam[cno].GetBukiFileNo() ){
				break;
			}
		}
	}
	if(i==CHAR_PARAM_MAX){
		for(i=0;i<CHAR_PARAM_MAX;i++){
			if(CharLoadTable[i].flag==0){
				if( (CharParam[cno].GetJobFileNo() >=200 && CharParam[cno].GetJobFileNo() <=225)	
				 || (CharParam[cno].GetJobFileNo() >=400)	
				 || (CharParam[cno].GetJobFileNo() >=314) ){ 
					wsprintf( fname, "an%03d%03d0l1.ani", CharParam[cno].GetJobFileNo(), CharParam[cno].GetBukiFileNo() );
				}else{
					int	mahura=0;
					if( CharParam[cno].GetJobFileNo() == 0 ){
						switch( ESC_GetFlag(2) ){
						case 0:	mahura = 100;	break;
						case 1:	mahura =   0;	break;
						case 2:	mahura = 200;	break;
						case 3:	mahura = 300;	break;
						}
					}

					switch(DrawSetting.low_level){
						default:
						case 0:	wsprintf( fname, "an%03d%03d0l1.ani", CharParam[cno].GetJobFileNo(), CharParam[cno].GetBukiFileNo()+mahura );	break;
						case 1:	wsprintf( fname, "an%03d%03d0l2.ani", CharParam[cno].GetJobFileNo(), CharParam[cno].GetBukiFileNo()+mahura );	break;
					}
				}


				char	*tname;
				if( PAC_CheckFile( D3DD_GetPackDir(), bak_tone_fname ) ){
					tname = bak_tone_fname;						
				}else{
					tname = time_tone_fname[BackStruct.time_no];
				}

				if( GD_LoadSprite( SPR_MAP_CHAR+i, fname, ALPHA_4444, FALSE, DrawSetting.mip_map, tname, eff_tone_fname_c, BackStruct.chroma_char, "btl" ) ){
					CharLoadTable[i].flag = TRUE;
					CharLoadTable[i].job_no  = CharParam[cno].GetJobFileNo();
					CharLoadTable[i].buki_no = CharParam[cno].GetBukiFileNo();

					GD_LoadTexture( BMP_EFFECT+2, "魔方陣3.tga", ALPHA_4444 );
				}
				break;
			}
		}
	}
	return i;
}


int	SetCharEquip( int cno, int eq_type, int ino )
{
	switch(eq_type){
		case 0:
			if(ino==0 && CharParam[ cno ].buki<6 ) break;

			ITM_GetItem( CharParam[ cno ].buki, 1 );
			if(ino==0){
				switch(ItemParam[CharParam[ cno ].buki].type){
				case ITEM_BUKI_SWORD:	CharParam[ cno ].buki = 1; break;	
				case ITEM_BUKI_SPEAR:	CharParam[ cno ].buki = 2; break;	
				case ITEM_BUKI_HAMMER:	CharParam[ cno ].buki = 3; break;	
				case ITEM_BUKI_BOW:		CharParam[ cno ].buki = 4; break;	
				case ITEM_BUKI_WAND:	CharParam[ cno ].buki = 5; break;	
				}
			}else{
				CharParam[ cno ].buki = ino;
			}
			break;
		case 1:
			ITM_GetItem( CharParam[ cno ].yoroi, 1 );
			CharParam[ cno ].yoroi = ino;
			break;
		case 2:
			ITM_GetItem( CharParam[ cno ].accessary1, 1 );
			CharParam[ cno ].accessary1 = ino;
			break;
		case 3:
			ITM_GetItem( CharParam[ cno ].accessary2, 1 );
			CharParam[ cno ].accessary2 = ino;
			break;
		default:
			DebugBox( NULL, "それは装備品ではありません" );
			return 0;
			break;
	}
	ITM_LostItem( ino, 1 );
	return 1;
}

int	CharEquipCheck( int cno, int ino, int eq_type )
{
	int	ret=0;

	if( cno==0 || cno==3 || cno==7 || cno==8 ){
		if( ino==19 || ino==37 || ino==63 || ino==139 || ino==221 || ino==257 ){
			return 0;
		}
	}
	
	switch(ino){
	case 284:
		if(cno!=0 || (50<=CharParam[cno].lv) )return 0;
		break;
	case 285:
		if(cno!=1 || (50<=CharParam[cno].lv) )return 0;
		break;
	case 286:
		if(cno!=2 || (50<=CharParam[cno].lv) )return 0;
		break;
	case 287:
		if(cno!=3 || (50<=CharParam[cno].lv) )return 0;
		break;
	case 288:
		if(cno!=4 || (50<=CharParam[cno].lv) )return 0;
		break;
	case 289:
		if(cno!=6 || (50<=CharParam[cno].lv) )return 0;
		break;
	case 290:
		if(cno!=5 || (50<=CharParam[cno].lv) )return 0;
		break;
	case 291:
		if(cno!=7 || (50<=CharParam[cno].lv) )return 0;
		break;
	case 292:
		if(cno!=8 || (50<=CharParam[cno].lv) )return 0;
		break;
	}
	switch(eq_type){
		case 0:
			ret = ItemParam[ino].type==JobParam[CharParam[cno].job].buki_type[0] || ItemParam[ino].type==JobParam[CharParam[cno].job].buki_type[1];
			break;
		case 1:
			ret = ITEM_YOROI_ARMOUR1<=ItemParam[ino].type && ItemParam[ino].type<=ITEM_YOROI_OTHER;
			break;
		case 2:
			ret = ITEM_ACCE_RING<=ItemParam[ino].type && ItemParam[ino].type<=ITEM_ACCE_OTHER1;
			break;
		case 3:
			ret = ITEM_ACCE_BOOTS<=ItemParam[ino].type && ItemParam[ino].type<=ITEM_ACCE_OTHER2;
			break;
	}
	return ret;
}
int	NotSellCheck( int ino )
{
	if(ITEM_TOOL_EVENT==ItemParam[ino].type) return 0;	
	if(ItemParam[ino].shop>=50) return 0;	
	if(ItemParam[ino].buy==0) return 0;	

	return 1;
}

void ReleaseMapChar( int ano )
{
	if(CharLoadTable[ano].flag){
		GD_ReleaseSprite( SPR_MAP_CHAR+ano );
	}
	ZeroMemory( &CharLoadTable[ano], sizeof(CHAR_LOAD_TABLE ) );
}

void ResetMapChar( int no )
{
	int	i;
	if(CharControl[no].flag){
		GD_ResetGraph( GRP_MAP_CHAR+no );	
		GD_ResetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX+no );	
		GD_ResetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no );	
		GD_ResetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX*3+no );	
		GD_ResetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+no );	
		GD_ResetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX*5+no );	

		GD_ReleaseTexture( BMP_MAP_CHAR+no );

		for(i=0;i<CHAR_CONTROL_MAX;i++){
			if(i==no) continue;
			if(CharControl[i].flag){
				if(CharControl[no].ano==CharControl[i].ano){
					break;
				}
			}
		}
		if(i==CHAR_CONTROL_MAX){
			ReleaseMapChar( CharControl[no].ano );
		}
		if(CharControl[no].ano2){
			CharControl[no].ano2 = 0;
			GD_ReleaseSprite( SPR_MAP_CHAR2+CharControl[no].cno );
		}

	}
	GFree( CharControl[no].char_sinkin );
	GFree( CharControl[no].move_tbl );
	ZeroMemory( &CharControl[no], sizeof(CHAR_CONTROL) );
}

void ResetMapCharAll( void )
{
	int	i;
	for(i=0;i<CHAR_CONTROL_MAX;i++){
		ResetMapChar( i );
		CharMarkerSelect[i]=0;		
		CharMarkerAtack[i]=0;		
	}
	CharMarkerPlayer = FALSE;	
	CharMarkerEnemy  = FALSE;	
	CharMarkerBoss   = -1;
}
void SetMapCharSpriteModeAll( int mode )
{
	int	i;

	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if( !CharControl[i].flag && !CharParam[CharControl[i].cno].did ) continue;
		GD_SetSpriteMode( GRP_MAP_CHAR+i, mode );
	}
	
}


typedef struct{
	int flag;
	int	cno;
	int	job;
	int	buki;

	int	px;
	int	py;
	int	dir;
	int	lv;
	int	attr;
	int	unit;
	char	name[32];

	int	engun;
	int	etime;	
	int	inter;
	int	etime_cnt;
	int	loop;	
	int	loop_cnt;

	int	move_think;
	int	action_think;
	int	tx;
	int	ty;
	int	range;

	int	rank_no;
	int	item_no;
}BACKUP_MAPCHAR;
BACKUP_MAPCHAR	BackUpMapChar[CHAR_CONTROL_MAX];

int CheckBackUpMapCharPlayer( int cno )
{
	int	k;
	for(k=0;k<10;k++){
		if(BackUpMapChar[k].flag && BackUpMapChar[k].cno==cno ){
			return k;
		}
	}
	return -1;
}

void SetBackUpMapCharPlayer( int no, int cpno, int px, int py, int dir )
{
	int cno = Change_Pno2Cno( cpno );

	ZeroMemory( &BackUpMapChar[no], sizeof(BACKUP_MAPCHAR) );

	BackUpMapChar[no].flag = TRUE;
	BackUpMapChar[no].cno  = cno;
	BackUpMapChar[no].job  = -1;
	BackUpMapChar[no].buki = -1;

	BackUpMapChar[no].px  = px;
	BackUpMapChar[no].py  = py;
	BackUpMapChar[no].dir = dir;
	BackUpMapChar[no].lv  = -1;
	BackUpMapChar[no].attr= -1;

	SetMapChar( no, cno, px, py, dir );
}



typedef struct{
	int		job;
	int		move;
	int		atack;
}JOB_RAND_THINK;

#define JOB_RAND_THINK_MAX	44

JOB_RAND_THINK	JobRandThink[JOB_RAND_THINK_MAX]={
	{ 0,  MOVE_THINK_ATACK,     ACTION_THINK_ATACK},  
	{ 1,  MOVE_THINK_ATACK,     ACTION_THINK_KNIGHT},  
	{ 2,  MOVE_THINK_ATACK,     ACTION_THINK_HEAL_MAGIC},  
	{ 3,  MOVE_THINK_ATACK,     ACTION_THINK_HEAL_MAGIC},  
	{ 4,  MOVE_THINK_SURPRISE,  ACTION_THINK_SHOT},  

	{ 6,  MOVE_THINK_ATACK,     ACTION_THINK_ATACK},  
	{ 7,  MOVE_THINK_ATACK,     ACTION_THINK_KNIGHT},  
	{ 8,  MOVE_THINK_ATACK,     ACTION_THINK_MAGIC},  
	{ 9,  MOVE_THINK_ATACK,     ACTION_THINK_MAGIC},  
	{10,  MOVE_THINK_SURPRISE,  ACTION_THINK_SHOT},  


	{21,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  
	{22,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  
	{23,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  
	{24,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  
	{25,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  
	{26,  MOVE_THINK_ATACK,  ACTION_THINK_HEAL_MAGIC},  
	{27,  MOVE_THINK_ATACK,  ACTION_THINK_MAGIC},  
	{28,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  

	{32,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  
	{45,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  
	{65,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  

	{33,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  
	{34,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  
	{35,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  


	{40,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  
	{66,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  
	{67,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  


	{41,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  
	{42,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  


	{46,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  


	{48,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  

	{59,  MOVE_THINK_SURPRISE,  ACTION_THINK_SHOT},  
	{68,  MOVE_THINK_SURPRISE,  ACTION_THINK_SHOT},  

	{60,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  

	{50,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  

	{51,  MOVE_THINK_ATACK,  ACTION_THINK_HEAL_MAGIC},  
	{52,  MOVE_THINK_ATACK,  ACTION_THINK_HEAL_MAGIC},  
	{53,  MOVE_THINK_ATACK,  ACTION_THINK_HEAL_MAGIC},  

	{54,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  
	{55,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  

	{56,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  
	{69,  MOVE_THINK_ATACK,  ACTION_THINK_ATACK},  

	{57,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  
	{58,  MOVE_THINK_ATACK,  ACTION_THINK_KNIGHT},  
};





int	JobRandSet[10][5][5]={
	
	{
		{ 21, 23,  4, 26, 27 },
		{ 22, 25, 10, 26, 27 },
		{ 21, 22, 25,  4, 26 },
		{ 28, 23, 25,  4, 27 },
		{ 21, 21, 28, 23, 25 },
	},{
	
		{ 46, 41, 51, -1, -1 },
		{ 46, 42, 52, -1, -1 },
		{ 46, 42, 53, -1, -1 },
		{ 46, 41, 53, -1, -1 },
		{ 46, 41, 51, -1, -1 },
	},{
	
		{ 54, 32, 56, 51, -1 },
		{ 54, 45, 56, 51, -1 },
		{ 55, 45, 69, 52, -1 },
		{ 55, 65, 69, 53, -1 },
		{ 54, 32, 56, 51, -1 },
	},{
	
		{ 56, 32, 67, 59, -1 },
		{ 69, 59, 45, 40, -1 },
		{ 56, 48, 66, 56, -1 },
		{ 60, 59, 65, 59, -1 },
		{ 56, 32, 67, 59, -1 },
	},{
	
		{ 32, 34, 42, 35, -1 },
		{ 32, 35, 41, 42, -1 },
		{ 45, 33, 41, 42, -1 },
		{ 65, 33, 42, 34, -1 },
		{ 32, 34, 42, 35, -1 },
	},{
	
		{ 56, 58, 34, 66, -1 },
		{ 56, 58, 35, 66, -1 },
		{ 69, 58, 41, 67, -1 },
		{ 69, 58, 42, 67, -1 },
		{ 56, 58, 34, 66, -1 },
	},{
	
		{ 50, 54, 32, 53, -1 },
		{ 50, 55, 32, 53, -1 },
		{ 50, 56, 32, 53, -1 },
		{ 50, 69, 32, 53, -1 },
		{ 50, 54, 32, 53, -1 },
	},{
	
		{ 48, 50, 68, -1, -1 },
		{ 48, 67, 68, -1, -1 },
		{ 60, 67, 68, -1, -1 },
		{ 60, 50, 68, -1, -1 },
		{ 48, 50, 68, -1, -1 },
	},{
	
		{ 57, 40, 50, -1, -1 },
		{ 57, 66, 50, -1, -1 },
		{ 57, 67, 54, -1, -1 },
		{ 57, 67, 55, -1, -1 },
		{ 57, 40, 50, -1, -1 },
	},{
	
		{ 6, 7, 10,  8,  9 },	
		{ 0, 1,  4,  2,  3 },	
		{ 6, 7,  4, 10,  4 },	
		{ 7, 2,  3,  8,  9 },	
		{ 6, 7,  4,  3,  9 },	
	}
};

int	JobRandType=-1;

int	SetJobRand( int job )
{
	if(job>=0) return job;

	int	map = -(job/10)-1;

	int	tairetu = (-job)%10;
	int	r;

	if(JobRandType==-1){
		if(JobRandSet[map][4][0]==-1){
			JobRandType = rand()%4;
		}else{
			JobRandType = rand()%5;
		}
	}else{
		if(JobRandSet[map][JobRandType][0]==-1){
			JobRandType = rand()%4;
		}
	}
	if(tairetu>=5){
		tairetu = rand()%5;
	}
	if(JobRandSet[map][JobRandType][tairetu]==-1){
		tairetu--;
		if(JobRandSet[map][JobRandType][tairetu]==-1){
			tairetu--;
		}
	}
	if( JobRandSet[map][JobRandType][tairetu]==-1){
		return 4;
	}

	if( JobRandSet[map][JobRandType][tairetu]==55){
		if( ESC_GetFlag(80) ){
			return 55;
		}else{
			return 54;
		}
	}

	int	job_ret = JobRandSet[map][JobRandType][tairetu];
	switch( ESC_GetFlag(81) ){
		case 0:
			switch( job_ret ){
				case 28:
				
					job_ret=21;
					break;
				
				case 45:	
				case 65:	
					job_ret=32;
					break;
				
				case 34:	
				case 35:	
					job_ret=33;
					break;
				
				case 66:	
				case 67:	
					job_ret=40;
					break;
				
				case 42:	
					job_ret=41;
					break;
				
				case 68:	
					job_ret=59;
					break;
				
				case 52:	
				case 53:	
					job_ret=51;
					break;
				
				case 69:	
					job_ret=56;
					break;
			}
			break;
		case 1:
			switch( job_ret ){
				
				case 28:
					job_ret=21;
					break;
				
				case 65:	
					job_ret=45;
					break;
				
				case 35:	
					job_ret=34;
					break;
				
				case 67:	
					job_ret=66;
					break;
				
				case 68:	
					job_ret=59;
					break;
				
				case 53:	
					job_ret=52;
					break;
				
				case 69:	
					job_ret=56;
					break;
			}
			break;
		default:
		case 2:
			switch( job_ret ){
				
				case 32:	
					job_ret=45;
					break;
				
				case 33:	
					job_ret=34;
					break;
				
				case 40:	
					job_ret=66;
					break;
				
				case 51:	
					job_ret=52;
					break;
			}
			break;
	}

	return job_ret;
}

void SetBackUpMapChar( int no, int cno, int job, int buki, int px, int py, int dir, int lv, int attr, int unit )
{
	int	job_rand=0;
	ZeroMemory( &BackUpMapChar[no+10], sizeof(BACKUP_MAPCHAR) );


	BackUpMapChar[no+10].flag = TRUE;
	BackUpMapChar[no+10].cno  = cno;
	BackUpMapChar[no+10].buki = buki;

	BackUpMapChar[no+10].px  = px;
	BackUpMapChar[no+10].py  = py;
	BackUpMapChar[no+10].dir = dir;
	BackUpMapChar[no+10].lv  = lv;
	BackUpMapChar[no+10].unit = unit;

	if(job<0){
		srand( timeGetTime()+px+py );
		job = SetJobRand(job);
		job_rand=1;
	}

	SetCharParam( cno, job, lv, buki, unit );

	if(attr==-1){
		BackUpMapChar[no+10].attr  = CharParam[cno].attr;
	}else{
		CharParam[cno].attr = attr;
		BackUpMapChar[no+10].attr = attr;
	}
	
	SetMapChar( no+10, cno, px, py, dir );

	if(job_rand){
		int	i;
		for(i=0;i<JOB_RAND_THINK_MAX;i++){
			if(JobRandThink[i].job==job){
				SetBackUpMapCharTnink( no, JobRandThink[i].move, JobRandThink[i].atack, -1, -1, -1 );
				break;
			}
		}
	}else{
		switch(ItemParam[CharParam[cno].buki].type){
			default:
			case ITEM_BUKI_SWORD:
			case ITEM_BUKI_HAMMER:
				SetMapCharThink( no+10, MOVE_THINK_ATACK, ACTION_THINK_ATACK );
				BackUpMapChar[no+10].move_think   = MOVE_THINK_ATACK;
				BackUpMapChar[no+10].action_think = ACTION_THINK_ATACK;
				break;
			case ITEM_BUKI_BOW:
				SetMapCharThink( no+10, MOVE_THINK_ATACK, ACTION_THINK_SHOT );
				BackUpMapChar[no+10].move_think   = MOVE_THINK_ATACK;
				BackUpMapChar[no+10].action_think = ACTION_THINK_SHOT;
				break;
			case ITEM_BUKI_SPEAR:
				SetMapCharThink( no+10, MOVE_THINK_ATACK, ACTION_THINK_MAGIC );
				BackUpMapChar[no+10].move_think   = MOVE_THINK_ATACK;
				BackUpMapChar[no+10].action_think = ACTION_THINK_MAGIC;
				break;
			case ITEM_BUKI_WAND:
				SetMapCharThink( no+10, MOVE_THINK_NO_MOVE, ACTION_THINK_MAGIC );
				BackUpMapChar[no+10].move_think   = MOVE_THINK_NO_MOVE;
				BackUpMapChar[no+10].action_think = ACTION_THINK_MAGIC;
				break;
		}
	}
	BackUpMapChar[no+10].job  = job;
	BackUpMapChar[no+10].tx=-1;
	BackUpMapChar[no+10].ty=-1;
	BackUpMapChar[no+10].range=-1;
	_mbscpy( (BYTE*)BackUpMapChar[no+10].name, (BYTE*)CharParam[cno].name );
}
void SetBackUpMapCharName( int no,int cno, char *str )
{
	SetCharParamName( cno, str );
	GD_CreateTextTexture( BMP_MAP_CHAR+no+10, (char*)CharParam[cno].name, 12, 10, 1, 0, 0, 1 );

	_mbscpy( (BYTE*)BackUpMapChar[no+10].name, (BYTE*)str );
}

void SetMapCharEngun( int no )
{
	CharControl[no].action    = CHAR_ACTION_DID;
	CharControl[no].step      = CHAR_STEP_DID;
	CharParam[CharControl[no].cno].did = TRUE;

	GD_SetGraphDisp( GRP_MAP_CHAR+no, FALSE );
}


void SetBackUpMapCharEngun( int no, int etime, int loop, int inter )
{
	SetMapCharEngun( no+10 );

	if(BackUpMapChar[no+10].flag){
		BackUpMapChar[no+10].engun = TRUE;
		BackUpMapChar[no+10].etime     = etime*60;
		if(inter==-1){
			BackUpMapChar[no+10].inter     = etime*60;
		}else{
			BackUpMapChar[no+10].inter     = inter*60;
		}
		BackUpMapChar[no+10].etime_cnt = 0;
		BackUpMapChar[no+10].loop      = loop;
		BackUpMapChar[no+10].loop_cnt  = 1;
	}
}

int GetBackUpMapCharItem( int no )
{
	return BackUpMapChar[no].item_no;
}
int GetBackUpMapCharRank( int no )
{
	return BackUpMapChar[no].rank_no;
}

void SetBackUpMapCharItem( int no, int rank, int ino )
{
	BackUpMapChar[no+10].rank_no = rank;
	BackUpMapChar[no+10].item_no = ino;
}

void SetBackUpMapCharTnink( int no, int mov_th, int atk_th, int tx, int ty, int range )
{
	SetMapCharThink( no+10, mov_th, atk_th, tx, ty, range );
	if(BackUpMapChar[no+10].flag){
		BackUpMapChar[no+10].move_think   = mov_th;
		BackUpMapChar[no+10].action_think = atk_th;
		BackUpMapChar[no+10].tx    = tx;
		BackUpMapChar[no+10].ty    = ty;
		BackUpMapChar[no+10].range = range;
	}
}
void ReloadBackUpMapChar( void )
{
	int	i;
	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if(BackUpMapChar[i].flag){
			if(BackUpMapChar[i].job!=-1){
				int	hujimi = CharParam[BackUpMapChar[i].cno].hujimi;

				SetCharParam( BackUpMapChar[i].cno, BackUpMapChar[i].job, BackUpMapChar[i].lv, BackUpMapChar[i].buki, BackUpMapChar[i].unit, BackUpMapChar[i].name );
				CharParam[ BackUpMapChar[i].cno ].attr = BackUpMapChar[i].attr;
				CharParam[ BackUpMapChar[i].cno ].hujimi=hujimi;
			}
			SetMapChar( i, BackUpMapChar[i].cno, BackUpMapChar[i].px, BackUpMapChar[i].py, BackUpMapChar[i].dir );
			if(BackUpMapChar[i].job!=-1){
				SetMapCharThink( i, BackUpMapChar[i].move_think, BackUpMapChar[i].action_think, BackUpMapChar[i].tx, BackUpMapChar[i].ty, BackUpMapChar[i].range );
			}
			if(BackUpMapChar[i].engun){
				SetMapCharEngun( i );
				BackUpMapChar[i].etime_cnt = 0;
				BackUpMapChar[i].loop_cnt  = 1;
			}
		}
	}
}
void ResetBackUpMapChar( void )
{
	JobRandType=-1;
	ZeroMemory( BackUpMapChar, sizeof(BACKUP_MAPCHAR)*CHAR_CONTROL_MAX );
}


void SetMapChar( int no, int cno, int px, int py, int dir )
{
	ResetMapChar( no );

	if(no<0 || no>=32){
		DebugBox(NULL,"それはだめだ");
	}
	if(cno<0 || cno>=64){
		DebugBox(NULL,"あぶないよ");
	}

	CharControl[no].flag = ON;		
	CharControl[no].cno  = cno;		

	
	if(CharParam[cno].unit==0){
		CharControl[no].old_action = CHAR_ACTION_STAND;	
		CharControl[no].action = CHAR_ACTION_STAND;	


		if( CharParam[cno].accessary2==235 || CharParam[cno].accessary2==228 ){
			SetMapCharThink( no, MOVE_THINK_ATACK, ACTION_THINK_KNIGHT );
		}
	}else{
		CharControl[no].action = CHAR_ACTION_STAND;	
	}

	CharControl[no].step   = CHAR_STEP_STAND;	

	CharControl[no].step_count=0;
	if(cno<32){
		CharControl[no].start_count=32;
	}else{
		CharControl[no].start_count=0;
	}
	CharControl[no].total_count=rand();

	CharControl[no].dir = dir;	

	CharControl[no].px = (float)px;
	CharControl[no].py = (float)py;	
	CharControl[no].tx[0] = (float)px;
	CharControl[no].ty[0] = (float)py;	

	CharControl[no].shock = 0;

	CharControl[no].tcno = -1;	

	CharControl[no].ano = LoadMapChar( cno );
	CharControl[no].ano2= 0;

	CharParam[cno].no = no;		


	
	if( JobParam[CharParam[cno].job].ex&EX_HAKO ){
		GD_SetSprite( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, 0, TRUE, 15, TRUE, TRUE, 0, FALSE );
	}else{
		GD_SetSprite( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, dir+8, TRUE, 15, TRUE, TRUE, 0, FALSE );
	}
	GD_SetGraphScaleParam( GRP_MAP_CHAR+no, SCALE_HIGHT );


	GD_SetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX+no, BMP_SYSTEM, LAY_HP_BAR+no, FALSE );	
	GD_SetGraphBlendParam( GRP_MAP_CHAR+CHAR_CONTROL_MAX+no, BLEND_ALPHA );
	GD_SetGraphDstRect( GRP_MAP_CHAR+CHAR_CONTROL_MAX+no, 0, 0, 64, 6 );
	
	GD_SetGraphPrimitive( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 4, LAY_HP_BAR+no+1, FALSE );
	GD_SetGraphBlendParam( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, BLEND_ALPHA );
	GD_SetGraphDstRect( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 0, 0, 60, 2 );
	
	if(CharParam[cno].unit==0){
		GD_SetGraphSrcRect( GRP_MAP_CHAR+CHAR_CONTROL_MAX+no, 256,0, 64,6 );

		GD_SetGraphColorNo( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 0, 255, 234, 107, 72 );
		GD_SetGraphColorNo( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 2, 255, 234, 107, 72 );

		GD_SetGraphColorNo( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 1, 255, 254, 245, 102 );
		GD_SetGraphColorNo( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 3, 255, 254, 245, 102 );
	}else{
		GD_SetGraphSrcRect( GRP_MAP_CHAR+CHAR_CONTROL_MAX+no, 256,6, 64,6 );

		GD_SetGraphColorNo( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 0, 255, 99, 100, 255 );
		GD_SetGraphColorNo( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 2, 255, 99, 100, 255 );

		GD_SetGraphColorNo( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 1, 255, 181, 185, 255 );
		GD_SetGraphColorNo( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, 3, 255, 181, 185, 255 );
	}
	GD_CreateTextTexture( BMP_MAP_CHAR+no, (char*)CharParam[cno].name, 12, 10, 1, 0, 0, 1 );
	GD_SetGraphText( GRP_MAP_CHAR+CHAR_CONTROL_MAX*3+no, BMP_MAP_CHAR+no, LAY_HP_BAR+no+1, FALSE );

	CharControl[no].char_sinkin = (BYTE*)GAlloc((MapData.head.w/MapData.head.sgrid) * (MapData.head.h/MapData.head.sgrid));

}

void SetMapCharDisp( int no, int disp )
{
	CharControl[no].disp = disp;
	
	GD_SetGraphDisp( GRP_MAP_CHAR+no, disp );
	GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX+no, disp );
	GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+no, disp );
}


void SetMapCharCommand( int no, int click )
{
	if(click){
		if( (CharControl[no].step!=CHAR_STEP_TECHNIC) && (CharControl[no].step!=CHAR_STEP_ITEM) ){
			if(CharControl[no].action!=CHAR_ACTION_COMMAND){
				if( CharParam[CharControl[no].cno].accessary2==235 || CharParam[CharControl[no].cno].accessary2==228 ){
					SND_Ng();
				}else if( CharParam[CharControl[no].cno].job==89 ){
				}else{
					SND_Ok();
					CharControl[no].old_action = CharControl[no].action;
					CharControl[no].action = CHAR_ACTION_COMMAND;
				}
			}
		}
	}else{
		if(Avg.auto_modo==0){
			LoadKipuMapCharThink( no );
		}
		CharControl[no].action = CHAR_ACTION_STAND;
	}
}

void CanselMapCharCommand( int no )
{
	CharControl[no].action = CharControl[no].old_action;
}


int SerchCharSinkin( int cno, float px, float py )
{
	int	i;
	float	work_x;
	float	work_y;


	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if(i==cno) continue;

		work_x = ABS(px-CharControl[i].px);
		work_y = ABS(py-CharControl[i].py);



		if( px-16<=CharControl[i].px && CharControl[i].px<px+16 && py-16<=CharControl[i].py && CharControl[i].py<py+16 ){
			return FALSE;
		}
	}
	return TRUE;
}




short	seath_x[2][1024];
short	seath_y[2][1024];
void SetCharRoutingTbl( int no, int tno, short seath_count, int char_sinkin )
{
	short	seath_count2, seath_count3, wait, i;
	int		saddr, np=0;
	int		w = MapData.head.w/MapData.head.sgrid;
	int		h = MapData.head.h/MapData.head.sgrid;
	short	px = (short)(CharControl[no].px/MapData.head.sgrid);
	short	py = (short)(CharControl[no].py/MapData.head.sgrid);

	wait=1;
	while(seath_count){
		if(seath_count>1024){
			break;
		}
		seath_count2=0;
		seath_count3=1024;
		for( i=0; i<seath_count ; i++ ){
			if( seath_x[np][i]<0 || seath_y[np][i]<0 || seath_x[np][i]>=w || seath_y[np][i]>=h ) continue;

			saddr = seath_y[np][i]*w + seath_x[np][i];
			if( CharControl[no].move_tbl[saddr]==-1 ) continue;
			if(char_sinkin){
				if(tno==-1){

					if( CharControl[no].char_sinkin[saddr] && CharControl[no].char_sinkin[saddr]!=tno+1 ) continue;
				}else{

					if( CharControl[no].char_sinkin[saddr] ) continue;
				}
			}

			if( CharControl[no].move_tbl[saddr]>wait ){
				CharControl[no].move_tbl[saddr]=wait;

				if( seath_x[np][i]==px && seath_y[np][i]==py ){
					seath_count3 = seath_count2;

				}
				if(seath_count3+4*3<=seath_count2){
					break;
				}
			}else{
				continue;
			}
			seath_x[!np][seath_count2] = seath_x[np][i]+0;	seath_y[!np][seath_count2] = seath_y[np][i]-1;
			seath_count2++;
			seath_x[!np][seath_count2] = seath_x[np][i]-1;	seath_y[!np][seath_count2] = seath_y[np][i]+0;
			seath_count2++;
			seath_x[!np][seath_count2] = seath_x[np][i]+1;	seath_y[!np][seath_count2] = seath_y[np][i]+0;
			seath_count2++;
			seath_x[!np][seath_count2] = seath_x[np][i]+0;	seath_y[!np][seath_count2] = seath_y[np][i]+1;
			seath_count2++;
		}
		np = !np;
		wait++;
		seath_count=seath_count2;
	}
}
void SetMapCharSkill( int no, int tno, float tx, float ty )
{
	int	cno = CharControl[no].cno;
	char	fname[MAX_PATH];
	double	radian;
	if(tno==-1){
		radian = atan2( ty-CharControl[no].py, tx-CharControl[no].px );
		SetCharDir( no, radian );
	}else if(tno==-2){
	}else{
		radian = atan2( CharControl[tno].py-CharControl[no].py, CharControl[tno].px-CharControl[no].px );
		SetCharDir( no, radian );
	}
	int	dir = CharControl[no].dir;


	if(1<=dir && dir<=3){
		GD_SetGraphRevParam( GRP_MAP_CHAR+no, REVERSE_W );
		wsprintf( fname, "an%03d%03d0%02d_%02d.ani", CharParam[cno].GetJobFileNo(), CharParam[cno].GetBukiFileNo(), 11, 8-dir );
	}else{
		wsprintf( fname, "an%03d%03d0%02d_%02d.ani", CharParam[cno].GetJobFileNo(), CharParam[cno].GetBukiFileNo(), 11, dir );
	}

	char	*tname;
	if( PAC_CheckFile( D3DD_GetPackDir(), bak_tone_fname ) ){
		tname = bak_tone_fname;						
	}else{
		tname = time_tone_fname[BackStruct.time_no];
	}

	GD_LoadSprite( SPR_MAP_CHAR2+cno, fname, ALPHA_4444, FALSE, DrawSetting.mip_map, tname, eff_tone_fname_c, BackStruct.chroma_char, "btl" );
}



void SetMapCharEvent( int no, int dir, int act, int loop )
{
	ResetMapCharEvent( no );

	if(dir!=-1)	CharControl[no].dir = dir;
	CharControl[no].event_flag  = 1;
	CharControl[no].event_act   = act;
	CharControl[no].event_count = 0;
	CharControl[no].event_act_loop = loop;

	
	if(act>8){
		int	cno = CharControl[no].cno;
		char	fname[MAX_PATH];
		if(dir==-1){
			CharControl[no].ano2 = 1;
			wsprintf( fname, "an%03d%03d0%02d.ani", CharParam[cno].GetJobFileNo(), CharParam[cno].GetBukiFileNo(), act );
		}else{
			CharControl[no].ano2 = 2;
			if(1<=dir && dir<=3){
				GD_SetGraphRevParam( GRP_MAP_CHAR+no, REVERSE_W );

				wsprintf( fname, "an%03d%03d0%02d_%02d.ani", CharParam[cno].GetJobFileNo(), CharParam[cno].GetBukiFileNo(), act, 8-dir );
			}else{
				wsprintf( fname, "an%03d%03d0%02d_%02d.ani", CharParam[cno].GetJobFileNo(), CharParam[cno].GetBukiFileNo(), act, dir );
			}
		}
		char	*tname;
		if( PAC_CheckFile( D3DD_GetPackDir(), bak_tone_fname ) ){
			tname = bak_tone_fname;						
		}else{
			tname = time_tone_fname[BackStruct.time_no];
		}
		GD_LoadSprite( SPR_MAP_CHAR2+cno, fname, ALPHA_4444, FALSE, DrawSetting.mip_map, tname, eff_tone_fname_c, BackStruct.chroma_char, "btl" );
	}
}


void ResetMapCharEvent( int no )
{
	CharControl[no].event_flag  = 0;
	if(CharControl[no].ano2){
		CharControl[no].ano2 = 0;
		GD_SetGraphRevParam( GRP_MAP_CHAR+no, REVERSE_NORMAL );
		GD_ReleaseSprite( SPR_MAP_CHAR2+CharControl[no].cno );
		GD_SetSpritePlay(GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, 0, TRUE, TRUE );
	}
}

BOOL WaitMapCharEvent( int no )
{
	return CharControl[no].event_flag!=1;
}

int SetMapCharTartgetMoveCheck( int no, int tx, int ty )
{
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;
	int	x = tx/MapData.head.sgrid;
	int	y = ty/MapData.head.sgrid;

	if(x<0)	return FALSE;
	if(y<0)	return FALSE;
	if(x>=w)	return FALSE;
	if(y>=h)	return FALSE;

	if( MapData.sinkin[y*w+x] ) return FALSE;
	return TRUE;
}


int SetMapCharTartgetMove( int no, int tx, int ty, int sinkin, int dir, int mov, int re_mov )
{
	int	x,y,i;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;

	ResetMapCharEvent( no );
	if(sinkin==0){
		if( MapData.sinkin[(ty/MapData.head.sgrid)*w+tx/MapData.head.sgrid] ) return FALSE;
	}
	if(sinkin!=-1){
		CharControl[no].event_sinkin = sinkin;
	}

	if(mov<=0){
		CharControl[no].mov = 0;
	}else{
		CharControl[no].mov = mov;
	}
	if(dir!=-1)	{
		CharControl[no].event_flag=1;
		CharControl[no].dir = dir;
	}else{
		CharControl[no].event_flag=0;
	}


	if(re_mov==0){




		CharControl[no].tx[CharControl[no].tcnt]  = (float)tx;
		CharControl[no].ty[CharControl[no].tcnt]  = (float)ty;
		CharControl[no].tcnt = min(CharControl[no].tcnt+1, 7);
	}
	CharControl[no].action = CHAR_ACTION_MOVE;
	CharControl[no].step   = CHAR_STEP_MOVE;


	GFree( CharControl[no].move_tbl );
	CharControl[no].move_tbl = (short*)GAlloc( w*h*2 );	


	if(sinkin==0 || CharControl[no].event_sinkin==0){
		for(y=0;y<h;y++){
			for(x=0;x<w;x++){
				switch(MapData.sinkin[y*w+x]){
				case 0:
					CharControl[no].move_tbl[y*w+x] = 30000;
					break;
				default:
					CharControl[no].move_tbl[y*w+x] = -1;
					break;
				}
			}
		}
	}else{
		for(y=0;y<h;y++){
			for(x=0;x<w;x++){
				CharControl[no].move_tbl[y*w+x] = 30000;
			}
		}
	}

	seath_x[0][0] = tx/MapData.head.sgrid;
	seath_y[0][0] = ty/MapData.head.sgrid;


	SetCharRoutingTbl( no, -1, 1, 1 );





	return TRUE;
}

void SetMapCharTartgetSuport( int no, int range )
{
	int	j;
	int	x,y;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;
	char	*sp_buf=NULL;

	sp_buf = (char*)GAlloc( w*h );	

	for(j=0;j<CHAR_CONTROL_MAX;j++){
		if( !CharControl[j].flag ) continue;
		if( CharParam[CharControl[j].cno].unit != CharParam[CharControl[no].cno].unit ) continue;
		if( CharParam[CharControl[j].cno].did ) continue;
		if( j==no ) continue;
		if( ItemParam[CharParam[CharControl[j].cno].buki].type==ITEM_BUKI_WAND) continue;

		int	sx = max(0,(CharControl[j].px-range)/MapData.head.sgrid);
		int	sy = max(0,(CharControl[j].py-range)/MapData.head.sgrid);
		int	ex = min(w,(CharControl[j].px+range)/MapData.head.sgrid);
		int	ey = min(h,(CharControl[j].py+range)/MapData.head.sgrid);

		for( y=sy ; y<ey ; y++ ){
			for( x=sx ; x<ex ; x++ ){
				if( pow(x-CharControl[j].px/MapData.head.sgrid,2)+pow(y-CharControl[j].py/MapData.head.sgrid,2) <= pow(range/MapData.head.sgrid,2) ){
					sp_buf[y*w+x] += 1;
				}
			}
		}
	}
	
	x = LIM( CharControl[no].px/MapData.head.sgrid, 0, w-1 );
	y = LIM( CharControl[no].py/MapData.head.sgrid, 0, h-1 );
	int	sp_rate = sp_buf[y*w+x];
	int	bak_rate = sp_rate;
	int	sp_x, sp_y;
	int range2=4096*2;
	for( y=0 ; y<h ; y++ ){
		for( x=0 ; x<w ; x++ ){
			if( MapData.sinkin[y*w+x]==0  ){
				if( CharControl[no].char_sinkin[y*w+x]==0 ){

					if( sp_rate <= sp_buf[y*w+x] ){
						if( sp_rate < sp_buf[y*w+x] ){
							range2=4096*2;
						}
						sp_rate = sp_buf[y*w+x];

						if( sqrt(pow(x-CharControl[no].px/MapData.head.sgrid,2)+pow(y-CharControl[no].py/MapData.head.sgrid,2)) < range2 ){
							range2 = sqrt(pow(x-CharControl[no].px/MapData.head.sgrid,2)+pow(y-CharControl[no].py/MapData.head.sgrid,2));
							sp_x = x;
							sp_y = y;
						}
					}
				}
			}
		}
	}
	GFree(sp_buf);
	if( sp_rate==bak_rate || range2==4096*2){
		SetMapCharStand( no );
	}else{
		CharControl[no].tcnt=0;
		SetMapCharTartgetMove( no, sp_x*MapData.head.sgrid, sp_y*MapData.head.sgrid );
	}
}


int CheckMapCharAtack( int no, int tcno )
{
	int	x,y;
	int	tx,ty;
	int	ret=0;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;
	short	range = CharParam[CharControl[no].cno].GetRange();



		range = range/MapData.head.sgrid-1;

	
	for(y=-range; y<range ;y++){
		for(x=-range; x<range ;x++){
			if( x*x+y*y>=(range-1)*(range-1) && x*x+y*y<=range*range ){
				tx = CharControl[tcno].px/MapData.head.sgrid+x;
				ty = CharControl[tcno].py/MapData.head.sgrid+y;
				
				if(tx<0) continue;
				if(ty<0) continue;
				if(tx>=w) continue;
				if(ty>=h) continue;
				if( MapData.sinkin[ty*w+tx]==0 ){
					ret |= 1;
				}
			}
		}
	}
	return ret;
}
void SetMapCharHangekiAtack( int no, int tcno )
{
	int	x,y;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;

	if( pow(CharControl[no].px-CharControl[tcno].px,2) + pow(CharControl[no].py-CharControl[tcno].py,2) > pow(CharParam[CharControl[no].cno].GetRange(),2) ){
		return ;
	}
	
	if(CharControl[no].aw) return ;
	if(CharControl[no].sw) return ;

	if(tcno==-1 ){
		SetMapCharStand( no );
		return ;
	}

	CharControl[no].tcno  = tcno;
	CharControl[no].serch_wait = 1;
	CharControl[no].action = CHAR_ACTION_HANGEKI;
	CharControl[no].step = CHAR_STEP_ATACK;


}


void SetMapCharTartgetAtack( int no, int tcno )
{
	int	x,y;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;

	if(tcno==-1){
		SetMapCharStand( no );
		return ;
	}

	CharControl[no].tx[0] = CharControl[tcno].px;
	CharControl[no].ty[0] = CharControl[tcno].py;

	CharControl[no].tcno  = tcno;
	CharControl[no].serch_wait = 1;
	CharControl[no].action = CHAR_ACTION_ATACK;
	if( CharControl[no].step!=CHAR_STEP_ATACK && CharControl[no].step!=CHAR_STEP_ATACK2 && CharControl[no].step!=CHAR_STEP_ATACK_WAIT ){
		CharControl[no].step   = CHAR_STEP_MOVE;
	}


	GFree( CharControl[no].move_tbl );
	CharControl[no].move_tbl = (short*)GAlloc( w*h*2 );	


	for(y=0;y<h;y++){
		for(x=0;x<w;x++){
			switch(MapData.sinkin[y*w+x]){
			case 0:
				CharControl[no].move_tbl[y*w+x] = 30000;
				break;
			default:
				CharControl[no].move_tbl[y*w+x] = -1;
				break;
			}
		}
	}

	short	seath_count=0;
	short	range=CharParam[CharControl[no].cno].GetRange();
	



		range = range/MapData.head.sgrid-1;


	
	for(y=-range; y<range ;y++){
		for(x=-range; x<range ;x++){
			if( x*x+y*y>=(range-1)*(range-1) && x*x+y*y<=range*range ){
				int	tx  = (CharControl[no].tx[0]/MapData.head.sgrid)+x;
				int	ty  = (CharControl[no].ty[0]/MapData.head.sgrid)+y;

				if( tx<0 ) continue;
				if( ty<0 ) continue;
				if( tx>=w ) continue;
				if( ty>=h ) continue;

				if( MapData.sinkin[ty*w+tx] ) continue;

				seath_x[0][seath_count] = (short)(CharControl[no].tx[0]/MapData.head.sgrid)+x;
				seath_y[0][seath_count] = (short)(CharControl[no].ty[0]/MapData.head.sgrid)+y;
				seath_count++;
			}
			if(seath_count>1024){
				DebugBox( NULL, "攻撃範囲がオーバーブローしました" );
			}
		}
	}
	if(seath_count==0) return ;


	SetCharRoutingTbl( no, tcno, seath_count, 1 );
}
void SetMapCharTartgetMagic( int no, int mgno, int tcno, int tx, int ty, int amend )
{
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;

	if(!amend){
		ResetMagicAction( no );
	}

	switch( MagicParam[mgno].scope_type ){
		case RANGE_TAGET:		
			CharControl[no].tcno  = tcno;
			break;
		case RANGE_SCOPE:		
			CharControl[no].tx[0] = (float)tx;
			CharControl[no].ty[0] = (float)ty;
			break;
		case RANGE_SELF:		
			CharControl[no].tcno  = no;
			break;
		case RANGE_SELF_SCOPE:	
			CharControl[no].tcno  = no;
			break;
		case RANGE_LINE:		
			CharControl[no].tcno  = no;
			break;
		case RANGE_FAN:			
			CharControl[no].tcno  = no;
			break;
		case RANGE_ALL:			
			CharControl[no].tcno  = no;
			break;
	}
	CharControl[no].mgno = mgno;

	if(!amend){
		switch(MagicParam[ mgno ].type){
			default:
				CharControl[no].action = CHAR_ACTION_MAGIC;
				CharControl[no].step   = CHAR_STEP_MAGIC_STAND;
				break;
			case MAGIC_BARD:
				CharControl[no].action = CHAR_ACTION_BARD;
				CharControl[no].step   = CHAR_STEP_BARD_STAND;
				SetBardAction( no );
				break;
			case MAGIC_SKILL:
				CharControl[no].action = CHAR_ACTION_TECHNIC;
				CharControl[no].step   = CHAR_STEP_TECHNIC_STAND;
				SetSkillAction(no);
				break;
		}
	}
}
void SetMapCharTartgetItem( int no, int ino, int tcno )
{
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;

	ResetMagicAction( no );
	CharControl[no].tcno  = tcno;

	CharControl[no].ino = ino;

	CharControl[no].action = CHAR_ACTION_ITEM;
	CharControl[no].step   = CHAR_STEP_ITEM;
	SetItemAction( no );
}


void SetMapCharStand( int no )
{
	CharControl[no].action = CHAR_ACTION_STAND;
	CharControl[no].step = CHAR_STEP_STAND;
	CharControl[no].serch_wait=0;
}



BOOL WaitMapCharTartget( int no )
{
	return CharControl[no].tx[0]==CharControl[no].px && CharControl[no].ty[0]==CharControl[no].py;
}
BOOL WaitMapCharTartget2( int no )
{
	return CharControl[no].tx[0]==CharControl[no].px && CharControl[no].ty[0]==CharControl[no].py && CharControl[no].tcnt==0;
}



void RenewCharSinkin(void)
{
	int	x,y, jx,jy;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;
	int	i,j;


	for(j=0;j<CHAR_CONTROL_MAX;j++){
		if(CharControl[j].flag && !CharParam[CharControl[j].cno].did){
			if(CharControl[j].step!=CHAR_STEP_MOVE)continue;
			
			ZeroMemory( CharControl[j].char_sinkin, w*h );
			for(i=0;i<CHAR_CONTROL_MAX;i++){
				if(CharControl[i].flag && !CharParam[CharControl[i].cno].did){
					if(i==j) continue;
					x=(int)(CharControl[i].px/MapData.head.sgrid);
					y=(int)(CharControl[i].py/MapData.head.sgrid);

					jx=(int)(CharControl[j].px/MapData.head.sgrid);
					jy=(int)(CharControl[j].py/MapData.head.sgrid);

					if( (jx-x)*(jx-x)+(jy-y)*(jy-y)<=3*3 ){
						CharControl[j].char_sinkin[y*w+x]=i+1;
					}
				}
			}
		}
	}
}



void RenewCharStepMove( int i )
{
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;
	int	px = (int)(CharControl[i].px/MapData.head.sgrid);
	int	py = (int)(CharControl[i].py/MapData.head.sgrid);
	int	tx = (int)(CharControl[i].tx[0]/MapData.head.sgrid);
	int	ty = (int)(CharControl[i].ty[0]/MapData.head.sgrid);

	if( JobParam[CharParam[CharControl[i].cno].job].ex&EX_BOSS2 ){
		return ;
	}


	int	speed = CharParam[CharControl[i].cno].GetMov();
	
	if(CharControl[i].mov){
		speed = CharControl[i].mov;
	}

	if( !(CharControl[i].px==CharControl[i].tx[0] && CharControl[i].py==CharControl[i].ty[0] && CharControl[i].tcnt==0) ){
		double	radian = atan2( CharControl[i].ty[0]-CharControl[i].py, CharControl[i].tx[0]-CharControl[i].px );
		float	x,y, x2,y2;
		x = (float)(cos(radian)*16);
		y = (float)(sin(radian)*16);

		float	fx,fy;
		int		xx,yy;
		int		taget_lost=0;

		if(CharControl[i].event_sinkin==0){
			if( y>0 ){
				for( fx=CharControl[i].px, fy=CharControl[i].py; fy<CharControl[i].ty[0]; fx+=x, fy+=y ){
					xx = (int)(fx/MapData.head.sgrid);
					yy = (int)(fy/MapData.head.sgrid);
					if(CharControl[i].action==CHAR_ACTION_ATACK){
						if(MapData.sinkin[ yy*w + xx ] || (CharControl[i].char_sinkin[yy*w+xx] && CharControl[i].char_sinkin[yy*w+xx]!=CharControl[i].tcno+1) ){
							taget_lost=1;
							break;
						}
					}else{
						if(MapData.sinkin[ yy*w + xx ] || (CharControl[i].char_sinkin[yy*w+xx] ) ){
							taget_lost=1;
							break;
						}
					}
				}
			}else{
				for( fx=CharControl[i].px, fy=CharControl[i].py; fy>CharControl[i].ty[0]; fx+=x, fy+=y ){
					xx = (int)(fx/MapData.head.sgrid);
					yy = (int)(fy/MapData.head.sgrid);
					if(CharControl[i].action==CHAR_ACTION_ATACK){
						if(MapData.sinkin[ yy*w + xx ] || (CharControl[i].char_sinkin[yy*w+xx] && CharControl[i].char_sinkin[yy*w+xx]!=CharControl[i].tcno+1) ){
							taget_lost=1;
							break;
						}
					}else{
						if(MapData.sinkin[ yy*w + xx ] || (CharControl[i].char_sinkin[yy*w+xx]) ){
							taget_lost=1;
							break;
						}
					}
				}
			}
			if( x>0 ){
				for( fx=CharControl[i].px, fy=CharControl[i].py; fx<CharControl[i].tx[0]; fx+=x, fy+=y ){
					xx = (int)(fx/MapData.head.sgrid);
					yy = (int)(fy/MapData.head.sgrid);
					if(CharControl[i].action==CHAR_ACTION_ATACK){
						if(MapData.sinkin[ yy*w + xx ] || (CharControl[i].char_sinkin[yy*w+xx] &&  CharControl[i].char_sinkin[yy*w+xx]!=CharControl[i].tcno+1) ){
							taget_lost=1;
							break;
						}
					}else{
						if(MapData.sinkin[ yy*w + xx ] || (CharControl[i].char_sinkin[yy*w+xx]) ){
							taget_lost=1;
							break;
						}
					}
				}
			}else{
				for( fx=CharControl[i].px, fy=CharControl[i].py; fx>CharControl[i].tx[0]; fx+=x, fy+=y ){
					xx = (int)(fx/MapData.head.sgrid);
					yy = (int)(fy/MapData.head.sgrid);
					if(CharControl[i].action==CHAR_ACTION_ATACK){
						if(MapData.sinkin[ yy*w + xx ] || (CharControl[i].char_sinkin[yy*w+xx] && CharControl[i].char_sinkin[yy*w+xx]!=CharControl[i].tcno+1) ){
							taget_lost=1;
							break;
						}
					}else{
						if(MapData.sinkin[ yy*w + xx ] || (CharControl[i].char_sinkin[yy*w+xx]) ){
							taget_lost=1;
							break;
						}
					}
				}
			}
		}

		if( !taget_lost ){

			x = (float)(cos(radian)*speed/500);
			y = (float)(sin(radian)*speed/500);
			if( ( (CharControl[i].px+x>=CharControl[i].tx[0] && CharControl[i].px<=CharControl[i].tx[0]) || (CharControl[i].px+x<=CharControl[i].tx[0] && CharControl[i].px>=CharControl[i].tx[0]) )
			  &&( (CharControl[i].py+y>=CharControl[i].ty[0] && CharControl[i].py<=CharControl[i].ty[0]) || (CharControl[i].py+y<=CharControl[i].ty[0] && CharControl[i].py>=CharControl[i].ty[0]) ) ){
				x2 = CharControl[i].tx[0];
				y2 = CharControl[i].ty[0];
			}else{
				x2 = CharControl[i].px + x;
				y2 = CharControl[i].py + y;
			}
			xx = ((int)x2)/MapData.head.sgrid;
			yy = ((int)y2)/MapData.head.sgrid;
			if(CharControl[i].event_sinkin==0){
				if(MapData.sinkin[ yy*w + xx ]==1){
					taget_lost = 1;
				}
				if(CharControl[i].action==CHAR_ACTION_ATACK){
					if(CharControl[i].char_sinkin[yy*w+xx] && CharControl[i].char_sinkin[yy*w+xx]!=CharControl[i].tcno+1 ){
						taget_lost=2;
					}
				}else{
					if( CharControl[i].char_sinkin[yy*w+xx]){
						taget_lost = 2;
					}
				}
			}



		}

		if( !taget_lost ){
			CharControl[i].px=x2;
			CharControl[i].py=y2;
		}else{
			int	xxx, yyy, wait, rr=0,rr2=0;
			
			for(rr2=0;rr2<2;rr2++){
				wait = CharControl[i].move_tbl[ py*w + px ]+2;
				if(wait==-1) wait = 30001;

				for( yyy=py-1 ; yyy<=py+1 ; yyy++ ){
					for( xxx=px-1 ; xxx<=px+1 ; xxx++ ){
						if(px==xxx && py==yyy ) continue;
						if(xxx<0 || yyy<0 || xxx>=w || yyy>=h ) continue;
						if(CharControl[i].move_tbl[ yyy*w + xxx ]==-1) continue;
						if(MapData.sinkin[ yyy*w + xxx ]) continue;


						if( CharControl[i].move_tbl[ yyy*w + xxx ] < wait ){
							if(taget_lost==1){
								radian = atan2( (yyy-py)/1.92233, double(xxx-px) );


							}else{
								radian = atan2( double(yyy-py), double(xxx-px) );
	
							}
	

							x2 = (float)(cos(radian)*speed/500);
							y2 = (float)(sin(radian)*speed/500);
							xx = (int)((CharControl[i].px+x2)/MapData.head.sgrid);
							yy = (int)((CharControl[i].py+y2)/MapData.head.sgrid);




							if( !(CharControl[i].char_sinkin[yy*w+xx]) ){
								if(MapData.sinkin[ yy*w + xx ]==0){
									x = x2;
									y = y2;
									wait = CharControl[i].move_tbl[ yyy*w + xxx ];

									rr=1;
								}
							}
						}
					}
				}
				if( rr ){
					break;
				}
			}
			if( rr == 0 ){




				x2 = CharControl[i].px + x;
				y2 = CharControl[i].py + y;






			}else{
				if( ((CharControl[i].px+x>=CharControl[i].tx[0] && CharControl[i].px<=CharControl[i].tx[0]) || (CharControl[i].px+x<=CharControl[i].tx[0] && CharControl[i].px>=CharControl[i].tx[0]) )
				 && ((CharControl[i].py+y>=CharControl[i].ty[0] && CharControl[i].py<=CharControl[i].ty[0]) || (CharControl[i].py+y<=CharControl[i].ty[0] && CharControl[i].py>=CharControl[i].ty[0]) ) ){
					CharControl[i].px = CharControl[i].tx[0];
					CharControl[i].py = CharControl[i].ty[0];
				}else{
					x2 = CharControl[i].px + x;
					y2 = CharControl[i].py + y;
					xx = (int)(x2/MapData.head.sgrid);
					yy = (int)(y2/MapData.head.sgrid);
					if(MapData.sinkin[ yy*w + xx ]==0){

						CharControl[i].px=x2;
						CharControl[i].py=y2;
					}else{
						DebugBox( NULL, "ルーティング失敗にょ！[進禁に突入]" );
						CharControl[i].px=x2;
						CharControl[i].py=y2;
					}
				}
			}

		}
		if(0){
			char	buf1[2048];
			char	buf2[256];
			int		x3,y3;
			STD_DeleteFile( "debug000.tet" );
			for(y3=0;y3<h;y3++){
				buf1[0]='\0';
				for(x3=0;x3<w;x3++){
					wsprintf( buf2, "%d,", CharControl[i].move_tbl[y3*w+x3] );
					strcat( buf1,buf2 );
				}
				strcat( buf1,"\n" );
				STD_ApendFile( "debug000.tet", (BYTE*)buf1, strlen(buf1) );
			}
		}


		CharControl[i].dir = ( (int)((radian/3.141592+1.0)*8+1)/2 + 2)%8;
	}
}

int	GetCharCommandWait(void)
{
	int	i;
	int	ret=-1;
	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if( !CharControl[i].flag ) continue;
		if( CharParam[CharControl[i].cno].did ) continue;

		if(CharControl[i].action == CHAR_ACTION_COMMAND ){

				ret=i;
				break;

		}
	}
	return ret;
}

double AttrRate( int attr1, int attr2 )
{
	double	rate=0;

	switch(attr1){
	case ATTR_KA:	
		switch(attr2){
		case ATTR_TI:	rate=-0.2;	break;	
		case ATTR_HUU:	rate=0.2;	break;	
		}
		break;
	case ATTR_TI:	
		switch(attr2){
		case ATTR_KA:	rate=-0.2;	break;	
		case ATTR_SUI:	rate=0.2;	break;	
		}
		break;
	case ATTR_SUI:	
		switch(attr2){
		case ATTR_HUU:	rate=-0.2;	break;	
		case ATTR_KA:	rate=0.2;	break;	
		}
		break;
	case ATTR_HUU:	
		switch(attr2){
		case ATTR_SUI:	rate=-0.2;	break;	
		case ATTR_TI:	rate=0.2;	break;	
		}
		break;

	case ATTR_HOSI:	
		switch(attr2){
		case ATTR_YAMI:	rate=0.2;	break;	
		case ATTR_SEI:	rate=0.2;	break;	
		}
		break;
	case ATTR_YAMI:	
		switch(attr2){
		case ATTR_HOSI:	rate=0.2;	break;	
		case ATTR_SEI:	rate=-0.2;	break;	
		}
		break;
	case ATTR_SEI:	
		switch(attr2){
		case ATTR_SEI:	rate=0.3;	break;	

		case ATTR_YAMI:	rate=0.2;	break;	
		case ATTR_HOSI:	rate=-0.3;	break;	
		break;
		}
	}
	return rate;
}


int AtackNormal( int no, int tno, int sample, int waza )
{
	float	atk = CharParam[CharControl[no].cno].GetAtk()+100+waza;
	float	def = CharParam[CharControl[tno].cno].GetDef()+100;
	int	dmg;
	int	tble[8] = { 0, -25, -50, -75, 100, 75, 50, 25  };

	int	kisyu   = JobParam[CharParam[CharControl[tno].cno].job].ex&EX_KISYU;
	int	atr_def = JobParam[CharParam[CharControl[tno].cno].job].ex&EX_ATR_DEF;
	int	sng_def = JobParam[CharParam[CharControl[tno].cno].job].ex&EX_SNG_DEF;
	int	avg_lim = JobParam[CharParam[CharControl[tno].cno].job].ex&EX_AVG_LIM;
	int	yumi_def = GetBardParam( tno, MG_YUMI_DEF );

	if(CharParam[CharControl[tno].cno].accessary1==200) avg_lim=1;
	if(CharParam[CharControl[tno].cno].accessary1==201) sng_def=1;
	if(CharParam[CharControl[tno].cno].accessary1==204) kisyu=1;

	double	dir1 = atan2( CharControl[tno].px-CharControl[no].px, CharControl[tno].py-CharControl[no].py )/ PAI;
	double	rate1= ABS(tble[CharControl[tno].dir]/100.0 + 1.0-dir1);
	if(rate1<1.0){
	}else if(rate1<2.0){
		rate1 = 2.0-rate1;
	}else{
		rate1 = rate1-2.0;
	}

	double	rate = rate1/3 + 1.0f;

	if(waza || kisyu ){
		rate=1.0f;
	}

	
	double	rate2;
	int		atk_attr;
	int		def_attr;
	if( ItemParam[CharParam[CharControl[no].cno].buki].attr==ATTR_NOT ){
		atk_attr = CharParam[CharControl[no].cno].attr;
	}else{
		atk_attr = ItemParam[CharParam[CharControl[no].cno].buki].attr;
	}
	if( ItemParam[CharParam[CharControl[tno].cno].yoroi].attr==ATTR_NOT ){
		def_attr = CharParam[CharControl[tno].cno].attr;
	}else{
		def_attr = ItemParam[CharParam[CharControl[tno].cno].yoroi].attr;
	}
	rate2 = AttrRate( atk_attr, def_attr );

	if( atr_def && rate2<0 ){
	}else{
		rate += rate2;
	}

	
	if( CharControl[tno].action==CHAR_ACTION_MAGIC && !sng_def ){
		rate += 0.2;
	}

	int	hit_param;
	if( CharParam[CharControl[tno].cno].GetAvd()>0 ){
		hit_param = 20*rate + 80*(CharParam[CharControl[no].cno].GetHit()*2-CharParam[CharControl[tno].cno].GetAvd())/CharParam[CharControl[tno].cno].GetAvd();
	}else{
		hit_param = 100;
	}

	if(avg_lim){ 
		hit_param = LIM( hit_param, 30, 99 );
	}else{
		hit_param = LIM( hit_param, 60, 99 );
	}
	if(waza) hit_param = 100;
	if(CharParam[CharControl[no].cno].buki==96)hit_param = 100;


	if( hit_param > rand()%100 || sample){
		atk = atk*rate;
		if(yumi_def){
			if( ItemParam[CharParam[CharControl[no].cno].buki].type==ITEM_BUKI_BOW ){
				def += yumi_def;
			}
		}
		dmg = LIM( (atk*atk/def-def/3)/4, 1, 99999 );
	}else{
		dmg = 0;
	}
	if(CharParam[CharControl[tno].cno].yoroi==161){	
		if( ItemParam[CharParam[CharControl[no].cno].buki].type==ITEM_BUKI_BOW ){
			dmg = dmg/4;
		}
	}

	if(!sample){
		if(dmg){
			switch( ItemParam[CharParam[CharControl[no].cno].buki].type ){
			case ITEM_BUKI_SWORD:	

				AVG_PlaySE3( 9430 );
				break;
			case ITEM_BUKI_SPEAR:	

				AVG_PlaySE3( 9450 );
				break;
			case ITEM_BUKI_HAMMER:	

				AVG_PlaySE3( 9410 );
				break;
			case ITEM_BUKI_BOW:		

				AVG_PlaySE3( 9420 );
				break;
			case ITEM_BUKI_WAND:	

				AVG_PlaySE3( 9440 );
				break;
			}
		}else{

			AVG_PlaySE3( 9401 );
		}
	}
	
	return dmg;
}

int AtackNormalDmg( int no, int tno, float radian, int waza )
{
	int	dmg = AtackNormal( no, tno, FALSE, waza );
	CHAR_PARAM	*ac = &CharParam[CharControl[no].cno];
	CHAR_PARAM	*tc = &CharParam[CharControl[tno].cno];

	float target_range = (float)sqrt( pow(CharControl[no].px-CharControl[tno].px,2)+pow(CharControl[no].py-CharControl[tno].py,2) );
	if(ac->GetRange()+64 <= target_range && !waza ){
		return 0;
	}

	if( tc->CheckStatusEffct(STATUS_DEF_HP_0) ){
		dmg=0;
	}
	tc->ResetStatusEffctEx( STATUS_DEF_HP_0 );
	tc->ResetStatusEffctEx( STATUS_DEF_MP_0 );

	if(CharControl[tno].action==CHAR_ACTION_STAND){
		if(ac->unit!=tc->unit){
			SetMapCharHangekiAtack( tno, no );
		}
	}

	SetHitAction( tno, dmg, radian);

	if(dmg){
		int	lvup;
		tc->AddHp( -dmg );

		if( tc->php<=0 && tc->did==FALSE ){
			tc->did = TRUE;
			lvup = ac->AddExpAll( tc->lv, TRUE );
		}else{
			lvup = ac->AddExpAll( tc->lv, FALSE );
		}
		if(lvup) SetHitAction( no, 0, 0.0f, HIT_LVUP );
		CharControl[tno].dmg = LIM( (dmg-tc->GetDex())/20, 10, 50);
	}

	return dmg;
}


typedef struct{
	int		flag;
	int		type;

	float	px;
	float	py;

	int		count;
	
	int		damege;
	float	radian;
}HIT_EFFECT_STRUCT;

#define HIT_EFFECT_MAX	32
HIT_EFFECT_STRUCT	HitEffectStruct[HIT_EFFECT_MAX];



void SetHitAction( int tno, int damege, float radian, int type )
{
	int	i,j,k;
	for(i=0;i<HIT_EFFECT_MAX;i++){
		if(!HitEffectStruct[i].flag){
			break;
		}
	}
	j=0;
	if(i==HIT_EFFECT_MAX){
		for(k=0;k<HIT_EFFECT_MAX;k++){
			if(j<HitEffectStruct[k].count){
				j=HitEffectStruct[k].count;
				i=k;
			}
		}
	}
	if(i==HIT_EFFECT_MAX) return ;

	HitEffectStruct[i].flag=TRUE;

	HitEffectStruct[i].count=0;
	HitEffectStruct[i].damege = damege;
	HitEffectStruct[i].radian = radian;
	HitEffectStruct[i].type   = type;

	if(type==HIT_DAMEGE){
		HitEffectStruct[i].px = CharControl[tno].px+rand()%33-16;
		HitEffectStruct[i].py = CharControl[tno].py+rand()%33-16-45;
		if(HitEffectStruct[i].damege){
			GD_SetSprite( GRP_HIT_EFFECT+i, SPR_HIT_EFEECT, 0, TRUE, LAY_EFFECT, FALSE, FALSE, 1, FALSE );
			GD_SetGraphBlendParam( GRP_HIT_EFFECT+i, BLEND_ALPHA );
			GD_SetGraphScaleParam( GRP_HIT_EFFECT+i, SCALE_HIGHT );
		}
	}else{
		HitEffectStruct[i].px = CharControl[tno].px;
		HitEffectStruct[i].py = CharControl[tno].py;
		GD_ResetGraph( GRP_HIT_EFFECT+i );
	}

	if(type==HIT_DAMEGE || 	type==HIT_HEAL ){
		if(HitEffectStruct[i].damege==0 && type==HIT_DAMEGE){
			GD_SetGraphStr( GRP_HIT_DAMEGE+i, BMP_HIT_DAMEGE+1, LAY_EFFECT+1, FALSE, "MISS" );
			GD_SetGraphColor( GRP_HIT_DAMEGE+i, 255, 84/2,201/2,128/2 );
			GD_SetGraphDstRect( GRP_HIT_DAMEGE+i, 0, 0, 14, 14 );
			GD_SetGraphSrcRect( GRP_HIT_DAMEGE+i, 0, 0, 14, 14 );
		}else{
			GD_SetGraphDigit( GRP_HIT_DAMEGE+i, BMP_HIT_DAMEGE, LAY_EFFECT+1, FALSE, HitEffectStruct[i].damege, 0 );
			GD_SetGraphDstRect( GRP_HIT_DAMEGE+i, 0, 0, 14, 14 );
			GD_SetGraphSrcRect( GRP_HIT_DAMEGE+i, 0, 0, 14, 14 );
			if(type==HIT_HEAL){
				GD_SetGraphColor( GRP_HIT_DAMEGE+i, 255,32,128,64 );
			}else{
				GD_SetGraphColor( GRP_HIT_DAMEGE+i, 255,128,128,128 );
			}
		}
		GD_SetGraphScaleParam( GRP_HIT_DAMEGE+i, SCALE_HIGHT );
	}else{
		switch(type){
			case HIT_POWUP:
				GD_SetGraphStr( GRP_HIT_DAMEGE+i, BMP_HIT_DAMEGE+1, LAY_EFFECT+1, FALSE, "POW UP" );
				GD_SetGraphColor( GRP_HIT_DAMEGE+i, 255, 157/2,205/2,255/2 );
				HitEffectStruct[i].damege = 6;
				break;
			case HIT_POWDOWN:
				GD_SetGraphStr( GRP_HIT_DAMEGE+i, BMP_HIT_DAMEGE+1, LAY_EFFECT+1, FALSE, "POW DOWN" );
				GD_SetGraphColor( GRP_HIT_DAMEGE+i, 255, 245/2,99/2,88/2 );
				HitEffectStruct[i].damege = 8;
				break;
			case HIT_DISPEL:
				GD_SetGraphStr( GRP_HIT_DAMEGE+i, BMP_HIT_DAMEGE+1, LAY_EFFECT+1, FALSE, "DISPEL" );
				GD_SetGraphColor( GRP_HIT_DAMEGE+i, 255, 157/2,205/2,255/2 );
				HitEffectStruct[i].damege = 5;
				break;
			case HIT_LVUP:
		AVG_PlaySE3( 9402 );
				GD_SetGraphStr( GRP_HIT_DAMEGE+i, BMP_HIT_DAMEGE+1, LAY_EFFECT+1, FALSE, "LVUP!" );
				GD_SetGraphColor( GRP_HIT_DAMEGE+i, 255, 241/2,245/2,118/2 );
				HitEffectStruct[i].damege = 5;
				break;
		}
		GD_SetGraphDstRect( GRP_HIT_DAMEGE+i, 0, 0, 14, 14 );
		GD_SetGraphSrcRect( GRP_HIT_DAMEGE+i, 0, 0, 14, 14 );


		GD_SetGraphScaleParam( GRP_HIT_DAMEGE+i, SCALE_HIGHT );
	}
}
int RenewHitAction( int char_active )
{
	int	i;
	float	x,y;
	float		rate;


		for(i=0;i<HIT_EFFECT_MAX;i++){
			if(HitEffectStruct[i].flag){
				switch(HitEffectStruct[i].type){
				case HIT_DAMEGE:
					if( GD_WaitSpriteCondition(GRP_HIT_EFFECT+i) || HitEffectStruct[i].count<32 ){
						if(char_active){
							HitEffectStruct[i].count++;
						}

						x = (HitEffectStruct[i].px-MapControl.px)*MapControl.zoom;
						y = (HitEffectStruct[i].py-MapControl.py)*MapControl.zoom;


						GD_SetGraphDisp( GRP_HIT_EFFECT+i, TRUE );
						GD_SetGraphZoom( GRP_HIT_EFFECT+i, MapControl.zoom );
						GD_SetGraphDstMove( GRP_HIT_EFFECT+i, x, y );

						GD_SetGraphDisp( GRP_HIT_DAMEGE+i, TRUE );

						if(HitEffectStruct[i].count<16){
							rate = POYO_CONUT( min(15,HitEffectStruct[i].count), 15,9);
		
							GD_SetGraphDstMove( GRP_HIT_DAMEGE+i, x-7*Digit4keta(HitEffectStruct[i].damege), y );
							GD_SetGraphCenter( GRP_HIT_DAMEGE+i, x - cos(HitEffectStruct[i].radian)*32, y+7 - sin(HitEffectStruct[i].radian)*32 );
							GD_SetGraphZoom( GRP_HIT_DAMEGE+i, 1.0f+rate );
							GD_SetGraphAlpha( GRP_HIT_DAMEGE+i, HitEffectStruct[i].count*17 );
						}else{
							rate = POYO_CONUT( max(0,30-HitEffectStruct[i].count), 15,11);
							GD_SetGraphDstMove( GRP_HIT_DAMEGE+i, x-7*Digit4keta(HitEffectStruct[i].damege), y+32-rate*32 );

							GD_SetGraphCenter( GRP_HIT_DAMEGE+i, x - cos(HitEffectStruct[i].radian)*32, y+7 - sin(HitEffectStruct[i].radian)*32 );

							GD_SetGraphAlpha( GRP_HIT_DAMEGE+i, (32-HitEffectStruct[i].count)*16 );
	
						}

					}else{
						GD_ResetGraph( GRP_HIT_EFFECT+i );
						GD_ResetGraph( GRP_HIT_DAMEGE+i );
						ZeroMemory( &HitEffectStruct[i], sizeof(HIT_EFFECT_STRUCT) );
					}
					break;
				case HIT_MISS:
				case HIT_HEAL:

				case HIT_POWUP:
				case HIT_POWDOWN:
				case HIT_DISPEL:
				case HIT_LVUP:
					if(HitEffectStruct[i].count<32){
						if(char_active){
							HitEffectStruct[i].count++;
						}

						x = (HitEffectStruct[i].px-MapControl.px)*MapControl.zoom;
						y = (HitEffectStruct[i].py-MapControl.py)*MapControl.zoom;

						GD_SetGraphDisp( GRP_HIT_DAMEGE+i, TRUE );
		
						rate = POYO_CONUT(HitEffectStruct[i].count,32,20);

						if(HitEffectStruct[i].type==HIT_HEAL){
							GD_SetGraphDstMove( GRP_HIT_DAMEGE+i, x-7*Digit4keta(HitEffectStruct[i].damege), y - 64*rate );
						}else{
							GD_SetGraphDstMove( GRP_HIT_DAMEGE+i, x-7*HitEffectStruct[i].damege, y - 64*rate );
						}
						GD_SetGraphCenter( GRP_HIT_DAMEGE+i, x, y - 64*rate+7 );
						GD_SetGraphZoom( GRP_HIT_DAMEGE+i, 1.0f+rate );

						if(HitEffectStruct[i].count<20){
							GD_SetGraphAlpha( GRP_HIT_DAMEGE+i, HitEffectStruct[i].count*16 );
						}else{
							GD_SetGraphAlpha( GRP_HIT_DAMEGE+i, 255-255*(HitEffectStruct[i].count-20)/12 );
						}
					}else{
						GD_ResetGraph( GRP_HIT_DAMEGE+i );
						ZeroMemory( &HitEffectStruct[i], sizeof(HIT_EFFECT_STRUCT) );
					}
					break;
				}
			}
		}

	return 1;
}

typedef struct{
	int	flag;
	int	tno;
	int	ano;

	float	px;
	float	py;

	int		count;
	int		waza;

}ARROW_STRUCT;

#define ARROW_MAX	32
ARROW_STRUCT	ArrowStruct[ARROW_MAX];

void ResetArrowAction( int i )
{
	GD_ResetGraph( GRP_ARROW+i );
	ZeroMemory( &ArrowStruct[i], sizeof(ARROW_STRUCT) );
}

void SetArrowAction( int no, int tno, int waza )
{
	int	i,j,k;
	for(i=0;i<ARROW_MAX;i++){
		if(!ArrowStruct[i].flag){
			break;
		}
	}
	j=0;
	if(i==ARROW_MAX){
		for(k=0;k<ARROW_MAX;k++){
			if(j<ArrowStruct[k].count){
				j=ArrowStruct[k].count;
				i=k;
			}
		}
	}
	if(i==ARROW_MAX) return ;

	ArrowStruct[i].flag=TRUE;
	ArrowStruct[i].tno=tno;
	ArrowStruct[i].ano=no;
	ArrowStruct[i].waza=waza;

	ArrowStruct[i].px = CharControl[no].px;
	ArrowStruct[i].py = CharControl[no].py-64;

	ArrowStruct[i].count=0;

	GD_SetGraph( GRP_ARROW+i, BMP_ARROW, LAY_EFFECT, FALSE );	
	GD_SetGraphBlendParam( GRP_ARROW+i, BLEND_ALPHA );
	GD_SetGraphScaleParam( GRP_ARROW+i, SCALE_HIGHT );
}


int RenewArrowAction( int char_active )
{
	int	i;
	double	radian;
	float	x,y;


		for(i=0;i<ARROW_MAX;i++){
			if(ArrowStruct[i].flag){
				x = CharControl[ ArrowStruct[i].tno ].px-ArrowStruct[i].px;
				y = (CharControl[ ArrowStruct[i].tno ].py-64)-ArrowStruct[i].py;

				radian = atan2( y, x );
				if( abs(x)<64 && abs(y)<64 ){
					AtackNormalDmg( ArrowStruct[i].ano, ArrowStruct[i].tno, radian, ArrowStruct[i].waza );
					ResetArrowAction( i );
				}else{
					x = (float)(cos(radian)*32);
					y = (float)(sin(radian)*32);
					ArrowStruct[i].px += x;
					ArrowStruct[i].py += y;

					if(char_active){
						ArrowStruct[i].count++;
					}
					
					GD_SetGraphDisp( GRP_ARROW+i, TRUE );
					GD_SetGraphRoll( GRP_ARROW+i, 180-D3DXToDegree(radian)-90 );
					GD_SetGraphZoom( GRP_ARROW+i, MapControl.zoom );
					GD_SetGraphCenter( GRP_ARROW+i, (ArrowStruct[i].px-MapControl.px+16)*MapControl.zoom, (ArrowStruct[i].py-MapControl.py+16)*MapControl.zoom );
					GD_SetGraphDstMove( GRP_ARROW+i, (ArrowStruct[i].px-MapControl.px)*MapControl.zoom, (ArrowStruct[i].py-MapControl.py)*MapControl.zoom );
				}
			}
		}

	return 1;
}


int WaitBattleEffect( void )
{
	int	i;
	for(i=0;i<HIT_EFFECT_MAX;i++){
		if(HitEffectStruct[i].flag){
			return 0;
		}
	}
	for(i=0;i<ARROW_MAX;i++){
		if(ArrowStruct[i].flag){
			return 0;
		}
	}

	return 1;
}




int RenewCharAction( int char_active )
{
	int	i,j;
	int	ret=0;
	POINT	hp_bar[CHAR_CONTROL_MAX];
	int		hp_bar_max=0;
	float	tx,ty;
	float	px,py;
	double	radian;
	int		char_active_magic=1;
	int		comand_cansel=0;

	RenewArrowAction( char_active );
	RenewHitAction( char_active );
	RenewMagicAction( char_active );
	RenewBardAction( char_active );
	RenewSkillAction( char_active );
	RenewItemAction( char_active );

	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if( CharControl[i].step==CHAR_STEP_MAGIC_WAIT || CharControl[i].step==CHAR_STEP_TECHNIC ){
			char_active_magic=0;
			break;
		}
	}
	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if( (CharControl[i].mw==0 && CharControl[i].step==CHAR_STEP_MAGIC)	
		 || (CharControl[i].step==CHAR_STEP_TECHNIC) ){
			comand_cansel=1;
		}
	}

	for(i=0;i<CHAR_CONTROL_MAX;i++){
		int	alpha=255;
		if( !CharControl[i].flag ) continue;

		px = (CharControl[i].px + ShakeDx - MapControl.px)*MapControl.zoom;
		py = (CharControl[i].py + ShakeDy - MapControl.py)*MapControl.zoom;

		if(char_active && (char_active_magic || CharControl[i].step==CHAR_STEP_MAGIC_WAIT || CharControl[i].step==CHAR_STEP_TECHNIC) ){
			if(CharControl[i].event_flag){
				if(CharControl[i].event_count==0){
					CharControl[i].event_count++;
					if( CharControl[i].ano2==1 ){
						GD_SetSpritePlay(GRP_MAP_CHAR+i, SPR_MAP_CHAR2+CharControl[i].cno, CharControl[i].dir, TRUE, TRUE, CharControl[i].event_act_loop );
					}else if( CharControl[i].ano2==2 ){
						GD_SetSpritePlay(GRP_MAP_CHAR+i, SPR_MAP_CHAR2+CharControl[i].cno, 0, TRUE, TRUE, CharControl[i].event_act_loop );
					}else{
						GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+CharControl[i].event_act*8, TRUE, TRUE, CharControl[i].event_act_loop );
					}
				}else{
					if( !GD_WaitSpriteCondition( GRP_MAP_CHAR+i) ){
						CharControl[i].event_flag=2;
					}
				}
				
				RenewCharStepMove( i );
				if( WaitMapCharTartget(i) ){
					CharControl[i].tcnt = LIM( CharControl[i].tcnt-1, 0, 7 );
					if(CharControl[i].tcnt==0){
					}else{
						for(j=0;j<7;j++){
							CharControl[i].tx[j]  = CharControl[i].tx[j+1];
							CharControl[i].ty[j]  = CharControl[i].ty[j+1];
						}
					}
				}
			}else{
				float	syahen;
				int		char_step = CharControl[i].step;
				if( CharParam[CharControl[i].cno].php<=0 ){
					CharParam[CharControl[i].cno].did = TRUE;
					CharControl[i].action=CHAR_ACTION_DID;	
					CharControl[i].step=CHAR_STEP_DID;
				}
			if( !(JobParam[CharParam[CharControl[i].cno].job].ex&EX_HAKO) || CharControl[i].step==CHAR_STEP_DID ){

				switch( CharControl[i].action ){
					case CHAR_ACTION_COMMAND:	
						ret=1;
						break;
					case CHAR_ACTION_STAND:	
						
						break;
					case CHAR_ACTION_MOVE:		
						if( GlobalCount%max(1,CharControl[i].serch_wait)==0 ){
							syahen = (float)sqrt( pow(CharControl[i].px-CharControl[i].tx[0],2) + pow(CharControl[i].py-CharControl[i].ty[0],2) );
							SetMapCharTartgetMove( i, CharControl[i].tx[0], CharControl[i].ty[0], -1, -1, CharControl[i].mov, 1 );
							CharControl[i].serch_wait=(int)syahen/16;
						}

						if( WaitMapCharTartget(i) ){
							CharControl[i].tcnt = LIM( CharControl[i].tcnt-1, 0, 7 );
							if(CharControl[i].tcnt==0){
								if( !comand_cansel && !CharThink[CharControl[i].cno].flag ){
									CharControl[i].step   = CHAR_STEP_STAND;
									SetMapCharCommand(i);
									ret=1;
								}else{
									SetMapCharStand( i );
								}
							}else{
								for(j=0;j<7;j++){
									CharControl[i].tx[j]  = CharControl[i].tx[j+1];
									CharControl[i].ty[j]  = CharControl[i].ty[j+1];
								}
							}
						}
						break;
					case CHAR_ACTION_HANGEKI:
						break;
					case CHAR_ACTION_ATACK:		
						if(CharControl[i].tcno==-1){
							if( !comand_cansel && !CharThink[CharControl[i].cno].flag ){
								CharControl[i].step   = CHAR_STEP_STAND;
								SetMapCharCommand(i);
								ret=1;
							}else{
								SetMapCharStand(i);
							}
						}else if(CharParam[CharControl[CharControl[i].tcno].cno].did && CharParam[CharControl[i].cno].unit==0 ){
							if( !comand_cansel && !CharThink[CharControl[i].cno].flag ){
								CharControl[i].step   = CHAR_STEP_STAND;
								SetMapCharCommand(i);
								ret=1;
							}else{
								SetMapCharStand(i);
							}
						}else{
							syahen = (float)sqrt( pow(CharControl[i].px-CharControl[CharControl[i].tcno].px,2) + pow(CharControl[i].py-CharControl[CharControl[i].tcno].py,2) );

							if( syahen>CharParam[CharControl[i].cno].GetRange() && CharControl[i].step==CHAR_STEP_ATACK ){
								CharControl[i].step = CHAR_STEP_MOVE;
							}else if( syahen<=CharParam[CharControl[i].cno].GetRange() && CharControl[i].step==CHAR_STEP_MOVE ){
								CharControl[i].step = CHAR_STEP_ATACK;
							}else if( CharControl[i].aw<=CharParam[CharControl[i].cno].GetWsp()/2 ){
								if( syahen>CharParam[CharControl[i].cno].GetRange() && CharControl[i].step==CHAR_STEP_ATACK_WAIT ){
									CharControl[i].step = CHAR_STEP_MOVE;
								}
							}
							if(CharControl[i].step==CHAR_STEP_MOVE){
								if(CharParam[CharControl[i].cno].unit==0){
									if( GlobalCount%max(1,CharControl[i].serch_wait)==0 ){

										SetMapCharTartgetAtack( i, CharControl[i].tcno );
										CharControl[i].serch_wait=(int)syahen/16;
									}
								}
							}
						}
						break;
					case CHAR_ACTION_TECHNIC:	
						break;
					case CHAR_ACTION_ITEM:	
						break;
					case CHAR_ACTION_MAGIC:		
						j=CharControl[i].step==CHAR_STEP_MAGIC_END;
						if(MagicParam[CharControl[i].mgno].scope_type==RANGE_TAGET){
							if(CharControl[i].tcno==-1){
								j=1;
							}else{
								j|=CharParam[CharControl[CharControl[i].tcno].cno].did;
							}
						}
						
						if( j ){
							if( !comand_cansel && !CharThink[CharControl[i].cno].flag ){
								CharControl[i].step   = CHAR_STEP_STAND;
								SetMapCharCommand(i);
								ret=1;
							}else{
								SetMapCharStand( i );
							}
						}
						break;

					case CHAR_ACTION_BARD:
						break;

					case CHAR_ACTION_DID:		
						if(BackUpMapChar[i].engun && MapControl.battle_flag){
							int	etime = 0;

							if(BackUpMapChar[i].loop_cnt==1){
								etime = BackUpMapChar[i].etime;
							}else{
								etime = BackUpMapChar[i].inter;
							}

							if( etime<=BackUpMapChar[i].etime_cnt ){
								if(BackUpMapChar[i].loop==0 || BackUpMapChar[i].loop>=BackUpMapChar[i].loop_cnt ){
									int	hujimi = CharParam[CharControl[i].cno].hujimi;

									SetCharParam( CharControl[i].cno, BackUpMapChar[i].job, BackUpMapChar[i].lv, BackUpMapChar[i].buki );

									CharParam[CharControl[i].cno].attr   = BackUpMapChar[i].attr;
									CharParam[CharControl[i].cno].hujimi = hujimi;
									SetMapChar( i, CharControl[i].cno, BackUpMapChar[i].px, BackUpMapChar[i].py, BackUpMapChar[i].dir );

									BackUpMapChar[i].etime_cnt=0;
									BackUpMapChar[i].loop_cnt++;

									SetMapCharStand( i );
									CharControl[i].start_count=0;
								}
							}
							BackUpMapChar[i].etime_cnt++;
						}
						break;
				}

				switch(char_step){
					case CHAR_STEP_STAND:
						GD_ResetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i );
	

						if( JobParam[CharParam[CharControl[i].cno].job].ex&EX_HAKO ){
						}else{
							if( JobParam[CharParam[CharControl[i].cno].job].ex&EX_BOSS2 ){
								if(CharParam[CharControl[i].cno].job==87 || CharParam[CharControl[i].cno].job==88){
									GD_SetSpriteNo( GRP_MAP_CHAR+i, CharControl[i].dir+8 );
									GD_SetSpriteLoop( GRP_MAP_CHAR+i, TRUE, 0 );
								}else{
									GD_SetSpriteNo( GRP_MAP_CHAR+i, CharControl[i].dir+16 );
									GD_SetSpriteLoop( GRP_MAP_CHAR+i, TRUE, 0 );
								}
							}else{
								if(CharControl[i].dmg==0){
									GD_SetSpriteNo( GRP_MAP_CHAR+i, CharControl[i].dir+8);
									GD_SetSpriteLoop( GRP_MAP_CHAR+i, TRUE, 0 );
								}
							}
						}
						break;

					case CHAR_STEP_MOVE:
						if(CharControl[i].aw<=CharParam[CharControl[i].cno].GetWsp()/2 ){
							if(CharControl[i].sw<=CharParam[CharControl[i].cno].GetWsp()/2 ){
								if(CharControl[i].dmg==0){
									RenewCharStepMove( i );
									if(CharControl[i].dir!=-1){
										GD_SetSpriteNo( GRP_MAP_CHAR+i, CharControl[i].dir);
										GD_SetSpriteLoop( GRP_MAP_CHAR+i, TRUE, 0 );
									}
								}
							}
						}
						break;
					case CHAR_STEP_ATACK:
						if(CharControl[i].tcno==-1){
							if( !comand_cansel && !CharThink[CharControl[i].cno].flag ){
								CharControl[i].step   = CHAR_STEP_STAND;
								SetMapCharCommand(i);
								ret=1;
							}else{
								SetMapCharStand(i);
							}
							break;
						}

						if(CharParam[CharControl[CharControl[i].tcno].cno].did ){
							if( !comand_cansel && !CharThink[CharControl[i].cno].flag ){
								CharControl[i].step   = CHAR_STEP_STAND;
								SetMapCharCommand(i);
								ret=1;
							}else{
								SetMapCharStand(i);
							}
							break;
						}
						radian = atan2( CharControl[ CharControl[i].tcno ].py-CharControl[i].py, CharControl[ CharControl[i].tcno ].px-CharControl[i].px );
						
						SetCharDir(i,radian);

						if(CharControl[i].aw==0 && CharControl[i].sw==0){

							GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+24, TRUE, TRUE, 1, FALSE );
							CharControl[i].step=CHAR_STEP_ATACK2;
							CharControl[ CharControl[i].tcno ].shock += 200;

							if(CharControl[i].action==CHAR_ACTION_HANGEKI && CharParam[CharControl[i].cno].CheckStatusEffct(STATUS_DEF_HANGEKI) ){
								CharControl[i].aw = 0;
							}else{
								CharControl[i].aw = CharParam[CharControl[i].cno].GetWsp();
							}

							CharControl[i].atk_count=0;



							switch( ItemParam[CharParam[CharControl[i].cno].buki].type ){
							case ITEM_BUKI_SWORD:	
								AVG_PlaySE3( 9431 );
								break;
							case ITEM_BUKI_SPEAR:	
								AVG_PlaySE3( 9451 );
								break;
							case ITEM_BUKI_HAMMER:	
								AVG_PlaySE3( 9411 );
								break;
							case ITEM_BUKI_BOW:		
								AVG_PlaySE3( 9421 );
								break;
							case ITEM_BUKI_WAND:	
								AVG_PlaySE3( 9441 );
								break;
							}
						}else{
							if(CharControl[i].action==CHAR_ACTION_HANGEKI){
								SetMapCharStand(i);
							}else{
								GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+16, TRUE, TRUE, 0, FALSE );
								CharControl[i].step=CHAR_STEP_ATACK_WAIT;
							}
						}
						break;
					case CHAR_STEP_ATACK2:
						if(CharControl[i].tcno==-1){
							if( !comand_cansel && !CharThink[CharControl[i].cno].flag ){
								CharControl[i].step   = CHAR_STEP_STAND;
								SetMapCharCommand(i);
								ret=1;
							}else{
								SetMapCharStand(i);
							}
							break;
						}

						radian = atan2( CharControl[ CharControl[i].tcno ].py-CharControl[i].py, CharControl[ CharControl[i].tcno ].px-CharControl[i].px );
						if( CharControl[i].atk_count==6*4 ){
	
							if( ItemParam[CharParam[CharControl[i].cno].buki].type==ITEM_BUKI_BOW){
								SetArrowAction( i, CharControl[i].tcno );
							}else{
								AtackNormalDmg( i, CharControl[i].tcno, radian );
							}
						}
						CharControl[i].atk_count++;
						
						if( !GD_WaitSpriteCondition(GRP_MAP_CHAR+i)  && CharControl[i].atk_count>6*4 ){
							CharControl[i].atk_count=0;

							SetCharDir(i,radian);


							GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+16, TRUE, TRUE, 0, FALSE );
							CharControl[i].step=CHAR_STEP_ATACK_WAIT;
						}
						break;
					case CHAR_STEP_ATACK_WAIT:
						if(CharControl[i].action==CHAR_ACTION_HANGEKI){
							SetMapCharStand(i);
						}else{
							if(CharControl[i].dmg==0){
								if(CharControl[i].tcno!=-1){
									radian = atan2( CharControl[ CharControl[i].tcno ].py-CharControl[i].py, CharControl[ CharControl[i].tcno ].px-CharControl[i].px );
									SetCharDir(i,radian);
		
									GD_SetSpriteNo( GRP_MAP_CHAR+i, CharControl[i].dir+16 );
								}
							}

							if(CharControl[i].aw<=0 && CharControl[i].sw<=0){
								CharControl[i].step=CHAR_STEP_ATACK;
							}
						}
						break;

					case CHAR_STEP_MAGIC_STAND:
						if(CharParam[CharControl[i].cno].job==87 || CharParam[CharControl[i].cno].job==88){
							GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+8, TRUE, TRUE, 0, FALSE );
						}else{
							GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+48, TRUE, TRUE, 0, FALSE );
						}
						CharControl[i].mw = GetMagicSng( CharControl[i].mgno, CharControl[i].cno );
						CharControl[i].step = CHAR_STEP_MAGIC;
						

						break;
					case CHAR_STEP_MAGIC:
						if(CharControl[i].dmg==1){
							if(CharParam[CharControl[i].cno].job==87 || CharParam[CharControl[i].cno].job==88){
								GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+8, TRUE, TRUE, 0, FALSE );
							}else{
								GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+48, TRUE, TRUE, 0, FALSE );
							}

						}

						if(CharControl[i].mw<=0){
							SetMagicAction( i );
							CharControl[i].mw=0;
	
								char_active_magic=0;

								GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+56, TRUE, TRUE, 1, FALSE );
								CharControl[i].step=CHAR_STEP_MAGIC_WAIT;
								CharControl[i].step_count=0;
	
						}
						break;
					case CHAR_STEP_MAGIC_WAIT:
	
						if( WaitMagicAction(i) ){
							ResetMagicAction(i);
							CharControl[i].step=CHAR_STEP_MAGIC_END;
							CharControl[i].step_count=0;
						}
						break;
					case CHAR_STEP_MAGIC_END:
						break;

					case CHAR_STEP_TECHNIC_STAND:			
						char_active_magic=0;
						CharControl[i].step=CHAR_STEP_TECHNIC;
						CharControl[i].step_count=0;
						break;
					case CHAR_STEP_TECHNIC:			
						if( WaitSkillAction(i) ){
							ResetSkillAction(i);

							CharControl[i].step=CHAR_STEP_TECHNIC_WAIT;
							CharControl[i].step_count=0;
						}
						break;
					case CHAR_STEP_TECHNIC_WAIT:	
						if(CharControl[i].sw<=0 ){
							if( !comand_cansel && !CharThink[CharControl[i].cno].flag ){
								CharControl[i].step   = CHAR_STEP_STAND;
								SetMapCharCommand(i);
								ret=1;
							}else{
								SetMapCharStand(i);
							}
						}
						break;

					case CHAR_STEP_ITEM:			
						char_active_magic=0;
						if( WaitItemAction(i) ){
							ResetItemAction(i);

							CharControl[i].step=CHAR_STEP_TECHNIC_WAIT;
							CharControl[i].step_count=0;
						}
						break;

					case CHAR_STEP_BARD_STAND:
						GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+48, TRUE, TRUE, 0, FALSE );
						CharControl[i].step=CHAR_STEP_BARD;
						break;
					case CHAR_STEP_BARD:
						if(CharControl[i].dmg==0){
	
							GD_SetSpriteNo( GRP_MAP_CHAR+i, CharControl[i].dir+48 );
						}
						break;

					case CHAR_STEP_DID:
						if(CharControl[i].step_count<16){
							alpha = 255-255*CharControl[i].step_count/16;
							GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*0+i, alpha );
						}else{
							for(j=0;j<6;j++){
								GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*j+i, FALSE );
							}
						}
	
	

						break;

				}
			}
				if(CharControl[i].step != CHAR_STEP_BARD && CharControl[i].bak_step != CHAR_STEP_BARD){
					ResetBardAction( i );
				}

				CharControl[i].shock = max( CharControl[i].shock-1, 0 );
				CharControl[i].dmg = max( CharControl[i].dmg-1, 0 );

				if(CharControl[i].dmg==0){
					CharControl[i].sw = max( CharControl[i].sw-1, 0 );
					CharControl[i].aw = max( CharControl[i].aw-1, 0 );
					CharControl[i].mw = max( CharControl[i].mw-1, 0 );
				}
				CharControl[i].step_count++;
				CharControl[i].total_count++;
				CharControl[i].start_count++;

				if(MapControl.battle_flag){
					if(CharControl[i].start_count<16){
						GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*0+i, alpha*CharControl[i].step_count/16 );
					}else{
						GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*0+i, alpha );
					}
				}else{
					GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*0+i, 255 );
				}

				if( CharControl[i].action==CHAR_ACTION_STAND){	
					CharControl[i].mp_count+= CharParam[CharControl[i].cno].GetMpr();
				}else{
					if( JobParam[CharParam[CharControl[i].cno].job].ex&EX_GINYU || CharParam[CharControl[i].cno].accessary1==203 ){
						if( CharControl[i].action!=CHAR_ACTION_BARD){	
							CharControl[i].mp_count+= CharParam[CharControl[i].cno].GetMpr()/2;
						}
					}
					


				}
				if(CharControl[i].mp_count>=50000){
					CharParam[CharControl[i].cno].AddMp( CharControl[i].mp_count/50000 );
					CharControl[i].mp_count = CharControl[i].mp_count%50000;
				}
				CharParam[CharControl[i].cno].RenewStatusEffct();

				CharControl[i].event_count = 0;
			}
		}

		if( JobParam[CharParam[CharControl[i].cno].job].ex&EX_HAKO ){
			if( CharParam[CharControl[i].cno].job==89 ){
				GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, 27, TRUE, TRUE, 1, FALSE );
			}else{
				GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, 0, TRUE, TRUE, 1, FALSE );
			}
		}else{
			if(CharControl[i].dmg){
				if( JobParam[CharParam[CharControl[i].cno].job].ex&EX_BOSS2 ){
				}else{
					if( !(CharControl[i].step==CHAR_STEP_ATACK || CharControl[i].step==CHAR_STEP_ATACK2 || CharControl[i].action==CHAR_ACTION_TECHNIC) ){
						GD_SetSpritePlay( GRP_MAP_CHAR+i, SPR_MAP_CHAR+CharControl[i].ano, CharControl[i].dir+64, TRUE, TRUE, 1, FALSE );
					}
				}
			}
		}

		GD_SetGraphLayer( GRP_MAP_CHAR+i, LAY_MAP_OBJECT+CharControl[i].py );
		GD_SetGraphDstMove( GRP_MAP_CHAR+i, px, py );
		GD_SetGraphZoom( GRP_MAP_CHAR+i, MapControl.zoom );
		GD_SetGraphColor2( GRP_MAP_CHAR+i, 128,128,128 );


		if(CharMarkerPlayer){
			if(CharParam[CharControl[i].cno].unit==0){
				GD_SetGraphColor2( GRP_MAP_CHAR+i, 128,128,128+STD_LimitLoop(GlobalCount*2,64) );
			}
		}
		if(CharMarkerEnemy){
			if(CharParam[CharControl[i].cno].unit==1){
				GD_SetGraphColor2( GRP_MAP_CHAR+i, 128+STD_LimitLoop(GlobalCount*2,64),128,128 );
			}
		}

		if(CharMarkerSelect[i]==1)	GD_SetGraphColor2( GRP_MAP_CHAR+i, 128,128+30,255 );
		if(CharMarkerAtack[i]==1)	GD_SetGraphColor2( GRP_MAP_CHAR+i, 128+110,128+6,128 );

		if(CharMarkerSelect[i]==2)	GD_SetGraphColor2( GRP_MAP_CHAR+i, 0,0,255 );
		if(CharMarkerAtack[i]==2)	GD_SetGraphColor2( GRP_MAP_CHAR+i, 255,0,0 );

		if(CharMarkerBoss==i)	GD_SetGraphColor2( GRP_MAP_CHAR+i, 128+STD_LimitLoop(GlobalCount*2,64),128,128 );






		int	hpx = px-32;
		int	hpy = py;
		int	j,k;

		
		if( hpx < 10 || hpx > 800-64-10 || hpy < 24 || hpy > 600-6-10 ){
			int	hpx2 = LIM(hpx,10,800-64-10);
			int	hpy2 = LIM(hpy,24,600- 6-10);

			while(1){
				k=0;
				for(j=0;j<hp_bar_max;j++){
					if( hp_bar[j].x > hpx2-72 && hp_bar[j].x < hpx2+72 &&
						hp_bar[j].y > hpy2-20 && hp_bar[j].y < hpy2+20 ){

						if( hpx < 10 )				hpx2+=72;
						else if( hpx > 800-64-10)	hpx2-=72;
						else if( hpy < 24)			hpy2+=20;
						else if( hpy > 600-6-10 )	hpy2-=20;
						k=1;
						break;
					}

				}
				if(k==0)
					break;
			}
			hpx = hp_bar[hp_bar_max].x = LIM(hpx2,10,800-64-10);
			hpy = hp_bar[hp_bar_max].y = LIM(hpy2,24,600- 6-10);
			hp_bar_max++;
			GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX+i,   128 );
			GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+i, 128 );
			GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*3+i, 128 );
			alpha=128;

			GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*3+i, TRUE );
		}else{
			hpx = LIM(hpx,10,800-64-10);
			hpy = LIM(hpy,24,600- 6-10);
			GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX+i, 255 );
			GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+i, 255 );
			GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*3+i, 255 );

			GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*3+i, FALSE );
		}
		GD_SetGraphDstMove( GRP_MAP_CHAR+CHAR_CONTROL_MAX+i, hpx, hpy );




		GD_SetGraphDstRect( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+i, hpx+9, hpy+2, 46*CharParam[CharControl[i].cno].php/CharParam[CharControl[i].cno].GetHp(), 2 );

		GD_SetGraphDstMove( GRP_MAP_CHAR+CHAR_CONTROL_MAX*3+i, hpx, hpy-12 );


		if(MapControl.battle_flag==0 || CharParam[CharControl[i].cno].did){
			GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*1+i, FALSE );
			GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+i, FALSE );
			GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*3+i, FALSE );
		}else{
			GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*1+i, TRUE );
			GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*2+i, TRUE );
			GD_SetGraphDisp( GRP_MAP_CHAR+CHAR_CONTROL_MAX*3+i, TRUE );
		}

		if(CharControl[i].step==CHAR_STEP_MAGIC){
			radian = CharControl[i].total_count/60.0*PAI*2;

			if( JobParam[CharParam[CharControl[i].cno].job].ex&EX_BOSS2 ){
				GD_SetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i, BMP_EFFECT+2, LAY_MAP_MOVE2, ON );
				if(CharParam[CharControl[i].cno].job==87 || CharParam[CharControl[i].cno].job==88){
					GD_SetGraphRoll2( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i, px+200*MapControl.zoom, py-88*MapControl.zoom, 256*MapControl.zoom, 128*MapControl.zoom, CharControl[i].total_count*2 );
				}else{
					GD_SetGraphRoll2( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i, px-52*MapControl.zoom, py-36*MapControl.zoom, 128*MapControl.zoom, 64*MapControl.zoom, CharControl[i].total_count*2 );
				}
			}else{
				GD_SetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i, BMP_EFFECT, LAY_MAP_MOVE2, ON );
				GD_SetGraphRoll2( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i, px, py, 64*MapControl.zoom, 32*MapControl.zoom, CharControl[i].total_count*2 );
			}
			GD_SetGraphBlendParam( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i, BLEND_ALPHA );
			GD_SetGraphScaleParam( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i, SCALE_HIGHT );

			GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i, 191+cos(radian)*64 );
		}
		if(	!(CHAR_STEP_MAGIC<=CharControl[i].step && CharControl[i].step<=CHAR_STEP_MAGIC_EFEECT) ){
			GD_ResetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+i );
		}
		if( CharControl[i].bak_step != CharControl[i].step ){
			CharControl[i].step_count=0;
		}
		CharControl[i].bak_step = CharControl[i].step;
	}
	return ret;
}

