#ifndef OCCUPANCY_WIFI_H
#define OCCUPANCY_WIFI_H

// Deze zijn gedefined in Kconfig.projbuild !!! configureer ze met menuconfig
#define OCC_WIFI_SSID		CONFIG_OCC_WIFI_SSID
#define OCC_WIFI_PASS		CONFIG_OCC_WIFI_PASSWORD
#define OCC_WIFI_MAXIMUM_RETRY	CONFIG_OCC_MAXIMUM_RETRY

void occupancy_wifi_init(void);
void occupancy_wifi_deinit(void);

#endif //OCCUPANCY_WIFI_H
