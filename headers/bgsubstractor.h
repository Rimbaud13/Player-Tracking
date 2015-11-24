#ifndef TEAM_MEMBERSHIP_DETECTOR_BGSUBSTRACTOR_H
#define TEAM_MEMBERSHIP_DETECTOR_BGSUBSTRACTOR_H

#include <exception>
#include <stdexcept>
#include <opencv2/highgui/highgui.hpp>
#include "frame_t.h"

#define TMD_BGS_DETECTS_SHADOWS false

namespace tmd {
    /* Class responsible of applying a BG substraction on a given video.
     * The BGS works as an iterator over the input video. You can extract
     * frames from the video one by one and apply a background substraction
     * on them.
     */

    class BGSubstractor {
    public:
        /**
         * Constructor of the Background Substractor.
         * input_video : The video to operate on.
         * camera_index : The index of the camera.
         * threshold : Threshold for the color distance to use during
         *              computation.
         * history : Size of the history to keep for the background.
         * learning_rate : Learning rate of the algorithm, ie. how does the
         * background model changes over time.
         */
        BGSubstractor(cv::VideoCapture *input_video, unsigned char camera_index,
                      float threshold = 256, int history = 500,
                      float learning_rate = 0.0);

        /**
         * Destructor of the BGS.
         */
        ~BGSubstractor();

        /**
         * Extract the next frame from the input_video.
         */
        frame_t *next_frame();

        /**
         * Set the bgs to a given frame.
         */
        bool jump_to_frame(int index);

        /**
         * Set the color distance to use when extracting the background.
         */
        void set_threshold_value(float th);

        /**
         * Set the size of the history.
         */
        void set_history_size(int s);

        /**
         * Set the learning rate to use.
         */
        void set_learning_rate(float lr);

    private:
        cv::Ptr<cv::BackgroundSubtractorMOG2> m_bgs;
        cv::VideoCapture *m_input_video;
        cv::Mat mCurrentFrame;
        unsigned char m_camera_index;
        double m_frame_index;
        double m_total_frame_count;
        float m_learning_rate;
    };
}

#endif //TEAM_MEMBERSHIP_DETECTOR_BGSUBSTRACTOR_H
