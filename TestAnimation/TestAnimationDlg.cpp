#include "stdafx.h"
#include "TestAnimation.h"
#include "TestAnimationDlg.h"
#include "DlgSetSleepTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef struct tagLVITEM64
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    __int64 pszText;
    int cchTextMax;
    int iImage;
    __int64 lParam;
#if (_WIN32_IE >= 0x0300)
    int iIndent;
#endif
#if (_WIN32_WINNT >= 0x0501)
    int iGroupId;
    UINT cColumns; // tile view columns
    __int64 puColumns;
#endif
#if _WIN32_WINNT >= 0x0600
    __int64 piColFmt;
    int iGroup; // readonly. only valid for owner data.
#endif
} LVITEM64;


CTestAnimationDlg::CTestAnimationDlg(CWnd* pParent /*=NULL*/)
	: CDlgAnimation(CTestAnimationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nSleepTimer = 0;

	m_nSleepTime = 60;
}

void CTestAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgAnimation::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestAnimationDlg, CDlgAnimation)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MENU_SELECT_ANIMATION, &CTestAnimationDlg::OnMenuSelectAnimation)
	ON_COMMAND(ID_MENU_SET_SLEEP_TIME, &CTestAnimationDlg::OnMenuSetSleepTime)
	ON_COMMAND(ID_MENU_EXIT, &CTestAnimationDlg::OnMenuExit)
END_MESSAGE_MAP()

void CTestAnimationDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 2)
	{
		KillTimer(2);
		m_nSleepTimer = NULL;

		SelectDestPostion();
		SetFrameRate(30);
	}

	return CDlgAnimation::OnTimer(nIDEvent);
}

void CTestAnimationDlg::AdjustPostion()
{
	RECT rcWindow;

	GetWindowRect(&rcWindow);
	rcWindow.left = rcWindow.left + m_nXMoveStep;
	rcWindow.top = rcWindow.top + m_nYMoveStep;
	rcWindow.right = rcWindow.left + m_nPicWidth;
	rcWindow.bottom = rcWindow.top + m_nPicHeight;
	MoveWindow(&rcWindow, FALSE);

	if(rcWindow.top < m_rcDestPostion.top)
	{
		m_nYMoveStep = 1;
	}
	else if(rcWindow.top == m_rcDestPostion.top)
	{
		m_nYMoveStep = 0;
	}
	else
	{
		m_nYMoveStep = -1;
	}

	if(rcWindow.left < m_rcDestPostion.left)
	{
		m_nXMoveStep = 1;
	}
	else if(rcWindow.left == m_rcDestPostion.left)
	{
		m_nXMoveStep = 0;
	}
	else
	{
		m_nXMoveStep = -1;
	}


	if(rcWindow.left == m_rcDestPostion.left && rcWindow.top == m_rcDestPostion.top)
	{
		StopAnimation();

		m_nSleepTimer = SetTimer(2, m_nSleepTime*1000, NULL); 
	}
}

void CTestAnimationDlg::SelectDestPostion()
{
	size_t nIconRectCount = m_vecDesktopIconRect.size();
	if(nIconRectCount == 0)
	{
		m_rcDestPostion.left   = (LONG)((m_nScreenWidth - m_nPicWidth)/2);
		m_rcDestPostion.top    = (LONG)((m_nScreenHeight - m_nPicHeight)/2);
		m_rcDestPostion.right  = m_rcDestPostion.left + m_nPicWidth;
		m_rcDestPostion.bottom = m_rcDestPostion.top + m_nPicHeight;
	}
	else
	{
		int nRandIdx = 0;
		RECT rcRandom;

		srand((UINT)time(NULL));
		nRandIdx = rand()%nIconRectCount;
		rcRandom = m_vecDesktopIconRect[nRandIdx];
		while(rcRandom.top < (LONG)(m_nPicHeight/2))
		{
			nRandIdx = rand()%nIconRectCount;
			rcRandom = m_vecDesktopIconRect[nRandIdx];
		}

		m_rcDestPostion.left   = rcRandom.left + (LONG)(abs((rcRandom.right - rcRandom.left + 1 - m_nPicWidth))/2);
		m_rcDestPostion.top    = rcRandom.top - (LONG)(m_nPicHeight/2);
		m_rcDestPostion.right  = m_rcDestPostion.left + m_nPicWidth;
		m_rcDestPostion.bottom = m_rcDestPostion.top + m_nPicHeight;
	}

	RECT rcWindow;
	GetWindowRect(&rcWindow);
	if(rcWindow.top < m_rcDestPostion.top)
	{
		m_nYMoveStep = 1;
	}
	else if(rcWindow.top == m_rcDestPostion.top)
	{
		m_nYMoveStep = 0;
	}
	else
	{
		m_nYMoveStep = -1;
	}

	if(rcWindow.left < m_rcDestPostion.left)
	{
		m_nXMoveStep = 1;
	}
	else if(rcWindow.left == m_rcDestPostion.left)
	{
		m_nXMoveStep = 0;
	}
	else
	{
		m_nXMoveStep = -1;
	}
}

BOOL CTestAnimationDlg::GetDesktopIconsRect(VEC_RECT & vecRect)
{
	int nIconCount = 0;
	HWND hWndDesktop = NULL;
	LVITEM64 lvi, *pLvi;
    TCHAR  szItem[512], szSubItem[512];
    TCHAR* pItem, *pSubItem;
    DWORD  dwPid;
    HANDLE hProcess;
	RECT   rcIcon, *pRectBuffer;
	CString strDbg;

    hWndDesktop = ::FindWindow(_T("Progman"), _T("Program Manager"));
	if(NULL == hWndDesktop)
	{
		return FALSE;
	}
    hWndDesktop = ::FindWindowEx(hWndDesktop, NULL, _T("SHELLDLL_DefView"), NULL);
	if(NULL == hWndDesktop)
	{
		return FALSE;
	}
    hWndDesktop = ::FindWindowEx(hWndDesktop, NULL, _T("SysListView32"), NULL);
	if(NULL == hWndDesktop)
	{
		return FALSE;
	}

	nIconCount = (int)::SendMessage(hWndDesktop, LVM_GETITEMCOUNT, 0, 0);
	if(nIconCount <= 0)
	{
		return FALSE;
	}

	GetWindowThreadProcessId(hWndDesktop, &dwPid);
	hProcess = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_QUERY_INFORMATION, FALSE, dwPid);
	if(NULL == hProcess)
	{
		return FALSE;
	}

	pLvi      = (LVITEM64*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM64),    MEM_COMMIT, PAGE_READWRITE);
    pItem     = (wchar_t*)VirtualAllocEx(hProcess,  NULL, 512*sizeof(wchar_t), MEM_COMMIT, PAGE_READWRITE);
    pSubItem  = (wchar_t*)VirtualAllocEx(hProcess,  NULL, 512*sizeof(wchar_t), MEM_COMMIT, PAGE_READWRITE);
	pRectBuffer = (RECT*)VirtualAllocEx(hProcess, NULL, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE);

	for(int i=0; i<nIconCount; i++) 
	{
        lvi.iSubItem = 0;
        lvi.pszText  = (__int64)pItem;
		lvi.cchTextMax = 512;
        WriteProcessMemory(hProcess, pLvi, &lvi, sizeof(LVITEM64), NULL);
        ::SendMessage( hWndDesktop, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)pLvi);

        lvi.iSubItem = 1;
        lvi.pszText  = (__int64)pSubItem;
		lvi.cchTextMax = 512;
        WriteProcessMemory(hProcess, pLvi, &lvi, sizeof(LVITEM64), NULL);
        ::SendMessage( hWndDesktop, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)pLvi);

		rcIcon.left = LVIR_BOUNDS;
        ::WriteProcessMemory( hProcess, pRectBuffer, &rcIcon, sizeof(RECT), NULL);
        ::SendMessage( hWndDesktop, LVM_GETITEMRECT, (WPARAM)i, (LPARAM)pRectBuffer);

        ReadProcessMemory(hProcess, pItem,       szItem,    512*sizeof(TCHAR), NULL);
        ReadProcessMemory(hProcess, pSubItem,    szSubItem, 512*sizeof(wchar_t), NULL); 
        ReadProcessMemory(hProcess, pRectBuffer, &rcIcon,   sizeof(RECT), NULL);

		//strDbg.Format(_T("%s - %s[LEFT:%d TOP:%d RIGHT:%d BOTTOM:%d]\n"), szItem, szSubItem, rcIcon.left,rcIcon.top,rcIcon.right,rcIcon.bottom);
        //OutputDebugString(strDbg);

		vecRect.push_back(rcIcon);
    }

    VirtualFreeEx(hProcess, pLvi,        0, MEM_RELEASE);
    VirtualFreeEx(hProcess, pItem,       0, MEM_RELEASE);
    VirtualFreeEx(hProcess, pSubItem,    0, MEM_RELEASE);
    VirtualFreeEx(hProcess, pRectBuffer, 0, MEM_RELEASE);

    CloseHandle(hProcess);

	return TRUE;
}

BOOL CTestAnimationDlg::OnInitDialog()
{
	CDlgAnimation::OnInitDialog();

	CString strAnimationPath;
	TCHAR szAnimationSelected[MAX_PATH];

	strAnimationPath = theApp.m_strModuleFilePath + _T("\\animation\\cfg.ini");

	ZeroMemory(szAnimationSelected, sizeof(szAnimationSelected));
	::GetPrivateProfileString(_T("config"), _T("sleep"), _T("60"), szAnimationSelected, MAX_PATH, strAnimationPath);
	if(_tcslen(szAnimationSelected) == 0)
	{
		_stprintf_s(szAnimationSelected, MAX_PATH, _T("60"));
		::WritePrivateProfileString(_T("config"), _T("sleep"), _T("60"), strAnimationPath);
	}
	m_nSleepTime = (UINT)_tcstoul(szAnimationSelected, NULL, 10);

	ZeroMemory(szAnimationSelected, sizeof(szAnimationSelected));
	::GetPrivateProfileString(_T("config"), _T("selected"), _T("eagle"), szAnimationSelected, MAX_PATH, strAnimationPath);
	if(_tcslen(szAnimationSelected) == 0)
	{
		_stprintf_s(szAnimationSelected, MAX_PATH, _T("eagle"));
		::WritePrivateProfileString(_T("config"), _T("selected"), _T("eagle"), strAnimationPath);
	}
	
	strAnimationPath.Format(_T("%s\\animation\\%s"), theApp.m_strModuleFilePath, szAnimationSelected);
	if(!LoadAllPicture(strAnimationPath, _T("PNG")))
	{
		return TRUE;
	}

	strAnimationPath.Format(_T("%s\\animation\\exiting"), theApp.m_strModuleFilePath, szAnimationSelected);
	if(!LoadExitPicture(strAnimationPath, _T("PNG")))
	{
		return TRUE;
	}

	m_nPicWidth  = m_pImgArr[0]->GetWidth();
	m_nPicHeight = m_pImgArr[0]->GetHeight();
	if(m_nPicWidth > 64)
	{
		m_nPicWidth = 64;
	}
	if(m_nPicHeight > 64)
	{
		m_nPicHeight = 64;
	}
	m_nScreenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
	m_nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(0, (int)((m_nScreenHeight-m_nPicHeight)/2), m_nPicWidth, m_nPicHeight);


	if(!GetDesktopIconsRect(m_vecDesktopIconRect))
	{
		PostMessage(WM_CLOSE);
		return TRUE;
	}

	SelectDestPostion();
	SetFrameRate(30);

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	return TRUE;
}

void CTestAnimationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDlgAnimation::OnPaint();
	}
}

HCURSOR CTestAnimationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestAnimationDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu popMenu;
	CMenu *pPopup = NULL;
	CPoint pt;
	CString strAnimationPath;
	CString strFileToFind;
	CFileFind ff;
	BOOL bWorking;
	SET_STRING ssDir;

	strAnimationPath.Format(_T("%s\\animation\\*"), theApp.m_strModuleFilePath);
	bWorking = ff.FindFile(strAnimationPath);
	while(bWorking)
	{
		bWorking = ff.FindNextFile();

		if(!ff.IsDirectory())
		{
			continue;
		}

		strAnimationPath = ff.GetFileName();
		if(strAnimationPath.CompareNoCase(_T("exiting")) == 0 ||
		   strAnimationPath.CompareNoCase(_T(".")) == 0 ||
		   strAnimationPath.CompareNoCase(_T("..")) == 0)
		{
			continue;
		}
		else
		{
			ssDir.insert(strAnimationPath);
		}
	}


	popMenu.LoadMenu(IDR_MENU_ANIMATION);
	pPopup = popMenu.GetSubMenu(0);
	if(ssDir.size() > 0)
	{
		
		CMenu menuAnimation;
		MENUITEMINFO mii;

		if(menuAnimation.CreateMenu())
		{
			ZeroMemory(&mii, sizeof(MENUITEMINFO));
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask  = MIIM_STRING|MIIM_ID;
			mii.fType  = MFT_STRING;
			mii.hSubMenu = NULL;

			m_mapAnimation.clear();

			WORD wIDStart = ID_MENU_EXIT + 1;
			SET_STRING::iterator it, itEnd = ssDir.end();
			for(it=ssDir.begin(); it!=itEnd; it++)
			{
				mii.wID = wIDStart++;
				mii.dwTypeData = (*it).GetBuffer(0);
				menuAnimation.InsertMenuItem(0, &mii, TRUE);
				(*it).ReleaseBuffer();

				m_mapAnimation.insert(MAP_ID_STRING::value_type((UINT)mii.wID, *it));
			}

			CString strMenuItem;

			theApp.LoadString(IDS_STRING130, strMenuItem);
			mii.dwTypeData = strMenuItem.GetBuffer(0);
			mii.fMask  = MIIM_STRING | MIIM_DATA | MIIM_SUBMENU;
			mii.hSubMenu = menuAnimation.m_hMenu;
			pPopup->InsertMenuItem(0, &mii, TRUE);
			strMenuItem.ReleaseBuffer();
			
			theApp.LoadString(IDS_STRING132, strMenuItem);
			pPopup->ModifyMenu(1, MF_BYPOSITION|MF_STRING, ID_MENU_SET_SLEEP_TIME, strMenuItem);

			theApp.LoadString(IDS_STRING134, strMenuItem);
			pPopup->ModifyMenu(2, MF_BYPOSITION|MF_STRING, ID_MENU_EXIT, strMenuItem);
		}
		
		ssDir.clear();
	}
	
	GetCursorPos(&pt);
	pPopup->TrackPopupMenu(TPM_CENTERALIGN|TPM_LEFTBUTTON, pt.x, pt.y, this);

	CDlgAnimation::OnRButtonDown(nFlags, point);
}

BOOL CTestAnimationDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (0 == nCode)
	{
		MAP_ID_STRING::iterator it = m_mapAnimation.find(nID);
		if(it != m_mapAnimation.end())
		{
			StopAnimation();
			ClearAllPicture();

			CString strAnimationPath;
			strAnimationPath.Format(_T("%s\\animation\\%s"), theApp.m_strModuleFilePath, it->second);
			if(!LoadAllPicture(strAnimationPath, _T("PNG")))
			{
				PostMessage(WM_CLOSE);
				return TRUE;
			}

			if(!GetDesktopIconsRect(m_vecDesktopIconRect))
			{
				PostMessage(WM_CLOSE);
				return TRUE;
			}

			SelectDestPostion();
			SetFrameRate(30);
			UpdateWindow();
		}
	}

	return CDlgAnimation::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CTestAnimationDlg::OnMenuSelectAnimation()
{
	
}

void CTestAnimationDlg::OnMenuSetSleepTime()
{
	CDlgSetSleepTime dlgSetSleepTime;
	if(dlgSetSleepTime.DoModal() == IDOK)
	{
		CString strAnimationPath;
		TCHAR szSleepTime[MAX_PATH];

		m_nSleepTime = dlgSetSleepTime.GetSleepTime();
		strAnimationPath = theApp.m_strModuleFilePath + _T("\\animation\\cfg.ini");

		ZeroMemory(szSleepTime, sizeof(szSleepTime));
		_stprintf_s(szSleepTime, MAX_PATH, _T("%d"), m_nSleepTime);
		::WritePrivateProfileString(_T("config"), _T("sleep"), szSleepTime, strAnimationPath);
	}
}

void CTestAnimationDlg::OnMenuExit()
{
	SetExiting();
}
