#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <vector>

namespace armor_detector
{

// 数字识别分类器 — OpenCV DNN 加载 ONNX
class DigitClassifier
{
public:
    static const std::vector<std::string> CLASS_NAMES;

    DigitClassifier();
    bool loadModel(const std::string & model_path);
    bool isLoaded() const { return loaded_; }

    // 输入：BGR原图 + 装甲板四角 → 输出：类别字符串（如"3","sentry"）
    std::string classify(const cv::Mat & bgr,
                         const cv::Point2f corners[4]);

private:
    cv::Mat extractROI(const cv::Mat & bgr,
                       const cv::Point2f corners[4]) const;
    cv::dnn::Net net_;
    bool loaded_ = false;
};

}  // namespace armor_detector
