#include "frame.hpp"
#include <chrono>
#include <cmath>
#include <utility>
#include "window.hpp"
std::vector<std::vector<cv::Point>> convert_mask_to_contours(cv::Mat& mask) {
  assert(mask.type() == CV_8UC1);
  std::vector<std::vector<cv::Point>> res;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(mask, res, hierarchy, cv::RETR_TREE,
                   cv::CHAIN_APPROX_SIMPLE);
  return res;
}

cv::Mat convert_mask_to_boundary_distance(
    cv::Mat& mask, std::vector<std::vector<cv::Point>>& contours) {
  cv::Mat res = cv::Mat(mask.rows, mask.cols, CV_64FC1, cv::Scalar(-1.0));
  for (int r = 0; r < res.rows; ++r) {
    for (int c = 0; c < res.cols; ++c) {
      for (auto& contour : contours) {
        double distance =
            std::abs(cv::pointPolygonTest(contour, cv::Point(r, c), true));
        if (res.at<double>(r, c) < 0) {
          res.at<double>(r, c) = distance;
        } else if (distance < res.at<double>(r, c)) {
          res.at<double>(r, c) = distance;
        }
      }
    }
  }
  return res;
}

cv::Mat convert_bgr_to_lab(cv::Mat& img) {
  assert(img.type() == CV_8UC3);
  cv::Mat res;
  // convert to float
  img.convertTo(res, CV_32F);
  // normalize value
  res /= 255.0;
  cv::cvtColor(res, res, CV_BGR2Lab);
  return res;
}

int Frame::window_id_cnt_ = 0;

Frame::Frame(int frame_id, cv::Mat&& frame, cv::Mat&& mask)
    : frame_id_(frame_id), frame_(std::move(frame)), mask_(std::move(mask)) {
  assert(this->frame_id_ == 0);
  contours_ = convert_mask_to_contours(mask_);
  frame_lab_ = convert_bgr_to_lab(frame_);
  boundary_distance_ = convert_mask_to_boundary_distance(mask_, contours_);
}

Frame::Frame(int frame_id, cv::Mat&& frame)
    : frame_id_(frame_id), frame_(std::move(frame)) {
  frame_lab_ = convert_bgr_to_lab(frame_);
}

void Frame::add_window(cv::Point center) {
  windows_.emplace(std::make_pair(window_id_cnt_, LocalWindow(center, *this)));
  ++window_id_cnt_;
}

void Frame::remove_window(int idx) {
  auto search = windows_.find(idx);
  if (search != windows_.end()) {
    windows_.erase(search);
  }
}

// TODO: test this
void Frame::initialize_windows() {
  for (auto& contour : contours_) {
    cv::Point prev, cur;
    double distance = 0.0;
    for (int i = 0; i < contour.size(); ++i) {
      if (i == 0) {
        cur = contour[0];
        add_window(cur);
        prev = cur;
      } else {
        cur = contour[i];
        double d = cv::norm(prev - cur);
        if (d >= WINDOW_INTERVAL) {
          cv::Point2d tmp(prev.x, prev.y);
          cv::Vec2d direction = cv::Vec2d(cur.x - prev.x, cur.y - prev.y);
          direction /= cv::norm(direction);

          if (distance != 0.0) {
            double offset = WINDOW_INTERVAL - distance;
            d -= offset;
            tmp.x += (direction[0] * offset);
            tmp.y += (direction[1] * offset);
            cv::Point tmp_center = tmp;
            add_window(tmp_center);
            distance = 0.0;
          }
          while (d >= WINDOW_INTERVAL) {
            double offset = WINDOW_INTERVAL;
            d -= offset;
            tmp.x += (direction[0] * offset);
            tmp.y += (direction[1] * offset);
            cv::Point tmp_center = tmp;
            add_window(tmp_center);
          }
          if (d > 0) {
            distance = d;
          }
        } else {
          distance += d;
          if (distance >= WINDOW_INTERVAL) {
            add_window(cur);
            distance = distance - WINDOW_INTERVAL;
          }
        }
        prev = cur;
      }
    }
  }
  auto start = std::chrono::high_resolution_clock::now();
 
  // initailize windows' info
  // #pragma omp parallel for num_threads(4)
  for (auto it = windows_.begin(); it != windows_.end(); ++it) {
    it->second.update_color_model();
    it->second.update_shape_confidence();
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::cout
      << "time consumed: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
      << "ms" << std::endl;
}

void Frame::show_windows() {
  cv::Mat drawboard = frame_.clone();
  for (auto it = windows_.begin(); it != windows_.end(); ++it) {
    cv::Point c = it->second.get_center();
    cv::rectangle(drawboard,
                  cv::Rect(c.x - HALF_WINDOW_LENGTH, c.y - HALF_WINDOW_LENGTH,
                           WINDOW_LENGTH, WINDOW_LENGTH),
                  cv::Scalar(0, 0, 255));
  }
  while (true) {
    cv::imshow(windowname, drawboard);
    int key = cv::waitKey(1) & 0xFF;
    if (key == 27) {
      break;
    }
  }
}

cv::Mat Frame::generate_combined_map() {
  cv::Mat res(frame_.rows, frame_.cols, CV_64FC1, cv::Scalar(-1.0));
  for (int r = 0; r < res.rows; ++r) {
    for (int c = 0; c < res.cols; ++r) {
    }
  }
  return res;
}
