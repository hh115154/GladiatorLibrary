// StaticSplitWnd3.h : main header file for the StaticSplitWnd3 application
//

#if !defined(AFX_StaticSplitWnd3_H__A27EEC79_5A32_11D6_8F32_00E04CE76240__INCLUDED_)
#define AFX_StaticSplitWnd3_H__A27EEC79_5A32_11D6_8F32_00E04CE76240__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CStaticSplitWnd3App:
// See StaticSplitWnd3.cpp for the implementation of this class
//

class CStaticSplitWnd3App : public CWinApp
{
public:
	CStaticSplitWnd3App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticSplitWnd3App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CStaticSplitWnd3App)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_StaticSplitWnd3_H__A27EEC79_5A32_11D6_8F32_00E04CE76240__INCLUDED_)
