#ifndef	_GM_AVGENDING_H_
#define _GM_AVGENDING_H_

extern void AVG_SetEnding( int eno, DWORD time );
extern void AVG_ResetEnding( void );
extern int AVG_WaitEnding( void );
extern void AVG_ControlEnding( void );

#endif//_GM_AVGENDING_H_
