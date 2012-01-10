
#ifndef	_OMAKE_H_
#define _OMAKE_H_

extern void OMK_SetCgMode( int bmp_set, int grp_set, int txt_set, int mus_lay );
extern int OMK_ControlCgMode( void );

extern int OMK_ControlOldCgMode( void );

extern void OMK_SetReplayMode( int bmp_set, int grp_set, int txt_set, int mus_lay, int flag );
extern int OMK_ControlReplayMode( void );

#define MUSIC_MODE_MAX	40
extern int	MusTable[MUSIC_MODE_MAX];

extern void OMK_SetSoundMode( int bmp_set, int grp_set, int txt_set, int mus_lay );
extern int OMK_ControlSoundMode( void );

extern void OMK_SetSiyuMode( int bmp_set, int grp_set, int txt_set, int mus_lay );
extern int OMK_ControlSiyuMode( void );


#endif	//_OMAKE_H_