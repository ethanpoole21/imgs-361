/** Implementation file for bilateral filtering
 *
 *  \file ipcv/bilateral_filtering/BilateralFilter.cpp
 *  \author ethan poole erp1787@rit.edu
 *  \date 11/7/2019
 */

#include "BilateralFilter.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cmath>

using namespace std;

namespace ipcv {

/** Bilateral filter an image
 *
 *  \param[in] src             source cv::Mat of CV_8UC3
 *  \param[out] dst            destination cv::Mat of ddepth type
 *  \param[in] sigma_distance  standard deviation of distance/closeness filter
 *  \param[in] sigma_range     standard deviation of range/similarity filter
 *  \param[in] radius          radius of the bilateral filter (if negative, use
 *                             twice the standard deviation of the distance/
 *                             closeness filter)
 *  \param[in] border_mode     pixel extrapolation method
 *  \param[in] border_value    value to use for constant border mode
 */

// srart code
bool BilateralFilter(const cv::Mat& src, cv::Mat& dst,
                     const double sigma_distance, const double sigma_range,
                     const int radius, const BorderMode border_mode,
                     uint8_t border_value) {
  // check if image is greyscale
  bool greyscale = false;
  for (int r = 0; r < src.rows; r++) {
    for (int c = 0; c < src.cols; c++) {
      int blue = cv::Scalar(src.at<cv::Vec3b>(r, c))[0];
      int green = cv::Scalar(src.at<cv::Vec3b>(r, c))[1];
      int red = cv::Scalar(src.at<cv::Vec3b>(r, c))[2];

      if (blue != green || green != red || red != blue) {
        greyscale = false;
        goto cut;
      } else {
        greyscale = true;
      }
    }
  }


cut:

cout << greyscale << endl;
// make destiation
  dst.create(src.size(), src.type());

  // convert source
  if (greyscale == false) {
    cv::cvtColor(src, src, cv::COLOR_BGR2Lab);
	cout << "lala" << endl;
  }

  // check radius
  double rad = radius;
  double sig_dist = (sigma_distance * 2);

  if (radius < 0) {
    rad = sig_dist;
  } else {
    rad = radius;
  }

  // make kernel
  int kernel_dim = (rad * 2) + 1;
  cv::Mat kernel;
  kernel.create(kernel_dim, kernel_dim, CV_32FC1);
  int size = ((kernel_dim / 2));

  // fill kernel with closeness values
  for (int a = 0; a < kernel.rows; a++) {
    for (int b = 0; b < kernel.cols; b++) {
      double dist = sqrt(pow((a - rad), 2) + pow((b - rad), 2));
      double closeness_factor = exp((-0.5) * pow((dist / sigma_distance), 2));
      kernel.at<float>(a, b) = closeness_factor;
    }
  }
  // border modes using open cv functoin
  cv::Mat border;
  border.create(src.rows + (2 * rad), src.cols + (2 * rad), CV_32FC1);
      if (border_mode == ipcv::BorderMode::REPLICATE) {
          cv::copyMakeBorder(src, border, rad, rad, rad, rad,
                             cv::BORDER_REPLICATE, border_value);
        } else if (border_mode == ipcv::BorderMode::CONSTANT) {
          cv::copyMakeBorder(src, border, rad, rad, rad, rad,
                             cv::BORDER_CONSTANT, border_value);
        cout << "q" << endl;
	}

  // loop through image and kernel
  for (int r = 0; r < src.rows; r++) {
    for (int c = 0; c < src.cols; c++) {
          double normalizer = 0;
          double sum = 0;
          double dim = (kernel_dim / 2);

          for (int rk = 0; rk < kernel_dim; rk++) {
            for (int ck = 0; ck < kernel_dim; ck++) {
                double factor =  pow(abs(src.at<cv::Vec3b>(r + rk - dim, c + ck - dim)[0] -
                              src.at<cv::Vec3b>(r, c)[0]) /
                              sigma_range,2);
		double similarity_factor =
                  exp((-0.5) * factor);

              sum += similarity_factor * kernel.at<float>(rk, ck) *
                     src.at<cv::Vec3b>(r + rk - dim, c + ck - dim)[0];

              normalizer += similarity_factor * kernel.at<float>(rk, ck);
            }
          }
          int output = sum / normalizer;
          if (greyscale == true) {
		for (int chan = 0 ; chan <= 2 ; chan ++){
                	dst.at<cv::Vec3b>(r, c)[chan] = output;
		}
          } else if (greyscale == false)  {
            dst.at<cv::Vec3b>(r, c)[0] = output;
            dst.at<cv::Vec3b>(r, c)[1] = cv::Scalar(src.at<cv::Vec3b>(r, c))[1];
            dst.at<cv::Vec3b>(r, c)[2] = cv::Scalar(src.at<cv::Vec3b>(r, c))[2];
	  }
      }
    }
  
  if (greyscale == false) {
    cv::cvtColor(dst, dst, cv::COLOR_Lab2BGR);
    cv::cvtColor(src, src, cv::COLOR_Lab2BGR);
  }

  return true;
}
}
