#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

const std::string windowname = "video snapcut";

std::string cv_type_to_string(int type);

void show_probability_map(cv::Mat& map);

void save_probability_map(cv::Mat& map, const std::string& filename);

double cross2d(cv::Vec2d a, cv::Vec2d b);