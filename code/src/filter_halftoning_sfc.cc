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


#include "filter_halftoning_sfc.h"
#include "glwidget.h"

using namespace _f_halftoning_sfc_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_sfc::_filter_halftoning_sfc()
{
  Cluster_size=HALFTONING_SFC_CLUSTER_SIZE_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_HALFTONING_SFC;

  Scaling_factor=1;

  Change_output_image_size=false;
  Use_dots=false;

}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc::reset_data()
{
  Cluster_size=HALFTONING_SFC_CLUSTER_SIZE_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc::step(int &Angle)
{
  while (Angle > 270)     /* Fold ANGLE to be 0, 90, 180, 270 */
    Angle -= 360;
  while (Angle <   0)
    Angle += 360;
  if (Angle == 0){
    Path.at<unsigned char>(Path_y*Width_image+Path_x)=RIGHT; /* +X */
    Path_x++;
  }
  else if (Angle == 90){
    Path.at<unsigned char>(Path_y*Width_image+Path_x)=UP;    /* +Y */
    Path_y++;
  }
  else if (Angle == 180){
    Path.at<unsigned char>(Path_y*Width_image+Path_x)=LEFT;  /* -X */
    Path_x--;
  }
  else if (Angle == 270){
    Path.at<unsigned char>(Path_y*Width_image+Path_x)=DOWN;  /* -Y */
    Path_y--;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc::hilbert(int Orient,int &Angle, int Level1)
{
  if (Level1-- <= 0) return;

  Angle += Orient * 90;
  hilbert(-Orient,Angle,Level1);
  step(Angle);
  Angle -= Orient * 90;
  hilbert(Orient,Angle,Level1);
  step(Angle);
  hilbert(Orient,Angle,Level1);
  Angle -= Orient * 90;
  step(Angle);
  hilbert(-Orient,Angle,Level1);
  Angle += Orient * 90;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc::generate_space_fill()
{
  int Angle=0;

  Path_x=Path_y=0;                         /* start of the trace */
  hilbert(1,Angle,Level);               /* generates a hilbert curve */
  Path.at<unsigned char>(Path_y*Width_image+Path_x)=END; /* indicates end of path */
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc::space_fill(cv::Mat *Input_image0, cv::Mat *Output_image0)
{
  int Accumulator=0, From_x=0,From_y=0, i,x,y;
  char Go_on=true;
  int Direction;

  // resize Path

  Path.create(Input_image0->rows,Input_image0->cols,CV_8U);

  Level=log2f((float)Input_image0->rows);
  Width_image=Input_image0->cols;

  generate_space_fill();    /* generates the spacefilling path    */
  do{         /* repeats until all pixels processed */
    for (x=From_x,y=From_y,i=0; i<Cluster_size && Go_on; i++){
      Direction=Path.at<unsigned char>(y*Width_image+x);
      Accumulator+=Input_image0->at<unsigned char>(y*Width_image+x);  /* gathers the total value of the cluster */
      switch(Direction){
        case UP:   y++; break;
        case DOWN: y--; break;
        case LEFT: x--; break;
        case RIGHT:x++; break;
        default: Go_on=false;     /* end of path ? */
      }
    } /* for */
    Go_on=true;

    for (x=From_x,y=From_y,i=0; i<Cluster_size && Go_on; i++){
      Direction=Path.at<unsigned char>(y*Width_image+x);
      if (Accumulator>=BLACK){  /* precipitates */
        Output_image0->at<unsigned char>(y*Width_image+x)=BLACK;
        Accumulator-=BLACK;
      }
      else Output_image0->at<unsigned char>(y*Width_image+x)=WHITE;
      switch(Direction){
        case UP:   y++; break;
        case DOWN: y--; break;
        case LEFT: x--; break;
        case RIGHT:x++; break;
        default: Go_on=false;     /* end of path ? */
      } /* switch */
      From_x=x; From_y=y;
    }
  } while (Go_on);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc::update()
{
  if (Input_image_0->cols!=Input_image_0->rows || (log2(Input_image_0->cols)-(int)log2(Input_image_0->cols)!=0)){
    QMessageBox MsgBox;
    MsgBox.setText("Filter Space Filling Curve");
    MsgBox.setInformativeText("Warning: the input image must be square and power of 2.");
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.exec();
    copy_input_to_output();
  }
  else{
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

        space_fill(Aux_input_image,Aux_output_image);
        cvtColor(*Aux_output_image,*Output_image_0,cv::COLOR_GRAY2RGB,3);

      }
      else std::cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << std::endl;
    }
    else{// the same number of channels
      space_fill(Aux_input_image,Output_image_0);
    }

    if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
    if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_sfc_ui::_filter_halftoning_sfc_ui(_gl_widget *GL_widget1, std::string Name1)
{
  Name=Name1;
  Qtw_filter_halftoning_sfc=new _qtw_filter_halftoning_sfc(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_sfc_ui::_filter_halftoning_sfc_ui(_gl_widget *GL_widget1, std::map<std::string, std::string> &Parameters, std::string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_halftoning_sfc=new _qtw_filter_halftoning_sfc(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_sfc_ui::~_filter_halftoning_sfc_ui()
{
  delete Qtw_filter_halftoning_sfc;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc_ui::reset_data()
{
  _filter_halftoning_sfc::reset_data();
  Qtw_filter_halftoning_sfc->set_parameter1(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc_ui::show()
{
  Qtw_filter_halftoning_sfc->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc_ui::hide()
{
  Qtw_filter_halftoning_sfc->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_halftoning_sfc_ui::get_link()
{
  return Qtw_filter_halftoning_sfc->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_sfc_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(HALFTONING_SFC_CLUSTER_SIZE_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["cluster_size"]=="default") parameter1(HALFTONING_SFC_CLUSTER_SIZE_DEFAULT);
      else parameter1(atoi(Parameters["cluster_size"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of HALFTONING_SFC filter");
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

void _filter_halftoning_sfc_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%d",parameter1());
  Parameters["cluster_size"]=std::string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_halftoning_sfc::_qtw_filter_halftoning_sfc(_gl_widget *GL_widget1,_filter_halftoning_sfc_ui *Filter1,std::string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

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

  connect(Slider_parameter1, SIGNAL(valueChanged(int)),this,SLOT(set_parameter1_slot(int)));

  Vertical_box_main->addWidget(Group_box_parameter1);

  Group_box_main->setLayout(Vertical_box_main);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_halftoning_sfc::set_parameter1(int Value)
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

void _qtw_filter_halftoning_sfc::set_parameter1_slot(int Size)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter1->setText(Str);
  Filter->parameter1(Size);
  GL_widget->update_effect(Filter->Name);
}
