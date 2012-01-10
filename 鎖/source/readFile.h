#ifndef _LEAFLIB_CREADFILE

#include <vector>

using namespace std;

struct file_info{
	char	fname[31];
	char	bCompress;
	int		pack_size;
	int		seekPoint;
};

struct stream_info{
	int		num;
	int		seekPoint;
};

struct arc_file_info{
	HANDLE		handle;
	int			fileCount;
	WORD		resID;
	char		dirName[24];
	file_info	*pack_file;
	vector<stream_info>	streamInfo;
	int					streamMax;
	arc_file_info(){
		ZeroMemory(this,sizeof(arc_file_info));
		resID = 0xffff;	
	}
};

class ClReadFile
{
public:
	ClReadFile();
	~ClReadFile();

	int OpenPackFile(LPCSTR packName,int packNum=-1);
	int OpenPackFileFromRes(WORD resID);
	int ReadPackFile(int arcFileNum, LPCSTR decFile, char **fileBuf);
	int ReadPackFileNum(int arcFileNum, int num, char **fileBuf);
	int ReadPackFileNumFromRes(int arcFileNum, int num, char **fileBuf);
	int StreamOpenFile(int arcFileNum,LPCSTR decFile,int &size);
	int StreamOpenFileNum(int arcFileNum,int num,int &size);
	ClResult StreamCloseFile(int arcFileNum,int streamNum);
	int StreamReadFile(int arcFileNum,int streamNum,char *read_buf,int size);
	ClResult StreamSeekFile(int arcFileNum,int streamNum,int seekSize,int moveMethod);
	ClResult ExtractFileNum(int arcFileNum, int num);
	ClResult ExtractFile(int srcFileNum, LPCSTR decFile);
	char *GetFileName(int arcFileNum,int num);
	char *GetArcDirName(int arcFileNum){return ArcFile[arcFileNum].dirName;}
	int DecodePackFile(int read_size, BYTE *readFile, char **fileBuf);
	BOOL FindFile(int arcFileNum, LPCSTR decFile);
	int SearchFile(int arcFileNum, LPCSTR decFile,BOOL errcheck=FALSE);
	int SequentialSearchFile(int arcFileNum, LPCSTR decFile);
	int GetSeekPoint(int arcFileNum,int num){ return ArcFile[arcFileNum].pack_file[num].seekPoint; }
private:
	vector<arc_file_info>	ArcFile;
	int						openFileNum;

};

int ReadPackFileFromMem(char *decFile,char *packMem,char **fileBuf);

extern ClReadFile	*readFile;

#define _LEAFLIB_CREADFILE
#endif  _LEAFLIB_CREADFILE