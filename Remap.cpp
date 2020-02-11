/** Implementation file for remapping source values to map locations
 *
 *  \file ipcv/geometric_transformation/Remap.cpp
 *  \author Ethan Poole erp1787
 *  \date 11-5-2019
 */

#include "Remap.h"

#include <iostream>

using namespace std;

namespace ipcv {

/** Remap source values to the destination array at map1, map2 locations
 *
 *  \param[in] src            source cv::Mat of CV_8UC3
 *  \param[out] dst           destination cv::Mat of CV_8UC3 for remapped values
 *  \param[in] map1           cv::Mat of CV_32FC1 (size of the destination map)
 *                            containing the horizontal (x) coordinates at
 *                            which to resample the source data
 *  \param[in] map2           cv::Mat of CV_32FC1 (size of the destination map)
 *                            containing the vertical (y) coordinates at
 *                            which to resample the source data
 *  \param[in] interpolation  interpolation to be used for resampling
 *  \param[in] border_mode    border mode to be used for out of bounds pixels
 *  \param[in] border_value   border value to be used when constant border mode
 *                            is to be used
 */
bool Remap(const cv::Mat& src, cv::Mat& dst, const cv::Mat& map1,
           const cv::Mat& map2, const Interpolation interpolation,
           const BorderMode border_mode, const uint8_t border_value) {
  dst.create(map1.size(), src.type());

	// BILINEAR
  
  if(interpolation == ipcv::Interpolation::LINEAR){
	  for (int r = 0; r <map1.rows; r++){
		  for (int c = 0; c <map1.cols; c++){
		
		  	int x_value = static_cast<int>(map1.at<float>(r,c));
		  
		  	int y_value = static_cast<int>(map2.at<float>(r,c));
		  
			// selecting border value 
			// Replicate draws out the edge, constant uses the user border value as the border
		

			if(x_value>=src.cols || x_value<= 0 || y_value>= src.rows || y_value <= 0) {
		  		if(border_mode == ipcv::BorderMode::REPLICATE){
				
					if (x_value>= src.cols){
						x_value = src.cols-1;
					}
					if (x_value< 0){
						x_value = 0;
					}
					if (y_value>= src.rows){
                                	        y_value = src.cols-1;
                        	    	}
					if (y_value< 0){
                                        	y_value = 0;
                                	}

					dst.at<cv::Vec3b>(r,c) = src.at<cv::Vec3b>(y_value,x_value);
			
				}

				if(border_mode == ipcv::BorderMode::CONSTANT){
			
					cv::Vec3b border = {border_value,border_value,border_value};
					dst.at<cv::Vec3b>(r,c) = border;
				}
			}

			else{
					// apply bilinear to the image
					// this part doesnt work
				
					cv::Vec3b A; 
						
					cv::Vec3b B;

					A = (src.at<cv::Vec3b>(y_value+1,x_value)-src.at<cv::Vec3b>(y_value,x_value))* (map2.at<float>(r,c)-y_value)+(src.at<cv::Vec3b>(y_value,x_value));
					
					B = (src.at<cv::Vec3b>(y_value+1,x_value+1)-src.at<cv::Vec3b>(y_value,x_value+1))*(map2.at<float>(r,c)-y_value)+(src.at<cv::Vec3b>(y_value,x_value));
					
					cv::Vec3b C = (A-B)*(map1.at<float>(r,c)-y_value)+A;
				
					dst.at<cv::Vec3b>(r,c) = C;
				
				cout<< C << endl;
				
			}
				
		}
	}
  }
	//NEAREST
	
  if(interpolation == ipcv::Interpolation::NEAREST){
	 for (int r = 0; r <map1.rows; r++){
                  for (int c = 0; c <map1.cols; c++){

			 // use reounding trick

			int x_value = static_cast<int>(map1.at<float>(r,c)+0.5);
                        
                        int y_value = static_cast<int>(map2.at<float>(r,c)+0.5);

			
			if (x_value >= src.cols|| x_value <0 || y_value >= src.rows || y_value <0){


				// same border modes as bilinear


				if(border_mode == ipcv::BorderMode::CONSTANT){
					cv::Vec3b border = {border_value,border_value,border_value};
                                	dst.at<cv::Vec3b>(r,c) = border;
                        	
				}

					//replicate replicates each edge pixel value out to border value horizontally and vertically

				if(border_mode == ipcv::BorderMode::REPLICATE){
					if (x_value>= src.cols){
                                        	x_value = src.cols-1;
                                	}
                                	if (x_value< 0){
                                        	x_value = 0;
                                	}
                                	if (y_value>= src.rows){
                                        	y_value = src.cols-1;
                                	}
                                	if (y_value< 0){
                                        	y_value = 0;
                                	}

                                	dst.at<cv::Vec3b>(r,c) = src.at<cv::Vec3b>(y_value,x_value);
				}
			}

			else{
				// apply nearest neighbor algorithym 
				 
				dst.at<cv::Vec3b>(r,c) = src.at<cv::Vec3b>(y_value,x_value);	

		  			
	  		}	
	 	}
  	}
  }

  return true;
}
}
