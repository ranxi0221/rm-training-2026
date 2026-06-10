# Armor Detector — ROS2 装甲板检测

## 项目功能

基于 OpenCV + ROS2 的机器人装甲板识别模块。从工业相机获取图像，通过 HSV 颜色分离、形态学处理、轮廓筛选和几何配对，实时检测并定位装甲板。

**核心流程：** 图像采集 → HSV 颜色分离 → 形态学处理 → 灯条轮廓提取/筛选 → 灯条配对 → 装甲板四角计算 → EMA 时序平滑 → 结果发布

**支持特性：**
- 红/蓝双色灯条检测（`target_color: red / blue / both`）
- CLAHE 局部对比度增强（适应过曝/过暗场景）
- EMA 指数平滑（减少帧间抖动）
- 调试图像发布（灯条框 + 装甲板框叠加）

## 架构设计：检测逻辑 vs 节点逻辑
  armor_detector_node.cpp          ← ROS2 节点（胶水层）
  职责：订阅图像 → cv_bridge     
       读取 YAML 参数           
        调用 detector_.detect()  
       发布 /armor_result       
       发布 /armor_debug_image  

            cv::Mat → ArmorPlate

  armor_detector.cpp            ← 检测算法（纯 OpenCV）
  职责：preprocess() 预处理           不依赖 ROS2
       extractLightBars() 灯条  
       matchArmorPlate() 配对   
        drawDebug() 绘制         
        EMA 时序平滑             


关键设计原则： 检测逻辑输入 `cv::Mat`，输出 `ArmorPlate`，与 ROS2 完全一致。同一套代码可被**离线图片、视频、bag、真实相机**四种输入源复用，无需修改。

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

# 3. 编译装甲板检测包（默认只需编译这一个包）
colcon build --packages-select armor_detector

# 4. 加载编译产物
source install/setup.bash
```

> **注意：** 默认启动为 bag 回放模式，不需要编译相机驱动。如果你有真实相机、需要相机模式，才需要额外编译 `mindvision_camera`：
> ```bash
> colcon build --packages-select mindvision_camera armor_detector
> ```

**常见问题：**
- 链接错误 → 确认 `pkg-config --modversion opencv4` 返回 4.5+
- 找不到 mindvision_camera → 仅相机模式需要；离线调试用 bag 模式即可，无需安装相机驱动

## 运行方式

### 一键启动

```bash
# Bag 回放模式（默认，不需要相机）
# 本地测试 bag 路径：bags/armor_test
ros2 launch armor_detector armor.launch.py bag_path:=bags/armor_test target_color:=both

# 真实相机模式（需先编译 mindvision_camera 并连接相机）
ros2 launch armor_detector armor.launch.py input_source:=camera target_color:=both
```

### 输入源切换

通过 `input_source` 参数一键切换：

| 模式 | 命令 | 说明 |
|------|------|------|
| **Bag 回放** | `input_source:=bag`（默认） | 仅启动检测节点，循环播放指定 bag 文件，无需硬件 |
| **真实相机** | `input_source:=camera` | 启动相机驱动 + 检测节点，需编译 `mindvision_camera` |

```bash
# 示例1：用 bag 离线调试（默认模式）
ros2 launch armor_detector armor.launch.py bag_path:=bags/armor_test

# 示例2：用真实相机检测红蓝双色
ros2 launch armor_detector armor.launch.py input_source:=camera target_color:=both
```

### 分步启动（调试用）
```bash
# 终端1：启动相机节点
ros2 run mindvision_camera mindvision_camera_node

# 终端2：启动检测节点
ros2 run armor_detector armor_detector_node --ros-args -p target_color:=red
```

### 有相机时的实时检测
```bash
# 方式1：用 launch 的 camera 模式（一键启动相机+检测）
ros2 launch armor_detector armor.launch.py input_source:=camera target_color:=both

# 方式2：手动分步启动
ros2 run mindvision_camera mindvision_camera_node                                    # 终端1：相机
ros2 run armor_detector armor_detector_node --ros-args -p target_color:=red           # 终端2：检测


## 输入源

项目支持两种输入源，通过 launch 参数 `input_source` 一键切换：

| 模式 | 输入来源 | 需要硬件 | 适用场景 |
|------|----------|:--:|------|
| **bag**（默认） | ros2 bag 文件回放 | ❌ | 离线复现、算法调试、无相机环境 |
| **camera** | 迈德威视工业相机（USB3.0） | ✅ | 真实比赛/联调 |

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
# 默认：bag 回放模式
ros2 launch armor_detector armor.launch.py bag_path:=/path/to/bag_file

# 切换到真实相机（需编译 mindvision_camera）
ros2 launch armor_detector armor.launch.py input_source:=camera
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

## W6 改进记录（2026-06-10）

### 选择方向：配对问题

**失败现象：** 灯条能检测到（红/蓝小框存在），但黄色装甲板框不稳定——倾斜时变三角形或消失、长方形装甲板掉框、同画面多装甲板只框一个。

**问题定位：** 三个关键 bug——
1. `matchArmorPlate` 右灯条错误取左边缘（X排序后永远取pts[0][1]），导致装甲板框扭曲
2. 红蓝灯条混在一起按X排序，不同色灯条隔开同色配对
3. 配对只返回第一个结果，不支持多装甲板

### 定向改进内容

| 改动 | 文件 | 说明 |
|------|------|------|
| 角点提取修复 | `armor_detector.cpp` | 每灯条独立按Y分上下取极值X，不依赖全局排序和角度 |
| 颜色分组配对 | `armor_detector.cpp` | 红蓝分开配对，同色全遍历，支持不重叠多配对 |
| 多装甲板返回 | `armor_detector.hpp/cpp/node` | `detect()` 返回 `vector<ArmorPlate>`，同画面支持红+蓝两个框 |
| 配对参数放宽 | `armor_params.yaml` | `pair_max_dy: 350`, `pair_min_dx: 5`, `pair_max_dx: 2500` |
| 灯条筛选放宽 | `armor_params.yaml` | `light_min_area: 10`, `light_max_area: 50000`, `light_max_ratio: 1.5` |

### 同素材前后对比

| 对比项 | 改前 | 改后 |
|------|------|------|
| 倾斜30° | 黄框变三角形或消失 | 黄框稳定贴合 |
| 长方形装甲板 | 黄框频繁掉帧 | 稳定跟踪 |
| 红蓝同画面 | 只出一个框 | 两个框同时显示 |
| 远近移动 | 近/远都掉 | 全程不掉 |

> 对比截图：[project/armor_detector/assets/w6_compare/](assets/w6_compare/) — before.png / after.png

### W6 最终参数

保存位置：`config/armor_params.yaml`（暗环境优化版）
核心参数：`smooth_alpha: 0.3`, `pair_max_dy: 350`, `pair_min_dx: 5`, `pair_max_dx: 2500`

### 失败样例（本周未解决）

| # | 现象 | 原因判断 | 下一步 |
|:--:|------|------|------|
| 1 | 环境变亮后识别不稳定 | HSV 阈值针对极暗环境调优，亮环境灯条过曝偏白 | 需实测亮环境 HSV 值后建立第二套参数 |
| 2 | 亮环境有少量误检 | 背景物体反射光落入 HSV 范围 | 增加面积/形状/亮度梯度二次筛选 |
| 3 | 倾斜角度超过30°时掉框 | `pair_max_dy` 已达上限，物理倾斜超出几何约束 | 考虑角度自适应或灯条端点匹配 |
| 4 | 无法检测两个以上同色装甲板 | `collectPairs` 限制每灯条只配对一次 | 需引入图匹配算法 |
| 5 | 光照自适应未完成 | `enable_adaptive_brightness` 参数存在但阈值需要实测校准 | 实测亮环境 HSV + 建立 bright_profile |

---

## 当前已知局限

### 算法局限
- **光照适应性差**：HSV 阈值针对暗环境调优，亮环境性能下降
- **仅支持2目标**：最多检测一红一蓝，同色多目标不支持
- **倾斜角度限制**：超过30°后配对失败
- **无预测跟踪**：未实现卡尔曼滤波或光流跟踪

### 工程局限
- **相机连接不稳定**：MindVision SDK 偶发 `user control fd open failed`
- **无数字识别**：仅检测装甲板位置，未识别数字

### 待解决（优先级排序）
1. 亮环境 HSV 参数实测 + 自适应完善
2. 倾斜角度 > 30° 的配对支持
3. 添加 ROI 裁剪减少无效计算
4. 集成卡尔曼滤波实现运动预测
5. 数字识别模块

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
CMakeLists.txt          # 编译规则
package.xml             # 包声明
config/
 armor_params.yaml   # ★ 所有可调参数（30+ 项）
 launch/
 armor.launch.py     # ★ 一键启动（支持 camera/bag 切换）
 include/armor_detector/
 armor_detector.hpp  # 数据结构 + 检测器类声明
 src/
   armor_detector.cpp       # 检测算法库（纯 OpenCV，不依赖 ROS）
   armor_detector_node.cpp  # ROS2 节点（订阅/发布/参数管理）
   README.md               # 本文档


