/** @file computer_vision.cpp
 *  @author Andrew Jin
 *  @competition IARRC 2019
 */

#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

#include "computer_vision.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

#include <vector>

/*
 * @name ComputerVision 
 * @brief Constructor
 * @param nh: ROS node handler
*/
ComputerVision::ComputerVision(ros::NodeHandle nh) :
nh_(nh)
{
  InitializeSubscribers();
  InitializePublishers();

  //sync.reset(new Sync(left_right_sync_policy(10), left_camera_subscriber_, right_camera_subscriber_));
  //sync->registerCallback(boost::bind(&ComputerVision::LeftRightSyncCameraCallback, this, _1, _2));
  // left_camera_subscriber_.subscribe(nh, "left/image_rect_color", 1);
  // right_camera_subscriber_.subscribe(nh, "right/image_rect_color", 1);
  // TimeSynchronizer<Image, CameraInfo> sync(image_sub, info_sub, 10);
  // sync.registerCallback(boost::bind(&LeftRightSyncCameraCallback, _1, _2));
}

/*
 * @name ~ComputerVision
 * @brief Destructor
*/
ComputerVision::~ComputerVision() {

}

/*
 * @name InitializeSubscribers
 * @brief 
*/
void ComputerVision::InitializeSubscribers() {
  //message_filters::Subscriber<sensor_msgs::Image> left_camera_subscriber_(nh_, "/zed/left/image_rect_color", 1);
  //message_filters::Subscriber<sensor_msgs::Image> right_camera_subscriber_(nh_, "/zed/right/image_rect_color", 1);

  left_camera_subscriber_.subscribe(nh_, "/zed/left/image_rect_color", 1);
  right_camera_subscriber_.subscribe(nh_, "/zed/right/image_rect_color", 1);
  lrsync_.reset(new LRSYNC(LRPOLICY(0), left_camera_subscriber_, right_camera_subscriber_));
  lrsync_->registerCallback(boost::bind(&ComputerVision::LeftRightSyncCameraCallback, this, _1, _2));
  
  // rgb_camera_subscriber_ = nh_.subscribe("rgb/image_rect_color", 0, &ComputerVision::RGBCameraCallback, this);
  // left_header_subscriber_ = nh_.subscribe("/zed/left/image_rect_color", 0, &ComputerVision::LeftCameraCallback, this);
  // right_header_subscriber_ = nh_.subscribe("/zed/right/image_rect_color", 0, &ComputerVision::RightCameraCallback, this);
  // depth_camera_subscriber_ = nh_.subscribe("depth/depth_registered", 0, &ComputerVision::DepthCameraCallback, this);
}

void ComputerVision::InitializePublishers() {
  test_publisher = nh_.advertise<sensor_msgs::Image>("/test_stitching", 1);
}

void ComputerVision::RGBCameraCallback(const sensor_msgs::Image& msg){

}

void ComputerVision::LeftRightSyncCameraCallback(const sensor_msgs::ImageConstPtr& left_msg, const sensor_msgs::ImageConstPtr& right_msg) {
  ROS_INFO("SYNCHRONIZED POLICY CALLBACK");

  // Convert sensor_msgs::Image to a BGR8 cv::Mat 
  cv::Mat left_img_bgr8 = (cv_bridge::toCvCopy(left_msg, sensor_msgs::image_encodings::BGR8))->image;
  cv::Mat right_img_bgr8 = (cv_bridge::toCvCopy(right_msg, sensor_msgs::image_encodings::BGR8))->image;

  // Stich the images together using OpenCV's image stitching algorithm
  std::vector<cv::Mat> imgs;
  imgs.push_back(left_img_bgr8);
  imgs.push_back(right_img_bgr8);

  cv::Mat result;
  cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create();
  cv::Stitcher::Status status = stitcher->stitch(imgs, result);

  // Publish so we can visualize in rviz
  cv_bridge::CvImage img_bridge_output;
  std_msgs::Header header; // empty header
  header.stamp = ros::Time::now(); // time
  img_bridge_output = cv_bridge::CvImage(header, sensor_msgs::image_encodings::BGR8, result);

  test_publisher.publish(img_bridge_output.toImageMsg());
  ROS_INFO("SYNCHRONIZED DATA PUBLUISHED");
}

void ComputerVision::LeftCameraCallback(const sensor_msgs::Image& msg) {
    ROS_INFO("LEFT FRAME HEADER:");
    //get header info
    std_msgs::Header h = msg.header;
    std::cout << h << std::endl; //all at once
    // std::cout<<h.stamp<<endl; //specific parts of it
    // std::cout<<h.stamp.sec<<endl;
    // std::cout<<h.stamp.nsec<<endl;
    // std::cout<<h.seq<<endl;
}

void ComputerVision::RightCameraCallback(const sensor_msgs::Image& msg) {
    ROS_INFO("RIGHT FRAME HEADER:");
    //get header info
    std_msgs::Header h = msg.header;
    //std::cout << h << std::endl; //all at once
}

void ComputerVision::DepthCameraCallback(const sensor_msgs::Image& msg) {
    
}