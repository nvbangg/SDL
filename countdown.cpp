#include "common.h"

// Hàm định dạng thời gian từ giây thành chuỗi "HH:MM:SS"
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

// Hàm thực hiện việc đếm ngược
bool runCountdown(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm, int &countdownTime)
{
    bool running = true, paused = false, alarmPlayed = false;
    SDL_Event e;
    Uint32 startTime = SDL_GetTicks(), pauseStartTime = 0, elapsedPausedTime = 0;
    int lastRemainingTime = -1;
    // Mở font chữ với kích thước 50
    TTF_Font *textFont = TTF_OpenFont("data/digital.ttf", 50);
    while (running)
    {
        SDL_Rect pauseButton = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 50, 80, 40};
        SDL_Rect resetButton = {WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT / 2 + 50, 80, 40};

        // Xử lý sự kiện
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                return false;
            if (e.type == SDL_WINDOWEVENT)
                handleWindowEvent(e, window);
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Xử lý nhấn nút "Pause" hoặc "Repeat"
                if (checkClickButton(x, y, pauseButton))
                {
                    if (lastRemainingTime > 0)
                    {
                        paused = !paused;
                        if (paused)
                            pauseStartTime = SDL_GetTicks();
                        else
                            elapsedPausedTime += SDL_GetTicks() - pauseStartTime;
                    }
                    else if (countdownTime > 0) // Kiểm tra nếu countdownTime > 0
                    {
                        // Khi nhấn nút "Repeat", khởi động lại countdown
                        startTime = SDL_GetTicks();
                        elapsedPausedTime = 0;
                        paused = false;
                        alarmPlayed = false;
                    }
                }

                // Xử lý nhấn nút "Reset"
                if (checkClickButton(x, y, resetButton))
                    return true; // Reset lại countdown
            }
        }

        // Nếu không tạm dừng, tính toán thời gian còn lại
        if (!paused)
        {
            int remainingTime = countdownTime - (SDL_GetTicks() - startTime - elapsedPausedTime) / 1000;
            if (remainingTime < 0)
                remainingTime = 0;

            lastRemainingTime = remainingTime;
            clearRenderer(renderer, BACKGROUND_COLOR);

            // Vẽ thời gian còn lại hoặc hiển thị "TIME OUT"
            string timeText = (remainingTime > 0) ? formatTime(remainingTime) : "TIME OUT";
            drawText(renderer, font, timeText.c_str(), TIME_COLOR, 0, -80);

            // Vẽ thanh tiến trình
            drawProgressBar(renderer, countdownTime, remainingTime);

            // Vẽ các nút
            if (remainingTime > 0)
            {
                drawButton(renderer, paused ? "Resume" : "Pause", pauseButton);
            }
            else
            {
                // Khi hết thời gian, đổi nút "Pause" thành "Repeat"
                drawButton(renderer, "Repeat", pauseButton);
                if (!alarmPlayed)
                {
                    Mix_PlayChannel(-1, alarm, 0); // Phát âm thanh báo động khi hết thời gian
                    alarmPlayed = true;            // Đánh dấu đã phát âm thanh
                }
            }
            drawButton(renderer, "Reset", resetButton);

            SDL_RenderPresent(renderer);
        }
        else // Nếu đang tạm dừng, hiển thị trạng thái Paused
        {
            clearRenderer(renderer, BACKGROUND_COLOR);
            drawText(renderer, textFont, "Paused", PAUSE_TEXT_COLOR, 0, -150);
            drawText(renderer, font, formatTime(lastRemainingTime).c_str(), TIME_COLOR, 0, -80);
            drawButton(renderer, "Resume", pauseButton);
            drawButton(renderer, "Reset", resetButton);
            SDL_RenderPresent(renderer);
        }

        SDL_Delay(1000 / 60); // Giảm tải CPU
    }
    TTF_CloseFont(textFont);
    return false;
}
