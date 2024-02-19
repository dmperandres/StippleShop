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


#include "filter_dot_ebg.h"
#include "glwidget.h"

using namespace _f_dot_ebg_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_dot_ebg::_filter_dot_ebg()
{
  Scaling_factor=DOT_EBG_SCALING_FACTOR_DEFAULT;
  Num_rows_dot_matrix=DOT_EBG_NUM_ROWS_DOT_MATRIX_DEFAULT;
  Num_cols_dot_matrix=DOT_EBG_NUM_COLS_DOT_MATRIX_DEFAULT;
  Dot_size_min=DOT_EBG_MIN_DOT_SIZE_DEFAULT;
  Dot_size_max=DOT_EBG_MAX_DOT_SIZE_DEFAULT;
  Pixel_density=DOT_EBG_PIXEL_DENSITY_DEFAULT;
  Pixel_density_factor=VEC_PIXEL_DENSITY_FACTOR[Pixel_density];

  Num_channels_input_image_0=1;
  Num_channels_input_image_1=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_DOT_EBG;

  Change_output_image_size=DOT_EBG_CHANGE_OUTPUT_IMAGE_SIZE_DEFAULT;

  Seeds_initialized=false;
  Dots_texture_packet=0;
  Use_dots=true;
  Modulate_dot_size=DOT_EBG_MODULATE_DOT_SIZE_DEFAULT;
  Black_dots=false;
  Black_threshold=DOT_EBG_BLACK_THRESHOLD_DEFAULT;

  Dots=nullptr;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg::reset_data()
{
  Scaling_factor=DOT_EBG_SCALING_FACTOR_DEFAULT;
  Num_rows_dot_matrix=DOT_EBG_NUM_ROWS_DOT_MATRIX_DEFAULT;
  Num_cols_dot_matrix=DOT_EBG_NUM_COLS_DOT_MATRIX_DEFAULT;
  Dot_size_min=DOT_EBG_MIN_DOT_SIZE_DEFAULT;
  Dot_size_max=DOT_EBG_MAX_DOT_SIZE_DEFAULT;
  Pixel_density=DOT_EBG_PIXEL_DENSITY_DEFAULT;
  Pixel_density_factor=VEC_PIXEL_DENSITY_FACTOR[Pixel_density];

  Modulate_dot_size=DOT_EBG_MODULATE_DOT_SIZE_DEFAULT;
  Black_dots=false;
  Black_threshold=DOT_EBG_BLACK_THRESHOLD_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg::set_dots_images(vector<vector<cv::Mat *> > *Dots1)
{
  Dots=Dots1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg::pixel_density(_f_dot_ebg_ns::_pixel_density Pixel_density1)
{
  if (Pixel_density1!=Pixel_density){
    Pixel_density=Pixel_density1;
    Scaling_factor=_f_dot_ebg_ns::VEC_PIXEL_DENSITY_FACTOR[(int)Pixel_density];

    // the size of the output image must be changed
    Output_image_0->release();
    Output_image_0->create(Input_image_0->rows*Scaling_factor,Input_image_0->cols*Scaling_factor,CV_8U);
  }
}


/*****************************************************************************//**
 * computes the resulting dot of mixing the original dot pixel with the output image pixel
 * The accumulation effect is simulated with the product
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg::put_dot(cv::Mat *Output_image0,float Row1,float Col1,unsigned int Selected_dot_size1,unsigned int Index_row1,unsigned int Index_col1)
{
  int Row,Col;
  int Row_out,Col_out;
  unsigned char Value;
  unsigned int Row_aux=Selected_dot_size1*Index_row1*Scaling_factor;
  unsigned int Col_aux=Selected_dot_size1*Index_col1*Scaling_factor;

  // All the pixels of the dot are modified
  for (Row=0;Row<(int)(Selected_dot_size1*Scaling_factor);Row++){
    for (Col=0;Col<(int)(Selected_dot_size1*Scaling_factor);Col++){
      // get the value of the image of dots of size Selected_dot_size1

      Value=(*Dots)[(int)Pixel_density][Selected_dot_size1]->at<unsigned char>(Row+Row_aux,Col+Col_aux);
      // output position
      Row_out=Row1+Row;
      Col_out=Col1+Col;

      if (Row_out<0 || Row_out>=Output_image0->rows || Col_out<0 || Col_out>=Output_image0->cols) continue;
      // the row and col are correct, the color is changed
      if (Black_dots==true){
        if (Value<=Black_threshold) Value=(unsigned char)BLACK;
        else Value=(unsigned char)WHITE;
      }
      Output_image0->at<unsigned char>(Row_out,Col_out)=(unsigned char)((float)Output_image0->at<unsigned char>(Row_out,Col_out)*(float)Value/255.0);
    }
  }
}


/*****************************************************************************//**
 * computes the stippling process
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg::stippling_svg(QPainter &Painter)
{
  int Row,Col,Selected_dot_size;
  float Pos_row,Pos_col;
  _vertex2f Position;
  unsigned int Counter_of_dots=0;
  unsigned int Gray_value;

  cv::flip(*Input_image_0,*Input_image_0,0);

  QPainterPath starPath;
  starPath.moveTo(90,50);
  for (int i = 1; i < 5; ++i) {
    starPath.lineTo(50 + 40* std::cos(0.8 * i * M_PI),50 + 40* std::sin(0.8 * i * M_PI));
  }
  starPath.closeSubpath();
  starPath.setFillRule(Qt::WindingFill);

  QPainterPath textPath;
  QFont timesFont("Times", 50);
  timesFont.setStyleStrategy(QFont::ForceOutline);
  textPath.addText(10,70, timesFont,"@");

  Index_row.init(0,Num_rows_dot_matrix-1);
  Index_col.init(0,Num_cols_dot_matrix-1);
  Dot_size.init(Dot_size_min,Dot_size_max);

  Index_row.set_seed(1000);
  Dot_size.set_seed(5000);
  Index_col.set_seed(300);

  for (Row=0;Row<Input_image_0->rows;Row++){
    for (Col=0;Col<Input_image_0->cols;Col++){
      if (Input_image_0->at<unsigned char>(Row,Col)==BLACK){
        // compute a random dot size
        if (Modulate_dot_size==false) Selected_dot_size=Dot_size.value();
        else{
          Gray_value=255-Input_image_1->at<unsigned char>(Row,Col);
          Selected_dot_size=Gray_value/52+4;
        }

        Pos_row=(float)Row*(float)Scaling_factor-((float)Selected_dot_size*Scaling_factor)/2;
        Pos_col=(float)Col*(float)Scaling_factor-((float)Selected_dot_size*Scaling_factor)/2;

        // for dot
        Painter.drawEllipse(QPoint(Pos_col,Pos_row),Selected_dot_size,Selected_dot_size);

        // for star
//        Scale=((0.4-0.2)*(Selected_dot_size-4)/(8-4))+0.2; // for star
//        Painter.save();
//        Painter.translate(Pos_col,Pos_row);
//        Painter.rotate(-90); // for star
//        Painter.scale(Scale,Scale);
//        Painter.drawPath(starPath); // for star
//        Painter.restore();

        // for text
//        Scale=((0.6-0.3)*(Selected_dot_size-4)/(8-4))+0.3; // for text
//        Painter.save();
//        Painter.translate(Pos_col,Pos_row);
//        Painter.scale(Scale,Scale);
//        Painter.drawPath(textPath);
//        Painter.restore();
      }
    }
  }

  set_info1(Counter_of_dots);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg::stippling(cv::Mat *Input_image0,cv::Mat *Input_image1,cv::Mat *Output_image0)
{
  int Row,Col,Selected_dot_size;
  float Pos_row,Pos_col;
  int Selected_row,Selected_col;
  _vertex2f Position;
  unsigned int Counter_of_dots=0;
  unsigned int Gray_value;

  Index_row.init(0,Num_rows_dot_matrix-1);
  Index_col.init(0,Num_cols_dot_matrix-1);
  Dot_size.init(Dot_size_min,Dot_size_max);

  Index_row.set_seed(1000);
  Dot_size.set_seed(5000);
  Index_col.set_seed(300);

  for (Row=0;Row<Input_image0->rows;Row++){
    for (Col=0;Col<Input_image0->cols;Col++){
      if (Input_image0->at<unsigned char>(Row,Col)==BLACK){
        // compute a random dot size
        if (Modulate_dot_size==false) Selected_dot_size=Dot_size.value();
        else{
          Gray_value=255-Input_image1->at<unsigned char>(Row,Col);
          Selected_dot_size=Gray_value/52+4; // 52 to get 5 values
        }

        Counter_of_dots++;
        //
        Selected_row=Index_row.value();
        Selected_col=Index_col.value();

        Pos_row=(float)Row*(float)Scaling_factor-((float)Selected_dot_size*Scaling_factor)/2;
        Pos_col=(float)Col*(float)Scaling_factor-((float)Selected_dot_size*Scaling_factor)/2;

        put_dot(Output_image0,Pos_row,Pos_col,Selected_dot_size,Selected_row,Selected_col);
      }
    }
  }

  set_info1(Counter_of_dots);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg::update()
{
  if (Input_image_0->cols!=Input_image_1->cols || Input_image_0->rows!=Input_image_1->rows){
    QMessageBox MsgBox;
    MsgBox.setText("Filter Dot EBG");
    MsgBox.setInformativeText("Warning: the input images must have the same sizes");
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.exec();
    copy_input_to_output();
  }
  else{
    cv::Mat *Aux_input_image0=nullptr;
    cv::Mat *Aux_input_image1=nullptr;
    cv::Mat *Aux_output_image=nullptr;

    // check the output size
    if (Input_image_0->cols!=Output_image_0->cols || Input_image_0->rows!=Output_image_0->rows){
      Output_image_0->release();
      Output_image_0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
    }

    // Check the number of input channels of input image 1
    if (Input_image_0->channels()!=Num_channels_input_image_0){// Different number of channels
      if (Input_image_0->channels()==3){
        // conversion

        Aux_input_image0=new cv::Mat;
        Aux_input_image0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
        cvtColor(*Input_image_0,*Aux_input_image0,cv::COLOR_BGR2GRAY,1);
      }
      else cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << endl;
    }
    else{// the same number of channels
      Aux_input_image0=Input_image_0;
    }

    // Check the number of input channels of input image 2
    if (Input_image_1->channels()!=Num_channels_input_image_1){// Different number of channels
      if (Input_image_1->channels()==3){
        // conversion

        Aux_input_image1=new cv::Mat;
        Aux_input_image1->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
        cvtColor(*Input_image_0,*Aux_input_image1,cv::COLOR_BGR2GRAY,1);
      }
      else cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << endl;
    }
    else{// the same number of channels
      Aux_input_image1=Input_image_1;
    }

    // Check the number of output channels
    if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
      if (Output_image_0->channels()==3){
        // conversion
        Aux_output_image=new cv::Mat;
        Aux_output_image->create(Output_image_0->rows,Output_image_0->cols,CV_8UC1);

        // set the output image to white
        Output_image_0->setTo(cv::Scalar(255,255,255));
        stippling(Aux_input_image0,Aux_input_image1,Aux_output_image);
        cvtColor(*Aux_output_image,*Output_image_0,cv::COLOR_GRAY2RGB,3);
      }
      else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
    }
    else{// the same number of channels
      // set the output image to white
      Output_image_0->setTo(255);
      stippling(Aux_input_image0,Aux_input_image1,Output_image_0);
    }

    if (Aux_input_image0!=nullptr && Aux_input_image0!=Input_image_0) delete Aux_input_image0;
    if (Aux_input_image1!=nullptr && Aux_input_image1!=Input_image_1) delete Aux_input_image1;
    if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg::save_seeds()
{
  Index_row.save_seed();
  Index_col.save_seed();
  Dot_size.save_seed();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg::load_seeds()
{
  Index_row.load_seed();
  Index_col.load_seed();
  Dot_size.load_seed();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_dot_ebg_ui::_filter_dot_ebg_ui(_gl_widget *GL_widget1, string Name1)
{
  Name=Name1;
  Qtw_filter_dot_ebg=new _qtw_filter_dot_ebg(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_dot_ebg_ui::_filter_dot_ebg_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Box_name)
{
  read_parameters(Parameters);
  Name=Box_name;
  Qtw_filter_dot_ebg=new _qtw_filter_dot_ebg(GL_widget1,this,Box_name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_dot_ebg_ui::~_filter_dot_ebg_ui()
{
  delete Qtw_filter_dot_ebg;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg_ui::reset_data()
{
  _filter_dot_ebg::reset_data();
  Qtw_filter_dot_ebg->set_parameter2(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg_ui::show()
{
  Qtw_filter_dot_ebg->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg_ui::hide()
{
  Qtw_filter_dot_ebg->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_dot_ebg_ui::get_link()
{
  return Qtw_filter_dot_ebg->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(DOT_EBG_PIXEL_DENSITY_DEFAULT);
    parameter2(DOT_EBG_MODULATE_DOT_SIZE_DEFAULT);
    parameter3(DOT_EBG_BLACK_DOTS_DEFAULT);
    parameter4(DOT_EBG_BLACK_THRESHOLD_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["pixel_density"]=="default") parameter1(DOT_EBG_PIXEL_DENSITY_DEFAULT);
      else{
        if (Parameters["pixel_density"]=="300PPI") parameter1(_f_dot_ebg_ns::PIXEL_DENSITY_300PPI);
        if (Parameters["pixel_density"]=="600PPI") parameter1(_f_dot_ebg_ns::PIXEL_DENSITY_600PPI);
        if (Parameters["pixel_density"]=="1200PPI") parameter1(_f_dot_ebg_ns::PIXEL_DENSITY_1200PPI);
      }

      if (Parameters["modulate_dot_size"]=="default") parameter2(DOT_EBG_MODULATE_DOT_SIZE_DEFAULT);
      else{
        if (Parameters["modulate_dot_size"]=="true") parameter2(true);
        else parameter2(false);
      }

      if (Parameters["black_dots"]=="default") parameter3(DOT_EBG_BLACK_DOTS_DEFAULT);
      else{
        if (Parameters["black_dots"]=="true") parameter3(true);
        else parameter3(false);
      }

      if (Parameters["black_threshold"]=="default") parameter4(DOT_EBG_BLACK_THRESHOLD_DEFAULT);
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

void _filter_dot_ebg_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  switch((int)parameter1()){
  case 0:sprintf(Aux,"%s","300PPI");break;
  case 1:sprintf(Aux,"%s","600PPI");break;
  case 2:sprintf(Aux,"%s","1200PPI");break;
  }
  Parameters["pixel_density"]=string(Aux);

  if (parameter2()) sprintf(Aux,"%s","true");
  else sprintf(Aux,"%s","false");
  Parameters["modulate_dot_size"]=string(Aux);

  if (parameter3()) sprintf(Aux,"%s","true");
  else sprintf(Aux,"%s","false");
  Parameters["black_dots"]=string(Aux);

  sprintf(Aux,"%d",parameter4());
  Parameters["black_threshold"]=string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_ebg_ui::set_info1(unsigned int Value1)
{
  Qtw_filter_dot_ebg->set_info1(Value1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_dot_ebg::_qtw_filter_dot_ebg(_gl_widget *GL_widget1,_filter_dot_ebg_ui *Filter1,string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_main=new QVBoxLayout;

  // pixel_density
  // parameter 1
  Group_box_parameter1=new QGroupBox(tr(String_group_box_parameter1.c_str()));
  Group_box_parameter1->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter1 = new QVBoxLayout;

  Combo_parameter1 = new QComboBox;
  Combo_parameter1->addItem(tr("300ppi"));
  Combo_parameter1->addItem(tr("600ppi"));
  Combo_parameter1->addItem(tr("1200ppi"));
  Combo_parameter1->setCurrentIndex((int)_f_dot_ebg_ns::DOT_EBG_PIXEL_DENSITY_DEFAULT);
  Combo_parameter1->setToolTip(tr(String_parameter1_tooltip.c_str()));

  connect(Combo_parameter1, SIGNAL(currentIndexChanged(int)),this,SLOT(set_parameter1_slot(int)));

  Vertical_box_parameter1->addWidget(Combo_parameter1);

  Group_box_parameter1->setLayout(Vertical_box_parameter1);

  // modulate size
  // Parameter 2
  Group_box_parameter2=new QGroupBox(tr(String_group_box_parameter2.c_str()));
  Group_box_parameter2->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter2 = new QVBoxLayout;

  Checkbox_parameter2=new QCheckBox(tr(String_checkbox_parameter2.c_str()));
  if (Filter->parameter1()) Checkbox_parameter2->setChecked(true);
  else Checkbox_parameter2->setChecked(false);
  Checkbox_parameter2->setToolTip(tr(String_parameter2_tooltip.c_str()));

  connect(Checkbox_parameter2,SIGNAL(stateChanged(int)),this,SLOT(set_parameter2_slot(int)));

  Vertical_box_parameter2->addWidget(Checkbox_parameter2);

  Group_box_parameter2->setLayout(Vertical_box_parameter2);

  // Black dots
  // Parameter 3
  Group_box_parameter3=new QGroupBox(tr(String_group_box_parameter3.c_str()));
  Group_box_parameter3->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter3 = new QVBoxLayout;

  Checkbox_parameter3=new QCheckBox(tr(String_checkbox_parameter3.c_str()));
  if (Filter->parameter2()) Checkbox_parameter3->setChecked(true);
  else Checkbox_parameter3->setChecked(false);
  Checkbox_parameter3->setToolTip(tr(String_parameter2_tooltip.c_str()));

  connect(Checkbox_parameter3,SIGNAL(stateChanged(int)),this,SLOT(set_parameter3_slot(int)));

  Vertical_box_parameter3->addWidget(Checkbox_parameter3);

  Group_box_parameter3->setLayout(Vertical_box_parameter3);

  // Parameter 4
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

  connect(Slider_parameter4, SIGNAL(valueChanged(int)),this,SLOT(set_parameter4_slot(int)));

  Group_box_parameter4->setLayout(Grid_parameter4);

  // info
  QGroupBox *Group_box_info1=new QGroupBox(tr(String_group_box_info1.c_str()));
  Group_box_info1->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Verticalbox_info1 = new QVBoxLayout;

  Line_edit_info1=new QLineEdit();
  Line_edit_info1->setAlignment(Qt::AlignRight);
  Line_edit_info1->setReadOnly(true);
  Line_edit_info1->setEnabled(false);

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

void _qtw_filter_dot_ebg::set_parameter1(int Value)
{
  Combo_parameter1->setCurrentIndex(Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_ebg::set_parameter2(bool Value)
{
  if (Value) Checkbox_parameter2->setChecked(true);
  else Checkbox_parameter2->setChecked(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_ebg::set_parameter3(bool Value)
{
  if (Value) Checkbox_parameter3->setChecked(true);
  else Checkbox_parameter3->setChecked(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_ebg::set_parameter4(unsigned int Value)
{
  //warnig to the adjust
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

void _qtw_filter_dot_ebg::set_info1(unsigned int Value)
{
  //warnig to the adjust
  QString Str;

  Str=Str.number(Value);
  Line_edit_info1->setText(Str);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_ebg::set_parameter1_slot(int Value)
{
  Filter->parameter1(Value);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_ebg::set_parameter2_slot(int Value)
{
  if (Value==Qt::Checked) Filter->parameter2(true);
  else Filter->parameter2(false);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_ebg::set_parameter3_slot(int Value)
{
  if (Value==Qt::Checked) Filter->parameter3(true);
  else Filter->parameter3(false);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_ebg::set_parameter4_slot(int Size)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter4->setText(Str);
  Filter->parameter4(Size);
  GL_widget->update_effect(Filter->Name);
}
