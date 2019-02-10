#pragma once

#include "readme.h"

// CMassageDlg dialog

class CMassageDlg : public CDialog
{
	DECLARE_DYNAMIC(CMassageDlg)

public:
	CMassageDlg(CWnd* pParent = NULL);   // standard constructor
	CMassageDlg(CString strTemp,CWnd* pParent = NULL);
	virtual ~CMassageDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMassage;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
