#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <wchar.h>
#include <thread>
#include <atomic>

#define SCREEN_WIDTH 320	
#define SCREEN_HEIGHT 224
#define SPRITE_WIDTH 32
#define SPRITE_HEIGHT 32
#define SPRITE_SHEET_WIDTH 320
#define SPRITE_SHEET_HEIGHT 416
#define CHARACTER_SPRITE_WIDTH 32
#define CHARACTER_SPRITE_HEIGHT 48
#define CHARACTER_SPRITE_SHEET_WIDTH 96
#define CHARACTER_SPRITE_SHEET_HEIGHT 192

using namespace std;

struct SpriteID
{
	string name;
	int x = 0, y = 0;
};

wstring spriteSheet;
wstring characterSpriteSheet;
SpriteID charPositition = {"startPoint", 0, 0};
string spriteMap[(SCREEN_WIDTH / SPRITE_WIDTH) * (SCREEN_HEIGHT / SPRITE_HEIGHT)] = {
	"I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", //"I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3",
	"I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", //"I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4",
	"H2", "A9", "G3", "A9", "A9", "A9", "A9", "A9", "A9", "A9", //"A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "J1", "J1",
	"A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", //"A9", "A9", "A9", "A9", "A9", "H1", "A9", "A9", "J1", "J1",
	"J1", "A9", "A9", "A9", "A9", "A9", "H1", "A9", "A9", "A4", //"A9", "A9", "A9", "A9", "A9", "A9", "A9", "I3", "J3", "J1",
	"J1", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A4", //"A9", "H1", "A9", "A9", "A9", "H3", "A9", "I4", "J4", "J1",
	"J1", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A4", //"A9", "A9", "A9", "I3", "J3", "A9", "A9", "A9", "A9", "J1",
	//"J1", "I3", "J3", "A9", "A9", "I3", "J3", "A9", "A9", "A4", "A9", "A9", "A9", "I4", "J4", "A9", "A9", "A9", "J1", "J1",
	//"J1", "I4", "J4", "A9", "A9", "I5", "J5", "A9", "A9", "A4", "A9", "A9", "A9", "A9", "A9", "A9", "J2", "A9", "A9", "J1",
	//"J1", "A9", "A9", "A9", "A9", "I5", "J5", "A9", "A9", "A4", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "J1",
	//"J1", "H1", "A9", "A9", "A9", "I5", "J5", "A9", "A9", "A4", "A9", "A9", "A9", "J1", "A9", "A9", "A9", "I3", "J3", "J1",
	//"J1", "J1", "A9", "A9", "A9", "I4", "J4", "A9", "A9", "A4", "A9", "A9", "A9", "A9", "J1", "J1", "A9", "I4", "J4", "J1",
	//"J1", "J1", "J1", "J1", "H1", "A9", "A9", "A9", "A9", "A4", "A9", "A9", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1",
	//"J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1",
};

SpriteID sprites[(SPRITE_SHEET_WIDTH / SPRITE_WIDTH) * (SPRITE_SHEET_HEIGHT / SPRITE_HEIGHT)] = {};
SpriteID characterSprites[(CHARACTER_SPRITE_SHEET_WIDTH / CHARACTER_SPRITE_WIDTH) * (CHARACTER_SPRITE_SHEET_HEIGHT / CHARACTER_SPRITE_HEIGHT)] = {};


void LoadSprite(int x1, int y1, string spriteName);
void LoadCharacter(int x1, int y1, string spriteName);
void Draw();
void Input();

wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
DWORD dwScreenBufferData = 0;

int main() {
	
	SetConsoleActiveScreenBuffer(hConsole);

	_COORD coord = { SCREEN_WIDTH, SCREEN_HEIGHT };
	SetConsoleScreenBufferSize(hConsole, coord);

	_CONSOLE_FONT_INFOEX font = { 0 };
	font.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	font.nFont;
	font.dwFontSize.X = 3;
	font.dwFontSize.Y = 3;
	font.FaceName;
	font.FontFamily = FF_DONTCARE;
	font.FontWeight = FW_NORMAL;
	SetCurrentConsoleFontEx(hConsole, false, &font);

	// Assign codes to sprites

	string temp;
	char letter = 'A';
	int number = 1;
	int x = 0;
	int y = 0;
	string spriteID;

	for (int i = 0; i < (SPRITE_SHEET_WIDTH / SPRITE_WIDTH); ++i) {
		for (int j = 0; j < (SPRITE_SHEET_HEIGHT / SPRITE_HEIGHT); j++) {
			sprites[j * (SPRITE_SHEET_WIDTH / SPRITE_WIDTH) + i] = {letter + to_string(number), x * SPRITE_WIDTH, y * SPRITE_HEIGHT };
				number++;
				y++;
		}
		letter++;
		x++;
		y = 0;
		number = 1;
	}
	//Assign code to character sprites
	// Reset values
	letter = 'A';
	number = 1;
	x = 0;
	y = 0;
	
	for (int i = 0; i < (CHARACTER_SPRITE_SHEET_WIDTH / CHARACTER_SPRITE_WIDTH); ++i) {
		for (int j = 0; j < (CHARACTER_SPRITE_SHEET_HEIGHT / CHARACTER_SPRITE_HEIGHT); j++) {
			characterSprites[j * (CHARACTER_SPRITE_SHEET_WIDTH / CHARACTER_SPRITE_WIDTH) + i] = { letter + to_string(number), x * CHARACTER_SPRITE_WIDTH, y * CHARACTER_SPRITE_HEIGHT };
			number++;
			y++;
		}
		letter++;
		x++;
		y = 0;
		number = 1;
	}
	// Load map sprite sheet
	ifstream file("EditedTileset_32x32_1bit.dat");
	if (file.is_open())
	{
		while (getline(file, temp))
		{
			for (int i = 0; i < temp.size(); i++) {
				if (temp[i] == '1')
					spriteSheet += L'\u2588';
				else if (temp[i] == '0')
					spriteSheet += L' ';
			}
		}
	}

	// Load character sprite sheet
	ifstream file2("character.dat");
	if (file2.is_open())
	{
		while (getline(file2, temp))
		{
			for (int i = 0; i < temp.size(); i++) {
				if (temp[i] == '1')
					characterSpriteSheet += L'\u2588';
				else if (temp[i] == '0')
					characterSpriteSheet += L' ';
				else
					characterSpriteSheet += L'X'; // "Alpha" channel
			}
		}
	}

	// Game loop
	
	while (true) { 
		Draw();
		Input();
			
		screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwScreenBufferData);
	}
	return 0;
}

void LoadSprite(int x1, int y1, string spriteName) {
	SpriteID temp;
	temp = {spriteName, 0, 0};

	for (int i = 0; i < (SCREEN_WIDTH / SPRITE_WIDTH) * (SCREEN_HEIGHT / SPRITE_HEIGHT); i++) {
		if (sprites[i].name == temp.name) {
			temp.x = sprites[i].x;
			temp.y = sprites[i].y;
			break;
		}
	}

	for (int i = 0 + x1; i < (SPRITE_WIDTH + x1); i++) {
		for (int j = 0 + y1; j < (SPRITE_HEIGHT + y1); j++) {
			screen[j * SCREEN_WIDTH + i] = spriteSheet[(temp.y + j - y1) * SPRITE_SHEET_WIDTH + (temp.x + i - x1)];
		}
	}
}

void LoadCharacter(int x1, int y1, string spriteName) {
	SpriteID temp;
	temp = { spriteName, 0, 0 };

	for (int i = 0; i < (CHARACTER_SPRITE_SHEET_WIDTH / CHARACTER_SPRITE_WIDTH) * (CHARACTER_SPRITE_SHEET_HEIGHT / CHARACTER_SPRITE_HEIGHT); i++) {
		if (characterSprites[i].name == temp.name) {
			temp.x = characterSprites[i].x;
			temp.y = characterSprites[i].y;
			break;
		}
	}

	for (int i = 0 + x1; i < (CHARACTER_SPRITE_WIDTH + x1); i++) {
		for (int j = 0 + y1; j < (CHARACTER_SPRITE_HEIGHT + y1); j++) {
			if (characterSpriteSheet[(temp.y + j - y1) * CHARACTER_SPRITE_SHEET_WIDTH + (temp.x + i - x1)] == L'\u2588' || characterSpriteSheet[(temp.y + j - y1) * CHARACTER_SPRITE_SHEET_WIDTH + (temp.x + i - x1)] == L' ') {
				screen[j * SCREEN_WIDTH + i] = characterSpriteSheet[(temp.y + j - y1) * CHARACTER_SPRITE_SHEET_WIDTH + (temp.x + i - x1)];
			}
		}
	}
}

void Draw() {
	for (int i = 0; i < SCREEN_WIDTH / SPRITE_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT / SPRITE_HEIGHT; j++) {
			LoadSprite(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, spriteMap[j * (SCREEN_WIDTH / SPRITE_WIDTH) + i]);
		}
	}
}

void Input() {
		if (GetAsyncKeyState(0x53)) {	// S
			LoadCharacter(charPositition.x, charPositition.y += 3, "A1");
			LoadCharacter(charPositition.x, charPositition.y += 3, "B1");
			LoadCharacter(charPositition.x, charPositition.y += 3, "C1");
		}
		else if (GetAsyncKeyState(0x41)) { // A
			LoadCharacter(charPositition.x -= 3, charPositition.y, "A2");
			LoadCharacter(charPositition.x -= 3, charPositition.y, "B2");
			LoadCharacter(charPositition.x -= 3, charPositition.y, "C2");
		}
		else if (GetAsyncKeyState(0x44)) {	// D
			LoadCharacter(charPositition.x += 3, charPositition.y, "A3");
			LoadCharacter(charPositition.x += 3, charPositition.y, "B3");
			LoadCharacter(charPositition.x += 3, charPositition.y, "C3");
		}
		else if (GetAsyncKeyState(0x57)) { // W
			LoadCharacter(charPositition.x, charPositition.y -= 3, "A4");
			LoadCharacter(charPositition.x, charPositition.y -= 3, "B4");
			LoadCharacter(charPositition.x, charPositition.y -= 3, "C4");
		}
}
