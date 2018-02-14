#pragma once

#ifdef POGOCMP_SHARED_BUILD
#ifdef _WIN32
#ifdef POGOCMP_EXPORTS
#define POGOCMP_API __declspec(dllexport)
#else
#define POGOCMP_API __declspec(dllimport)
#endif
#else // simply assume *nix + GCC-like compiler
#define POGOCMP_API __attribute__((visibility("default")))
#endif
#else
#define POGOCMP_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

POGOCMP_API int PoGoCmpVersion();

#ifdef __cplusplus
}
#endif
