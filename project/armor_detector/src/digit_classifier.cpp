#include "armor_detector/digit_classifier.hpp"

namespace armor_detector
{

const std::vector<std::string> DigitClassifier::CLASS_NAMES = {
    "1", "2", "3", "4", "5", "sentry", "outpost", "base", "not_armor"
};

DigitClassifier::DigitClassifier() {}

bool DigitClassifier::loadModel(const std::string & model_path)
{
    try { net_ = cv::dnn::readNet(model_path); loaded_ = !net_.empty(); return loaded_; }
    catch (...) { loaded_ = false; return false; }
}

cv::Mat DigitClassifier::extractROI(const cv::Mat & bgr,
                                     const cv::Point2f corners[4]) const
{
    float xm=corners[0].x,xM=corners[0].x,ym=corners[0].y,yM=corners[0].y;
    for(int i=1;i<4;i++){xm=std::min(xm,corners[i].x);xM=std::max(xM,corners[i].x);ym=std::min(ym,corners[i].y);yM=std::max(yM,corners[i].y);}
    cv::Rect bbox((int)xm,(int)ym,(int)(xM-xm),(int)(yM-ym));
    bbox &= cv::Rect(0,0,bgr.cols,bgr.rows);
    if(bbox.width<10||bbox.height<10) return cv::Mat::zeros(1,1,CV_32F);

    int cx=bbox.x+bbox.width/2, cy=bbox.y+bbox.height/2;
    float aspect=(float)bbox.width/std::max(1,bbox.height);
    int pct=(aspect>1.2f)?60:35;  // 长方形60%，正方形35%
    int cw=std::max(14,bbox.width*pct/100);
    int ch=std::max(14,bbox.height*85/100);
    cv::Rect inner(cx-cw/2,cy-ch/2,cw,ch);
    inner &= cv::Rect(0,0,bgr.cols,bgr.rows);
    if(inner.width<4||inner.height<4) return cv::Mat::zeros(1,1,CV_32F);

    cv::Mat gray;
    cv::cvtColor(bgr(inner),gray,cv::COLOR_BGR2GRAY);

    float s=std::min(32.0f/gray.cols,32.0f/gray.rows);
    int nw=std::max(1,(int)(gray.cols*s)), nh=std::max(1,(int)(gray.rows*s));
    cv::Mat rs; cv::resize(gray,rs,cv::Size(nw,nh));
    cv::Mat canvas=cv::Mat::zeros(32,32,CV_8UC1);
    rs.copyTo(canvas(cv::Rect((32-nw)/2,(32-nh)/2,nw,nh)));

    cv::Mat blob; canvas.convertTo(blob,CV_32F,1.0/255.0);
    return cv::dnn::blobFromImage(blob);
}

std::string DigitClassifier::classify(const cv::Mat & bgr,
                                       const cv::Point2f corners[4])
{
    if(!loaded_||bgr.empty()) return "";
    for(int i=0;i<4;i++) if(corners[i].x<1||corners[i].y<1||corners[i].x>bgr.cols-1||corners[i].y>bgr.rows-1) return "";

    auto infer=[&](const cv::Mat&blob)->std::pair<int,float>{
        if(blob.total()<=1) return {-1,-1e9f};
        try{ net_.setInput(blob); cv::Mat out=net_.forward();
            double mv; cv::Point ml; cv::minMaxLoc(out.reshape(1,1),nullptr,&mv,nullptr,&ml);
            return {ml.x,(float)mv}; } catch(...){ return {-1,-1e9f}; }
    };

    // 正向推理
    cv::Mat blob=extractROI(bgr,corners);
    auto[idx,conf]=infer(blob);

    // 180°翻转推理（解决数字方向倒置）
    if(blob.total()>1){
        cv::Mat gray; blob.reshape(1,32).convertTo(gray,CV_8U,255.0);
        cv::Mat flip; cv::flip(gray,flip,-1);
        cv::Mat f; flip.convertTo(f,CV_32F,1.0/255.0);
        auto[i2,c2]=infer(cv::dnn::blobFromImage(f));
        bool g1=(idx>=0&&idx<=4), g2=(i2>=0&&i2<=4);
        if(g2&&!g1){idx=i2;} else if(g1==g2&&c2>conf){idx=i2;}
    }

    if(idx>=0&&idx<(int)CLASS_NAMES.size()) return CLASS_NAMES[idx];
    return "";
}

}  // namespace armor_detector
