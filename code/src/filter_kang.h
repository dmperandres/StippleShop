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


#ifndef _FILTER_KANG_H
#define _FILTER_KANG_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>

#include "line_edit.h"
#include <string>
#include "filter.h"

#include <opencv.hpp>

#define DEFINED_FILTER_KANG

namespace _f_kang_ns
{
  // Radius
  // parameter 1
  const std::string String_group_box_parameter1("Radius");
  const std::string String_label_parameter1_min("1 ");
  const std::string String_label_parameter1_max("10");
  const std::string String_label_parameter1_tooltip("Display the radius");
  const int Parameter1_min_value=1;
  const int Parameter1_max_value=10;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=1;
  const int Parameter1_tick_interval=2;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_slider_tooltip("Controls the radius");

  // gaussian line length
  // parameter 2
  const std::string String_group_box_parameter2("Gaussian line length");
  const std::string String_label_parameter2_min("1 ");
  const std::string String_label_parameter2_max("25");
  const std::string String_label_parameter2_tooltip("Display the size of the kernel of gaussian for the line length");
  const int Parameter2_min_value=1;
  const int Parameter2_max_value=25;
  const int Parameter2_single_step=1;
  const int Parameter2_page_step=2;
  const int Parameter2_tick_interval=5;
  const bool Parameter2_set_tracking=false;
  const std::string String_parameter2_slider_tooltip("Controls the size of the kernel of gaussian for the line length");

  // DOG gaussian surround
  // parameter 3
  const std::string String_group_box_parameter3("Surround gaussian size");
  const std::string String_label_parameter3_min("5 ");
  const std::string String_label_parameter3_max("51");
  const std::string String_label_parameter3_tooltip("Display the size of the kernel of surround gaussian");
  const int Parameter3_min_value=5;
  const int Parameter3_max_value=51;
  const int Parameter3_single_step=1;
  const int Parameter3_page_step=10;
  const int Parameter3_tick_interval=10;
  const bool Parameter3_set_tracking=false;
  const std::string String_parameter3_slider_tooltip("Controls the size of the kernel of surround gaussian");

  // DOG gaussian center
  // parameter4
  const std::string String_group_box_parameter4("Center gaussian size");
  const std::string String_label_parameter4_min("3 ");
  const std::string String_label_parameter4_max("49");
  const std::string String_label_parameter4_tooltip("Display the size of the kernel of center gaussian");
  const int Parameter4_min_value=3;
  const int Parameter4_max_value=49;
  const int Parameter4_single_step=1;
  const int Parameter4_page_step=10;
  const int Parameter4_tick_interval=10;
  const bool Parameter4_set_tracking=false;
  const std::string String_parameter4_slider_tooltip("Controls the size of the kernel of center gaussian");

  // Default values
  const int KERNEL_SIZE_CENTER_DEFAULT=5;
  const int KERNEL_SIZE_SURROUND_DEFAULT=9;
  const int KERNEL_SIZE_LINE_LENGTH_DEFAULT=9;
  const float RADIUS_DEFAULT=5;
  const float RO_DEFAULT=0.997;
  const float ETA_DEFAULT=1.0;
  const int LINES_ITERATIONS_DEFAULT=3;
  const int ETF_ITERATIONS_DEFAULT=3;

#ifdef WIN_COMPILER
  const double M_PI = 3.1415926535897932;
#endif
}

class _gl_widget;
class _qtw_filter_kang;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_kang : public _filter
{
public:


  _filter_kang();
  ~_filter_kang(){};
  void reset_data();
  bool change_output_image_size(){return Change_output_image_size;};
  bool use_dots(){return Use_dots;};


  void kang(cv::Mat *Aux_input_image, cv::Mat *Aux_output_image);
  void update();

  void edge_tangent_flow(cv::Mat *Input_image,cv::Mat *Output_image);
  void draw_vectors(cv::Mat *Input_image,cv::Mat *Output_image,cv::Mat *X,cv::Mat *Y);

  float line(int initial_i, int initial_j, int S, int T,cv::Mat *Image,std::vector<float> &Function_f,std::vector<float> &Gauss_m);
  void line_extraction(cv::Mat *Image_lines,cv::Mat *Aux_input_image);

  void radius(float Radius1){Radius=Radius1;};
  float radius(){return Radius;};

  void kernel_size_line_length(int Kernel_size_line_length1){Kernel_size_line_length=Kernel_size_line_length1;};
  int kernel_size_line_length(){return Kernel_size_line_length;};

  void kernel_size_surround(int Kernel_size_surround1){Kernel_size_surround=Kernel_size_surround1;};
  int kernel_size_surround(){return Kernel_size_surround;};

  void kernel_size_center(int Kernel_size_center1){Kernel_size_center=Kernel_size_center1;};
  int kernel_size_center(){return Kernel_size_center;};

private:
  cv::Mat ETF_x[2];
  cv::Mat ETF_y[2];

  float Delta_line_length;
  float Delta_line_width;
  float Theta;
  float Eta;

  int Final_position;

  float Radius;
  int Kernel_size_center;
  int Kernel_size_surround;
  int Kernel_size_line_length;
  float Ro;
  int Lines_iterations;
  int ETF_iterations;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_kang_ui :public _filter_kang
{
public:
  _filter_kang_ui(_gl_widget *GL_widget1,std::string Name1="Kang parameters");
  _filter_kang_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Name1="Kang parameters");
  ~_filter_kang_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(int Value){radius((float)Value);};
  int  parameter1(){return (int)radius();};

  void parameter2(int Value){kernel_size_line_length(Value);};
  int  parameter2(){return kernel_size_line_length();};

  void parameter3(int Value){kernel_size_surround(Value);};
  int  parameter3(){return kernel_size_surround();};

  void parameter4(int Value){kernel_size_center(Value);};
  int  parameter4(){return kernel_size_center();};

  private:
  _qtw_filter_kang *Qtw_filter_kang;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_kang: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_kang(_gl_widget *GL_widget1,_filter_kang_ui *Filter1,std::string Box_name="Kang parameters");
  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void parameter1(int Value);
  void parameter2(int Value);
  void parameter3(int Value);
  void parameter4(int Value);


protected slots:
  void parameter1_slot(int Value);
  void parameter2_slot(int Value);
  void parameter3_slot(int Value);
  void parameter4_slot(int Value);


private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter1;
  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;
  QGroupBox *Group_box_parameter4;

  // Radius
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  // Line length size
  QSlider *Slider_parameter2;
  QLineEdit *Line_edit_parameter2;

  // Surround kernel size
  QSlider *Slider_parameter3;
  QLineEdit *Line_edit_parameter3;

  // Center kernel size
  QSlider *Slider_parameter4;
  QLineEdit *Line_edit_parameter4;

  _filter_kang_ui *Filter;
  _gl_widget *GL_widget;
};

#endif
