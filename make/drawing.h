#ifndef DRAWING_H
#define DRAWING_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
using namespace std;

void drawButton(SDL_Renderer *renderer, const string &label, SDL_Rect &buttonRect);

void drawTime(SDL_Renderer *renderer, TTF_Font *font, const string &timeText);

void drawProgressBar(SDL_Renderer *renderer, int totalTime, int remainingTime);

#endif // DRAWING_H