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


#include "filter_bilateral.h"
#include "glwidget.h"

using namespace _f_bilateral_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_bilateral::_filter_bilateral()
{
  Kernel_size=BILATERAL_KERNEL_SIZE_DEFAULT;
  Num_iterations=BILATERAL_NUM_ITERATIONS_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_BILATERAL;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_bilateral::reset_data()
{
  Kernel_size=BILATERAL_KERNEL_SIZE_DEFAULT;
  Num_iterations=BILATERAL_NUM_ITERATIONS_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_bilateral::update()
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
    Aux_input_image=Input_image_0;
  }

  // Check the number of output channels
  if (Input_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
    if (Output_image_0->channels()==3){
      // conversion
      Aux_output_image=new cv::Mat;
      Aux_output_image->create(Output_image_0->rows,Output_image_0->cols,CV_8UC1);

      cv::bilateralFilter(*Aux_input_image,*Aux_output_image,(int)Kernel_size,Kernel_size*2,Kernel_size/2);

      cvtColor(*Aux_output_image,*Output_image_0,cv::COLOR_GRAY2RGB,3);
    }
    else std::cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << std::endl;
  }
  else{// the same number of channels
    cv::bilateralFilter(*Aux_input_image,*Output_image_0,(int)Kernel_size,Kernel_size*2,Kernel_size/2);
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_bilateral::kernel_size(int Kernel_size1)
{
  Kernel_size=Kernel_size1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_bilateral::num_iterations(int Num_iterations1)
{
  Num_iterations=Num_iterations1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_bilateral_ui::_filter_bilateral_ui(_gl_widget *GL_widget1, std::string Name1)
{
  Name=Name1;
  Qtw_filter_bilateral=new _qtw_filter_bilateral(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_bilateral_ui::_filter_bilateral_ui(_gl_widget *GL_widget1, std::map<std::string, std::string> &Parameters, std::string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_bilateral=new _qtw_filter_bilateral(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_bilateral_ui::~_filter_bilateral_ui()
{
  delete Qtw_filter_bilateral;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_bilateral_ui::reset_data()
{
  _filter_bilateral::reset_data();
  Qtw_filter_bilateral->set_parameter1(parameter1());
  Qtw_filter_bilateral->set_parameter2(parameter2());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_bilateral_ui::show()
{
  Qtw_filter_bilateral->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_bilateral_ui::hide()
{
  Qtw_filter_bilateral->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_bilateral_ui::get_link()
{
  return Qtw_filter_bilateral->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_bilateral_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(BILATERAL_KERNEL_SIZE_DEFAULT);
    parameter2(BILATERAL_NUM_ITERATIONS_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["kernel_size"]=="default") parameter1(BILATERAL_KERNEL_SIZE_DEFAULT);
      else parameter1(atoi(Parameters["kernel_size"].c_str()));
      if (Parameters["iterations"]=="default") parameter2(BILATERAL_NUM_ITERATIONS_DEFAULT);
      else parameter2(atoi(Parameters["iterations"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of BILATERAL filter");
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

void _filter_bilateral_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%d",parameter1());
  Parameters["kernel_size"]=std::string(Aux);
  sprintf(Aux,"%d",parameter2());
  Parameters["iterations"]=std::string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_bilateral::_qtw_filter_bilateral(_gl_widget *GL_widget1,_filter_bilateral_ui *Filter1,std::string Box_name)
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
  // warnig to the adjust
  Str=Str.number(Filter->parameter1());
  Line_edit_parameter1->setText(Str);

  Slider_parameter1 = new QSlider(Qt::Horizontal);
  Slider_parameter1->setRange(Parameter1_min_value,Parameter1_max_value);
  Slider_parameter1->setSingleStep(Parameter1_single_step);
  Slider_parameter1->setPageStep(Parameter1_page_step);
  Slider_parameter1->setTickInterval(Parameter1_tick_interval);
  Slider_parameter1->setTickPosition(QSlider::TicksRight);
  Slider_parameter1->setTracking(Parameter1_set_tracking);
  // warnig to the adjust
  Slider_parameter1->setValue(Filter->parameter1());
  Slider_parameter1->setToolTip(tr(String_parameter1_tooltip.c_str()));

  Grid_parameter1->addWidget(Line_edit_parameter1,0,1,Qt::AlignCenter);
  Grid_parameter1->addWidget(Label_parameter1_min,1,0,Qt::AlignRight);
  Grid_parameter1->addWidget(Slider_parameter1,1,1);
  Grid_parameter1->addWidget(Label_parameter1_max,1,2,Qt::AlignLeft);

  Group_box_parameter1->setLayout(Grid_parameter1);

  // Parameter 2
  Group_box_parameter2=new QGroupBox(tr(String_group_box_parameter2.c_str()));
  Group_box_parameter2->setAlignment(Qt::AlignCenter);

  QGridLayout *Grid_parameter2 = new QGridLayout;

  QLabel *Label_parameter2_min= new QLabel(tr(String_label_parameter2_min.c_str()));
  QLabel *Label_parameter2_max= new QLabel(tr(String_label_parameter2_max.c_str()));

  Line_edit_parameter2=new QLineEdit();
  Line_edit_parameter2->setAlignment(Qt::AlignRight);
  Line_edit_parameter2->setReadOnly(true);
  Line_edit_parameter2->setEnabled(false);
  Line_edit_parameter2->setToolTip(tr(String_label_parameter2_tooltip.c_str()));
  Str=Str.number(Filter->parameter2());
  Line_edit_parameter2->setText(Str);

  Slider_parameter2 = new QSlider(Qt::Horizontal);
  Slider_parameter2->setRange(Parameter2_min_value,Parameter2_max_value);
  Slider_parameter2->setSingleStep(Parameter2_single_step);
  Slider_parameter2->setPageStep(Parameter2_page_step);
  Slider_parameter2->setTickInterval(Parameter2_tick_interval);
  Slider_parameter2->setTickPosition(QSlider::TicksRight);
  Slider_parameter2->setTracking(Parameter2_set_tracking);
  Slider_parameter2->setValue(Filter->parameter2());
  Slider_parameter2->setToolTip(tr(String_parameter2_slider_tooltip.c_str()));

  Grid_parameter2->addWidget(Line_edit_parameter2,0,1,Qt::AlignCenter);
  Grid_parameter2->addWidget(Label_parameter2_min,1,0,Qt::AlignRight);
  Grid_parameter2->addWidget(Slider_parameter2,1,1);
  Grid_parameter2->addWidget(Label_parameter2_max,1,2,Qt::AlignLeft);

  Group_box_parameter2->setLayout(Grid_parameter2);

  Vertical_box_main->addWidget(Group_box_parameter1);
  Vertical_box_main->addWidget(Group_box_parameter2);

  Group_box_main->setLayout(Vertical_box_main);

  connect(Slider_parameter1, SIGNAL(valueChanged(int)),this,SLOT(set_parameter1_slot(int)));
  connect(Slider_parameter2, SIGNAL(valueChanged(int)),this,SLOT(set_parameter2_slot(int)));
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_bilateral::set_parameter1(int Value)
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

void _qtw_filter_bilateral::set_parameter2(int Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_parameter2->setText(Str);
  Slider_parameter2->setValue(Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_bilateral::set_parameter1_slot(int Size)
{
  QString Str;

  Str=Str.number(Size);
  Line_edit_parameter1->setText(Str);
  Filter->parameter1(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_bilateral::set_parameter2_slot(int Size)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter2->setText(Str);
  Filter->parameter2(Size);
  GL_widget->update_effect(Filter->Name);
}
