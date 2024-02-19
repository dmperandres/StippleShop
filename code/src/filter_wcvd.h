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


#ifndef _FILTER_WCVD_H
#define _FILTER_WCVD_H

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QProgressDialog>
#include <QCoreApplication>

#include <opencv.hpp>

#include "line_edit.h"
#include <string>
#include "filter.h"
#include "vertex.h"
#include "random.h"

//#include "gl2ps.h"

namespace _f_wcvd_ns
{
  // Defines for controling the drawing of the if the WCVD
  #define WCVD_DRAWING
  //#define WCVD_FIRST_TIME

  // Number of dots
  // parameter 1
  const std::string String_group_box_parameter1("Percent of dots");
  const std::string String_label_parameter1_min("0%");
  const std::string String_label_parameter1_max("100% ");
  const std::string String_label_parameter1_tooltip("Display the percentage of dots that will be computed");
  const int Parameter1_min_value=0;
  const int Parameter1_max_value=100;
  const int Parameter1_single_step=1;
  const int Parameter1_page_step=5;
  const int Parameter1_tick_interval=5;
  const bool Parameter1_set_tracking=false;
  const std::string String_parameter1_tooltip("Controls the number of dots to be computed");

  // Number of dots
  // parameter 2
  const std::string String_group_box_parameter2("Number of dots");
  const std::string String_parameter2_tooltip("Controls the number of dots to be computed");

  // modulate dot size
  // parameter 3
  const std::string String_group_box_parameter3("Save intermediate results");
  const std::string String_checkbox_parameter3("Save");
  const std::string String_parameter3_tooltip("Controls if the intermediate results are saved");


  // Stop condition % of moving centroidals
  // parameter 4
  const std::string String_group_box_parameter4("Stop condition (movement)");
  const std::string String_label_parameter4_min("0%  ");
  const std::string String_label_parameter4_max("100%");
  const std::string String_label_parameter4_tooltip("Display the percentage of dots that must be in fixed positions to stop the computation");
  const int Parameter4_min_value=0;
  const int Parameter4_max_value=100;
  const int Parameter4_single_step=3;
  const int Parameter4_page_step=5;
  const int Parameter4_tick_interval=5;
  const bool Parameter4_set_tracking=false;
  const std::string String_parameter4_tooltip("Controls the number of dots that must be in fixed positions to stop the computation");

  // Stop condition Number of iterations
  // parameter 5
  const std::string String_group_box_parameter5("Stop condition (iterations)");
  const std::string String_parameter5_tooltip("Controls the number of iterations to stop the computation");

  // Default values
  const float WCVD_NUMBER_OF_DARK_DOTS_DEFAULT=1000;
  const float WCVD_NUMBER_OF_DOTS_DEFAULT=1;
  const unsigned int WCVD_PERCENT_OF_DOTS_DEFAULT=25;
  const bool WCWD_SAVE_INTERMEDIATE_IMAGES_DEFAULT=false;
  //const float WCVD_MIN_NUMBER_OF_DOTS_DEFAULT=1;
  const int WCVD_PERCENT_FIXED_CENTROIDALS=99;
  const int WCVD_NUM_ITERACTIONS_DEFAULT=1000;

  const float DOTS_FACTOR=0.35f;//0.125;
  const float MIN_DOTS_DISTANCE=1.0f; // 1 pixel

  const unsigned int Num_renderbuffers=2;

  const int Front_plane=-100;
  const int Back_plane=100;
  const int Apex_height=60;
  const int Point_height=80;

  const _vertex3uc Vec_colors[]={{255,0,0},{0,255,0},{0,0,255},{0,255,255},{255,0,255},{255,255,0}};
  const int MAX_COLORS=5;

  class _pixel
  {
  public:
    unsigned char Red;
    unsigned char Green;
    unsigned char Blue;
  };

  #ifdef WINDOWS
  const double M_PI=3.1415926535897932;
  #endif

  const char FILE_NAME[]="aux_code/wcvd/wcvd_%02d.png";
}

class _gl_widget;
class _qtw_filter_wcvd;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_wcvd : public _filter
{
public:
  _filter_wcvd();
  ~_filter_wcvd(){};
  void reset_data();
  bool change_output_image_size(){return Change_output_image_size;};
  bool use_dots(){return Use_dots;};

  // aux functions
  void initialize();
  void count_dark_pixels(cv::Mat *Input_image);
  void create_random_points(cv::Mat *Input_image);
  void copy_points();
  void create_primitive(unsigned int Index);
  void create();
  void compute_centroids(cv::Mat *Input_image);
  void draw_new_points(float R,float G,float B);
  void draw_weighted_points(cv::Mat *Input_image);
  void draw_points(cv::Mat *Input_image);
  void draw_dots(cv::Mat *Output_image);
  void draw_triangles();
  void clear_window();
  void change_mvp();

  // for drawing the WCVD
  void create_primitive_drawing(unsigned int Index);
  void create_drawing();
  void hsv_to_rbg(float h,float s, float v,unsigned char &r,unsigned char &g,unsigned char &b);

  void wcvd(cv::Mat *Input_image0,cv::Mat *Output_image0);
  void update();

  void percent_of_dots(unsigned int Percent_of_dots1);
  unsigned int percent_of_dots(){return Percent_of_dots;};

  void save_intermediate_images(bool Value){Save_intermediate_images=Value;}
  bool save_intermediate_images(){return Save_intermediate_images;};

  void number_of_dots(unsigned int Number_of_dots1);
  unsigned int number_of_dots(){return Number_of_good_dots;};

  void percent_fixed_centroidals(int Percent_fixed_centroidals1){Percent_fixed_centroidals=Percent_fixed_centroidals1;};
  int  percent_fixed_centroidals(){return Percent_fixed_centroidals;};

  void number_of_iteractions(int Number_of_iteractions1){Number_of_iteractions=Number_of_iteractions1;};
  int  number_of_iteractions(){return Number_of_iteractions;};

  void save_wcvd_image_to_file(std::string File_name);
  void save_wcvd_image();

  virtual void update_percent_of_dots(){};
  virtual void update_number_of_dots(){};

  void save_GL_state();
  void restore_GL_state();

  _gl_widget *GL_widget;

  unsigned int Number_of_dots;
  unsigned int Number_of_dark_dots;

  unsigned int Framebuffer; // id for the framebuffer
  unsigned int Renderbuffers[_f_wcvd_ns::Num_renderbuffers]; // id for the renderbuffers

  // geometry
  vector<_vertex3f> Vertices; // coordinates of vertices
  vector<_vertex3i> Triangles; // index of vertices for each triangle
  vector<_vertex3uc> Colors; // Colors of vertices
  // the original positions
  vector<_vertex3f> Points;
  // The positions obtained from computing the centroid. They are modulated by the weight
  vector<_vertex3f> New_positions;
  // for reading the image
  vector<_vertex3uc> Pixels;
  // for counting the number of pixels that contribute (the weight). It is a float because the position is modulated by the tone
  vector<float> Pixels_count;
  vector<_vertex3uc> Colors_cones;

  // for drawing
  int Window_width;
  int Window_height;

  int Num_divisions;
  int Num_vertices;
  int Num_faces;
  int Radius;
  int Height;
  int Moved_points;
  int Percent_fixed_centroidals;
  int Number_of_iteractions;

  bool Local_change;
  unsigned int Number_of_good_dots;
  unsigned int Percent_of_dots;
  bool Dark_points_counted;

  _random_uniform_double C1;

  cv::Mat Aux_image;
  bool Save_intermediate_images;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_wcvd_ui :public _filter_wcvd
{
public:
  _filter_wcvd_ui(_gl_widget *GL_widget1,std::string Box_name="WCVD parameters");
  _filter_wcvd_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,std::string Box_name="WCVD parameters");
  ~_filter_wcvd_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);

  void parameter1(int Value){percent_of_dots((unsigned int)Value);};
  int parameter1(){return (int)percent_of_dots();};

  void parameter2(int Value){number_of_dots((unsigned int)Value);};
  int parameter2(){return (int)number_of_dots();};

  void parameter3(bool Value){save_intermediate_images(Value);};
  bool parameter3(){return save_intermediate_images();};

  void parameter4(int Value){percent_fixed_centroidals(Value);};
  int parameter4(){return percent_fixed_centroidals();};

  void parameter5(int Value){number_of_iteractions(Value);};
  int parameter5(){return number_of_iteractions();};

  void local_change(bool Value){Local_change=Value;};

  void update_percent_of_dots();
  void update_number_of_dots();

  private:
  _qtw_filter_wcvd *Qtw_filter_wcvd;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_wcvd: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_wcvd(_gl_widget *GL_widget1,_filter_wcvd_ui *Filter1,std::string Box_name="WCVD parameters");
  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);
  void set_parameter2(int Value);
  void set_parameter3(bool Value);
  void set_parameter4(int Value);
  void set_parameter5(int Value);

protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);
  void set_parameter3_slot(int Value);
  void set_parameter4_slot(int Value);
  void set_parameter5_slot(int Value);

private:
  QGroupBox *Group_box_main;
  QGroupBox *Group_box_parameter1;  
  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;
  QGroupBox *Group_box_parameter4;
  QGroupBox *Group_box_parameter5;

  // Percent of dots
  QSlider *Slider_parameter1;
  QLineEdit *Line_edit_parameter1;

  // Number of dots
  QSpinBox *Spinbox_parameter2;

  // save wcvd images
  QCheckBox *Checkbox_parameter3;

  // Percent of fixed centroidals
  QSlider *Slider_parameter4;
  QLineEdit *Line_edit_parameter4;

  // number of iteractions
  QSpinBox *Spinbox_parameter5;

  _filter_wcvd_ui *Filter;
  _gl_widget *GL_widget;
};
#endif
