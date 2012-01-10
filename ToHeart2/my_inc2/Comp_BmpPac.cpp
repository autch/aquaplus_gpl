#include "mm_std.h"

#include "Comp_bmppac.h"
#include "Comp_LZS.h"
#include "Comp_pac.h"
#include "bmp.h"




typedef struct{
	HANDLE			fh;


	DWORD			fp;
	BMP_MOVE_INFO	bmi;
	DWORD			next_size[2];
	char			*buf;
	char			db_buf;
	long			count;
	long			cond;
	long			loop;
	DWORD			start_time;

	int				r;
	int				g;
	int				b;
}MOVE_INFO;

MOVE_INFO	MoveInfo[MOVIE_MAX];

void PAC_PackFileMovie_Init( void )
{
	PAC_PackFileMovie_Term();
}

void PAC_PackFileMovie_Term( void )
{
	int	i;

	for(i=0;i<MOVIE_MAX;i++){
		PAC_PackFileMovie_Close( i );
	}
}

int PAC_PackFileMovie_Open( char *PackFname )
{
	int		i;
	int		size;

	for(i=0;i<32;i++){
		if(!MoveInfo[i].fh)	break;
	}
	if(i==32) return -1;

	PAC_PackFileMovie_Close( i );

	MoveInfo[i].fh = CREATE_READ_FILE2( PackFname, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN );
	if( MoveInfo[i].fh==INVALID_HANDLE_VALUE ){
		MoveInfo[i].fh=NULL;

		return -2;
	}

	ReadFile( MoveInfo[i].fh, &MoveInfo[i].bmi, sizeof(BMP_MOVE_INFO), (unsigned long *)&size, NULL);
	ReadFile( MoveInfo[i].fh, MoveInfo[i].next_size, sizeof(DWORD)*2, (unsigned long *)&size, NULL);
	
	GFree( MoveInfo[i].buf );
	MoveInfo[i].buf    = (char *)GAlloc( MoveInfo[i].bmi.sx*MoveInfo[i].bmi.sy + 1024 + 8 );
	MoveInfo[i].db_buf = 0;

	MoveInfo[i].count      = 0;
	MoveInfo[i].start_time = 0;
	MoveInfo[i].cond       = MOVE_COND_STOP;
	MoveInfo[i].loop       = 1;
	MoveInfo[i].r          = 128;
	MoveInfo[i].g          = 128;
	MoveInfo[i].b          = 128;

	return i;
}

BOOL PAC_PackFileMovie_Close( int move_no )
{
	if(MoveInfo[move_no].buf){
		GFree( MoveInfo[move_no].buf );
	}
	if(MoveInfo[move_no].fh){
		if( CloseHandle( MoveInfo[move_no].fh ) ){
			MoveInfo[move_no].fh = NULL;
		}else{
			DebugBox( NULL,"“®‰æƒtƒ@ƒCƒ‹‚ª•Â‚¶‚Ü‚¹‚ñ");
		}
	}

	ZeroMemory( &MoveInfo[move_no], sizeof(MOVE_INFO) );
	MoveInfo[move_no].r = 128;
	MoveInfo[move_no].g = 128;
	MoveInfo[move_no].b = 128;

	return TRUE;
}

void PAC_PackFileMovie_GetSize( int move_no, int *sx, int *sy )
{
	*sx = MoveInfo[move_no].bmi.sx;
	*sy = MoveInfo[move_no].bmi.sy;
}
BOOL PAC_PackFileMovie_GetSabun( int move_no )
{
	return MoveInfo[move_no].bmi.sabun;
}

int PAC_PackFileMovie_GetCondition( int move_no )
{
	return MoveInfo[move_no].cond;
}
int PAC_PackFileMovie_GetLoop( int move_no )
{
	return MoveInfo[move_no].loop;
}

BOOL PAC_PackFileMovie_Start( int move_no, int cond, int loop )
{
	MOVE_INFO	*mi = &MoveInfo[move_no];
	BOOL		ret = FALSE;
	int			size;

	if( MoveInfo[move_no].fh ) {
		mi->db_buf     = 0;
		mi->count      = 0;
		mi->start_time = STD_timeGetTime();
		mi->cond       = cond;
		if(loop==-1){
			mi->loop = max(0,mi->loop-1);
		}else{
			mi->loop   = loop;
		}

		SetFilePointer( mi->fh, sizeof(BMP_MOVE_INFO), NULL, FILE_BEGIN );
		ReadFile( mi->fh, mi->next_size, sizeof(DWORD)*2, (unsigned long *)&size, NULL );

		ret = TRUE;
	}
	return ret;
}

BOOL PAC_PackFileMovie_Loop( int move_no, int loop )
{
	BOOL	ret = FALSE;

	if( MoveInfo[move_no].fh ){
		MoveInfo[move_no].loop = loop;
		ret = TRUE;
	}
	return ret;
}

BOOL PAC_PackFileMovie_Play( int move_no, int loop )
{
	BOOL	ret = FALSE;

	if( MoveInfo[move_no].fh ){
		if( MoveInfo[move_no].cond != MOVE_COND_PLAY ){
			MoveInfo[move_no].start_time = STD_timeGetTime()-MoveInfo[move_no].start_time;
			MoveInfo[move_no].cond = MOVE_COND_PLAY;
			MoveInfo[move_no].loop = loop;

			ret = TRUE;
		}
	}
	return ret;
}

BOOL PAC_PackFileMovie_Pause( int move_no )
{
	BOOL	ret = FALSE;

	if( MoveInfo[move_no].fh ){
		if( MoveInfo[move_no].cond == MOVE_COND_PLAY ){
			MoveInfo[move_no].cond = MOVE_COND_PAUSE;
			MoveInfo[move_no].start_time = STD_timeGetTime()-MoveInfo[move_no].start_time;
		}
	}
	return ret;
}
void PAC_PackFileMovie_Bright( int move_no, int r, int g, int b )
{
	MoveInfo[move_no].r = r;
	MoveInfo[move_no].g = g;
	MoveInfo[move_no].b = b;
}


static int PAC_DecodeMemoryBmpTbl( MOVE_INFO *mi, void *dest, int draw_mode, int bai, DWORD now_esize )
{
	BMP_T		*bmp_t = (BMP_T *)dest;
	BMP_F		*bmp_f = (BMP_F *)dest;
	BMP_H		*bmp_h = (BMP_H *)dest;
	BMP_BT		*bmp_bt = (BMP_BT *)dest;
	int			buf_point=0;
	DWORD		dsize;
	RGB32		pal[256];
	WORD		wpal[256];
	char		*src = mi->buf+sizeof(RGB32)*256;
	int			i;

	if( now_esize==(DWORD)(mi->bmi.sx*mi->bmi.sy+1024) ){
		switch( draw_mode ){
			case 8:
				CopyMemory( bmp_bt->pal, mi->buf, sizeof(RGB32)*256 );
				CopyMemory( bmp_bt->buf, src, bmp_bt->sx*bmp_bt->sy );
				break;
			case 16:
				BMP_ChangePal_TH(wpal,(RGB32*)mi->buf);
				for( i=0 ; i < bmp_h->sx*bmp_h->sy ; i++ ){
					bmp_h->buf[i] = wpal[*(src++)];
				}
				break;
			case 24:
				CopyMemory( pal, mi->buf, sizeof(RGB32)*256 );
				for( i=0 ; i < bmp_f->sx*bmp_f->sy-1 ; i++ ){
					*(RGB32*)&bmp_f->buf[i] = pal[*(src++)];
				}
				bmp_f->buf[i].r = pal[*src].r;
				bmp_f->buf[i].g = pal[*src].g;
				bmp_f->buf[i].b = pal[*src].b;
				break;
			case 32:
				CopyMemory( pal, mi->buf, sizeof(RGB32)*256 );
				for( i=0 ; i < bmp_t->sx*bmp_t->sy ; i++ ){
					bmp_t->buf[i] = pal[*(src++)];
				}
				break;
		}
	}else{
		switch( draw_mode ){
			case 8:
				buf_point = (bmp_bt->sy - mi->bmi.sy*((bai)?2:1))/2 * bmp_bt->sx;
				dsize = LZS_DecodeMemoryBmpTbl_BYTE( bmp_bt->buf+buf_point, bmp_bt->pal, mi->buf, now_esize, mi->bmi.sabun );
				break;
			case 16:
				buf_point = (bmp_h->sy - mi->bmi.sy*((bai)?2:1))/2 * bmp_h->sx;
				dsize = LZS_DecodeMemoryBmpTbl_WORD(  bmp_h->buf+buf_point, mi->buf, now_esize, mi->bmi.sabun, bai, mi->r, mi->g, mi->b );
				break;
			case 24:
				buf_point = (bmp_f->sy - mi->bmi.sy*((bai)?2:1))/2 * bmp_f->sx;
				dsize = LZS_DecodeMemoryBmpTbl_3BYTE( (char*)bmp_f->buf+buf_point, mi->buf, now_esize, mi->bmi.sabun, bai, mi->r, mi->g, mi->b );
	
				break;
			case 32:
				buf_point = (bmp_t->sy - mi->bmi.sy*((bai)?2:1))/2 * bmp_t->sx;
				dsize = LZS_DecodeMemoryBmpTbl_DWORD( bmp_t->buf+buf_point, mi->buf, now_esize, mi->bmi.sabun, bai, mi->r, mi->g, mi->b );
				break;
		}
	}
	return dsize;
}
BOOL PAC_PackFileMovie_NextRead( MOVE_INFO *mi, void *dest, int draw_mode, int bai, int count, int skip )
{
	int			i;
	DWORD		esize;
	DWORD		now_esize;
	DWORD		key_frame=0;


	for(i=0;i<count;i++){
		if( mi->bmi.sabun ){
			now_esize = mi->next_size[0];
			key_frame = mi->next_size[1]&0x7fffffff;

			mi->count += LIM(11-skip,1,10);
			ReadFile( mi->fh, mi->buf, now_esize, &esize, NULL);
			
			mi->next_size[0] = *(DWORD*)&mi->buf[now_esize-8];
			mi->next_size[1] = *(DWORD*)&mi->buf[now_esize-4];

			PAC_DecodeMemoryBmpTbl( mi, dest, draw_mode, bai, now_esize-8 );

			if(now_esize > esize){
				return FALSE;
			}
		}else{
			now_esize = mi->next_size[0];
			key_frame = mi->next_size[1]&0x7fffffff;
			mi->count += LIM(11-skip,1,10);
			
			if(i+1==count || key_frame){

				ReadFile( mi->fh, mi->buf, now_esize, &esize, NULL);

				mi->next_size[0] = *(DWORD*)&mi->buf[now_esize-8];
				mi->next_size[1] = *(DWORD*)&mi->buf[now_esize-4];
				PAC_DecodeMemoryBmpTbl( mi, dest, draw_mode, bai, now_esize-8 );

				if(now_esize > esize){
					return FALSE;
				}
				break;
			}else{
				SetFilePointer( mi->fh, now_esize-8, NULL, FILE_CURRENT );
				
				ReadFile( mi->fh, mi->next_size, 8, &esize, NULL);
				if(8 > esize){
					SetFilePointer( mi->fh, -(int)(now_esize-8), NULL, FILE_CURRENT );
					ReadFile( mi->fh, mi->buf, now_esize, &esize, NULL );
					PAC_DecodeMemoryBmpTbl( mi, dest, draw_mode, bai, now_esize-8 );
					return FALSE;
				}
			}
		}
		if( key_frame )
			break;	
	}
	return TRUE;
}

BOOL PAC_PackFileMovie_Read( int move_no, void *dest, int draw_mode, int bai, int frame, int skip )
{
	MOVE_INFO	*mi = &MoveInfo[move_no];
	int			now_time, plan_time;
	int			sleep_time, over_frame=0, draw_frame;

	if( mi->fh ){
		if(mi->cond==MOVE_COND_PLAY){
			draw_frame = max(mi->bmi.frame,1);

			now_time   = STD_timeGetTime();									
			plan_time  = mi->start_time + mi->count*100/draw_frame;	
			sleep_time = plan_time-now_time;							
			if(sleep_time<0)
				over_frame = -sleep_time*draw_frame/1000;	

			
			if(sleep_time>0){
				return mi->cond;


			}
			
			if( !PAC_PackFileMovie_NextRead( mi, dest, draw_mode, bai, over_frame+1, skip) ){	
				switch(mi->loop){
					case 0:	PAC_PackFileMovie_Start( move_no, MOVE_COND_PLAY, -1 );	break;
					case 1:	PAC_PackFileMovie_Start( move_no, MOVE_COND_END,  -1 );	break;
					default:PAC_PackFileMovie_Start( move_no, MOVE_COND_PLAY, -1 ); break;
				}
			}else{
			}
		}
	}
	return mi->cond;
}



#define RBUF_SIZE		4096	
#define UPPER_LIMIT		18		
#define THRESHOLD		2
#define NIL				RBUF_SIZE	

static	char	text_buf[RBUF_SIZE+UPPER_LIMIT-1];


int  LZS_DecodeMemoryBmpTbl_DWORD( RGB32 *dst, char *src, int file_size, int sabun, int bai, int rr, int gg, int bb )
{
	int		j, r, c;
	DWORD	flags = 0;
	RGB32	*dst2;
	char	*src2;
	int		src_pos = 0;
	RGB32	*tbl;
	int		x=0;

	FillMemory( text_buf, RBUF_SIZE-UPPER_LIMIT, ' ' );
	r = RBUF_SIZE - UPPER_LIMIT;  flags = 0;
	dst2 = dst;
	src2 = src + sizeof(DWORD)*256;
	tbl  = (RGB32*)src;
	if(rr!=128 || gg!=128 || bb!=128)
		BMP_ChangePalBright_Full( tbl, rr, gg, bb );

	src_pos += sizeof(DWORD)*256;

	if(!bai){
		if(!sabun){
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ){
					DebugPrintf("%s",src_pos);
					break;
				}

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					*dst2 = tbl[c];
					dst2++;	r++;
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ){
						DebugPrintf("%s",src_pos);
						break;
					}
					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++,r++,dst2++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						*dst2 = tbl[ text_buf[c&(RBUF_SIZE-1)] ];
					}
				}
			}
		}else{
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ){
					DebugPrintf("%s",src_pos);
					break;
				}
				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					if(c!=0xff)	*dst2 = tbl[c];
					dst2++;	r++;
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ){
						DebugPrintf("%s",src_pos);
						break;
					}
					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++,r++,dst2++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						if(text_buf[c&(RBUF_SIZE-1)]!=0xff)	*dst2 = tbl[ text_buf[c&(RBUF_SIZE-1)] ];
					}
				}
			}
		}
	}else{
		if(!sabun){
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					*dst2 = *(dst2+1) = *(dst2+640) = *(dst2+641) = tbl[c];

					dst2+=2;	r++;	x++;
					if(x>=320){
						x=0;
						dst2+=640;
					}
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						*dst2 = *(dst2+1) = *(dst2+640) = *(dst2+641) = tbl[ text_buf[c&(RBUF_SIZE-1)] ];

						dst2+=2;	r++;	x++;
						if(x>=320){
							x=0;
							dst2+=640;
						}
					}
				}
			}
		}else{
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}
				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					if(c!=0xff)
						*dst2 = *(dst2+1) = *(dst2+640) = *(dst2+641) = tbl[c];

					dst2+=2;	r++;	x++;
					if(x>=320){
						x=0;
						dst2+=640;
					}
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						if(text_buf[c&(RBUF_SIZE-1)]!=0xff)
							*dst2 = *(dst2+1) = *(dst2+640) = *(dst2+641) = tbl[ text_buf[c&(RBUF_SIZE-1)] ];

						dst2+=2;	r++;	x++;
						if(x>=320){
							x=0;
							dst2+=640;
						}
					}
				}
			}
		}
	}

	return r - (RBUF_SIZE - UPPER_LIMIT)+sizeof(DWORD)*256;
}

int  LZS_DecodeMemoryBmpTbl_3BYTE( char *dst, char *src, int file_size, int sabun, int bai, int rr, int gg, int bb )
{
	int		j, r, c;
	DWORD	flags = 0;
	char	*dst2;
	char	*src2;
	int		src_pos = 0;
	RGB32	*tbl;
	int		x=0,y=0;

	FillMemory( text_buf, RBUF_SIZE-UPPER_LIMIT, ' ' );
	r = (RBUF_SIZE - UPPER_LIMIT);  flags = 0;
	dst2 = dst;
	src2 = src + sizeof(DWORD)*256;
	tbl = (RGB32*)src;
	if(rr!=128 || gg!=128 || bb!=128)
		BMP_ChangePalBright_Full( tbl, rr, gg, bb );

	src_pos += sizeof(DWORD)*256;

	if(!bai){
		if(!sabun){
			file_size--;

			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					*(RGB32*)dst2 = tbl[c];
					dst2+=3;	r++;
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++,r++,dst2+=3 ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						*(RGB32*)dst2 = tbl[ text_buf[c&(RBUF_SIZE-1)] ];
					}
				}
			}

			if( flags & 0x01 ){
				text_buf[r&(RBUF_SIZE-1)] = c;
				*(WORD*)dst2 = *(WORD*)&tbl[c];
				*(dst2+2)    = tbl[c].r;
				dst2+=3;	r++;
			} else {
				c |= (j&0xf0) << 4;
				j = (j&0x0f) + THRESHOLD;

				for( ; j>=0 ; j--,c++,r++,dst2+=3 ){
					text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
					*(WORD*)dst2 = *(WORD*)&tbl[ text_buf[c&(RBUF_SIZE-1)] ];
					*(dst2+2)    = tbl[ text_buf[c&(RBUF_SIZE-1)] ].r;
				}
			}

		}else{
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					if(c!=0xff){
						*(WORD*)dst2 = *(WORD*)&tbl[c];
						*(dst2+2)    = tbl[c].r;
					}
					dst2+=3;	r++;
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++,r++,dst2+=3 ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						if(text_buf[c&(RBUF_SIZE-1)]!=0xff){
							*(WORD*)dst2 = *(WORD*)&tbl[ text_buf[c&(RBUF_SIZE-1)] ];
							*(dst2+2)    = tbl[ text_buf[c&(RBUF_SIZE-1)] ].r;
						}
					}
				}
			}
		}
	}else{
		if(!sabun){
			file_size--;
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					*(RGB32*)(dst2+3) = *(RGB32*)dst2 = *(RGB32*)(dst2+1923) = *(RGB32*)(dst2+1920) = tbl[c];

					dst2+=6;	r++;	x++;
					if(x>=320){
						x=0;
						dst2+=1920;
					}
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						*(RGB32*)(dst2+3) = *(RGB32*)dst2 = *(RGB32*)(dst2+1923) = *(RGB32*)(dst2+1920) = tbl[ text_buf[c&(RBUF_SIZE-1)] ];

						dst2+=6;	r++;	x++;
						if(x>=320){
							x=0;
							dst2+=1920;
						}
					}
				}
			}

			if( flags & 0x01 ){
				text_buf[r&(RBUF_SIZE-1)] = c;
				*(WORD*)(dst2+3) = *(WORD*)dst2 = *(WORD*)(dst2+641*3) = *(WORD*)(dst2+640*3) = *(WORD*)&tbl[c];
				*(dst2+3+2)      = *(dst2+2)    = *(dst2+641*3+2)      = *(dst2+640*3+2)      = tbl[c].r;

				dst2+=6;	r++;	x++;
				if(x>=320){
					x=0;
					dst2+=1920;
				}
			} else {
				c |= (j&0xf0) << 4;
				j = (j&0x0f) + THRESHOLD;

				for( ; j>=0 ; j--,c++ ){
					text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
					*(WORD*)(dst2+3) = *(WORD*)dst2 = *(WORD*)(dst2+641*3) = *(WORD*)(dst2+640*3) = *(WORD*)&tbl[ text_buf[c&(RBUF_SIZE-1)] ];
					*(dst2+3+2)      = *(dst2+2)    = *(dst2+641*3+2)      = *(dst2+640*3+2)      = tbl[ text_buf[c&(RBUF_SIZE-1)] ].r;

					dst2+=6;	r++;	x++;
					if(x>=320){
						x=0;
						dst2+=1920;
					}
				}
			}
		}else{
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;


					if(c!=0xff){
						*(WORD*)(dst2+3) = *(WORD*)dst2 = *(WORD*)(dst2+641*3) = *(WORD*)(dst2+640*3) = *(WORD*)&tbl[c];
						*(dst2+3+2)      = *(dst2+2)    = *(dst2+641*3+2)      = *(dst2+640*3+2)      = tbl[c].r;
					}

					dst2+=6;	r++;	x++;
					if(x>=320){
						x=0;

						dst2+=640*3;
					}
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						if(text_buf[c&(RBUF_SIZE-1)]!=0xff){
							*(WORD*)(dst2+3) = *(WORD*)dst2 = *(WORD*)(dst2+641*3) = *(WORD*)(dst2+640*3) = *(WORD*)&tbl[ text_buf[c&(RBUF_SIZE-1)] ];
							*(dst2+3+2)      = *(dst2+2)    = *(dst2+641*3+2)      = *(dst2+640*3+2)      = tbl[ text_buf[c&(RBUF_SIZE-1)] ].r;
						}

						dst2+=6;	r++;	x++;
						if(x>=320){
							x=0;

							dst2+=640*3;
						}
					}
				}
			}
		}
	}

	return r - (RBUF_SIZE - UPPER_LIMIT)+sizeof(DWORD)*256;
}

int  LZS_DecodeMemoryBmpTbl_WORD( WORD *dst, char *src, int file_size, int sabun, int bai, int rr, int gg, int bb )
{
	int		j, r, c;
	DWORD	flags = 0;
	WORD	*dst2;
	char	*src2;
	int		src_pos = 0;
	WORD	tbl[256];
	int		x=0;

	FillMemory( text_buf, RBUF_SIZE-UPPER_LIMIT, ' ' );
	r = RBUF_SIZE - UPPER_LIMIT;  flags = 0;
	dst2 = dst;
	src2 = src + sizeof(DWORD)*256;
	if(rr!=128 || gg!=128 || bb!=128)	BMP_ChangePalBright_TH( tbl, (RGB32*)src, rr, gg, bb );
	else								BMP_ChangePal_TH( tbl, (RGB32*)src );

	src_pos += sizeof(DWORD)*256;

	if(!bai){
		if(!sabun){
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					*dst2 = tbl[c];
					dst2++;	r++;
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++,r++,dst2++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						*dst2 = tbl[ text_buf[c&(RBUF_SIZE-1)] ];
					}
				}
			}
		}else{
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					if(c!=0xff) *dst2 = tbl[c];
					dst2++;	r++;
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++,r++,dst2++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						if(text_buf[c&(RBUF_SIZE-1)]!=0xff) *dst2 = tbl[ text_buf[c&(RBUF_SIZE-1)] ];
					}
				}
			}
		}
	}else{
		if(!sabun){
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					*dst2 = *(dst2+1) = *(dst2+640) = *(dst2+641) = tbl[c];

					dst2+=2;	r++;	x++;
					if(x>=320){
						x=0;
						dst2+=640;
					}
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						*dst2 = *(dst2+1) = *(dst2+640) = *(dst2+641) = tbl[ text_buf[c&(RBUF_SIZE-1)] ];

						dst2+=2;	r++;	x++;
						if(x>=320){
							x=0;
							dst2+=640;
						}
					}
				}
			}
		}else{
			for( ; ; flags>>=1 ){
				if( (flags&0x100) == 0 ){
					flags = *src2 | 0xff00;
					src_pos++;	src2++;
				}

				c = *src2;	src2++;	src_pos++;
				if( src_pos > file_size ) break;

				if( flags & 0x01 ){
					text_buf[r&(RBUF_SIZE-1)] = c;
					if(c!=0xff)
						*dst2 = *(dst2+1) = *(dst2+640) = *(dst2+641) = tbl[c];

					dst2+=2;	r++;	x++;
					if(x>=320){
						x=0;
						dst2+=640;
					}
				} else {
					j = *src2;	src2++;	src_pos++;
					if( src_pos > file_size ) break;

					c |= (j&0xf0) << 4;
					j = (j&0x0f) + THRESHOLD;

					for( ; j>=0 ; j--,c++ ){
						text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
						if(text_buf[c&(RBUF_SIZE-1)]!=0xff)
							*dst2 = *(dst2+1) = *(dst2+640) = *(dst2+641) = tbl[ text_buf[c&(RBUF_SIZE-1)] ];

						dst2+=2;	r++;	x++;
						if(x>=320){
							x=0;
							dst2+=640;
						}
					}
				}
			}
		}
	}

	return r - (RBUF_SIZE - UPPER_LIMIT)+sizeof(DWORD)*256;
}


int  LZS_DecodeMemoryBmpTbl_BYTE( char *dst, RGB32 *tbl, char *src, int file_size, int sabun )
{
	int		j, r, c;
	DWORD	flags = 0;
	char	*dst2;
	char	*src2;
	int		src_pos = 0;

	FillMemory( text_buf, RBUF_SIZE-UPPER_LIMIT, ' ' );
	r = RBUF_SIZE - UPPER_LIMIT;  flags = 0;
	dst2 = dst;
	src2 = src + sizeof(DWORD)*256;
	CopyMemory( tbl, src, sizeof(RGB32)*256 );
	src_pos += sizeof(DWORD)*256;

	if(!sabun){
		for( ; ; flags>>=1 ){
			if( (flags&0x100) == 0 ){
				flags = *src2 | 0xff00;
				if( src_pos >= file_size ) break;
				src_pos++;	src2++;
			}

			if( src_pos >= file_size ) break;
			c = *src2;	src2++;	src_pos++;

			if( flags & 0x01 ){
				*dst2 = text_buf[r&(RBUF_SIZE-1)] = c;
				dst2++;	r++;
			} else {
				if( src_pos >= file_size ) break;
				j = *src2;	src2++;	src_pos++;

				c |= (j&0xf0) << 4;
				j = (j&0x0f) + THRESHOLD;

				for( ; j>=0 ; j--,c++,r++,dst2++ ){
					*dst2 = text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
				}
			}
		}
	}else{
		for( ; ; flags>>=1 ){
			if( (flags&0x100) == 0 ){
				flags = *src2 | 0xff00;
				if( src_pos >= file_size ) break;
				src_pos++;	src2++;
			}

			if( src_pos >= file_size ) break;
			c = *src2;	src2++;	src_pos++;

			if( flags & 0x01 ){
				text_buf[r&(RBUF_SIZE-1)] = c;
				if(c!=0xff)
					*dst2 = c;
				dst2++;	r++;
			} else {
				if( src_pos >= file_size ) break;
				j = *src2;	src2++;	src_pos++;

				c |= (j&0xf0) << 4;
				j = (j&0x0f) + THRESHOLD;

				for( ; j>=0 ; j--,c++,r++,dst2++ ){
					text_buf[r&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
					if(text_buf[c&(RBUF_SIZE-1)]!=0xff)
						*dst2 = text_buf[c&(RBUF_SIZE-1)];
				}
			}
		}
	}
	return r - (RBUF_SIZE - UPPER_LIMIT)+sizeof(DWORD)*256;
}



