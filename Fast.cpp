/** Implementation file for finding corner features using FAST
 *
 *  \file ipcv/corners/Fast.cpp
 *  ethan poole erp1787@rit.edu
 *  11/20/19
 */

#include "Corners.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"

using namespace std;

namespace ipcv {

/** Apply the FAST corner detector to a color image
 *
 *  \param[in] src     source cv::Mat of CV_8UC3
 *  \param[out] dst    destination cv:Mat of CV_8UC3
 *  \param[in] difference_threshold
 *                     brightness threshold to be used to determine whether
 *                     a surrounding pixels is brighter than or darker than
 *                     the candidate corner pixel
 *  \param[in] contiguous_threshold
 *                     number of contiguous pixels that must appear in
 *                     sequence in order for a candidate pixel to be
 *                     considered a corner pixel
 *  \param[in] nonmaximal_suppression
 *                     boolean parameter indicating whether non-maximal
 *                     suppression should be used to eliminate "clumping"
 *                     of identified corner points
 */
bool Fast(const cv::Mat& src, cv::Mat& dst, const int difference_threshold,
          const int contiguous_threshold, const bool nonmaximal_supression) {
  cv::Mat src_gray;
  cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);

  dst.create(src.size(), CV_8UC1);
  dst = 0;

  bool is_corner;
  int sum;
  // hardcode points

  float points[16][2] = {{3, 0},  {3, 1},  {2, 2},  {1, 3},  {0, 3},   {-1, 3},
                       {-2, 2}, {-3, 1}, {-3, 0}, {-3, 1}, {-2, -2}, {-1, -3},
                       {0, -3}, {1, -3}, {2, -2}, {3, -1}};

  // loop through image
  for (int r = 3; r < src_gray.rows - 3; r++) {
    for (int c = 3; c < src_gray.cols - 3; c++) {
      is_corner = false;
      int count;
      int count_inrow;
      // loop through kernel
      count = 0;
      count_inrow = 0;
      int x;
      for (x = 0; x < 16; x++) {
	      //christian said this is how its supposed to look
	      //might have to switch threshold and center is what i think
        if (abs(src_gray.at<uint8_t>(r + points[x][0], c + points[x][1]) -
                difference_threshold) >  src_gray.at<uint8_t>(r, c)) {
          count_inrow++;

          if (count_inrow > count) {
            count = count_inrow;
          }
        } else {
          count_inrow = 0;
        }
      }
      cout << count << endl;

      // check contigularity
      if (count >= contiguous_threshold) {
        is_corner = true;
      } else {
        is_corner = false;
      }
      if (is_corner == true) {
        dst.at<uint8_t>(r, c) = 255;
      } else if (is_corner == false) {
        dst.at<uint8_t>(r, c) = 0;
      }
    }
  }

  return true;
}
}
