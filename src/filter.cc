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


#include "filter.h"



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter::~_filter()
{

}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter::set_images(cv::Mat *Input_image0,cv::Mat *Output_image0,cv::Mat *Input_image1)
{
  assert(Input_image0!=nullptr);
  assert(Output_image0!=nullptr);
  Input_image_0=Input_image0;
  Output_image_0=Output_image0;
  Input_image_1=Input_image1;
  Ready=true;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter::copy_input_to_output()
{
  // Check the number of input channels
  if (Input_image_0->channels()!=Output_image_0->channels()){// Different number of input channels
    if (Input_image_0->channels()==3){ // color to gray
      cvtColor(*Input_image_0,*Output_image_0,CV_BGR2GRAY,1);
    }
    else{ // gray to color
      cvtColor(*Input_image_0,*Output_image_0,CV_GRAY2RGB,3);
    }
  }
  else{// the same number of channels
    *Output_image_0=Input_image_0->clone();
  }

  cv::line(*Output_image_0, cv::Point(0,0),cv::Point(Output_image_0->cols,Output_image_0->rows),0,5);
  cv::line(*Output_image_0, cv::Point(0,Output_image_0->rows),cv::Point(Output_image_0->cols,0),0,5);
}
