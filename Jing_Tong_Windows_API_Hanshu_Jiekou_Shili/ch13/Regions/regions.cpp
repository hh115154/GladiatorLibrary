/* ************************************
*《精通Windows API》
* 示例代码
* Regions.c
* 13.8 区域的填充和反转
**************************************/
/* 头文件 */
#include <windows.h>
/* 全局变量 */
LPSTR szAppName = "RGN";
LPSTR szTitle = "RGN Sample";
/* 函数声明 */
LRESULT CALLBACK WndProc( HWND ,  UINT ,  WPARAM ,  LPARAM );

/*************************************
* ElliRgns
* 功能	创建椭圆区域，并进行填充和反转
**************************************/
HRGN ElliRgns(HWND hwnd, POINT point)
{
	// RECT 区域、画刷
	RECT rect, rectClient;
	HRGN hrgn;
	HBRUSH hBrushOld, hBrush;
	// DC
	HDC hdc = GetDC(hwnd);
	GetClientRect(hwnd,&rectClient);
	// 点的周围一块区域
	rect.left = point.x - 40;
	rect.right = point.x + 40;
	rect.top = point.y - 30;
	rect.bottom = point.y + 30;
	// 通过RECT 创建椭圆区域
	hrgn = CreateEllipticRgnIndirect(&rect);
	// 创建画刷
	hBrush = CreateSolidBrush(RGB(0,255,0));
	// 为DC 选择画刷
	hBrushOld = (HBRUSH)SelectObject(hdc,hBrush);
	// 使用当前画刷绘制区域
	PaintRgn(hdc,hrgn);
	// 等待一断时间后，将刚才绘制的区域进行反转
	Sleep(3000);
	InvertRgn(hdc,hrgn);
	// 释放资源
	hBrush = (HBRUSH)SelectObject(hdc,hBrushOld);
	DeleteObject(hBrush);
	DeleteObject(hrgn);
	DeleteDC( hdc );
	return 0;
}

int WINAPI WinMain(
				   HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	MSG msg;
	HWND hWnd;
	WNDCLASS wc;

	wc.style = CS_OWNDC; 
	wc.lpfnWndProc = (WNDPROC)WndProc; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = hInstance;
	wc.hIcon = NULL; 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName; 

	if (!RegisterClass(&wc))
	{
		return (FALSE);
	}

	hWnd = CreateWindow(
		szAppName, 
		szTitle, 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 
		NULL, 	NULL,	hInstance, NULL 
		);

	if (!hWnd)
	{
		return (FALSE);
	}

	ShowWindow(hWnd, nCmdShow); 
	UpdateWindow(hWnd); 

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)(msg.wParam);

	UNREFERENCED_PARAMETER(lpCmdLine); 
}


LRESULT CALLBACK WndProc(
						 HWND hWnd, 
						 UINT message, 
						 WPARAM uParam, 
						 LPARAM lParam)
{

	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
		break;

	case WM_LBUTTONDOWN:
		{
			// 获得点击的位置，传递给ElliRgns
			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			ElliRgns(hWnd, point);
		}
		break;

	case WM_DESTROY: 
		PostQuitMessage(0);
		break;

	default: 
		return (DefWindowProc(hWnd, message, uParam, lParam));
	}
	return (0);
}