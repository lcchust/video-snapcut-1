#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>
#include <set>
#include <string>

// global variable
const std::string filename = "../resources/cv_desk.png";
const std::string windowname = "video snapcut";
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

static void construct_local_window(cv::Mat& img, std::vector<cv::Point>& contour) {
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

int main() {
  // load image
  cv::Mat img = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
  assert(img.data != nullptr);
  // // create window
  cv::namedWindow(windowname, CV_WINDOW_AUTOSIZE);
  create_initial_frame_boundry(img);
  construct_local_window(img, global_contour);

  // // construct local window
  // for (int i = 0; i < coords.size(); ++i) {
  //   if (i % 10 == 0) {
  //     int x = coords[i].first;
  //     int y = coords[i].second;
  //     cv::rectangle(img_back_up, cv::Rect(x - 25, y - 25, 50, 50),
  //                   cv::Scalar(0, 0, 255));
  //   }
  // }
  // cv::imshow(windowname, img_back_up);
  // cv::waitKey(0);

  // // the user drawed forms a contour
  // // using the contour and pointPolygonTest to test whether a point inside
  // // cv::pointPolygonTest(contour, cv::Point(1, 1), true); // use true for
  // // distance for each window, train local classifier

  // // load next image

  // // use feature matching and affine transformation get large motion
  // // use optical flow to get small motion

  // // estimate new boundary
  // // update classifier

  // cv::destroyAllWindows();
  return 0;
}