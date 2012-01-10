
#include <windows.h>
#include <mmsystem.h>
#include <winnls32.h>

#include "movie.h"

static HWND AviWindowHWND=NULL;

void MOV_DebugBox( HWND hwnd, char *mes, ... )
{
	char	buf[512];
	if(hwnd==NULL) hwnd = AviWindowHWND;

	wvsprintf( buf, mes, (char *)(&mes+1) );
	MessageBox( hwnd, buf, "警告", MB_OK|MB_SETFOREGROUND );
}



typedef struct{
	BOOL			flag;	
	DWORD			mode;	

	PAVIFILE		pAVIIn;				

	AVISTREAMINFO	audioStreamInfo;	
	PAVISTREAM		pAudioStream;
	AVISTREAMINFO	videoStreamInfo;	
	PAVISTREAM		pVideoStream;

	PGETFRAME		pVideoFrame;		
	void			*VideoBuf;			
	DWORD			VideoBufSize;
	BITMAPINFOHEADER	bi;

	WAVEFORMATEX	audioFmt;


	DWORD			videoFrame;		
	DWORD			videoFrame2;	
	DWORD			audioFrame;		
	DWORD			audioSample;	

	DWORD			playTime;		
	DWORD			pauseTime;



	float			frameTime;		



	char			bUseAudio;
	char			bUseVideo;

}AVI_STRUCT;


AVI_STRUCT	AviStruct[AVI_MAX];



int MOV_OpenFile( char *fname, int w, int h, int bit )
{
	int					i,size;
	int					ret=-1;
	AVIFILEINFO			FileInfo;
	PAVISTREAM			pAVIStream;
	AVISTREAMINFO		StreamInfo;
	HRESULT				hResult;

	AVI_STRUCT			*as;

	for( i=0 ; i<AVI_MAX ; i++ ){
		if( !AviStruct[i].flag ){
			ret = i;
			as  = &AviStruct[i];
			break;
		}
	}
	if( ret==-1 ){
		MOV_DebugBox( NULL, "AVIストリームが多過ぎます[4つまで]" );
		return -1;
	}
	
	AVIFileInit();


	{
		CLSID clsid={0x00020000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};

		hResult = AVIFileOpen( &as->pAVIIn, fname, OF_READ | OF_SHARE_DENY_NONE, NULL );
	}

	switch(hResult){
		case 0:	break;	
		case AVIERR_BADFORMAT:		MOV_DebugBox( NULL, "ファイルが破損しているか、Codecがインストールされていないため、読み取りできませんでした。" );	return -1;
		case AVIERR_MEMORY:			MOV_DebugBox( NULL, "メモリ不足のため、ファイルを開けませんでした。" );			return -1;
		case AVIERR_FILEREAD:		MOV_DebugBox( NULL, "ファイルの読み取り中にディスク エラーが発生しました。" );	return -1;
		case AVIERR_FILEOPEN:		MOV_DebugBox( NULL, "ファイルのオープン中にディスク エラーが発生しました。" );	return -1;
		case REGDB_E_CLASSNOTREG:	MOV_DebugBox( NULL, "AVIFileOpen 関数で指定されたファイルタイプを持つハンドラが、レジストリに見つかりませんでした " );	return -1;
		case E_NOINTERFACE:
		default:					MOV_DebugBox( NULL, "何らかの原因により AVIFileOpen関数が失敗しました[%08x] ", hResult );		return -1;
	}
	
	FileInfo.dwStreams = 0L;
	hResult = AVIFileInfo( as->pAVIIn, &FileInfo, sizeof(AVIFILEINFO) );
	if(hResult){
		MOV_DebugBox( NULL, "AVI情報の取得に失敗しました()" );
		return  -1;
	}

	
	for( i=0; i<(int)FileInfo.dwStreams ; i++ ){
		hResult = AVIFileGetStream( as->pAVIIn, &pAVIStream, 0L, i );
		if(hResult) break;
		hResult = AVIStreamInfo( pAVIStream, &StreamInfo, sizeof(AVISTREAMINFO) );
		if(hResult) continue;

		if( StreamInfo.fccType==streamtypeVIDEO ){		
			as->pVideoStream = pAVIStream;
			as->videoStreamInfo = StreamInfo;

		}else if(StreamInfo.fccType==streamtypeAUDIO){	
			as->bUseAudio = FALSE;
			OutputDebugString("AVI音声は未サポートです\n");
			break;

			as->bUseAudio = TRUE;
			as->pAudioStream = pAVIStream;
			as->audioStreamInfo = StreamInfo;
			as->audioFrame = 0;

			size = sizeof(WAVEFORMATEX);
			AVIStreamReadFormat( as->pAudioStream, 0, &as->audioFmt, (long *)&size );
			if( WAVE_FORMAT_PCM==as->audioFmt.wFormatTag ){
				as->audioFmt.cbSize = 0;

				break;
			}else{


				break;
			}
		}
	}

	as->flag  = TRUE;

	switch( MOV_SetVideoBufFormat( ret, w, h, bit,NULL ) ){
		default:
		case -1:
			MOV_DebugBox( NULL, "AVIが描画できません。\nCodecがインストールされていないと思われます。\nCD内の[iv5setup.exe]を実行して、インストール作業を行って下さい。" );
			as->bUseVideo = FALSE;
			break;
		case 0:
			as->bUseVideo = TRUE;
			break;
		case 1:
			OutputDebugString( "指定フォーマットで描画できません。標準フォーマットで再生します。\n" );

			as->bUseVideo = TRUE;
			break;
	}
	
	{
		BITMAPINFOHEADER	*lpbmpih = (BITMAPINFOHEADER*)AVIStreamGetFrame( as->pVideoFrame, 0 );
		if(lpbmpih==NULL) return ret;
		as->VideoBuf     = ((char*)lpbmpih)+lpbmpih->biSize;
		as->VideoBufSize = lpbmpih->biSizeImage;
	}

	
	as->mode  = AVI_STOP;
	as->videoFrame = 0;
	as->videoFrame2 = -1;
	as->pauseTime = as->playTime  = timeGetTime();
	as->frameTime = 1000.0f * as->videoStreamInfo.dwScale / as->videoStreamInfo.dwRate;

	return ret;
}


BOOL MOV_CloseFile( int mov_no )
{
	if( AviStruct[mov_no].flag ){
		AVIStreamGetFrameClose( AviStruct[mov_no].pVideoFrame );

		AVIFileRelease( AviStruct[mov_no].pAVIIn );
		AVIFileExit();
	}
	ZeroMemory( &AviStruct[mov_no], sizeof(AVI_STRUCT) );
	return TRUE;
}



BOOL MOV_Play( int mov_no )
{
	AVI_STRUCT	*as = &AviStruct[mov_no];

	if(as->flag){
		as->mode       = AVI_PLAY;
		as->audioFrame = 0;
		as->videoFrame  = 0;
		as->videoFrame2 = -1;
		as->playTime   = timeGetTime();
		return TRUE;
	}else{
		return FALSE;
	}
}


BOOL MOV_RePlay( int mov_no )
{
	AVI_STRUCT	*as = &AviStruct[mov_no];

	if(as->flag){
		as->mode     = AVI_PLAY;
		as->playTime = as->playTime + (timeGetTime()-as->pauseTime);
		return TRUE;
	}else{
		return FALSE;
	}
}


BOOL MOV_Stop( int mov_no )
{
	AVI_STRUCT	*as = &AviStruct[mov_no];

	if(as->flag){
		as->mode       = AVI_STOP;
		as->pauseTime  = timeGetTime();
		return TRUE;
	}else{
		return FALSE;
	}
}




int MOV_SetVideoBufFormat( int mov_no, int w, int h, int bit, BITMAPINFOHEADER *bi )
{
	BITMAPINFOHEADER	bi2;
	long				size;
	AVI_STRUCT			*as;
	int					ret=0;
	h=480;

	if(bi==NULL)	bi = &bi2;

	if( mov_no<0 || mov_no>=AVI_MAX )	return -1;
	if( !AviStruct[mov_no].flag )		return -1;
	
	as = &AviStruct[mov_no];

	if( (as->bi.biBitCount==bit || 0==bit) &&
		(as->bi.biWidth   ==w   || 0==w) &&
		(as->bi.biHeight  ==h   || 0==h) ){
		*bi = as->bi;
		return 2;
	}

	
	size = bi->biSize   = sizeof(BITMAPINFOHEADER);
	if(w)	bi->biWidth    = w;
	else	bi->biWidth    = as->bi.biWidth;
	if(h)	bi->biHeight   = -h;
	else	bi->biHeight   = as->bi.biHeight;
	bi->biPlanes = 1;
	if(bit)	bi->biBitCount = bit;
	else	bi->biBitCount = as->bi.biBitCount;
	bi->biCompression   = BI_RGB;
	bi->biSizeImage     = (bi->biWidth)*(-bi->biHeight)*(bi->biBitCount/8);
	bi->biXPelsPerMeter = bi->biYPelsPerMeter = 0;
	bi->biClrImportant  = bi->biClrUsed = 0;

	
	as->pVideoFrame = AVIStreamGetFrameOpen( as->pVideoStream, bi );
	if(NULL==as->pVideoFrame){	
		ret = 1;
		AVIStreamReadFormat( as->pVideoStream, 0, bi, (long *)&size );

		if(bi->biHeight>0) bi->biHeight = -bi->biHeight;						
		bi->biCompression   = BI_RGB;
		bi->biSizeImage     = (bi->biWidth)*(-bi->biHeight)*(bi->biBitCount/8);	
		bi->biXPelsPerMeter = bi->biYPelsPerMeter = 0;
		bi->biClrImportant  = bi->biClrUsed = 0;

		as->pVideoFrame = AVIStreamGetFrameOpen( as->pVideoStream, bi );
		if(NULL==as->pVideoFrame){	
			return -1;
		}
	}
	as->bi = *bi;
	return ret;
}


int MOV_GetVideoBufFormat( int mov_no, BITMAPINFOHEADER *bi )
{
	if( bi==NULL )	return FALSE;
	if( mov_no<0 || mov_no>=AVI_MAX )	return FALSE;
	if( !AviStruct[mov_no].flag )		return FALSE;
	
	*bi = AviStruct[mov_no].bi;
	return TRUE;
}


DWORD MOV_GetVideoBuf( int mov_no, void **buf, int *w, int *h, int *bit )
{
	if( AviStruct[mov_no].flag && AviStruct[mov_no].bUseVideo ){
		*buf = AviStruct[mov_no].VideoBuf;
		if(bit)*bit = AviStruct[mov_no].bi.biBitCount;
		if(w)  *w   = AviStruct[mov_no].bi.biWidth;
		if(h)  *h   = abs(AviStruct[mov_no].bi.biHeight);
		return AviStruct[mov_no].VideoBufSize;
	}else{
		*buf = NULL;
		return 0;
	}
}


int MOV_GetMode( int mov_no )
{
	if( AviStruct[mov_no].flag ){
		return AviStruct[mov_no].mode;
	}else{
		return -1;
	}
}


int MOV_GetNowFrameVideo( int mov_no )
{
	if( AviStruct[mov_no].flag ){
		return AviStruct[mov_no].videoFrame;
	}else{
		return -1;
	}
}


int MOV_GetMaxFrameVideo( int mov_no )
{
	if( AviStruct[mov_no].flag ){
		return AviStruct[mov_no].videoStreamInfo.dwLength;
	}else{
		return -1;
	}
}


int MOV_GetNowFrameAudio( int mov_no )
{
	if( AviStruct[mov_no].flag ){
		return AviStruct[mov_no].audioFrame * AviStruct[mov_no].audioSample;
	}else{
		return -1;
	}
}


int MOV_GetMaxFrameAudio( int mov_no )
{
	if( AviStruct[mov_no].flag ){
		return AviStruct[mov_no].audioStreamInfo.dwLength;
	}else{
		return -1;
	}
}





BOOL MOV_RenewMovie( int mov_no )
{
	int					draw_flag;
	BITMAPINFOHEADER	*lpbmpih;
	DWORD				nowTime;

	AVI_STRUCT			*as = &AviStruct[mov_no];

	nowTime = timeGetTime();
	if( as->flag&& as->bUseVideo && as->mode==AVI_PLAY ){
		
		if(as->bUseAudio){
			draw_flag = as->videoFrame!=as->videoFrame2;
		}else{
			draw_flag = 0;
			while( as->playTime + as->frameTime*(as->videoFrame+1) < nowTime ){
				as->videoFrame ++;
				draw_flag = 1;
				if( as->videoFrame >= as->videoStreamInfo.dwLength ){
					as->mode = AVI_STOP;
					return FALSE;
				}
			}
		}
		if(draw_flag){
			as->videoFrame2=as->videoFrame;
			lpbmpih = (BITMAPINFOHEADER*)AVIStreamGetFrame( as->pVideoFrame, as->videoFrame );
			if(lpbmpih==NULL) return FALSE;
			as->VideoBuf     = ((char*)lpbmpih)+lpbmpih->biSize;
			as->VideoBufSize = lpbmpih->biSizeImage;
		}
	}

	return TRUE;
}



BOOL MOV_RenewMovieAudio( int mov_no, int audioSize, char *audioBuf )
{
	AVI_STRUCT	*as = &AviStruct[mov_no];
	DWORD		sample_size;
	DWORD		sample_pos;
	HRESULT		hResult;
	
	if( as->flag && as->bUseAudio && as->mode==AVI_PLAY ){
		sample_size = audioSize/as->audioStreamInfo.dwSampleSize;
		sample_pos  = as->audioFrame * sample_size;
		as->audioSample = sample_size;
		as->audioFrame++;

		if( sample_pos+sample_size>=as->audioStreamInfo.dwLength ){
			ZeroMemory( audioBuf, audioSize );
			as->mode = AVI_STOP;
			return FALSE;
		}
		hResult = AVIStreamRead( as->pAudioStream, sample_pos, sample_size, audioBuf, audioSize, NULL, NULL );

		as->videoFrame = (int)(((float)sample_pos*as->videoStreamInfo.dwLength) / as->audioStreamInfo.dwLength);
		if(hResult){
			return FALSE;
		}
	}

	return TRUE;
}
