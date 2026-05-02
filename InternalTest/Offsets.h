#pragma once
namespace AssaultCube
{
	struct SDL_Surface;

	typedef void(__cdecl* SDL_WM_SetCaption_t)(const char* title, const char* icon);
	typedef SDL_Surface* (__cdecl* IMG_Load_t)(const char* file);
	typedef void(__cdecl* SDL_WM_SetIcon_t)(SDL_Surface* icon, uint8_t* mask);

	inline SDL_WM_SetCaption_t SDL_WM_SetCaption = (SDL_WM_SetCaption_t)0x49E58C;
	inline IMG_Load_t IMG_Load = (IMG_Load_t)0x49E5B6;
	//SDL.dll
	inline SDL_WM_SetIcon_t SDL_WM_SetIcon = (SDL_WM_SetIcon_t)0x6812B110;
	
	/*
	"ac_client.exe"+637E4:
	db 89 0A 8B 76 14
	//mov [edx],ecx
	//mov esi,[esi+14]
	*/
	inline uintptr_t shotDelay = 0x4637E4;
	inline BYTE shotDelayPatch[5] = { 0x90, 0x90, 0x8B, 0x76, 0x14 };

	/*
	"ac_client.exe"+EE3F8:
	db 0A D7 23 BC
	// -0,009999999776f
	*/
	inline uintptr_t kickBackMultiplyer = 0x4EE3F8;
	inline BYTE kickBackMultiplyerPatch[4] = { 0x00, 0x00, 0x00, 0x00 };
}