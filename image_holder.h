#ifndef IMAGE_HOLDER_H
#define IMAGE_HOLDER_H

#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>

class ImageHolder
{
public:
    cv::Mat color;
    cv::Mat gray;
    std::vector<cv::Rect> diff_rects;

    ImageHolder();
    void Init();
    void Update(const cv::Mat &frame);

private:
    cv::Ptr<cv::BackgroundSubtractorGMG> fgbg_;

    static const int MINIMUM_AREA = 300;
    static const int GMG_INIT_FRAME_NUM = 20;
    static const double GMG_THRESHOLD = 0.7;
};

#endif // IMAGE_HOLDER_H
