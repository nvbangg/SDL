#pragma once // Đảm bảo file này chỉ được include một lần trong quá trình biên dịch

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

// Màu sắc
const SDL_Color BACKGROUND_COLOR = {0, 0, 0, 255};   // Màu nền: đen
const SDL_Color TEXT_COLOR = {0, 0, 0, 255};         // Màu chữ: Đen
const SDL_Color TIME_COLOR = {0, 255, 0, 255};       // Màu thời gian: xanh lá
const SDL_Color BUTTON_COLOR = {255, 255, 255, 255}; // Màu nút: trắng
const SDL_Color PAUSE_TEXT_COLOR = {255, 0, 0, 255}; // Màu chữ tạm dừng: đỏ

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

//  Xử lý lỗi
void handleError(const string &message, SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm);
// Hàm xử lý sự kiện cửa sổ
void handleWindowEvent(SDL_Event &e, SDL_Window *window);
// Khởi tạo SDL
void initSDL(SDL_Window *&window, SDL_Renderer *&renderer, TTF_Font *&font, Mix_Chunk *&alarm);
// Đóng SDL
void closeSDL(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm);

// Thiết lập màu nền cho renderer
void clearRenderer(SDL_Renderer *renderer, SDL_Color color);
// Check click button
bool checkClickButton(int x, int y, const SDL_Rect &button);
// Vẽ chữ lên màn hình
void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int xOffset, int yOffset);
// Vẽ nút
void drawButton(SDL_Renderer *renderer, const string &label, SDL_Rect &buttonRect);
// Vẽ thanh tiến trình
void drawProgressBar(SDL_Renderer *renderer, int totalTime, int remainingTime);

// Lưu thời gian vào file
void saveTimeToFile(int countdownTime);
// Tải thời gian từ file
int loadTimeFromFile();
// Đặt lại thời gian trong file về 0
void resetTimeInFile();

//Hàm xử lý đầu vào
int inputTime(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, int defaultTime);
//Hàm đếm ngược
bool runCountdown(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm, int &countdownTime);
