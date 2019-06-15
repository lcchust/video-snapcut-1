#include "window.hpp"
#include <cmath>
#include <iostream>
#include "frame.hpp"
double weight_function(double distance) {
  return std::exp((-distance * distance) / (SIGMA_C * SIGMA_C));
}

LocalWindow::LocalWindow(cv::Point _center, Frame& cur_frame)
    : center_(_center), cur_frame_(cur_frame) {
  base_ =
      cv::Point(center_.x - HALF_WINDOW_LENGTH, center_.y - HALF_WINDOW_LENGTH);
}

void LocalWindow::update_color_model() {
  cv::Mat comp_idx(WINDOW_LENGTH, WINDOW_LENGTH, CV_32SC1);
  init_gmms();
  assign_gmm_component(comp_idx);
  learn_gmm(comp_idx);
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

      double p_f = foreground_gmm_(pixel);
      double p_b = background_gmm_(pixel);

      double p_c = p_f / (p_f + p_b);
      color_probability_.at<double>(r, c) = p_c;

      // conf
      double weight = weight_function(distance);
      confidence_nume += (std::abs(mask_label - p_c) * weight);
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

void LocalWindow::update_combined_map(cv::Mat& nume_map, cv::Mat& deno_map,
                                      cv::Mat& count_map) {
  for (int r = 0; r < WINDOW_LENGTH; ++r) {
    for (int c = 0; c < WINDOW_LENGTH; ++c) {
      int x = base_.x + c;
      int y = base_.y + r;
      double p_f = integrated_probabitlity_map_.at<double>(r, c);
      cv::Vec2d vec(center_.x - x, center_.y - y);
      double distance = cv::norm(vec);
      double factor = 1.0 / (distance + ELIPSON);
      nume_map.at<double>(y, x) += (p_f * factor);
      deno_map.at<double>(y, x) += (factor);
      count_map.at<uint8_t>(y, x) = 1;
    }
  }
}

void LocalWindow::init_gmms() {
  const int kMeansItCount = 10;
  const int kMeansType = cv::KMEANS_PP_CENTERS;
  cv::Mat bgdLabels, fgdLabels;
  std::vector<cv::Vec3f> bgdSamples, fgdSamples;
  for (int r = 0; r < WINDOW_LENGTH; ++r) {
    for (int c = 0; c < WINDOW_LENGTH; ++c) {
      int x = base_.x + c;
      int y = base_.y + r;
      if (cur_frame_.mask_.at<uint8_t>(y, x) == MASK_FOREGROUND) {
        fgdSamples.push_back(cur_frame_.frame_lab_.at<cv::Vec3f>(y, x));
      } else {
        bgdSamples.push_back(cur_frame_.frame_lab_.at<cv::Vec3f>(y, x));
      }
    }
  }
  assert(!bgdSamples.empty() && !bgdSamples.empty());
  cv::Mat _bgdSamples((int)bgdSamples.size(), 3, CV_32FC1, &bgdSamples[0][0]);
  cv::kmeans(_bgdSamples, GMM::componentsCount, bgdLabels,
             cv::TermCriteria(CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0,
             kMeansType);
  cv::Mat _fgdSamples((int)fgdSamples.size(), 3, CV_32FC1, &fgdSamples[0][0]);
  cv::kmeans(_fgdSamples, GMM::componentsCount, fgdLabels,
             cv::TermCriteria(CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0,
             kMeansType);
  background_gmm_.initLearning();
  for (int i = 0; i < (int)bgdSamples.size(); i++) {
    background_gmm_.addSample(bgdLabels.at<int>(i, 0), bgdSamples[i]);
  }
  background_gmm_.endLearning();

  foreground_gmm_.initLearning();
  for (int i = 0; i < (int)fgdSamples.size(); i++) {
    foreground_gmm_.addSample(fgdLabels.at<int>(i, 0), fgdSamples[i]);
  }
  foreground_gmm_.endLearning();
}

void LocalWindow::assign_gmm_component(cv::Mat& comp_idx) {
  for (int r = 0; r < WINDOW_LENGTH; ++r) {
    for (int c = 0; c < WINDOW_LENGTH; ++c) {
      int x = base_.x + c;
      int y = base_.y + r;
      cv::Vec3d color = cur_frame_.frame_lab_.at<cv::Vec3f>(y, x);
      comp_idx.at<int>(r, c) =
          cur_frame_.mask_.at<uint8_t>(y, x) == MASK_FOREGROUND
              ? foreground_gmm_.whichComponent(color)
              : foreground_gmm_.whichComponent(color);
    }
  }
}

void LocalWindow::learn_gmm(cv::Mat& comp_idx) {
  background_gmm_.initLearning();
  foreground_gmm_.initLearning();
  for (int ci = 0; ci < GMM::componentsCount; ci++) {
    for (int r = 0; r < WINDOW_LENGTH; ++r) {
      for (int c = 0; c < WINDOW_LENGTH; ++c) {
        int x = base_.x + c;
        int y = base_.y + r;

        if (comp_idx.at<int>(r, c) == ci &&
            cur_frame_.boundary_distance_.at<double>(y, x) >
                BOUNDARY_DISTANCE_THRESHOLD) {
          cv::Vec3f pixel = cur_frame_.frame_lab_.at<cv::Vec3f>(y, x);
          if (cur_frame_.mask_.at<uint8_t>(y, x) == MASK_FOREGROUND) {
            foreground_gmm_.addSample(ci, pixel);
            foreground_samples_.push_back(pixel);
          } else {
            background_gmm_.addSample(ci, pixel);
            background_samples_.push_back(pixel);
          }
        }
      }
    }
  }
  background_gmm_.endLearning();
  foreground_gmm_.endLearning();
}

void LocalWindow::integration() {
  integrated_probabitlity_map_ =
      cv::Mat(WINDOW_LENGTH, WINDOW_LENGTH, CV_64FC1);
  for (int r = 0; r < WINDOW_LENGTH; ++r) {
    for (int c = 0; c < WINDOW_LENGTH; ++c) {
      int x = base_.x + c;
      int y = base_.y + r;
      int label = cur_frame_.mask_.at<uint8_t>(y, x) == MASK_FOREGROUND ? 1 : 0;
      integrated_probabitlity_map_.at<double>(r, c) =
          shape_confidence_.at<double>(r, c) * label +
          (1 - shape_confidence_.at<double>(r, c)) *
              color_probability_.at<double>(r, c);
    }
  }
}

void LocalWindow::update_center(cv::Point center) {
  center_ = center;
  base_.x = center_.x - HALF_WINDOW_LENGTH;
  base_.y = center_.y - HALF_WINDOW_LENGTH;
}

void LocalWindow::update_center_optical_flow(cv::Mat& flow) {
  double x_sum = 0.0, y_sum = 0.0;
  int cnt = 0;
  for (int r = 0; r < WINDOW_LENGTH; ++r) {
    for (int c = 0; c < WINDOW_LENGTH; ++c) {
      int x = base_.x + c;
      int y = base_.y + r;
      if (x >= 0 && x <= cur_frame_.mask_.cols && y >= 0 &&
          y <= cur_frame_.mask_.rows) {
        if (cur_frame_.mask_.at<uint8_t>(y, x) == MASK_FOREGROUND) {
          const cv::Point2f flowatxy = flow.at<cv::Point2f>(y, x);
          x_sum += flowatxy.x;
          y_sum += flowatxy.y;
          cnt++;
        }
      }
    }
  }
  int displacement_x = std::round(x_sum / cnt);
  int displacement_y = std::round(y_sum / cnt);
  center_.x += displacement_x;
  center_.y += displacement_y;
  base_.x = center_.x - HALF_WINDOW_LENGTH;
  base_.y = center_.y - HALF_WINDOW_LENGTH;
}