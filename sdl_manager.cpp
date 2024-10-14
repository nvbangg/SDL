#include "common.h"

// Hàm handleError xử lý lỗi và giải phóng tài nguyên
// message: Thông báo lỗi cần hiển thị
void handleError(const string &message, SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm)
{
    // Hiển thị thông báo lỗi và lỗi SDL
    cerr << message << " SDL Error: " << SDL_GetError() << endl;
    // Giải phóng font nếu nó đã được khởi tạo
    if (font)
        TTF_CloseFont(font);
    // Giải phóng âm thanh nếu nó đã được khởi tạo
    if (alarm)
        Mix_FreeChunk(alarm);
    // Đóng âm thanh
    Mix_CloseAudio();
    // Kết thúc SDL_ttf
    TTF_Quit();
    // Giải phóng renderer nếu nó đã được khởi tạo
    if (renderer)
        SDL_DestroyRenderer(renderer);
    // Giải phóng cửa sổ nếu nó đã được khởi tạo
    if (window)
        SDL_DestroyWindow(window);
    // Kết thúc SDL
    SDL_Quit();
    // Thoát chương trình với mã lỗi -1
    exit(-1);
}

// Hàm xử lý sự kiện cửa sổ
void handleWindowEvent(SDL_Event &e, SDL_Window *window)
{
    if (e.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        int newWidth = e.window.data1;
        int newHeight = e.window.data2;

        if (newWidth < 500)
        {
            newWidth = 500;
        }
        if (newHeight < 400)
        {
            newHeight = 400;
        }

        SDL_SetWindowSize(window, newWidth, newHeight);
        WINDOW_WIDTH = newWidth;
        WINDOW_HEIGHT = newHeight;
    }
}

// Hàm initSDL khởi tạo các thành phần của SDL
void initSDL(SDL_Window *&window, SDL_Renderer *&renderer, TTF_Font *&font, Mix_Chunk *&alarm)
{
    // Khởi tạo SDL và kiểm tra lỗi
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        handleError("SDL could not initialize!", window, renderer, font, alarm);
    // Tạo cửa sổ và kiểm tra lỗi
    window = SDL_CreateWindow("Countdown Timer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window)
        handleError("Window could not be created!", window, renderer, font, alarm);
    // Tạo renderer SDL với cửa sổ đã tạo, sử dụng driver mặc định (-1), và sử dụng tăng tốc phần cứng
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        handleError("Renderer could not be created!", window, renderer, font, alarm);
    // Khởi tạo SDL_ttf và kiểm tra lỗi
    if (TTF_Init() == -1)
        handleError("TTF could not initialize!", window, renderer, font, alarm);
    // Tải font và kiểm tra lỗi
    font = TTF_OpenFont("data/digital.ttf", 100);
    if (!font)
        handleError("Font could not be loaded!", window, renderer, font, alarm);
    // Khởi tạo SDL_mixer và kiểm tra lỗi
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        handleError("SDL_mixer could not initialize!", window, renderer, font, alarm);
    // Tải âm thanh và kiểm tra lỗi
    alarm = Mix_LoadWAV("data/alarm.mp3");
    if (!alarm)
        handleError("Sound could not be loaded!", window, renderer, font, alarm);

    // Bắt đầu nhận đầu vào văn bản từ người dùng
    SDL_StartTextInput();
}

void saveTimeToFile(int countdownTime)
{
    std::ofstream outFile("config.txt");
    if (outFile.is_open())
    {
        outFile << countdownTime;
        outFile.close();
    }
}

int loadTimeFromFile()
{
    std::ifstream inFile("config.txt");
    int countdownTime = 0;
    if (inFile.is_open())
    {
        inFile >> countdownTime;
        inFile.close();
    }
    return countdownTime;
}
void resetTimeInFile()
{
    std::ofstream outFile("config.txt");
    if (outFile.is_open())
    {
        outFile << 0;
        outFile.close();
    }
}

// Hàm closeSDL giải phóng các tài nguyên của SDL
void closeSDL(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm)
{
    // Giải phóng font nếu nó đã được khởi tạo
    if (font)
        TTF_CloseFont(font);
    // Giải phóng âm thanh nếu nó đã được khởi tạo
    if (alarm)
        Mix_FreeChunk(alarm);
    // Giải phóng renderer nếu nó đã được khởi tạo
    if (renderer)
        SDL_DestroyRenderer(renderer);
    // Giải phóng cửa sổ nếu nó đã được khởi tạo
    if (window)
        SDL_DestroyWindow(window);
    // Đóng âm thanh
    Mix_CloseAudio();
    // Kết thúc SDL_ttf
    TTF_Quit();
    // Kết thúc SDL_mixer
    Mix_Quit();
    // Kết thúc SDL
    SDL_Quit();
}