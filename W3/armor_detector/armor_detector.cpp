#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

using namespace cv;
using namespace std;

struct ArmorInfo {
    string color;
    vector<Point2f> points;
    float confidence;
};

// ------------------------- 图片模式（稳定参数，不要改动） -------------------------
void getMasksImage(const Mat& bgr, Mat& redMask, Mat& blueMask) {
    Mat hsv;
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    // 红色
    Mat red1, red2;
    inRange(hsv, Scalar(0, 0, 100), Scalar(40, 200, 255), red1);
    inRange(hsv, Scalar(150, 0, 100), Scalar(180, 200, 255), red2);
    redMask = red1 | red2;
    // 蓝色
    inRange(hsv, Scalar(80, 30, 150), Scalar(115, 200, 255), blueMask);
}

void morphImage(Mat& mask) {
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
}

bool isLightBarImage(const RotatedRect& rect) {
    float w = rect.size.width, h = rect.size.height;
    if (w > h) swap(w, h);
    float aspect = h / w;
    float area = rect.size.area();
    if (area < 20 || area > 400) return false;
    if (aspect < 1.2 || aspect > 12.0) return false;
    float angle = rect.angle;
    if (angle > 45) angle = 90 - angle;
    if (fabs(angle) > 60) return false;
    return true;
}

bool pairLightBarsImage(const RotatedRect& l1, const RotatedRect& l2, float& distance) {
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

vector<Point2f> getOrderedPoints(const RotatedRect& left, const RotatedRect& right) {
    Point2f leftPts[4], rightPts[4];
    left.points(leftPts);
    right.points(rightPts);
    vector<Point2f> all;
    for (int i=0;i<4;i++) all.push_back(leftPts[i]);
    for (int i=0;i<4;i++) all.push_back(rightPts[i]);
    sort(all.begin(), all.end(), [](Point2f a, Point2f b){return a.x<b.x;});
    Point2f topLeft = all[0].y < all[1].y ? all[0] : all[1];
    Point2f bottomLeft = all[0].y < all[1].y ? all[1] : all[0];
    Point2f topRight = all[2].y < all[3].y ? all[2] : all[3];
    Point2f bottomRight = all[2].y < all[3].y ? all[3] : all[2];
    return {topLeft, topRight, bottomRight, bottomLeft};
}

vector<ArmorInfo> detectImage(const Mat& bgr) {
    vector<ArmorInfo> results;
    Mat redMask, blueMask;
    getMasksImage(bgr, redMask, blueMask);
    morphImage(redMask);
    morphImage(blueMask);
    
    vector<vector<Point>> redContours, blueContours;
    findContours(redMask, redContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    findContours(blueMask, blueContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
    vector<RotatedRect> redBars, blueBars;
    for (auto& c : redContours) {
        RotatedRect rect = minAreaRect(c);
        if (isLightBarImage(rect)) redBars.push_back(rect);
    }
    for (auto& c : blueContours) {
        RotatedRect rect = minAreaRect(c);
        if (isLightBarImage(rect)) blueBars.push_back(rect);
    }
    
    // 红色配对
    for (size_t i=0;i<redBars.size();i++) {
        for (size_t j=i+1;j<redBars.size();j++) {
            float dist;
            if (pairLightBarsImage(redBars[i], redBars[j], dist)) {
                RotatedRect left = redBars[i].center.x < redBars[j].center.x ? redBars[i] : redBars[j];
                RotatedRect right = redBars[i].center.x < redBars[j].center.x ? redBars[j] : redBars[i];
                vector<Point2f> pts = getOrderedPoints(left, right);
                results.push_back({"RED", pts, 1.0f});
            }
        }
    }
    // 蓝色配对
    for (size_t i=0;i<blueBars.size();i++) {
        for (size_t j=i+1;j<blueBars.size();j++) {
            float dist;
            if (pairLightBarsImage(blueBars[i], blueBars[j], dist)) {
                RotatedRect left = blueBars[i].center.x < blueBars[j].center.x ? blueBars[i] : blueBars[j];
                RotatedRect right = blueBars[i].center.x < blueBars[j].center.x ? blueBars[j] : blueBars[i];
                vector<Point2f> pts = getOrderedPoints(left, right);
                results.push_back({"BLUE", pts, 1.0f});
            }
        }
    }
    return results;
}

// ------------------------- 视频模式（针对白色/暗色灯条，极度宽松，加入伽马校正） -------------------------
Mat gammaCorrection(const Mat& src, float gamma = 1.5) {
    Mat dst;
    src.convertTo(dst, CV_32F, 1.0/255.0);
    pow(dst, gamma, dst);
    dst.convertTo(dst, CV_8U, 255.0);
    return dst;
}

void getMasksVideo(const Mat& bgr, Mat& mask, const string& color) {
    // 先做伽马校正，增强暗部
    Mat enhanced = gammaCorrection(bgr);
    Mat hsv;
    cvtColor(enhanced, hsv, COLOR_BGR2HSV);
    
    if (color == "RED") {
        Mat red1, red2;
        // 极宽松参数：H 0-60 和 150-180，S 0-200，V 30-255
        inRange(hsv, Scalar(0, 0, 30), Scalar(60, 200, 255), red1);
        inRange(hsv, Scalar(150, 0, 30), Scalar(180, 200, 255), red2);
        mask = red1 | red2;
    } else if (color == "BLUE") {
        // 蓝色：H 80-120，S 0-255，V 30-255
        inRange(hsv, Scalar(80, 0, 30), Scalar(120, 255, 255), mask);
    }
}

void morphVideo(Mat& mask) {
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
}

bool isLightBarVideo(const RotatedRect& rect) {
    float w = rect.size.width, h = rect.size.height;
    if (w > h) swap(w, h);
    float aspect = h / w;
    float area = rect.size.area();
    if (area < 5 || area > 1000) return false;
    if (aspect < 0.5 || aspect > 20) return false;
    float angle = rect.angle;
    if (angle > 45) angle = 90 - angle;
    if (fabs(angle) > 80) return false;
    return true;
}

bool pairLightBarsVideo(const RotatedRect& l1, const RotatedRect& l2, float& distance) {
    float dy = fabs(l1.center.y - l2.center.y);
    float dx = fabs(l1.center.x - l2.center.x);
    if (dy > 80) return false;      // 垂直偏差过大
    if (dx < 20) return false;      // 水平距离过小
    if (dx > 200) return false;     // 水平距离过大（可选）
    float h1 = max(l1.size.width, l1.size.height);
    float h2 = max(l2.size.width, l2.size.height);
    if (min(h1, h2) / max(h1, h2) < 0.5) return false; // 高度相似度提高
    distance = dx;
    return true;
}


vector<ArmorInfo> detectVideo(const Mat& bgr, const string& color) {
    vector<ArmorInfo> results;
    Mat mask;
    getMasksVideo(bgr, mask, color);
    morphVideo(mask);
    
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<RotatedRect> bars;
    for (auto& c : contours) {
        RotatedRect rect = minAreaRect(c);
        if (isLightBarVideo(rect)) bars.push_back(rect);
    }
    for (size_t i=0; i<bars.size(); i++) {
        for (size_t j=i+1; j<bars.size(); j++) {
            float dist;
            if (pairLightBarsVideo(bars[i], bars[j], dist)) {
                RotatedRect left = bars[i].center.x < bars[j].center.x ? bars[i] : bars[j];
                RotatedRect right = bars[i].center.x < bars[j].center.x ? bars[j] : bars[i];
                vector<Point2f> pts = getOrderedPoints(left, right);
                results.push_back({color, pts, 1.0f});
            }
        }
    }
    return results;
}

void drawResult(Mat& img, const vector<ArmorInfo>& armors) {
    for (const auto& armor : armors) {
        Scalar color = (armor.color == "RED") ? Scalar(0,0,255) : Scalar(255,0,0);
        vector<Point> intPts;
        for (const auto& p : armor.points) intPts.push_back(Point(p.x, p.y));
        polylines(img, intPts, true, color, 2);
        putText(img, armor.color, intPts[0], FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "用法:\n  图片: ./armor_detector <图片路径>\n  批量视频: ./armor_detector --batch" << endl;
        return -1;
    }
    
    if (string(argv[1]) == "--batch") {
        vector<pair<string, string>> videos = {
            {"assets/Blue_1000.mp4", "BLUE"},
            {"assets/Blue_3000.mp4", "BLUE"},
            {"assets/Blue_5000.mp4", "BLUE"},
            {"assets/Red_1000.mp4", "RED"},
            {"assets/Red_3000.mp4", "RED"},
            {"assets/Red_5000.mp4", "RED"}
        };
        for (const auto& v : videos) {
            VideoCapture cap(v.first);
            if (!cap.isOpened()) {
                cerr << "无法打开视频: " << v.first << endl;
                continue;
            }
            double fps = cap.get(CAP_PROP_FPS);
            int w = cap.get(CAP_PROP_FRAME_WIDTH), h = cap.get(CAP_PROP_FRAME_HEIGHT);
            string outName = "output_" + v.first.substr(v.first.find_last_of("/\\") + 1);
            VideoWriter writer(outName, VideoWriter::fourcc('M','J','P','G'), fps, Size(w,h));
            if (!writer.isOpened()) {
                cerr << "无法创建输出视频: " << outName << endl;
                continue;
            }
            Mat frame, outFrame;
            int cnt = 0;
            while (cap.read(frame)) {
                outFrame = frame.clone();
                vector<ArmorInfo> armors = detectVideo(frame, v.second);
                drawResult(outFrame, armors);
                writer.write(outFrame);
                cnt++;
                if (cnt % 30 == 0) {
                    cout << v.first << " 帧 " << cnt << " 检测到 " << armors.size() << " 个装甲板" << endl;
                }
            }
            cout << "完成 " << v.first << "，总帧数 " << cnt << "，输出至 " << outName << endl;
        }
    } else {
        Mat img = imread(argv[1]);
        if (img.empty()) {
            cerr << "无法读取图片: " << argv[1] << endl;
            return -1;
        }
        vector<ArmorInfo> armors = detectImage(img);
        Mat result = img.clone();
        drawResult(result, armors);
        imwrite("armor_result.png", result);
        cout << "检测到 " << armors.size() << " 个装甲板" << endl;
    }
    return 0;
}