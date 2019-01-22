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


#include "filter_stippling_cas.h"
#include "glwidget.h"

using namespace _f_stippling_cas_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_cas::_filter_stippling_cas()
{
  Kernel_size=HALFTONING_CAH_KERNEL_SIZE_DEFAULT;
  Exponent=HALFTONING_CAH_EXPONENT_DEFAULT;
  G_plus=HALFTONING_CAH_G_PLUS_DEFAULT;
  G_minus=HALFTONING_CAH_G_MINUS_DEFAULT;
  K=HALFTONING_CAH_K_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_STIPPLING_CAS;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_cas::reset_data()
{
  Kernel_size=HALFTONING_CAH_KERNEL_SIZE_DEFAULT;
  Exponent=HALFTONING_CAH_EXPONENT_DEFAULT;
  G_plus=HALFTONING_CAH_G_PLUS_DEFAULT;
  G_minus=HALFTONING_CAH_G_MINUS_DEFAULT;
  K=HALFTONING_CAH_K_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_cas::stippling(cv::Mat *Input_image0,cv::Mat *Output_image0)
{
  int Value,Error;
  int Kernel_radius=Kernel_size/2;
  float Pixel_radious;
  float Sum_weights;
  cv::Mat Weights(Kernel_size,Kernel_size,CV_32F);
  float Factor,Operand,Residual;
  cv::Mat Pixels_computed;
  cv::Mat Input_image_aux;
  float Stipple_size; // this is r in the paper
  float Adjustment_factor; // Sxy in the paper
  float Area_error; // e0 in the paper

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

      Value=(int) Input_image_aux.at<unsigned char>(Row,Col);

      // the value of the stipple size is computed
      Stipple_size=HALFTONING_CAH_STIPPLE_SIZE_MIN+((HALFTONING_CAH_STIPPLE_SIZE_MAX-HALFTONING_CAH_STIPPLE_SIZE_MIN)*(float)(255-Value)/255.);

      Area_error=(Stipple_size*Stipple_size-1)*K;

      Value+=Residual;

      // initialize values for difusion n
      if (Value<=128){ // to black, positive error  Error=Value-0
        // the pixel is black
        Output_image0->at<unsigned char>(Row,Col)=0;

        Error=Value;

        Factor=1;
        Operand=0;

        //Adjustment factor. Error>0 -> Sxy=(r)^G+ -> (stipple_size)^G+
        Adjustment_factor=pow(Stipple_size,G_plus);
      }
      else{ // to white, negative Error: 255-Value
        Output_image0->at<unsigned char>(Row,Col)=255;

        Error=Value-255;

        Factor=-1;
        Operand=255;
        //Adjustment factor. Error<0 -> Sxy=(1/r)^G- -> (1/stipple_size)^G
        Adjustment_factor=pow(1/Stipple_size,G_minus);
      }
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
        // I=I+Weight*(Error+Area_error)*Adjusment_factor
        Residual=0;
        for (int k=-Kernel_radius;k<=Kernel_radius;k++){
          for (int l=-Kernel_radius;l<=Kernel_radius;l++){
            if (k!=0 || l!=0){ // the center is not computed because is the pixels that produce the error
              if (Pixels_computed.at<unsigned char>(Row+k,Col+l)==0){// use only the pixels not treated

                Value=(int)((float)Input_image_aux.at<unsigned char>(Row+k,Col+l)+Weights.at<float>(k+Kernel_radius,l+Kernel_radius)*((float)Error+Area_error)*Adjustment_factor);

                if (Value>255){
                  Value=255;
                }
                if (Value<0){
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

void _filter_stippling_cas::update()
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
    Aux_input_image=Input_image_0;
  }

  // Check the number of output channels
  if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
    if (Output_image_0->channels()==3){
      // conversion
      Aux_output_image=new cv::Mat;
      Aux_output_image->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
      stippling(Aux_input_image,Aux_output_image);
      cvtColor(*Aux_input_image,*Output_image_0,CV_GRAY2RGB,3);
    }
    else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    stippling(Aux_input_image,Output_image_0);
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_cas_ui::_filter_stippling_cas_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_stippling_cas=new _qtw_filter_stippling_cas(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_cas_ui::_filter_stippling_cas_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Box_name)
{
  read_parameters(Parameters);
  Name=Box_name;
  Qtw_filter_stippling_cas=new _qtw_filter_stippling_cas(GL_widget1,this,Box_name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_cas_ui::~_filter_stippling_cas_ui()
{
  delete Qtw_filter_stippling_cas;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_cas_ui::reset_data()
{
  _filter_stippling_cas::reset_data();
  Qtw_filter_stippling_cas->set_parameter1(parameter1());
  Qtw_filter_stippling_cas->set_parameter2(parameter2());
  Qtw_filter_stippling_cas->set_parameter3(parameter3());
  Qtw_filter_stippling_cas->set_parameter4(parameter4());
  Qtw_filter_stippling_cas->set_parameter5(parameter5());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_cas_ui::show()
{
  Qtw_filter_stippling_cas->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_cas_ui::hide()
{
  Qtw_filter_stippling_cas->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_stippling_cas_ui::get_link()
{
  return Qtw_filter_stippling_cas->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_cas_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(HALFTONING_CAH_KERNEL_SIZE_DEFAULT);
    parameter2(HALFTONING_CAH_EXPONENT_DEFAULT);
    parameter3(HALFTONING_CAH_G_PLUS_DEFAULT);
    parameter4(HALFTONING_CAH_G_MINUS_DEFAULT);
    parameter5(HALFTONING_CAH_K_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["kernel_size"]=="default") parameter1(HALFTONING_CAH_KERNEL_SIZE_DEFAULT);
      else parameter1(atoi(Parameters["kernel_size"].c_str()));
      if (Parameters["exponent"]=="default") parameter2(HALFTONING_CAH_EXPONENT_DEFAULT);
      else parameter2(atof(Parameters["exponent"].c_str()));
      if (Parameters["g+"]=="default") parameter3(HALFTONING_CAH_G_PLUS_DEFAULT);
      else parameter3(atoi(Parameters["g+"].c_str()));
      if (Parameters["g-"]=="default") parameter4(HALFTONING_CAH_G_MINUS_DEFAULT);
      else parameter4(atoi(Parameters["g-"].c_str()));
      if (Parameters["k"]=="default") parameter5(HALFTONING_CAH_K_DEFAULT);
      else parameter5(atof(Parameters["k"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of STIPPLING_CAS filter");
      MsgBox.exec();
      exit(-1);
    };
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_cas_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%d",parameter1());
  Parameters["kernel_size"]=string(Aux);
  sprintf(Aux,"%5.2f",parameter2());
  Parameters["exponent"]=string(Aux);
  sprintf(Aux,"%d",parameter3());
  Parameters["g+"]=string(Aux);
  sprintf(Aux,"%d",parameter4());
  Parameters["g-"]=string(Aux);
  sprintf(Aux,"%5.2f",parameter5());
  Parameters["k"]=string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_stippling_cas::_qtw_filter_stippling_cas(_gl_widget *GL_widget1,_filter_stippling_cas_ui *Filter1,string Box_name)
{
  QString Str;
  char Aux[100];

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

  // Parameter3
  QGroupBox *Group_box_parameter3=new QGroupBox(tr(String_group_box_parameter3.c_str()));
  Group_box_parameter3->setAlignment(Qt::AlignCenter);

  QGridLayout *Grid_parameter3 = new QGridLayout;

  QLabel *Label_parameter3_min= new QLabel(tr(String_label_parameter3_min.c_str()));
  QLabel *Label_parameter3_max= new QLabel(tr(String_label_parameter3_max.c_str()));

  Line_edit_parameter3=new QLineEdit();
  Line_edit_parameter3->setAlignment(Qt::AlignRight);
  Line_edit_parameter3->setReadOnly(true);
  Line_edit_parameter3->setEnabled(false);
  Line_edit_parameter3->setToolTip(tr(String_label_parameter3_tooltip.c_str()));
  Str=Str.number(Filter->parameter3());
  Line_edit_parameter3->setText(Str);

  Slider_parameter3 = new QSlider(Qt::Horizontal);
  Slider_parameter3->setRange(Parameter3_min_value,Parameter3_max_value);
  Slider_parameter3->setSingleStep(Parameter3_single_step);
  Slider_parameter3->setPageStep(Parameter3_page_step);
  Slider_parameter3->setTickInterval(Parameter3_tick_interval);
  Slider_parameter3->setTickPosition(QSlider::TicksRight);
  Slider_parameter3->setTracking(Parameter3_set_tracking);
  Slider_parameter3->setValue(Filter->parameter1());
  Slider_parameter3->setToolTip(tr(String_parameter3_slider_tooltip.c_str()));

  Grid_parameter3->addWidget(Line_edit_parameter3,0,1,Qt::AlignCenter);
  Grid_parameter3->addWidget(Label_parameter3_min,1,0,Qt::AlignRight);
  Grid_parameter3->addWidget(Slider_parameter3,1,1);
  Grid_parameter3->addWidget(Label_parameter3_max,1,2,Qt::AlignLeft);

  Group_box_parameter3->setLayout(Grid_parameter3);

  // Parameter4
  QGroupBox *Group_box_parameter4=new QGroupBox(tr(String_group_box_parameter4.c_str()));
  Group_box_parameter4->setAlignment(Qt::AlignCenter);

  QGridLayout *Grid_parameter4 = new QGridLayout;

  QLabel *Label_parameter4_min= new QLabel(tr(String_label_parameter4_min.c_str()));
  QLabel *Label_parameter4_max= new QLabel(tr(String_label_parameter4_max.c_str()));

  Line_edit_parameter4=new QLineEdit();
  Line_edit_parameter4->setAlignment(Qt::AlignRight);
  Line_edit_parameter4->setReadOnly(true);
  Line_edit_parameter4->setEnabled(false);
  Line_edit_parameter4->setToolTip(tr(String_label_parameter4_tooltip.c_str()));
  Str=Str.number(Filter->parameter4());
  Line_edit_parameter4->setText(Str);

  Slider_parameter4 = new QSlider(Qt::Horizontal);
  Slider_parameter4->setRange(Parameter4_min_value,Parameter4_max_value);
  Slider_parameter4->setSingleStep(Parameter4_single_step);
  Slider_parameter4->setPageStep(Parameter4_page_step);
  Slider_parameter4->setTickInterval(Parameter4_tick_interval);
  Slider_parameter4->setTickPosition(QSlider::TicksRight);
  Slider_parameter4->setTracking(Parameter4_set_tracking);
  Slider_parameter4->setValue(Filter->parameter1());
  Slider_parameter4->setToolTip(tr(String_parameter4_slider_tooltip.c_str()));

  Grid_parameter4->addWidget(Line_edit_parameter4,0,1,Qt::AlignCenter);
  Grid_parameter4->addWidget(Label_parameter4_min,1,0,Qt::AlignRight);
  Grid_parameter4->addWidget(Slider_parameter4,1,1);
  Grid_parameter4->addWidget(Label_parameter4_max,1,2,Qt::AlignLeft);

  Group_box_parameter4->setLayout(Grid_parameter4);

  // Parameter 5
  Group_box_parameter5=new QGroupBox(tr(String_group_box_parameter5.c_str()));
  Group_box_parameter5->setAlignment(Qt::AlignCenter);

  QGridLayout *Grid_parameter5 = new QGridLayout;

  QLabel *Label_parameter5_min= new QLabel(tr(String_label_parameter5_min.c_str()));
  QLabel *Label_parameter5_max= new QLabel(tr(String_label_parameter5_max.c_str()));

  Line_edit_parameter5=new QLineEdit();
  Line_edit_parameter5->setAlignment(Qt::AlignRight);
  Line_edit_parameter5->setReadOnly(true);
  Line_edit_parameter5->setEnabled(false);
  Line_edit_parameter5->setToolTip(tr(String_label_parameter5_tooltip.c_str()));

  sprintf(Aux,"%4.2f",Filter->parameter5());
  Str=Aux;
  Line_edit_parameter5->setText(Str);

  Slider_parameter5 = new QSlider(Qt::Horizontal);
  Slider_parameter5->setRange(Parameter5_min_value,Parameter5_max_value);
  Slider_parameter5->setSingleStep(Parameter5_single_step);
  Slider_parameter5->setPageStep(Parameter5_page_step);
  Slider_parameter5->setTickInterval(Parameter5_tick_interval);
  Slider_parameter5->setTickPosition(QSlider::TicksRight);
  Slider_parameter5->setTracking(Parameter5_set_tracking);
  Slider_parameter5->setValue(Filter->parameter5()*Parameter5_factor);
  Slider_parameter5->setToolTip(tr(String_parameter5_slider_tooltip.c_str()));

  Grid_parameter5->addWidget(Line_edit_parameter5,0,1,Qt::AlignCenter);
  Grid_parameter5->addWidget(Label_parameter5_min,1,0,Qt::AlignRight);
  Grid_parameter5->addWidget(Slider_parameter5,1,1);
  Grid_parameter5->addWidget(Label_parameter5_max,1,2,Qt::AlignLeft);

  Group_box_parameter5->setLayout(Grid_parameter5);

  Vertical_box_main->addWidget(Group_box_parameter1);
  Vertical_box_main->addWidget(Group_box_parameter2);
  Vertical_box_main->addWidget(Group_box_parameter3);
  Vertical_box_main->addWidget(Group_box_parameter4);
  Vertical_box_main->addWidget(Group_box_parameter5);

  Group_box_main->setLayout(Vertical_box_main);

  connect(Slider_parameter1, SIGNAL(valueChanged(int)),this,SLOT(set_parameter1_slot(int)));
  connect(Slider_parameter2, SIGNAL(valueChanged(int)),this,SLOT(set_parameter2_slot(int)));
  connect(Slider_parameter3, SIGNAL(valueChanged(int)),this,SLOT(set_parameter3_slot(int)));
  connect(Slider_parameter4, SIGNAL(valueChanged(int)),this,SLOT(set_parameter4_slot(int)));
  connect(Slider_parameter5, SIGNAL(valueChanged(int)),this,SLOT(set_parameter5_slot(int)));
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_cas::set_parameter1(int Value)
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

void _qtw_filter_stippling_cas::set_parameter2(float Value)
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

void _qtw_filter_stippling_cas::set_parameter3(int Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_parameter3->setText(Str);
  Slider_parameter3->setValue(Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_cas::set_parameter4(int Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_parameter4->setText(Str);
  Slider_parameter4->setValue(Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_cas::set_parameter5(float Value)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%4.2f",Value);
  Str=Aux;
  Line_edit_parameter5->setText(Str);
  Slider_parameter5->setValue(Value*Parameter5_factor);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_cas::set_parameter1_slot(int Size)
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

void _qtw_filter_stippling_cas::set_parameter2_slot(int Size)
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


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_cas::set_parameter3_slot(int Size)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter3->setText(Str);
  Slider_parameter3->setValue(Size);
  Filter->parameter3(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_cas::set_parameter4_slot(int Size)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter4->setText(Str);
  Slider_parameter4->setValue(Size);
  Filter->parameter4(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_cas::set_parameter5_slot(int Size)
{
  QString Str;
  char Aux[100];
  float Value=(float)Size/Parameter5_factor;

  sprintf(Aux,"%4.2f",Value);
  Str=Aux;
  Line_edit_parameter5->setText(Str);
  Filter->parameter5(Value);
  GL_widget->update_effect(Filter->Name);
}
