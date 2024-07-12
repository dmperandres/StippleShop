/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2019
 * GPL
 *
 * In case that you use all or part of this code, please include a reference to this article:

 * Domingo Martín, Germán Arroyo, Alejandro Rodríguez and Tobias Isenberg.
 * A survey of digital stippling.
 * Computer & Graphics 67, PP. 24-44, 2017.
 * DOI information: https://doi.org/10.1016/j.cag.2017.05.001
 */


#include "image_IO.h"

//using namespace std;

/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _image_IO::read_image(std::string Name,cv::Mat &Image_in)
{
  // #ifdef WINDOWS
  // std::string Aux_string;
  // for (int i=0;i<Name.size();i++){
  //   if (Name[i]=='/'){
  //     Aux_string.push_back('\\');
  //   }
  //   else Aux_string.push_back(Name[i]);
  // }
  // Name=Aux_string;
  // #endif

  Image_in=cv::imread(Name, cv::IMREAD_UNCHANGED);

  if( !Image_in.data ){ // check if the image has been loaded properly
    std::cout << "Error reading the image: " << Name << std::endl;
    exit(-1);
  }

  if (Image_in.channels()==4){
    cv::cvtColor(Image_in,Image_in,cv::COLOR_BGRA2BGR);
  }

  if (Image_in.depth()!=CV_8U){
    std::cout << "Error in the depth of the image" << std::endl;
    exit(-1);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _image_IO::write_image(std::string Name,cv::Mat &Image_out)
{
  cv::imwrite(Name, Image_out);
}
