#include <iostream>
#include "../headers/frame_t.h"
#include "../headers/test_cases/test_suite.h"
#include "../headers/manual_player_extractor.h"
#include "../headers/calibration_tool.h"
#include "../headers/dpm_player_extractor.h"
#include "../headers/features_extractor.h"
#include "../headers/feature_comparator.h"
#include "../headers/dpm_calibrator.h"
#include "../headers/pipeline.h"
#include "../headers/training_set_creator.h"

#include "../headers/sdl_binds/sdl_binds.h"
#include "../headers/simple_pipeline.h"
#include "../headers/pipeline_thread.h"
#include "../headers/multithreaded_pipeline.h"
#include "../headers/dpm.h"

void show_body_parts(cv::Mat image, tmd::player_t *p);

void extract_player_image(void);

void dpm_feature_extractor_test(void);

void pipeline(void);

void pipeline_class_tests(void);

void dpm_whole_frame(void);

void test_blob_separation(void);

void create_training_set(void);

void memleak_video_capture(void) {
    tmd::BGSubstractor bgs("./res/videos/alone-red-no-ball/", 0, 300, 10);
    while (1) {
        tmd::frame_t *frame = bgs.next_frame();
        cv::imshow("FGrame", frame->mask_frame);
        cv::waitKey(0);
        tmd::free_frame(frame);
    }
}

void test_fast_dpm(void) {
    tmd::DPM fastDPM;
    tmd::frame_t blob;
    blob.frame_index = 0;
    blob.camera_index = 0;
    blob.mask_frame = cv::imread(
            "./res/manual_extraction/frame5847_originalimage0.jpg", 0);
    blob.original_frame = cv::imread(
            "./res/manual_extraction/frame5847_originalimage0.jpg");
    double t1 = cv::getTickCount();
    fastDPM.extract_players_and_body_parts(&blob);
    double t2 = cv::getTickCount();
    std::cout << "Time = " << (t2 - t1) / cv::getTickFrequency() << std::endl;
    std::cout << "end" << std::endl;
}

int main(int argc, char *argv[]) {
    /*create_training_set();
    return 0;
     */
    tmd::Config::load_config();
    /*tmd::Pipeline *pipeline = new tmd::MultithreadedPipeline(
                                      "./res/videos/uni-hockey/ace_0.mp4", 2,
            "./res/xmls/person.xml");*/
    tmd::Pipeline *pipeline = new tmd::MultithreadedPipeline("./res/videos/uni-hockey/", 0, 4, 0, 10, 1);
    tmd::frame_t *frame = pipeline->next_frame();

    double t1 = cv::getTickCount();
    int count = 0;
    int max_frames = -1;
    std::string folder = "./res/pipeline_results/complete_pipeline/uni/with "
            "blob separator/";

    while (frame != NULL) {
        std::string frame_index = std::to_string(count);
        std::string file_name = folder + "/frame" + frame_index + ".jpg";
        cv::imwrite(file_name, tmd::draw_player_on_frame(frame, true));
        std::cout << "Save frame " << frame_index << std::endl;
        tmd::free_frame(frame);
        frame = pipeline->next_frame();
        count++;
        if (count == max_frames) {
            break;
        }
    }

    delete pipeline;
    double t2 = cv::getTickCount();
    std::cout << "Time = " << (t2 - t1) / cv::getTickFrequency() << std::endl;
    return EXIT_SUCCESS;
}


void create_training_set(void) {
    tmd::Config::load_config();

    tmd::TrainingSetCreator *trainer = new tmd::TrainingSetCreator("./res/videos/uni-hockey/", 0,
                                                                   "./res/xmls/person.xml", 0, 300, 1);
    tmd::frame_t *frame = trainer->next_frame();

    int count = 0;
    int max_frames = 10;

    while (frame != NULL) {
        std::string frame_index = std::to_string(count);
        std::cout << "Finished frame " << frame_index << std::endl;
        tmd::free_frame(frame);
        frame = trainer->next_frame();
        count++;
        if (count == max_frames) {
            break;
        }
    }
    tmd::free_frame(frame);
    trainer->write_centers();
    delete trainer;
}

void test_blob_separation(void) {
    tmd::player_t *player = new tmd::player_t;
    player->original_image = cv::imread(
            "./res/manual_extraction/frame5847_originalimage0.jpg");
    player->mask_image = cv::imread(
            "./res/manual_extraction/frame5847_maskimage0.jpg", 0);
    player->frame_index = 0;

    std::vector<tmd::player_t *> players;
    players.push_back(player);

    std::vector<tmd::player_t *> new_players = tmd::BlobSeparator::separate_blobs
            (players);

    std::cout << "new_player size = " << new_players.size() << std::endl;

    for (int i = 0; i < new_players.size(); i++) {
        cv::imshow("Player", new_players[i]->original_image);
        cv::waitKey(0);
    }
}

void dpm_whole_frame(void) {
    tmd::player_t *player = new tmd::player_t;
    player->original_image = cv::imread("./res/images/uni0.jpg");
    const int rows = player->original_image.rows;
    const int cols = player->original_image.cols;
    player->mask_image = cv::Mat(rows, cols, CV_8U);
    for (int l = 0; l < rows; l++) {
        for (int j = 0; j < cols; j++) {
            player->mask_image.at<uchar>(l, j) = 255;
        }
    }

    tmd::DPMDetector dpmDetector;
    dpmDetector.extractBodyParts(player);
    show_body_parts(player->original_image, player);
}

void show_body_parts(cv::Mat image, tmd::player_t *p) {
    std::vector<cv::Rect> parts = p->features.body_parts;
    CvScalar color;
    color.val[0] = 255;
    color.val[1] = 0;
    color.val[2] = 255;
    color.val[3] = 255;
    CvScalar torso;
    torso.val[0] = 255;
    torso.val[1] = 255;
    torso.val[2] = 0;
    torso.val[3] = 255;
    const int thickness = 1;
    const int line_type = 8; // 8 connected line.
    const int shift = 0;
    cv::Mat destImg = image.clone();
    std::cout << "Parts count = " << parts.size() << std::endl;
    for (int i = parts.size(); i > 0; i--) {
        if (i % 6 == 0) {
            std::cout << "i = " << i << std::endl;
            destImg = image.clone();
        }
        CvRect r;
        r.x = parts[i].x;
        r.y = parts[i].y;
        r.width = parts[i].width;
        r.height = parts[i].height;
        cv::rectangle(destImg, r, color, thickness, line_type, shift);
        cv::imshow("Body parts", destImg);
        cv::waitKey(0);
    }
}