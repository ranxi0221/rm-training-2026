from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import PathJoinSubstitution

def generate_launch_description():
    pkg_share = FindPackageShare('training_pkg')
    param_file = PathJoinSubstitution([pkg_share, 'config', 'talker_params.yaml'])

    talker = Node(
        package='training_pkg',
        executable='talker',
        name='talker_node',
        output='screen',
        parameters=[param_file]
    )
    listener = Node(
        package='training_pkg',
        executable='listener',
        name='listener_node',
        output='screen'
    )
    return LaunchDescription([talker, listener])