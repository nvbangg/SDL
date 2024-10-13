#include "common.h"

// Thiết lập màu nền cho renderer
void clearRenderer(SDL_Renderer *renderer, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

//Check click button
bool checkClickButton(int x, int y, const SDL_Rect &button)
{
    return (x >= button.x && x <= button.x + button.w &&
            y >= button.y && y <= button.y + button.h);
}

// Vẽ chữ lên màn hình
void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int xOffset, int yOffset)
{
    // Tạo surface chứa chữ
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    // Tạo texture từ surface
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    // Giải phóng surface
    SDL_FreeSurface(textSurface);

    // Lấy kích thước của texture
    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

    // Tính toán vị trí chính giữa màn hình cộng thêm xOffset và yOffset
    int x = (WINDOW_WIDTH - textWidth) / 2 + xOffset;
    int y = (WINDOW_HEIGHT - textHeight) / 2 + yOffset;

    // Đặt vị trí và kích thước của chữ
    SDL_Rect renderQuad = {x, y, textWidth, textHeight};

    // Vẽ chữ lên màn hình
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    // Giải phóng texture
    SDL_DestroyTexture(textTexture);
}

// Hàm vẽ nút bấm
void drawButton(SDL_Renderer *renderer, const string &label, SDL_Rect &buttonRect)
{
    // Đặt màu vẽ
    SDL_SetRenderDrawColor(renderer, BUTTON_COLOR.r, BUTTON_COLOR.g, BUTTON_COLOR.b, BUTTON_COLOR.a);
    // Vẽ hình chữ nhật của nút bấm
    SDL_RenderFillRect(renderer, &buttonRect);

    // Mở font chữ với kích thước 24
    TTF_Font *buttonFont = TTF_OpenFont("data/digital.ttf", 24);
    if (!buttonFont)
    {
        // Hiển thị lỗi nếu không thể tải font
        cerr << "Không thể tải font! TTF Error: " << TTF_GetError() << endl;
        return;
    }

    // Đặt màu chữ
    SDL_Color textColor = TEXT_COLOR;
    // Tạo surface chứa chữ
    SDL_Surface *textSurface = TTF_RenderText_Solid(buttonFont, label.c_str(), textColor);
    // Tạo texture từ surface
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textWidth, textHeight;
    // Lấy kích thước của texture
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    // Đặt vị trí và kích thước của chữ
    SDL_Rect textRect = {buttonRect.x + (buttonRect.w - textWidth) / 2, buttonRect.y + (buttonRect.h - textHeight) / 2, textWidth, textHeight};

    // Vẽ chữ lên nút bấm
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Giải phóng surface
    SDL_FreeSurface(textSurface);
    // Giải phóng texture
    SDL_DestroyTexture(textTexture);
    // Đóng font
    TTF_CloseFont(buttonFont);
}

void drawTime(SDL_Renderer *renderer, TTF_Font *font, const string &timeText)
{
    // Đặt màu chữ
    SDL_Color textColor = TIME_COLOR;

    // Tính toán kích thước chữ
    int textWidth, textHeight;
    TTF_SizeText(font, timeText.c_str(), &textWidth, &textHeight);

    // Tính toán vị trí để chữ nằm giữa màn hình
    int x = (WINDOW_WIDTH - textWidth) / 2;
    int y = (WINDOW_HEIGHT - textHeight) / 2 - 80;

    // Gọi hàm drawText để vẽ thời gian
    drawText(renderer, font, timeText.c_str(), textColor, x, y);
}

// Hàm vẽ thanh tiến trình
void drawProgressBar(SDL_Renderer *renderer, int totalTime, int remainingTime)
{
    // Đặt chiều rộng của thanh tiến trình
    int barWidth = WINDOW_WIDTH - 100;
    // Đặt chiều cao của thanh tiến trình
    int barHeight = 20;
    // Tính chiều rộng của phần đã hoàn thành
    int filledWidth = (remainingTime * barWidth) / totalTime;

    // Đặt vị trí và kích thước của thanh tiến trình
    SDL_Rect progressBar = {50, 20, barWidth, barHeight};
    // Đặt vị trí và kích thước của phần đã hoàn thành
    SDL_Rect filledBar = {50, 20, filledWidth, barHeight};

    // Đặt màu vẽ
    SDL_SetRenderDrawColor(renderer, BUTTON_COLOR.r, BUTTON_COLOR.g, BUTTON_COLOR.b, BUTTON_COLOR.a);
    // Vẽ thanh tiến trình
    SDL_RenderFillRect(renderer, &progressBar);

    // Đặt màu vẽ là màu xanh lá cây
    SDL_SetRenderDrawColor(renderer, TIME_COLOR.r, TIME_COLOR.g, TIME_COLOR.b, TIME_COLOR.a);
    // Vẽ phần đã hoàn thành của thanh tiến trình
    SDL_RenderFillRect(renderer, &filledBar);
}