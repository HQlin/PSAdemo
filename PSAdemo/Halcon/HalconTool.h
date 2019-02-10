#pragma once
#include "HalconCpp.h"
#include <vector>

using namespace std;
using namespace HalconCpp;

class HalconTool
{
public:	
	HTuple hv_WindowHandle;
	HalconTool(void);
	~HalconTool(void);

	/************************************************
	*   名称：CloseCamera
	*   功能：将拍到的图像或者HObject类型的图案，显示在绑定的窗口上
	*	参数：
	ho_Image：显示的图像

	*	返回值：空
	* 	修改日期：2017年7月5日
	************************************************/
	void ShowImage(HObject& ho_Image);

	/************************************************
	*   名称：SetWindow
	*   功能：设置窗口句柄为显示图像准备
	*	参数：
	pCWnd：窗口句柄数组

	*	返回值：空
	* 	修改日期：2017年7月5日
	************************************************/
	void SetWindow(CWnd * pCWnd);

	HTuple WindowHandle0, WindowHandle1;
	void SetWindow(CWnd * pCWnd0, CWnd * pCWnd1);
	void ShowLinearImage(const HObject& ho_Image);

	//在窗口显示文字
	void DisplayMessage(HTuple hv_WindowHandle, CString cString, HTuple hv_CoordSystem = "window", 
		HTuple hv_Row = 12, HTuple hv_Column = 12, HTuple hv_Color = "green", HTuple hv_Box = "false");								//在窗口显示文字

	//算法返回轮廓
	vector<HObject> outlines;
	//模拟底板轮廓
	vector<HObject> downlines;
	//模拟贴片轮廓
	vector<HObject> uplines;
	//显示文字
	CString strReslut;
	//当前图像比率
	double m_scale;
	//当前图像偏移
	double m_X;
	double m_Y;

	double PartRow0;
	double PartCol0;
	double PartRow1;
	double PartCol1;

};

