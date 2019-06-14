#include "graphcuttester.h"
#include <iostream>

GraphCutTester::GraphCutTester(cv::Mat& _img, cv::Mat& _probmap, cv::Mat& _mask):
    img(_img), probmap(_probmap), mask(_mask), beta(calc_beta())
{

    std::cout << beta << " " << gamma << std::endl;

    cv::Mat leftW = cv::Mat(img.rows, img.cols, CV_64FC1);
    cv::Mat upleftW = cv::Mat(img.rows, img.cols, CV_64FC1);
    cv::Mat upW = cv::Mat(img.rows, img.cols, CV_64FC1 );
    cv::Mat uprightW = cv::Mat(img.rows, img.cols, CV_64FC1);


    calc_NWeights(leftW, upleftW, upW, uprightW);


    std::cout << leftW <<std::endl;
    cv::imshow("up", upW);
    cv::waitKey(0);


    construct_graph(leftW, upleftW, upW, uprightW);
    graph.maxFlow();
}

double GraphCutTester::calc_beta()
{
    double beta = 0;
    for (int r = 0; r < img.rows; r++) {
        for(int c = 0; c < img.cols; c++) {
            cv::Vec3d color = img.at<cv::Vec3b>(r, c);
            if (c > 0) { // left
                cv::Vec3d diff = color - (cv::Vec3d)img.at<cv::Vec3b>(r, c - 1);
                beta += diff.dot(diff);
            }
            if (r > 0 && c > 0) { // upleft
                cv::Vec3d diff = color - (cv::Vec3d)img.at<cv::Vec3b>(r - 1, c - 1);
                beta += diff.dot(diff);
            }
            if (r > 0) { // up
                cv::Vec3d diff = color - (cv::Vec3d)img.at<cv::Vec3b>(r - 1, c);
                beta += diff.dot(diff);
            }
            if (r > 0 && c < img.cols - 1) { // upright
                cv::Vec3d diff = color - (cv::Vec3d)img.at<cv::Vec3b>(r - 1, c + 1);
                beta += diff.dot(diff);
            }
        }
    }
    if (beta <= std::numeric_limits<double>::epsilon())
        beta = 0;
    else
        beta = 1.f / (2 * beta / (4*img.cols*img.rows - 3*img.cols - 3*img.rows + 2));

    return beta;
}

void GraphCutTester::calc_NWeights(cv::Mat& leftW, cv::Mat& upleftW, cv::Mat& upW, cv::Mat& uprightW)
{
    const double gammaDivSqrt2 = gamma / std::sqrt(2.0f);

    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            cv::Vec3d color = img.at<cv::Vec3b>(r, c);

            if (c > 0) { // left
                cv::Vec3d diff = color - (cv::Vec3d)img.at<cv::Vec3b>(r, c - 1);
                leftW.at<double>(r, c) = gamma * exp(- beta * diff.dot(diff));
            } else {
                leftW.at<double>(r, c) = 0;
            }

            if (c > 0 && r > 0) { // upleft
                cv::Vec3d diff = color - (cv::Vec3d)img.at<cv::Vec3b>(r - 1, c - 1);
                upleftW.at<double>(r, c) = gammaDivSqrt2 * exp(- beta * diff.dot(diff));
            } else {
                upleftW.at<double>(r, c) = 0;
            }

            if (r > 0) { // up
                cv::Vec3d diff = color - (cv::Vec3d)img.at<cv::Vec3b>(r - 1, c);
                upW.at<double>(r, c) = gamma * exp(- beta * diff.dot(diff));
            } else {
                upW.at<double>(r, c) = 0;
            }

            if (r > 0 && c < img.cols - 1) { // upright
                cv::Vec3d diff = color - (cv::Vec3d)img.at<cv::Vec3b>(r - 1, c + 1);
                uprightW.at<double>(r, c) = gammaDivSqrt2 * exp(- beta * diff.dot(diff));
            } else {
                uprightW.at<double>(r, c) = 0;
            }
        }
    }
}

void GraphCutTester::construct_graph(const cv::Mat& leftW, const cv::Mat& upleftW,
                                     const cv::Mat& upW, const cv::Mat& uprightW)
{
    int vtxCount = img.cols * img.rows,
        edgeCount = 2 * (4*img.cols*img.rows - 3*(img.cols + img.rows) + 2);

    graph.create(vtxCount, edgeCount);

    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            // add node
            int vtxIdx = graph.addVtx();
            cv::Vec3d color = img.at<cv::Vec3b>(r, c);

            // set t-weights
            double toSink = -log( probmap.at<double>(r, c));
            double fromSource = -log( 1.0 - probmap.at<double>(r, c));
            graph.addTermWeights(vtxIdx, fromSource, toSink);

            // set n-weights
            if (c > 0) {
                double w = leftW.at<double>(r, c);
                graph.addEdges(vtxIdx, vtxIdx - 1, w, w);
            }
            if (c > 0 && r > 0) {
                double w = upleftW.at<double>(r, c);
                graph.addEdges(vtxIdx, vtxIdx - img.cols - 1, w, w);
            }
            if (r > 0) {
                double w = upW.at<double>(r, c);
                graph.addEdges(vtxIdx, vtxIdx - img.cols, w, w);
            }
            if (c < img.cols - 1 && r > 0) {
                double w = uprightW.at<double>(r, c);
                graph.addEdges(vtxIdx, vtxIdx - img.cols + 1, w, w);
            }
        }
    }
}

void GraphCutTester::get_segmentation(cv::Mat& segmask)
{
    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            if (graph.inSourceSegment(r * img.cols + c)) {
                //std::cout << "1 ";
                segmask.at<uchar>(r, c) = 255;
            } else {
                //std::cout << "0 ";
                segmask.at<uchar>(r, c) = 0;
            }
        }
        //std::cout << std::endl;
    }
}

