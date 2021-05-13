#ifndef OCCUPANCY_CAM_H
#define OCCUPANCY_CAM_H

#include <esp_camera.h>

// Voor resolutie 320x240, uncomment dit:
//#define OCC_CAM_FRAME_SIZE FRAMESIZE_QVGA
// Voor resolutie 800x600, uncomment dit:
#define OCC_CAM_FRAME_SIZE FRAMESIZE_SVGA
// Voor resolutie 1024x768, uncomment dit:
//#define OCC_CAM_FRAME_SIZE FRAMESIZE_XGA

void occupancy_cam_init(void);
void occupancy_cam_deinit(void);

#endif //OCCUPANCY_CAM_H
