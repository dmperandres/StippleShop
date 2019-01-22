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


#ifndef _FILTER_GAUSSIAN_H
#define _FILTER_GAUSSIAN_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>

#ifdef X64
  #include <opencv.hpp>
#else
  #include <core/core.hpp>
  #include <highgui/highgui.hpp>
#endif

#include "line_edit.h"
#include <string>
#include "filter.h"

#define DEFINED_FILTER_GAUSSIAN

namespace _f_gaussian_ns
{
  // gaussian umbral
  // parameter 1
  const std::string String_group_box_parameter1("Kernel size");
  const std::string String_label_parameter1_min("1 ");
  const std::string String_label_parameter1_max("255");
  const std::string String_label_parameter1_tooltip("Display the size of the gaussian kernel");
  const int Parameter1_min_value=1;
  const int Parameter1_max_value=255;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=10;
  const int Parameter1_tick_interval=15;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_tooltip("Controls the size of the kernel");

  // Default values
  const int GAUSSIAN_KERNEL_SIZE_DEFAULT=1;
}

class _gl_widget;
class _qtw_filter_gaussian;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_gaussian : public _filter
{
public:
    _filter_gaussian();
    ~_filter_gaussian(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};


    void update();
    void kernel_size(int Kernel_size1){Kernel_size=Kernel_size1;};
    int kernel_size(){return Kernel_size;};

    int Kernel_size;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_gaussian_ui :public _filter_gaussian
{
public:
  _filter_gaussian_ui(_gl_widget *GL_widget1,std::string Name1="Gaussian parameters");
  _filter_gaussian_ui(_gl_widget *GL_widget1, std::map<std::string,std::string> &Parameters, std::string Name1="Gaussian parameters");
  ~_filter_gaussian_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(int Value){_filter_gaussian::kernel_size(Value);};
  int parameter1(){return _filter_gaussian::kernel_size();};

  private:
  _qtw_filter_gaussian *Qtw_filter_gaussian;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_gaussian: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_gaussian(_gl_widget *GL_widget1,_filter_gaussian_ui *Filter1,std::string Box_name="Gaussian parameters");
  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);

protected slots:
  void set_parameter1_slot(int Value);

private:
  QGroupBox *Group_box_main;

  // Kernel size
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  _filter_gaussian_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
