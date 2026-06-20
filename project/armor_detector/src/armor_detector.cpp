#include "armor_detector/armor_detector.hpp"
#include <algorithm>
#include <cmath>
#include <string>
#include <map>

namespace armor_detector
{

// 构造
ArmorDetector::ArmorDetector()
{
}

ArmorDetector::ArmorDetector(const DetectorParams & params)
    : params_(params)
{
}

void ArmorDetector::setParams(const DetectorParams & params)
{
    params_ = params;
}

const DetectorParams & ArmorDetector::getParams() const
{
    return params_;
}

bool ArmorDetector::loadDigitModel(const std::string & model_path)
{
    return digit_classifier_.loadModel(model_path);
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
    const cv::Mat & bgr,
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

    // W7 BGR纯度预过滤：绝对值+比例双重要求
    {
        std::vector<cv::Mat> bgr_chans;
        cv::split(bgr, bgr_chans);
        cv::Mat purity;
        if (color == "blue") {
            cv::Mat bright, dom_r, dom_g;
            cv::compare(bgr_chans[0], 120, bright, cv::CMP_GT); // B>120
            cv::compare(bgr_chans[0], bgr_chans[2] * 1.7, dom_r, cv::CMP_GT);
            cv::compare(bgr_chans[0], bgr_chans[1] * 1.7, dom_g, cv::CMP_GT);
            purity = bright & dom_r & dom_g;
        } else {
            cv::Mat bright, dom_b, dom_g;
            cv::compare(bgr_chans[2], 120, bright, cv::CMP_GT); // R>120
            cv::compare(bgr_chans[2], bgr_chans[0] * 1.7, dom_b, cv::CMP_GT);
            cv::compare(bgr_chans[2], bgr_chans[1] * 1.7, dom_g, cv::CMP_GT);
            purity = bright & dom_b & dom_g;
        }
        mask = mask & purity;
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
std::vector<ArmorPlate> ArmorDetector::matchArmorPlate(const std::vector<LightBar> & light_bars)
{
    std::vector<ArmorPlate> results;

    if (light_bars.size() < 2) {
        return results;
    }

    // ---- W6修复：按颜色分组，各自独立配对 ----
    // 避免红蓝混排导致同色灯条被不同色灯条隔开而漏配
    auto getCorners = [](const cv::RotatedRect & rect, bool takeMinX,
                          cv::Point2f & top, cv::Point2f & bot) {
        cv::Point2f pts[4];
        rect.points(pts);
        std::sort(pts, pts + 4,
            [](const cv::Point2f & a, const cv::Point2f & b) { return a.y < b.y; });
        if (takeMinX) {
            top = (pts[0].x < pts[1].x) ? pts[0] : pts[1];
            bot = (pts[2].x < pts[3].x) ? pts[2] : pts[3];
        } else {
            top = (pts[0].x > pts[1].x) ? pts[0] : pts[1];
            bot = (pts[2].x > pts[3].x) ? pts[2] : pts[3];
        }
    };

    // W6: 收集同色所有有效配对（不重叠），支持多装甲板
    auto collectPairs = [&](const std::string & color) -> std::vector<ArmorPlate> {
        std::vector<ArmorPlate> res;
        std::vector<LightBar> same_color;
        for (const auto & lb : light_bars)
            if (lb.color == color) same_color.push_back(lb);
        if (same_color.size() < 2) return res;

        std::sort(same_color.begin(), same_color.end(),
            [](const LightBar & a, const LightBar & b) {
                return a.rect.center.x < b.rect.center.x; });

        std::vector<bool> used(same_color.size(), false);

        for (size_t i = 0; i < same_color.size(); ++i) {
            if (used[i]) continue;
            for (size_t j = i + 1; j < same_color.size(); ++j) {
                if (used[j]) continue;
                float dx = std::fabs(same_color[i].rect.center.x - same_color[j].rect.center.x);
                float dy = std::fabs(same_color[i].rect.center.y - same_color[j].rect.center.y);
                if (dy > params_.pair_max_dy) continue;
                if (dx < params_.pair_min_dx || dx > params_.pair_max_dx) continue;
                // W7: 两个灯条必须高度相近（真配对两灯条尺寸一致）
                float h1 = std::max(same_color[i].rect.size.height, same_color[i].rect.size.width);
                float h2 = std::max(same_color[j].rect.size.height, same_color[j].rect.size.width);
                float w1 = std::min(same_color[i].rect.size.width, same_color[i].rect.size.height);
                float w2 = std::min(same_color[j].rect.size.width, same_color[j].rect.size.height);
                if (h1 < 5 || h2 < 5) continue;
                if (std::min(h1, h2) / std::max(h1, h2) < 0.4f) continue; // 高度差>2.5×拒
                if (std::min(w1, w2) / std::max(w1, w2) < 0.3f) continue; // 宽度差>3×拒

                ArmorPlate armor;
                cv::Point2f l_top, l_bot, r_top, r_bot;
                getCorners(same_color[i].rect, true, l_top, l_bot);
                getCorners(same_color[j].rect, false, r_top, r_bot);
                armor.corners[0] = l_top;
                armor.corners[1] = l_bot;
                armor.corners[2] = r_bot;
                armor.corners[3] = r_top;
                armor.center = (armor.corners[0] + armor.corners[1]
                              + armor.corners[2] + armor.corners[3]) * 0.25f;
                armor.detected = true;
                armor.color = color;
                res.push_back(armor);
                used[i] = used[j] = true;
                break;
            }
        }
        return res;
    };

    // 收集所有颜色的装甲板
    auto reds = collectPairs("red");
    auto blues = collectPairs("blue");
    results.insert(results.end(), reds.begin(), reds.end());
    results.insert(results.end(), blues.begin(), blues.end());
    return results;
}
// 核心检测接口

std::vector<ArmorPlate> ArmorDetector::detect(const cv::Mat & bgr,
                                              const std::string & target_color)
{
    // 1. 预处理
    cv::Mat processed = preprocess(bgr);

    // 2. 转 HSV
    cv::Mat hsv;
    cv::cvtColor(processed, hsv, cv::COLOR_BGR2HSV);

    // 2.5 亮环境模式：手动开启后提高 S/V 门槛过滤误检（默认关闭）
    int saved_red1_sl = params_.red1_s_low, saved_red1_sh = params_.red1_s_high;
    int saved_red1_vl = params_.red1_v_low;
    int saved_red2_sl = params_.red2_s_low, saved_red2_sh = params_.red2_s_high;
    int saved_red2_vl = params_.red2_v_low;
    int saved_blue_sl  = params_.blue_s_low,  saved_blue_sh  = params_.blue_s_high;
    int saved_blue_vl  = params_.blue_v_low,  saved_blue_vh  = params_.blue_v_high;
    int saved_blue_hl  = params_.blue_h_low,  saved_blue_hh  = params_.blue_h_high;
    float saved_light_min = params_.light_min_area;
    float saved_pair_dy   = params_.pair_max_dy;
    float saved_pair_dx_min = params_.pair_min_dx;
    float saved_ratio     = params_.light_max_ratio;
    float saved_smooth    = params_.smooth_alpha;
    bool saved_morph_open = params_.enable_morph_open;
    int saved_morph_k     = params_.morph_kernel_size;

    if (params_.enable_adaptive_brightness) {
        params_.red1_s_low = 30;  params_.red1_v_low = 100;
        params_.red2_s_low = 30;  params_.red2_v_low = 100;
        params_.blue_s_low  = 40; params_.blue_v_low  = 180; // 放松保持蓝灯条稳定
        params_.blue_h_low  = 100; params_.blue_h_high = 120;
        params_.light_min_area = 30;
        params_.light_max_ratio = 0.5f;
        params_.pair_max_dy    = 500;
        params_.pair_min_dx    = 3;
        params_.smooth_alpha   = 0.9f;
    }

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

    // 恢复原始参数
    params_.red1_s_low = saved_red1_sl; params_.red1_s_high = saved_red1_sh;
    params_.red1_v_low = saved_red1_vl;
    params_.red2_s_low = saved_red2_sl; params_.red2_s_high = saved_red2_sh;
    params_.red2_v_low = saved_red2_vl;
    params_.blue_s_low  = saved_blue_sl;  params_.blue_s_high  = saved_blue_sh;
    params_.blue_v_low  = saved_blue_vl;  params_.blue_v_high  = saved_blue_vh;
    params_.blue_h_low  = saved_blue_hl;  params_.blue_h_high  = saved_blue_hh;
    params_.light_min_area = saved_light_min;
    params_.pair_max_dy    = saved_pair_dy;
    params_.pair_min_dx    = saved_pair_dx_min;
    params_.light_max_ratio = saved_ratio;
    params_.smooth_alpha    = saved_smooth;
    params_.enable_morph_open = saved_morph_open;
    params_.morph_kernel_size = saved_morph_k;

    // 4. 配对装甲板
    auto armors = matchArmorPlate(all_lights);
    last_light_bars_ = all_lights;

    // 3.5 LED验证：峰值比过滤反光（全模式生效，阈值自适应）
    {
        cv::Mat gray;
        cv::cvtColor(processed, gray, cv::COLOR_BGR2GRAY);
        std::vector<LightBar> verified;
        for (const auto & lb : all_lights) {
            int cx = (int)lb.rect.center.x, cy = (int)lb.rect.center.y;
            cv::Rect croi(cx-4, cy-4, 8, 8);
            cv::Rect eroi(cx-12, cy-12, 24, 24);
            croi &= cv::Rect(0, 0, gray.cols, gray.rows);
            eroi  &= cv::Rect(0, 0, gray.cols, gray.rows);
            if (croi.area() < 9 || eroi.area() < 16) continue;
            // 峰值比：真LED中心极亮，峰值>>平均值
            double max_gray, mean_gray;
            cv::minMaxLoc(gray(croi), nullptr, &max_gray, nullptr, nullptr);
            mean_gray = cv::mean(gray(croi))[0];
            double edge_mean = cv::mean(gray(eroi))[0];
            if (max_gray < 180 || max_gray < edge_mean * 2.5 || max_gray < mean_gray * 1.5) continue;

            if (params_.enable_adaptive_brightness) {
                cv::Mat bgr_roi = processed(croi);
                std::vector<cv::Mat> chans; cv::split(bgr_roi, chans);
                double maxB, maxG, maxR;
                cv::minMaxLoc(chans[0], nullptr, &maxB);
                cv::minMaxLoc(chans[1], nullptr, &maxG);
                cv::minMaxLoc(chans[2], nullptr, &maxR);
                if (lb.color == "blue" && (maxB < maxR * 1.5 || maxB < maxG * 1.5)) continue;
                if (lb.color == "red"  && (maxR < maxB * 1.5 || maxR < maxG * 1.5)) continue;
            }

            verified.push_back(lb);
        }
        all_lights = verified;
    }

    // 4.5 每色只保留面积最大的1个装甲板（暗/亮模式都生效）
    {
        auto bestPerColor = [&](const std::string & color) -> ArmorPlate {
            ArmorPlate best; best.detected = false;
            float best_area = 0;
            for (const auto & a : armors) {
                if (a.color != color) continue;
                cv::Rect2f bb(a.corners[0], a.corners[2]);
                float area = bb.area();
                if (area > best_area) { best_area = area; best = a; }
            }
            return best;
        };
        std::vector<ArmorPlate> filtered;
        for (const auto & c : {"red", "blue"}) {
            auto best = bestPerColor(c);
            if (best.detected) filtered.push_back(best);
        }
        armors = filtered;
    }

    // 5. 时序平滑（EMA）—— 按颜色匹配上一帧装甲板
    if (!armors.empty() && !last_armors_.empty()) {
        float a = params_.smooth_alpha;
        float b = 1.0f - a;
        for (auto & cur : armors) {
            for (const auto & prev : last_armors_) {
                if (cur.color == prev.color && prev.detected) {
                    cur.center.x = a * prev.center.x + b * cur.center.x;
                    cur.center.y = a * prev.center.y + b * cur.center.y;
                    for (int i = 0; i < 4; ++i) {
                        cur.corners[i].x = a * prev.corners[i].x + b * cur.corners[i].x;
                        cur.corners[i].y = a * prev.corners[i].y + b * cur.corners[i].y;
                    }
                    break;
                }
            }
        }
    }

    // 5.5 W7: 数字识别 + 有限帧间记忆（最多5帧，避免假数字锁死）
    if (digit_classifier_.isLoaded()) {
        static std::map<std::string, std::pair<std::string, int>> memo; // color→{digit, remain}
        for (auto & armor : armors) {
            armor.digit = digit_classifier_.classify(processed, armor.corners);
            // 当前有效数字→更新记忆
            if (!armor.digit.empty() && armor.digit != "not_armor") {
                memo[armor.color] = {armor.digit, 30};
            }
            // 当前为空→从记忆取（剩余帧数>0）
            else if (memo.count(armor.color) && memo[armor.color].second > 0) {
                armor.digit = memo[armor.color].first;
                memo[armor.color].second--;
            }
        }
    }

    last_armors_ = armors;
    return armors;
}


// 调试绘制

cv::Mat ArmorDetector::drawDebug(const cv::Mat & bgr,
                                  const std::vector<LightBar> & light_bars,
                                  const std::vector<ArmorPlate> & armors) const
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

    // ---- 绘制装甲板（W6: 支持多个） ----
    for (const auto & armor : armors) {
        if (!armor.detected) continue;
        cv::Scalar armor_color(0, 255, 255);

        for (int i = 0; i < 4; ++i) {
            cv::line(dbg, armor.corners[i], armor.corners[(i + 1) % 4], armor_color, 3);
        }
        cv::line(dbg, armor.corners[0], armor.corners[2], cv::Scalar(0, 255, 255), 1, cv::LINE_AA);
        cv::line(dbg, armor.corners[1], armor.corners[3], cv::Scalar(0, 255, 255), 1, cv::LINE_AA);
        cv::circle(dbg, armor.center, 8, cv::Scalar(0, 255, 255), -1);
    }

    // W7: 翻转文字——在小图上写字→旋转180°→贴到装甲板视觉左上角
    for (const auto & armor : armors) {
        if (!armor.detected) continue;
        std::string label = armor.color + "_" +
            (armor.digit.empty() ? "?" : armor.digit);
        cv::Mat text_mat(24, 140, CV_8UC3, cv::Scalar(0,0,0));
        cv::putText(text_mat, label, cv::Point(4, 18),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 2);
        cv::Mat text_flip;
        cv::flip(text_mat, text_flip, -1);
        // 找装甲板视觉左上角(min X, min Y)
        cv::Point2f tl = armor.corners[0];
        for (int i = 1; i < 4; ++i) {
            if (armor.corners[i].x < tl.x) tl.x = armor.corners[i].x;
            if (armor.corners[i].y < tl.y) tl.y = armor.corners[i].y;
        }
        int px = std::max(0, (int)tl.x - 5);
        int py = std::max(0, (int)tl.y - 28);
        cv::Rect roi(px, py, text_flip.cols, text_flip.rows);
        roi &= cv::Rect(0, 0, dbg.cols, dbg.rows);
        if (roi.width > 10 && roi.height > 10)
            text_flip(cv::Rect(0, 0, roi.width, roi.height)).copyTo(dbg(roi));
    }

    return dbg;
}

}  // namespace armor_detector
