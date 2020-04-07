#include "imu.h"

#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

IMU::IMU() {
  for(int i=0; i<3; i++) {
    pDataXYZ[i] = 0;
    pGyroDataXYZ[i] = 0.0;
  }

  rebase_kalman();
            
  BSP_GYRO_Init();
  BSP_GYRO_LowPower(0);
  BSP_ACCELERO_Init();
  BSP_ACCELERO_LowPower(0);
}

void IMU::get_sensor(float* data) {
  BSP_GYRO_GetXYZ(pGyroDataXYZ);
  BSP_ACCELERO_AccGetXYZ(pDataXYZ);
  for(int i=0; i<3; i++){
    *(data+i) = *(pGyroDataXYZ+i);
    *(data+i+3) = *(pDataXYZ+i);
  }
}

void IMU::get_sensor_kalman(float* data) {
  get_sensor(data);

  for (int j = 0; j < 6; j++)
  {
    kalman_gain[j] = update_kalman_gain(var[j], deviation[j]);
    *(data+j+6) = update_est_current(kalman_gain[j], *(data+j), est_current[j]);
    var[j] = update_var(kalman_gain[j], var[j]);
  }
  
}

float IMU::update_kalman_gain(float var, float deviation) {
  //float kalman_gain = var / (var + deviation);
  return var / (var + deviation);
}

float IMU::update_est_current(float kalman_gain, float measure, float est_last) {
  //float est_current = est_last + kalman_gain * (measure - est_last);
  return est_last + kalman_gain * (measure - est_last);
}

float IMU::update_var(float kalman_gain, float var) {
  //var = (1 - kalman_gain) * var;
  return (1 - kalman_gain) * var;
}

void IMU::rebase_kalman() {
  for(int i=0; i<6; i++) {
    var[i] = 255;
    deviation[i] = 25;
    kalman_gain[i] = 0;
    est_current[i] = 500;
  }
}
