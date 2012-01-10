#ifndef	_GM_CHAR_THINK_H_
#define _GM_CHAR_THINK_H_

#include "GM_Char.h"



enum{
	MOVE_THINK_ATACK,		
	MOVE_THINK_SURPRISE,	
	MOVE_THINK_SEARCH,		
	MOVE_THINK_DEFENSE,		
	MOVE_THINK_NO_MOVE,		
	MOVE_THINK_NO_MOVE2,	
	MOVE_THINK_SUPPORT		
};

enum{
	ACTION_THINK_ATACK,		
	ACTION_THINK_SHOT,		
	ACTION_THINK_KNIGHT,	
	ACTION_THINK_MAGIC,		
	ACTION_THINK_HEAL,		
	ACTION_THINK_SHOT_TEL,	
	ACTION_THINK_HEAL_MAGIC,
};

typedef struct{
	short	flag;
	short	move;	
	short	action;	

	short	tno;	
	float	tx, ty;	
	short	range;	
	int		count;
	int		mode;		
	int		mode_cnt;	

	short	heito_tno;	
	short	heito_cnt;	

	short	telepo;

	short	kipu;
}CHAR_THINK_PARAM;

extern CHAR_THINK_PARAM	CharThink[CHAR_PARAM_MAX];


extern void SetMapCharThinkTouhatu( int no, int tno );
extern void AllAtackMapCharThink( void );


extern void SetKipuMapCharThink( int no );
extern void LoadKipuMapCharThink( int no );


extern void SetMapCharThink( int no, int move_think, int action_think, int tx=0, int ty=0, int range=512 );
extern void ResetMapCharThink( int no );

extern void RenewMapCharThink( void );



#endif

