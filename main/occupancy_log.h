#ifndef OCCUPANCY_LOG_H
#define OCCUPANCY_LOG_H

#include <esp_log.h>

#define OCC_LOG_TAG "occupancy-meter-camera"

#define OCC_LOG_INFO(message) ESP_LOGI(OCC_LOG_TAG, message);
#define OCC_LOG_ERROR(message) ESP_LOGE(OCC_LOG_TAG, message);

#endif //OCCUPANCY_LOG_H
