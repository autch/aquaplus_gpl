/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Encoder main module -
 *
 *  Copyright(C) 2002	  Michael Militzer <isibaar@xvid.org>
 *			   2002-2003 Peter Ross <pross@xvid.org>
 *			   2002	  Daniel Smith <danielsmith@astroboymail.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "encoder.h"
#include "prediction/mbprediction.h"
#include "global.h"
#include "utils/timer.h"
#include "image/image.h"
#include "image/font.h"
#include "motion/sad.h"
#include "motion/motion.h"
#include "motion/gmc.h"

#include "bitstream/cbp.h"
#include "utils/mbfunctions.h"
#include "bitstream/bitstream.h"
#include "bitstream/mbcoding.h"
#include "utils/emms.h"
#include "bitstream/mbcoding.h"
#include "quant/quant_matrix.h"
#include "utils/mem_align.h"

/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/

static int FrameCodeI(Encoder * pEnc,
					  Bitstream * bs);

static int FrameCodeP(Encoder * pEnc,
					  Bitstream * bs);

static void FrameCodeB(Encoder * pEnc,
					   FRAMEINFO * frame,
					   Bitstream * bs);


/*****************************************************************************
 * Encoder creation
 *
 * This function creates an Encoder instance, it allocates all necessary
 * image buffers (reference, current and bframes) and initialize the internal
 * xvid encoder paremeters according to the XVID_ENC_PARAM input parameter.
 *
 * The code seems to be very long but is very basic, mainly memory allocation
 * and cleaning code.
 *
 * Returned values :
 *	- 0				- no errors
 *	- XVID_ERR_MEMORY - the libc could not allocate memory, the function
 *						cleans the structure before exiting.
 *						pParam->handle is also set to NULL.
 *
 ****************************************************************************/

/*
 * Simplify the "fincr/fbase" fraction
*/
static void
simplify_time(int *inc, int *base)
{
	/* common factor */
	int i = *inc;
	while (i > 1) {
		if (*inc % i == 0 && *base % i == 0) {
			*inc /= i;
			*base /= i;
			i = *inc;
			continue;
		}
		i--;
	}

	if (*base > 65535 || *inc > 65535) {
		int *biggest;
		int *other;
		float div;

		if (*base > *inc) {
			biggest = base;
			other = inc;
		} else {
			biggest = inc;
			other = base;
		}
		
		div = ((float)*biggest)/((float)65535);
		*biggest = (int)(((float)*biggest)/div);
		*other = (int)(((float)*other)/div);
	}
}


int
enc_create(xvid_enc_create_t * create)
{
	Encoder *pEnc;
  int n;

	if (XVID_VERSION_MAJOR(create->version) != 1) /* v1.x.x */
		return XVID_ERR_VERSION;

	if (create->width%2 || create->height%2)
		return XVID_ERR_FAIL;

	if (create->width<=0 || create->height<=0)
		return XVID_ERR_FAIL;

	/* allocate encoder struct */

	pEnc = (Encoder *) xvid_malloc(sizeof(Encoder), CACHE_LINE);
	if (pEnc == NULL)
		return XVID_ERR_MEMORY;
	memset(pEnc, 0, sizeof(Encoder));

	pEnc->mbParam.profile = create->profile;

	/* global flags */
	pEnc->mbParam.global_flags = create->global;

	/* width, height */
	pEnc->mbParam.width = create->width;
	pEnc->mbParam.height = create->height;
	pEnc->mbParam.mb_width = (pEnc->mbParam.width + 15) / 16;
	pEnc->mbParam.mb_height = (pEnc->mbParam.height + 15) / 16;
	pEnc->mbParam.edged_width = 16 * pEnc->mbParam.mb_width + 2 * EDGE_SIZE;
	pEnc->mbParam.edged_height = 16 * pEnc->mbParam.mb_height + 2 * EDGE_SIZE;

	/* framerate */
	pEnc->mbParam.fincr = MAX(create->fincr, 0);
	pEnc->mbParam.fbase = create->fincr <= 0 ? 25 : create->fbase;
	if (pEnc->mbParam.fincr>0)
		simplify_time(&pEnc->mbParam.fincr, &pEnc->mbParam.fbase);

	/* zones */
	if(create->num_zones > 0) {
		pEnc->num_zones = create->num_zones;
		pEnc->zones = xvid_malloc(sizeof(xvid_enc_zone_t) * pEnc->num_zones, CACHE_LINE);
		if (pEnc->zones == NULL)
			goto xvid_err_memory0;
		memcpy(pEnc->zones, create->zones, sizeof(xvid_enc_zone_t) * pEnc->num_zones);
	} else {
		pEnc->num_zones = 0;
		pEnc->zones = NULL;
	}

	/* plugins */
	if(create->num_plugins > 0) {
		pEnc->num_plugins = create->num_plugins;
		pEnc->plugins = xvid_malloc(sizeof(xvid_enc_plugin_t) * pEnc->num_plugins, CACHE_LINE);
		if (pEnc->plugins == NULL)
			goto xvid_err_memory0;
	} else {
		pEnc->num_plugins = 0;
		pEnc->plugins = NULL;
	}

	for (n=0; n<pEnc->num_plugins;n++) {
		xvid_plg_create_t pcreate;
		xvid_plg_info_t pinfo;

		memset(&pinfo, 0, sizeof(xvid_plg_info_t));
		pinfo.version = XVID_VERSION;
		if (create->plugins[n].func(0, XVID_PLG_INFO, &pinfo, 0) >= 0) {
			pEnc->mbParam.plugin_flags |= pinfo.flags;
		}

		memset(&pcreate, 0, sizeof(xvid_plg_create_t));
		pcreate.version = XVID_VERSION;
		pcreate.num_zones = pEnc->num_zones;
		pcreate.zones = pEnc->zones;
		pcreate.width = pEnc->mbParam.width;
		pcreate.height = pEnc->mbParam.height;
		pcreate.mb_width = pEnc->mbParam.mb_width;
		pcreate.mb_height = pEnc->mbParam.mb_height;
		pcreate.fincr = pEnc->mbParam.fincr;
		pcreate.fbase = pEnc->mbParam.fbase;
		pcreate.param = create->plugins[n].param;

		pEnc->plugins[n].func = NULL;   /* disable plugins that fail */
		if (create->plugins[n].func(0, XVID_PLG_CREATE, &pcreate, &pEnc->plugins[n].param) >= 0) {
			pEnc->plugins[n].func = create->plugins[n].func;
		}
	}

	if ((pEnc->mbParam.global_flags & XVID_GLOBAL_EXTRASTATS_ENABLE) ||
		(pEnc->mbParam.plugin_flags & XVID_REQPSNR)) {
		pEnc->mbParam.plugin_flags |= XVID_REQORIGINAL; /* psnr calculation requires the original */
	}

	/* temp dquants */
	if ((pEnc->mbParam.plugin_flags & XVID_REQDQUANTS)) {
		pEnc->temp_dquants = (int *) xvid_malloc(pEnc->mbParam.mb_width *
						pEnc->mbParam.mb_height * sizeof(int), CACHE_LINE);
		if (pEnc->temp_dquants==NULL)
			goto xvid_err_memory1a;
	}

	/* bframes */
	pEnc->mbParam.max_bframes = MAX(create->max_bframes, 0);
	pEnc->mbParam.bquant_ratio = MAX(create->bquant_ratio, 0);
	pEnc->mbParam.bquant_offset = create->bquant_offset;

	/* min/max quant */
	for (n=0; n<3; n++) {
		pEnc->mbParam.min_quant[n] = create->min_quant[n] > 0 ? create->min_quant[n] : 2;
		pEnc->mbParam.max_quant[n] = create->max_quant[n] > 0 ? create->max_quant[n] : 31;
	}

	/* frame drop ratio */
	pEnc->mbParam.frame_drop_ratio = MAX(create->frame_drop_ratio, 0);

	/* max keyframe interval */
	pEnc->mbParam.iMaxKeyInterval = create->max_key_interval <= 0 ? (10 * (int)pEnc->mbParam.fbase) / (int)pEnc->mbParam.fincr : create->max_key_interval;

	/* allocate working frame-image memory */

	pEnc->current = xvid_malloc(sizeof(FRAMEINFO), CACHE_LINE);
	pEnc->reference = xvid_malloc(sizeof(FRAMEINFO), CACHE_LINE);

	if (pEnc->current == NULL || pEnc->reference == NULL)
		goto xvid_err_memory1;

	/* allocate macroblock memory */

	pEnc->current->mbs =
		xvid_malloc(sizeof(MACROBLOCK) * pEnc->mbParam.mb_width *
					pEnc->mbParam.mb_height, CACHE_LINE);
	pEnc->reference->mbs =
		xvid_malloc(sizeof(MACROBLOCK) * pEnc->mbParam.mb_width *
					pEnc->mbParam.mb_height, CACHE_LINE);

	if (pEnc->current->mbs == NULL || pEnc->reference->mbs == NULL)
		goto xvid_err_memory2;

	/* allocate quant matrix memory */

	pEnc->mbParam.mpeg_quant_matrices =
		xvid_malloc(sizeof(uint16_t) * 64 * 8, CACHE_LINE);

	if (pEnc->mbParam.mpeg_quant_matrices == NULL)
		goto xvid_err_memory2a;

	/* allocate interpolation image memory */

	if ((pEnc->mbParam.plugin_flags & XVID_REQORIGINAL)) {
		image_null(&pEnc->sOriginal);
		image_null(&pEnc->sOriginal2);
	}

	image_null(&pEnc->f_refh);
	image_null(&pEnc->f_refv);
	image_null(&pEnc->f_refhv);

	image_null(&pEnc->current->image);
	image_null(&pEnc->reference->image);
	image_null(&pEnc->vInterH);
	image_null(&pEnc->vInterV);
	image_null(&pEnc->vInterHV);

	if ((pEnc->mbParam.plugin_flags & XVID_REQORIGINAL)) {
		if (image_create
			(&pEnc->sOriginal, pEnc->mbParam.edged_width,
			 pEnc->mbParam.edged_height) < 0)
			goto xvid_err_memory3;

		if (image_create
			(&pEnc->sOriginal2, pEnc->mbParam.edged_width,
			 pEnc->mbParam.edged_height) < 0)
			goto xvid_err_memory3;
	}

	if (image_create
		(&pEnc->f_refh, pEnc->mbParam.edged_width,
		 pEnc->mbParam.edged_height) < 0)
		goto xvid_err_memory3;
	if (image_create
		(&pEnc->f_refv, pEnc->mbParam.edged_width,
		 pEnc->mbParam.edged_height) < 0)
		goto xvid_err_memory3;
	if (image_create
		(&pEnc->f_refhv, pEnc->mbParam.edged_width,
		 pEnc->mbParam.edged_height) < 0)
		goto xvid_err_memory3;

	if (image_create
		(&pEnc->current->image, pEnc->mbParam.edged_width,
		 pEnc->mbParam.edged_height) < 0)
		goto xvid_err_memory3;
	if (image_create
		(&pEnc->reference->image, pEnc->mbParam.edged_width,
		 pEnc->mbParam.edged_height) < 0)
		goto xvid_err_memory3;
	if (image_create
		(&pEnc->vInterH, pEnc->mbParam.edged_width,
		 pEnc->mbParam.edged_height) < 0)
		goto xvid_err_memory3;
	if (image_create
		(&pEnc->vInterV, pEnc->mbParam.edged_width,
		 pEnc->mbParam.edged_height) < 0)
		goto xvid_err_memory3;
	if (image_create
		(&pEnc->vInterHV, pEnc->mbParam.edged_width,
		 pEnc->mbParam.edged_height) < 0)
		goto xvid_err_memory3;

/* Create full bitplane for GMC, this might be wasteful */
	if (image_create
		(&pEnc->vGMC, pEnc->mbParam.edged_width,
		 pEnc->mbParam.edged_height) < 0)
		goto xvid_err_memory3;

	/* init bframe image buffers */

	pEnc->bframenum_head = 0;
	pEnc->bframenum_tail = 0;
	pEnc->flush_bframes = 0;
	pEnc->closed_bframenum = -1;

	/* B Frames specific init */
	pEnc->bframes = NULL;

	if (pEnc->mbParam.max_bframes > 0) {

		pEnc->bframes =
			xvid_malloc(pEnc->mbParam.max_bframes * sizeof(FRAMEINFO *),
						CACHE_LINE);

		if (pEnc->bframes == NULL)
			goto xvid_err_memory3;

		for (n = 0; n < pEnc->mbParam.max_bframes; n++)
			pEnc->bframes[n] = NULL;


		for (n = 0; n < pEnc->mbParam.max_bframes; n++) {
			pEnc->bframes[n] = xvid_malloc(sizeof(FRAMEINFO), CACHE_LINE);

			if (pEnc->bframes[n] == NULL)
				goto xvid_err_memory4;

			pEnc->bframes[n]->mbs =
				xvid_malloc(sizeof(MACROBLOCK) * pEnc->mbParam.mb_width *
							pEnc->mbParam.mb_height, CACHE_LINE);

			if (pEnc->bframes[n]->mbs == NULL)
				goto xvid_err_memory4;

			image_null(&pEnc->bframes[n]->image);

			if (image_create
				(&pEnc->bframes[n]->image, pEnc->mbParam.edged_width,
				 pEnc->mbParam.edged_height) < 0)
				goto xvid_err_memory4;

		}
	}

	/* init incoming frame queue */
	pEnc->queue_head = 0;
	pEnc->queue_tail = 0;
	pEnc->queue_size = 0;

	pEnc->queue =
		xvid_malloc((pEnc->mbParam.max_bframes+1) * sizeof(QUEUEINFO),
					CACHE_LINE);

	if (pEnc->queue == NULL)
		goto xvid_err_memory4;

	for (n = 0; n < pEnc->mbParam.max_bframes+1; n++)
		image_null(&pEnc->queue[n].image);


	for (n = 0; n < pEnc->mbParam.max_bframes+1; n++) {
		if (image_create
			(&pEnc->queue[n].image, pEnc->mbParam.edged_width,
			 pEnc->mbParam.edged_height) < 0)
			goto xvid_err_memory5;
	}

	/* timestamp stuff */

	pEnc->mbParam.m_stamp = 0;
	pEnc->m_framenum = 0;
	pEnc->current->stamp = 0;
	pEnc->reference->stamp = 0;

	/* other stuff */

	pEnc->iFrameNum = 0;
	pEnc->fMvPrevSigma = -1;

	create->handle = (void *) pEnc;

	init_timer();
	init_mpeg_matrix(pEnc->mbParam.mpeg_quant_matrices);

	return 0;   /* ok */

	/*
	 * We handle all XVID_ERR_MEMORY here, this makes the code lighter
	 */

  xvid_err_memory5:

	for (n = 0; n < pEnc->mbParam.max_bframes+1; n++) {
			image_destroy(&pEnc->queue[n].image, pEnc->mbParam.edged_width,
						  pEnc->mbParam.edged_height);
		}

	xvid_free(pEnc->queue);

  xvid_err_memory4:

	if (pEnc->mbParam.max_bframes > 0) {
		int i;

		for (i = 0; i < pEnc->mbParam.max_bframes; i++) {

			if (pEnc->bframes[i] == NULL)
				continue;

			image_destroy(&pEnc->bframes[i]->image, pEnc->mbParam.edged_width,
						  pEnc->mbParam.edged_height);
			xvid_free(pEnc->bframes[i]->mbs);
			xvid_free(pEnc->bframes[i]);
		}

		xvid_free(pEnc->bframes);
	}

  xvid_err_memory3:

	if ((pEnc->mbParam.plugin_flags & XVID_REQORIGINAL)) {
		image_destroy(&pEnc->sOriginal, pEnc->mbParam.edged_width,
					  pEnc->mbParam.edged_height);
		image_destroy(&pEnc->sOriginal2, pEnc->mbParam.edged_width,
					  pEnc->mbParam.edged_height);
	}

	image_destroy(&pEnc->f_refh, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->f_refv, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->f_refhv, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);

	image_destroy(&pEnc->current->image, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->reference->image, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->vInterH, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->vInterV, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->vInterHV, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);

/* destroy GMC image */
	image_destroy(&pEnc->vGMC, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);

  xvid_err_memory2a:
	xvid_free(pEnc->mbParam.mpeg_quant_matrices);

  xvid_err_memory2:
	xvid_free(pEnc->current->mbs);
	xvid_free(pEnc->reference->mbs);

  xvid_err_memory1:
	xvid_free(pEnc->current);
	xvid_free(pEnc->reference);

  xvid_err_memory1a:
	if ((pEnc->mbParam.plugin_flags & XVID_REQDQUANTS)) {
		xvid_free(pEnc->temp_dquants);
	}

  xvid_err_memory0:
	for (n=0; n<pEnc->num_plugins;n++) {
		if (pEnc->plugins[n].func) {
			pEnc->plugins[n].func(pEnc->plugins[n].param, XVID_PLG_DESTROY, 0, 0);
		}
	}
	xvid_free(pEnc->plugins);

	xvid_free(pEnc->zones);

	xvid_free(pEnc);

	create->handle = NULL;

	return XVID_ERR_MEMORY;
}

/*****************************************************************************
 * Encoder destruction
 *
 * This function destroy the entire encoder structure created by a previous
 * successful enc_create call.
 *
 * Returned values (for now only one returned value) :
 *	- 0	 - no errors
 *
 ****************************************************************************/

int
enc_destroy(Encoder * pEnc)
{
	int i;

	/* B Frames specific */
	for (i = 0; i < pEnc->mbParam.max_bframes+1; i++) {
		image_destroy(&pEnc->queue[i].image, pEnc->mbParam.edged_width,
					  pEnc->mbParam.edged_height);
	}

	xvid_free(pEnc->queue);

	if (pEnc->mbParam.max_bframes > 0) {

		for (i = 0; i < pEnc->mbParam.max_bframes; i++) {

			if (pEnc->bframes[i] == NULL)
				continue;

			image_destroy(&pEnc->bframes[i]->image, pEnc->mbParam.edged_width,
					  pEnc->mbParam.edged_height);
			xvid_free(pEnc->bframes[i]->mbs);
			xvid_free(pEnc->bframes[i]);
		}

		xvid_free(pEnc->bframes);

	}

	/* All images, reference, current etc ... */

	image_destroy(&pEnc->current->image, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->reference->image, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->vInterH, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->vInterV, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->vInterHV, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->f_refh, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->f_refv, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->f_refhv, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);
	image_destroy(&pEnc->vGMC, pEnc->mbParam.edged_width,
				  pEnc->mbParam.edged_height);

	if ((pEnc->mbParam.plugin_flags & XVID_REQORIGINAL)) {
		image_destroy(&pEnc->sOriginal, pEnc->mbParam.edged_width,
					  pEnc->mbParam.edged_height);
		image_destroy(&pEnc->sOriginal2, pEnc->mbParam.edged_width,
					  pEnc->mbParam.edged_height);
	}

	/* Encoder structure */

	xvid_free(pEnc->current->mbs);
	xvid_free(pEnc->current);

	xvid_free(pEnc->reference->mbs);
	xvid_free(pEnc->reference);

	if ((pEnc->mbParam.plugin_flags & XVID_REQDQUANTS)) {
		xvid_free(pEnc->temp_dquants);
	}


	if (pEnc->num_plugins>0) {
		xvid_plg_destroy_t pdestroy;
		memset(&pdestroy, 0, sizeof(xvid_plg_destroy_t));

		pdestroy.version = XVID_VERSION;
		pdestroy.num_frames = pEnc->m_framenum;

		for (i=0; i<pEnc->num_plugins;i++) {
			if (pEnc->plugins[i].func) {
				pEnc->plugins[i].func(pEnc->plugins[i].param, XVID_PLG_DESTROY, &pdestroy, 0);
			}
		}
		xvid_free(pEnc->plugins);
	}

	xvid_free(pEnc->mbParam.mpeg_quant_matrices);

	if (pEnc->num_plugins>0)
		xvid_free(pEnc->zones);

	xvid_free(pEnc);

	return 0;  /* ok */
}


/*
  call the plugins
  */

static void call_plugins(Encoder * pEnc, FRAMEINFO * frame, IMAGE * original,
						 int opt, int * type, int * quant, xvid_enc_stats_t * stats)
{
	unsigned int i, j;
	xvid_plg_data_t data;

	/* set data struct */

	memset(&data, 0, sizeof(xvid_plg_data_t));
	data.version = XVID_VERSION;

	/* find zone */
	for(i=0; i<pEnc->num_zones && pEnc->zones[i].frame<=frame->frame_num; i++) ;
	data.zone = i>0 ? &pEnc->zones[i-1] : NULL;

	data.width = pEnc->mbParam.width;
	data.height = pEnc->mbParam.height;
	data.mb_width = pEnc->mbParam.mb_width;
	data.mb_height = pEnc->mbParam.mb_height;
	data.fincr = frame->fincr;
	data.fbase = pEnc->mbParam.fbase;
	data.bquant_ratio = pEnc->mbParam.bquant_ratio;
	data.bquant_offset = pEnc->mbParam.bquant_offset;

	for (i=0; i<3; i++) {
		data.min_quant[i] = pEnc->mbParam.min_quant[i];
		data.max_quant[i] = pEnc->mbParam.max_quant[i];
	}

	data.reference.csp = XVID_CSP_PLANAR;
	data.reference.plane[0] = pEnc->reference->image.y;
	data.reference.plane[1] = pEnc->reference->image.u;
	data.reference.plane[2] = pEnc->reference->image.v;
	data.reference.stride[0] = pEnc->mbParam.edged_width;
	data.reference.stride[1] = pEnc->mbParam.edged_width/2;
	data.reference.stride[2] = pEnc->mbParam.edged_width/2;

	data.current.csp = XVID_CSP_PLANAR;
	data.current.plane[0] = frame->image.y;
	data.current.plane[1] = frame->image.u;
	data.current.plane[2] = frame->image.v;
	data.current.stride[0] = pEnc->mbParam.edged_width;
	data.current.stride[1] = pEnc->mbParam.edged_width/2;
	data.current.stride[2] = pEnc->mbParam.edged_width/2;

	data.frame_num = frame->frame_num;

	if (opt == XVID_PLG_BEFORE) {
		data.type = *type;
		data.quant = *quant;

		data.vol_flags = frame->vol_flags;
		data.vop_flags = frame->vop_flags;
		data.motion_flags = frame->motion_flags;

	} else if (opt == XVID_PLG_FRAME) {
		data.type = coding2type(frame->coding_type);
		data.quant = frame->quant;

		if ((pEnc->mbParam.plugin_flags & XVID_REQDQUANTS)) {
			data.dquant = pEnc->temp_dquants;
			data.dquant_stride = pEnc->mbParam.mb_width;
			memset(data.dquant, 0, data.mb_width*data.mb_height);
		}
	
	} else { /* XVID_PLG_AFTER */
		if ((pEnc->mbParam.plugin_flags & XVID_REQORIGINAL)) {
			data.original.csp = XVID_CSP_PLANAR;
			data.original.plane[0] = original->y;
			data.original.plane[1] = original->u;
			data.original.plane[2] = original->v;
			data.original.stride[0] = pEnc->mbParam.edged_width;
			data.original.stride[1] = pEnc->mbParam.edged_width/2;
			data.original.stride[2] = pEnc->mbParam.edged_width/2;
		}

		if ((frame->vol_flags & XVID_VOL_EXTRASTATS) ||
			(pEnc->mbParam.plugin_flags & XVID_REQPSNR)) {

 			data.sse_y =
				plane_sse( original->y, frame->image.y,
						   pEnc->mbParam.edged_width, pEnc->mbParam.width,
						   pEnc->mbParam.height);

			data.sse_u =
				plane_sse( original->u, frame->image.u,
						   pEnc->mbParam.edged_width/2, pEnc->mbParam.width/2,
						   pEnc->mbParam.height/2);

 			data.sse_v =
				plane_sse( original->v, frame->image.v,
						   pEnc->mbParam.edged_width/2, pEnc->mbParam.width/2,
						   pEnc->mbParam.height/2);
		}

		data.type = coding2type(frame->coding_type);
		data.quant = frame->quant;

		if ((pEnc->mbParam.plugin_flags & XVID_REQDQUANTS)) {
			data.dquant = pEnc->temp_dquants;
			data.dquant_stride = pEnc->mbParam.mb_width;

			for (j=0; j<pEnc->mbParam.mb_height; j++)
			for (i=0; i<pEnc->mbParam.mb_width; i++) {
				data.dquant[j*data.dquant_stride + i] = frame->mbs[j*pEnc->mbParam.mb_width + i].dquant;
			}
		}

		data.vol_flags = frame->vol_flags;
		data.vop_flags = frame->vop_flags;
		data.motion_flags = frame->motion_flags;

		data.length = frame->length;
		data.kblks = frame->sStat.kblks;
		data.mblks = frame->sStat.mblks;
		data.ublks = frame->sStat.ublks;

		/* New code */
		data.stats.type      = coding2type(frame->coding_type);
		data.stats.quant     = frame->quant;
		data.stats.vol_flags = frame->vol_flags;
		data.stats.vop_flags = frame->vop_flags;
		data.stats.length    = frame->length;
		data.stats.hlength   = frame->length - (frame->sStat.iTextBits / 8);
		data.stats.kblks     = frame->sStat.kblks;
		data.stats.mblks     = frame->sStat.mblks;
		data.stats.ublks     = frame->sStat.ublks;
		data.stats.sse_y     = data.sse_y;
		data.stats.sse_u     = data.sse_u;
		data.stats.sse_v     = data.sse_v;

		if (stats)
			*stats = data.stats;
	}

	/* call plugins */
	for (i=0; i<(unsigned int)pEnc->num_plugins;i++) {
		emms();
		if (pEnc->plugins[i].func) {
			if (pEnc->plugins[i].func(pEnc->plugins[i].param, opt, &data, 0) < 0) {
				continue;
			}
		}
	}
	emms();

	/* copy modified values back into frame*/
	if (opt == XVID_PLG_BEFORE) {
		*type = data.type;
		*quant = data.quant > 0 ? data.quant : 2;   /* default */

		frame->vol_flags = data.vol_flags;
		frame->vop_flags = data.vop_flags;
		frame->motion_flags = data.motion_flags;
	
	} else if (opt == XVID_PLG_FRAME) {

		if ((pEnc->mbParam.plugin_flags & XVID_REQDQUANTS)) {
			for (j=0; j<pEnc->mbParam.mb_height; j++)
			for (i=0; i<pEnc->mbParam.mb_width; i++) {
				frame->mbs[j*pEnc->mbParam.mb_width + i].dquant = data.dquant[j*data.mb_width + i];
			}
		} else {
			for (j=0; j<pEnc->mbParam.mb_height; j++)
			for (i=0; i<pEnc->mbParam.mb_width; i++) {
				frame->mbs[j*pEnc->mbParam.mb_width + i].dquant = 0;
			}
		}
		frame->mbs[0].quant = data.quant; /* FRAME will not affect the quant in stats */
	}


}


static __inline void inc_frame_num(Encoder * pEnc)
{
	pEnc->current->frame_num = pEnc->m_framenum;
	pEnc->current->stamp = pEnc->mbParam.m_stamp;	/* first frame is zero */

	pEnc->mbParam.m_stamp += pEnc->current->fincr;
	pEnc->m_framenum++;	/* debug ticker */
}

static __inline void dec_frame_num(Encoder * pEnc)
{
	pEnc->mbParam.m_stamp -= pEnc->mbParam.fincr;
	pEnc->m_framenum--;	/* debug ticker */
}

static __inline void 
MBSetDquant(MACROBLOCK * pMB, int x, int y, MBParam * mbParam)
{
	if (pMB->cbp == 0) {
		/* we want to code dquant but the quantizer value will not be used yet
			let's find out if we can postpone dquant to next MB
		*/	
		if (x == mbParam->mb_width-1 && y == mbParam->mb_height-1) {
			pMB->dquant = 0; /* it's the last MB of all, the easiest case */
			return;
		} else {
			MACROBLOCK * next = pMB + 1;
			const MACROBLOCK * prev = pMB - 1;
			if (next->mode != MODE_INTER4V && next->mode != MODE_NOT_CODED)
				/* mode allows dquant change in the future */
				if (abs(next->quant - prev->quant) <= 2) {
					/* quant change is not out of range */
					pMB->quant = prev->quant;
					pMB->dquant = 0;
					next->dquant = next->quant - prev->quant;
					return;
				}
		}		
	}
	/* couldn't skip this dquant */
	pMB->mode = MODE_INTER_Q;
}
			


static __inline void
set_timecodes(FRAMEINFO* pCur,FRAMEINFO *pRef, int32_t time_base)
{

	pCur->ticks = (int32_t)pCur->stamp % time_base;
	pCur->seconds =  ((int32_t)pCur->stamp / time_base)	- ((int32_t)pRef->stamp / time_base) ;

#if 0 	/* HEAVY DEBUG OUTPUT */
	fprintf(stderr,"WriteVop:   %d - %d \n",
			((int32_t)pCur->stamp / time_base), ((int32_t)pRef->stamp / time_base));
	fprintf(stderr,"set_timecodes: VOP %1d   stamp=%lld ref_stamp=%lld  base=%d\n",
			pCur->coding_type, pCur->stamp, pRef->stamp, time_base);
	fprintf(stderr,"set_timecodes: VOP %1d   seconds=%d   ticks=%d   (ref-sec=%d  ref-tick=%d)\n",
			pCur->coding_type, pCur->seconds, pCur->ticks, pRef->seconds, pRef->ticks);
#endif
}

static int
gcd(int a, int b)
{
	int r ;

	if (b > a) {
		r = a;
		a = b;
		b = r;
	}

	while ((r = a % b)) {
		a = b;
		b = r;
	}
	return b;
}

static void
simplify_par(int *par_width, int *par_height)
{

	int _par_width  = (!*par_width)  ? 1 : (*par_width<0)  ? -*par_width:  *par_width;
	int _par_height = (!*par_height) ? 1 : (*par_height<0) ? -*par_height: *par_height;
	int divisor = gcd(_par_width, _par_height);

	_par_width  /= divisor;
	_par_height /= divisor;

	/* 2^8 precision maximum */
	if (_par_width>255 || _par_height>255) {
		float div;
		emms();
		if (_par_width>_par_height)
			div = (float)_par_width/255;
		else
			div = (float)_par_height/255;

		_par_width  = (int)((float)_par_width/div);
		_par_height = (int)((float)_par_height/div);
	}

	*par_width = _par_width;
	*par_height = _par_height;

	return;
}


/*****************************************************************************
 * IPB frame encoder entry point
 *
 * Returned values :
 *	- >0			   - output bytes
 *	- 0				- no output
 *	- XVID_ERR_VERSION - wrong version passed to core
 *	- XVID_ERR_END	 - End of stream reached before end of coding
 *	- XVID_ERR_FORMAT  - the image subsystem reported the image had a wrong
 *						 format
 ****************************************************************************/


int
enc_encode(Encoder * pEnc,
			   xvid_enc_frame_t * xFrame,
			   xvid_enc_stats_t * stats)
{
	xvid_enc_frame_t * frame;
	int type;
	Bitstream bs;

	if (XVID_VERSION_MAJOR(xFrame->version) != 1 || (stats && XVID_VERSION_MAJOR(stats->version) != 1))	/* v1.x.x */
		return XVID_ERR_VERSION;

	xFrame->out_flags = 0;

	start_global_timer();
	BitstreamInit(&bs, xFrame->bitstream, 0);


	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * enqueue image to the encoding-queue
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (xFrame->input.csp != XVID_CSP_NULL)
	{
		QUEUEINFO * q = &pEnc->queue[pEnc->queue_tail];

		start_timer();
		if (image_input
			(&q->image, pEnc->mbParam.width, pEnc->mbParam.height,
			pEnc->mbParam.edged_width, (uint8_t**)xFrame->input.plane, xFrame->input.stride,
			xFrame->input.csp, xFrame->vol_flags & XVID_VOL_INTERLACING))
		{
			emms();
			return XVID_ERR_FORMAT;
		}
		stop_conv_timer();

		if ((xFrame->vop_flags & XVID_VOP_CHROMAOPT)) {
			image_chroma_optimize(&q->image,
				pEnc->mbParam.width, pEnc->mbParam.height, pEnc->mbParam.edged_width);
		}

		q->frame = *xFrame;

		if (xFrame->quant_intra_matrix)
		{
			memcpy(q->quant_intra_matrix, xFrame->quant_intra_matrix, 64*sizeof(unsigned char));
			q->frame.quant_intra_matrix = q->quant_intra_matrix;
		}

		if (xFrame->quant_inter_matrix)
		{
			memcpy(q->quant_inter_matrix, xFrame->quant_inter_matrix, 64*sizeof(unsigned char));
			q->frame.quant_inter_matrix = q->quant_inter_matrix;
		}

		pEnc->queue_tail = (pEnc->queue_tail + 1) % (pEnc->mbParam.max_bframes+1);
		pEnc->queue_size++;
	}


	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * bframe flush code
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

repeat:

	if (pEnc->flush_bframes)
	{
		if (pEnc->bframenum_head < pEnc->bframenum_tail) {

			DPRINTF(XVID_DEBUG_DEBUG,"*** BFRAME (flush) bf: head=%i tail=%i   queue: head=%i tail=%i size=%i\n",
					pEnc->bframenum_head, pEnc->bframenum_tail,
					pEnc->queue_head, pEnc->queue_tail, pEnc->queue_size);

			if ((pEnc->mbParam.plugin_flags & XVID_REQORIGINAL)) {
				image_copy(&pEnc->sOriginal2, &pEnc->bframes[pEnc->bframenum_head]->image,
						   pEnc->mbParam.edged_width, pEnc->mbParam.height);
			}

			FrameCodeB(pEnc, pEnc->bframes[pEnc->bframenum_head], &bs);
			call_plugins(pEnc, pEnc->bframes[pEnc->bframenum_head], &pEnc->sOriginal2, XVID_PLG_AFTER, 0, 0, stats);
			pEnc->bframenum_head++;

			goto done;
		}

		/* write an empty marker to the bitstream.

		   for divx5 decoder compatibility, this marker must consist
		   of a not-coded p-vop, with a time_base of zero, and time_increment
		   indentical to the future-referece frame.
		*/

		if ((pEnc->mbParam.global_flags & XVID_GLOBAL_PACKED && pEnc->bframenum_tail > 0)) {
			int tmp;
			int bits;

			DPRINTF(XVID_DEBUG_DEBUG,"*** EMPTY bf: head=%i tail=%i   queue: head=%i tail=%i size=%i\n",
				pEnc->bframenum_head, pEnc->bframenum_tail,
				pEnc->queue_head, pEnc->queue_tail, pEnc->queue_size);

			bits = BitstreamPos(&bs);

			tmp = pEnc->current->seconds;
			pEnc->current->seconds = 0; /* force time_base = 0 */

			BitstreamWriteVopHeader(&bs, &pEnc->mbParam, pEnc->current, 0, pEnc->current->quant);
			BitstreamPad(&bs);
			pEnc->current->seconds = tmp;

			/* add the not-coded length to the reference frame size */
			pEnc->current->length += (BitstreamPos(&bs) - bits) / 8;
			call_plugins(pEnc, pEnc->current, &pEnc->sOriginal, XVID_PLG_AFTER, 0, 0, stats);

			/* flush complete: reset counters */
			pEnc->flush_bframes = 0;
			pEnc->bframenum_head = pEnc->bframenum_tail = 0;
			goto done;

		}

		/* flush complete: reset counters */
		pEnc->flush_bframes = 0;
		pEnc->bframenum_head = pEnc->bframenum_tail = 0;
	}

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * dequeue frame from the encoding queue
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (pEnc->queue_size == 0)		/* empty */
	{
		if (xFrame->input.csp == XVID_CSP_NULL)	/* no futher input */
		{

			DPRINTF(XVID_DEBUG_DEBUG,"*** FINISH bf: head=%i tail=%i   queue: head=%i tail=%i size=%i\n",
				pEnc->bframenum_head, pEnc->bframenum_tail,
				pEnc->queue_head, pEnc->queue_tail, pEnc->queue_size);

			if (!(pEnc->mbParam.global_flags & XVID_GLOBAL_PACKED) && pEnc->mbParam.max_bframes > 0) {
				call_plugins(pEnc, pEnc->current, &pEnc->sOriginal, XVID_PLG_AFTER, 0, 0, stats);
			}

			/* if the very last frame is to be b-vop, we must change it to a p-vop */
			if (pEnc->bframenum_tail > 0) {

				SWAP(FRAMEINFO*, pEnc->current, pEnc->reference);
				pEnc->bframenum_tail--;
				SWAP(FRAMEINFO*, pEnc->current, pEnc->bframes[pEnc->bframenum_tail]);

				/* convert B-VOP to P-VOP */
				pEnc->current->quant  = 100*pEnc->current->quant - pEnc->mbParam.bquant_offset;
				pEnc->current->quant += pEnc->mbParam.bquant_ratio - 1; /* to avoid rouding issues */
				pEnc->current->quant /= pEnc->mbParam.bquant_ratio;

				if ((pEnc->mbParam.plugin_flags & XVID_REQORIGINAL)) {
					image_copy(&pEnc->sOriginal, &pEnc->current->image,
						   pEnc->mbParam.edged_width, pEnc->mbParam.height);
				}

				DPRINTF(XVID_DEBUG_DEBUG,"*** PFRAME bf: head=%i tail=%i   queue: head=%i tail=%i size=%i\n",
				pEnc->bframenum_head, pEnc->bframenum_tail,
				pEnc->queue_head, pEnc->queue_tail, pEnc->queue_size);
				pEnc->mbParam.frame_drop_ratio = -1; /* it must be a coded vop */

				FrameCodeP(pEnc, &bs);


				if ((pEnc->mbParam.global_flags & XVID_GLOBAL_PACKED) && pEnc->bframenum_tail==0) {
					call_plugins(pEnc, pEnc->current, &pEnc->sOriginal, XVID_PLG_AFTER, 0, 0, stats);
				}else{
					pEnc->flush_bframes = 1;
					goto done;
				}
			}
			DPRINTF(XVID_DEBUG_DEBUG, "*** END\n");

			emms();
			return XVID_ERR_END;	/* end of stream reached */
		}
		goto done;	/* nothing to encode yet; encoder lag */
	}

	/* the current FRAME becomes the reference */
	SWAP(FRAMEINFO*, pEnc->current, pEnc->reference);

	/* remove frame from encoding-queue (head), and move it into the current */
	image_swap(&pEnc->current->image, &pEnc->queue[pEnc->queue_head].image);
	frame = &pEnc->queue[pEnc->queue_head].frame;
	pEnc->queue_head = (pEnc->queue_head + 1) % (pEnc->mbParam.max_bframes+1);
	pEnc->queue_size--;


	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * init pEnc->current fields
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	pEnc->current->fincr = pEnc->mbParam.fincr>0 ? pEnc->mbParam.fincr : frame->fincr;
	inc_frame_num(pEnc);
	pEnc->current->vol_flags = frame->vol_flags;
	pEnc->current->vop_flags = frame->vop_flags;
	pEnc->current->motion_flags = frame->motion;
	pEnc->current->fcode = pEnc->mbParam.m_fcode;
	pEnc->current->bcode = pEnc->mbParam.m_fcode;


	if ((xFrame->vop_flags & XVID_VOP_CHROMAOPT)) {
		image_chroma_optimize(&pEnc->current->image,
			pEnc->mbParam.width, pEnc->mbParam.height, pEnc->mbParam.edged_width);
	}

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * frame type & quant selection
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	type = frame->type;
	pEnc->current->quant = frame->quant;

	call_plugins(pEnc, pEnc->current, NULL, XVID_PLG_BEFORE, &type, &pEnc->current->quant, stats);

	if (type > 0){ 	/* XVID_TYPE_?VOP */
		type = type2coding(type);	/* convert XVID_TYPE_?VOP to bitstream coding type */
	} else{		/* XVID_TYPE_AUTO */
		if (pEnc->iFrameNum == 0 || (pEnc->mbParam.iMaxKeyInterval > 0 && pEnc->iFrameNum >= pEnc->mbParam.iMaxKeyInterval)){
			pEnc->iFrameNum = 0;
			type = I_VOP;
		}else{
			type = MEanalysis(&pEnc->reference->image, pEnc->current,
							  &pEnc->mbParam, pEnc->mbParam.iMaxKeyInterval,
							  pEnc->iFrameNum, pEnc->bframenum_tail, xFrame->bframe_threshold,
							  (pEnc->bframes) ? pEnc->bframes[pEnc->bframenum_head]->mbs: NULL);
		}
	}

	if (type != I_VOP) 
		pEnc->current->vol_flags = pEnc->mbParam.vol_flags; /* don't allow VOL changes here */

	/* bframes buffer overflow check */
	if (type == B_VOP && pEnc->bframenum_tail >= pEnc->mbParam.max_bframes) {
		type = P_VOP;
	}

	pEnc->iFrameNum++;

	if ((pEnc->current->vop_flags & XVID_VOP_DEBUG)) {
		image_printf(&pEnc->current->image, pEnc->mbParam.edged_width, pEnc->mbParam.height, 5, 5,
			"%d  st:%lld  if:%d", pEnc->current->frame_num, pEnc->current->stamp, pEnc->iFrameNum);
	}

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * encode this frame as a b-vop
	 * (we dont encode here, rather we store the frame in the bframes queue, to be encoded later)
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	if (type == B_VOP) {
		if ((pEnc->current->vop_flags & XVID_VOP_DEBUG)) {
			image_printf(&pEnc->current->image, pEnc->mbParam.edged_width, pEnc->mbParam.height, 5, 200, "BVOP");
		}

		if (frame->quant < 1) {
			pEnc->current->quant = ((((pEnc->reference->quant + pEnc->current->quant) *
				pEnc->mbParam.bquant_ratio) / 2) + pEnc->mbParam.bquant_offset)/100;

		} else {
			pEnc->current->quant = frame->quant;
		}

		if (pEnc->current->quant < 1)
			pEnc->current->quant = 1;
		else if (pEnc->current->quant > 31)
			pEnc->current->quant = 31;

		DPRINTF(XVID_DEBUG_DEBUG,"*** BFRAME (store) bf: head=%i tail=%i   queue: head=%i tail=%i size=%i  quant=%i\n",
				pEnc->bframenum_head, pEnc->bframenum_tail,
				pEnc->queue_head, pEnc->queue_tail, pEnc->queue_size,pEnc->current->quant);

		/* store frame into bframe buffer & swap ref back to current */
		SWAP(FRAMEINFO*, pEnc->current, pEnc->bframes[pEnc->bframenum_tail]);
		SWAP(FRAMEINFO*, pEnc->current, pEnc->reference);

		pEnc->bframenum_tail++;

		goto repeat;
	}


		DPRINTF(XVID_DEBUG_DEBUG,"*** XXXXXX bf: head=%i tail=%i   queue: head=%i tail=%i size=%i\n",
				pEnc->bframenum_head, pEnc->bframenum_tail,
				pEnc->queue_head, pEnc->queue_tail, pEnc->queue_size);

	/* for unpacked bframes, output the stats for the last encoded frame */
	if (!(pEnc->mbParam.global_flags & XVID_GLOBAL_PACKED) && pEnc->mbParam.max_bframes > 0)
	{
		if (pEnc->current->stamp > 0) {
			call_plugins(pEnc, pEnc->reference, &pEnc->sOriginal, XVID_PLG_AFTER, 0, 0, stats);
		}
		else
			stats->type = XVID_TYPE_NOTHING;
	}

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * closed-gop
	 * if the frame prior to an iframe is scheduled as a bframe, we must change it to a pframe
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (type == I_VOP && (pEnc->mbParam.global_flags & XVID_GLOBAL_CLOSED_GOP) && pEnc->bframenum_tail > 0) {

		/* place this frame back on the encoding-queue (head) */
		/* we will deal with it next time */
		dec_frame_num(pEnc);
		pEnc->iFrameNum--;

		pEnc->queue_head = (pEnc->queue_head + (pEnc->mbParam.max_bframes+1) - 1) % (pEnc->mbParam.max_bframes+1);
		pEnc->queue_size++;
		image_swap(&pEnc->current->image, &pEnc->queue[pEnc->queue_head].image);

		/* grab the last frame from the bframe-queue */

		pEnc->bframenum_tail--;
		SWAP(FRAMEINFO*, pEnc->current, pEnc->bframes[pEnc->bframenum_tail]);

		if ((pEnc->current->vop_flags & XVID_VOP_DEBUG)) {
			image_printf(&pEnc->current->image, pEnc->mbParam.edged_width, pEnc->mbParam.height, 5, 100, "DX50 BVOP->PVOP");
		}

		/* convert B-VOP quant to P-VOP */
		pEnc->current->quant  = 100*pEnc->current->quant - pEnc->mbParam.bquant_offset;
		pEnc->current->quant += pEnc->mbParam.bquant_ratio - 1; /* to avoid rouding issues */
		pEnc->current->quant /= pEnc->mbParam.bquant_ratio;
		type = P_VOP;
	}


	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * encode this frame as an i-vop
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (type == I_VOP) {

		DPRINTF(XVID_DEBUG_DEBUG,"*** IFRAME bf: head=%i tail=%i   queue: head=%i tail=%i size=%i\n",
				pEnc->bframenum_head, pEnc->bframenum_tail,
				pEnc->queue_head, pEnc->queue_tail, pEnc->queue_size);

		if ((pEnc->current->vop_flags & XVID_VOP_DEBUG)) {
			image_printf(&pEnc->current->image, pEnc->mbParam.edged_width, pEnc->mbParam.height, 5, 200, "IVOP");
		}

		pEnc->iFrameNum = 1;

		/* ---- update vol flags at IVOP ----------- */
		pEnc->mbParam.vol_flags = pEnc->current->vol_flags;

		/* Aspect ratio */
		switch(frame->par) {
		case XVID_PAR_11_VGA:
		case XVID_PAR_43_PAL:
		case XVID_PAR_43_NTSC:
		case XVID_PAR_169_PAL:
		case XVID_PAR_169_NTSC:
		case XVID_PAR_EXT:
			pEnc->mbParam.par = frame->par;
			break;
		default:
			pEnc->mbParam.par = XVID_PAR_11_VGA;
			break;
		}

		/* For extended PAR only, we try to sanityse/simplify par values */
		if (pEnc->mbParam.par == XVID_PAR_EXT) {
			pEnc->mbParam.par_width  = frame->par_width;
			pEnc->mbParam.par_height = frame->par_height;
			simplify_par(&pEnc->mbParam.par_width, &pEnc->mbParam.par_height);
		}

		if ((pEnc->mbParam.vol_flags & XVID_VOL_MPEGQUANT)) {
			if (frame->quant_intra_matrix != NULL)
				set_intra_matrix(pEnc->mbParam.mpeg_quant_matrices, frame->quant_intra_matrix);
			if (frame->quant_inter_matrix != NULL)
				set_inter_matrix(pEnc->mbParam.mpeg_quant_matrices, frame->quant_inter_matrix);
		}

		/* prevent vol/vop misuse */

		if (!(pEnc->current->vol_flags & XVID_VOL_REDUCED_ENABLE))
			pEnc->current->vop_flags &= ~XVID_VOP_REDUCED;

		if (!(pEnc->current->vol_flags & XVID_VOL_INTERLACING))
			pEnc->current->vop_flags &= ~(XVID_VOP_TOPFIELDFIRST|XVID_VOP_ALTERNATESCAN);

		/* ^^^------------------------ */

		if ((pEnc->mbParam.plugin_flags & XVID_REQORIGINAL)) {
			image_copy(&pEnc->sOriginal, &pEnc->current->image,
				   pEnc->mbParam.edged_width, pEnc->mbParam.height);
		}

		FrameCodeI(pEnc, &bs);
		xFrame->out_flags |= XVID_KEYFRAME;

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * encode this frame as an p-vop
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	} else { /* (type == P_VOP || type == S_VOP) */

		DPRINTF(XVID_DEBUG_DEBUG,"*** PFRAME bf: head=%i tail=%i   queue: head=%i tail=%i size=%i\n",
				pEnc->bframenum_head, pEnc->bframenum_tail,
				pEnc->queue_head, pEnc->queue_tail, pEnc->queue_size);

		if ((pEnc->current->vop_flags & XVID_VOP_DEBUG)) {
			image_printf(&pEnc->current->image, pEnc->mbParam.edged_width, pEnc->mbParam.height, 5, 200, "PVOP");
		}

		if ((pEnc->mbParam.plugin_flags & XVID_REQORIGINAL)) {
			image_copy(&pEnc->sOriginal, &pEnc->current->image,
				   pEnc->mbParam.edged_width, pEnc->mbParam.height);
		}

		if ( FrameCodeP(pEnc, &bs) == 0 ) {
			/* N-VOP, we mustn't code b-frames yet */
			call_plugins(pEnc, pEnc->current, &pEnc->sOriginal, XVID_PLG_AFTER, 0, 0, stats);
			goto done;
		}
	}


	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * on next enc_encode call we must flush bframes
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

/*done_flush:*/

	pEnc->flush_bframes = 1;

	/* packed & queued_bframes: dont bother outputting stats here, we do so after the flush */
	if ((pEnc->mbParam.global_flags & XVID_GLOBAL_PACKED) && pEnc->bframenum_tail > 0) {
		goto repeat;
	}

	/* packed or no-bframes or no-bframes-queued: output stats */
	if ((pEnc->mbParam.global_flags & XVID_GLOBAL_PACKED) || pEnc->mbParam.max_bframes == 0 ) {
		call_plugins(pEnc, pEnc->current, &pEnc->sOriginal, XVID_PLG_AFTER, 0, 0, stats);
	}

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * done; return number of bytes consumed
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

done:

	stop_global_timer();
	write_timer();

	emms();
	return BitstreamLength(&bs);
}


static void SetMacroblockQuants(MBParam * const pParam, FRAMEINFO * frame)
{
	unsigned int i;
	MACROBLOCK * pMB = frame->mbs;
	int quant = frame->mbs[0].quant; /* set by XVID_PLG_FRAME */
	if (quant > 31)
		frame->quant = quant = 31;
	else if (quant < 1)
		frame->quant = quant = 1;

	for (i = 0; i < pParam->mb_height * pParam->mb_width; i++) {
		quant += pMB->dquant;
		if (quant > 31)
			quant = 31;
		else if (quant < 1)
			quant = 1;
		pMB->quant = quant;
		pMB++;
	}
}


static __inline void
CodeIntraMB(Encoder * pEnc,
			MACROBLOCK * pMB)
{

	pMB->mode = MODE_INTRA;

	/* zero mv statistics */
	pMB->mvs[0].x = pMB->mvs[1].x = pMB->mvs[2].x = pMB->mvs[3].x = 0;
	pMB->mvs[0].y = pMB->mvs[1].y = pMB->mvs[2].y = pMB->mvs[3].y = 0;
	pMB->sad8[0] = pMB->sad8[1] = pMB->sad8[2] = pMB->sad8[3] = 0;
	pMB->sad16 = 0;

	if (pMB->dquant != 0) {
		pMB->mode = MODE_INTRA_Q;
	}
}



static int
FrameCodeI(Encoder * pEnc,
		   Bitstream * bs)
{
	int bits = BitstreamPos(bs);
	int mb_width = pEnc->mbParam.mb_width;
	int mb_height = pEnc->mbParam.mb_height;

	DECLARE_ALIGNED_MATRIX(dct_codes, 6, 64, int16_t, CACHE_LINE);
	DECLARE_ALIGNED_MATRIX(qcoeff, 6, 64, int16_t, CACHE_LINE);

	uint16_t x, y;

	if ((pEnc->current->vol_flags & XVID_VOL_REDUCED_ENABLE))
	{
		mb_width = (pEnc->mbParam.width + 31) / 32;
		mb_height = (pEnc->mbParam.height + 31) / 32;

		/* 16x16->8x8 downsample requires 1 additional edge pixel*/
		/* XXX: setedges is overkill */
		start_timer();
		image_setedges(&pEnc->current->image,
			pEnc->mbParam.edged_width, pEnc->mbParam.edged_height,
			pEnc->mbParam.width, pEnc->mbParam.height, 0);
		stop_edges_timer();
	}

	pEnc->mbParam.m_rounding_type = 1;
	pEnc->current->rounding_type = pEnc->mbParam.m_rounding_type;
	pEnc->current->coding_type = I_VOP;

	call_plugins(pEnc, pEnc->current, NULL, XVID_PLG_FRAME, NULL, NULL, NULL);

	SetMacroblockQuants(&pEnc->mbParam, pEnc->current);

	BitstreamWriteVolHeader(bs, &pEnc->mbParam, pEnc->current);

	set_timecodes(pEnc->current,pEnc->reference,pEnc->mbParam.fbase);

	BitstreamPad(bs);

	BitstreamWriteVopHeader(bs, &pEnc->mbParam, pEnc->current, 1, pEnc->current->mbs[0].quant);

	pEnc->current->sStat.iTextBits = 0;
	pEnc->current->sStat.kblks = mb_width * mb_height;
	pEnc->current->sStat.mblks = pEnc->current->sStat.ublks = 0;

	for (y = 0; y < mb_height; y++)
		for (x = 0; x < mb_width; x++) {
			MACROBLOCK *pMB =
				&pEnc->current->mbs[x + y * pEnc->mbParam.mb_width];

			CodeIntraMB(pEnc, pMB);

			MBTransQuantIntra(&pEnc->mbParam, pEnc->current, pMB, x, y,
							  dct_codes, qcoeff);

			start_timer();
			MBPrediction(pEnc->current, x, y, pEnc->mbParam.mb_width, qcoeff);
			stop_prediction_timer();

			start_timer();
			if (pEnc->current->vop_flags & XVID_VOP_GREYSCALE)
			{	pMB->cbp &= 0x3C;		/* keep only bits 5-2 */
				qcoeff[4*64+0]=0;		/* zero, because for INTRA MBs DC value is saved */
				qcoeff[5*64+0]=0;
			}
			MBCoding(pEnc->current, pMB, qcoeff, bs, &pEnc->current->sStat);
			stop_coding_timer();
		}

	if ((pEnc->current->vop_flags & XVID_VOP_REDUCED))
	{
		image_deblock_rrv(&pEnc->current->image, pEnc->mbParam.edged_width,
			pEnc->current->mbs, mb_width, mb_height, pEnc->mbParam.mb_width,
			16, 0);
	}
	emms();

	BitstreamPadAlways(bs); /* next_start_code() at the end of VideoObjectPlane() */

	pEnc->current->length = (BitstreamPos(bs) - bits) / 8;

	pEnc->fMvPrevSigma = -1;
	pEnc->mbParam.m_fcode = 2;

	pEnc->current->is_edged = 0; /* not edged */
	pEnc->current->is_interpolated = -1; /* not interpolated (fake rounding -1) */

	return 1;					/* intra */
}


#define INTRA_THRESHOLD 0.5
#define BFRAME_SKIP_THRESHHOLD 30


/* FrameCodeP also handles S(GMC)-VOPs */
static int
FrameCodeP(Encoder * pEnc,
		   Bitstream * bs)
{
	float fSigma;
	int bits = BitstreamPos(bs);

	DECLARE_ALIGNED_MATRIX(dct_codes, 6, 64, int16_t, CACHE_LINE);
	DECLARE_ALIGNED_MATRIX(qcoeff, 6, 64, int16_t, CACHE_LINE);

	int x, y, k;
	int iSearchRange;
	int skip_possible;
	FRAMEINFO *const current = pEnc->current;
	FRAMEINFO *const reference = pEnc->reference;
	MBParam * const pParam = &pEnc->mbParam;
	int mb_width = pParam->mb_width;
	int mb_height = pParam->mb_height;
	int coded = 1;


	/* IMAGE *pCurrent = &current->image; */
	IMAGE *pRef = &reference->image;

	if ((current->vop_flags & XVID_VOP_REDUCED))
	{
		mb_width = (pParam->width + 31) / 32;
		mb_height = (pParam->height + 31) / 32;
	}


	if (!reference->is_edged) {	
		start_timer();
		image_setedges(pRef, pParam->edged_width, pParam->edged_height,
					   pParam->width, pParam->height, 0);
		stop_edges_timer();
		reference->is_edged = 1;
	}

	pParam->m_rounding_type = 1 - pParam->m_rounding_type;
	current->rounding_type = pParam->m_rounding_type;
	current->fcode = pParam->m_fcode;

	if ((current->vop_flags & XVID_VOP_HALFPEL)) {
		if (reference->is_interpolated != current->rounding_type) {
			start_timer();
			image_interpolate(pRef, &pEnc->vInterH, &pEnc->vInterV,
							  &pEnc->vInterHV, pParam->edged_width,
							  pParam->edged_height,
							  (pParam->vol_flags & XVID_VOL_QUARTERPEL),
							  current->rounding_type);
			stop_inter_timer();
			reference->is_interpolated = current->rounding_type;
		}
	}

	current->coding_type = P_VOP;

	call_plugins(pEnc, pEnc->current, NULL, XVID_PLG_FRAME, NULL, NULL, NULL);

	SetMacroblockQuants(&pEnc->mbParam, current);

	start_timer();
	if (current->vol_flags & XVID_VOL_GMC )	/* GMC only for S(GMC)-VOPs */
	{	int gmcval;
		current->warp = GlobalMotionEst( current->mbs, pParam, current, reference,
								 &pEnc->vInterH, &pEnc->vInterV, &pEnc->vInterHV);

		if (current->motion_flags & XVID_ME_GME_REFINE) {
			gmcval = GlobalMotionEstRefine(&current->warp,
										   current->mbs, pParam,
										   current, reference,
										   &current->image,
										   &reference->image,
										   &pEnc->vInterH,
										   &pEnc->vInterV,
										   &pEnc->vInterHV);
		} else {
			gmcval = globalSAD(&current->warp, pParam, current->mbs,
							   current,
							   &reference->image,
							   &current->image,
							   pEnc->vGMC.y);
		}

		gmcval += /*current->quant*/ 2 * (int)(pParam->mb_width*pParam->mb_height);

		/* 1st '3': 3 warpoints, 2nd '3': 16th pel res (2<<3) */
		generate_GMCparameters(	3, 3, &current->warp,
				pParam->width, pParam->height,
				&current->new_gmc_data);

		if ( (gmcval<0) && ( (current->warp.duv[1].x != 0) || (current->warp.duv[1].y != 0) ||
			 (current->warp.duv[2].x != 0) || (current->warp.duv[2].y != 0) ) )
		{
			current->coding_type = S_VOP;

			generate_GMCimage(&current->new_gmc_data, &reference->image,
				pParam->mb_width, pParam->mb_height,
				pParam->edged_width, pParam->edged_width/2,
				pParam->m_fcode, ((pParam->vol_flags & XVID_VOL_QUARTERPEL)?1:0), 0,
				current->rounding_type, current->mbs, &pEnc->vGMC);

		} else {

			generate_GMCimage(&current->new_gmc_data, &reference->image,
				pParam->mb_width, pParam->mb_height,
				pParam->edged_width, pParam->edged_width/2,
				pParam->m_fcode, ((pParam->vol_flags & XVID_VOL_QUARTERPEL)?1:0), 0,
				current->rounding_type, current->mbs, NULL);	/* no warping, just AMV */
		}
	}

	MotionEstimation(&pEnc->mbParam, current, reference,
					 &pEnc->vInterH, &pEnc->vInterV, &pEnc->vInterHV,
					 &pEnc->vGMC, 256*4096);


	stop_motion_timer();

	set_timecodes(current,reference,pParam->fbase);

	BitstreamWriteVopHeader(bs, &pEnc->mbParam, current, 1, current->mbs[0].quant);

	current->sStat.iTextBits = current->sStat.iMvSum = current->sStat.iMvCount =
		current->sStat.kblks = current->sStat.mblks = current->sStat.ublks = 0;


	for (y = 0; y < mb_height; y++) {
		for (x = 0; x < mb_width; x++) {
			MACROBLOCK *pMB =
				&current->mbs[x + y * pParam->mb_width];

			int bIntra = (pMB->mode == MODE_INTRA) || (pMB->mode == MODE_INTRA_Q);

			if (bIntra) {
				CodeIntraMB(pEnc, pMB);
				MBTransQuantIntra(&pEnc->mbParam, current, pMB, x, y,
								  dct_codes, qcoeff);

				start_timer();
				MBPrediction(current, x, y, pParam->mb_width, qcoeff);
				stop_prediction_timer();

				current->sStat.kblks++;

				if (pEnc->current->vop_flags & XVID_VOP_GREYSCALE)
				{	pMB->cbp &= 0x3C;		/* keep only bits 5-2 */
					qcoeff[4*64+0]=0;		/* zero, because for INTRA MBs DC value is saved */
					qcoeff[5*64+0]=0;
				}
				MBCoding(current, pMB, qcoeff, bs, &current->sStat);
				stop_coding_timer();
				continue;
			}

			start_timer();
			MBMotionCompensation(pMB, x, y, &reference->image,
								 &pEnc->vInterH, &pEnc->vInterV,
								 &pEnc->vInterHV, &pEnc->vGMC,
								 &current->image,
								 dct_codes, pParam->width,
								 pParam->height,
								 pParam->edged_width,
								 (current->vol_flags & XVID_VOL_QUARTERPEL),
								 (current->vop_flags & XVID_VOP_REDUCED),
								 current->rounding_type);

			stop_comp_timer();

			pMB->field_pred = 0;

			if (pMB->mode != MODE_NOT_CODED)
			{	pMB->cbp =
					MBTransQuantInter(&pEnc->mbParam, current, pMB, x, y,
									  dct_codes, qcoeff);
			}

			if (pMB->dquant != 0)
				MBSetDquant(pMB, x, y, &pEnc->mbParam);


			if (pMB->cbp || pMB->mvs[0].x || pMB->mvs[0].y ||
				   pMB->mvs[1].x || pMB->mvs[1].y || pMB->mvs[2].x ||
				   pMB->mvs[2].y || pMB->mvs[3].x || pMB->mvs[3].y) {
				current->sStat.mblks++;
			}  else {
				current->sStat.ublks++;
			}

			start_timer();

			/* Finished processing the MB, now check if to CODE or SKIP */

			skip_possible = (pMB->cbp == 0) && (pMB->mode == MODE_INTER) &&
							(pMB->dquant == 0);

			if (current->coding_type == S_VOP)
				skip_possible &= (pMB->mcsel == 1);
			else if (current->coding_type == P_VOP) {
				if ((pParam->vol_flags & XVID_VOL_QUARTERPEL))
					skip_possible &= ( (pMB->qmvs[0].x == 0) && (pMB->qmvs[0].y == 0) );
				else
					skip_possible &= ( (pMB->mvs[0].x == 0) && (pMB->mvs[0].y == 0) );
			}

			if ( (pMB->mode == MODE_NOT_CODED) || (skip_possible)) {

/* This is a candidate for SKIPping, but for P-VOPs check intermediate B-frames first */

				if (current->coding_type == P_VOP)	/* special rule for P-VOP's SKIP */
				{
					int bSkip = 1;

					for (k=pEnc->bframenum_head; k< pEnc->bframenum_tail; k++)
					{
						int iSAD;
						iSAD = sad16(reference->image.y + 16*y*pParam->edged_width + 16*x,
									pEnc->bframes[k]->image.y + 16*y*pParam->edged_width + 16*x,
								pParam->edged_width,BFRAME_SKIP_THRESHHOLD);
						if (iSAD >= BFRAME_SKIP_THRESHHOLD * pMB->quant)
						{	bSkip = 0;
							break;
						}
					}

					if (!bSkip) {	/* no SKIP, but trivial block */
						if((pParam->vol_flags & XVID_VOL_QUARTERPEL)) {
							VECTOR predMV = get_qpmv2(current->mbs, pParam->mb_width, 0, x, y, 0);
							pMB->pmvs[0].x = - predMV.x;
							pMB->pmvs[0].y = - predMV.y;
						}
						else {
							VECTOR predMV = get_pmv2(current->mbs, pParam->mb_width, 0, x, y, 0);
							pMB->pmvs[0].x = - predMV.x;
							pMB->pmvs[0].y = - predMV.y;
						}
						pMB->mode = MODE_INTER;
						pMB->cbp = 0;
						MBCoding(current, pMB, qcoeff, bs, &current->sStat);
						stop_coding_timer();

						continue;	/* next MB */
					}
				}
				/* do SKIP */

				pMB->mode = MODE_NOT_CODED;
				MBSkip(bs);
				stop_coding_timer();
				continue;	/* next MB */
			}
			/* ordinary case: normal coded INTER/INTER4V block */

			if ((current->vop_flags & XVID_VOP_GREYSCALE))
			{	pMB->cbp &= 0x3C;		/* keep only bits 5-2 */
				qcoeff[4*64+0]=0;		/* zero, because DC for INTRA MBs DC value is saved */
				qcoeff[5*64+0]=0;
			}

			if((pParam->vol_flags & XVID_VOL_QUARTERPEL)) {
				VECTOR predMV = get_qpmv2(current->mbs, pParam->mb_width, 0, x, y, 0);
				pMB->pmvs[0].x = pMB->qmvs[0].x - predMV.x;
				pMB->pmvs[0].y = pMB->qmvs[0].y - predMV.y;
				DPRINTF(XVID_DEBUG_MV,"mv_diff (%i,%i) pred (%i,%i) result (%i,%i)\n", pMB->pmvs[0].x, pMB->pmvs[0].y, predMV.x, predMV.y, pMB->mvs[0].x, pMB->mvs[0].y);
			} else {
				VECTOR predMV = get_pmv2(current->mbs, pParam->mb_width, 0, x, y, 0);
				pMB->pmvs[0].x = pMB->mvs[0].x - predMV.x;
				pMB->pmvs[0].y = pMB->mvs[0].y - predMV.y;
				DPRINTF(XVID_DEBUG_MV,"mv_diff (%i,%i) pred (%i,%i) result (%i,%i)\n", pMB->pmvs[0].x, pMB->pmvs[0].y, predMV.x, predMV.y, pMB->mvs[0].x, pMB->mvs[0].y);
			}


			if (pMB->mode == MODE_INTER4V)
			{	int k;
				for (k=1;k<4;k++)
				{
					if((pParam->vol_flags & XVID_VOL_QUARTERPEL)) {
						VECTOR predMV = get_qpmv2(current->mbs, pParam->mb_width, 0, x, y, k);
						pMB->pmvs[k].x = pMB->qmvs[k].x - predMV.x;
						pMB->pmvs[k].y = pMB->qmvs[k].y - predMV.y;
				DPRINTF(XVID_DEBUG_MV,"mv_diff (%i,%i) pred (%i,%i) result (%i,%i)\n", pMB->pmvs[k].x, pMB->pmvs[k].y, predMV.x, predMV.y, pMB->mvs[k].x, pMB->mvs[k].y);
					} else {
						VECTOR predMV = get_pmv2(current->mbs, pParam->mb_width, 0, x, y, k);
						pMB->pmvs[k].x = pMB->mvs[k].x - predMV.x;
						pMB->pmvs[k].y = pMB->mvs[k].y - predMV.y;
				DPRINTF(XVID_DEBUG_MV,"mv_diff (%i,%i) pred (%i,%i) result (%i,%i)\n", pMB->pmvs[k].x, pMB->pmvs[k].y, predMV.x, predMV.y, pMB->mvs[k].x, pMB->mvs[k].y);
					}

				}
			}

			MBCoding(current, pMB, qcoeff, bs, &pEnc->current->sStat);
			stop_coding_timer();

		}
	}

	if ((current->vop_flags & XVID_VOP_REDUCED))
	{
		image_deblock_rrv(&current->image, pParam->edged_width,
			current->mbs, mb_width, mb_height, pParam->mb_width,
			16, 0);
	}

	emms();

	if (current->sStat.iMvCount == 0)
		current->sStat.iMvCount = 1;

	fSigma = (float) sqrt((float) current->sStat.iMvSum / current->sStat.iMvCount);

	iSearchRange = 1 << (3 + pParam->m_fcode);

	if ((fSigma > iSearchRange / 3)
		&& (pParam->m_fcode <= (3 +  (pParam->vol_flags & XVID_VOL_QUARTERPEL?1:0)  )))	/* maximum search range 128 */
	{
		pParam->m_fcode++;
		iSearchRange *= 2;
	} else if ((fSigma < iSearchRange / 6)
			   && (pEnc->fMvPrevSigma >= 0)
			   && (pEnc->fMvPrevSigma < iSearchRange / 6)
			   && (pParam->m_fcode >= (2 + (pParam->vol_flags & XVID_VOL_QUARTERPEL?1:0) )))	/* minimum search range 16 */
	{
		pParam->m_fcode--;
		iSearchRange /= 2;
	}

	pEnc->fMvPrevSigma = fSigma;

	/* frame drop code */
#if 0
	DPRINTF(XVID_DEBUG_DEBUG, "kmu %i %i %i\n", current->sStat.kblks, current->sStat.mblks, current->sStat.ublks);
#endif
	if (current->sStat.kblks + current->sStat.mblks <=
		(pParam->frame_drop_ratio * mb_width * mb_height) / 100 &&
		pEnc->mbParam.max_bframes == 0)
	{
		current->sStat.kblks = current->sStat.mblks = 0;
		current->sStat.ublks = mb_width * mb_height;

		BitstreamReset(bs);

		set_timecodes(current,reference,pParam->fbase);
		BitstreamWriteVopHeader(bs, &pEnc->mbParam, current, 0, current->mbs[0].quant);

		/* copy reference frame details into the current frame */
		current->quant = reference->quant;
		current->motion_flags = reference->motion_flags;
		current->rounding_type = reference->rounding_type;
		current->fcode = reference->fcode;
		current->bcode = reference->bcode;
		current->stamp = reference->stamp;
		image_copy(&current->image, &reference->image, pParam->edged_width, pParam->height);
		memcpy(current->mbs, reference->mbs, sizeof(MACROBLOCK) * mb_width * mb_height);
		coded = 0;
	
	} else {

		pEnc->current->is_edged = 0; /* not edged */
		pEnc->current->is_interpolated = -1; /* not interpolated (fake rounding -1) */

		/* what was this frame's interpolated reference will become 
			forward (past) reference in b-frame coding */

		image_swap(&pEnc->vInterH, &pEnc->f_refh);
		image_swap(&pEnc->vInterV, &pEnc->f_refv);
		image_swap(&pEnc->vInterHV, &pEnc->f_refhv);
	}

	/* XXX: debug
	{
		char s[100];
		sprintf(s, "\\%05i_cur.pgm", pEnc->m_framenum);
		image_dump_yuvpgm(&current->image,
			pParam->edged_width,
			pParam->width, pParam->height, s);

		sprintf(s, "\\%05i_ref.pgm", pEnc->m_framenum);
		image_dump_yuvpgm(&reference->image,
			pParam->edged_width,
			pParam->width, pParam->height, s);
	}
	*/

	BitstreamPadAlways(bs); /* next_start_code() at the end of VideoObjectPlane() */

	current->length = (BitstreamPos(bs) - bits) / 8;

	return coded;
}


static void
FrameCodeB(Encoder * pEnc,
		   FRAMEINFO * frame,
		   Bitstream * bs)
{
	int bits = BitstreamPos(bs);
	DECLARE_ALIGNED_MATRIX(dct_codes, 6, 64, int16_t, CACHE_LINE);
	DECLARE_ALIGNED_MATRIX(qcoeff, 6, 64, int16_t, CACHE_LINE);
	uint32_t x, y;

	IMAGE *f_ref = &pEnc->reference->image;
	IMAGE *b_ref = &pEnc->current->image;

	#ifdef BFRAMES_DEC_DEBUG
	FILE *fp;
	static char first=0;
#define BFRAME_DEBUG  	if (!first && fp){ \
		fprintf(fp,"Y=%3d   X=%3d   MB=%2d   CBP=%02X\n",y,x,mb->mode,mb->cbp); \
	}

	/* XXX: pEnc->current->global_flags &= ~XVID_VOP_REDUCED;  reduced resoltion not yet supported */

	if (!first){
		fp=fopen("C:\\XVIDDBGE.TXT","w");
	}
#endif

	/* forward  */
	if (!pEnc->reference->is_edged) {
		image_setedges(f_ref, pEnc->mbParam.edged_width,
					   pEnc->mbParam.edged_height, pEnc->mbParam.width,
					   pEnc->mbParam.height, 0);
		pEnc->current->is_edged = 1;	
	}

	if (pEnc->reference->is_interpolated != 0) {
		start_timer();
		image_interpolate(f_ref, &pEnc->f_refh, &pEnc->f_refv, &pEnc->f_refhv,
						  pEnc->mbParam.edged_width, pEnc->mbParam.edged_height,
						  (pEnc->mbParam.vol_flags & XVID_VOL_QUARTERPEL), 0);
		stop_inter_timer();
		pEnc->reference->is_interpolated = 0;
	}

	/* backward */
	if (!pEnc->current->is_edged) {
		image_setedges(b_ref, pEnc->mbParam.edged_width,
					   pEnc->mbParam.edged_height, pEnc->mbParam.width,
					   pEnc->mbParam.height, 0);
		pEnc->current->is_edged = 1;
	}

	if (pEnc->current->is_interpolated != 0) {
		start_timer();
		image_interpolate(b_ref, &pEnc->vInterH, &pEnc->vInterV, &pEnc->vInterHV,
						pEnc->mbParam.edged_width, pEnc->mbParam.edged_height,
						(pEnc->mbParam.vol_flags & XVID_VOL_QUARTERPEL), 0);
		stop_inter_timer();
		pEnc->current->is_interpolated = 0;
	}

	frame->coding_type = B_VOP;
	call_plugins(pEnc, pEnc->current, NULL, XVID_PLG_FRAME, NULL, NULL, NULL);

	start_timer();
	MotionEstimationBVOP(&pEnc->mbParam, frame,
						 ((int32_t)(pEnc->current->stamp - frame->stamp)),				/* time_bp */
						 ((int32_t)(pEnc->current->stamp - pEnc->reference->stamp)), 	/* time_pp */
						 pEnc->reference->mbs, f_ref,
						 &pEnc->f_refh, &pEnc->f_refv, &pEnc->f_refhv,
						 pEnc->current, b_ref, &pEnc->vInterH,
						 &pEnc->vInterV, &pEnc->vInterHV);
	stop_motion_timer();

	set_timecodes(frame, pEnc->reference,pEnc->mbParam.fbase);
	BitstreamWriteVopHeader(bs, &pEnc->mbParam, frame, 1, frame->quant);

	frame->sStat.iTextBits = 0;
	frame->sStat.iMvSum = 0;
	frame->sStat.iMvCount = 0;
	frame->sStat.kblks = frame->sStat.mblks = frame->sStat.ublks = 0;
	frame->sStat.mblks = pEnc->mbParam.mb_width * pEnc->mbParam.mb_height;
	frame->sStat.kblks = frame->sStat.ublks = 0;

	for (y = 0; y < pEnc->mbParam.mb_height; y++) {
		for (x = 0; x < pEnc->mbParam.mb_width; x++) {
			MACROBLOCK * const mb = &frame->mbs[x + y * pEnc->mbParam.mb_width];

			/* decoder ignores mb when refence block is INTER(0,0), CBP=0 */
			if (mb->mode == MODE_NOT_CODED) {
				if (pEnc->mbParam.plugin_flags & XVID_REQORIGINAL) {
					MBMotionCompensation(mb, x, y, f_ref, NULL, f_ref, NULL, NULL, &frame->image,
											NULL, 0, 0, pEnc->mbParam.edged_width, 0, 0, 0);
				}

				continue;
			}

			if (mb->mode != MODE_DIRECT_NONE_MV || pEnc->mbParam.plugin_flags & XVID_REQORIGINAL) {
				MBMotionCompensationBVOP(&pEnc->mbParam, mb, x, y, &frame->image,
									 f_ref, &pEnc->f_refh, &pEnc->f_refv,
									 &pEnc->f_refhv, b_ref, &pEnc->vInterH,
									 &pEnc->vInterV, &pEnc->vInterHV,
									 dct_codes);

				if (mb->mode == MODE_DIRECT_NO4V) mb->mode = MODE_DIRECT;
				mb->quant = frame->quant;

				if (mb->mode != MODE_DIRECT_NONE_MV)
					mb->cbp = MBTransQuantInterBVOP(&pEnc->mbParam, frame, mb, x, y,  dct_codes, qcoeff);

				if ( (mb->mode == MODE_DIRECT) && (mb->cbp == 0)
					&& (mb->pmvs[3].x == 0) && (mb->pmvs[3].y == 0) ) {
					mb->mode = MODE_DIRECT_NONE_MV;	/* skipped */
				}
			}

			/* keep only bits 5-2 -- Chroma blocks will just be skipped by the
			 * coding function for BFrames, that's why we don't zero teh DC
			 * coeffs */
			if ((frame->vop_flags & XVID_VOP_GREYSCALE))
				mb->cbp &= 0x3C;

			start_timer();
			MBCodingBVOP(frame, mb, qcoeff, frame->fcode, frame->bcode, bs,
						 &frame->sStat);
			stop_coding_timer();
		}
	}

	emms();

	/* TODO: dynamic fcode/bcode ??? */

	BitstreamPadAlways(bs); /* next_start_code() at the end of VideoObjectPlane() */
	frame->length = (BitstreamPos(bs) - bits) / 8;

#ifdef BFRAMES_DEC_DEBUG
	if (!first){
		first=1;
		if (fp)
			fclose(fp);
	}
#endif
}
