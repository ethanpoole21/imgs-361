/** Implementation file for finding corner features using Harris
 *
 *  \file ipcv/corners/Harris.cpp
 *  \Ethan Poole erp1787
 *  \date 11/19/2019
 */

#include "Corners.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include <eigen3/Eigen/Dense>

using namespace std;

namespace ipcv {

/** Apply the Harris corner detector to a color image
 *
 *  \param[in] src     source cv::Mat of CV_8UC3
 *  \param[out] dst    destination cv:Mat of CV_32FC1
 *  \param[in] sigma   standard deviation of the Gaussian blur kernel
 *  \param[in] k       free parameter in the equation
 *                        dst = (lambda1)(lambda2) - k(lambda1 + lambda2)^2
 */

bool Harris(const cv::Mat& src, cv::Mat& dst, const float sigma,
            const float k) {
  cv::Mat src_gray;
  cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
  //src_gray.convertTo(src_gray, CV_32FC1);

  dst.create(src_gray.size(), CV_32FC1);

  // create matricies for delta x and delta y
  cv::Mat kern1(1, 3, CV_32FC1);
  cv::Mat kern2(3, 1, CV_32FC1);

  kern1.at<float>(0, 0) = -1;
  kern1.at<float>(0, 1) = 0;
  kern1.at<float>(0, 2) = 1;

  kern2.at<float>(0, 0) = -1;
  kern2.at<float>(1, 0) = 0;
  kern2.at<float>(2, 0) = 1;

  // cout << kern1 << endl;
  // cout << kern2 << endl;

  // filter source
  cv::Point q;
  q.x = -1;
  q.y = -1;
  // find x  partial derivatives
  cv::Mat x1(src_gray.rows, src_gray.cols, CV_32FC1);
  cv::filter2D(src_gray, x1, -1, kern1, q, 0);
  x1.convertTo(x1, CV_32FC1);
  cv::Mat x2(src_gray.rows, src_gray.cols, CV_32FC1);
  cv::filter2D(src_gray, x2, -1, (kern1 * -1), q, 0);
  x2.convertTo(x2, CV_32FC1);
  cv::Mat didx;
  cv::filter2D(src_gray, didx, CV_32F, kern1);
  didx/= 140;

  // find y  partial derivatives
  cv::Mat y1(src_gray.rows, src_gray.cols, CV_32FC1);
  cv::filter2D(src_gray, y1, -1, kern2, q, 0);
  y1.convertTo(y1, CV_32FC1);
  cv::Mat y2(src_gray.rows, src_gray.cols, CV_32FC1);
  cv::filter2D(src_gray, y2, -1, (kern2 * -1), q, 0);
  y2.convertTo(y2, CV_32FC1);
  cv::Mat didy;
  cv::filter2D(src_gray, didy, CV_32F, kern2);
  didy/= 140;

  // find product of both derivatives
  cv::Mat x(src_gray.rows, src_gray.cols, CV_32FC1);
  cv::Mat y(src_gray.rows, src_gray.cols, CV_32FC1);
  cv::Mat xy(src_gray.rows, src_gray.cols, CV_32FC1);
  cv::Mat didxdy(src_gray.rows, src_gray.cols, CV_32FC1);

  for (int r = 0; r < didy.rows; r++) {
    for (int c = 0; c < didy.cols; c++) {
      didxdy.at<float>(r, c) = (didx.at<float>(r, c)) * (didx.at<float>(r, c));
      didx.at<float>(r, c) *= didx.at<float>(r, c);
      didx.at<float>(r, c) *= didy.at<float>(r, c);
    }
  }
  // compute values
  x = x1 + x2;
  y = y1 + y2;
  multiply(x, y, xy);
  // cout << xy << endl;

  cv::Mat X;
  cv::Mat Y;
  cv::Mat XY;

  multiply(x, x, X);
  multiply(y, y, Y);
  multiply(x, y, XY);

  // make kernel and fill

  cv::Mat kernel(2 * sigma + 1, 2 * sigma + 1, CV_32FC1);

  for (int r = 0; r < kernel.rows; r++) {
    for (int c = 0; c < kernel.cols; c++) {
      double dist = sqrt(pow((r - sigma), 2) + ((c - sigma), 2));
      kernel.at<float>(r, c) = exp((-0.5) * pow((dist / sigma), 2));
    }
  }
  // FILL MATRICIES

  cv::Mat a, b, C;

  // a.create(x.size(), CV_32FC1);
  // b.create(x.size(), CV_32FC1);
  // C.create(x.size(), CV_32FC1);

  cv::filter2D(didx, a, -1, kernel);
  cv::filter2D(didy, b, -1, kernel);
  cv::filter2D(didxdy, C, -1, kernel);

  cv::Mat t, d;
  t.create(a.size(), a.depth());
  d.create(a.size(), a.depth());

  // epic fail
  for (int r = 0; r < a.rows; r++) {
    for (int c = 0; c < a.cols; c++) {
      t.at<float>(r, c) = a.at<float>(r, c) + b.at<float>(r, c);
      d.at<float>(r, c) = (a.at<float>(r, c) * b.at<float>(r, c)) -
                          (C.at<float>(r, c) * C.at<float>(r, c));
      float response;
      response =
          d.at<float>(r, c) - (k * t.at<float>(r, c) * t.at<float>(r, c));

      if (response > 0) {
        dst.at<float>(r, c) = response;
      } else {
        dst.at<float>(r, c) = 0;
      }
    }
  }

  return true;
}
}
