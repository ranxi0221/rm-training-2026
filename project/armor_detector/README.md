# Armor Detector — ROS2 装甲板检测

## 项目功能

基于 OpenCV + ROS2 的机器人装甲板识别模块。从工业相机获取图像，通过 HSV 颜色分离、形态学处理、轮廓筛选和几何配对，实时检测并定位装甲板。

**核心流程：** 图像采集 → HSV 颜色分离 → 形态学处理 → 灯条轮廓提取/筛选 → 灯条配对 → 装甲板四角计算 → EMA 时序平滑 → 结果发布

**支持特性：**
- 红/蓝双色灯条检测（`target_color: red / blue / both`）
- CLAHE 局部对比度增强（适应过曝/过暗场景）
- EMA 指数平滑（减少帧间抖动）
- 调试图像发布（灯条框 + 装甲板框叠加）

## 依赖

### 系统环境
- Ubuntu 22.04 LTS
- ROS2 Humble
- GCC 11+ / CMake 3.22+
- colcon 构建系统

### ROS2 包依赖（package.xml）
- `rclcpp` — ROS2 C++ 客户端库
- `sensor_msgs` — 图像消息类型
- `std_msgs` — 标准消息类型
- `cv_bridge` — ROS ↔ OpenCV 图像桥接
- `OpenCV` 4.x — 计算机视觉库

### 第三方库
- MindVision SDK `libMVSDK.so`（仅相机驱动 `mindvision_camera` 需要，检测模块本身不依赖）

## 编译方式

```bash
# 1. 确保 ROS2 Humble 环境已加载
source /opt/ros/humble/setup.bash

# 2. 进入工作空间根目录
cd ~/ros2_ws

# 3. 编译装甲板检测包
colcon build --packages-select armor_detector

# 4. 如需同时编译相机驱动
colcon build --packages-select mindvision_camera armor_detector

# 5. 加载编译产物
source install/setup.bash
```

**常见问题：**
- 链接错误 → 确认 `pkg-config --modversion opencv4` 返回 4.5+
- 找不到 mindvision_camera → 确认相机驱动包已放入 `src/` 并同时编译

## 运行方式

### 一键启动（推荐）

```bash
# 真实相机模式（默认）
ros2 launch armor_detector armor.launch.py target_color:=red

# Bag 回放模式（不需要相机，bag 本地路径：bags/armor_test）
ros2 launch armor_detector armor.launch.py input_source:=bag bag_path:=bags/armor_test
```

### 输入源切换

通过 `input_source` 参数一键切换：

| 模式 | 命令 | 说明 |
|------|------|------|
| **真实相机** | `input_source:=camera`（默认） | 启动 `mindvision_camera` 驱动节点 + 检测节点 |
| **Bag 回放** | `input_source:=bag` | 仅启动检测节点，自动循环播放指定 bag 文件 |

```bash
# 示例1：用真实相机检测红色装甲板
ros2 launch armor_detector armor.launch.py input_source:=camera target_color:=red

# 示例2：用 bag 复现蓝色装甲板检测
ros2 launch armor_detector armor.launch.py input_source:=bag bag_path:=~/bags/armor_test
```

### 分步启动（调试用）
```bash
# 终端1：启动相机节点
ros2 run mindvision_camera mindvision_camera_node

# 终端2：启动检测节点
ros2 run armor_detector armor_detector_node --ros-args -p target_color:=red
```

### 无相机时的离线调试
```bash
# 方式1：用 launch 的 bag 模式
ros2 launch armor_detector armor.launch.py input_source:=bag bag_path:=<bag_file>

# 方式2：手动播放 bag + 启动检测节点
ros2 bag play <bag_file> --loop          # 终端1
ros2 run armor_detector armor_detector_node --ros-args -p target_color:=red  # 终端2

# 方式3：用本地图片测试（发布单帧）
ros2 run image_publisher image_publisher_node <image_file>
```

## 输入源

项目支持两种输入源，通过 launch 参数 `input_source` 一键切换：

| 模式 | 输入来源 | 需要硬件 | 适用场景 |
|------|----------|:--:|------|
| **camera** | 迈德威视工业相机（USB3.0） | ✅ | 真实比赛/联调 |
| **bag** | ros2 bag 文件回放 | ❌ | 离线复现、算法调试、无相机环境 |

### 输入规格

| 项目 | 值 |
|------|-----|
| **输入话题** | `/image_raw`（可在 launch 中通过 `image_topic` 覆盖） |
| **消息类型** | `sensor_msgs/Image` |
| **图像格式** | BGR8（8 位三通道彩色） |
| **分辨率** | 1280 × 1024 |
| **帧率** | ~15 FPS（取决于相机曝光设置） |

### 切换方法

```bash
# 切换到真实相机
ros2 launch armor_detector armor.launch.py input_source:=camera

# 切换到 bag 回放（需提前录好 bag 文件）
ros2 launch armor_detector armor.launch.py input_source:=bag bag_path:=/path/to/bag_file
```

> 订阅使用 **SensorDataQoS**，确保图像传输可靠性。若使用其他相机，只需将话题对齐到 `/image_raw`（BGR8 格式）即可。

## 输出话题或结果

| 话题 | 消息类型 | 说明 |
|------|----------|------|
| `/armor_result` | `std_msgs/String` | 检测结果字符串 |
| `/armor_debug_image` | `sensor_msgs/Image` | 带标注的调试图像（灯条框 + 装甲板框） |

### `/armor_result` 格式

```
# 检测到装甲板时：
armor_detected color=red center=(320,240)

# 未检测到时：
no_armor
```

## 参数入口

参数分两层：

### Launch 层参数（launch 传参）

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `input_source` | `camera` | 输入源：`camera`（真实相机）或 `bag`（bag 回放） |
| `target_color` | `red` | 目标颜色：`red` / `blue` / `both` |
| `image_topic` | `/image_raw` | 订阅的图像话题 |
| `bag_path` | 空 | bag 文件路径（`input_source:=bag` 时需指定） |
| `enable_debug` | `true` | 是否发布调试图像 |

### 算法层参数（YAML 配置文件）

**主配置文件：** [`config/armor_params.yaml`](config/armor_params.yaml)

所有检测参数集中在这个 YAML 文件中，由 launch 文件加载，支持运行时覆盖：

```bash
# launch 传参覆盖
ros2 launch armor_detector armor.launch.py target_color:=blue image_topic:=/camera/image

# 运行时动态修改
ros2 param set /armor_detector_node red1_s_low 50

# 查看所有当前参数
ros2 param list /armor_detector_node
```

### 核心参数速查

| 分类 | 参数 | 默认值 | 说明 | 调试建议 |
|------|------|--------|------|----------|
| **通用** | `target_color` | `"red"` | 目标颜色：`red` / `blue` / `both` | 先确认单色能提取到 |
| | `brightness_alpha` | `0.3` | 亮度缩放系数 | 过曝↓（0.2），过暗↑（0.5~1.0） |
| | `enable_clahe` | `true` | CLAHE 对比度增强 | 灯条不明显时开启 |
| | `enable_debug` | `true` | 发布调试图像 | 调试完可关闭节省带宽 |
| **红色 HSV** | `red1_s_low` | `100` | 饱和度下限 | 漏检↓（60），误检↑（150） |
| | `red1_v_low` | `50` | 亮度下限 | 同饱和度思路 |
| **蓝色 HSV** | `blue_h_low` / `blue_h_high` | `95` / `130` | 色相范围 | 检测不到时扩大范围 |
| | `blue_s_low` | `80` | 饱和度下限 | 漏检→降低 |
| **形态学** | `morph_kernel_size` | `5` | 腐蚀膨胀核大小 | 噪声多→增大（7~9） |
| **灯条筛选** | `light_min_area` | `20` | 最小面积（像素²） | 远距离漏检→降至 10 |
| | `light_max_ratio` | `1.0` | 宽高比上限 | 竖条 width<height，ratio<1 保留 |
| **装甲板配对** | `pair_max_dy` | `50` | 配对 Y 偏差上限（像素） | 对不齐时放宽至 80~100 |
| | `pair_max_dx` | `800` | 配对 X 间距上限（像素） | 近距离→增大 |
| | `smooth_alpha` | `0.7` | EMA 平滑系数 | 抖动大→增大（0.8~0.9） |

## 当前已知局限

### 算法局限
- **光照适应性差**：HSV 阈值针对实验室恒定光照调优，赛场复杂光照下识别率下降
- **仅支持单目标**：当前版本检测单个装甲板，无法同时处理多个敌方机器人
- **运动模糊敏感**：高速运动时图像模糊导致灯条检测失败
- **无预测跟踪**：未实现卡尔曼滤波或光流跟踪，目标短暂丢失后无法预测位置
- **颜色分离粗粒度**：仅使用固定 HSV 阈值，未做自适应阈值调整

### 工程局限
- **相机连接不稳定**：MindVision SDK 偶发 `user control fd open failed`，需重启节点恢复
- **无 GPU 加速**：纯 CPU 运算，嵌入式设备上可能达到性能瓶颈
- **无动态参数重配置**：不支持 rqt_reconfigure 在线调参
- **无数字识别**：仅检测装甲板位置，未识别装甲板上的数字
- **多平台支持有限**：相机驱动仅适配 MindVision，海康等相机需另写桥接

### 待解决（优先级排序）
1. 双色灯条自适应切换（红/蓝检测）
2. 添加 ROI 裁剪减少无效计算
3. 集成卡尔曼滤波实现运动预测
4. 多装甲板同时检测支持
5. 动态参数重配置（rqt_reconfigure）
6. 数字识别模块

---

## 附录：W3 → W4 衔接记录

<details>
<summary>从离线识别到 ROS2 节点的迁移过程（点击展开）</summary>

### 代码迁移策略

1. W3 的离线检测逻辑封装到 `armor_detector.cpp` 的 `ArmorDetector` 类中
2. ROS2 节点（`armor_detector_node.cpp`）只负责：订阅图像 → `cv_bridge` 转换 → 调用 `detector_.detect()` → 发布结果
3. 算法细节（HSV 阈值、轮廓筛选、灯条配对）全部在类内部，回调函数不堆代码

### W4 新问题与解决

| 问题 | 原因 | 解决 |
|------|------|------|
| 装甲板框颜色错误 | BGR `(0,0,255)` 被错误解析为蓝色 | 改用黄色 `(0,255,255)` 绘制装甲板框 |
| 装甲板四角不正确 | 旧算法只取前两个点，丢失底部角点 | 按 X 分左右 → 每边按 Y 分上下 → 正确四角 |
| 只识别红色 | `if (color_ == "red")` 无 else | 添加蓝色 HSV 阈值，支持 `red/blue/both` |
| `equalizeHist` 崩溃 | 错误用于 3 通道 BGR 图 | 改用 LAB + CLAHE 在 L 通道增强 |
| 检测框剧烈抖动 | 无时序滤波 | EMA 平滑（`smooth_alpha=0.7`） |

</details>

## 附录：W4 → W5 工程化整理记录

<details>
<summary>W5 工程化整理内容（点击展开）</summary>

### 代码结构确认

W4 的代码结构已经满足要求，W5 无需重构：

| 文件 | 职责 | 依赖 |
|------|------|------|
| `armor_detector.hpp` | 数据结构 + `ArmorDetector` 类声明 | OpenCV |
| `armor_detector.cpp` | 检测算法实现（预处理/灯条提取/配对/平滑/绘制） | OpenCV |
| `armor_detector_node.cpp` | ROS2 节点（订阅→调用检测→发布结果） | OpenCV + ROS2 |

> 检测逻辑输入 `cv::Mat`，输出 `ArmorPlate`，与 ROS2 完全解耦。同一套检测逻辑可被离线图片、视频、bag 和真实相机复用。

### 参数补全

W4 的 `armor_params.yaml` 已包含全部 30+ 参数，覆盖：
- 预处理：`brightness_alpha`, `enable_clahe`, `clahe_clip`, `clahe_tile`
- 红色 HSV：`red1_*`, `red2_*`（两段式，共 12 个值）
- 蓝色 HSV：`blue_*`（共 6 个值）
- 形态学：`morph_kernel_size`, `enable_morph_open`, `enable_morph_close`
- 灯条筛选：`light_min_area`, `light_max_area`, `light_max_ratio`
- 配对：`pair_max_dy`, `pair_min_dx`, `pair_max_dx`
- 平滑：`smooth_alpha`

W5 新增 launch 层参数：`input_source`, `bag_path`, `enable_debug`

### launch 完善

```
W4: launch 强制启动相机 → 无相机时报错
W5: 新增 input_source 参数 → camera 模式启动相机，bag 模式自动播放 bag
```

### 目录结构

```
armor_detector/
├── CMakeLists.txt          # 编译规则
├── package.xml             # 包声明
├── config/
│   └── armor_params.yaml   # ★ 所有可调参数（30+ 项）
├── launch/
│   └── armor.launch.py     # ★ 一键启动（支持 camera/bag 切换）
├── include/armor_detector/
│   └── armor_detector.hpp  # 数据结构 + 检测器类声明
├── src/
│   ├── armor_detector.cpp       # 检测算法库（纯 OpenCV，不依赖 ROS）
│   └── armor_detector_node.cpp  # ROS2 节点（订阅/发布/参数管理）
└── README.md               # 本文档
```

</details>

---

**维护者：** tiexuejuan  
**最后更新：** 2026-06-02
