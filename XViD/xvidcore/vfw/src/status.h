#ifndef _STATUS_H_
#define _STATUS_H_

#include <windows.h>

/* int64_t */
#if defined(_MSC_VER) || defined (__WATCOMC__)
#	define int64_t  __int64
#	define uint64_t unsigned __int64
#else
#	include <inttypes.h>
#endif

typedef struct
{
	double fps;

	HWND hDlg;
	HWND hGraph;
	HDC hDc;

	unsigned int width;
	unsigned int width31;
	unsigned int height;
	unsigned int stride;
	unsigned char * buffer;

	TEXTMETRIC tm;
	BITMAPINFO * bi;

	int64_t count[4];
	int min_quant[4];
	int max_quant[4];
	int quant[3][31];
	int max_quant_frames;
	int min_length[4];
	int max_length[4];
	int64_t tot_length[4];
} status_t;


void status_create(status_t * s, unsigned int fps_inc, unsigned int fps_base);
void status_update(status_t *s, int type, int length, int quant);
void status_destroy(status_t *s);
void status_destroy_always(status_t *s);

#endif /* _STATUS_H_ */
