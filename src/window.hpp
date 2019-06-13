#pragma once
#include <map>
#include <opencv2/opencv.hpp>
#include <vector>
class Frame;

const int MASK_FOREGROUND = 255;
const int MASK_BACKGROUND = 0;

const int WINDOW_LENGTH = 50;
const int HALF_WINDOW_LENGTH = 25;
const double WINDOW_INTERVAL = 33; // 2/3 of WINDOW_LENGTH
const int COLOR_COMPONENT = 3;

const double BOUNDARY_DISTANCE_THRESHOLD = 5.0;
const double FOREGROUND_THRESHOLD = 0.75;
const double BACKGROUND_THRESHOLD = 0.25;

const int SIGMA_C = 25;
const int SIGMA_MIN = 2;
const int SIGMA_MAX = 50;
const double F_CUTOFF = 0.85;
const int R = 2;

double weight_function(double distance);

class LocalWindow {
 public:
  LocalWindow() = default;

  LocalWindow(cv::Point _center, Frame& cur_frame);

  void update_color_model();

  // void update_color_model(LocalWindow& local);

  void update_shape_confidence();

  cv::Mat get_color_probability_map() { return color_probability_; }

  double get_color_confidence() { return color_confidence_; }

  cv::Mat get_shape_confidence() { return shape_confidence_; }

  cv::Point get_center() { return center_; }

  cv::Point get_base() { return base_; }

 private:
  static double calculate_sigma_shape(double color_confidence);
  // left upper corner
  cv::Point base_;
  cv::Point center_;
  cv::Ptr<cv::ml::EM> foreground_gmm_;
  cv::Ptr<cv::ml::EM> background_gmm_;
  cv::Mat foreground_gmm_mat_;
  cv::Mat background_gmm_mat_;
  cv::Mat color_probability_;
  double color_confidence_;
  cv::Mat shape_confidence_;
  double sigma_shape_;
  Frame& cur_frame_;
};