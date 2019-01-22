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


#ifndef _FILTER_CCVT_H
#define _FILTER_CCVT_H

#include <QDialog>
#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>
#include <QProgressDialog>
#include "line_edit.h"
#include <string>
#include "filter.h"

#define DEFINED_FILTER_CCVT

namespace _f_ccvt_ns
{
  // Number of dots
  // parameter 1
  const std::string String_group_box_parameter1("Number of dots");
  const std::string String_parameter1_tooltip("Controls the number of dots to be computed");

  // Number of points per dot
  // parameter 1
  const std::string String_group_box_parameter2("Number of points per dot");
  const std::string String_parameter2_tooltip("Controls the number of points (sites) per dot");

  // Default values
  const int CCVT_NUMBER_OF_DOTS_DEFAULT=1000;
  const int CCVT_MINIMUM_NUMBER_OF_DOTS_DEFAULT=10;
  const int CCVT_NUMBER_OF_POINTS_PER_DOT_DEFAULT=10;

  class _dot_data
  {
  public:

    bool operator<( const _dot_data& Dot_data1) const
    {
          return Value < Dot_data1.Value;
    };

    unsigned int x;
    unsigned int y;
    unsigned int Value;
  };
}

class _gl_widget;
class _qtw_filter_ccvt;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_ccvt : public _filter
{
public:
    _filter_ccvt();
    ~_filter_ccvt(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};

    void ccvt(cv::Mat *Input_image,cv::Mat *Output_image);
    void update();

    void number_of_dots(unsigned int Number_of_dots1){Number_of_dots=Number_of_dots1;};
    unsigned int number_of_dots(){return Number_of_dots;};

    void number_of_points_per_dot(unsigned int Number_of_points_per_dot1){Number_of_points_per_dot=Number_of_points_per_dot1;};
    unsigned int number_of_points_per_dot(){return Number_of_points_per_dot;};

    virtual void update_number_of_dots(){};
    virtual void update_number_of_points_per_dots(){};

private:
    unsigned int Number_of_dots;
    unsigned int Number_of_points_per_dot;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_ccvt_ui : public _filter_ccvt
{
public:
  _filter_ccvt_ui(_gl_widget *GL_widget1, std::string Name1="CCVT parameters");
  _filter_ccvt_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Name1="CCVT parameters");
  ~_filter_ccvt_ui();
  void reset_data();


  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);
  void get_parameters(std::map<std::string,std::string> &Parameters);

  void parameter1(int Value){number_of_dots((unsigned int)Value);};
  int parameter1(){return (int)number_of_dots();};

  void parameter2(int Value){number_of_points_per_dot((unsigned int)Value);};
  int parameter2(){return (int)number_of_points_per_dot();};

  void update_number_of_dots();
  void update_number_of_points_per_dots();

private:
  _qtw_filter_ccvt *Qtw_filter_ccvt;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_ccvt: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_ccvt(_gl_widget *GL_widget1,_filter_ccvt_ui *Filter1,std::string Box_name="CCVT parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);
  void set_parameter2(int Value);

protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);


private:
  QGroupBox *Group_box_main;
  QGroupBox *Group_box_parameter1;
  QGroupBox *Group_box_parameter2;

  // Number of dots
  QSpinBox *Spinbox_parameter1;

  // Number of points per dot
  QSpinBox *Spinbox_parameter2;

  _filter_ccvt_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
