// RadioMenuDemoDoc.h : interface of the CRadioMenuDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOMENUDEMODOC_H__0E92A7C2_BF22_4163_BF7B_23A4025BE2D6__INCLUDED_)
#define AFX_RADIOMENUDEMODOC_H__0E92A7C2_BF22_4163_BF7B_23A4025BE2D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRadioMenuDemoDoc : public CDocument
{
protected: // create from serialization only
	CRadioMenuDemoDoc();
	DECLARE_DYNCREATE(CRadioMenuDemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioMenuDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRadioMenuDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRadioMenuDemoDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOMENUDEMODOC_H__0E92A7C2_BF22_4163_BF7B_23A4025BE2D6__INCLUDED_)
