## 第二周

### 本周任务清单

[x]安装 ROS2 Humble 桌面版
[x]跑通 turtlesim（小海龟窗口出现，键盘控制移动）
[x]确认新终端里能正常使用 ros2 命令
[x]用 turtlesim 观察现成节点和话题
[x]学会 ros2 topic list / echo / info
[x]在 README.md 里写清 workspace / package / node / topic 分别是什么
[x]创建一个 C++ 包 training_pkg
[x]写 talker 和 listener 两个节点
[x]colcon build 后能正常运行并持续打印
[x]写一个 launch.py 同时启动两个节点
[x]至少给 talker 加一个参数
[x]能分别用命令行和 launch 跑通参数
[x]学会 ros2 bag record / play / info
[x]能用这些命令查看自己写的 talker / listener
[x]给参数补默认值和范围说明
[x]把参数写进单独的 YAML 再由 launch 加载
[x]录一个短 bag，重新开新终端只靠 bag play 复现一次
[x]再加一个最小节点，订阅你自己的话题并打印频率
[x]补一个 service 或自定义消息的小尝试


### 概念记录
- **Node（节点）**：一个可执行进程。例如 `/turtlesim` 节点负责显示海龟，`/teleop_turtle` 节点读取键盘输入。
- **Topic（话题）**：节点间通信的通道。例如 `/turtle1/cmd_vel` 传输速度指令，`/turtle1/pose` 发布位置信息。
    操作命令
- `ros2 topic list`：列出所有活跃话题。
- `ros2 topic info <topic>`：显示话题的消息类型、发布者/订阅者数量。
- `ros2 topic echo <topic>`：打印话题上的实时消息。
#### 示例
- 查看话题列表：`ros2 topic list`
- 查看 `/turtle1/cmd_vel` 信息：`ros2 topic info /turtle1/cmd_vel`
- 监听海龟位姿：`ros2 topic echo /turtle1/pose`
- **Workspace（工作空间）**：*Workspace（工作空间）**：一个目录，用于组织 ROS2 项目。通常包含 `src`（源码）、`build`（编译中间文件）、`install`（可执行文件）、`log`（日志）。例如创建的 `~/ros2_ws`
- **Package（功能包）**：：ROS2 软件的最小单元，包含节点、配置文件、消息定义等。例如 `turtlesim` 是一个包，提供了小海龟仿真。
使用 ros2 bag record /chatter -o my_bag 录制了自己的 talker 发布的话题。
使用 ros2 bag info my_bag 查看 bag 信息。
使用 ros2 bag play my_bag 回放，listener 重新接收历史消息。
 **基础通信**  
   - 创建 C++ 包 `training_pkg`，编写 `talker`（发布者）和 `listener`（订阅者）节点。  
   - 使用 `colcon build` 编译成功，两个节点能同时运行并持续打印消息。

**launch 与参数**  
   - 编写 `talker_listener.launch.py` 同时启动两个节点。  
   - 为 `talker` 添加参数 `talker_period_ms`，能分别通过命令行和 launch 文件传参（例如 `--ros-args -p talker_period_ms:=2000` 和 `ros2 launch ... talker_period_ms:=1500`）。  
   - 参数添加默认值和范围说明（100ms~5000ms，超出自动裁剪并警告）。  
   - 将参数写入单独的 YAML 文件 `config/talker_params.yaml`，由 launch 加载。

**bag 调试**  
   - 使用 `ros2 bag record /chatter -o my_bag` 录制自己的 talker 发布的话题。  
   - 使用 `ros2 bag info my_bag` 查看 bag 信息。  
   - 使用 `ros2 bag play my_bag` 回放，listener 重新接收历史消息（复现实验）。
   - **参数默认值和范围**：在 `talker.cpp` 中添加范围检查，超出阈值时打印警告并自动调整。  
   - **YAML 参数文件**：创建 `config/talker_params.yaml`，通过 launch 加载。  
   - **额外节点**：添加 `freq_printer` 节点，订阅 `/chatter` 话题，每秒打印接收频率。  
   - **service 调用**：使用内置服务类型 `example_interfaces/srv/AddTwoInts`，实现加法服务端 `add_server`，客户端调用成功。

### 操作命令汇总

#### 基本概念查看
```bash
ros2 node list
ros2 topic list
ros2 topic info /turtle1/cmd_vel
ros2 topic echo /turtle1/pose
 
talker / listener 编译及运行
                    ~/ros2_ws
                     colcon build --packages-select training_pkg
                     source install/setup.bash
                     ros2 run training_pkg talker
                     ros2 run training_pkg listener
命令行传参
        bash
        ros2 run training_pkg talker --ros-args -p talker_period_ms:=2000

launch 传参
        bash
        ros2 launch training_pkg talker_listener.launch.py talker_period_ms:=1500

YAML 参数 launch
      bash
      ros2 launch training_pkg talker_listener.launch.py

bag 录制与回放
    bash
    ros2 bag record /chatter -o my_bag
    ros2 bag info my_bag
    ros2 bag play my_bag

频率节点
    bash
    ros2 run training_pkg freq_printer

service 服务端与客户端
bash
# 终端1
ros2 run training_pkg add_server
# 终端2
ros2 service call /add_two_ints example_interfaces/srv/AddTwoInts "{a: 8, b: 5}"

#### 问题1：ROS2 安装时 curl 下载公钥失败（Failed to connect to raw.githubusercontent.com）
现象：执行 sudo curl ... -o /usr/share/keyrings/ros-archive-keyring.gpg 报错 连接被拒绝。
解决过程：
       手动修改 /etc/hosts，添加 185.199.108.133 raw.githubusercontent.com 解析。仍然失败后，改用浏览器手动下载 ros.key 文件，再用 sudo cp 放到 /usr/share/keyrings/。但 apt update 仍报 NO_PUBKEY F42ED6FBAB17C654。 最终用 sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-key F42ED6FBAB17C654 导入密钥，并修改 /etc/apt/sources.list.d/ros2.list 去掉 signed-by 字段，使用传统仓库配置。之后 sudo apt update 正常，成功安装 ros-humble-desktop

#### 问题2：小海龟动不了（键盘控制无反应）
现象：ros2 run turtlesim turtle_teleop_key 显示 “Reading from keyboard...”，但按方向键小海龟不移动。
解决过程：
     检查终端焦点：鼠标单击键盘控制终端窗口，确保它处于激活状态。用 ros2 topic echo /turtle1/cmd_vel 测试，发现按方向键时该话题有数据输出，说明键盘节点正常。重启 turtlesim_node 和 turtle_teleop_key 后问题依旧。最终重新安装 ros-humble-turtlesim 包：sudo apt install --reinstall ros-humble-turtlesim，之后小海龟恢复正常移动。  

 ####问题3：colcon build 时找不到 rosidl_generate_interfaces 或 example_interfaces
    现象：编译失败，提示缺少依赖或 CMake 语法错误。
    解决：在 CMakeLists.txt 中正确添加 find_package(example_interfaces REQUIRED)，并删除自定义接口相关行（当使用内置服务时）。确保 ament_target_dependencies(add_server rclcpp example_interfaces)
