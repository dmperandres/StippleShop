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


#ifndef GLWIDGET
#define GLWIDGET

#include <GL/glew.h>
#include <QOpenGLWidget>
#include <QMatrix4x4>

#include <opencv.hpp>

#include <math.h>
#include <string>
#include <map>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <sys/timeb.h>
#include <QMessageBox>
#include <QSvgGenerator>
#include <QPainter>

#include "window.h"
#include "images_tab.h"
#include "filters_tab.h"
#include "shaders.h"
#include "image_IO.h"
#include "map_list.h"

// filters

#include "filter.h"

#ifdef DEFINE_FILTER_BILATERAL
#include "filter_bilateral.h"
#endif

#ifdef DEFINE_FILTER_CANNY
#include "filter_canny.h"
#endif

#ifdef DEFINE_FILTER_CCVT
#include "filter_ccvt.h"
#endif

#ifdef DEFINE_FILTER_COMBINATION
#include "filter_combination.h"
#endif

#ifdef DEFINE_FILTER_HALFTONING_CONTRAST_AWARE
#include "filter_halftoning_cah.h"
#endif

#ifdef DEFINE_FILTER_CONTRAST_BRIGHT
#include "filter_contrast_bright.h"
#endif

#ifdef DEFINE_FILTER_DILATION
#include "filter_dilation.h"
#endif

#ifdef DEFINE_FILTER_DISTANCE_FIELD
#include "filter_distance_field.h"
#endif

#ifdef DEFINE_FILTER_DOG
#include "filter_dog.h"
#endif

#ifdef DEFINE_FILTER_DOT_EBG
#include "filter_dot_ebg.h"
#endif

#ifdef DEFINE_FILTER_DOT_SVG
#include "filter_dot_svg.h"
#endif

#ifdef DEFINE_FILTER_EROTION
#include "filter_erotion.h"
#endif

#ifdef DEFINE_FILTER_GAUSSIAN
#include "filter_gaussian.h"
#endif

#ifdef DEFINE_FILTER_HALFTONING_ADAPTIVE_CLUSTERING_SELECTIVE_PRECIPITATION
#include "filter_halftoning_acsp.h"
#endif

#ifdef DEFINE_FILTER_HALFTONING_OSTROMOUKHOV
#include "filter_halftoning_ost.h"
#endif

#ifdef DEFINE_FILTER_HALFTONING_SPACE_FILLING_CURVE
#include "filter_halftoning_sfc.h"
#endif

#ifdef DEFINE_FILTER_INVERSION
#include "filter_inversion.h"
#endif

#ifdef DEFINE_FILTER_KANG
#include "filter_kang.h"
#endif

#ifdef DEFINE_FILTER_MEASURE_SSIM_PSNR
#include "filter_measure_ssim_psnr.h"
#endif

#ifdef DEFINE_FILTER_RETINEX
#include "filter_retinex.h"
#endif

#ifdef DEFINE_FILTER_STIPPLING_BY_EXAMPLE
#include "filter_stippling_sbe.h"
#endif

#ifdef DEFINE_FILTER_STIPPLING_EXAMPLE_BASED_GRAYSCALE
#include "filter_stippling_ebg.h"
#endif

#ifdef DEFINE_FILTER_STIPPLING_CONTRAST_AWARE
#include "filter_stippling_cas.h"
#endif

#ifdef DEFINE_FILTER_RWT
#include "filter_rwt.h"
#endif

#ifdef DEFINE_FILTER_WCVD
#include "filter_wcvd.h"
#endif

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <QDir>
#include <QList>

namespace _f_gl_widget_ns
{
  typedef enum {TYPE_FILTER,INPUT_IMAGE_0,OUTPUT_IMAGE_0,INPUT_IMAGE_1} _info_filter;

  const int NUM_TABS=4;

  const string FILE_NAME_PAPER={"./papers/default.png"};

  typedef enum {RESOLUTION_300PPI,RESOLUTION_600PPI,RESOLUTION_1200PPI,LAST_RESOLUTION} _resolution;

  const int VEC_RESOLUTION[3]={300,600,1200};
  const int VEC_RESOLUTION_FACTOR[3]={1,2,4};

  const unsigned int MIN_DOT_SIZE=4;
  const unsigned int MAX_DOT_SIZE=8;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _gl_widget : public QOpenGLWidget
{
Q_OBJECT

public:
  static int Counter_draw;

  // filters
  //_map_list<_filter *> Filters;
  _map_list<shared_ptr<_filter>> Filters;
  // output images
  //_map_list<cv::Mat *> Images;
  _map_list<shared_ptr<cv::Mat>> Images;
  // Pipeline (maintains the order of filters)
  //vector<_filter *> Pipeline;
  vector<shared_ptr<_filter>> Pipeline;

        _gl_widget(_window *Window1,QWidget *Parent = 0);
        ~_gl_widget();
  void  clear_data();
  void  update_images();
  void  read_image(std::string File_name);
  void  save_image(std::string File_name);

  void  read_dots();
  void  refresh_image();
  void  refresh_image(cv::Mat *Image);
  void  update_all_filters();

  void  insert_filters();

  void  set_image(std::string Image_name);
  void  set_filter(std::string Filter_name);
  void  show_images();
  void  show_filters();

  void  update_aux(std::string Name);
  void  update_effect(std::string Name);

  void read_data_effect(std::string Name);
  void write_data_effect(std::string Name);
  void read_effect(std::string Name);

  //void create_filters_from_json();
  void create_filters_from_blocks(map<std::string, _block> *Map_data_blocks1, vector<std::string> *Vec_order1, map<std::string, map<std::string, std::string> > *Map_filters_parameters1);
  void create_filters_from_blocks();
  void update_filters_parameters();

  // update images with the selectec dots texture packet
  void update_filter_using_dots_texture_packet(int Dots_packet1);

  // change tab
  void change_tab(int Index1);

  // activate the dots packets
  void selected_dots_index(int Selected_dots_index1){Selected_dots_index=Selected_dots_index1;}
  // aux
  void  check2(std::string File_name1,std::string File_name2);
  //
  bool image_loaded(){return Image_loaded;};

  // Tabs
  _images_tab *Images_tab;
  _filters_tab *Filters_tab;

  map<std::string,_block> *Map_data_blocks;
  vector<std::string> *Vec_order;
  map<std::string,map<std::string,std::string>> *Map_filters_parameters;

protected:
  void initializeGL() Q_DECL_OVERRIDE;;
  void paintGL() Q_DECL_OVERRIDE;;
  void resizeGL(int width, int height) Q_DECL_OVERRIDE;;
  void mousePressEvent(QMouseEvent *Event);
  void mouseReleaseEvent(QMouseEvent *Event);
  void mouseDoubleClickEvent(QMouseEvent *Event);
  void mouseMoveEvent(QMouseEvent *Event);
  void wheelEvent(QWheelEvent *Event);

  void draw_objects();
  void refresh();

  void adjust_image_sizes(int &Width1, int &Height1);

private:
  _window *Window;

  GLuint Program1;
  GLuint Texture1;
  GLuint VAO1;
  GLuint VBO1_1;
  GLuint VBO1_2;

  vector<QImage> Vec_images;

  std::vector<_vertex3f> Vertices;
  std::vector<_vertex2f> Tex_coordinates;

  float Window_width;
  float Window_height;

  vector<vector<float>> Scale_drawing_mode;
  vector<vector<_vertex2f>> Translation;

  // translation of the window
  bool Change_pos;
  QTime Click_time;

  int Initial_position_X;
  int Initial_position_Y;

  bool Ready;
  bool Effect_ready;
  bool Image_loaded;

  //
  std::string Selected_image;
  std::string Selected_filter;
  int Selected_filter_index;

  // index of the image
  int Selected_image_index; // for images
  int Selected_dots_index; // for dots
  int Selected_dot_design_index; // for dot design

  int Selected_tab;
  int Selected_index; // the general index for mouse
  //
  int Max_texture_size;

  shared_ptr<cv::Mat> Image_cv_gray;
  shared_ptr<cv::Mat> Image_cv_gray1;
  shared_ptr<cv::Mat> Image_cv_color;
  shared_ptr<cv::Mat> Frame_buffer;
  shared_ptr<cv::Mat> Original_image;

  // Images of dots for stippling (no ppi level)
  vector<vector<cv::Mat *>> Dots;

  // Image for paper
  cv::Mat Paper;

  // parameters for filters
  vector< map<std::string,std::string> > Filters_json_data;

  // for optimizing the updating
  // for each filter name, it gives the list to other filters that has the filter name as input
  map<std::string,vector<std::string> > Graph;

  bool Painting;
  bool Paper_loaded;
  bool Active_dots_tabs;
  bool Drawn;
  bool Image_changed;
};


#endif
