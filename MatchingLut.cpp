/** Implementation file for image enhancement using histogram matching
 *
 *  \file ipcv/histogram_enhancement/MatchingLut.cpp
 *  \author <your name> (<your e-mail address>)
 *  \date DD MMM YYYY
 */

#include "MatchingLut.h"

#include <iostream>

#include "imgs/ipcv/utils/Utils.h"

using namespace std;

namespace ipcv {

/** Create a 3-channel (color) LUT using histogram matching
 *
 *  \param[in] src   source cv::Mat of CV_8UC3
 *  \param[in] h     the histogram in cv:Mat(3, 256) that the
 *                   source is to be matched to
 *  \param[out] lut  3-channel look up table in cv::Mat(3, 256)
 */
bool MatchingLut(const cv::Mat& src, const cv::Mat& h, cv::Mat& lut) {
  cv::Mat_<int> src_h;
  ipcv::Histogram(src, src_h);

  cv::Mat_<double> src_cdf;
  ipcv::HistogramToCdf(src_h, src_cdf);

  cv::Mat_<double> tgt_cdf;
  ipcv::HistogramToCdf(h, tgt_cdf);

  lut.create(3, 256, CV_8UC1);

  //cout<< src_cdf << endl;
  //cout<< "chungus" << endl;
  //cout<< tgt_cdf << endl;

  for (int r =  0; r < src_cdf.rows; r++){

	  //loop through channels

	for (int c = 0; c < src_cdf.cols; c++){

		// loop through source CDFs

			double src_CDF = src_cdf.at<double>(r,c);
	  		double output;	
	  		double lowdifference = 1;

                		for (int f = 0; f < tgt_cdf.cols; f++){

					// loop through the target CDFs
		  			
					auto difference = abs(src_CDF-tgt_cdf.at<double>(r,f));
					if(difference > lowdifference){
					}

					// find the closest value of src to tgt 

					else if (difference < lowdifference){
						output = f;
						lowdifference = difference;

					}
				}
				// fill in lut
			 lut.at<uint8_t>(r,c) = static_cast<uint8_t>(output);
		
	  }
  }


  return true;
}
}
