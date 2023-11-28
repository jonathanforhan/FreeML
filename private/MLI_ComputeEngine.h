#pragma once

#include <vulkan/vulkan.h>
#include "MLI_Instance.h"
#include "MLI_PhysicalDevice.h"
#include "MLI_LogicalDevice.h"

#if  (_DEBUG || !NDEBUG)
#define MLI_VALIDATION_LAYERS_ENABLED 1
#else
#define MLI_VALIDATION_LAYERS_ENABLED 0
#endif

typedef struct MLI_ComputeEngine {
    MLI_Instance instance;
    MLI_PhysicalDevice physicalDevice;
    MLI_LogicalDevice logicalDevice;
} MLI_ComputeEngine;

VkResult MLI_ComputeEngine_create(MLI_ComputeEngine* computeEngine);
void MLI_ComputeEngine_destroy(MLI_ComputeEngine* computeEngine);
