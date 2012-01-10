#ifndef _OGGDEC

#include <windows.h>
#include "codec.h"

#define OGG_DEC_ERR			-1
#define OGG_DEC_END			0
#define OGG_DEC_CONTINUE	1
#define OGG_DEC_REPLAY		2

class OggDec{
	ogg_int16_t			convbuffer[4096];
	int					convsize;
	int					eos;
	ogg_sync_state		oy; 
	ogg_stream_state	os; 
	ogg_page			og; 
	ogg_packet			op; 
	vorbis_info			vi; 
	vorbis_comment		vc; 
	vorbis_dsp_state	vd; 
	vorbis_block		vb; 
	WAVEFORMATEX		wfmt;
	BYTE				bLoop;
	BYTE				bWaveGet;
	BYTE				decStep;
	BYTE				d1loop;
	char				*leak_mem;
	int					leakSize;
	int					allocMemSize;

public:
	OggDec(){
		Start_ogg();
	}
	~OggDec(){
		Clear_ogg();
	}
	void Start_ogg(void);	
	void Clear_ogg(void);	
	WORD GetVer(void);		
	BOOL GetWaveformat(WAVEFORMATEX *wfx,char *buf);
	int  DecodeOGG(char *inmemory,int inmemsize,
					char *outmemory,int outmemsize,int *done);
	void SetLoop(BOOL set){bLoop = set;}
};

#define _OGGDEC
#endif  _OGGDEC
