#include "MLI_LogicalDevice.h"

#include <assert.h>
#include <stdlib.h>

#include "MLI_Log.h"
#include "MLI_PhysicalDevice.h"

void MLI_LogicalDevice_create(MLI_LogicalDevice* logicalDevice, MLI_PhysicalDevice* physicalDevice) {
    assert(logicalDevice != NULL);
    assert(physicalDevice != NULL);

    ML_Result result = MLI_Queue_queryComputeQueueIndex(&logicalDevice->computeQueueIndex,
                                                        physicalDevice->physicalDevice);
    assert(result == ML_SUCCESS);

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0U,
        .queueFamilyIndex = logicalDevice->computeQueueIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0U,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL,
        .pEnabledFeatures = &deviceFeatures,
    };

    VkResult r = vkCreateDevice(physicalDevice->physicalDevice,
                                &deviceCreateInfo,
                                NULL,
                                &logicalDevice->logicalDevice);
    if (r != VK_SUCCESS) {
        MLI_LOG_ERROR("Vulkan Device create failure code: %d", r);
        exit(EXIT_FAILURE);
    }

    vkGetDeviceQueue(logicalDevice->logicalDevice,
                     logicalDevice->computeQueueIndex,
                     0,
                     &logicalDevice->computeQueue.queue);
}

void MLI_LogicalDevice_destroy(MLI_LogicalDevice* logicalDevice) {
    assert(logicalDevice != NULL);
    vkDestroyDevice(logicalDevice->logicalDevice, NULL);
}
