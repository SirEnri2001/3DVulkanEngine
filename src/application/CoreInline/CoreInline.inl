#pragma once

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#define DLL_IMPORT __attribute__((visibility("default")))
#endif