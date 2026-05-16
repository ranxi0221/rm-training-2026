from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    period_arg = DeclareLaunchArgument(
        'talker_period_ms',
        default_value='500',
        description='Talker publishing period in milliseconds'
    )
    period = LaunchConfiguration('talker_period_ms')
    
    talker = Node(
        package='training_pkg',
        executable='talker',
        name='talker_node',
        output='screen',
        parameters=[{'talker_period_ms': period}]
    )
    listener = Node(
        package='training_pkg',
        executable='listener',
        name='listener_node',
        output='screen'
    )
    return LaunchDescription([period_arg, talker, listener])