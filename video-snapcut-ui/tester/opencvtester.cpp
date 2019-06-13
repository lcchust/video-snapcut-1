#include "opencvtester.h"
#include <vector>

OpencvTester::OpencvTester()
{

}

void OpencvTester::test_ORB_pos_estimation(cv::Mat& img_1, cv::Mat& img_2, cv::Mat& mask, cv::Mat& im_out)
{
    cv::imshow("input1", img_1);
    cv::imshow("input2", img_2);
    cv::imshow("input3", mask);

    std::vector<cv::KeyPoint> keypoints_1, keypoints_2;
    std::vector<cv::DMatch> matches;

    cv::Mat descriptors_1, descriptors_2;
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();
    cv::Ptr<cv::DescriptorExtractor> descriptor = cv::ORB::create();
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");

    detector->detect(img_1, keypoints_1, mask);
    detector->detect(img_2, keypoints_2);

    descriptor->compute(img_1, keypoints_1, descriptors_1);
    descriptor->compute(img_2, keypoints_2, descriptors_2);

// display features
//    cv::Mat outimg1;
//    cv::drawKeypoints( img_1, keypoints_1, outimg1, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
//    cv::imshow("features", outimg1);

    std::vector<cv::DMatch> match;
    matcher->match(descriptors_1, descriptors_2, match);

    double min_dist = 10000, max_dist = 0;
    for (int i = 0; i < descriptors_1.rows; i++) {
        double dist = match[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

    std::vector<cv::Point2f> points_1, points_2;

    double threshold = std::max(2 * min_dist, 30.0);
    for (int i = 0; i < descriptors_1.rows; i++) {
        if (match[i].distance <= threshold) {
            matches.push_back(match[i]);
            points_1.push_back(keypoints_1[match[i].queryIdx].pt);
            points_2.push_back(keypoints_2[match[i].trainIdx].pt);
        }
    }

// display matches
//    cv::Mat img_match;
//    cv::Mat img_goodmatch;
//    cv::drawMatches(img_1, keypoints_1, img_2, keypoints_2, match, img_match);
//    cv::drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_goodmatch);
//    cv::imshow("all matches", img_match);
//    cv::imshow("good matches", img_goodmatch);

    cv::Mat h = cv::findHomography(points_1, points_2);
    cv::warpPerspective(img_1, im_out, h, img_2.size());

    cv::imshow("result", im_out);
    cv::waitKey(0);

    return;
}

void OpencvTester::test_optical_flow(cv::Mat &img_warped, cv::Mat &img_2)
{
    cv::Mat img_warped_gray, img_2_gray;
    cv::cvtColor(img_warped, img_warped_gray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img_2, img_2_gray, cv::COLOR_BGR2GRAY);

    cv::Mat flow;
    cv::calcOpticalFlowFarneback(img_warped_gray, img_2_gray, flow, 0.5, 1, 10, 2, 7, 1.2, 0);

    cv::Mat img_2_saved(img_2);

    for (int y = 0; y < img_warped.rows; y += 5) {
         for (int x = 0; x < img_warped.cols; x += 5) {
              const cv::Point2f flowatxy = flow.at<cv::Point2f>(y, x) * 10;
              cv::line(img_2_saved, cv::Point(x, y), cv::Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), cv::Scalar(255,0,0));
              cv::circle(img_2_saved, cv::Point(x, y), 1, cv::Scalar(0, 0, 0), -1);
         }
    }

    cv::imshow("prev_gray", img_warped_gray);
    cv::imshow("cur_gray", img_2_gray);
    cv::imshow("prew", img_2_saved);
    cv::waitKey(0);
}
