#include <Windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <chrono>

#define SCREEN_WIDTH 512			
#define SCREEN_HEIGHT 288
#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define TILE_MAP_WIDTH 320
#define TILE_MAP_HEIGHT 416
#define SPRITE_WIDTH 32
#define SPRITE_HEIGHT 32
#define SPRITE_SHEET_WIDTH 96
#define SPRITE_SHEET_HEIGHT 192
#define MAP_WIDTH 46 // Tiles not pixels
#define MAP_HEIGHT 27 // Tiles not pixels

using namespace std;

struct AssetID	// Used for map tiles and character sprites IDs
{
	int id = 0, x = 0, y = 0;
};

wstring tileSheet;	// To store tile sheet
wstring characterSpriteSheet;	// To store character sprite sheet
vector<int> map; // To store full map. Composed of 46x27 tiles of 32x32 pixels
int playerPositionX = 288;	// Initial player position x
int playerPositionY = 32;	// Initial player position y
int cameraPositionX = 0;
int cameraPositionY = 0;

AssetID tiles[(TILE_MAP_WIDTH / TILE_WIDTH) * (TILE_MAP_HEIGHT / TILE_HEIGHT)] = {}; // Total number of tiles on tileSheet
AssetID characterSprites[(SPRITE_SHEET_WIDTH / SPRITE_WIDTH) * (SPRITE_SHEET_HEIGHT / SPRITE_HEIGHT)] = {}; // Total number of sprites of characterSpriteSheet
bool bufferTag = true; // Boolean to keep track or what buffer is being displayed. true = buffer1, false = buffer2

void LoadMapTile(int x1, int y1, int tileID);	// Function to load map tiles to screen character array
void LoadCharacterSprite(int x1, int y1, int spriteID);	// Function to load character sprites to screen character array
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
auto fpsTime1 = chrono::system_clock::now();
auto fpsTime2 = chrono::system_clock::now();
wstring appName = L"One Bit Game Engine";
wchar_t s[256];

int main() {
	
	SetConsoleActiveScreenBuffer(buffer1);

	_COORD coord = { SCREEN_WIDTH, SCREEN_HEIGHT };

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

	SetCurrentConsoleFontEx(buffer1, false, &font);
	SetCurrentConsoleFontEx(buffer2, false, &font);
	SetConsoleActiveScreenBuffer(buffer1);

	// Assign codes to tiles
	string temp;
	int id = 1;
	int x = 0;
	int y = 0;

	for (int i = 0; i < (TILE_MAP_WIDTH / TILE_WIDTH); i++) {
		for (int j = 0; j < (TILE_MAP_HEIGHT / TILE_HEIGHT); j++) {
			id = (j * 10) + i + 1;
			tiles[j * (TILE_MAP_WIDTH / TILE_WIDTH) + i] = {id, x * TILE_WIDTH, y * TILE_HEIGHT };
			y++;
		}
		x++;
		y = 0;
	}

	//Assign codes to character sprites
	// Reset values
	id = 1;
	x = 0;
	y = 0;
	
	for (int i = 0; i < (SPRITE_SHEET_WIDTH / SPRITE_WIDTH); i++) {
		for (int j = 0; j < (SPRITE_SHEET_HEIGHT / SPRITE_HEIGHT); j++) {
			id = (j * 3) + i + 1;
			characterSprites[j * (SPRITE_SHEET_WIDTH / SPRITE_WIDTH) + i] = { id, x * SPRITE_WIDTH, y * SPRITE_HEIGHT };
			id = (i * j) + j;
			y++;
		}
		x++;
		y = 0;
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
	ifstream file2("player32x32.dat");
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

	// Load Map
	ifstream file3("map46x27.dat");
	if (file3.is_open())
	{
		while (getline(file3, temp))
		{
			stringstream linestream(temp);
			string token;
			while (getline(linestream, token, ',')) {
				map.push_back(stoi(token));
			}
		}
	}

	// Game loop
	while (true) { 
		Input();
		Draw(bufferTag);
	}
	return 0;
}

void LoadMapTile(int x1, int y1, int tileId) {
	AssetID temp = {tileId, 0, 0};

	for (int i = 0; i < (SCREEN_WIDTH / TILE_WIDTH) * (SCREEN_HEIGHT / TILE_HEIGHT); i++) {
		if (tiles[i].id == temp.id) {
			temp.x = tiles[i].x;
			temp.y = tiles[i].y;
			break;
		}
	}

	for (int i = 0 + x1; i < (TILE_WIDTH + x1); i++) {
		for (int j = 0 + y1; j < (TILE_HEIGHT + y1); j++) {
			screen[j * SCREEN_WIDTH + i] = tileSheet[(temp.y + static_cast<std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>::size_type>(j) - y1) * TILE_MAP_WIDTH + (temp.x + static_cast<unsigned long long>(i) - x1)];
		}
	}
}

void LoadCharacterSprite(int x1, int y1, int spriteId) {
	AssetID temp = {spriteId, 0, 0 };

	for (int i = 0; i < (SPRITE_SHEET_WIDTH / SPRITE_WIDTH) * (SPRITE_SHEET_HEIGHT / SPRITE_HEIGHT); i++) {
		if (characterSprites[i].id == temp.id) {
			temp.x = characterSprites[i].x;
			temp.y = characterSprites[i].y;
			break;
		}
	}

	for (int i = 0 + x1; i < (SPRITE_WIDTH + x1); i++) {
		for (int j = 0 + y1; j < (SPRITE_HEIGHT + y1); j++) {
			if (characterSpriteSheet[(temp.y + static_cast<std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>::size_type>(j) - y1) * SPRITE_SHEET_WIDTH + (temp.x + static_cast<unsigned long long>(i) - x1)] == L'\u2588' || characterSpriteSheet[(temp.y + static_cast<std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>::size_type>(j) - y1) * SPRITE_SHEET_WIDTH + (temp.x + static_cast<unsigned long long>(i) - x1)] == L' ') {
				screen[j * SCREEN_WIDTH + i] = characterSpriteSheet[(temp.y + static_cast<std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>::size_type>(j) - y1) * SPRITE_SHEET_WIDTH + (temp.x + static_cast<unsigned long long>(i) - x1)];
			}
		}
	}
}

void Update() {
	for (int i = 0; i < SCREEN_WIDTH / TILE_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT / TILE_HEIGHT; j++) {
			LoadMapTile(i * TILE_WIDTH, j * TILE_HEIGHT, map.at(static_cast<std::vector<int, std::allocator<int>>::size_type>(j) * MAP_WIDTH + i));
		}
	}
}

void Input() {
	if (GetAsyncKeyState(0x53)) {	// S
		keyInput =  'S';
		if (playerPositionY < SCREEN_HEIGHT - SPRITE_HEIGHT) {
			Update();
			if (prevSprite == 0)
				LoadCharacterSprite(playerPositionX, playerPositionY += 4, 1);

			else if (prevSprite == 1)
				LoadCharacterSprite(playerPositionX, playerPositionY += 4, 2);

			else if (prevSprite == 2)
				LoadCharacterSprite(playerPositionX, playerPositionY += 4, 3);

			else if (prevSprite == 3)
				LoadCharacterSprite(playerPositionX, playerPositionY += 4, 2);
		}
			prevKeyInput = keyInput;
			if (prevSprite < 3)
				prevSprite += 1;
			else
				prevSprite = 0;

	}
	else if (GetAsyncKeyState(0x41)) { // A
		keyInput = 'A';
		if (playerPositionX > 0) {
			Update();
			if (prevSprite == 0)
				LoadCharacterSprite(playerPositionX -= 4, playerPositionY, 4);

			else if (prevSprite == 1)
				LoadCharacterSprite(playerPositionX -= 4, playerPositionY, 5);

			else if (prevSprite == 2)
				LoadCharacterSprite(playerPositionX -= 4, playerPositionY, 6);

			else if (prevSprite == 3)
				LoadCharacterSprite(playerPositionX -= 4, playerPositionY, 5);
		}
			prevKeyInput = keyInput;
			if (prevSprite < 3)
				prevSprite += 1;
			else
				prevSprite = 0;
	}
	else if (GetAsyncKeyState(0x44)) {	// D
		keyInput = 'D';
		if (playerPositionX < SCREEN_WIDTH - SPRITE_WIDTH) {
			Update();
			if (prevSprite == 0)
				LoadCharacterSprite(playerPositionX += 4, playerPositionY, 7);

			else if (prevSprite == 1)
				LoadCharacterSprite(playerPositionX += 4, playerPositionY, 8);

			else if (prevSprite == 2)
				LoadCharacterSprite(playerPositionX += 4, playerPositionY, 9);

			else if (prevSprite == 3)
				LoadCharacterSprite(playerPositionX += 4, playerPositionY, 8);
		}
			prevKeyInput = keyInput;
			if (prevSprite < 3)
				prevSprite += 1;
			else
				prevSprite = 0;

	}
	else if (GetAsyncKeyState(0x57)) { // W
		keyInput = 'W';
		if (playerPositionY > 0) {
			Update();
			if (prevSprite == 0)
				LoadCharacterSprite(playerPositionX, playerPositionY -= 4, 10);

			else if (prevSprite == 1)
				LoadCharacterSprite(playerPositionX, playerPositionY -= 4, 11);

			else if (prevSprite == 2)
				LoadCharacterSprite(playerPositionX, playerPositionY -= 4, 12);

			else if (prevSprite == 3)
				LoadCharacterSprite(playerPositionX, playerPositionY -= 4, 11);
		}
			prevKeyInput = keyInput;
			if (prevSprite < 3)
				prevSprite += 1;
			else
				prevSprite = 0;
	}
	else if(!GetAsyncKeyState(0x53) || !GetAsyncKeyState(0x41) || !GetAsyncKeyState(0x44) || !GetAsyncKeyState(0x57)) {
		Update();
		if(prevKeyInput == 'S')
			LoadCharacterSprite(playerPositionX, playerPositionY += 0, 2);
		else if (prevKeyInput == 'A')
			LoadCharacterSprite(playerPositionX, playerPositionY += 0, 5);
		else if (prevKeyInput == 'D')
			LoadCharacterSprite(playerPositionX, playerPositionY += 0, 8);
		else if (prevKeyInput == 'W')
			LoadCharacterSprite(playerPositionX, playerPositionY += 0, 11);
	}
}
void Draw(bool bufferTag) {
	HANDLE tempBuffer;
	fpsTime2 = chrono::system_clock::now();
	chrono::duration<float> passedTime = fpsTime2 - fpsTime1;
	fpsTime1 = fpsTime2;
	float elapsedTime = passedTime.count();

	swprintf_s(s, 256, L"%s - FPS: %3.2f", appName.c_str(), 1.0f / elapsedTime);
	SetConsoleTitle(s);

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
	//Sleep(25); // To cap FPS to 20 approximately and keep animation at normal speed.
}