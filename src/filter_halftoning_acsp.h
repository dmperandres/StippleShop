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


#ifndef _FILTER_HALFTONING_ACSP_H
#define _FILTER_HALFTONING_ACSP_H

#ifdef X64
  #include <opencv.hpp>
#else
  #include <core/core.hpp>
  #include <highgui/highgui.hpp>
#endif

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include "line_edit.h"
#include <string>
#include <vector>
#include "filter.h"
#include "vertex.h"

#define DEFINED_FILTER_ADAPTIVE_CLUSTERING_SELECTIVE_PRECIPITATION

namespace _f_halftoning_acsp
{
  // parameter 1
  const std::string String_group_box_parameter1("Cluster size");
  const std::string String_label_parameter1_min("1 ");
  const std::string String_label_parameter1_max("256");
  const std::string String_label_parameter1_tooltip("Display the value of cluster size");
  const int Parameter1_min_value=1;
  const int Parameter1_max_value=256;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=10;
  const int Parameter1_tick_interval=15;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_tooltip("Controls the size of clusters");

  // parameter 2
  const std::string String_group_box_parameter2("Threshold");
  const std::string String_label_parameter2_min("0 ");
  const std::string String_label_parameter2_max("600");
  const std::string String_label_parameter2_tooltip("Display the value of threshold");
  const int Parameter2_min_value=0;
  const int Parameter2_max_value=600;
  const int Parameter2_single_step=1;
  const int Parameter2_page_step=10;
  const int Parameter2_tick_interval=25;
  const bool Parameter2_set_tracking=false;
  const std::string String_parameter2_slider_tooltip("Controls the threshold");

  //parameter3
  const std::string String_group_box_parameter3("Adaptive clustering");
  const std::string String_checkbox_parameter3("Adaptive clustering");
  const std::string String_parameter3_tooltip("Controls the activation of adaptive clustering");

  //parameter4
  const std::string String_group_box_parameter4("Selective precipitation");
  const std::string String_checkbox_parameter4("Selective precipitation");
  const std::string String_parameter4_tooltip("Controls the activation of selective precipitation");

  // Default values
  const int HALFTONING_ACSP_CLUSTER_SIZE_DEFAULT=9;
  const int HALFTONING_ACSP_THRESHOLD_DEFAULT=100;
  const bool HALFTONING_ACSP_SELECTIVE_PRECIPITATION_DEFAULT=true;
  const bool HALFTONING_ACSP_ADAPTIVE_CLUSTERING_DEFAULT=true;

  const std::string BOX_TEXT="Halftoning ACSP parameters";
  const unsigned char BLACK=255;
  const unsigned char WHITE=0;
  const unsigned char LEFT=0;
  const unsigned char RIGHT=1;
  const unsigned char UP=2;
  const unsigned char DOWN=3;
  const unsigned char END=255;
}

class _gl_widget;
class _qtw_filter_halftoning_acsp;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_halftoning_acsp : public _filter
{
public:
  _filter_halftoning_acsp();
  ~_filter_halftoning_acsp(){};
  void reset_data();
  bool change_output_image_size(){return Change_output_image_size;};
  bool use_dots(){return Use_dots;};

  void update();

  void cluster_size(int Cluster_size1){Cluster_size=Cluster_size1;}
  int  cluster_size(){return Cluster_size;};

  void threshold(int Threshold1){Threshold=Threshold1;}
  int  threshold(){return Threshold;};

  void adaptive_clustering(bool Adaptive_clustering1){Adaptive_clustering=Adaptive_clustering1;};
  bool adaptive_clustering(){return Adaptive_clustering;};

  void selective_precipitation(bool Selective_precipitation1){Selective_precipitation=Selective_precipitation1;};
  bool selective_precipitation(){return Selective_precipitation;};

  void step(int &Angle);
  void hilbert(int Orient,int &Angle, int Level);
  void generate_space_fill();
  void space_fill(cv::Mat *Input_image0,cv::Mat *Output_image0);
  void move(int &x,int &y);

  cv::Mat Path;

  int Path_x,Path_y;
  int Width_image;
  int Level;
  int Cluster_size;
  int Threshold;
  bool Adaptive_clustering;
  bool Selective_precipitation;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_halftoning_acsp_ui : public _filter_halftoning_acsp
{
public:
  _filter_halftoning_acsp_ui(_gl_widget *GL_widget1, std::string Name1="Halftonning ACSP parameters");
  _filter_halftoning_acsp_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Name1="Halftonning ACSP parameters");
  ~_filter_halftoning_acsp_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);
  void get_parameters(std::map<std::string,std::string> &Parameters);

  void parameter1(int Value){_filter_halftoning_acsp::cluster_size(Value);};
  int parameter1(){return _filter_halftoning_acsp::cluster_size();};

  void parameter2(int Value){_filter_halftoning_acsp::threshold(Value);};
  int parameter2(){return _filter_halftoning_acsp::threshold();};

  void parameter3(bool Value){_filter_halftoning_acsp::adaptive_clustering(Value);};
  bool parameter3(){return _filter_halftoning_acsp::adaptive_clustering();};

  void parameter4(bool Value){_filter_halftoning_acsp::selective_precipitation(Value);};
  bool parameter4(){return _filter_halftoning_acsp::selective_precipitation();};

private:
  _qtw_filter_halftoning_acsp *Qtw_filter_halftoning_acsp;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_halftoning_acsp: public QWidget
{
  Q_OBJECT
public:
  _qtw_filter_halftoning_acsp(_gl_widget *GL_widget1,_filter_halftoning_acsp_ui *Filter1,std::string Box_name="Halftonning ACSP parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);
  void set_parameter2(int Value);
  void set_parameter3(bool Value);
  void set_parameter4(bool Value);

protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);
  void set_parameter3_slot(int Value);
  void set_parameter4_slot(int Value);

private:
  QGroupBox *Group_box_main;
  QGroupBox *Group_box_parameter1;
  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;
  QGroupBox *Group_box_parameter4;

  // cluster size
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  // threshold
  QSlider *Slider_parameter2;
  QLineEdit *Line_edit_parameter2;

  // adaptive clustering
  QCheckBox *Checkbox_parameter3;

  // selective precipitation
  QCheckBox *Checkbox_parameter4;

  _filter_halftoning_acsp_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
