#include <math.h>
#include "ClCommon.h"
#include "soundDS.h"


#define WAVE_FORMAT_OGG		26447
int			bgmHandle	= -1;
int			voiceHandle	= -1;
int			evtHandle[hEvtSndMax] = {-1,-1,-1,-1};

struct RiffHead{
	char	riff[4];
	int		size;
	char	type[4];
};

struct FmtHead{
	char	fmt[4];
	int		size;
};

ClSoundDS	*lpSoundDS = NULL;
LPDIRECTSOUNDBUFFER		lpTmpBuffer;

DWORD WINAPI SoundLoop(LPVOID param);
DWORD WINAPI fo_exec(LPVOID param);
DWORD WINAPI StreamLoop(LPVOID param);

ClSoundDS::ClSoundDS(HWND hWnd,ClReadFile *readFile,BOOL b3DSound)
{
	HRESULT			hr;
	DSBUFFERDESC	dsbdDesc;
	WAVEFORMATEX	wfx;

	ZeroMemory(this,sizeof(ClSoundDS));
	lpReadFile = readFile;
	hr = DirectSoundCreate8(NULL,&lpDSound,NULL);
	if(hr!=DS_OK)return;
	hr = lpDSound->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE);
	if(hr!=DS_OK)return;
	ZeroMemory(&dsbdDesc,sizeof(DSBUFFERDESC));
	dsbdDesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbdDesc.dwFlags |= DSBCAPS_LOCSOFTWARE;
	if(b3DSound) dsbdDesc.dwFlags |= DSBCAPS_CTRL3D;
	dsbdDesc.lpwfxFormat = NULL;
	hr = lpDSound->CreateSoundBuffer(&dsbdDesc, &lpPrimaryBuffer, NULL);
	if(DS_OK!=hr) return;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample/8);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize = 0;
	lpPrimaryBuffer->SetFormat(&wfx);
	lpDSound->SetCooperativeLevel(hWnd,DSSCL_NORMAL);
	CreateDummyBuffer();
	Enable = TRUE;
} // ClSoundDS::ClSoundDS

ClResult ClSoundDS::CreateSoundObj(int &handle,int arcFileNum,int playNum,BOOL bStream,BOOL b3DSound)
{
	if(!Enable)return NULL;

	char	*fileBuf;
	BOOL	ret;

	if(handle>=0) ReleaseSoundObj(handle);
	handle = SearchObjTable();
	if(-1 == handle)return Cl_PCM_NOTFOUND;

	if(bStream){
		ret = CreateStreamBuffer(handle,arcFileNum,playNum);
		if(-1==ret)bStream = FALSE;
	}
	if(FALSE==bStream){
		lpReadFile->ReadPackFileNum(arcFileNum,playNum,&fileBuf);
		ret = AddSoundBuffer(handle,fileBuf,b3DSound);
		cl_free(fileBuf);
	}
	if(FALSE==ret)return Cl_PCM_NOTFOUND;
	SetObjTable(handle,TRUE);
	return Cl_OK;
} // ClSoundDS::CreateSoundObj

void ClSoundDS::ReleaseSoundObj(int handle)
{
	ReleaseSoundBuffer(handle);
	SetObjTable(handle,FALSE);
} // ClSoundDS::ReleaseSoundObj

void ClSoundDS::ReleaseSoundBuffer(int handle)
{
	ClSoundBuffer	*lpSoundBuffer;

	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer)return;
	if(lpSoundBuffer==lpDSBufferTop){
		lpDSBufferTop = lpSoundBuffer->lpNext;
	}else{
		lpSoundBuffer->lpPrev->lpNext = lpSoundBuffer->lpNext;
		if(lpSoundBuffer->lpNext)lpSoundBuffer->lpNext->lpPrev = lpSoundBuffer->lpPrev;
	}
	if(lpSoundBuffer==lpDSBufferTail){
		lpDSBufferTail = lpSoundBuffer->lpPrev;
	}
	lpSoundBuffer->lpNext = NULL;
	delete lpSoundBuffer;
} // ClSoundDS::ReleaseSoundBuffer

ClResult ClSoundDS::ChangeMusic(int &handle,int arcFileNum,int fnum,BOOL bFade,int repeat)
{
	BOOL			ret;
	ClSoundBuffer	*lpSoundBuffer;
	int				num;

	char	fn[32];
	wsprintf(fn,"BGM_%03d.ogg",fnum);
	num = readFile->SearchFile(pack_bgmfile,fn);
	if(num==(-1)){
		wsprintf(fn,"BGM_%03d_a.ogg",fnum);
		num = readFile->SearchFile(pack_bgmfile,fn);
	}
	if(num==(-1))return Cl_PCM_NOTFOUND;
	saveInf.musicNum = fnum;

	lpSoundBuffer = SelectSoundBuffer(handle);
	if(lpSoundBuffer){
		int status = GetStatus(handle);
		if(status != PCM_STOP){
			if(lpSoundBuffer->orgMusicNum==num) return Cl_OK;
			if(FALSE==bFade){
				Stop(handle);
			}else{
				lpSoundBuffer->nextArcFileNum = arcFileNum;
				lpSoundBuffer->nextMusicNum = num;
				lpSoundBuffer->nextRepeat = repeat;
				if(status!=PCM_FADEOUT)FadeOut(handle);
				return Cl_OK;
			}
		}
		ret = CreateStreamBuffer(handle,arcFileNum,num);
		if(-1==ret)return Cl_PCM_NOTFOUND;
	}else{
		CreateSoundObj(handle,arcFileNum,num,TRUE);
		if(-1==handle)return Cl_PCM_NOTFOUND;
	}
	int volume = sysInf.bgmVolume*0xff /10;
	SetVolume(handle,volume);

	play(handle,repeat);
	return Cl_OK;
} // ClSoundDS::ChangeMusic

ClResult ClSoundDS::playVoice(int &handle,int arcFileNum,char *fname,BOOL bLoop)
{
	char	workBuf[64];
	BOOL			ret;
	ClSoundBuffer	*lpSoundBuffer;
	
	wsprintf(workBuf,"%s.ogg",fname);
	int num = readFile->SearchFile(arcFileNum,workBuf);
	if(num==-1){
		myOutputDebugString("Voiceﾌｧｲﾙ『%s』が見つかりませんでした。\n",workBuf);
		return Cl_PCM_NOTFOUND;
	}
	if(0==sysInf.voiceVolume)return Cl_OK;
	if(keyState.data_skip || sysInf.bAlreadyRead)return Cl_OK;
	lpSoundBuffer = SelectSoundBuffer(handle);
	if(lpSoundBuffer){
		int status = GetStatus(handle);
		if(status != PCM_STOP){
			Stop(handle);
		}
		ret = CreateStreamBuffer(handle,arcFileNum,num);
		if(-1==ret)return Cl_PCM_NOTFOUND;
	}else{
		CreateSoundObj(handle,arcFileNum,num,TRUE);
		if(-1==handle)return Cl_PCM_NOTFOUND;
	}
	int volume = sysInf.voiceVolume*0xff /10;
	SetVolume(handle,volume);
	if(FALSE==bLoop){
		play(handle,1);
	}else{
		play(handle,0);
	}
	return Cl_OK;
} // ClSoundDS::playVoice

ClResult ClSoundDS::play(int handle,int repeat)
{
	ClSoundBuffer	*lpSoundBuffer;
	HRESULT			hr;

	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer)return Cl_PCM_NOTFOUND;
	lpSoundBuffer->endPt = -1;
	if(PCM_STOP!=lpSoundBuffer->status) Stop(handle);
	lpSoundBuffer->repeat = repeat;
	lpSoundBuffer->nextMusicNum = -1;
	if(memory_sound==lpSoundBuffer->bufType){
		switch(repeat){
		  case (1):
			lpSoundBuffer->lpDSBuffer->Play(0,0,0);
			break;
		  case (0):
			lpSoundBuffer->lpDSBuffer->Play(0,0,DSBPLAY_LOOPING);
			break;
		  default:
			{
				DWORD	dwThreadID;
				DSBPOSITIONNOTIFY  pn;
				lpSoundBuffer->lpDSBuffer->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&lpSoundBuffer->lpDSNotify);
				lpSoundBuffer->hEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
				pn.dwOffset = DSBPN_OFFSETSTOP;
				pn.hEventNotify = lpSoundBuffer->hEvent[0];
				lpSoundBuffer->lpDSNotify->SetNotificationPositions(1,&pn);
				lpSoundBuffer->hLoopThread = CreateThread(NULL,0,SoundLoop,lpSoundBuffer,0,&dwThreadID);
				lpSoundBuffer->lpDSBuffer->Play(0,0,0);
			}
			break;
		}
	}else{
		lpReadFile->StreamSeekFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpSoundBuffer->dataTopOffset,FILE_BEGIN);
		char		*lpBlockAdd1, *lpBlockAdd2;
		DWORD		blockSize1, blockSize2;

		hr = lpSoundBuffer->lpDSBuffer->Lock(0,lpSoundBuffer->streamBlockSize, (LPVOID*)&lpBlockAdd1, &blockSize1,(LPVOID*)&lpBlockAdd2, &blockSize2, 0);
		if(DS_OK==hr){
			if(WAVE_FORMAT_OGG==lpSoundBuffer->wFormatTag){
				char workBuf[4096];
				char outBuf[0x20000];
				int	 rsize,outsize,ret;
				rsize = lpReadFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,workBuf,4096);
				ret = lpSoundBuffer->o_dat->DecodeOGG(workBuf,rsize,outBuf,0x20000,&outsize);

				int remainSize = blockSize1;
				int writeSize = 0;

				while(remainSize>=outsize && remainSize>0){
					CopyMemory(lpBlockAdd1+writeSize,outBuf,outsize);
					remainSize -= outsize;
					writeSize += outsize;
					if(ret==OGG_DEC_END && lpSoundBuffer->nextStream>=0){
						int size;
						lpSoundBuffer->readFile->StreamCloseFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum);
						lpSoundBuffer->streamNum = lpSoundBuffer->readFile->StreamOpenFileNum(lpSoundBuffer->arcFileNum,lpSoundBuffer->nextStream,size);
						lpSoundBuffer->musicNum = lpSoundBuffer->nextStream;
						lpSoundBuffer->nextStream = -1;
						ret = OGG_DEC_CONTINUE;
					}
					if(ret!=OGG_DEC_CONTINUE){
						ZeroMemory(lpBlockAdd1+writeSize,remainSize);
						remainSize = 0;
						break;
					}
					if(remainSize > 0){
						rsize = lpReadFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,workBuf,4096);
						ret = lpSoundBuffer->o_dat->DecodeOGG(workBuf,rsize,outBuf,0x20000,&outsize);
					}
				}
				if(remainSize){
					CopyMemory(lpBlockAdd1+writeSize,outBuf,remainSize);
					lpSoundBuffer->leakSize = outsize -remainSize;
					lpSoundBuffer->leakBuff = new BYTE[lpSoundBuffer->leakSize];
					CopyMemory(lpSoundBuffer->leakBuff,outBuf +remainSize,lpSoundBuffer->leakSize);
				}
			}else if(NULL==lpSoundBuffer->hAcm){
				lpReadFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpBlockAdd1,blockSize1);
				if(lpSoundBuffer->streamBlockSize>blockSize1){
					lpReadFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpBlockAdd2,blockSize2);
				}
			}else{
				lpReadFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,(char *)lpSoundBuffer->acmSrc.lpStream,lpSoundBuffer->acmSrc.dwStreamSize);
				MMRESULT nError = acmStreamConvert(lpSoundBuffer->hAcm,&lpSoundBuffer->ash, ACM_STREAMCONVERTF_BLOCKALIGN);
				if(0==nError){
					int remainSize = blockSize1;
					int writeSize = 0;

					while(remainSize>=lpSoundBuffer->ash.cbDstLengthUsed && remainSize>0){
						CopyMemory(lpBlockAdd1+writeSize,lpSoundBuffer->acmDst.lpStream,lpSoundBuffer->ash.cbDstLengthUsed);
						remainSize -= lpSoundBuffer->ash.cbDstLengthUsed;
						writeSize += lpSoundBuffer->ash.cbDstLengthUsed;
						if(remainSize > 0){
							lpReadFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,(char *)lpSoundBuffer->acmSrc.lpStream,lpSoundBuffer->acmSrc.dwStreamSize);
							nError = acmStreamConvert(lpSoundBuffer->hAcm,&lpSoundBuffer->ash, ACM_STREAMCONVERTF_BLOCKALIGN);
						}
					}
					if(remainSize){
						CopyMemory(lpBlockAdd1+writeSize,lpSoundBuffer->acmDst.lpStream,remainSize);
						lpSoundBuffer->leakSize = lpSoundBuffer->ash.cbDstLengthUsed -remainSize;
						lpSoundBuffer->leakBuff = new BYTE[lpSoundBuffer->leakSize];
						CopyMemory(lpSoundBuffer->leakBuff,lpSoundBuffer->acmDst.lpStream +remainSize,lpSoundBuffer->leakSize);
					}
				}
			}
			lpSoundBuffer->lpDSBuffer->Unlock(lpBlockAdd1, blockSize1, lpBlockAdd2, blockSize2);
		}
		DWORD	dwThreadID;
		DSBPOSITIONNOTIFY  pn[3];
		lpSoundBuffer->lpDSBuffer->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&lpSoundBuffer->lpDSNotify);
		lpSoundBuffer->hEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
		lpSoundBuffer->hEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
		lpSoundBuffer->hEvent[2] = CreateEvent(NULL, FALSE, FALSE, NULL);
		pn[0].dwOffset = DWORD(lpSoundBuffer->streamBlockSize*0.1f);
		pn[0].hEventNotify = lpSoundBuffer->hEvent[0];
		pn[1].dwOffset = DWORD(lpSoundBuffer->streamBlockSize*1.1f);
		pn[1].hEventNotify = lpSoundBuffer->hEvent[1];
		pn[2].dwOffset = DSBPN_OFFSETSTOP;
		pn[2].hEventNotify = lpSoundBuffer->hEvent[2];
		lpSoundBuffer->lpDSNotify->SetNotificationPositions(3,pn);
		lpSoundBuffer->hLoopThread = CreateThread(NULL,0,StreamLoop,lpSoundBuffer,0,&dwThreadID);
		lpSoundBuffer->lpDSBuffer->Play(0,0,DSBPLAY_LOOPING);
	}
	lpSoundBuffer->status = PCM_PLAY;
	return Cl_OK;
} // ClSoundDS::play

void ClSoundDS::NextMusicStart()
{
	ClSoundBuffer	*lpSoundBuffer;
	NextMusicHandle			*lpNext = lpNextMusic;

	while(lpNext){
		lpSoundBuffer = SelectSoundBuffer(lpNext->handle);
		if(lpSoundBuffer && lpSoundBuffer->nextMusicNum>=0){
			int handle = lpSoundBuffer->handle;
			int nextRepeat = lpSoundBuffer->nextRepeat;
			BOOL ret = lpSoundDS->CreateStreamBuffer(lpSoundBuffer->handle,lpSoundBuffer->nextArcFileNum,lpSoundBuffer->nextMusicNum);
			int volume = sysInf.bgmVolume*0xff /10;
			SetVolume(handle,volume);
			if(TRUE==ret) lpSoundDS->play(handle,nextRepeat);
		}
		lpNext = lpNext->next;
	}
	my_delete(lpNextMusic);
} // ClSoundDS::NextMusicStart

ClResult ClSoundDS::FadeOut(int handle)
{
	ClSoundBuffer	*lpSoundBuffer;
	DWORD			dwThreadID;

	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer || lpSoundBuffer->status==PCM_STOP) return Cl_OK;
	if(lpSoundBuffer->status==PCM_PLAY){
		lpSoundBuffer->repeat = 0;
		lpSoundBuffer->status = PCM_FADEOUT;
		lpSoundBuffer->hFadeThread = CreateThread(NULL,0,fo_exec,lpSoundBuffer,0,&dwThreadID);
		SetThreadPriority(lpSoundBuffer->hFadeThread, THREAD_PRIORITY_ABOVE_NORMAL);
	}
	return Cl_PLAY_CONTINUE;
} // ClSoundDS::FadeOut

void ClSoundDS::Stop(int handle)
{
	ClSoundBuffer	*lpSoundBuffer;

	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer || PCM_STOP==lpSoundBuffer->status)return;
	if(PCM_FADEOUT==lpSoundBuffer->status){
		lpSoundBuffer->status = PCM_FADEOUTBREAK;
		while(PCM_STOP!=lpSoundBuffer->status)Sleep(1);
	}else{
		lpSoundBuffer->lpDSBuffer->Stop();
		lpSoundBuffer->lpDSBuffer->SetCurrentPosition(0);
		lpSoundBuffer->status = PCM_STOP;
	}
	while(lpSoundBuffer->hEvent[0])
		Sleep(1);
} // ClSoundDS::Stop

int ClSoundDS::GetStatus(int handle)
{
	ClSoundBuffer	*lpSoundBuffer;
	DWORD	status;

	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer || PCM_STOP==lpSoundBuffer->status){
		return PCM_STOP;
	}
	lpSoundBuffer->lpDSBuffer->GetStatus(&status);
	if(!(status & DSBSTATUS_PLAYING))lpSoundBuffer->status = PCM_STOP;
	return lpSoundBuffer->status;
} // ClSoundDS::GetStatus

void ClSoundDS::SetVolume(int handle,int value)
{
	ClSoundBuffer	*lpSoundBuffer;
	LONG			db = DSBVOLUME_MIN;

	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer) return;
	if(value > 0){
		db = LONG(log10(value/255.0f) *20*100);
	}
	lpSoundBuffer->lpDSBuffer->SetVolume(db);
} // ClSoundDS::SetVolume

int ClSoundDS::SearchObjTable()
{
	for(int i=0;i<32;i++){
		if(0xffffffff!=objTable[i]){
			for(int j=0;j<32;j++){
				if(0==((1<<j) & objTable[i]) )return (i*32+j);
			}
		}
	}
	return -1;
} // ClSoundDS::SearchObjTable

void ClSoundDS::SetObjTable(int handle,BOOL bSet)
{
	if(handle >= 1024)return;
	DWORD	dw = 1;
	dw = dw << (handle%32);
	if(bSet){
		objTable[handle/32] |= dw;
	}else{
		dw = ~dw;
		objTable[handle/32] &= dw;
	}
} // ClSoundDS::SetObjTable

int ClSoundDS::GetMusicNo(int handle)
{
	ClSoundBuffer	*lpSoundBuffer;

	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer) return -1;
	return lpSoundBuffer->orgMusicNum;
} // ClSoundDS::GetMusicNo

BOOL ClSoundDS::CreateStreamBuffer(int handle,int arcFileNum,int playNum)
{
	int				size,stream,dataTopOffset,nextStream = -1;
	HRESULT			hr;
	RiffHead		riffHead;
	FmtHead			fmtHead;
	PCMWAVEFORMAT	pcmFormat;
	BOOL			bOGG = FALSE;

	ReleaseSoundBuffer(handle);
	stream = lpReadFile->StreamOpenFileNum(arcFileNum,playNum,size);
	if(pack_bgmfile!=arcFileNum && pack_voice!=arcFileNum){
		lpReadFile->StreamReadFile(arcFileNum,stream,(char *)&riffHead,sizeof(RiffHead));
		if(strncmp(riffHead.riff,"RIFF",4) || strncmp(riffHead.type,"WAVE",4)){
			lpReadFile->StreamCloseFile(arcFileNum,stream);
			return FALSE;
		}
		lpReadFile->StreamReadFile(arcFileNum,stream,(char *)&fmtHead,sizeof(FmtHead));
		lpReadFile->StreamReadFile(arcFileNum,stream,(char *)&pcmFormat,sizeof(PCMWAVEFORMAT));
	}else{
		char chkFmt[4];
		bOGG = TRUE;
		char *fname = lpReadFile->GetFileName(arcFileNum,playNum);
		lpReadFile->StreamReadFile(arcFileNum,stream,(char *)&chkFmt,4);
		lpReadFile->StreamSeekFile(arcFileNum,stream,0,FILE_BEGIN);
		if(0!=strncmp(chkFmt,"OggS",4))return FALSE;
		dataTopOffset = 0;
		if(NULL==lpDSBufferTop){
			lpDSBufferTop = lpDSBufferTail = new ClSoundBuffer;
		}else{
			lpDSBufferTail->lpNext = new ClSoundBuffer;
			lpDSBufferTail->lpNext->lpPrev = lpDSBufferTail;
			lpDSBufferTail = lpDSBufferTail->lpNext;
		}
		lpDSBufferTail->dataTopOffset = dataTopOffset;
		lpDSBufferTail->o_dat = new OggDec;
		if(_strnicmp(&fname[strlen(fname)-6],"_A.",3) == 0){
			char workBuf[32];
			strcpy(workBuf,fname);
			workBuf[strlen(fname)-5] = 'B';
			nextStream = lpReadFile->SearchFile(arcFileNum,workBuf);
		}
		WAVEFORMATEX	dstWF;
		char			workBuf[4096];
		lpReadFile->StreamReadFile(arcFileNum,stream,workBuf,4096);
		lpDSBufferTail->o_dat->GetWaveformat(&dstWF,workBuf);

		DSBUFFERDESC	dsbdesc;
		ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS;
		dsbdesc.dwBufferBytes = dstWF.nAvgBytesPerSec *2;
		dsbdesc.lpwfxFormat = &dstWF;
		hr = lpDSound->CreateSoundBuffer(&dsbdesc, &lpTmpBuffer, NULL);
		lpTmpBuffer->QueryInterface(IID_IDirectSoundBuffer8,(LPVOID *)&lpDSBufferTail->lpDSBuffer);
		RELEASE(lpTmpBuffer);
		lpDSBufferTail->streamBlockSize = dstWF.nAvgBytesPerSec;
	}
	if(bOGG){
		pcmFormat.wf.wFormatTag = WAVE_FORMAT_OGG;
	}else if(pcmFormat.wf.wFormatTag != WAVE_FORMAT_PCM){
		WAVEFORMATEX	dstWF;
		WAVEFORMATEX	*pwfxInfo;
		WORD			cbExtraAlloc;
		MMRESULT		mmResult;
		lpReadFile->StreamReadFile(arcFileNum,stream,(char *)&cbExtraAlloc,sizeof(WORD));
		pwfxInfo = (WAVEFORMATEX *)cl_malloc(sizeof(WAVEFORMATEX) +cbExtraAlloc);
		CopyMemory(pwfxInfo,&pcmFormat,sizeof(PCMWAVEFORMAT));
		pwfxInfo->cbSize = cbExtraAlloc;
		lpReadFile->StreamReadFile(arcFileNum,stream,(char *)pwfxInfo +sizeof(WAVEFORMATEX),cbExtraAlloc);
		lpReadFile->StreamSeekFile(arcFileNum,stream,sizeof(RiffHead),FILE_BEGIN);
		lpReadFile->StreamReadFile(arcFileNum,stream,(char *)&fmtHead,sizeof(FmtHead));
		dataTopOffset = sizeof(RiffHead) +sizeof(FmtHead);
		while(0!=strncmp(fmtHead.fmt,"data",4)){
			lpReadFile->StreamSeekFile(arcFileNum,stream,fmtHead.size,FILE_CURRENT);
			dataTopOffset += fmtHead.size;
			lpReadFile->StreamReadFile(arcFileNum,stream,(char *)&fmtHead,sizeof(FmtHead));
			dataTopOffset += sizeof(FmtHead);
		}
		ZeroMemory(&dstWF,sizeof(dstWF));
		dstWF.wFormatTag = WAVE_FORMAT_PCM;
		mmResult = acmFormatSuggest(
			NULL,
			pwfxInfo,
			&dstWF,
			sizeof(dstWF),
			ACM_FORMATSUGGESTF_WFORMATTAG);
		if(mmResult != 0){
			myOutputDebugString("オーディオ圧縮が無効です\n");
			return FALSE;
		}
		if(NULL==lpDSBufferTop){
			lpDSBufferTop = lpDSBufferTail = new ClSoundBuffer;
		}else{
			lpDSBufferTail->lpNext = new ClSoundBuffer;
			lpDSBufferTail->lpNext->lpPrev = lpDSBufferTail;
			lpDSBufferTail = lpDSBufferTail->lpNext;
		}
		lpDSBufferTail->dataTopOffset = dataTopOffset;
		acmStreamOpen(&lpDSBufferTail->hAcm, NULL,pwfxInfo,&dstWF, NULL, 0L, 0L, ACM_STREAMOPENF_NONREALTIME);
		size = pwfxInfo->nAvgBytesPerSec;
		if(size % pwfxInfo->nBlockAlign){
			size = (size/pwfxInfo->nBlockAlign +1)*pwfxInfo->nBlockAlign;
		}
		acmStreamSize(lpDSBufferTail->hAcm,size, &lpDSBufferTail->acmDst.dwStreamSize, ACM_STREAMSIZEF_SOURCE);
		acmStreamSize(lpDSBufferTail->hAcm, lpDSBufferTail->acmDst.dwStreamSize, &lpDSBufferTail->acmSrc.dwStreamSize, ACM_STREAMSIZEF_DESTINATION);
		acmStreamSize(lpDSBufferTail->hAcm, lpDSBufferTail->acmSrc.dwStreamSize, &lpDSBufferTail->acmDst.dwStreamSize, ACM_STREAMSIZEF_SOURCE);
		cl_free(pwfxInfo);
		if(fmtHead.size < lpDSBufferTail->acmSrc.dwStreamSize*2){
			acmStreamClose(lpDSBufferTail->hAcm, 0);
			lpDSBufferTail->hAcm = NULL;
			lpReadFile->StreamCloseFile(arcFileNum,stream);
			if(lpDSBufferTop == lpDSBufferTail){
				delete lpDSBufferTail;
				lpDSBufferTop = lpDSBufferTail = NULL;
			}else{
				lpDSBufferTail = lpDSBufferTail->lpPrev;
				delete(lpDSBufferTail->lpNext);
				lpDSBufferTail->lpNext = NULL;
			}
			return -1;
		}
		lpDSBufferTail->acmSrc.lpStream = (LPBYTE)cl_malloc(lpDSBufferTail->acmSrc.dwStreamSize);
		lpDSBufferTail->acmDst.lpStream = (LPBYTE)cl_malloc(lpDSBufferTail->acmDst.dwStreamSize);
		ZeroMemory(&lpDSBufferTail->ash,sizeof(lpDSBufferTail->ash));
		lpDSBufferTail->ash.cbStruct		= sizeof(lpDSBufferTail->ash);
		lpDSBufferTail->ash.pbSrc			= lpDSBufferTail->acmSrc.lpStream;
		lpDSBufferTail->ash.cbSrcLength		= lpDSBufferTail->acmSrc.dwStreamSize;
		lpDSBufferTail->ash.dwSrcUser		= lpDSBufferTail->acmSrc.dwStreamSize;
		lpDSBufferTail->ash.pbDst			= lpDSBufferTail->acmDst.lpStream;
		lpDSBufferTail->ash.cbDstLength		= lpDSBufferTail->acmDst.dwStreamSize;
		lpDSBufferTail->ash.dwDstUser		= lpDSBufferTail->acmDst.dwStreamSize;
		acmStreamPrepareHeader(lpDSBufferTail->hAcm,&lpDSBufferTail->ash,0);
		DSBUFFERDESC	dsbdesc;
		ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS;
		dsbdesc.dwBufferBytes = lpDSBufferTail->acmDst.dwStreamSize *2;
		dsbdesc.lpwfxFormat = &dstWF;
		hr = lpDSound->CreateSoundBuffer(&dsbdesc, &lpTmpBuffer, NULL);
		lpTmpBuffer->QueryInterface(IID_IDirectSoundBuffer8,(LPVOID *)&lpDSBufferTail->lpDSBuffer);
		RELEASE(lpTmpBuffer);
		lpDSBufferTail->streamBlockSize = lpDSBufferTail->acmDst.dwStreamSize;
	}else{
		dataTopOffset = sizeof(RiffHead) +sizeof(FmtHead) +fmtHead.size;
		lpReadFile->StreamSeekFile(arcFileNum,stream,fmtHead.size -sizeof(PCMWAVEFORMAT),FILE_CURRENT);
		lpReadFile->StreamReadFile(arcFileNum,stream,(char *)&fmtHead,sizeof(FmtHead));
		dataTopOffset += sizeof(FmtHead);
		while(0!=strncmp(fmtHead.fmt,"data",4)){
			lpReadFile->StreamSeekFile(arcFileNum,stream,fmtHead.size,FILE_CURRENT);
			dataTopOffset += fmtHead.size;
			lpReadFile->StreamReadFile(arcFileNum,stream,(char *)&fmtHead,sizeof(FmtHead));
			dataTopOffset += sizeof(FmtHead);
		}
		if(fmtHead.size < pcmFormat.wf.nAvgBytesPerSec*2){
			lpReadFile->StreamCloseFile(arcFileNum,stream);
			return -1;
		}
		if(NULL==lpDSBufferTop){
			lpDSBufferTop = lpDSBufferTail = new ClSoundBuffer;
		}else{
			lpDSBufferTail->lpNext = new ClSoundBuffer;
			lpDSBufferTail->lpNext->lpPrev = lpDSBufferTail;
			lpDSBufferTail = lpDSBufferTail->lpNext;
		}
		lpDSBufferTail->dataTopOffset = dataTopOffset;
		DSBUFFERDESC	dsbdesc;
		WAVEFORMATEX	audioFmt;
		ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS;
		CopyMemory(&audioFmt,&pcmFormat,sizeof(WAVEFORMAT));
		audioFmt.wBitsPerSample = pcmFormat.wBitsPerSample;
		audioFmt.cbSize = 0;
		dsbdesc.dwBufferBytes = audioFmt.nAvgBytesPerSec *2;
		dsbdesc.lpwfxFormat = &audioFmt;
		hr = lpDSound->CreateSoundBuffer(&dsbdesc, &lpTmpBuffer, NULL);
		lpTmpBuffer->QueryInterface(IID_IDirectSoundBuffer8,(LPVOID *)&lpDSBufferTail->lpDSBuffer);
		RELEASE(lpTmpBuffer);
		lpDSBufferTail->streamBlockSize = audioFmt.nAvgBytesPerSec;
	}
	lpDSBufferTail->arcFileNum = arcFileNum;
	lpDSBufferTail->musicNum = lpDSBufferTail->orgMusicNum = playNum;
	lpDSBufferTail->streamNum = stream;
	lpDSBufferTail->nextStream = nextStream;
	lpDSBufferTail->handle = handle;
	lpDSBufferTail->bufType = stream_sound;
	lpDSBufferTail->readFile = lpReadFile;
	lpDSBufferTail->wFormatTag = pcmFormat.wf.wFormatTag;
	return TRUE;
} // ClSoundDS::CreateStreamBuffer

BOOL ClSoundDS::CreateDummyBuffer()
{
	WAVEFORMATEX	dstWF;
	LPBYTE			lpBlockAdd1, lpBlockAdd2;
	DWORD			blockSize1, blockSize2;

	dstWF.wFormatTag = WAVE_FORMAT_PCM;
	dstWF.nChannels = 1;
	dstWF.nSamplesPerSec = 44100;
	dstWF.nBlockAlign = 2;
	dstWF.nAvgBytesPerSec = 88200;
	dstWF.wBitsPerSample = 16;
	dstWF.cbSize = 0;
	DSBUFFERDESC	dsbdesc;
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS;
	dsbdesc.dwBufferBytes = dstWF.nAvgBytesPerSec /2;
	dsbdesc.lpwfxFormat = &dstWF;
	HRESULT hr = lpDSound->CreateSoundBuffer(&dsbdesc, &lpTmpBuffer, NULL);
	if(hr==DS_OK){
		lpTmpBuffer->QueryInterface(IID_IDirectSoundBuffer8,(LPVOID *)&lpDummyBuffer);
		RELEASE(lpTmpBuffer);
		lpDummyBuffer->SetVolume(DSBVOLUME_MIN);
		lpDummyBuffer->Lock(0,dsbdesc.dwBufferBytes, (LPVOID*)&lpBlockAdd1, &blockSize1,(LPVOID*)&lpBlockAdd2, &blockSize2, 0);
		ZeroMemory(lpBlockAdd1,blockSize1);
		if(blockSize2) ZeroMemory(lpBlockAdd2,blockSize2);
		lpDummyBuffer->Unlock(lpBlockAdd1, blockSize1, lpBlockAdd2, blockSize2);
		lpDummyBuffer->Play(0,0,DSBPLAY_LOOPING);
	}
	return TRUE;
} // ClSoundDS::CreateDummyBuffer

BOOL ClSoundDS::AddSoundBuffer(int handle,char *fileBuf,BOOL b3DSound)
{
	HRESULT			hr;
	RiffHead		riffHead;
	FmtHead			fmtHead;
	PCMWAVEFORMAT	pcmFormat;
	LPBYTE			lpBlockAdd1, lpBlockAdd2;
	DWORD			blockSize1, blockSize2;

	CopyMemory(&riffHead,fileBuf,sizeof(RiffHead));
	fileBuf += sizeof(RiffHead);
	if(strncmp(riffHead.riff,"RIFF",4) || strncmp(riffHead.type,"WAVE",4)){
		return FALSE;
	}
	if(NULL==lpDSBufferTop){
		lpDSBufferTop = lpDSBufferTail = new ClSoundBuffer;
	}else{
		lpDSBufferTail->lpNext = new ClSoundBuffer;
		lpDSBufferTail->lpNext->lpPrev = lpDSBufferTail;
		lpDSBufferTail = lpDSBufferTail->lpNext;
	}
	lpDSBufferTail->bufType = memory_sound;
	CopyMemory(&fmtHead,fileBuf,sizeof(FmtHead));
	fileBuf += sizeof(FmtHead);
	CopyMemory(&pcmFormat,fileBuf,sizeof(PCMWAVEFORMAT));
	if(pcmFormat.wf.wFormatTag == WAVE_FORMAT_PCM){
		fileBuf += fmtHead.size;
		CopyMemory(&fmtHead,fileBuf,sizeof(FmtHead));
		fileBuf += sizeof(FmtHead);
		while(0!=strncmp(fmtHead.fmt,"data",4)){
			fileBuf += fmtHead.size;
			CopyMemory(&fmtHead,fileBuf,sizeof(FmtHead));
			fileBuf += sizeof(FmtHead);
		}
		DSBUFFERDESC	dsbdesc;
		WAVEFORMATEX	audioFmt;
		ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS;
		if(b3DSound) dsbdesc.dwFlags |= (DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE);
		dsbdesc.dwBufferBytes = fmtHead.size;
		CopyMemory(&audioFmt,&pcmFormat,sizeof(WAVEFORMAT));
		audioFmt.wBitsPerSample = pcmFormat.wBitsPerSample;
		audioFmt.cbSize = 0;
		dsbdesc.lpwfxFormat = &audioFmt;
		hr = lpDSound->CreateSoundBuffer(&dsbdesc, &lpTmpBuffer, NULL);
		lpTmpBuffer->QueryInterface(IID_IDirectSoundBuffer8,(LPVOID *)&lpDSBufferTail->lpDSBuffer);
		RELEASE(lpTmpBuffer);
		hr = lpDSBufferTail->lpDSBuffer->Lock(0,fmtHead.size, (LPVOID*)&lpBlockAdd1, &blockSize1,(LPVOID*)&lpBlockAdd2, &blockSize2, 0);
		if(DS_OK==hr){
			CopyMemory(lpBlockAdd1,fileBuf,blockSize1);
			if(fmtHead.size>blockSize1){
				CopyMemory(lpBlockAdd2,fileBuf+blockSize1,blockSize2);
			}
			lpDSBufferTail->lpDSBuffer->Unlock(lpBlockAdd1, blockSize1, lpBlockAdd2, blockSize2);
		}
		lpDSBufferTail->handle = handle;
	}else{
		WAVEFORMATEX	dstWF;
		WAVEFORMATEX	*pwfxInfo;
		WORD			cbExtraAlloc;
		MMRESULT		mmResult;
		fileBuf += sizeof(PCMWAVEFORMAT);
		CopyMemory(&cbExtraAlloc,fileBuf,sizeof(WORD));
		fileBuf += sizeof(WORD);
		pwfxInfo = (WAVEFORMATEX *)cl_malloc(sizeof(WAVEFORMATEX) +cbExtraAlloc);
		CopyMemory(pwfxInfo,&pcmFormat,sizeof(PCMWAVEFORMAT));
		pwfxInfo->cbSize = cbExtraAlloc;
		CopyMemory((char *)pwfxInfo +sizeof(WAVEFORMATEX),fileBuf,cbExtraAlloc);
		fileBuf += (fmtHead.size -sizeof(WAVEFORMATEX));
		CopyMemory(&fmtHead,fileBuf,sizeof(FmtHead));
		fileBuf += sizeof(FmtHead);
		while(0!=strncmp(fmtHead.fmt,"data",4)){
			fileBuf += fmtHead.size;
			CopyMemory(&fmtHead,fileBuf,sizeof(FmtHead));
			fileBuf += sizeof(FmtHead);
		}
		ZeroMemory(&dstWF,sizeof(dstWF));
		dstWF.wFormatTag = WAVE_FORMAT_PCM;
		mmResult = acmFormatSuggest(
			NULL,
			pwfxInfo,
			&dstWF,
			sizeof(dstWF),
			ACM_FORMATSUGGESTF_WFORMATTAG);
		if(mmResult != 0){
			myOutputDebugString("オーディオ圧縮が無効です\n");
			return FALSE;
		}
		HACMSTREAM				hAcm = NULL;
		ACMSTREAMHEADER			ash;
		AcmBuffer				acmDst;
		AcmBuffer				acmSrc;

		acmStreamOpen(&hAcm, NULL,pwfxInfo,&dstWF, NULL, 0L, 0L, ACM_STREAMOPENF_NONREALTIME);
		acmSrc.dwStreamSize = fmtHead.size;
		acmStreamSize(hAcm, acmSrc.dwStreamSize, &acmDst.dwStreamSize, ACM_STREAMSIZEF_SOURCE);
		cl_free(pwfxInfo);
		acmSrc.lpStream = (LPBYTE)cl_malloc(acmSrc.dwStreamSize);
		acmDst.lpStream = (LPBYTE)cl_malloc(acmDst.dwStreamSize);
		ZeroMemory(&ash,sizeof(ash));
		ash.cbStruct		= sizeof(ash);
		ash.pbSrc			= acmSrc.lpStream;
		ash.cbSrcLength		= acmSrc.dwStreamSize;
		ash.dwSrcUser		= acmSrc.dwStreamSize;
		ash.pbDst			= acmDst.lpStream;
		ash.cbDstLength		= acmDst.dwStreamSize;
		ash.dwDstUser		= acmDst.dwStreamSize;
		acmStreamPrepareHeader(hAcm,&ash,0);
		CopyMemory(acmSrc.lpStream,fileBuf,acmSrc.dwStreamSize);
		MMRESULT nError = acmStreamConvert(hAcm,&ash, ACM_STREAMCONVERTF_BLOCKALIGN);
		DSBUFFERDESC	dsbdesc;
		ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS;
		if(b3DSound) dsbdesc.dwFlags |= (DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE);
		dsbdesc.dwBufferBytes = ash.cbDstLengthUsed;
		dsbdesc.lpwfxFormat = &dstWF;
		hr = lpDSound->CreateSoundBuffer(&dsbdesc, &lpTmpBuffer, NULL);
		lpTmpBuffer->QueryInterface(IID_IDirectSoundBuffer8,(LPVOID *)&lpDSBufferTail->lpDSBuffer);
		RELEASE(lpTmpBuffer);
		hr = lpDSBufferTail->lpDSBuffer->Lock(0,ash.cbDstLengthUsed, (LPVOID*)&lpBlockAdd1, &blockSize1,(LPVOID*)&lpBlockAdd2, &blockSize2, 0);
		if(DS_OK==hr){
			if(ash.cbDstLengthUsed < blockSize1){
				CopyMemory(lpBlockAdd1, acmDst.lpStream, ash.cbDstLengthUsed);
				ZeroMemory(lpBlockAdd1 +ash.cbDstLengthUsed, blockSize1 -ash.cbDstLengthUsed);
				if(blockSize2)ZeroMemory(lpBlockAdd2,blockSize2);
			}else{
				CopyMemory(lpBlockAdd1, acmDst.lpStream, blockSize1);
				if(blockSize2){
					CopyMemory(lpBlockAdd2, acmDst.lpStream+blockSize1, ash.cbDstLengthUsed -blockSize1);
				}
			}
			lpDSBufferTail->lpDSBuffer->Unlock(lpBlockAdd1, blockSize1, lpBlockAdd2, blockSize2);
		}
		if(hAcm){
			acmStreamUnprepareHeader(hAcm,&ash,0);
			acmStreamClose(hAcm,0);
			cl_free(acmSrc.lpStream);
			cl_free(acmDst.lpStream);
		}
		lpDSBufferTail->handle = handle;
	}
	return TRUE;
} // ClSoundDS::AddSoundBuffer

ClSoundBuffer *ClSoundDS::SelectSoundBuffer(int handle)
{
	ClSoundBuffer	*lpSoundBuffer;

	if(handle<0)return NULL;
	lpSoundBuffer = lpDSBufferTop;
	while(lpSoundBuffer && lpSoundBuffer->handle!=handle){
		lpSoundBuffer = lpSoundBuffer->lpNext;
	}
	return lpSoundBuffer;
} // ClSoundDS::SelectSoundBuffer

void ClSoundDS::CreateD3DSoundListener(D3DVALUE distance)
{
	if(!Enable)return;
	if(DS_OK != lpPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8,(LPVOID *)&lpListener)){
		return;
	}
	lpListener->SetDistanceFactor(distance,DS3D_IMMEDIATE);
	lpListener->SetVelocity(0,0,0,DS3D_IMMEDIATE);
	lpListener->SetRolloffFactor(5.0f,DS3D_IMMEDIATE);
	lpListener->SetPosition(65535,65535,65535,DS3D_IMMEDIATE);
} // ClSoundDS::CreateD3DSoundListener

int ClSoundDS::Create3DBuffer(int effectNum,D3DVALUE x,D3DVALUE y,D3DVALUE z)
{
	ClSoundBuffer	*lpSoundBuffer;
	int				handle;

	if(!Enable)return -1;

	CreateSoundObj(handle,pack_sound,effectNum,FALSE,TRUE);
	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer)return -1;
	lpSoundBuffer->lpDSBuffer->QueryInterface(IID_IDirectSound3DBuffer8,(LPVOID *)&lpSoundBuffer->lpDSB3D);
	lpSoundBuffer->lpDSB3D->SetVelocity(0,0,0,DS3D_IMMEDIATE);
	lpSoundBuffer->lpDSB3D->SetMinDistance(50.0f,DS3D_IMMEDIATE);
	lpSoundBuffer->lpDSB3D->SetMaxDistance(250.0f,DS3D_IMMEDIATE);
	lpSoundBuffer->lpDSB3D->SetMode(DS3DMODE_NORMAL,DS3D_IMMEDIATE);
	lpSoundBuffer->lpDSB3D->SetPosition(x,y,z,DS3D_IMMEDIATE);
	return handle;
} // ClSoundDS::Create3DBuffer

BOOL ClSoundDS::SetPosition(int handle,D3DVALUE x,D3DVALUE y,D3DVALUE z)
{
	ClSoundBuffer	*lpSoundBuffer;

	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer)return FALSE;
	if(NULL==lpSoundBuffer->lpDSB3D)return FALSE;
	lpSoundBuffer->lpDSB3D->SetPosition(x,y,z,DS3D_IMMEDIATE);
	return TRUE;
} // ClSoundDS::SetPosition

void ClSoundDS::Set3DListener(float x,float y,float z,float deg)
{
	if(!Enable)return;
	if(lpSoundDS->lpListener){
		lpSoundDS->lpListener->SetPosition(x,y,z,DS3D_IMMEDIATE);
		lpSoundDS->lpListener->SetOrientation(sinf(DEGtoRAD(deg)),0,cosf(DEGtoRAD(deg)),
			0,1.0f,0,DS3D_IMMEDIATE);
	}
} // ClSoundDS::Set3DListener

BOOL ClSoundDS::Play3D(int handle,BOOL bLoop)
{
	ClSoundBuffer	*lpSoundBuffer;

	if(!Enable)return FALSE;
	lpSoundBuffer = SelectSoundBuffer(handle);
	if(NULL==lpSoundBuffer)return FALSE;
	if(NULL==lpSoundBuffer->lpDSB3D)return FALSE;
	int volume = sysInf.seVolume*0xff /10;
	SetVolume(handle,volume);
	if(bLoop){
		lpSoundBuffer->lpDSBuffer->Play(0,0,DSBPLAY_LOOPING);
	}else{
		lpSoundBuffer->lpDSBuffer->Play(0,0,0);
	}
	return TRUE;
} // ClSoundDS::Play3D

static DWORD WINAPI StreamLoop(LPVOID param)
{
	DWORD			size,pt;
	HRESULT			hr;
	char			*lpBlockAdd1,*lpBlockAdd2;
	DWORD			blockSize1, blockSize2;
	ClSoundBuffer	*lpSoundBuffer = (ClSoundBuffer *)param;
	CRITICAL_SECTION	cr_section;
	InitializeCriticalSection(&cr_section);
	while(TRUE){
		pt = WaitForMultipleObjects(3,lpSoundBuffer->hEvent,FALSE,INFINITE);
		EnterCriticalSection(&cr_section);
		switch(pt){
		  case (WAIT_OBJECT_0):
		  case (WAIT_OBJECT_0+1):
			size = pt -WAIT_OBJECT_0 +1;
			if(size > 1)size = 0;
			hr = lpSoundBuffer->lpDSBuffer->Lock(lpSoundBuffer->streamBlockSize*size,lpSoundBuffer->streamBlockSize, (LPVOID*)&lpBlockAdd1, &blockSize1,(LPVOID*)&lpBlockAdd2, &blockSize2, 0);
			if(DS_OK==hr){
				if(WAVE_FORMAT_OGG==lpSoundBuffer->wFormatTag){
					char	workBuf[4096],outBuf[0x20000];
					int		rsize,outsize,ret;
					int		remainSize = blockSize1;
					int		writeSize = 0;
					if(lpSoundBuffer->leakSize){
						if(lpSoundBuffer->leakSize > blockSize1){
							CopyMemory(lpBlockAdd1,lpSoundBuffer->leakBuff,blockSize1);
							lpSoundBuffer->leakSize -= blockSize1;
							MoveMemory(lpSoundBuffer->leakBuff,lpSoundBuffer->leakBuff+blockSize1,lpSoundBuffer->leakSize);
							remainSize = 0;
							lpSoundBuffer->lpDSBuffer->Unlock(lpBlockAdd1, blockSize1, lpBlockAdd2, blockSize2);
							break;
						}else{
							CopyMemory(lpBlockAdd1,lpSoundBuffer->leakBuff,lpSoundBuffer->leakSize);
							writeSize += lpSoundBuffer->leakSize;
							remainSize -= lpSoundBuffer->leakSize;
							lpSoundBuffer->leakSize = 0;
							my_deletes(lpSoundBuffer->leakBuff);
						}
					}
					if(lpSoundBuffer->endPt>=0){
						ZeroMemory(lpBlockAdd1+writeSize, remainSize);
						ZeroMemory(lpBlockAdd2, blockSize2);
						lpSoundBuffer->lpDSBuffer->Unlock(lpBlockAdd1, blockSize1, lpBlockAdd2, blockSize2);
						if(pt==lpSoundBuffer->endPt){
							lpSoundBuffer->lpDSBuffer->Stop();
							goto SoundStop;
						}
						break;
					}
					rsize = lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,workBuf,4096);
					ret = lpSoundBuffer->o_dat->DecodeOGG(workBuf,rsize,outBuf,0x20000,&outsize);
					if(ret==OGG_DEC_END){
oggFilEnd:
						if(1==lpSoundBuffer->repeat){
							if(remainSize >= outsize){
								CopyMemory(lpBlockAdd1+writeSize, outBuf,outsize);
								writeSize  += outsize;
								remainSize -= outsize;
								ZeroMemory(lpBlockAdd1+writeSize, remainSize);
								if(blockSize2)ZeroMemory(lpBlockAdd2,blockSize2);
							}else{
								CopyMemory(lpBlockAdd1+writeSize, outBuf, remainSize);
							}
							lpSoundBuffer->lpDSBuffer->Unlock(lpBlockAdd1, blockSize1, lpBlockAdd2, blockSize2);
							if((-1)==lpSoundBuffer->endPt){
								lpSoundBuffer->endPt = pt;
							}
							break;
						}else{
							if(lpSoundBuffer->repeat) lpSoundBuffer->repeat --;
							if(lpSoundBuffer->nextStream>=0){
								int size;
								lpSoundBuffer->readFile->StreamCloseFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum);
								lpSoundBuffer->streamNum = lpSoundBuffer->readFile->StreamOpenFileNum(lpSoundBuffer->arcFileNum,lpSoundBuffer->nextStream,size);
								lpSoundBuffer->musicNum = lpSoundBuffer->nextStream;
								lpSoundBuffer->nextStream = -1;
							}
							lpSoundBuffer->readFile->StreamSeekFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpSoundBuffer->dataTopOffset,FILE_BEGIN);
							while(remainSize>=outsize && remainSize>0){
								CopyMemory(lpBlockAdd1+writeSize,outBuf,outsize);
								remainSize -= outsize;
								writeSize += outsize;
								if(remainSize > 0){
									rsize = lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,
										workBuf,4096);
									ret = lpSoundBuffer->o_dat->DecodeOGG(workBuf,rsize,outBuf,0x20000,&outsize);
									if(OGG_DEC_END==ret){
										goto oggFilEnd;
									}
								}
							}
							if(remainSize){
								CopyMemory(lpBlockAdd1+writeSize,outBuf,remainSize);
								lpSoundBuffer->leakSize = outsize -remainSize;
								lpSoundBuffer->leakBuff = new BYTE[lpSoundBuffer->leakSize];
								CopyMemory(lpSoundBuffer->leakBuff,outBuf +remainSize,lpSoundBuffer->leakSize);
							}
						}
					}else{
						while(remainSize>=outsize && remainSize>0){
							CopyMemory(lpBlockAdd1+writeSize,outBuf,outsize);
							remainSize -= outsize;
							writeSize += outsize;
							if(remainSize > 0){
								rsize = lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,workBuf,4096);
								ret = lpSoundBuffer->o_dat->DecodeOGG(workBuf,rsize,outBuf,0x20000,&outsize);
								if(OGG_DEC_END==ret){
									goto oggFilEnd;
								}
							}
						}
						if(remainSize){
							CopyMemory(lpBlockAdd1+writeSize,outBuf,remainSize);
							lpSoundBuffer->leakSize = outsize -remainSize;
							lpSoundBuffer->leakBuff = new BYTE[lpSoundBuffer->leakSize];
							CopyMemory(lpSoundBuffer->leakBuff,outBuf +remainSize,lpSoundBuffer->leakSize);
						}
					}
				}else if(NULL==lpSoundBuffer->hAcm){
					size = lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpBlockAdd1,blockSize1);
					if(size < blockSize1){
						if(1==lpSoundBuffer->repeat){
							ZeroMemory(lpBlockAdd1+size,blockSize1-size);
							lpSoundBuffer->repeat = 0;
							lpSoundBuffer->lpDSBuffer->Play(0,0,0);
						}else{
							if(lpSoundBuffer->repeat) lpSoundBuffer->repeat --;
							lpSoundBuffer->readFile->StreamSeekFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpSoundBuffer->dataTopOffset,FILE_BEGIN);
							lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpBlockAdd1+size,blockSize1-size);
						}
					}else{
						if(lpSoundBuffer->streamBlockSize>blockSize1){
							size = lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpBlockAdd2,blockSize2);
							if(size < blockSize2){
								if(1==lpSoundBuffer->repeat){
									ZeroMemory(lpBlockAdd2+size,blockSize2-size);
									lpSoundBuffer->repeat = 0;
									lpSoundBuffer->lpDSBuffer->Play(0,0,0);
								}else{
									if(lpSoundBuffer->repeat) lpSoundBuffer->repeat --;
									lpSoundBuffer->readFile->StreamSeekFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpSoundBuffer->dataTopOffset,FILE_BEGIN);
									lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpBlockAdd2+size,blockSize2-size);
								}
							}
						}
					}
				}else{
					int		remainSize = blockSize1;
					int		writeSize = 0;
					if(lpSoundBuffer->leakSize){
						CopyMemory(lpBlockAdd1,lpSoundBuffer->leakBuff,lpSoundBuffer->leakSize);
						writeSize += lpSoundBuffer->leakSize;
						remainSize -= lpSoundBuffer->leakSize;
						lpSoundBuffer->leakSize = 0;
						my_deletes(lpSoundBuffer->leakBuff);
					}
					size = lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,
						(char *)lpSoundBuffer->acmSrc.lpStream,lpSoundBuffer->acmSrc.dwStreamSize);
					if(size < lpSoundBuffer->acmSrc.dwStreamSize){
						lpSoundBuffer->ash.cbSrcLength = lpSoundBuffer->ash.dwSrcUser = size;
						MMRESULT nError = acmStreamConvert(lpSoundBuffer->hAcm,&lpSoundBuffer->ash,0);
						lpSoundBuffer->ash.cbSrcLength = lpSoundBuffer->ash.dwSrcUser = lpSoundBuffer->acmSrc.dwStreamSize;
						if(1==lpSoundBuffer->repeat){
							if(0==nError){
								if(remainSize >= lpSoundBuffer->ash.cbDstLengthUsed){
									CopyMemory(lpBlockAdd1+writeSize, lpSoundBuffer->acmDst.lpStream, lpSoundBuffer->ash.cbDstLengthUsed);
									writeSize  += lpSoundBuffer->ash.cbDstLengthUsed;
									remainSize -= lpSoundBuffer->ash.cbDstLengthUsed;
									ZeroMemory(lpBlockAdd1+writeSize, remainSize);
									if(blockSize2)ZeroMemory(lpBlockAdd2,blockSize2);
								}else{
									CopyMemory(lpBlockAdd1+writeSize, lpSoundBuffer->acmDst.lpStream, remainSize);
									CopyMemory(lpBlockAdd2, lpSoundBuffer->acmDst.lpStream +remainSize,lpSoundBuffer->ash.cbDstLengthUsed -remainSize);
									if(blockSize2 > lpSoundBuffer->ash.cbDstLengthUsed -remainSize){
										ZeroMemory(lpBlockAdd2 +(lpSoundBuffer->ash.cbDstLengthUsed -remainSize),blockSize2-(lpSoundBuffer->ash.cbDstLengthUsed -remainSize));
									}
								}
								lpSoundBuffer->repeat = 0;
								lpSoundBuffer->lpDSBuffer->Play(0,0,0);
							}
						}else{
							if(lpSoundBuffer->repeat) lpSoundBuffer->repeat --;
							lpSoundBuffer->readFile->StreamSeekFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,lpSoundBuffer->dataTopOffset,FILE_BEGIN);
							if(0==nError){
								while(remainSize>=lpSoundBuffer->ash.cbDstLengthUsed && remainSize>0){
									CopyMemory(lpBlockAdd1+writeSize,lpSoundBuffer->acmDst.lpStream,lpSoundBuffer->ash.cbDstLengthUsed);
									remainSize -= lpSoundBuffer->ash.cbDstLengthUsed;
									writeSize += lpSoundBuffer->ash.cbDstLengthUsed;
									if(remainSize > 0){
										size = lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,
											(char *)lpSoundBuffer->acmSrc.lpStream,lpSoundBuffer->acmSrc.dwStreamSize);
										nError = acmStreamConvert(lpSoundBuffer->hAcm,&lpSoundBuffer->ash,ACM_STREAMCONVERTF_BLOCKALIGN);
									}
								}
								if(remainSize){
									CopyMemory(lpBlockAdd1+writeSize,lpSoundBuffer->acmDst.lpStream,remainSize);
									lpSoundBuffer->leakSize = lpSoundBuffer->ash.cbDstLengthUsed -remainSize;
									lpSoundBuffer->leakBuff = new BYTE[lpSoundBuffer->leakSize];
									CopyMemory(lpSoundBuffer->leakBuff,lpSoundBuffer->acmDst.lpStream +remainSize,lpSoundBuffer->leakSize);
								}
							}
						}
					}else{
						MMRESULT nError = acmStreamConvert(lpSoundBuffer->hAcm,&lpSoundBuffer->ash, ACM_STREAMCONVERTF_BLOCKALIGN);
						if(0==nError){
							while(remainSize>=lpSoundBuffer->ash.cbDstLengthUsed && remainSize>0){
								CopyMemory(lpBlockAdd1+writeSize,lpSoundBuffer->acmDst.lpStream,lpSoundBuffer->ash.cbDstLengthUsed);
								remainSize -= lpSoundBuffer->ash.cbDstLengthUsed;
								writeSize += lpSoundBuffer->ash.cbDstLengthUsed;
								if(remainSize > 0){
									size = lpSoundBuffer->readFile->StreamReadFile(lpSoundBuffer->arcFileNum,lpSoundBuffer->streamNum,
										(char *)lpSoundBuffer->acmSrc.lpStream,lpSoundBuffer->acmSrc.dwStreamSize);
									nError = acmStreamConvert(lpSoundBuffer->hAcm,&lpSoundBuffer->ash, ACM_STREAMCONVERTF_BLOCKALIGN);
								}
							}
							if(remainSize){
								CopyMemory(lpBlockAdd1+writeSize,lpSoundBuffer->acmDst.lpStream,remainSize);
								lpSoundBuffer->leakSize = lpSoundBuffer->ash.cbDstLengthUsed -remainSize;
								lpSoundBuffer->leakBuff = new BYTE[lpSoundBuffer->leakSize];
								CopyMemory(lpSoundBuffer->leakBuff,lpSoundBuffer->acmDst.lpStream +remainSize,lpSoundBuffer->leakSize);
							}
						}
					}
				}
				lpSoundBuffer->lpDSBuffer->Unlock(lpBlockAdd1, blockSize1, lpBlockAdd2, blockSize2);
			}
			break;
		  default:
SoundStop:
			lpSoundBuffer->repeat = 0;
			lpSoundBuffer->lpDSNotify->Release();
			lpSoundBuffer->lpDSNotify = NULL;
			CloseHandle(lpSoundBuffer->hEvent[0]);
			CloseHandle(lpSoundBuffer->hEvent[1]);
			CloseHandle(lpSoundBuffer->hEvent[2]);
			lpSoundBuffer->hEvent[0] = lpSoundBuffer->hEvent[1] = lpSoundBuffer->hEvent[2] = NULL;
			lpSoundBuffer->status = PCM_STOP;
			LeaveCriticalSection(&cr_section);
			DeleteCriticalSection(&cr_section);
			CloseHandle(lpSoundBuffer->hLoopThread);
			lpSoundBuffer->hLoopThread = NULL;
			ExitThread(TRUE);
			return 0L;
		}
		LeaveCriticalSection(&cr_section);
	}
	DeleteCriticalSection(&cr_section);
	ExitThread(TRUE);
	return 0L;
} // StreamLoop

DWORD WINAPI SoundLoop(LPVOID param)
{
	CRITICAL_SECTION	cr_section;
	ClSoundBuffer	*lpSoundBuffer = (ClSoundBuffer *)param;

	InitializeCriticalSection(&cr_section);
	while(TRUE){
		WaitForSingleObject(lpSoundBuffer->hEvent[0], INFINITE);
		EnterCriticalSection(&cr_section);
		lpSoundBuffer->repeat --;
		if(lpSoundBuffer->repeat<=0 || PCM_STOP==lpSoundBuffer->status){
			lpSoundBuffer->repeat = 0;
			lpSoundBuffer->lpDSNotify->Release();
			lpSoundBuffer->lpDSNotify = NULL;
			CloseHandle(lpSoundBuffer->hEvent[0]);
			lpSoundBuffer->hEvent[0] = NULL;
			lpSoundBuffer->status = PCM_STOP;
			break;
		}
		lpSoundBuffer->lpDSBuffer->Play(0,0,0);
		LeaveCriticalSection(&cr_section);
	}
	DeleteCriticalSection(&cr_section);
	CloseHandle(lpSoundBuffer->hLoopThread);
	lpSoundBuffer->hLoopThread = NULL;
	ExitThread(TRUE);
	return 0L;
} // SoundLoop

DWORD WINAPI fo_exec(LPVOID param)
{
	ClSoundBuffer	*lpSoundBuffer = (ClSoundBuffer *)param;
	lpSoundBuffer->bFade = TRUE;
	lpSoundBuffer->status = PCM_FADEOUT;
	for(int i=15;i>0;i--){
		if(PCM_FADEOUT!=lpSoundBuffer->status)break;
		LONG	db = DSBVOLUME_MIN;
		if(sysInf.bgmVolume > 0){
			db = LONG(log10((sysInf.bgmVolume/10.0f)*i/16.0f) *20*100);
		}
		lpSoundBuffer->lpDSBuffer->SetVolume(db);
		Sleep(50);
	}
	if(PCM_STOP!=lpSoundBuffer->status){
		lpSoundBuffer->lpDSBuffer->Stop();
		lpSoundBuffer->lpDSBuffer->SetCurrentPosition(0);
		lpSoundBuffer->lpDSBuffer->SetVolume(DSBVOLUME_MAX);
		while(lpSoundBuffer->hEvent[0]) Sleep(1);
	}
	lpSoundBuffer->status = PCM_STOP;
	CloseHandle(lpSoundBuffer->hFadeThread);
	lpSoundBuffer->hFadeThread = NULL;
	if(lpSoundBuffer->nextMusicNum>=0){
		NextMusicHandle *lpNextMusic = lpSoundDS->lpNextMusic;
		if(NULL==lpSoundDS->lpNextMusic){
			lpSoundDS->lpNextMusic = new NextMusicHandle;
			lpNextMusic = lpSoundDS->lpNextMusic;
		}else{
			while(lpNextMusic->next)lpNextMusic = lpNextMusic->next;
			lpNextMusic->next = new NextMusicHandle;
			lpNextMusic = lpNextMusic->next;
		}
		lpNextMusic->handle = lpSoundBuffer->handle;
	}
	lpSoundBuffer->bFade = FALSE;
	ExitThread(TRUE);
	return 0L;
} // fo_exec
