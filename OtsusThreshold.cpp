/** Implementation file for finding Otsu's threshold
 *
 *  \file ipcv/otsus_threshold/OtsusThreshold.cpp
 *  \author ethan poole erp1787)
 *  \date 9/19/2019
 */

#include "OtsusThreshold.h"
#include <math.h>
#include <iostream>

#include "imgs/ipcv/utils/Utils.h"

using namespace std;

namespace ipcv {

/** Find Otsu's threshold for each channel of a 3-channel (color) image
 *
 *  \param[in] src          source cv::Mat of CV_8UC3
 *  \param[out] threshold   threshold values for each channel of a 3-channel
 *                          color image in cv::Vec3b
 */
bool OtsusThreshold(const cv::Mat& src, cv::Vec3b& threshold) {
  threshold = cv::Vec3b();

  // create cdf and pdf from histograms
  cv::Mat_<int> src_h;
  ipcv::Histogram(src, src_h);

  cv::Mat_<double> cdf;
  ipcv::HistogramToCdf(src_h, cdf);

  cv::Mat_<double> pdf;
  ipcv::HistogramToPdf(src_h, pdf);

  int DCmax = 255;

  for (int p = 0; p < src.channels(); p++) {
    // cout << src.channels() << endl;
    double maxsigmab = 0;

    for (int K = 1; K < DCmax; K++) {
      // initialize sigmab variables outside summation loops
      double mewk = 0, omegak = 0, mewt = 0;

      // cout<< mewk << endl;

      // calculate mew(k) and omega(k) values for each channel count summation

      for (int c = 0; c <= K; c++) {
        mewk += (c * pdf.at<double>(p, c));
      }

      omegak += cdf.at<double>(p, K);

      for (int a = 0; a <= DCmax; a++) {
        mewt += (a * pdf.at<double>(p, a));
      }
      // calculate seperability for each summation

      double sigmab = 0;
      sigmab = pow((mewt * omegak - mewk), 2) / (omegak * (1 - omegak));
      // cout << sigmab << endl;

      // find optimal threshold
      if (sigmab > maxsigmab) {
        maxsigmab = sigmab;
        threshold[p] = K;
      }
    }
  }
  return true;
}
}
// used the clang command to format code
