#ifndef API_CONFIG_H
#define API_CONFIG_H

#ifdef _MSC_VER
#define MUNINN_CALL __fastcall

#ifdef BUILDING_DLL
#define MUNINN_API __declspec(dllexport)
#else
#define MUNINN_API __declspec(dllimport)
#endif

#else // Non-MSVC compilers
#define MUNINN_API
#define MUNINN_CALL
#endif // !_MSC_VER

#ifndef _In_
#define _In_
#endif // !_In_

#ifndef _Out_
#define _Out_
#endif // !_Out_

#ifndef _Out_writes_
#define _Out_writes_(size)
#endif // !_Out_writes_

#ifndef _Out_opt_
#define _Out_opt_
#endif // !_Out_opt_
#endif // !API_CONFIG_H