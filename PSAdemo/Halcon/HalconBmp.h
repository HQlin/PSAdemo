//循环文件夹读图
////初始化
//HalconBmp *bmp = NULL;
//vector<char*> dir;
//dir.push_back("G:/MCEQVIn0/2018-01-26/101");
//dir.push_back("G:/MCEQVIn0/2018-01-26/101");
//dir.push_back("G:/MCEQVIn0/2018-01-26/101");
//bmp = new HalconBmp(dir, 3);

////使用
//HObject ho_Image;
//bmp->GetPic(2, ho_Image);

////释放
//if(NULL != bmp)
//	delete bmp;
#pragma once
#include<vector>
#include "HalconCpp.h"

using namespace std;
using namespace HalconCpp;

class HalconBmp
{
public:
	HalconBmp(vector<char*> dir);
	~HalconBmp(void);

	vector<HTuple> hv_ImageFilesVector;					//文件夹数组
	vector<int>    hv_IndexVector;						//图片路径标识

	bool GetPic(int type, HObject &ho_Image, CString &path);			//获取某文件夹图片		
	bool  CheckFolderExist(CString charstrPath);						//目录是否存在的检查
};

