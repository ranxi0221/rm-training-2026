#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

using namespace cv;
using namespace std;

// ================== 装甲板信息结构体 ==================
struct ArmorInfo {
    string color;                   // "RED" 或 "BLUE"
    vector<Point2f> points;         // 四点顺序：左上、右上、右下、左下
    float confidence;
};

// ================== 检测函数（参数基于您之前成功的数据） ==================
vector<ArmorInfo> detectArmor(const Mat& bgr) {
    vector<ArmorInfo> results;
    Mat hsv;
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    
    // 颜色分离（根据您的采样优化）
    Mat redMask, blueMask;
    // 红色：H 0-80 和 150-180，S 0-200，V 50-255（宽范围）
    Mat red1, red2;
    inRange(hsv, Scalar(0, 0, 50), Scalar(80, 200, 255), red1);
    inRange(hsv, Scalar(150, 0, 50), Scalar(180, 200, 255), red2);
    redMask = red1 | red2;
    // 蓝色：H 80-115，S 30-200，V 150-255
    inRange(hsv, Scalar(80, 30, 150), Scalar(115, 200, 255), blueMask);
    
    // 形态学处理
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));
    morphologyEx(redMask, redMask, MORPH_CLOSE, kernel);
    morphologyEx(redMask, redMask, MORPH_CLOSE, kernel);
    morphologyEx(redMask, redMask, MORPH_OPEN, kernel);
    morphologyEx(blueMask, blueMask, MORPH_CLOSE, kernel);
    morphologyEx(blueMask, blueMask, MORPH_CLOSE, kernel);
    morphologyEx(blueMask, blueMask, MORPH_OPEN, kernel);
    
    // 找轮廓
    vector<vector<Point>> redContours, blueContours;
    findContours(redMask, redContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    findContours(blueMask, blueContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
    // 灯条筛选（基于您的数据：蓝色面积80左右，红色面积120左右）
    auto isLightBar = [](const RotatedRect& rect) -> bool {
        float w = rect.size.width, h = rect.size.height;
        if (w > h) swap(w, h);
        float aspect = h / w;
        float area = rect.size.area();
        if (area < 15 || area > 400) return false;   // 放宽到15-400
        if (aspect < 1.0 || aspect > 15.0) return false;
        float angle = rect.angle;
        if (angle > 45) angle = 90 - angle;
        if (fabs(angle) > 70) return false;
        return true;
    };
    
    // 灯条配对
    auto pairLightBars = [](const RotatedRect& l1, const RotatedRect& l2, float& distance) -> bool {
        float dy = fabs(l1.center.y - l2.center.y);
        float dx = fabs(l1.center.x - l2.center.x);
        if (dy > 100) return false;
        if (dx < 15) return false;
        float h1 = max(l1.size.width, l1.size.height);
        float h2 = max(l2.size.width, l2.size.height);
        if (min(h1, h2) / max(h1, h2) < 0.2) return false;
        distance = dx;
        return true;
    };
    
    // 四点排序
    auto getOrderedPoints = [](const RotatedRect& left, const RotatedRect& right) -> vector<Point2f> {
        Point2f leftPts[4], rightPts[4];
        left.points(leftPts);
        right.points(rightPts);
        vector<Point2f> all;
        for (int i = 0; i < 4; i++) all.push_back(leftPts[i]);
        for (int i = 0; i < 4; i++) all.push_back(rightPts[i]);
        sort(all.begin(), all.end(), [](const Point2f& a, const Point2f& b) { return a.x < b.x; });
        Point2f topLeft = all[0].y < all[1].y ? all[0] : all[1];
        Point2f bottomLeft = all[0].y < all[1].y ? all[1] : all[0];
        Point2f topRight = all[2].y < all[3].y ? all[2] : all[3];
        Point2f bottomRight = all[2].y < all[3].y ? all[3] : all[2];
        return {topLeft, topRight, bottomRight, bottomLeft};
    };
    
    // 提取候选
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
    for (size_t i = 0; i < redBars.size(); i++) {
        for (size_t j = i+1; j < redBars.size(); j++) {
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
    for (size_t i = 0; i < blueBars.size(); i++) {
        for (size_t j = i+1; j < blueBars.size(); j++) {
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

// ================== 绘制结果 ==================
void drawResult(Mat& img, const vector<ArmorInfo>& armors) {
    for (const auto& armor : armors) {
        Scalar color = (armor.color == "RED") ? Scalar(0, 0, 255) : Scalar(255, 0, 0);
        vector<Point> intPts;
        for (const auto& p : armor.points) intPts.push_back(Point(p.x, p.y));
        polylines(img, intPts, true, color, 2);
        putText(img, armor.color, intPts[0], FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
    }
}

// ================== 主函数 ==================
int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "用法:" << endl;
        cout << "  图片: ./armor_detector <图片路径>" << endl;
        cout << "  视频: ./armor_detector --video <视频路径> [输出路径]" << endl;
        cout << "  批量: ./armor_detector --batch" << endl;
        return -1;
    }
    
    string mode = argv[1];
    if (mode == "--video" && argc >= 3) {
        string videoPath = argv[2];
        string outPath = (argc >= 4) ? argv[3] : "output_" + videoPath.substr(videoPath.find_last_of("/\\") + 1);
        VideoCapture cap(videoPath);
        if (!cap.isOpened()) {
            cerr << "无法打开视频: " << videoPath << endl;
            return -1;
        }
        double fps = cap.get(CAP_PROP_FPS);
        int width = cap.get(CAP_PROP_FRAME_WIDTH);
        int height = cap.get(CAP_PROP_FRAME_HEIGHT);
        VideoWriter writer(outPath, VideoWriter::fourcc('M','J','P','G'), fps, Size(width, height));
        if (!writer.isOpened()) {
            cerr << "无法创建输出视频: " << outPath << endl;
            return -1;
        }
        Mat frame, outFrame;
        int frameCount = 0;
        while (cap.read(frame)) {
            outFrame = frame.clone();
            vector<ArmorInfo> armors = detectArmor(frame);
            // 每30帧打印一次，避免刷屏
            if (frameCount % 30 == 0) {
                cout << "帧 " << frameCount << " 检测到 " << armors.size() << " 个装甲板" << endl;
            }
            drawResult(outFrame, armors);
            writer.write(outFrame);
            frameCount++;
        }
        cout << "处理完成，总帧数 " << frameCount << "，输出到 " << outPath << endl;
        cap.release();
        writer.release();
    } 
    else if (mode == "--batch") {
        // 相对路径：视频放在 ../assets/videos/raw/ 下
        vector<string> videos = {
            "../assets/videos/raw/Blue_1000.mp4",
            "../assets/videos/raw/Blue_3000.mp4",
            "../assets/videos/raw/Blue_5000.mp4",
            "../assets/videos/raw/Red_1000.mp4",
            "../assets/videos/raw/Red_3000.mp4",
            "../assets/videos/raw/Red_5000.mp4"
        };
        for (const auto& v : videos) {
            string outName = "output_" + v.substr(v.find_last_of("/\\") + 1);
            VideoCapture cap(v);
            if (!cap.isOpened()) {
                cerr << "跳过无法打开的视频: " << v << endl;
                continue;
            }
            double fps = cap.get(CAP_PROP_FPS);
            int width = cap.get(CAP_PROP_FRAME_WIDTH);
            int height = cap.get(CAP_PROP_FRAME_HEIGHT);
            VideoWriter writer(outName, VideoWriter::fourcc('M','J','P','G'), fps, Size(width, height));
            Mat frame, outFrame;
            int cnt = 0;
            while (cap.read(frame)) {
                outFrame = frame.clone();
                vector<ArmorInfo> armors = detectArmor(frame);
                if (cnt % 30 == 0) {
                    cout << v << " 帧 " << cnt << " 检测到 " << armors.size() << " 个装甲板" << endl;
                }
                drawResult(outFrame, armors);
                writer.write(outFrame);
                cnt++;
            }
            cout << "完成 " << v << "，总帧数 " << cnt << "，输出到 " << outName << endl;
        }
    }
    else {
        // 图片模式
        string imgPath = argv[1];
        Mat img = imread(imgPath);
        if (img.empty()) {
            cerr << "无法读取图片: " << imgPath << endl;
            return -1;
        }
        vector<ArmorInfo> armors = detectArmor(img);
        Mat result = img.clone();
        drawResult(result, armors);
        string outPath = (argc >= 3) ? argv[2] : "armor_result.png";
        imwrite(outPath, result);
        cout << "检测到 " << armors.size() << " 个装甲板，结果保存至 " << outPath << endl;
    }
    return 0;
}