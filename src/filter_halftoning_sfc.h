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


#ifndef _FILTER_HALFTONING_SFC_H
#define _FILTER_HALFTONING_SFC_H

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
#include "line_edit.h"
#include <string>
#include <vector>
#include "filter.h"
#include "vertex.h"

#define DEFINED_FILTER_SPACE_FILLING_CURVE

namespace _f_halftoning_sfc_ns
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

  // Default values
  const int HALFTONING_SFC_CLUSTER_SIZE_DEFAULT=9;

  const std::string BOX_TEXT="Halftoning SFC parameters";

  const unsigned char BLACK=255;
  const unsigned char WHITE=0;
  const unsigned char LEFT=0;
  const unsigned char RIGHT=1;
  const unsigned char UP=2;
  const unsigned char DOWN=3;
  const unsigned char END=255;
}

class _gl_widget;
class _qtw_filter_halftoning_sfc;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_halftoning_sfc : public _filter
{
public:
    _filter_halftoning_sfc();
    ~_filter_halftoning_sfc(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};

    void update();
    void cluster_size(int Cluster_size1){Cluster_size=Cluster_size1;}
    int  cluster_size(){return Cluster_size;};

    void step(int &Angle);
    void hilbert(int Orient,int &Angle, int Level);
    void generate_space_fill();
    void space_fill(cv::Mat *Input_image0,cv::Mat *Output_image0);

    cv::Mat Path;

    int Path_x,Path_y;
    int Width_image;
    int Level;
    int Cluster_size;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_halftoning_sfc_ui : public _filter_halftoning_sfc
{
public:
  _filter_halftoning_sfc_ui(_gl_widget *GL_widget1, std::string Name1="Halftonning SFC parameters");
  _filter_halftoning_sfc_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Name1="Halftonning SFC parameters");
  ~_filter_halftoning_sfc_ui();
  void reset_data();


  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);
  void get_parameters(std::map<std::string,std::string> &Parameters);

  void parameter1(int Value){_filter_halftoning_sfc::cluster_size(Value);};
  int parameter1(){return _filter_halftoning_sfc::cluster_size();};


private:
  _qtw_filter_halftoning_sfc *Qtw_filter_halftoning_sfc;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_halftoning_sfc: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_halftoning_sfc(_gl_widget *GL_widget1,_filter_halftoning_sfc_ui *Filter1,std::string Box_name="Halftonning SFC parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);

protected slots:
  void set_parameter1_slot(int Value);

private:
  QGroupBox *Group_box_main;
  QGroupBox *Group_box_parameter1;

  // cluster size
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  _filter_halftoning_sfc_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
