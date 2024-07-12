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


#ifndef _FILTER_STIPPLING_EBG_H
#define _FILTER_STIPPLING_EBG_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include "line_edit.h"
#include <string>
#include "filter.h"
#include "random.h"

#define DEFINED_FILTER_EXAMPLE_GRAYSCALE_BASED_STIPPLING

namespace _f_stippling_ebg_ns
{
  // parameter 1: pixel density
  const std::string String_group_box_parameter1("Pixel density");

  // parameter 2
  const std::string String_group_box_parameter2("Displacement ramdomness");
  const std::string String_label_parameter2_min("0 ");
  const std::string String_label_parameter2_max("1");
  const std::string String_label_parameter2_tooltip("Display the value of displacement ramdomness");
  const int Parameter2_min_value=0;
  const int Parameter2_max_value=100;
  const int Parameter2_single_step=1;
  const int Parameter2_page_step=5;
  const int Parameter2_tick_interval=10;
  const bool Parameter2_set_tracking=false;
  const std::string String_parameter2_tooltip("Controls the ramdom displacement of dots");
  // warning: the capture value must be adjusted to a odd value
  const int Parameter2_factor=100;

  // parameter 3: output mode
  const std::string String_group_box_parameter3("Output mode");

  // parameter 4: Black level
  const std::string String_group_box_parameter4("Black level");
  const std::string String_label_parameter4_min("0  ");
  const std::string String_label_parameter4_max("255");
  const std::string String_label_parameter4_tooltip("Display the value of the black level");
  const int parameter4_min_value=0;
  const int parameter4_max_value=255;
  const int parameter4_single_step=1;
  const int parameter4_page_step=5;
  const int parameter4_tick_interval=10;
  const bool parameter4_set_tracking=false;
  const std::string String_parameter4_tooltip("Controls the black level used as a threshold");

  // info 1
  const std::string String_group_box_info1("Number of dots");

  // Default values
  const int BLACK=0.;
  const int WHITE=255.;

  typedef enum {PIXEL_DENSITY_300PPI,PIXEL_DENSITY_600PPI,PIXEL_DENSITY_1200PPI,LAST_RESOLUTION} _pixel_density;
  typedef enum {OUTPUT_MODE_GRAYSCALE,OUTPUT_MODE_MONO_COLOR,OUTPUT_MODE_LAST} _output_mode;

  const int VEC_PIXEL_DENSITY[3]={300,600,1200};
  const int VEC_PIXEL_DENSITY_FACTOR[3]={1,2,4};

  const unsigned short MIN_DOT_SIZE=4;
  const unsigned short MAX_DOT_SIZE=8;

  const int AVERAGE_DOT_SIZE=(MAX_DOT_SIZE+MIN_DOT_SIZE)/2;

  const unsigned short NUM_ROWS_DOT_MATRIX=4;
  const unsigned short NUM_COLS_DOT_MATRIX=10;

  const float STIPPLING_EBG_SCALING_FACTOR_DEFAULT=3.0;
  const float STIPPLING_EBG_DISPLACEMENT_DEFAULT=0.25;
  const int STIPPLING_EBG_NUM_ROWS_DOT_MATRIX_DEFAULT=NUM_ROWS_DOT_MATRIX;
  const int STIPPLING_EBG_NUM_COLS_DOT_MATRIX_DEFAULT=NUM_COLS_DOT_MATRIX;
  const int STIPPLING_EBG_MIN_DOT_SIZE_DEFAULT=MIN_DOT_SIZE;
  const int STIPPLING_EBG_MAX_DOT_SIZE_DEFAULT=MAX_DOT_SIZE;
  const int STIPPLING_EBG_PIXEL_DENSITY_DEFAULT=(int) PIXEL_DENSITY_300PPI;
  const _output_mode STIPPLING_EBG_OUTPUT_MODE_DEFAULT=OUTPUT_MODE_GRAYSCALE;
  const int STIPPLING_EBG_BLACK_LEVEL_DEFAULT=200;
}


class _gl_widget;
class _qtw_filter_stippling_ebg;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_stippling_ebg : public _filter
{
public:
  _filter_stippling_ebg();
  ~_filter_stippling_ebg(){};
  void reset_data();
  void update();
  bool change_output_image_size(){return Change_output_image_size;};
  bool use_dots(){return Use_dots;};

  void output_image_size(int &Width1,int &Height1);

  void pixel_density(int Pixel_density1);
  int pixel_density(){return Pixel_density;};

  void displacement_ramdomness(float Displacement_ramdomness1);
  float displacement_ramdomness(){return Displacement_ramdomness;};

  void output_mode(int Output_mode1){Output_mode=Output_mode1;};
  int output_mode(){return Output_mode;};

  void black_level(int Black_level1){Black_level=Black_level1;};
  int black_level(){return Black_level;};

  void set_dots_images(std::vector<std::vector<cv::Mat *>> *Dots);
  void set_dots_texture_packet(int Dots_texture_packet1){Dots_texture_packet=Dots_texture_packet1;}

  void put_dot(cv::Mat *Output_image1, float Row1, float Col1, unsigned int Selected_dot_size1, unsigned int Index_row1, unsigned int Index_col1);
  void stippling(cv::Mat *Input_image0, cv::Mat *Output_image0);

  void save_seeds();
  void load_seeds();

  virtual void set_info1(unsigned int Value){Q_UNUSED(Value)};

  float Displacement_ramdomness;
  int Num_rows_dot_matrix,Num_cols_dot_matrix;
  int Dot_size_min,Dot_size_max;
  int Dots_texture_packet;
  int Pixel_density;
  int Pixel_density_factor;
  int Output_mode;
  int Black_level;

  std::vector<std::vector<cv::Mat *>> *Dots;

  _random_uniform_int Index_row,Index_col;
  _random_uniform_int Dot_size;
  _random_uniform_double Displacement;

  bool Seeds_initialized;

  unsigned int Counter_of_dots;

protected:
  void adjust_image_sizes(int &Width1, int &Height1);
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_stippling_ebg_ui : public _filter_stippling_ebg
{
public:
  _filter_stippling_ebg_ui(_gl_widget *GL_widget1,std::string Name1="Stippling EBS parameters");
  _filter_stippling_ebg_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Box_name="Stippling EBS parameters");
  ~_filter_stippling_ebg_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(int Value){pixel_density(Value);};
  int parameter1(){return pixel_density();};

  void parameter2(float Value){_filter_stippling_ebg::displacement_ramdomness(Value);};
  float parameter2(){return _filter_stippling_ebg::displacement_ramdomness();};

  void parameter3(int Value){output_mode(Value);};
  int parameter3(){return (int) output_mode();};

  void parameter4(int Value){black_level(Value);};
  int parameter4(){return black_level();};

  void set_info1(unsigned int Value1);

private:
  _qtw_filter_stippling_ebg *Qtw_filter_stippling_ebg;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_stippling_ebg: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_stippling_ebg(_gl_widget *GL_widget1,_filter_stippling_ebg_ui *Filter1,std::string Box_name="Stippling EBS parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);
  void set_parameter2(float Value);
  void set_parameter3(int Value);
  void set_parameter4(int Value);

  void set_info1(unsigned int Value);

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

  QGroupBox *Group_box_info1;

  // output resolution
  QComboBox *Combo_box_parameter1;

  // Displacement ramdomness
  QSlider *Slider_parameter2;
  QLineEdit *Line_edit_parameter2;

  // output mode
  QComboBox *Combo_box_parameter3;

  // black level
  QLineEdit *Line_edit_parameter4;
  QSlider *Slider_parameter4;

  // number of dots
   QLineEdit *Line_edit_info1;

  _filter_stippling_ebg_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
