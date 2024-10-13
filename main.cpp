#include "common.h"

int main(int argc, char *argv[])
{

    SDL_Window *window = NULL;     // Con trỏ tới cửa sổ SDL
    SDL_Renderer *renderer = NULL; // Con trỏ tới renderer SDL
    TTF_Font *font = NULL;          // Con trỏ tới font chữ SDL
    Mix_Chunk *alarm = NULL;       // Con trỏ tới âm thanh báo động SDL

    // Khởi tạo và kiểm tra lỗi các thành phần liên quan
    initSDL(window, renderer, font, alarm);

    bool reset = false; // Biến để kiểm tra xem có cần reset đếm ngược không
    do
    {
        // Nhập thời gian đếm ngược từ người dùng
        int countdownTime = inputTime(renderer, font);

        // Chạy đếm ngược và kiểm tra xem có cần reset không
        reset = runCountdown(renderer, font, alarm, countdownTime);
    } while (reset); // Lặp lại nếu cần reset

    // Đóng SDL và giải phóng tài nguyên
    closeSDL(window, renderer, font, alarm);

    return 0; // Kết thúc chương trình
}