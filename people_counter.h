#ifndef PEOPLE_COUNTER_H
#define PEOPLE_COUNTER_H

#include "tracking_person.h"

typedef enum {
    PC_DEBUG_COUNT,
} PC_DEBUG_ENUM;

class PeopleCounter
{
public:
    PeopleCounter();
    void Count(const std::vector<TrackingPerson> &tracking_people);
    void DrawForDebug(cv::Mat &drawn, PC_DEBUG_ENUM type);
};

#endif // PEOPLE_COUNTER_H
