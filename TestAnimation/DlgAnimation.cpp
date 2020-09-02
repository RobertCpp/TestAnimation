#include "stdafx.h"
#include "TestAnimation.h"
#include "DlgAnimation.h"

IMPLEMENT_DYNAMIC(CDlgAnimation, CDialog)

CDlgAnimation::CDlgAnimation(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	m_dwImgCount = 0;
	m_dwImgExitCount = 0;
	m_dwCurrentImgIdx = 0;
	m_nAnimationTimer = 0;
	m_nExitTimer = 0;
	m_nFrameRate = 10;
	m_nFrameIdx = 0;
	m_nXMoveStep = 0;
	m_nYMoveStep = 0;
	m_nPicWidth  = 0;
	m_nPicHeight = 0;
	m_bExiting = FALSE;

	for(DWORD i=0; i<MAX_ANIMATION_CNT; i++)
	{
		m_pImgArr[i] = NULL;
		m_pImgExit[i] = NULL;
	}

	m_pImgStill = NULL;
}

CDlgAnimation::~CDlgAnimation()
{
	for(DWORD i=0; i<m_dwImgCount; i++)
	{
		if(m_pImgArr[i] != NULL)
		{
			delete m_pImgArr[i];
			m_pImgArr[i] = NULL;
		}
	}

	for(DWORD i=0; i<m_dwImgExitCount; i++)
	{
		if(m_pImgExit[i] != NULL)
		{
			delete m_pImgExit[i];
			m_pImgExit[i] = NULL;
		}
	}

	if(NULL != m_pImgStill)
	{
		delete m_pImgStill;
		m_pImgStill = NULL;
	}
}

void CDlgAnimation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAnimation, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDlgAnimation::LoadAllPicture(UINT nStartResId, UINT nStopResId, LPCTSTR lpType)
{
	HINSTANCE hInst = NULL;
	HRSRC hRsrc = NULL;
	DWORD dwLen = 0;
	HGLOBAL hGlobal = NULL;
	BYTE* lpMem = NULL;
	BYTE* lpData = NULL;
	IStream * pIStream = NULL;
	Image * pPicImage = NULL;

	hInst = AfxGetInstanceHandle();
	if(NULL == hInst)
	{
		return FALSE;
	}

	m_dwImgCount = nStopResId - nStartResId + 1;

	for(UINT nId=nStartResId; nId<=nStopResId; nId++)
	{
		hRsrc = FindResource(hInst, MAKEINTRESOURCE(nId), lpType);
		if(NULL == hRsrc)
		{
			return FALSE;
		}

		dwLen = SizeofResource(hInst, hRsrc);
		if(0 == dwLen)
		{
			return FALSE;
		}
		lpMem = (BYTE*)LoadResource(hInst, hRsrc);
		if(0 == dwLen)
		{
			return FALSE;
		}
		hGlobal = GlobalAlloc(GMEM_FIXED, dwLen);
		if(NULL == hGlobal)
		{
			return FALSE;
		}
		lpData = (BYTE*)GlobalLock(hGlobal);
		if(NULL == lpData)
		{
			return FALSE;
		}
		memcpy(lpData, lpMem, dwLen);

		pIStream = NULL;
		if(S_OK != CreateStreamOnHGlobal(hGlobal, FALSE, &pIStream) || NULL == pIStream)
		{
			return FALSE;
		}

		pPicImage = Image::FromStream(pIStream);
		if(NULL == pPicImage)
		{
			return FALSE;
		}
		m_pImgArr[nId-nStartResId] = pPicImage;

		GlobalUnlock(hGlobal);
		pIStream->Release();
		FreeResource(lpMem);
	}
	

	return TRUE;
}

BOOL CDlgAnimation::LoadAllPicture(UINT nFileStartIdx, UINT nFileStopIdx, UINT nFileNumWidth, LPCTSTR lpFilePrefix, LPCTSTR lpFileExt)
{
	CString strFileDir = theApp.m_strModuleFilePath + _T("\\") + lpFilePrefix;
	CString strFilePattern;
	CString strFileName;
	CString strTmp;
	Image * pPicImage = NULL;
	UINT nFileIdx = 0;

	strFilePattern.Format(_T("%%0%dd.%s"), nFileNumWidth, lpFileExt);

	for(nFileIdx=nFileStartIdx; nFileIdx<=nFileStopIdx; nFileIdx++)
	{
		strTmp = strFileDir + strFilePattern;
		strFileName.Format(strTmp, nFileIdx);

		pPicImage = Image::FromFile(strFileName);
		if(NULL != pPicImage)
		{
			m_pImgArr[nFileIdx-nFileStartIdx] = pPicImage;
		}
		else
		{
			break;
		}
	}

	m_dwImgCount = nFileIdx-nFileStartIdx;
	return (m_dwImgCount>0)?TRUE:FALSE;
}

BOOL CDlgAnimation::LoadAllPicture(LPCTSTR lpPicPath, LPCTSTR lpFileExt)
{
	CString strFileToFind;
	CFileFind ff;
	BOOL bWorking;
	Image * pPicImage = NULL;
	UINT nFileIdx = 0;
	SET_STRING ssPic;
	SET_STRING::iterator it, itEnd;

	strFileToFind.Format(_T("%s\\0.%s"), lpPicPath, lpFileExt);
	m_pImgStill = Image::FromFile(strFileToFind);
	if(NULL == m_pImgStill)
	{
		return FALSE;
	}

	strFileToFind.Format(_T("%s\\*.%s"), lpPicPath, lpFileExt);
	bWorking = ff.FindFile(strFileToFind);
	while(bWorking)
	{
		bWorking = ff.FindNextFile();

		strFileToFind = ff.GetFileName();
		if(strFileToFind.Compare(_T("0.png")) == 0)
		{
			continue;
		}

		strFileToFind = ff.GetFilePath();
		ssPic.insert(strFileToFind);
	}

	m_dwImgCount = (DWORD)ssPic.size();
	if(m_dwImgCount == 0)
	{
		return FALSE;
	}
	else
	{
		itEnd = ssPic.end();
		for(it=ssPic.begin(); it!=itEnd; it++)
		{
			strFileToFind = *it;
			pPicImage = Image::FromFile(strFileToFind);
			if(NULL != pPicImage)
			{
				m_pImgArr[nFileIdx++] = pPicImage;
				pPicImage = NULL;
			}
		}
		ssPic.clear();
	}
	

	return TRUE;
}

void CDlgAnimation::ClearAllPicture()
{
	for(DWORD i=0; i<m_dwImgCount; i++)
	{
		if(m_pImgArr[i] != NULL)
		{
			delete m_pImgArr[i];
			m_pImgArr[i] = NULL;
		}
	}

	if(NULL != m_pImgStill)
	{
		delete m_pImgStill;
		m_pImgStill = NULL;
	}

	m_nFrameIdx = 0;
	m_dwImgCount = 0;
	m_dwCurrentImgIdx = 0;
}

BOOL CDlgAnimation::LoadExitPicture(LPCTSTR lpPicPath, LPCTSTR lpFileExt)
{
	CString strFileToFind;
	CFileFind ff;
	BOOL bWorking;
	Image * pPicImage = NULL;
	UINT nFileIdx = 0;
	SET_STRING ssPic;
	SET_STRING::iterator it, itEnd;


	strFileToFind.Format(_T("%s\\*.%s"), lpPicPath, lpFileExt);
	bWorking = ff.FindFile(strFileToFind);
	while(bWorking)
	{
		bWorking = ff.FindNextFile();

		strFileToFind = ff.GetFileName();
		if(strFileToFind.Compare(_T("0.png")) == 0)
		{
			continue;
		}

		strFileToFind = ff.GetFilePath();
		ssPic.insert(strFileToFind);
	}

	m_dwImgExitCount = (DWORD)ssPic.size();
	if(m_dwImgExitCount == 0)
	{
		return FALSE;
	}
	else
	{
		itEnd = ssPic.end();
		for(it=ssPic.begin(); it!=itEnd; it++)
		{
			strFileToFind = *it;
			pPicImage = Image::FromFile(strFileToFind);
			if(NULL != pPicImage)
			{
				m_pImgExit[nFileIdx++] = pPicImage;
				pPicImage = NULL;
			}
		}
		ssPic.clear();
	}
	

	return TRUE;
}

LRESULT CDlgAnimation::OnNcHitTest(CPoint point)
{
	LRESULT uHitTest = CDialog::OnNcHitTest(point);
	if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0)
	{
		return (uHitTest == HTCLIENT) ? HTCAPTION : uHitTest;
	}
	else
	{
		return uHitTest;
	}
	
}

BOOL CDlgAnimation::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd,GWL_EXSTYLE)^WS_EX_LAYERED);
	SetLayeredWindowAttributes(RGB(255,255,255), 0, LWA_COLORKEY);

	return TRUE;
}

void CDlgAnimation::SetFrameRate(UINT nMillisecFrameRate)
{
	if(m_nAnimationTimer != 0)
	{
		KillTimer(m_nAnimationTimer);
		m_nAnimationTimer = 0;
	}

	m_nFrameRate = nMillisecFrameRate;
	m_nAnimationTimer = SetTimer(1, m_nFrameRate, NULL);
}

void CDlgAnimation::StopAnimation()
{
	if(m_nAnimationTimer != 0)
	{
		KillTimer(1);
		m_nAnimationTimer = 0;
	}
}

void CDlgAnimation::SetExiting()
{
	StopAnimation();
	if(m_dwImgExitCount > 0 && NULL != m_pImgExit)
	{
		m_dwCurrentImgIdx = 0;
		m_nExitTimer = SetTimer(3, 20, NULL);
		m_bExiting = TRUE;
	}
	else
	{
		CWnd * pWndMain = AfxGetMainWnd();
		pWndMain->PostMessage(WM_CLOSE);
	}	
}

void CDlgAnimation::AdjustPostion()
{
	RECT rcWindow;
	int nStartX = 0;

	GetWindowRect(&rcWindow);
	nStartX = rcWindow.left + m_nXMoveStep;
	if(nStartX > m_nScreenWidth)
	{
		nStartX = -m_nPicWidth;
	}

	rcWindow.left = nStartX;
	rcWindow.top = rcWindow.top;
	rcWindow.right = rcWindow.left + m_nPicWidth;
	rcWindow.bottom = rcWindow.top + m_nPicHeight;
	MoveWindow(&rcWindow, FALSE);
}

void CDlgAnimation::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		m_dwCurrentImgIdx++;
		if(m_dwCurrentImgIdx == m_dwImgCount)
		{
			m_dwCurrentImgIdx = 0;
		}

		m_nFrameIdx++;

		AdjustPostion();

		Invalidate();
	}
	else if(nIDEvent == 3)
	{
		m_dwCurrentImgIdx++;
		Invalidate();
	}

	return CDialog::OnTimer(nIDEvent);
}

BOOL CDlgAnimation::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CDlgAnimation::OnPaint()
{
	CPaintDC dc(this);
	DrawAll(&dc);
}

void CDlgAnimation::DrawAll(CDC* pDC)
{
	CDC memDC;
	RECT rcWindow;
	CBitmap bmpEmpty;
	CBitmap *pOldBitmap = NULL;
	int nOldMode = 0;
	Image * pCurrentImg = NULL;

	if(m_nAnimationTimer != 0)
	{
		pCurrentImg = m_pImgArr[m_dwCurrentImgIdx];
	}
	else
	{
		if(m_bExiting)
		{
			pCurrentImg = m_pImgExit[m_dwCurrentImgIdx];
		}
		else
		{
			pCurrentImg = m_pImgStill;
		}
	}

	if(m_nPicWidth > 0 && m_nPicHeight > 0 && NULL != pCurrentImg)
	{
		memDC.CreateCompatibleDC(pDC);
		bmpEmpty.CreateCompatibleBitmap(pDC, m_nPicWidth, m_nPicHeight);
		pOldBitmap = memDC.SelectObject(&bmpEmpty);

		Gdiplus::Graphics graphics(memDC.GetSafeHdc());
		graphics.Clear(Color(255, 255, 255));

		GetWindowRect(&rcWindow);
		if(m_rcDestPostion.left >= rcWindow.left)
		{
			graphics.DrawImage(pCurrentImg, 0, 0, m_nPicWidth, m_nPicHeight);
		}
		else
		{
			Point points[] = 
			{
				Point(m_nPicWidth, 0),
				Point(0, 0),
				Point(m_nPicWidth, m_nPicHeight)
			};
			graphics.DrawImage(pCurrentImg, points, 3);
		}

		nOldMode = pDC->SetStretchBltMode(COLORONCOLOR);
		pDC->StretchBlt(0,0, m_nPicWidth, m_nPicHeight, &memDC, 0, 0, m_nPicWidth, m_nPicHeight, SRCCOPY);
		pDC->SetStretchBltMode(nOldMode);

		bmpEmpty.DeleteObject();
		memDC.SelectObject(pOldBitmap);
		memDC.DeleteDC();
	}
	else
	{
		CDialog::OnPaint();
	}

	if(m_bExiting && m_dwCurrentImgIdx == m_dwImgExitCount)
	{
		CWnd * pWndMain = AfxGetMainWnd();
		pWndMain->PostMessage(WM_CLOSE);
	}
}