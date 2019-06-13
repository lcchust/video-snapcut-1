#include <iostream>
#include "window.hpp"
#include "frame.hpp"
#include <cmath>
double weight_function(double distance) {
  return std::exp((-distance * distance) / (SIGMA_C * SIGMA_C));
}

LocalWindow::LocalWindow(cv::Point _center, Frame& cur_frame)
    : center_(_center),
      cur_frame_(cur_frame),
      foreground_gmm_(cv::ml::EM::create()),
      background_gmm_(cv::ml::EM::create()) {
  base_ =
      cv::Point(center_.x - HALF_WINDOW_LENGTH, center_.y - HALF_WINDOW_LENGTH);
  // set color GMM params
  foreground_gmm_->setClustersNumber(COLOR_COMPONENT);
  foreground_gmm_->setCovarianceMatrixType(cv::ml::EM::COV_MAT_SPHERICAL);
  foreground_gmm_->setTermCriteria(cv::TermCriteria(
      cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 0.1));

  background_gmm_->setClustersNumber(COLOR_COMPONENT);
  background_gmm_->setCovarianceMatrixType(cv::ml::EM::COV_MAT_SPHERICAL);
  background_gmm_->setTermCriteria(cv::TermCriteria(
      cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 0.1));
}

void LocalWindow::update_color_model() {
  std::vector<float> fg_vec;
  std::vector<float> bg_vec;
  for (int r = 0; r < WINDOW_LENGTH; ++r) {
    for (int c = 0; c < WINDOW_LENGTH; ++c) {
      int x = base_.x + c;
      int y = base_.y + r;
   
      if (cur_frame_.boundary_distance_.at<double>(y, x) >
          BOUNDARY_DISTANCE_THRESHOLD) {
        auto pixel = cur_frame_.frame_lab_.at<cv::Vec3f>(y, x);
        if (cur_frame_.mask_.at<uint8_t>(y, x) == MASK_FOREGROUND) {
          fg_vec.push_back(pixel[0]);
          fg_vec.push_back(pixel[1]);
          fg_vec.push_back(pixel[2]);
        } else if (cur_frame_.mask_.at<uint8_t>(y, x) == MASK_BACKGROUND) {
          bg_vec.push_back(pixel[0]);
          bg_vec.push_back(pixel[1]);
          bg_vec.push_back(pixel[2]);
        } else {
          std::cout << x << " " << y << std::endl;
          std::cout << (int)cur_frame_.mask_.at<uint8_t>(y, x) << std::endl;
          exit(1);
        }
      }
    }
  }
  foreground_gmm_mat_ = cv::Mat(fg_vec.size(), 1, CV_32FC1, fg_vec.data());
  foreground_gmm_mat_ = foreground_gmm_mat_.reshape(1, fg_vec.size() / 3);
  background_gmm_mat_ = cv::Mat(bg_vec.size(), 1, CV_32FC1, bg_vec.data());
  background_gmm_mat_ = background_gmm_mat_.reshape(1, bg_vec.size() / 3);

  
  // train
  foreground_gmm_->trainEM(foreground_gmm_mat_);
  background_gmm_->trainEM(background_gmm_mat_);

  auto fg_weights = foreground_gmm_->getWeights();
  auto bg_weights = background_gmm_->getWeights();

  color_probability_ = cv::Mat(WINDOW_LENGTH, WINDOW_LENGTH, CV_64FC1);
  // color probability and confidence
  double confidence_deno = 0.0;
  double confidence_nume = 0.0;
  for (int r = 0; r < WINDOW_LENGTH; ++r) {
    for (int c = 0; c < WINDOW_LENGTH; ++c) {
      int x = base_.x + c;
      int y = base_.y + r;
      auto pixel = cur_frame_.frame_lab_.at<cv::Vec3f>(y, x);
      uint8_t mask_val = cur_frame_.mask_.at<uint8_t>(y, x);
      double mask_label = mask_val == MASK_FOREGROUND ? 1.0 : 0.0;
      double distance = cur_frame_.boundary_distance_.at<double>(y, x);

      // prob
      cv::Mat p1, p2;
      foreground_gmm_->predict2(pixel, p1);
      background_gmm_->predict2(pixel, p2);
      double p_f = fg_weights.dot(p1);
      double p_b = bg_weights.dot(p2);
      double p_c = p_f / (p_f + p_b);
      color_probability_.at<double>(r, c) = p_c;

      // conf
      double weight = weight_function(distance);
      confidence_nume += ((mask_label - p_c) * weight);
      confidence_deno += weight;
    }
  }
  color_confidence_ = 1 - confidence_nume / confidence_deno;
}

void LocalWindow::update_shape_confidence() {
  shape_confidence_ = cv::Mat(WINDOW_LENGTH, WINDOW_LENGTH, CV_64FC1);
  sigma_shape_ = calculate_sigma_shape(color_confidence_);
  for (int r = 0; r < WINDOW_LENGTH; ++r) {
    for (int c = 0; c < WINDOW_LENGTH; ++c) {
      int x = base_.x + c;
      int y = base_.y + r;
      double distance = cur_frame_.boundary_distance_.at<double>(y, x);
      shape_confidence_.at<double>(r, c) =
          1 - std::exp((-distance * distance) / (sigma_shape_ * sigma_shape_));
    }
  }
}

double LocalWindow::calculate_sigma_shape(double color_confidence) {
  if (color_confidence >= F_CUTOFF && color_confidence <= 1.0) {
    return SIGMA_MIN +
           ((SIGMA_MAX - SIGMA_MIN) / ((1 - F_CUTOFF) * (1 - F_CUTOFF))) *
               (color_confidence - F_CUTOFF) * (color_confidence - F_CUTOFF);
  } else {
    return SIGMA_MIN;
  }
}