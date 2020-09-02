#pragma once

class CDlgSetSleepTime : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetSleepTime)

public:
	CDlgSetSleepTime(CWnd* pParent = NULL);
	virtual ~CDlgSetSleepTime();
	UINT GetSleepTime();
	enum { IDD = IDD_DLG_SET_RESTTIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()


protected:
	CComboBox m_cbxTimeList;
	CString   m_strSleepTime;
};
