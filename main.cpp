#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "image_holder.h"
#include "tracking_person.h"
#include "people_detector.h"
#include "people_tracker.h"

using namespace std;

class App
{
public:
    App()
    {

    }

    void Run()
    {
        cv::VideoCapture capture("Resources/768x576.avi");
        cv::Mat frame;
        // for debug
        cv::Mat draw_mat;

        // i do not know why this is here, ref. bgfg_gmg.cpp
        cv::initModule_video();

        ImageHolder image_holder;
        image_holder.Init();

        std::vector<TrackingPerson> tracking_people;

        PeopleDetector people_detector;
        people_detector.Init();

        PeopleTracker people_tracker;

//        int64 start_time;

        while (true) {
//            start_time = cv::getTickCount();

            capture >> frame;
            if (frame.empty()) {
                break;
            }

            // image_holder has color, gray, diff images
            image_holder.Update(frame);

            // move rects of people with lucas-kanade optical flow
            people_tracker.Track(image_holder, tracking_people);
            // add new people to tracking_people
            people_detector.Detect(image_holder, tracking_people);

            frame.copyTo(draw_mat);
            for (int p_index = 0; p_index < (int)tracking_people.size(); p_index++) {
                cv::rectangle(draw_mat, tracking_people[p_index].bounding_rect[1], cv::Scalar(0, 255, 0), 3);
                int cur_index = 0;
                for (int f_index = 0; f_index < (int)tracking_people[p_index].lk_status.size(); f_index++) {
                    if (!tracking_people[p_index].lk_status[f_index] || tracking_people[p_index].track_points[0].empty()) {
                        continue;
                    }
                    cv::line(draw_mat, tracking_people[p_index].track_points[0][f_index], tracking_people[p_index].track_points[1][cur_index], cv::Scalar(0, 0, 255), 2);
                    cur_index++;
                }
                tracking_people[p_index].OverwriteLog();
            }

            cv::imshow("debug", draw_mat);

//            std::cout << (cv::getTickCount() - start_time) / cv::getTickFrequency() * 1000 << std::endl;

            if (0 < cvWaitKey(1)) {
                break;
            }
        }
    }
};

int main()
{
    App app;
    app.Run();

    return 0;
}

