#pragma once
#include "CalgorithmRegion.h"
#include "afxcmn.h"
#include "afxwin.h"

#include "readme.h"

// CModelBuildDlg 对话框

class CModelBuildDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModelBuildDlg)

public:
	CModelBuildDlg(HObject &scrImage, int type, CalgorithmRegion::ModelParameter &Parameter, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModelBuildDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_MODEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnRect();
	afx_msg void OnBnClickedBtnModel();
	afx_msg void OnBnClickedBtnSave();

	int type;					//0：底板模板， 1：贴片模板
	HObject scrImage;			//原图像
	HalconTool ToolModel;		//模板显示图像工具
	virtual BOOL OnInitDialog();
	// 开始角度
	double m_AStart;
	// 角度范围
	double m_ARange;
	// 合格分数
	double m_Score;

	//增强图像对比
	afx_msg void OnBnClickedCheckMult();
	afx_msg void OnNMCustomdrawSliderMult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderAdd(NMHDR *pNMHDR, LRESULT *pResult);

	// 是否增强图像对比
	BOOL m_IsMultImage;
	// 增强图像对比mult
	double m_Mult;
	// 增强图像对比add
	double m_Add;
	// mult滑动条
	CSliderCtrl m_MultSc;
	// add滑动条
	CSliderCtrl m_AddSc;

	//针对鼠标移动
	//鼠标是否按下左键
	bool m_MouseDown;
	//保存鼠标按下时的坐标
	CPoint origin;  
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//直线1直线2区域
	afx_msg void OnBnClickedBtnLine1();
	afx_msg void OnBnClickedBtnLine2();
	// 基准线
	CComboBox m_Line;
	// 识别方式
	CComboBox m_check;
	afx_msg void OnCbnSelchangeComboCheck();

	//圆弧1圆弧2区域
	afx_msg void OnBnClickedBtnC1();
	afx_msg void OnBnClickedBtnC2();
	// 基准点
	CComboBox m_Point;

	//贴片底板参数
	CalgorithmRegion::ModelParameter Parameter;
	afx_msg void OnBnClickedBtnRect1();
	// 匹配方式
	int m_ModelCheck;
	afx_msg void OnBnClickedModle();
	afx_msg void OnBnClickedLines();
	afx_msg void OnBnClickedCir();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRect();

	//原模板区域 擦除后剩下区域 模板轮廓 擦除区域
	HObject ho_Rectangle, ho_Rectangle_model, ho_Region_xld, ho_RubberRectangle;

	// 擦除橡皮檫大小
	CSliderCtrl m_RubberSize;
	afx_msg void OnBnClickedCheckEdit();
	// 是否启用擦除功能
	BOOL m_isRubber;
	afx_msg void OnBnClickedCenter();

	//窗口坐标转图像坐标
	void ViewToImage(double &dx,double &dy);
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedSigleCir();
	// 是否使用矩形2选择模板
	BOOL m_UseRect2;
	afx_msg void OnBnClickedBtnLine();
	afx_msg void OnBnClickedSigleLine();


	// 中值滤波器掩码
	int m_median;
	afx_msg void OnNMCustomdrawSliderMedian(NMHDR *pNMHDR, LRESULT *pResult);
	// 中值滤波掩码滑动条
	CSliderCtrl m_MedianSc;
	afx_msg void OnBnClickedCheckMedian();
	// 是否使用中值滤波
	BOOL m_UseMedian;
	afx_msg void OnBnClickedBtnC();
	afx_msg void OnClose();
	afx_msg void OnEnChangeEditMult();
};
