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


#ifndef _FILTER_DOT_SVG_H
#define _FILTER_DOT_SVG_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QPainter>
#include <QSvgGenerator>
#include "line_edit.h"
#include <string>
#include "filter.h"
#include "random.h"

#define DEFINED_FILTER_EXAMPLE_BASED_STIPPLING

namespace _f_dot_svg_ns
{
  // resolution
  // parameter 1
  const std::string String_group_box_parameter1("Dot type (For SVG saving)");
  const std::string String_combo_parameter1("Dot type");
  const std::string String_parameter1_tooltip("This value allows to change the type of dot for SVG saving");

  // modulate dot size
  // parameter 2
  const std::string String_group_box_parameter2("Modulate dot size");
  const std::string String_checkbox_parameter2("Modulate dot size");
  const std::string String_parameter2_tooltip("Controls if the size is modulated by the tone of the image");

  // antialias
  // parameter 3
  const std::string String_group_box_parameter3("Antialias");
  const std::string String_checkbox_parameter3("Antialias");
  const std::string String_parameter3_tooltip("Controls the antialias");

  // Min_dot_size
  //parameter4
  const std::string String_group_box_parameter4("Min dot size");
  const std::string String_label_parameter4_min("1 ");
  const std::string String_label_parameter4_max("30");
  const std::string String_label_parameter4_tooltip("Display the value of the min dot size");
  const int Parameter4_min_value=1;
  const int Parameter4_max_value=30;
  const int Parameter4_single_step=1;
  const int Parameter4_page_step=2;
  const int Parameter4_tick_interval=1;
  const bool Parameter4_set_tracking=false;
  const std::string String_parameter4_slider_tooltip("Controls the min size of dots");

  // Max_dot_size
  //parameter5
  const std::string String_group_box_parameter5("Max dot size");
  const std::string String_label_parameter5_min("1 ");
  const std::string String_label_parameter5_max("30");
  const std::string String_label_parameter5_tooltip("Display the value of the max dot size");
  const int Parameter5_min_value=1;
  const int Parameter5_max_value=30;
  const int Parameter5_single_step=1;
  const int Parameter5_page_step=2;
  const int Parameter5_tick_interval=1;
  const bool Parameter5_set_tracking=false;
  const std::string String_parameter5_slider_tooltip("Controls the max size of dots");

  // info 1
  const std::string String_group_box_info1("Number of dots");

  // Default values
  const int BLACK=0.;
  const int WHITE=255.;

  typedef enum {DOT_TYPE_CIRCLE,DOT_TYPE_STAR,DOT_TYPE_AT,LAST_RESOLUTION} _dot_type;

  const int VEC_RESOLUTION[3]={300,600,1200};
  //const int VEC_RESOLUTION_FACTOR[3]={1,2,4};
  const int VEC_RESOLUTION_FACTOR[3]={1,2,4};

  const unsigned short MIN_DOT_SIZE=1;
  const unsigned short MAX_DOT_SIZE=10;
  const int DOT_SVG_MIN_DOT_SIZE_DEFAULT=1;
  const int DOT_SVG_MAX_DOT_SIZE_DEFAULT=3;

  const float DOT_SVG_SCALING_FACTOR_DEFAULT=1; //3.0 for ebs, 1 for normal, 4 for 1200ppi
  const float DOT_SVG_DISPLACEMENT_DEFAULT=0;// 0.25
  const _dot_type DOT_SVG_DOT_TYPE_DEFAULT=DOT_TYPE_CIRCLE;

  const unsigned int DOT_SVG_BLACK_LEVEL_DEFAULT=220;

  const bool DOT_SVG_MODULATE_DOT_SIZE_DEFAULT=false;
  const bool DOT_SVG_ANTIALIAS_DEFAULT=false;

  const bool DOT_SVG_CHANGE_OUTPUT_IMAGE_SIZE_DEFAULT=true;

#ifdef WIN_COMPILER
  const double M_PI = 3.1415926535897932;
#endif
}


class _gl_widget;
class _qtw_filter_dot_svg;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_dot_svg : public _filter
{
public:
  _filter_dot_svg();
  ~_filter_dot_svg(){};
  void reset_data();
  void update();

  bool change_output_image_size(){return Change_output_image_size;};
  bool use_dots(){return Use_dots;};

  void dot_type(_f_dot_svg_ns::_dot_type Dot_type1){Dot_type=Dot_type1;}
  _f_dot_svg_ns::_dot_type dot_type(){return Dot_type;};

  void modulate_dot_size(bool Modulate_dot_size1){Modulate_dot_size=Modulate_dot_size1;};
  bool modulate_dot_size(){return Modulate_dot_size;};

  void antialias(bool Antialias1){Antialias=Antialias1;};
  bool antialias(){return Antialias;};

  void min_dot_size(int Min_dot_size1){Min_dot_size=Min_dot_size1;};
  int min_dot_size(){return Min_dot_size;};

  void max_dot_size(int Max_dot_size1){Max_dot_size=Max_dot_size1;};
  int max_dot_size(){return Max_dot_size;};

  void set_dots_images(std::vector<std::vector<cv::Mat *>> *Dots);
  void set_dots_texture_packet(int Dots_texture_packet1){Dots_texture_packet=Dots_texture_packet1;}

  void put_dot(cv::Mat *Output_image1, float Row1, float Col1, unsigned int Selected_dot_size1, unsigned int Index_row1, unsigned int Index_col1);
  void stippling(cv::Mat *Input_image0, cv::Mat *Input_image1, cv::Mat *Output_image0);

  void stippling_svg(QPainter &Painter);
  void save_svg(QString File_name1);

  void save_seeds();
  void load_seeds();

  virtual void set_info1(unsigned int Value){Q_UNUSED(Value)};

  bool Modulate_dot_size;
  bool Antialias;
  int Num_rows_dot_matrix,Num_cols_dot_matrix;
  int Dots_texture_packet;

  int Min_dot_size;
  int Max_dot_size;
  std::vector<std::vector<cv::Mat *>> *Dots;

  _random_uniform_int Index_row,Index_col;
  _random_uniform_int Dot_size;
  _f_dot_svg_ns::_dot_type Dot_type;

  bool Seeds_initialized;

  unsigned int Counter_of_dots;

  bool Local_change;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_dot_svg_ui : public _filter_dot_svg
{
public:
  _filter_dot_svg_ui(_gl_widget *GL_widget1,std::string Name1="Dot SVG parameters");
  _filter_dot_svg_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Box_name="Dot SVG parameters");
  ~_filter_dot_svg_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(int Dot_type1){dot_type((_f_dot_svg_ns::_dot_type) Dot_type1);};
  int parameter1(){return (int)dot_type();};

  void parameter2(bool Value){modulate_dot_size(Value);};
  bool parameter2(){return modulate_dot_size();};

  void parameter3(bool Value){antialias(Value);};
  bool parameter3(){return antialias();};

  void parameter4(int Value){min_dot_size(Value);};
  int parameter4(){return min_dot_size();};

  void parameter5(int Value){max_dot_size(Value);};
  int parameter5(){return max_dot_size();};

  void set_info1(unsigned int Value1);

  void local_change(bool Value){Local_change=Value;};

private:
  _qtw_filter_dot_svg *Qtw_filter_dot_svg;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_dot_svg: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_dot_svg(_gl_widget *GL_widget1,_filter_dot_svg_ui *Filter1,std::string Box_name="Dot SVG parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};


  void set_parameter1(int Value);
  void set_parameter2(bool Value);
  void set_parameter3(bool Value);
  void set_parameter4(int Value);
  void set_parameter5(int Value);
  void set_info1(unsigned int Value);

protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);
  void set_parameter3_slot(int Value);
  void set_parameter4_slot(int Value);
  void set_parameter5_slot(int Value);
  void set_parameter6_slot();


private:
  QGroupBox *Group_box_main;
  QGroupBox *Group_box_parameter1;
  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;
  QGroupBox *Group_box_parameter4;
  QGroupBox *Group_box_parameter5;
  // info
  QGroupBox *Group_box_info1;
  //
  QComboBox *Combo_parameter1;
  // Modulate_dot_size
  QCheckBox *Checkbox_parameter2;

  // Antialias
  QCheckBox *Checkbox_parameter3;

  // min_dot_size
  QSlider *Slider_parameter4;
  QLineEdit *Line_edit_parameter4;

  // max_dot_size
  QSlider *Slider_parameter5;
  QLineEdit *Line_edit_parameter5;

  // number of dots
   QLineEdit *Line_edit_info1;

  _filter_dot_svg_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
