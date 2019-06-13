#pragma once
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include "util.hpp"
#include "window.hpp"

std::vector<std::vector<cv::Point>> convert_mask_to_contours(cv::Mat& mask);

cv::Mat convert_mask_to_boundary_distance(
    cv::Mat& mask, std::vector<std::vector<cv::Point>>& contours);

cv::Mat convert_bgr_to_lab(cv::Mat& img);

class Frame {
 public:
  friend class LocalWindow;

  Frame() = default;

  // used for first frame
  Frame(int frame_id, cv::Mat&& frame, cv::Mat&& mask);

  // used for other frames
  Frame(int frame_id, cv::Mat&& frame);

  // getter
  int get_frame_id() const { return frame_id_; }

  auto& get_frame() { return frame_; }

  auto& get_mask() { return mask_; }

  auto& get_contours_() { return contours_; }

  auto& get_windows() { return windows_; }

  auto& get_boundary_distance() { return boundary_distance_; }
  
  void initialize_windows();

  void add_window(cv::Point center);

  void remove_window(int idx);

  cv::Mat generate_combined_map();

  // opencv Function
  void show_windows();

 private:
  static int window_id_cnt_;
  int frame_id_;
  cv::Mat frame_;
  cv::Mat frame_lab_;
  cv::Mat boundary_distance_;
  cv::Mat mask_;
  std::vector<std::vector<cv::Point>> contours_;
  std::unordered_map<int, LocalWindow> windows_;
};