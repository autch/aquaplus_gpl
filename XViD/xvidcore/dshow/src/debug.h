#ifndef _DSHOW_DEBUG_
#define _DSHOW_DEBUG_

#ifdef _DEBUG
#include <stdio.h>	/* vsprintf */
#define DPRINTF_BUF_SZ  1024
static __inline void
DPRINTF(char *fmt, ...)
{
	va_list args;
	char buf[DPRINTF_BUF_SZ];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	OutputDebugString(buf);
}
#else
static __inline void 
DPRINTF(char *fmt, ...) { }
#endif


#endif /* _DSHOW_DEBUG */