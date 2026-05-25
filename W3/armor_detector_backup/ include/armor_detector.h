#ifndef ARMOR_DETECTOR_H
#define ARMOR_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

struct ArmorInfo {
    std::string color;                     // "RED" 或 "BLUE"
    std::vector<cv::Point2f> points;       // 四点顺序：左上、右上、右下、左下
    float confidence;                      // 置信度（暂用1.0）
};

// 主检测接口：输入BGR图像，返回装甲板信息列表
std::vector<ArmorInfo> detectArmor(const cv::Mat& bgr);

#endif