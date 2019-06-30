#ifndef RUN_H
#define RUN_H

#include <iostream>
#include <string>
#include <cstdio>

#include "video_snapcut.hpp"

class Run
{
public:
    Run(std::string _path);
    bool forward();
    void redo();
    Frame* getCurFrame();

    std::string getCurFramePath(Frame *frame);
    std::string getCurFrameSegPath(Frame *frame);
    std::string getCurFrameCutPath(Frame *frame);
    std::string getCurFrameSegAlphaPath(Frame *frame);
    std::string getCurFrameCutAlphaPath(Frame *frame);

    void doAlphaMatting(Frame *frame);
    void doAlphaMatting(Frame *frame, cv::Mat& user_mask);

private:

    std::string format_string;
    std::string format_string_prob;
    std::string format_string_out;
    std::string format_string_cut;
    std::string format_string_out_alpha;
    std::string format_string_cut_alpha;
    std::string mask_path;
    std::string trimap_path;

    std::vector<Frame> frames;
    int frame_cnt = 0;
    int img_num;

};

#endif // RUN_H
