#include <opencv2/opencv.hpp>
#include <iostream>
#include "armor_detector.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    // 支持命令行参数，默认可执行文件所在目录的 assets/armor.jpg
    string imgPath = (argc > 1) ? argv[1] : "assets/armor.jpg";
    Mat img = imread(imgPath);
    if (img.empty()) {
        cerr << "无法读取图片: " << imgPath << endl;
        return -1;
    }
    vector<ArmorInfo> armors = detectArmor(img);
    
    Mat result = img.clone();
    for (const auto& armor : armors) {
        Scalar color = (armor.color == "RED") ? Scalar(0,0,255) : Scalar(255,0,0);
        vector<Point> intPts;
        for (const auto& p : armor.points) intPts.push_back(Point(p.x, p.y));
        polylines(result, intPts, true, color, 2);
        putText(result, armor.color, intPts[0], FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
    }
    imwrite("armor_result.png", result);
    cout << "检测到 " << armors.size() << " 个装甲板，结果保存为 armor_result.png" << endl;
    return 0;
}