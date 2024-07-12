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


#include "filter_distance_field.h"
#include "glwidget.h"

using namespace _f_distance_field_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_distance_field::_filter_distance_field()
{
  Line_width=DISTANCE_FIELD_LINE_WIDTH_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_DISTANCE_FIELD;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_distance_field::reset_data()
{
  Line_width=DISTANCE_FIELD_LINE_WIDTH_DEFAULT;
}


/*****************************************************************************//**
 * jump-flooding method
 *
 *
 *
 *****************************************************************************/

void _filter_distance_field::jump_flooding(cv::Mat *Input_image,cv::Mat *Output_image)
{
  std::vector<_f_distance_field_ns::_point> Image1;
  std::vector<_f_distance_field_ns::_point> Image2;
  std::vector<_f_distance_field_ns::_point> *Read,*Write;
  _f_distance_field_ns::_point Point;
  _f_distance_field_ns::_point Point_aux;
  int Step;
  bool Read_image1;
  int Pos;
  int Pos1;
  float Distance;
  float Distance_aux;

  Image1.resize(Input_image->total());
  Image2.resize(Input_image->total());

  // asigning the seeds (distance=0,x0,y0)
  for (int i=0;i<Input_image->rows;i++){
    for (int j=0;j<Input_image->cols;j++){
      if (Input_image->at<unsigned char>(i,j)==0){
        // this is a seed
        Point.Distance=0;
        Point.x=j;
        Point.y=i;
        Image1[i*Input_image->cols+j]=Point;
      }
    }

  }

  if (Input_image->cols>=Input_image->rows) Step=Input_image->cols/2;
  else Step=Input_image->rows/2;

  Read_image1=true;

  while (Step>=1){
    // select the reading buffer
    if (Read_image1==true){
      Read=&Image1;
      Write=&Image2;
    }
    else{
      Write=&Image1;
      Read=&Image2;
    }

    // iterate all the pixels
    for (int i=0;i<Input_image->rows;i++){
      for (int j=0;j<Input_image->cols;j++){
        Pos=i*Input_image->cols+j;

        // Closest seed (if any)
        Point=(*Read)[Pos];
        // Go ahead and write our current closest seed, if any. If we don't do this
        // we might lose this information if we don't update our seed this round.
        (*Write)[Pos]=Point;

        // this is a seed, skip it
        if (Point.x == j && Point.y ==i) continue;

        if(Point.x == -1 || Point.y== -1 ){
          // No closest seed has been found yet
          Distance = -1;
        }
        else{
         // Current closest seed's distance
         Distance = sqrtf((Point.x-j)*(Point.x-j) + (Point.y-i)*(Point.y-i));
        }

        int Pos_y;
        int Pos_x;
        for( int k = -1; k <= 1; k++ ){
          for( int l = -1; l <= 1; l++ ){
            if (k!=0 || l!=0){
              // not to check with itself

              // Calculate neighbor's row and column
              Pos_y = i + k * Step;
              Pos_x = j + l * Step;

              // If the neighbor is outside the bounds of the buffer, skip it
              if( Pos_x < 0 || Pos_x >= Input_image->cols || Pos_y < 0 || Pos_y >= Input_image->rows ) continue;

              // Calculate neighbor's absolute index
              Pos1 =(Pos_y*Input_image->cols)+Pos_x;

              // Retrieve the neighbor
              Point_aux=(*Read)[Pos1];

              // If the neighbor doesn't have a closest seed yet, skip it
              if(Point_aux.x==-1 || Point_aux.y==-1) continue;

              // Calculate the distance from us to the neighbor's closest seed
              Distance_aux=sqrtf((Point_aux.x-j)*(Point_aux.x-j) + (Point_aux.y-i)*(Point_aux.y-i));

              // If dist is -1, it means we have no closest seed, so we might as well take this one
              // Otherwise, only adopt this new seed if it's closer than our current closest seed
              if (Distance==-1 || Distance_aux<Distance) {
                Point_aux.Distance=Distance_aux;
                (*Write)[Pos]=Point_aux;
                Distance=Distance_aux;
              }
            }
          }
        }
      }
    }

    // Half the step.
    Step /= 2;
    // Swap the buffers for the next round
    Read_image1=!Read_image1;
  }

  float Value;
  for (unsigned int i=0;i<Input_image->total();i++){
    Value=(*Write)[i].Distance;
    Value=Value/(float) Line_width;
    if ((int)Value%2==0) Value=255;
    else Value=0;
    Output_image->at<unsigned char>(i)=(int)Value;

  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_distance_field::update()
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
  if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
    if (Output_image_0->channels()==3){
      // conversion
      Aux_output_image=new cv::Mat;
      Aux_output_image->create(Output_image_0->rows,Output_image_0->cols,CV_8UC1);

      jump_flooding(Aux_input_image,Aux_output_image);
      cvtColor(*Aux_output_image,*Output_image_0,cv::COLOR_GRAY2RGB,3);
    }
    else std::cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << std::endl;
  }
  else{// the same number of channels
    jump_flooding(Aux_input_image,Output_image_0);
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_distance_field_ui::_filter_distance_field_ui(_gl_widget *GL_widget1,std::string Name1)
{
  Name=Name1;
  Qtw_filter_distance_field=new _qtw_filter_distance_field(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_distance_field_ui::_filter_distance_field_ui(_gl_widget *GL_widget1, std::map<std::string,std::string> &Parameters, std::string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_distance_field=new _qtw_filter_distance_field(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_distance_field_ui::~_filter_distance_field_ui()
{
  delete Qtw_filter_distance_field;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_distance_field_ui::reset_data()
{
  _filter_distance_field::reset_data();
  Qtw_filter_distance_field->set_parameter1(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_distance_field_ui::show()
{
  Qtw_filter_distance_field->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_distance_field_ui::hide()
{
  Qtw_filter_distance_field->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_distance_field_ui::get_link()
{
  return Qtw_filter_distance_field->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_distance_field_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(DISTANCE_FIELD_LINE_WIDTH_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["line_width"]=="default") parameter1(DISTANCE_FIELD_LINE_WIDTH_DEFAULT);
      else parameter1(atoi(Parameters["line_width"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of DISTACE FIELD filter");
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

void _filter_distance_field_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%d",parameter1());
  Parameters["line_width"]=std::string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_distance_field::_qtw_filter_distance_field(_gl_widget *GL_widget1,_filter_distance_field_ui *Filter1,std::string Box_name)
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
  Slider_parameter1->setValue(Filter->parameter1());
  // warnig to the adjust
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

void _qtw_filter_distance_field::set_parameter1(int Value)
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

void _qtw_filter_distance_field::set_parameter1_slot(int Size)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%d",Size);
  Str=Aux;
  Line_edit_parameter1->setText(Str);
  Filter->parameter1(Size);
  GL_widget->update_effect(Filter->Name);
}
