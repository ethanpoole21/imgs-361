/** Implementation file for image filtering
 *
 *  \file ipcv/spatial_filtering/Filter2D.cpp
 *  Ethan Poole erp1787
 *  \date 10-20-2019
 */

#include "Filter2D.h"

#include <iostream>

using namespace std;

namespace ipcv {

/** Correlates an image with the provided kernel
 *
 *  \param[in] src          source cv::Mat of CV_8UC3
 *  \param[out] dst         destination cv::Mat of ddepth type
 *  \param[in] ddepth       desired depth of the destination image
 *  \param[in] kernel       convolution kernel (or rather a correlation
 *                          kernel), a single-channel floating point matrix
 *  \param[in] anchor       anchor of the kernel that indicates the relative
 *                          position of a filtered point within the kernel;
 *                          the anchor should lie within the kernel; default
 *                          value (-1,-1) means that the anchor is at the
 *                          kernel center
 *  \param[in] delta        optional value added to the filtered pixels
 *  \param[in] border_value value to use for constant border mode
 */

bool Filter2D(const cv::Mat& src, cv::Mat& dst, const int ddepth,
              const cv::Mat& kernel, const cv::Point anchor, const int delta,
              const BorderMode border_mode, const uint8_t border_value) {
  dst.create(src.rows, src.cols, CV_8UC3);

  clock_t startTime = clock();
  // initialise sum
  double sum = 0;
  // loop thorough inmge and kernel
  for (int x = 0; x < 3; x++) {
 for (int r = 0; r < src.rows; r++) {
      for (int c = 0; c < src.cols - 1; c++) {
        // redet sum
        sum = 0;
        for (int kr = 0; kr < kernel.rows; kr++) {
          for (int kc = 0; kc < kernel.cols; kc++) {
            // set sum for each kernel
            sum +=
                kernel.at<float>(kr, kc) * src.at<cv::Vec3b>(r + kr, c + kc)[x];
          }
        }
        // check if sum is in bounds
        if (sum >= 255) {
          sum = 255;
        }
        if (sum < 0) {
          sum = 0;
        }
        dst.at<cv::Vec3b>(r + 1, c + 1)[x] = (sum + delta);
      }
    }
  }

  cout << "took " << double(clock() - startTime) / (double)CLOCKS_PER_SEC
       << " seconds" << endl;

  return true;
}
}
