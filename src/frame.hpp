#pragma once
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include "util.hpp"
#include "window.hpp"
#include <list>
#include "../video-snapcut-ui/video-snapcut/myline.h"

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

  auto& get_user_mask() { return user_mask_; }

  auto& get_frame_lab() { return frame_lab_; }

  auto& get_mask() { return mask_; }

  auto& get_contours_() { return contours_; }

  auto& get_windows() { return windows_; }

  auto& get_boundary_distance() { return boundary_distance_; }

  void initialize_windows();

  void motion_propagate(Frame& prev);

  void update_windows(Frame& prev);

  void add_window(cv::Point center);

  void remove_window(int idx);

  void update_mask(cv::Mat& mask);

  void update_user_mask(cv::Mat& mask);

  cv::Mat generate_combined_map();
  
  void generate(cv::Mat& res);
  // opencv Function
  void show_windows();

  QImage* getQImage();

  std::list<MyLine*> fgdList, bgdList;
  void addfgd(MyLine *);

 private:
  cv::Mat geometric_transformation(cv::Mat& img1, cv::Mat& img2, cv::Mat& mask);

  static int window_id_cnt_;
  int frame_id_;
  cv::Mat frame_;
  cv::Mat frame_lab_;
  cv::Mat boundary_distance_;
  cv::Mat mask_;
  cv::Mat user_mask_;
  std::vector<std::vector<cv::Point>> contours_;
  std::unordered_map<int, LocalWindow> windows_;
};
