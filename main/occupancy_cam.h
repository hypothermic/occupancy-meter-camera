#ifndef OCCUPANCY_CAM_H
#define OCCUPANCY_CAM_H

#include <esp_camera.h>

// Voor resolutie 320x240, uncomment dit:
//#define OCC_CAM_FRAME_SIZE FRAMESIZE_QVGA
// Voor resolutie 800x600, uncomment dit:
#define OCC_CAM_FRAME_SIZE FRAMESIZE_SVGA
// Voor resolutie 1024x768, uncomment dit:
//#define OCC_CAM_FRAME_SIZE FRAMESIZE_XGA

/*
 * Initialiseer de OV2620 cameramodule met de juiste instellingen voor ons bordje.
 */
void occupancy_cam_init(void);

/*
 * Schakel de camera functionaliteit uit.
 * Eigenlijk het omgekeerde van occupancy_cam_init.
 */
void occupancy_cam_deinit(void);

#endif //OCCUPANCY_CAM_H
