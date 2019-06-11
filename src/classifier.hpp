#pragma once
#include <map>
#include <opencv2/opencv.hpp>
#include <vector>
class LocalClassifier {
 private:
  // constants
  static constexpr int SIDE_LENGTH = 50;
  static constexpr int HALF_SIDE_LENGTH = 25;
  static constexpr int GAUSSIAN_COMPONENT_N = 3;

  // sigma shape
  static constexpr int SIGMA_MIN = 2;
  static constexpr int SIGMA_MAX = SIDE_LENGTH;
  static constexpr double F_CUTOFF = 0.85;
  static constexpr int R = 2;

  // threshold
  static constexpr double FOREGROUND_THRESHOLD = 0.75;
  static constexpr double BACKGROUND_THRESHOLD = 0.25;

 public:
  LocalClassifier(cv::Point _center, cv::Mat& cur_frame,
                  std::vector<cv::Point>& contour)
      : center_(_center),
        cur_frame_ptr_(&cur_frame),
        foreground_gmm_(cv::ml::EM::create()),
        background_gmm_(cv::ml::EM::create()) {
    // add contour
    for (auto& p : contour) {
      if (p.x >= center_.x - HALF_SIDE_LENGTH &&
          p.x <= center_.x + HALF_SIDE_LENGTH &&
          p.y >= center_.y - HALF_SIDE_LENGTH &&
          p.y <= center_.y + HALF_SIDE_LENGTH) {
        contour_.push_back(p);
      }
    }

    // set color GMM params
    foreground_gmm_->setClustersNumber(GAUSSIAN_COMPONENT_N);
    foreground_gmm_->setCovarianceMatrixType(cv::ml::EM::COV_MAT_SPHERICAL);
    foreground_gmm_->setTermCriteria(cv::TermCriteria(
        cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 0.1));
    
    background_gmm_->setClustersNumber(GAUSSIAN_COMPONENT_N);
    background_gmm_->setCovarianceMatrixType(cv::ml::EM::COV_MAT_SPHERICAL);
    background_gmm_->setTermCriteria(cv::TermCriteria(
        cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 0.1));

    // TODO: assign shape mask  
  }

  void train_color_model();

  cv::Mat get_color_probability_map();

  double get_color_confidence();

  cv::Mat get_shape_confidence();

 private:
  cv::Point center_;
  cv::Ptr<cv::ml::EM> foreground_gmm_;
  cv::Ptr<cv::ml::EM> background_gmm_;
  double color_confidence_;
  double sigma_color_;
  cv::Mat shape_mask_;
  cv::Mat shape_confidence_;
  double sigma_shape_;
  std::vector<cv::Point> contour_;
  cv::Mat* cur_frame_ptr_;

};