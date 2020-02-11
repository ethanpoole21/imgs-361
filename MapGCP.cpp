/** Implementation file for finding source image coordinates for a source-to-map
 *  remapping using ground control points
 *
 *  \file ipcv/geometric_transformation/MapGCP.cpp
 *  \author Ethan Poole Erp1787
 *  \date 11-7-2019
 */

#include "MapGCP.h"

#include <iostream>
#include <math.h>
#include <vector>
#include <eigen3/Eigen/Dense>
#include <opencv2/core.hpp>

using namespace std;

namespace ipcv {

/** Find the source coordinates (map1, map2) for a ground control point
 *  derived mapping polynomial transformation
 *
 *  \param[in] src   source cv::Mat of CV_8UC3
 *  \param[in] map   map (target) cv::Mat of CV_8UC3
 *  \param[in] src_points
 *                   vector of cv::Points representing the ground control
 *                   points from the source image
 *  \param[in] map_points
 *                   vector of cv::Points representing the ground control
 *                   points from the map image
 *  \param[in] order  mapping polynomial order
 *                      EXAMPLES:
 *                        order = 1
 *                          a0*x^0*y^0 + a1*x^1*y^0 +
 *                          a2*x^0*y^1
 *                        order = 2
 *                          a0*x^0*y^0 + a1*x^1*y^0 + a2*x^2*y^0 +
 *                          a3*x^0*y^1 + a4*x^1*y^1 +
 *                          a5*x^0*y^2
 *                        order = 3
 *                          a0*x^0*y^0 + a1*x^1*y^0 + a2*x^2*y^0 + a3*x^3*y^0 +
 *                          a4*x^0*y^1 + a5*x^1*y^1 + a6*x^2*y^1 +
 *                          a7*x^0*y^2 + a8*x^1*y^2 +
 *                          a9*x^0*y^3
 *  \param[out] map1  cv::Mat of CV_32FC1 (size of the destination map)
 *                    containing the horizontal (x) coordinates at which to
 *                    resample the source data
 *  \param[out] map2  cv::Mat of CV_32FC1 (size of the destination map)
 *                    containing the vertical (y) coordinates at which to
 *                    resample the source data
 */
bool MapGCP(const cv::Mat src, const cv::Mat map,
            const vector<cv::Point> src_points,
            const vector<cv::Point> map_points, const int order,
            cv::Mat& map1, cv::Mat& map2) {
	
	//create empty map files 
	map1.create(map.size(),CV_32FC1);
	map2.create(map.size(),CV_32FC1);



	//find coeffitients to generate polynomial with
	int coeffs = 0;

	for(int o = 0; o <order +2; o++){
		for (int n =  0; n < o; n++){
			coeffs++;
		}
	}
	cout<< coeffs << endl;

	//create matricies
        Eigen::MatrixXd model_matrix(map_points.size(), coeffs);

	 // create point matricies
        Eigen::MatrixXd point_x(src_points.size(), 1);
        Eigen::MatrixXd point_y(src_points.size(), 1);

	// create coefficient matrix

	Eigen::MatrixXd x_coeff(coeffs,1);
	Eigen::MatrixXd y_coeff(coeffs,1);
		
	
	double num_points = map_points.size();
	double src_size = src_points.size();

	//loop calculating exponent values for given order
	
	vector<int> x_exponents;
	vector<int> y_exponents;
	
	for(int y = 0; y<= order; y++){
		for(int x = 0; x<= order; x++){
			if (x+y <= order){
				x_exponents.push_back(x);
				y_exponents.push_back(y);
			}
		}
	}

	//cout<< x_exponents << endl;
	//cout<< y_exponents << endl;
	
	

	// create matrix that for mapping functions (includes transpose and inverse
	
	for(int r = 0;r <num_points; r++){
                for (int c = 0; c < coeffs; c++){
		
			model_matrix(r,c) = pow(map_points[r].x, x_exponents[c]) * pow(map_points[r].y, y_exponents[c]); 
		}
	}

	// could be error
	for (int a = 0; a<src_size; a++){
		point_x (a,0) = src_points[a].x;
		point_y (a,0) = src_points[a].y;
	}
	

	Eigen::MatrixXd transpose = model_matrix.transpose();
	//Eigen::MatrixXd inverse = (transpose.inverse()*model_matrix.inverse());

	//Eigen::MatrixXd inverse = (transpose*model_matrix).inverse();
	//cout << transpose << endl;
	
	// create coefficient matricies that are transposed and incerted correctly back to map space
		
	x_coeff = (transpose * model_matrix).inverse() * transpose * point_x; 
	y_coeff = (transpose * model_matrix).inverse() * transpose * point_y;

	cout<< x_coeff << endl;
	cout<< y_coeff << endl;
	
	// fill in the mao with the new coordinates
	for (int r = 0; r<map.rows; r++){
		for (int c = 0; c<map.cols; c++){
			for(int f = 0; f<coeffs; f++){
				map1.at<float>(r,c) += x_coeff(f,0) *pow(c,x_exponents[f]) * pow(r,y_exponents[f]);
				map2.at<float>(r,c) += y_coeff(f,0) *pow(c,x_exponents[f]) * pow(r,y_exponents[f]);
			}
		}
	}


  return true;
}
}
