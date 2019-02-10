#pragma once

//移植需导入全局变量声明h文件
#include "../PSAdemo.h"

//1#部署说明
//文件：PSAdemo.h
//1.全局变量声明
//extern HalconBmp *g_pBmp;								//读图工具
//extern HalconTool g_ToolDown;							//显示图像工具
//extern bool isRunPic;									//是否跑图
//extern int g_SaveImage;								//识别是否保存图像(D:/图像备份/)
//extern CalgorithmRegion *g_pCalgorithmRegion;			//轮廓算法工具


//文件：PSAdemo.cpp
//2.全局变量初始化
//CalgorithmRegion *g_pCalgorithmRegion;			//轮廓算法工具
//HalconBmp *g_pBmp;								//读图工具
//HalconTool g_ToolDown;							//显示图像工具
////true 开发跑图模式
////false 工控机模式
//bool isRunPic = true;								//是否跑图
//int g_SaveImage = 0;								//识别是否保存图像


//文件：PSAdemoDlg.h
//3.类成员变量
//模板参数
//CalgorithmRegion::ModelParameter DownParameter;


//文件：PSAdemoDlg.cpp

//2#开发环境
//Halcon11.0.1(win32) + vs2010

//3#工程结构
/*
--Halcon
	|-CalgorithmRegion	算法类
	|-HalconBmp			读图类
	|-HalconTool		显示图像文字类
--mfcDlg
	|-MassageDlg		提示对话框
	|-ModelBuildDlg		编辑模板对话框
	|-readme.h			工程使用说明
*/


//4#使用说明
/*
一.管理模板
1.所有模板：指显示已存在的模板。
2.当前模板：指当前操作模板。

二,获取图像
1.保存编辑模板的原图像。

三.编辑模板
1.增强图像（非必选）。
2.中值滤波（非必选）。
3.匹配方式（必选）：
	其他匹配方式都是在模板匹配基础上，进行指定标志模板的相对区域，对直线、矩形、圆弧的二次识别；
	所有匹配方式都基于一基准点与一基准线进行识别，并在底板图像与贴片图像上进行定位贴合（本工程以编辑底板模板为例，贴片相同）。
4.匹配参数（模板匹配）
4.1搜索范围：指在图像中固定的矩形区域进行识别模板，一般为全图范围。
4.2选择模板：指被识别的标志模板（如果匹配方式为模板匹配，则需要勾选角度，使得标志模板具有角度属性；如果不勾选角度，标志模板都为零度）。
4.3擦除：指对标志模板进行编辑，可擦除掉非标志模板部分（即擦除部分不进行识别），滑动条可控制橡皮擦大小。
4.4开始角度、角度范围：指识别模板的角度范围（一般选择模板角度为中点，开始角度绝对值为角度范围的一半）。举例：选择模板为零度，开始角度为-10度，角度范围为20度，则识别模板角度范围为-10~10度，识别角度必须在该范围才算识别成功。
4.5合格分数：指识别模板分数，识别分数必须大于合格分数才算识别成功。
4.6保存模板：最后保存编辑模板所有数据，并显示原图像的识别效果。

四.识别模板
1.使用已保存的编辑模板，获取一图像识别模板（0K为识别成功，NG为识别失败）。
*/
