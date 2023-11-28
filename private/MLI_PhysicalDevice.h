#pragma once

#include <vulkan/vulkan.h>
#include "MLI_Fwd.h"

typedef struct MLI_PhysicalDevice {
    VkPhysicalDevice physicalDevice;
} MLI_PhysicalDevice;

void MLI_PhysicalDevice_find(MLI_PhysicalDevice* physicalDevice, const MLI_Instance* instance);
int32_t MLI_PhysicalDevice_evaluate(const VkPhysicalDevice device);
