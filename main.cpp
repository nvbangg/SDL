#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
using namespace std;

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 400;

void handleError(const string &message, SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm)
{
    cerr << message << " SDL Error: " << SDL_GetError() << endl;
    if (font)
        TTF_CloseFont(font);
    if (alarm)
        Mix_FreeChunk(alarm);
    Mix_CloseAudio();
    TTF_Quit();
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
    exit(-1);
}

void initSDL(SDL_Window *&window, SDL_Renderer *&renderer, TTF_Font *&font, Mix_Chunk *&alarm)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        handleError("SDL không thể khởi tạo!", window, renderer, font, alarm);

    window = SDL_CreateWindow("Đồng hồ điện tử đếm ngược", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
        handleError("Không thể tạo cửa sổ!", window, renderer, font, alarm);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        handleError("Không thể tạo renderer!", window, renderer, font, alarm);

    if (TTF_Init() == -1)
        handleError("TTF không thể khởi tạo!", window, renderer, font, alarm);

    font = TTF_OpenFont("data/digital.ttf", 50);
    if (!font)
        handleError("Không thể tải font!", window, renderer, font, alarm);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        handleError("SDL_mixer không thể khởi tạo!", window, renderer, font, alarm);

    alarm = Mix_LoadWAV("data/alarm.mp3");
    if (!alarm)
        handleError("Không thể tải âm thanh!", window, renderer, font, alarm);

    SDL_StartTextInput();
}

void closeSDL(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm)
{
    if (font)
        TTF_CloseFont(font);
    if (alarm)
        Mix_FreeChunk(alarm);
    Mix_CloseAudio();
    TTF_Quit();
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

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

int inputTime(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Color textColor = {255, 255, 255};
    auto inputText = [&](const string &prompt, int yPosition) -> string
    {
        string inputText = "";
        bool done = false;
        SDL_Event e;
        bool inputChanged = true;

        while (!done)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    done = true;
                    exit(0);
                }
                else if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_RETURN)
                    {
                        done = true;
                    }
                    else if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
                    {
                        inputText.pop_back();
                        inputChanged = true;
                    }
                }
                else if (e.type == SDL_TEXTINPUT)
                {
                    if (isdigit(e.text.text[0]))
                    {
                        inputText += e.text.text;
                        inputChanged = true;
                    }
                }
            }

            if (inputChanged)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_Rect clearRect = {50, yPosition, WINDOW_WIDTH - 100, 60};
                SDL_RenderFillRect(renderer, &clearRect);

                SDL_Surface *promptSurface = TTF_RenderText_Solid(font, prompt.c_str(), textColor);
                SDL_Texture *promptTexture = SDL_CreateTextureFromSurface(renderer, promptSurface);
                int promptWidth, promptHeight;
                SDL_QueryTexture(promptTexture, NULL, NULL, &promptWidth, &promptHeight);
                SDL_Rect promptRect = {50, yPosition, promptWidth, promptHeight};
                SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);
                SDL_FreeSurface(promptSurface);
                SDL_DestroyTexture(promptTexture);

                SDL_Surface *textSurface = TTF_RenderText_Solid(font, inputText.c_str(), textColor);
                SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int textWidth, textHeight;
                SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
                SDL_Rect textRect = {50 + promptWidth + 10, yPosition, textWidth, textHeight};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);

                SDL_RenderPresent(renderer);
                inputChanged = false;
            }
        }

        return inputText;
    };

    int hours = 0, minutes = 0, seconds = 0;
    bool validInput = false;
    while (!validInput)
    {
        try
        {
            hours = stoi(inputText("Enter hours:", 70));
            minutes = stoi(inputText("Enter minutes:", 170));
            seconds = stoi(inputText("Enter seconds:", 270));
            validInput = true;
        }
        catch (const invalid_argument &)
        {
            cerr << "Invalid input. Please enter numeric values." << endl;
        }
    }

    int totalSeconds = hours * 3600 + minutes * 60 + seconds;
    return totalSeconds;
}

void drawButton(SDL_Renderer *renderer, const string &label, SDL_Rect &buttonRect)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    TTF_Font *buttonFont = TTF_OpenFont("data/digital.ttf", 24);
    if (!buttonFont)
    {
        cerr << "Không thể tải font! TTF Error: " << TTF_GetError() << endl;
        return;
    }
    SDL_Color textColor = {0, 0, 0};
    SDL_Surface *textSurface = TTF_RenderText_Solid(buttonFont, label.c_str(), textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    SDL_Rect textRect = {buttonRect.x + (buttonRect.w - textWidth) / 2, buttonRect.y + (buttonRect.h - textHeight) / 2, textWidth, textHeight};

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(buttonFont);
}

void drawProgressBar(SDL_Renderer *renderer, int totalTime, int remainingTime)
{
    int barWidth = WINDOW_WIDTH - 100;
    int barHeight = 20;
    int filledWidth = (remainingTime * barWidth) / totalTime;

    SDL_Rect progressBar = {50, 20, barWidth, barHeight};
    SDL_Rect filledBar = {50, 20, filledWidth, barHeight};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &progressBar);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &filledBar);
}

bool runCountdown(SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm, int &countdownTime)
{
    SDL_Color textColor = {0, 255, 0};
    SDL_Color pausedTextColor = {255, 0, 0};
    bool running = true;
    bool paused = false;
    SDL_Event e;
    Uint32 startTime = SDL_GetTicks();
    Uint32 pauseStartTime = 0;
    int lastRemainingTime = -1;
    int elapsedPausedTime = 0;

    SDL_Rect pauseButton = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 60, 80, 40};
    SDL_Rect resetButton = {WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT - 60, 80, 40};

    // Increase font size for countdown
    TTF_Font *largeFont = TTF_OpenFont("data/digital.ttf", 100);
    if (!largeFont)
    {
        cerr << "Không thể tải font! TTF Error: " << TTF_GetError() << endl;
        return false;
    }

    // Kiểm tra nếu thời gian đếm ngược là 0
    if (countdownTime == 0)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Hiển thị TIME OUT ngay lập tức
        string timeText = "TIME OUT";
        Mix_PlayChannel(-1, alarm, 0); // Phát âm thanh cảnh báo

        SDL_Surface *textSurface = TTF_RenderText_Solid(largeFont, timeText.c_str(), textColor);
        SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        int textWidth, textHeight;
        SDL_QueryTexture(timeTexture, NULL, NULL, &textWidth, &textHeight);
        SDL_Rect renderQuad = {(WINDOW_WIDTH - textWidth) / 2, (WINDOW_HEIGHT - textHeight) / 2, textWidth, textHeight};

        SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad);
        SDL_DestroyTexture(timeTexture);

        resetButton.x = (WINDOW_WIDTH - resetButton.w) / 2;
        drawButton(renderer, "Reset", resetButton);

        SDL_RenderPresent(renderer);

        // Đợi người dùng nhấn nút reset
        bool reset = false;
        while (!reset)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    running = false;
                    reset = true;
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    if (x >= resetButton.x && x <= resetButton.x + resetButton.w &&
                        y >= resetButton.y && y <= resetButton.y + resetButton.h)
                    {
                        TTF_CloseFont(largeFont);
                        return true; // Nhấn reset
                    }
                }
            }
        }
    }

    while (running)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (x >= pauseButton.x && x <= pauseButton.x + pauseButton.w &&
                    y >= pauseButton.y && y <= pauseButton.y + pauseButton.h && lastRemainingTime > 0)
                {
                    paused = !paused;
                    if (paused)
                    {
                        pauseStartTime = SDL_GetTicks();
                    }
                    else
                    {
                        elapsedPausedTime += SDL_GetTicks() - pauseStartTime;
                    }
                }

                if (x >= resetButton.x && x <= resetButton.x + resetButton.w &&
                    y >= resetButton.y && y <= resetButton.y + resetButton.h)
                {
                    TTF_CloseFont(largeFont);
                    return true;
                }
            }
        }

        if (!paused)
        {
            Uint32 currentTime = SDL_GetTicks();
            int elapsedSeconds = (currentTime - startTime - elapsedPausedTime) / 1000;
            int remainingTime = countdownTime - elapsedSeconds;

            if (remainingTime < 0)
            {
                remainingTime = 0;
            }

            if (remainingTime != lastRemainingTime)
            {
                lastRemainingTime = remainingTime;

                string timeText = (remainingTime > 0) ? formatTime(remainingTime) : "TIME OUT";

                if (remainingTime == 0)
                {
                    Mix_PlayChannel(-1, alarm, 0);
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                drawProgressBar(renderer, countdownTime, remainingTime);

                SDL_Surface *textSurface = TTF_RenderText_Solid(largeFont, timeText.c_str(), textColor);
                SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_FreeSurface(textSurface);

                int textWidth, textHeight;
                SDL_QueryTexture(timeTexture, NULL, NULL, &textWidth, &textHeight);
                SDL_Rect renderQuad = {(WINDOW_WIDTH - textWidth) / 2, (WINDOW_HEIGHT - textHeight) / 2, textWidth, textHeight};

                SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad);
                SDL_DestroyTexture(timeTexture);

                if (remainingTime > 0)
                {
                    drawButton(renderer, paused ? "Resume" : "Pause", pauseButton);
                    drawButton(renderer, "Reset", resetButton);
                }
                else
                {
                    resetButton.x = (WINDOW_WIDTH - resetButton.w) / 2;
                    drawButton(renderer, "Reset", resetButton);
                }

                SDL_RenderPresent(renderer);
            }
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_Surface *pausedSurface = TTF_RenderText_Solid(font, "Paused", pausedTextColor);
            SDL_Texture *pausedTexture = SDL_CreateTextureFromSurface(renderer, pausedSurface);
            SDL_FreeSurface(pausedSurface);

            int pausedTextWidth, pausedTextHeight;
            SDL_QueryTexture(pausedTexture, NULL, NULL, &pausedTextWidth, &pausedTextHeight);
            SDL_Rect pausedRect = {(WINDOW_WIDTH - pausedTextWidth) / 2, (WINDOW_HEIGHT - pausedTextHeight) / 4, pausedTextWidth, pausedTextHeight};
            SDL_RenderCopy(renderer, pausedTexture, NULL, &pausedRect);
            SDL_DestroyTexture(pausedTexture);

            string timeText = formatTime(lastRemainingTime);
            SDL_Surface *textSurface = TTF_RenderText_Solid(largeFont, timeText.c_str(), textColor);
            SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);

            int textWidth, textHeight;
            SDL_QueryTexture(timeTexture, NULL, NULL, &textWidth, &textHeight);
            SDL_Rect renderQuad = {(WINDOW_WIDTH - textWidth) / 2, (WINDOW_HEIGHT - textHeight) / 2, textWidth, textHeight};

            SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad);
            SDL_DestroyTexture(timeTexture);

            drawButton(renderer, "Resume", pauseButton);
            drawButton(renderer, "Reset", resetButton);

            SDL_RenderPresent(renderer);
        }

        SDL_Delay(1000 / 60);
    }

    TTF_CloseFont(largeFont);
    return false;
}

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = NULL;
    Mix_Chunk *alarm = NULL;

    initSDL(window, renderer, font, alarm);

    bool reset = false;
    do
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        int countdownTime = inputTime(renderer, font);

        reset = runCountdown(renderer, font, alarm, countdownTime);
    } while (reset);

    closeSDL(window, renderer, font, alarm);

    return 0;
}