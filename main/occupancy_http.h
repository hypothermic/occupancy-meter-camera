#ifndef OCCUPANCY_HTTP_H
#define OCCUPANCY_HTTP_H

/*
 * Start de HTTP server.
 * Het wifi MOET verbonden zijn met een hotspot, anders crasht deze functie.
 * Hopelijk is dit al eerder een keer geregeld in occupancy_wifi.c
 */
void occupancy_http_start(void);

/*
 * Stopt de HTTP server.
 */
void occupancy_http_stop(void);

#endif //OCCUPANCY_HTTP_H
