#pragma once
#include "ML_Macro.h"

ML_BEGIN

typedef enum ML_Result {
    ML_SUCCESS = 0,
    ML_ERROR_OUT_HOST_OF_MEMORY = -1,
    ML_ERROR_OUT_DEVICE_OF_MEMORY = -2,
    ML_ERROR_FEATURE_NOT_PRESENT = -8,
} ML_Result;

ML_END
