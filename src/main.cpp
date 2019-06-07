#include <opencv2/opencv.hpp>

#include <iostream>
#include <set>
#include <string>
cv::Mat img;
bool drawing = false;
int global_x = -1;
int global_y = -1;
const std::string filename = "../resources/cv_desk.png";
const std::string windowname = "video snapcut";
std::vector<std::pair<int, int>> coords;
std::vector<cv::Point> contour;
int mode;
void mouse_callback(int event, int x, int y, int flags, void* userdata) {
  if (event == CV_EVENT_LBUTTONDOWN) {
    drawing = true;
    global_x = x;
    global_y = y;
    coords.emplace_back(std::make_pair(global_x, global_y));
  } else if (event == CV_EVENT_MOUSEMOVE) {
    if (drawing == true) {
      cv::line(img, cv::Point(global_x, global_y), cv::Point(x, y),
               cv::Scalar(0, 0, 255), 1);
      global_x = x;
      global_y = y;
      coords.emplace_back(std::make_pair(global_x, global_y));
    }
  } else if (event == CV_EVENT_LBUTTONUP) {
    drawing = false;
    cv::line(img, cv::Point(global_x, global_y), cv::Point(x, y),
             cv::Scalar(0, 0, 255), 1);
    coords.emplace_back(std::make_pair(x, y));
  }
}

void mouse_callback1(int event, int x, int y, int flags, void* userdata) {
  if (event == CV_EVENT_LBUTTONDOWN) {
    int res = cv::pointPolygonTest(contour, cv::Point(x, y), false);
    if (res == 1) {
      std::cout << "true" << std::endl;
    } else {
      std::cout << "false" << std::endl;
    }
  }
}

int main() {
  img = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
  cv::Mat img_back_up = img.clone();
  cv::namedWindow(windowname, CV_WINDOW_AUTOSIZE);

  cv::setMouseCallback(windowname, mouse_callback, nullptr);
  while (true) {
    cv::imshow(windowname, img);
    int key = cv::waitKey(1) & 0xFF;
    if (key == 27) {
      break;
    }
  }
  // construct local window
  for (int i = 0; i < coords.size(); ++i) {
    if (i % 10 == 0) {
      int x = coords[i].first;
      int y = coords[i].second;
      cv::rectangle(img_back_up, cv::Rect(x - 25, y - 25, 50, 50),
                    cv::Scalar(0, 0, 255));
    }
  }
  // cv::rectangle(img_back_up, cv::Rect(200, 200, 200, 200),
  //               cv::Scalar(0, 0, 255));
  cv::imshow(windowname, img_back_up);
  cv::waitKey(0);
  // contour.emplace_back(cv::Point(200, 200));
  // contour.emplace_back(cv::Point(200, 400));
  // contour.emplace_back(cv::Point(400, 400));
  // contour.emplace_back(cv::Point(400, 200));

  // the user drawed forms a contour
  // using the contour and pointPolygonTest to test whether a point inside
  // cv::pointPolygonTest(contour, cv::Point(1, 1), true); // use true for
  // distance for each window, train local classifier

  // load next image

  // use feature matching and affine transformation get large motion
  // use optical flow to get small motion

  // estimate new boundary
  // update classifier
  // std::vector<std::vector<cv::Point>> tmp;
  // tmp.push_back(contour);
  // cv::drawContours(img_back_up, tmp, 0, cv::Scalar(0, 0, 255));
  // cv::setMouseCallback(windowname, mouse_callback1, nullptr);
  // while (true) {
  //   cv::imshow(windowname, img_back_up);
  //   int key = cv::waitKey(1) & 0xFF;
  //   if (key == 27) {
  //     break;
  //   }
  // }
  cv::destroyAllWindows();
  return 0;
}