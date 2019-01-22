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


#include "filter_kang.h"
#include "glwidget.h"

using namespace _f_kang_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_kang::_filter_kang()
{
  Radius=RADIUS_DEFAULT;
  Kernel_size_center=KERNEL_SIZE_CENTER_DEFAULT;
  Kernel_size_surround=KERNEL_SIZE_SURROUND_DEFAULT;
  Kernel_size_line_length=KERNEL_SIZE_LINE_LENGTH_DEFAULT;
  Ro=RO_DEFAULT;
  Lines_iterations=LINES_ITERATIONS_DEFAULT;
  Delta_line_width=1.0;
  Delta_line_length=1.0;
  Theta=0.5;
  Eta=ETA_DEFAULT;
  ETF_iterations=ETF_ITERATIONS_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_KANG;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang::reset_data()
{
  Radius=RADIUS_DEFAULT;
  Kernel_size_center=KERNEL_SIZE_CENTER_DEFAULT;
  Kernel_size_surround=KERNEL_SIZE_SURROUND_DEFAULT;
  Kernel_size_line_length=KERNEL_SIZE_LINE_LENGTH_DEFAULT;
  Ro=RO_DEFAULT;
  Lines_iterations=LINES_ITERATIONS_DEFAULT;
  Delta_line_width=1.0;
  Delta_line_length=1.0;
  Theta=0.5;
  Eta=ETA_DEFAULT;
  ETF_iterations=ETF_ITERATIONS_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang::kang(cv::Mat *Aux_input_image,cv::Mat *Aux_output_image)
{
  edge_tangent_flow(Aux_input_image,Aux_output_image);

  cv::Mat Image_lines;
  Image_lines.create(Input_image_0->rows,Input_image_0->cols,CV_32F);
  Image_lines.setTo(255);

  line_extraction(&Image_lines,Aux_input_image);
  Image_lines.convertTo(*Aux_output_image,CV_8U);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang::update()
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
  }
  else{// the same number of channels
    Aux_input_image=Input_image_0;
  }

  // Check the number of output channels
  if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
    if (Output_image_0->channels()==3){
      // conversion
      Aux_output_image=new cv::Mat;
      Aux_output_image->create(Output_image_0->rows,Output_image_0->cols,CV_8UC1);
      kang(Aux_input_image,Aux_output_image);
      cvtColor(*Aux_output_image,*Output_image_0,CV_GRAY2RGB,3);
    }
    else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    kang(Aux_input_image,Output_image_0);
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang::edge_tangent_flow(cv::Mat *Input_image,cv::Mat *Output_image)
{
  Q_UNUSED(Output_image)
  // Gradient
  cv::Mat Gradient_x;
  cv::Mat Gradient_y;
  cv::Mat Gradient_magnitude;

  // Initial ETF
  // it is the perpendicular to the gradient

  for (auto i=0;i<2;i++){
    ETF_x[i].create(Input_image->rows,Input_image->cols,CV_32F);
    ETF_y[i].create(Input_image->rows,Input_image->cols,CV_32F);
  }

  Gradient_x.create(Input_image->rows,Input_image->cols,CV_32F);
  Gradient_y.create(Input_image->rows,Input_image->cols,CV_32F);
  Gradient_magnitude.create(Input_image->rows,Input_image->cols,CV_32F);

  // Gradient -> Sobel in x and y
  cv::Sobel(*Input_image,Gradient_x,CV_32F,1,0,3);
  cv::Sobel(*Input_image,Gradient_y,CV_32F,0,1,3);

  // magnitude
  cv::magnitude(Gradient_x,Gradient_y,Gradient_magnitude);

  // normalization and assigning the perpendicular to the intial ETF
  // given the (x,y) Perpendicular-> (-y,x)
  cv::divide(Gradient_x,Gradient_magnitude,ETF_y[0]); // x -> aux
  cv::divide(Gradient_y,Gradient_magnitude,ETF_x[0],-1); // -y -> x

  // To draw
//  Gradient_magnitude.convertTo(*Output_image,CV_8U);
//  draw_vectors(Input_image,Output_image,&ETF_x[0],&ETF_y[0]);

  float Direction_weight;
  float Magnitude_weight;
  float New_x;
  float New_y;
  float Module;

  // progress bar
  int Count_progress;
  int Num_pass=ETF_iterations*Input_image->rows;
  QProgressDialog Progress("Computing ETF...", "Abort",0,Num_pass);
  Progress.setWindowModality(Qt::WindowModal);
  Progress.setMinimumDuration(0);
  Progress.setCancelButton(0);

  // the process is repeated
  for (auto Iteration=0;Iteration<ETF_iterations;Iteration++){
    // progress bar
    Count_progress=Iteration*Input_image->rows;
    // the image is traversed
    for (auto Row=0;Row<Input_image->rows;Row++){
      // progress bar
      Progress.setValue(Count_progress++);
      if (Progress.wasCanceled()) break;

      for (auto Col=0;Col<Input_image->cols;Col++){
        // the inner loop for the kernel
        New_x=0;
        New_y=0;
        for (auto Kernel_row=-Radius;Kernel_row<Radius;Kernel_row++){
          for (auto Kernel_col=-Radius;Kernel_col<Radius;Kernel_col++){
            // check that the position is included in the image and can be computed
            if ((Row+Kernel_row)>=0 && (Row+Kernel_row)<Input_image->rows && (Col+Kernel_col)>=0 && (Col+Kernel_col)<Input_image->cols){
              // the spatia weight is computed with an if
              if (sqrt(Kernel_row*Kernel_row+Kernel_col*Kernel_col)<Radius){
                // compute the magnitude weight
                Magnitude_weight=0.5*(1+tanh(Eta*(Gradient_magnitude.at<float>(Row+Kernel_row,Col+Kernel_col)-Gradient_magnitude.at<float>(Row,Col))));
                // compute the direction weight as the dot product
                Direction_weight=ETF_x[Iteration%2].at<float>(Row+Kernel_row,Col+Kernel_col)*ETF_x[Iteration%2].at<float>(Row,Col)+ETF_y[Iteration%2].at<float>(Row+Kernel_row,Col+Kernel_col)*ETF_y[Iteration%2].at<float>(Row,Col);
                // see the paper for the equation

                New_x+=ETF_x[Iteration%2].at<float>(Row+Kernel_row,Col+Kernel_col)*Magnitude_weight*Direction_weight;
                New_y+=ETF_y[Iteration%2].at<float>(Row+Kernel_row,Col+Kernel_col)*Magnitude_weight*Direction_weight;

              }
            }
          }
        }

        Module=sqrt(New_x*New_x+New_y*New_y);
        if (Module==0) Module=1;

        ETF_x[(Iteration+1)%2].at<float>(Row,Col)=New_x/Module;
        ETF_y[(Iteration+1)%2].at<float>(Row,Col)=New_y/Module;
      }
    }
  }

  Progress.setValue(Num_pass);
//  Gradient_magnitude.convertTo(*Output_image,CV_8U);
  Final_position=(ETF_iterations-1)%2;
  // To draw
//  draw_vectors(Input_image,Output_image,&ETF_x[Final_position],&ETF_y[Final_position]);
}




/*****************************************************************************//**
 * Given the initial position, tries to create a line. It moves along the direction of the ETF
 * to create the line, but for each pixel, it applies a DOG in the perpendicular direction of the ETF
 * Given the length of the line (Kernel_size_line_legth) in pixels, and the with of the line
 * (Kernel_size_line_width) in pixels, it moves the curren position using the values of the ETF which
 * produces a new position. This movement is from the intial position to -Kernel_size_line_legth and
 * +Kernel_size_line_legth. From the new position, the DOG is applied to the pixels that are in the
 * perpendicular to the ETF (the gradient), from -Kernel_size_line_width to Kernel_size_line_width
 *
 *
 *
 *****************************************************************************/

float  _filter_kang::line(int Initial_row, int Initial_col, int Kernel_size_line_legth, int Kernel_size_line_width,cv::Mat *Image,vector<float> &Difference_of_gaussians,vector<float> &Gauss_line_lenght)
{
  float Output = 0;
  int Central_row = Initial_row;
  int Central_col = Initial_col;
  int Pos_row, Pos_col;
  float Delta_row_width,Delta_col_width;
  float Pos_row_ini,Pos_col_ini;

  // first try to move in the -line_legth part
  for(int Row = 0; Row >= -Kernel_size_line_legth; Row--){
    if(!(Central_row >= 0 && Central_row < Image->rows && Central_col >= 0 && Central_col < Image->cols)) break;

    Delta_col_width= Delta_line_width*ETF_x[Final_position].at<float>(Central_row, Central_col);
    Delta_row_width= Delta_line_width*ETF_y[Final_position].at<float>(Central_row, Central_col);

    Pos_col_ini=(float)Central_col-Delta_row_width*-Kernel_size_line_width;
    Pos_row_ini=(float)Central_row+Delta_col_width*-Kernel_size_line_width;

    for (int i=0;i<2*Kernel_size_line_width+1;i++){
      Pos_row=round((float)Pos_row_ini+Delta_col_width*(float)i);
      Pos_col=round((float)Pos_col_ini-Delta_row_width*(float)i);
      if(Pos_row >= 0 && Pos_row < Image->rows && Pos_col >= 0 && Pos_col < Image->cols){
        Output += (float) Image->at<unsigned char>(Pos_row, Pos_col)*Difference_of_gaussians[i]*Gauss_line_lenght[Row + Kernel_size_line_legth];
      }
    }

    // compute the new central pixel using the ETF
    Central_row = round((float)Central_row - Delta_line_length*ETF_y[Final_position].at<float>(Central_row, Central_col));
    Central_col = round((float)Central_col - Delta_line_length*ETF_x[Final_position].at<float>(Central_row, Central_col));
  }
  // then try to move in the +line_legth part

  // get the new central pixel
  Central_row = round((float)Initial_row + Delta_line_length*ETF_y[Final_position].at<float>(Initial_row, Initial_col));
  Central_col = round((float)Initial_col + Delta_line_length*ETF_x[Final_position].at<float>(Initial_row, Initial_col));

  for(int Row = 1; Row <= Kernel_size_line_legth; Row++){
    if(!(Central_row >= 0 && Central_row < Image->rows && Central_col >= 0 && Central_col < Image->cols)) break;

    // the new deltas
    Delta_col_width=Delta_line_width*ETF_x[Final_position].at<float>(Central_row, Central_col);
    Delta_row_width=Delta_line_width*ETF_y[Final_position].at<float>(Central_row, Central_col);

    Pos_col_ini=(float)Central_col-Delta_row_width*-Kernel_size_line_width;
    Pos_row_ini=(float)Central_row+Delta_col_width*-Kernel_size_line_width;

    for (int i=0;i<2*Kernel_size_line_width+1;i++){
      Pos_row=round((float)Pos_row_ini+Delta_col_width*(float)i);
      Pos_col=round((float)Pos_col_ini-Delta_row_width*(float)i);
      if(Pos_row >= 0 && Pos_row < Image->rows && Pos_col >= 0 && Pos_col < Image->cols){
        Output += (float) Image->at<unsigned char>(Pos_row, Pos_col)*Difference_of_gaussians[i]*Gauss_line_lenght[Row + Kernel_size_line_legth];
      }
    }

    // compute the new central pixel using the ETF
    Central_row = round((float)Central_row + Delta_line_length*ETF_y[Final_position].at<float>(Central_row, Central_col));
    Central_col = round((float)Central_col + Delta_line_length*ETF_x[Final_position].at<float>(Central_row, Central_col));
  }

  return Output;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang::line_extraction(cv::Mat *Image_lines, cv::Mat *Aux_input_image)
{
  cv::Mat Input_image;

  // copy the original image
  Aux_input_image->copyTo(Input_image);

  cv::Mat Gaussian_center=cv::getGaussianKernel(Kernel_size_center,-1,CV_32F);
  cv::Mat Gaussian_surround=cv::getGaussianKernel(Kernel_size_surround,-1,CV_32F);

  // vector with the values of the kernels
  vector<float> Vec_gaussian_center;
  vector<float> Vec_gaussian_surround;
  vector<float> Difference_of_gaussians;
  vector<float> Gauss_line_lenght;

  Difference_of_gaussians.resize(Kernel_size_surround);
  Vec_gaussian_center.resize(Kernel_size_surround);
  Vec_gaussian_surround.resize(Kernel_size_surround);

  //
  for (auto i=0;i<Gaussian_surround.rows;i++) Vec_gaussian_surround[i]=Ro*Gaussian_surround.at<float>(i);
  int Aux=(Kernel_size_surround-Kernel_size_center)/2;
  for (auto i=0;i<Gaussian_center.rows;i++) Vec_gaussian_center[i+Aux]=Gaussian_center.at<float>(i);
  for (unsigned int i=0;i<Difference_of_gaussians.size();i++) Difference_of_gaussians[i]=Vec_gaussian_center[i]-Vec_gaussian_surround[i];

  //
  cv::Mat Gaussian_line=cv::getGaussianKernel(Kernel_size_line_length,-1,CV_32F);
  Gauss_line_lenght.resize(Kernel_size_line_length);
  for(unsigned int i=0;i<Gauss_line_lenght.size(); i++) Gauss_line_lenght[i]=Gaussian_line.at<float>(i);

  // progress bar
  int Count_progress;
  int Num_pass=Lines_iterations*Image_lines->rows;
  QProgressDialog Progress("Computing lines...", "Abort",0,Num_pass);
  Progress.setWindowModality(Qt::WindowModal);
  Progress.setMinimumDuration(0);
  Progress.setCancelButton(0);

  // repeat the process
  for(auto Iteration = 0; Iteration < Lines_iterations; Iteration++){
    // progress bar
    Count_progress=Iteration*Input_image.rows;
    // create the values
    for(auto Row = 0; Row < Image_lines->rows; Row++){
      // progress bar
      Progress.setValue(Count_progress++);
      if (Progress.wasCanceled()) break;
      for(auto Col = 0; Col < Image_lines->cols; Col++){
        // for each pixel treats to create a line using the ETF
        // the kernel sizes
        Image_lines->at<float>(Row,Col)=line(Row, Col, Kernel_size_line_length/2, Kernel_size_surround/2, &Input_image,Difference_of_gaussians, Gauss_line_lenght);
      }
    }

    // create the image of lines
    for(auto Row = 0; Row < Image_lines->rows; Row++){
      for(auto Col = 0; Col < Image_lines->cols; Col++){
        if(Image_lines->at<float>(Row,Col) < 0 && (1 + tanh(Image_lines->at<float>(Row,Col))) < Theta){
          Image_lines->at<float>(Row,Col)=0;
//          Input_image.at<unsigned char>(Row,Col)=0;
        }
        else Image_lines->at<float>(Row,Col)=255;
      }
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang::draw_vectors(cv::Mat *Input_image,cv::Mat *Output_image,cv::Mat *X,cv::Mat *Y)
{
  float x,y;
  float x1,y1;
  int Factor=16;
  int Half=Factor/2;
  float Size=(float)Half;

  cv::resize(*Input_image,*Output_image,cv::Size(0,0), Factor,Factor,cv::INTER_NEAREST);

  for (auto Row=0;Row<Input_image->rows;Row++){
    for (auto Col=0;Col<Input_image->cols;Col++){
      x=X->at<float>(Row,Col);
      y=Y->at<float>(Row,Col);
      x1=Size*x;
      y1=Size*y;
      cv::arrowedLine(*Output_image,cv::Point2i(Col*Factor+Half,Row*Factor+Half),cv::Point2i(Col*Factor+Half+round(x1),Row*Factor+Half+round(y1)),cv::Scalar(0),1,8,0,0.5);
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_kang_ui::_filter_kang_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_kang=new _qtw_filter_kang(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_kang_ui::_filter_kang_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_kang=new _qtw_filter_kang(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_kang_ui::~_filter_kang_ui()
{
  delete Qtw_filter_kang;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang_ui::reset_data()
{
  _filter_kang::reset_data();
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang_ui::show()
{
  Qtw_filter_kang->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang_ui::hide()
{
  Qtw_filter_kang->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_kang_ui::get_link()
{
  return Qtw_filter_kang->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_kang_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(RADIUS_DEFAULT);
    parameter2(KERNEL_SIZE_LINE_LENGTH_DEFAULT);
    parameter3(KERNEL_SIZE_SURROUND_DEFAULT);
    parameter4(KERNEL_SIZE_CENTER_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["radius"]=="default") parameter1(RADIUS_DEFAULT);
      else parameter1(atoi(Parameters["radius"].c_str()));
      if (Parameters["line_lenght"]=="default") parameter2(KERNEL_SIZE_LINE_LENGTH_DEFAULT);
      else parameter2(atoi(Parameters["line_lenght"].c_str()));
      if (Parameters["surround_kernel_size"]=="default") parameter3(KERNEL_SIZE_SURROUND_DEFAULT);
      else parameter3(atoi(Parameters["surround_kernel_size"].c_str()));
      if (Parameters["center_kernel_size"]=="default") parameter4(KERNEL_SIZE_CENTER_DEFAULT);
      else parameter4(atoi(Parameters["center_kernel_size"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of KANG filter");
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

void _filter_kang_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%d",parameter1());
  Parameters["radius"]=string(Aux);
  sprintf(Aux,"%d",parameter2());
  Parameters["line_lenght"]=string(Aux);
  sprintf(Aux,"%d",parameter3());
  Parameters["surround_kernel_size"]=string(Aux);
  sprintf(Aux,"%d",parameter4());
  Parameters["center_kernel_size"]=string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_kang::_qtw_filter_kang(_gl_widget *GL_widget1,_filter_kang_ui *Filter1,string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);
//  Group_box_main->hide();

  QVBoxLayout *Vertical_box_main=new QVBoxLayout;

  // Parameter1
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
  Slider_parameter1->setValue(Filter->parameter1());
  Slider_parameter1->setToolTip(tr(String_parameter1_slider_tooltip.c_str()));

  Grid_parameter1->addWidget(Line_edit_parameter1,0,1,Qt::AlignCenter);
  Grid_parameter1->addWidget(Label_parameter1_min,1,0,Qt::AlignRight);
  Grid_parameter1->addWidget(Slider_parameter1,1,1);
  Grid_parameter1->addWidget(Label_parameter1_max,1,2,Qt::AlignLeft);

  Group_box_parameter1->setLayout(Grid_parameter1);

  connect(Slider_parameter1, SIGNAL(valueChanged(int)),this,SLOT(parameter1_slot(int)));

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

  connect(Slider_parameter2, SIGNAL(valueChanged(int)),this,SLOT(parameter2_slot(int)));


  // parameter 3
  Group_box_parameter3=new QGroupBox(tr(String_group_box_parameter3.c_str()));
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
  Slider_parameter3->setValue(Filter->parameter3());
  Slider_parameter3->setToolTip(tr(String_parameter3_slider_tooltip.c_str()));

  Grid_parameter3->addWidget(Line_edit_parameter3,0,1,Qt::AlignCenter);
  Grid_parameter3->addWidget(Label_parameter3_min,1,0,Qt::AlignRight);
  Grid_parameter3->addWidget(Slider_parameter3,1,1);
  Grid_parameter3->addWidget(Label_parameter3_max,1,2,Qt::AlignLeft);

  Group_box_parameter3->setLayout(Grid_parameter3);

  connect(Slider_parameter3, SIGNAL(valueChanged(int)),this,SLOT(parameter3_slot(int)));

  // parameter 4
  Group_box_parameter4=new QGroupBox(tr(String_group_box_parameter4.c_str()));
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
  Slider_parameter4->setValue(Filter->parameter4());
  Slider_parameter4->setToolTip(tr(String_parameter4_slider_tooltip.c_str()));

  Grid_parameter4->addWidget(Line_edit_parameter4,0,1,Qt::AlignCenter);
  Grid_parameter4->addWidget(Label_parameter4_min,1,0,Qt::AlignRight);
  Grid_parameter4->addWidget(Slider_parameter4,1,1);
  Grid_parameter4->addWidget(Label_parameter4_max,1,2,Qt::AlignLeft);

  Group_box_parameter4->setLayout(Grid_parameter4);

  connect(Slider_parameter4, SIGNAL(valueChanged(int)),this,SLOT(parameter4_slot(int)));

  //
  Vertical_box_main->addWidget(Group_box_parameter1);
  Vertical_box_main->addWidget(Group_box_parameter2);
  Vertical_box_main->addWidget(Group_box_parameter3);
  Vertical_box_main->addWidget(Group_box_parameter4);

  Group_box_main->setLayout(Vertical_box_main);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_kang::parameter1(int Value)
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

void _qtw_filter_kang::parameter2(int Value)
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

void _qtw_filter_kang::parameter3(int Value)
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

void _qtw_filter_kang::parameter4(int Value)
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

void _qtw_filter_kang::parameter1_slot(int Size)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter1->setText(Str);
  Filter->parameter1(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_kang::parameter2_slot(int Size)
{
  QString Str;
  char Aux[100];

  if (Size%2==0) Size++;

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter2->setText(Str);
  Filter->parameter2(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_kang::parameter3_slot(int Size)
{
  QString Str;
  char Aux[100];

  if (Size%2==0) Size++;
  if (Size<=Filter->parameter4()) Size=Filter->parameter4()+2;
  Slider_parameter3->blockSignals(true);
  Slider_parameter3->setValue(Size);
  Slider_parameter3->blockSignals(false);

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter3->setText(Str);
  Filter->parameter3(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_kang::parameter4_slot(int Size)
{
  QString Str;
  char Aux[100];

  if (Size%2==0) Size++;
  if (Size>=Filter->parameter3()) Size=Filter->parameter3()-2;
  Slider_parameter4->blockSignals(true);
  Slider_parameter4->setValue(Size);
  Slider_parameter4->blockSignals(false);

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter4->setText(Str);
  Filter->parameter4(Size);
  GL_widget->update_effect(Filter->Name);
}
