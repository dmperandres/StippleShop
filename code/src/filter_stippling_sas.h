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


#ifndef _FILTER_STRUCTURE_AWARE_STIPPLING_H
#define _FILTER_STRUCTURE_AWARE_STIPPLING_H

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

#define DEFINED_FILTER_STRUCTURE_AWARE_STIPPLING

namespace _f_stippling_sas_ns
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

  // G+
  // parameter 3
  const std::string String_group_box_parameter3("G+");
  const std::string String_label_parameter3_min("1 ");
  const std::string String_label_parameter3_max("10 ");
  const std::string String_label_parameter3_tooltip("Display the G+ value");
  const int Parameter3_min_value=1;
  const int Parameter3_max_value=10;
  const int Parameter3_single_step=1;
  const int Parameter3_page_step=2;
  const int Parameter3_tick_interval=1;
  const bool Parameter3_set_tracking=false;
  const std::string String_parameter3_slider_tooltip("Controls the G+ value");

  // G-
  // parameter 4
  const std::string String_group_box_parameter4("G-");
  const std::string String_label_parameter4_min("1 ");
  const std::string String_label_parameter4_max("10 ");
  const std::string String_label_parameter4_tooltip("Display the G- value");
  const int Parameter4_min_value=1;
  const int Parameter4_max_value=10;
  const int Parameter4_single_step=1;
  const int Parameter4_page_step=2;
  const int Parameter4_tick_interval=1;
  const bool Parameter4_set_tracking=false;
  const std::string String_parameter4_slider_tooltip("Controls the G- value");

  // k value
  // parameter 5
  const std::string String_group_box_parameter5("k");
  const std::string String_label_parameter5_min("0 ");
  const std::string String_label_parameter5_max("1 ");
  const std::string String_label_parameter5_tooltip("Display the size of the gaussian filter");
  const int Parameter5_min_value=0;
  const int Parameter5_max_value=100;
  const int Parameter5_single_step=10;
  const int Parameter5_page_step=20;
  const int Parameter5_tick_interval=10;
  const bool Parameter5_set_tracking=false;
  const std::string String_parameter5_slider_tooltip("Controls the exponent");
  const float Parameter5_factor=100;

  // Default values
  const int SAS_KERNEL_SIZE_DEFAULT=7;
  const float SAS_EXPONENT_DEFAULT=2.0;
  const int SAS_G_PLUS_DEFAULT=5;
  const int SAS_G_MINUS_DEFAULT=5;
  const float SAS_K_DEFAULT=0;
  const float SAS_STIPPLE_SIZE_MIN=1;
  const float SAS_STIPPLE_SIZE_MAX=2;
}

class _gl_widget;
class _qtw_filter_stippling_sas;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_stippling_sas : public _filter
{
public:
    _filter_stippling_sas();
    ~_filter_stippling_sas(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};


    void stippling(cv::Mat *Input_image0,cv::Mat *Output_image0);
    void update();

    void kernel_size(int Kernel_size1){Kernel_size=Kernel_size1;};
    void exponent(float Exponent1){Exponent=Exponent1;};
    void g_plus(int G_plus1){G_plus=G_plus1;};
    void g_minus(int G_minus1){G_minus=G_minus1;};
    void k(float K1){K=K1;};

    int kernel_size(){return Kernel_size;};
    float exponent(){return Exponent;};
    int g_plus(){return G_plus;};
    int g_minus(){return G_minus;};
    float k(){return K;};

    int Kernel_size;
    float Exponent;
    int G_plus;
    int G_minus;
    float K;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_stippling_sas_ui :public _filter_stippling_sas
{
public:
  _filter_stippling_sas_ui(_gl_widget *GL_widget1,std::string Name1="Contrast-Aware Stippling parameters");
  _filter_stippling_sas_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Box_name="Contrast-Aware Stippling parameters");
  ~_filter_stippling_sas_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(int Value){_filter_stippling_sas::kernel_size(Value);};
  int parameter1(){return _filter_stippling_sas::kernel_size();};

  void parameter2(float Value){_filter_stippling_sas::exponent(Value);};
  float parameter2(){return _filter_stippling_sas::exponent();};

  void parameter3(int Value){_filter_stippling_sas::g_plus(Value);};
  int parameter3(){return _filter_stippling_sas::g_plus();};

  void parameter4(int Value){_filter_stippling_sas::g_minus(Value);};
  int parameter4(){return _filter_stippling_sas::g_minus();};

  void parameter5(float Value){_filter_stippling_sas::k(Value);};
  float parameter5(){return _filter_stippling_sas::k();};

  private:
  _qtw_filter_stippling_sas *Qtw_filter_stippling_sas;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_stippling_sas: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_stippling_sas(_gl_widget *GL_widget1,_filter_stippling_sas_ui *Filter1,std::string Box_name="Contrast-Aware Stippling parameters");
  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Size);
  void set_parameter2(float Value);
  void set_parameter3(int Size);
  void set_parameter4(int Size);
  void set_parameter5(float Value);


protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);
  void set_parameter3_slot(int Value);
  void set_parameter4_slot(int Value);
  void set_parameter5_slot(int Value);

private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;
  QGroupBox *Group_box_parameter4;
  QGroupBox *Group_box_parameter5;

  // Kernel size
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  // Exponent
  QSlider *Slider_parameter2;
  QLineEdit *Line_edit_parameter2;

  // G+
  QSlider *Slider_parameter3;
  QLineEdit *Line_edit_parameter3;

  // G-
  QSlider *Slider_parameter4;
  QLineEdit *Line_edit_parameter4;

  // k
  QSlider *Slider_parameter5;
  QLineEdit *Line_edit_parameter5;

  _filter_stippling_sas_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
