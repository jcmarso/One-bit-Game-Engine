#include <Windows.h>
#include <string>
#include <fstream>
#include <chrono>

#define SCREEN_WIDTH 320			
#define SCREEN_HEIGHT 224
#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define TILE_MAP_WIDTH 320
#define TILE_MAP_HEIGHT 416
#define SPRITE_WIDTH 32
#define SPRITE_HEIGHT 48
#define SPRITE_SHEET_WIDTH 96
#define SPRITE_SHEET_HEIGHT 192

using namespace std;

struct AssetID	// Used for map tiles and character sprites IDs
{
	string name;
	int x = 0, y = 0;
};

wstring tileSheet;	// To store tile sheet
wstring characterSpriteSheet;	// To store character sprite sheet
AssetID charPositition = {"startPoint", 0, 0};	// Initial character position


// Full map. Composed of tiles of 32x32 dimension. In this case represented by a code formed by A to J for columns,
// and 1 to 13 for rows, like "A1", "J2" and so on.

string tileMap[280/*(SCREEN_WIDTH / TILE_WIDTH) * (SCREEN_HEIGHT / TILE_HEIGHT)*/] = {	
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

AssetID tiles[(TILE_MAP_WIDTH / TILE_WIDTH) * (TILE_MAP_HEIGHT / TILE_HEIGHT)] = {}; // Total number of tiles on tileSheet
AssetID characterSprites[(SPRITE_SHEET_WIDTH / SPRITE_WIDTH) * (SPRITE_SHEET_HEIGHT / SPRITE_HEIGHT)] = {}; // Total number of sprites of characterSpriteSheet
bool bufferTag = true; // Boolean to keep track or what buffer is being displayed. true = buffer1, false = buffer2

void LoadMapTile(int x1, int y1, string tileName);	// Function to load map tiles to screen character array
void LoadCharacterSprite(int x1, int y1, string spriteName);	// Function to load character sprites to screen character array
void Update();
void Input();
void Draw(bool bufferTag);

wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT]; // Pointer to character array to be written to console.
HANDLE buffer1 = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
HANDLE buffer2 = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
DWORD dwScreenBufferData = 0;
int prevSprite = 0;
char keyInput = ' ';
char prevKeyInput = ' ';

int main() {
	auto time1 = std::chrono::system_clock::now();
	auto time2 = std::chrono::system_clock::now();
	wstring appName = L"One Bit Game Engine";
	wchar_t s[256];
	SetConsoleActiveScreenBuffer(buffer1);

	_COORD coord = { SCREEN_WIDTH, SCREEN_HEIGHT };

	SetConsoleScreenBufferSize(buffer1, coord);
	SetConsoleScreenBufferSize(buffer1, coord);
	SetConsoleScreenBufferSize(buffer2, coord);

	_CONSOLE_FONT_INFOEX font = { 0 };
	font.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	font.nFont;
	font.dwFontSize.X = 4;
	font.dwFontSize.Y = 4;
	font.FaceName;
	font.FontFamily = FF_DONTCARE;
	font.FontWeight = FW_NORMAL;

	SetCurrentConsoleFontEx(buffer1, false, &font);
	SetCurrentConsoleFontEx(buffer2, false, &font);
	SetConsoleActiveScreenBuffer(buffer1);

	// Assign codes to tiles

	string temp;
	char letter = 'A';
	int number = 1;
	int x = 0;
	int y = 0;
	string AssetID;

	for (int i = 0; i < (TILE_MAP_WIDTH / TILE_WIDTH); ++i) {
		for (int j = 0; j < (TILE_MAP_HEIGHT / TILE_HEIGHT); j++) {
			tiles[j * (TILE_MAP_WIDTH / TILE_WIDTH) + i] = {letter + to_string(number), x * TILE_WIDTH, y * TILE_HEIGHT };
				number++;
				y++;
		}
		letter++;
		x++;
		y = 0;
		number = 1;
	}
	//Assign codes to character sprites
	// Reset values
	letter = 'A';

	number = 1;
	x = 0;
	y = 0;
	
	for (int i = 0; i < (SPRITE_SHEET_WIDTH / SPRITE_WIDTH); ++i) {
		for (int j = 0; j < (SPRITE_SHEET_HEIGHT / SPRITE_HEIGHT); j++) {
			characterSprites[j * (SPRITE_SHEET_WIDTH / SPRITE_WIDTH) + i] = { letter + to_string(number), x * SPRITE_WIDTH, y * SPRITE_HEIGHT };
			number++;
			y++;
		}
		letter++;
		x++;
		y = 0;
		number = 1;
	}
	// Load map tile sheet to tileSheet variable
	ifstream file("EditedTileset_32x32_1bit.dat");
	if (file.is_open())
	{
		while (getline(file, temp))
		{
			for (int i = 0; i < temp.size(); i++) {
				if (temp[i] == '1')
					tileSheet += L'\u2588';
				else if (temp[i] == '0')
					tileSheet += L' ';
			}
		}
	}

	// Load character sprite sheet to characterSpriteSheet variable
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
	Update();
	WriteConsoleOutputCharacter(buffer1, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwScreenBufferData);

	// Game loop
	while (true) { 
		Input();
		Draw(bufferTag);

		time2 = std::chrono::system_clock::now();
		std::chrono::duration<float> passedTime = time2 - time1;
		time1 = time2;
		float elapsedTime = passedTime.count();

		swprintf_s(s, 256, L"%s - FPS: %3.2f", appName.c_str(), 1.0f / elapsedTime);
		SetConsoleTitle(s);

	}
	return 0;
}

void LoadMapTile(int x1, int y1, string tileName) {
	AssetID temp = {tileName, 0, 0};

	for (int i = 0; i < (SCREEN_WIDTH / TILE_WIDTH) * (SCREEN_HEIGHT / TILE_HEIGHT); i++) {
		if (tiles[i].name == temp.name) {
			temp.x = tiles[i].x;
			temp.y = tiles[i].y;
			break;
		}
	}

	for (int i = 0 + x1; i < (TILE_WIDTH + x1); i++) {
		for (int j = 0 + y1; j < (TILE_HEIGHT + y1); j++) {
			screen[j * SCREEN_WIDTH + i] = tileSheet[(temp.y + j - y1) * TILE_MAP_WIDTH + (temp.x + i - x1)];
		}
	}
}

void LoadCharacterSprite(int x1, int y1, string spriteName) {
	AssetID temp = { spriteName, 0, 0 };

	for (int i = 0; i < (SPRITE_SHEET_WIDTH / SPRITE_WIDTH) * (SPRITE_SHEET_HEIGHT / SPRITE_HEIGHT); i++) {
		if (characterSprites[i].name == temp.name) {
			temp.x = characterSprites[i].x;
			temp.y = characterSprites[i].y;
			break;
		}
	}

	for (int i = 0 + x1; i < (SPRITE_WIDTH + x1); i++) {
		for (int j = 0 + y1; j < (SPRITE_HEIGHT + y1); j++) {
			if (characterSpriteSheet[(temp.y + j - y1) * SPRITE_SHEET_WIDTH + (temp.x + i - x1)] == L'\u2588' || characterSpriteSheet[(temp.y + j - y1) * SPRITE_SHEET_WIDTH + (temp.x + i - x1)] == L' ') {
				screen[j * SCREEN_WIDTH + i] = characterSpriteSheet[(temp.y + j - y1) * SPRITE_SHEET_WIDTH + (temp.x + i - x1)];
			}
		}
	}
}

void Update() {
	for (int i = 0; i < SCREEN_WIDTH / TILE_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT / TILE_HEIGHT; j++) {
			LoadMapTile(i * TILE_WIDTH, j * TILE_HEIGHT, tileMap[j * (SCREEN_WIDTH / TILE_WIDTH) + i]);
		}
	}
}

void Input() {
	Update();
	if (GetAsyncKeyState(0x53)) {	// S
		keyInput =  'S';

		if (prevSprite == 0)
			LoadCharacterSprite(charPositition.x, charPositition.y += 2, "A1");

		else if (prevSprite == 1)
			LoadCharacterSprite(charPositition.x, charPositition.y += 2, "B1");

		else if (prevSprite == 2)
			LoadCharacterSprite(charPositition.x, charPositition.y += 2, "C1");

		else if (prevSprite == 3)
			LoadCharacterSprite(charPositition.x, charPositition.y += 2, "B1");

		prevKeyInput = keyInput;
		if (prevSprite < 3)
			prevSprite += 1;
		else
			prevSprite = 0;
	}
	else if (GetAsyncKeyState(0x41)) { // A
		keyInput = 'A';
		if (prevSprite == 0)
			LoadCharacterSprite(charPositition.x -= 2, charPositition.y, "A2");

		else if (prevSprite == 1)
			LoadCharacterSprite(charPositition.x -= 2, charPositition.y, "B2");

		else if (prevSprite == 2)
			LoadCharacterSprite(charPositition.x -= 2, charPositition.y, "C2");

		else if (prevSprite == 3)
			LoadCharacterSprite(charPositition.x -= 2, charPositition.y, "B2");

		prevKeyInput = keyInput;
		if (prevSprite < 3)
			prevSprite += 1;
		else
			prevSprite = 0;
	}
	else if (GetAsyncKeyState(0x44)) {	// D
		keyInput = 'D';

		if (prevSprite == 0)
			LoadCharacterSprite(charPositition.x += 2, charPositition.y, "A3");

		else if (prevSprite == 1)
			LoadCharacterSprite(charPositition.x += 2, charPositition.y, "B3");

		else if (prevSprite == 2)
			LoadCharacterSprite(charPositition.x += 2, charPositition.y, "C3");

		else if (prevSprite == 3)
			LoadCharacterSprite(charPositition.x += 2, charPositition.y, "B3");

		prevKeyInput = keyInput;
		if (prevSprite < 3)
			prevSprite += 1;
		else
			prevSprite = 0;

	}
	else if (GetAsyncKeyState(0x57)) { // W
		keyInput = 'W';

		if (prevSprite == 0)
			LoadCharacterSprite(charPositition.x, charPositition.y -= 2, "A4");

		else if (prevSprite == 1)
			LoadCharacterSprite(charPositition.x, charPositition.y -= 2, "B4");

		else if (prevSprite == 2)
			LoadCharacterSprite(charPositition.x, charPositition.y -= 2, "C4");

		else if (prevSprite == 3)
			LoadCharacterSprite(charPositition.x, charPositition.y -= 2, "B4");

		prevKeyInput = keyInput;
		if (prevSprite < 3)
			prevSprite += 1;
		else
			prevSprite = 0;
	}
	else if(!GetAsyncKeyState(0x53) || !GetAsyncKeyState(0x41) || !GetAsyncKeyState(0x44) || !GetAsyncKeyState(0x57)) {
		if(prevKeyInput == 'S')
			LoadCharacterSprite(charPositition.x, charPositition.y += 0, "B1");
		else if (prevKeyInput == 'A')
			LoadCharacterSprite(charPositition.x, charPositition.y += 0, "B2");
		else if (prevKeyInput == 'D')
			LoadCharacterSprite(charPositition.x, charPositition.y += 0, "B3");
		else if (prevKeyInput == 'W')
			LoadCharacterSprite(charPositition.x, charPositition.y += 0, "B4");
	}
}
void Draw(bool bufferTag) {
	HANDLE tempBuffer;

	if (bufferTag == false) {
		tempBuffer = buffer1;
		SetConsoleActiveScreenBuffer(tempBuffer);
		WriteConsoleOutputCharacter(tempBuffer, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwScreenBufferData);
		bufferTag = true;
	}
	else if (bufferTag == true) {
		tempBuffer = buffer2;
		SetConsoleActiveScreenBuffer(tempBuffer);
		WriteConsoleOutputCharacter(tempBuffer, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwScreenBufferData);
		bufferTag = false;
	}
}