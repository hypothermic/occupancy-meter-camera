#ifndef OCCUPANCY_LOG_H
#define OCCUPANCY_LOG_H

#include <esp_log.h>

/*
 * De logger prefix die in de serial verschijnt. Gewoon de naam van onze applicatie.
 */
#define OCC_LOG_TAG "occupancy-meter-camera"

/*
 * Log een informatie bericht naar de serial.
 */
#define OCC_LOG_INFO(message) ESP_LOGI(OCC_LOG_TAG, message);

/*
 * Log een error bericht naar de serial.
 */
#define OCC_LOG_ERROR(message) ESP_LOGE(OCC_LOG_TAG, message);

#endif //OCCUPANCY_LOG_H
