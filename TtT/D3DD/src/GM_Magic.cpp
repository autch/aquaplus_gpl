#include <windows.h>

#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "comp_pac.h"

#include "main.h"

#include "GlobalDraw.h"

#include "GM_Main.h"
#include "GM_Battle.h"
#include "GM_Char.h"
#include "GM_Item.h"
#include "GM_Magic.h"

#include "GM_Avg.h"

#include "GM_CharThink.h"

#include <math.h>

#include "DAT_MagicParam.h"



BYTE	MagicTypeChar[10]={
	'z',	
	's',	
	'h',	
	'd',	
	'b',	
	'e',	
	'm'		
};


int	GetMagicSound( int mgno )
{
	if(MagicParam[ mgno ].sound!=-1){
		AVG_PlaySE3( MagicParam[ mgno ].sound, 255 );
	}
	return 1;
}


int	GetMagicNo( int type, int mno )
{
	int	i;
	for(i=0;i<MAGIC_MAX;i++){
		if( MagicParam[i].type == type && MagicParam[i].mno == mno ){
			return i;
		}
	}
	return -1;
}

int GetMagicRange( int mgno, int cno )
{
	int	range = MagicParam[mgno].range + ( MagicParam[mgno].range*CharParam[cno].GetWiz() )/2000;
	range = min(range,MagicParam[mgno].range*2);
	return range;
}
int GetMagicScope( int mgno, int cno )
{
	int	scope = MagicParam[mgno].scope + ( MagicParam[mgno].scope*CharParam[cno].GetMnd() )/2000;
	scope = min(scope,MagicParam[mgno].scope*2);
	return scope;
}
int GetMagicSng( int mgno, int cno )
{
	int	sng;

	if(JobParam[CharParam[ cno ].job].ex&EX_MGK_SPD || CharParam[cno].accessary1==202){
		sng = max( MagicParam[mgno].sng - CharParam[cno].GetSng()/3, MagicParam[mgno].sng/2 )*2/3;
	}else{
		sng = max( MagicParam[mgno].sng - CharParam[cno].GetSng()/3, MagicParam[mgno].sng/2 );
	}
	return sng;
}


typedef struct{
	int		flag;




	BYTE	step;

	short	no;
	short	mgno;

	short	tno;
	float	tx;
	float	ty;

	int		size;
	int		px,py;

	int		count;
}MAGIC_EFFECT_STRUCT;

#define MAGIC_EFFECT_MAX	32
MAGIC_EFFECT_STRUCT	MagicEffectStruct[MAGIC_EFFECT_MAX];



void SetMagicAction( int no )
{

	int		i,j,k;
	for(i=0;i<MAGIC_EFFECT_MAX;i++){
		if(!MagicEffectStruct[i].flag){
			break;
		}
	}
	j=0;
	if(i==MAGIC_EFFECT_MAX){
		for(k=0;k<MAGIC_EFFECT_MAX;k++){
			if(j<MagicEffectStruct[k].count){
				j=MagicEffectStruct[k].count;
				i=k;
			}
		}
	}
	if(i==MAGIC_EFFECT_MAX) return ;

	MagicEffectStruct[i].flag  = ON;
	MagicEffectStruct[i].step  = 0;
	MagicEffectStruct[i].count = 0;
	MagicEffectStruct[i].no    = no;
	MagicEffectStruct[i].mgno  = CharControl[no].mgno;
	MagicEffectStruct[i].tno = CharControl[no].tcno;
	MagicEffectStruct[i].tx = CharControl[no].tx[0];
	MagicEffectStruct[i].ty = CharControl[no].ty[0];

	if( JobParam[CharParam[CharControl[no].cno].job].ex&EX_BOSS2 ){
		if(CharParam[CharControl[i].cno].job==87 || CharParam[CharControl[i].cno].job==88){
			MagicEffectStruct[i].size = 128;
			MagicEffectStruct[i].px = 200;
			MagicEffectStruct[i].py = -88;
		}else{
			MagicEffectStruct[i].size = 64;
			MagicEffectStruct[i].px = -52;
			MagicEffectStruct[i].py = -36;
		}
	}else{
		MagicEffectStruct[i].size = 32;
	}


	CharParam[CharControl[no].cno].AddMp( -MagicParam[ CharControl[no].mgno ].mp );
}
void ResetMagicAction( int no )
{
	int		i;
	for(i=0;i<MAGIC_EFFECT_MAX;i++){
		if(MagicEffectStruct[i].flag){
			if(MagicEffectStruct[i].no==no){
				GD_ResetGraph( GRP_MG_EFFECT+i );
				GD_ReleaseSprite( SPR_MG_EFFECT+i );

				ZeroMemory( &MagicEffectStruct[i], sizeof(MAGIC_EFFECT_STRUCT) );
				break;
			}
		}
	}
}
void ResetMagicActionAll(void)
{
	int		i;
	for(i=0;i<MAGIC_EFFECT_MAX;i++){
		if(MagicEffectStruct[i].flag){
			GD_ResetGraph( GRP_MG_EFFECT+i );
			GD_ReleaseSprite( SPR_MG_EFFECT+i );

		}
		ZeroMemory( &MagicEffectStruct[i], sizeof(MAGIC_EFFECT_STRUCT) );
	}
}

BOOL WaitMagicAction( int no )
{
	int		i;
	for(i=0;i<MAGIC_EFFECT_MAX;i++){
		if(MagicEffectStruct[i].flag){
			if(MagicEffectStruct[i].no==no){
				return MagicEffectStruct[i].step==4;
			}
		}
	}
	return TRUE;
}
BOOL CheckMagicAction( int no )
{
	int		i;
	for(i=0;i<MAGIC_EFFECT_MAX;i++){
		if(MagicEffectStruct[i].flag){
			if(MagicEffectStruct[i].no==no){
				return TRUE;
			}
		}
	}
	return FALSE;
}

int MagicStnNormal( int no, int tno, int mgno )
{
	float	mgk = CharParam[CharControl[no].cno].GetMgk() +100+MagicParam[mgno].param[0];
	float	rgs = CharParam[CharControl[tno].cno].GetRgs()+100;
	double	rate=1.0;

	rate += AttrRate( MagicParam[mgno].attr, CharParam[CharControl[tno].cno].attr );
	

	return 1;
}
int MagicStnNormalSet( int no, int tno, int mgno )
{
	int	stn = MagicDmageNormal(no, tno, mgno );
	CHAR_PARAM	*ac = &CharParam[CharControl[no].cno];
	CHAR_PARAM	*tc = &CharParam[CharControl[tno].cno];

	if( stn ){
		CharControl[ tno ].dmg += 200;
	}else{
		SetHitAction( tno, 0, HIT_DAMEGE );
	}
	return stn;
}


int MagicDmageNormal( int no, int tno, int mgno )
{
	float	mgk = CharParam[CharControl[no].cno].GetMgk() +100+MagicParam[mgno].param[0];
	float	rgs = CharParam[CharControl[tno].cno].GetRgs()+100;
	int	dmg;
	double	rate=1.0, rate2;

	if(MagicParam[mgno].attr == CharParam[CharControl[no].cno].attr){
		rate=1.1;
	}
	rate2 = AttrRate( MagicParam[mgno].attr, CharParam[CharControl[tno].cno].attr );
	if(JobParam[CharParam[CharControl[tno].cno].job].ex&EX_ATR_DEF){
		if(rate2<0){
			rate += rate2;
		}
	}else{
		rate += rate2;
	}

	dmg = LIM( (mgk*mgk/rgs-rgs/3)/5, 1, 99999 );

	return dmg;
}
int MagicDmageNormalSet( int no, int tno, int mgno, int *did )
{
	int	dmg = MagicDmageNormal(no, tno, mgno );
	CHAR_PARAM	*ac = &CharParam[CharControl[no].cno];
	CHAR_PARAM	*tc = &CharParam[CharControl[tno].cno];

	if( tc->CheckStatusEffct(STATUS_DEF_MP_0) ){
		dmg=0;
	}
	tc->ResetStatusEffctEx( STATUS_DEF_HP_0 );
	tc->ResetStatusEffctEx( STATUS_DEF_MP_0 );

	SetHitAction( tno, dmg, 0 );
	if(dmg){
		dmg = min(tc->php, dmg);

		tc->AddHp( -dmg );
		if( tc->php<=0 && tc->did==FALSE ){
			*did=1;
			tc->did = TRUE;
		}
		CharControl[ tno ].dmg += 10;
	}
	return dmg;
}

int MagicMpDmageNormal( int no, int tno, int mgno )
{
	float	mgk = CharParam[CharControl[no].cno].GetMgk() +MagicParam[mgno].param[0];
	float	rgs = CharParam[CharControl[tno].cno].GetRgs()+100;
	int	dmg;
	double	rate=1.0;

	rate += AttrRate( MagicParam[mgno].attr, CharParam[CharControl[tno].cno].attr );

	dmg = LIM( (mgk*mgk/rgs-rgs/3)/5, 1, 99999 );

	dmg = min( CharParam[CharControl[tno].cno].pmp, dmg);

	return dmg;
}
int MagicMpDmageNormalSet( int no, int tno, int mgno )
{
	int	dmg = MagicMpDmageNormal(no, tno, mgno )/2;
	CHAR_PARAM	*ac = &CharParam[CharControl[no].cno];
	CHAR_PARAM	*tc = &CharParam[CharControl[tno].cno];

	SetHitAction( tno, dmg, 0 );
	if(dmg){

		tc->AddMp( -dmg );
		CharControl[ tno ].dmg += 10;
	}
	return dmg;
}


int MagicRestNormal( int no, int tno, int mgno )
{
	int	rst = MagicParam[mgno].param[0] + MagicParam[mgno].param[0] * CharParam[CharControl[no].cno].GetRst()/200;
	int	php = CharParam[CharControl[tno].cno].php;
	int	hp = CharParam[CharControl[tno].cno].GetHp();
	int	rst2;

	rst2 = LIM( rst, 0, hp-php );
	return rst2;
}
int MagicRestNormalSet( int no, int tno, int mgno )
{
	int	dmg = MagicRestNormal( no, tno, mgno );
	CHAR_PARAM	*ac = &CharParam[CharControl[no].cno];
	CHAR_PARAM	*tc = &CharParam[CharControl[tno].cno];

	SetHitAction( tno, dmg, 0, HIT_HEAL );
	if(dmg){
		tc->AddHp( dmg );
	}
	return dmg;
}
int MagicStatusEffctSet( int no, int tno, int eff, int param )
{
	CHAR_PARAM	*ac = &CharParam[CharControl[no].cno];
	CHAR_PARAM	*tc = &CharParam[CharControl[tno].cno];

	if(JobParam[tc->job].ex&EX_GAKUSI && param<0){
		SetHitAction( tno, 0, 0 );
		return 1;
	}

	if(param<0){
		SetHitAction( tno, 0, 0, HIT_POWDOWN );
	}else if(param>0){
		SetHitAction( tno, 0, 0, HIT_POWUP );
	}else{
		SetHitAction( tno, 0, 0, HIT_DISPEL );
	}

	tc->SetStatusEffct( eff, param );

	return 1;
}


int RenewMagicAction( int char_active )
{
	int		no, tno;
	float	tx,ty;
	int		i;


	if(1){
		for(i=0;i<MAGIC_EFFECT_MAX;i++){
			if(MagicEffectStruct[i].flag){
				no  = MagicEffectStruct[i].no;



				tno = MagicEffectStruct[i].tno;
				tx  = MagicEffectStruct[i].tx;
				ty  = MagicEffectStruct[i].ty;

				switch( MagicParam[ MagicEffectStruct[i].mgno ].scope_type ){
					case RANGE_TAGET:		
						tx = CharControl[ tno ].px;
						ty = CharControl[ tno ].py;
						break;
					case RANGE_SELF:			
					case RANGE_SELF_SCOPE:	
						tx = CharControl[ no ].px;
						ty = CharControl[ no ].py;
						break;
					case RANGE_SCOPE:		
					case RANGE_LINE:		
					case RANGE_FAN:			
					case RANGE_ALL:			
						break;
				}

				CHAR_PARAM	*cp = &CharParam[CharControl[no].cno];
				int	j;
				int	mgno = MagicEffectStruct[i].mgno;
				int	param_count=0;
				int	scope = GetMagicScope( mgno, CharControl[no].cno );
				int	scope_type = MagicParam[mgno].scope_type;
				float	mgx,mgy,mgw,mgh;
				float	scope_zoom=1.0f;

				if( MagicParam[mgno].scope ){
					scope_zoom = scope/MagicParam[mgno].scope;
				}
				switch(MagicEffectStruct[i].step){
					case 0:
						if(MagicEffectStruct[i].count<16){
							mgx = (CharControl[no].px-MapControl.px+MagicEffectStruct[i].px)*MapControl.zoom;
							mgy = (CharControl[no].py-MapControl.py+MagicEffectStruct[i].py-MagicEffectStruct[i].count)*MapControl.zoom;
							mgw = 2*MagicEffectStruct[i].size*MapControl.zoom*(MagicEffectStruct[i].count+16)/16;
							mgh =   MagicEffectStruct[i].size*MapControl.zoom*(MagicEffectStruct[i].count+16)/16;
						}else{
							int	count=MagicEffectStruct[i].count-16;

							mgx = ((CharControl[no].px-MapControl.px+MagicEffectStruct[i].px   )*(16-count) + (tx-MapControl.px)*count)/16*MapControl.zoom;
							mgy = ((CharControl[no].py-MapControl.py+MagicEffectStruct[i].py-16)*(16-count) + (ty-MapControl.py)*count)/16*MapControl.zoom;
							mgw = 2*MagicEffectStruct[i].size*MapControl.zoom*(48-MagicEffectStruct[i].count)/16;
							mgh =   MagicEffectStruct[i].size*MapControl.zoom*(48-MagicEffectStruct[i].count)/16;
						}
						GD_SetGraphRoll2( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+no, mgx, mgy, mgw, mgh, CharControl[no].total_count*4.0f );
						GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+no, 255 );

						if( MagicEffectStruct[i].count>=32 ){
							MagicEffectStruct[i].step=1;
							MagicEffectStruct[i].count=0;

			

							char	buf[256];
							char	buf2[256];
	
							if( GetMagicNo( MAGIC_DARK,  0 )==mgno || GetMagicNo( MAGIC_DARK,  1 )==mgno ){
								if(DrawSetting.low_level){
									wsprintf( buf,  "mg%c%03da_low.ani", MagicTypeChar[ MagicParam[ mgno ].type ], MagicParam[ mgno ].mno );
									wsprintf( buf2, "mg%c%03db_low.ani", MagicTypeChar[ MagicParam[ mgno ].type ], MagicParam[ mgno ].mno );
								}else{
									wsprintf( buf,  "mg%c%03da.ani", MagicTypeChar[ MagicParam[ mgno ].type ], MagicParam[ mgno ].mno );
									wsprintf( buf2, "mg%c%03db.ani", MagicTypeChar[ MagicParam[ mgno ].type ], MagicParam[ mgno ].mno );
								}
								GD_LoadSprite( SPR_MG_EFFECT+i, buf, ALPHA_4444 );
								GD_LoadSprite( SPR_MG_WORK+0, buf2, ALPHA_4444 );

								GD_SetSprite( GRP_MG_WORK+0, SPR_MG_WORK+0, 0, TRUE, LAY_EFFECT, OFF, FALSE, 1, FALSE );
								GD_SetGraphScaleParam( GRP_MG_WORK+0, SCALE_HIGHT );
							}else{
								if(DrawSetting.low_level){
									wsprintf( buf, "mg%c%03d_low.ani", MagicTypeChar[ MagicParam[ mgno ].type ], MagicParam[ mgno ].mno );
								}else{
									wsprintf( buf, "mg%c%03d.ani", MagicTypeChar[ MagicParam[ mgno ].type ], MagicParam[ mgno ].mno );
								}
								GD_LoadSprite( SPR_MG_EFFECT+i, buf, ALPHA_4444 );

								if( GetMagicNo( MAGIC_ZOKUSEI, 51 )==mgno ){
									GD_SetSprite( GRP_MG_WORK+0, SPR_MG_EFFECT+i, 0, TRUE, LAY_EFFECT, OFF, FALSE, 1, FALSE );
									GD_SetGraphScaleParam( GRP_MG_WORK+0, SCALE_HIGHT );
								}
							}
							GetMagicSound( mgno );

							GD_SetSprite( GRP_MG_EFFECT+i, SPR_MG_EFFECT+i, 0, TRUE, LAY_EFFECT, OFF, FALSE, 1, FALSE );
							GD_SetGraphScaleParam( GRP_MG_EFFECT+i, SCALE_HIGHT );
						}
						break;
					case 1:
						mgx = (tx-MapControl.px)*MapControl.zoom;
						mgy = (ty-MapControl.py)*MapControl.zoom;
						mgw = 2*MagicEffectStruct[i].size*MapControl.zoom;
						mgh =   MagicEffectStruct[i].size*MapControl.zoom;

						GD_SetGraphRoll2( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+no, mgx, mgy, mgw, mgh, CharControl[no].total_count*8.0f );
						GD_SetGraphCenter( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+no, mgx, mgy );
						GD_SetGraphZoom( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+no, (MagicEffectStruct[i].count+60.0f)/60.0f );
						GD_SetGraphAlpha( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+no, 255*(16-MagicEffectStruct[i].count)/16 );
						
						GD_SetGraphDisp( GRP_MG_EFFECT+i, ON );
						GD_SetGraphDstMove( GRP_MG_EFFECT+i, (tx-MapControl.px)*MapControl.zoom, (ty-MapControl.py)*MapControl.zoom );
						GD_SetGraphZoom( GRP_MG_EFFECT+i, MapControl.zoom*MagicParam[mgno].zoom*(1+DrawSetting.low_level) * scope_zoom );


						tx = CharControl[ no ].px;
						ty = CharControl[ no ].py;
						GD_SetGraphDisp( GRP_MG_WORK+0, ON );
						GD_SetGraphDstMove( GRP_MG_WORK+0, (tx-MapControl.px)*MapControl.zoom, (ty-MapControl.py)*MapControl.zoom );
						GD_SetGraphZoom( GRP_MG_WORK+0, MapControl.zoom*MagicParam[mgno].zoom*(1+DrawSetting.low_level) * scope_zoom );

						if( GetMagicNo( MAGIC_ZOKUSEI, 51 )==mgno ){	
							GD_SetGraphAlpha( GRP_MAP_CHAR+no,  abs(256-16*MagicEffectStruct[i].count) );
							GD_SetGraphAlpha( GRP_MAP_CHAR+tno, abs(256-16*MagicEffectStruct[i].count) );
							if(MagicEffectStruct[i].count==16){
								if( !(JobParam[CharParam[ CharControl[ tno ].cno].job].ex&EX_BOSS2) ){
									CharControl[ no ].px = CharControl[ tno ].px;
									CharControl[ no ].py = CharControl[ tno ].py;
									CharControl[ tno ].px = tx;
									CharControl[ tno ].py = ty;
								}
							}
						}

						if( !GD_WaitSpriteCondition(GRP_MG_EFFECT+i) ){
							GD_ResetGraph( GRP_MG_EFFECT+i );
							GD_ResetGraph( GRP_MAP_CHAR+CHAR_CONTROL_MAX*4+no );
							GD_ResetGraph( GRP_MG_WORK+0 );

							MagicEffectStruct[i].count=0;
							MagicEffectStruct[i].step=2;
						}




						break;
					case 2:
					{
						int	dmg=0;
						for( param_count=0 ; param_count<2 ; param_count++ ){
							
							int	mgk = MagicParam[mgno].param[param_count]+cp->GetMgk();
							int	rgs;
							int	rst = MagicParam[mgno].param[param_count] + MagicParam[mgno].param[param_count] * cp->GetRst()/100;
							int	did=0,lv=0,did_count=0;
							int	work_param1;
							int	work_param2;

							switch( MagicParam[mgno].param_type[param_count] ){
							case MG_TYPE_HP_GET:
								SetHitAction( no, dmg, 0, HIT_HEAL );
								cp->AddHp( dmg );
								break;
							case MG_TYPE_MP_GET:
								SetHitAction( no, dmg, 0, HIT_HEAL );
								cp->AddMp( dmg );
								break;
							case MG_TYPE_TENI:
								if( JobParam[CharParam[ CharControl[ tno ].cno].job].ex&EX_BOSS2 ){
									SetHitAction( tno, 0, 0 );
								}
								break;

							case MG_TYPE_STAN:
								switch(scope_type){
									case RANGE_TAGET:		
									case RANGE_SELF:		
										dmg = MagicStnNormalSet(no, tno, mgno );
										lv = CharParam[CharControl[tno].cno].lv;
										break;
									case RANGE_SCOPE:		
									case RANGE_SELF_SCOPE:	
										for(j=0;j<CHAR_CONTROL_MAX;j++){
											if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
												if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
													dmg = MagicStnNormalSet(no, j, mgno );
													lv = CharParam[CharControl[j].cno].lv;
												}
											}
										}
										break;
									case RANGE_LINE:		
									case RANGE_FAN:			
									case RANGE_ALL:			
										break;
								}
								if(lv)
									AVG_PlaySE3( 8 );
								break;

							case MG_TYPE_DAMAGE:	
								switch(scope_type){
									case RANGE_TAGET:		
									case RANGE_SELF:		
										did=0;
										dmg += MagicDmageNormalSet(no, tno, mgno, &did );
										if(did){
											did_count = min(did_count+1,3);
										}
										lv = CharParam[CharControl[tno].cno].lv;
										break;
									case RANGE_SCOPE:		
									case RANGE_SELF_SCOPE:	
										for(j=0;j<CHAR_CONTROL_MAX;j++){
											if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
												if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
													did=0;
													dmg += MagicDmageNormalSet(no, j, mgno, &did );
													if(did){
														did_count = min(did_count+1,3);
													}
													if( lv < CharParam[CharControl[j].cno].lv ){
														lv = CharParam[CharControl[j].cno].lv;
													}
												}
											}
										}
										break;
									case RANGE_LINE:		
									case RANGE_FAN:			
									case RANGE_ALL:			
										break;
								}
								if(lv)
									AVG_PlaySE3( 8 );
								break;
							case MG_TYPE_DAMAGE_MP:	
								switch(scope_type){
									case RANGE_TAGET:		
									case RANGE_SELF:		
										dmg += MagicMpDmageNormalSet(no, tno, mgno );
										lv = CharParam[CharControl[tno].cno].lv;
										break;
									case RANGE_SCOPE:		
									case RANGE_SELF_SCOPE:	
										for(j=0;j<CHAR_CONTROL_MAX;j++){
											if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
												if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
													dmg += MagicMpDmageNormalSet(no, j, mgno );
													if( lv < CharParam[CharControl[j].cno].lv ){
														lv = CharParam[CharControl[j].cno].lv;
													}
												}
											}
										}
										break;
									case RANGE_LINE:		
									case RANGE_FAN:			
									case RANGE_ALL:			
										break;
								}
								if(lv)
									AVG_PlaySE3( 8 );
								break;


							case MG_TYPE_HEAL:	
								switch(scope_type){
									case RANGE_TAGET:		
									case RANGE_SELF:		
										if(MagicRestNormalSet( no, tno, mgno ) ){
											lv = CharParam[CharControl[tno].cno].lv;
										}
										break;
									case RANGE_SCOPE:		
									case RANGE_SELF_SCOPE:	
										for(j=0;j<CHAR_CONTROL_MAX;j++){
											if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
												if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
													if( MagicRestNormalSet( no, j, mgno ) ){
														if( lv < CharParam[CharControl[j].cno].lv ){
															lv = CharParam[CharControl[j].cno].lv;
														}
													}
												}
											}
										}
										break;
									case RANGE_LINE:		
									case RANGE_FAN:			
									case RANGE_ALL:			
										break;
								}
								break;
							case MG_TYPE_HACK:
							
							case MG_TYPE_ATK:
							case MG_TYPE_MGK:
							case MG_TYPE_DEF:
							case MG_TYPE_RGS:
							case MG_TYPE_WSP:
							case MG_TYPE_MOV:
							case MG_TYPE_DISPEL:
								switch( MagicParam[mgno].param_type[param_count] ){
									case MG_TYPE_ATK:	work_param1 = STATUS_EFF_ATK;	break;
									case MG_TYPE_MGK:	work_param1 = STATUS_EFF_MGK;	break;
									case MG_TYPE_DEF:	work_param1 = STATUS_EFF_DEF;	break;
									case MG_TYPE_RGS:	work_param1 = STATUS_EFF_RGS;	break;
									case MG_TYPE_WSP:	work_param1 = STATUS_EFF_WSP;	break;
									case MG_TYPE_MOV:	work_param1 = STATUS_EFF_MOV;	break;
									case MG_TYPE_DISPEL:	work_param1 = -1;	break;
									case MG_TYPE_HACK:	work_param1 = STATUS_HACK;		break;
								}
								work_param2 = MagicParam[mgno].param[param_count];

								switch(scope_type){
									case RANGE_TAGET:		
									case RANGE_SELF:		
										MagicStatusEffctSet( no, tno, work_param1, work_param2 );
										lv = CharParam[CharControl[tno].cno].lv;
										break;
									case RANGE_SCOPE:		
									case RANGE_SELF_SCOPE:	
										for(j=0;j<CHAR_CONTROL_MAX;j++){
											if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
												if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
													MagicStatusEffctSet( no, j, work_param1, work_param2 );
													if( lv < CharParam[CharControl[j].cno].lv ){
														lv = CharParam[CharControl[j].cno].lv;
													}
												}
											}
										}
										break;
									case RANGE_LINE:		
									case RANGE_FAN:			
									case RANGE_ALL:			
										break;
								}
								break;
							}

							if(did_count){
								int	ii,jj=0;
								for(ii=0;ii<did_count;ii++){
									jj |= CharParam[CharControl[no].cno].AddExpAll( lv, TRUE );
								}
								if(jj){
									SetHitAction( no, 0, 0.0f, HIT_LVUP );
								}
							}else{
								if( CharParam[CharControl[no].cno].AddExpAll( lv, FALSE ) ){
									SetHitAction( no, 0, 0.0f, HIT_LVUP );
								}
							}
						}

						MagicEffectStruct[i].step=3;
						MagicEffectStruct[i].count=0;
					}
						break;
					case 3:
						if( MagicEffectStruct[i].count>=32 ){
							MagicEffectStruct[i].step=4;
						}
						break;
					case 4:
						break;
				}
				if(char_active){
					MagicEffectStruct[i].count++;
				}
			}
		}
	}
	return 1;
}


typedef struct{
	int		flag;

	float	px;
	float	py;
	int		scope;

	short	no;
	short	mgno;

	float	count[8];
	float	dir[8];
	float	speed[8];
	int		bcount;
	int		mpcount;
}BARD_STRUCT;

#define BARD_MAX	10
BARD_STRUCT	BardStruct[BARD_MAX];


void SetBardAction( int no )
{
	int		i,j;

	for(i=0;i<BARD_MAX;i++){
		if(BardStruct[i].flag){
			if(BardStruct[i].no==no){
				break;
			}
		}
	}
	if(i==BARD_MAX){
		for(i=0;i<BARD_MAX;i++){
			if(!BardStruct[i].flag){
				break;
			}
		}
	}
	if(i==BARD_MAX){
		DebugBox( NULL, "吟遊エフェクト多すぎ" );
		return ;
	}







	BardStruct[i].flag  = ON;
	BardStruct[i].no    = no;
	BardStruct[i].mgno  = CharControl[no].mgno;
	BardStruct[i].px    = CharControl[no].px;
	BardStruct[i].py    = CharControl[no].py;
	BardStruct[i].scope = GetMagicScope( CharControl[no].mgno, CharControl[no].cno );
	BardStruct[i].bcount= 0;
	BardStruct[i].mpcount= 0;
	for(j=0;j<8;j++){
		BardStruct[i].count[i] = 0;
		BardStruct[i].dir[j]   = D3DXToRadian( rand()%360 );
		BardStruct[i].speed[j] = (5+rand()%20)/15.0f;

		GD_SetGraph( GRP_BARD_EFFECT+i*8+j, BMP_EFFECT+1, LAY_EFFECT, OFF );
		GD_SetGraphSrcRect( GRP_BARD_EFFECT+i*8+j, (float)(j%4*32), (float)(j/4*32), 32, 32 );
		GD_SetGraphBlendParam( GRP_BARD_EFFECT+i*8+j, BLEND_ALPHA );
		GD_SetGraphScaleParam( GRP_BARD_EFFECT+i*8+j, SCALE_HIGHT );
	}



}
void ResetBardAction( int no )
{
	int		i,j;
	for( i=0 ; i<BARD_MAX ; i++ ){
		if( BardStruct[i].flag ){
			if( BardStruct[i].no==no ){
				for( j=0 ; j<8; j++ ){
					GD_ResetGraph( GRP_BARD_EFFECT+i*8+j );
				}
				ZeroMemory( &BardStruct[i], sizeof(BARD_STRUCT) );
				break;
			}
		}
	}
}
void ResetBardActionAll(void)
{
	int	i,j;
	for( i=0 ; i<BARD_MAX ; i++ ){
		if( BardStruct[i].flag ){
			for( j=0 ; j<8; j++ ){
				GD_ResetGraph( GRP_BARD_EFFECT+i*8+j );
			}
		}
		ZeroMemory( &BardStruct[i], sizeof(BARD_STRUCT) );
	}
}

void GetBardExp( int no, int tlv )
{
	int	i;
	for( i=0 ; i<BARD_MAX ; i++ ){
		if( BardStruct[i].flag ){
			if( CharParam[CharControl[no].cno].unit != CharParam[ CharControl[BardStruct[i].no].cno ].unit) continue;

			if( pow(BardStruct[i].px-CharControl[no].px,2) + pow(BardStruct[i].py-CharControl[no].py,2) < pow(BardStruct[i].scope,2) ){
				float exp_rate;
				int	lv = CharParam[CharControl[BardStruct[i].no].cno].lv;

				if(tlv<=lv) exp_rate = (tlv-lv+5)/5.0f;
				else		exp_rate = (pow(tlv-lv,2)+5)/5.0f;

				exp_rate = LIM( exp_rate, 0.1f, 10.0f); 
				int	add_exp = (int)( (tlv+10)*(tlv+1)*exp_rate );

				if( CharParam[CharControl[BardStruct[i].no].cno].AddExp( add_exp/5/10, 0 ) ){
					SetHitAction( BardStruct[i].no, 0, 0.0f, HIT_LVUP );
				}
			}
		}
	}

}

int GetBardParam( int no, DWORD type )
{
	int		i;
	int		param_count;
	int		ret=0;

	for( i=0 ; i<BARD_MAX ; i++ ){
		if( BardStruct[i].flag ){
			if( CharParam[CharControl[no].cno].unit != CharParam[ CharControl[BardStruct[i].no].cno ].unit) continue;
			if( pow(BardStruct[i].px-CharControl[no].px,2) + pow(BardStruct[i].py-CharControl[no].py,2) < pow(BardStruct[i].scope,2) ){
				for( param_count=0 ; param_count<2 ; param_count++ ){
					if( MagicParam[BardStruct[i].mgno].param_type[param_count] == type ){
						if(MagicParam[BardStruct[i].mgno].param[param_count]<0){
							ret += min( -10, MagicParam[BardStruct[i].mgno].param[param_count]*CharParam[CharControl[BardStruct[i].no].cno].GetSng() / 100 );
						}else{
							ret += max( 10, MagicParam[BardStruct[i].mgno].param[param_count]*CharParam[CharControl[BardStruct[i].no].cno].GetSng() / 100 );
						}
					}
				}
			}
		}
	}
	return ret;
}

int RenewBardAction( int char_active )
{
	int		i,j;
	int		param_count;
	float	px,py;

	for(i=0;i<BARD_MAX;i++){
		if(BardStruct[i].flag){




			px = BardStruct[i].px = CharControl[BardStruct[i].no].px;
			py = BardStruct[i].py = CharControl[BardStruct[i].no].py;

			CHAR_PARAM	*cp = &CharParam[CharControl[BardStruct[i].no].cno];
			int	mgno = BardStruct[i].mgno;

			for( param_count=0 ; param_count<1 ; param_count++ ){
				int	param = max( 1, MagicParam[mgno].param[param_count] * cp->GetSng() / 1000 );

				if(char_active){
					BardStruct[i].bcount += param;
				}

				if( BardStruct[i].bcount>100 ){
					BardStruct[i].bcount=0;
					for(j=0;j<CHAR_CONTROL_MAX;j++){
						if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
							if(pow(CharControl[j].px-px,2)+pow(CharControl[j].py-py,2) < pow(BardStruct[i].scope,2) ){

								switch( MagicParam[mgno].param_type[param_count] ){
								case MG_TYPE_DAMAGE:	
									if( CharParam[CharControl[j].cno].unit != CharParam[CharControl[BardStruct[i].no].cno].unit){
										CharParam[CharControl[j].cno].AddHp(-1);
									}
									break;
								case MG_TYPE_HEAL:		
									if( CharParam[CharControl[j].cno].unit == CharParam[CharControl[BardStruct[i].no].cno].unit){
										CharParam[CharControl[j].cno].AddHp(1);
									}
									break;
								case MG_TYPE_MP:		
									if( CharParam[CharControl[j].cno].unit == CharParam[CharControl[BardStruct[i].no].cno].unit){
										if(j!=BardStruct[i].no){
											CharParam[CharControl[j].cno].AddMp(1);
										}
									}
									break;
								}
							}
						}
					}
				}
			}

			for(j=0;j<8;j++){
				float	px = BardStruct[i].px-MapControl.px + cos(BardStruct[i].dir[j])*BardStruct[i].count[j];
				float	py = BardStruct[i].py-MapControl.py + sin(BardStruct[i].dir[j])*BardStruct[i].count[j] - BardStruct[i].count[j];

				GD_SetGraphDisp( GRP_BARD_EFFECT+i*8+j, ON );
				GD_SetGraphDstRect( GRP_BARD_EFFECT+i*8+j, (px-16)*MapControl.zoom, (py-16)*MapControl.zoom, 32, 32 );
				GD_SetGraphZoom( GRP_BARD_EFFECT+i*8+j, BardStruct[i].count[j]*MapControl.zoom/60 );
				GD_SetGraphCenter( GRP_BARD_EFFECT+i*8+j, px*MapControl.zoom, py*MapControl.zoom );

				GD_SetGraphAlpha( GRP_BARD_EFFECT+i*8+j, 255*(30-max(0,BardStruct[i].count[j]-60))/30.0f );

				if(char_active){
					BardStruct[i].count[j] += BardStruct[i].speed[j];
				}
				if( BardStruct[i].count[j]>90 ){
					BardStruct[i].dir[j]   = D3DXToRadian( rand()%360 );
					BardStruct[i].speed[j] = (5+rand()%20)/15.0f;
					BardStruct[i].count[j] = 0;;
				}
			}
			if(char_active){
				BardStruct[i].mpcount += MagicParam[mgno].mp;
			}
			if( BardStruct[i].mpcount>60 ){
				BardStruct[i].mpcount=0;
				cp->AddMp(-1);
				if(cp->pmp<=0){
					cp->pmp=0;
					SetMapCharStand(BardStruct[i].no);
				}
			}

		}
	}
	return 1;
}


typedef struct{
	int		flag;

	BYTE	step;

	short	no;
	short	mgno;

	short	tno;
	float	tx;
	float	ty;
	float	tx2[4];
	float	ty2[4];

	int		count;
}SKILL_EFFECT_STRUCT;

#define SKILL_MAX	32

SKILL_EFFECT_STRUCT	SkillEffectStruct[SKILL_MAX];


BOOL SetSkillAction( int no )
{
	int	i;
	ResetSkillAction( no );

	for( i=0 ; i<SKILL_MAX ; i++ ){
		if( SkillEffectStruct[i].flag==0 ){
			break;
		}
	}

	SkillEffectStruct[i].flag  = TRUE;
	SkillEffectStruct[i].step  = 0;
	SkillEffectStruct[i].count = 0;

	SkillEffectStruct[i].no   = no;
	SkillEffectStruct[i].mgno = CharControl[no].mgno;

	SkillEffectStruct[i].tno = CharControl[no].tcno;
	SkillEffectStruct[i].tx  = CharControl[no].tx[0];
	SkillEffectStruct[i].ty  = CharControl[no].ty[0];

	CharParam[CharControl[no].cno].AddMp( -MagicParam[ CharControl[no].mgno ].mp );
	return 1;
}
void ResetSkillAction( int no )
{
	int	i;
	for( i=0 ; i<SKILL_MAX ; i++ ){
		if(SkillEffectStruct[i].flag){
			if(SkillEffectStruct[i].no==no){
				SkillEffectStruct[i].flag  = FALSE;
				SkillEffectStruct[i].step  = 0;
				SkillEffectStruct[i].count = 0;
			}
		}
	}
}
BOOL WaitSkillAction( int no )
{
	int	i;
	for( i=0 ; i<SKILL_MAX ; i++ ){
		if(SkillEffectStruct[i].flag){
			if(SkillEffectStruct[i].no==no){
				return SkillEffectStruct[i].step==5;
			}
		}
	}
	return TRUE;
}


int RenewSkillAction( int char_active )
{
	int		no, tno;
	float	tx,ty, px,py;
	int		i,j;
	char	buf[256];
	char	buf2[256];

int	ii;
for( ii=0 ; ii<SKILL_MAX ; ii++ ){
	if(SkillEffectStruct[ii].flag){
		no  = SkillEffectStruct[ii].no;
		tno = SkillEffectStruct[ii].tno;
		tx  = SkillEffectStruct[ii].tx;
		ty  = SkillEffectStruct[ii].ty;
		px = CharControl[ no ].px;
		py = CharControl[ no ].py;

		switch( MagicParam[ SkillEffectStruct[ii].mgno ].scope_type ){
			case RANGE_TAGET:		
				tx = CharControl[ tno ].px;
				ty = CharControl[ tno ].py;
				break;
			case RANGE_SELF:			
			case RANGE_SELF_SCOPE:	
				tx = CharControl[ no ].px;
				ty = CharControl[ no ].py;
				break;
			case RANGE_SCOPE:		
			case RANGE_LINE:		
			case RANGE_FAN:			
			case RANGE_ALL:			
				break;
		}

		CHAR_PARAM	*cp = &CharParam[CharControl[no].cno];
		int	mgno = SkillEffectStruct[ii].mgno;
		int	param_count=0;
		int	scope = GetMagicScope( mgno, CharControl[no].cno );
		int	scope_type = MagicParam[mgno].scope_type;

		int	dmg=0,lv=0,did=0;
		int	dir = CharControl[no].dir;

	


		if( GetMagicNo( MAGIC_SKILL, 13 )==mgno ){
			switch(SkillEffectStruct[ii].step){
			case 0:
				GetMagicSound( mgno );

				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+24, TRUE, TRUE, 1, FALSE );
				SkillEffectStruct[ii].step=1;
				break;
			case 1:
				if( !GD_WaitSpriteCondition(GRP_MAP_CHAR+no) ){
					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=2;
				}
				break;
			case 2:
				SkillEffectStruct[ii].count=0;
				SkillEffectStruct[ii].step=3;
				break;
			case 3:
				switch(scope_type){
					case RANGE_TAGET:		
					case RANGE_SELF:		
						dmg += AtackNormalDmg( no, tno, 0, MagicParam[mgno].param[0] );
						lv = CharParam[CharControl[tno].cno].lv;
						break;
					case RANGE_SCOPE:		
					case RANGE_SELF_SCOPE:	
						for(j=0;j<CHAR_CONTROL_MAX;j++){
							if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
								if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
									
									dmg += AtackNormalDmg( no, j, 0, MagicParam[mgno].param[0] );
									if( lv < CharParam[CharControl[j].cno].lv ){
										lv = CharParam[CharControl[j].cno].lv;
									}
								}
							}
						}
						break;
				}

				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+8, TRUE, TRUE, 1, FALSE );
				CharControl[no].sw =  MagicParam[mgno].sng;

				SkillEffectStruct[ii].count=0;
				SkillEffectStruct[ii].step=4;
				break;
			case 4:
				if(SkillEffectStruct[ii].count>30){
					SkillEffectStruct[ii].step=5;
				}
				break;
			case 5:
				break;
			}
		}else if( GetMagicNo( MAGIC_SKILL, 12 )==mgno ){
			switch(SkillEffectStruct[ii].step){
			case 0:
				if(DrawSetting.low_level){
					wsprintf( buf, "mgm%03d_low.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+0, buf, ALPHA_4444 );
				}else{
					wsprintf( buf, "mgm%03d.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+0, buf, ALPHA_4444 );
				}
				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+24, TRUE, TRUE, 1, FALSE );
				SkillEffectStruct[ii].step=1;
				SetBrightBattleMap( 96, 96, 96, 30,0 );
				break;
			case 1:
				if( !GD_WaitSpriteCondition(GRP_MAP_CHAR+no) ){
					GetMagicSound( mgno );

					GD_SetSprite( GRP_MG_WORK+0, SPR_MG_WORK+0, 0, TRUE, LAY_EFFECT+1, OFF, FALSE, 1, FALSE );
					GD_SetGraphBlendParam( GRP_MG_WORK+0, BLEND_ADD );
					GD_SetGraphScaleParam( GRP_MG_WORK+0, SCALE_HIGHT );

					GD_SetSprite( GRP_MG_WORK+1, SPR_MG_WORK+0, 1, TRUE, LAY_EFFECT+0, OFF, FALSE, 1, FALSE );
					GD_SetGraphScaleParam( GRP_MG_WORK+1, SCALE_HIGHT );


					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=2;
				}
				break;
			case 2:
				px = px-43;
				py = py-163;

				
				GD_SetGraphDisp( GRP_MG_WORK+0, ON );
				GD_SetGraphDstMove( GRP_MG_WORK+0, (px-MapControl.px)*MapControl.zoom, (py-MapControl.py)*MapControl.zoom);
				GD_SetGraphZoom( GRP_MG_WORK+0, 3*MapControl.zoom*(1+DrawSetting.low_level) );
				
				GD_SetGraphDisp( GRP_MG_WORK+1, ON );
				GD_SetGraphDstMove( GRP_MG_WORK+1, (px-MapControl.px)*MapControl.zoom, (py-MapControl.py)*MapControl.zoom );
				GD_SetGraphRoll( GRP_MG_WORK+1, 45 );
				GD_SetGraphZoom( GRP_MG_WORK+1, 3*MapControl.zoom*(1+DrawSetting.low_level) );

				if( !GD_WaitSpriteCondition(GRP_MG_WORK+1) ){
					for(i=0;i<2;i++){
						GD_ResetGraph( GRP_MG_WORK+i );
						GD_ReleaseSprite( SPR_MG_WORK+i );
					}
					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=3;
					SetBrightBattleMap( 128, 128, 128, 30,0 );
				}
				break;
			case 3:
				for(j=0;j<CHAR_CONTROL_MAX;j++){
					if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
						if(pow(CharControl[j].px-px,2)+pow(CharControl[j].py-py,2) < pow(scope,2) ){
							if( (dir == (int)(((atan2( py-CharControl[j].py, px-CharControl[j].px )/3.141592+1.0)*8+1)/2 + 6)%8)
							 || (dir == (int)(((atan2( py-CharControl[j].py, px-CharControl[j].px )/3.141592+1.0)*8+2)/2 + 6)%8) ){

								dmg += AtackNormalDmg( no, j, 0, MagicParam[mgno].param[0] );

							}
						}
					}
				}
				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+8, TRUE, TRUE, 0, FALSE );

				CharControl[no].sw =  MagicParam[mgno].sng;

				SkillEffectStruct[ii].count=0;
				SkillEffectStruct[ii].step=4;
				break;
			case 4:
				if(SkillEffectStruct[ii].count>30){
					SkillEffectStruct[ii].step=5;
				}
				break;
			case 5:
				break;
			}
			
		}else if( GetMagicNo( MAGIC_SKILL, 0 )==mgno ){
			switch(SkillEffectStruct[ii].step){
			case 0:
				if(DrawSetting.low_level){
					wsprintf( buf, "mgm%03d_low.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+0, buf, ALPHA_4444 );
				}else{
					wsprintf( buf, "mgm%03d.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+0, buf, ALPHA_4444 );
				}
				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+8*7, TRUE, TRUE, 1, FALSE );
				SkillEffectStruct[ii].step=1;
				break;
			case 1:
				if( !GD_WaitSpriteCondition(GRP_MAP_CHAR+no) ){
					GetMagicSound( mgno );

					GD_SetSprite( GRP_MG_WORK+0, SPR_MG_WORK+0, 0, TRUE, LAY_EFFECT+0, TRUE, TRUE, 1, FALSE );
					GD_SetGraphScaleParam( GRP_MG_WORK+0, SCALE_HIGHT );

					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=2;
				}
				break;
			case 2:
				px=px+44;
				py=py-56;
				GD_SetGraphDstMove( GRP_MG_WORK+0, (px-MapControl.px)*MapControl.zoom, (py-MapControl.py)*MapControl.zoom);
				GD_SetGraphZoom( GRP_MG_WORK+0, MapControl.zoom*(1+DrawSetting.low_level)*2 );
				if( !GD_WaitSpriteCondition(GRP_MG_WORK+0) ){
					GD_ResetGraph( GRP_MG_WORK+0 );
					GD_ReleaseSprite( SPR_MG_WORK+0 );
					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=3;
				}
				break;
			case 3:
				{

					for(j=0;j<CHAR_CONTROL_MAX;j++){
						if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
							if(pow(CharControl[j].px-px,2)+pow(CharControl[j].py-py,2) < pow(scope,2) ){
								if( (dir == (int)(((atan2( py-CharControl[j].py, px-CharControl[j].px )/3.141592+1.0)*8+1)/2 + 6)%8)
								 || (dir == (int)(((atan2( py-CharControl[j].py, px-CharControl[j].px )/3.141592+1.0)*8+2)/2 + 6)%8) ){

									dmg += MagicDmageNormalSet(no, j, mgno, &did );
								}
							}
						}
					}
				}

				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+8*2, TRUE, TRUE, 0, FALSE );
				CharControl[no].sw = MagicParam[mgno].sng;

				SkillEffectStruct[ii].count=0;
				SkillEffectStruct[ii].step=4;
				break;
			case 4:
				if(SkillEffectStruct[ii].count>30){
					SkillEffectStruct[ii].step=5;
				}
				break;
			case 5:
				break;
			}
		
		}else if( GetMagicNo( MAGIC_SKILL, 10 )==mgno ){
			switch(SkillEffectStruct[ii].step){
			case 0:
				if(DrawSetting.low_level){
					wsprintf( buf, "mgm%03da_low.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+0, buf, ALPHA_4444 );
					wsprintf( buf, "mgm%03db_low.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+1, buf, ALPHA_4444 );
					wsprintf( buf, "mgm%03dc_low.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+2, buf, ALPHA_4444 );
					wsprintf( buf, "mgm%03de_low.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+3, buf, ALPHA_4444 );

					wsprintf( buf, "mgz000_low.ani" );
					GD_LoadSprite( SPR_MG_WORK+4, buf, ALPHA_4444 );
				}else{
					wsprintf( buf, "mgm%03da.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+0, buf, ALPHA_4444 );
					wsprintf( buf, "mgm%03db.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+1, buf, ALPHA_4444 );
					wsprintf( buf, "mgm%03dc.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+2, buf, ALPHA_4444 );
					wsprintf( buf, "mgm%03de.ani", MagicParam[ mgno ].mno );
					GD_LoadSprite( SPR_MG_WORK+3, buf, ALPHA_4444 );
					wsprintf( buf, "mgz000.ani" );
					GD_LoadSprite( SPR_MG_WORK+4, buf, ALPHA_4444 );
				}
				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+8*6, TRUE, TRUE, 1, FALSE );
				SkillEffectStruct[ii].step=1;
				break;
			case 1:
				if( !GD_WaitSpriteCondition(GRP_MAP_CHAR+no) ){
					GetMagicSound( mgno );

					for(i=0;i<4;i++){
						GD_SetSprite( GRP_MG_WORK+i, SPR_MG_WORK+i, 0, TRUE, LAY_EFFECT+i, OFF, FALSE, 0, FALSE );
						GD_SetGraphScaleParam( GRP_MG_WORK+i, SCALE_HIGHT );
					}

					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=2;
				}
				break;
			case 2:
				if(CharParam[CharControl[no].cno].job==98){
					py=py-64;
				}else{
					py=py-110;
				}

				tx = tx + cos( SkillEffectStruct[ii].count*3.14*2/128 )*64;
				ty = ty + sin( SkillEffectStruct[ii].count*3.14*2/128 )*64;
				GD_SetGraphDisp( GRP_MG_WORK+0, ON );
				GD_SetGraphDstMove( GRP_MG_WORK+0, (px-MapControl.px)*MapControl.zoom, (py-MapControl.py)*MapControl.zoom );
				GD_SetGraphRoll( GRP_MG_WORK+0, 180-D3DXToDegree( atan2(py-ty,px-tx) ) );

				i = min(SkillEffectStruct[ii].count,15);

				GD_SetGraphZoom( GRP_MG_WORK+0, sqrt(pow(py-ty,2)+pow(px-tx,2))*MapControl.zoom*(1+DrawSetting.low_level)/220*i/15 );

				GD_SetGraphDisp( GRP_MG_WORK+1, ON );
				GD_SetGraphDisp( GRP_MG_WORK+3, ON );
				GD_SetGraphDstMove( GRP_MG_WORK+1, (px-MapControl.px)*MapControl.zoom, (py-MapControl.py)*MapControl.zoom);
				GD_SetGraphDstMove( GRP_MG_WORK+3, (px-MapControl.px)*MapControl.zoom, (py-MapControl.py)*MapControl.zoom);

				if( SkillEffectStruct[ii].count%32==31 ){
					SkillEffectStruct[ii].tx2[SkillEffectStruct[ii].count/32] = tx;
					SkillEffectStruct[ii].ty2[SkillEffectStruct[ii].count/32] = ty;

					AVG_PlaySE3(9125,255);
					j = 4+SkillEffectStruct[ii].count/32;
					GD_SetSprite( GRP_MG_WORK+j, SPR_MG_WORK+4, 0, TRUE, LAY_EFFECT+i, TRUE, FALSE, 1, FALSE );
					GD_SetGraphScaleParam( GRP_MG_WORK+i, SCALE_HIGHT );
				}
				for(i=0;i<4;i++){
					GD_SetGraphDstMove( GRP_MG_WORK+4+i, (SkillEffectStruct[ii].tx2[i]-MapControl.px)*MapControl.zoom, (SkillEffectStruct[ii].ty2[i]-MapControl.py)*MapControl.zoom);
					GD_SetGraphZoom( GRP_MG_WORK+4+i, MapControl.zoom*(1+DrawSetting.low_level) );
				}

				if( SkillEffectStruct[ii].count>15 ){
					GD_SetGraphDisp( GRP_MG_WORK+2, ON );
					GD_SetGraphDstMove( GRP_MG_WORK+2, (tx-MapControl.px)*MapControl.zoom, (ty-44-MapControl.py)*MapControl.zoom);
				}

				for(i=1;i<4;i++){
					GD_SetGraphZoom( GRP_MG_WORK+i, MapControl.zoom*(1+DrawSetting.low_level) );
				}
				j = max(SkillEffectStruct[ii].count,96)-96;
				for(i=0;i<8;i++){
					GD_SetGraphAlpha( GRP_MG_WORK+i, 255-255*j/32 );
				}

				if( SkillEffectStruct[ii].count>128 ){
					for(i=0;i<4;i++){
						GD_ResetGraph( GRP_MG_WORK+i );
						GD_ResetGraph( GRP_MG_WORK+4+i );
						GD_ReleaseSprite( SPR_MG_WORK+i );
					}
					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=3;
				}
				break;
			case 3:
				{
					switch(scope_type){
						case RANGE_TAGET:		
						case RANGE_SELF:		
							dmg += AtackNormalDmg( no, tno, 0, MagicParam[mgno].param[0] );

							lv = CharParam[CharControl[tno].cno].lv;
							break;
						case RANGE_SCOPE:		
						case RANGE_SELF_SCOPE:	
							for(j=0;j<CHAR_CONTROL_MAX;j++){
								if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
									if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
										
										dmg += AtackNormalDmg( no, j, 0, MagicParam[mgno].param[0] );

										if( lv < CharParam[CharControl[j].cno].lv ){
											lv = CharParam[CharControl[j].cno].lv;
										}
									}
								}
							}
							break;
						case RANGE_LINE:		
						case RANGE_FAN:			
						case RANGE_ALL:			
							break;
					}
				}

				CharControl[no].sw =  MagicParam[mgno].sng;

				SkillEffectStruct[ii].count=0;
				SkillEffectStruct[ii].step=4;
				break;
			case 4:
				if(SkillEffectStruct[ii].count>30){
					SkillEffectStruct[ii].step=5;
				}
				break;
			case 5:
				break;
			}
		}else if(  (GetMagicNo( MAGIC_SKILL, 903 )==mgno)	


				|| (GetMagicNo( MAGIC_SKILL, 902 )==mgno) ){

			int	cnt_x1;
			int	cnt_x2;
			float	xx;
			static int	cpno;
			static int	lr_flag=0;

			int	hanma = GetMagicNo( MAGIC_SKILL, 902 )==mgno;

			switch(SkillEffectStruct[ii].step){
			case 0:


				if(hanma){
					SetMapCharSkill( no, -1, tx, ty );
				}else{
					double	radian;
					radian = atan2( ty-CharControl[no].py, tx-CharControl[no].px );
					SetCharDir( no, radian );
					GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+16, TRUE, TRUE, 0, FALSE );
				}
				cpno = Change_Jno2Pno( CharParam[CharControl[no].cno].job );
				if( cpno!=-1 ){
					AVG_PlayBTVoice( 0, cpno );
					switch(CharParam[CharControl[no].cno].job){
						case 12: 
						case 17: 
						case 15: 
						case 20: 
							lr_flag=0;
							break;

						case 13: 
						case 18: 
						case 19: 
						case 29: 
							lr_flag=1;
							break;
					}

					wsprintf( buf, "k%02d00.tga", cpno );	GD_LoadTexture( BMP_MG_EFFECT+0, buf, ALPHA_1555 );
					wsprintf( buf, "k%02d01.tga", cpno );	GD_LoadTexture( BMP_MG_EFFECT+1, buf, ALPHA_1555 );
					wsprintf( buf, "k%02d02.tga", cpno );	GD_LoadTexture( BMP_MG_EFFECT+2, buf, ALPHA_1555 );

					GD_SetGraph( GRP_MG_WORK+0, BMP_MG_EFFECT+0, LAY_EFFECT+2, TRUE );
					GD_SetGraph( GRP_MG_WORK+1, BMP_MG_EFFECT+1, LAY_EFFECT+1, TRUE );
					GD_SetGraph( GRP_MG_WORK+2, BMP_MG_EFFECT+1, LAY_EFFECT+1, TRUE );
					GD_SetGraph( GRP_MG_WORK+3, BMP_MG_EFFECT+2, LAY_EFFECT+0, TRUE );
					GD_SetGraph( GRP_MG_WORK+4, BMP_MG_EFFECT+2, LAY_EFFECT+0, TRUE );

					GD_SetGraphSrcRect( GRP_MG_WORK+1, 0, 0, 512, 230 );
					GD_SetGraphSrcRect( GRP_MG_WORK+1, 0, 0, 512, 230 );
					GD_SetGraphSrcRect( GRP_MG_WORK+2, 0, 0, 512, 230 );
					GD_SetGraphSrcRect( GRP_MG_WORK+3, 0, 0, 512, 236 );
					GD_SetGraphSrcRect( GRP_MG_WORK+4, 0, 0, 512, 236 );

					GD_SetGraphDstMove( GRP_MG_WORK+0, -872,     32 );
					GD_SetGraphDstRect( GRP_MG_WORK+1, 0,     32, 800, 230 );
					GD_SetGraphDstRect( GRP_MG_WORK+2, 0+800, 32, 800, 230 );
					GD_SetGraphDstRect( GRP_MG_WORK+3, 0,     32-3, 800, 236 );
					GD_SetGraphDstRect( GRP_MG_WORK+4, 0+800, 32-3, 800, 236 );

					GD_SetGraphBlendParam( GRP_MG_WORK+0, BLEND_ADD );
					
					GD_SetGraphBlendParam( GRP_MG_WORK+1, BLEND_ADD );
					GD_SetGraphScaleParam( GRP_MG_WORK+1, SCALE_HIGHT );
					GD_SetGraphBlendParam( GRP_MG_WORK+2, BLEND_ADD );
					GD_SetGraphScaleParam( GRP_MG_WORK+2, SCALE_HIGHT );

					GD_SetGraphBlendParam( GRP_MG_WORK+3, BLEND_ALPHA );
					GD_SetGraphScaleParam( GRP_MG_WORK+3, SCALE_HIGHT );
					GD_SetGraphAlpha( GRP_MG_WORK+3, 192 );

					GD_SetGraphBlendParam( GRP_MG_WORK+4, BLEND_ALPHA );
					GD_SetGraphScaleParam( GRP_MG_WORK+4, SCALE_HIGHT );
					GD_SetGraphAlpha( GRP_MG_WORK+4, 192 );

					if(hanma){
						SetScrolBattleMap( tx, ty-128, 1.0f, 15, 2 );
					}else{
						SetScrolBattleMap( tx, ty, 1.0f, 15, 2 );
					}

					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=1;
				}else{
					SkillEffectStruct[ii].count=60;
					SkillEffectStruct[ii].step=2;

					if(hanma){
						GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR2+CharControl[no].cno, 0, TRUE, TRUE, 1, FALSE );
					}else{
						GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+24, TRUE, TRUE, 0, FALSE );
					}
				}
				AVG_PlaySE3( 5901, 255 );

				break;
			case 1:
				if(lr_flag){
					xx = 60-SkillEffectStruct[ii].count;
					xx = -72+(872*xx*xx/(60*60));
				}else{
					xx = 60-SkillEffectStruct[ii].count;
					xx = -(872*xx*xx/(60*60));
				}
				GD_SetGraphDstMove( GRP_MG_WORK+0, xx,     32 );

				cnt_x1 = SkillEffectStruct[ii].count*6;
				cnt_x2 = SkillEffectStruct[ii].count*8;

				GD_SetGraphDstMove( GRP_MG_WORK+1, -cnt_x1%800,     32 );
				GD_SetGraphDstMove( GRP_MG_WORK+2, -cnt_x1%800+800, 32 );
				GD_SetGraphDstMove( GRP_MG_WORK+3, -cnt_x2%800,     32-3 );
				GD_SetGraphDstMove( GRP_MG_WORK+4, -cnt_x2%800+800, 32-3 );


				GD_SetGraphAlpha( GRP_MG_WORK+1, 255*SkillEffectStruct[ii].count/60 );
				GD_SetGraphAlpha( GRP_MG_WORK+2, 255*SkillEffectStruct[ii].count/60 );
				GD_SetGraphAlpha( GRP_MG_WORK+3, 192*SkillEffectStruct[ii].count/60 );
				GD_SetGraphAlpha( GRP_MG_WORK+4, 192*SkillEffectStruct[ii].count/60 );

				if(SkillEffectStruct[ii].count>60){
					SkillEffectStruct[ii].step=2;
					if(hanma){
						GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR2+CharControl[no].cno, 0, TRUE, TRUE, 1, FALSE );
					}else{
						GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+24, TRUE, TRUE, 0, FALSE );
					}
				}
				break;
			case 2:
				if(cpno!=-1){

					cnt_x1 = SkillEffectStruct[ii].count*6;
					cnt_x2 = SkillEffectStruct[ii].count*8;

					GD_SetGraphDstMove( GRP_MG_WORK+1, -cnt_x1%800,     32 );
					GD_SetGraphDstMove( GRP_MG_WORK+2, -cnt_x1%800+800, 32 );
					GD_SetGraphDstMove( GRP_MG_WORK+3, -cnt_x2%800,     32-3 );
					GD_SetGraphDstMove( GRP_MG_WORK+4, -cnt_x2%800+800, 32-3 );

					if(SkillEffectStruct[ii].count>30+60){
						xx = SkillEffectStruct[ii].count-(30+60);

						GD_SetGraphAlpha( GRP_MG_WORK+0, 255*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+1, 255*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+2, 255*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+3, 192*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+4, 192*(60-xx)/60 );
					}
				}
				if(hanma){
					switch(SkillEffectStruct[ii].count){
					case  4*7+60:
						for(j=0;j<CHAR_CONTROL_MAX;j++){
							if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
								if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
									if(no!=j){
										dmg += AtackNormalDmg( no, j, 0, MagicParam[mgno].param[0]-400 );
										if( lv < CharParam[CharControl[j].cno].lv ){
											lv = CharParam[CharControl[j].cno].lv;
										}
									}
								}
							}
						}
						if( CharParam[CharControl[no].cno].AddExpAll( lv, FALSE ) ){
							SetHitAction( no, 0, 0.0f, HIT_LVUP );
						}
						break;
					case 10*7+60:
						for(j=0;j<CHAR_CONTROL_MAX;j++){
							if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
								if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
									if(no!=j){
										dmg += AtackNormalDmg( no, j, 0, MagicParam[mgno].param[0]-200 );
										if( lv < CharParam[CharControl[j].cno].lv ){
											lv = CharParam[CharControl[j].cno].lv;
										}
									}
								}
							}
						}
						if( CharParam[CharControl[no].cno].AddExpAll( lv, FALSE ) ){
							SetHitAction( no, 0, 0.0f, HIT_LVUP );
						}
						break;
					case 16*7+60:
						for(j=0;j<CHAR_CONTROL_MAX;j++){
							if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
								if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
									if(no!=j){
										dmg += AtackNormalDmg( no, j, 0, MagicParam[mgno].param[0] );
										if( lv < CharParam[CharControl[j].cno].lv ){
											lv = CharParam[CharControl[j].cno].lv;
										}
									}
								}
							}
						}
						if( CharParam[CharControl[no].cno].AddExpAll( lv, FALSE ) ){
							SetHitAction( no, 0, 0.0f, HIT_LVUP );
						}

						break;
					}
				}else{
					int	k,r;
					switch(SkillEffectStruct[ii].count){
					case  12+60:
					case  12+60+30*1:
					case  12+60+30*2:
					case  12+60+30*3:
					case  12+60+30*4:
						k=0;
						for(j=0;j<CHAR_CONTROL_MAX;j++){
							if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
								if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
									if(no!=j){
										k++;
									}
								}
							}
						}
						if(k>0){
							r=rand()%k;
							k=0;
							for(j=0;j<CHAR_CONTROL_MAX;j++){
								if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
									if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
										if(no!=j){
											if(r==k){
												SetArrowAction( no, j, MagicParam[mgno].param[0] );
												break;
											}
											k++;
										}
									}
								}
							}
						}
						break;
					}
				}

				if( (!GD_WaitSpriteCondition(GRP_MAP_CHAR+no) && hanma) || (SkillEffectStruct[ii].count>12+60+30*4 && !hanma) ){
					if(cpno!=-1){
						for(j=0;j<5;j++){
							GD_ResetGraph( GRP_MG_WORK+j );
						}
						for(j=0;j<3;j++){
							GD_ReleaseTexture( BMP_MG_EFFECT+j );
						}
					}

					CharControl[no].sw =  MagicParam[mgno].sng;
					GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+16, TRUE, TRUE, 1, FALSE );
					GD_ReleaseSprite( SPR_MAP_CHAR2+CharControl[no].cno );
					GD_SetGraphRevParam( GRP_MAP_CHAR+no, REVERSE_NORMAL );

					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=3;
				}
				break;
			case 3:
				if(hanma){
					if( SkillEffectStruct[ii].count>30 ){
						SkillEffectStruct[ii].step=5;
					}
				}else{
					if( SkillEffectStruct[ii].count>60 ){
						SkillEffectStruct[ii].step=5;
					}
				}
				break;
			case 5:
				break;
			}
		}else if( GetMagicNo( MAGIC_SKILL, 901 )==mgno ){


			int	cnt_x1;
			int	cnt_x2;
			float	xx;
			static int	cpno;
			static int	lr_flag=0;
			switch(SkillEffectStruct[ii].step){
			case 0:


				SetMapCharSkill( no, -2,0,0 );
				cpno = Change_Jno2Pno( CharParam[CharControl[no].cno].job );
				if( cpno!=-1 ){
					AVG_PlayBTVoice( 0, cpno );
					switch(CharParam[CharControl[no].cno].job){
						case 12: 
						case 17: 
						case 15: 
						case 20: 
							lr_flag=0;
							break;

						case 13: 
						case 18: 
						case 19: 
						case 29: 
							lr_flag=1;
							break;
					}

					wsprintf( buf, "k%02d00.tga", cpno );	GD_LoadTexture( BMP_MG_EFFECT+0, buf, ALPHA_1555 );
					wsprintf( buf, "k%02d01.tga", cpno );	GD_LoadTexture( BMP_MG_EFFECT+1, buf, ALPHA_1555 );
					wsprintf( buf, "k%02d02.tga", cpno );	GD_LoadTexture( BMP_MG_EFFECT+2, buf, ALPHA_1555 );

					GD_SetGraph( GRP_MG_WORK+0, BMP_MG_EFFECT+0, LAY_EFFECT+2, TRUE );
					GD_SetGraph( GRP_MG_WORK+1, BMP_MG_EFFECT+1, LAY_EFFECT+1, TRUE );
					GD_SetGraph( GRP_MG_WORK+2, BMP_MG_EFFECT+1, LAY_EFFECT+1, TRUE );
					GD_SetGraph( GRP_MG_WORK+3, BMP_MG_EFFECT+2, LAY_EFFECT+0, TRUE );
					GD_SetGraph( GRP_MG_WORK+4, BMP_MG_EFFECT+2, LAY_EFFECT+0, TRUE );

					GD_SetGraphSrcRect( GRP_MG_WORK+1, 0, 0, 512, 230 );
					GD_SetGraphSrcRect( GRP_MG_WORK+1, 0, 0, 512, 230 );
					GD_SetGraphSrcRect( GRP_MG_WORK+2, 0, 0, 512, 230 );
					GD_SetGraphSrcRect( GRP_MG_WORK+3, 0, 0, 512, 236 );
					GD_SetGraphSrcRect( GRP_MG_WORK+4, 0, 0, 512, 236 );

					GD_SetGraphDstMove( GRP_MG_WORK+0, -872,     32 );
					GD_SetGraphDstRect( GRP_MG_WORK+1, 0,     32, 800, 230 );
					GD_SetGraphDstRect( GRP_MG_WORK+2, 0+800, 32, 800, 230 );
					GD_SetGraphDstRect( GRP_MG_WORK+3, 0,     32-3, 800, 236 );
					GD_SetGraphDstRect( GRP_MG_WORK+4, 0+800, 32-3, 800, 236 );

					GD_SetGraphBlendParam( GRP_MG_WORK+0, BLEND_ADD );
					
					GD_SetGraphBlendParam( GRP_MG_WORK+1, BLEND_ADD );
					GD_SetGraphScaleParam( GRP_MG_WORK+1, SCALE_HIGHT );
					GD_SetGraphBlendParam( GRP_MG_WORK+2, BLEND_ADD );
					GD_SetGraphScaleParam( GRP_MG_WORK+2, SCALE_HIGHT );

					GD_SetGraphBlendParam( GRP_MG_WORK+3, BLEND_ALPHA );
					GD_SetGraphScaleParam( GRP_MG_WORK+3, SCALE_HIGHT );
					GD_SetGraphAlpha( GRP_MG_WORK+3, 192 );

					GD_SetGraphBlendParam( GRP_MG_WORK+4, BLEND_ALPHA );
					GD_SetGraphScaleParam( GRP_MG_WORK+4, SCALE_HIGHT );
					GD_SetGraphAlpha( GRP_MG_WORK+4, 192 );

					SetScrolBattleMap( CharControl[no].px, CharControl[no].py-128, 1.0f, 15, 2 );

					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=1;
				}else{
					SkillEffectStruct[ii].count=60;
					SkillEffectStruct[ii].step=2;
					GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR2+CharControl[no].cno, 0, TRUE, TRUE, 1, FALSE );
				}
				AVG_PlaySE3( 5901, 255 );

				break;
			case 1:
				if(lr_flag){
					xx = 60-SkillEffectStruct[ii].count;
					xx = -72+(872*xx*xx/(60*60));
				}else{
					xx = 60-SkillEffectStruct[ii].count;
					xx = -(872*xx*xx/(60*60));
				}
				GD_SetGraphDstMove( GRP_MG_WORK+0, xx,     32 );

				cnt_x1 = SkillEffectStruct[ii].count*6;
				cnt_x2 = SkillEffectStruct[ii].count*8;

				GD_SetGraphDstMove( GRP_MG_WORK+1, -cnt_x1%800,     32 );
				GD_SetGraphDstMove( GRP_MG_WORK+2, -cnt_x1%800+800, 32 );
				GD_SetGraphDstMove( GRP_MG_WORK+3, -cnt_x2%800,     32-3 );
				GD_SetGraphDstMove( GRP_MG_WORK+4, -cnt_x2%800+800, 32-3 );


				GD_SetGraphAlpha( GRP_MG_WORK+1, 255*SkillEffectStruct[ii].count/60 );
				GD_SetGraphAlpha( GRP_MG_WORK+2, 255*SkillEffectStruct[ii].count/60 );
				GD_SetGraphAlpha( GRP_MG_WORK+3, 192*SkillEffectStruct[ii].count/60 );
				GD_SetGraphAlpha( GRP_MG_WORK+4, 192*SkillEffectStruct[ii].count/60 );

				if(SkillEffectStruct[ii].count>60){
					SkillEffectStruct[ii].step=2;
					GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR2+CharControl[no].cno, 0, TRUE, TRUE, 1, FALSE );
				}
				break;
			case 2:
				if(cpno!=-1){

					cnt_x1 = SkillEffectStruct[ii].count*6;
					cnt_x2 = SkillEffectStruct[ii].count*8;

					GD_SetGraphDstMove( GRP_MG_WORK+1, -cnt_x1%800,     32 );
					GD_SetGraphDstMove( GRP_MG_WORK+2, -cnt_x1%800+800, 32 );
					GD_SetGraphDstMove( GRP_MG_WORK+3, -cnt_x2%800,     32-3 );
					GD_SetGraphDstMove( GRP_MG_WORK+4, -cnt_x2%800+800, 32-3 );

					if(SkillEffectStruct[ii].count>60){
						xx = SkillEffectStruct[ii].count-60;

						GD_SetGraphAlpha( GRP_MG_WORK+0, 255*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+1, 255*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+2, 255*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+3, 192*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+4, 192*(60-xx)/60 );
					}
				}
				if( !GD_WaitSpriteCondition(GRP_MAP_CHAR+no) ){
					if(cpno!=-1){
						for(j=0;j<5;j++){
							GD_ResetGraph( GRP_MG_WORK+j );
						}
						for(j=0;j<3;j++){
							GD_ReleaseTexture( BMP_MG_EFFECT+j );
						}
					}

					GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+16, TRUE, TRUE, 1, FALSE );
					GD_ReleaseSprite( SPR_MAP_CHAR2+CharControl[no].cno );
					GD_SetGraphRevParam( GRP_MAP_CHAR+no, REVERSE_NORMAL );
					SkillEffectStruct[ii].step=3;
				}
				break;
			case 3:
				for(j=0;j<CHAR_CONTROL_MAX;j++){
					if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
						if(pow(CharControl[j].px-px,2)+pow(CharControl[j].py-py,2) < pow(scope,2) ){
							if(no!=j){
								dmg += AtackNormalDmg( no, j, 0, MagicParam[mgno].param[0] );
								lv += CharParam[CharControl[no].cno].AddExpAll( CharParam[CharControl[j].cno].lv, FALSE );
							}
						}
					}
				}
				if( lv ){
					SetHitAction( no, 0, 0.0f, HIT_LVUP );
				}
				CharControl[no].sw =  MagicParam[mgno].sng;
				SkillEffectStruct[ii].count=0;
				SkillEffectStruct[ii].step=4;
				break;
			case 4:
				if( SkillEffectStruct[ii].count>30 ){
					SkillEffectStruct[ii].step=5;
				}
				break;
			case 5:
				break;
			}
		}else if( GetMagicNo( MAGIC_SKILL, 900 )==mgno ){


			int	cnt_x1;
			int	cnt_x2;
			float	xx;
			static int	cpno;
			static int	lr_flag=0;
			switch(SkillEffectStruct[ii].step){
			case 0:

	

				SetMapCharSkill( no, tno,0,0 );
				cpno = Change_Jno2Pno( CharParam[CharControl[no].cno].job );
				if( cpno!=-1 ){
					AVG_PlayBTVoice( 0, cpno );
					switch(CharParam[CharControl[no].cno].job){
						case 12: 
						case 17: 
						case 15: 
						case 20: 
							lr_flag=0;
							break;

						case 13: 
						case 18: 
						case 19: 
						case 29: 
							lr_flag=1;
							break;
					}

					wsprintf( buf, "k%02d00.tga", cpno );	GD_LoadTexture( BMP_MG_EFFECT+0, buf, ALPHA_1555 );
					wsprintf( buf, "k%02d01.tga", cpno );	GD_LoadTexture( BMP_MG_EFFECT+1, buf, ALPHA_1555 );
					wsprintf( buf, "k%02d02.tga", cpno );	GD_LoadTexture( BMP_MG_EFFECT+2, buf, ALPHA_1555 );

					GD_SetGraph( GRP_MG_WORK+0, BMP_MG_EFFECT+0, LAY_EFFECT+2, TRUE );
					GD_SetGraph( GRP_MG_WORK+1, BMP_MG_EFFECT+1, LAY_EFFECT+1, TRUE );
					GD_SetGraph( GRP_MG_WORK+2, BMP_MG_EFFECT+1, LAY_EFFECT+1, TRUE );
					GD_SetGraph( GRP_MG_WORK+3, BMP_MG_EFFECT+2, LAY_EFFECT+0, TRUE );
					GD_SetGraph( GRP_MG_WORK+4, BMP_MG_EFFECT+2, LAY_EFFECT+0, TRUE );

					GD_SetGraphSrcRect( GRP_MG_WORK+1, 0, 0, 512, 230 );
					GD_SetGraphSrcRect( GRP_MG_WORK+1, 0, 0, 512, 230 );
					GD_SetGraphSrcRect( GRP_MG_WORK+2, 0, 0, 512, 230 );
					GD_SetGraphSrcRect( GRP_MG_WORK+3, 0, 0, 512, 236 );
					GD_SetGraphSrcRect( GRP_MG_WORK+4, 0, 0, 512, 236 );

					GD_SetGraphDstMove( GRP_MG_WORK+0, -872,     32 );
					GD_SetGraphDstRect( GRP_MG_WORK+1, 0,     32, 800, 230 );
					GD_SetGraphDstRect( GRP_MG_WORK+2, 0+800, 32, 800, 230 );
					GD_SetGraphDstRect( GRP_MG_WORK+3, 0,     32-3, 800, 236 );
					GD_SetGraphDstRect( GRP_MG_WORK+4, 0+800, 32-3, 800, 236 );

					GD_SetGraphBlendParam( GRP_MG_WORK+0, BLEND_ADD );
					
					GD_SetGraphBlendParam( GRP_MG_WORK+1, BLEND_ADD );
					GD_SetGraphScaleParam( GRP_MG_WORK+1, SCALE_HIGHT );
					GD_SetGraphBlendParam( GRP_MG_WORK+2, BLEND_ADD );
					GD_SetGraphScaleParam( GRP_MG_WORK+2, SCALE_HIGHT );

					GD_SetGraphBlendParam( GRP_MG_WORK+3, BLEND_ALPHA );
					GD_SetGraphScaleParam( GRP_MG_WORK+3, SCALE_HIGHT );
					GD_SetGraphAlpha( GRP_MG_WORK+3, 192 );

					GD_SetGraphBlendParam( GRP_MG_WORK+4, BLEND_ALPHA );
					GD_SetGraphScaleParam( GRP_MG_WORK+4, SCALE_HIGHT );
					GD_SetGraphAlpha( GRP_MG_WORK+4, 192 );

					SetScrolBattleMap( CharControl[tno].px, CharControl[tno].py-128, 1.0f, 15, 2 );

					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=1;
				}else{
					SkillEffectStruct[ii].count=60;
					SkillEffectStruct[ii].step=2;
					GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR2+CharControl[no].cno, 0, TRUE, TRUE, 1, FALSE );
				}
				AVG_PlaySE3( 5901, 255 );

				break;
			case 1:
				if(lr_flag){
					xx = 60-SkillEffectStruct[ii].count;
					xx = -72+(872*xx*xx/(60*60));
				}else{
					xx = 60-SkillEffectStruct[ii].count;
					xx = -(872*xx*xx/(60*60));
				}
				GD_SetGraphDstMove( GRP_MG_WORK+0, xx,     32 );

				cnt_x1 = SkillEffectStruct[ii].count*6;
				cnt_x2 = SkillEffectStruct[ii].count*8;

				GD_SetGraphDstMove( GRP_MG_WORK+1, -cnt_x1%800,     32 );
				GD_SetGraphDstMove( GRP_MG_WORK+2, -cnt_x1%800+800, 32 );
				GD_SetGraphDstMove( GRP_MG_WORK+3, -cnt_x2%800,     32-3 );
				GD_SetGraphDstMove( GRP_MG_WORK+4, -cnt_x2%800+800, 32-3 );


				GD_SetGraphAlpha( GRP_MG_WORK+1, 255*SkillEffectStruct[ii].count/60 );
				GD_SetGraphAlpha( GRP_MG_WORK+2, 255*SkillEffectStruct[ii].count/60 );
				GD_SetGraphAlpha( GRP_MG_WORK+3, 192*SkillEffectStruct[ii].count/60 );
				GD_SetGraphAlpha( GRP_MG_WORK+4, 192*SkillEffectStruct[ii].count/60 );

				if(SkillEffectStruct[ii].count>60){
					SkillEffectStruct[ii].step=2;
					GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR2+CharControl[no].cno, 0, TRUE, TRUE, 1, FALSE );
				}
				break;
			case 2:
				if(cpno!=-1){

					cnt_x1 = SkillEffectStruct[ii].count*6;
					cnt_x2 = SkillEffectStruct[ii].count*8;

					GD_SetGraphDstMove( GRP_MG_WORK+1, -cnt_x1%800,     32 );
					GD_SetGraphDstMove( GRP_MG_WORK+2, -cnt_x1%800+800, 32 );
					GD_SetGraphDstMove( GRP_MG_WORK+3, -cnt_x2%800,     32-3 );
					GD_SetGraphDstMove( GRP_MG_WORK+4, -cnt_x2%800+800, 32-3 );

					if(SkillEffectStruct[ii].count>30+60){
						xx = SkillEffectStruct[ii].count-(30+60);

						GD_SetGraphAlpha( GRP_MG_WORK+0, 255*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+1, 255*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+2, 255*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+3, 192*(60-xx)/60 );
						GD_SetGraphAlpha( GRP_MG_WORK+4, 192*(60-xx)/60 );
					}
				}
				dmg=0;
				switch(SkillEffectStruct[ii].count){
				case  7*7+60:	dmg = AtackNormalDmg( no, tno, 0, MagicParam[mgno].param[0]-600 );	break;
				case 11*7+60:	dmg = AtackNormalDmg( no, tno, 0, MagicParam[mgno].param[0]-400 );	break;
				case 14*7+60:	dmg = AtackNormalDmg( no, tno, 0, MagicParam[mgno].param[0]-200 );	break;
				case 22*7+60:	dmg = AtackNormalDmg( no, tno, 0, MagicParam[mgno].param[0]     );	break;
				}
				if(dmg){
					if( CharParam[CharControl[no].cno].AddExpAll( CharParam[CharControl[tno].cno].lv, FALSE ) ){
						SetHitAction( no, 0, 0.0f, HIT_LVUP );
					}
				}

				if( !GD_WaitSpriteCondition(GRP_MAP_CHAR+no) ){
					if(cpno!=-1){
						for(j=0;j<5;j++){
							GD_ResetGraph( GRP_MG_WORK+j );
						}
						for(j=0;j<3;j++){
							GD_ReleaseTexture( BMP_MG_EFFECT+j );
						}
					}

					CharControl[no].sw =  MagicParam[mgno].sng;
					GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+16, TRUE, TRUE, 1, FALSE );
					GD_ReleaseSprite( SPR_MAP_CHAR2+CharControl[no].cno );
					GD_SetGraphRevParam( GRP_MAP_CHAR+no, REVERSE_NORMAL );
					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=3;
				}
				break;
			case 3:
				if( SkillEffectStruct[ii].count>30 ){
					SkillEffectStruct[ii].step=5;
				}
				break;
			case 5:
				break;
			}
		}else{
			int	work_param1;
			int	work_param2;
			switch(SkillEffectStruct[ii].step){
			case 0:

				AVG_PlaySE3( 5901, 255 );

				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+7*8, TRUE, TRUE, 1, FALSE );
				SkillEffectStruct[ii].step=1;
				break;
			case 1:
				if( !GD_WaitSpriteCondition(GRP_MAP_CHAR+no) ){
					SkillEffectStruct[ii].count=0;
					SkillEffectStruct[ii].step=2;
				}
				break;
			case 2:
				SkillEffectStruct[ii].count=0;
				SkillEffectStruct[ii].step=3;
				break;
			case 3:
				switch(scope_type){
					case RANGE_TAGET:		
					case RANGE_SELF:		
						switch( MagicParam[mgno].param_type[0]){
							case MG_TYPE_HEAL:
								MagicRestNormalSet( no, tno, mgno );
								break;
							case MG_TYPE_ATK:
							case MG_TYPE_DEF:
							case MG_TYPE_AVD:
							case MG_TYPE_HIT:
							case MG_TYPE_MOV:
							case MG_TYPE_WSP:

							case SK_DEF_HP_0:	
							case SK_DEF_MP_0:	
							case SK_DEF_HANGEKI:
							case SK_ITEM_BAI:	
								switch( MagicParam[mgno].param_type[0] ){
									case MG_TYPE_ATK:	work_param1 = STATUS_EFF_ATK;	break;
									case MG_TYPE_MGK:	work_param1 = STATUS_EFF_MGK;	break;
									case MG_TYPE_DEF:	work_param1 = STATUS_EFF_DEF;	break;
									case MG_TYPE_RGS:	work_param1 = STATUS_EFF_RGS;	break;
									case MG_TYPE_WSP:	work_param1 = STATUS_EFF_WSP;	break;
									case MG_TYPE_MOV:	work_param1 = STATUS_EFF_MOV;	break;

									case MG_TYPE_AVD:	work_param1 = STATUS_EFF_AVD;	break;
									case MG_TYPE_HIT:	work_param1 = STATUS_EFF_HIT;	break;

									case SK_DEF_HP_0:	work_param1 = STATUS_DEF_HP_0;		break;
									case SK_DEF_MP_0:	work_param1 = STATUS_DEF_MP_0;		break;
									case SK_DEF_HANGEKI:work_param1 = STATUS_DEF_HANGEKI;	break;
									case SK_ITEM_BAI:	work_param1 = STATUS_ITEM_BAI;		break;
								}
								work_param2 = MagicParam[mgno].param[0];
								MagicStatusEffctSet( no, tno, work_param1, work_param2 );
								break;

							case SK_TOUHATU:	
								SetMapCharThinkTouhatu( tno, no );
								break;
						}
						lv = CharParam[CharControl[tno].cno].lv;
						break;
					case RANGE_SCOPE:		
					case RANGE_SELF_SCOPE:	
						for(j=0;j<CHAR_CONTROL_MAX;j++){
							if( CharControl[j].flag && !CharParam[CharControl[j].cno].did ){
								if(pow(CharControl[j].px-tx,2)+pow(CharControl[j].py-ty,2) < pow(scope,2) ){
									if( MagicParam[mgno].param_type[0]==SK_TOUHATU){
										SetMapCharThinkTouhatu( j, no );
									}
									if( lv < CharParam[CharControl[j].cno].lv ){
										lv = CharParam[CharControl[j].cno].lv;
									}
								}
							}
						}
						break;
					case RANGE_ALL:
						AllAtackMapCharThink();
						lv = CharParam[CharControl[no].cno].lv;
						break;
				}
				if( CharParam[CharControl[no].cno].AddExpAll( lv, FALSE ) ){
					SetHitAction( no, 0, 0.0f, HIT_LVUP );
				}

				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+8, TRUE, TRUE, 1, FALSE );
				CharControl[no].sw =  MagicParam[mgno].sng;

				SkillEffectStruct[ii].count=0;
				SkillEffectStruct[ii].step=4;
				break;
			case 4:
				if(SkillEffectStruct[ii].count>30){
					SkillEffectStruct[ii].step=5;
				}
				break;
			case 5:
				break;
			}
		}
		if(char_active){
			SkillEffectStruct[ii].count++;
		}
	}
}
	return 1;
}


typedef struct{
	int		flag;

	BYTE	step;

	short	no;
	short	ino;

	short	tno;

	int		count;
}ITEM_EFFECT_STRUCT;

#define ITEM_EFFECT_MAX	32
ITEM_EFFECT_STRUCT	ItemEffectStruct[ITEM_EFFECT_MAX];


void SetItemAction( int no )
{
	int		i,j,k;
	for(i=0;i<ITEM_EFFECT_MAX;i++){
		if(!ItemEffectStruct[i].flag){
			break;
		}
	}
	j=0;
	if(i==ITEM_EFFECT_MAX) return ;

	ItemEffectStruct[i].flag  = ON;
	ItemEffectStruct[i].step  = 0;
	ItemEffectStruct[i].count = 0;
	ItemEffectStruct[i].no    = no;
	ItemEffectStruct[i].ino   = CharControl[no].ino;
	ItemEffectStruct[i].tno   = CharControl[no].tcno;

	ITM_LostItem( CharControl[no].ino, 1 );

}
void ResetItemAction( int no )
{
	int		i,j,k;
	for(i=0;i<ITEM_EFFECT_MAX;i++){
		if(ItemEffectStruct[i].flag){
			if(ItemEffectStruct[i].no==no){
				GD_ResetGraph( GRP_ITEM_WORK+0 );
				GD_ReleaseSprite( SPR_ITEM_WORK+0 );
				ZeroMemory( &ItemEffectStruct[i], sizeof(ITEM_EFFECT_STRUCT) );
				break;
			}
		}
	}
}
BOOL WaitItemAction( int no )
{
	int		i;
	for(i=0;i<ITEM_EFFECT_MAX;i++){
		if(ItemEffectStruct[i].flag){
			if(ItemEffectStruct[i].no==no){
				return ItemEffectStruct[i].step==4;
			}
		}
	}
	return TRUE;
}
void ResetMagicActionAll2(void)
{
	int		i,j;
	GD_ResetGraph( GRP_ITEM_WORK+0 );
	GD_ReleaseSprite( SPR_ITEM_WORK+0 );

	for(j=0;j<5;j++){
		GD_ResetGraph( GRP_MG_WORK+j );
	}
	for(j=0;j<3;j++){
		GD_ReleaseTexture( BMP_MG_EFFECT+j );
	}
	for(i=0;i<8;i++){
		GD_ReleaseSprite( SPR_MG_WORK+i );
	}

	for(i=0;i<ITEM_EFFECT_MAX;i++){
		ZeroMemory( &ItemEffectStruct[i], sizeof(ITEM_EFFECT_STRUCT) );
	}

	for( i=0 ; i<SKILL_MAX ; i++ ){
		ZeroMemory( &SkillEffectStruct[i], sizeof(SKILL_EFFECT_STRUCT) );
	}
}

int RenewItemAction( int char_active )
{
	int		no, tno;
	float	tx,ty, px,py;
	int		i,j;
	char	buf[256];
	char	buf2[256];

	int	ii;
	for( ii=0 ; ii<SKILL_MAX ; ii++ ){
		if(ItemEffectStruct[ii].flag){
			no  = ItemEffectStruct[ii].no;
			tno = ItemEffectStruct[ii].tno;
			CHAR_PARAM	*cp = &CharParam[CharControl[no].cno];

			tx  = CharControl[ tno ].px;
			ty  = CharControl[ tno ].py;
			px = CharControl[ no ].px;
			py = CharControl[ no ].py;

			int	ino = ItemEffectStruct[ii].ino;

			int	dmg,lv=0;
			int	dir = CharControl[no].dir;

		
			switch(ItemEffectStruct[ii].step){
			case 0:
	
				wsprintf( buf, "item000.ani" );
				GD_LoadSprite( SPR_ITEM_WORK+0, buf, ALPHA_4444 );

				GD_SetSprite( GRP_ITEM_WORK+0, SPR_ITEM_WORK+0, 0, TRUE, LAY_EFFECT, TRUE, FALSE, 1, FALSE );
				GD_SetGraphScaleParam( GRP_ITEM_WORK+0, SCALE_HIGHT );
				GD_SetGraphDstMove( GRP_ITEM_WORK+0, (tx-MapControl.px)*MapControl.zoom, (ty-MapControl.py)*MapControl.zoom );
				GD_SetGraphZoom( GRP_ITEM_WORK+0, MapControl.zoom*(1+DrawSetting.low_level) );

				AVG_PlaySE3( 8100, 255 );
				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, dir+7*8, TRUE, TRUE, 1, FALSE );
				ItemEffectStruct[ii].step=1;
				break;
			case 1:
				GD_SetGraphDstMove( GRP_ITEM_WORK+0, (tx-MapControl.px)*MapControl.zoom, (ty-MapControl.py)*MapControl.zoom );
				GD_SetGraphZoom( GRP_ITEM_WORK+0, MapControl.zoom*(1+DrawSetting.low_level) );
				if( !GD_WaitSpriteCondition(GRP_MAP_CHAR+no) ){
					if( !GD_WaitSpriteCondition(GRP_ITEM_WORK+0) ){
						
						GD_ResetGraph( GRP_ITEM_WORK+0 );
						GD_ReleaseSprite( SPR_ITEM_WORK+0 );

						ItemEffectStruct[ii].count=0;
						ItemEffectStruct[ii].step=2;
					}
				}
				break;
			case 2:
				lv=0;
				dmg = ItemParam[ItemEffectStruct[ii].ino].hp;

				

				if(CharParam[CharControl[no].cno].CheckStatusEffct(STATUS_ITEM_BAI)){
					dmg*=2;
				}
				if(dmg){
					SetHitAction( tno, dmg, 0, HIT_HEAL );
					CharParam[CharControl[tno].cno].AddHp( dmg );
					lv = CharParam[CharControl[tno].cno].lv;
				}

				dmg = ItemParam[ItemEffectStruct[ii].ino].mp;
				if(CharParam[CharControl[no].cno].CheckStatusEffct(STATUS_ITEM_BAI)){
					dmg*=2;
				}
				if(dmg){
					SetHitAction( tno, dmg, 0, HIT_HEAL );
					CharParam[CharControl[tno].cno].AddMp( dmg );
					lv = CharParam[CharControl[tno].cno].lv;
				}
				
				if( CharParam[CharControl[no].cno].AddExpAll( lv, FALSE ) ){
					SetHitAction( no, 0, 0.0f, HIT_LVUP );
				}

				GD_SetSpritePlay( GRP_MAP_CHAR+no, SPR_MAP_CHAR+CharControl[no].ano, CharControl[no].dir+8, TRUE, TRUE, 1, FALSE );
				CharControl[no].sw =  200;

				ItemEffectStruct[ii].count=0;
				ItemEffectStruct[ii].step=3;
				break;
			case 3:
				if(ItemEffectStruct[ii].count>30){
					ItemEffectStruct[ii].step=4;
				}
				break;
			case 4:
				break;
			}
			if(char_active){
				ItemEffectStruct[ii].count++;
			}
		}
	}
	return 1;
}
