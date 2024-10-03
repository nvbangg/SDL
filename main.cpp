#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <bits/stdc++.h>
using namespace std;

// Kích thước cửa sổ
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 300;

// Hàm giải phóng tài nguyên
void close(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm)
{
    if (font != NULL)
    {
        TTF_CloseFont(font);
    }
    Mix_FreeChunk(alarm); // Giải phóng âm thanh
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

// Hàm khai báo các điều kiện cần thiết và nhập thời gian
int initAndInput(SDL_Window *&window, SDL_Renderer *&renderer, TTF_Font *&font, Mix_Chunk *&alarm)
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
    font = TTF_OpenFont("data/Digital_Clock.otf", 100);
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

    // Nhập thời gian
    int hours, minutes, seconds;
    do
    {
        cout << "Enter hours: ";
        cin >> hours;
        cout << "Enter minutes: ";
        cin >> minutes;
        cout << "Enter seconds: ";
        cin >> seconds;
        if (hours < 0 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60)
            cout << "Thời gian không hợp lệ! Vui lòng nhập lại." << endl;
    } while (hours < 0 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60);

    return hours * 3600 + minutes * 60 + seconds; // Trả về tổng số giây
}

// Hàm xử lý và hiển thị đếm ngược
void runCountdown(SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm, int countdownTime)
{
    SDL_Color textColor = {0, 255, 0}; // Màu chữ xanh lá cho chữ số điện tử
    bool running = true;
    SDL_Event e;
    Uint32 startTime = SDL_GetTicks(); // Lấy thời gian bắt đầu
    int lastRemainingTime = -1;        // Biến lưu lại thời gian cũ để so sánh

    // Vòng lặp chính
    while (running)
    {
        // Xử lý sự kiện
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Tính thời gian đã trôi qua
        Uint32 currentTime = SDL_GetTicks();
        int elapsedSeconds = (currentTime - startTime) / 1000;
        int remainingTime = countdownTime - elapsedSeconds;

        // Nếu hết thời gian, dừng đếm ngược và phát âm thanh cảnh báo
        if (remainingTime < 0)
        {
            remainingTime = 0;
        }

        // Chỉ cập nhật màn hình khi thời gian thay đổi
        if (remainingTime != lastRemainingTime)
        {
            lastRemainingTime = remainingTime;

            // Chuyển thời gian thành chuỗi "HH:MM:SS"
            string timeText;
            if (remainingTime > 0)
            {
                timeText = formatTime(remainingTime);
            }
            else
            {
                timeText = "TIME OUT";         // Hiển thị "Hết giờ" khi kết thúc
                Mix_PlayChannel(-1, alarm, 0); // Phát âm thanh cảnh báo
            }

            // Xóa màn hình
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu nền đen
            SDL_RenderClear(renderer);

            // Tạo texture từ chuỗi thời gian
            SDL_Surface *textSurface = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
            SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);

            // Lấy kích thước texture
            int textWidth, textHeight;
            SDL_QueryTexture(timeTexture, NULL, NULL, &textWidth, &textHeight);

            // Đặt vị trí để vẽ (giữa màn hình)
            SDL_Rect renderQuad = {(WINDOW_WIDTH - textWidth) / 2, (WINDOW_HEIGHT - textHeight) / 2, textWidth, textHeight};

            // Vẽ texture lên renderer
            SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad);

            // Cập nhật màn hình
            SDL_RenderPresent(renderer);

            // Hủy texture sau khi đã render
            SDL_DestroyTexture(timeTexture);
        }

        // Giới hạn tốc độ khung hình (60 FPS)
        SDL_Delay(1000 / 60);
    }
}

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;
    Mix_Chunk *alarm = NULL;

    // Khai báo và nhập thời gian
    int countdownTime = initAndInput(window, renderer, font, alarm);

    // Chạy đếm ngược
    runCountdown(renderer, font, alarm, countdownTime);

    // Giải phóng tài nguyên
    close(window, renderer, font, alarm);

    return 0;
}
