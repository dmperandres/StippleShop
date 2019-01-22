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


#ifndef _FILTER_DOG_H
#define _FILTER_DOG_H

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

#define DEFINED_FILTER_DOG

namespace _f_dog_ns
{
  // gaussian umbral
  // parameter 1
  const std::string String_group_box_parameter1("Gaussian umbral");
  const std::string String_label_parameter1_min("0 ");
  const std::string String_label_parameter1_max("255");
  const std::string String_label_parameter1_tooltip("Display the size of the gaussian umbral");
  const int Parameter1_min_value=0;
  const int Parameter1_max_value=255;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=10;
  const int Parameter1_tick_interval=15;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_tooltip("Controls the appearance of borders: more to the left, less to the right");

  // big size
  // parameter 2
  const std::string String_group_box_parameter2("Big gaussian size");
  const std::string String_label_parameter2_min("5 ");
  const std::string String_label_parameter2_max("51");
  const std::string String_label_parameter2_tooltip("Display the size of the gaussian filter");
  const int Parameter2_min_value=5;
  const int Parameter2_max_value=51;
  const int Parameter2_single_step=1;
  const int Parameter2_page_step=10;
  const int Parameter2_tick_interval=10;
  const bool Parameter2_set_tracking=false;
  const std::string String_parameter2_slider_tooltip("Controls the size of the gaussian filter");

  // small size
  //parameter3
  const std::string String_group_box_parameter3("Small gaussian size");
  const std::string String_label_parameter3_min("3 ");
  const std::string String_label_parameter3_max("51");
  const std::string String_label_parameter3_tooltip("Display the value ofthe size of the small gaussian");
  const int Parameter3_min_value=3;
  const int Parameter3_max_value=51;
  const int Parameter3_single_step=1;
  const int Parameter3_page_step=10;
  const int Parameter3_tick_interval=10;
  const bool Parameter3_set_tracking=false;
  const std::string String_parameter3_slider_tooltip("Controls the size of the gaussian filter");

  // Default values
  const int DOG_UMBRAL_DEFAULT=250;
  const int DOG_BIG_GAUSSIAN_SIZE_DEFAULT=25;
  const int DOG_SMALL_GAUSSIAN_SIZE_DEFAULT=3;
}

class _gl_widget;
class _qtw_filter_dog;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_dog : public _filter
{
public:
    _filter_dog();
    ~_filter_dog(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};


    void update();
    void dog_umbral(int Umbral);
    void big_gaussian_size(int Size);
    void small_gaussian_size(int Size);

    int dog_umbral(){return Dog_umbral;};
    int big_gaussian_size(){return Big_gaussian_size;};
    int small_gaussian_size(){return Small_gaussian_size;};

    int Dog_umbral,Big_gaussian_size,Small_gaussian_size;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_dog_ui :public _filter_dog
{
public:
  _filter_dog_ui(_gl_widget *GL_widget1, std::string Name1="DoG parameters");
  _filter_dog_ui(_gl_widget *GL_widget1, std::map<std::string,std::string> &Parameters, std::string Name1="DoG parameters");
  ~_filter_dog_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);

  void parameter1(int Value){_filter_dog::dog_umbral(Value);};
  void parameter2(int Value){_filter_dog::big_gaussian_size(Value);};
  void parameter3(int Value){_filter_dog::small_gaussian_size(Value);};

  int parameter1(){return _filter_dog::dog_umbral();};
  int parameter2(){return _filter_dog::big_gaussian_size();};
  int parameter3(){return _filter_dog::small_gaussian_size();};

  private:
  _qtw_filter_dog *Qtw_filter_dog;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_dog: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_dog(_gl_widget *GL_widget1,_filter_dog_ui *Filter1,std::string Box_name="DoG parameters");
  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);
  void set_parameter2(int Value);
  void set_parameter3(int Value);

protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);
  void set_parameter3_slot(int Value);

private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;

  // DOG umbral
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  // big gaussian size
  QSlider *Slider_parameter2;
  QLineEdit *Line_edit_parameter2;

  // small gaussian size
  QSlider *Slider_parameter3;
  QLineEdit *Line_edit_parameter3;


  _filter_dog_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
