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


#ifndef _FILTER_HALFTONING_CAH_H
#define _FILTER_HALFTONING_CAH_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>

#include <opencv.hpp>

#include "line_edit.h"
#include <string>
#include "filter.h"

#define DEFINED_FILTER_CONTRAST_AWARE_HALFTONING

namespace _f_halftoning_cah_ns
{
  // Kernel size
  // parameter 1
  const std::string String_group_box_parameter1("Kernel size");
  const std::string String_label_parameter1_min("3 ");
  const std::string String_label_parameter1_max("15");
  const std::string String_label_parameter1_tooltip("Display the size of the kernel");
  const int Parameter1_min_value=3;
  const int Parameter1_max_value=15;
  const int Parameter1_single_step=2;
  const int Parameter1_page_step=4;
  const int Parameter1_tick_interval=2;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_tooltip("Controls the the effect");

  // Exponent for the contrast function
  // parameter 2
  const std::string String_group_box_parameter2("Exponent");
  const std::string String_label_parameter2_min("1 ");
  const std::string String_label_parameter2_max("5 ");
  const std::string String_label_parameter2_tooltip("Display the size of the gaussian filter");
  const int Parameter2_min_value=100;
  const int Parameter2_max_value=500;
  const int Parameter2_single_step=10;
  const int Parameter2_page_step=20;
  const int Parameter2_tick_interval=50;
  const bool Parameter2_set_tracking=false;
  const std::string String_parameter2_slider_tooltip("Controls the exponent");
  const float Parameter2_factor=100;

  // Default values
  const int HALFTONING_CAH_KERNEL_SIZE_DEFAULT=7;
  const float HALFTONING_CAH_EXPONENT_DEFAULT=2.6f;
}

class _gl_widget;
class _qtw_filter_halftoning_cah;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_halftoning_cah : public _filter
{
public:
    _filter_halftoning_cah();
    ~_filter_halftoning_cah(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};


    void halftoning(cv::Mat *Input_image0,cv::Mat *Output_image0);
    void update();
    void kernel_size(int Kernel_size1){Kernel_size=Kernel_size1;};
    void exponent(float Exponent1){Exponent=Exponent1;};

    int kernel_size(){return Kernel_size;};
    float exponent(){return Exponent;};

    int Kernel_size;
    float Exponent;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_halftoning_cah_ui :public _filter_halftoning_cah
{
public:
  _filter_halftoning_cah_ui(_gl_widget *GL_widget1,std::string Name1="Contrast-Aware halftoning parameters");
  _filter_halftoning_cah_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Name1="Contrast-Aware halftoning parameters");
  ~_filter_halftoning_cah_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(int Value){_filter_halftoning_cah::kernel_size(Value);};
  int parameter1(){return _filter_halftoning_cah::kernel_size();};

  void parameter2(float Value){_filter_halftoning_cah::exponent(Value);};  
  float parameter2(){return _filter_halftoning_cah::exponent();};

  private:
  _qtw_filter_halftoning_cah *Qtw_filter_halftoning_cah;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_halftoning_cah: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_halftoning_cah(_gl_widget *GL_widget1,_filter_halftoning_cah_ui *Filter1,std::string Box_name="Contrast-Aware halftoning parameters");
  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Size);
  void set_parameter2(float Value);

protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);

private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;

  // Kernel size
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  // Exponent
  QSlider *Slider_parameter2;
  QLineEdit *Line_edit_parameter2;

  _filter_halftoning_cah_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
