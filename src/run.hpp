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

    void doAlphaMatting(Frame *frame);

private:

    std::string format_string;
    std::string format_string_out;
    std::string format_string_prob;
    std::string format_string_cut;
    std::string mask_path;
    std::string trimap_path;

    std::vector<Frame> frames;
    int frame_cnt = 0;

};

#endif // RUN_H
