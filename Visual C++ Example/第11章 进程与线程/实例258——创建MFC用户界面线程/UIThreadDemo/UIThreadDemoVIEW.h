// UIThreadDemoView.h : interface of the CUIThreadDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIThreadDemoVIEW_H__8FAD0B0A_9DF1_4292_A73E_4A316858530A__INCLUDED_)
#define AFX_UIThreadDemoVIEW_H__8FAD0B0A_9DF1_4292_A73E_4A316858530A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CUIThreadDemoView : public CView
{
protected: // create from serialization only
	CUIThreadDemoView();
	DECLARE_DYNCREATE(CUIThreadDemoView)

// Attributes
public:
	CUIThreadDemoDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUIThreadDemoView)
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
	virtual ~CUIThreadDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CUIThreadDemoView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUiThread();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in UIThreadDemoView.cpp
inline CUIThreadDemoDoc* CUIThreadDemoView::GetDocument()
   { return (CUIThreadDemoDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UIThreadDemoVIEW_H__8FAD0B0A_9DF1_4292_A73E_4A316858530A__INCLUDED_)
