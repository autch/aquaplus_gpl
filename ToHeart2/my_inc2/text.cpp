
 
  
   
    
     
      

#include "mm_std.h"


#include "draw.h"
#include "drawprim.h"


#include "text.h"


static RGB32	FCT[32]={
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




static int GetDigit( char *str, int *cnt,int *digit )
{
	int dcnt = 0;

	*digit = 0;
	while( isdigit( str[*cnt] ) ){
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

int TXT_GetTextCountEnter( char *str )
{
	char	*buf = (char*)GAlloc( strlen(str)+1 ), *ptr;
	int		i=0,j=0;
	int		count,ret=0,space=0;

	while( str[i]!='\0' ){
		j=0;
		space=0;
		ptr = buf+i;
		while( str[i]!='\0' && str[i]!='\n' && !(str[i]=='\\'&&str[i+1]=='n') ){
			if( ('!'<=str[i] && str[i]<='~') || ('｡'<=str[i] && str[i]<='ﾟ') ){
				i++;
			}else if( ' '==str[i] ){
				i++;
				space++;
			}else{
				ptr[j++] = str[i++];
				ptr[j++] = str[i++];
			}
		}
		if(str[i]=='\\'&&str[i+1]=='n') i++;
		if(str[i]!='\0') i++;
		ptr[j] = '\0';
		count = TXT_DrawTextEx( NULL, 0, 0, 0, 256, 256, 0, 0, NULL, NULL, NULL, 20, ptr, 0, -1, -1, 128, 128, 128, 256, 0, 1 )*2+space;
		if(count>ret) ret=count;
	}

	GFree(buf);
	return ret;
}
int TXT_GetTextEndKeyWait( char *str )
{
	int	len = strlen(str);
	int	i=2;

	while(len-i>0){
		if( *(str+len-i)=='k' || *(str+len-i)=='n' || *(str+len-i)=='\\' || *(str+len-i)=='>' ){
			if( *(str+len-i)=='\\' ){
				if( *(str+len-i+1)=='k' ){
					return TRUE;
				}
			}
		}else{
			break;
		}
		i++;
	}
	return FALSE;
}


int TXT_GetTextDispStr( char *str, char *normal_str )
{
	return TXT_DrawTextEx( NULL, 0, 0, 0, 256, 256, 0, 0, NULL, NULL, NULL, 20, str, 0, -1, -1, 128, 128, 128, 256, 0, 1, normal_str );
}

int TXT_GetTextCount( char *str, int step_cnt )
{
	return TXT_DrawTextEx( NULL, 0, 0, 0, 256, 256, 0, 0, NULL, NULL, NULL, 20, str, 0, -1, step_cnt, 128, 128, 128, 256, 0, 1 );
}



int TXT_DrawText( void *dest,int draw_mode2, int sx, int sy, int w, int h, int pixh_w, int pich_h, int *px2, int *py2, RECT *clip,
					int font, char *str, int color, int text_cnt, int step_cnt, int r, int g, int b, int alph, int kage, int kaigyou_musi )
{
			TXT_DrawTextEx( dest, draw_mode2, sx, sy, w, h, pixh_w, pich_h, px2, py2, clip, font, str, color, text_cnt, step_cnt, r, g, b, alph, kage, 0, NULL, kaigyou_musi );
	return	TXT_DrawTextEx( dest, draw_mode2, sx, sy, w, h, pixh_w, pich_h, px2, py2, clip, font, str, color, text_cnt, step_cnt, r, g, b, alph,    0, 0, NULL, kaigyou_musi );
}




int	TH2_Flag=OFF;

int TXT_DrawTextEx( void *dest, int draw_mode2, int sx, int sy, int w, int h, int pixh_w, int pich_h, int *px2, int *py2, RECT *clip,
					int font, char *str, int color, int text_cnt, int step_cnt, int r, int g, int b, int alph, int kage, int cnt_flag, char *normal_str, int kaigyou_musi )
{

	int		digit;
	int		px = sx, py = sy;
	int		cnt = 0;
	int		cnt2 = 0;
	int		kflag = 0;
	int		cno = color;
	int		fno = font&0xff, fno2 = font&0xff;
	int		tag_cnt = 0;
	int		tag_param[16];
	int		tag_back[16];
	int		draw_flag=0;
	int		accent_flag=0;
	char	rubi_str[64];
	RGB32	rgb;
	int		rfno = 16;
	int		alph2=alph;
	int		wait=0;
	int		speed=10;
	int		step=0;
	int		end_flag=0;
	int		amari=0;
	int		kaig=0;
	int		px_bak=0;
	int		hankaku_err=0;
	
	static int hankaku_err_flag=0;
	if(font&0xff00 && hankaku_err_flag==0){
		hankaku_err_flag=1;
		hankaku_err=1;
	}

	font = font&0xff;
	enum{
		TAG_DIGIT,
		TAG_COLOR,
		TAG_FONT,
		TAG_RUBI,
		TAG_ACCENT,
		TAG_SPEED,
		TAG_WAIT,
	};
	TXT_GetMsgSpeed( -1 );
	if(str==NULL){
		return 0;
	}

	
	while( cnt2 < text_cnt || text_cnt==-1 || ( cnt2==text_cnt && !amari)  ) {	
		amari = TXT_GetMsgSpeed(-2);
		if(wait){
			wait--;

			cnt2++;
			continue;
		}
		if(alph==-1){
			if(text_cnt==-1){	alph2 = 256;							}
			else			{	alph2 = LIM(text_cnt-cnt2, 0, 16)*16;	}
			if(step<step_cnt){ alph2 = 256; }
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
					px_bak=px;
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
							px_bak=px;
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
						tag_back[tag_cnt]  = cno;
						tag_cnt++;

						cnt++;
						if( GetDigit( str, &cnt, &digit ) ){
							cno = digit;
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
							wait = digit*2;
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
							rgb = BMP_ChangeBright_Full( FCT[cno], r, g, b );
							
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
							if(!cnt_flag) FNT_DrawText( dest, draw_mode2, rx, py-rfno-1,pw,0, rubi_str, rfno, clip, rgb.r, rgb.g, rgb.b, alph2, (kage)?kage-1:0 );
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
				if(!TH2_Flag){
					switch( tag_param[tag_cnt] ){
						case TAG_DIGIT:									break;
						case TAG_COLOR:		cno = tag_back[tag_cnt];	break;
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
				}
				break;
			default:	
				draw_flag = 1;
				break;
		}
		if(draw_flag){
			draw_flag = 0;
			rgb = BMP_ChangeBright_Full( FCT[cno], r, g, b );
			if( (0x00<=str[cnt] && str[cnt]<0x80) || (0xa0<=str[cnt] && str[cnt]<0xe0) ) {
				
				if(normal_str) { *normal_str = str[cnt]; normal_str++; }
				if(!cnt_flag) FNT_DrawChar( dest, draw_mode2, px, py, str[cnt], fno, clip, rgb.r, rgb.g, rgb.b, alph2, kage, 1 );
				cnt2 += TXT_GetMsgSpeed( speed );

				if(accent_flag){
					if(!cnt_flag) FNT_DrawChar( dest, draw_mode2, px+(fno-rfno/2)/2, py-rfno+1, '･', rfno, clip, rgb.r, rgb.g, rgb.b, alph2, (kage)?kage-1:0, 1 );
				}
				
				px += fno/2+pixh_w;
				kaig=0;
				if(hankaku_err){
					DebugBox( NULL, "ToHert2では半角文字列は使用できません！" );
				}
			}else{	
				
				if(normal_str) { *(WORD*)normal_str = *(WORD*)&str[cnt]; normal_str+=2; }
				if(!cnt_flag) FNT_DrawChar( dest, draw_mode2, px, py, *(WORD*)&str[cnt], fno, clip, rgb.r, rgb.g, rgb.b, alph2, kage, 0 );
				cnt++;
				cnt2 += TXT_GetMsgSpeed( speed );

				if(accent_flag){
					if(!cnt_flag) FNT_DrawChar( dest, draw_mode2, px+(fno-rfno/2)/2, py-rfno+1, '･', rfno, clip, rgb.r, rgb.g, rgb.b, alph2,  (kage)?kage-1:0, 1 );
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
					if(
						strncmp( &str[ cnt ], "。", 2 ) && strncmp( &str[ cnt ], "、", 2 ) &&
						strncmp( &str[ cnt ], "，", 2 ) && strncmp( &str[ cnt ], "．", 2 ) &&
						strncmp( &str[ cnt ], "・", 2 ) && strncmp( &str[ cnt ], "…", 2 ) &&
						strncmp( &str[ cnt ], "ー", 2 ) && strncmp( &str[ cnt ], "ー", 2 ) &&
						strncmp( &str[ cnt ], "：", 2 ) && strncmp( &str[ cnt ], "；", 2 ) &&
						strncmp( &str[ cnt ], "？", 2 ) && strncmp( &str[ cnt ], "！", 2 ) &&
						strncmp( &str[ cnt ], "］", 2 ) && strncmp( &str[ cnt ], "”", 2 ) &&
						strncmp( &str[ cnt ], "　", 2 ) && strncmp( &str[ cnt ], "）", 2 ) &&
						strncmp( &str[ cnt ], "」", 2 ) && strncmp( &str[ cnt ], "』", 2 ) &&

						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) &&
						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) &&
						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) &&
						strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) && strncmp( &str[ cnt ], "", 2 ) )
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
			if( py-sy==h*(font+pich_h) && px==sx ){
			}else{
				if(err==0){
					err=1;

				}
				break;
			}
		}
	}
	if(px2){
		if(kaigyou_musi){
			*px2=px;
			if(py2) *py2 = py;
		}else{
			if(px==sx && py!=sy && (px_bak-sx <= w*font) ){
				*px2=px_bak;
				if(py2) *py2 = py- (fno2+pich_h);
			}else{
				*px2=px;
				if(py2) *py2 = py;
			}
		}
	}else{
		if(py2) *py2 = py;
	}
	
	return (cnt_flag) ? cnt2 : (px==sx)? py-sy : py-sy+fno;
}
