#ifndef _FILTER_DISTANCE_FIELD_H
#define _FILTER_DISTANCE_FIELD_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include "line_edit.h"
#include <string>
#include "filter.h"

#define DEFINED_FILTER_DISTANCE_FIELD

namespace _f_distance_field_ns
{
  // parameter 1
  const std::string String_group_box_parameter1("Line_width");
  const std::string String_label_parameter1_min("1 ");
  const std::string String_label_parameter1_max("10");
  const std::string String_label_parameter1_tooltip("Display the value of Canny size");
  const int Parameter1_min_value=1;
  const int Parameter1_max_value=10;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=1;
  const int Parameter1_tick_interval=1;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_tooltip("Controls the appearance of borders: more to the left, less to the right");

  // Default values
  const int DISTANCE_FIELD_LINE_WIDTH_DEFAULT=3;

  class _point
  {
  public:
    float Distance=-1;
    int x=-1;
    int y=-1;
  };
}

class _gl_widget;
class _qtw_filter_distance_field;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_distance_field : public _filter
{
public:
    _filter_distance_field();
    ~_filter_distance_field(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};

    void jump_flooding(cv::Mat *Input_image,cv::Mat *Output_image);
    void update();
    void line_width(int Line_width1){Line_width=Line_width1;}

    int line_width(){return Line_width;};

    int Line_width;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_distance_field_ui : public _filter_distance_field
{
public:
  _filter_distance_field_ui(_gl_widget *GL_widget1,std::string Name1="Distance field parameters");
  _filter_distance_field_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters, std::string Name1="Distance field parameters");
  ~_filter_distance_field_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);

  void parameter1(int Value){_filter_distance_field::line_width(Value);};
  int parameter1(){return _filter_distance_field::line_width();};

private:
  _qtw_filter_distance_field *Qtw_filter_distance_field;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_distance_field: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_distance_field(_gl_widget *GL_widget1,_filter_distance_field_ui *Filter1,std::string Box_name="Distance field parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);


protected slots:
  void set_parameter1_slot(int Value);

private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter1;

  // Line width
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  _filter_distance_field_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
