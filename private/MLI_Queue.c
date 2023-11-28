#include "MLI_Queue.h"
#include <assert.h>
#include <stdlib.h>

#include "MLI_ComputeEngine.h"

ML_Result MLI_Queue_queryComputeQueueIndex(uint32_t* index, const VkPhysicalDevice physicalDevice) {
    assert(index != NULL);
    assert(physicalDevice != NULL);

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

    ML_Result ret = ML_ERROR_FEATURE_NOT_PRESENT;
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            *index = i;
            ret = ML_SUCCESS;
            break;
        }
    }

    free(queueFamilies);
    return ret;
}
