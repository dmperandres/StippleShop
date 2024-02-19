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


#ifndef _FILTER_MEASURE_SSIM_PSNR_H
#define _FILTER_MEASURE_SSIM_PSNR_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include "line_edit.h"
#include <string>
#include "filter.h"

#define DEFINED_FILTER_MEASURE_SSIM_PSNR

namespace _f_measure_ssim_psnr_ns
{
  // SSIM
  const std::string String_group_box_info1("MSSIM (%)");
  // PSNR
  const std::string String_group_box_info2("PSNR (dB)");
}

class _gl_widget;
class _qtw_filter_measure_ssim_psnr;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_measure_ssim_psnr : public _filter
{
public:
    _filter_measure_ssim_psnr();
    ~_filter_measure_ssim_psnr(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};

    void update();

    cv::Scalar getMSSIM(cv::Mat *i1,cv::Mat *i2);
    double getPSNR(cv::Mat *I1,cv::Mat *I2);

    virtual void set_info1(float SSIM1,float PSNR1){Q_UNUSED(SSIM1) Q_UNUSED(PSNR1)};
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_measure_ssim_psnr_ui : public _filter_measure_ssim_psnr
{
public:
  _filter_measure_ssim_psnr_ui(_gl_widget *GL_widget1,std::string Name1="SSIM-PSNR parameters");
  _filter_measure_ssim_psnr_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters, std::string Box_name="SSIM-PSNR parameters");
  ~_filter_measure_ssim_psnr_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);

  void set_info1(float SSIM1,float PSNR1);

private:
  _qtw_filter_measure_ssim_psnr *Qtw_filter_measure_ssim_psnr;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_measure_ssim_psnr: public QWidget
{
  Q_OBJECT

public:
  _qtw_filter_measure_ssim_psnr(_gl_widget *GL_widget1,_filter_measure_ssim_psnr_ui *Filter1,std::string Box_name="SSIM-PSNR parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_info1(float SSIM1,float PSNR1);

private:
  QGroupBox *Group_box_main;

  // SSIM
  QLineEdit *Line_edit_info1;
  // PSNR
  QLineEdit *Line_edit_info2;

  _filter_measure_ssim_psnr_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
