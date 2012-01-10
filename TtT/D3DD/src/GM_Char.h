#ifndef	_GM_CHAR_H_
#define _GM_CHAR_H_

#include "GM_Item.h"
#include "GM_Magic.h"

enum{
	CHAR_STEP_STAND,	

	CHAR_STEP_MOVE,	

	CHAR_STEP_ATACK,		
	CHAR_STEP_ATACK2,		
	CHAR_STEP_ATACK_WAIT,	

	CHAR_STEP_TECHNIC_STAND,
	CHAR_STEP_TECHNIC,		
	CHAR_STEP_TECHNIC_WAIT,	
	
	CHAR_STEP_MAGIC_STAND,	
	CHAR_STEP_MAGIC,		
	CHAR_STEP_MAGIC_WAIT,	
	CHAR_STEP_MAGIC_EFEECT,	
	CHAR_STEP_MAGIC_END,	

	CHAR_STEP_BARD_STAND,	
	CHAR_STEP_BARD,			

	CHAR_STEP_ITEM,		

	CHAR_STEP_DID,		
};


enum{
	CHAR_ACTION_COMMAND,	

	CHAR_ACTION_STAND,	

	CHAR_ACTION_MOVE,	

	CHAR_ACTION_ATACK,		

	CHAR_ACTION_TECHNIC,		
	
	CHAR_ACTION_MAGIC,		

	CHAR_ACTION_BARD,		

	CHAR_ACTION_DID,	

	CHAR_ACTION_HANGEKI,	

	CHAR_ACTION_ITEM,		
};



typedef struct{
	BYTE	flag;	


	BYTE	cno;	

	short	disp;	





	BYTE	action;	
	BYTE	old_action;	

	BYTE	step;	
	BYTE	bak_step;	
	int		step_count;
	int		total_count;
	int		start_count;

	short	event_flag;
	short	event_act;
	short	event_act_loop;
	int		event_count;
	int		event_sinkin;

	short	ano;	
	short	ano2;	

	BYTE	dir;	

	float	px,py;	
	float	tx[8],ty[8];	
	int		tcnt;	
	short	tcno;	
	short	mgno;
	short	ino;

	short	mov;

	short	serch_wait;	

	short	*move_tbl;	

	long	shock;	
	short	dmg;	
	long	atk_count;

	long	mp_count;	
	short	aw;	
	short	mw;	
	short	sw;	

	BYTE	*char_sinkin;
}CHAR_CONTROL;

typedef struct{
	long	money;
	int		retry;
}PLAYER_PARAM;

extern PLAYER_PARAM		PlayerParam;


enum{
	ATTR_UNKNOWN=-1,	
	ATTR_NOT=0,			
	ATTR_KA,			
	ATTR_TI,			
	ATTR_SUI,			
	ATTR_HUU,			
	ATTR_HOSI,			
	ATTR_SEI,			
	ATTR_YAMI,			
	ATTR_ELE,			
};


typedef struct{
	BYTE	name[20];	
	short	jno;	

	short	ex;		
	short	attr;	

	short	hp;		
	short	mp;		

	short	str;	
	short	vit;	
	short	dex;	
	short	spd;	
	short	wiz;	
	short	mnd;	

	short	mov;	

	BYTE	buki_type[2];
	short	buki_no[5];

}JOB_PARAM;
#define JOB_MAX	128

extern JOB_PARAM	JobParam[JOB_MAX];

#define EX_BOSS		0x0001
#define EX_BOSS2	0x0002
#define EX_HITO		0x0004
#define EX_HAKO		0x0008	

#define EX_ATR_DEF	0x0010	
#define EX_AVG_LIM	0x0020	
#define EX_KISYU	0x0040	
#define EX_SNG_DEF	0x0080	

#define EX_MGK_SPD	0x0100	
#define EX_KARI		0x0200	
#define EX_GINYU	0x0400	
#define EX_GAKUSI	0x0800	



enum{
	STATUS_EFF_NOT=0,
	STATUS_EFF_ATK,
	STATUS_EFF_MGK,
	STATUS_EFF_DEF,
	STATUS_EFF_RGS,
	STATUS_EFF_WSP,
	STATUS_EFF_MOV,

	STATUS_EFF_HIT,
	STATUS_EFF_AVD,

	STATUS_DEF_HP_0,	
	STATUS_DEF_MP_0,	
	STATUS_DEF_HANGEKI,
	STATUS_ITEM_BAI,	
	STATUS_HACK,

};

#define STATUS_EFF_MAX	5
#define MAGIC_CHAR_MAX	128


typedef struct{
	BYTE	flag;		
	BYTE	party;		
	BYTE	unit;		

	BYTE	name[20];	

	BYTE	job;	
	BYTE	job2;	
	BYTE	attr;	

	short	lv;		

	long	exp;	

	long	hp;		
	long	mp;		

	long	str;	
	long	vit;	
	long	dex;	
	long	spd;	
	long	wiz;	
	long	mnd;	

	short	mov;	

	short	buki;		
	short	yoroi;		
	short	accessary1;	
	short	accessary2;	

	short	magic[MAGIC_CHAR_MAX];
	short	skill[MAGIC_CHAR_MAX];
}CHAR_PARAM_SAVE;

extern int	Oberisuku;

typedef struct{
	BYTE	flag;		
	BYTE	party;		
	BYTE	unit;		

	BYTE	name[20];	

	BYTE	job;	
	BYTE	job2;	
	BYTE	attr;	

	short	lv;		

	long	exp;	

	long	hp;		
	long	mp;		

	long	str;	
	long	vit;	
	long	dex;	
	long	spd;	
	long	wiz;	
	long	mnd;	

	short	mov;	

	short	buki;		
	short	yoroi;		
	short	accessary1;	
	short	accessary2;	

	short	magic[MAGIC_CHAR_MAX];
	short	skill[MAGIC_CHAR_MAX];


	BYTE	did;	
	BYTE	hujimi;	

	long	php;		
	long	pmp;		

	short	eff_type[STATUS_EFF_MAX];
	short	eff_param[STATUS_EFF_MAX];
	short	eff_count[STATUS_EFF_MAX];

	short	bad_type[5];
	short	bad_param[5];


	short	no;

	void SetStatusEffct( int type, int param ){
		int	i;
		if(param<0){
			if(JobParam[job].ex&EX_BOSS) return;
			if(JobParam[job].ex&EX_GAKUSI) return;
		}

		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==type){
				break;
			}
		}
		if(i==STATUS_EFF_MAX){
			for(i=0;i<STATUS_EFF_MAX;i++){
				if(eff_type[i]==STATUS_EFF_NOT){
					break;
				}
			}
		}
		if(i==STATUS_EFF_MAX){
			for(i=STATUS_EFF_MAX-1;i>=1;i--){
				eff_type[i]  = eff_type[i-1];
				eff_param[i] = eff_param[i-1];
			}
			i=0;
		}
		eff_type[i]  = type;
		eff_param[i] = param;
		if(STATUS_HACK==type){
			eff_count[i] = 60*15;
		}else if( STATUS_DEF_HANGEKI==type){
			eff_count[i] = 60*30;
		}else{
			eff_count[i] = 60*60;
		}
	}
	void RenewStatusEffct(void){
		int	i;
		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]!=STATUS_EFF_NOT){
				eff_count[i]--;
				if(eff_count[i]<=0){
					eff_type[i]  = STATUS_EFF_NOT;
					eff_param[i] = 0;
					eff_count[i] = 0;
				}
			}
		}
	}
	int CheckStatusEffct( int check_eff ){
		int	i;
		int	sum_eff=0;
		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==check_eff){
				sum_eff += eff_param[i];
			}
		}

		if(!(JobParam[job].ex&EX_HITO) && unit==0 ){
			if( STATUS_EFF_ATK<=check_eff && check_eff<=STATUS_EFF_AVD)
				sum_eff -= 50*Oberisuku/8;
		}
		
		sum_eff += GetBardParam( no, check_eff-STATUS_EFF_ATK+MG_TYPE_ATK );
		return sum_eff;
	}
	void ResetStatusEffctEx( int dis_eff){
		int	i;
		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==dis_eff ){
				eff_type[i]  = STATUS_EFF_NOT;
				eff_param[i] = 0;
				eff_count[i] = 0;
			}
		}
		return ;
	}
	void ResetStatusEffct(void){
		int	i;
		for(i=0;i<STATUS_EFF_MAX;i++){
			eff_type[i]  = STATUS_EFF_NOT;
			eff_param[i] = 0;
			eff_count[i] = 0;
		}
	}


	void LearningMagic( int type, int mno );
	void LuUpMagic( void );

	int GetNextExp(void);
	int AddExp( int add, int tlv=TRUE );
	int AddExpAll( int tlv, int tdid );
	void SetLvUp( int flag=TRUE );

	int GetHp(void){
		return hp + ItemParam[buki].hp + ItemParam[yoroi].hp + ItemParam[accessary1].hp + ItemParam[accessary2].hp;
	}
	int GetMp(void){
		return mp + ItemParam[buki].mp + ItemParam[yoroi].mp + ItemParam[accessary1].mp + ItemParam[accessary2].mp;
	}

	int GetStr(void){
		return str + ItemParam[buki].str + ItemParam[yoroi].str + ItemParam[accessary1].str + ItemParam[accessary2].str;
	}
	int GetVit(void){
		return vit + ItemParam[buki].vit + ItemParam[yoroi].vit + ItemParam[accessary1].vit + ItemParam[accessary2].vit;
	}
	int GetDex(void){
		return dex + ItemParam[buki].dex + ItemParam[yoroi].dex + ItemParam[accessary1].dex + ItemParam[accessary2].dex;
	}
	int GetSpd(void){
		return spd + ItemParam[buki].spd + ItemParam[yoroi].spd + ItemParam[accessary1].spd + ItemParam[accessary2].spd;
	}
	int GetWiz(void){
		return wiz + ItemParam[buki].wiz + ItemParam[yoroi].wiz + ItemParam[accessary1].wiz + ItemParam[accessary2].wiz;
	}
	int GetMnd(void){
		return mnd + ItemParam[buki].mnd + ItemParam[yoroi].mnd + ItemParam[accessary1].mnd + ItemParam[accessary2].mnd;
	}



	
		
		
		
		
	
		
	int GetAtk(void){
		int	i,atk=0, eff=0;
		switch(ItemParam[buki].type){
			default:
			case ITEM_BUKI_SWORD:	
				atk = (int)(GetStr()*2 + GetDex() + ItemParam[buki].atk + ItemParam[yoroi].atk + ItemParam[accessary1].atk + ItemParam[accessary2].atk);
				break;
			case ITEM_BUKI_WAND:	
			case ITEM_BUKI_SPEAR:	
				atk = (int)(GetStr()*1.6 + GetDex()*1.6 + ItemParam[buki].atk + ItemParam[yoroi].atk + ItemParam[accessary1].atk + ItemParam[accessary2].atk);
				break;
			case ITEM_BUKI_HAMMER:	
				atk = (int)(GetStr()*2.8 + ItemParam[buki].atk + ItemParam[yoroi].atk + ItemParam[accessary1].atk + ItemParam[accessary2].atk);
				break;
			case ITEM_BUKI_BOW:		
				atk = (int)(GetStr() + GetDex()*2 + ItemParam[buki].atk + ItemParam[yoroi].atk + ItemParam[accessary1].atk + ItemParam[accessary2].atk);
				break;
		}
		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==STATUS_EFF_ATK){
				eff+=eff_param[i];
			}
		}
		if(!(JobParam[job].ex&EX_HITO) && unit==0 ){
			eff -= 50*Oberisuku/8;
		}
		atk = atk * (100+eff) / 100;
		atk += GetBardParam( no, MG_TYPE_ATK );
		atk = LIM(atk,1,99999);
		return atk;
	}
	
	int GetDef(void){
		int	i,def, eff=0;
		def = GetVit()*2 + GetSpd() + ItemParam[buki].def + ItemParam[yoroi].def + ItemParam[accessary1].def + ItemParam[accessary2].def;

		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==STATUS_EFF_DEF){
				eff+=eff_param[i];
			}
		}
		if(!(JobParam[job].ex&EX_HITO) && unit==0 ){
			eff -= 50*Oberisuku/8;
		}
		def = def * (100+eff) / 100;
		def += GetBardParam( no, MG_TYPE_DEF );
		def = LIM(def,1,99999);
		return def;
	}
	
		
		
	int GetMgk(void){
		int	i,mgk, eff=0;
		mgk = GetWiz()*2 + GetMnd();
		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==STATUS_EFF_MGK){
				eff+=eff_param[i];
			}
		}
		if(!(JobParam[job].ex&EX_HITO) && unit==0 ){
			eff -= 50*Oberisuku/8;
		}
		mgk = mgk * (100+eff) / 100;
		mgk += GetBardParam( no, MG_TYPE_MGK );
		mgk = LIM(mgk,1,99999);
		
		return mgk;
	}
	
	int GetRgs(void){
		int	i,rgs, eff=0;
		rgs = GetMnd()*2 + GetWiz();
		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==STATUS_EFF_RGS){
				eff+=eff_param[i];
			}
		}
		if(!(JobParam[job].ex&EX_HITO) && unit==0 ){
			eff -= 50*Oberisuku/8;
		}
		rgs = rgs * (100+eff) / 100;
		rgs += GetBardParam( no, MG_TYPE_RGS );
		rgs = LIM(rgs,1,99999);
		return rgs;
	}
	
	int GetHit(void){
		int	hit = GetDex()*2 + GetSpd(), i, eff=0;
		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==STATUS_EFF_HIT){
				eff+=eff_param[i];
			}
		}
		if(!(JobParam[job].ex&EX_HITO) && unit==0 ){
			eff -= 50*Oberisuku/8;
		}
		hit = hit * (100+eff) / 100;

		hit = LIM(hit,1,99999);
		return hit;
	}
	
	int GetAvd(void){
		int	avd = GetSpd()*2 + GetDex(), i, eff=0;
		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==STATUS_EFF_AVD){
				eff+=eff_param[i];
			}
		}
		if(!(JobParam[job].ex&EX_HITO) && unit==0 ){
			eff -= 50*Oberisuku/8;
		}
		avd = avd * (100+eff) / 100;

		avd = LIM(avd,1,99999);
		return avd;
	}
	
	int GetRst(void){
		return GetMnd()*2 + GetWiz();
	}
	
	int GetSng(void){

		return GetWiz() + GetMnd() + GetDex();
	}
	
	int GetMpr(void){
		return GetMnd()*3+500;
	}

	
	int GetMov(void){
		int	i,mov2, eff=0;
		mov2 = mov + ItemParam[buki].mov + ItemParam[yoroi].mov + ItemParam[accessary1].mov + ItemParam[accessary2].mov;

		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==STATUS_EFF_MOV){
				eff+=eff_param[i];
			}
		}
		if(!(JobParam[job].ex&EX_HITO) && unit==0 ){
			eff -= 50*Oberisuku/8;
		}
		mov2 = mov2 * (100+eff) / 100;
		mov2 += GetBardParam( no, MG_TYPE_MOV );
		mov2 = LIM(mov2, 50, 1000 );
		return mov2;
	}

	
	int GetWsp(void){
		int	i,wsp, eff=0;

		for(i=0;i<STATUS_EFF_MAX;i++){
			if(eff_type[i]==STATUS_EFF_WSP){
				eff+=eff_param[i];
			}
		}
		if(!(JobParam[job].ex&EX_HITO) && unit==0 ){
			eff -= 50*Oberisuku/8;
		}
		wsp	= ItemParam[buki].wsp + ItemParam[yoroi].wsp+ItemParam[accessary1].wsp+ItemParam[accessary2].wsp - (GetStr()*2 + GetDex() + GetSpd());
		wsp = LIM(wsp,ItemParam[buki].wsp_max, ItemParam[buki].wsp+ItemParam[yoroi].wsp+ItemParam[accessary1].wsp+ItemParam[accessary2].wsp );

		wsp -= (GetStr() + GetDex() + GetSpd()) * eff / 100;
		wsp -= GetBardParam( no, MG_TYPE_WSP );

		wsp = max(wsp,ItemParam[buki].wsp_max/2);
		return wsp;
	}

	
	int GetRange(void){
		int	rrr=ItemParam[buki].range;
		if(ItemParam[buki].range==256) rrr=288;
		return rrr;
	}

	int AddHp( int add ){
		int	_hp = GetHp();
		if(hujimi) return php;
		php = LIM(php+add,0,_hp);
		return php;
	}
	int AddMp( int add ){
		int	_mp = GetMp();
		pmp = LIM(pmp+add,0,_mp);
		return pmp;
	}


	
	int GetJobFileNo(void){
		return JobParam[job].jno;
	}
	
	int GetBukiFileNo(void){
		if(buki==0){
			return 99;
		}else{
			return JobParam[job].buki_no[ItemParam[buki].type];
		}
	}
}CHAR_PARAM;
#define CHAR_PARAM_MAX	64
extern CHAR_PARAM	CharParam[CHAR_PARAM_MAX];
extern CHAR_PARAM	InitCharParam[10];

typedef struct{
	short	hp;		
	short	mp;		

	short	str;	
	short	vit;	
	short	dex;	
	short	spd;	
	short	wiz;	
	short	mnd;	
}JOB_PARAM2;


extern JOB_PARAM2	JobParam2[32+1];







typedef struct{
}ENEMY_PARAM;


extern char *CHR_GetCharName( int sex );

#define CHAR_CONTROL_MAX	32
extern CHAR_CONTROL	CharControl[CHAR_CONTROL_MAX];


extern int		CharMarkerSelect[CHAR_CONTROL_MAX];	
extern int		CharMarkerAtack[CHAR_CONTROL_MAX];	

extern int		CharMarkerPlayer;	
extern int		CharMarkerEnemy;	
extern int		CharMarkerBoss;	

extern void SetCharDir( int i, double radian );

extern void NewYouhei( void );		
extern int NokoriYouhei( void );	

extern void ChangeCharJob32( int cno, int job_no );
extern void ChangeCharJob( int cno );

extern void ResetCharParam( int cno );
extern void SetCharParam( int cno, int job_no, int lv, int buki, int unit=1, char *name=NULL );
extern void SetCharParamName( int cno, char *name );

extern void TestSetParam( int lv );

extern void AllPartyHeal( void );

extern int GetPartyLevel( int cpno );
extern void SetPartyChar( int cpno, int disp, int lv,int equip=-1 );
extern int Change_Pno2Cno( int cpno );
extern int Change_Jno2Pno( int jno );

extern int	SetCharEquip( int cno, int eq_type, int ino );
extern int	CharEquipCheck( int cno, int ino, int eq_type );
extern int	NotSellCheck( int ino );


extern int CheckBackUpMapCharPlayer( int cno );
extern void SetBackUpMapCharPlayer( int pno, int cpno, int px, int py, int dir );
extern void SetBackUpMapChar( int no, int cno, int job, int buki, int px, int py, int dir, int lv, int attr, int unit );
extern void SetBackUpMapCharName( int no,int cno, char *str );
extern void SetBackUpMapCharEngun( int no, int etime, int loop, int inter );

extern int GetBackUpMapCharItem( int no );
extern int GetBackUpMapCharRank( int no );
extern void SetBackUpMapCharItem( int no, int itype, int ino );

extern void SetBackUpMapCharTnink( int no, int mov_th, int atk_th, int tx, int ty, int range );
extern void ReloadBackUpMapChar( void );
extern void ResetBackUpMapChar( void );


extern void ResetMapChar( int no );
extern void ResetMapCharAll( void );
extern void SetMapCharSpriteModeAll( int mode );
extern void SetMapChar( int no, int cno, int px, int py, int dir );

extern void SetMapCharDisp( int no, int disp );
extern void SetMapCharCommand( int no, int click=0 );
extern void CanselMapCharCommand( int no );


extern void SetMapCharSkill( int no, int tno, float tx, float ty );
extern void SetMapCharEvent( int no, int dir, int act, int loop );
extern void ResetMapCharEvent( int no );
extern BOOL WaitMapCharEvent( int no );

extern int SetMapCharTartgetMoveCheck( int no, int tx, int ty );
extern int SetMapCharTartgetMove( int no, int tx, int ty, int sinkin=0, int dir=-1, int mov=-1, int re_mov=0 );
extern void SetMapCharTartgetSuport( int no, int range );

extern int CheckMapCharAtack( int no, int tcno );
extern void SetMapCharTartgetAtack( int no, int tcno );
extern void SetMapCharTartgetMagic( int no, int mgno, int tcno, int tx, int ty, int amend=FALSE );
extern void SetMapCharTartgetItem( int no, int ino, int tcno );

extern void SetMapCharStand( int no );

extern BOOL WaitMapCharTartget( int no );
extern BOOL WaitMapCharTartget2( int no );

extern void RenewCharSinkin(void);

extern double AttrRate( int attr1, int attr2 );

extern void SetArrowAction( int no, int tno, int waza=0 );
extern int AtackNormal( int no, int tno, int sample, int waza=0 );

extern int	GetCharCommandWait(void);

extern int RenewCharAction( int char_active );

enum{
	HIT_DAMEGE=0,
	HIT_HEAL,
	HIT_MISS,
	HIT_POWUP,
	HIT_POWDOWN,
	HIT_DISPEL,
	HIT_LVUP,
	HIT_STN,
};
extern void SetHitAction( int tno, int damege, float radian, int type=0 );
extern int AtackNormalDmg( int no, int tno, float radian, int waza=0 );

extern int WaitBattleEffect( void );


#endif

