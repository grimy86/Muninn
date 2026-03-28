#ifndef MUNINN_API_CONFIG_H
#define MUNINN_API_CONFIG_H

#ifdef __cplusplus
#define MUNINN_API extern "C"
#else
#define MUNINN_API
#endif

#ifdef _MSC_VER
#define MUNINN_CALL __fastcall
#else
#define MUNINN_CALL
#endif

// Optional SAL macros
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

#endif // !MUNINN_API_CONFIG_H