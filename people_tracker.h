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

    static const int MAX_FEATURE_NUM = 500;

    // TODO: tune this parameter
    static const int MAXIMUM_MISS_COUNT = 0;

    static const int FEATURE_MAXIMUM_NUM = 500;
    static const int FEATURE_MINIMUM_DISTANCE = 1;
};

#endif // PEOPLE_TRACKER_H
