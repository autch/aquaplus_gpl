#include "xvid_dec.h"

/*****************************************************************************
 *               Global vars in module and constants
 ****************************************************************************/

BOOL XviDDec::Start_XviD(int Xsize,int Ysize,int csp)
{
/*****************************************************************************
 *        Memory allocation
 ****************************************************************************/
	Clear_XviD();
	width = Xsize;
	height = Ysize;
	colorspace = csp;
	filenr = 0;
	totalsize = 0;
	forceSkip = FALSE;
	buffer_size =  2*width*height;

	xvid_gbl_init_t   xvid_gbl_init;
	xvid_dec_create_t xvid_dec_create;
	/*------------------------------------------------------------------------
	 * XviD core initialization
	 *----------------------------------------------------------------------*/
	/* Version */
	xvid_gbl_init.version = XVID_VERSION;
#ifndef _DEBUG
	xvid_gbl_init.cpu_flags = 0;
#else
	xvid_gbl_init.cpu_flags = XVID_CPU_FORCE;
#endif
	xvid_global(NULL, 0, &xvid_gbl_init, NULL);

	/*------------------------------------------------------------------------
	 * XviD encoder initialization
	 *----------------------------------------------------------------------*/
	/* Version */
	xvid_dec_create.version = XVID_VERSION;
	/*
	 * Image dimensions -- set to 0, xvidcore will resize when ever it is
	 * needed
	 */
	xvid_dec_create.width = width;
	xvid_dec_create.height = height;
	int ret = xvid_decore(NULL, XVID_DEC_CREATE, &xvid_dec_create, NULL);
	dec_handle = xvid_dec_create.handle;
	if(ret) return FALSE;
	bInit = TRUE;
	bContinue = FALSE;
	return TRUE;
}

/* close decoder to release resources */
void XviDDec::Clear_XviD()
{
	if(NULL==dec_handle)return;
	xvid_decore(dec_handle, XVID_DEC_DESTROY, NULL, NULL);
	dec_handle = NULL;
}

WORD XviDDec::GetVer()
{
	return 0x0104;
}


int XviDDec::DecodeXviD(BYTE *inmemory, int inmemsize,
				int *rest, BYTE *outmemory,DWORD pitch)
{
	int used_bytes = 0;
	xvid_dec_stats_t xvid_dec_stats;

	if(bInit){
		mp4_buffer = inmemory;
		useful_bytes = inmemsize;
		mp4_ptr = mp4_buffer;
		bInit = FALSE;
	}
	out_buffer = outmemory;
	if(bContinue){
		useful_bytes += inmemsize;
		bContinue = FALSE;
	}
	if(FALSE==bFlash){
		do{
			/* Decode frame */
			used_bytes = dec_main(mp4_ptr, out_buffer, useful_bytes, &xvid_dec_stats,pitch);
			/* Update buffer pointers */
			if(used_bytes > 0) {
				mp4_ptr += used_bytes;
				useful_bytes -= used_bytes;
				/* Total size */
				totalsize += used_bytes;
			}
		}while((xvid_dec_stats.type<XVID_TYPE_IVOP || xvid_dec_stats.type>XVID_TYPE_BVOP) && useful_bytes>0);

		if(useful_bytes<=0)
			bFlash = TRUE;
	}else{
        do {
		    used_bytes = dec_main(NULL, out_buffer, -1, &xvid_dec_stats,pitch);
        }while(used_bytes>=0 && xvid_dec_stats.type <= 0);
		if (used_bytes < 0){
			bFlash = FALSE;
			return XviD_DEC_END;
		}
	}
	filenr++;
	if (mp4_ptr > mp4_buffer + buffer_size*3/4) {
		*rest = (mp4_buffer + buffer_size - mp4_ptr);

		// Move data if needed
		if (*rest > 0)
			memcpy(mp4_buffer, mp4_ptr, *rest);

		// Update mp4_ptr
		mp4_ptr = mp4_buffer;
		bContinue = TRUE;
		return XviD_DEC_NEEDMORE;
	}
	return XviD_DEC_CONTINUE;
}

/* decode one frame  */
int XviDDec::dec_main(BYTE *istream,
		 BYTE *ostream,
		 int istream_size,
		 xvid_dec_stats_t *xvid_dec_stats,
		 DWORD pitch)
{
	xvid_dec_frame_t xvid_dec_frame;

	/* Set version */
	xvid_dec_frame.version = XVID_VERSION;
	xvid_dec_stats->version = XVID_VERSION;


	/* No general flags to set */
	xvid_dec_frame.general          = 0;
//    xvid_dec_frame.general = XVID_LOWDELAY;	/* force low_delay_default mode */

	/* Input stream */
	xvid_dec_frame.bitstream        = istream;
	xvid_dec_frame.length           = istream_size;

	/* Output frame structure */
	xvid_dec_frame.output.plane[0]  = ostream;
	xvid_dec_frame.output.stride[0] = pitch;
	xvid_dec_frame.output.csp       = colorspace;
	if(forceSkip)xvid_dec_frame.output.csp = XVID_CSP_NULL;

	return xvid_decore(dec_handle, XVID_DEC_DECODE, &xvid_dec_frame, xvid_dec_stats);
}
