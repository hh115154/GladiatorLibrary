// HtmlVIE.h : main header file for the HtmlVIE application
//

#if !defined(AFX_HtmlVIE_H__6CEE0C4A_7C95_42C0_BEEA_EA075E5FAE14__INCLUDED_)
#define AFX_HtmlVIE_H__6CEE0C4A_7C95_42C0_BEEA_EA075E5FAE14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CHtmlVIEApp:
// See HtmlVIE.cpp for the implementation of this class
//

class CHtmlVIEApp : public CWinApp
{
public:
	CHtmlVIEApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlVIEApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CHtmlVIEApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HtmlVIE_H__6CEE0C4A_7C95_42C0_BEEA_EA075E5FAE14__INCLUDED_)
