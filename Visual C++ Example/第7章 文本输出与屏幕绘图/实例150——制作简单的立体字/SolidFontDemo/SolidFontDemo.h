// SolidFontDemo.h : main header file for the SOLIDFONTDEMO application
//

#if !defined(AFX_SOLIDFONTDEMO_H__E9B1061C_323B_4614_9C10_FD3FB68CF258__INCLUDED_)
#define AFX_SOLIDFONTDEMO_H__E9B1061C_323B_4614_9C10_FD3FB68CF258__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSolidFontDemoApp:
// See SolidFontDemo.cpp for the implementation of this class
//

class CSolidFontDemoApp : public CWinApp
{
public:
	CSolidFontDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSolidFontDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSolidFontDemoApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOLIDFONTDEMO_H__E9B1061C_323B_4614_9C10_FD3FB68CF258__INCLUDED_)
