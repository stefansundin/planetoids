#include "MyWinApp.h"
#include "MainFrame.h"

BOOL CMyWinApp::InitInstance()
{
	CMainFrame *pFrame=new CMainFrame;
	m_pMainWnd=pFrame;
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}
