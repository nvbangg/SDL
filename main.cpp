#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
using namespace std;

// Kích thước cửa sổ
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 300;

// Hàm khởi tạo SDL, font và âm thanh
void init(SDL_Window *&window, SDL_Renderer *&renderer, TTF_Font *&font, Mix_Chunk *&alarm)
{
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        cout << "SDL không thể khởi tạo! SDL Error: " << SDL_GetError() << endl;
        exit(-1);
    }

    // Tạo cửa sổ
    window = SDL_CreateWindow("Đồng hồ điện tử đếm ngược", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    // Tạo renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1)
    {
        cout << "TTF không thể khởi tạo! TTF Error: " << TTF_GetError() << endl;
        exit(-1);
    }
    // Tải font chữ số điện tử
    font = TTF_OpenFont("data/digital.ttf", 50);
    if (font == NULL)
    {
        cout << "Không thể tải font! TTF Error: " << TTF_GetError() << endl;
        exit(-1);
    }

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cout << "SDL_mixer không thể khởi tạo! SDL_mixer Error: " << Mix_GetError() << endl;
        exit(-1);
    }

    // Tải âm thanh cảnh báo
    alarm = Mix_LoadWAV("data/alarm.mp3");
    if (alarm == NULL)
    {
        cout << "Không thể tải âm thanh! SDL_mixer Error: " << Mix_GetError() << endl;
        exit(-1);
    }

    // Bật chế độ nhận sự kiện bàn phím
    SDL_StartTextInput();
}

// Hàm giải phóng tài nguyên
void close(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm)
{
    if (font != NULL)
    {
        TTF_CloseFont(font);
    }
    if (alarm != NULL)
    {
        Mix_FreeChunk(alarm);
    }
    Mix_CloseAudio();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Chuyển đổi thời gian thành định dạng "HH:MM:SS"
string formatTime(int seconds)
{
    int hours = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return (hours < 10 ? "0" : "") + to_string(hours) + ":" +
           (minutes < 10 ? "0" : "") + to_string(minutes) + ":" +
           (secs < 10 ? "0" : "") + to_string(secs);
}

// Hàm hiển thị và nhập thời gian
int inputTime(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Color textColor = {255, 255, 255}; // Màu trắng

    auto inputText = [&](const string &prompt, int yPosition) -> string
    {
        string inputText = "";
        bool done = false;
        SDL_Event e;
        bool inputChanged = true;

        while (!done)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    done = true;
                    exit(0);
                }
                else if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_RETURN)
                    {
                        done = true; // Kết thúc khi nhấn Enter
                    }
                    else if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
                    {
                        inputText.pop_back(); // Xóa ký tự cuối khi nhấn Backspace
                        inputChanged = true;
                    }
                }
                else if (e.type == SDL_TEXTINPUT)
                {
                    if (isdigit(e.text.text[0]))
                    {
                        inputText += e.text.text;
                        inputChanged = true;
                    }
                }
            }

            if (inputChanged)
            {
                // Chỉ xóa phần nhập của dòng hiện tại
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_Rect clearRect = {50, yPosition, WINDOW_WIDTH - 100, 60}; // Xóa phần của prompt và inputText
                SDL_RenderFillRect(renderer, &clearRect);

                // Vẽ lại prompt
                SDL_Surface *promptSurface = TTF_RenderText_Solid(font, prompt.c_str(), textColor);
                SDL_Texture *promptTexture = SDL_CreateTextureFromSurface(renderer, promptSurface);
                int promptWidth, promptHeight;
                SDL_QueryTexture(promptTexture, NULL, NULL, &promptWidth, &promptHeight);
                SDL_Rect promptRect = {50, yPosition, promptWidth, promptHeight};
                SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);
                SDL_FreeSurface(promptSurface);
                SDL_DestroyTexture(promptTexture);

                // Vẽ lại inputText
                SDL_Surface *textSurface = TTF_RenderText_Solid(font, inputText.c_str(), textColor);
                SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int textWidth, textHeight;
                SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
                SDL_Rect textRect = {50 + promptWidth + 10, yPosition, textWidth, textHeight};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);

                SDL_RenderPresent(renderer);
                inputChanged = false;
            }
        }

        return inputText;
    };

    // Nhập giờ
    int hours = stoi(inputText("Enter hours:", 50));

    // Nhập phút
    int minutes = stoi(inputText("Enter minutes:", 100));

    // Nhập giây
    int seconds = stoi(inputText("Enter seconds:", 150));

    return hours * 3600 + minutes * 60 + seconds;
}

// Hàm xử lý và hiển thị đếm ngược
void runCountdown(SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm, int countdownTime)
{
    SDL_Color textColor = {0, 255, 0}; // Màu chữ xanh lá
    bool running = true;
    SDL_Event e;
    Uint32 startTime = SDL_GetTicks();
    int lastRemainingTime = -1;

    while (running)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        int elapsedSeconds = (currentTime - startTime) / 1000;
        int remainingTime = countdownTime - elapsedSeconds;

        if (remainingTime < 0)
        {
            remainingTime = 0;
        }

        if (remainingTime != lastRemainingTime)
        {
            lastRemainingTime = remainingTime;

            string timeText = (remainingTime > 0) ? formatTime(remainingTime) : "TIME OUT";

            if (remainingTime == 0)
            {
                Mix_PlayChannel(-1, alarm, 0);
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_Surface *textSurface = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
            SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);

            int textWidth, textHeight;
            SDL_QueryTexture(timeTexture, NULL, NULL, &textWidth, &textHeight);
            SDL_Rect renderQuad = {(WINDOW_WIDTH - textWidth) / 2, (WINDOW_HEIGHT - textHeight) / 2, textWidth, textHeight};

            SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad);
            SDL_RenderPresent(renderer);

            SDL_DestroyTexture(timeTexture);
        }

        SDL_Delay(1000 / 60);
    }
}

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;
    Mix_Chunk *alarm = NULL;

    // Khởi tạo các điều kiện cần thiết
    init(window, renderer, font, alarm);

    // Nhập thời gian
    int countdownTime = inputTime(renderer, font);

    // Chạy đếm ngược
    runCountdown(renderer, font, alarm, countdownTime);

    // Giải phóng tài nguyên
    close(window, renderer, font, alarm);

    return 0;
}
