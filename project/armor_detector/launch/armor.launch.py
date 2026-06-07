from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration, PythonExpression
from launch.actions import DeclareLaunchArgument, LogInfo, ExecuteProcess, TimerAction
from launch.conditions import IfCondition, UnlessCondition
import os
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # 参数文件路径
    pkg_share = get_package_share_directory('armor_detector')
    default_params = os.path.join(pkg_share, 'config', 'armor_params.yaml')

    # ---- 可命令行覆盖的参数 ----
    # 默认 bag 模式：只编译 armor_detector 就能跑，不依赖 mindvision_camera
    input_source = LaunchConfiguration('input_source', default='bag')
    target_color = LaunchConfiguration('target_color', default='red')
    image_topic  = LaunchConfiguration('image_topic',  default='/image_raw')
    bag_path     = LaunchConfiguration('bag_path',     default='')
    enable_debug = LaunchConfiguration('enable_debug', default='true')

    # ---- 条件判断 ----
    is_camera = PythonExpression(["'", input_source, "' == 'camera'"])
    is_bag    = PythonExpression(["'", input_source, "' == 'bag'"])
    has_bag   = PythonExpression(["'", bag_path, "' != ''"])

    return LaunchDescription([

        # 声明 launch 参数
    
        DeclareLaunchArgument('input_source', default_value='bag',
            description="输入源: 'bag'（默认，离线回放）或 'camera'（真实相机，需编译 mindvision_camera）"),
        DeclareLaunchArgument('target_color', default_value='red',
            description='目标颜色: red / blue / both'),
        DeclareLaunchArgument('image_topic', default_value='/image_raw',
            description='订阅的图像话题名称'),
        DeclareLaunchArgument('bag_path', default_value='',
            description='bag 文件路径（input_source:=bag 时需指定，本地路径：bags/armor_test）'),
        DeclareLaunchArgument('enable_debug', default_value='true',
            description='是否发布调试图像'),

        # 相机模式：启动迈德威视相机节点
        Node(
            condition=IfCondition(is_camera),
            package='mindvision_camera',
            executable='mindvision_camera_node',
            name='camera_node',
            output='screen',
        ),

        # Bag 模式：仅在指定了 bag_path 时才播放
        ExecuteProcess(
            condition=IfCondition(PythonExpression(
                ["'", input_source, "' == 'bag' and '", bag_path, "' != ''"])),
            cmd=['ros2', 'bag', 'play', bag_path, '--loop'],
            output='screen',
        ),

        # Bag 模式提示
        LogInfo(
            condition=IfCondition(is_bag),
            msg="[armor_detector] Bag 模式：请确保 bag 文件存在且包含 /image_raw 话题",
        ),

        
        # 装甲板检测节点（两种模式都启动）
        
        Node(
            package='armor_detector',
            executable='armor_detector_node',
            name='armor_detector_node',
            output='screen',
            parameters=[default_params, {
                'target_color': target_color,
                'image_topic': image_topic,
                'enable_debug': enable_debug,
            }],
        ),
    ])
