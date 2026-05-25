#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "armor_detector.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "用法:" << endl;
        cout << "  处理单个视频: ./video_demo <视频路径> [输出路径]" << endl;
        cout << "  批量处理6个默认视频: ./video_demo --batch" << endl;
        return -1;
    }
    
    string mode = argv[1];
    if (mode == "--batch") {
        // 批量处理6个视频（相对于可执行文件的路径）
        vector<string> videos = {
            "assets/Blue_1000.mp4",
            "assets/Blue_3000.mp4",
            "assets/Blue_5000.mp4",
            "assets/Red_1000.mp4",
            "assets/Red_3000.mp4",
            "assets/Red_5000.mp4"
        };
        for (const auto& v : videos) {
            string outName = "output_" + v.substr(v.find_last_of("/\\") + 1);
            VideoCapture cap(v);
            if (!cap.isOpened()) {
                cerr << "跳过无法打开的视频: " << v << endl;
                continue;
            }
            double fps = cap.get(CAP_PROP_FPS);
            int width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
            int height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);
            VideoWriter writer(outName, VideoWriter::fourcc('M','J','P','G'), fps, Size(width, height));
            if (!writer.isOpened()) {
                cerr << "无法创建输出视频: " << outName << endl;
                continue;
            }
            Mat frame, outFrame;
            int cnt = 0;
            while (cap.read(frame)) {
                outFrame = frame.clone();
                vector<ArmorInfo> armors = detectArmor(frame);
                for (const auto& armor : armors) {
                    Scalar color = (armor.color == "RED") ? Scalar(0,0,255) : Scalar(255,0,0);
                    vector<Point> intPts;
                    for (const auto& p : armor.points) intPts.push_back(Point(p.x, p.y));
                    polylines(outFrame, intPts, true, color, 2);
                    putText(outFrame, armor.color, intPts[0], FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
                }
                writer.write(outFrame);
                cnt++;
                if (cnt % 30 == 0) cout << v << " 已处理 " << cnt << " 帧" << endl;
            }
            cout << "完成 " << v << "，总帧数 " << cnt << "，输出到 " << outName << endl;
            cap.release();
            writer.release();
        }
    } 
    else {
        // 单个视频模式
        string videoPath = argv[1];
        string outPath = (argc >= 3) ? argv[2] : "output_" + videoPath.substr(videoPath.find_last_of("/\\") + 1);
        VideoCapture cap(videoPath);
        if (!cap.isOpened()) {
            cerr << "无法打开视频: " << videoPath << endl;
            return -1;
        }
        double fps = cap.get(CAP_PROP_FPS);
        int width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
        int height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);
        VideoWriter writer(outPath, VideoWriter::fourcc('M','J','P','G'), fps, Size(width, height));
        if (!writer.isOpened()) {
            cerr << "无法创建输出视频: " << outPath << endl;
            return -1;
        }
        Mat frame, outFrame;
        int cnt = 0;
        while (cap.read(frame)) {
            outFrame = frame.clone();
            vector<ArmorInfo> armors = detectArmor(frame);
            for (const auto& armor : armors) {
                Scalar color = (armor.color == "RED") ? Scalar(0,0,255) : Scalar(255,0,0);
                vector<Point> intPts;
                for (const auto& p : armor.points) intPts.push_back(Point(p.x, p.y));
                polylines(outFrame, intPts, true, color, 2);
                putText(outFrame, armor.color, intPts[0], FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
            }
            writer.write(outFrame);
            cnt++;
        }
        cout << "完成，总帧数 " << cnt << "，输出到 " << outPath << endl;
        cap.release();
        writer.release();
    }
    return 0;
}