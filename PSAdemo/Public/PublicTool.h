/************************************************ 
*   版权：广州恺煜兴智能科技有限公司
* 	许可证：

* 	文件名称：PublicTool.h 
* 	修改日期：2017年07月05日  
* 	功能：公共静态函数类
************************************************/
#pragma once
#include "HalconCpp.h"
#include <vector>
#include <TlHelp32.h>

using namespace HalconCpp;
using namespace std;

class CPublicTool
{
public:
	CPublicTool(void);
	~CPublicTool(void);

	//MFC中几种常用的字符串分割方法
	//https://blog.csdn.net/xiaominggunchuqu/article/details/50465020
	static vector<CString> SplitCString(CString strSource, char ch);

	/************************************************
	*   名称：MyW2A
	*   功能：转换宽字节CString类型为char类型，解决W2A()在try语句段中无法使用的问题（注意使用完char*需要释放内存）
	*	参数：
	str：CString类型变量

	*	返回值：
	char*（注意使用完char*需要释放内存）
	* 	修改日期：2017年7月5日
	************************************************/
	static char* MyW2A(const CString& str);

	/************************************************
	*   名称：HImage2CBitmap
	*   功能：HObject 转 CBitmap
	*	参数：
	pImage：HImage类型变量
	wImage：CBitmap指针变量

	*	返回值：空
	* 	修改日期：2017年7月5日
	************************************************/
	static void HImage2CBitmap(HObject pImage,CBitmap *wImage);

	/************************************************
	*   名称：GetFilePath
	*   功能：获取工程文件的绝对路径
	*	参数：
	name：工程中某个文件名或相对路径

	*	返回值：绝对路径
	* 	修改日期：2017年7月5日
	************************************************/
	static CString GetFilePath(CString name);		

	/************************************************
	*   名称：GetFilePath
	*   功能：找到D盘之后第一个硬盘盘符，一般为E盘；若只存在两个盘则返回D盘，只存在一个则返回C盘
	*	参数：空

	*	返回值：绝对路径
	* 	修改日期：2017年7月5日
	************************************************/
	static CString GetTheDisk();

	/************************************************
	*   名称：DeleteDirectory
	*   功能：删除一个文件夹下几天前保存的所有内容
	*	参数：
	directory_path：删除文件夹中的所有内容
	days：天数

	*	返回值：空
	* 	修改日期：2017年7月5日
	************************************************/
	static void DeleteDirectory(CString directory_path, int days);	

	/************************************************
	*   名称：GetProcessIDFromName
	*   功能：根据进程名获取进程ID，用于关闭该软件已经打开的一个示例
	*	参数：
	name：程序exe的名称(包括后缀)

	*	返回值：
	0：没有该程序exe运行
	-1：函数执行失败
	其它：运行程序exe的ID
	* 	修改日期：2017年7月5日
	************************************************/
	static DWORD GetProcessIDFromName(const WCHAR * name);

	//判断字符串是否以数字开头
	static bool IsCStringNumStart(CString cstr);

	//复制文件或文件夹  
	static bool CopyFile(CString pTo,CString pFrom);
};

