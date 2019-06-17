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
            std::abs(cv::pointPolygonTest(contour, cv::Point(c, r), true));
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
  user_mask_ = cv::Mat(frame_.rows, frame_.cols, CV_8UC1, cv::Scalar(127));
}

Frame::Frame(int frame_id, cv::Mat&& frame)
    : frame_id_(frame_id), frame_(std::move(frame)) {
  frame_lab_ = convert_bgr_to_lab(frame_);
  user_mask_ = cv::Mat(frame_.rows, frame_.cols, CV_8UC1, cv::Scalar(127));
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

void Frame::initialize_windows() {
  windows_.clear();
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
          double offset = WINDOW_INTERVAL - distance;
          distance += d;
          if (distance >= WINDOW_INTERVAL) {
            cv::Point2d tmp(prev.x, prev.y);
            cv::Vec2d direction = cv::Vec2d(cur.x - prev.x, cur.y - prev.y);
            direction /= cv::norm(direction);
            tmp.x += (direction[0] * offset);
            tmp.y += (direction[1] * offset);
            cv::Point tmp_center = tmp;
            add_window(tmp_center);
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
    it->second.integration();
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "time consumed: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
                   .count()
            << "ms" << std::endl;
}

void Frame::show_windows() {
  cv::Mat drawboard = frame_.clone();
  cv::cvtColor(mask_, drawboard, CV_GRAY2BGR);
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
  cv::Mat res(frame_.rows, frame_.cols, CV_64FC1, cv::Scalar(0.0));
  cv::Mat deno(frame_.rows, frame_.cols, CV_64FC1, cv::Scalar(0.0));
  cv::Mat nume(frame_.rows, frame_.cols, CV_64FC1, cv::Scalar(0.0));
  cv::Mat count(frame_.rows, frame_.cols, CV_8UC1, cv::Scalar(0));
  for (auto it = windows_.begin(); it != windows_.end(); ++it) {
    cv::Point center = it->second.get_center();
    if (boundary_distance_.at<double>(center) > HALF_WINDOW_LENGTH) {
      continue;
    }
    it->second.update_combined_map(nume, deno, count);
  }
  for (int r = 0; r < res.rows; ++r) {
    for (int c = 0; c < res.cols; ++c) {
      if (count.at<uint8_t>(r, c) == 0) {
        if (mask_.at<uint8_t>(r, c) == MASK_FOREGROUND) {
          res.at<double>(r, c) = 1.0;
        } else {
          res.at<double>(r, c) = 0.0;
        }
      } else {
        res.at<double>(r, c) = nume.at<double>(r, c) / deno.at<double>(r, c);
      }
    }
  }
  return res;
}

cv::Mat Frame::geometric_transformation(cv::Mat& img1, cv::Mat& img2,
                                        cv::Mat& mask) {
  std::vector<cv::KeyPoint> keypoints1, keypoints2;
  std::vector<cv::DMatch> matches;
  cv::Mat descriptors1, descriptors2;
  cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();
  cv::Ptr<cv::DescriptorExtractor> descriptor = cv::ORB::create();
  cv::Ptr<cv::DescriptorMatcher> matcher =
      cv::DescriptorMatcher::create("BruteForce-Hamming");

  detector->detect(img1, keypoints1, mask);
  detector->detect(img2, keypoints2);

  descriptor->compute(img1, keypoints1, descriptors1);
  descriptor->compute(img2, keypoints2, descriptors2);

  matcher->match(descriptors1, descriptors2, matches);

  double min_dist = 10000, max_dist = 0;
  for (int i = 0; i < descriptors1.rows; i++) {
    double dist = matches[i].distance;
    if (dist < min_dist) {
      min_dist = dist;
    }
    if (dist > max_dist) {
      max_dist = dist;
    }
  }

  std::vector<cv::Point2f> points1, points2;

  double threshold = std::max(2 * min_dist, 30.0);
  for (int i = 0; i < descriptors1.rows; i++) {
    if (matches[i].distance <= threshold) {
      points1.push_back(keypoints1[matches[i].queryIdx].pt);
      points2.push_back(keypoints2[matches[i].trainIdx].pt);
    }
  }

  cv::Mat H = cv::findHomography(points1, points2);
  return H;
}

void Frame::motion_propagate(Frame& prev) {
  // compute H
  cv::Mat H = geometric_transformation(prev.frame_, this->frame_, prev.mask_);
  // warp frame and mask
  cv::Mat warpped_frame, warpped_mask;
  cv::warpPerspective(prev.frame_, warpped_frame, H, this->frame_.size());
  cv::warpPerspective(prev.mask_, warpped_mask, H, prev.mask_.size());

  assert(warpped_mask.type() == CV_8UC1);
  // update mask
  this->mask_ = warpped_mask;
  // imshow("fuck", this->mask_);
  // cv::waitKey(0);
  contours_ = convert_mask_to_contours(mask_);
  boundary_distance_ = convert_mask_to_boundary_distance(mask_, contours_);
  
  // warp window centers
  std::vector<cv::Point2f> centers;
  auto& prev_windows = prev.get_windows();
  for (auto it = prev_windows.begin(); it != prev_windows.end(); ++it) {
    centers.push_back(it->second.get_center());
  }
  std::vector<cv::Point2f> warpped_centers(centers.size());
  cv::perspectiveTransform(centers, warpped_centers, H);
  int cnt = 0;
  for (auto it = prev_windows.begin(); it != prev_windows.end(); ++it) {
    windows_.emplace(
        std::make_pair(it->first, LocalWindow(warpped_centers[cnt], *this)));
    cnt++;
  }

  // optical flow
  cv::Mat warped_gray, img_2_gray;
  cv::cvtColor(warpped_frame, warped_gray, cv::COLOR_BGR2GRAY);
  cv::cvtColor(this->frame_, img_2_gray, cv::COLOR_BGR2GRAY);

  cv::Mat flow;
  cv::calcOpticalFlowFarneback(warped_gray, img_2_gray, flow, 0.5, 1, 10, 2,
                               7, 1.2, 0);

  for (auto it = windows_.begin(); it != windows_.end(); ++it) {
    it->second.update_center_optical_flow(flow);
  }
}

void Frame::update_windows(Frame& prev) {
  for (auto it = prev.windows_.begin(); it != prev.windows_.end(); ++it) {
    auto search = windows_.find(it->first);
    if (search != windows_.end()) {
      cv::Point center = search->second.get_center();
      if (boundary_distance_.at<double>(center) > HALF_WINDOW_LENGTH) {
        continue;
      }
      search->second.update_color_model(it->second);
      search->second.update_shape_confidence();
      search->second.integration();
    }
  }
}

void Frame::update_mask(cv::Mat& mask) {
  mask_ = mask.clone();
  contours_ = convert_mask_to_contours(mask_);
  boundary_distance_ = convert_mask_to_boundary_distance(mask_, contours_);
}

void Frame::update_user_mask(cv::Mat &mask) {
  user_mask_ = mask.clone();
}

QImage* Frame::getQImage() {
    cv::Mat *inputImage = new cv::Mat;
    cv::cvtColor(frame_, *inputImage, CV_BGR2RGB);
    QImage* img = new QImage((const unsigned char*)(inputImage->data),
                             inputImage->cols, inputImage->rows,
                             inputImage->cols * inputImage->channels(),
                             QImage::Format_RGB888);
    return img;
}

void Frame::addfgd(MyLine *line)
{
    std::cout << line << std::endl;
    fgdList.push_back(line);
    std::cout << "fuck" << std::endl;
}
