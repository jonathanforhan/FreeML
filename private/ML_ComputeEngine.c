#include "ML_ComputeEngine.h"

#include <stdlib.h>
#include "MLI_ComputeEngine.h"
#include "MLI_Log.h"

ML_Result ML_ComputeEngine_create(ML_ComputeEngine* computeEngine) {
    MLI_ComputeEngine* computeEngineImpl = malloc(sizeof(MLI_ComputeEngine));
    if (computeEngineImpl == NULL) {
        return ML_ERROR_OUT_HOST_OF_MEMORY;
    }
    VkResult result = MLI_ComputeEngine_create(computeEngineImpl);
    if (result != VK_SUCCESS) {
        MLI_LOG_ERROR("Error on create code: %d", result);
        exit(EXIT_FAILURE);
    }
    *computeEngine = computeEngineImpl;
    return ML_SUCCESS;
}

void ML_ComputeEngine_destroy(ML_ComputeEngine* computeEngine) {
    MLI_ComputeEngine_destroy((MLI_ComputeEngine*)computeEngine);
    free(computeEngine);
}
