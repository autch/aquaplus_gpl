#ifndef	_GM_MAGIC_H_
#define _GM_MAGIC_H_

enum{
	MAGIC_ZOKUSEI=0,	
	MAGIC_SENSEI,		
	MAGIC_HOLY,			
	MAGIC_DARK,			
	MAGIC_BARD,			
	MAGIC_ELF,			
	MAGIC_MONSTAR,		
	MAGIC_SKILL,
};


enum{
	RANGE_TAGET=0,		
	RANGE_SCOPE,		
	RANGE_SELF,			
	RANGE_SELF_SCOPE,	
	RANGE_LINE,			
	RANGE_FAN,			
	RANGE_ALL,			
};
enum{
	MG_TYPE_NOT=0,		

	MG_TYPE_DAMAGE,		
	MG_TYPE_DAMAGE_MP,	
	MG_TYPE_HEAL,		
	MG_TYPE_MP,			
	MG_TYPE_HP_GET,
	MG_TYPE_MP_GET,

	MG_TYPE_DISPEL,		

	MG_TYPE_ATK,		
	MG_TYPE_MGK,		

	MG_TYPE_DEF,		
	MG_TYPE_RGS,		

	MG_TYPE_WSP,		
	MG_TYPE_MOV,		

	MG_TYPE_HIT,		
	MG_TYPE_AVD,		

	MG_TYPE_STAN,		
	MG_TYPE_POISON,		

	MG_TYPE_HACK,		
	MG_TYPE_TENI,		

	SK_TOUHATU,		
	SK_ALL_ATACK,	
	SK_SENPUKU,		

	SK_DEF_HP_0,
	SK_DEF_MP_0,
	SK_DEF_HANGEKI,
	SK_ITEM_BAI,
	MG_YUMI_DEF,
};

typedef struct{
	BYTE	name[32];	
	BYTE	type;
	short	mno;

	short	mp;			
	short	sng;		
	short	range;		
	short	scope;		
	short	scope_type;	
	short	attr;		
	float	zoom;		

	short	param[2];	
	DWORD	param_type[2];	

	int		sound;
	char	*help;

}MAGIC_PARAM;
#define MAGIC_MAX	128
extern MAGIC_PARAM	MagicParam[MAGIC_MAX];

extern BYTE	MagicTypeChar[10];

extern int	GetMagicNo( int type, int mno );

extern int GetMagicRange( int mgno, int cno );
extern int GetMagicScope( int mgno, int cno );
extern int GetMagicSng( int mgno, int cno );

extern int MagicDmageNormal( int no, int tno, int mgno );	
extern int MagicRestNormal( int no, int tno, int mgno );	


extern void SetMagicAction( int no );
extern void ResetMagicAction( int no );
extern void ResetMagicActionAll(void);


extern BOOL WaitMagicAction( int no );
extern BOOL CheckMagicAction( int no );

extern int RenewMagicAction( int char_active );



extern void SetBardAction( int no );
extern void ResetBardAction( int no );
extern void ResetBardActionAll(void);
extern void GetBardExp( int no, int tlv );

extern int GetBardParam( int no, DWORD type );
extern int RenewBardAction( int char_active );


extern BOOL SetSkillAction( int no );
extern void ResetSkillAction( int no );
extern BOOL WaitSkillAction( int no );

extern int RenewSkillAction( int char_active );


extern void SetItemAction( int no );
extern void ResetItemAction( int no );

extern BOOL WaitItemAction( int no );
extern int RenewItemAction( int char_active );

extern void ResetMagicActionAll2(void);


#endif

