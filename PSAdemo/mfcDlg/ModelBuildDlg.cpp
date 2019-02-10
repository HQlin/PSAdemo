// ModelBuildDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ModelBuildDlg.h"
#include "afxdialogex.h"
#include "MassageDlg.h"

// CModelBuildDlg 对话框

IMPLEMENT_DYNAMIC(CModelBuildDlg, CDialogEx)

CModelBuildDlg::CModelBuildDlg(HObject &scrImage, int type, 
	CalgorithmRegion::ModelParameter &Parameter, CWnd* pParent /*=NULL*/)
	: CDialogEx(CModelBuildDlg::IDD, pParent)
	, m_AStart(0)
	, m_ARange(0)
	, m_Score(0)
	, m_IsMultImage(FALSE)
	, m_Mult(0)
	, m_Add(0)
	, m_MouseDown(false)
	, m_ModelCheck(0)
	, m_isRubber(FALSE)
	, m_UseRect2(FALSE)
	, m_median(0)
	, m_UseMedian(FALSE)
{
	this->type = type;
	this->Parameter = Parameter;
	this->scrImage = scrImage;
}

CModelBuildDlg::~CModelBuildDlg()
{
}

void CModelBuildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ASTART, m_AStart);
	DDX_Text(pDX, IDC_ARANGE, m_ARange);
	DDX_Text(pDX, IDC_SCORE, m_Score);
	DDX_Check(pDX, IDC_CHECK_MULT, m_IsMultImage);
	DDX_Text(pDX, IDC_EDIT_MULT, m_Mult);
	DDX_Text(pDX, IDC_EDIT_ADD, m_Add);
	DDX_Control(pDX, IDC_SLIDER_MULT, m_MultSc);
	DDX_Control(pDX, IDC_SLIDER_ADD, m_AddSc);
	DDX_Control(pDX, IDC_COMBO_LINE, m_Line);
	DDX_Control(pDX, IDC_COMBO_CHECK, m_check);
	DDX_Control(pDX, IDC_COMBO_C, m_Point);
	DDX_Radio(pDX, IDC_MODLE, m_ModelCheck);
	DDX_Control(pDX, IDC_SLIDER_EDIT, m_RubberSize);
	DDX_Check(pDX, IDC_CHECK_EDIT, m_isRubber);
	DDX_Check(pDX, IDC_CHECK_RECT2, m_UseRect2);
	DDX_Text(pDX, IDC_EDIT_MEDIAN, m_median);
	DDX_Control(pDX, IDC_SLIDER_MEDIAN, m_MedianSc);
	DDX_Check(pDX, IDC_CHECK_MEDIAN, m_UseMedian);
}


BEGIN_MESSAGE_MAP(CModelBuildDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_RECT, &CModelBuildDlg::OnBnClickedBtnRect)
	ON_BN_CLICKED(IDC_BTN_MODEL, &CModelBuildDlg::OnBnClickedBtnModel)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CModelBuildDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_CHECK_MULT, &CModelBuildDlg::OnBnClickedCheckMult)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MULT, &CModelBuildDlg::OnNMCustomdrawSliderMult)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ADD, &CModelBuildDlg::OnNMCustomdrawSliderAdd)
	ON_BN_CLICKED(IDC_BTN_LINE1, &CModelBuildDlg::OnBnClickedBtnLine1)
	ON_BN_CLICKED(IDC_BTN_LINE2, &CModelBuildDlg::OnBnClickedBtnLine2)
	ON_CBN_SELCHANGE(IDC_COMBO_CHECK, &CModelBuildDlg::OnCbnSelchangeComboCheck)
	ON_BN_CLICKED(IDC_BTN_C1, &CModelBuildDlg::OnBnClickedBtnC1)
	ON_BN_CLICKED(IDC_BTN_C2, &CModelBuildDlg::OnBnClickedBtnC2)
	ON_BN_CLICKED(IDC_BTN_RECT1, &CModelBuildDlg::OnBnClickedBtnRect1)
	ON_BN_CLICKED(IDC_MODLE, &CModelBuildDlg::OnBnClickedModle)
	ON_BN_CLICKED(IDC_LINES, &CModelBuildDlg::OnBnClickedLines)
	ON_BN_CLICKED(IDC_CIR, &CModelBuildDlg::OnBnClickedCir)
	ON_BN_CLICKED(IDC_RECT, &CModelBuildDlg::OnBnClickedRect)
	ON_BN_CLICKED(IDC_CHECK_EDIT, &CModelBuildDlg::OnBnClickedCheckEdit)
	ON_BN_CLICKED(IDC_CENTER, &CModelBuildDlg::OnBnClickedCenter)
	ON_BN_CLICKED(IDC_BTN_TEST, &CModelBuildDlg::OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_SIGLE_CIR, &CModelBuildDlg::OnBnClickedSigleCir)
	ON_BN_CLICKED(IDC_BTN_LINE, &CModelBuildDlg::OnBnClickedBtnLine)
	ON_BN_CLICKED(IDC_SIGLE_LINE, &CModelBuildDlg::OnBnClickedSigleLine)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MEDIAN, &CModelBuildDlg::OnNMCustomdrawSliderMedian)
	ON_BN_CLICKED(IDC_CHECK_MEDIAN, &CModelBuildDlg::OnBnClickedCheckMedian)
	ON_BN_CLICKED(IDC_BTN_C, &CModelBuildDlg::OnBnClickedBtnC)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT_MULT, &CModelBuildDlg::OnEnChangeEditMult)
END_MESSAGE_MAP()


// CModelBuildDlg 消息处理程序


void CModelBuildDlg::OnBnClickedBtnRect()
{
	//搜索范围
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_RECT, butCStr);
	if(L"点击右键确定" != butCStr)
	{
		SetDlgItemText(IDC_BTN_RECT, L"点击右键确定");
		switch(type)
		{
		case 0:
			//底板
			g_pCalgorithmRegion->SetDownRect(ToolModel, scrImage, Parameter);
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->SetUpRect(ToolModel, scrImage, Parameter);
			break;
		}
		SetDlgItemText(IDC_BTN_RECT, L"搜索范围");
	}
}


void CModelBuildDlg::OnBnClickedBtnModel()
{
	UpdateData(true);
	//选择模板
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_MODEL, butCStr);
	if(L"点击右键确定" != butCStr)
	{
		SetDlgItemText(IDC_BTN_MODEL, L"点击右键确定");
		switch(type)
		{
		case 0:
			//底板
			g_pCalgorithmRegion->BuildDownModel(ToolModel, scrImage,
				m_UseRect2, Parameter);
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->BuildUpModel(ToolModel, scrImage,
				m_UseRect2, Parameter);
			break;
		}	
		SetDlgItemText(IDC_BTN_MODEL, L"选择模板");
	}
	//当前模板
	switch(type)
	{
	case 0:
		//底板		
		g_pCalgorithmRegion->RestoreDownModel(ToolModel, scrImage, 
			ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld, 0,
			Parameter);
		break;
	case 1:
		//贴片
		g_pCalgorithmRegion->RestoreUpModel(ToolModel, scrImage, 
			ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld, 0,
			Parameter);
		break;
	}
}


void CModelBuildDlg::OnBnClickedBtnSave()
{
	//图像预处理
	OnBnClickedCheckMult();
	//匹配方式
	OnBnClickedModle();

	//擦除
	UpdateData(true);
	if(m_isRubber)
	{
		m_isRubber = false;
		UpdateData(false);
		OnBnClickedCheckEdit();
	}

	//保存模板
	switch(type)
	{
	case 0:
		//底板		
		g_pCalgorithmRegion->SaveDownParameter(Parameter);
		break;
	case 1:
		//贴片
		g_pCalgorithmRegion->SaveUpParameter(Parameter);
		break;
	}	

	//识别测试
	vector<double> results;
	bool ret = false;
	switch(type)
	{
	case 0:
		{
			//底板		
			g_pCalgorithmRegion->SetDownModel(Parameter.model_Name, Parameter);
			ret = g_pCalgorithmRegion->FindDownModel(ToolModel, scrImage, results, Parameter, g_SaveImage);
		}
		break;
	case 1:
		{
			//贴片
			g_pCalgorithmRegion->SetUpModel(Parameter.model_Name, Parameter);
			ret = g_pCalgorithmRegion->FindUpModel(ToolModel, scrImage, results, Parameter, g_SaveImage);
		}
		break;
	}	
	ToolModel.ShowImage(scrImage);
	if(!ret)
	{
		AfxMessageBox(L"模板保存失败，请按步骤重新编辑模板"); 
	}
}


BOOL CModelBuildDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//标题
	SetWindowTextW(L"编辑模板："+Parameter.model_Name);

	// TODO:  在此添加额外的初始化
	ToolModel.SetWindow(GetDlgItem(IDC_STATIC_MODELVIEW));

	GetDlgItem(IDC_EDIT_MULT)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_ADD)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_MEDIAN)->EnableWindow(false);

	//基准线初始化
	m_Line.InsertString(0, L"直线1");
	m_Line.InsertString(1, L"直线2");

	//基准点初始化
	m_Point.InsertString(0, L"圆弧1");
	m_Point.InsertString(1, L"圆弧2");
	
	//识别方式
	m_check.InsertString(0, L"模板匹配");
	m_check.InsertString(1, L"直线匹配");
	m_check.InsertString(2, L"圆弧匹配");
	m_check.InsertString(3, L"矩形匹配");
	m_check.InsertString(4, L"实心圆匹配");

	switch(type)
	{
	case 0:
		//底板
		m_AStart = (double)Parameter.hv_AStart;
		m_ARange = (double)Parameter.hv_ARange;
		m_Score = (double)Parameter.hv_Score;

		m_Mult = (double)Parameter.hv_Mult;
		m_Add = (double)Parameter.hv_Add;
		m_IsMultImage = Parameter.is_Mult;

		m_median = Parameter.hv_Parameter_median;
		m_UseMedian = Parameter.hv_median_bool;

		m_Line.SetCurSel(Parameter.linesParameter.baseline);
		m_Point.SetCurSel(Parameter.circlesParameter.basepoint);
		if(L"" == Parameter.check)
		{
			m_ModelCheck = 0;
			m_check.SetCurSel(m_ModelCheck);
		}
		else if(L"Lines" == Parameter.check)
		{
			m_ModelCheck = 1;
			m_check.SetCurSel(m_ModelCheck);
		}
		else if(L"Circles" == Parameter.check)
		{
			m_ModelCheck = 2;
			m_check.SetCurSel(m_ModelCheck);
		}
		else if(L"Rects" == Parameter.check)
		{
			m_ModelCheck = 3;
			m_check.SetCurSel(m_ModelCheck);
		}
		else if(L"SolidCircle" == Parameter.check)
		{
			m_ModelCheck = 4;
			m_check.SetCurSel(m_ModelCheck);
		}
		else if(L"Line" == Parameter.check)
		{
			m_ModelCheck = 5;
			m_check.SetCurSel(m_ModelCheck);
		}
		break;
	case 1:
		//贴片
		m_AStart = (double)Parameter.hv_AStart;
		m_ARange = (double)Parameter.hv_ARange;
		m_Score = (double)Parameter.hv_Score;

		m_Mult = (double)Parameter.hv_Mult;
		m_Add = (double)Parameter.hv_Add;
		m_IsMultImage = Parameter.is_Mult;

		m_median = Parameter.hv_Parameter_median;
		m_UseMedian = Parameter.hv_median_bool;

		m_Line.SetCurSel(Parameter.linesParameter.baseline);
		m_Point.SetCurSel(Parameter.circlesParameter.basepoint);
		if(L"" == Parameter.check)
		{
			m_ModelCheck = 0;
			m_check.SetCurSel(m_ModelCheck);
		}
		else if(L"Lines" == Parameter.check)
		{
			m_ModelCheck = 1;
			m_check.SetCurSel(m_ModelCheck);
		}
		else if(L"Circles" == Parameter.check)
		{
			m_ModelCheck = 2;
			m_check.SetCurSel(m_ModelCheck);
		}
		else if(L"Rects" == Parameter.check)
		{
			m_ModelCheck = 3;
			m_check.SetCurSel(m_ModelCheck);
		}		
		else if(L"SolidCircle" == Parameter.check)
		{
			m_ModelCheck = 4;
			m_check.SetCurSel(m_ModelCheck);
		}
		break;
	}	

	//橡皮檫大小
	m_RubberSize.SetRange(10,50,TRUE);	
	m_RubberSize.SetPos(25);

	UpdateData(false);

	//图像对比度初始化
	//Mult:0.01~0.1
	//Add：-2000~0
	m_MultSc.SetRange(1,10,TRUE);	
	m_MultSc.SetPos(m_Mult*100);
	m_AddSc.SetRange(-2000,0,TRUE);	
	m_AddSc.SetPos(m_Add);

	//中值滤波器
	m_MedianSc.SetRange(1,20,TRUE);	
	m_MedianSc.SetPos(m_median);

	//图像显示大小
	CRect rect0;
	GetDlgItem(IDC_STATIC_MODELVIEW)->GetWindowRect(rect0);
	CRect rect1;
	rect1.SetRect(0, 0, rect0.Height(), rect0.Height());
	GetDlgItem(IDC_STATIC_MODELVIEW)->MoveWindow(rect1, TRUE);

	OnBnClickedCheckMult();
	OnBnClickedModle();

	//当前模板
	switch(type)
	{
	case 0:
		//底板		
		g_pCalgorithmRegion->RestoreDownModel(ToolModel, scrImage, 
			ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld, 0,
			Parameter);
		break;
	case 1:
		//贴片
		g_pCalgorithmRegion->RestoreUpModel(ToolModel, scrImage, 
			ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld, 0,
			Parameter);
		break;
	}

	//显示跑图测试按钮
	if(isRunPic)
		GetDlgItem(IDC_BTN_TEST)->ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CModelBuildDlg::OnBnClickedCheckMult()
{
	//是否增强图像对比
	UpdateData(true);
	
	Parameter.hv_Mult = m_Mult;
	Parameter.hv_Add = m_Add;
	Parameter.is_Mult = m_IsMultImage;
	Parameter.hv_Parameter_median = m_median;
	Parameter.hv_median_bool = m_UseMedian;

	HObject dstImage;	
	switch(type)
	{
	case 0:
		//底板
		dstImage = g_pCalgorithmRegion->SetPretreatImage(ToolModel, scrImage,
			g_pCalgorithmRegion->DownSetting, Parameter, 
			Parameter.model_Name);
		break;
	case 1:
		//贴片
		dstImage = g_pCalgorithmRegion->SetPretreatImage(ToolModel, scrImage,
			g_pCalgorithmRegion->UpSetting, Parameter, 
			Parameter.model_Name);
		break;
	}
	ToolModel.ShowImage(dstImage);
}


void CModelBuildDlg::OnNMCustomdrawSliderMult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	//mult
	m_Mult = m_MultSc.GetPos()/100.0;
	UpdateData(false);
	OnBnClickedCheckMult();
	*pResult = 0;
}


void CModelBuildDlg::OnNMCustomdrawSliderAdd(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	//add
	m_Add = m_AddSc.GetPos();
	UpdateData(false);
	OnBnClickedCheckMult();
	*pResult = 0;
}

BOOL CModelBuildDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg ->message == WM_KEYDOWN)  // If a keydown message
	{
		double oldX = ToolModel.m_X;
		double oldY = ToolModel.m_Y;

		if(VK_LEFT==pMsg ->wParam )
		{
			ToolModel.m_X = oldX - 20;
		}
		else if(VK_DOWN==pMsg ->wParam)
		{
			ToolModel.m_Y = oldY + 20;
		}
		else if(VK_UP==pMsg ->wParam)
		{
			ToolModel.m_Y = oldY - 20;
		}
		else if(VK_RIGHT==pMsg ->wParam)
		{
			ToolModel.m_X = oldX + 20;
		}
		OnBnClickedCheckMult();
	}

	//鼠标左键弹按下
	if (pMsg->message == WM_LBUTTONDOWN)  
    {  
		CRect rect0;
		GetDlgItem(IDC_STATIC_MODELVIEW)->GetWindowRect(rect0);
		CPoint  pt;
        GetCursorPos(&pt);	
		if (rect0.PtInRect(pt))
		{
			//鼠标移动到指定窗口内		
			UpdateData(true);
			if(m_isRubber)
			{
				//橡皮檫
				double dx = pt.x-rect0.left;
				double dy = pt.y-rect0.top;
				ViewToImage(dx, dy);
				HObject rubber;
				GenCircle(&rubber, dy, dx, m_RubberSize.GetPos());		
				Union2(rubber, ho_RubberRectangle, &ho_RubberRectangle);

				//擦除模板
				g_pCalgorithmRegion->EditModel(ToolModel, scrImage, 
					ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld);
			}
			else
			{
				//鼠标按下
				m_MouseDown = true;
				//保存位置
				origin = pMsg->pt;
			}
			return true;
		}
    }  
    //鼠标左键弹起  
    else if (pMsg->message == WM_LBUTTONUP)  
    {  
		m_MouseDown = false;
    }  
	//鼠标移动
	else if (pMsg->message == WM_MOUSEMOVE)  
	{  
		int zDelta= pMsg->wParam;
		CRect rect0;
		GetDlgItem(IDC_STATIC_MODELVIEW)->GetWindowRect(rect0);
		CPoint  pt;
        GetCursorPos(&pt);	
		if (rect0.PtInRect(pt))
        {
			//鼠标移动到指定窗口内		
			UpdateData(true);
			if(m_isRubber)
			{
				//橡皮檫
				double dx = pt.x-rect0.left;
				double dy = pt.y-rect0.top;
				ViewToImage(dx, dy);
				HObject rubber;
				GenCircle(&rubber, dy, dx, m_RubberSize.GetPos());
				ToolModel.downlines.clear();
				ToolModel.downlines.push_back(rubber);			
				OnBnClickedCheckMult();
			}
			else
			{
				//移动图像
				if(m_MouseDown)
				{
					double oldX = ToolModel.m_X;
					double oldY = ToolModel.m_Y;

					if(pMsg->pt.x > origin.x && (pMsg->pt.x - origin.x)>5)
					{
						ToolModel.m_X = oldX - 5;

						origin.x = pMsg->pt.x ;
					}
					else if(pMsg->pt.x < origin.x && (origin.x - pMsg->pt.x)>5)
					{
						ToolModel.m_X = oldX + 5;

						origin.x = pMsg->pt.x ;
					}

					if(pMsg->pt.y > origin.y&& (pMsg->pt.y - origin.y)>5)
					{
						/*ToolModel.m_Y = oldY - 0.3*(point.y - origin.y);*/
						ToolModel.m_Y = oldY - 5;

						origin.y = pMsg->pt.y;
					}
					else if(pMsg->pt.y < origin.y&& (origin.y - pMsg->pt.y)>5)
					{
						/*ToolModel.m_Y = oldY + 0.3*(origin.y - point.y);*/
						ToolModel.m_Y = oldY + 5;

						origin.y = pMsg->pt.y;
					}
					OnBnClickedCheckMult();
				}
			}
			return true;
        }
	}  
	//鼠标中键滚动
	else if (pMsg->message == WM_MOUSEWHEEL)  
	{
		int zDelta= pMsg->wParam;
		CRect rect0;
		GetDlgItem(IDC_STATIC_MODELVIEW)->GetWindowRect(rect0);

		//鼠标移动到指定窗口内
		if((rect0.top < pMsg->pt.y && rect0.bottom > pMsg->pt.y) && (rect0.left < pMsg->pt.x && rect0.right > pMsg->pt.x))
		{
			double oldscale = ToolModel.m_scale;
			if(zDelta > 0)
			{
				ToolModel.m_scale = oldscale * 1.25;			
			}
			else if(zDelta < 0)
			{
				ToolModel.m_scale = oldscale * 0.75;
			}
			OnBnClickedCheckMult();
		}
	}	
	//return CDialogEx::PreTranslateMessage(pMsg);
	return false;
}


void CModelBuildDlg::OnBnClickedBtnLine1()
{
	//直线1区域
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_LINE1, butCStr);
	if(L"点击右键确定" != butCStr)
	{
		SetDlgItemText(IDC_BTN_LINE1, L"点击右键确定");
		switch(type)
		{
		case 0:
			//底板
			g_pCalgorithmRegion->SetDownLinesRect(ToolModel, scrImage, 1, Parameter);			
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->SetUpLinesRect(ToolModel, scrImage, 1, Parameter);
			break;
		}	
		SetDlgItemText(IDC_BTN_LINE1, L"直线1区域");
	}
}


void CModelBuildDlg::OnBnClickedBtnLine2()
{
	//直线2区域
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_LINE2, butCStr);
	if(L"点击右键确定" != butCStr)
	{
		SetDlgItemText(IDC_BTN_LINE2, L"点击右键确定");
		switch(type)
		{
		case 0:
			//底板
			g_pCalgorithmRegion->SetDownLinesRect(ToolModel, scrImage, 2, Parameter);			
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->SetUpLinesRect(ToolModel, scrImage, 2, Parameter);
			break;
		}	
		SetDlgItemText(IDC_BTN_LINE2, L"直线2区域");
	}
}


void CModelBuildDlg::OnCbnSelchangeComboCheck()
{
	//识别方式
	UpdateData(true);		
	CString check;
	switch(m_check.GetCurSel())
	{
	case 0:
		check = L"";	
		//直线
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_HIDE);
		//圆弧
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_HIDE);

		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_HIDE);
		break;
	case 1:
		check = L"Lines";	
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_HIDE);
				
		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BASE)->SetWindowText(L"基准线");
				
		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_HIDE);
		break;
	case 2:
		check = L"Circles";	
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BASE)->SetWindowText(L"基准点");
				
		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_HIDE);
		break;
	case 3:
		check = L"Rects";	
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_HIDE);
				
		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_SHOW);
		break;
	}
	Parameter.hv_AStart = m_AStart;
	Parameter.hv_ARange = m_ARange;
	Parameter.hv_Score = m_Score;
	Parameter.linesParameter.baseline = m_Line.GetCurSel();
	Parameter.circlesParameter.basepoint = m_Point.GetCurSel();
	Parameter.check = check;
}


void CModelBuildDlg::OnBnClickedBtnC1()
{
	//圆弧1区域
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_C1, butCStr);
	if(L"点击右键确定" != butCStr)
	{
		SetDlgItemText(IDC_BTN_C1, L"点击右键确定");
		switch(type)
		{
		case 0:
			//底板
			g_pCalgorithmRegion->SetDownCirclesRect(ToolModel, scrImage, 1, Parameter);			
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->SetUpCirclesRect(ToolModel, scrImage, 1, Parameter);
			break;
		}	
		SetDlgItemText(IDC_BTN_C1, L"圆弧1区域");
	}
}


void CModelBuildDlg::OnBnClickedBtnC2()
{
	//圆弧2区域
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_C2, butCStr);
	if(L"点击右键确定" != butCStr)
	{
		SetDlgItemText(IDC_BTN_C2, L"点击右键确定");
		switch(type)
		{
		case 0:
			//底板
			g_pCalgorithmRegion->SetDownCirclesRect(ToolModel, scrImage, 2, Parameter);			
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->SetUpCirclesRect(ToolModel, scrImage, 2, Parameter);
			break;
		}	
		SetDlgItemText(IDC_BTN_C2, L"圆弧2区域");
	}
}


void CModelBuildDlg::OnBnClickedBtnRect1()
{
	//矩形区域
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_RECT1, butCStr);
	if(L"点击右键确定" != butCStr)
	{
		SetDlgItemText(IDC_BTN_RECT1, L"点击右键确定");
		switch(type)
		{
		case 0:
			//底板
			g_pCalgorithmRegion->SetDownRectsRect(ToolModel, scrImage, Parameter);			
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->SetUpRectsRect(ToolModel, scrImage, Parameter);
			break;
		}	
		SetDlgItemText(IDC_BTN_RECT1, L"矩形区域");
	}
}

void CModelBuildDlg::OnBnClickedBtnLine()
{
	//单直线区域
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_LINE, butCStr);
	if(L"点击右键确定" != butCStr)
	{
		SetDlgItemText(IDC_BTN_LINE, L"点击右键确定");
		switch(type)
		{
		case 0:
			//底板
			g_pCalgorithmRegion->SetDownLineRect(ToolModel, scrImage, Parameter);			
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->SetUpLineRect(ToolModel, scrImage, Parameter);
			break;
		}	
		SetDlgItemText(IDC_BTN_LINE, L"直线区域");
	}
}


void CModelBuildDlg::OnBnClickedModle()
{
	//模板匹配
	UpdateData(true);
	CString check;
	switch(m_ModelCheck)
	{
	case 0:
		check = L"";	
		//模板
		GetDlgItem(IDC_BTN_MODEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_RECT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SLIDER_EDIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_EDIT)->ShowWindow(SW_SHOW);
		//直线
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_HIDE);
		//圆弧
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_HIDE);

		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_HIDE);
		//单直线
		GetDlgItem(IDC_BTN_LINE)->ShowWindow(SW_HIDE);
		//单圆弧
		GetDlgItem(IDC_BTN_C)->ShowWindow(SW_HIDE);
		break;
	case 1:
		check = L"Lines";	
		//模板
		GetDlgItem(IDC_BTN_MODEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_RECT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SLIDER_EDIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_EDIT)->ShowWindow(SW_SHOW);
		//直线
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BASE)->SetWindowText(L"基准线");

		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_HIDE);
		//单直线
		GetDlgItem(IDC_BTN_LINE)->ShowWindow(SW_HIDE);
		//单圆弧
		GetDlgItem(IDC_BTN_C)->ShowWindow(SW_HIDE);
		break;
	case 2:
		check = L"Circles";	
		//模板
		GetDlgItem(IDC_BTN_MODEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_RECT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SLIDER_EDIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_EDIT)->ShowWindow(SW_SHOW);
		//直线
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BASE)->SetWindowText(L"基准点");

		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_HIDE);
		//单直线
		GetDlgItem(IDC_BTN_LINE)->ShowWindow(SW_HIDE);
		//单圆弧
		GetDlgItem(IDC_BTN_C)->ShowWindow(SW_HIDE);
		break;
	case 3:
		check = L"Rects";	
		//模板
		GetDlgItem(IDC_BTN_MODEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_RECT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SLIDER_EDIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_EDIT)->ShowWindow(SW_SHOW);
		//直线
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_HIDE);

		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_SHOW);
		//单直线
		GetDlgItem(IDC_BTN_LINE)->ShowWindow(SW_HIDE);
		//单圆弧
		GetDlgItem(IDC_BTN_C)->ShowWindow(SW_HIDE);
		break;
	case 4:
		check = L"SolidCircle";	
		//模板
		//GetDlgItem(IDC_BTN_MODEL)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_CHECK_RECT2)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_SLIDER_EDIT)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_CHECK_EDIT)->ShowWindow(SW_HIDE);
		//直线
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_HIDE);
		//圆弧
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_HIDE);

		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_HIDE);
		//单直线
		GetDlgItem(IDC_BTN_LINE)->ShowWindow(SW_HIDE);

		//单圆弧
		GetDlgItem(IDC_BTN_C)->ShowWindow(SW_SHOW);
		break;
	case 5:
		check = L"Line";	
		//模板
		GetDlgItem(IDC_BTN_MODEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_RECT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SLIDER_EDIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_EDIT)->ShowWindow(SW_SHOW);
		//直线
		GetDlgItem(IDC_BTN_LINE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_LINE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LINE)->ShowWindow(SW_HIDE);
		//圆弧
		GetDlgItem(IDC_BTN_C1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_C2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_C)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_BASE)->ShowWindow(SW_HIDE);

		//矩形
		GetDlgItem(IDC_BTN_RECT1)->ShowWindow(SW_HIDE);

		//单直线
		GetDlgItem(IDC_BTN_LINE)->ShowWindow(SW_SHOW);

		//单圆弧
		GetDlgItem(IDC_BTN_C)->ShowWindow(SW_HIDE);
		break;
	}
	Parameter.hv_AStart = m_AStart;
	Parameter.hv_ARange = m_ARange;
	Parameter.hv_Score = m_Score;
	Parameter.linesParameter.baseline = m_Line.GetCurSel();
	Parameter.circlesParameter.basepoint = m_Point.GetCurSel();
	Parameter.check = check;
}


void CModelBuildDlg::OnBnClickedLines()
{
	//直线匹配
	OnBnClickedModle();
}


void CModelBuildDlg::OnBnClickedCir()
{
	//圆弧匹配
	OnBnClickedModle();
}


void CModelBuildDlg::OnBnClickedRect()
{
	//矩形匹配
	OnBnClickedModle();
}

void CModelBuildDlg::OnBnClickedSigleCir()
{
	//实心圆匹配
	OnBnClickedModle();
}

void CModelBuildDlg::OnBnClickedSigleLine()
{
	//单直线匹配
	OnBnClickedModle();
}

void CModelBuildDlg::OnBnClickedCheckEdit()
{
	//擦除
	UpdateData(true);
	bool ret = false;
	if(m_isRubber)
	{
		//启动擦除功能
		//还原模板
		switch(type)
		{
		case 0:
			//底板		
			ret = g_pCalgorithmRegion->RestoreDownModel(ToolModel, scrImage, 
				ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld, 1,
				Parameter);
			break;
		case 1:
			//贴片
			ret = g_pCalgorithmRegion->RestoreUpModel(ToolModel, scrImage, 
				ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld, 1,
				Parameter);
			break;
		}
		if(!ret)
		{
			m_isRubber = false;
			ToolModel.downlines.clear();
			UpdateData(false);
		}
	}
	else
	{
		//完成擦除
		ToolModel.downlines.clear();

		//保存擦除后模板
		switch(type)
		{
		case 0:
			//底板		
			g_pCalgorithmRegion->SaveDownModel(ToolModel, scrImage,
				ho_Rectangle_model, Parameter);
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->SaveUpModel(ToolModel, scrImage,
				ho_Rectangle_model, Parameter);
			break;
		}

		OnBnClickedCenter();
	}
}


void CModelBuildDlg::OnBnClickedCenter()
{
	//重心
	switch(type)
	{
	case 0:
		//底板
		g_pCalgorithmRegion->SaveDownGravity(ToolModel, scrImage, 
			ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld,
			Parameter);
		break;
	case 1:
		//贴片
		g_pCalgorithmRegion->SaveUpGravity(ToolModel, scrImage, 
			ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld,
			Parameter);
		break;
	}
}

void CModelBuildDlg::ViewToImage(double &dx,double &dy)
{
	//窗口坐标转图像坐标
	CRect picRect;
	GetDlgItem(IDC_STATIC_MODELVIEW)->GetWindowRect(&picRect);
	ScreenToClient(picRect);

	//double ratioX = (double)g_pMachineCur->GetDownCam()->GetPicWidth()/picRect.Width();
	//double ratioY = (double)g_pMachineCur->GetDownCam()->GetPicHeight()/picRect.Height();

	double ratioX = (double)(ToolModel.PartCol1-ToolModel.PartCol0)/picRect.Width();
	double ratioY = (double)(ToolModel.PartRow1-ToolModel.PartRow0)/picRect.Height();

	dx = dx*ratioX + ToolModel.PartCol0;
	dy = dy*ratioY + ToolModel.PartRow0;
}


void CModelBuildDlg::OnBnClickedBtnTest()
{
	//跑图测试
	//HObject image;
	//vector<double> findResults;
	//bool ret = false;

	////计算贴合过程
	//vector<double> parameters, results;
	////吸嘴位置结果
	//vector<CString> paths;
	//vector<vector<double>> allResults;
	//CString path;
	//int count = 36;

	////测试变量
	//vector<double> X_paste, Y_paste, X_obj, Y_obj;
	//for(int i=0; i< count; i++)
	//{
	//	switch(type)
	//	{
	//	case 0:
	//		{
	//			//底板		
	//			g_pBmp->GetPic(0, image, path);
	//			ret = g_pCalgorithmRegion->FindDownModel(ToolModel, image, findResults, Parameter, g_SaveImage);
	//		}
	//		break;
	//	case 1:
	//		{
	//			//贴片
	//			g_pBmp->GetPic(1, image, path);
	//			ret = g_pCalgorithmRegion->FindUpModel(ToolModel, image, findResults, Parameter, g_SaveImage);

	//			parameters.clear();
	//			//底板数据（机械坐标系）
	//			parameters.push_back(0);
	//			parameters.push_back(i*20);
	//			parameters.push_back(0);
	//			//底板偏移数据（以底板直角为坐标系）
	//			parameters.push_back(0);
	//			parameters.push_back(0);
	//			parameters.push_back(0);
	//			//贴片数据（吸嘴中心坐标系）
	//			parameters.push_back(findResults[0]);
	//			parameters.push_back(findResults[1]);
	//			parameters.push_back(findResults[2]);
	//		}
	//		break;
	//	}	
	//	
	//	g_pCalgorithmRegion->GetResult(parameters, results);

	//	parameters.push_back(results[0]);
	//	parameters.push_back(results[1]);
	//	parameters.push_back(results[2]);
	//	paths.push_back(path);
	//	allResults.push_back(parameters);
	//	ToolModel.ShowImage(image);

	//	X_paste.push_back(allResults[i][6]);
	//	Y_paste.push_back(allResults[i][7]);
	//	X_obj.push_back(allResults[i][9]);
	//	Y_obj.push_back(allResults[i][10]);
	//}

	AfxMessageBox(L"跑图结束，断点分析数据"); 
}


void CModelBuildDlg::OnNMCustomdrawSliderMedian(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// 中值滤波器掩码
	m_median = m_MedianSc.GetPos();
	UpdateData(false);
	OnBnClickedCheckMult();
	*pResult = 0;
}


void CModelBuildDlg::OnBnClickedCheckMedian()
{
	//2.是否使用中值滤波器
	OnBnClickedCheckMult();
}


void CModelBuildDlg::OnBnClickedBtnC()
{
	//圆弧区域
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_C, butCStr);
	if(L"点击右键确定" != butCStr)
	{
		SetDlgItemText(IDC_BTN_C, L"点击右键确定");
		switch(type)
		{
		case 0:
			//底板
			g_pCalgorithmRegion->SetDownCircleRect(ToolModel, scrImage, 
				Parameter);
			break;
		case 1:
			//贴片
			g_pCalgorithmRegion->SetUpCircleRect(ToolModel, scrImage, 
				Parameter);
			break;
		}	
		SetDlgItemText(IDC_BTN_C, L"圆弧区域");
	}
}


void CModelBuildDlg::OnClose()
{
	bool ret = false;
	switch(type)
	{
	case 0:
		//底板
		ret = g_pCalgorithmRegion->CompareDownParameters(Parameter);
		break;
	case 1:
		//贴片
		ret = g_pCalgorithmRegion->CompareUpParameters(Parameter);
		break;
	}
	if(!ret)
	{
		CMassageDlg l_MassageDlg(_T("确定要保存模板吗？"),this);
		int ret = l_MassageDlg.DoModal();
		if(IDOK == ret)
		{
			OnBnClickedBtnSave();
		}
		if(IDCANCEL == ret)
		{
			
		}
	}
	CDialogEx::OnClose();
}


void CModelBuildDlg::OnEnChangeEditMult()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
