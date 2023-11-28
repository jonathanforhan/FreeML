#include "MLI_PhysicalDevice.h"

#include <assert.h>
#include <stdlib.h>
#include "MLI_Instance.h"
#include "MLI_Log.h"
#include "MLI_Queue.h"

void MLI_PhysicalDevice_find(MLI_PhysicalDevice* physicalDevice, const MLI_Instance* instance) {
    assert(physicalDevice != NULL);
    assert(instance != NULL);

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        MLI_LOG_ERROR("failed to find GPU with Vulkan Support");
        exit(EXIT_FAILURE);
    }
    VkPhysicalDevice* devices = malloc(deviceCount * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance->instance, &deviceCount, devices);

    int32_t bestScore = INT32_MIN;
    for (uint32_t i = 0; i < deviceCount; i++) {
        int32_t score = MLI_PhysicalDevice_evaluate(devices[i]);
        if (score > bestScore) {
            bestScore = score;
            physicalDevice->physicalDevice = devices[i];
        }
    }

    if (bestScore < 0) {
        MLI_LOG_WARNING("Could not find optimal hardware performance may suffer");
    }

    free(devices);
}

int32_t MLI_PhysicalDevice_evaluate(const VkPhysicalDevice device) {
    assert(device != VK_NULL_HANDLE);

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int32_t score = 0;

    uint32_t computeQueueIndex;
    ML_Result result = MLI_Queue_queryComputeQueueIndex(&computeQueueIndex, device);
    if (result != ML_SUCCESS) {
        score -= 1000;
    }
    if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score -= 1000;
    }
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

