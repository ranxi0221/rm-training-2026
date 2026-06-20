# 2026 RoboMaster 培训记录

## 仓库简介

本仓库用于记录本人参加 2026 RoboMaster 培训期间的学习内容、代码示例及项目进展。
当前主要目录结构：

- `W1/`：第一周任务（环境搭建、Git、C++ 基础、命令行小程序雏形）
- `W2/`：第二周内容（ROS2环境、基础通信、launch、bag、service）
- `W3/`：第三周内容（OpenCV 装甲板离线识别与视频处理）
- `W5/`：第五周保姆级教学（工程化整理详解）
- `project/armor_detector/`：装甲板检测项目（W4-W7 持续迭代）
 所有代码在 Ubuntu 22.04 下使用 VSCode + g++/colcon/CMake 编译运行。

## 仓库说明
本仓库用于记录RM培训期间的学习内容和项目代码。

## 目录结构

## 学习进度

### 第一周 (W1)
- [x] 搭建开发环境
- [x] 注册GitHub账号
- [x] 配置Git用户名和邮箱
- [x] 创建公开仓库
- [x] C++基础学习
- [x] 命令行小程序
#### 本周完成内容
     基础开发环境：
     安装 VSCode、C/C++ 插件、build-essential、CMake、Git。
     终端验证 g++、cmake、git 可用。
     编写 hello world 并成功编译运行。

Git 与 GitHub：
     注册 GitHub 账号（用户名 ranxi0221）。
     配置本机用户名与邮箱。
     创建公开仓库 rm-training-2026。
     维护根目录 README.md，记录每周学习过程。
     预留目录 W1/、W2/、W3/ 及 project/armor_detector/。

 C++ 基础学习
- 学习函数定义、调用、值传递与引用传递、默认参数、函数重载。
- 学习引用作为函数参数修改变量。
- 学习简单类的定义与成员函数。
- 学习 `vector`、`string`、`map` 的基本操作。
- 了解头文件（`.h`）与源文件（`.cpp`）拆分的方法。
- 掌握用 CMake 编译小工程（`split_demo` 示例）。
- 编写了控制流（if/switch/for/while/do-while/break/continue/goto）的交互式示例。

 命令行小程序（必做+进阶）
- 实现读取命令行参数的问候程序，支持多个参数依次问候。
- 支持 `-h` 输出帮助信息。
- 添加了缺省提示（无参数时提示用法）。
- 编写了最小 `CMakeLists.txt`，使用 `build/` 目录完成 `cmake + make` 编译。
- 将程序拆分为 `src/` 和 `include/`（`greet.h`、`greet.cpp`、`main.cpp`）。
- 在 `CMakeLists.txt` 中添加了 `-Wall -Wextra` 编译警告选项。
- 完成了 Git 分支操作：新建分支 `feature-greet-update`，修改文件后合并回 `main` 分支。

#### 本周提交的代码

    W1/cpp_examples/hello.cpp – 第一个 C++ 程序

    W1/cpp_examples/function_tutorial.cpp – 函数完整示例（值传递、引用、重载、默认参数）

    W1/cpp_examples/reference_tutorial.cpp – 引用作为参数交换变量

    W1/cpp_examples/class_tutorial.cpp – 简单类的定义和使用

    W1/cpp_examples/stl_tutorial.cpp – vector、string、map 基本操作
    
    W1/screenshots/ – 环境确认及运行截图（g++/cmake/git 版本、hello world 运行）

    W1/cpp_examples/switch_examples.cpp(int类型与char类型S) 
    W1/split_demo/`：头文件与源文件拆分 + CMake 示例。
-   W1/greet_project/：命令行小程序的完整代码（拆分版）及 `CMakeLists.txt`。


### 遇到的问题和解决过程

#### 问题1：Git push 时提示密码错误
问题描述：使用GitHub密码推送代码时提示"Password authentication is not supported"

解决方案：改用Personal Access Token。在GitHub Settings → Developer settings → Personal access tokens → Generate new token，勾选repo权限，用token代替密码推送。

#### 问题2：本地文件夹名和远程仓库名不一致
问题描述：本地是rm-training-2025，远程是rm-training-2026

解决方案：使用`mv`命令重命名本地文件夹，保持名称一致。

#### 问题3: 编译运行时在终端里显示没有目录文件
解决方案：重新命名文件进行核查

#### 问题4: 编写程序时多次字母拼错
解决方案：仔细编写多次检查

#### 问题5：运行程序是没有注意空格
解决方案./jump_examples(./后面不要空格S)

#### 问题 6：GitHub 网络不稳定，经常无法访问或 push 失败
尝试方案：配置 SSH 密钥、安装 Watt Toolkit（Steam++）加速器。
最终解决：对于必须上传的文件，改用网页分批上传；Git 命令行暂未能稳定使用，后续继续排查网络原因。
#### 问题 7：VSCode IntelliSense 找不到头文件，显示红色波浪线
- 原因：编辑器默认不包含项目自定义的 `include` 目录。  
- 解决：按 `Ctrl+Shift+P` → `C/C++: Edit Configurations (UI)` → 在 `Include path` 中添加 `${workspaceFolder}/W1/greet_project/include`（不影响实际编译）。

#### 问题 8：CMake 编译时因 `CMakeLists.txt` 拼写错误失败
- 现象：`cmake_minimum_required` 误写为 `cmake_minimum_reguired` 或 `VERDION`。  
- 解决：仔细核对关键字，确保拼写正确。

#### 问题 9：VSCode 终端中 `cd` 进 `build` 目录后执行 `cmake ..` 提示找不到 `CMakeLists.txt`
- 原因：在错误的目录（如 `rm-training-2026/build`）执行了命令。  
- 解决：确保在包含 `CMakeLists.txt` 的项目根目录（如 `greet_project`）下创建 `build` 子目录，再执行 `cmake ..`。

#### 问题 10：Git 分支操作中找不到 `从分支合并...` 选项
- 解决：使用命令面板（`Ctrl+Shift+P`）输入 `Git: 合并分支` 完成合并。

#### 尚未解决的问题
  
  命令行 `git push` 因 `gnutls_handshake` 错误仍无法直接推送，依赖网页上传或加速器（有时有效）。  
- CMake 的进阶用法（如条件编译、查找库）尚未深入。  
- 类的继承与多态还未开始学习。  
- 命令行小程序的分支合并 在 VSCode 图形界面中操作不够流畅，最终通过终端命令完成
- 一直在利用deepseek一步步指令完成任务，技术还不成熟

## 第二周

### 本周任务清单

[x]安装 ROS2 Humble 桌面版
[x]跑通 turtlesim（小海龟窗口出现，键盘控制移动）
[x]确认新终端里能正常使用 ros2 命令
[x]用 turtlesim 观察现成节点和话题
[x]学会 ros2 topic list / echo / info
[X]在 README.md 里写清 workspace / package / node / topic 分别是什么
[X]创建一个 C++ 包 training_pkg
[x]写 talker 和 listener 两个节点
[x]colcon build 后能正常运行并持续打印
[X]写一个 launch.py 同时启动两个节点
[x]至少给 talker 加一个参数
[x]能分别用命令行和 launch 跑通参数
[x]学会 ros2 bag record / play / info
[X]能用这些命令查看自己写的 talker / listener
[x]给参数补默认值和范围说明
[x]把参数写进单独的 YAML 再由 launch 加载
[x]录一个短 bag，重新开新终端只靠 bag play 复现一次
[x]再加一个最小节点，订阅你自己的话题并打印频率
[x]补一个 service 或自定义消息的小尝试


### 概念记录
- Node（节点）：一个可执行进程。例如 `/turtlesim` 节点负责显示海龟，`/teleop_turtle` 节点读取键盘输入。
- Topic（话题）：节点间通信的通道。例如 `/turtle1/cmd_vel` 传输速度指令，`/turtle1/pose` 发布位置信息。
    操作命令
- `ros2 topic list`：列出所有活跃话题。
- `ros2 topic info <topic>`：显示话题的消息类型、发布者/订阅者数量。
- `ros2 topic echo <topic>`：打印话题上的实时消息。
#### 示例
- 查看话题列表：`ros2 topic list`
- 查看 `/turtle1/cmd_vel` 信息：`ros2 topic info /turtle1/cmd_vel`
- 监听海龟位姿：`ros2 topic echo /turtle1/pose`
- Workspace（工作空间）：*Workspace（工作空间）**：一个目录，用于组织 ROS2 项目。通常包含 `src`（源码）、`build`（编译中间文件）、`install`（可执行文件）、`log`（日志）。例如创建的 `~/ros2_ws`
- Package（功能包）：：ROS2 软件的最小单元，包含节点、配置文件、消息定义等。例如 `turtlesim` 是一个包，提供了小海龟仿真。
使用 ros2 bag record /chatter -o my_bag 录制了自己的 talker 发布的话题。
使用 ros2 bag info my_bag 查看 bag 信息。
使用 ros2 bag play my_bag 回放，listener 重新接收历史消息。
 基础通信 
   - 创建 C++ 包 `training_pkg`，编写 `talker`（发布者）和 `listener`（订阅者）节点。  
   - 使用 `colcon build` 编译成功，两个节点能同时运行并持续打印消息。

launch 与参数  
   - 编写 `talker_listener.launch.py` 同时启动两个节点。  
   - 为 `talker` 添加参数 `talker_period_ms`，能分别通过命令行和 launch 文件传参（例如 `--ros-args -p talker_period_ms:=2000` 和 `ros2 launch ... talker_period_ms:=1500`）。  
   - 参数添加默认值和范围说明（100ms~5000ms，超出自动裁剪并警告）。  
   - 将参数写入单独的 YAML 文件 `config/talker_params.yaml`，由 launch 加载。

bag 调试 
   - 使用 `ros2 bag record /chatter -o my_bag` 录制自己的 talker 发布的话题。  
   - 使用 `ros2 bag info my_bag` 查看 bag 信息。  
   - 使用 `ros2 bag play my_bag` 回放，listener 重新接收历史消息（复现实验）。
   - 参数默认值和范围：在 `talker.cpp` 中添加范围检查，超出阈值时打印警告并自动调整。  
   - YAML 参数文件：创建 `config/talker_params.yaml`，通过 launch 加载。  
   - 额外节点：添加 `freq_printer` 节点，订阅 `/chatter` 话题，每秒打印接收频率。  
   - service 调用：使用内置服务类型 `example_interfaces/srv/AddTwoInts`，实现加法服务端 `add_server`，客户端调用成功。

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


## 第二周 OpenCV 装甲板识别
### 本周任务清单

[x] 离线识别：代码、red_mask、blue_mask、灯条候选结果图、装甲板几何框结果图；README 写清识别流程、颜色分离方法、关键参数、灯条筛选条件、配对条件和最终框的来源。

[x] 连续素材验证与代码整理：代码、全部 6 个视频的处理说明、输出视频；README 写清如何整理成后续可接进 ROS2 的结构。

[x] 失败样例分析：至少 2-3 张误检或漏检样例；README 写清问题位置和下周排查重点。

## 本周完成内容
### 离线识别

    实现了完整的装甲板检测流程，能够输出：

        red_mask.png / blue_mask.png（颜色分离后的二值掩膜）

        light_candidates.png（筛选后的灯条候选框）

        armor_result.png（最终装甲板几何框及颜色标签）

    蓝色装甲板识别了上下两条灯条，红色装甲板识别稳定，red_mask里面有识别其他区域的红色物体但最终装甲板几何框和颜色标签正确

### 连续素材验证与代码整理

    将检测流程封装为 processFrame() 函数，逐帧处理视频。

    处理了 Blue_1000/3000/5000.mp4 和 Red_1000/3000/5000.mp4 共 6 个视频，输出带装甲板框的视频文件。

### 代码结构（可接入 ROS2）

    所有检测逻辑独立成函数：colorSeparation, morphologyProcess, isLightBar, pairLightBars, getArmorPoints。

    processFrame(frame, output) 可直接被 ROS2 图像回调调用，输出画框图像。

    视频处理程序 video_processor.cpp 演示了逐帧调用方式。

### 本周提交的代码和结果

    离线识别代码：W3/armor_detector/src/armor_detector.cpp

    视频处理代码：W3/armor_detector/src/video_processor.cpp

    中间结果截图：W3/screenshots/red_mask.png, blue_mask.png, light_candidates.png, armor_result.png

    输出视频：output_Blue_1000.mp4, output_Blue_3000.mp4, output_Blue_5000.mp4, output_Red_1000.mp4, output_Red_3000.mp4, output_Red_5000.mp4

## 识别流程、参数与条件
### 颜色分离方法（HSV）

    红色：H 0‑80 和 150‑180，S 0‑200，V 50‑255（为捕获过亮荧光灯条降低 V 下限）

    蓝色：H 80‑115，S 30‑200，V 150‑255（稳定参数，未改动）

### 形态学处理

    内核：椭圆，尺寸 9×9

    两次闭运算（填充灯条断裂） + 一次开运算（去除小噪点）

### 灯条筛选条件

    面积：20 ≤ 面积 ≤ 300（针对小面积荧光灯条）

    长宽比：1.0 ≤ 高度/宽度 ≤ 15.0

    角度：与竖直方向夹角 ≤ 60°

### 灯条配对条件

    垂直偏差 dy ≤ 80 像素

    水平距离 dx ≥ 15 像素

    高度相似度：较小高度/较大高度 ≥ 0.3

### 最终装甲板框来源

从左右两个灯条的最小外接矩形顶点，经排序得到左上、右上、右下、左下四个有序点，使用 polylines 绘制四边形，并用 putText 标注颜色标签。

## 主要问题及解决过程
### 问题1：蓝色灯条大面积误检（地板也被识别）

    现象：blue_mask 中地板出现大片白色区域。

    解决：提高蓝色 S 下限（30）和 V 下限（150），缩窄 H 范围（80-115），并增加形态学闭运算。最终蓝色稳定。

### 问题2：红色灯条过亮、饱和度极低（几乎呈白色），难以提取

    现象：采样点 HSV 为 H=6‑30，S=7‑34，V=255。常规红色阈值无法提取。

    解决：将红色 V 下限降至 50，S 下限设为 0，H 范围扩大到 0‑80 和 150‑180；灯条筛选面积下限降至 20。成功提取一侧灯条，另一侧始终无法同时出现（推测因灯条断裂或面积过小）。

### 问题3：红色误检（图中两条暗红色长条被识别为灯条）

    现象：red_mask 中出现大面积暗红色区域（H=164，S=166，V=103）。

    解决：后处理中只保留面积 < 500 的候选，剔除大块误检。暗红色长条被成功过滤。

### 问题4：视频文件路径含中文导致 OpenCV 无法打开

    现象：VideoCapture 失败，但 VLC 可以播放。

    解决：将视频文件夹“测试视频”重命名为英文 raw，并修改代码中的路径。重新编译后所有视频成功处理。

### 尚未解决的问题

    蓝色识别还是不到位

    红色误检仍存在于 red_mask 中（虽然被面积过滤去除框），颜色分离不够纯净。

    视频处理帧率较低，未做优化，影响实时性。

    图片识别依旧有许多问题进阶项未能完成


## 下周排查重点

    改进红色灯条提取：尝试 Lab 色彩空间或边缘检测辅助定位。

    处理灯条断裂：增大闭运算内核（如 15×15）或使用膨胀操作。

    颜色分离自适应：对高亮区域动态调整 HSV 阈值。

    增加跟踪：利用前一帧检测结果预测当前帧位置。

    封装 ROS2 节点：将 processFrame 接入 ROS2 图像订阅回调，发布装甲板位置信息。

## 第四周：装甲板检测项目（armor_detector）

详细文档：[project/armor_detector/README.md](project/armor_detector/README.md) — 包含完整参数速查表、W3→W4 迁移记录、调试建议等。

### 项目功能

基于 OpenCV + ROS2 的实时装甲板识别模块。从工业相机获取图像，通过 HSV 颜色分离、形态学处理、轮廓筛选和几何配对，实时检测并定位装甲板，发布检测结果和调试图像。

核心流程： 图像采集 → HSV 颜色分离（红/蓝）→ 形态学处理 → 灯条轮廓筛选 → 灯条配对 → 装甲板四角计算 → EMA 时序平滑 → 结果发布

### 依赖

| 类型 | 依赖项 |
|------|--------|
| 系统 | Ubuntu 22.04, ROS2 Humble, GCC 11+, CMake 3.22+, colcon |
| ROS2 包 | `rclcpp`, `sensor_msgs`, `std_msgs`, `cv_bridge`, `OpenCV` 4.x |
| 第三方 | MindVision SDK `libMVSDK.so`（仅相机驱动需要，检测模块本身不依赖） |

### 编译方式

```bash
cd ~/ros2_ws
colcon build --packages-select armor_detector
source install/setup.bash
```

如需同时编译相机驱动：`colcon build --packages-select mindvision_camera armor_detector`

### 运行方式

```bash
# 一键启动 — 真实相机模式（默认）
ros2 launch armor_detector armor.launch.py target_color:=red

# 一键启动 — Bag 回放模式（不需要相机）
ros2 launch armor_detector armor.launch.py input_source:=bag bag_path:=/path/to/test.bag

# 分步调试
ros2 run mindvision_camera mindvision_camera_node                          # 终端1：相机
ros2 run armor_detector armor_detector_node --ros-args -p target_color:=red  # 终端2：检测
```

### 输入源

通过 `input_source` 参数一键切换：

| 模式 | 参数 | 需要硬件 | 适用场景 |
|------|------|:--:|------|
| 真实相机 | `input_source:=camera` | | 比赛/联调 |
| Bag 回放 | `input_source:=bag` |  | 离线复现、算法调试 |

| 项目 | 值 |
|------|-----|
| 输入话题 | `/image_raw`（可在 launch 中覆盖） |
| 消息类型 | `sensor_msgs/Image` |
| 图像格式 | BGR8, 1280 × 1024, ~15 FPS |

### 输出话题或结果

| 话题 | 消息类型 | 说明 |
|------|----------|------|
| `/armor_result` | `std_msgs/String` | 检测结果：`armor_detected color=red center=(x,y)` 或 `no_armor` |
| `/armor_debug_image` | `sensor_msgs/Image` | 带灯条框和装甲板框标注的调试图像 |

### 参数入口

配置文件： `project/armor_detector/config/armor_params.yaml`

所有检测参数（HSV 阈值、形态学核大小、灯条筛选条件、配对条件、平滑系数等）集中在此 YAML 文件中，支持：
- launch 传参覆盖：`ros2 launch armor_detector armor.launch.py target_color:=blue`
- 运行时动态修改：`ros2 param set /armor_detector_node red1_s_low 50`

> 完整参数表见 [project/armor_detector/README.md]

### 当前已知局限

- 光照适应性差：HSV 阈值针对实验室恒定光照调优，赛场复杂光照下识别率下降
- 仅支持单目标：当前版本只检测一个装甲板，不支持多目标同时检测
- 无 GPU 加速：纯 CPU 运算，嵌入式设备上可能达到性能瓶颈
- 无运动预测：未集成卡尔曼滤波或光流跟踪，目标短暂丢失后无法预测位置
- 相机连接偶发中断：MindVision SDK 偶发 `user control fd open failed`，需重启节点
- 未集成数字识别：仅检测装甲板位置，未识别装甲板上的数字

---

## 第五周：工程化整理

### 本周任务

- [x] 代码结构审查：确认检测逻辑与节点逻辑分离
- [x] launch 完善：新增 `input_source` 参数支持 camera/bag 一键切换
- [x] 参数补全：YAML 配置文件已覆盖全部 30+ 算法参数，新增 launch 层参数
- [x] 保留固定 bag 用于复现：`project/armor_detector/bags/armor_test`（本地，不入 git）
- [x] 运行截图：`project/assets/armor_debug_screenshot.png`（blue armor detected）

### 本周完成内容

#### 代码结构确认

W4 代码已满足工程化要求，检测逻辑 `ArmorDetector` 类（输入 `cv::Mat` → 输出 `ArmorPlate`）与 ROS2 节点完全解耦。同一套检测逻辑可被离线图片、视频、bag 和真实相机复用。

#### launch 完善

```
W4: launch 强制启动相机 → 无相机报错
W5: 新增 input_source 参数 → camera 模式启动相机，bag 模式自动播放 bag
```

新增 launch 参数：`input_source`（camera/bag）、`bag_path`、`enable_debug`

#### 参数补全

W4 YAML 已包含全部算法参数（预处理、红/蓝 HSV、形态学、灯条筛选、配对、平滑），W5 补全了 launch 层参数。

#### 相机调试记录

MindVision SDK 报 `user control fd open failed` 但不影响图像采集（系 SDK 内部辅助功能报错，主图像流正常）。排查过程中发现：
需先加载 ROS2 环境再启动
相机枚举和初始化正常：`MV-SUA133GC sn=041062620083 port=USB3.0`

#### Bag 复现

本地 bag 路径：`project/armor_detector/bags/armor_test`（3 秒，含 /image_raw + /armor_result + /armor_debug_image）
详细介绍在 [project/armor_detector/README.md]

## 第六周：配对问题定向改进 + 多装甲板支持

详细记录：[project/armor_detector/README.md#w6-改进记录](project/armor_detector/README.md#w6-改进记录2026-06-10)

### 选方向：配对问题
灯条能检测到但黄框不稳定（倾斜变三角形、长方形掉框、多装甲板只框一个）。

### 改动内容
- 修复 `matchArmorPlate` 角点提取 bug（右灯条错取左边缘 → 每灯条独立取极值）
- 红蓝按颜色分组配对，不再混排漏配
- `detect()` 返回多装甲板，同画面同时框红+蓝
- 配对参数放宽：`pair_max_dy: 350`, `pair_min_dx: 5`, `pair_max_dx: 2500`

### 效果
倾斜 30° 黄框稳定、长方形装甲板不掉、红蓝同画面各出一个框。

### 未解决
亮环境光照适应、倾斜 > 30°、同色多目标。

## 第七周：数字识别模型接入 + 误检定向解决

详细记录：[project/armor_detector/README.md#w7-改进记录](project/armor_detector/README.md#w7-改进记录2026-06-19)

### 完成内容
- 接入 `tiny_resnet.onnx` 数字分类模型（OpenCV DNN），32×32 灰度图输入，9 类输出
- 装甲板四角 ROI 提取：长方形 60%/正方形 35% 中心裁切
- **BGR 颜色纯度过滤**（核心改进）：蓝 B>R×1.7 且 B>G×1.7，红同理。从源头过滤白色/灰色误检
- 翻转文字标签标注数字类别（`blue_1`、`red_3`）
- 灯条尺寸相似性配对（高度差 < 2.5×、宽度差 < 3×）
- 帧间数字记忆保留 30 帧

### 效果
数字识别稳定、误检大幅减少（BGR 纯度过滤）、两种装甲板均能识别。

### 未解决
亮环境误检残留、正方形装甲板图标干扰、数字偶尔闪烁。
