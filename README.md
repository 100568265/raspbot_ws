# Raspbot ROS2 自主移动平台

基于 Raspberry Pi 5 + Yahboom Raspbot V2（麦轮底盘）的项目驱动 ROS2 系统。
从底层硬件驱动到自主导航，按里程碑逐步搭建一个分层的移动机器人系统。

- **平台**：Raspberry Pi 5 + Raspbot V2（4×TT 电机麦轮、驱动板 I2C@0x2B、超声波、4 路循迹、2DOF 摄像头云台）
- **系统**：Ubuntu 24.04 + ROS2 Jazzy
- **语言**：C++（rclcpp）

## 包结构

| 包 | 作用 |
|---|---|
| `raspbot_interfaces` | 自定义消息/服务/动作定义（`.msg` / `.srv` / `.action`） |
| `raspbot_demo` | 学习与验证用节点（发布订阅、QoS 实验等） |

### 已定义接口

- `raspbot_interfaces/msg/WheelSpeed` —— 四轮速度（含 `std_msgs/Header` 时间戳与 frame_id）

## 构建

```bash
cd ~/raspbot_ws
colcon build
source install/setup.bash
```

> 工作空间已配置 `colcon_defaults.yaml`（默认开启 `symlink-install` 与 `compile_commands.json` 导出，供 clangd 使用）。

## 运行（当前可用）

四轮速度发布/订阅 demo：

```bash
# 终端 1：发布者（每 500ms 发一帧 WheelSpeed）
ros2 run raspbot_demo wheel_pub

# 终端 2：订阅者
ros2 run raspbot_demo wheel_sub

# 命令行直接观测话题
ros2 topic echo /wheel_speed
ros2 topic info /wheel_speed --verbose
```

## 进度

- [x] **M0** 工作空间 + 工程底盘（colcon / ament / overlay / clangd）
- [x] **M1** 发布订阅 + 自定义消息（WheelSpeed，含时间戳纪律）
- [x] **M2** QoS 与 DDS 深化
- [ ] **M3** Service + Action
- [ ] **M4** C++ HAL 移植 + chassis_driver（I2C 0x2B 底盘驱动）
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
