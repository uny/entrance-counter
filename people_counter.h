#ifndef PEOPLE_COUNTER_H
#define PEOPLE_COUNTER_H

#include "tracking_person.h"


class PeopleCounter
{
public:
    PeopleCounter();
    void Count(const std::vector<TrackingPerson> &tracking_people);
};

#endif // PEOPLE_COUNTER_H
