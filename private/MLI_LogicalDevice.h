#pragma once

#include <vulkan/vulkan.h>
#include "MLI_Fwd.h"
#include "MLI_Queue.h"

typedef struct MLI_LogicalDevice {
    VkDevice logicalDevice;
    MLI_Queue computeQueue;
    uint32_t computeQueueIndex;

    VkBuffer* buffers;
    uint32_t bufferCount;
} MLI_LogicalDevice;

void MLI_LogicalDevice_create(MLI_LogicalDevice* logicalDevice, MLI_PhysicalDevice* physicalDevice);
void MLI_LogicalDevice_destroy(MLI_LogicalDevice* logicalDevice);

ML_Result MLI_LogicalDevice_createBuffer();
