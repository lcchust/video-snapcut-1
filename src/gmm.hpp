#pragma once
#include <opencv2/opencv.hpp>
class GMM
{
public:
    static const int componentsCount = 5;

    GMM( cv::Mat& _model );
    double operator()( const cv::Vec3d color ) const;
    double operator()( int ci, const cv::Vec3d color ) const;
    int whichComponent( const cv::Vec3d color ) const;

    void initLearning();
    void addSample( int ci, const cv::Vec3d color );
    void endLearning();

private:
    void calcInverseCovAndDeterm(int ci, double singularFix);
    cv::Mat model;
    double* coefs;
    double* mean;
    double* cov;

    double inverseCovs[componentsCount][3][3];
    double covDeterms[componentsCount];

    double sums[componentsCount][3];
    double prods[componentsCount][3][3];
    int sampleCounts[componentsCount];
    int totalSampleCount;
};
