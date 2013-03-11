#ifndef PEOPLE_TRACKER_H
#define PEOPLE_TRACKER_H

#include <opencv2/core/core.hpp>

#include "image_holder.h"
#include "tracking_person.h"

class PeopleTracker
{
public:
    PeopleTracker();
    void Track(const ImageHolder &image_holder, std::vector<TrackingPerson> &tracking_people);

private:
    cv::Mat prev_frame_;

    const int MAX_FEATURE_NUM = 500;

    const int MISS_THRESHOLD = 10;
    const int REPLOT_THRESHOLD = 100;

    const int FEATURE_MAXIMUM_NUM = 500;
    const double FEATURE_QUALITY = 0.01;
    const int FEATURE_MINIMUM_DISTANCE = 1;

    const double TRACK_MINIMUM_NORM = 5.0;

    void JustifyNextFeaturesPoint(std::vector<cv::Point2f> features);
};

#endif // PEOPLE_TRACKER_H
