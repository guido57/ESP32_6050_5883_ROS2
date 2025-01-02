// IMU.cpp

#include "imu.h"
#include <sensor_msgs/msg/imu.h>
//#include <tf2/LinearMath/Quaternion.h>
#include <cmath>
#include <chrono>
#include <geometry_msgs/msg/quaternion.h>
#include <sensor_msgs/msg/imu.h>


// Constructor
IMU::IMU() {}

// Initialize the IMU
bool IMU::initialize() {
    bool success = true;

    // Initialize QMC5883L magnetometer
    qmc5883 = new DFRobot_QMC5883(&Wire, /*I2C addr*/QMC5883_ADDRESS);
    if (!qmc5883->begin()) {
        printf("Failed to initialize QMC5883L magnetometer\r\n");
        success = false;
    } else {
        qmc5883->setRange(QMC5883_RANGE_8GA);
        qmc5883->setMeasurementMode(QMC5883_CONTINOUS);
        qmc5883->setDataRate(QMC5883_DATARATE_200HZ);
        //qmc5883.setOverSampling(DFRobot_QMC5883::OSR_512);
    }

    // Initialize MPU6050 accelerometer and gyroscope
    if (!mpu.begin()) {
        printf("Failed to initialize MPU6050\r\n");
        success = false;
    } else {
        mpu.setGyroRange(MPU6050_RANGE_250_DEG);
        mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    }

    return success;
}

// Read magnetometer data
bool IMU::readMagnetometerData(float &x, float &y, float &z) {
    // int16_t rawX, rawY, rawZ;
    sVector_t svt;
    svt=qmc5883->readRaw();
        // x = static_cast<float>(rawX);
        // y = static_cast<float>(rawY);
        // z = static_cast<float>(rawZ);
    x = svt.XAxis;
    y = svt.YAxis;
    z = svt.ZAxis;
    return true;
    
}

// Read accelerometer and gyroscope data
bool IMU::readMPUData(float &ax, float &ay, float &az, float &gx, float &gy, float &gz) {
    
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    if(mpu.getEvent(&a, &g, &temp)){
        ax = a.acceleration.x ;
        ay = a.acceleration.y;
        az = a.acceleration.z;

        gx = a.gyro.x;
        gy = a.gyro.y;
        gz = a.gyro.z;

        return true;
    }
    return false; // Failed to read data
}

// Publish IMU data as a ROS2 IMU message
void IMU::publish(rcl_publisher_t * publisher) {
    float mx, my, mz;
    float ax, ay, az, gx, gy, gz;

    if (!readMagnetometerData(mx, my, mz)) {
        printf("Failed to read magnetometer data\r\n");
        return;
    }

    if (!readMPUData(ax, ay, az, gx, gy, gz)) {
        printf("Failed to read MPU6050 data\r\n");
        return;
    }

    // Normalize magnetometer data for orientation computation
    float magnitude = std::sqrt(mx * mx + my * my + mz * mz);
    if (magnitude > 0.0f) {
        mx /= magnitude;
        my /= magnitude;
        mz /= magnitude;
    }

    // Convert magnetometer data to a quaternion (simplified example, assumes no tilt)
    geometry_msgs__msg__Quaternion orientation;
    orientation.x = 0.0;
    orientation.y = 0.0;
    orientation.z = atan2(my, mx);
    orientation.w = 1.0;
    //orientation.normalize();



    sensor_msgs__msg__Imu imu_msg;

    // Populate header
    imu_msg.header.stamp.sec = rmw_uros_epoch_millis() / 1000;
    imu_msg.header.stamp.nanosec = rmw_uros_epoch_nanos() %1000000L;
    imu_msg.header.frame_id.data     = const_cast<char*>("imu_link");
    imu_msg.header.frame_id.size     = strlen(imu_msg.header.frame_id.data);
    imu_msg.header.frame_id.capacity = imu_msg.header.frame_id.size + 1;
    // Populate orientation (magnetometer data)
    imu_msg.orientation.x = orientation.x;
    imu_msg.orientation.y = orientation.y;
    imu_msg.orientation.z = orientation.z;
    imu_msg.orientation.w = orientation.w;

    imu_msg.orientation_covariance[0] = -1; // Mark as unknown

    // Populate angular velocity (gyroscope data)
    imu_msg.angular_velocity.x = gx;
    imu_msg.angular_velocity.y = gy;
    imu_msg.angular_velocity.z = gz;

    imu_msg.angular_velocity_covariance[0] = -1; // Mark as unknown

    // Populate linear acceleration (accelerometer data)
    imu_msg.linear_acceleration.x = ax;
    imu_msg.linear_acceleration.y = ay;
    imu_msg.linear_acceleration.z = az;

    imu_msg.linear_acceleration_covariance[0] = -1; // Mark as unknown

    rcl_ret_t ret = rcl_publish(publisher, &imu_msg, NULL);
    if(ret != RCL_RET_OK)
        printf("Error publishing IMU message\r\n");
}
