
// PSAdemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CPSAdemoDlg 对话框
class CPSAdemoDlg : public CDialogEx
{
// 构造
public:
	CPSAdemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PSADEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnGetpic();
	afx_msg void OnBnClickedBtnEditmodel();
	afx_msg void OnBnClickedBtnFindmodel();

	//3.类成员变量
	//模板参数
	CalgorithmRegion::ModelParameter DownParameter;
	// 所有模板
	CComboBox m_AllModel;
	vector<CString> m_DownModels;
	afx_msg void OnCbnSelchangeAllModel();
	afx_msg void OnBnClickedBtnAdd();
	// 新添模板名称
	CString m_AddName;
	afx_msg void OnBnClickedBtnDelete();
	// 当前模板
	CString m_CurModel;
};
