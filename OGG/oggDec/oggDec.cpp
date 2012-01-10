
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "oggdec.h"

#include <io.h>
#include <fcntl.h>

void OggDec::Start_ogg()
{
	memset(this,0,sizeof(OggDec));
	ogg_sync_init(&oy);
}
  
void OggDec::Clear_ogg()
{
	ogg_sync_clear(&oy);
	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
	bWaveGet = FALSE;
	bLoop = FALSE;
	decStep = 0;
	eos = 0;
	if(leakSize){
		free(leak_mem);
		leak_mem = NULL;
		leakSize = 0;
		allocMemSize = 0;
	}
}

WORD OggDec::GetVer()
{
	return (0x0107);
}

BOOL OggDec::GetWaveformat(WAVEFORMATEX *wfx,char *buf)
{
	char	*buffer;

	if(NULL==wfx)return FALSE;
	if(bWaveGet){
		memcpy(wfx,&wfmt,sizeof(WAVEFORMATEX));
		return TRUE;
	}else if(NULL==buf){
		return FALSE;
	}
	buffer=ogg_sync_buffer(&oy,4096);
	memcpy(buffer,buf,4096);
	ogg_sync_wrote(&oy,4096);
	ogg_sync_pageout(&oy,&og);
	ogg_stream_init(&os,ogg_page_serialno(&og));
	vorbis_info_init(&vi);
	vorbis_comment_init(&vc);
	ogg_stream_pagein(&os,&og);
	ogg_stream_packetout(&os,&op);
	vorbis_synthesis_headerin(&vi,&vc,&op);

	wfx->wFormatTag = WAVE_FORMAT_PCM;
	wfx->nChannels = vi.channels;
	wfx->wBitsPerSample = 16;
	wfx->nSamplesPerSec = vi.rate;
	wfx->nBlockAlign = wfx->nChannels * (wfx->wBitsPerSample/8);
	wfx->nAvgBytesPerSec = wfx->nSamplesPerSec * wfx->nBlockAlign;
	wfx->cbSize = 0;

	ogg_sync_clear(&oy);
	ogg_stream_clear(&os);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
	return TRUE;
}

int OggDec::DecodeOGG(char *inmemory,int inmemsize,
                char *outmemory,int outmemsize,int *done)
{
	char	*buffer;
	int		i;

	*done = 0;
	if(leakSize){
		if(outmemsize >= leakSize){
			memcpy(outmemory,leak_mem,leakSize);
			*done = leakSize;
			free(leak_mem);
			leak_mem = NULL;
			leakSize = 0;
			allocMemSize = 0;
		}else{
			memcpy(outmemory,leak_mem,outmemsize);
			*done = outmemsize;
			memmove(leak_mem,leak_mem+outmemsize,leakSize-outmemsize);
			leakSize -= outmemsize;
		}
		if(inmemsize==0) return (OGG_DEC_CONTINUE);
	}
	if(0==decStep){
		eos = 0;
		buffer=ogg_sync_buffer(&oy,4096);
		memcpy(buffer,inmemory,inmemsize);
		ogg_sync_wrote(&oy,inmemsize);
		if(ogg_sync_pageout(&oy,&og)!=1){
			if(inmemsize<4096){
				Clear_ogg();
				return(OGG_DEC_END);
			}
			return(OGG_DEC_ERR);
		}
		ogg_stream_init(&os,ogg_page_serialno(&og));
		vorbis_info_init(&vi);
		vorbis_comment_init(&vc);
		if(ogg_stream_pagein(&os,&og)<0){ 
			return(OGG_DEC_ERR);
		}
		if(ogg_stream_packetout(&os,&op)!=1){ 
			return(OGG_DEC_ERR);
		}
		if(vorbis_synthesis_headerin(&vi,&vc,&op)<0){ 
			return(OGG_DEC_ERR);
		}
		wfmt.wFormatTag = WAVE_FORMAT_PCM;
		wfmt.nChannels = vi.channels;
		wfmt.wBitsPerSample = 16;
		wfmt.nSamplesPerSec = vi.rate;
		wfmt.nBlockAlign = wfmt.nChannels * (wfmt.wBitsPerSample/8);
		wfmt.nAvgBytesPerSec = wfmt.nSamplesPerSec * wfmt.nBlockAlign;
		wfmt.cbSize = 0;
		bWaveGet = TRUE;
		decStep = 1;
		d1loop=0;
		return (OGG_DEC_CONTINUE);
	}
	if(1==decStep){
		while(d1loop<2){
			while(d1loop<2){
				int result=ogg_sync_pageout(&oy,&og);
				if(result==0)break;
				if(result==1){
					ogg_stream_pagein(&os,&og);
					while(d1loop<2){
						result=ogg_stream_packetout(&os,&op);
						if(result==0)break;
						if(result<0){
							return(OGG_DEC_ERR);
						}
						vorbis_synthesis_headerin(&vi,&vc,&op);
						d1loop++;
					}
				}
			}
			buffer=ogg_sync_buffer(&oy,4096);
			memcpy(buffer,inmemory,inmemsize);
			if(inmemsize==0 && d1loop<2){
				return(OGG_DEC_ERR);
			}
			ogg_sync_wrote(&oy,inmemsize);
			if(d1loop<2)
				return (OGG_DEC_CONTINUE);
		}
		decStep = 2;
		convsize=4096/vi.channels;
		vorbis_synthesis_init(&vd,&vi);
		vorbis_block_init(&vd,&vb);
		return (OGG_DEC_CONTINUE);
	}
	while(!eos){
	  while(!eos){
		int result=ogg_sync_pageout(&oy,&og);
		if(result==0)break;
		if(result<0){
		}else{
			ogg_stream_pagein(&os,&og);
			while(1){
				result=ogg_stream_packetout(&os,&op);
				if(result==0)break;
				if(result<0){
				}else{
					float **pcm;
					int samples;
		  
					if(vorbis_synthesis(&vb,&op)==0)
						vorbis_synthesis_blockin(&vd,&vb);
					while((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0){
						int j;
						int clipflag=0;
						int bout=(samples<convsize?samples:convsize);
		
						for(i=0;i<vi.channels;i++){
							ogg_int16_t *ptr=convbuffer+i;
							float  *mono=pcm[i];
							for(j=0;j<bout;j++){
#if 1
								int val=int(mono[j]*32767.f);
#else
								int val=mono[j]*32767.f+drand48()-0.5f;
#endif
								if(val>32767){
								  val=32767;
								  clipflag=1;
								}
								if(val<-32768){
								  val=-32768;
								  clipflag=1;
								}
								*ptr=val;
								ptr+=(vi.channels);
							}
						}
						if(NULL==leak_mem){
							if(outmemsize >= (*done +2*vi.channels*bout)){
								memcpy(outmemory+(*done),convbuffer,2*vi.channels*bout);
								*done += 2*vi.channels*bout;
							}else{
								allocMemSize = 0x100000;
								leak_mem = (char *)malloc(allocMemSize);
							}
						}
						if(leak_mem){
							if(allocMemSize < (leakSize + 2*vi.channels*bout)){
								allocMemSize += 0x100000;
								realloc(leak_mem,allocMemSize);
							}
							memcpy(leak_mem+leakSize,convbuffer,2*vi.channels*bout);
							leakSize += 2*vi.channels*bout;
						}
						vorbis_synthesis_read(&vd,bout);
					}	    
				}
			}
			if(ogg_page_eos(&og))eos=1;
		}
	  }
	  if(!eos){
		buffer=ogg_sync_buffer(&oy,4096);
		memcpy(buffer,inmemory,inmemsize);
		ogg_sync_wrote(&oy,inmemsize);
		if(inmemsize==0) eos=1;
		return (OGG_DEC_CONTINUE);
	  }
	}
	if(bLoop){
		eos = 0;
		return OGG_DEC_REPLAY;
	}
	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
	decStep = 0;

	return (OGG_DEC_CONTINUE);
}
