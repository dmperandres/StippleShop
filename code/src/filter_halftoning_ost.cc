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


#include "filter_halftoning_ost.h"
#include "glwidget.h"

using namespace _f_halftoning_ost_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_ost::_filter_halftoning_ost()
{
  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_HALFTONING_OST;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}



/*****************************************************************************//**
 * interchange vectors
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_ost::shift_buffers(double *&Carry_line_0,double *&Carry_line_1,int Width)
{
  double *Tmp;

  // the vectors are interchanged
  Tmp=Carry_line_0;
  Carry_line_0=Carry_line_1;
  Carry_line_1=Tmp;
  // the vector is initilized to 0
  memset (Carry_line_1, 0.,Width*sizeof(double));
}


/*****************************************************************************//**
 * computes the error diffusion using ostromoukhov
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_ost::distribute_error_ostromoukhov(int Col_pos,double Difference,int Direction, int Input_level,double *Carry_line_0,double *Carry_line_1)
{
  double Right_term, Down_left_term, Down_term;
  _coefficients Aux_coefs=Coefficients[Input_level];

  Right_term=(double)Aux_coefs.Right_pixel*Difference/(double)Aux_coefs.Sum;
  Down_left_term=(double)Aux_coefs.Down_left_pixel*Difference/(double)Aux_coefs.Sum;
  Down_term=Difference-(Right_term+Down_left_term);

  Carry_line_0[Col_pos+Direction]+=Right_term;
  Carry_line_1[Col_pos-Direction]+=Down_left_term;
  Carry_line_1[Col_pos]+=Down_term;
}


/*****************************************************************************//**
 * computes the error diffusion using floyd-Steinberg
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_ost::distribute_error_floyd_steinberg(int Col_pos,double Difference,int Direction,double *Carry_line_0,double *Carry_line_1)
{
  double Difference_2 = Difference + Difference ;
  double Difference_3 = Difference + Difference_2 ;
  double Difference_5 = Difference_3 + Difference_2 ;
  double Difference_7 = Difference_5 + Difference_2 ;

  Carry_line_0[Col_pos+Direction] += (double) (Difference_7/16.);
  Carry_line_1[Col_pos-Direction] += (double) (Difference_3/16.);
  Carry_line_1[Col_pos] += (double) (Difference_5/16.);
  Carry_line_1[Col_pos+Direction] += (double) (Difference/16.);
}


/*****************************************************************************//**
 * computes the halfoning
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_ost::halftoning(cv::Mat *Input_image0,cv::Mat *Output_image0)
{
  int Col_pos, Row_pos;
  int Col_start, Col_stop, Col_step, Row_start, Row_stop, Direction;
  int Input, Intensity;
  double Threshold = 127.5, Corrected_level, Difference;
  double *Carry_line_0; // carry buffer; current line
  double *Carry_line_1; // carry buffer; current line + 1
  int Width=Input_image0->cols;
  int Height=Input_image0->rows;
  int Counter=0;

  //printf("Making ostromoukhov output image...\n");
  Carry_line_0=1+(double *)calloc(Width+2,sizeof(double));
  Carry_line_1=1+(double *)calloc(Width+2,sizeof(double));

  Row_start = 0; Row_stop=Height;
  for (Row_pos = Row_start; Row_pos < Row_stop; Row_pos++) {
    if ((Row_pos & 1)==0){ // even lines
      Direction=TO_RIGHT;
      Col_start=0;Col_stop=Width;Col_step=1;
    }
    else{ // odd lines
      Direction=TO_LEFT;
      Col_start=Width-1;Col_stop=-1;Col_step=-1;
    }

    for (Col_pos = Col_start; Col_pos != Col_stop; Col_pos += Col_step) {
      Input=(int) Input_image0->at<unsigned char>(Row_pos,Col_pos);

      Corrected_level=Input+Carry_line_0[Col_pos]; // offset +1
      if (Corrected_level<=Threshold){
        Intensity=BLACK; // put black
        Counter++;
      }
      else Intensity=WHITE; // put white
      //
      Difference=Corrected_level-Intensity;
      distribute_error_ostromoukhov(Col_pos,Difference,Direction,Input,Carry_line_0,Carry_line_1);
      //
      if (Input==(int)BLACK || Intensity==(int)BLACK) Output_image0->at<unsigned char>(Row_pos,Col_pos)=BLACK;
      else Output_image0->at<unsigned char>(Row_pos,Col_pos)=WHITE;
    } // Col_pos-cycle
    shift_buffers(Carry_line_0,Carry_line_1,Width);
  } // Row_pos-cycle
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_ost::update()
{
  cv::Mat *Aux_input_image=nullptr;
  cv::Mat *Aux_output_image=nullptr;

  // check the output size
  if (Input_image_0->cols!=Output_image_0->cols || Input_image_0->rows!=Output_image_0->rows){
    Output_image_0->release();
    Output_image_0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
  }

  // Check the number of input channels
  if (Input_image_0->channels()!=Num_channels_input_image_0){// Different number of channels
    if (Input_image_0->channels()==3){
      // conversion
      Aux_input_image=new cv::Mat;
      Aux_input_image->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
      cvtColor(*Input_image_0,*Aux_input_image,cv::COLOR_BGR2GRAY,1);
    }
    else cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    Aux_input_image=Input_image_0;
  }

  // Check the number of output channels
  if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
    if (Output_image_0->channels()==3){
      // conversion
      Aux_output_image=new cv::Mat;
      Aux_output_image->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
      halftoning(Aux_input_image,Aux_output_image);
      cvtColor(*Aux_input_image,*Output_image_0,cv::COLOR_GRAY2RGB,3);
    }
    else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    halftoning(Aux_input_image,Output_image_0);
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_ost_ui::_filter_halftoning_ost_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_halftoning_ost=new _qtw_filter_halftoning_ost(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_ost_ui::_filter_halftoning_ost_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_halftoning_ost=new _qtw_filter_halftoning_ost(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_ost_ui::~_filter_halftoning_ost_ui()
{
  delete Qtw_filter_halftoning_ost;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_ost_ui::show()
{
  Qtw_filter_halftoning_ost->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_ost_ui::hide()
{
  Qtw_filter_halftoning_ost->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_halftoning_ost_ui::get_link()
{
  return Qtw_filter_halftoning_ost->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_halftoning_ost::_qtw_filter_halftoning_ost(_gl_widget *GL_widget1,_filter_halftoning_ost_ui *Filter1,string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

 Group_box_main=new QGroupBox(tr(Box_name.c_str()));
 Group_box_main->hide();
}
