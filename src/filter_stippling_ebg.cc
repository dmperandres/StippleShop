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


#include "filter_stippling_ebg.h"
#include "glwidget.h"

using namespace _f_stippling_ebg_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_ebg::_filter_stippling_ebg()
{
  Scaling_factor=STIPPLING_EBG_SCALING_FACTOR_DEFAULT;
  Displacement_ramdomness=STIPPLING_EBG_DISPLACEMENT_DEFAULT;
  Num_rows_dot_matrix=STIPPLING_EBG_NUM_ROWS_DOT_MATRIX_DEFAULT;
  Num_cols_dot_matrix=STIPPLING_EBG_NUM_COLS_DOT_MATRIX_DEFAULT;
  Dot_size_min=STIPPLING_EBG_MIN_DOT_SIZE_DEFAULT;
  Dot_size_max=STIPPLING_EBG_MAX_DOT_SIZE_DEFAULT;
  Pixel_density=STIPPLING_EBG_PIXEL_DENSITY_DEFAULT;
  Pixel_density_factor=VEC_PIXEL_DENSITY_FACTOR[Pixel_density];
  Black_level=STIPPLING_EBG_BLACK_LEVEL_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_STIPPLING_EBG;

  Change_output_image_size=true;

  Seeds_initialized=false;
  Dots_texture_packet=0;
  Use_dots=true;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::reset_data()
{
  Scaling_factor=STIPPLING_EBG_SCALING_FACTOR_DEFAULT;
  Displacement_ramdomness=STIPPLING_EBG_DISPLACEMENT_DEFAULT;
  Num_rows_dot_matrix=STIPPLING_EBG_NUM_ROWS_DOT_MATRIX_DEFAULT;
  Num_cols_dot_matrix=STIPPLING_EBG_NUM_COLS_DOT_MATRIX_DEFAULT;
  Dot_size_min=STIPPLING_EBG_MIN_DOT_SIZE_DEFAULT;
  Dot_size_max=STIPPLING_EBG_MAX_DOT_SIZE_DEFAULT;
  Pixel_density=STIPPLING_EBG_PIXEL_DENSITY_DEFAULT;
  Pixel_density_factor=VEC_PIXEL_DENSITY_FACTOR[Pixel_density];
  Black_level=STIPPLING_EBG_BLACK_LEVEL_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::set_dots_images(vector<vector<cv::Mat *> > *Dots1)
{
  Dots=Dots1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::pixel_density(int Pixel_density1)
{
  Pixel_density=Pixel_density1;
  Pixel_density_factor=VEC_PIXEL_DENSITY_FACTOR[Pixel_density];
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::output_image_size(int &Width1,int &Height1)
{
  // the size of the output image must be changed
  Width1=(int)((float)Input_image_0->cols*Scaling_factor*Pixel_density_factor);
  Height1=(int)((float)Input_image_0->rows*Scaling_factor*Pixel_density_factor);

  adjust_image_sizes(Width1,Height1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::put_dot(cv::Mat *Output_image0,float Row1,float Col1,unsigned int Selected_dot_size1,unsigned int Index_row1,unsigned int Index_col1)
{
  int Row,Col;
  int Row_out,Col_out;
  unsigned char Value;
  unsigned int Row_aux=Selected_dot_size1*Index_row1*Pixel_density_factor;
  unsigned int Col_aux=Selected_dot_size1*Index_col1*Pixel_density_factor;

  // All the pixels of the dot are modified
  for (Row=0;Row<(int)(Selected_dot_size1*Pixel_density_factor);Row++){
    for (Col=0;Col<(int)(Selected_dot_size1*Pixel_density_factor);Col++){
      // get the value of the image of dots of size Selected_dot_size1

      Value=(*Dots)[Pixel_density][Selected_dot_size1]->at<unsigned char>(Row+Row_aux,Col+Col_aux);
      // output position
      Row_out=Row1+Row;
      Col_out=Col1+Col;

      if (Row_out<0 || Row_out>=Output_image0->rows || Col_out<0 || Col_out>=Output_image0->cols) continue;
      // the row and col are correct, the color is changed
      Output_image0->at<unsigned char>(Row_out,Col_out)=(unsigned char)((float)Output_image0->at<unsigned char>(Row_out,Col_out)*(float)Value/255.0);
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::stippling(cv::Mat *Input_image0,cv::Mat *Output_image0)
{
  int Row,Col,Selected_dot_size;
  float Pos_row,Pos_col;
  int Selected_row,Selected_col;
  _vertex2f Position;
  unsigned int Counter_of_dots=0;

  if (Seeds_initialized==false){
    Index_row.seed();
    Dot_size.seed();
    Index_col.seed();
    Displacement.seed();
    save_seeds();
    Seeds_initialized=true;
  }
  else load_seeds();

  Index_row.init(0,Num_rows_dot_matrix-1);
  Index_col.init(0,Num_cols_dot_matrix-1);
  Dot_size.init(Dot_size_min,Dot_size_max-1);
  Displacement.init(-Displacement_ramdomness,Displacement_ramdomness);

  for (Row=0;Row<Input_image0->rows;Row++){
    for (Col=0;Col<Input_image0->cols;Col++){
      if (Input_image0->at<unsigned char>(Row,Col)==BLACK){
        // compute a random dot size
        Selected_dot_size=Dot_size.value();

        Counter_of_dots++;
        //
        Selected_row=(int)Index_row.value();
        Selected_col=(int)Index_col.value();

        Pos_row=(float)Row*Scaling_factor*(float)Pixel_density_factor+(Displacement.value()*(float)AVERAGE_DOT_SIZE*(float)Pixel_density_factor)-(float)Selected_dot_size/2;
        Pos_col=(float)Col*Scaling_factor*(float)Pixel_density_factor+(Displacement.value()*(float)AVERAGE_DOT_SIZE*(float)Pixel_density_factor)-(float)Selected_dot_size/2;
        put_dot(Output_image0,Pos_row,Pos_col,Selected_dot_size,Selected_row,Selected_col);
      }
    }
  }

  set_info1(Counter_of_dots);

  if (Output_mode==OUTPUT_MODE_MONO_COLOR){
    for (Row=0;Row<Output_image_0->rows;Row++){
      for (Col=0;Col<Output_image_0->cols;Col++){
        if (Output_image_0->at<unsigned char>(Row,Col)<=Black_level) Output_image_0->at<unsigned char>(Row,Col)=BLACK;
        else Output_image_0->at<unsigned char>(Row,Col)=WHITE;
      }
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::update()
{
  cv::Mat *Aux_input_image=nullptr;
  cv::Mat *Aux_output_image=nullptr;

  int Width1,Height1;

  output_image_size(Width1,Height1);
  // check the output size
  if (Input_image_0->cols!=Width1 || Input_image_0->rows!=Height1){
    Output_image_0->release();
    Output_image_0->create(Height1,Width1,CV_8UC1);
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
      Aux_output_image->create(Output_image_0->rows,Output_image_0->cols,CV_8UC1);

      // set the output image to white
      Output_image_0->setTo(cv::Scalar(255,255,255));
      stippling(Aux_input_image,Aux_output_image);
      cvtColor(*Aux_output_image,*Output_image_0,CV_GRAY2RGB,3);
    }
    else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    // set the output image to white
    Output_image_0->setTo(255);
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

void _filter_stippling_ebg::displacement_ramdomness(float Displacement_ramdomness1)
{
  Displacement_ramdomness=Displacement_ramdomness1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::save_seeds()
{
  Index_row.save_seed();
  Index_col.save_seed();
  Displacement.save_seed();
  Dot_size.save_seed();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::load_seeds()
{
  Index_row.load_seed();
  Index_col.load_seed();
  Displacement.load_seed();
  Dot_size.load_seed();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg::adjust_image_sizes(int &Width1, int &Height1)
{
  float Rest;

  if (Width1%4!=0){
    Rest=round((float)Width1/4.0);
    Width1=(int)Rest*4;

  }
  if (Height1%4!=0){
    Rest=round((float)Height1/4.0);
    Height1=(int)Rest*4;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_ebg_ui::_filter_stippling_ebg_ui(_gl_widget *GL_widget1, string Name1)
{
  Name=Name1;
  Qtw_filter_stippling_ebg=new _qtw_filter_stippling_ebg(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_ebg_ui::_filter_stippling_ebg_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Box_name)
{
  read_parameters(Parameters);
  Name=Box_name;
  Qtw_filter_stippling_ebg=new _qtw_filter_stippling_ebg(GL_widget1,this,Box_name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_ebg_ui::~_filter_stippling_ebg_ui()
{
  delete Qtw_filter_stippling_ebg;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg_ui::reset_data()
{
  _filter_stippling_ebg::reset_data();
  Qtw_filter_stippling_ebg->set_parameter2(parameter2());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg_ui::show()
{
  Qtw_filter_stippling_ebg->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg_ui::hide()
{
  Qtw_filter_stippling_ebg->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_stippling_ebg_ui::get_link()
{
  return Qtw_filter_stippling_ebg->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(STIPPLING_EBG_PIXEL_DENSITY_DEFAULT);
    parameter2(STIPPLING_EBG_DISPLACEMENT_DEFAULT);
    parameter3(STIPPLING_EBG_OUTPUT_MODE_DEFAULT);
    parameter4(STIPPLING_EBG_BLACK_LEVEL_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{

      if (Parameters["pixel_density"]=="default") parameter1(STIPPLING_EBG_PIXEL_DENSITY_DEFAULT);
      else{
        if (Parameters["pixel_density"]=="300ppi") parameter1((int) PIXEL_DENSITY_300PPI);
        if (Parameters["pixel_density"]=="600ppi") parameter1((int) PIXEL_DENSITY_600PPI);
        if (Parameters["pixel_density"]=="1200ppi") parameter1((int) PIXEL_DENSITY_1200PPI);
      }

      if (Parameters["displacement_ramdomness"]=="default") parameter2(STIPPLING_EBG_DISPLACEMENT_DEFAULT);
      else parameter2(atof(Parameters["displacement_ramdomness"].c_str()));

      if (Parameters["black_dots"]=="default") parameter3(STIPPLING_EBG_OUTPUT_MODE_DEFAULT);
      else{
        if (Parameters["black_dots"]=="true") parameter3((int) OUTPUT_MODE_MONO_COLOR);
        else parameter3((int) OUTPUT_MODE_GRAYSCALE);
      }

      if (Parameters["black_threshold"]=="default") parameter4(STIPPLING_EBG_BLACK_LEVEL_DEFAULT);
      else parameter4(atoi(Parameters["black_threshold"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of STIPPLING_EBG filter");
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

void _filter_stippling_ebg_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  switch (parameter1()){
  case 0:Parameters["pixel_density"]=string("300ppi");break;
  case 1:Parameters["pixel_density"]=string("600ppi");break;
  case 2:Parameters["pixel_density"]=string("1200ppi");break;
  }

  sprintf(Aux,"%5.2f",parameter2());
  Parameters["displacement_ramdomness"]=string(Aux);

  if (parameter3()==(int) OUTPUT_MODE_MONO_COLOR) Parameters["black_dots"]=string("true");
  else Parameters["black_dots"]=string("false");

  sprintf(Aux,"%3d",parameter4());
  Parameters["black_threshold"]=string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_ebg_ui::set_info1(unsigned int Value1)
{
  Qtw_filter_stippling_ebg->set_info1(Value1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_stippling_ebg::_qtw_filter_stippling_ebg(_gl_widget *GL_widget1,_filter_stippling_ebg_ui *Filter1,string Box_name)
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

  QVBoxLayout *Vertical_box_parameter1=new QVBoxLayout;

  Combo_box_parameter1 = new QComboBox;
  Combo_box_parameter1->addItem(tr("300ppi"));
  Combo_box_parameter1->addItem(tr("600ppi"));
  Combo_box_parameter1->addItem(tr("1200ppi"));
  Combo_box_parameter1->setCurrentIndex(_f_stippling_ebg_ns::STIPPLING_EBG_PIXEL_DENSITY_DEFAULT);
  Combo_box_parameter1->setToolTip(tr("This value allows to change the output pixel density\n300ppi: mode to print in low resolution mono printers and for on-screen files\n600ppi: mode to print in medium resolution mono printers\n1200ppi"));
  connect(Combo_box_parameter1, SIGNAL(currentIndexChanged(int)), this,SLOT(set_parameter1_slot(int)));

  Vertical_box_parameter1->addWidget(Combo_box_parameter1);

  Group_box_parameter1->setLayout(Vertical_box_parameter1);

  // Parameter2
  QGroupBox *Group_box_parameter2=new QGroupBox(tr(String_group_box_parameter2.c_str()));
  Group_box_parameter2->setAlignment(Qt::AlignCenter);

  QGridLayout *Grid_parameter2 = new QGridLayout;

  QLabel *Label_parameter2_min= new QLabel(tr(String_label_parameter2_min.c_str()));
  QLabel *Label_parameter2_max= new QLabel(tr(String_label_parameter2_max.c_str()));

  Line_edit_parameter2=new QLineEdit();
  Line_edit_parameter2->setAlignment(Qt::AlignRight);
  Line_edit_parameter2->setReadOnly(true);
  Line_edit_parameter2->setEnabled(false);
  Line_edit_parameter2->setToolTip(tr(String_label_parameter2_tooltip.c_str()));
  // warnig to the adjust
  Str=Str.number(Filter->parameter2());
  Line_edit_parameter2->setText(Str);

  Slider_parameter2 = new QSlider(Qt::Horizontal);
  Slider_parameter2->setRange(Parameter2_min_value,Parameter2_max_value);
  Slider_parameter2->setSingleStep(Parameter2_single_step);
  Slider_parameter2->setPageStep(Parameter2_page_step);
  Slider_parameter2->setTickInterval(Parameter2_tick_interval);
  Slider_parameter2->setTickPosition(QSlider::TicksRight);
  Slider_parameter2->setTracking(Parameter2_set_tracking);
  // warnig to the adjust
  Slider_parameter2->setValue(Filter->parameter2()*Parameter2_factor);
  Slider_parameter2->setToolTip(tr(String_parameter2_tooltip.c_str()));

  Grid_parameter2->addWidget(Line_edit_parameter2,0,1,Qt::AlignCenter);
  Grid_parameter2->addWidget(Label_parameter2_min,1,0,Qt::AlignRight);
  Grid_parameter2->addWidget(Slider_parameter2,1,1);
  Grid_parameter2->addWidget(Label_parameter2_max,1,2,Qt::AlignLeft);

  Group_box_parameter2->setLayout(Grid_parameter2);

  connect(Slider_parameter2, SIGNAL(valueChanged(int)),this,SLOT(set_parameter2_slot(int)));

  // parameter 3 output mode
  QGroupBox *Group_box_parameter3=new QGroupBox(tr(String_group_box_parameter3.c_str()));
  Group_box_parameter3->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter3=new QVBoxLayout;

  Combo_box_parameter3 = new QComboBox;
  Combo_box_parameter3->addItem(tr("Gray-scale"));
  Combo_box_parameter3->addItem(tr("Black&White"));
  Combo_box_parameter3->setToolTip(tr("This value allows to change the number of colors of the output\nGray-scale: Normal mode\nBlack-and-white: mode to print in high resolution mono printers. The user must select the black threshold"));

  Vertical_box_parameter3->addWidget(Combo_box_parameter3);

  Group_box_parameter3->setLayout(Vertical_box_parameter3);

  connect(Combo_box_parameter3, SIGNAL(currentIndexChanged(int)), this,SLOT(set_parameter3_slot(int)));

  // parameter 4 Black level
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
  // warnig to the adjust
  Str=Str.number(Filter->parameter4());
  Line_edit_parameter4->setText(Str);

  Slider_parameter4 = new QSlider(Qt::Horizontal);
  Slider_parameter4->setRange(parameter4_min_value,parameter4_max_value);
  Slider_parameter4->setSingleStep(parameter4_single_step);
  Slider_parameter4->setPageStep(parameter4_page_step);
  Slider_parameter4->setTickInterval(parameter4_tick_interval);
  Slider_parameter4->setTickPosition(QSlider::TicksRight);
  Slider_parameter4->setTracking(parameter4_set_tracking);
  // warnig to the adjust
  Slider_parameter4->setValue(Filter->parameter4());
  Slider_parameter4->setToolTip(tr(String_parameter4_tooltip.c_str()));

  Grid_parameter4->addWidget(Line_edit_parameter4,0,1,Qt::AlignCenter);
  Grid_parameter4->addWidget(Label_parameter4_min,1,0,Qt::AlignRight);
  Grid_parameter4->addWidget(Slider_parameter4,1,1);
  Grid_parameter4->addWidget(Label_parameter4_max,1,2,Qt::AlignLeft);

  Group_box_parameter4->setLayout(Grid_parameter4);
  Group_box_parameter4->setEnabled(false);

  connect(Slider_parameter4, SIGNAL(valueChanged(int)),this,SLOT(set_parameter4_slot(int)));

  // info
  QGroupBox *Group_box_info1=new QGroupBox(tr(String_group_box_info1.c_str()));
  Group_box_info1->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Verticalbox_info1 = new QVBoxLayout;

  Line_edit_info1=new QLineEdit();
  Line_edit_info1->setAlignment(Qt::AlignRight);
  Line_edit_info1->setReadOnly(true);
  Line_edit_info1->setEnabled(false);
  // warnig to the adjust
  Str=Str.number(0);
  Line_edit_info1->setText(Str);

  Verticalbox_info1->addWidget(Line_edit_info1);

  Group_box_info1->setLayout(Verticalbox_info1);

  Vertical_box_main->addWidget(Group_box_parameter1);
  Vertical_box_main->addWidget(Group_box_parameter2);
  Vertical_box_main->addWidget(Group_box_parameter3);
  Vertical_box_main->addWidget(Group_box_parameter4);
  Vertical_box_main->addWidget(Group_box_info1);

  Group_box_main->setLayout(Vertical_box_main);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_ebg::set_parameter2(float Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_parameter2->setText(Str);
  Value=Value*(float)Parameter2_factor;
  Slider_parameter2->setValue((int)Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_ebg::set_parameter3(int Value)
{
  Combo_box_parameter3->setCurrentIndex(Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_ebg::set_parameter4(int Value)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%d",Value);
  Str=Aux;
  Line_edit_parameter4->setText(Str);
  Slider_parameter4->setValue(Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_ebg::set_info1(unsigned int Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_info1->setText(Str);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_ebg::set_parameter1_slot(int Value)
{
  Filter->parameter1(Value);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_ebg::set_parameter2_slot(int Size)
{
  QString Str;
  char Aux[100];
  float Value=(float)Size/(float)Parameter2_factor;

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

void _qtw_filter_stippling_ebg::set_parameter3_slot(int Value)
{
  switch((_output_mode) Value){
  case OUTPUT_MODE_GRAYSCALE:
    Group_box_parameter4->setEnabled(false);
    break;
  case OUTPUT_MODE_MONO_COLOR:
    Group_box_parameter4->setEnabled(true);
    break;
  default:break;
  }
  Filter->parameter3(Value);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_ebg::set_parameter4_slot(int Value)
{
QString Str;
char Aux[100];

  sprintf(Aux,"%d",Value);
  Str=Aux;
  Line_edit_parameter4->setText(Str);
  Filter->parameter4(Value);
  GL_widget->update_effect(Filter->Name);
}
