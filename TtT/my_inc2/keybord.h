
 
  
   
    
     
      

#ifndef	_KEYBORD_H_
#define _KEYBORD_H_

#define VK_0	'0'
#define VK_1	'1'
#define VK_2	'2'
#define VK_3	'3'
#define VK_4	'4'
#define VK_5	'5'
#define VK_6	'6'
#define VK_7	'7'
#define VK_8	'8'
#define VK_9	'9'


#define VK_A	'A'
#define VK_B	'B'
#define VK_C	'C'
#define VK_D	'D'
#define VK_E	'E'
#define VK_F	'F'
#define VK_G	'G'
#define VK_H	'H'
#define VK_I	'I'
#define VK_J	'J'
#define VK_K	'K'
#define VK_L	'L'
#define VK_M	'M'
#define VK_N	'N'
#define VK_O	'O'
#define VK_P	'P'
#define VK_Q	'Q'
#define VK_R	'R'
#define VK_S	'S'
#define VK_T	'T'
#define VK_U	'U'
#define VK_V	'V'
#define VK_W	'W'
#define VK_X	'X'
#define VK_Y	'Y'
#define VK_Z	'Z'

typedef struct{
	char		up;
	char		down;
	char		left;
	char		right;

	char		enter;
	char		space;
	char		bs;
	char		ctrl;
	char		shift;
	char		alt;
	char		tab;
	char		esc;

	char		pup;
	char		pdown;
	char		end;
	char		home;
	
	char		f1;
	char		f2;
	char		f3;
	char		f4;
	char		f5;
	char		f6;
	char		f7;
	char		f8;
	char		f9;
	char		f10;
	char		f11;
	char		f12;

	char		k0;
	char		k1;
	char		k2;
	char		k3;
	char		k4;
	char		k5;
	char		k6;
	char		k7;
	char		k8;
	char		k9;

	char		n0;
	char		n1;
	char		n2;
	char		n3;
	char		n4;
	char		n5;
	char		n6;
	char		n7;
	char		n8;
	char		n9;

	char		a;
	char		b;
	char		c;
	char		d;
	char		e;
	char		f;
	char		g;
	char		h;
	char		i;
	char		j;
	char		k;
	char		l;
	char		n;
	char		m;
	char		o;
	char		p;
	char		q;
	char		r;
	char		s;
	char		t;
	char		u;
	char		v;
	char		w;
	char		x;
	char		y;
	char		z;
}KEY_STRUCT;

typedef struct{
	KEY_STRUCT		btn;
	KEY_STRUCT		trg;
	KEY_STRUCT		btrg;
}KEY_CONDITION;

extern KEY_CONDITION		KeyCond;

extern void KEY_RenewKeybord( int active );
extern BOOL KEY_KeyCmp( char *str );


#endif