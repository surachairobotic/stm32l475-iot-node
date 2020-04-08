#ifndef __IMU_H
#define __IMU_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32l4xx_hal.h"

class IMU{
  public:
    IMU();
    void get_sensor(float* data);
    void get_sensor_kalman(float* data);
    void rebase_kalman();
    
  private:
    float update_kalman_gain(float var, float deviation);
    float update_est_current(float kalman_gain, float measure, float est_last);
    float update_var(float kalman_gain, float var);

    float var[6], deviation[6], kalman_gain[6], est_current[6];
    int16_t pDataXYZ[3], old_pDataXYZ[3];
    float pGyroDataXYZ[3], old_pGyroDataXYZ[3];
};

#ifdef __cplusplus
}
#endif

#endif /* __IMU_H */

