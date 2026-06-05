#include "armor_detector/armor_detector.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace armor_detector
{

// 构造
ArmorDetector::ArmorDetector()
{
    last_armor_.detected = false;//初始化，还没有检测到装甲板
}

ArmorDetector::ArmorDetector(const DetectorParams & params)
    : params_(params) //用传入的参数初始化
{
    last_armor_.detected = false;
}

void ArmorDetector::setParams(const DetectorParams & params)
{
    params_ = params;
}

const DetectorParams & ArmorDetector::getParams() const
{
    return params_;
}

// 预处理：降低亮度 + CLAHE 增强对比度
cv::Mat ArmorDetector::preprocess(const cv::Mat & bgr)
{
    // 1. 降低亮度
    cv::Mat darkened;
    bgr.convertTo(darkened, -1, params_.brightness_alpha, 0);

    if (!params_.enable_clahe) {
        return darkened;
    }

    // 2. CLAHE：转到 LAB，在 L 通道做自适应直方图均衡，再转回 BGR
    //    （修复之前 equalizeHist 错误用于 3 通道图像的问题）
    cv::Mat lab;
    cv::cvtColor(darkened, lab, cv::COLOR_BGR2Lab);

    std::vector<cv::Mat> lab_chans;
    cv::split(lab, lab_chans);

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(params_.clahe_clip,
        cv::Size(params_.clahe_tile, params_.clahe_tile));
    clahe->apply(lab_chans[0], lab_chans[0]);  // 只处理 L 通道

    cv::merge(lab_chans, lab);
    cv::Mat result;
    cv::cvtColor(lab, result, cv::COLOR_Lab2BGR);

    return result;
}

// 提取灯条
std::vector<LightBar> ArmorDetector::extractLightBars(
    const cv::Mat & /*bgr*/,
    const cv::Mat & hsv,
    const std::string & color)
{
    cv::Mat mask;

    if (color == "red") {
        // 红色 HSV：两段式（绕过 0° 红线）
        cv::Mat m1, m2;
        cv::inRange(hsv,
            cv::Scalar(params_.red1_h_low, params_.red1_s_low, params_.red1_v_low),
            cv::Scalar(params_.red1_h_high, params_.red1_s_high, params_.red1_v_high),
            m1);
        cv::inRange(hsv,
            cv::Scalar(params_.red2_h_low, params_.red2_s_low, params_.red2_v_low),
            cv::Scalar(params_.red2_h_high, params_.red2_s_high, params_.red2_v_high),
            m2);
        mask = m1 | m2;
    }
    else if (color == "blue") {
        // 蓝色 HSV
        cv::inRange(hsv,
            cv::Scalar(params_.blue_h_low, params_.blue_s_low, params_.blue_v_low),
            cv::Scalar(params_.blue_h_high, params_.blue_s_high, params_.blue_v_high),
            mask);
    }
    else {
        return {};  // 不支持的颜色
    }

    // 形态学去噪
    if (params_.enable_morph_open || params_.enable_morph_close) {
        cv::Mat k = cv::getStructuringElement(cv::MORPH_RECT,
            cv::Size(params_.morph_kernel_size, params_.morph_kernel_size));

        if (params_.enable_morph_open) {
            cv::morphologyEx(mask, mask, cv::MORPH_OPEN, k);
        }
        if (params_.enable_morph_close) {
            cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, k);
        }
    }

    // 保存 mask 供调试
    debug_mask_ = mask.clone();

    // 找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 筛选灯条
    std::vector<LightBar> lights;
    for (const auto & c : contours) {
        double area = cv::contourArea(c);

        // 面积过滤
        if (area < params_.light_min_area || area > params_.light_max_area) {
            continue;
        }

        cv::RotatedRect rect = cv::minAreaRect(c);

        // 宽高比过滤：灯条是竖长的，width < height
        float w = std::min(rect.size.width, rect.size.height);
        float h = std::max(rect.size.width, rect.size.height);
        if (h < 1e-5f) continue;  // 防止除零

        float ratio = w / h;
        if (ratio > params_.light_max_ratio) {
            continue;
        }

        // 确保 rect.size.width 是短边（宽），height 是长边（高）
        // 便于后续配对使用正确的角度
        if (rect.size.width > rect.size.height) {
            std::swap(rect.size.width, rect.size.height);
            rect.angle += 90.0f;
        }

        LightBar lb;
        lb.rect = rect;
        lb.color = color;
        lights.push_back(lb);
    }

    return lights;
}

// 配对装甲板 — 精准角点法
//
// 从每个灯条的 4 个旋转矩形角点中提取左/右边缘，直接用灯条角点做装甲板四角。
// 黄色框精准贴到灯条端点，不会外扩。
ArmorPlate ArmorDetector::matchArmorPlate(const std::vector<LightBar> & light_bars)
{
    ArmorPlate result;
    result.detected = false;

    if (light_bars.size() < 2) {
        return result;
    }

    // 按 X 坐标排序（左 → 右）
    std::vector<LightBar> sorted = light_bars;
    std::sort(sorted.begin(), sorted.end(),
        [](const LightBar & a, const LightBar & b) {
            return a.rect.center.x < b.rect.center.x;
        });

    // 遍历相邻灯条对
    for (size_t i = 0; i < sorted.size() - 1; ++i) {
        const auto & left = sorted[i];
        const auto & right = sorted[i + 1];

        // 必须是同色
        if (left.color != right.color) continue;

        float dx = std::fabs(left.rect.center.x - right.rect.center.x);
        float dy = std::fabs(left.rect.center.y - right.rect.center.y);

        if (dy > params_.pair_max_dy) continue;
        if (dx < params_.pair_min_dx || dx > params_.pair_max_dx) continue;

        // ---- 提取每个灯条的左/右边缘角点 ----
        auto extractEdges = [](const cv::RotatedRect & rect,
                                cv::Point2f & top, cv::Point2f & bot) {
            cv::Point2f pts[4];
            rect.points(pts);

            // 按 X 排序：pts[0],pts[1] = 左边两点, pts[2],pts[3] = 右边两点
            std::sort(pts, pts + 4,
                [](const cv::Point2f & a, const cv::Point2f & b) { return a.x < b.x; });

            // 左边两点按 Y 分上下
            if (pts[0].y > pts[1].y) std::swap(pts[0], pts[1]);
            top = pts[0];   // 左上
            bot = pts[1];   // 左下
        };

        cv::Point2f l_top, l_bot, r_top, r_bot;
        extractEdges(left.rect, l_top, l_bot);
        extractEdges(right.rect, r_top, r_bot);

        // 装甲板四角 = 左灯条左边缘 + 右灯条右边缘
        cv::Point2f armor_pts[4];
        armor_pts[0] = l_top;   // 左上
        armor_pts[1] = l_bot;   // 左下
        armor_pts[2] = r_bot;   // 右下
        armor_pts[3] = r_top;   // 右上

        // 计算中心
        result.center = cv::Point2f(0, 0);
        for (int j = 0; j < 4; ++j) {
            result.corners[j] = armor_pts[j];
            result.center += armor_pts[j];
        }
        result.center *= 0.25f;

        result.detected = true;
        result.color = left.color;
        return result;
    }

    return result;
}
// 核心检测接口

ArmorPlate ArmorDetector::detect(const cv::Mat & bgr, const std::string & target_color)
{
    // 1. 预处理
    cv::Mat processed = preprocess(bgr);

    // 2. 转 HSV
    cv::Mat hsv;
    cv::cvtColor(processed, hsv, cv::COLOR_BGR2HSV);

    // 3. 提取灯条
    std::vector<LightBar> all_lights;

    if (target_color == "red" || target_color == "both") {
        auto red_lights = extractLightBars(processed, hsv, "red");
        all_lights.insert(all_lights.end(), red_lights.begin(), red_lights.end());
    }
    if (target_color == "blue" || target_color == "both") {
        auto blue_lights = extractLightBars(processed, hsv, "blue");
        all_lights.insert(all_lights.end(), blue_lights.begin(), blue_lights.end());
    }

    // 4. 配对装甲板
    ArmorPlate armor = matchArmorPlate(all_lights);

    // 5. 保存灯条列表供外部调试绘制
    last_light_bars_ = all_lights;

    // 6. 时序平滑（EMA）
    if (armor.detected && last_armor_.detected && armor.color == last_armor_.color) {
        float a = params_.smooth_alpha;
        float b = 1.0f - a;

        armor.center.x = a * last_armor_.center.x + b * armor.center.x;
        armor.center.y = a * last_armor_.center.y + b * armor.center.y;

        for (int i = 0; i < 4; ++i) {
            armor.corners[i].x = a * last_armor_.corners[i].x + b * armor.corners[i].x;
            armor.corners[i].y = a * last_armor_.corners[i].y + b * armor.corners[i].y;
        }
    }

    last_armor_ = armor;
    return armor;
}


// 调试绘制

cv::Mat ArmorDetector::drawDebug(const cv::Mat & bgr,
                                  const std::vector<LightBar> & light_bars,
                                  const ArmorPlate & armor) const
{
    cv::Mat dbg = bgr.clone();

    // ---- 绘制灯条 ----
    for (const auto & lb : light_bars) {
        cv::Scalar color;
        if (lb.color == "red") {
            color = cv::Scalar(0, 0, 255);    // BGR 红色
        } else {
            color = cv::Scalar(255, 0, 0);    // BGR 蓝色
        }

        cv::Point2f pts[4];
        lb.rect.points(pts);
        for (int j = 0; j < 4; ++j) {
            cv::line(dbg, pts[j], pts[(j + 1) % 4], color, 2);
        }

        // 在灯条中心标注颜色
        cv::putText(dbg, lb.color,
            lb.rect.center + cv::Point2f(5, 0),
            cv::FONT_HERSHEY_SIMPLEX, 0.4, color, 1);
    }

    // ---- 绘制装甲板 ----
    if (armor.detected) {
        // 装甲板用亮黄色（BGR: 青+绿 = 黄色），与红/蓝灯条区分
        cv::Scalar armor_color(0, 255, 255);  // BGR 黄色

        // 四条边
        for (int i = 0; i < 4; ++i) {
            cv::line(dbg,
                armor.corners[i],
                armor.corners[(i + 1) % 4],
                armor_color, 3);
        }

        // 对角线（辅助观察角点是否正确）
        cv::line(dbg, armor.corners[0], armor.corners[2],
            cv::Scalar(0, 255, 255), 1, cv::LINE_AA);
        cv::line(dbg, armor.corners[1], armor.corners[3],
            cv::Scalar(0, 255, 255), 1, cv::LINE_AA);

        // 中心圆
        cv::circle(dbg, armor.center, 8, cv::Scalar(0, 255, 255), -1);

        // 角点编号标注
        for (int i = 0; i < 4; ++i) {
            cv::putText(dbg, std::to_string(i),
                armor.corners[i] + cv::Point2f(3, -3),
                cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
        }

        // 颜色标签
        cv::putText(dbg, "ARMOR:" + armor.color,
            armor.center + cv::Point2f(10, 0),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 2);
    }

    return dbg;
}

}  // namespace armor_detector
