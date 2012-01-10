/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - XviD Decoder part of the DShow Filter  -
 *
 *  Copyright(C) 2002-2003 Peter Ross <pross@xvid.org>
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

#ifndef _FILTER_H_
#define _FILTER_H_

#include <xvid.h>
#include "IXvidDecoder.h"


#define XVID_NAME_L		L"XviD MPEG-4 Video Decoder"

/* --- fourcc --- */

#define FOURCC_XVID	mmioFOURCC('X','V','I','D')
#define FOURCC_DIVX	mmioFOURCC('D','I','V','X')
#define FOURCC_DX50	mmioFOURCC('D','X','5','0')
#define FOURCC_MP4V	mmioFOURCC('m','p','4','v')

/* --- media uids --- */

DEFINE_GUID(CLSID_XVID,		mmioFOURCC('x','v','i','d'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_XVID_UC,	mmioFOURCC('X','V','I','D'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DIVX,		mmioFOURCC('d','i','v','x'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DIVX_UC,	mmioFOURCC('D','I','V','X'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DX50,		mmioFOURCC('d','x','5','0'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DX50_UC,	mmioFOURCC('D','X','5','0'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_MP4V,		mmioFOURCC('m','p','4','v'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);


/* MEDIATYPE_IYUV is not always defined in the directx headers */
DEFINE_GUID(CLSID_MEDIASUBTYPE_IYUV, 0x56555949, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);


class CXvidDecoder : public CVideoTransformFilter, public IXvidDecoder, public ISpecifyPropertyPages
{

public :

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	DECLARE_IUNKNOWN;

	CXvidDecoder(LPUNKNOWN punk, HRESULT *phr);
	~CXvidDecoder();

	HRESULT CheckInputType(const CMediaType * mtIn);
	HRESULT GetMediaType(int iPos, CMediaType * pmt);
	HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt);
	
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
	HRESULT DecideBufferSize(IMemAllocator * pima, ALLOCATOR_PROPERTIES * pProperties);

	HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);

	STDMETHODIMP GetPages(CAUUID * pPages);
	STDMETHODIMP FreePages(CAUUID * pPages);

private :

	HRESULT ChangeColorspace(GUID subtype, GUID formattype, void * format);
	void CloseLib();

	xvid_dec_create_t m_create;
	xvid_dec_frame_t m_frame;

	HINSTANCE m_hdll;
	int (*xvid_global_func)(void *handle, int opt, void *param1, void *param2);
	int (*xvid_decore_func)(void *handle, int opt, void *param1, void *param2);
	int ar_x, ar_y;

	int rgb_flip;
};


#endif /* _FILTER_H_ */
