
#include "ClCommon.h"
#include <mbstring.h>


ClReadFile	*readFile = NULL;


inline BOOL CopyMemOffset(void *dst, char **src, size_t size)
{
	CopyMemory(dst,*src,size);
	(*src) += size;
	return TRUE;
}

int my_strcmpi(BYTE *str1,BYTE *str2)
{
	int		i,len1,len2,min,diff,c[2];
	min = len1 = lstrlen((LPCSTR)str1);
	len2 = lstrlen((LPCSTR)str2);
	if(len2 < min)min = len2;
	for(i=0;i<min;i++){
		c[0] = str1[i];
		if(c[0]>=0x41 && c[0]<=0x5A)c[0] += 0x20;
		c[1] = str2[i];
		if(c[1]>=0x41 && c[1]<=0x5A)c[1] += 0x20;
		diff = c[0] - c[1];
		if(0==diff)continue;
		return (diff);
	}
	return (len1-len2);
} 




struct LACHEAD{
	char	head[4];	
	int		fileCnt;	
};

#define Ns   4096
#define Fs     18


int ClReadFile::OpenPackFile(LPCSTR packName,int packNum)
{
	int			i,j;
	LACHEAD		head;
	HANDLE		handle;
	DWORD		rsize;
	char		filename[MAX_PATH];
	arc_file_info	arcFileInfo;

	wsprintf(filename,"%s.PAK",packName);
	
	handle = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0L,NULL);
	if(INVALID_HANDLE_VALUE==handle)return -1;	

	arcFileInfo.handle = handle;
	lstrcpy(arcFileInfo.dirName,packName);
	
	ReadFile(handle,&head,sizeof(LACHEAD),&rsize,NULL);
	arcFileInfo.fileCount = head.fileCnt;
	arcFileInfo.pack_file = new file_info[head.fileCnt];
	if((-1)==packNum || packNum>=openFileNum){
		ArcFile.push_back(arcFileInfo);
	}else{
		my_deletes(ArcFile[packNum].pack_file);
		ArcFile[packNum] = arcFileInfo;
	}
	
	ReadFile(handle,arcFileInfo.pack_file,sizeof(file_info)*head.fileCnt,&rsize,NULL);
	
	for(i=0;i<arcFileInfo.fileCount;i++){
		for(j=0;j<int(lstrlen(arcFileInfo.pack_file[i].fname));j++){
			arcFileInfo.pack_file[i].fname[j] = ~(arcFileInfo.pack_file[i].fname[j]);
		}
	}
	if(packNum!=(-1) && packNum<openFileNum){
		return packNum;
	}
	openFileNum ++;
	return (openFileNum -1);
} 


int ClReadFile::ReadPackFile(int arcFileNum, LPCSTR decFile, char **fileBuf)
{
	int		i;
#ifdef _WINDOWS
	
	HANDLE	hFile;
	DWORD	rsize;
	int		out_size;
	char	filename[MAX_PATH];

	
	wsprintf(filename,"%s\\%s",ArcFile[arcFileNum].dirName,decFile);

	if(GetFileAttributes(filename) == FILE_ATTRIBUTE_ARCHIVE){		
		hFile = CreateFile(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|MY_FILE_SCAN,NULL);
		out_size = GetFileSize(hFile,NULL);
		*fileBuf = (char *)cl_malloc(out_size+1);
		ReadFile(hFile,*fileBuf,out_size,&rsize,NULL);
		(*fileBuf)[out_size] = '\0';
		CloseHandle(hFile);
		return (out_size);
	}
#endif
	i = SearchFile(arcFileNum,decFile,TRUE);
	if( (-1)==i ) return 0;	
	return(ReadPackFileNum(arcFileNum,i,fileBuf));
} 

int ClReadFile::DecodePackFile(int read_size,BYTE *readFile, char **fileBuf)
{
	int i, j, k, r, c;
	unsigned int flags;
	int out_size,write_size;
	unsigned char	*rfP,*ofP;
	unsigned char text[Ns+Fs-1];

	out_size  = *(int *)readFile;
	rfP = readFile + 4;
	*fileBuf = (char *)cl_malloc(out_size+1);
	if(NULL==*fileBuf){
		return 0;
	}
	ofP = (unsigned char *)*fileBuf;
	write_size = 0;
	ZeroMemory(text, Ns - Fs);					
	r = Ns - Fs;  flags = 0;
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			c = *(rfP ++); read_size --;
			if (read_size < 0) break;
			flags = c | 0xff00;
		}
		if (flags & 1) {
			c = *(rfP ++); read_size --;
			if (read_size < 0) break;
			*(ofP ++) = c;  write_size ++;
			if(write_size>out_size){
				cl_free(readFile);	cl_free(*fileBuf);
				return 0;
			}
			text[r++] = c;  r &= (Ns - 1);
		} else {
			i = *(rfP ++); read_size --;
			if (read_size < 0) break;
			j = *(rfP ++); read_size --;
			if (read_size < 0) break;
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + 2;
			for (k = 0; k <= j; k++) {
				c = text[(i + k) & (Ns - 1)];
				*(ofP ++) = c; write_size ++;
				if(write_size>out_size){
					cl_free(readFile);	cl_free(*fileBuf);
					return 0;
				}
				text[r++] = c;  r &= (Ns - 1);
			}
		}
	}
	(*fileBuf)[out_size] = 0;
	return (out_size);
} 


int ClReadFile::ReadPackFileNum(int arcFileNum, int num, char **fileBuf)
{
	DWORD	rsize;
	int out_size;
	unsigned char	*readFile;

	if(num >= ArcFile[arcFileNum].fileCount) return 0;	
	SetFilePointer(ArcFile[arcFileNum].handle,ArcFile[arcFileNum].pack_file[num].seekPoint,NULL,FILE_BEGIN);
	if(0==ArcFile[arcFileNum].pack_file[num].bCompress){	
		out_size = ArcFile[arcFileNum].pack_file[num].pack_size;
		*fileBuf = (char *)cl_malloc(out_size+1);
		ReadFile(ArcFile[arcFileNum].handle,*fileBuf,out_size,&rsize,NULL);
		(*fileBuf)[out_size] = 0;
		return (out_size);
	}
	readFile = (unsigned char *)cl_malloc(ArcFile[arcFileNum].pack_file[num].pack_size+1);
	if(NULL==readFile)return 0;
	ReadFile(ArcFile[arcFileNum].handle,readFile,ArcFile[arcFileNum].pack_file[num].pack_size,&rsize,NULL);
	readFile[ArcFile[arcFileNum].pack_file[num].pack_size] = 0;

	out_size = DecodePackFile(ArcFile[arcFileNum].pack_file[num].pack_size - 4,readFile,fileBuf);
	cl_free(readFile);
	return out_size;
} 


int ClReadFile::StreamOpenFile(int arcFileNum,LPCSTR decFile,int &size)
{
	int		i;
	i = SearchFile(arcFileNum,decFile);
	if( (-1)==i ) return -1;	
	return(StreamOpenFileNum(arcFileNum,i,size));
} 


int ClReadFile::StreamOpenFileNum(int arcFileNum,int num,int &size)
{
	int			stNum;

	if(num >= ArcFile[arcFileNum].fileCount) return -1;	
	if(ArcFile[arcFileNum].pack_file[num].bCompress) return -1;	

	for(stNum=0;stNum<ArcFile[arcFileNum].streamMax;stNum++){
		if(0 > ArcFile[arcFileNum].streamInfo[stNum].num){	
			ArcFile[arcFileNum].streamInfo[stNum].num = num;
			ArcFile[arcFileNum].streamInfo[stNum].seekPoint = ArcFile[arcFileNum].pack_file[num].seekPoint;
			break;
		}
	}
	if(stNum==ArcFile[arcFileNum].streamMax){
		stream_info		streamInfo;
		streamInfo.num = num;
		streamInfo.seekPoint = ArcFile[arcFileNum].pack_file[num].seekPoint;
		ArcFile[arcFileNum].streamInfo.push_back(streamInfo);
		ArcFile[arcFileNum].streamMax ++;
	}
	size = ArcFile[arcFileNum].pack_file[num].pack_size;
	return stNum;
} 


ClResult ClReadFile::StreamCloseFile(int arcFileNum,int streamNum)
{
	if(streamNum>=ArcFile[arcFileNum].streamMax || 0==ArcFile[arcFileNum].streamMax) return Cl_FILE_NOTFOUND;
	if(streamNum == (ArcFile[arcFileNum].streamMax-1)){	
		ArcFile[arcFileNum].streamInfo.pop_back();
		ArcFile[arcFileNum].streamMax --;
	}else{
		ArcFile[arcFileNum].streamInfo[streamNum].num = -1;
	}
	return Cl_OK;
} 


int ClReadFile::StreamReadFile(int arcFileNum,int streamNum,char *read_buf,int size)
{
	DWORD	rsize;
	int		fileEnd;

	
	if(streamNum>=ArcFile[arcFileNum].streamMax || 0>ArcFile[arcFileNum].streamInfo[streamNum].num) return 0;
	if(Cl_FILE_SEEK_ERROR == StreamSeekFile(arcFileNum,streamNum,0,FILE_CURRENT)){
		return 0;	
	}
	
	fileEnd = ArcFile[arcFileNum].pack_file[ArcFile[arcFileNum].streamInfo[streamNum].num].seekPoint +
		ArcFile[arcFileNum].pack_file[ArcFile[arcFileNum].streamInfo[streamNum].num].pack_size;
	if( (ArcFile[arcFileNum].streamInfo[streamNum].seekPoint +size) > fileEnd){
		size = fileEnd - ArcFile[arcFileNum].streamInfo[streamNum].seekPoint;
	}
	
	ReadFile(ArcFile[arcFileNum].handle,read_buf,size,&rsize,NULL);
	ArcFile[arcFileNum].streamInfo[streamNum].seekPoint += rsize;
	return rsize;
} 


ClResult ClReadFile::StreamSeekFile(int arcFileNum,int streamNum,int seekSize,int moveMethod)
{
	DWORD	ret,seekPoint;
	switch(moveMethod){
	  case (FILE_CURRENT):
		seekPoint = ArcFile[arcFileNum].streamInfo[streamNum].seekPoint +seekSize;
		break;
	  case (FILE_BEGIN):
		seekPoint = ArcFile[arcFileNum].pack_file[ArcFile[arcFileNum].streamInfo[streamNum].num].seekPoint +seekSize;
		break;
	  case (FILE_END):
		seekPoint = ArcFile[arcFileNum].pack_file[ArcFile[arcFileNum].streamInfo[streamNum].num].seekPoint
			+ArcFile[arcFileNum].pack_file[ArcFile[arcFileNum].streamInfo[streamNum].num].pack_size
			+seekSize;
		break;
	}
	ret = SetFilePointer(ArcFile[arcFileNum].handle,seekPoint,NULL,FILE_BEGIN);
	if(ret==0xffffffff) return Cl_FILE_SEEK_ERROR;	
	ArcFile[arcFileNum].streamInfo[streamNum].seekPoint = seekPoint;
	return Cl_OK;
} 


ClResult ClReadFile::ExtractFile(int arcFileNum, LPCSTR decFile, LPCSTR newFile)
{
	char	*fileBuf;
	DWORD	Size,wSize;

	Size = ReadPackFile(arcFileNum,decFile,&fileBuf);
	if(0==Size) return Cl_FILE_NOTFOUND;
	HANDLE fh = CreateFile(newFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,NULL);
	if(INVALID_HANDLE_VALUE==fh)return Cl_CANNOT_CREATFILE;
	WriteFile(fh,fileBuf,Size,&wSize,NULL);
	CloseHandle(fh);
	cl_free(fileBuf);
	return Cl_OK;
} 


char *ClReadFile::GetFileName(int arcFileNum,int num)
{
	if(num >= ArcFile[arcFileNum].fileCount) return NULL;	
	return (ArcFile[arcFileNum].pack_file[num].fname);
} 


ClReadFile::ClReadFile()
{
	int		i;
	
	ArcFile.reserve(15);
	for(i=0;i<15;i++){
		ArcFile[i].handle = NULL;
		ArcFile[i].fileCount = 0;
		ArcFile[i].pack_file = NULL;
	}
	openFileNum = 0;
} 


ClReadFile::~ClReadFile()
{
	int			i;
	for(i=0;i<openFileNum;i++){
		CloseHandle(ArcFile[i].handle);
		delete ArcFile[i].pack_file;
	}
} 





int ClReadFile::SearchFile(int arcFileNum, LPCSTR decFile,BOOL errCheck)
{
	int		i,j;
	int		min,max;

	
	i = ArcFile[arcFileNum].fileCount /2;
	min = 0; max = ArcFile[arcFileNum].fileCount;

	while(0!=(j=my_strcmpi((BYTE *)ArcFile[arcFileNum].pack_file[i].fname,(BYTE *)decFile))){

		if(j<0){
			if(min==i){
				i = -1;
				break;
			}
			min = i;
		}else{
			if(max==i){
				i = -1;
				break;
			}
			max = i;
		}
		i = (max+min)/2;
	}
	if( (-1)==i){

#ifdef _DEBUG
		if(errCheck)
			MessageBox(NULL,decFile,"Ì§²Ùµ°ÌßÝ´×°",MB_OK|MB_ICONERROR);
#endif _DEBUG
	}
	return i;
} 

int ClReadFile::SequentialSearchFile(int arcFileNum, LPCSTR decFile)
{	
	for(int i=0;i<ArcFile[arcFileNum].fileCount;i++){
		if(_stricmp(ArcFile[arcFileNum].pack_file[i].fname,decFile) == 0)break;
	}
	if(i==ArcFile[arcFileNum].fileCount){	
		return -1;
	}
	return i;
} 





int ReadPackFileFromMem(char *decFile,char *packMem,char **fileBuf)
{
	int			i,j,min,max,out_size = 0;
	LACHEAD		head;
	file_info	*pack_file;
	char		*tmpMem = packMem;

	CopyMemOffset(&head,&tmpMem,sizeof(LACHEAD));
	pack_file = new file_info[head.fileCnt];
	
	CopyMemOffset(pack_file,&tmpMem,sizeof(file_info)*head.fileCnt);
	
	for(i=0;i<head.fileCnt;i++){
		for(j=0;j<int(lstrlen(pack_file[i].fname));j++){
			pack_file[i].fname[j] = ~(pack_file[i].fname[j]);
		}
	}
	
	i = head.fileCnt /2;
	min = 0; max = head.fileCnt;
	while(0!=(j=_stricmp(pack_file[i].fname,decFile))){
		if(j<0){
			if(min==i){
				i = -1;
				break;
			}
			min = i;
		}else{
			if(max==i){
				i = -1;
				break;
			}
			max = i;
		}
		i = (max+min)/2;
	}
	if( (-1)==i){

	}else{
		tmpMem = packMem +pack_file[i].seekPoint;
		if(0==pack_file[i].bCompress){	
			out_size = pack_file[i].pack_size;
			*fileBuf = (char *)cl_malloc(out_size+1);
			CopyMemory(*fileBuf,tmpMem,out_size);
			(*fileBuf)[out_size] = 0;
		}else{							
			out_size = readFile->DecodePackFile(pack_file[i].pack_size - 4,(LPBYTE)tmpMem,fileBuf);
		}
	
	}
	delete[] pack_file;
	return out_size;
} 