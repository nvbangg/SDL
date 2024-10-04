#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
using namespace std;

const int WINDOW_WIDTH = 600, WINDOW_HEIGHT = 300;

// Hàm giải phóng tài nguyên
void close(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm)
{
    if (font)
        TTF_CloseFont(font);
    if (alarm)
        Mix_FreeChunk(alarm);
    Mix_CloseAudio();
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// Chuyển đổi thời gian thành định dạng "HH:MM:SS"
string formatTime(int seconds)
{
    int h = seconds / 3600, m = (seconds % 3600) / 60, s = seconds % 60;
    return (h < 10 ? "0" : "") + to_string(h) + ":" + (m < 10 ? "0" : "") + to_string(m) + ":" + (s < 10 ? "0" : "") + to_string(s);
}

// Hàm nhận và trả về giá trị thời gian (h, m, s)
int inputTime(SDL_Renderer *renderer, TTF_Font *font, const string &prompt, int maxValue)
{
    SDL_Color textColor = {255, 255, 255};
    string inputText = "";
    SDL_Event e;
    SDL_StartTextInput(); // Bắt đầu nhận sự kiện bàn phím
    while (true)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                exit(0);
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && !inputText.empty())
            {
                int val = stoi(inputText);
                if (val <= maxValue)
                    return val; // Trả về giá trị hợp lệ
                inputText = ""; // Xóa input nếu giá trị không hợp lệ
            }
            if (e.type == SDL_TEXTINPUT && isdigit(e.text.text[0]))
                inputText += e.text.text;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty())
                inputText.pop_back();
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); // Xóa màn hình
        SDL_Surface *promptSurface = TTF_RenderText_Solid(font, prompt.c_str(), textColor);
        if (!promptSurface)
        {
            cerr << "Error rendering text: " << TTF_GetError() << endl;
            break;
        }
        SDL_Texture *promptTexture = SDL_CreateTextureFromSurface(renderer, promptSurface);
        SDL_Rect promptRect = {50, 50, promptSurface->w, promptSurface->h}; // Định vị prompt
        SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);
        SDL_DestroyTexture(promptTexture);
        SDL_FreeSurface(promptSurface);

        SDL_Surface *inputSurface = TTF_RenderText_Solid(font, inputText.c_str(), textColor);
        if (!inputSurface)
        {
            cerr << "Error rendering input text: " << TTF_GetError() << endl;
            break;
        }
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
        SDL_Rect inputRect = {50, 150, inputSurface->w, inputSurface->h}; // Định vị input
        SDL_RenderCopy(renderer, inputTexture, NULL, &inputRect);
        SDL_DestroyTexture(inputTexture);
        SDL_FreeSurface(inputSurface);

        SDL_RenderPresent(renderer); // Cập nhật màn hình
    }
    return 0;
}

// Hàm khởi tạo SDL và nhập thời gian
int initAndInput(SDL_Window *&window, SDL_Renderer *&renderer, TTF_Font *&font, Mix_Chunk *&alarm)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return -1;
    }

    if (TTF_Init() == -1)
    {
        cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
        return -1;
    }

    window = SDL_CreateWindow("Đồng hồ điện tử đếm ngược", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        return -1;
    }

    font = TTF_OpenFont("data/Digital_Clock.otf", 50);
    if (!font)
    {
        cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << endl;
        return -1;
    }

    alarm = Mix_LoadWAV("data/alarm.mp3"); // Tải âm thanh cảnh báo
    if (!alarm)
    {
        cerr << "Failed to load alarm sound! Mix_Error: " << Mix_GetError() << endl;
        return -1;
    }

    int h = inputTime(renderer, font, "Nhập giờ (0-23):", 23);
    int m = inputTime(renderer, font, "Nhập phút (0-59):", 59);
    int s = inputTime(renderer, font, "Nhập giây (0-59):", 59);
    SDL_StopTextInput(); // Tắt chế độ nhận sự kiện bàn phím
    return h * 3600 + m * 60 + s;
}

// Hàm đếm ngược và hiển thị thời gian
void runCountdown(SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm, int countdownTime)
{
    SDL_Color textColor = {0, 255, 0};
    bool running = true, alarmPlayed = false;
    SDL_Event e;
    Uint32 startTime = SDL_GetTicks();
    while (running)
    {
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT)
                running = false;
        int remainingTime = countdownTime - (SDL_GetTicks() - startTime) / 1000;
        if (remainingTime <= 0)
        {
            remainingTime = 0;
            if (!alarmPlayed)
            {
                Mix_PlayChannel(-1, alarm, 0);
                alarmPlayed = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); // Xóa màn hình
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, formatTime(remainingTime).c_str(), textColor);
        if (!textSurface)
        {
            cerr << "Error rendering countdown text: " << TTF_GetError() << endl;
            break;
        }
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {WINDOW_WIDTH / 2 - textSurface->w / 2, WINDOW_HEIGHT / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect); // Hiển thị thời gian đếm ngược
        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(textSurface);
        SDL_RenderPresent(renderer); // Cập nhật màn hình
    }
}

int main(int argc, char *args[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;
    Mix_Chunk *alarm = NULL;
    int countdownTime = initAndInput(window, renderer, font, alarm); // Nhập thời gian
    if (countdownTime > 0)
    {
        runCountdown(renderer, font, alarm, countdownTime); // Chạy đếm ngược
    }
    close(window, renderer, font, alarm); // Giải phóng tài nguyên
    return 0;
}
