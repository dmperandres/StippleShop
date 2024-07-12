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


#ifndef _FILTER_BILATERAL_H
#define _FILTER_BILATERAL_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include "line_edit.h"
#include <string>
#include "filter.h"

#define DEFINED_FILTER_BILATERAL

namespace _f_bilateral_ns
{
  // parameter 1
  const std::string String_group_box_parameter1("Size");
  const std::string String_label_parameter1_min("1 ");
  const std::string String_label_parameter1_max("50");
  const std::string String_label_parameter1_tooltip("Display the value of Kernel size");
  const int Parameter1_min_value=1;
  const int Parameter1_max_value=50;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=5;
  const int Parameter1_tick_interval=5;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_tooltip("Controls the effect on the dilation");
  const int Parameter1_divisor=10;

  // parameter 2
  const std::string String_group_box_parameter2("Iterations");
  const std::string String_label_parameter2_min("0 ");
  const std::string String_label_parameter2_max("15");
  const std::string String_label_parameter2_tooltip("Display the number of iterations");
  const int Parameter2_min_value=0;
  const int Parameter2_max_value=15;
  const int Parameter2_single_step=1;
  const int Parameter2_page_step=3;
  const int Parameter2_tick_interval=3;
  const bool Parameter2_set_tracking=false;
  const std::string String_parameter2_slider_tooltip("Controls the effect of dilation");

  // Default values
  const int BILATERAL_KERNEL_SIZE_DEFAULT=1;
  const int BILATERAL_NUM_ITERATIONS_DEFAULT=0;
}

class _gl_widget;
class _qtw_filter_bilateral;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_bilateral : public _filter
{
public:
    _filter_bilateral();
    ~_filter_bilateral(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};


    void update();
    void kernel_size(int Kernel_size1);
    void num_iterations(int Num_iterations1);

    int kernel_size(){return Kernel_size;};
    int num_iterations(){return Num_iterations;};

    float Kernel_size,Num_iterations;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_bilateral_ui : public _filter_bilateral
{
public:
  _filter_bilateral_ui(_gl_widget *GL_widget1,std::string Name1="Bilateral parameters");
  _filter_bilateral_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Name1="Bilateral parameters");
  ~_filter_bilateral_ui();
  void reset_data();


  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(int Value){_filter_bilateral::kernel_size(Value);};
  int parameter1(){return _filter_bilateral::kernel_size();};

  void parameter2(int Value){_filter_bilateral::num_iterations(Value);};
  int parameter2(){return _filter_bilateral::num_iterations();};

private:
  _qtw_filter_bilateral *Qtw_filter_bilateral;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_bilateral: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_bilateral(_gl_widget *GL_widget1,_filter_bilateral_ui *Filter1,std::string Box_name="Bilateral parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);
  void set_parameter2(int Value);

protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);

private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;

  // contrast
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  // bright
  QSlider *Slider_parameter2;
  QLineEdit *Line_edit_parameter2;

  _filter_bilateral_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
