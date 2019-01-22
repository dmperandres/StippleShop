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


#include "filter_inversion.h"
#include "glwidget.h"

//using namespace _f_inversion;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_inversion::_filter_inversion()
{
  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_INVERSION;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_inversion::update()
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
      cvtColor(*Input_image_0,*Aux_input_image,CV_BGR2GRAY,1);
    }
    else cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    Aux_input_image=new cv::Mat;
    Aux_input_image->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
    *Aux_input_image=Input_image_0->clone();
  }
  // Check the number of output channels
  if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
    if (Output_image_0->channels()==3){
      // conversion
      Aux_output_image=new cv::Mat;
      Aux_output_image->create(Output_image_0->rows,Output_image_0->cols,CV_8UC1);

      cv::bitwise_not(*Aux_input_image,*Aux_output_image);
      cvtColor(*Aux_output_image,*Output_image_0,CV_GRAY2RGB,3);
    }
    else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    cv::bitwise_not(*Aux_input_image,*Output_image_0);
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Input_image_0) delete Aux_output_image;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_inversion_ui::_filter_inversion_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_inversion=new _qtw_filter_inversion(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_inversion_ui::_filter_inversion_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_inversion=new _qtw_filter_inversion(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_inversion_ui::~_filter_inversion_ui()
{
  delete Qtw_filter_inversion;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_inversion_ui::reset_data()
{
  _filter_inversion::reset_data();
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_inversion_ui::show()
{
  Qtw_filter_inversion->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_inversion_ui::hide()
{
  Qtw_filter_inversion->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_inversion_ui::get_link()
{
  return Qtw_filter_inversion->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_inversion_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
    Q_UNUSED(Parameters)
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_inversion_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
    Q_UNUSED(Parameters)
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_inversion::_qtw_filter_inversion(_gl_widget *GL_widget1,_filter_inversion_ui *Filter1,string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

  Group_box_main->hide();
}
