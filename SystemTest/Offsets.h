#pragma once
typedef void(__cdecl* SDL_WM_SetCaption_t)(const char*, const char*);

SDL_WM_SetCaption_t SDL_WM_SetCaption = (SDL_WM_SetCaption_t)0x49E58C;

// usage
SDL_WM_SetCaption("New Game Title", 0);