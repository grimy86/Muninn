#ifndef MUNINN_API_CONFIG_H
#define API_CONFIG_H

#ifdef __cplusplus
#define MUNINN_EXTERN_C extern "C"
#else
#define MUNINN_EXTERN_C
#endif

#ifdef _MSC_VER

#define MUNINN_CALL __fastcall

#ifdef BUILDING_DLL
#define MUNINN_API MUNINN_EXTERN_C __declspec(dllexport)
#else
#define MUNINN_API MUNINN_EXTERN_C __declspec(dllimport)
#endif

#else // Non-MSVC

#define MUNINN_API MUNINN_EXTERN_C
#define MUNINN_CALL

#endif // _MSC_VER

#ifndef _In_
#define _In_
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _Out_writes_
#define _Out_writes_(size)
#endif

#ifndef _Out_opt_
#define _Out_opt_
#endif

#endif // API_CONFIG_H