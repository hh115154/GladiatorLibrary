// FormViewDemoDoc.cpp : implementation of the CFormViewDemoDoc class
//

#include "stdafx.h"
#include "FormViewDemo.h"

#include "FormViewDemoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoDoc

IMPLEMENT_DYNCREATE(CFormViewDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CFormViewDemoDoc, CDocument)
	//{{AFX_MSG_MAP(CFormViewDemoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoDoc construction/destruction

CFormViewDemoDoc::CFormViewDemoDoc()
{
	// TODO: add one-time construction code here

}

CFormViewDemoDoc::~CFormViewDemoDoc()
{
}

BOOL CFormViewDemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoDoc serialization

void CFormViewDemoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoDoc diagnostics

#ifdef _DEBUG
void CFormViewDemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFormViewDemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoDoc commands
