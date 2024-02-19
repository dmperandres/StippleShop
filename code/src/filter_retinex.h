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


#ifndef _FILTER_RETINEX_H
#define _FILTER_RETINEX_H

#include <QDialog>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QGridLayout>
#include <QSlider>
#include "line_edit.h"
//#include "qslider1.h"
#include <string>
#include <limits>
#include "filter.h"

#define DEFINED_FILTER_RETINEX

namespace _f_retinex_ns
{
  //parameter1
  const std::string String_group_box_parameter1("Color restoration variance");
  const std::string String_label_parameter1_min("0 ");
  const std::string String_label_parameter1_max("4");
  const std::string String_label_parameter1_tooltip("Display the value of the number of kernels");
  const int Parameter1_min_value=0;
  const int Parameter1_max_value=255;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=10;
  const int Parameter1_tick_interval=15;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_slider_tooltip("Controls the color restoration variation");

  typedef enum {FILTER_UNIFORM,FILTER_LOW,FILTER_HIGH} _filter_mode;

  // max-min values
  const int RETINEX_MAX_NUM_KERNELS=5;
  const int RETINEX_MIN_GAUSSIAN_KERNEL_SIZE=16;
  const int RETINEX_MAX_GAUSSIAN_KERNEL_SIZE=250;
  const int RETINEX_KERNEL_SIZE_WIDTH=150;
  const int RETINEX_MAX_COLOR_RESTORATION_VARIANCE=4;

  // default values
  const _filter_mode RETINEX_FILTER_MODE_DEFAULT=FILTER_UNIFORM;
  const int RETINEX_NUM_KERNELS_DEFAULT=3;
  const int RETINEX_GAUSSIAN_KERNEL_SIZE_DEFAULT=80;
  const float RETINEX_COLOR_RESTORATION_VARIANCE_DEFAULT=1;
}

class _gl_widget;
class _qtw_filter_retinex;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_retinex : public _filter
{
public:
  _filter_retinex();
  ~_filter_retinex(){};
  void reset_data();
  bool change_output_image_size(){return Change_output_image_size;};
  bool use_dots(){return Use_dots;};

  void update();

  void color_restoration_variance(float Color_restoration_variance1);
  void kernel_size(unsigned int Pos,int Value);

  int num_kernels(){return Num_kernels;};
  float color_restoration_variance(){return Color_restoration_variance;};
  int kernel_size(unsigned int Pos){
      if ((int)Pos<Num_kernels) return Vec_kernel_size[Pos];
      else return(3);
  };

  int Kernel_size;
  int Num_kernels;
  float Color_restoration_variance;
  int Vec_kernel_size[_f_retinex_ns::RETINEX_MAX_NUM_KERNELS];
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_retinex_ui : public _filter_retinex
{
public:
  _filter_retinex_ui(_gl_widget *GL_widget1,std::string Name1="Retinex parameters");
  _filter_retinex_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Name1="Retinex parameters");
  ~_filter_retinex_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(float Value){_filter_retinex::color_restoration_variance(Value);};
  float parameter1(){return _filter_retinex::color_restoration_variance();};

private:
  _qtw_filter_retinex *Qtw_filter_retinex;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_retinex: public QWidget
{
   Q_OBJECT
public:

  _qtw_filter_retinex(_gl_widget *GL_widget1,_filter_retinex_ui *Filter1,std::string Box_name="Retinex parameters");
  ~_qtw_filter_retinex();
  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(float Value);

protected slots:
  void set_parameter1_slot(int Value);

private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter1;

  // Color_restoration_variance
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  _filter_retinex_ui *Filter;
  _gl_widget *GL_widget;
};

#endif
