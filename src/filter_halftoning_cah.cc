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


#include "filter_halftoning_cah.h"
#include "glwidget.h"

using namespace _f_halftoning_cah_ns;


/*****************************************************************************//**
 * function for setting computing the halftoning with CAH technique
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_cah::_filter_halftoning_cah()
{
  Kernel_size=HALFTONING_CAH_KERNEL_SIZE_DEFAULT;
  Exponent=HALFTONING_CAH_EXPONENT_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_HALFTONING_CAH;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_cah::reset_data()
{
  Kernel_size=HALFTONING_CAH_KERNEL_SIZE_DEFAULT;
  Exponent=HALFTONING_CAH_EXPONENT_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_cah::halftoning(cv::Mat *Input_image0,cv::Mat *Output_image0)
{
  int Value,Error;
  int Kernel_radius=Kernel_size/2;
  float Pixel_radious;
  float Sum_weights;
  cv::Mat Weights(Kernel_size,Kernel_size,CV_32F);
  float Factor,Operand,Residual;
  cv::Mat Pixels_computed;
  cv::Mat Input_image_aux;

  // the input image is copied because its values are changed
  Input_image_aux=Input_image0->clone();

  // the output image is initialized to white
  Output_image0->setTo(255);

  // initilize the mat for treated pixels to false
  Pixels_computed=cv::Mat::zeros(Input_image_aux.rows,Input_image_aux.cols,CV_8U);

  Error=0;
  Residual=0;
  // all the pixels of the imput image are treated
  for (int Row=Kernel_radius;Row<Input_image_aux.rows-Kernel_radius;Row++){
    for (int Col=Kernel_radius;Col<Input_image_aux.cols-Kernel_radius; Col++){

      Value=(int) Input_image_aux.at<unsigned char>(Row,Col)+Residual;

      // initialize values for difusion n
      // Error>=0 W=I/R^k
      // Error<0  W=(255-I)/R^K
      if (Value<=128){ // to black, positive error  Error=Value-0
        //cout << "Pixel Negro " << endl;
        // the pixel is black
        Output_image0->at<unsigned char>(Row,Col)=0;
        //
        Error=Value;
        //
        Factor=1;
        Operand=0;
      }
      else{ // to white, negative Error: 255-Value
        //cout << "Pixel blanco " << endl;

        Output_image0->at<unsigned char>(Row,Col)=255;
        //
        Error=Value-255;
        //
        Factor=-1;
        Operand=255;
      }

      //getchar();

      // compute the weights W=I/R^k (for pixels not computed)
      // reset the Weights
      Weights.zeros(Kernel_size,Kernel_size,CV_32F);
      Sum_weights=0;
      for (int k=-Kernel_radius;k<=Kernel_radius;k++){
        for (int l=-Kernel_radius;l<=Kernel_radius;l++){
          if (k!=0 || l!=0){ // the center is not computed because is the pixels that produce the error
            if (Pixels_computed.at<unsigned char>(Row+k,Col+l)==0){ // use only the pixels not treated
              Pixel_radious=sqrt(k*k+l*l);
              Pixel_radious=pow(Pixel_radious,Exponent);
              // the same operation is used for positive and negative errors using the Factor and Operand variables
              Weights.at<float>(k+Kernel_radius,l+Kernel_radius)=(Operand+Factor*(float)Input_image_aux.at<unsigned char>(Row+k,Col+l))/Pixel_radious;
              Sum_weights+=Weights.at<float>(k+Kernel_radius,l+Kernel_radius);
            }
          }
        }
      }

      if (Sum_weights>0){
        // now the weights are normalized
        for (int k=-Kernel_radius;k<=Kernel_radius;k++){
          for (int l=-Kernel_radius;l<=Kernel_radius;l++){
            if (k!=0 || l!=0){ // the center is not computed because is the pixels that produce the error
              if (Pixels_computed.at<unsigned char>(Row+k,Col+l)==0){ // use only the pixels not treated
                Weights.at<float>(k+Kernel_radius,l+Kernel_radius)/=Sum_weights;
              }
            }
          }
        }
        // now the error is difussed depending on the error of the trated pixel and the weight
        // I=I+Error*Weight
        Residual=0;
        for (int k=-Kernel_radius;k<=Kernel_radius;k++){
          for (int l=-Kernel_radius;l<=Kernel_radius;l++){
            if (k!=0 || l!=0){ // the center is not computed because is the pixels that produce the error
              if (Pixels_computed.at<unsigned char>(Row+k,Col+l)==0){// use only the pixels not treated
                Value=(int)((float)Input_image_aux.at<unsigned char>(Row+k,Col+l)+Weights.at<float>(k+Kernel_radius,l+Kernel_radius)*(float)Error);

                if (Value>255){
                  //cout << "residuo positivo " << Value << endl;
                  Residual+=Value-255;
                  Value=255;
                }
                if (Value<0){
                  //cout << "residuo negativo " << Value << endl;
                  Residual+=Value;
                  Value=0;
                }
                Input_image_aux.at<unsigned char>(Row+k,Col+l)=Value;
              }
            }
          }
        }
      }

      // the pixel is marked as treated
      Pixels_computed.at<unsigned char>(Row,Col)=255;
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_cah::update()
{
  cv::Mat *Aux_input_image=nullptr;
  cv::Mat *Aux_output_image=nullptr;

  // check the output size
  if (Input_image_0->cols!=Output_image_0->cols || Input_image_0->rows!=Output_image_0->rows){
    Output_image_0->release();
    Output_image_0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
  }

//  *Output_image_0=Input_image_0->clone();
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
    Aux_input_image=Input_image_0;
  }

  // Check the number of output channels
  if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
    if (Output_image_0->channels()==3){
      // conversion
      Aux_output_image=new cv::Mat;
      Aux_output_image->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
      halftoning(Aux_input_image,Aux_output_image);
      cvtColor(*Aux_input_image,*Output_image_0,CV_GRAY2RGB,3);
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

_filter_halftoning_cah_ui::_filter_halftoning_cah_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_halftoning_cah=new _qtw_filter_halftoning_cah(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_cah_ui::_filter_halftoning_cah_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_halftoning_cah=new _qtw_filter_halftoning_cah(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_cah_ui::~_filter_halftoning_cah_ui()
{
  delete Qtw_filter_halftoning_cah;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_cah_ui::reset_data()
{
  _filter_halftoning_cah::reset_data();
  Qtw_filter_halftoning_cah->set_parameter1(parameter1());
  Qtw_filter_halftoning_cah->set_parameter2(parameter2());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_cah_ui::show()
{
  Qtw_filter_halftoning_cah->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_cah_ui::hide()
{
  Qtw_filter_halftoning_cah->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_halftoning_cah_ui::get_link()
{
  return Qtw_filter_halftoning_cah->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_cah_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(HALFTONING_CAH_KERNEL_SIZE_DEFAULT);
    parameter2(HALFTONING_CAH_EXPONENT_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["kernel_size"]=="default") parameter1(HALFTONING_CAH_KERNEL_SIZE_DEFAULT);
      else parameter1(atoi(Parameters["kernel_size"].c_str()));
      if (Parameters["exponent"]=="default") parameter2(HALFTONING_CAH_EXPONENT_DEFAULT);
      else parameter2(atof(Parameters["exponent"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of HALFTONING_CAH filter");
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

void _filter_halftoning_cah_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%d",parameter1());
  Parameters["kernel_size"]=string(Aux);
  sprintf(Aux,"%5.2f",parameter2());
  Parameters["exponent"]=string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_halftoning_cah::_qtw_filter_halftoning_cah(_gl_widget *GL_widget1,_filter_halftoning_cah_ui *Filter1,string Box_name)
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
  char Aux[100];
  sprintf(Aux,"%4.2f",Filter->parameter2());
  Str=Aux;
  Line_edit_parameter2->setText(Str);

  Slider_parameter2 = new QSlider(Qt::Horizontal);
  Slider_parameter2->setRange(Parameter2_min_value,Parameter2_max_value);
  Slider_parameter2->setSingleStep(Parameter2_single_step);
  Slider_parameter2->setPageStep(Parameter2_page_step);
  Slider_parameter2->setTickInterval(Parameter2_tick_interval);
  Slider_parameter2->setTickPosition(QSlider::TicksRight);
  Slider_parameter2->setTracking(Parameter2_set_tracking);
  Slider_parameter2->setValue(Filter->parameter2()*Parameter2_factor);
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

void _qtw_filter_halftoning_cah::set_parameter1(int Value)
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

void _qtw_filter_halftoning_cah::set_parameter2(float Value)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%4.2f",Value);
  Str=Aux;
  Line_edit_parameter2->setText(Str);
  Slider_parameter2->setValue(Value*Parameter2_factor);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_halftoning_cah::set_parameter1_slot(int Size)
{
  QString Str;
  char Aux[100];

  if (Size%2==0) Size++;

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter1->setText(Str);
  Slider_parameter1->setValue(Size);
  Filter->parameter1(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_halftoning_cah::set_parameter2_slot(int Size)
{
  QString Str;
  char Aux[100];
  float Value=(float)Size/Parameter2_factor;

  sprintf(Aux,"%4.2f",Value);
  Str=Aux;
  Line_edit_parameter2->setText(Str);
  Filter->parameter2(Value);
  GL_widget->update_effect(Filter->Name);
}
