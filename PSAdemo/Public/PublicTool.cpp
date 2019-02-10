#include "StdAfx.h"
#include "PublicTool.h"
#include <string.h>

CPublicTool::CPublicTool(void)
{
}

CPublicTool::~CPublicTool(void)
{
}

vector<CString> CPublicTool::SplitCString(CString strSource, char ch)
{  
	CString strTmp;
	vector<CString> vecString;
	int iPos=0;
	while(AfxExtractSubString(strTmp,strSource,iPos,ch))
	{
		iPos++;
		vecString.push_back(strTmp);
	}  
	return vecString;
}

char* CPublicTool::MyW2A(const CString& str){
	int n = str.GetLength();
	int len = WideCharToMultiByte(CP_ACP, 0, str, n, NULL, 0, NULL, NULL);
	char * strchar = new char[len+1];
	WideCharToMultiByte(CP_ACP, 0, str, n+1, strchar, len+1, NULL, NULL);
	strchar[len] = '\0';
	return strchar;
}

void CPublicTool::HImage2CBitmap(HObject pImage,CBitmap *wImage)
{
    HTuple lpcsType[MAX_STRING];
    HTuple  lPointer,width,height,channels;
    HTuple  lPointerR,lPointerG,lPointerB;
    CountChannels(pImage,&channels);
    //获取Halcon数据的Long指针
    if (channels == 3)
    {
        GetImagePointer3(pImage,&lPointerR,&lPointerG,&lPointerB,lpcsType,&width,&height);
    }else
    {
        GetImagePointer1(pImage,&lPointer,lpcsType,&width,&height);
    }


    //创建文件头
    BYTE tmp[sizeof(BITMAPINFO)+1024];
    BITMAPINFO *bmi = (BITMAPINFO*)tmp;
    HBITMAP hBmp;

    memset(bmi,0,sizeof(BITMAPINFO));
    bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi->bmiHeader.biWidth = width;
    bmi->bmiHeader.biHeight = -height; //正负可以上下翻转图像
    bmi->bmiHeader.biPlanes = 1;
    bmi->bmiHeader.biBitCount = 8*channels[0].L();
    bmi->bmiHeader.biCompression = BI_RGB;
    bmi->bmiHeader.biSizeImage = 0;        //if biCompression is BI_RGB,this can be 0
    bmi->bmiHeader.biClrImportant =0 ;
    //初始化数据
    switch(8*channels[0].L()) 
    { 
    case 8 : 
        for(int i=0 ; i < 256 ; i++){ 
            bmi->bmiColors[i].rgbBlue = i;
            bmi->bmiColors[i].rgbGreen= i;
            bmi->bmiColors[i].rgbRed= i;
        }
        break;
    case 32:
    case 24: 
        ((DWORD*) bmi->bmiColors)[0] = 0x00FF0000;
        ((DWORD*) bmi->bmiColors)[1] = 0x0000FF00;
        ((DWORD*) bmi->bmiColors)[2] = 0x000000FF;
        break; 
    } 
    hBmp = ::CreateDIBSection(NULL,bmi,DIB_RGB_COLORS,NULL,0,0);

    //Halcon转换成BYTE数据
	BYTE *pData = new BYTE[width[0].L()*height[0].L()*channels[0].L()];
    long byteStep = 0;
    if (channels == 3)
    {
        byteStep = 3*width;
        BYTE *pHimageR = (BYTE*)lPointerR.SArr();
        BYTE *pHimageG = (BYTE*)lPointerG.SArr();
        BYTE *pHimageB = (BYTE*)lPointerB.SArr();
        for (int i=0; i<height; ++i)
        {
            for (int j=0; j<width; ++j)
            {
                *(pData + i*byteStep + 3*j + 0) = *pHimageB;
                *(pData + i*byteStep + 3*j + 1) = *pHimageG;
                *(pData + i*byteStep + 3*j + 2) = *pHimageR;
                pHimageR++;
                pHimageG++;
                pHimageB++;
            }
        }
    }else
    {
        byteStep = width;
        BYTE *pHimage = (BYTE*)lPointer[0].L();
        for (int i=0; i<height[0].L(); ++i)
        {
            for (int j=0; j<width[0].L(); ++j)
            {
                *(pData + i*byteStep + j) = *pHimage;
                pHimage++;
            }
        }
    }

    //BYTE数据拷贝
    SetDIBits(NULL,hBmp,0,height[0].L(),pData,bmi,DIB_RGB_COLORS);

    //CBitmap关联HBITMAP
	if( wImage->m_hObject )
	{
		wImage->Detach();           //切断CWnd和窗口联系
	}
	wImage->Attach(hBmp);      //将句柄HBITMAP m_hBitmap与CBitmap m_bitmap关联

    delete [] pData;

    return;
}

CString CPublicTool::GetFilePath(CString name)
{
	WCHAR filepath[512];
	GetModuleFileName(NULL,filepath,512);
	CString fileroot(filepath);
	fileroot = fileroot.Left(fileroot.ReverseFind('\\')+1);
	fileroot += name;
	//return fileroot;
	//为适应原软件方程式路径设计
	return name;
}

CString CPublicTool::GetTheDisk()
{
	TCHAR szBuf[64] = {0};
	CString theDisk("C:\\");
	if( GetLogicalDriveStrings(64, szBuf))
	{
		TCHAR* pTmp = szBuf;
		do
		{
			if( DRIVE_FIXED == GetDriveType(pTmp) )
			{
				theDisk = pTmp;
			}
			else
			{
				break;
			}
			pTmp = _tcschr(pTmp, _T('\0'));
		}
		while (*(++pTmp));
	}
	return theDisk;
}

void CPublicTool::DeleteDirectory(CString directory_path,int days)  
{     
	CFileFind finder;  
	CString path;  

	path = directory_path + L"/*.*";
	BOOL bWorking = finder.FindFile(path);  
	while(bWorking){  
		bWorking = finder.FindNextFile();  
		CTime fTime;
		CTime time = CTime::GetCurrentTime() - CTimeSpan( days, 0, 0, 0 );
		finder.GetCreationTime(fTime);
		if(fTime<time)
		{
			if(finder.IsDirectory() && !finder.IsDots()){//处理文件夹  
				DeleteDirectory(finder.GetFilePath(), days); //递归删除文件夹  
				RemoveDirectory(finder.GetFilePath());  
			}  
			else{//处理文件  
				DeleteFileW(finder.GetFilePath());  
			}  
		}
	}  
}  

DWORD CPublicTool::GetProcessIDFromName(const WCHAR * name)
{
	HANDLE snapshot;
	PROCESSENTRY32 processinfo;
	processinfo.dwSize = sizeof(processinfo);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(snapshot == NULL)
		return FALSE;

	BOOL status = Process32First(snapshot, &processinfo);
	while(status)
	{
		if(_wcsicmp(name,processinfo.szExeFile)==0)
			return processinfo.th32ProcessID;
		status = Process32Next(snapshot,&processinfo);
	}
	return -1;
}

//判断字符串是否以数字开头
bool CPublicTool::IsCStringNumStart(CString cstr)
{
	char *cstrChar = MyW2A(cstr);
	if(cstrChar[0] >= '0' && cstrChar[0] <= '9')
		return true;
	return false;
}
