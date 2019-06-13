#include "util.hpp"

std::string cv_type_to_string(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch (depth) {
    case CV_8U:
      r = "8U";
      break;
    case CV_8S:
      r = "8S";
      break;
    case CV_16U:
      r = "16U";
      break;
    case CV_16S:
      r = "16S";
      break;
    case CV_32S:
      r = "32S";
      break;
    case CV_32F:
      r = "32F";
      break;
    case CV_64F:
      r = "64F";
      break;
    default:
      r = "User";
      break;
  }

  r += "C";
  r += (chans + '0');

  return r;
}


void show_probability_map(cv::Mat& map) {
  assert(map.type() == CV_64FC1);
  cv::Mat tmp = map.clone();
  
  tmp *= 255.0;
  cv::Mat drawboard;
  tmp.convertTo(drawboard, CV_8U);
  cv::cvtColor(drawboard, drawboard, cv::COLOR_GRAY2BGR);
    while (true) {
    cv::imshow(windowname, drawboard);
    int key = cv::waitKey(1) & 0xFF;
    if (key == 27) {
      break;
    }
  }
}