#include <esp_event.h>
#include <esp_netif.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_wifi_default.h>
#include <sdkconfig.h>

#include "occupancy_http.h"
#include "occupancy_log.h"
#include "occupancy_wifi.h"

/*
 * Deze functie wordt aangeroepen wanneer de hotspot ons een IP adres heeft gegeven.
 * De HTTP server wordt opgestart zodat we requests kunnen ontvangen.
 */
static void handle_got_ip_event(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/*
 * Deze functie wordt aangeroepen bij de volgende events:
 * - De Wifi zender is gereed
 * - De Wifi connectie is verbroken
 * In het switch statement worden beide cases behandeld
 *
 * Als de connectie verbroken is dan zorgt deze functie er voor dat
 * de HTTP server gestopt wordt.
 */
static void handle_wifi_event(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/*
 * De event handlers voor de WIFI_EVENT's en IP_EVENT's
 */
static esp_event_handler_instance_t event_handler_wifi, event_handler_ip;

/*
 * Het aantal keer dat opnieuw is geprobeerd om de wifi verbinding tot stand te brengen
 */
static int retry_count = 0;

/*
 * Initialiseer de onboard wifi module.
 * Zodra de wifi verbonden is wordt automatisch de HTTP server gestart.
 */
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
							    &event_handler_wifi));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
							    &handle_got_ip_event, NULL,
							    &event_handler_ip));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
}

// Hebben we waarschijnlijk niet nodig, maar voor de goede orde heb ik dit toch gemaakt, mocht het ooit nodig zijn
/*
 * Schakel de wifi functionaleit uit.
 * Eigenlijk het omgekeerde van occupancy_wifi_init, behalve dat de ESP-NETIF niet gedeinit wordt.
 */
void occupancy_wifi_deinit(void) {
	// Stop HTTP server als deze nog aan stond
	occupancy_http_stop();

	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler_ip));
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler_wifi));
}

/*
 * Deze functie wordt aangeroepen wanneer de hotspot ons een IP adres heeft gegeven.
 * De HTTP server wordt opgestart zodat we requests kunnen ontvangen.
 */
static void handle_got_ip_event(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

	// Start HTTP server
	occupancy_http_start();

	// Log naar serial zodat we het IP weten enzo
	ESP_LOGI(OCC_LOG_TAG,
		"IP-adres ontvangen: " IPSTR ", "
		"gateway: " IPSTR ", "
		"netmask: " IPSTR,
		IP2STR(&event->ip_info.ip),
		IP2STR(&event->ip_info.gw),
		IP2STR(&event->ip_info.netmask));

	// Reset connectie retry count
	retry_count = 0;
}

/*
 * Deze functie wordt aangeroepen bij de volgende events:
 * - De Wifi zender is gereed
 * - De Wifi connectie is verbroken
 * In het switch statement worden beide cases behandeld
 *
 * Als de connectie verbroken is dan zorgt deze functie er voor dat
 * de HTTP server gestopt wordt.
 */
static void handle_wifi_event(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	switch (event_id) {
		// Wifi zender is gereed om te starten
		case WIFI_EVENT_STA_START:
			esp_wifi_connect();
			break;

			// Wifi verbinding onderbroken
		case WIFI_EVENT_STA_DISCONNECTED:
			// Stop HTTP server
			occupancy_http_stop();

			// Is het maximum connection retries bereikt?
			if (retry_count < OCC_WIFI_MAXIMUM_RETRY) {
				// Nee, probeer nog een keer te verbinden
				OCC_LOG_INFO("Opnieuw proberen om te verbinden...")
				esp_wifi_connect();
				++retry_count;
			} else {
				// Ja, print een error. TODO handle dit
				OCC_LOG_ERROR("Fout bij het verbinden met de hotspot")
			}
	}
}