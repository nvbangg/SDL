#include "make/input.h"
#include "make/drawing.h"
#include <vector>
#include <iostream>
using namespace std;

int inputTime(SDL_Renderer *renderer, TTF_Font *font)
{
    // Thiết lập màu vẽ cho renderer (màu đen, không trong suốt)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // Xóa renderer với màu đã thiết lập (tô toàn bộ renderer bằng màu đen)
    SDL_RenderClear(renderer);
    // Hiển thị renderer lên cửa sổ
    SDL_RenderPresent(renderer);

    SDL_Color textColor = {0, 255, 0}; // Màu xanh lá cho đồng hồ
    string inputText = "000000";       // Chuỗi ký tự ban đầu cho thời gian
    bool done = false;                 // Biến kiểm soát vòng lặp nhập liệu
    SDL_Event e;                       // Biến sự kiện SDL

    // Nút Start và Clear nằm ngay dưới đồng hồ
    SDL_Rect startButton = {600 / 2 - 100, 400 / 2 - 10, 80, 40};
    SDL_Rect clearButton = {600 / 2 + 20, 400 / 2 - 10, 80, 40};

    // Nút số từ 0 đến 4 ở hàng trên, từ 5 đến 9 ở hàng dưới
    vector<SDL_Rect> digitButtons(10);
    for (int i = 0; i < 5; ++i) // Hàng trên: 0-4
    {
        digitButtons[i] = {50 + i * 100, 400 / 2 + 50, 80, 40};
    }
    for (int i = 5; i < 10; ++i) // Hàng dưới: 5-9
    {
        digitButtons[i] = {50 + (i - 5) * 100, 400 / 2 + 110, 80, 40};
    }

    TTF_Font *largeFont = TTF_OpenFont("data/digital.ttf", 100); // Mở font chữ lớn với kích thước 100
    if (!largeFont)
    {
        cerr << "Không thể tải font! TTF Error: " << TTF_GetError() << endl; // Hiển thị lỗi nếu không thể tải font
        return 0;
    }

    while (!done) // Vòng lặp nhập liệu
    {
        while (SDL_PollEvent(&e) != 0) // Xử lý sự kiện
        {
            if (e.type == SDL_QUIT) // Kiểm tra sự kiện thoát
            {
                done = true;
                exit(0);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) // Kiểm tra sự kiện nhấn chuột
            {
                int x, y;
                SDL_GetMouseState(&x, &y); // Lấy vị trí chuột

                // Kiểm tra nhấn nút số
                for (int i = 0; i < 10; ++i)
                {
                    if (x >= digitButtons[i].x && x <= digitButtons[i].x + digitButtons[i].w &&
                        y >= digitButtons[i].y && y <= digitButtons[i].y + digitButtons[i].h)
                    {
                        inputText = inputText.substr(1) + to_string(i); // Cập nhật chuỗi thời gian
                    }
                }

                // Kiểm tra nút Start
                if (x >= startButton.x && x <= startButton.x + startButton.w &&
                    y >= startButton.y && y <= startButton.y + startButton.h)
                {
                    done = true; // Kết thúc nhập liệu
                }

                // Kiểm tra nút Clear
                if (x >= clearButton.x && x <= clearButton.x + clearButton.w &&
                    y >= clearButton.y && y <= clearButton.y + clearButton.h)
                {
                    inputText = "000000"; // Đặt lại chuỗi thời gian
                }
            }
        }

        // Xóa màn hình và hiển thị đồng hồ lớn màu xanh lá
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Đặt màu vẽ là màu đen
        SDL_RenderClear(renderer); // Xóa màn hình với màu vẽ hiện tại

        string formattedTime = inputText.substr(0, 2) + ":" + inputText.substr(2, 2) + ":" + inputText.substr(4, 2); // Định dạng thời gian

        //Tạo texture chứa chữ xanh lá với font
        SDL_Surface *textSurface = TTF_RenderText_Solid(largeFont, formattedTime.c_str(), textColor); // Tạo surface chứa chữ
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);               // Tạo texture từ surface

        int textWidth, textHeight;
        // Lấy kích thước của texture
        SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
        // Đặt vị trí và kích thước của chữ
        SDL_Rect renderQuad = {(600 - textWidth) / 2, (400 - textHeight) / 2 - 80, textWidth, textHeight}; 

        SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad); // Vẽ chữ lên màn hình
        SDL_FreeSurface(textSurface);                             // Giải phóng surface
        SDL_DestroyTexture(textTexture);                          // Giải phóng texture

        // Vẽ nút Start và Clear
        drawButton(renderer, "Start", startButton);
        drawButton(renderer, "Clear", clearButton);

        // Vẽ các nút số
        for (int i = 0; i < 10; ++i)
        {
            drawButton(renderer, to_string(i), digitButtons[i]);
        }

        SDL_RenderPresent(renderer); // Hiển thị renderer lên cửa sổ
    }

    TTF_CloseFont(largeFont); // Đóng font

    // Chuyển đổi input thành thời gian
    int hours = stoi(inputText.substr(0, 2));
    int minutes = stoi(inputText.substr(2, 2));
    int seconds = stoi(inputText.substr(4, 2));

    return hours * 3600 + minutes * 60 + seconds; // Trả về thời gian dưới dạng số giây
}