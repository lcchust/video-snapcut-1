#ifndef GRAPHCUTTESTER_H
#define GRAPHCUTTESTER_H

#include <opencv2/opencv.hpp>
#include "gcgraph.hpp"

class GraphCutTester
{
public:
    GraphCutTester(cv::Mat& _img, cv::Mat& _probmap, cv::Mat& _mask);

    double calc_beta();
    void calc_NWeights(cv::Mat& leftW, cv::Mat& upleftW, cv::Mat& upW, cv::Mat& uprightW);
    void construct_graph(const cv::Mat& leftW, const cv::Mat& upleftW,
                          const cv::Mat& upW, const cv::Mat& uprightW);
    void get_segmentation(cv::Mat& segmask);

private:
    cv::Mat& img;
    cv::Mat& probmap;
    cv::Mat& mask;

    cv::detail::GCGraph<double> graph;

    const double beta;
    const double gamma = 50;
    const double lambda = 9 * gamma;
};

#endif // GRAPHCUTTESTER_H
