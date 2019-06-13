#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>
#include <set>
#include <string>

#include "video_snapcut.hpp"

// global variable
const char* format_string = "../resources/keyframe-%02d.jpg";

cv::Mat* global_mat_ptr = nullptr;
bool drawing = false;
int global_x = -1;
int global_y = -1;
// std::vector<std::pair<int, int>> coords;
std::vector<cv::Point> global_contour;
int mode;

void mouse_callback(int event, int x, int y, int flags, void* userdata) {
  if (event == CV_EVENT_LBUTTONDOWN) {
    drawing = true;
    global_x = x;
    global_y = y;
    global_contour.emplace_back(cv::Point(global_x, global_y));
  } else if (event == CV_EVENT_MOUSEMOVE) {
    if (drawing == true) {
      cv::line(*global_mat_ptr, cv::Point(global_x, global_y), cv::Point(x, y),
               cv::Scalar(0, 0, 255), 1);
      global_x = x;
      global_y = y;
      global_contour.emplace_back(cv::Point(global_x, global_y));
    }
  } else if (event == CV_EVENT_LBUTTONUP) {
    drawing = false;
    cv::line(*global_mat_ptr, cv::Point(global_x, global_y), cv::Point(x, y),
             cv::Scalar(0, 0, 255), 1);
    global_contour.emplace_back(cv::Point(global_x, global_y));
  }
}

static void create_initial_frame_boundry(cv::Mat& img) {
  cv::Mat initial_frame = img.clone();
  global_mat_ptr = &initial_frame;
  cv::setMouseCallback(windowname, mouse_callback, nullptr);
  std::cout << "User draw initial boundary" << std::endl;
  while (true) {
    cv::imshow(windowname, initial_frame);
    int key = cv::waitKey(1) & 0xFF;
    if (key == 27) {
      break;
    }
  }
  global_mat_ptr = nullptr;
  std::cout << "User draw complete" << std::endl;
}

static void construct_local_window(cv::Mat& img,
                                   std::vector<cv::Point>& contour) {
  assert(contour.size() != 0);
  cv::Mat img_back_up = img.clone();
  double distance = 0.0;
  cv::Point last_point;
  for (int i = 0; i < contour.size(); ++i) {
    cv::Point p = contour[i];
    if (i != 0) {
      int cx = p.x, cy = p.y, lx = last_point.x, ly = last_point.y;
      double d = sqrt((cx - lx) * (cx - lx) + (cy - ly) * (cy - ly));
      distance += d;
      if (distance >= 15.0) {
        cv::rectangle(img_back_up, cv::Rect(p.x - 25, p.y - 25, 50, 50),
                      cv::Scalar(0, 0, 255));
        distance = 0.0;
      }
    } else {
      cv::rectangle(img_back_up, cv::Rect(p.x - 25, p.y - 25, 50, 50),
                    cv::Scalar(0, 0, 255));
    }
    last_point = p;
  }
  while (true) {
    cv::imshow(windowname, img_back_up);
    int key = cv::waitKey(1) & 0xFF;
    if (key == 27) {
      break;
    }
  }
}

int main(int argc, char* argv[]) {
  cv::namedWindow(windowname, CV_WINDOW_AUTOSIZE);
  std::vector<Frame> frames;

  cv::Mat current_frame;
  int frame_cnt = 0;
  Frame* cur_frame_ptr = nullptr;
  Frame* prev_frame_ptr = nullptr;

  while (true) {
    char filename_buffer[128];
    sprintf(filename_buffer, format_string, frame_cnt + 1);
    std::string file = filename_buffer;
    current_frame = cv::imread(file);
    if (current_frame.data == nullptr) {
      break;
    }
      
    // speicial handle frame 0
    if (frame_cnt == 0) {
      // TODO: get mask from outside
      cv::Mat mask = imread("../resources/keyframe-01-mask.png", cv::IMREAD_GRAYSCALE);
      frames.emplace_back(frame_cnt, std::move(current_frame), std::move(mask));
      frames[0].initialize_windows();
      frames[0].show_windows();
      auto combined_map = frames[0].generate_combined_map();
      auto& windows = frames[0].get_windows();
      for (auto it = 0; it != windows.size(); ++it) {
        auto search = windows.find(it);
        // it->second.integration();
        show_probability_map(search->second.get_foreground_sample());
        show_probability_map(search->second.get_background_sample());
        show_probability_map(search->second.get_shape_confidence());
        show_probability_map(search->second.get_color_probability_map());
      }
      
      
      // show_probability_map(combined_map);
    } else {
      // frames.emplace_back(frame_cnt, std::move(current_frame));

    }
    ++frame_cnt;
  }
  std::cout << "total frames: " << frame_cnt << std::endl;
  
  cv::destroyAllWindows();
  return 0;
}