/** Implementation file for displaying the fourier transform of an image
 *
 *  \file apps/fft_display/fft_display.cpp
 *  \author ethan poole erp1787
 *  \date 12/7/19
 *
 */

//please have mercy on my soul

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <filesystem>

#include <iostream>
#include <string>
#include <math.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "imgs/ipcv/utils/Utils.h"

using namespace cv;
using namespace std;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  cv::Mat mag;

  // coomand line for input image
  // boost stuff cut and pasted from old project
  bool verbose = false;
  string src_filename = "";
  po::options_description options("Options");
  options.add_options()("help,h", "display this message")(
      "verbose,v", po::bool_switch(&verbose), "verbose [default is silent]")(
      "source-filename,i", po::value<string>(&src_filename), "source filename");
  po::positional_options_description positional_options;
  positional_options.add("source-filename", -1);
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(options)
                .positional(positional_options)
                .run(),
            vm);
  po::notify(vm);

  // to test comment out to use boost
  src_filename =
      "/cis/faculty/cnspci/public_html/courses/common/images/lenna-512x512.tif";
  // create input source with row for imaginary
  cv::Mat src = cv::imread(src_filename, cv::IMREAD_GRAYSCALE);
  src.convertTo(src, CV_64FC1);

  // normalize and center
  // cv::Mat vis_dft;
  // vis_dft = ipcv::DftMagnitude(dft,ipcv::DFT_MAGNITUDE_LOG +
  // ipcv::DFT_MAGNITUDE_CENTER +ipcv::DFT_MAGNITUDE_NORMALIZE);

  // declare vaiables because i have to use them outside loop
  cv::Mat inv_sum;
  cv::Mat dad_sum;
  cv::Mat inv_current_comp;
  cv::Mat output_boi;
  cv::Mat end_image;

  // cout<< "B" << endl;

  // add imaginary
  cv::Mat src_complex[2] = {src, cv::Mat::zeros(src.size(), CV_64FC1)};
  cv::Mat input;
  cv::merge(src_complex, 2, input);

  // cout<< "A" << endl;

  // calculate dft of source
  cv::Mat output;
  cv::dft(input, output, cv::DFT_COMPLEX_OUTPUT | cv::DFT_SCALE);

  // set variables for keys
  bool run = true;
  bool pause = false;
  bool step = false;
  int key = cv::waitKey(1);

  if (key == 32) {
    step = true;
    pause = true;
  }

  double max_value;

  cv::Mat max_dft;
  max_dft.create(src.size(), CV_64FC2);

  // find real and imaginary parts of dft

  cv::Mat dft_imag[2];
  cv::split(output, dft_imag);

  max_value = 100.0;
  while (run == true) {
    if (pause == false || step == true) {
      cv::Point max;
      cout << yaa << endl;
      cv::minMaxLoc(dft_imag[0], NULL, &max_value, NULL, &max);

      for (int n = 0; n <= 1; n++) {
        max_dft.at<cv::Vec2d>(max)[n] = output.at<cv::Vec2d>(max)[n];
      }

      // perform dft
      cv::dft(max_dft, inv_sum, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
      inv_sum.convertTo(inv_sum, CV_8UC1);

      cv::Mat current_comp;
      current_comp = cv::Mat::zeros(src.size(), CV_64FC2);
      for (int n = 0; n <= 1; n++) {
        current_comp.at<cv::Vec2d>(max)[n] = output.at<cv::Vec2d>(max)[n];
      }
      // fill in magnitudes
      dad_sum = ipcv::DftMagnitude(max_dft, ipcv::DFT_MAGNITUDE_LOG +
                                                ipcv::DFT_MAGNITUDE_CENTER) *
                255;
      dad_sum.convertTo(dad_sum, CV_8UC1);

      // find inverse dft of component
      cv::dft(current_comp, inv_current_comp,
              cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
      // convert and prepare
      inv_current_comp.convertTo(inv_current_comp, CV_8UC1);
      double max_value_output;
      cv::minMaxLoc(inv_current_comp, NULL, &max_value_output);
      inv_current_comp += 128;

      // scale output
      output_boi = ((inv_current_comp - 128) * 255) / max_value_output;

      for (int n = 0; n <= 1; n++) {
        dft_imag[n].at<double>(max) = 0;
      }
    }

    cv::Mat end_image =
        ipcv::DftMagnitude(output, ipcv::DFT_MAGNITUDE_CENTER +
                                       ipcv::DFT_MAGNITUDE_LOG +
                                       ipcv::DFT_MAGNITUDE_NORMALIZE) *
        255;

    end_image.convertTo(end_image, CV_8UC1);

    // pring out all calculated values
    if (verbose) {
      cv::imshow("1", src);
      cv::imshow("2", inv_sum);
      cv::imshow("3", dad_sum);
      cv::imshow("4", inv_current_comp);
      cv::imshow("5", end_image);
      cv::imshow("6", output_boi);
    }

    // check to see if any key was pressed
    if (max_value <= 0) {
      run = false;
      cout << AHHH << endl;
    } else if (key == 113 || key == 81 || key == 27) {
      run = false;
      cout << ahh << endl;
    }
    if (pause == true && (key == 80 || key == 112)) {
      pause = false;
    } else if (pause == false && (key == 80 || key == 112)) {
      pause = true;
    }
  }
  return EXIT_SUCCESS;
}
