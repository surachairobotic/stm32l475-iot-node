#include <mbed.h>
#include <vector>
#include <string>
#include <kalman.h>
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

struct Names {
  enum type { toa, bank, inn, o, por, menghorng, michael, wari, aoff, test};
};

int8_t name = Names::toa;




//kalman path//
Kalman kalmanX;
Kalman kalmanY;

double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
int16_t tempRaw;

double gyroXangle, gyroYangle; // Angle calculate using the gyro only
double compAngleX, compAngleY; // Calculated angle using a complementary filter
double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter
// timer = micros();
//NO TIMER


float update_kalman_gain(float var,float deviation)
{
  float kalman_gain=var/(var+deviation);
    return kalman_gain;
}
float update_est_current(float kalman_gain,float measure,float est_last)
{
  float est_current=est_last+kalman_gain*(measure-est_last);
  return est_current;
}

float update_var(float kalman_gain , float var)
{
  var=(1-kalman_gain)*var;
  return var;
}
int main() {

    DigitalOut led(LED1), led2(LED2);
    int8_t device_id = name;
    unsigned long seq = 1;
 
    // Start Read sensor
    float sensor_value = 0;
    int16_t pDataXYZ[3] = {0};
    float pGyroDataXYZ[3] = {0};
	
    printf("Sensor init : start\n");
    BSP_GYRO_Init();
    BSP_GYRO_LowPower(0);
    BSP_ACCELERO_Init();
    BSP_ACCELERO_LowPower(0);
    printf("Sensor init : complete\n");
    
    int k = 0;
    Timer t;
    t.start();
    float l_time[9], previous_t=0.0;
    int32_t gyro[3] = {0};
    t.reset();
    previous_t=t.read();

    //kalmannnnnnnnnnnnnnnnnnnnnnnnnnnn
    float R = 0.5;
    float est = 980;
    float var = 255;
    float deviation =25;
    float kalman_gain=0;
    float est_current=est;

    while(1) {
      while(k<10000000){
        k=k+1;
      }
              BSP_GYRO_GetXYZ(pGyroDataXYZ);
        for(int i=0; i<3; i++)
          {
            gyro[i] = pGyroDataXYZ[i];
          }
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);

        // gyroX=pGyroDataXYZ[0];
        // gyroY=pGyroDataXYZ[1];
        // gyroZ=pGyroDataXYZ[2];
        // accX=gyro[0];
        // accY=gyro[1];
        // accZ=gyro[2];
        
        kalman_gain=update_kalman_gain(var,deviation);
        // printf("kalman_gain = %f \t",kalman_gain);
        est_current=update_est_current(kalman_gain,pDataXYZ[2],est_current);
        // printf("est_current = %f \t",est_current);  
        var=update_var(kalman_gain,var);
        // printf("var = %f \r\n",var);


        printf("MEA_ACC = %d \r\n",pDataXYZ[2]);
        printf("EST_ACC = %f \r\n",est_current);
        // kalman_gain=var/(var+deviation);
        // est_output=est_last+kalman_gain*(measure-est_last)
        // var=(1-kalman_gain)*var

        // printf( "GYRO=%d,%d,%d\r\n", gyro[0], gyro[1], gyro[2]); 
        // printf( "ACC=%d,%d,%d\r\n",  pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]); // roll pitch yaw
      k=0;
    }
    t.stop();
}

