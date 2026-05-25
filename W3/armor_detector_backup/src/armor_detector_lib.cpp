#include "armor_detector.h"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <algorithm>

using namespace cv;
using namespace std;

// 颜色分离（HSV阈值）
static void colorSeparation(const Mat& bgr, Mat& redMask, Mat& blueMask) {
    Mat hsv;
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    // 红色：H 0-80 和 150-180，S 0-200，V 50-255（宽范围，捕捉荧光红）
    Mat red1, red2;
    inRange(hsv, Scalar(0, 0, 50), Scalar(80, 200, 255), red1);
    inRange(hsv, Scalar(150, 0, 50), Scalar(180, 200, 255), red2);
    redMask = red1 | red2;
    // 蓝色：H 80-115，S 30-200，V 150-255（稳定）
    inRange(hsv, Scalar(80, 30, 150), Scalar(115, 200, 255), blueMask);
}

// 形态学处理（闭运算填充断裂，开运算去噪）
static void morphologyProcess(Mat& mask) {
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
}

// 灯条筛选（基于面积、长宽比、角度）
static bool isLightBar(const RotatedRect& rect) {
    float w = rect.size.width, h = rect.size.height;
    if (w > h) swap(w, h);
    float aspect = h / w;
    float area = rect.size.area();
    // 根据您之前的数据：蓝色面积约80，红色约120，范围15-400足够
    if (area < 15 || area > 400) return false;
    if (aspect < 1.0 || aspect > 15.0) return false;
    float angle = rect.angle;
    if (angle > 45) angle = 90 - angle;
    if (fabs(angle) > 70) return false;
    return true;
}

// 灯条配对（左右配对，垂直偏差，水平距离，高度相似）
static bool pairLightBars(const RotatedRect& l1, const RotatedRect& l2, float& distance) {
    float dy = fabs(l1.center.y - l2.center.y);
    float dx = fabs(l1.center.x - l2.center.x);
    if (dy > 100) return false;
    if (dx < 15) return false;
    float h1 = max(l1.size.width, l1.size.height);
    float h2 = max(l2.size.width, l2.size.height);
    if (min(h1, h2) / max(h1, h2) < 0.2) return false;
    distance = dx;
    return true;
}

// 根据左右灯条获得四个有序角点（左上、右上、右下、左下）
static vector<Point2f> getOrderedPoints(const RotatedRect& left, const RotatedRect& right) {
    Point2f leftPts[4], rightPts[4];
    left.points(leftPts);
    right.points(rightPts);
    vector<Point2f> all;
    for (int i = 0; i < 4; ++i) all.push_back(leftPts[i]);
    for (int i = 0; i < 4; ++i) all.push_back(rightPts[i]);
    // 按x坐标排序
    sort(all.begin(), all.end(), [](const Point2f& a, const Point2f& b) { return a.x < b.x; });
    // 最左边两个点，按y排序
    Point2f topLeft = all[0].y < all[1].y ? all[0] : all[1];
    Point2f bottomLeft = all[0].y < all[1].y ? all[1] : all[0];
    // 最右边两个点，按y排序
    Point2f topRight = all[2].y < all[3].y ? all[2] : all[3];
    Point2f bottomRight = all[2].y < all[3].y ? all[3] : all[2];
    return {topLeft, topRight, bottomRight, bottomLeft};
}

// 主检测函数
vector<ArmorInfo> detectArmor(const Mat& bgr) {
    vector<ArmorInfo> results;
    Mat redMask, blueMask;
    colorSeparation(bgr, redMask, blueMask);
    morphologyProcess(redMask);
    morphologyProcess(blueMask);
    
    // 可选：保存中间结果（用于调试，不强制）
    // imwrite("debug_red_mask.png", redMask);
    // imwrite("debug_blue_mask.png", blueMask);
    
    vector<vector<Point>> redContours, blueContours;
    findContours(redMask, redContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    findContours(blueMask, blueContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
    vector<RotatedRect> redBars, blueBars;
    for (auto& c : redContours) {
        RotatedRect rect = minAreaRect(c);
        if (isLightBar(rect)) redBars.push_back(rect);
    }
    for (auto& c : blueContours) {
        RotatedRect rect = minAreaRect(c);
        if (isLightBar(rect)) blueBars.push_back(rect);
    }
    
    // 红色配对
    for (size_t i = 0; i < redBars.size(); ++i) {
        for (size_t j = i+1; j < redBars.size(); ++j) {
            float dist;
            if (pairLightBars(redBars[i], redBars[j], dist)) {
                RotatedRect left = redBars[i].center.x < redBars[j].center.x ? redBars[i] : redBars[j];
                RotatedRect right = redBars[i].center.x < redBars[j].center.x ? redBars[j] : redBars[i];
                vector<Point2f> pts = getOrderedPoints(left, right);
                results.push_back({"RED", pts, 1.0f});
            }
        }
    }
    // 蓝色配对
    for (size_t i = 0; i < blueBars.size(); ++i) {
        for (size_t j = i+1; j < blueBars.size(); ++j) {
            float dist;
            if (pairLightBars(blueBars[i], blueBars[j], dist)) {
                RotatedRect left = blueBars[i].center.x < blueBars[j].center.x ? blueBars[i] : blueBars[j];
                RotatedRect right = blueBars[i].center.x < blueBars[j].center.x ? blueBars[j] : blueBars[i];
                vector<Point2f> pts = getOrderedPoints(left, right);
                results.push_back({"BLUE", pts, 1.0f});
            }
        }
    }
    return results;
}