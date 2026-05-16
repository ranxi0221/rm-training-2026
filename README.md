# 2026 RoboMaster 培训记录

## 仓库简介

本仓库用于记录本人参加 2026 RoboMaster 培训期间的学习内容、代码示例及项目进展。
当前主要目录结构：

- `W1/`：第一周任务（环境搭建、Git、C++ 基础、命令行小程序雏形）
- `W2/`：第二周内容（待填充）
- `W3/`：第三周内容（待填充）
- `project/armor_detector/`：最终装甲板检测项目（待完成）
所有代码均在 **Ubuntu 22.04** 环境下使用 **VSCode + g++** 编译运行，并已通过 `git` 与 GitHub 仓库关联。

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
- **原因**：编辑器默认不包含项目自定义的 `include` 目录。  
- **解决**：按 `Ctrl+Shift+P` → `C/C++: Edit Configurations (UI)` → 在 `Include path` 中添加 `${workspaceFolder}/W1/greet_project/include`（不影响实际编译）。

#### 问题 8：CMake 编译时因 `CMakeLists.txt` 拼写错误失败
- **现象**：`cmake_minimum_required` 误写为 `cmake_minimum_reguired` 或 `VERDION`。  
- **解决**：仔细核对关键字，确保拼写正确。

#### 问题 9：VSCode 终端中 `cd` 进 `build` 目录后执行 `cmake ..` 提示找不到 `CMakeLists.txt`
- **原因**：在错误的目录（如 `rm-training-2026/build`）执行了命令。  
- **解决**：确保在包含 `CMakeLists.txt` 的项目根目录（如 `greet_project`）下创建 `build` 子目录，再执行 `cmake ..`。

#### 问题 10：Git 分支操作中找不到 `从分支合并...` 选项
- **解决**：使用命令面板（`Ctrl+Shift+P`）输入 `Git: 合并分支` 完成合并。

#### 尚未解决的问题
  
  命令行 `git push` 因 `gnutls_handshake` 错误仍无法直接推送，依赖网页上传或加速器（有时有效）。  
- CMake 的进阶用法（如条件编译、查找库）尚未深入。  
- 类的继承与多态还未开始学习。  
- 命令行小程序的分支合并 在 VSCode 图形界面中操作不够流畅，最终通过终端命令完成
- 一直在利用deepseek一步步指令完成任务，技术还不成熟

## 第二周

### 本周任务清单

- [x]安装 ROS2 Humble 桌面版
- [x]跑通 turtlesim（小海龟窗口出现，键盘控制移动）
- [x]确认新终端里能正常使用 ros2 命令
- [x]用 turtlesim 观察现成节点和话题
- [x]学会 ros2 topic list / echo / info
- [x]在 README.md 里写清 workspace / package / node / topic 分别是什么
- [x]创建一个 C++ 包 training_pkg
- [x]写 talker 和 listener 两个节点
- [x]colcon build 后能正常运行并持续打印
- [x]写一个 launch.py 同时启动两个节点
- [x]至少给 talker 加一个参数
- [x]能分别用命令行和 launch 跑通参数
- [x]学会 ros2 bag record / play / info
- [x]能用这些命令查看自己写的 talker / listener
- [x]给参数补默认值和范围说明
- [x]把参数写进单独的 YAML 再由 launch 加载
- [x]录一个短 bag，重新开新终端只靠 bag play 复现一次
- [x]再加一个最小节点，订阅你自己的话题并打印频率
- [x]补一个 service 或自定义消息的小尝试


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
