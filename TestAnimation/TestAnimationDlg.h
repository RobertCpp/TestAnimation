#pragma once

#include "DlgAnimation.h"
#include <VECTOR>
#include <MAP>

typedef std::vector<RECT> VEC_RECT;
typedef std::map<UINT, CString> MAP_ID_STRING;

class CTestAnimationDlg : public CDlgAnimation
{
public:
	CTestAnimationDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_TESTANIMATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	BOOL GetDesktopIconsRect(VEC_RECT & vecRect);
	void SelectDestPostion();

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	virtual void AdjustPostion();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMenuSelectAnimation();
	afx_msg void OnMenuSetSleepTime();
	afx_msg void OnMenuExit();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	VEC_RECT m_vecDesktopIconRect;
	UINT_PTR m_nSleepTimer;
	UINT     m_nSleepTime;

	MAP_ID_STRING m_mapAnimation;
};
