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


#ifndef _FILTER_CONTRAST_BRIGHT_H
#define _FILTER_CONTRAST_BRIGHT_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include "line_edit.h"
#include <string>
#include "filter.h"

#define DEFINED_FILTER_CONTRAST_BRIGHT

namespace _f_contrast_bright_ns
{
  // parameter 1
  const std::string String_group_box_parameter1("Contrast");
  const std::string String_label_parameter1_min("0.1 ");
  const std::string String_label_parameter1_max("5");
  const std::string String_label_parameter1_tooltip("Display the value of contrast value");
  const int Parameter1_min_value=1;
  const int Parameter1_max_value=50;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=5;
  const int Parameter1_tick_interval=5;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_tooltip("Controls the appearance of contrast");
  // warning: the capture value must be adjusted to a odd value
  const int Parameter1_divisor=10;

  // parameter 2
  const std::string String_group_box_parameter2("Bright");
  const std::string String_label_parameter2_min("-255 ");
  const std::string String_label_parameter2_max("255");
  const std::string String_label_parameter2_tooltip("Display the value of bright");
  const int Parameter2_min_value=-255;
  const int Parameter2_max_value=255;
  const int Parameter2_single_step=10;
  const int Parameter2_page_step=25;
  const int Parameter2_tick_interval=25;
  const bool Parameter2_set_tracking=false;
  const std::string String_parameter2_slider_tooltip("Controls the appearance of bright");

  // Default values
  const int CANNY_CONTRAST_DEFAULT=1;
  const int CANNY_BRIGHT_DEFAULT=0;
}

class _gl_widget;
class _qtw_filter_contrast_bright;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_contrast_bright : public _filter
{
public:
    _filter_contrast_bright();
    ~_filter_contrast_bright(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};

    void update();
    void contrast(float Contrast1);
    void bright(float Bright1);

    float contrast(){return Contrast;};
    float bright(){return Bright;};

    float Contrast;
    int Bright;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_contrast_bright_ui : public _filter_contrast_bright
{
public:
  _filter_contrast_bright_ui(_gl_widget *GL_widget1,std::string Name1="Contrast-Bright parameters");
  _filter_contrast_bright_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Box_name="Contrast-Bright parameters");
  ~_filter_contrast_bright_ui();
  void reset_data();


  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);

  void parameter1(float Value){_filter_contrast_bright::contrast(Value);};
  float parameter1(){return _filter_contrast_bright::contrast();};

  void parameter2(int Value){_filter_contrast_bright::bright(Value);};  
  int parameter2(){return _filter_contrast_bright::bright();};

private:
  _qtw_filter_contrast_bright *Qtw_filter_contrast_bright;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_contrast_bright: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_contrast_bright(_gl_widget *GL_widget1,_filter_contrast_bright_ui *Filter1,std::string Box_name="Contrast-Bright parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(float Value);
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

  _filter_contrast_bright_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
