#include <Windows.h>
#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <wchar.h>

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

string spriteSheet;
string characterSpriteSheet;
SpriteID charPositition = {"startPoint", 160, 96};
string map[640 * 448];
string spriteMap[20 * 14] = {
	"I3", "J3", "A4", "I3", "J3", "I3", "J3", "I3", "J3", "I3", //"J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3",
	"I4", "J4", "A4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", //"J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4",
	"A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", //"A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4",
	"J1", "G4", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", //"J1", "J1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	"J1", "G5", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", //"J1", "J1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	"J1", "F6", "H3", "F6", "H3", "F6", "H3", "J1", "A4", "J1", //"J1", "H1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	"J1", "F6", "H3", "F6", "H3", "F6", "H3", "J1", "A4", "J1", //"J1", "J1", "J2", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	//"J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", //"J1", "J1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	//"J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", //"J1", "J1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	//"J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", //"J1", "J1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	//"J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", //"J1", "J1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	//"J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", //"J1", "J1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	//"J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", //"J1", "J1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
	//"J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", //"J1", "J1", "J1", "J1", "J1", "J1", "A4", "A4", "A4", "A4",
};

SpriteID sprites[(SPRITE_SHEET_WIDTH / SPRITE_WIDTH) * (SPRITE_SHEET_HEIGHT / SPRITE_HEIGHT)] = {};
SpriteID characterSprites[(CHARACTER_SPRITE_SHEET_WIDTH / CHARACTER_SPRITE_WIDTH) * (CHARACTER_SPRITE_SHEET_HEIGHT / CHARACTER_SPRITE_HEIGHT)] = {};


void LoadSprite(int x1, int y1, string spriteName);
void LoadCharacter(int x1, int y1, string spriteName);
void LoadMap();
void Input();
void Draw();

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

	// Load sprite sheet
	ifstream file("tileset_32x32_1bit.dat");
	if (file.is_open())
	{
		while (getline(file, temp))
		{
			for (int i = 0; i < temp.size(); i++)
				spriteSheet += temp[i];
		}
	}

	// Load character sprite sheet
	ifstream file2("character.dat");
	if (file2.is_open())
	{
		while (getline(file2, temp))
		{
			for (int i = 0; i < temp.size(); i++)
				characterSpriteSheet += temp[i];
		}
	}

	// Color screen white
	/*for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			screen[j * SCREEN_WIDTH + i] = L'\u2588';
			map[j * SCREEN_WIDTH + i] = 1;
		}
	}*/
	
	
	

	// Game loop

	
	while (true) { 
		LoadMap();
		Input();
		Draw();
		
		WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwScreenBufferData);
		screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = '\0';
		//Sleep(16);
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
			map[j * SCREEN_WIDTH + i] = spriteSheet[(temp.y + j - y1) * SPRITE_SHEET_WIDTH + (temp.x + i - x1)];
		}
	}
}

void LoadCharacter(int x1, int y1, string spriteName) {
	SpriteID temp;
	temp = { spriteName, 0, 0 };

	for (int i = 0; i < 130; i++) {
		if (characterSprites[i].name == temp.name) {
			temp.x = characterSprites[i].x;
			temp.y = characterSprites[i].y;
			break;
		}
	}

	for (int i = 0 + x1; i < (CHARACTER_SPRITE_WIDTH + x1); i++) {
		for (int j = 0 + y1; j < (CHARACTER_SPRITE_HEIGHT + y1); j++) {
			if(characterSpriteSheet[(temp.y + j - y1) * CHARACTER_SPRITE_SHEET_WIDTH + (temp.x + i - x1)] == '1' || characterSpriteSheet[(temp.y + j - y1) * CHARACTER_SPRITE_SHEET_WIDTH + (temp.x + i - x1)] == '0')
			spriteMap[j * SCREEN_WIDTH + i] = characterSpriteSheet[(temp.y + j - y1) * CHARACTER_SPRITE_SHEET_WIDTH + (temp.x + i - x1)];
		}
	}
}

void LoadMap() {
	for (int i = 0; i < SCREEN_WIDTH / SPRITE_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT / SPRITE_HEIGHT; j++) {
			LoadSprite(i * 32, j * 32, spriteMap[j * (SCREEN_WIDTH / SPRITE_WIDTH) + i]);
		}
	}
}

void Input() {

		
		if (GetAsyncKeyState(0x53)) {	// S
			LoadCharacter(charPositition.x, charPositition.y += 2, "A1");
			LoadCharacter(charPositition.x, charPositition.y += 2, "B1");
			LoadCharacter(charPositition.x, charPositition.y += 2, "C1");
		}
		else if (GetAsyncKeyState(0x41)) { // A
			LoadCharacter(charPositition.x -= 2, charPositition.y, "A2");
			LoadCharacter(charPositition.x -= 2, charPositition.y, "B2");
			LoadCharacter(charPositition.x -= 2, charPositition.y, "C2");
		}
		else if (GetAsyncKeyState(0x44)) {	// D
			LoadCharacter(charPositition.x += 2, charPositition.y, "A3");
			LoadCharacter(charPositition.x += 2, charPositition.y, "B3");
			LoadCharacter(charPositition.x += 2, charPositition.y, "C3");
		}
		else if (GetAsyncKeyState(0x57)) { // W
			LoadCharacter(charPositition.x, charPositition.y -= 2, "A4");
			LoadCharacter(charPositition.x, charPositition.y -= 2, "B4");
			LoadCharacter(charPositition.x, charPositition.y -= 2, "C4");
		}
}

void Draw() {
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			if (map[j * SCREEN_WIDTH + i] == "0")
				screen[j * SCREEN_WIDTH + i] = L' ';
			else if (map[j * SCREEN_WIDTH + i] == "1")
				screen[j * SCREEN_WIDTH + i] = L'\u2588';
		}
	}
}