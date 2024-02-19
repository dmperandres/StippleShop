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


#ifndef _FILTER_DOT_EBG_H
#define _FILTER_DOT_EBG_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include "line_edit.h"
#include <string>
#include "filter.h"
#include "random.h"

#define DEFINED_FILTER_EXAMPLE_BASED_STIPPLING

namespace _f_dot_ebg_ns
{
  // pixel_density
  // parameter 1
  const std::string String_group_box_parameter1("Pixel density");
  const std::string String_combo_parameter1("Pixel density");
  const std::string String_parameter1_tooltip("This value allows to change the output pixel_density\n300ppi: mode to print in low pixel_density mono printers and for on-screen files\n600ppi: mode to print in medium pixel_density mono printers\n1200ppi: mode to print in high pixel_density mono printers");

  // modulate dot size
  // parameter 2
  const std::string String_group_box_parameter2("Modulate dot size");
  const std::string String_checkbox_parameter2("Modulate dot size");
  const std::string String_parameter2_tooltip("Controls if the size is modulated by the tone of the image");

  // Black dots
  // parameter 3
  const std::string String_group_box_parameter3("Black dots");
  const std::string String_checkbox_parameter3("Black dots");
  const std::string String_parameter3_tooltip("Controls if the size is modulated by the tone of the image");

  // Black threshold
  // parameter 4
  const std::string String_group_box_parameter4("Black threshold");
  const std::string String_label_parameter4_min("0 ");
  const std::string String_label_parameter4_max("255");
  const std::string String_label_parameter4_tooltip("Display the value of threshold for black level");
  const int Parameter4_min_value=0;
  const int Parameter4_max_value=255;
  const int Parameter4_single_step=1;
  const int Parameter4_page_step=10;
  const int Parameter4_tick_interval=15;
  const bool Parameter4_set_tracking=false;
  const std::string String_parameter4_slider_tooltip("Controls the level to get black color");

  // info 1
  const std::string String_group_box_info1("Number of dots");

  // Default values
  const int BLACK=0.;
  const int WHITE=255.;

  typedef enum {PIXEL_DENSITY_300PPI,PIXEL_DENSITY_600PPI,PIXEL_DENSITY_1200PPI,LAST_PIXEL_DENSITY} _pixel_density;

  const int VEC_PIXEL_DENSITY[3]={300,600,1200};
  const int VEC_PIXEL_DENSITY_FACTOR[3]={1,2,4};

  const unsigned short MIN_DOT_SIZE=4;
  const unsigned short MAX_DOT_SIZE=8;

  const int AVERAGE_DOT_SIZE=(MAX_DOT_SIZE+MIN_DOT_SIZE)/2;

  const unsigned short NUM_ROWS_DOT_MATRIX=4;
  const unsigned short NUM_COLS_DOT_MATRIX=10;

  const float DOT_EBG_SCALING_FACTOR_DEFAULT=1; //3.0 for ebs, 1 for normal, 4 for 1200ppi
  const float DOT_EBG_DISPLACEMENT_DEFAULT=0;// 0.25
  const int DOT_EBG_NUM_ROWS_DOT_MATRIX_DEFAULT=NUM_ROWS_DOT_MATRIX;
  const int DOT_EBG_NUM_COLS_DOT_MATRIX_DEFAULT=NUM_COLS_DOT_MATRIX;
  const int DOT_EBG_MIN_DOT_SIZE_DEFAULT=MIN_DOT_SIZE;
  const int DOT_EBG_MAX_DOT_SIZE_DEFAULT=MAX_DOT_SIZE;

  const _pixel_density DOT_EBG_PIXEL_DENSITY_DEFAULT=PIXEL_DENSITY_300PPI;
  const bool DOT_EBG_MODULATE_DOT_SIZE_DEFAULT=false;
  const bool DOT_EBG_BLACK_DOTS_DEFAULT=false;
  const unsigned int DOT_EBG_BLACK_THRESHOLD_DEFAULT=220;

  const bool DOT_EBG_CHANGE_OUTPUT_IMAGE_SIZE_DEFAULT=true;

  #ifdef WINDOWS
  const double M_PI = 3.1415926535897932;
  #endif
}


class _gl_widget;
class _qtw_filter_dot_ebg;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_dot_ebg : public _filter
{
public:
  _filter_dot_ebg();
  ~_filter_dot_ebg(){};
  void reset_data();
  void update();

  bool change_output_image_size(){return Change_output_image_size;};
  bool use_dots(){return Use_dots;};

  void pixel_density(_f_dot_ebg_ns::_pixel_density Resolution1);
  _f_dot_ebg_ns::_pixel_density pixel_density(){return Pixel_density;};

  void modulate_dot_size(bool Modulate_dot_size1){Modulate_dot_size=Modulate_dot_size1;};
  bool modulate_dot_size(){return Modulate_dot_size;};

  void black_dots(bool Black_dots1){Black_dots=Black_dots1;};
  bool black_dots(){return Black_dots;};

  void black_threshold(unsigned int Black_threshold1){Black_threshold=Black_threshold1;};
  unsigned int black_threshold(){return Black_threshold;};

  void set_dots_images(std::vector<std::vector<cv::Mat *>> *Dots);
  void set_dots_texture_packet(int Dots_texture_packet1){Dots_texture_packet=Dots_texture_packet1;}

  void put_dot(cv::Mat *Output_image1, float Row1, float Col1, unsigned int Selected_dot_size1, unsigned int Index_row1, unsigned int Index_col1);
  void stippling(cv::Mat *Input_image0, cv::Mat *Input_image1, cv::Mat *Output_image0);

  void stippling_svg(QPainter &Painter);

  void save_seeds();
  void load_seeds();

  virtual void set_info1(unsigned int Value){Q_UNUSED(Value)};

  bool Modulate_dot_size;
  bool Black_dots;
  unsigned int Black_threshold;

  int Num_rows_dot_matrix,Num_cols_dot_matrix;
  int Dot_size_min,Dot_size_max;
  int Dots_texture_packet;
  int Pixel_density_factor;
  std::vector<std::vector<cv::Mat *>> *Dots;

  _random_uniform_int Index_row,Index_col;
  _random_uniform_int Dot_size;
  _f_dot_ebg_ns::_pixel_density Pixel_density;

  bool Seeds_initialized;

  unsigned int Counter_of_dots;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_dot_ebg_ui : public _filter_dot_ebg
{
public:
  _filter_dot_ebg_ui(_gl_widget *GL_widget1,std::string Name1="Dot EBS parameters");
  _filter_dot_ebg_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Box_name="Dot EBS parameters");
  ~_filter_dot_ebg_ui();
  void reset_data();


  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(int Resolution1){pixel_density((_f_dot_ebg_ns::_pixel_density) Resolution1);};
  int parameter1(){return (int)pixel_density();};

  void parameter2(bool Value){modulate_dot_size(Value);};
  bool parameter2(){return modulate_dot_size();};

  void parameter3(bool Value){black_dots(Value);};
  bool parameter3(){return black_dots();};

  void parameter4(unsigned int Value){black_threshold(Value);};
  unsigned int parameter4(){return black_threshold();};

  void set_info1(unsigned int Value1);

private:
  _qtw_filter_dot_ebg *Qtw_filter_dot_ebg;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_dot_ebg: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_dot_ebg(_gl_widget *GL_widget1,_filter_dot_ebg_ui *Filter1,std::string Box_name="Dot EBS parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);
  void set_parameter2(bool Value);
  void set_parameter3(bool Value);
  void set_parameter4(unsigned int Value);
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
  // info
  QGroupBox *Group_box_info1;

  //
  QComboBox *Combo_parameter1;
  // Modulate_dot_size
  QCheckBox *Checkbox_parameter2;

  // Black dots
  QCheckBox *Checkbox_parameter3;

  // black threshold
  QSlider *Slider_parameter4;
  QLineEdit *Line_edit_parameter4;

  // number of dots
   QLineEdit *Line_edit_info1;

  _filter_dot_ebg_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
