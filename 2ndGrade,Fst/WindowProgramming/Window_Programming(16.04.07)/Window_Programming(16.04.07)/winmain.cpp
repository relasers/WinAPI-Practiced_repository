#include<Windows.h>
#include<Tchar.h> 
#include<time.h>


#define BODY_SIZE 20 // 애벌레 몸체의 사이즈다
#define MORE_SIZE 8 // 커다란 적의 머리

#define CLIENT_WIDTH 800 // 클라이언트 가로크기
#define CLIENT_HEIGHT 800 // 클라이언트 세로크기
#define MX_SPEED 10 // 벌레 최대속도 / 작을수록 빠르다
#define MN_SPEED 200 // 벌레 최소속도 / 클수록 느리다
#define DEFAULT_SPEED 70 // 벌레의 초기 속도

#define E_SPEED 100 // 적 애벌레의 속도 

#define S_OFF 0 // 스위치 꺼짐
#define S_ON 1 // 스위치 켜짐

#define JUMP_POWER 10 // 점프

typedef struct {
	int hx; //머리 (head_x)
	int hy; //머리 (head_y)
	int tx; // 꼬리 (tail_x)
	int ty; // 꼬리 (tail_y)
}Warm;

typedef enum {
	Left, Right, Up, Down
}Direction;

typedef struct {
	int jump;
	int direction;
}Jumping;

HINSTANCE g_hInst;
LPCTSTR IpszClass = L"Window Class 3-1"; // 이게 빨간줄이 나오면 유니코드 문제때문에 그렇다
										 // L"문자열" 문자열 앞에 대문자 L을 붙인다 아니면 유니코드에서 멀티바이트로 바꾸던가
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpszCmdParam, int nCmdShow) { // WinMain 대소문자 준수할 것!
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass; // EX 는 붙여도 되고 안붙여도 된다.
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW; // 윈도우 출력 스타일
	WndClass.lpfnWndProc = (WNDPROC)WndProc; // 왠만해선 이름은 그대로 쓰자
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //  기본적으로 흑,회,화 지원 빨강 이런거 만들려면 색깔 직접 만들어아함
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = IpszClass; // 이렇게 정해놨으면 이후에도 똑같은 이름, 이거로 써라
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // 클래스 12개// 반드시 12개 다 써야한다
	RegisterClassEx(&WndClass);
	// API가 지원해주는 함수
	hWnd = CreateWindow( //윈도우 핸들값 반환
		IpszClass, //윈도우 클래스 이름
		IpszClass, //윈도우 타이틀 이름
		WS_OVERLAPPEDWINDOW, //윈도우 스타일
		CW_USEDEFAULT, //윈도우 위치 x
		CW_USEDEFAULT, //윈도우 위치 y
		CLIENT_WIDTH, // 윈도우 가로크기
		CLIENT_HEIGHT, //윈도우 세로크기
		NULL, //부모 윈도우 핸들
		(HMENU)NULL, //메뉴핸들
		hInstance,//응용프로그램 인스턴스
		NULL);//생성윈도우 정보

	ShowWindow(hWnd, nCmdShow); // 윈도우 화면 출력
	UpdateWindow(hWnd); // O/S에 WM_PAINT  메시지 전송 

	while (GetMessage(&Message, 0, 0, 0)) { // 윈도우 프로시져에서 PostQuitMessage() 호출 때까지 처리 
		TranslateMessage(&Message);  // ex) Shift ‘a’→ 대문자 ‘A’ 
		DispatchMessage(&Message);  // WinMain → WinProc 
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {

	srand((unsigned)time(NULL));
	PAINTSTRUCT ps;
	HDC hdc; // 항상 첫번째 인자는 hDC 다! 
	HPEN hPen, oldPen;
	HBRUSH hBrush, oldBrush;

	static int direction = Direction::Right; // 애벌레의 이동방향 초기는 오른쪽이다.
	static RECT rectView; // 창 크기를 저장하는 RECT 구조체

	static Warm warm; // 애벌레의 초기 위치
	static Warm temp; // 창 끝에 닿았을때 머리와 꼬리를 맞바꾸기위한 임시 위치 저장 값

	static Warm enemy;


	static int speed = DEFAULT_SPEED; // 애벌레의 초기 속도

	static Jumping jumping = {0,Direction::Right}; // 점프 구조체 선언

	static int enemy_created = S_OFF; // 적이 생성되었다


	srand((unsigned)time(NULL));
	//유니코드를 사용할 떄는 char 대신 wchar_t 를 쓰고 문자열 앞에 L를 붙인다

	switch (iMessage) {// 메시지 번호// case: 메시지에 따라 처리  
	case WM_CREATE:
		rectView = { 0,0,LOWORD(lParam),HIWORD(lParam) };
		GetClientRect(hWnd, &rectView);
		warm = { BODY_SIZE * 10 , BODY_SIZE * 10 , warm.hx - BODY_SIZE , warm.hy - BODY_SIZE };
		enemy = { BODY_SIZE * 2 , BODY_SIZE * 2 , enemy.hx - BODY_SIZE , enemy.hy - BODY_SIZE };
		SetTimer(hWnd, 1, speed, NULL); // 첫번쨰 애벌레의 속도
		SetTimer(hWnd, 2, 5000, NULL); // 적 애벌레가 생성된다 // 5000ms 초 뒤에 적이 생성된다
		return 0;
////////////////////////////////////////////////////////////////////////////////////
	case WM_TIMER:
		switch (wParam) {
		case 1: // 내 벌레가 움직인다
			if (0 < jumping.jump) {
				if (direction == Direction::Right || direction == Direction::Left) {
					if (JUMP_POWER/2 < jumping.jump) {
						warm.tx = warm.hx;
						warm.ty = warm.hy;
						warm.hy -= BODY_SIZE;
						enemy.tx = enemy.hx;
						enemy.ty = enemy.hy;
						enemy.hy -= BODY_SIZE;
					}
					else {
						warm.tx = warm.hx;
						warm.ty = warm.hy;
						warm.hy += BODY_SIZE;
						enemy.tx = enemy.hx;
						enemy.ty = enemy.hy;
						enemy.hy += BODY_SIZE;
					}
					jumping.jump--;

					if (warm.hy < 0)
						warm.hy = 0;
					if (enemy.hy < 0)
						enemy.hy = 0;
				}
				else if (direction == Direction::Down || direction == Direction::Up) {
					if (JUMP_POWER/2 < jumping.jump) {
						warm.tx = warm.hx;
						warm.ty = warm.hy;
						warm.hx += BODY_SIZE;
						enemy.tx = enemy.hx;
						enemy.ty = enemy.hy;
						enemy.hx += BODY_SIZE;
					}
					else {
						warm.tx = warm.hx;
						warm.ty = warm.hy;
						warm.hx -= BODY_SIZE;
						enemy.tx = enemy.hx;
						enemy.ty = enemy.hy;
						enemy.hx -= BODY_SIZE;
					}
					jumping.jump--;

					if (rectView.right < warm.hx + BODY_SIZE*2)
						warm.hx = rectView.right - BODY_SIZE;
					if (rectView.right < enemy.hx + BODY_SIZE*2)
						enemy.hx = rectView.right - BODY_SIZE;
				}
			}
			

			switch (direction) {
			case Direction::Up:
				if (warm.hy <= 0) {
					temp = { warm.hx,warm.hy,0,0 };
					warm = { warm.tx,warm.ty,temp.hx,temp.hy };
					direction = Direction::Down;
				}
				else {
					warm.tx = warm.hx;
					warm.ty = warm.hy;
					warm.hy -= BODY_SIZE;
					if (warm.hy <= 0) {
						warm.hy = 0;
						warm.ty = BODY_SIZE;
					}
				}
				break;
			case Direction::Down:
				if (rectView.bottom <= warm.hy + BODY_SIZE * 2) {
					temp = { warm.hx,warm.hy,0,0 };
					warm = { warm.tx,warm.ty,temp.hx,temp.hy };
					direction = Direction::Up;
				}
				else {
					warm.tx = warm.hx;
					warm.ty = warm.hy;
					warm.hy += BODY_SIZE;
					if (rectView.bottom <= warm.hy + BODY_SIZE) {
						warm.hy = rectView.bottom - BODY_SIZE;
						warm.ty = warm.hy - BODY_SIZE;
					}
				}

				break;
			case Direction::Left:
				if (warm.hx <= 0) {
					temp = { warm.hx,warm.hy,0,0 };
					warm = { warm.tx,warm.ty,temp.hx,temp.hy };
					direction = Direction::Right;
				}
				else {
					warm.tx = warm.hx;
					warm.ty = warm.hy;
					warm.hx -= BODY_SIZE;

					if (warm.hx < 0) { // 머리가 왼쪽 벽을 넘어가면 0 으로 재조정해준다
						warm.hx = 0;
						warm.tx = BODY_SIZE;
					}
				}

				break;
			case Direction::Right:
				if (rectView.right <= warm.hx + BODY_SIZE) {
					temp = { warm.hx,warm.hy,0,0 };
					warm = { warm.tx,warm.ty,temp.hx,temp.hy };
					direction = Direction::Left;
				}
				else {
					warm.tx = warm.hx;
					warm.ty = warm.hy;
					warm.hx += BODY_SIZE;
					if (rectView.right <= warm.hx + BODY_SIZE) { // 머리가 오른쪽 벽을 넘어가면 벽-BODY_SIZE 로 위치를 재조정한다
						warm.hx = rectView.right-BODY_SIZE;
						warm.tx = warm.hx - BODY_SIZE;
					}
				}
			}
			break;
		case 2: // 타이머 2를 죽인후 적 애벌레 타이머를 불러온다
			KillTimer(hWnd, 2);
			enemy_created = S_ON;
			SetTimer(hWnd, 3, E_SPEED, NULL); // 적 애벌레가 생성된다
			break;
		case 3: // 적 애벌레가 네놈을 추격하는 부분
			enemy.tx = enemy.hx;
			enemy.ty = enemy.hy;
			if (enemy.hx < warm.hx) {
				enemy.hx += BODY_SIZE;
			}// 적보다 내가 오른쪽에 있다!

			if (warm.hx < enemy.hx) {
				enemy.hx -= BODY_SIZE;

			}// 적보다 내가 왼쪽에 있다!
			if (enemy.hy < warm.hy) {
				enemy.hy += BODY_SIZE;
			}// 적보다 내가 아래쪽에 있다!

			if (warm.hy < enemy.hy) {
				enemy.hy -= BODY_SIZE;
			}// 적보다 내가 위쪽에 있다!
			break;
	
		}
		
		InvalidateRect(hWnd, &rectView, TRUE);
		break;
//////////////////////////////////////////////////////////////////////////////////////////
	case WM_KEYDOWN:
		hdc = GetDC(hWnd);
		if (wParam == VK_UP) {
			direction = Direction::Up;
		}
		else if (wParam == VK_DOWN) {
			direction = Direction::Down;
		}
		else if (wParam == VK_LEFT) {
			direction = Direction::Left;
		}
		else if (wParam == VK_RIGHT) {
			direction = Direction::Right;
		}

		ReleaseDC(hWnd, hdc);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_CHAR:
		hdc = GetDC(hWnd);
		if (wParam == '+') {
			if (MX_SPEED <= speed) {
				speed -= 10;
				KillTimer(hWnd, 1);
				SetTimer(hWnd, 1, speed, NULL); // 첫번쨰 애벌레의 속도
				SendMessage(hWnd,WM_TIMER,1,0);
			}
		}
		else if (wParam == '-') {
			if (speed <= MN_SPEED) {
				speed += 10;
				KillTimer(hWnd, 1);
				SetTimer(hWnd, 1, speed, NULL); // 첫번쨰 애벌레의 속도
				SendMessage(hWnd, WM_TIMER, 1, 0);
			}
		}
		else if (wParam == VK_SPACE && jumping.jump == 0) {
			jumping.jump = JUMP_POWER;
			jumping.direction = direction;
		}
		ReleaseDC(hWnd, hdc);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT: //윈도우 다시 그려줘야 할 때
		hdc = BeginPaint(hWnd, &ps); // WM_PAINT 메세지에서 사용 , 이 외에서 쓸떄는 GetDC(),ReleaseDC() 를 쓴다(잠시 출력할때)
/*   --------------     나 의 에 벌 레 ^ ^                 ------------   */
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		oldPen = (HPEN)SelectObject(hdc, hPen); // 새로운 펜 사용 선언
		hBrush = CreateSolidBrush(RGB(50, 130, 255));
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Ellipse(hdc, warm.hx, warm.hy, warm.hx + BODY_SIZE, warm.hy + BODY_SIZE); // 머리를 그린다

		SelectObject(hdc, oldBrush);
		SelectObject(hdc, oldPen);
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
		oldPen = (HPEN)SelectObject(hdc, hPen); // 새로운 펜 사용 선언
		hBrush = CreateSolidBrush(RGB(150, 200, 255));
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Ellipse(hdc, warm.tx, warm.ty, warm.tx + BODY_SIZE, warm.ty + BODY_SIZE); // 꼬리를 그린다
		SelectObject(hdc, oldBrush);
		SelectObject(hdc, oldPen);
/*   --------------     적 의 에 벌 레  - - !              ------------   */
		if (S_OFF<enemy_created ) { // 적 생성 트리거가 발동했을때
			hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			oldPen = (HPEN)SelectObject(hdc, hPen); // 새로운 펜 사용 선언
			hBrush = CreateSolidBrush(RGB(255, 0, 0));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			Ellipse(hdc, enemy.hx-MORE_SIZE, enemy.hy- MORE_SIZE, enemy.hx + BODY_SIZE+ MORE_SIZE, enemy.hy + BODY_SIZE+ MORE_SIZE); // 꼬리를 그린다
			SelectObject(hdc, oldBrush);
			SelectObject(hdc, oldPen);

			hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
			oldPen = (HPEN)SelectObject(hdc, hPen); // 새로운 펜 사용 선언
			hBrush = CreateSolidBrush(RGB(255, 255, 0));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			Ellipse(hdc, enemy.tx, enemy.ty, enemy.tx + BODY_SIZE, enemy.ty + BODY_SIZE); // 꼬리를 그린다
			SelectObject(hdc, oldBrush);
			SelectObject(hdc, oldPen);
		}


/*  ----------------------------------------------------------------------    */

		SelectObject(hdc, oldBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, oldPen);
		DeleteObject(hPen);

		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE: // 윈도우가 다시 그려질때
		GetClientRect(hWnd, &rectView);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		KillTimer(hWnd, 3);
		PostQuitMessage(0);
		return 0;
	} //처리할 메시지만 case문에 나열
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));// CASE에서 정의되지 않은 메시지는 커널이 처리하도록 메시지 전달 
}

