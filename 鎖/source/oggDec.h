#ifndef _OGGDEC

#include <windows.h>
#include "codec.h"

#define OGG_DEC_ERR			-1		// エラー終了
#define OGG_DEC_END			0		// データ展開終了
#define OGG_DEC_CONTINUE	1		// データ展開中
#define OGG_DEC_REPLAY		2		// リプレイ開始

class OggDec{
	ogg_int16_t			convbuffer[4096]; /* take 8k out of the data segment, not the stack */
	int					convsize;
	int					eos;
	ogg_sync_state		oy; /* sync and verify incoming physical bitstream */
	ogg_stream_state	os; /* take physical pages, weld into a logical stream of packets */
	ogg_page			og; /* one Ogg bitstream page.  Vorbis packets are inside */
	ogg_packet			op; /* one raw packet of data for decode */
	vorbis_info			vi; /* struct that stores all the static vorbis bitstream settings */
	vorbis_comment		vc; /* struct that stores all the bitstream user comments */
	vorbis_dsp_state	vd; /* central working state for the packet->PCM decoder */
	vorbis_block		vb; /* local working space for packet->PCM decode */
	// ymz plus
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
	void Start_ogg(void);	// ライブラリ使用前にコール
	void Clear_ogg(void);	// ファイル再生を終了したらコール
	WORD GetVer(void);		// ライブラリのバージョンを返す
	BOOL GetWaveformat(WAVEFORMATEX *wfx,char *buf); // 指定バッファの展開後のWAVE形式のフォーマットを取得します。
	int  DecodeOGG(char *inmemory,int inmemsize,
					char *outmemory,int outmemsize,int *done);
	void SetLoop(BOOL set){bLoop = set;}
};

#define _OGGDEC
#endif  _OGGDEC
