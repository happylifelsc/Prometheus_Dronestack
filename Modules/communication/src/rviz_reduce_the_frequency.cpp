#include "rviz_reduce_the_frequency.hpp"

ReduceTheFrequency::ReduceTheFrequency(ros::NodeHandle &nh)
{
    int drone_id_;
    nh.param("ROBOT_ID", drone_id_, 1);

    // 订阅需要降低频率的话题
    octomap_point_cloud_centers_sub_ = nh.subscribe("/octomap_point_cloud_centers", 100, &ReduceTheFrequency::octomapPointCloudCentersCb, this);
    occupancy_inflate_sub_ = nh.subscribe("/uav" + std::to_string(drone_id_) + "_ego_planner_node/grid_map/occupancy_inflate", 10 , &ReduceTheFrequency::occupancyInflateCb, this);
    scan_sub_ = nh.subscribe("/scan",10 , &ReduceTheFrequency::scanCb, this);
    scan_filtered_sub_ = nh.subscribe("/scan_filtered",10 , &ReduceTheFrequency::scanFilteredCb, this);

    trajectory_sub_ = nh.subscribe("/uav" + std::to_string(drone_id_) + "/prometheus/trajectory", 10, &ReduceTheFrequency::trajectoryCb, this);

    odom_sub_ = nh.subscribe("/uav" + std::to_string(drone_id_) + "/prometheus/odom", 10, &ReduceTheFrequency::odomCb, this);

    // 发布降低频率后的话题
    octomap_pub_ = nh.advertise<sensor_msgs::PointCloud2>("/octomap_point_cloud_centers/reduce_the_frequency", 100);
    occupancy_inflate_pub_ = nh.advertise<sensor_msgs::PointCloud2>("/uav" + std::to_string(drone_id_) + "_ego_planner_node/grid_map/occupancy_inflate/reduce_the_frequency", 100);
    scan_pub_ = nh.advertise<sensor_msgs::LaserScan>("/scan/reduce_the_frequency", 100);
    scan_filtered_pub_ = nh.advertise<sensor_msgs::LaserScan>("/scan_filtered/reduce_the_frequency", 100);
    trajectory_pub_ = nh.advertise<nav_msgs::Path>("/uav" + std::to_string(drone_id_) + "/prometheus/trajectory/reduce_the_frequency", 100);
    odom_pub_ = nh.advertise<nav_msgs::Odometry>("/uav" + std::to_string(drone_id_) + "/prometheus/odom/reduce_the_frequency", 100);

    send_timer_1000MS = nh.createTimer(ros::Duration(1.0), &ReduceTheFrequency::send1000MS, this);
    send_timer_500MS = nh.createTimer(ros::Duration(0.5), &ReduceTheFrequency::send500MS, this);
    send_timer_200MS = nh.createTimer(ros::Duration(0.2), &ReduceTheFrequency::send200MS, this);
}

ReduceTheFrequency::~ReduceTheFrequency()
{

}

void ReduceTheFrequency::send1000MS(const ros::TimerEvent &time_event)
{
    if(octomap_point_cloud_ready)
    {
        octomap_pub_.publish(octomap_point_cloud);
        octomap_point_cloud_ready = false;
    }
    if(occupancy_inflate_ready)
    {
        occupancy_inflate_pub_.publish(occupancy_inflate_point_cloud);
        occupancy_inflate_ready = false;
    }
    if(scan_ready)
    {
        scan_pub_.publish(scan);
        scan_ready = false;
    }
    if(scan_filtered_ready)
    {
        scan_filtered_pub_.publish(scan_filtered);
        scan_filtered_ready = false;
    }
}

void ReduceTheFrequency::send500MS(const ros::TimerEvent &time_event)
{
    if(trajectory_ready)
    {
        trajectory_pub_.publish(trajectory);
        trajectory_ready = false;
    }
}

void ReduceTheFrequency::send200MS(const ros::TimerEvent &time_event)
{
    if(odom_ready)
    {
        odom_pub_.publish(odom);
        odom_ready = false;
    }
}

void ReduceTheFrequency::octomapPointCloudCentersCb(const sensor_msgs::PointCloud2::ConstPtr &msg)
{
    octomap_point_cloud = *msg;
    octomap_point_cloud_ready = true;
}


void ReduceTheFrequency::trajectoryCb(const nav_msgs::Path::ConstPtr &msg)
{
    trajectory = *msg;
    trajectory_ready = true;
}

void ReduceTheFrequency::scanCb(const sensor_msgs::LaserScan::ConstPtr &msg)
{
    scan = *msg;
    scan_ready = true;
}

void ReduceTheFrequency::scanFilteredCb(const sensor_msgs::LaserScan::ConstPtr &msg)
{
    scan_filtered  = *msg;
    scan_filtered_ready = true;
}

void ReduceTheFrequency::occupancyInflateCb(const sensor_msgs::PointCloud2::ConstPtr &msg)
{
    occupancy_inflate_point_cloud = *msg;
    occupancy_inflate_ready = true;
}

void ReduceTheFrequency::odomCb(const nav_msgs::Odometry::ConstPtr &msg)
{
    odom = *msg;
    odom_ready = true;
}