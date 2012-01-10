







#include <windows.h>
#include <mmsystem.h>
#include <winnls32.h>

#include "STD_Debug.h"
#include "STD_File.h"



#define RBUF_SIZE		4096	
#define UPPER_LIMIT		18		
#define THRESHOLD		2
#define NIL				RBUF_SIZE	

static	char	text_buf[RBUF_SIZE+UPPER_LIMIT-1];

static	int		match_position, match_length, lson[RBUF_SIZE+1], rson[RBUF_SIZE+257], dad[RBUF_SIZE+1];




static void	InitTree(void)
{
	int  i;

	for( i=RBUF_SIZE+1; i<=RBUF_SIZE+256; i++) rson[i]	= NIL;
	for( i=  0; i< RBUF_SIZE    ; i++) dad[i]	= NIL;
}




static void InsertNode( int r)
{
	int		i, p, cmp;
	char	*key;

	cmp = 1;
	key = &text_buf[r];
	p = RBUF_SIZE + 1 + key[0];
	rson[r] = lson[r] = NIL;
	match_length = 0;

	for(;;){
		if( cmp >= 0 ){
			if( rson[p] != NIL) p = rson[p];
			else { rson[p] = r; dad[r] = p; return;}
		} else {
			if( lson[p] != NIL) p = lson[p];
			else { lson[p] = r; dad[r] = p; return;}
		}
		for( i=1 ; i<UPPER_LIMIT ; i++ ) if( (cmp = key[i] - text_buf[p + i]) != 0 ) break;
		if( i > match_length ){
			match_position = p;
			if( (match_length = i) >= UPPER_LIMIT ) break;
		}
	}
	dad[r] = dad[p];  lson[r] = lson[p];  rson[r] = rson[p];
	dad[lson[p]] = r;  dad[rson[p]] = r;
	if (rson[dad[p]] == p) rson[dad[p]] = r;
	else                   lson[dad[p]] = r;
	dad[p] = NIL;  
}




static void DeleteNode( int p)
{
	int  q;
	
	if( dad[p] == NIL ) return;  
	if( rson[p] == NIL ) q = lson[p];
	else if( lson[p] == NIL ) q = rson[p];
	else {
		q = lson[p];
		if( rson[q] != NIL ){
			do {  q = rson[q];  } while( rson[q] != NIL );
			rson[dad[q]] = lson[q];  dad[lson[q]] = dad[q];
			lson[q] = lson[p];  dad[lson[p]] = q;
		}
		rson[q] = rson[p];  dad[rson[p]] = q;
	}
	dad[q] = dad[p];
	if( rson[dad[p]] == p ) rson[dad[p]] = q;  else lson[dad[p]] = q;
	dad[p] = NIL;
}





int	 InitEncode( char *fname, BYTE **dst, BYTE **src )
{
	int		size;
	if( fname == NULL ) return FALSE;

	size = STD_ReadFileMax( fname, src );
	if(size){
		*dst = (BYTE *)GlobalAlloc( GMEM_FIXED, size+8 );
	}
	return size;
}





int	 LZS_EncodeMemory( BYTE *dst, BYTE *src, int file_size )
{
	int		i, len, r, s, last_match_length, code_buf_ptr;
	BYTE	code_buf[17], mask;
	int		dst_pos=0, src_pos=0;

	CopyMemory( &dst[4], &file_size, 4 );	
	dst_pos = 8;
	if( file_size == 0 ) return FALSE;

	InitTree();
	code_buf[0] = 0;
	code_buf_ptr = mask = 1;
	s = 0;  r = RBUF_SIZE - UPPER_LIMIT;
	FillMemory( &text_buf[0], r, ' ' );

	for( len=0 ; (len<UPPER_LIMIT)&&(src_pos!=file_size) ; len++, src_pos++ ) text_buf[r+len] = src[src_pos];

	if( len == 0 ) return FALSE;
	for( i=1 ; i<=UPPER_LIMIT ; i++ ) InsertNode(r-i);
	InsertNode(r);
	
	do {
		if( match_length > len ) match_length = len;
		if( match_length <= THRESHOLD ){
			match_length = 1;
			code_buf[0] |= mask;
			code_buf[code_buf_ptr] = text_buf[r];
			code_buf_ptr++;
		} else {
			code_buf[code_buf_ptr]   = (char)match_position;
			code_buf[code_buf_ptr+1] = (char)( ((match_position>>4)&0xf0) | (match_length-(THRESHOLD+1)));
			code_buf_ptr += 2;
		}
		if( (mask<<=1) == 0 ){
			for( i=0 ; i<code_buf_ptr ; i++, dst_pos++ ){
				



				dst[dst_pos] = code_buf[i];
			}
			code_buf[0] = 0;  code_buf_ptr = mask = 1;
		}
		last_match_length = match_length;
		for( i=0 ; (i<last_match_length)&&(src_pos<file_size) ; i++,src_pos++ ){
			DeleteNode(s);
			text_buf[s] = src[src_pos];
			if( s < UPPER_LIMIT-1 ) text_buf[s+RBUF_SIZE] = src[src_pos];
			s = (s+1) & (RBUF_SIZE-1);  r = (r+1) & (RBUF_SIZE-1);
			InsertNode(r);
		}

		while( i++ < last_match_length ){
			DeleteNode(s);
			s = (s+1) & (RBUF_SIZE-1);  r = (r+1) & (RBUF_SIZE-1);
			if( --len ) InsertNode(r);
		}
	} while( len > 0 );

	if( code_buf_ptr > 1 ){
		for( i=0 ; i<code_buf_ptr ; i++, dst_pos++ ){
			



			dst[dst_pos] = code_buf[i];
		}
	}
	CopyMemory( &dst[0], &dst_pos, 4 );		

	return dst_pos;
}




int	 LZS_EncodeMemoryBmpTbl( BYTE *dst, BYTE *src, DWORD *tbl, int file_size, int key )
{
	int		i, len, r, s, last_match_length, code_buf_ptr;
	BYTE	code_buf[17], mask;
	int		dst_pos=0, src_pos=0;

	CopyMemory( &dst[4], &key, 4 );	
	CopyMemory( &dst[8], tbl, sizeof(DWORD)*256 );
	dst_pos = sizeof(DWORD)*256 + 8;
	file_size -= sizeof(DWORD)*256;
	if( file_size == 0 ) return FALSE;

	InitTree();
	code_buf[0] = 0;
	code_buf_ptr = mask = 1;
	s = 0;  r = RBUF_SIZE - UPPER_LIMIT;
	FillMemory( &text_buf[0], r, ' ' );

	for( len=0 ; (len<UPPER_LIMIT)&&(src_pos!=file_size) ; len++, src_pos++ ) text_buf[r+len] = src[src_pos];

	if( len == 0 ) return FALSE;
	for( i=1 ; i<=UPPER_LIMIT ; i++ ) InsertNode(r-i);
	InsertNode(r);

	
	do {
		if( match_length > len ) match_length = len;
		if( match_length <= THRESHOLD ){
			match_length = 1;
			code_buf[0] |= mask;
			code_buf[code_buf_ptr] = text_buf[r];
			code_buf_ptr++;
		} else {
			code_buf[code_buf_ptr]   = (char)match_position;
			code_buf[code_buf_ptr+1] = (char)( ((match_position>>4)&0xf0) | (match_length-(THRESHOLD+1)));
			code_buf_ptr += 2;
		}
		if( (mask<<=1) == 0 ){
			for( i=0 ; i<code_buf_ptr ; i++, dst_pos++ ){
				



				dst[dst_pos] = code_buf[i];
			}
			code_buf[0] = 0;  code_buf_ptr = mask = 1;
		}
		last_match_length = match_length;
		for( i=0 ; (i<last_match_length)&&(src_pos<file_size) ; i++,src_pos++ ){
			DeleteNode(s);
			text_buf[s] = src[src_pos];
			if( s < UPPER_LIMIT-1 ) text_buf[s+RBUF_SIZE] = src[src_pos];
			s = (s+1) & (RBUF_SIZE-1);  r = (r+1) & (RBUF_SIZE-1);
			InsertNode(r);
		}

		while( i++ < last_match_length ){
			DeleteNode(s);
			s = (s+1) & (RBUF_SIZE-1);  r = (r+1) & (RBUF_SIZE-1);
			if( --len ) InsertNode(r);
		}
	} while( len > 0 );

	if( code_buf_ptr > 1 ){
		for( i=0 ; i<code_buf_ptr ; i++, dst_pos++ ){
			



			dst[dst_pos] = code_buf[i];
		}
	}
	CopyMemory( &dst[0], &dst_pos, 4 );		

	return dst_pos;
}





int LZS_Encode( char *src_name, char *dst_name )
{
	BYTE	*src=NULL, *dst=NULL;
	int		file_size, encode_size;

	file_size = InitEncode( src_name, &dst, &src );
	if( file_size == 0 ){
		if(src)GlobalFree(src);
		if(dst)GlobalFree(dst);
		return 0;
	}

	encode_size = LZS_EncodeMemory( dst, src, file_size );
	if( encode_size == 0 ){
		if(src)GlobalFree(src);
		if(dst)GlobalFree(dst);
		return 0;
	}

	STD_WriteFile( dst_name, dst, encode_size );

	if(src)GlobalFree(src);
	if(dst)GlobalFree(dst);

	return encode_size;
}





static int	InitDecode( char *filename, BYTE **dst, BYTE **src, int *decode_size )
{
	DWORD	dum, type;
	HANDLE	hfile;
	int		file_size;

	if( filename == NULL ) return 0;

	hfile = CREATE_READ_FILE( filename);
	if( hfile == INVALID_HANDLE_VALUE ) return 0;

	
	file_size  = GetFileSize(hfile, NULL) - 8;
	*src = (BYTE *)GlobalAlloc( GMEM_FIXED, file_size );

	
	ReadFile( hfile, &type, 4, &dum, NULL );

	
	ReadFile( hfile, decode_size, sizeof(DWORD), &dum, NULL );
	
	ReadFile( hfile, *src, file_size, &dum, NULL );
	CloseHandle(hfile);
	
	*dst = (BYTE *)GlobalAlloc( GMEM_FIXED, *decode_size );

	return file_size;
}





int  LZS_DecodeMemory( BYTE *dst, BYTE *src, int file_size, int dst_size)
{
	int		j, c;
	int		ret_size;
	DWORD	flags = 0;
	BYTE	*dst2, *src2;
	int		src_pos = 0, dst_pos=0;
	int		err=0;

	FillMemory( text_buf, RBUF_SIZE-UPPER_LIMIT, ' ' );
	ret_size = RBUF_SIZE - UPPER_LIMIT;  flags = 0;
	dst2 = dst;
	src2 = src;

	for( ; ; flags>>=1 ){
		if( (flags&0x100) == 0 ){
			if( src_pos >= file_size ){
				DebugPrintf("ÇP");
				break;
			}
			flags = *src2 | 0xff00;
			src_pos++;	src2++;
		}

		if( src_pos >= file_size ){
			DebugPrintf("ÇQ");
			break;
		}
		c = *src2;	src2++;	src_pos++;

		if( flags & 0x01 ){
			*dst2 = text_buf[ret_size&(RBUF_SIZE-1)] = c;
			dst2++;	ret_size++;
			dst_pos++;
			if( dst_pos > dst_size ){ err=1; break; }
		} else {
			if( src_pos >= file_size ){
				DebugPrintf("ÇR");
				break;
			}
			j = *src2;	src2++;	src_pos++;

			c |= (j&0xf0) << 4;
			j = (j&0x0f) + THRESHOLD;

			for( ; j>=0 ; j--,c++,ret_size++, dst2++, dst_pos++ ){
				if( dst_pos > dst_size ){
					err=1;
					break;
				}
				*dst2 = text_buf[ret_size&(RBUF_SIZE-1)] = text_buf[c&(RBUF_SIZE-1)];
			}
			if( dst_pos > dst_size ){ 
				err=1;
				break;
			}
		}
	}
	if(err){
		DebugBox(NULL,"ÉGÉâÅ[ÇæÇ∑");
	}

	return ret_size - (RBUF_SIZE - UPPER_LIMIT);
}





BYTE *LZS_Decode( char *filename )
{
	BYTE	*src=NULL, *dst=NULL;
	int		file_size, decode_size;

	file_size = InitDecode(filename, &dst, &src, &decode_size );
	if( file_size == FALSE ) return NULL;

	LZS_DecodeMemory( dst, src, file_size, decode_size );

	GlobalFree(src);
	return dst;
}

BYTE *LZS_Decode2( BYTE *src, int file_size )
{
	DWORD	size;
	BYTE	*dst=NULL;

	
	size = *(DWORD*)(src+4);
	
	dst = (BYTE *)GlobalAlloc( GMEM_FIXED, size );
	
	LZS_DecodeMemory( dst, src+8, file_size - 8, size );
	GlobalFree(src);

	return dst;
}

