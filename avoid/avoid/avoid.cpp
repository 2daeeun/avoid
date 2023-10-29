//                   _ooOoo_                                       .======.
//                  o8888888o                                      |      |
//                  88" . "88                             .========'      '========.
//                  (| -_- |)                             |   _      xxxx      _   |
//                  O\  =  /O                             |  /_;-.__ / _\  _.-;_\  |
//               ____/`---'\____                          |     `-._`'`_/'`.-'     |
//             .'  \\|     |//  `.                        '========.`\   /`========'
//            /  \\|||  :  |||//  \								   | |  / |
//           /  _||||| -:- |||||-  \                               |/-.(  |
//           |   | \\\  -  /// |   |                               |\_._\ |
//           | \_|  ''\---/''  |   |                               | \ \`;|
//           \  .-\__  `-`  ___/-. /                               |  > |/|
//         ___`. .'  /--.--\  `. . __                              | / // |
//      ."" '<  `.___\_<|>_/___.'  >'"".                           | |//  |
//     | | :  `- \`.;`\ _ /`;.`/ - ` : | |                         | \(\  |
//     \  \ `-.   \_ __\ /__ _/   .-` /  /                         |  ``  |
//======`-.____`-.___\_____/___.-`____.-'======        \\jgs _  _\\| \//  |//_   _ \// _            
//                                              
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//                       이 코드에 부처님과 예수님의 은총이 깃들기를
//                                  Please have no BUG!
//
// avoid.cpp : 애플리케이션에 대한 진입점을 정의합니다.
#include <windows.h>
#include "framework.h"
#include "avoid.h"
//#include "Run.h"
#include <list>
#include <iostream>
#include <stdint.h>

#define MAX_LOADSTRING 100

using namespace std;

typedef struct _tabObject
{
	float left, top, right, bottom;
	float speed;
}PLAYER, OBJECT;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
// 직접 선언한 전역변수
HWND g_hWnd;
HDC g_hDC;
HDC g_memDC;
HBITMAP g_bitmap;
bool g_bLoop = true;                            // 리턴되는 메시지를 무한루프 돌리기 위한 것
PLAYER g_Player = { 375.f, 750.f, 425.f, 800.f, 300.f };	// Player가 생성되는 위치

list<OBJECT> objectlist;
float g_object1_cooltime = 0;
float g_object2_cooltime = 0;
float g_object3_cooltime = 0;
float g_object4_cooltime = 0;
int randnum = rand();
bool g_gameEnd = false;

// 시간 변수
LARGE_INTEGER g_tSecond;
LARGE_INTEGER g_tTime;
float g_fDeltaTime;
float g_fTimeScale = 1.f;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void Run();
void GenerateObject(list<OBJECT>& objectlist, float left, float top, float right, float bottom, float speed);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_AVOID, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	g_hDC = GetDC(g_hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AVOID));

	MSG msg;

	QueryPerformanceFrequency(&g_tSecond);  // 고해상도 타이머의 주기(1초당 진동 수)를 반환
	QueryPerformanceCounter(&g_tTime);      // 일정 지점 안에서 몇 번 진동 하였는가를 나타내는 함수

	/*
		// 기본 메시지 루프입니다:
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	*/

	// 이해 못함. 근데 어찌어찌 하니깐 돌아감
	// https://skmagic.tistory.com/282
	while (g_bLoop)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Run();
		}
	}
    ReleaseDC(g_hWnd, g_hDC);
	return (int)msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AVOID));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	// 배경색 변경 방법
	// wcex.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hWnd = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_memDC = GetDC(hWnd);
	g_bitmap = CreateCompatibleBitmap(g_memDC, 750, 900);
	g_hDC = CreateCompatibleDC(g_memDC);

	HBITMAP old_bitmap = (HBITMAP)SelectObject(g_hDC, g_bitmap);
	DeleteObject(old_bitmap);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

// 창 중앙에 고정 
void PlaceInCenterOfScreen(HWND window, DWORD style, DWORD exStyle)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	RECT clientRect;
	GetClientRect(window, &clientRect);

	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;

	SetWindowPos(window, NULL,
		screenWidth / 2 - clientWidth / 2,
		screenHeight / 2 - clientHeight / 2,
		clientWidth, clientHeight, 0
	);
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

{
	switch (message)
	{
		// 윈도우 크기 창 고정
	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 0;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 0;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 750;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 900;

		return FALSE;
	}
	
	case WM_CREATE:
		// 창 중앙에 고정
		PlaceInCenterOfScreen(hWnd, WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
		break;
	
	case WM_DESTROY:
		g_bLoop = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 델타타임(DeltaTime) 참조 (https://moai27.tistory.com/58)
// Game Loop 메커니즘 이해 참조 (https://luv-n-interest.tistory.com/1135)

void Run()
{
	// 이전 클록수
	LARGE_INTEGER tTime;
	// 이전 클록수 가져오기
	QueryPerformanceCounter(&tTime);

	 PatBlt(g_hDC, 0, 0, 750, 900, WHITENESS);

	// 호출1번당 이동시간 =	현재와 이전 클록수의 차이 / 1초당 클록수      ex) 36/360  
	g_fDeltaTime = (tTime.QuadPart - g_tTime.QuadPart) / (float)g_tSecond.QuadPart;
	// 이전시간을 지금시간으로 초기화해준다.
	g_tTime = tTime;

	// 플레이어 이동 설정
	float fSpeed = g_Player.speed * g_fDeltaTime * g_fTimeScale;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		g_Player.top -= fSpeed;
		g_Player.bottom -= fSpeed;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		g_Player.top += fSpeed;
		g_Player.bottom += fSpeed;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		g_Player.left -= fSpeed;
		g_Player.right -= fSpeed;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		g_Player.left += fSpeed;
		g_Player.right += fSpeed;
	}

	// object 생성
	g_object1_cooltime += g_fDeltaTime * g_fTimeScale;
	g_object2_cooltime += g_fDeltaTime * g_fTimeScale;
	g_object3_cooltime += g_fDeltaTime * g_fTimeScale;
	g_object4_cooltime += g_fDeltaTime * g_fTimeScale;

	if (g_object1_cooltime > ((randnum % 10) / (float)10)) {
		GenerateObject(objectlist, 800.f, 600.f, 850.f, 650.f, -500.f);
		g_object1_cooltime = 0;
		// srand(time(NULL));
		srand((int)time(NULL));
		randnum = rand();
	}
	if (g_object2_cooltime > ((randnum % 10) / (float)10)) {
		GenerateObject(objectlist, -50.f, 450.f, 0.f, 500.f, 500.f);
		g_object2_cooltime = 0;
		srand((int)time(NULL) * 2);
		randnum = rand();
	}
	if (g_object3_cooltime > ((randnum % 10) / (float)10)) {
		GenerateObject(objectlist, 800.f, 300.f, 850.f, 350.f, -500.f);
		g_object3_cooltime = 0;
		srand((int)time(NULL) * 3);
		randnum = rand();
	}
	if (g_object4_cooltime > ((randnum % 10) / (float)10)) {
		GenerateObject(objectlist, -50.f, 150.f, 0.f, 200.f, 500.f);
		g_object4_cooltime = 0;
		srand((int)time(NULL) * 4);
		randnum = rand();
	}

	//object 이동
	for (OBJECT& obj : objectlist)
	{
		float fObjSpeed = obj.speed * g_fDeltaTime * g_fTimeScale;
		obj.left += fObjSpeed;
		obj.right += fObjSpeed;
	}

	// 충돌
	for (OBJECT& obj : objectlist)
	{
		if (obj.left <= g_Player.right &&
			obj.right >= g_Player.left &&
			obj.top <= g_Player.bottom &&
			obj.bottom >= g_Player.top)
		{
			g_fTimeScale = 0;
			TextOut(g_hDC, 350, 400, TEXT("GAME OVER"), 9);
			TextOut(g_hDC, 335, 420, TEXT("SPACE to Restart"), 16);
			g_gameEnd = true;
		}
	}
	// 도착
	if (g_Player.top < 10.f)
	{
		g_fTimeScale = 0;
		TextOut(g_hDC, 370, 400, TEXT("CLEAR!!"), 7);
		TextOut(g_hDC, 335, 420, TEXT("SPACE to Restart"), 16);
		g_gameEnd = true;
	}
	// 재시작
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (g_gameEnd)
		{
			g_Player = { 375.f, 750.f, 425.f, 800.f, 300.f };
			g_gameEnd = false;
			g_fTimeScale = 1.f;
			Rectangle(g_hDC, 0.f, 0.f, 800.f, 800.f);
			objectlist.clear();
		}
	}

	Rectangle(g_hDC, g_Player.left, g_Player.top, g_Player.right, g_Player.bottom);
	Rectangle(g_hDC, 0.f, 0.f, 800.f, 10.f);

	// object
	for (OBJECT& obj : objectlist)
	{
		// 교수님이 Object의 색상들을 랜덤하게 바꾸게 해보라고 시켜서 야매로 함.
		// SelectObject(g_hDC, GetStockObject(rand() % 10));
		Rectangle(g_hDC, obj.left, obj.top, obj.right, obj.bottom);
	}

	  BitBlt(g_memDC, 0, 0, 750, 900, g_hDC, 0, 0, SRCCOPY);
}

void GenerateObject(list<OBJECT>& objectlist, float left, float top, float right, float bottom, float speed)
{
	// 전역변수의 left, top, right, bottom 햇가리는거 주의 하기
	OBJECT object;
	object.left = left;
	object.top = top;
	object.right = right;
	object.bottom = bottom;
	object.speed = speed;
	objectlist.push_back(object);
}
