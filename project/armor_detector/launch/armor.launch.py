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
    input_source = LaunchConfiguration('input_source', default='camera')
    target_color = LaunchConfiguration('target_color', default='red')
    image_topic  = LaunchConfiguration('image_topic',  default='/image_raw')
    bag_path     = LaunchConfiguration('bag_path',     default='')
    enable_debug = LaunchConfiguration('enable_debug', default='true')

    # ---- 条件判断：是否启动相机 ----
    # input_source == 'camera' 时 is_camera = true
    is_camera = PythonExpression(["'", input_source, "' == 'camera'"])
    is_bag    = PythonExpression(["'", input_source, "' == 'bag'"])

    return LaunchDescription([

        # 声明 launch 参数
    
        DeclareLaunchArgument('input_source', default_value='camera',
            description="输入源: 'camera'（真实相机）或 'bag'（ros2 bag 回放）"),
        DeclareLaunchArgument('target_color', default_value='red',
            description='目标颜色: red / blue / both'),
        DeclareLaunchArgument('image_topic', default_value='/image_raw',
            description='订阅的图像话题名称'),
        DeclareLaunchArgument('bag_path', default_value='',
            description='bag 文件路径（仅 input_source:=bag 时需要）'),
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

        # Bag 模式：自动播放 bag 文件（如果指定了 bag_path）
        ExecuteProcess(
            condition=IfCondition(is_bag),
            cmd=['ros2', 'bag', 'play', bag_path, '--loop'],
            output='screen',
            # 如果 bag_path 为空，仍然启动但会报错提示用户
        ),

        # Bag 模式的提示信息
      
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
