#include "opencvtester.h"
#include <vector>

OpencvTester::OpencvTester()
{

}

void OpencvTester::testORBPoseEstimation(cv::Mat& img_1, cv::Mat& img_2, cv::Mat& mask)
{
//    cv::imshow("input1", img_1);
//    cv::imshow("input2", img_2);
//    cv::imshow("input3", mask);

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
    cv::Mat im_out;
    cv::warpPerspective(img_1, im_out, h, img_2.size());

//    cv::imshow("result", im_out);
//    cv::waitKey(0);

    return;
}
