#include "../headers/pipeline.h"
#include "../headers/dpm_player_extractor.h"
#include "../headers/debug.h"

namespace tmd{
    Pipeline::Pipeline(std::string video_path, unsigned char camera_index,
                       std::string model_file, bool dpm, bool save_frames,
                       std::string output_folder) {
        m_video_path = video_path;
        m_video = new cv::VideoCapture;
        m_video->open(video_path);

        if (!m_video->isOpened()){
            throw std::invalid_argument("Error couldn't load the video in the"
                                                " piepline.");
        }

        m_camera_index = camera_index;
        m_bgSubstractor = new BGSubstractor(m_video, camera_index);

        if (dpm){
            m_playerExtractor = new DPMPlayerExtractor(model_file);
        }
        else{
            // TODO : Create a blob-detection instance.
        }

        m_featuresComparator = new FeatureComparator(2, 180,
                             FeatureComparator::readCentersFromFile(180, 1));

        m_running = false;
        m_using_dpm = dpm;
        m_start = 0;
        m_step = 1;
        m_end = -1;

        m_save = save_frames;
        m_output_folder = output_folder;
    }

    Pipeline::~Pipeline() {
        delete m_video;
        delete m_bgSubstractor;
        delete m_playerExtractor;
        delete m_featuresExtractor;
        delete m_featuresComparator;
    }

    frame_t* Pipeline::next_frame() {
        m_running = true;

        frame_t* frame = m_bgSubstractor->next_frame();
       	if (frame == NULL){
		    return NULL;
	    }

        std::vector<tmd::player_t*> players =
                m_playerExtractor->extract_player_from_frame(frame);

        tmd::debug("Pipeline", "next_frame", "Frame " + std::to_string
            (m_bgSubstractor->get_current_frame_index()) + " : " +
                std::to_string(players.size()) + " players detected");

        m_featuresExtractor->extractFeaturesFromPlayers(players);

        // TODO : run feature comparator.
    }
}
