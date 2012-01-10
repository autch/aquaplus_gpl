#ifndef	_GM_ITEM_H_
#define _GM_ITEM_H_

enum{
	ITEM_BUKI_SWORD=0,	
	ITEM_BUKI_SPEAR,	
	ITEM_BUKI_HAMMER,	
	ITEM_BUKI_BOW,		
	ITEM_BUKI_WAND,		
	ITEM_BUKI_OTHER,	

	ITEM_YOROI_ARMOUR1,	
	ITEM_YOROI_ARMOUR2,	
	ITEM_YOROI_CHAIN,	
	ITEM_YOROI_CLOTHE,	
	ITEM_YOROI_OTHER,	

	ITEM_ACCE_RING,		
	ITEM_ACCE_BRACELET,	
	ITEM_ACCE_GAUNTLET,	
	ITEM_ACCE_OTHER1,

	ITEM_ACCE_BOOTS,	
	ITEM_ACCE_NECKLACE,	
	ITEM_ACCE_HELMET,	
	ITEM_ACCE_CAP,		
	ITEM_ACCE_OTHER2,

	ITEM_TOOL_POTION1,	
	ITEM_TOOL_POTION2,	
	ITEM_TOOL_POTION3,	
	ITEM_TOOL_POTION4,	
	ITEM_TOOL_EVENT,	
	ITEM_TOOL_OTHER,	

	ITEM_NOTTING,	
};

typedef struct{
	BYTE	name[32];	

	BYTE	shop;	

	BYTE	type;	
	BYTE	attr;	


	
	short	hp;		
	short	mp;		

	short	str;	
	short	vit;	
	short	dex;	
	short	spd;	
	short	wiz;	
	short	mnd;	

	short	mpr;		
	short	sng;		

	short	mov;		

	
	short	wsp;		
	short	wsp_max;	

	short	atk;		
	short	def;		

	short	range;	
	long	buy;
	char	help[32*2*2];
}ITEM_PARAM;

#define ITEM_MAX	1024
extern ITEM_PARAM	ItemParam[ITEM_MAX];

typedef struct{
	short	ino;
	short	amount;
}ITEM_BOX;

extern ITEM_BOX	ItemBox[ITEM_MAX];

#define GET_BATTLE_ITEM_MAX	22

extern int	GetBttleItemCount;
extern int	GetBttleItem[GET_BATTLE_ITEM_MAX];

extern void ITM_GetBttleItemInit( void );

extern int ITM_ItemDropItem( int ino );
extern int ITM_ItemDropRank( int rank );
extern int ITM_ItemDrop( int job_no, int tlv,int kari );


extern void TestItemAll( void );

extern int ITM_GetItemEquipAmount( int ino );
extern int ITM_GetItemAmount( int ino );
extern int ITM_GetItem( int ino, int amount );
extern int ITM_LostItem( int ino, int amount );
extern int ITM_GetEquipType( int ino );


#endif

