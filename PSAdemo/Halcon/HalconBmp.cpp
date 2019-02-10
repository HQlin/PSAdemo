#include "StdAfx.h"
#include "HalconBmp.h"


HalconBmp::HalconBmp(vector<char*> dir)
{
	try{
		hv_ImageFilesVector.clear();
		hv_IndexVector.clear();
		int index = 0;
		for(int i=0;i<dir.size();i++)
		{	
			CString charstrPath(dir[i]);
			if(CheckFolderExist(charstrPath))
			{
				HTuple  hv_ImageFiles;
				ListFiles(dir[i], ((HTuple("files").Append("follow_links")).Append("recursive")), &hv_ImageFiles);
				TupleRegexpSelect(hv_ImageFiles, (HTuple("\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima)$").Append("ignore_case")), &hv_ImageFiles);
				hv_ImageFilesVector.push_back(hv_ImageFiles);
				hv_IndexVector.push_back(index);
			}
			else
			{
				break;
			}
		}
	}catch(...)
	{
	}
}


HalconBmp::~HalconBmp(void)
{
}

bool HalconBmp::GetPic(int type, HObject &ho_Image, CString &path)
{
	try{
		ReadImage(&ho_Image, HTuple(hv_ImageFilesVector[type][hv_IndexVector[type]]));
		string  str = hv_ImageFilesVector[type][hv_IndexVector[type]].S();
		path = str.c_str();
		hv_IndexVector[type]++;
		//判断循环
		if(hv_ImageFilesVector[type].TupleLength() == hv_IndexVector[type])
			hv_IndexVector[type] = 0;
	}catch(...)
	{
		AfxMessageBox(L"读取图片失败！检查文件夹路径！");
	}
	return true;
}

///目录是否存在的检查：
bool  HalconBmp::CheckFolderExist(CString charstrPath)
{
    WIN32_FIND_DATA  wfd;
    bool rValue = false;
    HANDLE hFind = FindFirstFile(charstrPath, &wfd);
    if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        rValue = true;   
    }
    FindClose(hFind);
    return rValue;
}