#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>
#include <set>
#include <string>

#include "video_snapcut.hpp"

// global variable
const char* format_string = "../resources/keyframe-%02d.jpg";

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
      cv::Mat mask = imread("../resources/keyframe-01-mask.png", cv::IMREAD_GRAYSCALE);
      frames.emplace_back(frame_cnt, std::move(current_frame), std::move(mask));
      frames[0].initialize_windows();
      frames[0].show_windows();
      auto combined_map = frames[0].generate_combined_map();
      show_probability_map(combined_map);
      // save_probability_map(combined_map, "p_f1.png");
    } else {
      // TODO:
      // frames.emplace_back(frame_cnt, std::move(current_frame));

    }
    ++frame_cnt;
  }
  std::cout << "total frames: " << frame_cnt << std::endl;
  
  cv::destroyAllWindows();
  return 0;
}