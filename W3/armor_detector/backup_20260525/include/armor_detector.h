#ifndef ARMOR_DETECTOR_H
#define ARMOR_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

// 装甲板检测结果结构体（规范明确，便于W4接入）
struct ArmorInfo {
    std::string color;                  // "RED" 或 "BLUE"
    std::vector<cv::Point2f> points;    // 四点顺序：左上、右上、右下、左下
    float confidence;                   // 置信度（暂用1.0）
};

// 算法模块：输入BGR图像，输出检测到的装甲板列表
std::vector<ArmorInfo> detectArmor(const cv::Mat& bgrImage);

#endif