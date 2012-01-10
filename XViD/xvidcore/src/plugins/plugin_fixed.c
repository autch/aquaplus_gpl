/*****************************************************************************
 *
 *  XviD VBR Library
 *  - Fixed quantizer controller implementation / Deprecated code -
 *
 *  Copyright (C) 2002-2003 Edouard Gomez <ed.gomez@free.fr>
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 *
 ****************************************************************************/


#include "../xvid.h"
#include "../image/image.h"


typedef struct
{
	int32_t quant_increment;
	int32_t quant_base;

	/* Number of quantizers in the *quant distribution */
	int nquant;

	/* Quantizer distribution */
	int *quant;
} rc_fixed_t;


static int gcd(int a, int b)
{
    int r;

    while(b) {
	    r = a % b;
	    a = b;
	    b = r;
    }

    return(a);
}


static int
distribute_quants(int **quants, int lquant, int n_lquant, int n_hquant)
{

	int i, n, _gcd;
	int nlow, nhigh;
	int low, high;
	int q, r;
	int a,b,min, max;
	int m, M;
	int *dist;

	/* Low quant */
	low = lquant;

	/* High quant is just low quant + 1 */
	high = low + 1;

	/* How much low quants we have to distribute */
	nlow = n_lquant;

	/* How much high quants we have to distribute */
	nhigh = n_hquant;

	/* Simplify the pattern, respecting the current ratio nhigh/nlow */
	_gcd = gcd( nlow, nhigh);
	nlow  = nlow  / _gcd;
	nhigh = nhigh / _gcd;

	/* Allocate the distributuion array */
	if((*quants = (int*)malloc((nhigh + nlow)*sizeof(int))) == NULL)
		return (-1);

	/* Validate distribution size */
	n = nlow + nhigh;

	/*
	 * Our goal now is to find packet length so we distribute the quants
	 * the best we can. The solution i chose is quite simple.
	 *
	 * Let's 'max' be max(nlow, nhigh)
	 * Let's 'min' be min(nlow, nhigh)
	 *
	 * Then if we compute the euclidian division of 'max' by 'min', the
	 * quotient 'q' represents the length of 'max type quant' packets
	 * length. Let's call 'r' the remaining part of the euclidian division.
	 *
	 * We could stop there and have something like this:
	 *         M == quant type of the quantity 'max'
	 *         m == quant type of the quantity 'min'
	 *
	 * (MM.........Mm)*(MMMMMM...M)
	 *  '----------'    '--------'
	 *  '----|------'       |-> repeated 'r' times
	 *       ||
	 *       ||--> repeated 'min' times
	 *       |---> repeated 'q' times'
	 *
	 *
	 * Let's look if we have precision quants.
	 *
	 * We have 'min' packets of (q+1) length
	 * We have also a packet of length 'r'
	 * So : min*(q+1) + r == (min*q + r) + min == max + min == n
	 *
	 * Ok this is good :-)
	 *
	 * Now let's complicate things a bit :-)))
	 *
	 * We will distribute the last M quantizers all over the distribution
	 * array. To achieve that, we build packets of (q+2) quantizers containing
	 * qM + 1m as usual plus 1M from the last remaining M quantizer packet and
	 * usual packets of (q+1) with qM and 1m.
	 *
	 * So we will have this kind of distribution:
	 *
 	 * (MM.........Mm)*(MMMM...Mm)
	 *  '----------'    '------'
	 *  '----|------'   '---|---'
	 *       ||             ||--> repeated 'min-r' times
	 *       ||             |---> repeated 'q' times
	 *       ||--> repeated 'r' times
	 *       |---> repeated 'q+1' times
	 *
	 *
	 * Let's check if it's still right:
	 *
	 * We'll have ((q+1)*r + q*(min-r))M quants == (q*r + r + q*min - q*r)M
	 *                                          == (q*min + r)M
	 *                                          == (max)M it's good
	 * And: (r + min - r)m quantizers == (min)m it's good too)
	 *
	 * NB: if the fixed quantizer is an integer then we have min == 0, so we
	 *     must take care of it apart (else the euclidian division leads to
	 *     an arithmetic signal - division by zero)
	 *
	 */


	max = MAX(nlow, nhigh);
	min = MIN(nlow, nhigh);

	/* Euclidian division */
	if(min == 0) {
		q = 0;
		r = max;
	} else {
		q = max/min;
		r = max%min;
	}

	/* How much packets of (q+1)M quantizers + 1m quantizer */
	a = r;

	/* How much packets of (q)M quantizers + 1m quantizer */
	b = min - r;

	/* First we set M */
	M = (max==nhigh)?high:low;

	/*
	 * Then as we can't guarantee that max != min, we are forced to set
	 * the other one according to the first value. We can't use the first
	 * formula with s/max/min/
	 */
	m = (M==high)?low:high;

	/*
	 * Ok now we know everything we have to know to distribute those funny
	 * quantizers. What about just doing it ?
	 */

	dist = *quants;

	/* Distribute big packets */
	for(i=0; i<a; i++) {

		int j;

		/* Repeat q+1 times the M quantizer */
		for(j=0; j<(q+1); j++) *(dist++) = M;

		/* Put a m quantizer */
		*(dist++) = m;

	}

	/* Distribute the little packets */
	for(i=0; i<b; i++) {

		int j;

		/* Repeat q times the M quantizer */
		for(j=0; j<q; j++) *(dist++) = M;

		/* Put a m quantizer */
		*(dist++) = m;

	}

	return(n);
}




static int rc_fixed_create(xvid_plg_create_t * create, rc_fixed_t ** handle)
{
    xvid_plugin_fixed_t * param = (xvid_plugin_fixed_t *)create->param;
	rc_fixed_t * rc;
	int quant_low;
	int nquant_low, nquant_high;

    /* allocate context struct */
	if((rc = malloc(sizeof(rc_fixed_t))) == NULL)
		return(XVID_ERR_MEMORY);

	/* Copy data to private area */
	rc->quant_increment = param->quant_increment;
	rc->quant_base = param->quant_base;

	/* Cut down the precision up to 1/precision */
	quant_low = rc->quant_increment/rc->quant_base;

	/* Force [1..31] range */
	if(quant_low < 1) {
		quant_low = 1;
		rc->quant_base = 1;
		rc->quant_increment = quant_low;
	} else if(quant_low > 30) {
		quant_low = 31;
		rc->quant_base = 1;
		rc->quant_increment = quant_low;
	}

	/* How much low quants we have to distribute) */
	nquant_low = rc->quant_base*(quant_low+1) - rc->quant_increment;
	nquant_high = rc->quant_base - nquant_low;

	/* Distribute the quantizers */
	rc->nquant = distribute_quants(&rc->quant,
								   quant_low,
								   nquant_low,
								   nquant_high);

	if(rc->quant == NULL) {
		free(rc);
		return(XVID_ERR_MEMORY);
	}

    *handle = rc;
	return(0);
}


static int rc_fixed_destroy(rc_fixed_t * rc, xvid_plg_destroy_t * destroy)
{
	free(rc);
	return(0);
}


static int rc_fixed_before(rc_fixed_t * rc, xvid_plg_data_t * data)
{
    data->quant = rc->quant[data->frame_num%rc->nquant];
    data->type = XVID_TYPE_AUTO;
    return 0;
}


int xvid_plugin_fixed(void * handle, int opt, void * param1, void * param2)
{
    switch(opt)
    {
    case XVID_PLG_INFO :
        return 0;

    case XVID_PLG_CREATE :
        return rc_fixed_create((xvid_plg_create_t*)param1, param2);

    case XVID_PLG_DESTROY :
        return rc_fixed_destroy((rc_fixed_t*)handle, (xvid_plg_destroy_t*)param1);

    case XVID_PLG_BEFORE :
        return rc_fixed_before((rc_fixed_t*)handle, (xvid_plg_data_t*)param1);

    case XVID_PLG_AFTER :
	    return 0;
    }

    return XVID_ERR_FAIL;
}
