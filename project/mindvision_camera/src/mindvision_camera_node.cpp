#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include "CameraApi.h"

class MindVisionCameraNode : public rclcpp::Node {
public:
    MindVisionCameraNode()
    : Node("mindvision_camera_node"), hdev_(-1),
      rgbBuf_(nullptr), rawBuf_(nullptr)
    {
        pub_ = create_publisher<sensor_msgs::msg::Image>("/image_raw", 10);

        if (!initCamera()) {
            RCLCPP_FATAL(this->get_logger(), "Camera init failed");
            return;
        }

        // 60fps（相机支持120fps，但检测算法需要时间，60fps平衡流畅度和性能）
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(16),
            std::bind(&MindVisionCameraNode::grab, this));

        RCLCPP_INFO(this->get_logger(), "✅ Camera node UP");
    }

    ~MindVisionCameraNode() {
        if (hdev_ != -1) {
            CameraStop(hdev_);
            CameraUnInit(hdev_);
            hdev_ = -1;
        }
        if (rgbBuf_) CameraAlignFree(rgbBuf_);
        if (rawBuf_) CameraAlignFree(rawBuf_);
    }

private:
    bool initCamera() {
    tSdkCameraDevInfo devs[4];
    int cnt = 4;
    CameraEnumerateDevice(devs, &cnt);
    if (!cnt) {
        RCLCPP_ERROR(this->get_logger(), "❌ No camera");
        return false;
    }

    int ret = CameraInit(&devs[0], -1, -1, &hdev_);
    if (ret != CAMERA_STATUS_SUCCESS) {
        RCLCPP_ERROR(this->get_logger(), "CameraInit err=%d", ret);
        return false;
    }

    CameraSetIspOutFormat(hdev_, CAMERA_MEDIA_TYPE_BGR8);

    // ✅ 强制关闭自动曝光，设置极低曝光
    CameraSetAeState(hdev_, FALSE);
    CameraSetExposureTime(hdev_, 3000);  // 3ms
    
    // ✅ 关闭自动增益（如果支持）
    // CameraSetAnalogGain(hdev_, 1.0);  // 最小增益
    // CameraSetDigitalGain(hdev_, 1.0); // 最小数字增益

    // 获取默认分辨率
    tSdkCameraCapbility cap;
    CameraGetCapability(hdev_, &cap);
    int w = cap.sResolutionRange.iWidthMax;
    int h = cap.sResolutionRange.iHeightMax;

    rgbBuf_ = (unsigned char*)CameraAlignMalloc(w * h * 3, 16);
    rawBuf_ = (unsigned char*)CameraAlignMalloc(w * h * 2, 16);

    CameraPlay(hdev_);
    return true;
}

    void grab() {
        tSdkFrameHead hdr;
        unsigned char* pRaw = nullptr;
        int r = CameraGetImageBuffer(hdev_, &hdr, &pRaw, 1000);
        if (r != CAMERA_STATUS_SUCCESS || !pRaw) return;

        CameraImageProcess(hdev_, pRaw, rgbBuf_, &hdr);

        // 相机ISP输出BGR8，直接使用
        cv::Mat bgr(hdr.iHeight, hdr.iWidth, CV_8UC3, rgbBuf_);

        auto msg = cv_bridge::CvImage(
            std_msgs::msg::Header(), "bgr8", bgr).toImageMsg();
        msg->header.stamp = now();
        pub_->publish(*msg);

        CameraReleaseImageBuffer(hdev_, pRaw);
    }

    int hdev_;
    unsigned char* rgbBuf_;
    unsigned char* rawBuf_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MindVisionCameraNode>());
    rclcpp::shutdown();
    return 0;
}