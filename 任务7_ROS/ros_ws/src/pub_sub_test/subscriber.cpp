#include "ros/ros.h"
#include "std_msgs/Int32.h"

void chatterCallback(const std_msgs::Int32::ConstPtr& msg)
{
    ROS_INFO("node_b : %d\n", msg->data);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "subscriber");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("num", 1000, chatterCallback);
    ros::spin();
    return 0;
}

