// IMU.h

#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>

#include <rclc/rclc.h>
#include <sensor_msgs/msg/imu.h>
//#include <tf2/LinearMath/Quaternion.h>
#include <DFRobot_QMC5883.h>
#include <Adafruit_MPU6050.h>


class IMU {
public:
    // Constructor
    IMU();

    // Initialize the IMU
    bool initialize();

    // Publish IMU data as a ROS2 IMU message
    void publish(rcl_publisher_t * publisher);

private:
    // Magnetometer instance
    DFRobot_QMC5883 * qmc5883;

    // MPU6050 instance
    Adafruit_MPU6050 mpu;

    // Read magnetometer data
    bool readMagnetometerData(float &x, float &y, float &z);

    // Read accelerometer and gyroscope data
    bool readMPUData(float &ax, float &ay, float &az, float &gx, float &gy, float &gz);
};

#endif // IMU_H
