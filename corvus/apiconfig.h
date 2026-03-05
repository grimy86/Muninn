#pragma once

#ifdef CORVUS_EXPORTS
#define CORVUS_API extern "C" __declspec(dllexport)
#else
#define CORVUS_API extern "C" __declspec(dllimport)
#endif

#define CORVUS_CALL __fastcall

#ifndef _In_
#define _In_
#endif // !_In_

#ifndef _Out_
#define _Out_
#endif // !_Out_

#ifndef _Out_opt_
#define _Out_opt_
#endif // !_Out_opt_