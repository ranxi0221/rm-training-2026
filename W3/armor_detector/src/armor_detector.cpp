#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

void colorSeparation(const Mat& bgr, Mat& redMask, Mat& blueMask) {
    Mat hsv;
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    // 红色：极宽松，V下限50
    Mat red1, red2;
    inRange(hsv, Scalar(0, 0, 50), Scalar(80, 200, 255), red1);
    inRange(hsv, Scalar(150, 0, 50), Scalar(180, 200, 255), red2);
    redMask = red1 | red2;
    // 蓝色：保持不变
    inRange(hsv, Scalar(80, 30, 150), Scalar(115, 200, 255), blueMask);
}

void morphologyProcess(Mat& mask) {
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
}

bool isLightBar(const RotatedRect& rect) {
    float w = rect.size.width, h = rect.size.height;
    if (w > h) swap(w, h);
    float aspect = h / w;
    float area = rect.size.area();
    // 非常宽松：只过滤明显不是灯条的区域（面积太大或太小）
    if (area < 10 || area > 1000) return false;
    if (aspect < 0.5 || aspect > 25) return false;
    float angle = rect.angle;
    if (angle > 45) angle = 90 - angle;
    if (fabs(angle) > 85) return false;
    return true;
}

bool pairLightBars(const RotatedRect& l1, const RotatedRect& l2, float& distance) {
    float dy = fabs(l1.center.y - l2.center.y);
    float dx = fabs(l1.center.x - l2.center.x);
    if (dy > 100) return false;
    if (dx < 10) return false;
    float h1 = max(l1.size.width, l1.size.height);
    float h2 = max(l2.size.width, l2.size.height);
    if (min(h1, h2) / max(h1, h2) < 0.2) return false;
    distance = dx;
    return true;
}

vector<Point2f> getArmorPoints(const RotatedRect& left, const RotatedRect& right) {
    Point2f leftPts[4], rightPts[4];
    left.points(leftPts);
    right.points(rightPts);
    vector<Point2f> all;
    for (int i=0;i<4;i++) all.push_back(leftPts[i]);
    for (int i=0;i<4;i++) all.push_back(rightPts[i]);
    sort(all.begin(), all.end(), [](const Point2f& a, const Point2f& b) { return a.x < b.x; });
    Point2f topLeft = all[0].y < all[1].y ? all[0] : all[1];
    Point2f bottomLeft = all[0].y < all[1].y ? all[1] : all[0];
    Point2f topRight = all[2].y < all[3].y ? all[2] : all[3];
    Point2f bottomRight = all[2].y < all[3].y ? all[3] : all[2];
    return {topLeft, topRight, bottomRight, bottomLeft};
}

int main() {
    string imgPath = "/home/tiexuejuan/rm-training-2026/W3/armor_detector/assets/armor.jpg";
    Mat img = imread(imgPath);
    if (img.empty()) { cout << "图片读取失败" << endl; return -1; }

    Mat redMask, blueMask;
    colorSeparation(img, redMask, blueMask);
    morphologyProcess(redMask);
    morphologyProcess(blueMask);
    imwrite("red_mask.png", redMask);
    imwrite("blue_mask.png", blueMask);

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

    // 输出红色候选信息
    cout << "红色候选个数: " << redBars.size() << endl;
    for (size_t i = 0; i < redBars.size(); ++i) {
        cout << "  area=" << redBars[i].size.area() << " at (" 
             << redBars[i].center.x << "," << redBars[i].center.y << ")" << endl;
    }

    // 过滤：保留面积在30-400之间的候选（去掉粉红色大块和极小噪点）
    vector<RotatedRect> filteredRed;
    for (auto& rect : redBars) {
        float a = rect.size.area();
        if (a >= 30 && a <= 400) filteredRed.push_back(rect);
    }
    redBars = filteredRed;
    cout << "过滤后红色候选个数: " << redBars.size() << endl;

    Mat candidateImg = img.clone();
    for (auto& r : redBars) {
        Point2f pts[4]; r.points(pts);
        for (int i=0;i<4;i++) line(candidateImg, pts[i], pts[(i+1)%4], Scalar(0,0,255),2);
    }
    for (auto& r : blueBars) {
        Point2f pts[4]; r.points(pts);
        for (int i=0;i<4;i++) line(candidateImg, pts[i], pts[(i+1)%4], Scalar(255,0,0),2);
    }
    imwrite("light_candidates.png", candidateImg);

    Mat result = img.clone();
    // 红色配对
    for (size_t i=0;i<redBars.size();i++) {
        for (size_t j=i+1;j<redBars.size();j++) {
            float dist;
            if (pairLightBars(redBars[i], redBars[j], dist)) {
                RotatedRect left = redBars[i].center.x < redBars[j].center.x ? redBars[i] : redBars[j];
                RotatedRect right = redBars[i].center.x < redBars[j].center.x ? redBars[j] : redBars[i];
                auto pts = getArmorPoints(left, right);
                vector<Point> intPts;
                for (auto& p : pts) intPts.push_back(Point(p.x,p.y));
                polylines(result, intPts, true, Scalar(0,0,255),2);
                putText(result, "RED", pts[0], FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,255),2);
            }
        }
    }
    // 蓝色配对
    for (size_t i=0;i<blueBars.size();i++) {
        for (size_t j=i+1;j<blueBars.size();j++) {
            float dist;
            if (pairLightBars(blueBars[i], blueBars[j], dist)) {
                RotatedRect left = blueBars[i].center.x < blueBars[j].center.x ? blueBars[i] : blueBars[j];
                RotatedRect right = blueBars[i].center.x < blueBars[j].center.x ? blueBars[j] : blueBars[i];
                auto pts = getArmorPoints(left, right);
                vector<Point> intPts;
                for (auto& p : pts) intPts.push_back(Point(p.x,p.y));
                polylines(result, intPts, true, Scalar(255,0,0),2);
                putText(result, "BLUE", pts[0], FONT_HERSHEY_SIMPLEX,0.5,Scalar(255,0,0),2);
            }
        }
    }
    imwrite("armor_result.png", result);
    cout << "完成" << endl;
    return 0;
}