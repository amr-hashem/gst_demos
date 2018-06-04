#ifndef IR_CAM_INFO_H
#define IR_CAM_INFO_H

#include <glib.h>
#include <string.h>

extern "C" {

typedef struct _area_temper {
    int32_t centerval;// center temperature of image
    int32_t max_val; // max(highest) temperature of image
    int32_t min_val; // min(lowest) temperature of image
    int32_t avg_val; // average temperature of image
}__attribute__ ((packed)) area_temper_t;

void emul_prev(unsigned char *data, size_t size) {
    //TODO, optimize using memcpy? not so useful since 00 00 33 00 recurring frequently
    const unsigned char EP_BYTE[] = {0, 0, 3};
    int delta = 0;
    for(int i = 2; i < size; ++i) {
        bool isEmulPrev = ! memcmp(data + i - 2, EP_BYTE, 3);
        if(isEmulPrev) ++delta;

        data[i] = data[i + delta];
    }
}

typedef struct {
    unsigned char gpsLatRef;
    unsigned short gpsLat;
    unsigned short gpsLatMm;
    unsigned short gpsLatSS;
    unsigned char gpsLonRef;
    unsigned short gpsLon;
    unsigned short gpsLonMM;
    unsigned short gpsLonSS;
    unsigned short gpsAltitudeRef;
    unsigned long gpsAltitude;
    unsigned short gpsSatellites;
    unsigned char gpsMeasureMode;
    unsigned short gPsDoP;
    float pitch;
    float roll;
    float yaw;
    long gpsLatVal;
    long gpsLonVal;
    float autopilot_pitch;
    float autopilot_roll;
    float autopilot_yaw;
    unsigned short voltage;
}__attribute__ ((packed)) YuneecGpsInfo;

typedef struct _ir_cam_info {
    u_int32_t info_size;
    area_temper_t all_area;
    area_temper_t custom_area;
    int32_t lock_max_tmper;
    int32_t lock_min_tmper;
    YuneecGpsInfo gps_info;
}__attribute__ ((packed)) ir_cam_info_t;

}

#endif // IR_CAM_INFO_H
