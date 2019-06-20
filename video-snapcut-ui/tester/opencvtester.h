#ifndef OPENCVTESTER_H
#define OPENCVTESTER_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

class OpencvTester
{
public:
    OpencvTester();

    static void test_ORB_pos_estimation(cv::Mat& img_1, cv::Mat& img_2, cv::Mat& mask, cv::Mat& im_out);
    static void test_optical_flow(cv::Mat& img_warped, cv::Mat& img_2);
};

#endif // OPENCVTESTER_H
