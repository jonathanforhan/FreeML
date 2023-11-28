#include "MLI_ComputeEngine.h"
#include <assert.h>
#include <stdlib.h>
#include "MLI_Instance.h"
#include "MLI_PhysicalDevice.h"

VkResult MLI_ComputeEngine_create(MLI_ComputeEngine* computeEngine) {
    assert(computeEngine != NULL);

    //--- Instance
    MLI_Instance_create(&computeEngine->instance);

    //--- Physical Device
    MLI_PhysicalDevice_find(&computeEngine->physicalDevice, &computeEngine->instance);

    //--- Logical Device and Compute Queue
    MLI_LogicalDevice_create(&computeEngine->logicalDevice, &computeEngine->physicalDevice);

    return VK_SUCCESS;
}

void MLI_ComputeEngine_destroy(MLI_ComputeEngine* computeEngine) {
    assert(computeEngine != NULL);

    MLI_LogicalDevice_destroy(&computeEngine->logicalDevice);
    MLI_Instance_destroy(&computeEngine->instance);
}
