#include "run.hpp"
#include <QMessageBox>
#include "../video-snapcut-ui/tester/sharedmatting.h"

#define BEGIN_NUM 1
#define ITERATIONS 3

Run::Run(std::string _path)
{
    // turtle
    //format_string = _path + "/%02d.jpg";
    // book
    format_string = _path + "/keyframe-%02d.jpg";
    // car
    //format_string = _path + "/test3-%02d.png";
    format_string_out = _path + "/frameseg-%02d.png";
    format_string_prob = _path + "/frameprob-%02d.png";
    format_string_cut = _path + "/cut-%02d.png";

    format_string_out_alpha = _path + "/framesegalpha-%02d.png";
    format_string_cut_alpha = _path + "/cutalpha-%02d.png";

    mask_path = _path + "/mask.png";
    trimap_path = _path + "/trimap.png";
}

bool Run::forward()
{
    char filename_buffer[128];
    sprintf(filename_buffer, format_string.c_str(), frame_cnt + BEGIN_NUM);
    std::string file = filename_buffer;
    cv::Mat current_frame;

    std::cout << file << std::endl;

    current_frame = cv::imread(file);
    if (current_frame.data == nullptr) {
        QMessageBox::about(NULL, "Warn", "image open failed!");
        return false;
    }

    if (frame_cnt == 0) {
        cv::Mat mask = imread(mask_path, cv::IMREAD_GRAYSCALE);
        frames.emplace_back(frame_cnt, std::move(current_frame), std::move(mask));
        frames[0].initialize_windows();

        cv::Mat fuck;
        frames[frame_cnt].get_frame().copyTo(fuck, mask);
        sprintf(filename_buffer, format_string_cut.c_str(), frame_cnt + BEGIN_NUM);
        file = filename_buffer;
        cv::imwrite(file, fuck);
        // frames[0].show_windows();
        // save_probability_map(combined_map, "p_f1.png");
    }
    else {
        frames.emplace_back(frame_cnt, std::move(current_frame));
        frames[frame_cnt].motion_propagate(frames[frame_cnt - 1]);
        // frames[frame_cnt].show_windows();

        frames[frame_cnt].update_windows(frames[frame_cnt - 1]);
        cv::Mat combined_map = frames[frame_cnt].generate_combined_map();

        char filename_buffer[128];
        sprintf(filename_buffer, format_string_prob.c_str(), frame_cnt + BEGIN_NUM);
        std::string file = filename_buffer;
        save_probability_map(combined_map, file);
        std::cout << "  [OK!]: prob_map generated!" << std::endl;
        cv::Mat prob = imread(file, cv::IMREAD_GRAYSCALE);
        prob.convertTo(prob, CV_64FC1, 1.0 / 255.0);
        //cv::imshow("prob", prob);

        GraphCutTester gc(frames[frame_cnt].get_frame(), prob, frames[frame_cnt].get_user_mask());
        cv::Mat seg = cv::Mat(prob.rows, prob.cols, CV_8UC1);
        gc.get_segmentation(seg);
        std::cout << "  [OK!]: segmentation generated!" << std::endl;
        sprintf(filename_buffer, format_string_out.c_str(), frame_cnt + BEGIN_NUM);
        file = filename_buffer;
        cv::imwrite(file, seg);

        frames[frame_cnt].update_mask(seg);
        frames[frame_cnt].initialize_windows();
        for (int i = 0; i < ITERATIONS; ++i) {
          frames[frame_cnt].update_windows(frames[frame_cnt - 1]);
          cv::Mat combined_map = frames[frame_cnt].generate_combined_map();
          sprintf(filename_buffer, format_string_prob.c_str(), frame_cnt + BEGIN_NUM);
          std::string file = filename_buffer;
          save_probability_map(combined_map, file);
          prob = imread(file, cv::IMREAD_GRAYSCALE);
          prob.convertTo(prob, CV_64FC1, 1.0 / 255.0);
          GraphCutTester gc(frames[frame_cnt].get_frame(), prob, frames[frame_cnt].get_user_mask());
          cv::Mat seg = cv::Mat(prob.rows, prob.cols, CV_8UC1);
          gc.get_segmentation(seg);

          sprintf(filename_buffer, format_string_out.c_str(), frame_cnt + BEGIN_NUM);
          file = filename_buffer;
          cv::imwrite(file, seg);

          frames[frame_cnt].update_mask(seg);
          frames[frame_cnt].initialize_windows();
          std::cout << "    iter: " << i << std::endl;
        }
        // frames[frame_cnt].show_windows();

        cv::Mat fuck;
        frames[frame_cnt].get_frame().copyTo(fuck, seg);
        sprintf(filename_buffer, format_string_cut.c_str(), frame_cnt + BEGIN_NUM);
        file = filename_buffer;
        cv::imwrite(file, fuck);
        std::cout << "frame: " << frame_cnt << std::endl;

        img_num = frame_cnt + BEGIN_NUM;
        // break;
        // save_probability_map(combined_map, "2.png");
        // getchar();
    }

    std::cout << "[OK!]: frame_cnt is " << frame_cnt << std::endl;
    ++frame_cnt;

    return true;
}

void Run::redo()
{
    char filename_buffer[128];
    sprintf(filename_buffer, format_string_out.c_str(), frame_cnt + BEGIN_NUM - 1);
    std::string file = filename_buffer;
    cv::Mat prob = imread(file, cv::IMREAD_GRAYSCALE);
    prob.convertTo(prob, CV_64FC1, 1.0 / 255.0);
    //cv::imshow("prob", prob);

    GraphCutTester gc(frames[frame_cnt - 1].get_frame(), prob, frames[frame_cnt - 1].get_user_mask());
    cv::Mat seg = cv::Mat(prob.rows, prob.cols, CV_8UC1);
    gc.get_segmentation(seg);
    std::cout << "  [OK!]: segmentation generated!" << std::endl;
    cv::imwrite(file, seg);

    frames[frame_cnt - 1].update_mask(seg);
    frames[frame_cnt - 1].initialize_windows();

//    for (int i = 0; i < 2; ++i) {
//      cv::Mat combined_map = frames[frame_cnt - 1].generate_combined_map();
//      sprintf(filename_buffer, format_string_prob.c_str(), frame_cnt + BEGIN_NUM - 1);
//      std::string file = filename_buffer;
//      save_probability_map(combined_map, file);
//      prob = imread(file, cv::IMREAD_GRAYSCALE);
//      prob.convertTo(prob, CV_64FC1, 1.0 / 255.0);
//      GraphCutTester gc(frames[frame_cnt - 1].get_frame(), prob, frames[frame_cnt - 1].get_user_mask());
//      seg = cv::Mat(prob.rows, prob.cols, CV_8UC1);
//      gc.get_segmentation(seg);
//      frames[frame_cnt - 1].update_mask(seg);
//      frames[frame_cnt - 1].initialize_windows();
//      std::cout << "    iter: " << i << std::endl;
//    }
    // frames[frame_cnt].show_windows();

    sprintf(filename_buffer, format_string_out.c_str(), frame_cnt + BEGIN_NUM - 1);
    file = filename_buffer;
    cv::imwrite(file, seg);
    cv::Mat fuck;
    frames[frame_cnt - 1].get_frame().copyTo(fuck, seg);
    sprintf(filename_buffer, format_string_cut.c_str(), frame_cnt + BEGIN_NUM - 1);
    file = filename_buffer;
    cv::imwrite(file, fuck);
    std::cout << "  [OK!] finish redo" << std::endl;
}

void Run::doAlphaMatting(Frame *frame)
{
    // generate trimap;
    cv::Mat trimap;
    cv::Mat& boundary = frame->get_boundary_distance();
    frame->get_mask().copyTo(trimap);

    for (int r = 0; r < boundary.rows; ++r) {
        for (int c = 0; c < boundary.cols; ++c) {
            double dis = boundary.at<double>(r, c);
            if (dis < 5) {
                trimap.at<uchar>(r, c) = 127;
            }
        }
    }
    cv::cvtColor(trimap, trimap, CV_GRAY2BGR);

    SharedMatting sm;
    sm.loadImage(frame->get_frame());
    sm.loadTrimap(trimap);
    sm.solveAlpha();
    std::cout << "  [OK!]: alpha matting done!" << std::endl;

    char filename_buffer[128];
    sprintf(filename_buffer, format_string_out_alpha.c_str(), frame->get_frame_id() + BEGIN_NUM);
    sm.save(filename_buffer);

    cv::Mat img_1, alpha;
    frame->get_frame().convertTo(img_1, CV_32FC3);
    sm.getMatteMat().convertTo(alpha, CV_32FC1, 1.0/255);
    cv::cvtColor(alpha, alpha, CV_GRAY2BGR);
    multiply(alpha, img_1, img_1);

    sprintf(filename_buffer, format_string_cut_alpha.c_str(), frame->get_frame_id() + BEGIN_NUM);
    cv::imwrite(filename_buffer, img_1);
}

void Run::doAlphaMatting(Frame *frame, cv::Mat& user_mask)
{
    // generate trimap;
    cv::Mat& boundary = frame->get_boundary_distance();
    cv::Mat trimap;
    frame->get_mask().copyTo(trimap);

    for (int r = 0; r < boundary.rows; ++r) {
        for (int c = 0; c < boundary.cols; ++c) {
            uchar user_color = user_mask.at<uchar>(r, c);
            if (user_color == 0 || user_color == 255)
                continue;
            double dis = boundary.at<double>(r, c);
            if (dis < 5 || user_color == 127) {
                trimap.at<uchar>(r, c) = 127;
            }
        }
    }
    cv::cvtColor(trimap, trimap, CV_GRAY2BGR);

    SharedMatting sm;
    sm.loadImage(frame->get_frame());
    sm.loadTrimap(trimap);
    sm.solveAlpha();
    std::cout << "    [OK!]: alpha matting done!" << std::endl;

    char filename_buffer[128];
    sprintf(filename_buffer, format_string_out_alpha.c_str(), frame->get_frame_id() + BEGIN_NUM);
    sm.save(filename_buffer);

    cv::Mat img_1, alpha;
    frame->get_frame().convertTo(img_1, CV_32FC3);
    sm.getMatteMat().convertTo(alpha, CV_32FC1, 1.0/255);
    cv::cvtColor(alpha, alpha, CV_GRAY2BGR);
    multiply(alpha, img_1, img_1);

    sprintf(filename_buffer, format_string_cut_alpha.c_str(), frame->get_frame_id() + BEGIN_NUM);
    cv::imwrite(filename_buffer, img_1);
}

Frame* Run::getCurFrame()
{
    //std::cout << "inside runner frames[" << frame_cnt << "]: " << &(frames[frame_cnt - 1]) << std::endl;
    return &(frames[frame_cnt - 1]);
}

std::string Run::getCurFramePath(Frame *frame)
{
    char filename_buffer[128];
    sprintf(filename_buffer, format_string.c_str(), frame->get_frame_id() + BEGIN_NUM);
    std::string file = filename_buffer;
    return file;
}

std::string Run::getCurFrameSegPath(Frame *frame)
{
    char filename_buffer[128];
    sprintf(filename_buffer, format_string_out.c_str(), frame->get_frame_id() + BEGIN_NUM);
    std::string file = filename_buffer;
    return file;
}

std::string Run::getCurFrameCutPath(Frame *frame)
{
    char filename_buffer[128];
    sprintf(filename_buffer, format_string_cut.c_str(), frame->get_frame_id() + BEGIN_NUM);
    std::string file = filename_buffer;
    return file;
}

std::string Run::getCurFrameSegAlphaPath(Frame *frame)
{
    char filename_buffer[128];
    sprintf(filename_buffer, format_string_out_alpha.c_str(), frame->get_frame_id() + BEGIN_NUM);
    std::string file = filename_buffer;
    return file;
}

std::string Run::getCurFrameCutAlphaPath(Frame *frame)
{
    char filename_buffer[128];
    sprintf(filename_buffer, format_string_cut_alpha.c_str(), frame->get_frame_id() + BEGIN_NUM);
    std::string file = filename_buffer;
    return file;
}

