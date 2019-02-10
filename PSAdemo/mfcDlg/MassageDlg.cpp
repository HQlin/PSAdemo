// MassageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MassageDlg.h"


// CMassageDlg dialog

IMPLEMENT_DYNAMIC(CMassageDlg, CDialog)

CMassageDlg::CMassageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMassageDlg::IDD, pParent)
{

}
CMassageDlg::CMassageDlg(CString strTemp,CWnd* pParent)
: CDialog(CMassageDlg::IDD, pParent)
{
	m_strMassage=strTemp;
}
CMassageDlg::~CMassageDlg()
{
}

void CMassageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMassageDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CMassageDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMassageDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMassageDlg message handlers

BOOL CMassageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	this->GetDlgItem(IDC_STATIC_MASSAGENOTE)->SetWindowText(m_strMassage);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMassageDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	this->OnOK();
}

void CMassageDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	this->OnCancel();
}
