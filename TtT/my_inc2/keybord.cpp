
 
  
   
    
     
      



#include "STD_Windows.h"
#include "STD_Debug.h"

#include "keybord.h"

KEY_CONDITION		KeyCond;
char	KeyBuf[256];


void KEY_GetKeyCondition( int active, KEY_STRUCT *key )
{
	int	i;

	if( active ){
		key->up		= (GetAsyncKeyState(VK_UP   )&0x8001) ? 1 : 0;
		key->down	= (GetAsyncKeyState(VK_DOWN )&0x8001) ? 1 : 0;
		key->left	= (GetAsyncKeyState(VK_LEFT )&0x8001) ? 1 : 0;
		key->right	= (GetAsyncKeyState(VK_RIGHT)&0x8001) ? 1 : 0;

		key->enter	= (GetAsyncKeyState(VK_RETURN )&0x8001) ? 1 : 0;
		key->space	= (GetAsyncKeyState(VK_SPACE  )&0x8001) ? 1 : 0;
		key->bs		= (GetAsyncKeyState(VK_BACK   )&0x8001) ? 1 : 0;
		key->ctrl	= (GetAsyncKeyState(VK_CONTROL)&0x8001) ? 1 : 0;
		key->shift	= (GetAsyncKeyState(VK_SHIFT  )&0x8001) ? 1 : 0;
		key->alt	= (GetAsyncKeyState(VK_MENU   )&0x8001) ? 1 : 0;
		key->tab	= (GetAsyncKeyState(VK_TAB    )&0x8001) ? 1 : 0;
		key->esc	= (GetAsyncKeyState(VK_ESCAPE )&0x8001) ? 1 : 0;

		key->pup	= (GetAsyncKeyState(VK_PRIOR)&0x8001) ? 1 : 0;
		key->pdown	= (GetAsyncKeyState(VK_NEXT )&0x8001) ? 1 : 0;
		key->end	= (GetAsyncKeyState(VK_END  )&0x8001) ? 1 : 0;
		key->home	= (GetAsyncKeyState(VK_HOME )&0x8001) ? 1 : 0;

		key->f1		= (GetAsyncKeyState(VK_F1)&0x8001) ? 1 : 0;
		key->f2		= (GetAsyncKeyState(VK_F2)&0x8001) ? 1 : 0;
		key->f3		= (GetAsyncKeyState(VK_F3)&0x8001) ? 1 : 0;
		key->f4		= (GetAsyncKeyState(VK_F4)&0x8001) ? 1 : 0;
		key->f5		= (GetAsyncKeyState(VK_F5)&0x8001) ? 1 : 0;
		key->f6		= (GetAsyncKeyState(VK_F6)&0x8001) ? 1 : 0;
		key->f7		= (GetAsyncKeyState(VK_F7)&0x8001) ? 1 : 0;
		key->f8		= (GetAsyncKeyState(VK_F8)&0x8001) ? 1 : 0;
		key->f9		= (GetAsyncKeyState(VK_F9)&0x8001) ? 1 : 0;
		key->f10	= (GetAsyncKeyState(VK_F10)&0x8001) ? 1 : 0;
		key->f11	= (GetAsyncKeyState(VK_F11)&0x8001) ? 1 : 0;
		key->f12	= (GetAsyncKeyState(VK_F12)&0x8001) ? 1 : 0;

		key->k0		= (GetAsyncKeyState(VK_0)&0x8001) ? 1 : 0;
		key->k1		= (GetAsyncKeyState(VK_1)&0x8001) ? 1 : 0;
		key->k2		= (GetAsyncKeyState(VK_2)&0x8001) ? 1 : 0;
		key->k3		= (GetAsyncKeyState(VK_3)&0x8001) ? 1 : 0;
		key->k4		= (GetAsyncKeyState(VK_4)&0x8001) ? 1 : 0;
		key->k5		= (GetAsyncKeyState(VK_5)&0x8001) ? 1 : 0;
		key->k6		= (GetAsyncKeyState(VK_6)&0x8001) ? 1 : 0;
		key->k7		= (GetAsyncKeyState(VK_7)&0x8001) ? 1 : 0;
		key->k8		= (GetAsyncKeyState(VK_8)&0x8001) ? 1 : 0;
		key->k9		= (GetAsyncKeyState(VK_9)&0x8001) ? 1 : 0;

		key->n0		= (GetAsyncKeyState(VK_NUMPAD0)&0x8001) ? 1 : 0;
		key->n1		= (GetAsyncKeyState(VK_NUMPAD1)&0x8001) ? 1 : 0;
		key->n2		= (GetAsyncKeyState(VK_NUMPAD2)&0x8001) ? 1 : 0;
		key->n3		= (GetAsyncKeyState(VK_NUMPAD3)&0x8001) ? 1 : 0;
		key->n4		= (GetAsyncKeyState(VK_NUMPAD4)&0x8001) ? 1 : 0;
		key->n5		= (GetAsyncKeyState(VK_NUMPAD5)&0x8001) ? 1 : 0;
		key->n6		= (GetAsyncKeyState(VK_NUMPAD6)&0x8001) ? 1 : 0;
		key->n7		= (GetAsyncKeyState(VK_NUMPAD7)&0x8001) ? 1 : 0;
		key->n8		= (GetAsyncKeyState(VK_NUMPAD8)&0x8001) ? 1 : 0;
		key->n9		= (GetAsyncKeyState(VK_NUMPAD9)&0x8001) ? 1 : 0;

		key->a		= (GetAsyncKeyState(VK_A)&0x8001) ? 1 : 0;
		key->b		= (GetAsyncKeyState(VK_B)&0x8001) ? 1 : 0;
		key->c		= (GetAsyncKeyState(VK_C)&0x8001) ? 1 : 0;
		key->d		= (GetAsyncKeyState(VK_D)&0x8001) ? 1 : 0;
		key->e		= (GetAsyncKeyState(VK_E)&0x8001) ? 1 : 0;
		key->f		= (GetAsyncKeyState(VK_F)&0x8001) ? 1 : 0;
		key->g		= (GetAsyncKeyState(VK_G)&0x8001) ? 1 : 0;
		key->h		= (GetAsyncKeyState(VK_H)&0x8001) ? 1 : 0;
		key->i		= (GetAsyncKeyState(VK_I)&0x8001) ? 1 : 0;
		key->j		= (GetAsyncKeyState(VK_J)&0x8001) ? 1 : 0;
		key->k		= (GetAsyncKeyState(VK_K)&0x8001) ? 1 : 0;
		key->l		= (GetAsyncKeyState(VK_L)&0x8001) ? 1 : 0;
		key->m		= (GetAsyncKeyState(VK_M)&0x8001) ? 1 : 0;
		key->n		= (GetAsyncKeyState(VK_N)&0x8001) ? 1 : 0;
		key->o		= (GetAsyncKeyState(VK_O)&0x8001) ? 1 : 0;
		key->p		= (GetAsyncKeyState(VK_P)&0x8001) ? 1 : 0;
		key->q		= (GetAsyncKeyState(VK_Q)&0x8001) ? 1 : 0;
		key->r		= (GetAsyncKeyState(VK_R)&0x8001) ? 1 : 0;
		key->s		= (GetAsyncKeyState(VK_S)&0x8001) ? 1 : 0;
		key->t		= (GetAsyncKeyState(VK_T)&0x8001) ? 1 : 0;
		key->u		= (GetAsyncKeyState(VK_U)&0x8001) ? 1 : 0;
		key->v		= (GetAsyncKeyState(VK_V)&0x8001) ? 1 : 0;
		key->w		= (GetAsyncKeyState(VK_W)&0x8001) ? 1 : 0;
		key->x		= (GetAsyncKeyState(VK_X)&0x8001) ? 1 : 0;
		key->y		= (GetAsyncKeyState(VK_Y)&0x8001) ? 1 : 0;
		key->z		= (GetAsyncKeyState(VK_Z)&0x8001) ? 1 : 0;
	}else{
		ZeroMemory( key, sizeof(KEY_STRUCT) );
		GetAsyncKeyState(VK_UP);
		GetAsyncKeyState(VK_DOWN);
		GetAsyncKeyState(VK_LEFT);
		GetAsyncKeyState(VK_RIGHT);
		
		GetAsyncKeyState(VK_RETURN);
		GetAsyncKeyState(VK_SPACE);
		GetAsyncKeyState(VK_BACK);

		GetAsyncKeyState(VK_CONTROL);
		GetAsyncKeyState(VK_SHIFT);
		GetAsyncKeyState(VK_MENU);
		GetAsyncKeyState(VK_TAB);
		GetAsyncKeyState(VK_ESCAPE);

		GetAsyncKeyState(VK_PRIOR);
		GetAsyncKeyState(VK_NEXT);
		GetAsyncKeyState(VK_END);
		GetAsyncKeyState(VK_HOME);

		for( i=VK_F1 ; i<=VK_F12 ; i++ ){
			GetAsyncKeyState(i);
		}
		for( i=VK_0 ; i<=VK_NUMPAD9 ; i++ ){
			GetAsyncKeyState(i);
		}
		for( i=VK_NUMPAD0 ; i<=VK_9 ; i++ ){
			GetAsyncKeyState(i);
		}
		for( i=VK_A ; i<=VK_Z ; i++ ){
			GetAsyncKeyState(i);
		}
	}
}

void KEY_GetKeyTrgger( KEY_STRUCT key, KEY_STRUCT bak, KEY_STRUCT *trg )
{
	int		i, ahit=0,akey;
	char	*keyb = (char*)&key;
	char	*bakb = (char*)&bak;
	char	*trgb = (char*)trg;
	
	for( i=0; i<sizeof(KEY_STRUCT) ;i++ ){
		*(trgb+i) = (*(keyb+i) && !*(bakb+i));
	}

	for( i=0; i<26 ;i++ ){
		ahit |= *(&trg->a+i);
		if( *(&trg->a+i) ) akey = i;
	}
	if(ahit){
		MoveMemory( &KeyBuf[1], &KeyBuf[0], 255 );
		KeyBuf[0] = 'a'+akey;
	}
}

void KEY_GetKeyButtonTrgger( KEY_STRUCT key, KEY_STRUCT *btrg )
{
	static KEY_STRUCT	bcnt;
	int		i;
	char	*keyb = (char*)&key;
	char	*btrgb = (char*)btrg;
	char	*bcntb = (char*)&bcnt;
	
	for( i=0; i<sizeof(KEY_STRUCT) ;i++ ){
		bcntb[i] = (keyb[i]) ? min(16,bcntb[i]+1) : 0 ;
		btrgb[i] = bcntb[i]>=16 || bcntb[i]==1;
	}
}

void KEY_RenewKeybord( int active )
{
	KEY_CONDITION	KeyBack = KeyCond;

	KEY_GetKeyCondition( active, &KeyCond.btn );
	KEY_GetKeyTrgger( KeyCond.btn, KeyBack.btn, &KeyCond.trg );
	KEY_GetKeyButtonTrgger( KeyCond.btn, &KeyCond.btrg );
}


BOOL KEY_KeyCmp( char *str )
{
	char	buf[256];
	int		len = strlen(str), i;
	BOOL	ret;

	for(i=0;i<len;i++)
		buf[i] = str[len-i-1];
	buf[i] = '\0';

	ret = !strnicmp( KeyBuf, buf, len );
	if(ret){
		ZeroMemory( KeyBuf, 256 );
	}
	return ret;
}
