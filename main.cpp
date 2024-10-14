#include "common.h"

int WINDOW_WIDTH = 600; // Chiều rộng cửa sổ SDL
int WINDOW_HEIGHT = 400; // Chiều cao cửa sổ SDL

SDL_Window *window = NULL;     // Con trỏ tới cửa sổ SDL
SDL_Renderer *renderer = NULL; // Con trỏ tới renderer SDL
TTF_Font *font = NULL;         // Con trỏ tới font chữ SDL
Mix_Chunk *alarm = NULL;       // Con trỏ tới âm thanh báo động SDL

int main(int argc, char *argv[])
{
    initSDL(window, renderer, font, alarm);

    bool reset = false; // Biến để kiểm tra xem có cần reset đếm ngược không
    do
    {
        // Tải thời gian từ file
        int countdownTime = loadTimeFromFile();

        // Hiển thị cửa sổ input với thời gian mặc định từ file
        countdownTime = inputTime(window, renderer, font, countdownTime);

        // Chạy đếm ngược và kiểm tra xem có cần reset không
        reset = runCountdown(window, renderer, font, alarm, countdownTime);

        // Nếu cần reset, đặt lại thời gian trong file về 0
        if (reset)
        {
            resetTimeInFile();
        }
    } while (reset); // Lặp lại nếu cần reset

    // Đóng SDL và giải phóng tài nguyên
    closeSDL(window, renderer, font, alarm);

    return 0; // Kết thúc chương trình
}