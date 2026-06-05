#pragma once//防止头文件被重复
#include <opencv2/opencv.hpp>
#include <vector>//容器
#include <string>//字符串

namespace armor_detector
{

// 灯条检测结果
struct LightBar
{
    cv::RotatedRect rect;       // 旋转矩形包含矩形中型坐标，宽和高，以及角度
    std::string color;           // "red" 或 "blue"
    float confidence = 1.0f;    // 置信度（保留扩展）
};

// 装甲板检测结果

struct ArmorPlate
{
    bool detected = false;
    cv::Point2f corners[4];     // 四个角点（左上、左下、右下、右上 顺序）
    cv::Point2f center;         // 中心点
    std::string color;           // "red" 或 "blue"
};

// 可调参数
struct DetectorParams
{
    // ---- 预处理 ----
    double brightness_alpha = 0.3;   // 亮度缩放系数（0~1，越小越暗）
    bool enable_clahe = true;        // 是否启用 CLAHE 增强对比度
    double clahe_clip = 2.0;         // CLAHE 对比度限制
    int clahe_tile = 8;              // CLAHE 网格大小

    // ---- 红色 HSV 阈值（两段：深红 + 浅红/过曝） ----
    int red1_h_low = 0,   red1_h_high = 10;
    int red1_s_low = 100, red1_s_high = 255;
    int red1_v_low = 50,  red1_v_high = 255;

    int red2_h_low = 170, red2_h_high = 179;
    int red2_s_low = 100, red2_s_high = 255;
    int red2_v_low = 50,  red2_v_high = 255;

    // ---- 蓝色 HSV 阈值 ----
    int blue_h_low = 95,  blue_h_high = 130;
    int blue_s_low = 80,  blue_s_high = 255;
    int blue_v_low = 50,  blue_v_high = 255;

    // ---- 形态学 ----
    int morph_kernel_size = 5;       // 开/闭运算核大小
    bool enable_morph_open = true;   // 开运算去噪
    bool enable_morph_close = true;  // 闭运算填洞

    // ---- 灯条筛选 ----
    float light_min_area = 20.0f;
    float light_max_area = 10000.0f;
    float light_max_ratio = 1.0f;     // width/height 最大值（竖条 width<height，所以 ratio<1）

    // ---- 装甲板配对 ----
    float pair_max_dy = 50.0f;        // 两灯条中心 Y 方向最大偏差
    float pair_min_dx = 20.0f;        // 两灯条中心 X 方向最小间距
    float pair_max_dx = 800.0f;       // 两灯条中心 X 方向最大间距

    // ---- 时序平滑 ----
    float smooth_alpha = 0.7f;        // EMA 平滑系数（越大越平滑）
};

// 装甲板检测器
class ArmorDetector
{
public:
    ArmorDetector();
    explicit ArmorDetector(const DetectorParams & params);

    // 设置/获取参数
    void setParams(const DetectorParams & params);
    const DetectorParams & getParams() const;

    // 核心检测：输入 BGR 图像 + 目标颜色，返回装甲板
    // target_color: "red" / "blue" / "both"
    // 当 target_color == "both" 时优先返回置信度更高的
    ArmorPlate detect(const cv::Mat & bgr, const std::string & target_color = "red");

    // 获取最近一帧检测到的灯条列表（供调试绘制用）
    const std::vector<LightBar> & getLastLightBars() const { return last_light_bars_; }

    // 调试绘制：在原图上绘制灯条和装甲板
    cv::Mat drawDebug(const cv::Mat & bgr,
                      const std::vector<LightBar> & light_bars,
                      const ArmorPlate & armor) const;

private:
    // 预处理：降亮度 + CLAHE
    cv::Mat preprocess(const cv::Mat & bgr);

    // 提取灯条
    std::vector<LightBar> extractLightBars(const cv::Mat & bgr,
                                           const cv::Mat & hsv,
                                           const std::string & color);

    // 配对装甲板 — 凸包法
    ArmorPlate matchArmorPlate(const std::vector<LightBar> & light_bars);

    DetectorParams params_;
    ArmorPlate last_armor_;              // 用于平滑,检测上一个画面
    std::vector<LightBar> last_light_bars_;  // 最近一帧灯条（供调试绘制）
    cv::Mat debug_mask_;                 // 保存 mask 供调试
};

}  // namespace armor_detector
