/** Implementation file for mapping a source quad on to a target quad
 *
 *  \file ipcv/geometric_transformation/MapQ2Q.cpp
 *  \author Ethan poole erp1787
 *  \date 10-11-2019
 */

#include "MapQ2Q.h"

#include <iostream>

#include <eigen3/Eigen/Dense>
#include <opencv2/core.hpp>

using namespace std;

namespace ipcv {

/** Find the source coordinates (map1, map2) for a quad to quad mapping
 *
 *  \param[in] src       source cv::Mat of CV_8UC3
 *  \param[in] tgt       target cv::Mat of CV_8UC3
 *  \param[in] src_vertices
 *                       vertices cv:Point of the source quadrilateral (CW)
 *                       which is to be mapped to the target quadrilateral
 *  \param[in] tgt_vertices
 *                       vertices cv:Point of the target quadrilateral (CW)
 *                       into which the source quadrilateral is to be mapped
 *  \param[out] map1     cv::Mat of CV_32FC1 (size of the destination map)
 *                       containing the horizontal (x) coordinates at
 *                       which to resample the source data
 *  \param[out] map2     cv::Mat of CV_32FC1 (size of the destination map)
 *                       containing the vertical (y) coordinates at
 *                       which to resample the source data
 */
bool MapQ2Q(const cv::Mat src, const cv::Mat tgt,
            const vector<cv::Point> src_vertices,
            const vector<cv::Point> tgt_vertices, cv::Mat& map1,
            cv::Mat& map2) {

map1.create(tgt.size(),CV_32FC1);
map2.create(tgt.size(),CV_32FC1);

//  Quadrilateral-to-quadrilateral mapping

// create 1 by 8 matrix for reverse coordinate mapping	
 Eigen::MatrixXd Pmap_to_image (8,1);

 Eigen::MatrixXd inv_transformation_matrix (8,8);

 inv_transformation_matrix << tgt_vertices[0].x, tgt_vertices[0].y, 1, 0, 0, 0, -(tgt_vertices[0].x*src_vertices[0].x) , -(tgt_vertices[0].y*src_vertices[0].x),
 			      tgt_vertices[1].x, tgt_vertices[1].y, 1, 0, 0, 0, -(tgt_vertices[1].x*src_vertices[1].x) , -(tgt_vertices[1].y*src_vertices[1].x),
			      tgt_vertices[2].x, tgt_vertices[2].y, 1, 0, 0, 0, -(tgt_vertices[2].x*src_vertices[2].x) , -(tgt_vertices[2].y*src_vertices[2].x),
                              tgt_vertices[3].x, tgt_vertices[3].y, 1, 0, 0, 0, -(tgt_vertices[3].x*src_vertices[3].x) , -(tgt_vertices[3].y*src_vertices[3].x),
			      0,0,0,tgt_vertices[0].x, tgt_vertices[0].y,1,-(tgt_vertices[0].x*src_vertices[0].y), -(tgt_vertices[0].y*src_vertices[0].y),
			      0,0,0,tgt_vertices[1].x, tgt_vertices[1].y,1,-(tgt_vertices[1].x*src_vertices[1].y), -(tgt_vertices[1].y*src_vertices[1].y),
			      0,0,0,tgt_vertices[2].x, tgt_vertices[2].y,1,-(tgt_vertices[2].x*src_vertices[2].y), -(tgt_vertices[2].y*src_vertices[2].y),
                              0,0,0,tgt_vertices[3].x, tgt_vertices[3].y,1,-(tgt_vertices[3].x*src_vertices[3].y), -(tgt_vertices[3].y*src_vertices[3].y);
  
  Eigen::MatrixXd transformation_matrix (8,8);

 // invert matrix		
  transformation_matrix = inv_transformation_matrix.inverse();

  Eigen::MatrixXd source_coords (8,1);

  source_coords << src_vertices[0].x, src_vertices[1].x, src_vertices[2].x, src_vertices[3].x, 
  		   src_vertices[0].y, src_vertices[1].y, src_vertices[2].y, src_vertices[3].y;

// fill in Pmap−to−image matrix
  Pmap_to_image = transformation_matrix *  source_coords;
 
//transform shape of Pmap−to−image matrix 
  Eigen::MatrixXd Pmap (3,3);

// for each point in the map image to find the location of the digital count value in the original image to transfer   
  Pmap << Pmap_to_image(0,0),Pmap_to_image(1,0), Pmap_to_image(2,0),
  	  Pmap_to_image(3,0), Pmap_to_image(4,0), Pmap_to_image(5,0),
	  Pmap_to_image(6,0), Pmap_to_image(7,0), 1;

// use Pmap matrix to transfrom from input to output

Eigen::MatrixXd input(3,1);
Eigen::MatrixXd output (3,1);

for (double r = 0; r < tgt.rows; r++) {
    for (double c = 0; c < tgt.cols; c++) {
 		input(0,0) = c;
		input(1,0) = r;
		input(2,0) = 1;

		output =  Pmap *  input;

		map1.at<float> (r,c) = output(0,0)/ output(2,0);
		map2.at<float> (r,c) = output(1,0)/ output(2,0);
  
    }
}
return true;
}
}
