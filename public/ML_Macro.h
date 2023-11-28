#pragma once

#if __cplusplus
#define ML_BEGIN extern "C" {
#define ML_END }
#else
#define ML_BEGIN
#define ML_END
#endif
