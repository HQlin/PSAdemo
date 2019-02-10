
// PSAdemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PSAdemo.h"
#include "PSAdemoDlg.h"
#include "afxdialogex.h"
#include "ModelBuildDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPSAdemoDlg 对话框




CPSAdemoDlg::CPSAdemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPSAdemoDlg::IDD, pParent)
	, m_AddName(_T(""))
	, m_CurModel(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPSAdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALL_MODEL, m_AllModel);
	DDX_Text(pDX, IDC_ADD_NAME, m_AddName);
	DDX_Text(pDX, IDC_CUR_MODEL, m_CurModel);
}

BEGIN_MESSAGE_MAP(CPSAdemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_GETPIC, &CPSAdemoDlg::OnBnClickedBtnGetpic)
	ON_BN_CLICKED(IDC_BTN_EDITMODEL, &CPSAdemoDlg::OnBnClickedBtnEditmodel)
	ON_BN_CLICKED(IDC_BTN_FINDMODEL, &CPSAdemoDlg::OnBnClickedBtnFindmodel)
	ON_CBN_SELCHANGE(IDC_ALL_MODEL, &CPSAdemoDlg::OnCbnSelchangeAllModel)
	ON_BN_CLICKED(IDC_BTN_ADD, &CPSAdemoDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CPSAdemoDlg::OnBnClickedBtnDelete)
END_MESSAGE_MAP()


// CPSAdemoDlg 消息处理程序

BOOL CPSAdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	//工具初始化
	g_pCalgorithmRegion = new CalgorithmRegion;
	vector<char*> dir;
	dir.push_back("../Pictures/Mark");
	//dir.push_back("../Pictures/1");
	//dir.push_back("../Pictures/2");
	//dir.push_back("../Pictures/3");
	//dir.push_back("../Pictures/4");
	g_pBmp = new HalconBmp(dir);

	//新建算法参数
	g_pCalgorithmRegion->AddPro(L"ProgramDoc\\lin");  
	//导入算法数据
	g_pCalgorithmRegion->SetPro(L"ProgramDoc\\lin");

	//所有底板模板
	m_AllModel.ResetContent();
	g_pCalgorithmRegion->GetDownModels(m_DownModels, DownParameter);
	for(int i = 0; i < m_DownModels.size(); i++)
	{
		m_AllModel.AddString(m_DownModels[i]);
	}
	m_AllModel.SetCurSel(0);
	//加载底板模板参数
	OnCbnSelchangeAllModel();

	g_ToolDown.SetWindow(GetDlgItem(IDC_STATIC_PIC));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPSAdemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPSAdemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPSAdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//获取图像逻辑
void CPSAdemoDlg::OnBnClickedBtnGetpic()
{
	//获取图像
	HObject image;

	if(isRunPic)
	{
		CString path;
		g_pBmp->GetPic(0, image, path);
	}
	else
	{
		//相机获取图像
	}

	USES_CONVERSION; 
	char* strChar = W2A(DownParameter.model_Path);
	try{
		WriteImage(image, "bmp", 0, strChar);
		g_ToolDown.ShowImage(image);
	}catch(...)
	{
		AfxMessageBox(L"获取图片失败！");
	}
}

//编辑模板逻辑
void CPSAdemoDlg::OnBnClickedBtnEditmodel()
{
	//编辑模板
	UpdateData(true);
	HObject image;

	USES_CONVERSION; 
	char* strChar = W2A(DownParameter.model_Path);
	try{
		ReadImage(&image, strChar);
	}catch(...)
	{
		AfxMessageBox(L"请先获取图像");
		return;
	}
	CModelBuildDlg dlg(image, 0, DownParameter);
	dlg.DoModal();
	//编辑模板后，必须加载一次重载数据
	if(!g_pCalgorithmRegion->SetDownModel(m_CurModel, DownParameter))
	{
		AfxMessageBox(L"加载底板模板失败");				
	}
}

//识别模板逻辑
void CPSAdemoDlg::OnBnClickedBtnFindmodel()
{
	//识别模板
	HObject image;

	if(isRunPic)
	{
		CString path;
		g_pBmp->GetPic(0, image, path);
	}
	else
	{
		//相机获取图像
	}

	vector<double> results;
	bool ret = g_pCalgorithmRegion->FindDownModel(g_ToolDown, image, results, DownParameter, g_SaveImage);
	if(isRunPic)
		g_ToolDown.ShowImage(image);

}


void CPSAdemoDlg::OnCbnSelchangeAllModel()
{
	CString strTemp;
	m_AllModel.GetWindowText(strTemp);
	//加载底板模板参数
	if(!g_pCalgorithmRegion->SetDownModel(strTemp, DownParameter))
	{
		AfxMessageBox(L"加载底板模板失败");				
	}
	else
	{
		m_CurModel = strTemp;
	}
	UpdateData(false);
}


void CPSAdemoDlg::OnBnClickedBtnAdd()
{
	//新添模板
	UpdateData(true);
	//新建底板模板
	if(!g_pCalgorithmRegion->AddDownModel(m_AddName, DownParameter))
	{
		AfxMessageBox(L"新建底板模板名称已存在");	
	}
		
	//所有底板模板
	m_AllModel.ResetContent();
	g_pCalgorithmRegion->GetDownModels(m_DownModels, DownParameter);
	for(int i = 0; i < m_DownModels.size(); i++)
	{
		m_AllModel.AddString(m_DownModels[i]);
	}
	m_AllModel.SetCurSel(0);
	//加载底板模板参数
	OnCbnSelchangeAllModel();
	m_AddName = L"";
	UpdateData(false);
}


void CPSAdemoDlg::OnBnClickedBtnDelete()
{
	//删除模板
	UpdateData(true);
	if(!g_pCalgorithmRegion->DeleteDownModel(m_CurModel, DownParameter))
	{
		AfxMessageBox(L"删除底板模板失败");	
	}

	//所有底板模板
	m_AllModel.ResetContent();
	g_pCalgorithmRegion->GetDownModels(m_DownModels, DownParameter);
	for(int i = 0; i < m_DownModels.size(); i++)
	{
		m_AllModel.AddString(m_DownModels[i]);
	}
	m_AllModel.SetCurSel(0);
	//加载底板模板参数
	OnCbnSelchangeAllModel();
}
