#pragma once
#include <Windows.h>
#include <cstdint>
#include <stdio.h>

namespace AssaultCube
{
	// TYPES
	struct SDL_Surface;
	typedef void(__cdecl* SDL_WM_SetCaption_t)(const char* title, const char* icon);
	typedef SDL_Surface* (__cdecl* IMG_Load_t)(const char* file);
	typedef void(__cdecl* SDL_WM_SetIcon_t)(SDL_Surface* icon, uint8_t* mask);
	typedef int(__cdecl* ScanForCheatEngine_t)();
	// add to Globals.h
	typedef void(__cdecl* SDL_GL_SwapBuffers_t)();

	// FUNCTION POINTERS
	inline SDL_WM_SetCaption_t SDL_WM_SetCaption = (SDL_WM_SetCaption_t)0x49E58C;
	inline IMG_Load_t IMG_Load = (IMG_Load_t)0x49E5B6;
	inline SDL_WM_SetIcon_t SDL_WM_SetIcon = (SDL_WM_SetIcon_t)0x6812B110; //SDL.dll
	inline SDL_GL_SwapBuffers_t SDL_GL_SwapBuffers = (SDL_GL_SwapBuffers_t)0x4DA3E0;

	// INSTRUCTION & FUNCTION START OFFSETS
	inline uintptr_t scanForCheatEngineFunction{ 0x4051D0 };
	inline uintptr_t shotDelayInstruction{ 0x4637E4 };
	inline uintptr_t recoilInstruction{ 0x462274 };

	// GAME VARIABLES
	inline uintptr_t cameraSetupFunction{ 0x404080 };
	inline uintptr_t localPlayer{ 0x50F4F4 };
	inline uintptr_t entityList{ 0x50F4F8 };
	inline uintptr_t maxPlayers{ 0x50F500 };
	inline uintptr_t gameTimer{ 0x509EB0 };
	inline uintptr_t kickBackMultiplyer{ 0x4EE3F8 };
	inline uintptr_t health{ 0xF8 };
	inline uintptr_t armor{ 0xFC };
	inline uintptr_t team{ 0x32C };
	inline uintptr_t weaponPtr{ 0x374 };
	inline uintptr_t ammoPtr{ 0x14 };
	inline uintptr_t posX{ 0x34 };
	inline uintptr_t posY{ 0x38 };
	inline uintptr_t posZ{ 0x3C };
	inline uintptr_t camX{ 0x10 };
	inline uintptr_t camY{ 0x14 };
	inline uintptr_t camZ{ 0x18 };
	inline uintptr_t yaw{ 0x40 };
	inline uintptr_t pitch{ 0x44 };
	inline uintptr_t roll{ 0x48 };

	// SHELLCODE PATCHES
	inline BYTE shotDelayPatch[5] = { 0x90, 0x90, 0x8B, 0x76, 0x14 };
	inline BYTE kickBackMultiplyerPatch[4] = { 0x00, 0x00, 0x00, 0x00 };

	// HOOK RELATED
	inline int recoilInstructionLength{ 7 };
	inline uintptr_t recoilJumpBackAddress{
		AssaultCube::recoilInstruction + recoilInstructionLength };
	inline BYTE* gateway{};
}