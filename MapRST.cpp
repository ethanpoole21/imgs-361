/** Implementation file for finding map coordinates for an RST transformation
 *
 *  \file ipcv/geometric_transformation/MapRST.cpp
 *  \author ethan Poole Erp1787
 *  \date 11-6-2019
 */

#include "MapRST.h"

#include <iostream>

#include <math.h>

#include <eigen3/Eigen/Dense>

#define _USE_MATH_DEFINES

using namespace std;

namespace ipcv {

/** Find the map coordinates (map1, map2) for an RST transformation
 *
 *  \param[in] src           source cv::Mat of CV_8UC3
 *  \param[in] angle         rotation angle (CCW) [radians]
 *  \param[in] scale_x       horizontal scale
 *  \param[in] scale_y       vertical scale
 *  \param[in] translation_x horizontal translation [+ to the right]
 *  \param[in] translation_y vertical translation [+ iup]
 *  \param[out] map1         cv::Mat of CV_32FC1 (size of the destination map)
 *                           containing the horizontal (x) coordinates at
 *                           which to resample the source data
 *  \param[out] map2         cv::Mat of CV_32FC1 (size of the destination map)
 *                           containing the vertical (y) coordinates at
 *                           which to resample the source data
 */
bool MapRST(const cv::Mat src, const double angle, const double scale_x,
            const double scale_y, const double translation_x,
            const double translation_y, cv::Mat& map1, cv::Mat& map2) {

// find and convert angle
  
  double Angle;
  double pi = M_PI;
	
  if (angle < 0){
	  Angle = angle + (2 * pi);
  }
  else{
	  Angle = angle;
  }

  while(Angle > ((90*pi)/180)){
	  Angle -= ((90 *pi)/180);
  }
  
  Eigen::MatrixXd transform(3, 3);

  double length = ((src.cols * sin(Angle)) + (src.rows * cos(Angle))) * scale_x;
  double height = ((src.cols * cos(Angle)) + (src.rows * sin(Angle))) * scale_y;

  // create and gill transformation matrix

  map1.create(length, height, CV_32FC1);
  map2.create(length, height, CV_32FC1);
 
  transform << 1, 0,translation_y, 
	       0, 1,translation_x, 
	       0, 0, 1;

  Eigen::MatrixXd scale(3, 3);

  scale << scale_y, 0, 0, 
	   0, scale_x, 0, 
	   0, 0, 1;

  Eigen::MatrixXd rotate(3, 3);

// Aidan method
//  rotate << (cos(-(pi/2-Angle)+pi)), (sin(-(pi/2-Angle)-pi)), 0,
//	    (-sin(-(pi/2-Angle)+pi)),(cos(-(pi/2-Angle)+pi)), 0,
//	    0, 0, 1;

  rotate << cos(Angle), sin(Angle), 0,
	    -sin(Angle),cos(Angle), 0,
	    0,0,1;

  Eigen::MatrixXd inverse_transformation_matrix(3, 3);
  Eigen::MatrixXd transformation_matrix(3, 3);

  inverse_transformation_matrix = rotate * scale * transform.transpose();
  // create transformation matrix
  transformation_matrix =  inverse_transformation_matrix.inverse();

  cout << transformation_matrix << endl;

  Eigen::MatrixXd src_coordinates(1, 3);
  Eigen::MatrixXd map_coordinates(1, 3);

//fill in maps with transformaed points
  
  for (double x = 0; x < map1.rows; x++) {
    for (double y = 0; y < map1.cols; y++) {
      map_coordinates(0, 0) = x - (height / 2);
      map_coordinates(0, 1) = y - (length / 2);
      map_coordinates(0, 2) = 1;

      //cout<< map_coordinates<< endl;	

      src_coordinates = map_coordinates * transformation_matrix;
	// fill in map with transormed coordinates
      map1.at<float>(x, y) = (src_coordinates(0, 1) + (src.rows / 2)) / src_coordinates(0, 2);
      map2.at<float>(x, y) = ((src.cols / 2) + src_coordinates(0, 0)) / (src_coordinates(0, 2));
    }
  }

  return true;
}
}
