#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>
#include <set>
#include <string>

#include "video_snapcut.hpp"

// global variable
const char* format_string = "../resources/keyframe-%02d.jpg";
const char* format_string_out = "../resources/keyframeseg-%02d.jpg";
const char* format_string_prob = "../resources/keyframeprob-%02d.jpg";

// const char* format_string = "../resources/test3/test3-%02d.png";
// const char* format_string_out = "../resources/test3/keyframeseg-%02d.jpg";
// const char* format_string_prob = "../resources/test3/keyframeprob-%02d.jpg";

// const char* format_string = "../resources/Frames1/%d.jpg";
// const char* format_string_out = "../resources/Frames1/keyframeseg-%02d.jpg";
// const char* format_string_prob = "../resources/Frames1/keyframeprob-%02d.jpg";

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
      // cv::Mat mask = imread("../resources/hehe.png", cv::IMREAD_GRAYSCALE);

      cv::Mat mask =
          imread("../resources/keyframe-01-mask.png", cv::IMREAD_GRAYSCALE);
      // cv::Mat mask = imread("../resources/Frames1/turtle-mask.png", cv::IMREAD_GRAYSCALE);
      frames.emplace_back(frame_cnt, std::move(current_frame), std::move(mask));
      frames[0].initialize_windows();
      std::cout << "cur window_cnt " << Frame::window_id_cnt_ << std::endl;
    } else {
      frames.emplace_back(frame_cnt, std::move(current_frame));
      frames[frame_cnt].motion_propagate(frames[frame_cnt - 1]);
      // frames[frame_cnt].show_windows();
      frames[frame_cnt].update_windows(frames[frame_cnt - 1]);
      cv::Mat combined_map = frames[frame_cnt].generate_combined_map();
      char filename_buffer[128];
      sprintf(filename_buffer, format_string_prob, frame_cnt + 1);
      std::string file = filename_buffer;
      save_probability_map(combined_map, file);
      cv::Mat prob = imread(file, cv::IMREAD_GRAYSCALE);
      prob.convertTo(prob, CV_64FC1, 1.0 / 255.0);
      GraphCutTester gc(frames[frame_cnt].get_frame(), prob);
      cv::Mat seg = cv::Mat(prob.rows, prob.cols, CV_8UC1);
      gc.get_segmentation(seg);
      frames[frame_cnt].update_mask(seg);
      frames[frame_cnt].move_windows();
      
       //frames[frame_cnt].initialize_windows();
      for (int i = 0; i < 3; ++i) {
        frames[frame_cnt].update_windows(frames[frame_cnt - 1]);
        cv::Mat combined_map = frames[frame_cnt].generate_combined_map();
        sprintf(filename_buffer, format_string_prob, frame_cnt + 1);
        std::string file = filename_buffer;
        save_probability_map(combined_map, file);
        prob = imread(file, cv::IMREAD_GRAYSCALE);
        prob.convertTo(prob, CV_64FC1, 1.0 / 255.0);
        GraphCutTester gc(frames[frame_cnt].get_frame(), prob);
        cv::Mat seg = cv::Mat(prob.rows, prob.cols, CV_8UC1);
        gc.get_segmentation(seg);
        frames[frame_cnt].update_mask(seg);
         frames[frame_cnt].move_windows();
        //frames[frame_cnt].initialize_windows();
      }
      // frames[frame_cnt].show_windows();
      sprintf(filename_buffer, format_string_out, frame_cnt + 1);
      file = filename_buffer;
      cv::imwrite(file, seg);
      cv::Mat fuck;
      frames[frame_cnt].get_frame().copyTo(fuck, seg);
      cv::imwrite("book-res-new" + std::to_string(frame_cnt) + ".png", fuck);
      std::cout << "frame: " << frame_cnt << std::endl;
      std::cout << "cur window_cnt " << Frame::window_id_cnt_ << std::endl;
    }
    ++frame_cnt;
  }
  std::cout << "total frames: " << frame_cnt << std::endl;

  cv::destroyAllWindows();
  return 0;
}