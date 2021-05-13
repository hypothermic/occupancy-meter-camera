#include <esp_event.h>
#include <esp_netif.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_wifi_default.h>
#include <sdkconfig.h>

#include "occupancy_http.h"
#include "occupancy_log.h"
#include "occupancy_wifi.h"

static esp_event_handler_instance_t instance_any_id, instance_got_ip;
static int retry_count = 0;

static void handle_got_ip_event(void* arg, esp_event_base_t event_base,
			    int32_t event_id, void* event_data) {
	ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

	occupancy_http_start();

	// Log naar serial zodat we het IP weten enzo
	ESP_LOGI(OCC_LOG_TAG,
		"IP-adres ontvangen: " IPSTR ","
		"gateway: " IPSTR ","
		"netmask: " IPSTR,
		IP2STR(&event->ip_info.ip),
		IP2STR(&event->ip_info.gw),
		IP2STR(&event->ip_info.netmask));

	retry_count = 0;
}

static void handle_wifi_event(void* arg, esp_event_base_t event_base,
			     int32_t event_id, void* event_data) {

	switch (event_id) {
		// Wifi zender is gereed om te starten
		case WIFI_EVENT_STA_START:
			esp_wifi_connect();
			break;

		// Wifi verbinding onderbroken
		case WIFI_EVENT_STA_DISCONNECTED:
			occupancy_http_stop();

			if (retry_count < OCC_WIFI_MAXIMUM_RETRY) {
				OCC_LOG_INFO("Opnieuw proberen om te verbinden...")
				esp_wifi_connect();
				++retry_count;
			} else {
				OCC_LOG_INFO("Fout bij het verbinden met de hotspot")
			}
			break;
	}
}

void occupancy_wifi_init(void) {
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT()
	wifi_config_t wifi_config = {
		.sta = {
			.ssid = OCC_WIFI_SSID,
			.password = OCC_WIFI_PASS,

			.pmf_cfg = {
				.capable = true,
				.required = false
			},
		},
	};

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
							    &handle_wifi_event, NULL,
							    &instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
							    &handle_got_ip_event, NULL,
							    &instance_got_ip));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
}

// Hebben we waarschijnlijk niet nodig, maar voor de goede orde heb ik dit toch gemaakt, mocht het ooit nodig zijn
void occupancy_wifi_deinit(void) {
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
}
