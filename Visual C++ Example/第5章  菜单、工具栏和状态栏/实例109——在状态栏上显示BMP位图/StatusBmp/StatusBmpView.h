// StatusBmpView.h : interface of the CStatusBmpView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSBMPVIEW_H__DEBE147D_9735_46E6_AB71_30CEE4C2C492__INCLUDED_)
#define AFX_STATUSBMPVIEW_H__DEBE147D_9735_46E6_AB71_30CEE4C2C492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CStatusBmpView : public CView
{
protected: // create from serialization only
	CStatusBmpView();
	DECLARE_DYNCREATE(CStatusBmpView)

// Attributes
public:
	CStatusBmpDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusBmpView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusBmpView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CStatusBmpView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in StatusBmpView.cpp
inline CStatusBmpDoc* CStatusBmpView::GetDocument()
   { return (CStatusBmpDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSBMPVIEW_H__DEBE147D_9735_46E6_AB71_30CEE4C2C492__INCLUDED_)
