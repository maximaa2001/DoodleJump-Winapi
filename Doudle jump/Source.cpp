#include "Hero.h"
#include "Platform.h"
#include <gdiplus.h>
#include <time.h>
#include <vector>
#define FON_IMAGE L"image/fon.jpg"
#define WIDTH 450
#define HEIGHT 700
#define GRAVITY_Y 1
#define START_COUNT_PLATFORMS 12

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawImage(HDC hdc, RECT rect, const WCHAR* path);
void initFon(RECT& fonRect);
UINT moveHero(HWND hWnd);
void initPlatforms(vector<Platform*>& platforms);
UINT movePlatform(HWND hWnd);
void generateNewPlatforms();


MSG msg;
HWND hwnd;
WNDCLASS wc;
RECT fonRect;
Hero hero(225.0f, 600.0f, 50.0f, 48.0f, L"image/hero.png");
vector<Platform*> platforms;
Platform platform(150.0f, 350.0f, 68.0f, 14.0f, L"image/platform.png");
Platform* platformJumped;
//Hero entity(225.0f, 600.0f, 50.0f, 48.0f, NULL);

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE prev, PWSTR szCmdLine, int nCndShow) {

	GdiplusStartupInput gdiplusStartupInput; // Связано с загрузкой картинки
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL); // Включаем GDI+
	srand(time(NULL));

	wc.hInstance = hInstance;
	wc.lpszClassName = L"MyApp";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if (!RegisterClass(&wc)) {
		return 0;
	}
	hwnd = CreateWindow(wc.lpszClassName, L"Doodle jump", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 450, 50, WIDTH, HEIGHT, NULL, NULL, wc.hInstance, NULL);

	ShowWindow(hwnd, nCndShow);
	//WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX     WS_OVERLAPPEDWINDOW

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken); // Завершение работы GDI+
	return 0;
}

RECT rect;
PAINTSTRUCT paint;
HDC hdc;

HDC hdcMem;
HBITMAP hbmMem;
HANDLE hold;


float mouseX;
float sx, sy;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CREATE:
		initFon(fonRect);
		initPlatforms(platforms);
		SetTimer(hWnd, 1, 15, (TIMERPROC) &moveHero);
		break;
	case WM_SIZE:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		GetClientRect(hWnd, &rect);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paint);

		hdcMem = CreateCompatibleDC(hdc);
		hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		hold = SelectObject(hdcMem, hbmMem);
		/*
		* начало двойной буферизации
		*/




		DrawImage(hdcMem, fonRect, FON_IMAGE);
		for (int i = 0; i < platforms.size(); i++) {
			DrawImage(hdcMem, platforms[i]->getRect(), platforms[i]->getImage());
		}
		
		DrawImage(hdcMem, hero.getRect(), hero.getImage());

	//	FillRect(hdcMem, &entity.getRect(), (HBRUSH)GetStockObject(BLACK_BRUSH));
		


		/*
		 конец двойной буферизации
		*/
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hold);
		DeleteObject(hbmMem);
		DeleteObject(hdcMem);

		EndPaint(hWnd, &paint);
		break;
	
	case WM_MOUSEMOVE:
		mouseX = LOWORD(lParam);
		if (hero.getIsLive()) {
			if (mouseX <= hero.getX()) {
				if (mouseX <= 0) {
					hero.moveRect(0, hero.getY());
				}
				else {
					hero.moveRect(mouseX, hero.getY());
				}
			}
			else if (mouseX >= hero.getX()) {
				if (mouseX >= sx - hero.getWidth()) {
					hero.moveRect(sx - hero.getWidth(), hero.getY());
				}
				else {
					hero.moveRect(mouseX, hero.getY());
				}
			}
		}
		break;
	case WM_ERASEBKGND:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DrawImage(HDC hdc, RECT rect,const WCHAR* path)
{
	Graphics graphics(hdc);
	Bitmap BackBuf(rect.right, rect.bottom, &graphics);
	Graphics temp(&BackBuf);
	Image img(path, true);
	temp.DrawImage(&img, 0, 0);

	graphics.DrawImage(&BackBuf, rect.left, rect.top, 0, 0, rect.right, rect.bottom, UnitPixel);
}

void initFon(RECT& fonRect) {
	fonRect.left = 0;
	fonRect.right = WIDTH;
	fonRect.top = 0;
	fonRect.bottom = HEIGHT;
}

void initPlatforms(vector<Platform*>& platforms) {// int y = min + (rand() % ((max - min) + 1));									 
	int minH = HEIGHT - 100; // min = HEIGHT - 180 max = HEIGHT
	int maxH = HEIGHT;
	
	for (int i = 0; i < START_COUNT_PLATFORMS; i++) {
		int x = rand() % (int) (WIDTH - platform.getWidth());
		int y = minH + rand() % (int)((maxH - minH) + 1);
		platforms.push_back(new Platform(x, y, 68.0f, 15.0f, L"image/platform.png"));
		if (minH - 100 < 0) {
			minH = 0;
			maxH = HEIGHT;
		}
		else {
			maxH = minH;
			minH = minH - 100;
		}
	}
}

UINT moveHero(HWND hWnd) {
	if (hero.getDir() == 1) {
		
		if (hero.getSpeed() == 0) {
			hero.setDir(-1);
			if (hero.getIsMovePlatform()) {
				hero.setIsMovePlatform(false);
				KillTimer(hWnd, 1);
				generateNewPlatforms();
				SetTimer(hWnd, 2, 15, (TIMERPROC)&movePlatform);
		
			}
		}
		else {
			
			hero.moveRect(hero.getX(), hero.getY() - (hero.getSpeed()));
			hero.setSpeed(hero.getSpeed() - GRAVITY_Y);
	
		}
	}
	else
	{
		for (int i = 0; i < platforms.size(); i++) {

			if (hero.getX() > platforms[i]->getX() + 8 && hero.getX() < platforms[i]->getX() + platforms[i]->getWidth() - 8
				&& hero.getY() + hero.getHeight() >= platforms[i]->getY()-8 && hero.getY() + hero.getHeight() <= platforms[i]->getY() + 10) {
				
				if (platforms[i] != platformJumped) {
					platformJumped = platforms[i];
					hero.setIsMovePlatform(true);
				}
				hero.setDir(1);
				hero.setSpeed(21);
				break;
			}
			else if (hero.getX() + hero.getWidth() > platforms[i]->getX() + 8 && hero.getX() + hero.getWidth() < platforms[i]->getX() + platforms[i]->getWidth() - 8
				&& hero.getY() + hero.getHeight() >= platforms[i]->getY() - 8 && hero.getY() + hero.getHeight() <= platforms[i]->getY() + 10) {
				
				if (platforms[i] != platformJumped) {
					platformJumped = platforms[i];
					hero.setIsMovePlatform(true);
				}
				hero.setDir(1);
				hero.setSpeed(21);
				break;
			}
		}
		if (hero.getDir() != 1) {
			if (hero.getSpeed() >= 14) {
				hero.setSpeed(14);
				hero.moveRect(hero.getX(), hero.getY() + hero.getSpeed());
				hero.setSpeed(hero.getSpeed());
			}
			else {
				hero.moveRect(hero.getX(), hero.getY() + (hero.getSpeed() + GRAVITY_Y));
				hero.setSpeed(hero.getSpeed() + GRAVITY_Y);

			}
			

		}	
		
	}
	InvalidateRect(hwnd, NULL, FALSE);
	return 0;
	
}

UINT movePlatform(HWND hWnd) {
	/*int y = (platform)->getY();
	for (int i = 0; i < platforms.size(); i++) {
		if (platforms[i]->getY() >= y) {
			delete platforms[i];
			platforms.erase(platforms.begin() + i);
		}
	}*/
	
	if (platformJumped->getY() + platformJumped->getHeight() < HEIGHT - platformJumped->getHeight() - 30) {
		hero.setY(hero.getY() + 10);

		for (int i = 0; i < platforms.size(); i++) {
			platforms[i]->setY((platforms[i]->getY() + 10));
		}
		
		InvalidateRect(hwnd, NULL, FALSE);
	}
	else {
		for (int i = 0; i < platforms.size(); i++) {
			if (platforms[i]->getY() + platforms[i]->getHeight() >= HEIGHT) {
				delete platforms[i];
				platforms.erase(platforms.begin() + i);
			}
		}
		KillTimer(hWnd, 2);
		SetTimer(hWnd, 1, 15, (TIMERPROC)&moveHero);
	}
	return 0;
}

void generateNewPlatforms() {						// int y = min + (rand() % ((max - min) + 1));
	float platf_y = platformJumped->getY();

	float area = HEIGHT - platf_y - platformJumped->getHeight() - 40;
	int count = (int)area / 100;
	int minH = -100; // min = HEIGHT - 180 max = HEIGHT
	int maxH = 0;
	
	if (area >= 100) {
		for (int i = 0; i < count + 1; i++) {
			int x = rand() % (int)(WIDTH - platform.getWidth());
			int y = minH + rand() % (int)((maxH - minH) + 1);
			platforms.push_back(new Platform(x, y, 68.0f, 15.0f, L"image/platform.png"));
			if (minH - 100 < area) {
				minH = -area;
				maxH = minH;
			}
			else if (minH == -area) {
				minH = -area;
				maxH = 0;
			}
			else {
				maxH = minH;
				minH = minH - 100;
			}
		}
	}
	else{
		int x = rand() % (int)(WIDTH - platform.getWidth());
		int y = -area + rand() % (int)((0 + area) + 1);
		platforms.push_back(new Platform(x, y, 68.0f, 15.0f, L"image/platform.png"));
	}
}