#ifndef COUNTDOWN_H // Kiểm tra nếu COUNTDOWN_H chưa được định nghĩa
#define COUNTDOWN_H // Định nghĩa COUNTDOWN_H để tránh việc định nghĩa lại nhiều lần

#include <SDL.h> 
#include <SDL_ttf.h> 
#include <SDL_mixer.h> 
using namespace std; 

bool runCountdown(SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm, int &countdownTime);

#endif // COUNTDOWN_H // Kết thúc kiểm tra định nghĩa COUNTDOWN_H