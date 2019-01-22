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


#ifndef _FILTER_INVERSION_H
#define _FILTER_INVERSION_H

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

#define DEFINED_FILTER_INVERSION

class _gl_widget;
class _qtw_filter_inversion;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_inversion : public _filter
{
public:
    _filter_inversion();
    ~_filter_inversion(){};
    void reset_data(){};
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};

    void update();
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_inversion_ui :public _filter_inversion
{
public:
  _filter_inversion_ui(_gl_widget *GL_widget1,std::string Name1="Inversion parameters");
  _filter_inversion_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Name1="Inversion parameters");
  ~_filter_inversion_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string, std::string> &Parameters);

  private:
  _qtw_filter_inversion *Qtw_filter_inversion;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_inversion: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_inversion(_gl_widget *GL_widget1,_filter_inversion_ui *Filter1,std::string Box_name="Inversion parameters");
  void show(){};
  void hide(){};
  QGroupBox *get_link(){return Group_box_main;};

protected slots:

private:
  QGroupBox *Group_box_main;

  _filter_inversion_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
