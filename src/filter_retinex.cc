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


#include "filter_retinex.h"
#include "glwidget.h"

using namespace _f_retinex_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_retinex::_filter_retinex()
{
  Kernel_size=255;
  Num_kernels=RETINEX_NUM_KERNELS_DEFAULT;
  Color_restoration_variance=RETINEX_COLOR_RESTORATION_VARIANCE_DEFAULT;

  // compute kernel sizes in a uniform way
  if (Num_kernels>1){
    float Step=(float)(Kernel_size-3)/(float) (Num_kernels-1);
    for (int i=0;i<Num_kernels;i++){
        Vec_kernel_size[i]=(float)i*Step+3.0;
        if (Vec_kernel_size[i]%2==0) Vec_kernel_size[i]++;
    }
  }
  else Vec_kernel_size[0]=128;

  Num_channels_input_image_0=3;
  Num_channels_output_image_0=3;

  Type_filter=_f_filter_ns::FILTER_RETINEX;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_retinex::reset_data()
{
  Kernel_size=255;
  Num_kernels=RETINEX_NUM_KERNELS_DEFAULT;
  Color_restoration_variance=RETINEX_COLOR_RESTORATION_VARIANCE_DEFAULT;

  // compute kernel sizes in a uniform way
  if (Num_kernels>1){
    float Step=(float)(Kernel_size-3)/(float) (Num_kernels-1);
    for (int i=0;i<Num_kernels;i++){
        Vec_kernel_size[i]=(float)i*Step+3.0;
        if (Vec_kernel_size[i]%2==0) Vec_kernel_size[i]++;
    }
  }
  else Vec_kernel_size[0]=128;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_retinex::update()
{
  // definition of iterators
  cv::MatIterator_<float> Iter_start0;
  cv::MatIterator_<float> Iter_end0;
  cv::MatIterator_<float> Iter_start1;
  cv::MatIterator_<float> Iter_start2;
  cv::MatIterator_<float> Iter_start3;
  cv::MatIterator_<float> Iter_start4;
  cv::MatIterator_<float> Iter_start5;

  // Computes the sizes of filters according to the number of filters and their distribution

  // check the output size
  if (Input_image_0->cols!=Output_image_0->cols || Input_image_0->rows!=Output_image_0->rows){
    Output_image_0->release();
    Output_image_0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
  }

  // computes the weight to apply when there are several filters -> MSR
  float Weight = 1./ (float) Num_kernels;

  vector<cv::Mat> Vec_channels_in_uc;
  vector<cv::Mat> Vec_channels_in_float;
  vector<cv::Mat> Vec_channels_in_float_log;
  vector<cv::Mat> Vec_channels_out_float;
  vector<cv::Mat> Vec_channels_out_uc;

  if (Input_image_0->rows<255 || Input_image_0->cols<255){
    cout << "Bug: one dimension of the input image is less than 255" << endl;
    QMessageBox Message_box;
    Message_box.setWindowTitle(QObject::tr("Warning"));
    Message_box.setText(QObject::tr("The size of the input of the Retinex filter must grether or equal to 256 pixels"));
    Message_box.exec();
    return;
  }

  // first check the number of channels
  if (Input_image_0->channels()!=Num_channels_input_image_0){
    // error withour conversion
    QMessageBox Message_box;
    Message_box.setWindowTitle(QObject::tr("Warning"));
    Message_box.setText(QObject::tr("The input of the Retinex filter must be conected to a color image"));
    Message_box.exec();
    return;
  }

  // split the RGB image in channels
  Vec_channels_in_uc.resize(Input_image_0->channels());

  cv::split(*Input_image_0,Vec_channels_in_uc);

  Vec_channels_in_float.resize(Vec_channels_in_uc.size());

  // for each channel 0=R,1=G,2=B convert from unsigned char to float
  for (unsigned int i=0;i<Vec_channels_in_uc.size();i++){
      Vec_channels_in_uc[i].convertTo(Vec_channels_in_float[i],CV_32F);
  }
  // copy the original channels and computing the log.
  // This will speed the computing of the retinex
  Vec_channels_in_float_log.resize(Vec_channels_in_float.size());
  for (unsigned int i=0;i<Vec_channels_in_float.size();i++){
      Vec_channels_in_float_log[i]=Vec_channels_in_float[i].clone();

      // compute the log of each pixel (A)
      Iter_start0= Vec_channels_in_float_log[i].begin<float>();
      Iter_end0=Vec_channels_in_float_log[i].end<float>();
      for (;Iter_start0!=Iter_end0;Iter_start0++){
        // it is added 1 to supress the posibility of computing log(0)=infinite
        *Iter_start0=log(*Iter_start0+1.0);
      }
  }

  // create the out channels as float buffers
  Vec_channels_out_float.resize(Vec_channels_in_float.size());
  for (unsigned int i=0;i<Vec_channels_in_float.size();i++){
    Vec_channels_out_float[i]=cv::Mat::zeros(Input_image_0->rows,Input_image_0->cols,CV_32F);
  }

  // auxiliar buffers (float)
  cv::Mat In,Out;

  //
  Out.create(Input_image_0->rows,Input_image_0->cols,Input_image_0->type());

  // for each channel 0=R,1=G,2=B
  for (unsigned int i=0;i<Vec_channels_in_float.size();i++){
    // the different size kernels are applied and added to obtain the final result
    for (int j=0;j<Num_kernels;j++){
      // copy the values of the original image channel and adds 1
      In=Vec_channels_in_float[i].clone();
      In+=1.0;

      cv::GaussianBlur(In, Out, cv::Size(Vec_kernel_size[j], Vec_kernel_size[j]), 0,0);

      Iter_start0=Out.begin<float>();
      Iter_end0=Out.end<float>();
      Iter_start1= Vec_channels_in_float_log[i].begin<float>();
      Iter_start2= Vec_channels_out_float[i].begin<float>();
      for (;Iter_start0!=Iter_end0;Iter_start0++,Iter_start1++,Iter_start2++){
        // we use the Log image previously computed (Vec_channels_in_float_log)
        (*Iter_start2)+=Weight*((*Iter_start1)-log(*Iter_start0));
      }
    In.~Mat();
    }
  }

  // Compute Color Restoration
  // Ci(x,y)=log[a Ii(x,y)]-log[ Ii(x,y)]_R + Ii(x,y)]_G + Ii(x,y)]_B]
  // It is unkown where the values come from

  float Alpha  = 128.;
  float Gain   = 1.;
  float Offset = 0.;
  float Log_rgb_value;

  // Vec_channels_out_float contains the result of MSR
  // Vec_channels_in_float_log contais de log values of the initial image

  Iter_start0=Vec_channels_out_float[0].begin<float>(); // out R
  Iter_end0=Vec_channels_out_float[0].end<float>();
  Iter_start1=Vec_channels_out_float[1].begin<float>(); // out G
  Iter_start2=Vec_channels_out_float[2].begin<float>(); // out B

  Iter_start3= Vec_channels_in_float[0].begin<float>(); // in R
  Iter_start4= Vec_channels_in_float[1].begin<float>(); // in G
  Iter_start5= Vec_channels_in_float[2].begin<float>(); // in B

  // each value for each channel depends on the log of the own value and the log of the RGB value
  for (;Iter_start0!=Iter_end0;Iter_start0++,Iter_start1++,Iter_start2++,Iter_start3++,Iter_start4++,Iter_start5++){
    // Compute the log of input image log(R+G+B). It is added 3 as previously
    Log_rgb_value=log(*Iter_start3+*Iter_start4+*Iter_start5+3.0);
    // Compute the out result for each channel
    *Iter_start0=Gain*((*Iter_start0)*(log(Alpha*(*Iter_start3+1.0))-Log_rgb_value))+Offset;
    *Iter_start1=Gain*(*Iter_start1*(log(Alpha*(*Iter_start4+1.0))-Log_rgb_value))+Offset;
    *Iter_start2=Gain*(*Iter_start2*(log(Alpha*(*Iter_start5+1.0))-Log_rgb_value))+Offset;
  }

  // Adapt the dynamics of the colors according to the statistics of the first and second order.
  // The use of the variance makes it possible to control the degree of saturation of the colors.

  // Compute mean and variance
  double Variance_squared;
  double Mean;
  double Variance;
  double Min,Max,Range;

  Variance_squared=0;
  Mean=0;

  Iter_start0=Vec_channels_out_float[0].begin<float>(); // out R
  Iter_end0=Vec_channels_out_float[0].end<float>();
  Iter_start1=Vec_channels_out_float[1].begin<float>(); // out G
  Iter_start2=Vec_channels_out_float[2].begin<float>(); // out B

  for (;Iter_start0!=Iter_end0;Iter_start0++,Iter_start1++,Iter_start2++){
    Mean+=*Iter_start0+*Iter_start1+*Iter_start2; // R+G+B
    Variance_squared+=(*Iter_start0)*(*Iter_start0)+(*Iter_start1)*(*Iter_start1)+(*Iter_start2)*(*Iter_start2);
  }

  Mean/=(float) Vec_channels_out_float[0].total()*3; // mean
  Variance_squared/=(float) Vec_channels_out_float[0].total()*3; // mean (x^2)
  Variance=Variance_squared-Mean*Mean;
  Variance=sqrt(Variance);
  // end of computing mean and variance

  // Compute the Max and Min values depending on the user variable
  Min=Mean-Color_restoration_variance*Variance;
  Max=Mean+Color_restoration_variance*Variance;

  Range=Max-Min;

  if (Range==0) Range=1.0;

  Iter_start0=Vec_channels_out_float[0].begin<float>(); // out R
  Iter_end0=Vec_channels_out_float[0].end<float>();
  Iter_start1=Vec_channels_out_float[1].begin<float>(); // out G
  Iter_start2=Vec_channels_out_float[2].begin<float>(); // out B

  for (;Iter_start0!=Iter_end0;Iter_start0++,Iter_start1++,Iter_start2++){
    *Iter_start0=255.0*(*Iter_start0-Min)/Range; // R
    if (*Iter_start0<0.0) *Iter_start0=0.0;
    if (*Iter_start0>255.0) *Iter_start0=255.0;

    *Iter_start1=255.0*(*Iter_start1-Min)/Range; // G
    if (*Iter_start1<0.0) *Iter_start1=0.0;
    if (*Iter_start1>255.0) *Iter_start1=255.0;

    *Iter_start2=255.0*(*Iter_start2-Min)/Range; // B
    if (*Iter_start2<0.0) *Iter_start2=0.0;
    if (*Iter_start2>255.0) *Iter_start2=255.0;
  }

  // creating unsigned char buffers
  Vec_channels_out_uc.resize(Vec_channels_out_float.size());

  // for each channel 0=R,1=G,2=B convert from float to unsigned char
  for (unsigned int i=0;i<Vec_channels_out_float.size();i++){
      Vec_channels_out_float[i].convertTo(Vec_channels_out_uc[i],CV_8U);
  }

  // Check the number of output channels
  if (Output_image_0->channels()==1){// conversion 3 to 1 channel
    cv::Mat *Aux_output_image;
    Aux_output_image=new cv::Mat;
    Aux_output_image->create(Input_image_0->rows,Input_image_0->cols,CV_8UC3);
    // joining the channels to the final image
    merge(Vec_channels_out_uc,*Aux_output_image);
    // from rgb to gray
    cvtColor(*Aux_output_image,*Output_image_0,CV_RGB2GRAY,1);

    delete Aux_output_image;
  }
  else{// the same number of channels
    // joining the channels to the final image
    merge(Vec_channels_out_uc,*Output_image_0);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_retinex::color_restoration_variance(float Color_restoration_variance1)
{
  Color_restoration_variance=Color_restoration_variance1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_retinex::kernel_size(unsigned int Pos,int Value)
{
  if ((int)Pos<Num_kernels) Vec_kernel_size[Pos]=Value;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_retinex_ui::_filter_retinex_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_retinex=new _qtw_filter_retinex(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_retinex_ui::_filter_retinex_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_retinex=new _qtw_filter_retinex(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_retinex_ui::~_filter_retinex_ui()
{
  delete Qtw_filter_retinex;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_retinex_ui::reset_data()
{
  _filter_retinex::reset_data();
  Qtw_filter_retinex->set_parameter1(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_retinex_ui::show()
{
  Qtw_filter_retinex->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_retinex_ui::hide()
{
  Qtw_filter_retinex->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_retinex_ui::get_link()
{
  return Qtw_filter_retinex->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_retinex_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(RETINEX_COLOR_RESTORATION_VARIANCE_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["color_restoration_variance"]=="default") parameter1(RETINEX_COLOR_RESTORATION_VARIANCE_DEFAULT);
      else parameter1(atof(Parameters["color_restoration_variance"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of RETINEX filter");
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

void _filter_retinex_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%5.2f",parameter1());
  Parameters["color_restoration_variance"]=string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_retinex::_qtw_filter_retinex(_gl_widget *GL_widget1,_filter_retinex_ui *Filter1,string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_main=new QVBoxLayout;

    // parameter 1
  Group_box_parameter1=new QGroupBox(tr(String_group_box_parameter1.c_str()));
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
  Slider_parameter1->setToolTip(tr(String_parameter1_slider_tooltip.c_str()));
  set_parameter1(Filter->parameter1());

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

_qtw_filter_retinex::~_qtw_filter_retinex()
{

}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_retinex::set_parameter1_slot(int Value)
{
  QString Str;
  char Aux[100];
  float Variance;

  Variance=(float)RETINEX_MAX_COLOR_RESTORATION_VARIANCE*(1-((255.0-(float)Value)/255.0));
  sprintf(Aux,"%2.2f",Variance);
  Str=Aux;
  Line_edit_parameter1->setText(Str);
  Filter->parameter1(Variance);
  GL_widget->update_effect(Filter->Name);
}



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_retinex::set_parameter1(float Value)
{
  QString Str;
  char Aux[100];
  int Int_value;

  Int_value=(int)(255.0*(Value/(float)RETINEX_MAX_COLOR_RESTORATION_VARIANCE));
  sprintf(Aux,"%2.2f",Value);
  Str=Aux;
  Line_edit_parameter1->setText(Str);
  Slider_parameter1->setValue(Int_value);
}
