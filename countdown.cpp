#include "make/countdown.h"
#include "make/drawing.h"
#include <iostream>
using namespace std;

// Hàm formatTime định dạng thời gian từ giây thành chuỗi "HH:MM:SS"
// seconds: Thời gian tính bằng giây
// Trả về: Chuỗi định dạng "HH:MM:SS"
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

// Hàm runCountdown thực hiện việc đếm ngược
// renderer: Con trỏ tới SDL_Renderer để vẽ lên màn hình
// font: Con trỏ tới TTF_Font để vẽ chữ lên màn hình
// alarm: Con trỏ tới Mix_Chunk để phát âm thanh báo động khi đếm ngược kết thúc
// countdownTime: Tham chiếu tới biến đếm ngược, giá trị này sẽ bị thay đổi trong quá trình đếm ngược
// Trả về: Trạng thái của hàm, true nếu cần reset, false nếu không
bool runCountdown(SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm, int &countdownTime)
{
    SDL_Color textColor = {0, 255, 0};       // Màu xanh lá cho đồng hồ
    SDL_Color pausedTextColor = {255, 0, 0}; // Màu đỏ cho trạng thái tạm dừng
    bool running = true;                     // Biến kiểm soát vòng lặp đếm ngược
    bool paused = false;                     // Biến kiểm soát trạng thái tạm dừng
    SDL_Event e;                             // Biến sự kiện SDL
    Uint32 startTime = SDL_GetTicks();       // Lấy thời gian bắt đầu
    Uint32 pauseStartTime = 0;               // Thời gian bắt đầu tạm dừng
    int lastRemainingTime = -1;              // Thời gian còn lại lần cuối
    int elapsedPausedTime = 0;               // Tổng thời gian đã tạm dừng

    // Nút Pause và Reset nằm ở dưới cùng của màn hình
    SDL_Rect pauseButton = {600 / 2 - 100, 400 - 60, 80, 40};
    SDL_Rect resetButton = {600 / 2 + 20, 400 - 60, 80, 40};

    TTF_Font *largeFont = TTF_OpenFont("data/digital.ttf", 100); // Mở font chữ lớn với kích thước 100
    if (!largeFont)
    {
        cerr << "Không thể tải font! TTF Error: " << TTF_GetError() << endl; // Hiển thị lỗi nếu không thể tải font
        return false;
    }

    if (countdownTime == 0) // Kiểm tra nếu thời gian đếm ngược là 0
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Đặt màu vẽ là màu đen
        SDL_RenderClear(renderer);                      // Xóa renderer với màu đã thiết lập

        string timeText = "TIME OUT";  // Chuỗi ký tự hiển thị khi hết thời gian
        Mix_PlayChannel(-1, alarm, 0); // Phát âm thanh báo động

        SDL_Surface *textSurface = TTF_RenderText_Solid(largeFont, timeText.c_str(), textColor); // Tạo surface chứa chữ
        SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface);          // Tạo texture từ surface
        SDL_FreeSurface(textSurface); // Giải phóng surface

        int textWidth, textHeight;
        SDL_QueryTexture(timeTexture, NULL, NULL, &textWidth, &textHeight); // Lấy kích thước của texture
        SDL_Rect renderQuad = {(600 - textWidth) / 2, (400 - textHeight) / 2, textWidth, textHeight}; // Đặt vị trí và kích thước của chữ

        SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad); // Vẽ chữ lên màn hình
        SDL_DestroyTexture(timeTexture);                          // Giải phóng texture

        resetButton.x = (600 - resetButton.w) / 2;  // Đặt lại vị trí nút Reset
        drawButton(renderer, "Reset", resetButton); // Vẽ nút Reset

        SDL_RenderPresent(renderer); // Hiển thị renderer lên cửa sổ

        bool reset = false; // Biến kiểm soát vòng lặp reset
        while (!reset)
        {
            while (SDL_PollEvent(&e) != 0) // Xử lý sự kiện
            {
                if (e.type == SDL_QUIT) // Kiểm tra sự kiện thoát
                {
                    running = false;
                    reset = true;
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN) // Kiểm tra sự kiện nhấn chuột
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y); // Lấy vị trí chuột

                    if (x >= resetButton.x && x <= resetButton.x + resetButton.w &&
                        y >= resetButton.y && y <= resetButton.y + resetButton.h)
                    {
                        TTF_CloseFont(largeFont); // Đóng font
                        return true;              // Trả về true để reset
                    }
                }
            }
        }
    }

    while (running) // Vòng lặp đếm ngược
    {
        while (SDL_PollEvent(&e) != 0) // Xử lý sự kiện
        {
            if (e.type == SDL_QUIT) // Kiểm tra sự kiện thoát
            {
                running = false;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) // Kiểm tra sự kiện nhấn chuột
            {
                int x, y;
                SDL_GetMouseState(&x, &y); // Lấy vị trí chuột

                if (x >= pauseButton.x && x <= pauseButton.x + pauseButton.w &&
                    y >= pauseButton.y && y <= pauseButton.y + pauseButton.h && lastRemainingTime > 0)
                {
                    paused = !paused; // Đổi trạng thái tạm dừng
                    if (paused)
                    {
                        pauseStartTime = SDL_GetTicks(); // Lấy thời gian bắt đầu tạm dừng
                    }
                    else
                    {
                        elapsedPausedTime += SDL_GetTicks() - pauseStartTime; // Cập nhật tổng thời gian đã tạm dừng
                    }
                }

                if (x >= resetButton.x && x <= resetButton.x + resetButton.w &&
                    y >= resetButton.y && y <= resetButton.y + resetButton.h)
                {
                    TTF_CloseFont(largeFont); // Đóng font
                    return true;              // Trả về true để reset
                }
            }
        }

        if (!paused) // Nếu không tạm dừng
        {
            Uint32 currentTime = SDL_GetTicks();                                       // Lấy thời gian hiện tại
            int elapsedSeconds = (currentTime - startTime - elapsedPausedTime) / 1000; // Tính thời gian đã trôi qua
            int remainingTime = countdownTime - elapsedSeconds;                        // Tính thời gian còn lại

            if (remainingTime < 0)
            {
                remainingTime = 0; // Đảm bảo thời gian còn lại không âm
            }

            if (remainingTime != lastRemainingTime) // Nếu thời gian còn lại thay đổi
            {
                lastRemainingTime = remainingTime; // Cập nhật thời gian còn lại lần cuối

                string timeText = (remainingTime > 0) ? formatTime(remainingTime) : "TIME OUT"; // Định dạng thời gian

                if (remainingTime == 0)
                {
                    Mix_PlayChannel(-1, alarm, 0); // Phát âm thanh báo động khi hết thời gian
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Đặt màu vẽ là màu đen
                SDL_RenderClear(renderer);                      // Xóa renderer với màu đã thiết lập

                drawProgressBar(renderer, countdownTime, remainingTime); // Vẽ thanh tiến trình

                SDL_Surface *textSurface = TTF_RenderText_Solid(largeFont, timeText.c_str(), textColor); // Tạo surface chứa chữ
                SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface);          // Tạo texture từ surface
                SDL_FreeSurface(textSurface);                                                            // Giải phóng surface

                int textWidth, textHeight;
                SDL_QueryTexture(timeTexture, NULL, NULL, &textWidth, &textHeight);                           // Lấy kích thước của texture
                SDL_Rect renderQuad = {(600 - textWidth) / 2, (400 - textHeight) / 2, textWidth, textHeight}; // Đặt vị trí và kích thước của chữ

                SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad); // Vẽ chữ lên màn hình
                SDL_DestroyTexture(timeTexture);                          // Giải phóng texture

                if (remainingTime > 0)
                {
                    drawButton(renderer, paused ? "Resume" : "Pause", pauseButton); // Vẽ nút Pause hoặc Resume
                    drawButton(renderer, "Reset", resetButton);                     // Vẽ nút Reset
                }
                else
                {
                    resetButton.x = (600 - resetButton.w) / 2;  // Đặt lại vị trí nút Reset
                    drawButton(renderer, "Reset", resetButton); // Vẽ nút Reset
                }

                SDL_RenderPresent(renderer); // Hiển thị renderer lên cửa sổ
            }
        }
        else // Nếu tạm dừng
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Đặt màu vẽ là màu đen
            SDL_RenderClear(renderer);                      // Xóa renderer với màu đã thiết lập

            SDL_Surface *pausedSurface = TTF_RenderText_Solid(font, "Paused", pausedTextColor); // Tạo surface chứa chữ "Paused"
            SDL_Texture *pausedTexture = SDL_CreateTextureFromSurface(renderer, pausedSurface); // Tạo texture từ surface
            SDL_FreeSurface(pausedSurface);                                                     // Giải phóng surface

            int pausedTextWidth, pausedTextHeight;
            SDL_QueryTexture(pausedTexture, NULL, NULL, &pausedTextWidth, &pausedTextHeight);                                     // Lấy kích thước của texture
            SDL_Rect pausedRect = {(600 - pausedTextWidth) / 2, (400 - pausedTextHeight) / 4, pausedTextWidth, pausedTextHeight}; // Đặt vị trí và kích thước của chữ "Paused"
            SDL_RenderCopy(renderer, pausedTexture, NULL, &pausedRect);                                                           // Vẽ chữ "Paused" lên màn hình
            SDL_DestroyTexture(pausedTexture);                                                                                    // Giải phóng texture

            string timeText = formatTime(lastRemainingTime);                                         // Định dạng thời gian còn lại
            SDL_Surface *textSurface = TTF_RenderText_Solid(largeFont, timeText.c_str(), textColor); // Tạo surface chứa chữ
            SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface);          // Tạo texture từ surface
            SDL_FreeSurface(textSurface);                                                            // Giải phóng surface

            int textWidth, textHeight;
            SDL_QueryTexture(timeTexture, NULL, NULL, &textWidth, &textHeight);                           // Lấy kích thước của texture
            SDL_Rect renderQuad = {(600 - textWidth) / 2, (400 - textHeight) / 2, textWidth, textHeight}; // Đặt vị trí và kích thước của chữ

            SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad); // Vẽ chữ lên màn hình
            SDL_DestroyTexture(timeTexture);                          // Giải phóng texture

            drawButton(renderer, "Resume", pauseButton); // Vẽ nút Resume
            drawButton(renderer, "Reset", resetButton);  // Vẽ nút Reset

            SDL_RenderPresent(renderer); // Hiển thị renderer lên cửa sổ
        }

        SDL_Delay(1000 / 60); // Giảm tải CPU bằng cách chờ 1/60 giây
    }

    TTF_CloseFont(largeFont); // Đóng font
    return false;             // Trả về false nếu không cần reset
}