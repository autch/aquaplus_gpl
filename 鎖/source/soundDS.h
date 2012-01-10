#ifndef _LEAFLIB_ClSoundDS

#include "ClCommon.h"
#include "readfile.h"
#include "oggDec.h"
#include <dsound.h>
#include <MMREG.H>
#include <msacm.h>

enum SoundBufType{
	memory_sound = 0,	
	stream_sound,		
};

enum CL_PCMSTATUS{
	PCM_STOP		= 0,	
	PCM_PLAY,				
	PCM_FADEOUT,			
	PCM_FADEOUTBREAK,		
};

struct AcmBuffer{
	DWORD	dwStreamSize;
	BYTE	*lpStream;
};

struct NextMusicHandle{
	int handle;
	NextMusicHandle *next;

	NextMusicHandle(){
		next = NULL;
	}
	~NextMusicHandle(){
		my_delete(next);
	}
};

struct ClSoundBuffer
{
	int						handle;
	SoundBufType			bufType;
	int						status;
	int						repeat;
	BYTE					bFade;
	BYTE					volume;
	WORD					wFormatTag;	
	int						streamBlockSize;
	int						dataTopOffset;
	int						arcFileNum;
	int						musicNum;
	int						streamNum;
	int						nextStream;
	int						orgMusicNum;
	int						fo_frame;
	int						fo_value;
	int						fo_step;
	HANDLE					hLoopThread;
	HANDLE					hFadeThread;
	HANDLE					hEvent[3];
	LPDIRECTSOUNDBUFFER8	lpDSBuffer;
	LPDIRECTSOUNDNOTIFY8	lpDSNotify;
	LPDIRECTSOUND3DBUFFER8	lpDSB3D;
	ClReadFile				*readFile;
	ClSoundBuffer			*lpPrev;
	ClSoundBuffer			*lpNext;
	int						nextArcFileNum;
	int						nextMusicNum;
	int						nextMusicVolume;
	int						nextRepeat;
	int						endPt;
	HACMSTREAM				hAcm;
	ACMSTREAMHEADER			ash;
	AcmBuffer				acmDst;
	AcmBuffer				acmSrc;
	int						leakSize;
	BYTE					*leakBuff;
	OggDec					*o_dat;

	ClSoundBuffer(){
		ZeroMemory(this,sizeof(ClSoundBuffer));
		bufType = memory_sound;
		status = PCM_STOP;
		streamNum = -1;
		orgMusicNum = musicNum = -1;
		nextMusicNum = -1;
		nextStream = -1;
		volume = 0xff;
	}
	~ClSoundBuffer(){
		if(bFade){
			status = PCM_FADEOUTBREAK;
			while(TRUE==bFade)Sleep(1);
		}
		if(hEvent[0]){
			status = PCM_STOP;
			lpDSBuffer->Stop();
			while(hEvent[0])Sleep(1);
		}
		if(hAcm){
			acmStreamUnprepareHeader(hAcm,&ash,0);
			acmStreamClose(hAcm,0);
			cl_free(acmSrc.lpStream);
			cl_free(acmDst.lpStream);
		}
		if(streamNum>=0)readFile->StreamCloseFile(arcFileNum,streamNum);
		if(hFadeThread) CloseHandle(hFadeThread);
		if(hLoopThread) CloseHandle(hLoopThread);
		if(lpDSNotify) lpDSNotify->Release();
		if(lpDSBuffer) lpDSBuffer->Release();
		if(lpDSB3D) lpDSB3D->Release();
		my_delete(leakBuff);
		my_delete(o_dat);
		if(lpNext) delete lpNext;
	}
};

struct OggLeafHead
{
	char	head[4];
	WORD	nChannels; 
	DWORD	nSamplesPerSec;
	DWORD	dataSize;
	BYTE	bLoop;
	WORD	loopCnt;
	DWORD	loopStart;
	DWORD	loopStartOrg;
	DWORD	loopStartDec;
	DWORD	loopEnd;
};

class ClSoundDS
{
	BOOL					Enable;
	LPDIRECTSOUND8			lpDSound;
	LPDIRECTSOUNDBUFFER		lpPrimaryBuffer;
	LPDIRECTSOUNDBUFFER8	lpDummyBuffer;
	ClSoundBuffer			*lpDSBufferTop;
	ClSoundBuffer			*lpDSBufferTail;
	ClReadFile				*lpReadFile;
	DWORD					objTable[32];
	OggLeafHead				oglHead;
	int						musicNum;
	int						musicVol;

public:
	BYTE					Pack_bgmfile;
	BYTE					Pack_sound;
	BYTE					Pack_voice;
	BYTE					bgmVolume;		
	BYTE					voiceVolume;	
	BYTE					seVolume;		
	BYTE					bChangeingMusic;
	int						voiceCnt;
	int						scriptNo;
	NextMusicHandle			*lpNextMusic;
	LPDIRECTSOUND3DLISTENER8	lpListener;

	ClSoundDS(HWND hWnd, ClReadFile *readFile, BYTE p_bgm, BYTE p_sound, BYTE p_voice, BYTE v_bgm, BYTE v_sound, BYTE v_voice, BOOL b3DSound=FALSE);
	~ClSoundDS()
	{
		if(lpNextMusic) delete lpNextMusic;
		if(lpDSBufferTop) delete lpDSBufferTop;
		if(lpDummyBuffer){
			Sleep(500);
			RELEASE(lpDummyBuffer);
		}
		RELEASE(lpPrimaryBuffer);
		RELEASE(lpListener);
		RELEASE(lpDSound);
	}

	ClResult CreateSoundObj(int &handle,int arcFileNum,int playNum,BOOL bStream=FALSE,BOOL b3DSound=FALSE);
	void ReleaseSoundObj(int handle);
	void ReleaseSoundBuffer(int handle);
	ClResult ChangeMusic(int &handle,int num,BOOL bFade,int repeat,int fo_frame=30,int volume=255);
	ClResult playVoice(int &handle,int arcFileNum,char *fname,BOOL bLoop=FALSE,int volume=255);
	ClResult playVoiceNum(int &handle,int arcFileNum,int charNo,BOOL bLoop=FALSE,int volume=255,int voice_set=-1);
	void SetVoiceNo(int cnt,int scrNo);
	ClResult play(int handle,int repeat);
	void NextMusicStart(void);
	void Stop(int handle);
	ClResult SetVolumeFade(int handle,int value, int frame=30);
	ClResult FadeOut(int handle,int frame=30);
	int GetStatus(int handle);
	BYTE GetVolume(int handle);
	void SetVolume(int handle,int value,int level);
	void SetVolumeLevel(int handle,int level);
	int GetRepeatNum(int handle);
	void CreateD3DSoundListener(D3DVALUE distance);
	int Create3DBuffer(int effectNum,D3DVALUE x,D3DVALUE y,D3DVALUE z);
	BOOL SetPosition(int handle,D3DVALUE x,D3DVALUE y,D3DVALUE z);
	void Set3DListener(float x,float y,float z,float deg);
	BOOL Play3D(int handle,BOOL bLoop);
	BOOL CreateStreamBuffer(int handle,int arcFileNum,int playNum);
	BOOL CreateDummyBuffer(void);

	inline LPDIRECTSOUND8 GetDirectSound(void){ return lpDSound; }
	inline LPDIRECTSOUNDBUFFER GetPrimarySoundBuffer(void){ return lpPrimaryBuffer; }
	int GetMusicNo(int handle);
	int GetMusicFileNo(void) { return musicNum; }
	int GetMusicVolume(void) { return musicVol; }

private:
	int SearchObjTable(void);
	void SetObjTable(int handle,BOOL bSet);
	BOOL AddSoundBuffer(int handle,char *fileBuf,int sndNum, BOOL b3DSound = FALSE);
	ClSoundBuffer *SelectSoundBuffer(int handle);
};

#define	hEvtSndMax		12
extern ClSoundDS	*lpSoundDS;
extern int			bgmHandle;
extern int			voiceHandle;
extern int			evtHandle[hEvtSndMax];

#define PlayVoice(x,y,z)	lpSoundDS->playVoice(voiceHandle,lpSoundDS->Pack_voice,x,y,z)
#define PlayVoiceNum(x,y,z,v)	lpSoundDS->playVoiceNum(voiceHandle,lpSoundDS->Pack_voice,x,y,z,v)
#define	PlayLoopEffect(x)		lpSoundDS->play(x,0)
#define CreateEffect(x,y)	lpSoundDS->CreateSoundObj(x,lpSoundDS->Pack_sound,y)
#define StopEffect(x)	lpSoundDS->Stop(x)
#define FadeOutEffect(x,frame)	lpSoundDS->FadeOut(x,frame)
#define ReleaseSoundHandle(x) {lpSoundDS->ReleaseSoundObj(x); x = -1;}
#define SetVolumeLevelG(handle,level){ if(handle>=0)lpSoundDS->SetVolumeLevel(handle, level); }

inline void PlayEffect(int x,BYTE vol=255,BOOL bLoop=FALSE)
{
	int	loopCnt = 1;
	if(bLoop)loopCnt = 0;
	lpSoundDS->SetVolume(x,vol,lpSoundDS->seVolume);
	lpSoundDS->play(x,loopCnt);
}

inline void PlayEffectNum(int &x,int y,int vol=255,BOOL bLoop=FALSE,int frame=0)
{
	char	fn[32];
	wsprintf(fn,"SE_%04d.wav",y);
	int num = readFile->SearchFile(lpSoundDS->Pack_sound,fn);
	if(num==(-1)) return;	
	CreateEffect(x,num);
	if(0==frame){
		PlayEffect(x,vol,bLoop);
	}else{
		PlayEffect(x,0,bLoop);
		lpSoundDS->SetVolumeFade(x, vol, frame);
	}
}

inline void ClearSound(int &x)
{
	CreateEffect(x,0);
	lpSoundDS->SetVolume(x,0,0);
	lpSoundDS->play(x,1);
}

#define _LEAFLIB_ClSoundDS
#endif  _LEAFLIB_ClSoundDS
