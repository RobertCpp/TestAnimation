#pragma once

#include "resource.h"


class CTestAnimationApp : public CWinApp
{
public:
	CTestAnimationApp();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void LoadString(UINT nId, CString & strToLoad);

	DECLARE_MESSAGE_MAP()

protected:
	ULONG_PTR m_gdiplusToken;
	UINT      m_nLangDelta;

public:
	CString   m_strModuleFilePath;
};

extern CTestAnimationApp theApp;