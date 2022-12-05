// 오른쪽만 뚫려있는 bounce1

#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ThreadFunc(LPVOID prc);
void DoCreateMain(HWND hWnd);
void DoButtonMain(HWND hWnd);
void DoPaintMain(HWND hWnd);
void OnTimer(HWND hWnd);

//공의 크기, 스틱의 크기(길이 넓이)
#define R 20 
#define BOUNCE 1
#define W 20
#define H 20

HBITMAP hBit;
HINSTANCE g_hInst;
HWND hWndMain, hWnd2;
HANDLE hThread;
//내가 만드는 프로그램의 제목, 윈도우 창 제목
LPCTSTR lpszClass = TEXT("Bounce3");

int flag = 0;
int btnflag = 0;

typedef struct trans_struct {
	RECT crt;
	int x;
	int y;
	int xv;
	int yv;
	int flag1;
	int flag2;
	int flag3;
}trans_struct, * Ptrans_struct;

trans_struct value;
Ptrans_struct Pv;

void moveBall();
void collision(HWND hWnd);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	//윈도우 프로그램 만드는 기본, 템플릿이라고 생각
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_BORDER | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 600,	//윈도우 크기
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	hWndMain = hWnd;

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//이렇게 만들어 놓으면 여기가 반복적으로 돌아가게 된다. 무한루프와 비슷
	COPYDATASTRUCT* pcd;

	switch (iMessage) {

	case WM_CREATE:
		DoCreateMain(hWnd);
		break;
	case WM_PAINT:
		DoPaintMain(hWnd);
		break;
	case WM_LBUTTONDOWN:
		DoButtonMain(hWnd);
		break;
	case WM_COPYDATA:
		pcd = (COPYDATASTRUCT*)lParam;
		Pv = (Ptrans_struct)pcd->lpData;
		value.flag3 = Pv->flag3;
		if (value.flag3 != 2) break;
		value.x =  R;
		value.crt = Pv->crt;
		value.y = Pv->y;
		value.xv = Pv->xv;
		value.yv = Pv->yv;
		value.flag3 = 1;
		break;
	case WM_TIMER:
		OnTimer(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		KillTimer(hWnd, 0);
		CloseHandle(hThread);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void DoCreateMain(HWND hWnd)
{
	GetClientRect(hWnd, &value.crt);

	value.x = 30;	//공의 x 좌표 초기값
	value.y = -30;//공의 y 좌표 초기값

	value.xv = 0; //(rand() % 4) + 5; //x벡터 속도
	value.yv = 0;//(rand() % 4) + 5; //y벡터 속도
}

void DoPaintMain(HWND hWnd)
{
	//타이머를 띄우는 곳
	HDC hdc, hMemDC;
	PAINTSTRUCT ps;
	HBITMAP OldBit;

	hdc = BeginPaint(hWnd, &ps);

	//화면이 바뀌었을 때, 새로 그리는 부분
	hMemDC = CreateCompatibleDC(hdc);
	OldBit = (HBITMAP)SelectObject(hMemDC, hBit);
	BitBlt(hdc, 0, 0, value.crt.right, value.crt.bottom, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);
	EndPaint(hWnd, &ps);
	return;
}

void DoButtonMain(HWND hWnd)
{
	//만약 초기에 실행했으면 안내창을 띄워주는 곳
	if (btnflag == 0) {
		MessageBox(hWnd, TEXT("프로그램이 실행되었습니다."), TEXT("OK"), MB_OK);
		SetTimer(hWnd, 1, 25, NULL);
	}

}

void OnTimer(HWND hWnd)
{
	TCHAR str[123];
	HDC hdc, hMemDC;
	HBITMAP OldBit;
	HPEN hPen, OldPen;
	HBRUSH hBrush, OldBrush;

	hdc = GetDC(hWnd);
	GetClientRect(hWnd, &value.crt);
	if (hBit == NULL)
	{
		hBit = CreateCompatibleBitmap(hdc, value.crt.right, value.crt.bottom);
	}
	hMemDC = CreateCompatibleDC(hdc);
	OldBit = (HBITMAP)SelectObject(hMemDC, hBit);

	FillRect(hMemDC, &value.crt, GetSysColorBrush(COLOR_WINDOW));

	hPen = CreatePen(PS_INSIDEFRAME, 2, RGB(0, 0, 0));
	OldPen = (HPEN)SelectObject(hMemDC, hPen);
	hBrush = CreateSolidBrush(RGB(255, 120, 0));
	OldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
	Ellipse(hMemDC, value.x - R, value.y - R, value.x + R, value.y + R);	 //공그리기


	hBrush = CreateSolidBrush(RGB(255, 255, 0));
	OldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);


	DeleteObject(SelectObject(hMemDC, OldPen));
	DeleteObject(SelectObject(hMemDC, OldBrush));

	wsprintf(str, TEXT("%d, %d, %d, %d"), value.x, value.y, value.xv, value.yv);
	TextOut(hMemDC, 100, 50, str, lstrlen(str));


	SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);
	ReleaseDC(hWnd, hdc);
	InvalidateRect(hWnd, NULL, FALSE);
	moveBall();  //공의 움직임
	collision(hWnd);



}


void moveBall()
{
	if (value.flag3 == 1) {
		if (value.y <= R || value.y >= value.crt.bottom - R) {
			value.yv *= -(BOUNCE);
			if (value.y <= R) value.y = R;
			if (value.y >= value.crt.bottom - R) value.y = value.crt.bottom - R;
		}
		value.x += (int)value.xv;
		value.y += (int)value.yv;
	}
	else {
		value.xv = 0;
		value.yv = 0;
	}
}

void collision(HWND hWnd)
{
	if (value.x > value.crt.right && value.flag3 == 1) {
		value.x -= R;
		value.xv *= (-BOUNCE);
	}
	else if (value.x < value.crt.left - R && value.flag3 == 1) {
		COPYDATASTRUCT copyData;
		value.flag2 = 2;
		value.flag3 = -1;
		copyData.dwData = 2;
		copyData.cbData = sizeof(value);
		copyData.lpData = &value;
		HWND hBounce = FindWindow(NULL, TEXT("Bounce2"));
		SendMessage(hBounce, WM_COPYDATA, (WPARAM)hWnd, &copyData);
	}
}

