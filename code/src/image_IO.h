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


#ifndef IMAGEIO_H
#define IMAGEIO_H

#include <opencv.hpp>

#include <iostream>
#include <string>


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _image_IO
{
public:
  std::string String;

  void read_image(std::string Name,cv::Mat &Image_in);
  void write_image(std::string Name, cv::Mat &Image_out);
};

#endif
