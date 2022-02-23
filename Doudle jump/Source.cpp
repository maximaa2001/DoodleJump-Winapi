#include "Hero.h"
#include <locale.h>
#include "Platform.h"
#include "Enemy.h"
#include "Bullet.h"
#include <gdiplus.h>
#include <time.h>
#include <vector>
#include <sstream>
#include <fstream>
#define FON_IMAGE L"image/fon.jpg"
#define WIDTH 450
#define HEIGHT 700
#define GRAVITY_Y 1
#define START_COUNT_PLATFORMS 12
#define FILE_SCORE "file.txt"


#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawImage(HDC hdc, RECT rect, const WCHAR* path);
void initFon(RECT& fonRect);
UINT moveHero(HWND hWnd);
void initPlatforms(vector<Platform*>& platforms);
UINT movePlatform(HWND hWnd);
void generateNewPlatforms(HWND hWnd);
void setScore();
void generateMonster(int area, HWND hWnd);
UINT moveFlyMonster(HWND hWnd);
UINT showShootImage(HWND hWnd);
UINT moveBullet(HWND hWnd);
void heroLose(HWND hWnd);
void setMaxValue();
void serialize();
void deserialize();


MSG msg;
HWND hwnd;
WNDCLASS wc;
RECT fonRect;
Hero hero(225.0f, 600.0f, 50.0f, 48.0f, L"image/hero.png",L"image/hero_shoot.png");
vector<Platform*> platforms;
vector<Enemy*> enemyes;
vector<Bullet*> bullets;
Platform platform(150.0f, 350.0f, 68.0f, 14.0f, L"image/platform.png");
Platform* platformJumped;
float coordinateJumpedEnemyY;
float JumpedEnemyHeight;
Enemy enemy(150.0f, 350.0f, 68.0f, 14.0f, L"image/enemy_2.png", 2, false);


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
HFONT font;
HFONT font2 = CreateFont(30, 18, 0, 0, 700, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, L"Times New Roman");
const CHAR* score = "";
const CHAR* scoreMax = "";
unsigned long currentScore = 0;
unsigned long monsterScore = 2000;
unsigned long maxValue = 0;
string stringScore;
string stringMax;
float shotX;
float shotY;
bool isJumpedMonster = false;
bool isPause = false;


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

		font = (HFONT)SelectObject(hdcMem, font2);
		SetTextColor(hdcMem, RGB(255, 0, 0));
		SetBkMode(hdcMem, TRANSPARENT);


		DrawImage(hdcMem, fonRect, FON_IMAGE);
		if (hero.getIsLive()) {
			for (int i = 0; i < platforms.size(); i++) {
				DrawImage(hdcMem, platforms[i]->getRect(), platforms[i]->getImage());
			}
			if (hero.getIsShoot()) {
				DrawImage(hdcMem, hero.getRect(), hero.getShootImage());
			}
			else {
				DrawImage(hdcMem, hero.getRect(), hero.getImage());
			}

			for (int i = 0; i < enemyes.size(); i++) {
				DrawImage(hdcMem, enemyes[i]->getRect(), enemyes[i]->getImage());
			}

			for (int i = 0; i < bullets.size(); i++) {
				DrawImage(hdcMem, bullets[i]->getRect(), bullets[i]->getImage());
			}
			setScore();
			TextOutA(hdcMem, 10, 0, score, strlen(score));
		}
		else {
			if (currentScore > maxValue) {
				serialize();
				maxValue = currentScore;
			}
			setMaxValue();
			TextOutA(hdcMem, WIDTH / 2 - 90, HEIGHT / 2 - 40, "Score: ", 7);
			TextOutA(hdcMem, WIDTH/2 + 30, HEIGHT/2 - 40, score, strlen(score));
			TextOutA(hdcMem, WIDTH / 2 - 140, HEIGHT / 2 , "Max Score: ", 11);
			TextOutA(hdcMem, WIDTH / 2 + 60, HEIGHT / 2, scoreMax, strlen(scoreMax));
			TextOutA(hdcMem, WIDTH / 2 - 140, HEIGHT / 2 + 40 , "PRESS SPACE", 11);
			
			
		}

		/*
		 конец двойной буферизации
		*/
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hold);
		SelectObject(hdcMem, font);
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
	case WM_LBUTTONDOWN:
		if (hero.getIsLive() && !hero.getIsShoot()) {
			hero.setIsShoot(true);
			SetTimer(hWnd, 4, 300, (TIMERPROC)&showShootImage);
			bullets.push_back(new Bullet(hero.getX() + (hero.getWidth() / 2), hero.getY(), 25, 18, L"image/bullet.png"));
			SetTimer(hWnd, 5, 1, (TIMERPROC)&moveBullet);
		}
		
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
			if (!hero.getIsLive()) {
				hero.moveRect(225, 600);
				hero.setIsLive(true);
				hero.setSpeed(21);
				hero.setDir(1);
				hero.setIsMovePlatform(false);
				currentScore = 0;
				monsterScore = 2000;
	
				Platform platform(210, HEIGHT-40, 68.0f, 14.0f, L"image/platform.png");

				initPlatforms(platforms);
				SetTimer(hWnd, 1, 15, (TIMERPROC)&moveHero);
				InvalidateRect(hWnd, NULL, FALSE);
				isPause = false;
			}
			break;
		case VK_ESCAPE:
			if (isPause) {
				SetTimer(hWnd, 1, 15, (TIMERPROC)&moveHero);
				isPause = false;
			}
			else {
				KillTimer(hWnd, 1);
				isPause = true;
			}
			break;
		default:
			break;
		}
		
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
		bool isNorm = true;
		for (int i = 0; i < platforms.size(); i++) {
			if (x > platforms[i]->getX() - platforms[i]->getWidth() && x < platforms[i]->getX() + platforms[i]->getWidth() && y > platforms[i]->getY() - 10 && y < platforms[i]->getY() + platforms[i]->getHeight()) {
				isNorm = false;
				break;
			}
		}
		if (isNorm) {
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
		else {
			i--;
		}
		
	}
}

UINT moveHero(HWND hWnd) {
	if (hero.getDir() == 1) {
		bool isHit = false;
		for (int enemy = 0; enemy < enemyes.size(); enemy++) {
			for (int heroX = hero.getX(); heroX < hero.getX() + hero.getHeight(); heroX++) {
				if (heroX >= enemyes[enemy]->getX() + 10 && heroX <= enemyes[enemy]->getX() + enemyes[enemy]->getWidth() - 10) {
					for (int heroY = hero.getY(); heroY < hero.getY() + hero.getHeight(); heroY++) {
						if (heroY >= enemyes[enemy]->getY() && heroY <= enemyes[enemy]->getY() + enemyes[enemy]->getHeight() - 20) {
							isHit = true;
							heroLose(hWnd);
							break;
						}
					}
				}
				if (isHit) {
					break;
				}
			}
			if (isHit) {
				break;
			}
		
		}
	}
	else {
		bool isHit = false;
		for (int enemy = 0; enemy < enemyes.size(); enemy++) {
			for (int heroX = hero.getX(); heroX < hero.getX() + hero.getWidth(); heroX++) {
				if (heroX >= enemyes[enemy]->getX() && heroX <= enemyes[enemy]->getX() + enemyes[enemy]->getWidth()) {
					if (hero.getY() + hero.getHeight() >= enemyes[enemy]->getY() && hero.getY() + hero.getHeight() < +enemyes[enemy]->getY() + enemyes[enemy]->getHeight()) {
						hero.setDir(1);
						hero.setSpeed(21);
						hero.setIsMovePlatform(true);
						isJumpedMonster = true;
						coordinateJumpedEnemyY = enemyes[enemy]->getY();
						JumpedEnemyHeight = enemyes[enemy]->getHeight();
						isHit = true;
						delete enemyes[enemy];
						enemyes.erase(enemyes.begin() + enemy);
						currentScore += 500;
						break;
;					}
				}
			}
			if (isHit) {
				break;
			}
		}
	}
	if (hero.getIsLive()) {
		if (hero.getDir() == 1) {
			if (hero.getSpeed() == 0) {
				hero.setDir(-1);
				if (hero.getIsMovePlatform()) {
					hero.setIsMovePlatform(false);
					KillTimer(hWnd, 1);
					generateNewPlatforms(hWnd);
					if (hero.getIsLive()) {
						SetTimer(hWnd, 2, 15, (TIMERPROC)&movePlatform);
					}
				}
			}
			else {
				hero.moveRect(hero.getX(), hero.getY() - (hero.getSpeed()));
				if (hero.getIsMovePlatform()) {
					currentScore += hero.getSpeed();
				}
				hero.setSpeed(hero.getSpeed() - GRAVITY_Y);
			}
		}
		else
		{
			for (int i = 0; i < platforms.size(); i++) {
				if (hero.getX() > platforms[i]->getX() + 8 && hero.getX() < platforms[i]->getX() + platforms[i]->getWidth() - 8
					&& hero.getY() + hero.getHeight() >= platforms[i]->getY() - 8 && hero.getY() + hero.getHeight() <= platforms[i]->getY() + 10) {

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
	}
	if (hero.getY() > HEIGHT) {
		heroLose(hWnd);
	}
	InvalidateRect(hWnd, NULL, FALSE);
	return 0;
	
}

UINT movePlatform(HWND hWnd) {

	if (hero.getIsLive()) {
		if (platformJumped->getY() + platformJumped->getHeight() < HEIGHT - platformJumped->getHeight() - 30 || isJumpedMonster) {

			if (isJumpedMonster) {
				if (coordinateJumpedEnemyY  < HEIGHT - 30) {
					hero.setY(hero.getY() + 10);

					for (int i = 0; i < platforms.size(); i++) {
						platforms[i]->setY((platforms[i]->getY() + 10));
					}

					for (int i = 0; i < enemyes.size(); i++) {
						enemyes[i]->setY((enemyes[i]->getY() + 10));
					}
					coordinateJumpedEnemyY += 10;
				}
				else {
					coordinateJumpedEnemyY = 0;
					isJumpedMonster = false;
				}

			}
			else {
				hero.setY(hero.getY() + 10);

				for (int i = 0; i < platforms.size(); i++) {
					platforms[i]->setY((platforms[i]->getY() + 10));
				}

				for (int i = 0; i < enemyes.size(); i++) {
					enemyes[i]->setY((enemyes[i]->getY() + 10));
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
		else {
			for (int i = 0; i < platforms.size(); i++) {
				if (platforms[i]->getY() + platforms[i]->getHeight() >= HEIGHT) {
					delete platforms[i];
					platforms.erase(platforms.begin() + i);
				}
			}
			for (int i = 0; i < enemyes.size(); i++) {
				if (enemyes[i]->getY() >= HEIGHT) {
					delete enemyes[i];
					enemyes.erase(enemyes.begin() + i);
				}
			}
			KillTimer(hWnd, 2);
			if (hero.getIsLive() && !isPause) {
				SetTimer(hWnd, 1, 15, (TIMERPROC)&moveHero);
			}
		}
	}
	else {
		KillTimer(hWnd, 2);
	}
	return 0;
}

void generateNewPlatforms(HWND hWnd) {						// int y = min + (rand() % ((max - min) + 1));
	float platf_y;
	float area;
	if (isJumpedMonster) {
		platf_y = coordinateJumpedEnemyY;
		area = HEIGHT - platf_y - 100;

	}
	else {
		platf_y = platformJumped->getY();
		area = HEIGHT - platf_y - platformJumped->getHeight() - 40;
	}
	int count = (int)area / 100;
	int minH = -100; // min = HEIGHT - 180 max = HEIGHT
	int maxH = 0;

	if (currentScore > monsterScore && area >= 100) {
		monsterScore += 2000;
		generateMonster(area, hWnd);
	}
	
	
	if (area >= 100) {
		for (int i = 0; i < count + 1; i++) {
			int x = rand() % (int)(WIDTH - platform.getWidth());
			int y = minH + rand() % (int)((maxH - minH) + 1);
			bool isNorm = true;
			for (int i = 0; i < enemyes.size(); i++) {
				if (x > enemyes[i]->getX() - platform.getWidth() && x < enemyes[i]->getX() + enemyes[i]->getWidth() && y > enemyes[i]->getY() - 10 && y < enemyes[i]->getY() + enemyes[i]->getHeight()) {
					isNorm = false;
					break;
				}
			}

			for (int i = 0; i < platforms.size(); i++) {
				if (x > platforms[i]->getX() - platforms[i]->getWidth() && x < platforms[i]->getX() + platforms[i]->getWidth() && y > platforms[i]->getY() - 10 && y < platforms[i]->getY() + platforms[i]->getHeight()) {
					isNorm = false;
					break;
				}
			}
			if (isNorm) {
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
			else {
				i--;
			}
			
		}
	}
	else{
		if (area >= 25) {
			int x = rand() % (int)(WIDTH - platform.getWidth());
			int y = -area + rand() % (int)((0 + area) + 1);
			platforms.push_back(new Platform(x, y, 68.0f, 15.0f, L"image/platform.png"));
		}
	}
}

void setScore() {
	stringScore = to_string(currentScore);
	score = stringScore.c_str();
}

void setMaxValue() {
	stringMax = to_string(maxValue);
	scoreMax = stringMax.c_str();
}

void generateMonster(int area, HWND hWnd) {// int y = min + (rand() % ((max - min) + 1));
	int x = rand() % (int)(WIDTH - platform.getWidth());
	int y = -area + rand() % (int)((0 + area) + 1);
	int type = 1 + (rand() % (6 - 1 + 1));
	switch (type)
	{
	case 1:
	case 2:
		enemyes.push_back(new Enemy(x, y, 100, 60, L"image/enemy_1.png", 2, false));
		break;
	case 3:
	case 4:
	case 5:
		enemyes.push_back(new Enemy(x, y, 60, 80, L"image/enemy_2.png", 1, true));
		SetTimer(hWnd, 3, 30, (TIMERPROC)&moveFlyMonster);
		break;
	case 6:
		enemyes.push_back(new Enemy(x, y, 100, 100, L"image/enemy_3.png", 3, false));
		break;
	}
}

UINT moveFlyMonster(HWND hWnd) {
	bool isExist = false;
	for (int i = 0; i < enemyes.size(); i++) {
		if (enemyes[i]->getIsCanFly()) {
			isExist = true;
			if (enemyes[i]->getDir() == 1) {
				if (enemyes[i]->getX() <= WIDTH - enemyes[i]->getWidth() - 5) {
					enemyes[i]->setX(enemyes[i]->getX() + 10);
				}
				else {
					enemyes[i]->setDir(-1);
				}
			}
			else {
				if (enemyes[i]->getX() >= 2) {
					enemyes[i]->setX(enemyes[i]->getX() - 10);
				}
				else {
					enemyes[i]->setDir(1);
				}
			}
		}
	}
	if (!isExist) {
		KillTimer(hWnd, 3);
	}
	return 0;
}

UINT showShootImage(HWND hWnd) {
	hero.setIsShoot(false);
	KillTimer(hWnd, 4);
	return 0;
}

UINT moveBullet(HWND hWnd) {
	for (int i = 0; i < bullets.size(); i++) {
		if (bullets[i]->getY() + bullets[i]->getHeight() < 0) {
			delete bullets[i];
			bullets.erase(bullets.begin() + i);
		}
	}
	for (int i = 0; i < bullets.size(); i++) {
		bullets[i]->setY(bullets[i]->getY() - 20);
	}
	bool isHit = false;
	for (int bullet = 0; bullet < bullets.size(); bullet++) {
		for (int x = bullets[bullet]->getX(); x < bullets[bullet]->getX() + bullets[bullet]->getWidth(); x++) {
			for (int enemy = 0; enemy < enemyes.size(); enemy++) {
				if (x >= enemyes[enemy]->getX() && x <= enemyes[enemy]->getX() + enemyes[enemy]->getWidth()) {
					for (int y = bullets[bullet]->getY(); y < bullets[bullet]->getY() + bullets[bullet]->getHeight(); y++) {
						if (y >= enemyes[enemy]->getY() && y <= enemyes[enemy]->getY() + enemyes[enemy]->getHeight()) {
							isHit = true;
							enemyes[enemy]->decrementLife();
							delete bullets[bullet];
							bullets.erase(bullets.begin() + bullet);
							if (enemyes[enemy]->getCountLife() <= 0) {
								delete enemyes[enemy];
								enemyes.erase(enemyes.begin() + enemy);
								currentScore += 500;
							}
							break;
						}
					}
				}
				if (isHit) {
					break;
				}
			}
			if (isHit) {
				break;
			}
		}
		if (isHit) {
			break;
		}
	}
	if (bullets.size() == 0) {
		KillTimer(hWnd, 5);
	}
	return 0;
}

void heroLose(HWND hWnd) {
	hero.setIsLive(false);

	KillTimer(hWnd, 1);

	for (int i = 0; i < enemyes.size(); i++) {
		delete enemyes[i];
	}
	enemyes.clear();
	for (int i = 0; i < platforms.size(); i++) {
		delete platforms[i];
	}
	platforms.clear();
	for (int i = 0; i < bullets.size(); i++) {
		delete  bullets[i];
	}
	bullets.clear();
	deserialize();
}

void serialize() {
	ofstream fout;
	fout.open(FILE_SCORE, ofstream::out);

	if (fout.is_open()) {
		fout.write((char*)&currentScore, sizeof(unsigned long));
	}

	fout.close();
}
void deserialize() {
	ifstream fin;
	fin.open(FILE_SCORE);
	if (fin.is_open()) {
		fin.read((char*)&maxValue, sizeof(unsigned long));  
	}
	fin.close();
}
