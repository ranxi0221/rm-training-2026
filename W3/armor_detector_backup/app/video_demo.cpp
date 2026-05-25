#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "armor_detector.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "用法: ./video_demo <视频路径> [输出路径]" << endl;
        return -1;
    }
    string videoPath = argv[1];
    string outPath = (argc >= 3) ? argv[2] : "output_" + videoPath.substr(videoPath.find_last_of("/\\") + 1);
    
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
        for (const auto& armor : armors) {
            Scalar color = (armor.color == "RED") ? Scalar(0, 0, 255) : Scalar(255, 0, 0);
            vector<Point> intPts;
            for (const auto& p : armor.points) intPts.push_back(Point(p.x, p.y));
            polylines(outFrame, intPts, true, color, 2);
            putText(outFrame, armor.color, intPts[0], FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
        }
        writer.write(outFrame);
        frameCount++;
        if (frameCount % 30 == 0) {
            cout << "已处理 " << frameCount << " 帧" << endl;
        }
    }
    cout << "处理完成，总帧数 " << frameCount << "，输出到 " << outPath << endl;
    cap.release();
    writer.release();
    return 0;
}