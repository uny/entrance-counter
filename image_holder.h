#ifndef IMAGE_HOLDER_H
#define IMAGE_HOLDER_H

#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>

class ImageHolder
{
public:
    cv::Mat color;
    cv::Mat gray;
    cv::Mat diff;

    ImageHolder();
    void Init();
    void Update(const cv::Mat &frame);

private:
    cv::Ptr<cv::BackgroundSubtractorGMG> fgbg_;

    const int MINIMUM_AREA = 300;
    const int GMG_INIT_FRAME_NUM = 20;
    const double GMG_THRESHOLD = 0.7;
};

#endif // IMAGE_HOLDER_H
