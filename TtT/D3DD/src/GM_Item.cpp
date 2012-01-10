#include <windows.h>

#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "comp_pac.h"
#include "escript.h"

#include "main.h"

#include "GlobalDraw.h"

#include "GM_Avg.h"
#include "GM_Main.h"
#include "GM_Battle.h"
#include "GM_Char.h"
#include "GM_Item.h"

#include <math.h>


#include "DAT_ItemParam.h"



ITEM_BOX	ItemBox[ITEM_MAX];

int	GetBttleItemCount=0;
int	GetBttleItem[GET_BATTLE_ITEM_MAX];

void ITM_GetBttleItemInit( void )
{
	int	i;
	for(i=0;i<GET_BATTLE_ITEM_MAX;i++){
		GetBttleItem[i] = 0;
	}
	GetBttleItemCount = 0;
}

int ITM_ItemDropItem( int ino )
{
	if(GetBttleItemCount<GET_BATTLE_ITEM_MAX){
		GetBttleItem[GetBttleItemCount++] = ino;
	}
	return ino;
}

int ITM_ItemDropRank( int rank )
{
	int	i;
	int	j = 0;
	int	r;
	r = rand()%100;
	while(1){
		for(i=0;i<ITEM_MAX;i++){
			if(ItemParam[i].type==ITEM_TOOL_EVENT) continue;
			if(ItemParam[i].name[0]==0) break;

			if( rank==ItemParam[i].shop ){
				if( rank>=50 && ITM_GetItemAmount(i) ) continue;
				j++;
				if(j>r){
					goto get_bt_item_end;
				}
			}
		}
		if(j==0) break;
	}
get_bt_item_end:

	if(j){
		ITM_ItemDropItem( i );
		return i;
	}else{
		return 0;
	}
}


int ITM_ItemDrop( int job_no, int tlv, int kari )
{

	int	rank;
	int	r;
	r = rand()%100;

	int	rea=0;
	int	nanido=99;

	switch( ESC_GetFlag(3) ){
		case 4:	nanido=95;	break;
		case 5:	nanido=90;	break;
	}
	if(r>=nanido-kari*5){
		r = rand()%100;
		if(r<10){
			rea=1;
			r = rand()%100;
			if(r>=80){
				if(tlv>=50){
					rea=2;
				}
			}
		}
	}
	if(rea==1 && tlv>=15){
		rank = 15+rand()%(tlv-15+1);
		rank = rank%50;
		rank = max(rank,15);
		if(rank<20){
			return ITM_ItemDropRank( 15 );
		}else{
			return ITM_ItemDropRank( rank/10*10 );
		}
	}
	if(rea==2 && tlv>=50){
		rank = 50+rand()%(tlv-50+1);
		return ITM_ItemDropRank( rank/10*10 );
	}
	r = rand()%100;
	switch(job_no){
		case 21:	case 22:	case 23:
		case 24:	case 25:	case 26:
		case 27:	case 28:
			if(r<10+kari*5){
				if(tlv>=10 && tlv<25){
					return ITM_ItemDropRank( 12 );
				}
			}
			break;
	}
	switch(job_no){
		case 41:
		case 42:
			if(r<10+kari*5){
				if(tlv>=10 && tlv<20){
					return ITM_ItemDropRank( 14 );
				}
			}
			break;
	}

	switch(job_no){
		case 43:
		case 44:
		case 46:
			if(r<20+kari*5){
				if(tlv>=10 && tlv<20){
					return ITM_ItemDropRank( 13 );
				}
			}
			break;
	}

	r = rand()%100;
	if(r>10+kari*5) return 0;

	r = rand()%100;

	tlv-=10;

	if(r<10){
		tlv+=5;
	}else if(r>=50){
		tlv-=5;
		if(r>=75){
			tlv-=5;
		}
	}

	rank=1;
	if( tlv<5 ){
		rank=1;
	}else if( tlv<10 ){
		rank=2;
	}else if( tlv<15 ){
		rank=3;
	}else if( tlv<20 ){
		rank=4;
	}else if( tlv<25 ){
		rank=5;
	}else if( tlv<30 ){
		rank=6;
	}else if( tlv<35 ){
		rank=7;
	}else if( tlv<40 ){
		rank=8;
	}else if( tlv<45 ){
		rank=9;
	}else if( tlv<50 ){
		rank=10;
	}else if( tlv<90 ){
		rank=11;
	}else{
		rank=(rand()%60+20)/10*10;
	}
	return ITM_ItemDropRank( rank );
}


void TestItemAll(void)
{
	return ;
	int	i;
	int	j=0;

	for(i=0;i<ITEM_MAX-1;i++){
		if(ItemParam[i+1].name[0]){
			ItemBox[j].ino    = i+1;
			ItemBox[j].amount = 1;
			j++;
		}
	}
}

void ITM_SortItem( int sort_type )
{
	int	i,j,k;
	ITEM_BOX	SortItemBox[ITEM_MAX];

	ZeroMemory( SortItemBox, sizeof(ITEM_BOX)*ITEM_MAX );
	switch(sort_type){
		default:
		case 0:
			k=0;
			for(i=0;i<ITEM_MAX;i++){
				for(j=0;j<ITEM_MAX;j++){
					if( ItemBox[j].ino==0 ) break;
					if( ItemBox[j].ino==i ){
						SortItemBox[k].ino = ItemBox[j].ino;
						SortItemBox[k].amount = ItemBox[j].amount;
						k++;
						break;
					}
				}
			}
			break;
	}
	CopyMemory( ItemBox, SortItemBox, sizeof(ITEM_BOX)*ITEM_MAX );
}


int ITM_GetItemEquipAmount( int ino )
{
	int	i, amount=0;;
	for(i=0;i<32;i++){
		if(CharParam[i].flag){
			if(CharParam[i].buki==ino) amount++;
			if(CharParam[i].yoroi==ino) amount++;
			if(CharParam[i].accessary1==ino) amount++;
			if(CharParam[i].accessary2==ino) amount++;
		}
	}
	return amount;
}

int ITM_GetItemAmount( int ino )
{
	int	i;
	for(i=0;i<ITEM_MAX;i++){
		if( ItemBox[i].ino==ino ){
			return ItemBox[i].amount;
		}
	}
	return 0;
}

int ITM_GetItem( int ino, int amount )
{
	int	i, a;

	switch(ino){
		case 284:
			AVG_SetGameFlag(20,1);
			break;
		case 285:
			AVG_SetGameFlag(21,1);
			break;
		case 286:
			AVG_SetGameFlag(22,1);
			break;
		case 287:
			AVG_SetGameFlag(23,1);
			break;
		case 288:
			AVG_SetGameFlag(24,1);
			break;
		case 289:
			AVG_SetGameFlag(25,1);
			break;
		case 290:
			AVG_SetGameFlag(26,1);
			break;
		case 291:
			AVG_SetGameFlag(27,1);
			break;
		case 292:
			AVG_SetGameFlag(28,1);
			break;
	}
	
	
	
	for(i=0;i<ITEM_MAX;i++){
		if( ItemBox[i].ino==ino ){
			a = ItemBox[i].amount = min(99,ItemBox[i].amount+amount);

ITM_SortItem( 0 );
			return a;
		}
	}
	for(i=0;i<ITEM_MAX;i++){
		if( ItemBox[i].ino==0 ){
			ItemBox[i].ino=ino;
			a = ItemBox[i].amount = min(99,amount);

ITM_SortItem( 0 );
			return a;
		}
	}
	return -1;
}
int ITM_LostItem( int ino, int amount )
{
	int	i,j;
	for(i=0;i<ITEM_MAX;i++){
		if( ItemBox[i].ino==ino ){
			ItemBox[i].amount = max(0,ItemBox[i].amount-amount);
			if(ItemBox[i].amount==0){
				ZeroMemory( &ItemBox[i], sizeof(ITEM_BOX) );
				for(j=i;j<ITEM_MAX-1;j++){
					CopyMemory( &ItemBox[j], &ItemBox[j+1], sizeof(ITEM_BOX) );
				}
			}
			return ItemBox[i].amount;
		}
	}
	return -1;
}


int ITM_GetEquipType( int ino )
{
	int	ret=0;

	if(       ITEM_BUKI_SWORD   <=ItemParam[ino].type && ItemParam[ino].type<=ITEM_BUKI_OTHER  ){
		ret=0;
	}else if( ITEM_YOROI_ARMOUR1<=ItemParam[ino].type && ItemParam[ino].type<=ITEM_YOROI_OTHER ){
		ret=1;
	}else if( ITEM_ACCE_RING    <=ItemParam[ino].type && ItemParam[ino].type<=ITEM_ACCE_OTHER1 ){
		ret=2;
	}else if( ITEM_ACCE_BOOTS   <=ItemParam[ino].type && ItemParam[ino].type<=ITEM_ACCE_OTHER2 ){
		ret=3;
	}else{
		ret=4;
	}
	return ret;
}

