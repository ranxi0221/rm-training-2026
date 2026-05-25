#ifndef ARMOR_DETECTOR_H
#define ARMOR_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

struct ArmorInfo {
    std::string color;
    std::vector<cv::Point2f> points;  // 左上、右上、右下、左下
    float confidence;
};

std::vector<ArmorInfo> detectArmor(const cv::Mat& bgrImage);

#endif