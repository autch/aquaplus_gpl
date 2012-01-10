/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - VFW codec header  -
 *
 *  Copyright(C) 2002-2003 Anonymous <xvid-devel@xvid.org>
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id$
 *
 ****************************************************************************/
#ifndef _CODEC_H_
#define _CODEC_H_

#include <vfw.h>
#include "config.h"
#include "status.h"

#define XVID_NAME_L		L"XVID"
#define XVID_DESC_L		L"XviD MPEG-4 Codec"

#define FOURCC_XVID	mmioFOURCC('X','V','I','D')
#define FOURCC_DIVX	mmioFOURCC('D','I','V','X')
#define FOURCC_DX50 mmioFOURCC('D','X','5','0')
/* yuyu		4:2:2 16bit, y-u-y-v, packed*/
#define FOURCC_YUYV	mmioFOURCC('Y','U','Y','V')
#define FOURCC_YUY2	mmioFOURCC('Y','U','Y','2')
/* yvyu		4:2:2 16bit, y-v-y-u, packed*/
#define FOURCC_YVYU	mmioFOURCC('Y','V','Y','U')
/* uyvy		4:2:2 16bit, u-y-v-y, packed */
#define FOURCC_UYVY	mmioFOURCC('U','Y','V','Y')
/* i420		y-u-v, planar */
#define FOURCC_I420	mmioFOURCC('I','4','2','0')
#define FOURCC_IYUV	mmioFOURCC('I','Y','U','V')
/* yv12		y-v-u, planar */
#define FOURCC_YV12	mmioFOURCC('Y','V','1','2')


typedef struct
{
	CONFIG config;

	// decoder
	void * dhandle;

	// encoder
	void * ehandle;
	unsigned int fincr;
	unsigned int fbase;
    status_t status;
    
    /* encoder min keyframe internal */
	int framenum;   
	int keyspacing;

	HINSTANCE m_hdll;
	int (*xvid_global_func)(void *handle, int opt, void *param1, void *param2);
	int (*xvid_encore_func)(void *handle, int opt, void *param1, void *param2);
	int (*xvid_decore_func)(void *handle, int opt, void *param1, void *param2);

	xvid_plugin_func *xvid_plugin_single_func;
	xvid_plugin_func *xvid_plugin_2pass1_func;
	xvid_plugin_func *xvid_plugin_2pass2_func;
	xvid_plugin_func *xvid_plugin_lumimasking_func;
	xvid_plugin_func *xvid_plugin_psnr_func;

} CODEC;



LRESULT compress_query(CODEC *, BITMAPINFO *, BITMAPINFO *);
LRESULT compress_get_format(CODEC *, BITMAPINFO *, BITMAPINFO *);
LRESULT compress_get_size(CODEC *, BITMAPINFO *, BITMAPINFO *);
LRESULT compress_frames_info(CODEC *, ICCOMPRESSFRAMES *);
LRESULT compress_begin(CODEC *, BITMAPINFO *, BITMAPINFO *);
LRESULT compress_end(CODEC *);
LRESULT compress(CODEC *, ICCOMPRESS *);

LRESULT decompress_query(CODEC *, BITMAPINFO *, BITMAPINFO *);
LRESULT decompress_get_format(CODEC *, BITMAPINFO *, BITMAPINFO *);
LRESULT decompress_begin(CODEC *, BITMAPINFO *, BITMAPINFO *);
LRESULT decompress_end(CODEC *);
LRESULT decompress(CODEC *, ICDECOMPRESS *);

extern int pp_dy, pp_duv, pp_dr, pp_fe; /* decoder options */

#endif /* _CODEC_H_ */
