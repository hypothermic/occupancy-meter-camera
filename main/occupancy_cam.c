#include "occupancy_cam.h"

#include <esp_camera.h>

/*
 * Initialiseer de OV2620 cameramodule met de juiste instellingen voor ons bordje.
 */
void occupancy_cam_init(void) {
	// Config met AI-Thinker specifieke poorten
	camera_config_t config;
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = 5;
	config.pin_d1 = 18;
	config.pin_d2 = 19;
	config.pin_d3 = 21;
	config.pin_d4 = 36;
	config.pin_d5 = 39;
	config.pin_d6 = 34;
	config.pin_d7 = 35;
	config.pin_xclk = 0;
	config.pin_pclk = 22;
	config.pin_vsync = 25;
	config.pin_href = 23;
	config.pin_sscb_sda = 26;
	config.pin_sscb_scl = 27;
	config.pin_pwdn = 32;
	config.pin_reset = -1;
	config.xclk_freq_hz = 20000000;
	config.pixel_format = PIXFORMAT_JPEG;
	config.frame_size = OCC_CAM_FRAME_SIZE;
	config.jpeg_quality = 10;
	config.fb_count = 2;

	// Initialiseer camera met de config van hierboven
	ESP_ERROR_CHECK(esp_camera_init(&config));
}

// Hebben we waarschijnlijk niet nodig, maar voor de goede orde heb ik dit toch gemaakt, mocht het ooit nodig zijn
/*
 * Schakel de camera functionaliteit uit.
 * Eigenlijk het omgekeerde van occupancy_cam_init.
 */
void occupancy_cam_deinit(void) {
	esp_camera_deinit();
}
