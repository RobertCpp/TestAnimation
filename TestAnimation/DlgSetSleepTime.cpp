// DlgSetSleepTime.cpp : 实现文件
//

#include "stdafx.h"
#include "TestAnimation.h"
#include "DlgSetSleepTime.h"


// CDlgSetSleepTime 对话框

IMPLEMENT_DYNAMIC(CDlgSetSleepTime, CDialog)

CDlgSetSleepTime::CDlgSetSleepTime(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetSleepTime::IDD, pParent)
{
	m_strSleepTime = _T("1");
}

CDlgSetSleepTime::~CDlgSetSleepTime()
{
}

void CDlgSetSleepTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbxTimeList);
}


BEGIN_MESSAGE_MAP(CDlgSetSleepTime, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSetSleepTime::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CDlgSetSleepTime::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strTmp;
	theApp.LoadString(IDS_STRING138, strTmp);
	GetDlgItem(IDC_STATIC_MINUTES)->SetWindowText(strTmp);

	for(int i=0; i<10; i++)
	{
		strTmp.Format(_T("%d"), i+1);
		m_cbxTimeList.InsertString(i, strTmp);
	}

	theApp.LoadString(IDS_STRING140, strTmp);
	GetDlgItem(IDOK)->SetWindowText(strTmp);
	theApp.LoadString(IDS_STRING142, strTmp);
	GetDlgItem(IDCANCEL)->SetWindowText(strTmp);

	return TRUE;
}

void CDlgSetSleepTime::OnBnClickedOk()
{
	UpdateData();

	int nItem = m_cbxTimeList.GetCurSel();
	if(nItem >= 0)
	{
		m_cbxTimeList.GetLBText(nItem, m_strSleepTime);
	}

	OnOK();
}

UINT CDlgSetSleepTime::GetSleepTime()
{
	UINT nSleepTime = (UINT)_tcstoul(m_strSleepTime.GetBuffer(0), NULL, 10);
	m_strSleepTime.ReleaseBuffer();
	return nSleepTime*60;
}