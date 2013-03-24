#include "people_counter.h"

#include <iostream>

PeopleCounter::PeopleCounter()
{
}

void PeopleCounter::Count(const std::vector<TrackingPerson> &tracking_people)
{
    int centroid_size;
    char intersect;
    for (const TrackingPerson &tracking_person : tracking_people) {
        centroid_size = tracking_person.centroid.size();
        if (1 < centroid_size) {
            intersect = Intersect(tracking_person.centroid[centroid_size - 1], tracking_person.centroid[centroid_size - 2]);
            switch (intersect) {
            case PC_INTERSECT_IN:
                std::cout << "in" << std::endl;
                break;
            case PC_INTERSECT_OUT:
                std::cout << "out" << std::endl;
                break;
            default:
                break;
            }
        }
    }
}

void PeopleCounter::DrawForDebug(cv::Mat &drawn)
{
    cv::line(drawn, FROM_POINT, TO_POINT, cv::Scalar(0, 0, 255), 3);
}

char PeopleCounter::Intersect(cv::Point prev_point, cv::Point next_point)
{
    char first = Intersect(FROM_POINT, TO_POINT, prev_point, next_point);
    char second = Intersect(prev_point, next_point, FROM_POINT, TO_POINT);
    if (first && second) {
        return first;
    }
    return 0;
}

char PeopleCounter::Intersect(cv::Point b1, cv::Point b2, cv::Point p1, cv::Point p2)
{
    bool first = CheckWhichSide(b1, b2, p1);
    bool second = CheckWhichSide(b1, b2, p2);

    if (first == second) {
        return 0;
    }
    return first ? 1 : 2;
}

bool PeopleCounter::CheckWhichSide(cv::Point b1, cv::Point b2, cv::Point point)
{
    if (0 < (b2.y - b1.y) * (point.x - b1.x) - (b2.x - b1.x) * (point.y - b1.y)) {
        return true;
    }
    return false;
}
