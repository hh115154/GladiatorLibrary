// MyHtmlDlg.h : main header file for the MYHTMLDLG application
//

#if !defined(AFX_MYHTMLDLG_H__8E58A1BE_0EA4_4D5B_A7CD_319D1832F02B__INCLUDED_)
#define AFX_MYHTMLDLG_H__8E58A1BE_0EA4_4D5B_A7CD_319D1832F02B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMyHtmlDlgApp:
// See MyHtmlDlg.cpp for the implementation of this class
//

class CMyHtmlDlgApp : public CWinApp
{
public:
	CMyHtmlDlgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyHtmlDlgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMyHtmlDlgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYHTMLDLG_H__8E58A1BE_0EA4_4D5B_A7CD_319D1832F02B__INCLUDED_)
