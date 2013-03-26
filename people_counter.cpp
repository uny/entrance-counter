#include "people_counter.h"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>

PeopleCounter::PeopleCounter()
{
}

void PeopleCounter::Count(const std::vector<TrackingPerson> &tracking_people)
{
    // for debug draw
    int p_index = 0;

    int centroid_size;
    char intersect;
    for (const TrackingPerson &tracking_person : tracking_people) {
        centroid_size = tracking_person.centroid.size();
        if (1 < centroid_size) {
            intersect = Intersect(tracking_person.centroid[centroid_size - 1], tracking_person.centroid[centroid_size - 2]);
            switch (intersect) {
            case PC_INTERSECT_IN:
                std::cout << "in" << std::endl;
                // for debug draw
                entered_.push_back(p_index);
                break;
            case PC_INTERSECT_OUT:
                std::cout << "out" << std::endl;
                // for debug draw
                exited_.push_back(p_index);
                break;
            default:
                break;
            }
        }
        // for debug draw
        p_index++;
    }
}

void PeopleCounter::DrawForDebug(cv::Mat &drawn, const std::vector<TrackingPerson> &tracking_people)
{
    static int in_cnt = 0;
    static int out_cnt = 0;

    static bool draw_flag = false;

    cv::line(drawn, FROM_POINT, TO_POINT, cv::Scalar(0, 0, 255), 3);

    for (int in_index : entered_) {
        cv::putText(drawn, "IN", tracking_people[in_index].bounding_rect[TP_TRANSITION_NEXT].tl(), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 0, 255), 2);
        cv::rectangle(drawn, tracking_people[in_index].bounding_rect[TP_TRANSITION_NEXT], cv::Scalar(0, 255, 0), 2);
        in_cnt++;
    }
    for (int out_index : exited_) {
        cv::putText(drawn, "OUT", tracking_people[out_index].bounding_rect[TP_TRANSITION_NEXT].tl(), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 0, 255), 2);
        cv::rectangle(drawn, tracking_people[out_index].bounding_rect[TP_TRANSITION_NEXT], cv::Scalar(0, 255, 0), 2);
        out_cnt++;
    }

    cv::rectangle(drawn, cv::Rect(0, 0, 320, 50), cv::Scalar(255, 255, 255), -1);
    cv::putText(drawn, cv::format("IN: %d, OUT: %d", in_cnt, out_cnt), cv::Point(0, 40), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 0, 0), 2);

    if (draw_flag) {
        draw_flag = false;
//        if (cv::waitKey(250)) {
//            return;
//        }
    }

    if (entered_.empty() && exited_.empty()) {
        return;
    }
    else {
        draw_flag = true;
    }

    entered_.clear();
    exited_.clear();
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
