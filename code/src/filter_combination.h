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


#ifndef _FILTER_COMBINATION_H
#define _FILTER_COMBINATION_H

#include <QDialog>
#include <QGroupBox>
#include <QComboBox>

#include <opencv.hpp>

#include "line_edit.h"
#include <string>
#include "filter.h"

#define DEFINED_FILTER_COMBINATION

namespace _f_combination_ns
{
  typedef enum {COMBINATION_PRODUCT,COMBINATION_ADD,COMBINATION_AND,COMBINATION_OR,COMBINATION_SUB,COMBINATION_DIF,COMBINATION_LAST} _combination_type;

  // parameter 1
  const std::string Parameter1_text[COMBINATION_LAST]={"Product","Add","And","Or","Sub","Dif"};

  const std::string String_group_box_parameter1("Operation");
  const std::string String_label_parameter1_min("0 ");
  const std::string String_label_parameter1_max("255");
  const std::string String_label_parameter1_tooltip("Display the operation");
  const int Parameter1_min_value=0;
  const int Parameter1_max_value=255;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=10;
  const int Parameter1_tick_interval=15;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_tooltip("Controls the appearance of borders: more to the left, less to the right");

  static std::map<_combination_type,std::string> Operations={{COMBINATION_PRODUCT,"product"},{COMBINATION_ADD,"add"},{COMBINATION_AND,"and"},{COMBINATION_OR,"or"},{COMBINATION_SUB,"sub"},{COMBINATION_DIF,"dif"}};

  static std::map<std::string,_combination_type> Operations_inv={{"product",COMBINATION_PRODUCT},{"add",COMBINATION_ADD},{"and",COMBINATION_AND},{"or",COMBINATION_OR},{"sub",COMBINATION_SUB},{"dif",COMBINATION_DIF}};
}

class _gl_widget;
class _qtw_filter_combination;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_combination : public _filter
{
public:
    _filter_combination();
    ~_filter_combination(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};


    void update();

    void combination_type(_f_combination_ns::_combination_type Combination_type1){Combination_type=Combination_type1;};
    _f_combination_ns::_combination_type combination_type(){return Combination_type;};

    _f_combination_ns::_combination_type Combination_type;
    bool Invert_input_image_0;
    bool Invert_input_image_1;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_combination_ui :public _filter_combination
{
public:
  _filter_combination_ui(_gl_widget *GL_widget1,std::string Name1="Combination parameters");
  _filter_combination_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Name1="Combination parameters");
  ~_filter_combination_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  void parameter1(_f_combination_ns:: _combination_type Value){_filter_combination::combination_type(Value);};
  _f_combination_ns::_combination_type parameter1(){return _filter_combination::combination_type();};

  private:
  _qtw_filter_combination *Qtw_filter_combination;

};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_combination: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_combination(_gl_widget *GL_widget1,_filter_combination_ui *Filter1,std::string Box_name="Combination parameters");
  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(_f_combination_ns::_combination_type Value);

protected slots:
  void set_parameter1_slot(int Value);

private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter1;

  // Combo
  QComboBox *Combo_parameter1;

  _filter_combination_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
