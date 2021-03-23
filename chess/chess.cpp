// chess.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <vector>
#include "pieces.h"
#include "chess.h"

using namespace std;

#define MAX_LOADSTRING 100
#define IMAGES_FOLDER L".\\Images\\"

enum PIECE_TYPE { KING, QUEEN, BISHOP, PAWN, ROOK, KNIGHT };
enum PIECE_COLOR { WHITE, BLACK };
const int PIECE_STEP = 170, BOARD_STEP = 75, MAX_COL = 8, MAX_ROW = 8;

Piece KingB(KING, BLACK, 0, 0);
Piece QueenB(QUEEN, BLACK, PIECE_STEP * 1, 0);
Piece BishopB(BISHOP, BLACK, PIECE_STEP * 2, 0);
Piece RookB(ROOK, BLACK, PIECE_STEP * 4, 0);
Piece PawnB(PAWN, BLACK, PIECE_STEP * 5, 0);
Piece KnightB(KNIGHT, BLACK, PIECE_STEP * 3, 0);

Piece KingW(KING, WHITE, 0, PIECE_STEP);
Piece QueenW(QUEEN, WHITE, PIECE_STEP * 1, PIECE_STEP);
Piece BishopW(BISHOP, WHITE, PIECE_STEP * 2, PIECE_STEP);
Piece RookW(ROOK, WHITE, PIECE_STEP * 4, PIECE_STEP);
Piece PawnW(PAWN, WHITE, PIECE_STEP * 5, PIECE_STEP);
Piece KnightW(KNIGHT, WHITE, PIECE_STEP * 3, PIECE_STEP);

vector<Piece> Pieces;
Piece* Board[8][8];
static bool selectPiece = FALSE, moveToPiece = FALSE;
int player = WHITE;
int changePlayer = 1;
bool kingMovedW = FALSE, kingMovedB = FALSE, lRookMovedW = FALSE, rRookMovedB = FALSE, rRookMovedW = FALSE, lRookMovedB = FALSE;
bool checkType;

struct EnPassant
{
	Gdiplus::Point pos;
	bool status = FALSE;
	int dirX = 0;
	int dirY = 0;
} EnPassant1, EnPassant2;



void init_pieces();
void draw_board(HWND);
void draw_rect(HWND, int, int);
bool check_move(Piece*, Gdiplus::Point, Gdiplus::Point);
bool check_pawn(Gdiplus::Point, Gdiplus::Point);
bool check_knight(Gdiplus::Point, Gdiplus::Point);
bool check_bishop(Gdiplus::Point, Gdiplus::Point);
bool check_rook(Gdiplus::Point, Gdiplus::Point);
bool check_queen(Gdiplus::Point, Gdiplus::Point);
bool check_king(Gdiplus::Point, Gdiplus::Point);
bool check_enPassant(Gdiplus::Point, Gdiplus::Point);
bool check_castling(Gdiplus::Point, Gdiplus::Point);
bool check_enpassant(Gdiplus::Point, Gdiplus::Point);
bool check_check(Gdiplus::Point, Gdiplus::Point);

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

	init_pieces();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHESS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
	ULONG_PTR gdipToken;
	Gdiplus::GdiplusStartupInput GdiplusStartupInput;
	Gdiplus::GdiplusStartup
	(
		&gdipToken,
		&GdiplusStartupInput,
		nullptr
	);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHESS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHESS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHESS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
      CW_USEDEFAULT, 0, 800, 660, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	static Gdiplus::Point startPos, moveToPos;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;


    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...

			draw_board(hWnd);

			TCHAR* text;
			if (player == WHITE) text = L"White move";
			else text = L"Black move";

			TextOut(hdc, 650, 10, text, 10);

			checkType = TRUE;

			if (!check_check(startPos, moveToPos))
			{
				text = L"CHECK!";
				TextOut(hdc, 650, 30, text, 6);
			}
			
			if (selectPiece)
			{
				draw_rect(hWnd, startPos.X, startPos.Y);
			}
			if (moveToPiece)
			{
				draw_rect(hWnd, moveToPos.X, moveToPos.Y);
			}
            EndPaint(hWnd, &ps);
        }
        break;

	//case WM_ERASEBKGND:
	//	return (LRESULT)1; // Say we handled it.

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_LBUTTONDOWN:
	{
		
		InvalidateRect(hWnd, nullptr, true);
		
		if (!selectPiece)
		{
			startPos.X = (LONG)LOWORD(lParam) / BOARD_STEP;
			startPos.Y = (LONG)HIWORD(lParam) / BOARD_STEP;
			if (Board[startPos.Y][startPos.X] != nullptr)
				if (Board[startPos.Y][startPos.X]->GetColor() == player)
					selectPiece = TRUE;
		}
		else
			{
				moveToPos.X = (LONG)LOWORD(lParam) / BOARD_STEP;
				moveToPos.Y = (LONG)HIWORD(lParam) / BOARD_STEP;
				if (Board[moveToPos.Y][moveToPos.X] != nullptr)
				{
					if (Board[moveToPos.Y][moveToPos.X]->GetColor() != player)
						moveToPiece = check_move(Board[startPos.Y][startPos.X], startPos, moveToPos);
				}
				else moveToPiece = check_move(Board[startPos.Y][startPos.X], startPos, moveToPos);
				if (moveToPiece)
				{
					Board[moveToPos.Y][moveToPos.X] = Board[startPos.Y][startPos.X];
					Board[startPos.Y][startPos.X] = nullptr;
					player = player + changePlayer;
					changePlayer = -changePlayer;
					moveToPiece = FALSE;
				}
				selectPiece = FALSE;
			}
	}
	break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// My functions

void init_pieces()
{
	int counter = 0;

	Pieces.push_back(RookB);
	Pieces.push_back(KnightB);
	Pieces.push_back(BishopB);
	Pieces.push_back(QueenB);
	Pieces.push_back(KingB);
	Pieces.push_back(BishopB);
	Pieces.push_back(KnightB);
	Pieces.push_back(RookB);
	for (int i = 0; i < 8; i++)
		Pieces.push_back(PawnB);

	for (int i = 0; i < 8; i++)
		Pieces.push_back(PawnW);

	Pieces.push_back(RookW);
	Pieces.push_back(KnightW);
	Pieces.push_back(BishopW);
	Pieces.push_back(QueenW);
	Pieces.push_back(KingW);
	Pieces.push_back(BishopW);
	Pieces.push_back(KnightW);
	Pieces.push_back(RookW);

// board setup

	for (int y = 0; y < 2; y++)
		for (int x = 0; x < 8; x++)
		{
			Board[y][x] = &Pieces[counter];
			counter++;
		}
	for (int y = 2; y < 6; y++)
		for (int x = 0; x < 8; x++)
		{
			Board[y][x] = nullptr;
		}
	for (int y = 6; y < 8; y++)
		for (int x = 0; x < 8; x++)
		{
			Board[y][x] = &Pieces[counter];
			counter++;
		}
}

void draw_board(HWND hWnd)
{

	int xpos, ypos;
	xpos = 0; ypos = 0;

	
	Gdiplus::Graphics gdipGraphics(hWnd);

	Gdiplus::Image board(IMAGES_FOLDER"board.png");

	gdipGraphics.DrawImage(&board, Gdiplus::Rect(0, 0, 600, 600));

	Gdiplus::Image figurice(IMAGES_FOLDER"pieces.png");

	for (int y = 0; y < 8; y++)
		for (int x = 0; x < 8; x++)
			if (Board[y][x] != nullptr) 
				gdipGraphics.DrawImage(&figurice, Gdiplus::Rect(5 + x * BOARD_STEP, 5 + y * BOARD_STEP, 60, 60), Board[y][x]->GetXcoord(), Board[y][x]->GetYcoord(), PIECE_STEP, PIECE_STEP, Gdiplus::Unit::UnitPixel);

	/*for (int i = 0; i < Pieces.size(); i++)
		gdipGraphics.DrawImage(&figurice, Gdiplus::Rect(5 + Pieces[i].GetXpos() * 75, 5 + Pieces[i].GetYpos() * 75, 60, 60), Pieces[i].GetXcoord(), Pieces[i].GetYcoord(), 170, 170, Gdiplus::Unit::UnitPixel);
		*/
	}

void draw_rect(HWND hWnd, int x, int y)
{
	auto gdipGraphics = Gdiplus::Graphics::FromHWND(hWnd);
	gdipGraphics->DrawRectangle(new Gdiplus::Pen(Gdiplus::Color::Red, 4), Gdiplus::Rect(x * BOARD_STEP, y * BOARD_STEP, BOARD_STEP, BOARD_STEP));
}

bool check_move(Piece* piece, Gdiplus::Point startPos, Gdiplus::Point moveTo)
{
	bool check;

	if (Board[moveTo.Y][moveTo.X] != nullptr)
	{
		if (Board[moveTo.Y][moveTo.X]->GetColor() == player)
			return FALSE;
	}

	switch (piece->GetType())
	{
	case PAWN:
		check = check_pawn(startPos, moveTo);
		break;
	case KNIGHT:
		check = check_knight(startPos, moveTo);
		break;
	case BISHOP:
		check = check_bishop(startPos, moveTo);
		break;
	case ROOK:
		check = check_rook(startPos, moveTo);
		break;
	case QUEEN:
		check = check_queen(startPos, moveTo);
		break;
	case KING:
		check = check_king(startPos, moveTo);
		break;
	default:
		check = TRUE;
		break;
	}
	if (check)
	{
		checkType = FALSE;
		check = check_check(startPos, moveTo);
	}
	return check;
}

bool check_pawn(Gdiplus::Point startPos, Gdiplus::Point moveTo)
{
	int stepY;

	if (startPos.Y != moveTo.Y)
		stepY = ((moveTo.Y - startPos.Y) / abs(moveTo.Y - startPos.Y));
	
	if (abs(startPos.X - moveTo.X) > 0 && abs(startPos.Y - moveTo.Y) == 0) return FALSE;
	if (abs(startPos.X - moveTo.X) > 1 || abs(startPos.Y - moveTo.Y) > 2) return FALSE;

	if (abs(startPos.X - moveTo.X) == 1 && abs(startPos.Y - moveTo.Y) == 1)
		if (check_enpassant(startPos, moveTo))
		{

			EnPassant1.status = FALSE;
			EnPassant2.status = FALSE;
			return TRUE;
		}

	
	

	if (stepY == 1 && player == WHITE) return FALSE;
	if (stepY == -1 && player == BLACK) return FALSE;

	if (startPos.Y != 1 && startPos.Y != 6)
	{
		if (startPos.Y + stepY != moveTo.Y) return FALSE;
	}
	else
	{
		if (startPos.Y + stepY != moveTo.Y)
		{
			if (startPos.Y + stepY + stepY != moveTo.Y)
				return FALSE;
		}
	}

	if (startPos.X != moveTo.X)
		if (abs(startPos.X - moveTo.X) == 1)
		{
			if (Board[moveTo.Y][moveTo.X] == nullptr)
				return FALSE;
			else
			{
				EnPassant1.status = FALSE;
				EnPassant2.status = FALSE;
				return TRUE;
			}
		}
	
	if (Board[moveTo.Y][moveTo.X] != nullptr) return FALSE;


	EnPassant1.status = FALSE;
	EnPassant2.status = FALSE;

	if (abs(moveTo.Y - startPos.Y) == 2 && (Board[moveTo.Y][moveTo.X + 1] != nullptr))
		if (Board[moveTo.Y][moveTo.X + 1]->GetColor() != player)
		{
			EnPassant1.pos.X = moveTo.X + 1;
			EnPassant1.pos.Y = moveTo.Y;
			EnPassant1.status = TRUE;
			EnPassant1.dirY = stepY;
			EnPassant1.dirX = -1;
		}
	if (abs(moveTo.Y - startPos.Y) == 2 && (Board[moveTo.Y][moveTo.X - 1] != nullptr))
		if (Board[moveTo.Y][moveTo.X - 1]->GetColor() != player)
		{
			EnPassant2.pos.X = moveTo.X - 1;
			EnPassant2.pos.Y = moveTo.Y;
			EnPassant2.status = TRUE;
			EnPassant2.dirY = stepY;
			EnPassant2.dirX = 1;
		}

	return TRUE;
}

bool check_knight(Gdiplus::Point startPos, Gdiplus::Point moveTo)
{
	if (abs(startPos.X - moveTo.X) > 2 || abs(startPos.Y - moveTo.Y) > 2 || startPos.X == moveTo.X || startPos.Y == moveTo.Y || (abs(startPos.X - moveTo.X) == abs(startPos.Y - moveTo.Y))) return FALSE;
	return TRUE;
}

bool check_bishop(Gdiplus::Point startPos, Gdiplus::Point moveTo)
{
	if (abs(startPos.X - moveTo.X) != abs(startPos.Y - moveTo.Y)) return FALSE;

	int stepX = startPos.X;
	int stepY = startPos.Y;

	while (stepX != (moveTo.X - ((moveTo.X - startPos.X) / (abs((moveTo.X - startPos.X))))))
	{
		stepX = stepX + ((moveTo.X - startPos.X) / (abs((moveTo.X - startPos.X))));
		stepY = stepY + ((moveTo.Y - startPos.Y) / (abs((moveTo.Y - startPos.Y))));
		if (Board[stepY][stepX] != nullptr) return FALSE;
	}
	return TRUE;
}

bool check_rook(Gdiplus::Point startPos, Gdiplus::Point moveTo)

{
	if ((startPos.X != moveTo.X) && (startPos.Y != moveTo.Y)) return FALSE;

	int stepX = startPos.X;
	int stepY = startPos.Y;

	if (startPos.X != moveTo.X)
		while (stepX != (moveTo.X - ((moveTo.X - startPos.X) / (abs((moveTo.X - startPos.X))))))
		{
			stepX = stepX + ((moveTo.X - startPos.X) / (abs((moveTo.X - startPos.X))));
			if (Board[stepY][stepX] != nullptr) return FALSE;
		}
	else
		while (stepY != (moveTo.Y - ((moveTo.Y - startPos.Y) / (abs((moveTo.Y - startPos.Y))))))
		{
			stepY = stepY + ((moveTo.Y - startPos.Y) / (abs((moveTo.Y - startPos.Y))));
			if (Board[stepY][stepX] != nullptr) return FALSE;
		}

	if (startPos.Y == 0 && startPos.X == 0 && !lRookMovedB) lRookMovedB = TRUE;
	if (startPos.Y == 0 && startPos.X == 7 && !rRookMovedB) rRookMovedB = TRUE;
	if (startPos.Y == 7 && startPos.X == 0 && !lRookMovedW) lRookMovedW = TRUE;
	if (startPos.Y == 7 && startPos.X == 7 && !rRookMovedW) rRookMovedW = TRUE;

	return TRUE;
}

bool check_queen(Gdiplus::Point startPos, Gdiplus::Point moveTo)
{
	if ((startPos.X != moveTo.X) && (startPos.Y != moveTo.Y) && (abs(startPos.X - moveTo.X) != abs(startPos.Y - moveTo.Y))) return FALSE;

	int stepX = startPos.X;
	int stepY = startPos.Y;

	if ((startPos.X != moveTo.X) && (startPos.Y != moveTo.Y))
		while (stepX != (moveTo.X - ((moveTo.X - startPos.X) / (abs((moveTo.X - startPos.X))))))
		{
			stepX = stepX + ((moveTo.X - startPos.X) / (abs((moveTo.X - startPos.X))));
			stepY = stepY + ((moveTo.Y - startPos.Y) / (abs((moveTo.Y - startPos.Y))));
			if (Board[stepY][stepX] != nullptr) return FALSE;
		}

	stepX = startPos.X;
	stepY = startPos.Y;

	if ((startPos.X != moveTo.X) && (startPos.Y == moveTo.Y))
		while (stepX != (moveTo.X - ((moveTo.X - startPos.X) / (abs((moveTo.X - startPos.X))))))
		{
			stepX = stepX + ((moveTo.X - startPos.X) / (abs((moveTo.X - startPos.X))));
			if (Board[stepY][stepX] != nullptr) return FALSE;
		}

	stepX = startPos.X;
	stepY = startPos.Y;

	if ((startPos.X == moveTo.X) && (startPos.Y != moveTo.Y))
		while (stepY != (moveTo.Y - ((moveTo.Y - startPos.Y) / (abs((moveTo.Y - startPos.Y))))))
		{
			stepY = stepY + ((moveTo.Y - startPos.Y) / (abs((moveTo.Y - startPos.Y))));
			if (Board[stepY][stepX] != nullptr) return FALSE;
		}

	return TRUE;
}

bool check_king(Gdiplus::Point startPos, Gdiplus::Point moveTo)
{
	if (abs(startPos.X - moveTo.X) == 2 && (startPos.Y - moveTo.Y) == 0) return check_castling(startPos, moveTo);
	if (abs(startPos.X - moveTo.X) > 1 || abs(startPos.Y - moveTo.Y) > 1) return FALSE;

	if (player == WHITE && !kingMovedW) kingMovedW = TRUE;
	if (player == BLACK && !kingMovedB) kingMovedB = TRUE;

	return TRUE;
}

bool check_castling(Gdiplus::Point startPos, Gdiplus::Point moveTo)
{
	bool check = FALSE;
	int stepX = startPos.X - moveTo.X;
	if (stepX == -2 && player == WHITE && !kingMovedW && !rRookMovedW)
		if (Board[startPos.Y][startPos.X + 1] == nullptr) check = TRUE;
	if (stepX == 2 && player == WHITE && !kingMovedW && !lRookMovedW)
		if (Board[startPos.Y][startPos.X - 1] == nullptr && Board[moveTo.Y][moveTo.X - 1] == nullptr) check = TRUE;
	if (stepX == 2 && player == BLACK && !kingMovedB && !lRookMovedB)
		if (Board[startPos.Y][startPos.X - 1] == nullptr && Board[moveTo.Y][moveTo.X - 1] == nullptr) check = TRUE;
	if (stepX == -2 && player == BLACK && !kingMovedB && !rRookMovedB)
		if (Board[startPos.Y][startPos.X + 1] == nullptr) check = TRUE;
	if (check)
	{
		if (stepX == -2)
		{
			Board[moveTo.Y][moveTo.X + stepX / abs(stepX)] = Board[moveTo.Y][7];
			Board[moveTo.Y][7] = nullptr;
		}
		else
		{
			Board[moveTo.Y][moveTo.X + stepX / abs(stepX)] = Board[moveTo.Y][0];
			Board[moveTo.Y][0] = nullptr;
		}
	}
	if (check && player == WHITE && !kingMovedW) kingMovedW = TRUE;
	if (check && player == BLACK && !kingMovedB) kingMovedB = TRUE;

	return check;
}

bool check_enpassant(Gdiplus::Point startPos, Gdiplus::Point moveTo)
{
	bool check = FALSE;

	if (startPos.X == EnPassant1.pos.X && startPos.Y == EnPassant1.pos.Y && EnPassant1.status && EnPassant1.dirX == (moveTo.X - startPos.X))
	{
		Board[moveTo.Y + EnPassant1.dirY][moveTo.X] = nullptr;
		check = TRUE;
	}
	if (startPos.X == EnPassant2.pos.X && startPos.Y == EnPassant2.pos.Y && EnPassant2.status && !check && EnPassant2.dirX == (moveTo.X - startPos.X))
	{
		Board[moveTo.Y + EnPassant2.dirY][moveTo.X] = nullptr;
		check = TRUE;
	}

	return check;
}

bool check_check(Gdiplus::Point startPos, Gdiplus::Point moveTo)
{

	Piece* tempBoard[8][8];
	Gdiplus::Point kingPos;
	int n;

	for (int y = 0; y < MAX_ROW; y++)
		for (int x = 0; x < MAX_COL; x++)
			tempBoard[y][x] = Board[y][x];

	if (!checkType)
	{
		tempBoard[moveTo.Y][moveTo.X] = tempBoard[startPos.Y][startPos.X];
		tempBoard[startPos.Y][startPos.X] = nullptr;
	}

	
	for (int y = 0; y < MAX_ROW; y++)
		for (int x = 0; x < MAX_COL; x++)
		{
			if (tempBoard[y][x] != nullptr)
				if (tempBoard[y][x]->GetType() == KING && tempBoard[y][x]->GetColor() == player)
				{
					kingPos.X = x;
					kingPos.Y = y;
				}
		}

	if (player == WHITE) n = -1;
	else n = 1;
	
	if (kingPos.Y + n < MAX_ROW && kingPos.Y + n >= 0 && kingPos.X + 1 < MAX_COL)
	if (tempBoard[kingPos.Y + n][kingPos.X + 1] != nullptr)
		if (tempBoard[kingPos.Y + n][kingPos.X + 1]->GetColor() != player)
			if (tempBoard[kingPos.Y + n][kingPos.X + 1]->GetType() == PAWN)
				return FALSE;

	if (kingPos.Y + n < MAX_ROW && kingPos.Y + n >= 0 && kingPos.X - 1 >= 0)
	if (tempBoard[kingPos.Y + n][kingPos.X - 1] != nullptr)
		if (tempBoard[kingPos.Y + n][kingPos.X - 1]->GetColor() != player)
			if (tempBoard[kingPos.Y + n][kingPos.X - 1]->GetType() == PAWN)
				return FALSE;

	for (int y = 0; y < kingPos.Y; y++)
		if (tempBoard[y][kingPos.X] != nullptr)
			if (tempBoard[y][kingPos.X]->GetColor() != player)
				if (tempBoard[y][kingPos.X]->GetType() == QUEEN || tempBoard[y][kingPos.X]->GetType() == ROOK)
					return FALSE;
				else break;
			else break;


	for (int y = kingPos.Y + 1; y < MAX_ROW; y++)
		if (tempBoard[y][kingPos.X] != nullptr)
			if (tempBoard[y][kingPos.X]->GetColor() != player)
				if (tempBoard[y][kingPos.X]->GetType() == QUEEN || tempBoard[y][kingPos.X]->GetType() == ROOK)
					return FALSE;
				else break;
			else break;

	for (int x = 0; x < kingPos.X; x++)
		if (tempBoard[kingPos.Y][x] != nullptr)
			if (tempBoard[kingPos.Y][x]->GetColor() != player)
				if (tempBoard[kingPos.Y][x]->GetType() == QUEEN || tempBoard[kingPos.Y][x]->GetType() == ROOK)
					return FALSE;
				else break;
			else break;


	for (int x = kingPos.X + 1; x < MAX_COL; x++)
		if (tempBoard[kingPos.Y][x] != nullptr)
			if (tempBoard[kingPos.Y][x]->GetColor() != player)
				if (tempBoard[kingPos.Y][x]->GetType() == QUEEN || tempBoard[kingPos.Y][x]->GetType() == ROOK)
					return FALSE;
				else break;
			else break;

	n = 1;

	while (kingPos.Y-n >= 0 && kingPos.X-n >= 0)
	{
		if (tempBoard[kingPos.Y-n][kingPos.X - n] != nullptr)
			if (tempBoard[kingPos.Y - n][kingPos.X - n]->GetColor() != player)
				if (tempBoard[kingPos.Y - n][kingPos.X - n]->GetType() == QUEEN || tempBoard[kingPos.Y - n][kingPos.X - n]->GetType() == BISHOP)
					return FALSE;
				else break;
			else break;
		n++;
	}

	n = 1;

	while (kingPos.Y + n < MAX_ROW && kingPos.X + n < MAX_COL)
	{
		if (tempBoard[kingPos.Y + n][kingPos.X + n] != nullptr)
			if (tempBoard[kingPos.Y + n][kingPos.X + n]->GetColor() != player)
				if (tempBoard[kingPos.Y + n][kingPos.X + n]->GetType() == QUEEN || tempBoard[kingPos.Y + n][kingPos.X + n]->GetType() == BISHOP)
					return FALSE;
				else break;
			else break;
		n++;
	}

	n = 1;

	while (kingPos.Y - n >= 0 && kingPos.X + n < MAX_COL)
	{
		if (tempBoard[kingPos.Y - n][kingPos.X + n] != nullptr)
			if (tempBoard[kingPos.Y - n][kingPos.X + n]->GetColor() != player)
				if (tempBoard[kingPos.Y - n][kingPos.X + n]->GetType() == QUEEN || tempBoard[kingPos.Y - n][kingPos.X + n]->GetType() == BISHOP)
					return FALSE;
				else break;
			else break;
		n++;
	}

	n = 1;

	while (kingPos.Y + n < MAX_ROW && kingPos.X - n >= 0)
	{
		if (tempBoard[kingPos.Y + n][kingPos.X - n] != nullptr)
			if (tempBoard[kingPos.Y + n][kingPos.X - n]->GetColor() != player)
				if (tempBoard[kingPos.Y + n][kingPos.X - n]->GetType() == QUEEN || tempBoard[kingPos.Y + n][kingPos.X - n]->GetType() == BISHOP)
					return FALSE;
				else break;
			else break;
		n++;

	}

	int y;
		for (int x = 1; x < 3; x++)
		{
			if (x == 1) y = 2;
			else y = 1;

				if (kingPos.Y - y >= 0 && kingPos.X - x >= 0)
					if (tempBoard[kingPos.Y - y][kingPos.X - x] != nullptr)
						if (tempBoard[kingPos.Y - y][kingPos.X - x]->GetColor() != player)
							if (tempBoard[kingPos.Y - y][kingPos.X - x]->GetType() == KNIGHT)
								return FALSE;
				if (kingPos.Y + y < MAX_ROW && kingPos.X + x < MAX_COL)
					if (tempBoard[kingPos.Y + y][kingPos.X + x] != nullptr)
						if (tempBoard[kingPos.Y + y][kingPos.X + x]->GetColor() != player)
							if (tempBoard[kingPos.Y + y][kingPos.X + x]->GetType() == KNIGHT)
								return FALSE;

				if (kingPos.Y + y < MAX_ROW && kingPos.X - x >= 0)
					if (tempBoard[kingPos.Y + y][kingPos.X - x] != nullptr)
						if (tempBoard[kingPos.Y + y][kingPos.X - x]->GetColor() != player)
							if (tempBoard[kingPos.Y + y][kingPos.X - x]->GetType() == KNIGHT)
								return FALSE;

				if (kingPos.Y - y >= 0 && kingPos.X + x < MAX_COL)
					if (tempBoard[kingPos.Y - y][kingPos.X + x] != nullptr)
						if (tempBoard[kingPos.Y - y][kingPos.X + x]->GetColor() != player)
							if (tempBoard[kingPos.Y - y][kingPos.X + x]->GetType() == KNIGHT)
								return FALSE;
		}

	return TRUE;
}