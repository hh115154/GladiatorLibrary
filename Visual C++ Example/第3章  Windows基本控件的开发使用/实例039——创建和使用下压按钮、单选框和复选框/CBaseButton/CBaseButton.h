// CBaseButton.h : main header file for the CBASEBUTTON application
//

#if !defined(AFX_CBASEBUTTON_H__C13789F5_E6B6_4F1C_809D_FEDF5AAC6C0C__INCLUDED_)
#define AFX_CBASEBUTTON_H__C13789F5_E6B6_4F1C_809D_FEDF5AAC6C0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCBaseButtonApp:
// See CBaseButton.cpp for the implementation of this class
//

class CCBaseButtonApp : public CWinApp
{
public:
	CCBaseButtonApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCBaseButtonApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCBaseButtonApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CBASEBUTTON_H__C13789F5_E6B6_4F1C_809D_FEDF5AAC6C0C__INCLUDED_)
