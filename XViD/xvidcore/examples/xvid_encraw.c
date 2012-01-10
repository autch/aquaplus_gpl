/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Console based test application  -
 *
 *  Copyright(C) 2002-2003 Christoph Lampert <gruel@web.de>
 *               2002-2003 Edouard Gomez <ed.gomez@free.fr>
 *               2003      Peter Ross <pross@xvid.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id$
 *
 ****************************************************************************/

/*****************************************************************************
 *  Application notes :
 *		                    
 *  A sequence of raw YUV I420 pics or YUV I420 PGM file format is encoded
 *  The speed is measured and frames' PSNR are taken from core. 
 *		                   
 *  The program is plain C and needs no libraries except for libxvidcore, 
 *  and maths-lib.
 *
 *  Use ./xvid_encraw -help for a list of options
 *	
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "xvid.h"

#undef READ_PNM

/*****************************************************************************
 *                            Quality presets
 ****************************************************************************/

static const int motion_presets[] = {
	/* quality 0 */
	0,

	/* quality 1 */
	XVID_ME_ADVANCEDDIAMOND16,

	/* quality 2 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16,

	/* quality 3 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
	XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8,

	/* quality 4 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
	XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8 |
	XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,

	/* quality 5 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
	XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8 |
	XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,

	/* quality 6 */
	XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 | XVID_ME_EXTSEARCH16 |
	XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8 | XVID_ME_EXTSEARCH8 |
	XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,

};
#define ME_ELEMENTS (sizeof(motion_presets)/sizeof(motion_presets[0]))

static const int vop_presets[] = {
	/* quality 0 */
	0,

	/* quality 1 */
	0,

	/* quality 2 */
	XVID_VOP_HALFPEL,

	/* quality 3 */
	XVID_VOP_HALFPEL | XVID_VOP_INTER4V,

	/* quality 4 */
	XVID_VOP_HALFPEL | XVID_VOP_INTER4V,

	/* quality 5 */
	XVID_VOP_HALFPEL | XVID_VOP_INTER4V |
	XVID_VOP_TRELLISQUANT,

	/* quality 6 */
	XVID_VOP_HALFPEL | XVID_VOP_INTER4V |
	XVID_VOP_TRELLISQUANT | XVID_VOP_HQACPRED,

};
#define VOP_ELEMENTS (sizeof(vop_presets)/sizeof(vop_presets[0]))

/*****************************************************************************
 *                     Command line global variables
 ****************************************************************************/

#define MAX_ZONES   64

static xvid_enc_zone_t ZONES[MAX_ZONES];
static int NUM_ZONES = 0;

/* Maximum number of frames to encode */
#define ABS_MAXFRAMENR 9999

static int ARG_STATS = 0;
static int ARG_DUMP = 0;
static int ARG_LUMIMASKING = 0;
static int ARG_BITRATE = 0;
static int ARG_SINGLE = 0;
static char *ARG_PASS1 = 0;
static char *ARG_PASS2 = 0;
static int ARG_QUALITY = ME_ELEMENTS - 1;
static float ARG_FRAMERATE = 25.00f;
static int ARG_MAXFRAMENR = ABS_MAXFRAMENR;
static int ARG_MAXKEYINTERVAL = 0;
static char *ARG_INPUTFILE = NULL;
static int ARG_INPUTTYPE = 0;
static int ARG_SAVEMPEGSTREAM = 0;
static int ARG_SAVEINDIVIDUAL = 0;
static char *ARG_OUTPUTFILE = NULL;
static int XDIM = 0;
static int YDIM = 0;
static int ARG_BQRATIO = 150;
static int ARG_BQOFFSET = 100;
static int ARG_MAXBFRAMES = 0;
static int ARG_PACKED = 0;
static int ARG_DEBUG = 0;
static int ARG_VOPDEBUG = 0;
static int ARG_GMC = 0;
static int ARG_QPEL = 0;
static int ARG_CLOSED_GOP = 0;

#ifndef READ_PNM
#define IMAGE_SIZE(x,y) ((x)*(y)*3/2)
#else
#define IMAGE_SIZE(x,y) ((x)*(y)*3)
#endif

#define MAX(A,B) ( ((A)>(B)) ? (A) : (B) )
#define SMALL_EPS (1e-10)

#define SWAP(a) ( (((a)&0x000000ff)<<24) | (((a)&0x0000ff00)<<8) | \
                  (((a)&0x00ff0000)>>8)  | (((a)&0xff000000)>>24) )

/****************************************************************************
 *                     Nasty global vars ;-)
 ***************************************************************************/

static int i;

/* the path where to save output */
static char filepath[256] = "./";

/* Internal structures (handles) for encoding and decoding */
static void *enc_handle = NULL;

/*****************************************************************************
 *               Local prototypes
 ****************************************************************************/

/* Prints program usage message */
static void usage();

/* Statistical functions */
static double msecond();

/* PGM related functions */
#ifndef READ_PNM
static int read_pgmheader(FILE * handle);
static int read_pgmdata(FILE * handle,
						unsigned char *image);
#else
static int read_pnmheader(FILE * handle);
static int read_pnmdata(FILE * handle,
						unsigned char *image);
#endif
static int read_yuvdata(FILE * handle,
						unsigned char *image);

/* Encoder related functions */
static int enc_init(int use_assembler);
static int enc_stop();
static int enc_main(unsigned char *image,
					unsigned char *bitstream,
					int *key,
					int *stats_type,
					int *stats_quant,
					int *stats_length,
					int stats[3]);

/*****************************************************************************
 *               Main function
 ****************************************************************************/

int
main(int argc,
	 char *argv[])
{

	unsigned char *mp4_buffer = NULL;
	unsigned char *in_buffer = NULL;
	unsigned char *out_buffer = NULL;

	double enctime;
	double totalenctime = 0.;
	float totalPSNR[3] = {0., 0., 0.};

	int totalsize;
	int result;
	int m4v_size;
	int key;
	int stats_type;
	int stats_quant;
	int stats_length;
	int use_assembler = 0;

	int input_num;
	int output_num;

	char filename[256];

	FILE *in_file = stdin;
	FILE *out_file = NULL;

	printf("xvid_encraw - raw mpeg4 bitstream encoder ");
	printf("written by Christoph Lampert 2002-2003\n\n");

	/* Is there a dumb XviD coder ? */
	if(ME_ELEMENTS != VOP_ELEMENTS) {
		fprintf(stderr, "Presets' arrays should have the same number of elements -- Please fill a bug to xvid-devel@xvid.org\n");
		return(-1);
	}

/*****************************************************************************
 *                            Command line parsing
 ****************************************************************************/

	for (i = 1; i < argc; i++) {

		if (strcmp("-asm", argv[i]) == 0) {
			use_assembler = 1;
		} else if (strcmp("-w", argv[i]) == 0 && i < argc - 1) {
			i++;
			XDIM = atoi(argv[i]);
		} else if (strcmp("-h", argv[i]) == 0 && i < argc - 1) {
			i++;
			YDIM = atoi(argv[i]);
        } else if (strcmp("-bitrate", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_BITRATE = atoi(argv[i]);
		} else if (strcmp("-single", argv[i]) == 0) {
			ARG_SINGLE = 1;
		} else if (strcmp("-pass1", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_PASS1 = argv[i];
		} else if (strcmp("-pass2", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_PASS2 = argv[i];
		} else if (strcmp("-max_bframes", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_MAXBFRAMES = atoi(argv[i]);
		} else if (strcmp("-packed", argv[i]) == 0) {
			ARG_PACKED = 1;
		} else if (strcmp("-bquant_ratio", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_BQRATIO = atoi(argv[i]);
		} else if (strcmp("-bquant_offset", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_BQOFFSET = atoi(argv[i]);

        } else if ((strcmp("-zq", argv[i]) == 0 || strcmp("-zw", argv[i]) == 0) && i < argc - 2) {

            if (NUM_ZONES >= MAX_ZONES) {
                fprintf(stderr,"warning: too many zones; zone ignored\n");
                continue;
            }
            ZONES[NUM_ZONES].mode = strcmp("-zq", argv[i])==0 ? XVID_ZONE_QUANT : XVID_ZONE_WEIGHT;
			i++;
            ZONES[NUM_ZONES].frame = atoi(argv[i]);            
            i++;
            ZONES[NUM_ZONES].increment  = (int)(atof(argv[i]) * 100);
            ZONES[NUM_ZONES].base  = 100;
            NUM_ZONES++;
        
		} else if (strcmp("-quality", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_QUALITY = atoi(argv[i]);
		} else if (strcmp("-framerate", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_FRAMERATE = (float) atof(argv[i]);
		} else if (strcmp("-max_key_interval", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_MAXKEYINTERVAL = atoi(argv[i]);
		} else if (strcmp("-i", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_INPUTFILE = argv[i];
		} else if (strcmp("-stats", argv[i]) == 0) {
			ARG_STATS = 1;
		} else if (strcmp("-dump", argv[i]) == 0) {
			ARG_DUMP = 1;
		} else if (strcmp("-lumimasking", argv[i]) == 0) {
			ARG_LUMIMASKING = 1;
		} else if (strcmp("-type", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_INPUTTYPE = atoi(argv[i]);
		} else if (strcmp("-frames", argv[i]) == 0 && i < argc - 1) {
			i++;
			ARG_MAXFRAMENR = atoi(argv[i]);
		} else if (strcmp("-save", argv[i]) == 0) {
			ARG_SAVEMPEGSTREAM = 1;
			ARG_SAVEINDIVIDUAL = 1;
		} else if (strcmp("-debug", argv[i]) == 0) {
			i++;
            if (sscanf(argv[i],"0x%x", &ARG_DEBUG) || sscanf(argv[i],"%d", &ARG_DEBUG)) ;
		} else if (strcmp("-o", argv[i]) == 0 && i < argc - 1) {
			ARG_SAVEMPEGSTREAM = 1;
			i++;
			ARG_OUTPUTFILE = argv[i];
		} else if (strcmp("-vop_debug", argv[i]) == 0) {
			ARG_VOPDEBUG = 1;
		} else if (strcmp("-qpel", argv[i]) == 0) {
			ARG_QPEL = 1;
		} else if (strcmp("-gmc", argv[i]) == 0) {
			ARG_GMC = 1;
		} else if (strcmp("-closed_gop", argv[i]) == 0) {
			ARG_CLOSED_GOP = 1;
		} else if (strcmp("-help", argv[i])) {
			usage();
			return (0);
		} else {
			usage();
			exit(-1);
		}

	}

/*****************************************************************************
 *                            Arguments checking
 ****************************************************************************/

	if (XDIM <= 0 || XDIM >= 4096 || YDIM <= 0 || YDIM >= 4096) {
		fprintf(stderr,
				"Trying to retreive width and height from PGM header\n");
		ARG_INPUTTYPE = 1;		/* pgm */
	}

	if (ARG_QUALITY < 0 ) {
		ARG_QUALITY = 0;
	} else if (ARG_QUALITY >= ME_ELEMENTS) {
		ARG_QUALITY = ME_ELEMENTS - 1;
	}

	if (ARG_FRAMERATE <= 0) {
		fprintf(stderr, "Wrong Framerate %s \n", argv[5]);
		return (-1);
	}

	if (ARG_MAXFRAMENR <= 0) {
		fprintf(stderr, "Wrong number of frames\n");
		return (-1);
	}

	if (ARG_INPUTFILE == NULL || strcmp(ARG_INPUTFILE, "stdin") == 0) {
		in_file = stdin;
	} else {

		in_file = fopen(ARG_INPUTFILE, "rb");
		if (in_file == NULL) {
			fprintf(stderr, "Error opening input file %s\n", ARG_INPUTFILE);
			return (-1);
		}
	}

	if (ARG_INPUTTYPE) {
#ifndef READ_PNM
		if (read_pgmheader(in_file)) {
#else
		if (read_pnmheader(in_file)) {
#endif
			fprintf(stderr,
					"Wrong input format, I want YUV encapsulated in PGM\n");
			return (-1);
		}
	}

	/* now we know the sizes, so allocate memory */
	in_buffer = (unsigned char *) malloc(IMAGE_SIZE(XDIM, YDIM));
	if (!in_buffer)
		goto free_all_memory;

	/* this should really be enough memory ! */
	mp4_buffer = (unsigned char *) malloc(IMAGE_SIZE(XDIM, YDIM) * 2);
	if (!mp4_buffer)
		goto free_all_memory;

/*****************************************************************************
 *                            XviD PART  Start
 ****************************************************************************/


	result = enc_init(use_assembler);
	if (result) {
		fprintf(stderr, "Encore INIT problem, return value %d\n", result);
		goto release_all;
	}

/*****************************************************************************
 *                            Main loop
 ****************************************************************************/

	if (ARG_SAVEMPEGSTREAM && ARG_OUTPUTFILE) {

		if ((out_file = fopen(ARG_OUTPUTFILE, "w+b")) == NULL) {
			fprintf(stderr, "Error opening output file %s\n", ARG_OUTPUTFILE);
			goto release_all;
		}

	} else {
		out_file = NULL;
	}

/*****************************************************************************
 *                       Encoding loop
 ****************************************************************************/

	totalsize = 0;

	result = 0;

	input_num = 0;				/* input frame counter */
	output_num = 0;				/* output frame counter */

	do {

		char *type;
		int sse[3];

		if (input_num >= ARG_MAXFRAMENR) {
			result = 1;
		}

		if (!result) {
			if (ARG_INPUTTYPE) {
				/* read PGM data (YUV-format) */
#ifndef READ_PNM
				result = read_pgmdata(in_file, in_buffer);
#else
				result = read_pnmdata(in_file, in_buffer);
#endif
			} else {
				/* read raw data (YUV-format) */
				result = read_yuvdata(in_file, in_buffer);
			}
		}

/*****************************************************************************
 *                       Encode and decode this frame
 ****************************************************************************/

		enctime = msecond();
		m4v_size =
			enc_main(!result ? in_buffer : 0, mp4_buffer, &key, &stats_type,
					 &stats_quant, &stats_length, sse);
		enctime = msecond() - enctime;

		/* Write the Frame statistics */

		printf("%5d: key=%i, time= %6.0f, len= %7d", !result ? input_num : -1,
			   key, (float) enctime, (int) m4v_size);

		if (stats_type > 0) {	/* !XVID_TYPE_NOTHING */

			switch (stats_type) {
			case XVID_TYPE_IVOP:
				type = "I";
				break;
			case XVID_TYPE_PVOP:
				type = "P";
				break;
			case XVID_TYPE_BVOP:
				type = "B";
				break;
			case XVID_TYPE_SVOP:
				type = "S";
				break;
			default:
				type = "U";
				break;
			}

			printf(" | type=%s, quant= %2d, len= %7d", type, stats_quant,
				   stats_length);

#define SSE2PSNR(sse, width, height) ((!(sse))?0.0f : 48.131f - 10*(float)log10((float)(sse)/((float)((width)*(height)))))

			if (ARG_STATS) {
				printf(", psnr y = %2.2f, psnr u = %2.2f, psnr v = %2.2f",
					   SSE2PSNR(sse[0], XDIM, YDIM), SSE2PSNR(sse[1], XDIM / 2,
															  YDIM / 2),
					   SSE2PSNR(sse[2], XDIM / 2, YDIM / 2));

				totalPSNR[0] += SSE2PSNR(sse[0], XDIM, YDIM);
				totalPSNR[1] += SSE2PSNR(sse[1], XDIM/2, YDIM/2);
				totalPSNR[2] += SSE2PSNR(sse[2], XDIM/2, YDIM/2);
			}

		}
#undef SSE2PSNR

		printf("\n");

		if (m4v_size < 0) {
			break;
		}

		/* Update encoding time stats */
		totalenctime += enctime;
		totalsize += m4v_size;

/*****************************************************************************
 *                       Save stream to file
 ****************************************************************************/

		if (m4v_size > 0 && ARG_SAVEMPEGSTREAM) {

			/* Save single files */
			if (ARG_SAVEINDIVIDUAL) {
				FILE *out;
				sprintf(filename, "%sframe%05d.m4v", filepath, output_num);
				out = fopen(filename, "w+b");
				fwrite(mp4_buffer, m4v_size, 1, out);
				fclose(out);
				output_num++;
			}

			/* Save ES stream */
			if (ARG_OUTPUTFILE && out_file)
				fwrite(mp4_buffer, 1, m4v_size, out_file);
		}

		input_num++;

		/* Read the header if it's pgm stream */
		if (!result && ARG_INPUTTYPE)
#ifndef READ_PNM
 			result = read_pgmheader(in_file);
#else
 			result = read_pnmheader(in_file);
#endif
	} while (1);



/*****************************************************************************
 *         Calculate totals and averages for output, print results
 ****************************************************************************/

	printf("Tot: enctime(ms) =%7.2f,               length(bytes) = %7d\n",
		   totalenctime, (int) totalsize);

	if (input_num > 0) {
		totalsize /= input_num;
		totalenctime /= input_num;
		totalPSNR[0] /= input_num;
		totalPSNR[1] /= input_num;
		totalPSNR[2] /= input_num;
	} else {
		totalsize = -1;
		totalenctime = -1;
	}

	printf("Avg: enctime(ms) =%7.2f, fps =%7.2f, length(bytes) = %7d",
		   totalenctime, 1000 / totalenctime, (int) totalsize);
   if (ARG_STATS) {
       printf(", psnr y = %2.2f, psnr u = %2.2f, psnr v = %2.2f",
    		  totalPSNR[0],totalPSNR[1],totalPSNR[2]);
	}
	printf("\n");


/*****************************************************************************
 *                            XviD PART  Stop
 ****************************************************************************/

  release_all:

	if (enc_handle) {
		result = enc_stop();
		if (result)
			fprintf(stderr, "Encore RELEASE problem return value %d\n",
					result);
	}

	if (in_file)
		fclose(in_file);
	if (out_file)
		fclose(out_file);

  free_all_memory:
	free(out_buffer);
	free(mp4_buffer);
	free(in_buffer);

	return (0);

}


/*****************************************************************************
 *                        "statistical" functions
 *
 *  these are not needed for encoding or decoding, but for measuring
 *  time and quality, there in nothing specific to XviD in these
 *
 *****************************************************************************/

/* Return time elapsed time in miliseconds since the program started */
static double
msecond()
{
#ifndef WIN32
	struct timeval tv;

	gettimeofday(&tv, 0);
	return (tv.tv_sec * 1.0e3 + tv.tv_usec * 1.0e-3);
#else
	clock_t clk;

	clk = clock();
	return (clk * 1000 / CLOCKS_PER_SEC);
#endif
}

/*****************************************************************************
 *                             Usage message
 *****************************************************************************/

static void
usage()
{
	fprintf(stderr, "Usage : xvid_stat [OPTIONS]\n\n");
	fprintf(stderr, "Input options:\n");
	fprintf(stderr, " -i      string : input filename (default=stdin)\n");
	fprintf(stderr, " -type   integer: input data type (yuv=0, pgm=1)\n");
	fprintf(stderr, " -w      integer: frame width ([1.2048])\n");
	fprintf(stderr, " -h      integer: frame height ([1.2048])\n");
	fprintf(stderr, " -frames integer: number of frames to encode\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Output options:\n");
	fprintf(stderr, " -dump    : save decoder output\n");
	fprintf(stderr, " -save    : save an Elementary Stream file per frame\n");
	fprintf(stderr, " -o string: save an Elementary Stream for the complete sequence\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "BFrames options:\n");
	fprintf(stderr, " -max_bframes   integer: max bframes (default=0)\n");
	fprintf(stderr,	" -bquant_ratio  integer: bframe quantizer ratio (default=150)\n");
	fprintf(stderr,	" -bquant_offset integer: bframe quantizer offset (default=100)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Rate control options:\n");
	fprintf(stderr, " -framerate float               : target framerate (>0 | default=25.0)\n");
	fprintf(stderr,	" -bitrate   integer             : target bitrate\n");
    fprintf(stderr,	" -single                        : single pass mode\n");
	fprintf(stderr, " -pass1     filename            : twopass mode (first pass)\n");
	fprintf(stderr,	" -pass2     filename            : twopass mode (2nd pass)\n");
	fprintf(stderr,	" -zq starting_frame float       : bitrate zone; quant\n");
	fprintf(stderr,	" -zw starting_frame float       : bitrate zone; weight\n");
    fprintf(stderr, " -max_key_interval integer      : maximum keyframe interval\n");
    fprintf(stderr, "\n");
	fprintf(stderr, "Other options\n");
	fprintf(stderr, " -asm            : use assembly optmized code\n");
	fprintf(stderr, " -quality integer: quality ([0..%d])\n", ME_ELEMENTS - 1);
	fprintf(stderr, " -qpel           : use quarter pixel ME\n");
	fprintf(stderr, " -gmc            : use global motion compensation\n");
	fprintf(stderr, " -packed         : packed mode\n");
	fprintf(stderr, " -closed_gop     : closed GOP mode\n");
	fprintf(stderr, " -lumimasking    : use lumimasking algorithm\n");
	fprintf(stderr, " -stats          : print stats about encoded frames\n");
	fprintf(stderr, " -debug          : activates xvidcore internal debugging output\n");
	fprintf(stderr, " -vop_debug      : print some info directly into encoded frames\n");
	fprintf(stderr, " -help           : prints this help message\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "NB: You can define %d zones repeating the -z[qw] option as many times as needed.\n", MAX_ZONES);
	fprintf(stderr, "\n");
}

/*****************************************************************************
 *                       Input and output functions
 *
 *      the are small and simple routines to read and write PGM and YUV
 *      image. It's just for convenience, again nothing specific to XviD
 *
 *****************************************************************************/

#ifndef READ_PNM
static int
read_pgmheader(FILE * handle)
{
	int bytes, xsize, ysize, depth;
	char dummy[2];

	bytes = fread(dummy, 1, 2, handle);

	if ((bytes < 2) || (dummy[0] != 'P') || (dummy[1] != '5'))
		return (1);

	fscanf(handle, "%d %d %d", &xsize, &ysize, &depth);
	if ((xsize > 4096) || (ysize > 4096*3/2) || (depth != 255)) {
		fprintf(stderr, "%d %d %d\n", xsize, ysize, depth);
		return (2);
	}
	if ((XDIM == 0) || (YDIM == 0)) {
		XDIM = xsize;
		YDIM = ysize * 2 / 3;
	}

	return (0);
}

static int
read_pgmdata(FILE * handle,
			 unsigned char *image)
{
	int i;
	char dummy;

	unsigned char *y = image;
	unsigned char *u = image + XDIM * YDIM;
	unsigned char *v = image + XDIM * YDIM + XDIM / 2 * YDIM / 2;

	/* read Y component of picture */
	fread(y, 1, XDIM * YDIM, handle);

	for (i = 0; i < YDIM / 2; i++) {
		/* read U */
		fread(u, 1, XDIM / 2, handle);

		/* read V */
		fread(v, 1, XDIM / 2, handle);

		/* Update pointers */
		u += XDIM / 2;
		v += XDIM / 2;
	}

	/*  I don't know why, but this seems needed */
	fread(&dummy, 1, 1, handle);

	return (0);
}
#else
static int
read_pnmheader(FILE * handle)
{
	int bytes, xsize, ysize, depth;
	char dummy[2];

	bytes = fread(dummy, 1, 2, handle);

	if ((bytes < 2) || (dummy[0] != 'P') || (dummy[1] != '6'))
		return (1);

	fscanf(handle, "%d %d %d", &xsize, &ysize, &depth);
	if ((xsize > 1440) || (ysize > 2880) || (depth != 255)) {
		fprintf(stderr, "%d %d %d\n", xsize, ysize, depth);
		return (2);
	}

	XDIM = xsize;
	YDIM = ysize;

	return (0);
}

static int
read_pnmdata(FILE * handle,
			 unsigned char *image)
{
	int i;
	char dummy;

	/* read Y component of picture */
	fread(image, 1, XDIM * YDIM * 3, handle);

	/*  I don't know why, but this seems needed */
	fread(&dummy, 1, 1, handle);

	return (0);
}
#endif

static int
read_yuvdata(FILE * handle,
			 unsigned char *image)
{

	if (fread(image, 1, IMAGE_SIZE(XDIM, YDIM), handle) !=
		(unsigned int) IMAGE_SIZE(XDIM, YDIM))
		return (1);
	else
		return (0);
}

/*****************************************************************************
 *     Routines for encoding: init encoder, frame step, release encoder
 ****************************************************************************/

/* sample plugin */

int
rawenc_debug(void *handle,
			 int opt,
			 void *param1,
			 void *param2)
{
	switch (opt) {
	case XVID_PLG_INFO:
		{
			xvid_plg_info_t *info = (xvid_plg_info_t *) param1;

			info->flags = XVID_REQDQUANTS;
			return 0;
		}

	case XVID_PLG_CREATE:
	case XVID_PLG_DESTROY:
	case XVID_PLG_BEFORE:
		return 0;

	case XVID_PLG_AFTER:
		{
			xvid_plg_data_t *data = (xvid_plg_data_t *) param1;
			int i, j;

			printf("---[ frame: %5i   quant: %2i   length: %6i ]---\n",
				   data->frame_num, data->quant, data->length);
			for (j = 0; j < data->mb_height; j++) {
				for (i = 0; i < data->mb_width; i++)
					printf("%2i ", data->dquant[j * data->dquant_stride + i]);
				printf("\n");
			}

			return 0;
		}
	}

	return XVID_ERR_FAIL;
}


#define FRAMERATE_INCR 1001


/* Initialize encoder for first use, pass all needed parameters to the codec */
static int
enc_init(int use_assembler)
{
	int xerr;
	//xvid_plugin_cbr_t cbr;
    xvid_plugin_single_t single;
	xvid_plugin_2pass1_t rc2pass1;
	xvid_plugin_2pass2_t rc2pass2;
	//xvid_plugin_fixed_t rcfixed;
	xvid_enc_plugin_t plugins[7];
	xvid_gbl_init_t xvid_gbl_init;
	xvid_enc_create_t xvid_enc_create;

	/*------------------------------------------------------------------------
	 * XviD core initialization
	 *----------------------------------------------------------------------*/

	/* Set version -- version checking will done by xvidcore */
	memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init));
	xvid_gbl_init.version = XVID_VERSION;
    xvid_gbl_init.debug = ARG_DEBUG;


	/* Do we have to enable ASM optimizations ? */
	if (use_assembler) {

#ifdef ARCH_IS_IA64
		xvid_gbl_init.cpu_flags = XVID_CPU_FORCE | XVID_CPU_ASM;
#else
		xvid_gbl_init.cpu_flags = 0;
#endif
	} else {
		xvid_gbl_init.cpu_flags = XVID_CPU_FORCE;
	}

	/* Initialize XviD core -- Should be done once per __process__ */
	xvid_global(NULL, XVID_GBL_INIT, &xvid_gbl_init, NULL);

	/*------------------------------------------------------------------------
	 * XviD encoder initialization
	 *----------------------------------------------------------------------*/

	/* Version again */
	memset(&xvid_enc_create, 0, sizeof(xvid_enc_create));
	xvid_enc_create.version = XVID_VERSION;

	/* Width and Height of input frames */
	xvid_enc_create.width = XDIM;
	xvid_enc_create.height = YDIM;
	xvid_enc_create.profile = XVID_PROFILE_AS_L4;

	/* init plugins  */
    xvid_enc_create.zones = ZONES;
    xvid_enc_create.num_zones = NUM_ZONES;

	xvid_enc_create.plugins = plugins;
	xvid_enc_create.num_plugins = 0;

	if (ARG_SINGLE) {
		memset(&single, 0, sizeof(xvid_plugin_single_t));
		single.version = XVID_VERSION;
		single.bitrate = ARG_BITRATE;

		plugins[xvid_enc_create.num_plugins].func = xvid_plugin_single;
		plugins[xvid_enc_create.num_plugins].param = &single;
		xvid_enc_create.num_plugins++;
	}

	if (ARG_PASS2) {
		memset(&rc2pass2, 0, sizeof(xvid_plugin_2pass2_t));
		rc2pass2.version = XVID_VERSION;
		rc2pass2.filename = ARG_PASS2;
		rc2pass2.bitrate = ARG_BITRATE;

		plugins[xvid_enc_create.num_plugins].func = xvid_plugin_2pass2;
		plugins[xvid_enc_create.num_plugins].param = &rc2pass2;
		xvid_enc_create.num_plugins++;
	}

	if (ARG_PASS1) {
		memset(&rc2pass1, 0, sizeof(xvid_plugin_2pass1_t));
		rc2pass1.version = XVID_VERSION;
		rc2pass1.filename = ARG_PASS1;

		plugins[xvid_enc_create.num_plugins].func = xvid_plugin_2pass1;
		plugins[xvid_enc_create.num_plugins].param = &rc2pass1;
		xvid_enc_create.num_plugins++;
	}

	if (ARG_LUMIMASKING) {
		plugins[xvid_enc_create.num_plugins].func = xvid_plugin_lumimasking;
		plugins[xvid_enc_create.num_plugins].param = NULL;
		xvid_enc_create.num_plugins++;
	}

	if (ARG_DUMP) {
		plugins[xvid_enc_create.num_plugins].func = xvid_plugin_dump;
		plugins[xvid_enc_create.num_plugins].param = NULL;
		xvid_enc_create.num_plugins++;
	}

#if 0
	if (ARG_DEBUG) {
		plugins[xvid_enc_create.num_plugins].func = rawenc_debug;
		plugins[xvid_enc_create.num_plugins].param = NULL;
		xvid_enc_create.num_plugins++;
	}
#endif

	/* No fancy thread tests */
	xvid_enc_create.num_threads = 0;

	/* Frame rate - Do some quick float fps = fincr/fbase hack */
	if ((ARG_FRAMERATE - (int) ARG_FRAMERATE) < SMALL_EPS) {
		xvid_enc_create.fincr = 1;
		xvid_enc_create.fbase = (int) ARG_FRAMERATE;
	} else {
		xvid_enc_create.fincr = FRAMERATE_INCR;
		xvid_enc_create.fbase = (int) (FRAMERATE_INCR * ARG_FRAMERATE);
	}

	/* Maximum key frame interval */
    if (ARG_MAXKEYINTERVAL > 0) {
        xvid_enc_create.max_key_interval = ARG_MAXKEYINTERVAL;
    }else {
	    xvid_enc_create.max_key_interval = (int) ARG_FRAMERATE *10;
    }

	/* Bframes settings */
	xvid_enc_create.max_bframes = ARG_MAXBFRAMES;
	xvid_enc_create.bquant_ratio = ARG_BQRATIO;
	xvid_enc_create.bquant_offset = ARG_BQOFFSET;

	/* Dropping ratio frame -- we don't need that */
	xvid_enc_create.frame_drop_ratio = 0;

	/* Global encoder options */
	xvid_enc_create.global = 0;

	if (ARG_PACKED)
		xvid_enc_create.global |= XVID_GLOBAL_PACKED;

	if (ARG_CLOSED_GOP)
		xvid_enc_create.global |= XVID_GLOBAL_CLOSED_GOP;

	if (ARG_STATS)
		xvid_enc_create.global |= XVID_GLOBAL_EXTRASTATS_ENABLE;

	/* I use a small value here, since will not encode whole movies, but short clips */
	xerr = xvid_encore(NULL, XVID_ENC_CREATE, &xvid_enc_create, NULL);

	/* Retrieve the encoder instance from the structure */
	enc_handle = xvid_enc_create.handle;

	return (xerr);
}

static int
enc_stop()
{
	int xerr;

	/* Destroy the encoder instance */
	xerr = xvid_encore(enc_handle, XVID_ENC_DESTROY, NULL, NULL);

	return (xerr);
}

static int
enc_main(unsigned char *image,
		 unsigned char *bitstream,
		 int *key,
		 int *stats_type,
		 int *stats_quant,
		 int *stats_length,
		 int sse[3])
{
	int ret;

	xvid_enc_frame_t xvid_enc_frame;
	xvid_enc_stats_t xvid_enc_stats;

	/* Version for the frame and the stats */
	memset(&xvid_enc_frame, 0, sizeof(xvid_enc_frame));
	xvid_enc_frame.version = XVID_VERSION;

	memset(&xvid_enc_stats, 0, sizeof(xvid_enc_stats));
	xvid_enc_stats.version = XVID_VERSION;

	/* Bind output buffer */
	xvid_enc_frame.bitstream = bitstream;
	xvid_enc_frame.length = -1;

	/* Initialize input image fields */
	if (image) {
		xvid_enc_frame.input.plane[0] = image;
#ifndef READ_PNM
		xvid_enc_frame.input.csp = XVID_CSP_I420;
		xvid_enc_frame.input.stride[0] = XDIM;
#else
		xvid_enc_frame.input.csp = XVID_CSP_BGR;
		xvid_enc_frame.input.stride[0] = XDIM*3;
#endif
	} else {
		xvid_enc_frame.input.csp = XVID_CSP_NULL;
	}

	/* Set up core's general features */
	xvid_enc_frame.vol_flags = 0;
	if (ARG_STATS)
		xvid_enc_frame.vol_flags |= XVID_VOL_EXTRASTATS;
	if (ARG_QPEL)
		xvid_enc_frame.vol_flags |= XVID_VOL_QUARTERPEL;
	if (ARG_GMC)
		xvid_enc_frame.vol_flags |= XVID_VOL_GMC;

	/* Set up core's general features */
	xvid_enc_frame.vop_flags = vop_presets[ARG_QUALITY];

    if (ARG_VOPDEBUG) {
        xvid_enc_frame.vop_flags |= XVID_VOP_DEBUG;
    }

	/* Frame type -- let core decide for us */
	xvid_enc_frame.type = XVID_TYPE_AUTO;

	/* Force the right quantizer -- It is internally managed by RC plugins */
	xvid_enc_frame.quant = 0;

	/* Set up motion estimation flags */
	xvid_enc_frame.motion = motion_presets[ARG_QUALITY];

	if (ARG_GMC)
		xvid_enc_frame.motion |= XVID_ME_GME_REFINE;

	if (ARG_QPEL)
		xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE16;
	if (ARG_QPEL && (xvid_enc_frame.vop_flags & XVID_VOP_INTER4V))
		xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE8;

	/* We don't use special matrices */
	xvid_enc_frame.quant_intra_matrix = NULL;
	xvid_enc_frame.quant_inter_matrix = NULL;

	/* Encode the frame */
	ret = xvid_encore(enc_handle, XVID_ENC_ENCODE, &xvid_enc_frame,
					  &xvid_enc_stats);

	*key = (xvid_enc_frame.out_flags & XVID_KEYFRAME);
	*stats_type = xvid_enc_stats.type;
	*stats_quant = xvid_enc_stats.quant;
	*stats_length = xvid_enc_stats.length;
	sse[0] = xvid_enc_stats.sse_y;
	sse[1] = xvid_enc_stats.sse_u;
	sse[2] = xvid_enc_stats.sse_v;

	return (ret);
}
