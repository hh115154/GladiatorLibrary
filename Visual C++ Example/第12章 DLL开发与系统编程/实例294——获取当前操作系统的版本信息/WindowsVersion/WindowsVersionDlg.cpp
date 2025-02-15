// WindowsVersionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsVersion.h"
#include "WindowsVersionDlg.h"

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
// CWindowsVersionDlg dialog

CWindowsVersionDlg::CWindowsVersionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWindowsVersionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWindowsVersionDlg)
	m_strInfo = _T("");
	m_strServerPack = _T("");
	m_strBuildNum = _T("");
	m_strMinorVer = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWindowsVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWindowsVersionDlg)
	DDX_Text(pDX, IDC_EDIT1, m_strInfo);
	DDX_Text(pDX, IDC_EDIT4, m_strServerPack);
	DDX_Text(pDX, IDC_EDIT3, m_strBuildNum);
	DDX_Text(pDX, IDC_EDIT2, m_strMinorVer);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWindowsVersionDlg, CDialog)
	//{{AFX_MSG_MAP(CWindowsVersionDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GETVERSION, OnGetversion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowsVersionDlg message handlers

BOOL CWindowsVersionDlg::OnInitDialog()
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

void CWindowsVersionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWindowsVersionDlg::OnPaint() 
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
HCURSOR CWindowsVersionDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWindowsVersionDlg::OnGetversion() 
{
	// TODO: Add your control notification handler code here
	int stat = 0;
	char buffer[20];
	OSVERSIONINFO versionInfo;
	//设定OSVERSIONINFO的大小
	versionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	// 获得版本信息
	if (!::GetVersionEx (&versionInfo)) 
	{
		m_strInfo = (_T ("不能获取操作系统版本"));
		return;
	}
	// 检查版本号
	if (versionInfo.dwMajorVersion == 5 && versionInfo.dwMinorVersion == 2)
	{
		m_strMinorVer = _T ("5.2");
		m_strInfo =  (_T ("Windows Server 2003"));
	}
	else if(versionInfo.dwMajorVersion == 5&&versionInfo.dwMinorVersion == 1)
	{
		m_strMinorVer = _T ("5.1");
		m_strInfo =  (_T ("Windows XP"));
	}
	else if(versionInfo.dwMajorVersion == 5&&versionInfo.dwMinorVersion == 0)
	{
		m_strMinorVer = _T ("5.0");
		m_strInfo =  (_T ("Windows 2000"));
	}
	else if(versionInfo.dwMajorVersion == 4&&versionInfo.dwMinorVersion == 0)
	{
		m_strMinorVer = _T ("4.0");
		m_strInfo =  (_T ("Windows NT 4.0"));
	}
	else if(versionInfo.dwMajorVersion==4&&versionInfo.dwMinorVersion == 90)
	{
		m_strMinorVer = _T ("4.90");
		m_strInfo =  (_T ("Windows Me"));
	}
	else if(versionInfo.dwMajorVersion == 4 && versionInfo.dwMinorVersion == 10)
	{
		m_strMinorVer = _T ("4.10");
		m_strInfo =  (_T ("Windows 98"));
	}
	// 获得创建号
	_itoa( versionInfo.dwBuildNumber, buffer, 10 );
	m_strBuildNum = CString (buffer);
	// 获得service pack信息
	m_strServerPack = CString (versionInfo.szCSDVersion);
	UpdateData(false);
}
