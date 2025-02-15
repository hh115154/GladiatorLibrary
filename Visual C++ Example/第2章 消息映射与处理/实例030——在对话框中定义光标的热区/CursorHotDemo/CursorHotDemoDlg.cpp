// CursorHotDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CursorHotDemo.h"
#include "CursorHotDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCursorHotDemoDlg dialog

CCursorHotDemoDlg::CCursorHotDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCursorHotDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCursorHotDemoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCursorHotDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCursorHotDemoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCursorHotDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CCursorHotDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCursorHotDemoDlg message handlers

BOOL CCursorHotDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCursorHotDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCursorHotDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCursorHotDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CCursorHotDemoDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	switch(pWnd->GetDlgCtrlID()) // 得到鼠标所在位置的控件的ID号
	{
		case IDC_BUTTON1:    // 鼠标位于按钮空间区域内
		{
			// 设置鼠标指针为"手"形指针
			SetCursor(AfxGetApp()->LoadCursor(IDC_MYHAND)); 
			// 将提示文字可见
			GetDlgItem(IDC_TEXT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TEXT)->SetWindowText("光标所在位置为按钮框!");
			return TRUE;
		}
		break;
		case IDC_EDIT1:    // 鼠标位于编辑框区域内
		{
			// 设置鼠标指针为"手"形指针
			SetCursor(AfxGetApp()->LoadCursor(IDC_MYHAND)); 
			// 将提示文字可见
			GetDlgItem(IDC_TEXT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TEXT)->SetWindowText("光标所在位置为编辑框!");
			return TRUE;
		}
		break;
		case IDC_MONTHCALENDAR1:    // 鼠标位于日历控件区域内
		{
			// 设置鼠标指针为"手"形指针
			SetCursor(AfxGetApp()->LoadCursor(IDC_MYHAND)); 
			// 将提示文字可见
			GetDlgItem(IDC_TEXT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TEXT)->SetWindowText("光标所在位置为日历控件!");
			return TRUE;
		}
		break;
	default:    // 鼠标离开了"热点"
		{
			// 将提示性文字隐藏
			GetDlgItem(IDC_TEXT)->ShowWindow(SW_HIDE);
			// 将鼠标指针设置为标准的鼠标指针
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); 
			return TRUE;
		}
	}
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
