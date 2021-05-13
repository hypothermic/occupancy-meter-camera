#ifndef OCCUPANCY_WIFI_H
#define OCCUPANCY_WIFI_H

// Deze zijn gedefined in Kconfig.projbuild !!! configureer ze met menuconfig
#define OCC_WIFI_SSID		CONFIG_OCC_WIFI_SSID
#define OCC_WIFI_PASS		CONFIG_OCC_WIFI_PASSWORD
#define OCC_WIFI_MAXIMUM_RETRY	CONFIG_OCC_MAXIMUM_RETRY

/*
 * Initialiseer de onboard wifi module.
 * Zodra de wifi verbonden is wordt automatisch de HTTP server gestart.
 */
void occupancy_wifi_init(void);

/*
 * Schakel de wifi functionaleit uit.
 * Eigenlijk het omgekeerde van occupancy_wifi_init, behalve dat de ESP-NETIF niet gedeinit wordt.
 */
void occupancy_wifi_deinit(void);

#endif //OCCUPANCY_WIFI_H
