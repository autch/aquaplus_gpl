/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Timer related header (used for internal debugging)  -
 *
 *  Copyright(C) 2002 Michael Militzer <isibaar@xvid.org>
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

#ifndef _ENCORE_TIMER_H
#define _ENCORE_TIMER_H

#if defined(_PROFILING_)

#include "../portab.h"

uint64_t count_frames;

extern void start_timer();
extern void start_global_timer();
extern void stop_dct_timer();
extern void stop_idct_timer();
extern void stop_motion_timer();
extern void stop_comp_timer();
extern void stop_edges_timer();
extern void stop_inter_timer();
extern void stop_quant_timer();
extern void stop_iquant_timer();
extern void stop_conv_timer();
extern void stop_transfer_timer();
extern void stop_coding_timer();
extern void stop_prediction_timer();
extern void stop_interlacing_timer();
extern void stop_global_timer();
extern void init_timer();
extern void write_timer();

#else

static __inline void
start_timer()
{
}
static __inline void
start_global_timer()
{
}
static __inline void
stop_dct_timer()
{
}
static __inline void
stop_idct_timer()
{
}
static __inline void
stop_motion_timer()
{
}
static __inline void
stop_comp_timer()
{
}
static __inline void
stop_edges_timer()
{
}
static __inline void
stop_inter_timer()
{
}
static __inline void
stop_quant_timer()
{
}
static __inline void
stop_iquant_timer()
{
}
static __inline void
stop_conv_timer()
{
}
static __inline void
stop_transfer_timer()
{
}
static __inline void
init_timer()
{
}
static __inline void
write_timer()
{
}
static __inline void
stop_coding_timer()
{
}
static __inline void
stop_interlacing_timer()
{
}
static __inline void
stop_prediction_timer()
{
}
static __inline void
stop_global_timer()
{
}

#endif

#endif							/* _TIMER_H_ */
