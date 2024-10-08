#include "make/sdl_utils.h"
#include <iostream>
using namespace std;

// Hàm handleError xử lý lỗi và giải phóng tài nguyên
// message: Thông báo lỗi cần hiển thị
// window: Con trỏ tới SDL_Window để giải phóng tài nguyên
// renderer: Con trỏ tới SDL_Renderer để giải phóng tài nguyên
// font: Con trỏ tới TTF_Font để giải phóng tài nguyên
// alarm: Con trỏ tới Mix_Chunk để giải phóng tài nguyên
void handleError(const string &message, SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm)
{
    cerr << message << " SDL Error: " << SDL_GetError() << endl; // Hiển thị thông báo lỗi và lỗi SDL
    if (font)
        TTF_CloseFont(font); // Giải phóng font nếu nó đã được khởi tạo
    if (alarm)
        Mix_FreeChunk(alarm); // Giải phóng âm thanh nếu nó đã được khởi tạo
    Mix_CloseAudio();         // Đóng âm thanh
    TTF_Quit();               // Kết thúc SDL_ttf
    if (renderer)
        SDL_DestroyRenderer(renderer); // Giải phóng renderer nếu nó đã được khởi tạo
    if (window)
        SDL_DestroyWindow(window); // Giải phóng cửa sổ nếu nó đã được khởi tạo
    SDL_Quit();                    // Kết thúc SDL
    exit(-1);                      // Thoát chương trình với mã lỗi -1
}

// Hàm initSDL khởi tạo các thành phần của SDL
// window: Tham chiếu tới con trỏ SDL_Window để khởi tạo cửa sổ
// renderer: Tham chiếu tới con trỏ SDL_Renderer để khởi tạo bộ vẽ
// font: Tham chiếu tới con trỏ TTF_Font để khởi tạo font chữ
// alarm: Tham chiếu tới con trỏ Mix_Chunk để khởi tạo âm thanh
void initSDL(SDL_Window *&window, SDL_Renderer *&renderer, TTF_Font *&font, Mix_Chunk *&alarm)
{
    // Khởi tạo SDL và kiểm tra lỗi
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        handleError("SDL could not initialize!", window, renderer, font, alarm);
    // Tạo cửa sổ và kiểm tra lỗi
    window = SDL_CreateWindow("Countdown Timer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_SHOWN);
    if (!window)
        handleError("Window could not be created!", window, renderer, font, alarm);
    // Tạo renderer và kiểm tra lỗi
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        handleError("Renderer could not be created!", window, renderer, font, alarm);
    // Khởi tạo SDL_ttf và kiểm tra lỗi
    if (TTF_Init() == -1)
        handleError("TTF could not initialize!", window, renderer, font, alarm);
    // Tải font và kiểm tra lỗi
    font = TTF_OpenFont("data/digital.ttf", 50);
    if (!font)
        handleError("Font could not be loaded!", window, renderer, font, alarm);
    // Khởi tạo SDL_mixer và kiểm tra lỗi
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        handleError("SDL_mixer could not initialize!", window, renderer, font, alarm);
    // Tải âm thanh và kiểm tra lỗi
    alarm = Mix_LoadWAV("data/alarm.mp3");
    if (!alarm)
        handleError("Sound could not be loaded!", window, renderer, font, alarm);

    SDL_StartTextInput(); // Bắt đầu nhận đầu vào văn bản từ người dùng
}

// Hàm closeSDL giải phóng các tài nguyên của SDL
// window: Con trỏ tới SDL_Window để giải phóng tài nguyên
// renderer: Con trỏ tới SDL_Renderer để giải phóng tài nguyên
// font: Con trỏ tới TTF_Font để giải phóng tài nguyên
// alarm: Con trỏ tới Mix_Chunk để giải phóng tài nguyên
void closeSDL(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm)
{
    if (font)
        TTF_CloseFont(font); // Giải phóng font nếu nó đã được khởi tạo
    if (alarm)
        Mix_FreeChunk(alarm); // Giải phóng âm thanh nếu nó đã được khởi tạo
    Mix_CloseAudio();         // Đóng âm thanh
    TTF_Quit();               // Kết thúc SDL_ttf
    if (renderer)
        SDL_DestroyRenderer(renderer); // Giải phóng renderer nếu nó đã được khởi tạo
    if (window)
        SDL_DestroyWindow(window); // Giải phóng cửa sổ nếu nó đã được khởi tạo
    SDL_Quit();                    // Kết thúc SDL
}