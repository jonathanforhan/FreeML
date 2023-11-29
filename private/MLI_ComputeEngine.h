#pragma once

#include <vulkan/vulkan.h>

#if  (_DEBUG || !NDEBUG)
#define MLI_VALIDATION_LAYERS_ENABLED 1
#else
#define MLI_VALIDATION_LAYERS_ENABLED 0
#endif

typedef struct MLI_ComputeEngine {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkQueue computeQueue;
    uint32_t computeQueueIndex;
    VkBuffer srcBuffer;
    VkDeviceMemory srcMemory;
    VkBuffer dstBuffer;
    VkDeviceMemory dstMemory;
    VkShaderModule shader;
} MLI_ComputeEngine;

//--- Create
VkResult MLI_ComputeEngine_create(MLI_ComputeEngine* computeEngine);
VkResult MLI_ComputeEngine_createInstance(MLI_ComputeEngine* computeEngine);
VkResult MLI_ComputeEngine_createLogicalDevice(MLI_ComputeEngine* computeEngine);
VkResult MLI_ComputeEngine_createSrcBuffer(MLI_ComputeEngine* computeEngine);
VkResult MLI_ComputeEngine_createDstBuffer(MLI_ComputeEngine* computeEngine);
VkResult MLI_ComputeEngine_createShader(MLI_ComputeEngine* computeEngine, const char* path);
VkResult MLI_ComputeEngine_createPipeline(MLI_ComputeEngine* computeEngine);

//--- Choose
VkResult MLI_ComputeEngine_choosePhysicalDevice(MLI_ComputeEngine* computeEngine);

//--- Check
VkBool32 MLI_ComputeEngine_checkInstanceValidationSupport(void);

//--- Query
VkResult MLI_ComputeEngine_queryComputeQueueIndex(const VkPhysicalDevice physicalDevice, uint32_t* index);

//--- Evaluate
int32_t MLI_ComputeEngine_evaluatePhysicalDevice(const MLI_ComputeEngine* computeEngine, const VkPhysicalDevice device);

void MLI_ComputeEngine_destroy(MLI_ComputeEngine* computeEngine);
