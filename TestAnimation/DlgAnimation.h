#pragma once

#define MAX_ANIMATION_CNT 1024

#include <set>

typedef std::set<CString> SET_STRING;

class CDlgAnimation : public CDialog
{
	DECLARE_DYNAMIC(CDlgAnimation)

public:
	CDlgAnimation(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CDlgAnimation();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	void DrawAll(CDC* pDC);
	virtual void AdjustPostion();
	
	BOOL LoadAllPicture(UINT nStartResId, UINT nStopResId, LPCTSTR lpType);
	BOOL LoadAllPicture(UINT nFileStartIdx, UINT nFileStopIdx, UINT nFileNumWidth, LPCTSTR lpFilePrefix, LPCTSTR lpFileExt);
	BOOL LoadAllPicture(LPCTSTR lpPicPath, LPCTSTR lpFileExt);
	BOOL LoadExitPicture(LPCTSTR lpPicPath, LPCTSTR lpFileExt);
	void ClearAllPicture();

	void SetFrameRate(UINT nMillisecFrameRate);
	void StopAnimation();
	void SetExiting();

	DECLARE_MESSAGE_MAP()

protected:
	Image *  m_pImgExit[MAX_ANIMATION_CNT];
	Image *  m_pImgArr[MAX_ANIMATION_CNT];
	Image *  m_pImgStill;
	DWORD    m_dwImgCount;
	DWORD    m_dwImgExitCount;
	DWORD    m_dwCurrentImgIdx;

	UINT_PTR m_nAnimationTimer;
	UINT     m_nFrameRate;
	UINT     m_nFrameIdx;

	UINT_PTR m_nExitTimer;
	BOOL     m_bExiting;

	int      m_nXMoveStep;
	int      m_nYMoveStep;
	int      m_nPicWidth;
	int      m_nPicHeight;
	int      m_nScreenWidth;
	int      m_nScreenHeight;

	RECT     m_rcDestPostion;
};
