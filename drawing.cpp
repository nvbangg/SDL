#include "make/drawing.h"
#include <SDL_ttf.h>
#include <iostream>
using namespace std;

void drawButton(SDL_Renderer *renderer, const string &label, SDL_Rect &buttonRect)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Đặt màu vẽ là màu trắng
    SDL_RenderFillRect(renderer, &buttonRect);            // Vẽ hình chữ nhật của nút bấm

    TTF_Font *buttonFont = TTF_OpenFont("data/digital.ttf", 24); // Mở font chữ với kích thước 24
    if (!buttonFont)
    {
        cerr << "Không thể tải font! TTF Error: " << TTF_GetError() << endl; // Hiển thị lỗi nếu không thể tải font
        return;
    }
    SDL_Color textColor = {0, 0, 0};                                                       // Đặt màu chữ là màu đen
    SDL_Surface *textSurface = TTF_RenderText_Solid(buttonFont, label.c_str(), textColor); // Tạo surface chứa chữ
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);        // Tạo texture từ surface

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);                                                                         // Lấy kích thước của texture
    SDL_Rect textRect = {buttonRect.x + (buttonRect.w - textWidth) / 2, buttonRect.y + (buttonRect.h - textHeight) / 2, textWidth, textHeight}; // Đặt vị trí và kích thước của chữ

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect); // Vẽ chữ lên nút bấm

    SDL_FreeSurface(textSurface);    // Giải phóng surface
    SDL_DestroyTexture(textTexture); // Giải phóng texture
    TTF_CloseFont(buttonFont);       // Đóng font
}

void drawTime(SDL_Renderer *renderer, TTF_Font *font, const string &timeText)
{
    SDL_Color textColor = {255, 255, 255};                                              // Đặt màu chữ là màu trắng
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, timeText.c_str(), textColor); // Tạo surface chứa chữ
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);     // Tạo texture từ surface

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);                           // Lấy kích thước của texture
    SDL_Rect renderQuad = {(600 - textWidth) / 2, (400 - textHeight) / 2, textWidth, textHeight}; // Đặt vị trí và kích thước của chữ

    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad); // Vẽ chữ lên màn hình
    SDL_FreeSurface(textSurface);                             // Giải phóng surface
    SDL_DestroyTexture(textTexture);                          // Giải phóng texture
}

void drawProgressBar(SDL_Renderer *renderer, int totalTime, int remainingTime)
{
    int barWidth = 600 - 100;                                 // Đặt chiều rộng của thanh tiến trình
    int barHeight = 20;                                       // Đặt chiều cao của thanh tiến trình
    int filledWidth = (remainingTime * barWidth) / totalTime; // Tính chiều rộng của phần đã hoàn thành

    SDL_Rect progressBar = {50, 20, barWidth, barHeight};  // Đặt vị trí và kích thước của thanh tiến trình
    SDL_Rect filledBar = {50, 20, filledWidth, barHeight}; // Đặt vị trí và kích thước của phần đã hoàn thành

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Đặt màu vẽ là màu trắng
    SDL_RenderFillRect(renderer, &progressBar);           // Vẽ thanh tiến trình

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Đặt màu vẽ là màu xanh lá cây
    SDL_RenderFillRect(renderer, &filledBar);         // Vẽ phần đã hoàn thành của thanh tiến trình
}