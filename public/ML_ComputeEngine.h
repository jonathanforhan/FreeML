#pragma once
#include "ML_Result.h"
#include "ML_Macro.h"

ML_BEGIN

/// @file ML_ComputeEngine.h
/// @brief API facing Compute Engine

/// @brief Opaque Handle to ML_ComputeEngine
typedef void* ML_ComputeEngine;

/// @brief Create a new ML_ComputeEngine
/// @param[out] computeEngine Pointer to compute engine to create
/// @return Status
ML_Result ML_ComputeEngine_create(ML_ComputeEngine* computeEngine);

/// @brief Create a new ML_ComputeEngine
/// @param[in] computeEngine ML_ComputeEngine to clean up
void ML_ComputeEngine_destroy(ML_ComputeEngine* computeEngine);

ML_END
