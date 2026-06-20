# Raspbot ROS2 自主移动平台

基于 Raspberry Pi 5 + Yahboom Raspbot V2（麦轮底盘）的项目驱动 ROS2 系统。
从底层硬件驱动到自主导航，按里程碑逐步搭建一个分层的移动机器人系统。

- **平台**：Raspberry Pi 5 + Raspbot V2（4×TT 电机麦轮、驱动板 I2C@0x2B、超声波、4 路循迹、2DOF 摄像头云台）
- **系统**：Ubuntu 24.04 + ROS2 Jazzy
- **语言**：C++（rclcpp）

## 系统架构（目标）
分层设计：硬件 HAL 解耦总线协议，运动学层做 cmd_vel↔轮速转换，上层节点（行为、导航、监控）只与标准话题交互。

## 包结构

| 包 | 作用 |
|---|---|
| `raspbot_interfaces` | 自定义消息/服务/动作定义（`.msg` / `.srv` / `.action`） |
| `raspbot_demo` | 节点与硬件驱动（发布订阅、Service/Action、底盘驱动、QoS 实验） |

### 已定义接口

| 接口 | 类型 | 说明 |
|---|---|---|
| `raspbot_interfaces/msg/WheelSpeed` | msg | 四轮速度（含 `std_msgs/Header`） |
| `raspbot_interfaces/srv/GetStatus` | srv | 查询系统状态（state / uptime） |
| `raspbot_interfaces/action/Rotate` | action | 原地转指定角度（goal/feedback/result，可取消） |

### 硬件 HAL

`raspbot_demo/include/raspbot_demo/raspbot_board.hpp` —— `RaspbotBoard` 类，封装 0x2B 驱动板的 I2C 寄存器协议：
- 电机控制（reg 0x01，麦轮 4 电机）、舵机云台（0x02）
- 超声波测距（0x07 开关 + 0x1a/0x1b 读距）、四路循迹（0x0a）
- RAII：析构自动 `stop_all()`；C++ 调 libi2c（`-li2c`）

## 构建

```bash
cd ~/raspbot_ws
colcon build
source install/setup.bash
```

> 工作空间已配置 `colcon_defaults.yaml`（默认开启 `symlink-install` 与 `compile_commands.json` 导出，供 clangd 使用）。

## 运行

### 底盘驱动 + 键盘遥控（M4）

```bash
# 终端 1：底盘驱动（订阅 /cmd_vel，解算后经 I2C 驱动电机）
ros2 run raspbot_demo chassis_driver

# 终端 2：键盘遥控（i 前进 / , 后退 / j,l 转向 / k 停）
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```

> ⚠️ 首次测试请将车架空（轮子悬空）、底盘电池开关打开。chassis_driver 内置 0.5s 看门狗，断连自动停车；Ctrl-C 退出经 RAII 析构停车。

### 发布订阅 + QoS（M1/M2）

```bash
ros2 run raspbot_demo wheel_pub      # 发 WheelSpeed
ros2 run raspbot_demo wheel_sub      # 收 WheelSpeed
ros2 topic echo /wheel_speed
ros2 topic info /wheel_speed --verbose   # 看两端 QoS 是否匹配
```

### Service / Action（M3）

```bash
# Service：查询状态
ros2 run raspbot_demo status_server
ros2 service call /get_status raspbot_interfaces/srv/GetStatus

# Action：原地转 90 度（带实时反馈，可中途取消）
ros2 run raspbot_demo rotate_server
ros2 action send_goal /rotate raspbot_interfaces/action/Rotate "{target_angle_deg: 90}" --feedback
```

## 进度

- [x] **M0** 工作空间 + 工程底盘（colcon / ament / overlay / clangd）
- [x] **M1** 发布订阅 + 自定义消息（WheelSpeed，含时间戳纪律）
- [x] **M2** QoS 与 DDS 深化（RxO 兼容性、传感器 vs 控制语义）
- [x] **M3** Service + Action（含异步客户端、execute 线程模型）
- [x] **M4** C++ HAL 移植 + chassis_driver（I2C 0x2B、麦轮解算、看门狗）
- [ ] **M5** 里程计 + TF
- [ ] **M6** URDF + RViz
- [ ] **M7** IMU + EKF 融合
- [ ] **M8** Lifecycle 节点
- [ ] **M9** ros2_control 重做底盘
- [ ] **M10** 传感器节点（超声波/循迹/相机）
- [ ] **M11** behavior_manager 状态机
- [ ] **M12** health_monitor + 故障注入
- [ ] **M13** Gazebo 仿真
- [ ] **M14** SLAM + Nav2 导航
- [ ] **M15** Launch 编排 + 系统管理
- [ ] **M16** Component + Executor + 实时性
- [ ] **M17** 调试工具链 + bag
- [ ] **M18** 工程化 + 测试 + CI
- [ ] **M19** 收尾 + 文档

## 许可

Apache-2.0
