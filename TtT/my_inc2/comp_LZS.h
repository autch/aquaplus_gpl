#ifndef _COMP_LZS_H_
#define _COMP_LZS_H_

extern int	InitEncode( char *fname, BYTE **dst, BYTE **src );
extern int	LZS_EncodeMemory( BYTE *dst, BYTE *src, int file_size );
extern int	LZS_EncodeMemoryBmpTbl( BYTE *dst, BYTE *src, DWORD *tbl, int file_size, int key );
extern int  LZS_Encode( char *oldName, char *newName );

extern int  LZS_DecodeMemory( BYTE *lpDst, BYTE *lpSrc, int filesize, int dst_size);

extern BYTE *LZS_Decode( char *filename );
extern BYTE *LZS_Decode2( BYTE *lpSrc, int filesize );


#endif
