#pragma once

#include <vulkan/vulkan.h>
#include "MLI_Fwd.h"
#include "ML_Result.h"

typedef struct MLI_Queue {
    VkQueue queue;
} MLI_Queue;

ML_Result MLI_Queue_queryComputeQueueIndex(uint32_t* index, const VkPhysicalDevice physicalDevice);
