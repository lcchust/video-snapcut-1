#pragma once
#include <map>
#include <opencv2/opencv.hpp>
#include <vector>
#include "gmm.hpp"
class Frame;

const int MASK_FOREGROUND = 255;
const int MASK_BACKGROUND = 0;

const int WINDOW_LENGTH = 50;
const int HALF_WINDOW_LENGTH = 25;
const double WINDOW_INTERVAL = 34;  // 2/3 of WINDOW_LENGTH
const int COLOR_COMPONENT = 3;

const double BOUNDARY_DISTANCE_THRESHOLD = 5.0;
const double FOREGROUND_THRESHOLD = 0.75;
const double BACKGROUND_THRESHOLD = 0.25;

const int SIGMA_C = 25;
const int SIGMA_MIN = 2;
const int SIGMA_MAX = 50;
const double F_CUTOFF = 0.65;
const int R = 2;

const double ELIPSON = 0.1;

double weight_function(double distance);

class LocalWindow {
 public:
  LocalWindow() = default;

  LocalWindow(cv::Point _center, Frame& cur_frame);

  void update_color_model();

  void update_color_model(LocalWindow& prev);

  void update_shape_confidence();

  void update_combined_map(cv::Mat& nume_map, cv::Mat& deno_map,
                           cv::Mat& count_map);

  void integration();

  void update_center(cv::Point center);

  void update_center_optical_flow(cv::Mat& flow);

  cv::Mat& get_color_probability_map() { return color_probability_; }

  double get_color_confidence() { return color_confidence_; }

  cv::Mat& get_shape_confidence() { return shape_confidence_; }

  cv::Point get_center() { return center_; }

  cv::Point get_base() { return base_; }

  auto& get_fg_gmm() { return foreground_gmm_; }

  auto& get_bg_gmm() { return background_gmm_; }

  auto& get_fg_samples() { return foreground_samples_; }

  auto& get_bg_samples() { return background_samples_; }

  auto& get_integrated_probabitlity_map() {
    return integrated_probabitlity_map_;
  }

 private:
  void init_gmms();
  void assign_gmm_component(cv::Mat& comp_idx);
  void learn_gmm(cv::Mat& comp_idx);
  void learn_gmm(cv::Mat& comp_idx,
                 std::vector<std::pair<int, cv::Vec3f>>& fg_samples,
                 std::vector<std::pair<int, cv::Vec3f>>& bg_samples);

  int count_foreground_pixels(cv::Mat& color_probability_map);
  static double calculate_sigma_shape(double color_confidence);
  // left upper corner
  cv::Point base_;
  cv::Point center_;
  GMM background_gmm_;
  GMM foreground_gmm_;
  std::vector<std::pair<int, cv::Vec3f>> foreground_samples_;
  std::vector<std::pair<int, cv::Vec3f>> background_samples_;
  cv::Mat color_probability_;
  cv::Mat integrated_probabitlity_map_;
  double color_confidence_;
  cv::Mat shape_confidence_;
  double sigma_shape_;
  Frame& cur_frame_;
};