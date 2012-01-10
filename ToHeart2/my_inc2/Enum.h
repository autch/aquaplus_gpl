#ifndef _ENUM_H_
#define _ENUM_H_

#include <windows.h>


extern int	EnumGraphicsDevice( void );
extern char *GetGraphicsDeviceName( int index );

extern int EnumSoundDevice( void );
extern char *GetSoundDeviceName( int index );


#endif 