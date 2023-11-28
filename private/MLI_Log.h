#pragma once
#include <stdio.h>

#define MLI_LOG_INFO(_Fmt, ...) do { fprintf(stdout, "FreeML Info: " _Fmt "\n", ##__VA_ARGS__); } while(0)
#define MLI_LOG_WARNING(_Fmt, ...) do { fprintf(stdout, "FreeML Warning: " _Fmt "\n", ##__VA_ARGS__); } while(0)
#define MLI_LOG_ERROR(_Fmt, ...) do { fprintf(stderr, "FreeML Error: " _Fmt "\n", ##__VA_ARGS__); } while(0)
