# OVERVIEW

This is a complete example to build an IMU WiFi ROS publisher using:
* ESP32
* MPU6050 Accelerometer and Gyroscope
* QMC5883 Magnetometer

# SCHEMATIC
![](docs/ESP32_6050_5883_schematic.png)

The MPU6050 standard I2C Address is at 0x68
The QMC5883 standard I2C Address is at 0x0D

# TEST

## Run a micro-ROS agent on a PC in your network

As any micro-ROS node, you need to run a ROS2 Agent to interface it.

```
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888 
 ```

If you don't know much about it, please read here how to run a micro-ROS agent on a PC in your network.
https://technologiehub.at/project-posts/micro-ros-on-esp32-tutorial/#:~:text=1.%20Create%20the%20micro%2DROS%20Agent

## Clone this repository, build it and upload it to your ESP32

I used VSCODE but I suppose you could also use Arduino IDE (with few changes)

If everything is Ok, you should see something like this on the VSCODE terminal:

```
.....
Connected to Wi-Fi
IP Address: 192.168.1.217
SSID: TIM-123467
BSSID: 6C:99:61:3F:0C:F4
rmw_uros_set_custom_transport...
rcl_get_default_allocator...
rclc_support_init...
rclc_node_init_default...
rclc_publisher_init_default /imu...
IMU initialization successful
```

while on the shell where you run the micro-ROS agent:

```
[1735850587.286703] info     | Root.cpp           | create_client            | create                 | client_key: 0x6B8857D0, session_id: 0x81
[1735850587.286800] info     | SessionManager.hpp | establish_session        | session established    | client_key: 0x6B8857D0, address: 192.168.1.217:47138
[1735850587.313214] info     | ProxyClient.cpp    | create_participant       | participant created    | client_key: 0x6B8857D0, participant_id: 0x000(1)
[1735850587.323833] info     | ProxyClient.cpp    | create_topic             | topic created          | client_key: 0x6B8857D0, topic_id: 0x000(2), participant_id: 0x000(1)
[1735850587.341449] info     | ProxyClient.cpp    | create_publisher         | publisher created      | client_key: 0x6B8857D0, publisher_id: 0x000(3), participant_id: 0x000(1)
[1735850587.352041] info     | ProxyClient.cpp    | create_datawriter        | datawriter created     | client_key: 0x6B8857D0, datawriter_id: 0x000(5), publisher_id: 0x000(3)
```
