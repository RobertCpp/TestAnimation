#include "stdafx.h"
#include "TestAnimation.h"
#include "TestAnimationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTestAnimationApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CTestAnimationApp::CTestAnimationApp()
{
	m_nLangDelta = 1;
}

CTestAnimationApp theApp;

void CTestAnimationApp::LoadString(UINT nId, CString & strToLoad)
{
	strToLoad.LoadString(nId + m_nLangDelta);
}

BOOL CTestAnimationApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	LCID lcid = GetSystemDefaultLCID();
	if(lcid == 0x804)
	{
		m_nLangDelta = 1;
	}
	else
	{
		m_nLangDelta = 0;
	}

	SetRegistryKey(_T("FeatureAnimation"));

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	HMODULE hModule = GetModuleHandle(NULL);
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(hModule, szFileName, MAX_PATH);
	*(_tcsrchr(szFileName, _T('\\'))) = 0;
	m_strModuleFilePath = szFileName;

	CTestAnimationDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	return FALSE;
}

int CTestAnimationApp::ExitInstance()
{
	GdiplusShutdown(m_gdiplusToken);
	return CWinApp::ExitInstance();
}