
#ifndef	_GM_AVGCHAR_H_
#define _GM_AVGCHAR_H_


extern void SetCharPosShake( int shx, int shy, int disp );
extern void SetCharBright( int r, int g, int b );

extern void SetCharHalfTone( int flag );


extern int AVG_CheckCharLocate( int char_no );
extern void AVG_SetChar( int char_no, int pose, int locate, int layer, int in_type, int bright, int alph, int frame );
extern void AVG_ReleaseChar( int char_index );
extern void AVG_ResetChar( int char_no, int out_type, int frame );

extern void AVG_OpenChar( void );
extern void AVG_CloseChar( void );

extern void AVG_SetCharPose( int char_no, int pose, int in_type, int frame );
extern void AVG_SetCharLocate( int char_no, int locate, int frame );
extern void AVG_SetCharLayer( int char_no, int layer );
extern void AVG_SetCharBright( int char_no, int fade, int fade_count );
extern void AVG_SetCharAlph( int char_no, int alph, int fade_count );
extern void AVG_SetBackReleaseChar( void );
extern void AVG_SetBackChar( int x, int y, int char_disp );
extern BOOL AVG_WaitChar( int char_no );
extern void AVG_ControlChar( void );
extern void AVG_ControlSpChar( void );


extern BOOL AVG_ConfigCheckChar(void);
extern void AVG_SetSaveDataChar( AVG_SAVE_DATA	*sdata );
extern void AVG_InitChar(void);


#endif//_GM_AVGCHAR_H_