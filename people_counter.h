#ifndef PEOPLE_COUNTER_H
#define PEOPLE_COUNTER_H

#include "tracking_person.h"

class PeopleCounter
{
public:
    PeopleCounter();
    void Count(const std::vector<TrackingPerson> &tracking_people);
    void DrawForDebug(cv::Mat &drawn);

private:
    const cv::Point FROM_POINT = cv::Point(400, 340);
    const cv::Point TO_POINT = cv::Point(690, 200);

    typedef enum {
        PC_INTERSECT_NONE,
        PC_INTERSECT_IN,
        PC_INTERSECT_OUT
    } PC_INTERSECT_ENUM;

    char Intersect(cv::Point prev_point, cv::Point next_point);
    /**
     * @brief Intersect
     * @param b1 a point of base line
     * @param b2 a point of base line
     * @param p1 target point
     * @param p2 target point
     * @return 0: no 1,2: yes
     */
    char Intersect(cv::Point b1, cv::Point b2, cv::Point p1, cv::Point p2);
    bool CheckWhichSide(cv::Point b1, cv::Point b2, cv::Point point);
};

#endif // PEOPLE_COUNTER_H
