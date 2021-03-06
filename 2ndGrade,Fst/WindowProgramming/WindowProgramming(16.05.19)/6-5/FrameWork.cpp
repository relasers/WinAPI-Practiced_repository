#include "stdafx.h"
#include "FrameWork.h"


CFrameWork::CFrameWork()
{

}

CFrameWork::~CFrameWork()
{
	Destroy();
}
// 클래스 함수선언은 헤더에서 하는게 아니고 cpp 파일에 한다.
bool CFrameWork::Initial(HWND h, RECT r, HINSTANCE inst) {
	// 변수명 앞에 자료형 쓰면 그건 지역변수가 되어버린다
	m_hwnd = h;
	m_rect = r;
	m_hInst = inst;
	return m_hwnd != NULL; // 윈도우 생성 실패하면 윈도우 핸들이 NULL 값이 들어간다.
						   // 이값이 NULL값일 경우 윈도우가 제대로 만들어지지 않았다는것, 널값이면 윈도우를 죽여라!
}
void CFrameWork::Destroy() { // 반환값은 없어도 상관없다, 어쩌피 죽을테니까, 메모리 누수 방지
							 //소멸자로 소멸할때 Destroy 호출, 소멸자는 함수가 죽어야만 나온다 , 죽이고 다시 생성하고 싶을때 쓰자
							 // 대를 잇게 할때 -> 그떄를 위해 함수를 미리 빼 놓는다
	PostQuitMessage(0);
};

LRESULT CFrameWork::WindowMessage(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {

	switch (iMessage) {// 메시지 번호// case: 메시지에 따라 처리
	case WM_TIMER:
		OnTimer(wParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		InputMouse(iMessage, wParam, lParam);
		break;
		/////////////////////////////////////////
	case WM_CHAR:
	case WM_KEYDOWN:
	case WM_KEYUP:
		InputKeyBoard(iMessage, wParam, lParam);
		break;
		///////////////////////////////////////////
	case WM_PAINT: //윈도우 다시 그려줘야 할 때
		hdc = BeginPaint(hwnd, &ps); // WM_PAINT 메세지에서 사용 , 이 외에서 쓸떄는 GetDC(),ReleaseDC() 를 쓴다(잠시 출력할때)

		OnDraw(hdc); // hdc 를 OnDraw 로 넘긴다.

		EndPaint(hwnd, &ps);
		break;
	case WM_COMMAND:
		WindowMenu(hwnd, iMessage, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	} //처리할 메시지만 case문에 나열
	return(DefWindowProc(hwnd, iMessage, wParam, lParam));// CASE에서 정의되지 않은 메시지는 커널이 처리하도록 메시지 전달 
}

void CFrameWork::OnCreate() { // 나중에 3-1학기떄 BuildObject 로 쓴다 , 동적메모리 릴리즈 용
	GetClientRect(m_hwnd, &m_rect);
	SetTimer(m_hwnd, 1, 1, NULL);
}
void CFrameWork::OnDraw(HDC hdc) { //여기다 드로우를 처리한다

	HDC LayDC;
	LayDC = CreateCompatibleDC(hdc); // hdc와 호환시킨다.
	HBITMAP Lay; // 레이어 생성
	Lay = CreateCompatibleBitmap(hdc, m_rect.right, m_rect.bottom); // 다블 버퍼링 ,hdc와 호환시키자.
	SelectObject(LayDC, Lay);
	{
		ClearScreen(LayDC); // 화면 클리어
		MemDC = CreateCompatibleDC(LayDC); // 이건 비트맵 뿌리기용

		SetBkMode(LayDC, TRANSPARENT);
		//////<여기다 그린다>//////////////////////////////////////////////////////////////////



		//////////////////////////////////////////////////////////////////////
		BitBlt(hdc, 0, 0, m_rect.right, m_rect.bottom, LayDC, 0, 0, SRCCOPY);
	}


	DeleteObject(Lay);
	DeleteObject(hdc);

	DeleteDC(LayDC);
	DeleteDC(MemDC);
	DeleteDC(hdc);

}
void CFrameWork::OnTimer(UINT id) {
	switch (id) {
	case WT_COUNTING:
		break;
	}
	InvalidateRect(m_hwnd, &m_rect, FALSE);
}
void CFrameWork::InputMouse(UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_LBUTTONDOWN:
		InvalidateRect(m_hwnd, &m_rect, FALSE);
		break;
	}
}

void CFrameWork::InputKeyBoard(UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_KEYDOWN:
		DLGPROC dialogFunc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> INT_PTR { return framework.WindowDialogProc(hwnd, msg, wParam, lParam); };
		hDlg = CreateDialog(m_hInst,
			MAKEINTRESOURCE(IDD_DIALOG),
			m_hwnd, dialogFunc);
		ShowWindow(hDlg, SW_SHOW);
		break;
	}
	InvalidateRect(m_hwnd, &m_rect, FALSE);
}
void CFrameWork::CreateBkBrush(COLORREF color) {
	if (hbrBackground) DeleteObject(hbrBackground);
	hbrBackground = (HBRUSH)CreateSolidBrush(color);
}
void CFrameWork::ClearScreen(HDC hdc) {
	FillRect(hdc, &m_rect, hbrBackground);
}
LRESULT CFrameWork::WindowMenu(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (iMessage)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		}
		EndDialog(hwnd, LOWORD(wParam));
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

BOOL CFrameWork::WindowDialogProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	
	switch (iMessage)
	{
	case WM_INITDIALOG:
		hCombo = GetDlgItem(hwnd, IDC_COMBO_YEAR);
		hList = GetDlgItem(hwnd, IDC_LIST);
		CheckRadioButton(hwnd,
			IDC_RADIO_MAN, IDC_RADIO_WOMAN, IDC_RADIO_MAN);
		
		for (int i = 1900; i < 2016; ++i) {
			_itow_s(i,Temp,10);
			SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)Temp);
		}

		return (BOOL)true;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_RADIO_WOMAN:
			radio = 1;
			break;
		case IDC_RADIO_MAN:
			radio = 0;
			break;
		case IDC_SORT:

			for (int i = 0; i < 50; ++i) {
				for (int j = 0; j < 256; ++j) {
					Forsort[i][j] = '\0';
				}
			}

			for (int i = 0; i < index; ++i) {
				SendMessage(hList, LB_GETTEXT, i, (LPARAM)Forsort[i]);
			}
			for (int i = index; 0 < i; --i) {
				for (int j = 0; j < i; ++j) {
					if(Forsort[j + 1][0] != '\0')
					if (wcscmp(Forsort[j], Forsort[j+1]) < 0) { //오르내림 괂리
						for (int x = 0; x < 256; ++x) {
							Temp[x] = Forsort[j][x];
						}
						for (int x = 0; x < 256; ++x) {
							Forsort[j][x] = Forsort[j+1][x];
						}
						for (int x = 0; x < 256; ++x) {
							Forsort[j + 1][x] = Temp[x];
						}
					}

				}
			}
			for (int i = 0; i < index; ++i) {
				SendMessage(hList, LB_DELETESTRING, 0, 0);
			}
			for (int i = 0; i < index; ++i) {
				SendMessage(hList, LB_INSERTSTRING, 0, (LPARAM)Forsort[i]);
			}

			break;
		case IDC_NEW:
			SetDlgItemText(hwnd, IDC_EDIT_NAME, L"");
			SetDlgItemText(hwnd, IDC_EDIT_NUMBER, L"");
			SetDlgItemText(hwnd, IDC_COMBO_YEAR, L"");
			break;
		case IDC_JOIN:
			GetDlgItemText(hwnd, IDC_EDIT_NAME, member.name, sizeof(member.name));
			GetDlgItemText(hwnd, IDC_EDIT_NUMBER, member.number, sizeof(member.number));
			member.sex = radio;
			member.year = selection+1900;
			switch (member.sex) {
			case 0:
				wsprintf(Temp, TEXT("남자"));
				break;
			case 1:
				wsprintf(Temp, TEXT("여자"));
				break;
			}
			wsprintf(Buffer, TEXT("이름 : %s, 전번 : %s, 성별 : %s, 출생년도 : %d")
				,member.name,
				member.number,Temp,member.year);
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)Buffer);
			index++;
			break;
		case IDC_BAN:
			SendMessage(hList, LB_DELETESTRING, listselection, 0);
			if(index!=0)
			index--;
			break;
		case IDC_COMBO_YEAR: // 콤보박스가 눌려짐
			if (HIWORD(wParam) == CBN_SELCHANGE) // 하나가 선택됨(상태 변경)
				selection = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			break;

		case IDC_LIST: // 콤보박스가 눌려짐
			if (HIWORD(wParam) == LBN_SELCHANGE)
				listselection = SendMessage(hList, LB_GETCURSEL, 0, 0);
			break;
		case IDC_WIN_CLOSE:
			Destroy();
			break;
		}

		//EndDialog(hwnd, LOWORD(wParam));
		return (BOOL)true;
	}
	return (BOOL)false;
}