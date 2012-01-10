#ifndef	_GM_WORLD_H_
#define _GM_WORLD_H_


#define WLD_GRP_0			(0)
#define WLD_GRP_WORLD		(WLD_GRP_0+1)
#define WLD_GRP_SYSTEM		(WLD_GRP_WORLD+32)
#define WLD_GRP_WINDW		(WLD_GRP_SYSTEM+32)

#define WLD_GRP_WORK		(WLD_GRP_WINDW+200)
#define WLD_GRP_DEBUG		(WLD_GRP_WORK+32)


#define WLD_BMP_0			(0)
#define WLD_BMP_WORLD		(WLD_BMP_0+1)
#define WLD_BMP_SYSTEM		(WLD_BMP_WORLD+2)
#define WLD_BMP_WORK		(WLD_BMP_SYSTEM+32)
#define WLD_BMP_DEBUG		(WLD_BMP_WORK+32)


#define WLD_SPR_0			(0)
#define WLD_SPR_WORLD		(WLD_SPR_0+1)
#define WLD_SPR_CHAR_SP		(WLD_SPR_WORLD+1)
#define WLD_SPR_WORK		(WLD_SPR_CHAR_SP+32)
#define WLD_SPR_DEBUG		(WLD_SPR_WORK+32)


#define WLD_LAY_0			(0)
#define WLD_LAY_WORLD		(WLD_LAY_0+1)
#define WLD_LAY_SYSTEM		(WLD_LAY_WORLD+5)
#define WLD_LAY_DEBUG		(WLD_LAY_SYSTEM+10)



#define WINDOW_COLOR_SYS	0


extern void WLD_SetRightBar18( int win_bno, int gno, int layer, int mouse );
extern void WLD_SetRightBar18Disp( int gno, int disp );
extern void WLD_ResetRightBar18( int gno );
extern void WLD_RenewRightBar18( int gno, int page, int page_max, int um=0, int dm=0, int click=0 );
extern int WLD_DragRightBar18( int *page, int *page_max, int select_no );

extern void SAV_StartReset( void );

extern int	WLD_SaveLoad( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse, int next );

extern short	ShopItemTable[5][256];
extern short	ShopItemMax[5];

extern void WLD_SetItemShopMenu( int item_no );
extern int	WLD_ItemShop( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse, int kajiya );

extern int	WLD_Heitan( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse );
extern int	WLD_ItemList( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse );
extern int	WLD_Butai( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse );

extern int	WLD_System( void );
extern int	SVP_System( void );
extern int	SVP2_System(void);
extern int	END_System(void);

extern int	CSL_Item1System( int kajiya );
extern int	CSL_Item2System( void );
extern int	CSL_System( void );

extern int	TTL_System(void);

#endif

