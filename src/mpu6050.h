// mpu6050.h - Driver for the MPU6050 accelerometer and gyroscope.
// https://github.com/maarten-pennings/MPU6050
#ifndef _MPU6050_H_
#define _MPU6050_H_
/**
 * TODO: PICO anpassen
*/
#ifndef PICO
#define PICO
#endif
#include <stdint.h>
#include <math.h>
#ifndef PICO
#include <Arduino.h>
#else
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdio.h>

#endif

// Generic constants
// =================

#define MPU6050_VERSION               "1.0"
#define MPU6050_GRAVITY_EARTH         9.80665 // m/s^2
#ifdef PICO
#define PI M_PI
#define I2C_SDA 8
#define I2C_SCL 9
#endif

// Data types storing the measurement results of the MPU6050
// =========================================================
int test_LKA(int a, int b);
/**
 * TODO: PICO anpassen
*/
#ifdef PICO
int millis(void);
#endif
// Internal structure storing calibration data
struct MPU6050_Calibrate_t { public:
    float accel_x, accel_y; 
    float gyro_x, gyro_y, gyro_z;
};

// Acceleration (in m/s^2) over three axis 
struct MPU6050_Accel_t { public:
    int   error; // 0 is ok, otherwise I2C error, see error_str()
    float x,y,z; // m/s^2
};

// Drift (in degree/s) over the three axis
struct MPU6050_Gyro_t { public:
    int   error; // 0 is ok, otherwise I2C error, see error_str()
    float x,y,z; // degree/s
};

// Temperature (Celsius)
struct MPU6050_Temp_t { public:
    int   error; // 0 is ok, otherwise I2C error, see error_str()
    float t;     // Celsius
};

// Orientation (degree) computed from acceleration and gyroscope
struct MPU6050_Dir_t { 
public:
    int      error;            // 0 is ok, otherwise I2C error, see error_str()
    float    roll, pitch, yaw; // degree 
private: friend class MPU6050;
    uint32_t gyro_time_ms;         
    float    gyro_angle_x;
    float    gyro_angle_y;
    float    gyro_angle_z;
    
};

// All data in one struct
struct MPU6050_t { public:
    MPU6050_Accel_t accel; 
    MPU6050_Gyro_t  gyro; 
    MPU6050_Dir_t   dir;
    MPU6050_Temp_t  temp;
};

// Data types configuring the MPU6050
// ==================================

// Determines the gyroscope range (Dps = degree per second). Select the lowest range for highest precision.
enum MPU6050_GyroRange {
    Max250Dps, Max500Dps, Max1000Dps, Max2000Dps
};

// Determines the accelerometer range (g=9.8m/s^2). Select the lowest range for highest precision.
enum MPU6050_AccelRange {
    Max2g, Max4g, Max8g, Max16g
};

// Configures the internal Digitally programmable Low Pass Filter (DLPF).
enum MPU6050_DLPFBandwidth {
    //           Accel Gyro
    Max260Hz, //   260  256 
    Max184Hz, //   184  188 
    Max94Hz,  //    94   98 
    Max44Hz,  //    44   42 
    Max21Hz,  //    21   20 
    Max10Hz,  //    10   10 
    Max5Hz    //     5    5 
};

// The Sample Rate is generated by dividing the gyroscope output rate by MPU6050_SampleRateDiv
// where Gyroscope Output Rate = 8kHz when the DLPF is disabled (DLPF_CFG = 0 or 7), and 1kHz when the DLPF is enabled (see Register 26).
enum MPU6050_SampleRateDiv {
    Div1, Div2, Div3, Div4, Div5, Div6,  Div7
};

// The MPU6050 driver class
// ========================

class MPU6050 {
    public: // Main interface
        // Constructor
        MPU6050();

        // Wake-up device, configure and calibrate it. Returns I2C communication result: 0 is ok, otherwise see error_str()
        #ifndef PICO
        int begin(int calibrationsamples=500, MPU6050_AccelRange ar= Max2g, MPU6050_GyroRange gr= Max250Dps, MPU6050_DLPFBandwidth bw= Max260Hz, MPU6050_SampleRateDiv sr= Div7);
        #else
        int begin(int calibrationsamples=500, 
                  MPU6050_AccelRange ar= Max2g, 
                  MPU6050_GyroRange gr= Max250Dps, 
                  MPU6050_DLPFBandwidth bw= Max260Hz, 
                  MPU6050_SampleRateDiv sr= Div7,
                  void* i2c_inst = i2c0);
        #endif
        // Execute measurement and return result
        MPU6050_t       get();

        // Many functions (and data types) return (store) an (I2C) error code. The following function converts it to a string
        const char* error_str(int error );

    public: // Power related functions. They all returns I2C communication result: 0 is ok, otherwise see error_str()
        int reset();  // Resets all registers; begin() should be called to use the device after reset()
        int absent(); // Returns 0 if the MPU6050 can be reached via I2C, and has correct who-am-i 
        int sleep();  // Sensor goes to sleep (power saving mode, no data measurements, the default on power up); begin() causes wake()
        int wake();   // Wake device up

    public: // Configuration of the sensors. They all return I2C communication result: 0 is ok, otherwise see error_str()
        int setAccelRange(MPU6050_AccelRange range);
        int setGyroRange(MPU6050_GyroRange range);
        int setDLPFBandwidth(MPU6050_DLPFBandwidth bandwidth);
        int setSampleRateDivider(MPU6050_SampleRateDiv divider);
        // Calibration of the sensors. Returns I2C communication result: 0 is ok, otherwise see error_str()
        int calibrate(int numsamples);

    //private:
        uint8_t             _devaddress;
        MPU6050_AccelRange  _accelrange;
        MPU6050_GyroRange   _gyrorange;
        MPU6050_Calibrate_t _calibrate;
        MPU6050_Dir_t       _direction;
        #ifdef PICO
        i2c_inst_t*         _i2c_inst;
        #endif
        
        // Converts acceleration data to x and y angles (no z...??)        
        float angle_x(MPU6050_Accel_t accel);
        float angle_y(MPU6050_Accel_t accel);
        
        // I2C high level functions
        MPU6050_Accel_t readAcceleration();
        MPU6050_Gyro_t  readGyroscope();
        MPU6050_Temp_t  readTemperature();
        void            updateDirection(MPU6050_Accel_t,MPU6050_Gyro_t);

        // Raw data to real-world units
        float rawTemperatureToCelsius(int16_t rawTemperature); // Celsius
        float rawGyroscopeToDps(int16_t rawGyroscope);         // degree/s
        float rawAccelerationToMps2(int16_t rawAcceleration);  // m/s^2

        // I2C access functions
        int read8   (uint8_t addr, uint8_t  *value);
        int read16  (uint8_t addr, uint16_t *value);
        int read3x16(uint8_t addr, uint16_t *value0, uint16_t *value1, uint16_t *value2 );
        int write8  (uint8_t addr, uint8_t   value);
};

#endif // _MPU6050_H_
