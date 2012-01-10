
 
  
   
    
     
      

#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "d3d_text.h"


#define RUBI_FONT_SIZE	12



static int GetDigit( char *str, int *cnt,int *digit, int keta=0 )
{
	int dcnt = 0;

	*digit = 0;
	while( _ismbcdigit( str[*cnt] ) ){

		if(keta!=0 && dcnt>=keta) break;
		*digit *= 10;
		*digit += str[*cnt] - '0';
		(*cnt)++;
		dcnt++;
	}
	if( str[*cnt]!=':' ){
		(*cnt)--;
	}
	return dcnt;
}



static int TXT_GetMsgSpeed( int speed )
{
	static int	speed_cnt=0;
	int			ret;
	
	if( speed==-2 ){
		ret = speed_cnt;
	}else if( speed==-1 ){
		speed_cnt = 0;
		ret = 0;
	}else{
		speed_cnt += speed%10;
		ret       = speed/10 + speed_cnt/10;
		speed_cnt = speed_cnt%10;
	}
	return ret;
}


int D3DTXT_DrawText( D3DD_TEXTURE *d3dTex, int sx, int sy, int w, int h, int pixh_w, int pich_h, int font, char *str, int kage, int r, int g, int b, int kr, int kg, int kb )
{
	if(kage==0){

		return D3DTXT_DrawTextEx( d3dTex, sx, sy, w, h, pixh_w, pich_h, font, str, r, g, b, 0 );
	}else{
				D3DTXT_DrawTextEx( d3dTex, sx, sy, w, h, pixh_w, pich_h, font, str, kr, kg, kb,  2 );
		return	D3DTXT_DrawTextEx( d3dTex, sx, sy, w, h, pixh_w, pich_h, font, str,  r,  g,  b, -1 );
	}
}


static D3DD_ARGB	FCT[32]={
	{ 255, 255, 255, 0 },	
	{ 192, 192, 192, 0 },	
	{  80,  80,  80, 0 },	
	{   0,   0,   0, 0 },	
	
	{   0,   0, 255, 0 },	
	{   0, 255,   0, 0 },	
	{ 255,   0,   0, 0 },	
	{   0, 255, 255, 0 },	

	{ 255, 255,   0, 0 },	
	{ 255,   0, 255, 0 },	
	{   0, 128, 255, 0 },	
	{ 128, 255,   0, 0 },	
	
	{ 255,   0, 128, 0 },	
	{   0, 255, 128, 0 },	
	{ 255, 128,   0, 0 },	
	{ 128,   0, 255, 0 },	

	{ 223, 230, 172, 0 },	
	{ 255, 225, 197, 0 },	
};
int D3DTXT_DrawTextEx( D3DD_TEXTURE *d3dTex, int sx, int sy, int w, int h, int pixh_w, int pich_h, int font, char *str, int r, int g, int b, int kage )
{
	int		digit;
	int		px = sx, py = sy;
	int		cnt = 0;
	int		cnt2 = 0;
	int		kflag = 0;
	int		fno = font, fno2 = font;
	int		tag_cnt = 0;
	int		tag_param[16];
	int		tag_back[16];
	int		draw_flag=0;
	int		accent_flag=0;
	char	rubi_str[64];
	int		rfno = RUBI_FONT_SIZE;
	int		wait=0;
	int		speed=10;
	int		step=0;
	int		end_flag=0;
	int		kaig=0;
	int		px_bak=0;
	int		rr=r, gg=g, bb=b;

	if(d3dTex->flag==0) return 0;

	enum{
		TAG_DIGIT,
		TAG_COLOR,
		TAG_BRIGHT,
		TAG_KAGE,
		TAG_FONT,
		TAG_RUBI,
		TAG_ACCENT,
		TAG_SPEED,
		TAG_WAIT,
	};
	
	while( 1 ) {	
		if(wait){
			wait--;
			cnt2++;
			continue;
		}
		
		switch( str[cnt] ){
			case '\0':		
				if( tag_cnt ){
					DebugBox( NULL, "タグが開いたままになっています。必ず閉じてください" );
				}
				end_flag=1;
				break;
			case '\n':
				if(!kaig){
					px = sx;
					py += fno2+pich_h;
					fno2 = fno;
				}
				kaig=0;
				break;
			case '^':	
				draw_flag = 1;
				str[cnt] = ' ';

				break;
			case '~':	
				str[cnt] = ',';
				draw_flag = 1;

				break;
			case '\\':
				cnt++;
				switch( str[cnt] ){
					case 'n':			
						if(!kaig){
							px = sx;
							py += fno2+pich_h;
							fno2 = fno;

						}
						kaig=0;
						break;
					case 'k':			
						step++;
						break;
					case '^':	case '~':
					case '<':	case '>':
					case '|':	case '\\':	
						draw_flag = 1;
						break;
				}
				break;
			case '<':		
				cnt++;
				if( tag_cnt >= 15 ){
					DebugBox( NULL, "タグを開き過ぎです。ＭＡＸ １５" );
					break;
				}
				switch( str[cnt] ){
					case 'd':	case 'D':	
						tag_param[tag_cnt] = TAG_DIGIT;
						tag_back[tag_cnt]  = 0;
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
						}
						break;
					case 'c':	case 'C':	
						tag_param[tag_cnt] = TAG_COLOR;
						tag_back[tag_cnt] = (rr<<16) | (gg<<8) | (bb);
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							if(kage<=0){
								rr=FCT[digit].r;
								gg=FCT[digit].g;
								bb=FCT[digit].b;
							}
						}
						break;
					case 'b':	case 'B':	
						tag_param[tag_cnt] = TAG_BRIGHT;
						tag_back[tag_cnt] = (rr<<16) | (gg<<8) | (bb);
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit, 3 ) ){
							if(kage<=0){
								rr=digit;
							}
						}
						cnt++;
						if( GetDigit( str, &cnt, &digit, 3 ) ){
							if(kage<=0){
								gg=digit;
							}
						}
						cnt++;
						if( GetDigit( str, &cnt, &digit, 3 ) ){
							if(kage<=0){
								bb=digit;
							}
						}
						break;
					case 'k':	case 'K':	
						tag_param[tag_cnt] = TAG_KAGE;
						tag_back[tag_cnt] = (rr<<16) | (gg<<8) | (bb);
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							if(kage>0){
								rr=digit;
							}
						}
						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							if(kage>0){
								gg=digit;
							}
						}
						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							if(kage>0){
								bb=digit;
							}
						}
						break;
					case 'f':	case 'F':	
						tag_param[tag_cnt] = TAG_FONT;
						tag_back[tag_cnt]  = fno;
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							fno = digit;
							fno2 = max(fno,fno2);
						}
						break;
					case 'r':	case 'R':	
						tag_param[tag_cnt] = TAG_RUBI;
						tag_back[tag_cnt]  = px;
						tag_cnt++;
						break;
					case 'a':	case 'A':	
						tag_param[tag_cnt] = TAG_ACCENT;
						tag_cnt++;
						accent_flag = 1;
						break;
					case 's':	case 'S':	
						tag_param[tag_cnt] = TAG_SPEED;
						tag_back[tag_cnt]  = speed;
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							switch(digit){
								case 0:	speed = 100;break;
								case 1:	speed = 80;	break;
								case 2:	speed = 60;	break;
								case 3:	speed = 40;	break;
								case 4:	speed = 20;	break;
								case 5:	speed = 10;	break;
								case 6:	speed = 6;	break;
								case 7:	speed = 4;	break;
								case 8:	speed = 2;	break;
								case 9:	speed = 1;	break;
								case 10:speed = 0;	break;
							}
						}
						break;
					case 'w':	case 'W':	
						tag_param[tag_cnt] = TAG_WAIT;
						tag_back[tag_cnt]  = 0;
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							wait = digit;
						}
						break;
				}
				break;
			case '|':		
				cnt++;
				switch(tag_param[tag_cnt-1]){
					case TAG_RUBI:	
						{
							int		i=0,rx,pw=0;
							while(str[cnt]!='>'){
								rubi_str[i++] = str[cnt++];
							}
							rubi_str[i] = '\0';
							
							rx = ((px-tag_back[tag_cnt-1]) - i*rfno/2)/2;
							if(rx<=0){
								pw=0;
							}else{
								if( (0x21 <= rubi_str[0] && rubi_str[0] <= 0x7e) || (0xa1 <= rubi_str[0] && rubi_str[0] <= 0xdf) ){
									pw = rx*2/(i);
								}else{
									pw = rx*2/(i/2);
								}
								rx = pw/2;
							}

							rx = tag_back[tag_cnt-1]+rx;
							D3DFNT_UpdateText( d3dTex, rx, py-rfno+5, pw,0, (BYTE*)rubi_str, rfno, r, g, b, (kage>1)?1:kage );
						}
						cnt--;
						break;
				}
				break;

			case '>':		
				if( tag_cnt <= 0 ){
					DebugBox( NULL, "閉じタグ( > )の数が、開いた数より多くなっています" );
					break;
				}
				tag_cnt--;
				switch( tag_param[tag_cnt] ){
					case TAG_DIGIT:		break;
					case TAG_COLOR:
						if(kage<=0){
							rr = (tag_back[tag_cnt]&0x00ff0000)>>16;
							gg = (tag_back[tag_cnt]&0x0000ff00)>>8;
							bb = (tag_back[tag_cnt]&0x000000ff);
						}
						break;
					case TAG_BRIGHT:
						if(kage<=0){
							rr = (tag_back[tag_cnt]&0x00ff0000)>>16;
							gg = (tag_back[tag_cnt]&0x0000ff00)>>8;
							bb = (tag_back[tag_cnt]&0x000000ff);
						}
						break;
					case TAG_KAGE:
						if(kage>0){
							rr = (tag_back[tag_cnt]&0x00ff0000)>>16;
							gg = (tag_back[tag_cnt]&0x0000ff00)>>8;
							bb = (tag_back[tag_cnt]&0x000000ff);
						}
						break;

					case TAG_FONT:
						fno = tag_back[tag_cnt];
						if(px==sx){
							fno2 = fno;
						}else{
							fno2 = max(fno,fno2);
						}
						break;

					case TAG_RUBI:									break;
					case TAG_ACCENT:	accent_flag = 0;			break;
					case TAG_SPEED:		speed = tag_back[tag_cnt];	break;
					case TAG_WAIT:									break;
				}
				break;
			default:	
				draw_flag = 1;
				break;
		}
		if(draw_flag){
			draw_flag = 0;
			if( (0x00<=str[cnt] && str[cnt]<0x80) || (0xa0<=str[cnt] && str[cnt]<0xe0) ) {
				
				D3DFNT_UpdateTextChar( d3dTex, px, py, str[cnt], fno, rr, gg, bb, kage );
				cnt2 += TXT_GetMsgSpeed( speed );

				if(accent_flag){
					D3DFNT_UpdateTextChar( d3dTex, px+(fno/2-rfno/2)/2+1, py-rfno+4, (BYTE)'･', rfno, rr, gg, bb, kage );
				}
				
				px += fno/2+pixh_w;
				kaig=0;
			}else{	
				
				D3DFNT_UpdateTextChar( d3dTex, px, py, *(WORD*)&str[cnt], fno, rr, gg, bb, kage );
				cnt++;
				cnt2 += TXT_GetMsgSpeed( speed );

				if(accent_flag){
					D3DFNT_UpdateTextChar( d3dTex, px+(fno-rfno/2)/2+1, py-rfno+4, (BYTE)'･', rfno, rr, gg, bb, kage );
				}
				
				px += fno+pixh_w;
				kaig=0;
			}
		}
		if(end_flag)break;
		cnt++;

		
		if( px-sx >= w*font-fno+1 ){
			if( tag_param[tag_cnt-1]!=TAG_RUBI ){
				if( kflag==1 ){
					if( strncmp( &str[ cnt ], "　", 2 ) )
					{
						kflag = 0;
						px_bak = px;
						px = sx;
						py += fno2+pich_h;
						fno2 = fno;
						kaig=1;
					}
				}else{
					if( strncmp( &str[ cnt ], "。", 2 ) && strncmp( &str[ cnt ], "、", 2 ) &&
						strncmp( &str[ cnt ], "？", 2 ) && strncmp( &str[ cnt ], "！", 2 ) &&
						strncmp( &str[ cnt ], "　", 2 ) && strncmp( &str[ cnt ], "）", 2 ) &&
						strncmp( &str[ cnt ], "」", 2 ) && strncmp( &str[ cnt ], "』", 2 ) &&
						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) &&
						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) &&
						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) )
					{
						px_bak = px;
						px = sx;
						py += fno2+pich_h;
						fno2 = fno;
						kaig=1;
					}else{
						kflag=1;
					}
				}
			}
		}
		if( py-sy >= h*(font+pich_h)-(fno+pich_h)+1 ){
			static int err=0;
			if(err==0){
				err=1;
				DebugBox(NULL,"行数が多すぎます");
			}
			break;
		}
	}
	
	return (px==sx)? py-sy : py-sy+fno;
}

void D3DTXT_DispTextChar( D3DD_DISP d3dDispWork, int px1, int py1, int px2, int py2, int ph, DWORD d3d_color )
{
	d3dDispWork.dx[3] = d3dDispWork.dx[0] + px2;
	d3dDispWork.dx[2] = d3dDispWork.dx[0] + px1;
	d3dDispWork.dx[1] = d3dDispWork.dx[0] + px2;
	d3dDispWork.dx[0] = d3dDispWork.dx[0] + px1;

	d3dDispWork.dy[3] = d3dDispWork.dy[0] + py2-2;
	d3dDispWork.dy[2] = d3dDispWork.dy[0] + py2-2;
	d3dDispWork.dy[1] = d3dDispWork.dy[0] + py1;
	d3dDispWork.dy[0] = d3dDispWork.dy[0] + py1;

	d3dDispWork.sx1[0] = (float)px1;
	d3dDispWork.sx1[1] = (float)px2;
	d3dDispWork.sy1[0] = (float)py1+ph;
	d3dDispWork.sy1[1] = (float)py2-2+ph;

	d3dDispWork.blend_param = BLEND_ALPHA;

	*(DWORD*)&d3dDispWork.color[0] = 
	*(DWORD*)&d3dDispWork.color[1] = 
	*(DWORD*)&d3dDispWork.color[2] = 
	*(DWORD*)&d3dDispWork.color[3] = d3d_color;
	*(DWORD*)&d3dDispWork.specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDispWork.specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDispWork.specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDispWork.specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	D3DD_DrawTexture( &d3dDispWork );
}



int D3DTXT_DispText( D3DD_DISP *d3dd, int sx, int sy, int w, int h, int pixh_w, int pich_h, int *px2, int *py2, RECT *clip,
					int font, char *str, int text_cnt, int step_cnt, int r, int g, int b, int alph, int kage, int cnt_flag, char *normal_str, int *step_wait  )
{
	int		digit;
	int		px = sx, py = sy;
	int		bx = sx;
	int		cnt = 0;
	int		cnt2 = 0, cnt3;
	int		kflag = 0;
	int		fno = font, fno2 = font;
	int		tag_cnt = 0;
	int		tag_param[16];
	int		tag_back[16];
	int		draw_flag=0;
	int		accent_flag=0;
	char	rubi_str[64];
	int		rfno = RUBI_FONT_SIZE;
	int		alph2=alph;
	int		wait=0;
	int		speed=10;
	int		step=0;
	int		end_flag=0;
	int		amari=0;
	int		kaig=0;
	int		px_bak=0;
	int		r2=r,g2=g,b2=b;

	if(px2)	*px2 = sx;
	if(py2) *py2 = sy;
	if(step_wait) *step_wait=FALSE;
	if(str==NULL){
		if(step_wait) *step_wait=TRUE;
		return TRUE;
	}
	enum{
		TAG_DIGIT,
		TAG_COLOR,
		TAG_BRIGHT,
		TAG_KAGE,
		TAG_FONT,
		TAG_RUBI,
		TAG_ACCENT,
		TAG_SPEED,
		TAG_WAIT,
	};
	TXT_GetMsgSpeed( -1 );
	int	kage2, kage3;
	switch( kage ){
		case -1:kage2 = 0;	kage3 = 0;	break;
		case 0:	kage2 = 0;	kage3 = 0;	break;
		case 1:	kage2 = 1;	kage3 = 1;	break;
		case 2:	kage2 = 1;	kage3 = 2;	break;
	}

	
	while( cnt2 < text_cnt || text_cnt==-1 || step<step_cnt || ( cnt2==text_cnt && !amari)  ) {	

		amari = TXT_GetMsgSpeed(-2);
		if(wait){
			wait--;

			if(step>=step_cnt) cnt2++;
			continue;
		}
		if(alph==-1){
			if(text_cnt==-1){	alph2 = alph;							}
			else			{	alph2 = LIM(text_cnt-cnt2, 0, 16)*16;	}
			if(step<step_cnt){ alph2 = alph; }
		}else{
			if(text_cnt==-1){	alph2 = alph;							}
			else			{	alph2 = LIM(text_cnt-cnt2, 0, 16)*16*alph/256;	}
			if(step<step_cnt){ alph2 = alph; }
		}
		
		switch( str[cnt] ){
			case '\0':		
				if( tag_cnt ){
					DebugBox( NULL, "タグが開いたままになっています。必ず閉じてください" );
				}
				end_flag=1;
				if(step_wait && alph2>=alph) *step_wait=TRUE;
				break;
			case '\n':
				if(!kaig){
					bx = sx;
					px = sx;
					py += fno2+pich_h;
					fno2 = fno;
					if(normal_str) { *normal_str = '\n'; normal_str++; }
				}
				kaig=0;
				break;
			case '^':	
				draw_flag = 1;
				str[cnt] = ' ';

				break;
			case '~':	
				str[cnt] = ',';
				draw_flag = 1;

				break;
			case '\\':
				cnt++;
				switch( str[cnt] ){
					case 'n':			
						if(!kaig){
							bx = sx;
							px = sx;
							py += fno2+pich_h;
							fno2 = fno;

							if(normal_str) { *normal_str = '\n'; normal_str++; }
						}
						kaig=0;
						break;
					case 'k':			
						if(step>=step_cnt && step_cnt!=-1 ){
							end_flag=1;
						}
						step++;
						break;
					case '^':	case '~':
					case '<':	case '>':
					case '|':	case '\\':	
						draw_flag = 1;
						break;
				}
				break;
			case '<':		
				cnt++;
				if( tag_cnt >= 15 ){
					DebugBox( NULL, "タグを開き過ぎです。ＭＡＸ １５" );
					break;
				}
				switch( str[cnt] ){
					case 'd':	case 'D':	
						tag_param[tag_cnt] = TAG_DIGIT;
						tag_back[tag_cnt]  = 0;
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
						}
						break;
					case 'c':	case 'C':	
						tag_param[tag_cnt] = TAG_COLOR;
						tag_back[tag_cnt]  = D3DCOLOR_ARGB(255,255,255,255);
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							tag_back[tag_cnt] = digit;
						}
						break;
					case 'b':	case 'B':	
						tag_param[tag_cnt] = TAG_BRIGHT;
						tag_back[tag_cnt] = D3DCOLOR_ARGB(255,255,255,255);
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
						}
						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
						}
						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
						}
						break;
					case 'k':	case 'K':	
						tag_param[tag_cnt] = TAG_KAGE;
						tag_back[tag_cnt] = D3DCOLOR_ARGB(255,255,255,255);
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
						}
						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
						}
						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
						}
						break;

					case 'f':	case 'F':	
						tag_param[tag_cnt] = TAG_FONT;
						tag_back[tag_cnt]  = fno;
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							fno = digit;
							fno2 = max(fno,fno2);
						}
						break;
					case 'r':	case 'R':	
						tag_param[tag_cnt] = TAG_RUBI;
						tag_back[tag_cnt]  = px;
						tag_cnt++;
						break;
					case 'a':	case 'A':	
						tag_param[tag_cnt] = TAG_ACCENT;
						tag_cnt++;
						accent_flag = 1;
						break;
					case 's':	case 'S':	
						tag_param[tag_cnt] = TAG_SPEED;
						tag_back[tag_cnt]  = speed;
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							switch(digit){
								case 0:	speed = 100;break;
								case 1:	speed = 80;	break;
								case 2:	speed = 60;	break;
								case 3:	speed = 40;	break;
								case 4:	speed = 20;	break;
								case 5:	speed = 10;	break;
								case 6:	speed = 6;	break;
								case 7:	speed = 4;	break;
								case 8:	speed = 2;	break;
								case 9:	speed = 1;	break;
								case 10:speed = 0;	break;
							}
						}
						break;
					case 'w':	case 'W':	
						tag_param[tag_cnt] = TAG_WAIT;
						tag_back[tag_cnt]  = 0;
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							wait = digit;
						}
						break;
				}
				break;
			case '|':		
				cnt++;
				switch(tag_param[tag_cnt-1]){
					case TAG_RUBI:	
						{
							int		i=0,rx,pw=0;
							while(str[cnt]!='>'){
								rubi_str[i++] = str[cnt++];
							}
							rubi_str[i] = '\0';
							
							rx = ((px-tag_back[tag_cnt-1]) - i*rfno/2)/2;
							if(rx<=0){
								pw=0;
							}else{
								if( (0x21 <= rubi_str[0] && rubi_str[0] <= 0x7e) || (0xa1 <= rubi_str[0] && rubi_str[0] <= 0xdf) ){
									pw = rx*2/(i);
								}else{
									pw = rx*2/(i/2);
								}
								rx = pw/2;
							}

							rx = tag_back[tag_cnt-1]+rx;
							D3DTXT_DispTextChar( *d3dd, rx, py-rfno+5, rx+rfno*strlen(rubi_str)+kage3, py-rfno+4+rfno+kage3, d3dText[d3dTexture[d3dd->textue_no].index_no].ph,
								D3DCOLOR_ARGB(min(255,alph2),r2,g2,b2) );
						}
						cnt--;
						break;
				}
				break;

			case '>':		
				if( tag_cnt <= 0 ){
					DebugBox( NULL, "閉じタグ( > )の数が、開いた数より多くなっています" );
					break;
				}
				tag_cnt--;
				switch( tag_param[tag_cnt] ){
					case TAG_DIGIT:									break;
					case TAG_COLOR:			break;
					case TAG_BRIGHT:		break;
					case TAG_KAGE:		break;
					case TAG_FONT:
						fno = tag_back[tag_cnt];
						if(px==sx){
							fno2 = fno;
						}else{
							fno2 = max(fno,fno2);
						}
						break;
					case TAG_RUBI:									break;
					case TAG_ACCENT:	accent_flag = 0;			break;
					case TAG_SPEED:		speed = tag_back[tag_cnt];	break;
					case TAG_WAIT:									break;
				}
				break;
			default:	
				draw_flag = 1;
				break;
		}
		if(draw_flag){
			draw_flag = 0;
			if( (0x00<=str[cnt] && str[cnt]<0x80) || (0xa0<=str[cnt] && str[cnt]<0xe0) ) {
				
				if(normal_str) { *normal_str = str[cnt]; normal_str++; }

				D3DTXT_DispTextChar( *d3dd, bx, py, px+(fno+1)/2+kage3, py+fno+kage3, d3dText[d3dTexture[d3dd->textue_no].index_no].ph, D3DCOLOR_ARGB(min(255,alph2),r2,g2,b2) );

				bx = px+(fno+1)/2+kage3;

				cnt3 = TXT_GetMsgSpeed( speed );
				if(step>=step_cnt) cnt2+=cnt3;

				if(accent_flag){
					D3DTXT_DispTextChar( *d3dd, px+(fno/2-rfno/2)/2+1, py-rfno+4, px+(fno/2-rfno/2)/2+1 + rfno/2, py+4, d3dText[d3dTexture[d3dd->textue_no].index_no].ph, D3DCOLOR_ARGB(min(255,alph2),r2,g2,b2) );
				}
				
				px += fno/2+pixh_w;
				kaig=0;
			}else{	
				
				if(normal_str) { *(WORD*)normal_str = *(WORD*)&str[cnt]; normal_str+=2; }

				D3DTXT_DispTextChar( *d3dd, bx, py, px+fno+kage3, py+fno+kage3, d3dText[d3dTexture[d3dd->textue_no].index_no].ph, D3DCOLOR_ARGB(min(255,alph2),r2,g2,b2) );
				bx = px+fno+kage3;

				cnt++;

				cnt3 = TXT_GetMsgSpeed( speed );
				if(step>=step_cnt) cnt2+=cnt3;

				if(accent_flag){
					D3DTXT_DispTextChar( *d3dd, px+(fno-rfno/2)/2+1, py-rfno+4, px+(fno-rfno/2)/2+1 + rfno/2, py+4, d3dText[d3dTexture[d3dd->textue_no].index_no].ph, D3DCOLOR_ARGB(min(255,alph2),r2,g2,b2) );
				}
				
				px += fno+pixh_w;
				kaig=0;
			}
		}
		if(end_flag)break;
		cnt++;

		
		if( px-sx >= w*font-fno+1 ){
			if( tag_param[tag_cnt-1]!=TAG_RUBI ){
				if( kflag==1 ){
					if( strncmp( &str[ cnt ], "　", 2 ) )
					{
						kflag = 0;
						px_bak = px;
						bx = sx;
						px = sx;
						py += fno2+pich_h;
						fno2 = fno;
						kaig=1;
					}
				}else{
					if( strncmp( &str[ cnt ], "。", 2 ) && strncmp( &str[ cnt ], "、", 2 ) &&
						strncmp( &str[ cnt ], "？", 2 ) && strncmp( &str[ cnt ], "！", 2 ) &&
						strncmp( &str[ cnt ], "　", 2 ) && strncmp( &str[ cnt ], "）", 2 ) &&
						strncmp( &str[ cnt ], "」", 2 ) && strncmp( &str[ cnt ], "』", 2 ) &&
						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) &&
						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) &&
						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) )
					{
						px_bak = px;
						bx = sx;
						px = sx;
						py += fno2+pich_h;
						fno2 = fno;
						kaig=1;
					}else{
						kflag=1;
					}
				}
			}
		}
		if( py-sy >= h*(font+pich_h)-(fno+pich_h)+1 ){
			static int err=0;
			if(err==0){
				err=1;
				DebugBox(NULL,"行数が多すぎます");
			}
			break;
		}
	}
	; 
	if(px2){
		if(px==sx && py!=sy && (px_bak-sx <= w*font) ){
			*px2=px_bak;
			if(py2) *py2 = py- (fno2+pich_h);
		}else{
			*px2=px;
			if(py2) *py2 = py;
		}
	}else{
		if(py2) *py2 = py;
	}
	if(*step_wait){
		return TRUE;
	}else{
		return cnt2 <= text_cnt-16;
	}

}

