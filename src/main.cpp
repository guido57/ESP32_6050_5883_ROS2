#include <micro_ros_arduino.h>
#include "imu.h"
#include "wifimonitor.h"

extern String ssid;
extern String pass;
extern IPAddress ros2_agent_ipa;
extern int ros2_agent_port;


rcl_publisher_t imu_publisher;
rcl_allocator_t allocator;
rcl_node_t node;
rclc_support_t support;

WiFiMonitor *wifimonitor;

// Initialize your IMU object
IMU imu;

#define CHECK_AND_REPORT(statement, error_msg)  \
    printf(error_msg); printf("...\r\n");       \
    if ((ret = (statement)) != RCL_RET_OK) {    \
        printf(error_msg " error=%d\r\n", ret); \
         \
    }

void setup() {
    // Start Serial for debugging
    Serial.begin(115200);

    wifimonitor = new WiFiMonitor();
    wifimonitor->Connect();
  
    struct my_micro_ros_agent_locator {
      IPAddress address;
      int port;
    } static locator;
  locator.address = ros2_agent_ipa;
  locator.port = ros2_agent_port;

  rcl_ret_t ret;
  CHECK_AND_REPORT(
    rmw_uros_set_custom_transport(false, (void *) &locator, arduino_wifi_transport_open, arduino_wifi_transport_close, arduino_wifi_transport_write, arduino_wifi_transport_read),
    "rmw_uros_set_custom_transport"
  );

  printf("rcl_get_default_allocator...\r\n");
  allocator = rcl_get_default_allocator();

  CHECK_AND_REPORT(rclc_support_init(&support, 0, NULL, &allocator), "rclc_support_init");

  CHECK_AND_REPORT(rclc_node_init_default(&node, "micro_ros_arduino_wifi_node", "", &support), "rclc_node_init_default");

  CHECK_AND_REPORT(rclc_publisher_init_default(&imu_publisher, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu), "/imu"), "rclc_publisher_init_default /imu");

    // Initialize IMU
    if (!imu.initialize()) {
        printf("Failed to initialize IMU sensors\r\n");
    } else {
        printf("IMU initialization successful\r\n");
    }

}

void loop() {
    // Publish IMU data at a frequency of 10 Hz
    imu.publish(&imu_publisher);

    // Add a delay to control the loop frequency
    delay(100);  // Delay 100 ms for 10 Hz publishing frequency
}
