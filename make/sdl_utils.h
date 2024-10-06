#ifndef SDL_UTILS_H 
#define SDL_UTILS_H 

#include <SDL.h>       
#include <SDL_ttf.h>   
#include <SDL_mixer.h>
#include <string>      
using namespace std;  

void handleError(const string &message, SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm);

void initSDL(SDL_Window *&window, SDL_Renderer *&renderer, TTF_Font *&font, Mix_Chunk *&alarm);

void closeSDL(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Mix_Chunk *alarm);

#endif // SDL_UTILS_H