#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <std_msgs/msg/string.hpp>
#include <cv_bridge/cv_bridge.h>

#include "armor_detector/armor_detector.hpp"

// ============================================================================
// ROS2 装甲板检测节点
//
// 职责：
//   1. 订阅图像话题（sensor_msgs/Image，SensorDataQoS）
//   2. 从 YAML 参数文件读取所有可调参数
//   3. 调用 ArmorDetector 执行检测逻辑
//   4. 发布 /armor_result 结果话题
//   5. 发布 /armor_debug_image 调试图像话题
// ============================================================================
class ArmorDetectorNode : public rclcpp::Node
{
public:
    ArmorDetectorNode()
        : Node("armor_detector_node")
    {
        // ---- 声明参数（与 YAML 中的 key 一一对应） ----
        this->declare_parameter<std::string>("image_topic", "/image_raw");
        this->declare_parameter<std::string>("target_color", "red");

        // 预处理
        this->declare_parameter<double>("brightness_alpha", 0.3);
        this->declare_parameter<bool>("enable_clahe", true);
        this->declare_parameter<double>("clahe_clip", 2.0);
        this->declare_parameter<int>("clahe_tile", 8);

        // 红色 HSV
        this->declare_parameter<int>("red1_h_low", 0);
        this->declare_parameter<int>("red1_h_high", 10);
        this->declare_parameter<int>("red1_s_low", 100);
        this->declare_parameter<int>("red1_s_high", 255);
        this->declare_parameter<int>("red1_v_low", 50);
        this->declare_parameter<int>("red1_v_high", 255);

        this->declare_parameter<int>("red2_h_low", 170);
        this->declare_parameter<int>("red2_h_high", 179);
        this->declare_parameter<int>("red2_s_low", 100);
        this->declare_parameter<int>("red2_s_high", 255);
        this->declare_parameter<int>("red2_v_low", 50);
        this->declare_parameter<int>("red2_v_high", 255);

        // 蓝色 HSV
        this->declare_parameter<int>("blue_h_low", 95);
        this->declare_parameter<int>("blue_h_high", 130);
        this->declare_parameter<int>("blue_s_low", 80);
        this->declare_parameter<int>("blue_s_high", 255);
        this->declare_parameter<int>("blue_v_low", 50);
        this->declare_parameter<int>("blue_v_high", 255);

        // 形态学
        this->declare_parameter<int>("morph_kernel_size", 5);
        this->declare_parameter<bool>("enable_morph_open", true);
        this->declare_parameter<bool>("enable_morph_close", true);

        // 灯条筛选
        this->declare_parameter<double>("light_min_area", 20.0);
        this->declare_parameter<double>("light_max_area", 10000.0);
        this->declare_parameter<double>("light_max_ratio", 1.0);

        // 装甲板配对
        this->declare_parameter<double>("pair_max_dy", 50.0);
        this->declare_parameter<double>("pair_min_dx", 20.0);
        this->declare_parameter<double>("pair_max_dx", 800.0);

        // 时序平滑
        this->declare_parameter<double>("smooth_alpha", 0.7);

        // 调试
        this->declare_parameter<bool>("enable_debug", true);

        // ---- 加载参数到检测器 ----
        loadParams();

        // ---- 创建订阅（SensorDataQoS） ----
        std::string image_topic = this->get_parameter("image_topic").as_string();
        sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            image_topic,
            rclcpp::SensorDataQoS(),
            std::bind(&ArmorDetectorNode::imageCb, this, std::placeholders::_1));

        // ---- 创建发布 ----
        pub_result_ = this->create_publisher<std_msgs::msg::String>("/armor_result", 10);
        pub_debug_ = this->create_publisher<sensor_msgs::msg::Image>("/armor_debug_image", 10);

        RCLCPP_INFO(this->get_logger(),
            "ArmorDetectorNode started | image: %s | target: %s",
            image_topic.c_str(),
            this->get_parameter("target_color").as_string().c_str());
    }

private:
    // ========================================================================
    // 从 ROS 参数加载到 DetectorParams
    // ========================================================================
    void loadParams()
    {
        armor_detector::DetectorParams p;

        p.brightness_alpha = this->get_parameter("brightness_alpha").as_double();
        p.enable_clahe     = this->get_parameter("enable_clahe").as_bool();
        p.clahe_clip       = this->get_parameter("clahe_clip").as_double();
        p.clahe_tile       = this->get_parameter("clahe_tile").as_int();

        p.red1_h_low  = this->get_parameter("red1_h_low").as_int();
        p.red1_h_high = this->get_parameter("red1_h_high").as_int();
        p.red1_s_low  = this->get_parameter("red1_s_low").as_int();
        p.red1_s_high = this->get_parameter("red1_s_high").as_int();
        p.red1_v_low  = this->get_parameter("red1_v_low").as_int();
        p.red1_v_high = this->get_parameter("red1_v_high").as_int();

        p.red2_h_low  = this->get_parameter("red2_h_low").as_int();
        p.red2_h_high = this->get_parameter("red2_h_high").as_int();
        p.red2_s_low  = this->get_parameter("red2_s_low").as_int();
        p.red2_s_high = this->get_parameter("red2_s_high").as_int();
        p.red2_v_low  = this->get_parameter("red2_v_low").as_int();
        p.red2_v_high = this->get_parameter("red2_v_high").as_int();

        p.blue_h_low  = this->get_parameter("blue_h_low").as_int();
        p.blue_h_high = this->get_parameter("blue_h_high").as_int();
        p.blue_s_low  = this->get_parameter("blue_s_low").as_int();
        p.blue_s_high = this->get_parameter("blue_s_high").as_int();
        p.blue_v_low  = this->get_parameter("blue_v_low").as_int();
        p.blue_v_high = this->get_parameter("blue_v_high").as_int();

        p.morph_kernel_size  = this->get_parameter("morph_kernel_size").as_int();
        p.enable_morph_open  = this->get_parameter("enable_morph_open").as_bool();
        p.enable_morph_close = this->get_parameter("enable_morph_close").as_bool();

        p.light_min_area  = this->get_parameter("light_min_area").as_double();
        p.light_max_area  = this->get_parameter("light_max_area").as_double();
        p.light_max_ratio = this->get_parameter("light_max_ratio").as_double();

        p.pair_max_dy = this->get_parameter("pair_max_dy").as_double();
        p.pair_min_dx = this->get_parameter("pair_min_dx").as_double();
        p.pair_max_dx = this->get_parameter("pair_max_dx").as_double();

        p.smooth_alpha = this->get_parameter("smooth_alpha").as_double();

        detector_.setParams(p);
    }

    // ========================================================================
    // 图像回调
    // ========================================================================
    void imageCb(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        // 1. ROS Image → OpenCV BGR
        cv::Mat bgr;
        try {
            bgr = cv_bridge::toCvCopy(msg, "bgr8")->image;
        } catch (const std::exception & e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge error: %s", e.what());
            return;
        }

        if (bgr.empty()) {
            return;
        }

        // 2. 调用检测器
        std::string target = this->get_parameter("target_color").as_string();
        auto armor = detector_.detect(bgr, target);

        // 3. 发布结果
        std_msgs::msg::String res;
        if (armor.detected) {
            res.data = "armor_detected color=" + armor.color +
                       " center=(" +
                       std::to_string(static_cast<int>(armor.center.x)) + "," +
                       std::to_string(static_cast<int>(armor.center.y)) + ")";
        } else {
            res.data = "no_armor";
        }
        pub_result_->publish(res);

        // 4. 调试图像
        if (this->get_parameter("enable_debug").as_bool()) {
            // 收集当前帧的灯条信息用于绘制
            // 注意：detect 内部没有暴露 light_bars，这里用单独的 extract 不太划算。
            // 改一下接口：detect 返回时同时输出 light_bars 列表。
            // 当前方案：detect() 一次调用完成检测+绘制更简洁，
            // 但为保持接口干净，这里我们用 detector 内部存储的 light_bars。
            //
            // 实际做法：ArmorDetector 的 detect() 现在也绘制并返回 debug 图。
            // 但 drawDebug 需要 light_bars 列表。折中方案是让 detect 内部保存
            // light_bars 到成员变量，供 drawDebug 使用。
            //
            // 或者更简单的：detect 返回后，用 detector.getLastLightBars() 获取。
            //
            // 为了最小化修改，这里让 detect() 返回 armor，但同时把 light_bars
            // 存入 detector_ 成员。drawDebug 从成员读取。
            auto dbg = detector_.drawDebug(bgr, detector_.getLastLightBars(), armor);
            pub_debug_->publish(
                *cv_bridge::CvImage(msg->header, "bgr8", dbg).toImageMsg());
        }

        // 5. 日志（限流）
        RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000,
            "Lights: %zu | Armor: %s",
            detector_.getLastLightBars().size(),
            armor.detected ? (armor.color + " detected").c_str() : "none");
    }

    // ---- 成员变量 ----
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_result_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_debug_;

    armor_detector::ArmorDetector detector_;
};

// ============================================================================
int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ArmorDetectorNode>());
    rclcpp::shutdown();
    return 0;
}
