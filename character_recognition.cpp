/** Implementation character recognition algorithyms
 *
 *  \file apple/character_Recognition/main.cpp
 *  \author ethan poole erp1787
 *  \date 10/31/2019
 */

// i got close enough 


#include <iostream>
#include <string>
#include <filesystem>
#include <ctime>
#include <cmath>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/program_options.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>


using namespace std;
namespace po = boost::program_options;


  /// SPATIAL METHOD
bool spatial(cv::Mat src, double threshold_s, cv::Mat kernel, double max) {

	int ddepth = -1;
                        // 2Dfilter the current kernel location
	cv::Mat values;
	cv::filter2D(src, values, ddepth ,kernel);

                        // test if the value is higher than the threshold
	int count1 = 0;	
        for (int r = 0; r<values.rows; r++){
        	for (int c = 0; c<values.cols; c++){
                	float response_value = values.at<uint8_t>(r,c) / max;
                        if (response_value > threshold_s) {
                                count1 ++;
                        
			}
                }
        }
return count1;             
}

  /// VECTOR METHOD

bool fuck_vector(cv::Mat src,double threshold_v, cv::Mat kernel) {

    

        int count2 = 0;
	double response_value = 0;

	double mag_kern, dot_product, mag_src = 0; 
	
	double kc = kernel.cols;
	double kr = kernel.rows;
	
	//make current kernel a vector and source a vector
	
	std::vector<int> vector_kernel;
	std::vector<int> vector_src;
	double vec_size = vector_kernel.size();
		
	for (int r = 0; r<kernel.rows; r++){
                for (int c = 0; c<kernel.cols; c++){ 		
			vector_kernel.push_back(kernel.at<float>(r, c));        	
			mag_kern += kernel.at<float>(r,c);
		}
	}
	mag_kern = sqrt(mag_kern);
	for (int sr = 0; sr<src.rows; sr++){
                for (int sc = 0; sc<src.cols; sc++){
			cv::Mat rectangle = src(cv::Rect(sc,sr,kc,kr));
			vector_src.assign(rectangle.begin<uchar>(),rectangle.end<uchar>());
		//find the angles
			for (int v = 0; v < vec_size; v++){
				dot_product += vector_kernel[v] * vector_src[v];
				mag_src += pow(vector_src[v],2);
			}
        		//relate angles to thresholds
		
			
		mag_src = sqrt(mag_src);
		
		response_value = dot_product/(mag_kern *mag_src);
		if (response_value > threshold_v) {
                                count2 ++;
       		}
        
	vector_kernel.clear();
		}	
	}	
return count2;
}


int main(int argc, char* argv[]) {

		// make user input using boost

	int approach_type = 0;

	po::options_description options("Options");
	options.add_options()("approach_type,a",po::value<int>(&approach_type), "approach type 0 is spatial, 1 is vector [default 0]");

	//start code
	bool verbose = false;

	//define thresholds
	double threshold_s = 0.999;
	double threshold_v = 0.970;

	// import source

	cv::Mat src;
	src = cv::imread("/cis/faculty/cnspci/public_html/courses/common/images/character_recognition/notAntiAliased/alphabet.tif", cv::IMREAD_GRAYSCALE);

	src = 255 - src;
        // normalise
       	src /=255;

        //cout << src << endl;
	// import kernels

	string path = "/cis/faculty/cnspci/public_html/courses/common/images/character_recognition/notAntiAliased/characters/";
	std::string extension = ".tif";


	const auto& entries = std::filesystem::directory_iterator(path);

	// make empty matrix
	cv::Mat histogram = cv::Mat(40, 1, CV_32FC1);
 
	double hist_loc = 0;

	//loop through kernels
	
	for (const auto& entry : entries) {
		if (entry.path().extension() == extension) {
			cv::Mat kernel = cv::imread(entry.path(), cv::IMREAD_UNCHANGED);
			kernel = 255 - kernel;
			kernel.convertTo(kernel,CV_32FC1);
			kernel /= 255;
			double max = cv::sum(kernel)[0];
			cout<< approach_type << endl;	
	//definie approach type then run approach
			
			switch(approach_type) {
				case 0:
					{
					float a  = spatial(src,threshold_s,kernel,max);
					histogram.at<float>(hist_loc,0) = a;
					hist_loc++;
					break;
					}
				case 1:
					{
					float b  = fuck_vector(src,threshold_v,kernel);
					histogram.at<float>(hist_loc,0) = b;
                                        hist_loc++;
					break;
					}
			}
		}
	
	}	
		cout<< histogram << endl;
	

	// print histogram that is generated



return EXIT_SUCCESS;
}
