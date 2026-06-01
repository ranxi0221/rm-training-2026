from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument
import os
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # 参数文件路径
    pkg_share = get_package_share_directory('armor_detector')
    default_params = os.path.join(pkg_share, 'config', 'armor_params.yaml')

    # 可命令行覆盖的参数
    target_color = LaunchConfiguration('target_color', default='red')
    image_topic = LaunchConfiguration('image_topic', default='/image_raw')

    return LaunchDescription([
        # 声明可覆盖的 launch 参数
        DeclareLaunchArgument('target_color', default_value='red',
            description='Target color: red / blue / both'),
        DeclareLaunchArgument('image_topic', default_value='/image_raw',
            description='Image topic to subscribe'),

        # ---- 相机节点（迈德威视） ----
        Node(
            package='mindvision_camera',
            executable='mindvision_camera_node',
            name='camera_node',
            output='screen',
        ),

        # ---- 装甲板检测节点 ----
        Node(
            package='armor_detector',
            executable='armor_detector_node',
            name='armor_detector_node',
            output='screen',
            parameters=[default_params, {
                'target_color': target_color,
                'image_topic': image_topic,
            }],
        ),
    ])
