#include <Windows.h>
#include <chrono>
#include <string>
#include <fstream>
#include <wchar.h>

#define SCREEN_WIDTH 640	
#define SCREEN_HEIGHT 448
#define MAP_SPRITE_WIDTH 32
#define MAP_SPRITE_HEIGHT 32
#define MAP_SPRITE_SHEET_WIDTH 320
#define MAP_SPRITE_SHEET_HEIGHT 416
#define CHARACTER_SPRITE_WIDTH 32
#define CHARACTER_SPRITE_HEIGHT 48
#define CHARACTER_SPRITE_SHEET_WIDTH 96
#define CHARACTER_SPRITE_SHEET_HEIGHT 192

using namespace std;

struct SpriteID	// Used for map and character sprites
{
	string name;
	int x = 0, y = 0;
};

wstring spriteSheet;	// To store map sprite sheet
wstring characterSpriteSheet;	// To store character sprite sheet
SpriteID charPositition = {"startPoint", 0, 0};	// Initial character position

// Full map. Composed of sprites of 32x32 dimension. In this case represented by a code formed by A to J for columns,
// and 1 to 13 for rows, like "A1", "J2" and so on.

string spriteMap[(SCREEN_WIDTH / MAP_SPRITE_WIDTH) * (SCREEN_HEIGHT / MAP_SPRITE_HEIGHT)] = {	
	"I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3", "I3", "J3",
	"I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4", "I4", "J4",
	"H2", "A9", "G3", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "J1", "J1",
	"A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", "A4", "A9", "A9", "A9", "A9", "A9", "H1", "A9", "A9", "J1", "J1",
	"J1", "A9", "A9", "A9", "A9", "A9", "H1", "A9", "A9", "A4", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "I3", "J3", "J1",
	"J1", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A4", "A9", "H1", "A9", "A9", "A9", "H3", "A9", "I4", "J4", "J1",
	"J1", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A4", "A9", "A9", "A9", "I3", "J3", "A9", "A9", "A9", "A9", "J1",
	"J1", "I3", "J3", "A9", "A9", "I3", "J3", "A9", "A9", "A4", "A9", "A9", "A9", "I4", "J4", "A9", "A9", "A9", "J1", "J1",
	"J1", "I4", "J4", "A9", "A9", "I5", "J5", "A9", "A9", "A4", "A9", "A9", "A9", "A9", "A9", "A9", "J2", "A9", "A9", "J1",
	"J1", "A9", "A9", "A9", "A9", "I5", "J5", "A9", "A9", "A4", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "A9", "J1",
	"J1", "H1", "A9", "A9", "A9", "I5", "J5", "A9", "A9", "A4", "A9", "A9", "A9", "J1", "A9", "A9", "A9", "I3", "J3", "J1",
	"J1", "J1", "A9", "A9", "A9", "I4", "J4", "A9", "A9", "A4", "A9", "A9", "A9", "A9", "J1", "J1", "A9", "I4", "J4", "J1",
	"J1", "J1", "J1", "J1", "H1", "A9", "A9", "A9", "A9", "A4", "A9", "A9", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1",
	"J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "A4", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1", "J1",
};

SpriteID sprites[(MAP_SPRITE_SHEET_WIDTH / MAP_SPRITE_WIDTH) * (MAP_SPRITE_SHEET_HEIGHT / MAP_SPRITE_HEIGHT)] = {};
SpriteID characterSprites[(CHARACTER_SPRITE_SHEET_WIDTH / CHARACTER_SPRITE_WIDTH) * (CHARACTER_SPRITE_SHEET_HEIGHT / CHARACTER_SPRITE_HEIGHT)] = {};
bool bufferTag = true; // Boolean to keep track or what buffer is being displayed. true = buffer1, false = buffer2

void LoadMapSprite(int x1, int y1, string spriteName);	// Function to load map sprites to spriteMap
void LoadCharacterSprite(int x1, int y1, string spriteName);	// Function to load character sprites to spriteMap
void Update();
void Input();
void Draw(bool bufferTag);

wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
HANDLE buffer1 = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
HANDLE buffer2 = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
DWORD dwScreenBufferData = 0;
wstring appName;
wchar_t s[256];

int main() {
	
	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	SetConsoleActiveScreenBuffer(hConsole);

	_COORD coord = { SCREEN_WIDTH, SCREEN_HEIGHT };

	SetConsoleScreenBufferSize(hConsole, coord);
	SetConsoleScreenBufferSize(buffer1, coord);
	SetConsoleScreenBufferSize(buffer2, coord);

	_CONSOLE_FONT_INFOEX font = { 0 };
	font.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	font.nFont;
	font.dwFontSize.X = 2;
	font.dwFontSize.Y = 2;
	font.FaceName;
	font.FontFamily = FF_DONTCARE;
	font.FontWeight = FW_NORMAL;

	SetCurrentConsoleFontEx(hConsole, false, &font);
	SetCurrentConsoleFontEx(buffer1, false, &font);
	SetCurrentConsoleFontEx(buffer2, false, &font);
	SetConsoleActiveScreenBuffer(hConsole);

	// Assign codes to sprites

	string temp;
	char letter = 'A';
	int number = 1;
	int x = 0;
	int y = 0;
	string spriteID;

	for (int i = 0; i < (MAP_SPRITE_SHEET_WIDTH / MAP_SPRITE_WIDTH); ++i) {
		for (int j = 0; j < (MAP_SPRITE_SHEET_HEIGHT / MAP_SPRITE_HEIGHT); j++) {
			sprites[j * (MAP_SPRITE_SHEET_WIDTH / MAP_SPRITE_WIDTH) + i] = {letter + to_string(number), x * MAP_SPRITE_WIDTH, y * MAP_SPRITE_HEIGHT };
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
	
	Update();
	while (true) { 
		Input();
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		swprintf_s(s, 256, L"OnBiGamE - %s - FPS: %3.2f", appName.c_str(), 1.0f / fElapsedTime);
		SetConsoleTitle(s);
	}
	return 0;
}

void LoadMapSprite(int x1, int y1, string spriteName) {
	SpriteID temp;
	temp = {spriteName, 0, 0};

	for (int i = 0; i < (SCREEN_WIDTH / MAP_SPRITE_WIDTH) * (SCREEN_HEIGHT / MAP_SPRITE_HEIGHT); i++) {
		if (sprites[i].name == temp.name) {
			temp.x = sprites[i].x;
			temp.y = sprites[i].y;
			break;
		}
	}

	for (int i = 0 + x1; i < (MAP_SPRITE_WIDTH + x1); i++) {
		for (int j = 0 + y1; j < (MAP_SPRITE_HEIGHT + y1); j++) {
			screen[j * SCREEN_WIDTH + i] = spriteSheet[(temp.y + j - y1) * MAP_SPRITE_SHEET_WIDTH + (temp.x + i - x1)];
		}
	}
}

void LoadCharacterSprite(int x1, int y1, string spriteName) {
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

void Update() {
	for (int i = 0; i < SCREEN_WIDTH / MAP_SPRITE_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT / MAP_SPRITE_HEIGHT; j++) {
			LoadMapSprite(i * MAP_SPRITE_WIDTH, j * MAP_SPRITE_HEIGHT, spriteMap[j * (SCREEN_WIDTH / MAP_SPRITE_WIDTH) + i]);
		}
	}
}

void Input() {
		if (GetAsyncKeyState(0x53)) {	// S
			Update();
			LoadCharacterSprite(charPositition.x, charPositition.y += 4, "A1");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x, charPositition.y += 4, "B1");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x, charPositition.y += 4, "C1");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x, charPositition.y += 4, "B1");
			Draw(bufferTag);
		}
		else if (GetAsyncKeyState(0x41)) { // A
			Update();
			LoadCharacterSprite(charPositition.x -= 4, charPositition.y, "A2");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x -= 4, charPositition.y, "B2");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x -= 4, charPositition.y, "C2");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x -= 4, charPositition.y, "B2");
			Draw(bufferTag);
	
		}
		else if (GetAsyncKeyState(0x44)) {	// D
			Update();
			LoadCharacterSprite(charPositition.x += 4, charPositition.y, "A3");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x += 4, charPositition.y, "B3");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x += 4, charPositition.y, "C3");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x += 4, charPositition.y, "B3");
			Draw(bufferTag);
	
		}
		else if (GetAsyncKeyState(0x57)) { // W
			Update();
			LoadCharacterSprite(charPositition.x, charPositition.y -= 4, "A4");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x, charPositition.y -= 4, "B4");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x, charPositition.y -= 4, "C4");
			Draw(bufferTag);
			Update();
			LoadCharacterSprite(charPositition.x, charPositition.y -= 4, "B4");
			Draw(bufferTag);
		}
}
void Draw(bool bufferTag) {
	HANDLE tempBuffer;

	if (bufferTag == false) {
		tempBuffer = buffer1;
		SetConsoleActiveScreenBuffer(tempBuffer);
		screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = '\0';
		WriteConsoleOutputCharacter(tempBuffer, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwScreenBufferData);
		bufferTag = true;
	}
	else if (bufferTag == true) {
		tempBuffer = buffer2;
		SetConsoleActiveScreenBuffer(tempBuffer);
		screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = '\0';
		WriteConsoleOutputCharacter(tempBuffer, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwScreenBufferData);
		bufferTag = false;
	}
}