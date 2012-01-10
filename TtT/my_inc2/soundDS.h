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
	WORD					wFormatTag;	
	
	int						streamBlockSize;
	int						dataTopOffset;
	int						arcFileNum;
	int						musicNum;
	int						streamNum;
	int						nextStream;
	int						orgMusicNum;
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
		bufType = memory_sound;
		status = PCM_STOP;
		hLoopThread = hFadeThread = NULL;
		hEvent[0] = hEvent[1] = hEvent[2] = NULL;
		lpDSBuffer = NULL;
		lpDSNotify = NULL;
		lpDSB3D = NULL;
		lpPrev = lpNext = NULL;
		hAcm = NULL;
		acmSrc.lpStream = acmDst.lpStream = NULL;
		leakBuff = NULL;
		leakSize = 0;
		o_dat = NULL;
		streamNum = -1;
		orgMusicNum = musicNum = -1;
		nextMusicNum = -1;
		nextStream = -1;
		bFade = FALSE;
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

public:
	NextMusicHandle			*lpNextMusic;
	LPDIRECTSOUND3DLISTENER8	lpListener;

	ClSoundDS(HWND hWnd,ClReadFile *readFile,BOOL b3DSound=FALSE);
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
	ClResult ChangeMusic(int &handle,int arcFileNum,int num,BOOL bFade,int repeat, int vol);
	ClResult ClSoundDS::GetPlayStreamVolume(int &handle, DWORD volme[2] );
	ClResult playVoice(int &handle,int arcFileNum,char *fname, int repeat, int vol);
	ClResult CheckVoiceFile(int arcFileNum,char *fname);

	ClResult play(int handle,int repeat);
	void NextMusicStart(void);
	void Stop(int handle);
	ClResult FadeOut(int handle);
	int GetStatus(int handle);
	void SetVolume(int handle,int value );
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

private:
	int SearchObjTable(void);
	void SetObjTable(int handle,BOOL bSet);
	BOOL AddSoundBuffer(int handle,char *fileBuf,BOOL b3DSound = FALSE);
	ClSoundBuffer *SelectSoundBuffer(int handle);
};

#define	hEvtSndMax		4
extern ClSoundDS	*lpSoundDS;
extern int			bgmHandle;
extern int			voiceHandle;
extern int			evtHandle[hEvtSndMax];

extern int	bgmVolume;
extern int	voiceVolume;
extern int	seVolume;


#define PlayVoice(x)	lpSoundDS->playVoice(voiceHandle,pack_voice,x)

#define CreateEffect(x,y)		lpSoundDS->CreateSoundObj(x,pack_sound,y)
#define	PlayLoopEffect(x)		lpSoundDS->play(x,0)
#define StopEffect(x)			lpSoundDS->Stop(x)
#define ReleaseSoundHandle(x) {lpSoundDS->ReleaseSoundObj(x); x = -1;}


inline void PlayEffect(int x,BYTE vol,BOOL bLoop=FALSE)
{
	int	loopCnt = 1;
	if(bLoop)loopCnt = 0;
	lpSoundDS->SetVolume(x,vol);
	lpSoundDS->play(x,loopCnt);
}

inline ClResult PlayEffectNum(int &x,int y,int vol,BOOL bLoop=FALSE)
{
	
	char	fn[256];
	wsprintf(fn,"SE_%04d.wav",y);

	char	fname2[256];
	char	fname3[256];
	wsprintf( fname2, "se\\%s", fn );
	wsprintf( fname3, "se\\buf\\%s", fn );
	CopyFile(fname2,fname3,TRUE);

	int num = readFile->SearchFile(pack_sound,fn);
	if(num==(-1)) return Cl_FILE_NOTFOUND;	
	CreateEffect(x,num);
	PlayEffect(x,vol,bLoop);
	return Cl_OK;
}

inline void ClearSound(int &x)
{
	CreateEffect(x,0);
	lpSoundDS->SetVolume(x,0);
	lpSoundDS->play(x,1);

}

#define _LEAFLIB_ClSoundDS
#endif  _LEAFLIB_ClSoundDS
