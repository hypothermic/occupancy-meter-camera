#include <esp_spi_flash.h>
#include <nvs_flash.h>
#include <sdkconfig.h>

#include "occupancy_cam.h"
#include "occupancy_log.h"
#include "occupancy_wifi.h"

static void init_nvs(void);

void app_main(void) {
	OCC_LOG_INFO("Start Occupancy Meter Camera")

	// Set minimum log level naar DEBUG, zodat alle INFO messages geprint worden
	esp_log_level_set(OCC_LOG_TAG, ESP_LOG_DEBUG);

	OCC_LOG_INFO("Initialiseer NVS")
	init_nvs();

	OCC_LOG_INFO("Initialiseer camera")
	occupancy_cam_init();

	OCC_LOG_INFO("Initialiseer Wifi")
	occupancy_wifi_init();

	OCC_LOG_INFO("Alles gereed")
}

static void init_nvs(void) {
	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK(ret);
}
