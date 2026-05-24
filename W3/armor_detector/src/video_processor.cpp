#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

using namespace cv;
using namespace std;

// 复制之前的函数声明和实现（从 armor_detector.cpp 中复制过来）
void colorSeparation(const Mat& bgr, Mat& redMask, Mat& blueMask) {
    Mat hsv;
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    Mat red1, red2;
    inRange(hsv, Scalar(0, 0, 50), Scalar(80, 200, 255), red1);
    inRange(hsv, Scalar(150, 0, 50), Scalar(180, 200, 255), red2);
    redMask = red1 | red2;
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
    if (area < 20 || area > 300) return false;
    if (aspect < 1.0 || aspect > 15.0) return false;
    float angle = rect.angle;
    if (angle > 45) angle = 90 - angle;
    if (fabs(angle) > 60) return false;
    return true;
}

bool pairLightBars(const RotatedRect& l1, const RotatedRect& l2, float& distance) {
    float dy = fabs(l1.center.y - l2.center.y);
    float dx = fabs(l1.center.x - l2.center.x);
    if (dy > 80) return false;
    if (dx < 15) return false;
    float h1 = max(l1.size.width, l1.size.height);
    float h2 = max(l2.size.width, l2.size.height);
    if (min(h1, h2) / max(h1, h2) < 0.3) return false;
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

// 单帧处理函数
void processFrame(const Mat& frame, Mat& output) {
    output = frame.clone();
    Mat redMask, blueMask;
    colorSeparation(frame, redMask, blueMask);
    morphologyProcess(redMask);
    morphologyProcess(blueMask);
    
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
    
    // 绘制红色候选（调试可选）
    for (auto& r : redBars) {
        Point2f pts[4]; r.points(pts);
        for (int i=0;i<4;i++) line(output, pts[i], pts[(i+1)%4], Scalar(0,0,255), 2);
    }
    for (auto& r : blueBars) {
        Point2f pts[4]; r.points(pts);
        for (int i=0;i<4;i++) line(output, pts[i], pts[(i+1)%4], Scalar(255,0,0), 2);
    }
    
    // 红色配对
    for (size_t i=0;i<redBars.size();i++) {
        for (size_t j=i+1;j<redBars.size();j++) {
            float dist;
            if (pairLightBars(redBars[i], redBars[j], dist)) {
                RotatedRect left = redBars[i].center.x < redBars[j].center.x ? redBars[i] : redBars[j];
                RotatedRect right = redBars[i].center.x < redBars[j].center.x ? redBars[j] : redBars[i];
                vector<Point2f> pts = getArmorPoints(left, right);
                vector<Point> intPts;
                for (auto& p : pts) intPts.push_back(Point(p.x,p.y));
                polylines(output, intPts, true, Scalar(0,0,255), 2);
                putText(output, "RED", pts[0], FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255), 2);
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
                vector<Point2f> pts = getArmorPoints(left, right);
                vector<Point> intPts;
                for (auto& p : pts) intPts.push_back(Point(p.x,p.y));
                polylines(output, intPts, true, Scalar(255,0,0), 2);
                putText(output, "BLUE", pts[0], FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0), 2);
            }
        }
    }
}

int main() {
    // 视频文件列表（请根据实际路径修改）
    vector<string> videoFiles = {
    "/home/tiexuejuan/rm-training-2026/W3/assets/videos/raw/Blue_1000.mp4",
    "/home/tiexuejuan/rm-training-2026/W3/assets/videos/raw/Blue_3000.mp4",
    "/home/tiexuejuan/rm-training-2026/W3/assets/videos/raw/Blue_5000.mp4",
    "/home/tiexuejuan/rm-training-2026/W3/assets/videos/raw/Red_1000.mp4",
    "/home/tiexuejuan/rm-training-2026/W3/assets/videos/raw/Red_3000.mp4",
    "/home/tiexuejuan/rm-training-2026/W3/assets/videos/raw/Red_5000.mp4"
};
    for (const auto& inputPath : videoFiles) {
        VideoCapture cap(inputPath);
        if (!cap.isOpened()) {
            cerr << "无法打开视频文件: " << inputPath << endl;
            continue;
        }
        // 获取视频属性
        double fps = cap.get(CAP_PROP_FPS);
        int width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
        int height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);
        // 生成输出文件名：在原文件名前加 "output_"
        size_t lastSlash = inputPath.find_last_of("/\\");
        string filename = inputPath.substr(lastSlash + 1);
        string outputPath = "output_" + filename;
        VideoWriter writer(outputPath, VideoWriter::fourcc('M','J','P','G'), fps, Size(width, height));
        if (!writer.isOpened()) {
            cerr << "无法创建输出视频文件: " << outputPath << endl;
            continue;
        }
        
        Mat frame, processed;
        int frameCount = 0;
        while (cap.read(frame)) {
            processFrame(frame, processed);
            writer.write(processed);
            frameCount++;
            if (frameCount % 30 == 0) cout << "处理 " << filename << ": " << frameCount << " 帧" << endl;
        }
        cout << "完成 " << filename << "，共 " << frameCount << " 帧，输出到 " << outputPath << endl;
        cap.release();
        writer.release();
    }
    return 0;
}