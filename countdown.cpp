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
bool runCountdown(SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm, int &countdownTime)
{
    // Biến kiểm soát vòng lặp đếm ngược
    bool running = true;
    // Biến kiểm soát trạng thái tạm dừng
    bool paused = false;
    // Biến sự kiện SDL
    SDL_Event e;
    // Lấy thời gian bắt đầu
    Uint32 startTime = SDL_GetTicks();
    // Thời gian bắt đầu tạm dừng
    Uint32 pauseStartTime = 0;
    // Thời gian còn lại lần cuối
    int lastRemainingTime = -1;
    // Tổng thời gian đã tạm dừng
    int elapsedPausedTime = 0;

    // Nút Pause và Reset nằm ở dưới cùng của màn hình
    SDL_Rect pauseButton = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 50, 80, 40};
    SDL_Rect resetButton = {WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT / 2 + 50, 80, 40};

    // Mở font chữ lớn với kích thước 100
    TTF_Font *largeFont = TTF_OpenFont("data/digital.ttf", 100);
    if (!largeFont)
    {
        cerr << "Không thể tải font! TTF Error: " << TTF_GetError() << endl;
        return false;
    }

    // Kiểm tra nếu thời gian đếm ngược là 0
    if (countdownTime == 0)
    {
        // Thiết lập màu nền cho renderer
        clearRenderer(renderer, BACKGROUND_COLOR);

        // Chuỗi ký tự hiển thị khi hết thời gian
        string timeText = "TIME OUT";
        // Phát âm thanh báo động
        Mix_PlayChannel(-1, alarm, 0);

        // Vẽ thời gian lên màn hình
        drawText(renderer, largeFont, timeText.c_str(), TIME_COLOR, 0, -80);

        // Đặt lại vị trí nút Reset
        resetButton.x = (WINDOW_WIDTH - resetButton.w) / 2;
        // Vẽ nút Reset
        drawButton(renderer, "Reset", resetButton);

        // Hiển thị renderer lên cửa sổ
        SDL_RenderPresent(renderer);

        // Biến kiểm soát vòng lặp reset
        bool reset = false;
        while (!reset)
        {
            // Xử lý sự kiện
            while (SDL_PollEvent(&e) != 0)
            {
                // Kiểm tra sự kiện thoát
                if (e.type == SDL_QUIT)
                {
                    running = false;
                    reset = true;
                }
                // Kiểm tra sự kiện nhấn chuột
                else if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    int x, y;
                    // Lấy vị trí chuột
                    SDL_GetMouseState(&x, &y);

                    // Sử dụng hàm kiểm tra nhấn nút
                    if (checkClickButton(x, y, resetButton))
                    {
                        // Đóng font
                        TTF_CloseFont(largeFont);
                        // Trả về true để reset
                        return true;
                    }
                }
            }
        }
    }

    // Vòng lặp đếm ngược
    while (running)
    {
        // Xử lý sự kiện
        while (SDL_PollEvent(&e) != 0)
        {
            // Kiểm tra sự kiện thoát
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            // Kiểm tra sự kiện nhấn chuột
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                // Lấy vị trí chuột
                SDL_GetMouseState(&x, &y);

                // Sử dụng hàm kiểm tra nhấn nút
                if (checkClickButton(x, y, pauseButton) && lastRemainingTime > 0)
                {
                    // Đổi trạng thái tạm dừng
                    paused = !paused;
                    if (paused)
                    {
                        // Lấy thời gian bắt đầu tạm dừng
                        pauseStartTime = SDL_GetTicks();
                    }
                    else
                    {
                        // Cập nhật tổng thời gian đã tạm dừng
                        elapsedPausedTime += SDL_GetTicks() - pauseStartTime;
                    }
                }

                if (checkClickButton(x, y, resetButton))
                {
                    // Đóng font
                    TTF_CloseFont(largeFont);
                    // Trả về true để reset
                    return true;
                }
            }
        }

        // Nếu không tạm dừng
        if (!paused)
        {
            // Lấy thời gian hiện tại
            Uint32 currentTime = SDL_GetTicks();
            // Tính thời gian đã trôi qua
            int elapsedSeconds = (currentTime - startTime - elapsedPausedTime) / 1000;
            // Tính thời gian còn lại
            int remainingTime = countdownTime - elapsedSeconds;

            // Đảm bảo thời gian còn lại không âm
            if (remainingTime < 0)
            {
                remainingTime = 0;
            }

            // Nếu thời gian còn lại thay đổi
            if (remainingTime != lastRemainingTime)
            {
                // Cập nhật thời gian còn lại lần cuối
                lastRemainingTime = remainingTime;

                // Định dạng thời gian
                string timeText = (remainingTime > 0) ? formatTime(remainingTime) : "TIME OUT";

                if (remainingTime == 0)
                {
                    // Phát âm thanh báo động khi hết thời gian
                    Mix_PlayChannel(-1, alarm, 0);
                }

                // Thiết lập màu nền cho renderer
                clearRenderer(renderer, BACKGROUND_COLOR);

                // Vẽ thanh tiến trình
                drawProgressBar(renderer, countdownTime, remainingTime);

                // Vẽ thời gian lên màn hình
                drawText(renderer, largeFont, timeText.c_str(), TIME_COLOR, 0, -80);

                if (remainingTime > 0)
                {
                    // Vẽ nút Pause hoặc Resume
                    drawButton(renderer, paused ? "Resume" : "Pause", pauseButton);
                    // Vẽ nút Reset
                    drawButton(renderer, "Reset", resetButton);
                }
                else
                {
                    // Đặt lại vị trí nút Reset
                    resetButton.x = (WINDOW_WIDTH - resetButton.w) / 2;
                    // Vẽ nút Reset
                    drawButton(renderer, "Reset", resetButton);
                }

                // Hiển thị renderer lên cửa sổ
                SDL_RenderPresent(renderer);
            }
        }
        else // Nếu tạm dừng
        {
            // Thiết lập màu nền cho renderer
            clearRenderer(renderer, BACKGROUND_COLOR);

            drawText(renderer, font, "Paused", PAUSE_TEXT_COLOR, 0, -150);

            // Định dạng thời gian còn lại
            string timeText = formatTime(lastRemainingTime);
            // Vẽ thời gian lên màn hình
            drawText(renderer, largeFont, timeText.c_str(), TIME_COLOR, 0, -80);

            // Vẽ nút Resume
            drawButton(renderer, "Resume", pauseButton);
            // Vẽ nút Reset
            drawButton(renderer, "Reset", resetButton);

            // Hiển thị renderer lên cửa sổ
            SDL_RenderPresent(renderer);
        }

        // Giảm tải CPU bằng cách chờ 1/60 giây
        SDL_Delay(1000 / 60);
    }

    // Đóng font
    TTF_CloseFont(largeFont);
    // Trả về false nếu không cần reset
    return false;
}