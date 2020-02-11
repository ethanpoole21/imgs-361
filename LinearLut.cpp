/** Implementation file for image enhancement using linear histogram statistics
 *
 *  \file ipcv/histogram_enhancement/LinearLut.cpp
 *  \author ethan poole erp1787
 *  \date DD MMM YYYY
 */

#include "LinearLut.h"
#include <algorithm>    // std::reverse
#include <vector>  
#include <iostream>

#include "imgs/ipcv/utils/Utils.h"

using namespace std;

namespace ipcv {

/** Create a 3-channel (color) LUT using linear histogram enhancement
 *
 *  \param[in] src          source cv::Mat of CV_8UC3
 *  \param[in] percentage   the total percentage to remove from the tails
 *                          of the histogram to find the extremes of the
 *                          linear enhancemnt function
 *  \param[out] lut         3-channel look up table in cv::Mat(3, 256)
 */
bool LinearLut(const cv::Mat& src, const int percentage, cv::Mat& lut) {
  cv::Mat_<int> h;
  ipcv::Histogram(src, h);

  cv::Mat_<double> cdf;
  ipcv::HistogramToCdf(h, cdf);

  lut.create(3, 256, CV_8UC1);

  double crush = (percentage/2.0)/100;
  double clip = (1-((percentage/2.0)/100));
  
  // find clipping and crushing values

  int x1;
  int x2;
  int upperlim;
  int lowerlim;
  //double val;



   for (int r =  0; r < cdf.rows; r++){
                
	   	int x1;
  		int x2;
		int upperlim;
  		int lowerlim;
		double val;

	   	// find crush and clip values
		  
	   	for (int c = 0; c < cdf.cols; c++){
			 
			double val = cdf.at<double>(r,c);
			 		
			 	if (val <= crush){
					x1 = c;
				}
				if (val <= clip){
                                	x2 = c;
				}
				else if (val >= clip && val <= crush){
				}
		}

   
	  // find linear equation
	  
	  double y1 = 0.0;
	  double y2 = 255.0;
	  double m = (y2-y1)/(double( x2-x1));

	  double b = -(m * x1);
	 
	  // fill in lut with linear values

   		for (int x = 0; x < lut.cols; x++){
			
			double y  = (m * x +b);

				if(y < 0.0){
                                        y = 0;
					lut.at<uint8_t>(r,x) = static_cast<uint8_t>(y);
                                }
                                else if(y > 255.0){
                               		y = 255;
					lut.at<uint8_t>(r,x) = static_cast<uint8_t>(y);
				}
				else{
                                       lut.at<uint8_t>(r,x) = static_cast<uint8_t>(y);
                                }
			

                }   
		
  }

  return true;
}
}
