
#ifndef	_SAVE_H_
#define _SAVE_H_


extern BOOL SAV_LoadHeadTime( int load_no, SYSTEMTIME *sys_time );
extern BOOL SAV_LoadHead( int load_no, char *buf, SYSTEMTIME *sys_time, BMP_SET *bmp_set, int *game_time);

extern void SAV_ErrSave( char *buf );
extern void SAV_Save( int save_no );
extern void SAV_Load( int load_no );

#endif//_SAVE_H_