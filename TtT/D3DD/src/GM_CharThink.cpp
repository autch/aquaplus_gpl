#include <windows.h>
#include <mouse.h>

#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "comp_pac.h"
#include "keybord.h"

#include "main.h"

#include "GlobalDraw.h"

#include "GM_Main.h"
#include "GM_Battle.h"
#include "GM_CharThink.h"
#include <math.h>
#include "GM_Magic.h"
#include "escript.h"

CHAR_THINK_PARAM	CharThink[CHAR_PARAM_MAX];


void SetMapCharThinkTouhatu( int no, int tno )
{
	if(CharControl[no].flag){
		if(CharParam[CharControl[no].cno].did) return ;
		if(CharParam[CharControl[no].cno].unit==CharParam[CharControl[tno].cno].unit ) return ;

		if( CharThink[CharControl[no].cno].flag ){
			CharThink[CharControl[no].cno].heito_tno = tno;
			CharThink[CharControl[no].cno].heito_cnt = 0;

			switch(CharThink[CharControl[no].cno].move){
			case MOVE_THINK_SEARCH:		
			case MOVE_THINK_DEFENSE:	
				CharThink[CharControl[no].cno].move = MOVE_THINK_ATACK;
			}
		}
	}
}
void AllAtackMapCharThink( void )
{
	int	i;
	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if( !CharControl[i].flag ) continue;
		if( CharParam[CharControl[i].cno].unit!=1 ) continue;
		if( CharParam[CharControl[i].cno].did ) continue;
		if( CharThink[CharControl[i].cno].flag==0 ) continue;

		switch(CharThink[CharControl[i].cno].move){
			case MOVE_THINK_SEARCH:		
			case MOVE_THINK_DEFENSE:	
				CharThink[CharControl[i].cno].move = MOVE_THINK_ATACK;
				break;
		}
	}
	
}

void SetMapCharThink( int no, int move_think, int action_think, int tx, int ty, int range )
{
	if(CharControl[no].flag){
		if( CharParam[CharControl[no].cno].accessary2==235 || CharParam[CharControl[no].cno].accessary2==228 ){
			move_think   = MOVE_THINK_ATACK;
			action_think = ACTION_THINK_KNIGHT;
		}

		if(!CharParam[CharControl[no].cno].did){
			CharControl[no].action = CHAR_ACTION_STAND;
		}
		
		CharThink[CharControl[no].cno].flag   = TRUE;
		CharThink[CharControl[no].cno].move   = move_think;
		CharThink[CharControl[no].cno].action = action_think;

		CharThink[CharControl[no].cno].tx  = tx;
		CharThink[CharControl[no].cno].ty  = ty;
		CharThink[CharControl[no].cno].tno = 0;

		CharThink[CharControl[no].cno].count = 0;
		if(range==-1){
			CharThink[CharControl[no].cno].range = 512;
		}else{
			CharThink[CharControl[no].cno].range = range;
		}

		CharThink[CharControl[no].cno].heito_tno = -1;
		CharThink[CharControl[no].cno].heito_cnt = 0;

		CharThink[CharControl[no].cno].telepo = 0;
		CharThink[CharControl[no].cno].kipu = 0;
	}else{
	}
}
void ResetMapCharThink( int no )
{
	if( CharParam[CharControl[no].cno].accessary2==235 || CharParam[CharControl[no].cno].accessary2==228 ){
		return ;
	}
	CharThink[CharControl[no].cno].flag   = FALSE;
	CharThink[CharControl[no].cno].kipu = 0;
}

void SetKipuMapCharThink( int no )
{
	if(CharThink[CharControl[no].cno].flag){
		CharThink[CharControl[no].cno].flag   = FALSE;
		CharThink[CharControl[no].cno].kipu = 1;
	}
}
void LoadKipuMapCharThink( int no )
{
	if(CharThink[CharControl[no].cno].kipu){
		CharThink[CharControl[no].cno].flag   = TRUE;
		CharThink[CharControl[no].cno].kipu = 0;
	}
}



BOOL MapCharThinkMagicAtack( int i, int mno, int &rate_bak, int &magic_no, int &tno, int &tx, int &ty )
{
	int	k;
	int	x,y;

	int		rate=0;
	float	px = CharControl[i].px;
	float	py = CharControl[i].py;
	int		cno = CharControl[i].cno;
	
	float	range = GetMagicRange( mno, cno );
	float	scope = GetMagicScope( mno, cno );

	if(MagicParam[mno].scope_type==RANGE_TAGET){
		for( k=0 ; k<CHAR_CONTROL_MAX ; k++ ){
			if(CharControl[k].flag && !CharParam[CharControl[k].cno].did){
				rate=0;
				if( pow(px-CharControl[k].px,2)+pow(py-CharControl[k].py,2) < pow(range,2) ){
					if(CharParam[CharControl[i].cno].unit==CharParam[CharControl[k].cno].unit){
						rate=0;
					}else{
						rate=MagicDmageNormal( i, k, mno );
						if(MagicParam[mno].param_type[0]==MG_TYPE_DAMAGE_MP){
							rate = min( CharParam[CharControl[k].cno].pmp, rate );
						}
					}
					if(rate_bak<rate){
						rate_bak = rate;
						magic_no = mno;
						tno = k;
						tx = 0;
						ty = 0;
					}
				}
			}
		}
	}else if( MagicParam[mno].scope_type==RANGE_FAN ){
		int	dir = CharControl[i].dir;

		rate=0;
		for( k=0 ; k<CHAR_CONTROL_MAX ; k++ ){
			if(CharControl[k].flag && !CharParam[CharControl[k].cno].did){
				if( pow(px-CharControl[k].px,2)+pow(py-CharControl[k].py,2) < pow(scope,2) ){

					if( (dir == (int)(((atan2( py-CharControl[k].py, px-CharControl[k].px )/3.141592+1.0)*8+1)/2 + 6)%8)
					 || (dir == (int)(((atan2( py-CharControl[k].py, px-CharControl[k].px )/3.141592+1.0)*8+2)/2 + 6)%8) ){
						if(CharParam[CharControl[i].cno].unit==CharParam[CharControl[k].cno].unit){

						}else{
							rate+=MagicDmageNormal( i, k, mno );
						}
					}
				}
			}
		}
		if(rate_bak<rate){
			rate_bak = rate;
			magic_no = mno;
			tno = -1;
			tx = px;
			ty = py;
		}

	}else{
		int	w = MapData.head.w;
		int	h = MapData.head.h;

		for( y=py-range ; y<py+range ; y+=64 ){
			float	rx = sqrt( pow(range,2) - pow(y-py,2) );

			for( x=px-rx ; x < px+rx ; x+=64 ){
				rate=0;
				for( k=0 ; k<CHAR_CONTROL_MAX ; k++ ){
					if(CharControl[k].flag && !CharParam[CharControl[k].cno].did){
						if( pow(x-CharControl[k].px,2)+pow(y-CharControl[k].py,2) < pow(scope,2) ){
							if(CharParam[CharControl[i].cno].unit==CharParam[CharControl[k].cno].unit){
								rate-=MagicDmageNormal( i, k, mno )*4;
							}else{
								rate+=MagicDmageNormal( i, k, mno );
							}
						}
					}
				}
				if(rate_bak<rate){
					rate_bak = rate;
					magic_no = mno;
					tno = -1;
					tx = x;
					ty = y;
				}
			}
		}
	}
	return magic_no!=-1;
}

int RenewMapCharThinkMagicAtack( int i, int setuyaku=0 )
{
	int	j, mno;

	int		magic_no=-1;
	int		tno=0, tx=0, ty=0;
	int		cno = CharControl[i].cno;
	int		rate_bak=0;

	if( CharControl[i].action==CHAR_ACTION_MAGIC ){
		if( CharParam[cno].pmp<MagicParam[CharControl[i].mgno].mp ){
			SetMapCharStand( i );
			return -1;
		}
		if(CharControl[i].mw%30==0){
			if(MagicParam[CharControl[i].mgno].param_type[0]==MG_TYPE_DAMAGE || MagicParam[CharControl[i].mgno].param_type[0]==MG_TYPE_DAMAGE_MP){
				if( MapCharThinkMagicAtack( i, CharControl[i].mgno, rate_bak, magic_no, tno, tx, ty ) ){
					SetMapCharTartgetMagic( i, magic_no, tno, tx, ty, TRUE );
				}else{
					SetMapCharStand( i );
				}
			}else{
				SetMapCharStand( i );
			}
		}else{
			return CharControl[i].mgno;
		}
	}else{
		if( CharThink[CharControl[i].cno].count<=0 ){
			int	r=1;
			if(CharParam[cno].magic[4]==-1){
				r=1;
			}else{
				r=4;
			}
			if(JobParam[CharParam[cno].job].ex&EX_BOSS2 && CharThink[cno].action==ACTION_THINK_MAGIC){
				for( j=0 ; j<MAGIC_CHAR_MAX ; j++ ){
					if(CharParam[cno].magic[j]==-1){
						break;
					}
				}
				if(j)	r=rand()%j;
				else	return -1;
				mno = CharParam[cno].magic[r];
				if(mno==-1)	return -1;
				if( CharParam[cno].pmp>=MagicParam[mno].mp ){
					if(MagicParam[mno].param_type[0]==MG_TYPE_DAMAGE || MagicParam[mno].param_type[0]==MG_TYPE_DAMAGE_MP){
						MapCharThinkMagicAtack( i, mno, rate_bak, magic_no, tno, tx, ty );
					}
				}
			}else{
				for( j=rand()%r ; j<MAGIC_CHAR_MAX ; j+=r ){
					mno = CharParam[cno].magic[j];
					if(mno==-1)	break;
					if( CharParam[cno].pmp>=MagicParam[mno].mp ){
						if( (setuyaku && MagicParam[mno].mp<30) || !setuyaku ){
							if(MagicParam[mno].param_type[0]==MG_TYPE_DAMAGE || MagicParam[mno].param_type[0]==MG_TYPE_DAMAGE_MP){
								MapCharThinkMagicAtack( i, mno, rate_bak, magic_no, tno, tx, ty );
							}
						}
					}
				}
			}
			if(magic_no!=-1){
				SetMapCharTartgetMagic( i, magic_no, tno, tx, ty );
			}else{
				CharThink[CharControl[i].cno].count = 30;
			}
		}else{
			CharThink[CharControl[i].cno].count--;
		}
	}
	return magic_no;
}

BOOL MapCharThinkMagicHeal( int i, int mno, int &rate_bak, int &magic_no, int &tno, int &tx, int &ty, int reload )
{
	int	k;
	int	x,y;

	int		rate=0;
	float	px = CharControl[i].px;
	float	py = CharControl[i].py;
	int		cno = CharControl[i].cno;

	float	range = GetMagicRange( mno, cno );
	float	scope = GetMagicScope( mno, cno );

	if(MagicParam[mno].scope_type==RANGE_TAGET){
		for( k=0 ; k<CHAR_CONTROL_MAX ; k++ ){
			if(CharControl[k].flag && !CharParam[CharControl[k].cno].did){
				rate=0;
				if( pow(px-CharControl[k].px,2)+pow(py-CharControl[k].py,2) < pow(range,2) || (reload && tno==k) ){
					if(CharParam[CharControl[i].cno].unit==CharParam[CharControl[k].cno].unit){
						rate=MagicRestNormal( i, k, mno );
						if( rate < (MagicParam[mno].param[0] + MagicParam[mno].param[0] * CharParam[cno].GetRst()/100 )/2
						 && CharParam[CharControl[k].cno].php > CharParam[CharControl[k].cno].GetHp()*3/4 ){
							rate=0;
						}else{
							rate += 100*(CharParam[CharControl[k].cno].GetHp()-CharParam[CharControl[k].cno].php) / CharParam[CharControl[k].cno].GetHp();
						}
					}else{
						rate=0;
					}
					rate -= MagicParam[ mno ].mp;

					if(rate_bak<rate){
						rate_bak = rate;
						magic_no = mno;
						tno = k;
						tx = 0;
						ty = 0;
					}
				}
			}
		}
	}else{
		int	rest_go=0;
		for( y=py-range ; y<py+range ; y+=64 ){
			float	rx = sqrt( pow(range,2) - pow(y-py,2) );
			for( x=px-rx ; x < px+rx ; x+=64 ){
				rest_go=0;
				rate=0;
				for( k=0 ; k<CHAR_CONTROL_MAX ; k++ ){
					if(CharControl[k].flag && !CharParam[CharControl[k].cno].did){
						if( pow(x-CharControl[k].px,2)+pow(y-CharControl[k].py,2) < pow(scope,2) ){
							if(CharParam[CharControl[i].cno].unit==CharParam[CharControl[k].cno].unit){
								if(CharParam[CharControl[k].cno].php <= CharParam[CharControl[k].cno].GetHp()*3/4 ){
									rest_go=1;
								}
								rate+=MagicRestNormal( i, k, mno );
							}else{
								rate-=MagicRestNormal( i, k, mno )*2;
							}
						}
					}
				}
				if( rate < (MagicParam[mno].param[0] + MagicParam[mno].param[0] * CharParam[cno].GetRst()/100) /2 && rest_go==0 ){
					rate=0;
				}else{
					rate -= MagicParam[ mno ].mp;
				}

				if(rate_bak<rate){
					rate_bak = rate;
					magic_no = mno;
					tno = -1;
					tx = x;
					ty = y;
				}
			}
		}
	}
	return magic_no!=-1;
}


int RenewMapCharThinkMagicHeal( int i )
{
	int		j;
	int		magic_no=-1, mno;
	int		tno=0, tx=0, ty=0;
	int		cno = CharControl[i].cno;
	int		rate_bak=0;

	if( CharControl[i].action==CHAR_ACTION_MAGIC ){
		if( CharParam[cno].pmp<MagicParam[CharControl[i].mgno].mp ){
			SetMapCharStand( i );
			return -1;
		}
		if(CharControl[i].mw%30==0){
			if(MagicParam[CharControl[i].mgno].param_type[0]==MG_TYPE_HEAL){
				if( MapCharThinkMagicHeal( i, CharControl[i].mgno, rate_bak, magic_no, tno, tx, ty, TRUE ) ){
					SetMapCharTartgetMagic( i, magic_no, tno, tx, ty, TRUE );
				}else{
					SetMapCharStand( i );
				}
			}else{
				SetMapCharStand( i );
			}
		}else{
			return CharControl[i].mgno;
		}
	}else{
		if( CharThink[CharControl[i].cno].count<=0 ){
			for(j=0;j<MAGIC_CHAR_MAX;j++){
				mno = CharParam[cno].magic[j];
				if(mno==-1)	break;

				if( CharParam[cno].pmp>=MagicParam[ mno ].mp ){
					if(MagicParam[mno].param_type[0]==MG_TYPE_HEAL){
						MapCharThinkMagicHeal( i, mno, rate_bak, magic_no, tno, tx, ty, FALSE );
					}
				}
			}
			if(magic_no!=-1){
				SetMapCharTartgetMagic( i, magic_no, tno, tx, ty );
			}else{
				CharThink[CharControl[i].cno].count = 30;
			}
		}else{
			CharThink[CharControl[i].cno].count--;
		}
	}
	return magic_no;
}


BOOL MapCharThinkMagicTelepo( int i, int mno, int &rate_bak, int &magic_no, int &tno, int &tx, int &ty, int reload )
{
	int	k;
	int	x,y;

	int		rate=0;
	float	px = CharControl[i].px;
	float	py = CharControl[i].py;
	int		cno = CharControl[i].cno;

	float	range = GetMagicRange( mno, cno );
	float	scope = GetMagicScope( mno, cno );

	for( k=0 ; k<CHAR_CONTROL_MAX ; k++ ){
		if(CharControl[k].flag && !CharParam[CharControl[k].cno].did){
			rate=0;
			if( pow(px-CharControl[k].px,2)+pow(py-CharControl[k].py,2) < pow(range,2) || (reload && tno==k) ){
				if(CharParam[CharControl[i].cno].unit==CharParam[CharControl[k].cno].unit){
					rate=0;
				}else{
					if(ESC_GetFlag(3)>3){
						rate=99999-CharParam[CharControl[k].cno].GetDef();
					}else{
						rate=CharParam[CharControl[k].cno].GetDef();
					}
				}

				if(rate_bak<rate){
					rate_bak = rate;
					magic_no = mno;
					tno = k;
					tx = 0;
					ty = 0;
				}
			}
		}
	}
	return magic_no!=-1;
}

int RenewMapCharThinkMagicTelepo( int i )
{
	int		j;
	int		magic_no=-1, mno;
	int		tno=0, tx=0, ty=0;
	int		cno = CharControl[i].cno;
	int		rate_bak=0;

	if( CharControl[i].action==CHAR_ACTION_MAGIC ){
		if( CharParam[cno].pmp<MagicParam[CharControl[i].mgno].mp ){
			SetMapCharStand( i );
			return -1;
		}
		if(CharControl[i].mw%30==0){
			if(CharControl[i].mgno==GetMagicNo( MAGIC_ZOKUSEI, 51 ) ){
				if( MapCharThinkMagicTelepo( i, CharControl[i].mgno, rate_bak, magic_no, tno, tx, ty, TRUE ) ){
					SetMapCharTartgetMagic( i, magic_no, tno, tx, ty, TRUE );
				}else{
					SetMapCharStand( i );
				}
			}else{
				SetMapCharStand( i );
			}
		}else{
			return CharControl[i].mgno;
		}
	}else{
		for(j=0;j<MAGIC_CHAR_MAX;j++){
			mno = CharParam[cno].magic[j];
			if(mno==-1)	break;
			if(mno!=GetMagicNo( MAGIC_ZOKUSEI, 51 ) )	continue;

			if( CharParam[cno].pmp>=MagicParam[ mno ].mp ){
				if(CharThink[CharControl[i].cno].count>600){
					MapCharThinkMagicTelepo( i, mno, rate_bak, magic_no, tno, tx, ty, FALSE );
				}else{
					CharThink[CharControl[i].cno].count++;
				}
			}
		}
		if(magic_no!=-1){
			SetMapCharTartgetMagic( i, magic_no, tno, tx, ty );
		}
	}
	return magic_no;

}

void RenewMapCharThink( void )
{
	int	i,j,k, mno;
	int	x,y;

	i = ESC_GetFlag(3);
	if(i==0){
		ESC_SetFlag(3,3);
	}


	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if( CharControl[i].step==CHAR_STEP_MAGIC_WAIT || CharControl[i].step==CHAR_STEP_TECHNIC ){
			return ;
		}
	}


	for(i=0;i<CHAR_CONTROL_MAX;i++){
		int		rate_bak=0,magic_no=-1;
		int		rate=0;
		int		tno=0, tx=0, ty=0;
		float	px = CharControl[i].px;
		float	py = CharControl[i].py;
		int		cno = CharControl[i].cno;

		if( !CharControl[i].flag ) continue;
		if( !CharParam[CharControl[i].cno].flag ) continue;
		if( !CharThink[CharControl[i].cno].flag ) continue;

		if( CharParam[CharControl[i].cno].did ) continue;

		if( CharControl[i].action==CHAR_ACTION_COMMAND ) continue;

		if(CharControl[i].dmg){
			if( CharThink[CharControl[i].cno].move==MOVE_THINK_SEARCH ){
				CharThink[CharControl[i].cno].move=MOVE_THINK_ATACK;
			}
		}
		CharThink[CharControl[i].cno].heito_cnt++;

		if( CharControl[i].action==CHAR_ACTION_TECHNIC ) continue;

		int	kt_interval=60*10;
	if( CharControl[i].step!=CHAR_STEP_ATACK2){
		switch(CharThink[CharControl[i].cno].action){
		case ACTION_THINK_SHOT_TEL:
			if( CharControl[i].step!=CHAR_STEP_MAGIC_END ){
				RenewMapCharThinkMagicTelepo(i);
			}else{
				if( ESC_GetFlag(3)<3 ){
					CharThink[CharControl[i].cno].action=ACTION_THINK_ATACK;
				}else{
					CharThink[CharControl[i].cno].action=ACTION_THINK_SHOT;
				}
			}
			break;
		case ACTION_THINK_ATACK:	
			break;
		case ACTION_THINK_SHOT:		
			if( ESC_GetFlag(3)<3 ){
				CharThink[CharControl[i].cno].action=ACTION_THINK_ATACK;
			}
			break;
		case ACTION_THINK_KNIGHT:	
			
			if(CharParam[CharControl[i].cno].job==29){
				kt_interval=60*15;
			}
			switch( CharThink[CharControl[i].cno].mode ){
			case 0:
				CharThink[CharControl[i].cno].mode_cnt++;
				if( CharThink[CharControl[i].cno].mode_cnt>kt_interval ){
					CharThink[CharControl[i].cno].mode=1;
				}
				break;
			case 1:
				j = RenewMapCharThinkMagicHeal(i);
				if( j==-1 ){
					j = RenewMapCharThinkMagicAtack(i);
					if( !(j==-1) ){
						CharThink[CharControl[i].cno].mode_cnt=0;
						CharThink[CharControl[i].cno].mode=3;
					}
				}else{
					CharThink[CharControl[i].cno].mode_cnt=0;
					CharThink[CharControl[i].cno].mode=2;
				}
				break;
			case 2:
				if( CharControl[i].step!=CHAR_STEP_MAGIC_END ){
					RenewMapCharThinkMagicHeal(i);
				}else{
					CharThink[CharControl[i].cno].mode_cnt=0;
					CharThink[CharControl[i].cno].mode=0;
				}
				break;
			case 3:
				if( CharControl[i].step!=CHAR_STEP_MAGIC_END && CharControl[i].step!=CHAR_STEP_TECHNIC_WAIT ){
					RenewMapCharThinkMagicAtack(i);
				}else{
					CharThink[CharControl[i].cno].mode_cnt=0;
					CharThink[CharControl[i].cno].mode=0;
				}
				break;
			}
			break;
		case ACTION_THINK_MAGIC:	
			RenewMapCharThinkMagicAtack(i);
			break;
		case ACTION_THINK_HEAL:		
			RenewMapCharThinkMagicHeal(i);
			break;
		case ACTION_THINK_HEAL_MAGIC:	
			

			switch( CharThink[CharControl[i].cno].mode ){
			case 0:
				CharThink[CharControl[i].cno].mode=1;
				break;
			case 1:
				j = RenewMapCharThinkMagicHeal(i);
				if( j==-1 ){
					j = RenewMapCharThinkMagicAtack(i,TRUE);
					if( !(j==-1) ){
						CharThink[CharControl[i].cno].mode_cnt=0;
						CharThink[CharControl[i].cno].mode=3;
					}
				}else{
					CharThink[CharControl[i].cno].mode_cnt=0;
					CharThink[CharControl[i].cno].mode=2;
				}
				break;
			case 2:
				if( CharControl[i].step!=CHAR_STEP_MAGIC_END ){
					RenewMapCharThinkMagicHeal(i);
				}else{
					CharThink[CharControl[i].cno].mode_cnt=0;
					CharThink[CharControl[i].cno].mode=0;
				}
				break;
			case 3:
				if( CharControl[i].step!=CHAR_STEP_MAGIC_END ){
					RenewMapCharThinkMagicAtack(i,TRUE);
				}else{
					CharThink[CharControl[i].cno].mode_cnt=0;
					CharThink[CharControl[i].cno].mode=0;
				}
				break;
			}
			break;
		}
	}
		float	range = 4096.0f;
		int		atack_no=-1;
		float	target_range;

		if(CharParam[CharControl[i].cno].job==87) continue; 
		if(CharParam[CharControl[i].cno].job==88) continue;	

		if( CharControl[i].action==CHAR_ACTION_MAGIC ) continue;
		if( CharControl[i].action==CHAR_ACTION_TECHNIC ) return ;


		int	self_unit;
		if( CharParam[CharControl[i].cno].CheckStatusEffct(STATUS_HACK) ){
			self_unit = !CharParam[CharControl[i].cno].unit;
		}else{
			self_unit = CharParam[CharControl[i].cno].unit;
		}


		if(CharThink[CharControl[i].cno].move==MOVE_THINK_SURPRISE){
			if( ESC_GetFlag(3)<3 ){
				CharThink[CharControl[i].cno].move=MOVE_THINK_ATACK;
			}
		}

		switch(CharThink[CharControl[i].cno].move){
		case MOVE_THINK_SURPRISE:
			if( GlobalCount%max(1,CharControl[i].serch_wait)==0 ){

				if(CharThink[CharControl[i].cno].heito_tno!=-1){
					int	heito_tno = CharThink[CharControl[i].cno].heito_tno;

					if( CharParam[CharControl[heito_tno].cno].did ){
						CharThink[CharControl[i].cno].heito_tno = -1;
						CharThink[CharControl[i].cno].heito_cnt = 0;
					}
					if( !CheckMapCharAtack( i, heito_tno ) ) continue;

					target_range = (float)sqrt( pow(CharControl[i].px-CharControl[heito_tno].px,2)+pow(CharControl[i].py-CharControl[heito_tno].py,2) );
					if( target_range < range ){
						range = target_range;
						atack_no=heito_tno;
					}
					if(CharThink[CharControl[i].cno].heito_cnt>60*10){
						CharThink[CharControl[i].cno].heito_tno = -1;
						CharThink[CharControl[i].cno].heito_cnt = 0;
					}
				}else{
					range=0;
					for(j=0;j<CHAR_CONTROL_MAX;j++){
						if( !CharControl[j].flag ) continue;
						if( i==j ) continue;
						if( CharParam[CharControl[j].cno].unit == self_unit ) continue;
						if( CharParam[CharControl[j].cno].did ) continue;

						target_range = 1000*AtackNormal( i, j, TRUE ) / (CharParam[CharControl[j].cno].php+1);
						if( target_range > range ){
							range = target_range;
							atack_no=j;
						}
					}
				}
				if(atack_no==-1){
					SetMapCharTartgetAtack( i, -1 );
					break;
				}

				SetMapCharTartgetAtack( i, atack_no );
				target_range = (float)sqrt( pow(CharControl[i].px-CharControl[CharControl[i].tcno].px,2) + pow(CharControl[i].py-CharControl[CharControl[i].tcno].py,2) );
				CharControl[i].serch_wait=(int)target_range/16;
			}
			break;

		case MOVE_THINK_ATACK:		
			if( GlobalCount%max(1,CharControl[i].serch_wait)==0 ){

				if(CharThink[CharControl[i].cno].heito_tno!=-1){
					int	heito_tno = CharThink[CharControl[i].cno].heito_tno;

					if( CharParam[CharControl[heito_tno].cno].did ){
						CharThink[CharControl[i].cno].heito_tno = -1;
						CharThink[CharControl[i].cno].heito_cnt = 0;
					}
					if( !CheckMapCharAtack( i, heito_tno ) ) continue;

					target_range = (float)sqrt( pow(CharControl[i].px-CharControl[heito_tno].px,2)+pow(CharControl[i].py-CharControl[heito_tno].py,2) );
					if( target_range < range ){
						range = target_range;
						atack_no=heito_tno;
					}
					if(CharThink[CharControl[i].cno].heito_cnt>60*10){
						CharThink[CharControl[i].cno].heito_tno = -1;
						CharThink[CharControl[i].cno].heito_cnt = 0;
					}
				}else{
					for(j=0;j<CHAR_CONTROL_MAX;j++){
						if( !CharControl[j].flag ) continue;
						if( i==j ) continue;
						if( CharParam[CharControl[j].cno].unit == self_unit ) continue;
						if( CharParam[CharControl[j].cno].did ) continue;
						if( !CheckMapCharAtack( i, j ) ) continue;

						target_range = (float)sqrt( pow(CharControl[i].px-CharControl[j].px,2)+pow(CharControl[i].py-CharControl[j].py,2) );
						if( target_range < range ){
							range = target_range;
							atack_no=j;
						}
					}
					if(CharThink[CharControl[i].cno].action==ACTION_THINK_SHOT){
						range=0;
						for(j=0;j<CHAR_CONTROL_MAX;j++){
							if( !CharControl[j].flag ) continue;
							if( i==j ) continue;
							if( CharParam[CharControl[j].cno].unit == self_unit ) continue;
							if( CharParam[CharControl[j].cno].did ) continue;

							target_range = (float)sqrt( pow(CharControl[i].px-CharControl[j].px,2)+pow(CharControl[i].py-CharControl[j].py,2) );
							if(CharParam[CharControl[i].cno].GetRange() > target_range ){
								target_range = 1000*AtackNormal( i, j, TRUE ) / (CharParam[CharControl[j].cno].php+1);
								if( target_range > range ){
									range = target_range;
									atack_no=j;
								}
							}
						}
					}
				}
				if(atack_no==-1){
					SetMapCharTartgetAtack( i, -1 );
					break;
				}

				SetMapCharTartgetAtack( i, atack_no );
				target_range = (float)sqrt( pow(CharControl[i].px-CharControl[CharControl[i].tcno].px,2) + pow(CharControl[i].py-CharControl[CharControl[i].tcno].py,2) );
				CharControl[i].serch_wait=(int)target_range/16;
			}
			break;
		case MOVE_THINK_SEARCH:		
		case MOVE_THINK_DEFENSE:	
if(appData.debug_mouse!=-1){
	if(KeyCond.btn.s){
		GD_SetGraphCircle( GRP_MAP_MOVE+i, 64, TRUE, LAY_EFFECT+2, TRUE );

		GD_SetGraphColorNo( GRP_MAP_MOVE+i, 0,   0, 0,   0, 0 );
		GD_SetGraphColorNo( GRP_MAP_MOVE+i, 1, 64, 12, 128, 22 );

		if( CharThink[CharControl[i].cno].ty<0 ){
			float	px = (CharControl[i].px-MapControl.px)*MapControl.zoom;
			float	py = (CharControl[i].py-MapControl.py)*MapControl.zoom;
			range = CharThink[CharControl[i].cno].range;

			GD_SetGraphDstRect( GRP_MAP_MOVE+i, (float)(px - range), (float)(py - range), (float)(range*2), (float)(range*2) );
			GD_SetGraphZoom( GRP_MAP_MOVE+i, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+i, px, py );
		}else{
			float	px = (CharThink[CharControl[i].cno].tx-MapControl.px)*MapControl.zoom;
			float	py = (CharThink[CharControl[i].cno].ty-MapControl.py)*MapControl.zoom;
			range = CharThink[CharControl[i].cno].range;

			GD_SetGraphDstRect( GRP_MAP_MOVE+i, (float)(px - range), (float)(py - range), (float)(range*2), (float)(range*2) );
			GD_SetGraphZoom( GRP_MAP_MOVE+i, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+i, px, py );
		}
	}
}

			if( GlobalCount%max(1,CharControl[i].serch_wait)==0 ){
				range = 4096.0f;
				for(j=0;j<CHAR_CONTROL_MAX;j++){
					if( !CharControl[j].flag ) continue;
					if( i==j ) continue;
					if( CharParam[CharControl[j].cno].unit == self_unit ) continue;
					if( CharParam[CharControl[j].cno].did ) continue;
					if( !CheckMapCharAtack( i, j ) ) continue;

					target_range = (float)sqrt( pow(CharControl[i].px-CharControl[j].px,2)+pow(CharControl[i].py-CharControl[j].py,2) );
					if( target_range < range ){
						range = target_range;
						atack_no=j;
					}
					if( CharThink[CharControl[i].cno].ty<0 ){
					}else{
						target_range = (float)sqrt( pow(CharThink[CharControl[i].cno].tx-CharControl[j].px,2)+pow(CharThink[CharControl[i].cno].ty-CharControl[j].py,2) );
						if( target_range < range ){
							range = target_range;
							atack_no=j;
						}
					}
				}
				if(range>CharThink[CharControl[i].cno].range){
					SetMapCharTartgetAtack( i, -1 );
					break;
				}
				if(CharThink[CharControl[i].cno].action==ACTION_THINK_SHOT){
					range=0;
					for(j=0;j<CHAR_CONTROL_MAX;j++){
						if( !CharControl[j].flag ) continue;
						if( i==j ) continue;
						if( CharParam[CharControl[j].cno].unit == self_unit ) continue;
						if( CharParam[CharControl[j].cno].did ) continue;

						target_range = (float)sqrt( pow(CharControl[i].px-CharControl[j].px,2)+pow(CharControl[i].py-CharControl[j].py,2) );
						if(CharParam[CharControl[i].cno].GetRange() > target_range ){
							target_range = 1000*AtackNormal( i, j, TRUE ) / (CharParam[CharControl[j].cno].php+1);
							if( target_range > range ){
								range = target_range;
								atack_no=j;
							}
						}
					}
				}else if( CharThink[CharControl[i].cno].ty>=0 ){
					range = 4096.0f;
					for(j=0;j<CHAR_CONTROL_MAX;j++){
						if( !CharControl[j].flag ) continue;
						if( i==j ) continue;
						if( CharParam[CharControl[j].cno].unit == self_unit ) continue;
						if( CharParam[CharControl[j].cno].did ) continue;

						target_range = (float)sqrt( pow(CharControl[i].px-CharControl[j].px,2)+pow(CharControl[i].py-CharControl[j].py,2) );
						if( target_range < range ){
							range = target_range;
							atack_no=j;
						}
					}
				}
				if(atack_no==-1){
					SetMapCharTartgetAtack( i, -1 );
					break;
				}




				SetMapCharTartgetAtack( i, atack_no );
				target_range = (float)sqrt( pow(CharControl[i].px-CharControl[CharControl[i].tcno].px,2) + pow(CharControl[i].py-CharControl[CharControl[i].tcno].py,2) );
				CharControl[i].serch_wait=(int)target_range/16;
			}
			break;
		case MOVE_THINK_NO_MOVE:	
			if( GlobalCount%max(1,CharControl[i].serch_wait)==0 ){
				if(CharThink[CharControl[i].cno].action==ACTION_THINK_SHOT){
					range=0;
					for(j=0;j<CHAR_CONTROL_MAX;j++){
						if( !CharControl[j].flag ) continue;
						if( i==j ) continue;
						if( CharParam[CharControl[j].cno].unit == self_unit ) continue;
						if( CharParam[CharControl[j].cno].did ) continue;

						target_range = (float)sqrt( pow(CharControl[i].px-CharControl[j].px,2)+pow(CharControl[i].py-CharControl[j].py,2) );
						if(CharParam[CharControl[i].cno].GetRange() > target_range ){
							target_range = 1000*AtackNormal( i, j, TRUE ) / (CharParam[CharControl[j].cno].php+1);
							if( target_range > range ){
								range = target_range;
								atack_no=j;
							}
						}
					}
				}else{
					for(j=0;j<CHAR_CONTROL_MAX;j++){
						if( !CharControl[j].flag ) continue;
						if( i==j ) continue;
						if( CharParam[CharControl[j].cno].unit == self_unit ) continue;
						if( CharParam[CharControl[j].cno].did ) continue;

						target_range = (float)sqrt( pow(CharControl[i].px-CharControl[j].px,2)+pow(CharControl[i].py-CharControl[j].py,2) );
						if(CharParam[CharControl[i].cno].GetRange() > target_range ){
							if( target_range < range ){
								range = target_range;
								atack_no=j;
							}
						}
					}
				}
				if(atack_no==-1){
					SetMapCharTartgetAtack( i, -1 );
					break;
				}

				target_range = (float)sqrt( pow(CharControl[i].px-CharControl[CharControl[i].tcno].px,2) + pow(CharControl[i].py-CharControl[CharControl[i].tcno].py,2) );
				SetMapCharTartgetAtack( i, atack_no );
				CharControl[i].serch_wait=(int)target_range/16;
			}
			CharControl[cno].tcnt=0;

			break;
		case MOVE_THINK_NO_MOVE2:	
			break;
		case MOVE_THINK_SUPPORT:	

			switch(CharThink[CharControl[i].cno].action){
			case ACTION_THINK_MAGIC:	SetMapCharTartgetSuport( i, 160 );	break;
			default:
			case ACTION_THINK_HEAL:		SetMapCharTartgetSuport( i, 224 );	break;
			}
			break;
		}
	}
}
