// PopUpMenuDoc.h : interface of the CPopUpMenuDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_POPUPMENUDOC_H__55886620_7D41_445E_A5FD_965C51A1972D__INCLUDED_)
#define AFX_POPUPMENUDOC_H__55886620_7D41_445E_A5FD_965C51A1972D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPopUpMenuDoc : public CDocument
{
protected: // create from serialization only
	CPopUpMenuDoc();
	DECLARE_DYNCREATE(CPopUpMenuDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopUpMenuDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPopUpMenuDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPopUpMenuDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POPUPMENUDOC_H__55886620_7D41_445E_A5FD_965C51A1972D__INCLUDED_)
