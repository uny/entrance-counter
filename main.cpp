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
        cv::Mat draw_mat;

        // i do not know why this is here ref. bgfg_gmg.cpp
        cv::initModule_video();

        ImageHolder image_holder;
        image_holder.Init();

        std::vector<TrackingPerson> tracking_people;

        PeopleDetector people_detector;
        people_detector.Init();

        PeopleTracker people_tracker;

        while (true) {
            capture >> frame;
            if (frame.empty()) {
                break;
            }

            image_holder.Update(frame);

            people_tracker.Track(image_holder, tracking_people);
            people_detector.Detect(image_holder, tracking_people);

            frame.copyTo(draw_mat);
            for (TrackingPerson tracking_person : tracking_people) {
                cv::rectangle(draw_mat, tracking_person.bounding_rect[1], cv::Scalar(0, 255, 0), 3);
                for (int i = 0; i < (int)tracking_person.lk_status.size(); i++) {
//                    if (!tracking_person.lk_status[i]) {
//                        std::cout << "error" << std::endl;
//                        continue;
//                    }
//                    std::cout << tracking_person.track_points[1][i] << std::endl;
                    cv::circle(draw_mat, tracking_person.track_points[1][i], 1, cv::Scalar(255, 0, 0), 3);
                }
                tracking_person.OverwriteLog();
            }

            cv::imshow("debug", draw_mat);

            if (0 < cvWaitKey(30)) {
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

