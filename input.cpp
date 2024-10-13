#include "common.h"
// Hàm nhập thời gian
int inputTime(SDL_Renderer *renderer, TTF_Font *font)
{
    // Thiết lập màu nền cho renderer
    clearRenderer(renderer, BACKGROUND_COLOR);
    // Hiển thị renderer lên cửa sổ
    SDL_RenderPresent(renderer);

    // Màu xanh lá cho đồng hồ (đã khai báo trong common.h)
    SDL_Color textColor = TIME_COLOR;
    // Chuỗi ký tự ban đầu cho thời gian
    string inputText = "000000";
    // Biến kiểm soát vòng lặp nhập liệu
    bool done = false;
    // Biến sự kiện SDL
    SDL_Event e;

    // Nút Start và Clear nằm ngay dưới đồng hồ
    SDL_Rect startButton = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 10, 80, 40};
    SDL_Rect clearButton = {WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT / 2 - 10, 80, 40};

    // Nút số từ 0 đến 4 ở hàng trên, từ 5 đến 9 ở hàng dưới
    vector<SDL_Rect> digitButtons(10);
    for (int i = 0; i < 5; ++i) // Hàng trên: 0-4
    {
        digitButtons[i] = {WINDOW_WIDTH / 2 - 240 + i * 100, WINDOW_HEIGHT / 2 + 50, 80, 40};
    }
    for (int i = 5; i < 10; ++i) // Hàng dưới: 5-9
    {
        digitButtons[i] = {WINDOW_WIDTH / 2 - 240 + (i - 5) * 100, WINDOW_HEIGHT / 2 + 110, 80, 40};
    }

    // Mở font chữ lớn với kích thước 100
    TTF_Font *largeFont = TTF_OpenFont("data/digital.ttf", 100);
    if (!largeFont)
    {
        // Hiển thị lỗi nếu không thể tải font
        cerr << "Không thể tải font! TTF Error: " << TTF_GetError() << endl;
        return 0;
    }

    // Vòng lặp nhập liệu
    while (!done)
    {
        // Xử lý sự kiện
        while (SDL_PollEvent(&e) != 0)
        {
            // Kiểm tra sự kiện thoát
            if (e.type == SDL_QUIT)
            {
                done = true;
                exit(0);
            }
            // Kiểm tra sự kiện nhấn chuột
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                // Lấy vị trí chuột
                SDL_GetMouseState(&x, &y);

                // Kiểm tra nhấn nút số
                for (int i = 0; i < 10; ++i)
                {
                    if (checkClickButton(x, y, digitButtons[i]))
                    {
                        // Cập nhật chuỗi thời gian
                        inputText = inputText.substr(1) + to_string(i);
                    }
                }

                // Kiểm tra nút Start
                if (checkClickButton(x, y, startButton))
                {
                    // Kết thúc nhập liệu
                    done = true;
                }

                // Kiểm tra nút Clear
                if (checkClickButton(x, y, clearButton))
                {
                    // Đặt lại chuỗi thời gian
                    inputText = "000000";
                }
            }
        }

        // Thiết lập màu nền cho renderer
        clearRenderer(renderer, BACKGROUND_COLOR);

        // Định dạng thời gian
        string formattedTime = inputText.substr(0, 2) + ":" + inputText.substr(2, 2) + ":" + inputText.substr(4, 2);
        // Vẽ thời gian lên màn hình
        drawText(renderer, largeFont, formattedTime.c_str(), TIME_COLOR, 0, -80);

        // Vẽ nút Start và Clear
        drawButton(renderer, "Start", startButton);
        drawButton(renderer, "Clear", clearButton);

        // Vẽ các nút số
        for (int i = 0; i < 10; ++i)
        {
            drawButton(renderer, to_string(i), digitButtons[i]);
        }

        // Hiển thị renderer lên cửa sổ
        SDL_RenderPresent(renderer);
    }

    // Đóng font
    TTF_CloseFont(largeFont);

    // Chuyển đổi input thành thời gian
    int hours = stoi(inputText.substr(0, 2));
    int minutes = stoi(inputText.substr(2, 2));
    int seconds = stoi(inputText.substr(4, 2));

    // Trả về thời gian dưới dạng số giây
    return hours * 3600 + minutes * 60 + seconds;
}