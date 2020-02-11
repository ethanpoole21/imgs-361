/** Implementation file for image quantization
 *
 *  \file ipcv/quantize/quantize.cpp
 *  \author Ethan Poole erp1787@rit.edu
 *  \date 04 09 2019
 */

#include "Quantize.h"

#include <iostream>

using namespace std;

/** Perform uniform grey-level quantization on a color image
 *
 *  \param[in] src                 source cv::Mat of CV_8UC3
 *  \param[in] quantization_levels the number of levels to which to quantize
 *                                 the image
 *  \param[out] dst                destination cv:Mat of CV_8UC3
 */
void Uniform(const cv::Mat& src, const int quantization_levels, cv::Mat& dst) {
  // create divisot
  double divisor = 256 / quantization_levels;
  // runthrough each pixel in image
  for (int r =  0; r < src.rows; r++){
	for (int c = 0; c < src.cols; c++){
		// find the translation from pixel location to digital count (help from ta)
		const auto& src_digitalcount = src.at<cv::Vec3b>(r,c);
		auto& dst_digitalcount = dst.at<cv::Vec3b>(r,c);
		//apply quantization to each channel
		for(int p = 0; p <src.channels(); p++){
			dst_digitalcount[p] = (src_digitalcount[p]/divisor);
		}
  	}
  }
}

/** Perform improved grey scale quantization on a color image
 *
 *  \param[in] src                 source cv::Mat of CV_8UC3
 *  \param[in] quantization_levels the number of levels to which to quantize
 *                                 the image
 *  \param[out] dst                destination cv:Mat of CV_8UC3
 */
void Igs(const cv::Mat& src, const int quantization_levels, cv::Mat& dst) {
  // create divisor quotents and remainder
  int divisor =256/quantization_levels;
  int remainder = 0;

  // runthrough each pixel in image ** EDIT CHANNEL LOOP IS FIRST **
 for(int k = 0; k < src.channels(); k++){
 	 for (int r = 0; r < src.rows; r++){
        	for (int c = 0; c < src.cols; c++){
		// find the translation from pixel location to digital count (help from ta)
                	int digiCount = src.at<cv::Vec3b>(r,c)[k];
               
	       		//quantize each channel but also uniformly add noise to each pixel in the form of remainders from the last loop
	       		dst.at<cv::Vec3b>(r,c)[k]  = ((digiCount += remainder) /divisor);
			remainder = (digiCount%divisor);

                }	
	}
  }
}

namespace ipcv {

bool Quantize(const cv::Mat& src, const int quantization_levels,
              const QuantizationType quantization_type, cv::Mat& dst) {
  dst.create(src.size(), src.type());

  switch (quantization_type) {
    case QuantizationType::uniform:
      Uniform(src, quantization_levels, dst);
      break;
    case QuantizationType::igs:
      Igs(src, quantization_levels, dst);
      break;
    default:
      cerr << "Specified quantization type is unsupported" << endl;
      return false;
  }

  return true;
}
}
