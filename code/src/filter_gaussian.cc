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


#include "filter_gaussian.h"
#include "glwidget.h"

using namespace _f_gaussian_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_gaussian::_filter_gaussian()
{
  Kernel_size=GAUSSIAN_KERNEL_SIZE_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_GAUSSIAN;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_gaussian::reset_data()
{
  Kernel_size=GAUSSIAN_KERNEL_SIZE_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_gaussian::update()
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
    else std::cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << std::endl;
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

      if (Kernel_size>1) cv::GaussianBlur(*Aux_input_image,*Aux_output_image,cv::Size(Kernel_size,Kernel_size),0,0);
      else *Aux_output_image=*Aux_input_image;
      cvtColor(*Aux_output_image,*Output_image_0,cv::COLOR_GRAY2RGB,3);
    }
    else std::cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << std::endl;
  }
  else{// the same number of channels
    if (Kernel_size>1) cv::GaussianBlur(*Aux_input_image,*Output_image_0,cv::Size(Kernel_size,Kernel_size),0,0);
    else *Output_image_0=*Aux_input_image;
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Input_image_0) delete Aux_output_image;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_gaussian_ui::_filter_gaussian_ui(_gl_widget *GL_widget1,std::string Name1)
{
  Name=Name1;
  Qtw_filter_gaussian=new _qtw_filter_gaussian(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_gaussian_ui::_filter_gaussian_ui(_gl_widget *GL_widget1, std::map<std::string, std::string> &Parameters, std::string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_gaussian=new _qtw_filter_gaussian(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_gaussian_ui::~_filter_gaussian_ui()
{
  delete Qtw_filter_gaussian;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_gaussian_ui::reset_data()
{
  _filter_gaussian::reset_data();
  Qtw_filter_gaussian->set_parameter1(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_gaussian_ui::show()
{
  Qtw_filter_gaussian->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_gaussian_ui::hide()
{
  Qtw_filter_gaussian->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_gaussian_ui::get_link()
{
  return Qtw_filter_gaussian->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_gaussian_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(GAUSSIAN_KERNEL_SIZE_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["kernel_size"]=="default") parameter1(GAUSSIAN_KERNEL_SIZE_DEFAULT);
      else parameter1(atoi(Parameters["kernel_size"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of GAUSSIAN filter");
      MsgBox.exec();
      exit(-1);
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_gaussian_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%d",parameter1());
  Parameters["kernel_size"]=std::string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_gaussian::_qtw_filter_gaussian(_gl_widget *GL_widget1,_filter_gaussian_ui *Filter1,std::string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_main=new QVBoxLayout;

  // Parameter1
  QGroupBox *Group_box_parameter1=new QGroupBox(tr(String_group_box_parameter1.c_str()));
  Group_box_parameter1->setAlignment(Qt::AlignCenter);

  QGridLayout *Grid_parameter1 = new QGridLayout;

  QLabel *Label_parameter1_min= new QLabel(tr(String_label_parameter1_min.c_str()));
  QLabel *Label_parameter1_max= new QLabel(tr(String_label_parameter1_max.c_str()));

  Line_edit_parameter1=new QLineEdit();
  Line_edit_parameter1->setAlignment(Qt::AlignRight);
  //Line_edit_parameter1->setValidator(Validator);
  Line_edit_parameter1->setReadOnly(true);
  Line_edit_parameter1->setEnabled(false);
  Line_edit_parameter1->setToolTip(tr(String_label_parameter1_tooltip.c_str()));
  Str=Str.number(Filter->parameter1());
  Line_edit_parameter1->setText(Str);

  Slider_parameter1 = new QSlider(Qt::Horizontal);
  Slider_parameter1->setRange(Parameter1_min_value,Parameter1_max_value);
  Slider_parameter1->setSingleStep(Parameter1_single_step);
  Slider_parameter1->setPageStep(Parameter1_page_step);
  Slider_parameter1->setTickInterval(Parameter1_tick_interval);
  Slider_parameter1->setTickPosition(QSlider::TicksRight);
  Slider_parameter1->setTracking(Parameter1_set_tracking);
  Slider_parameter1->setValue(Filter->parameter1());
  Slider_parameter1->setToolTip(tr(String_parameter1_tooltip.c_str()));

  Grid_parameter1->addWidget(Line_edit_parameter1,0,1,Qt::AlignCenter);
  Grid_parameter1->addWidget(Label_parameter1_min,1,0,Qt::AlignRight);
  Grid_parameter1->addWidget(Slider_parameter1,1,1);
  Grid_parameter1->addWidget(Label_parameter1_max,1,2,Qt::AlignLeft);

  Group_box_parameter1->setLayout(Grid_parameter1);

  Vertical_box_main->addWidget(Group_box_parameter1);

  Group_box_main->setLayout(Vertical_box_main);

  connect(Slider_parameter1, SIGNAL(valueChanged(int)),this,SLOT(set_parameter1_slot(int)));
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_gaussian::set_parameter1(int Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_parameter1->setText(Str);
  Slider_parameter1->setValue(Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_gaussian::set_parameter1_slot(int Size)
{
  QString Str;
  char Aux[100];

  if (Size%2==0){
    Size++;
    Slider_parameter1->setValue(Size);
  }

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter1->setText(Str);
  Filter->parameter1(Size);
  GL_widget->update_effect(Filter->Name);
}
