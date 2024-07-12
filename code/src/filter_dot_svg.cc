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


#include "filter_dot_svg.h"
#include "glwidget.h"

using namespace _f_dot_svg_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_dot_svg::_filter_dot_svg()
{
  Scaling_factor=DOT_SVG_SCALING_FACTOR_DEFAULT;
  Dot_type=DOT_SVG_DOT_TYPE_DEFAULT;
  Antialias=DOT_SVG_ANTIALIAS_DEFAULT;
  Min_dot_size=DOT_SVG_MIN_DOT_SIZE_DEFAULT;
  Max_dot_size=DOT_SVG_MAX_DOT_SIZE_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_input_image_1=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_DOT_SVG;

  Change_output_image_size=DOT_SVG_CHANGE_OUTPUT_IMAGE_SIZE_DEFAULT;

  Seeds_initialized=false;
  Dots_texture_packet=0;
  Use_dots=true;

  Local_change=false;

  Dots=nullptr;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg::reset_data()
{
  Scaling_factor=DOT_SVG_SCALING_FACTOR_DEFAULT;
  Dot_type=DOT_SVG_DOT_TYPE_DEFAULT;
  Modulate_dot_size=DOT_SVG_MODULATE_DOT_SIZE_DEFAULT;
  Antialias=DOT_SVG_ANTIALIAS_DEFAULT;
  Min_dot_size=DOT_SVG_MIN_DOT_SIZE_DEFAULT;
  Max_dot_size=DOT_SVG_MAX_DOT_SIZE_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg::set_dots_images(std::vector<std::vector<cv::Mat *> > *Dots1)
{
  Dots=Dots1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg::put_dot(cv::Mat *Output_image0,float Row1,float Col1,unsigned int Selected_dot_size1,unsigned int Index_row1,unsigned int Index_col1)
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

      Value=(*Dots)[0][Selected_dot_size1]->at<unsigned char>(Row+Row_aux,Col+Col_aux);
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

void _filter_dot_svg::stippling_svg(QPainter &Painter)
{
  int Row,Col,Selected_dot_size;
  float Pos_row,Pos_col;
  _vertex2f Position;
  unsigned int Gray_value;

  float Scale;

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

  Dot_size.init(Min_dot_size,Max_dot_size);
  Dot_size.set_seed(5000);

  for (Row=0;Row<Input_image_0->rows;Row++){
    for (Col=0;Col<Input_image_0->cols;Col++){
      if (Input_image_0->at<unsigned char>(Input_image_0->rows-Row-1,Col)==BLACK){
        // compute a random dot size
        if (Modulate_dot_size==false) Selected_dot_size=Dot_size.value();
        else{
          Gray_value=255-Input_image_1->at<unsigned char>(Input_image_1->rows-Row-1,Col);
          Selected_dot_size=round(((float)(Max_dot_size-Min_dot_size)*(float)Gray_value/255.0)+(float)Min_dot_size);
        }

        Pos_row=(float)Row*(float)Scaling_factor-((float)Selected_dot_size*Scaling_factor)/2;
        Pos_col=(float)Col*(float)Scaling_factor-((float)Selected_dot_size*Scaling_factor)/2;

        switch ((int)Dot_type){
        case DOT_TYPE_CIRCLE:
          Painter.drawEllipse(QPointF(Pos_col,Pos_row),(float)Selected_dot_size*0.4,(float)Selected_dot_size*0.4);
          break;
        case DOT_TYPE_STAR:
          Painter.save();
          Scale=((0.05-0.01)*(Selected_dot_size-Min_dot_size)/(Max_dot_size-Min_dot_size))+0.01;
          Painter.translate(Pos_col,Pos_row);
          Painter.rotate(-90);
          Painter.scale(Scale,Scale);
          Painter.drawPath(starPath);
          Painter.restore();
          break;
        case DOT_TYPE_AT:
          Painter.save();
          Scale=((0.05-0.01)*(Selected_dot_size-Min_dot_size)/(Max_dot_size-Min_dot_size))+0.01;
          Painter.translate(Pos_col,Pos_row);
          Painter.scale(Scale,Scale);
          Painter.drawPath(textPath);
          Painter.restore();
          break;
        }
      }
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg::save_svg(QString File_name1)
{
  cv::Mat Aux_image;

  // get the image
  Aux_image=Output_image_0->clone();

  cv::flip(Aux_image,Aux_image,0);

  QSvgGenerator Generator;
  Generator.setFileName(File_name1);
  Generator.setSize(QSize(Aux_image.cols,Aux_image.rows));
  Generator.setViewBox(QRect(0, 0, Aux_image.cols,Aux_image.rows));
  Generator.setTitle("SVG file");

  QPainter Painter;
  Painter.begin(&Generator);
  Painter.setClipRect(QRect(0, 0, Aux_image.cols,Aux_image.rows));
  Painter.setPen(Qt::NoPen);
  // the backgroung
  Painter.fillRect(QRect(0, 0, Aux_image.cols,Aux_image.rows), Qt::white);
  // color for figures
  Painter.setBrush(Qt::black);
  // paint
  stippling_svg(Painter);
  // close
  Painter.end();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg::stippling(cv::Mat *Input_image0,cv::Mat *Input_image1,cv::Mat *Output_image0)
{
  int Row,Col,Selected_dot_size;
  float Pos_row,Pos_col;
  _vertex2f Position;
  unsigned int Counter_of_dots=0;
  unsigned int Gray_value;

  int Window_width=Input_image0->cols;
  int Window_height=Input_image0->rows;

  Dot_size.init(Min_dot_size,Max_dot_size);
  Dot_size.set_seed(5000);

  GLuint Renderbuffers[2];
  GLuint Framebuffer;
  //renderbuffers
  glGenRenderbuffers(2, Renderbuffers);
  // color
  glBindRenderbuffer(GL_RENDERBUFFER, Renderbuffers[0]);
  glRenderbufferStorage(GL_RENDERBUFFER,GL_RGB8,Window_width,Window_height);
  // depth
  glBindRenderbuffer(GL_RENDERBUFFER, Renderbuffers[1]);
  glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24 ,Window_width,Window_height);

  // This is for the FBO
  glGenFramebuffers(1,&Framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER,Framebuffer);
  // attach the color
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER, Renderbuffers[0]);
  // attach the depth
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, Renderbuffers[1]);

  //
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,Window_width,0,Window_height,-100,100);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glViewport(0,0,Window_width,Window_height);

  //
  glClearColor(1,1,1,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  if (Antialias){
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }
  else{
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
  }

  glColor3ub(0,0,0);
  for (Row=0;Row<Input_image0->rows;Row++){
    for (Col=0;Col<Input_image0->cols;Col++){
      if (Input_image0->at<unsigned char>(Row,Col)==BLACK){
        // compute a random dot size
        if (Modulate_dot_size==false) Selected_dot_size=Dot_size.value();
        else{
          Gray_value=255-Input_image1->at<unsigned char>(Row,Col);
          Selected_dot_size=round(((float)(Max_dot_size-Min_dot_size)*(float)Gray_value/255.0)+(float)Min_dot_size);
        }

        Counter_of_dots++;

        Pos_row=(float)Row*(float)Scaling_factor-((float)Selected_dot_size*Scaling_factor)/2;
        Pos_col=(float)Col*(float)Scaling_factor-((float)Selected_dot_size*Scaling_factor)/2;

        glPointSize((float)Selected_dot_size);
        glBegin(GL_POINTS);
        glVertex2i(Pos_col,Pos_row);
        glEnd();
      }
    }
  }

  // read the image
  std::vector<GLubyte> Pixels;
  _vertex3uc Pixel;

  Pixels.resize(Window_width*Window_height);

  // read the data form the framebuffer. It is saved in Pixels
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glReadPixels(0,0,Window_width,Window_height,GL_RED,GL_UNSIGNED_BYTE,&Pixels[0]);

  for (int i=0;i<Window_height;i++){
    for (int j=0;j<Window_width;j++){
      // the pixel i,j is obtained
      Output_image0->at<unsigned char>(i,j)=Pixels[i*Window_width+j];
    }
  }

  // delete the render buffers and frame buffers
  glDeleteRenderbuffers(2, Renderbuffers);
  glDeleteFramebuffers(1,&Framebuffer);

  // the normal framebuffer take the control of drawing
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

  set_info1(Counter_of_dots);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg::update()
{
  if (Input_image_0->cols!=Input_image_1->cols || Input_image_0->rows!=Input_image_1->rows){
    QMessageBox MsgBox;
    MsgBox.setText("Filter Dot SVG");
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
      else std::cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << std::endl;
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
      else std::cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << std::endl;
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
      else std::cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << std::endl;
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

void _filter_dot_svg::save_seeds()
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

void _filter_dot_svg::load_seeds()
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

_filter_dot_svg_ui::_filter_dot_svg_ui(_gl_widget *GL_widget1, std::string Name1)
{
  Name=Name1;
  Qtw_filter_dot_svg=new _qtw_filter_dot_svg(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_dot_svg_ui::_filter_dot_svg_ui(_gl_widget *GL_widget1, std::map<std::string, std::string> &Parameters, std::string Box_name)
{
  read_parameters(Parameters);
  Name=Box_name;
  Qtw_filter_dot_svg=new _qtw_filter_dot_svg(GL_widget1,this,Box_name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_dot_svg_ui::~_filter_dot_svg_ui()
{
  delete Qtw_filter_dot_svg;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg_ui::reset_data()
{
  _filter_dot_svg::reset_data();
  Qtw_filter_dot_svg->set_parameter1(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg_ui::show()
{
  Qtw_filter_dot_svg->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg_ui::hide()
{
  Qtw_filter_dot_svg->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_dot_svg_ui::get_link()
{
  return Qtw_filter_dot_svg->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(DOT_SVG_DOT_TYPE_DEFAULT);
    parameter2(DOT_SVG_MODULATE_DOT_SIZE_DEFAULT);
    parameter2(DOT_SVG_ANTIALIAS_DEFAULT);
    parameter4(DOT_SVG_MIN_DOT_SIZE_DEFAULT);
    parameter5(DOT_SVG_MAX_DOT_SIZE_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["dot_type"]=="default") parameter1(DOT_TYPE_CIRCLE);
      else{
        if (Parameters["dot_type"]=="circle") parameter1(DOT_TYPE_CIRCLE);
        if (Parameters["dot_type"]=="star") parameter1(DOT_TYPE_STAR);
        if (Parameters["dot_type"]=="at") parameter1(DOT_TYPE_AT);
      }
      if (Parameters["modulate_dot_size"]=="default") parameter2(DOT_SVG_MODULATE_DOT_SIZE_DEFAULT);
      else{
        if (Parameters["modulate_dot_size"]=="true") parameter2(true);
        else parameter2(false);
      }
      if (Parameters["antialias"]=="default") parameter3(DOT_SVG_ANTIALIAS_DEFAULT);
      else{
        if (Parameters["antialias"]=="true") parameter3(true);
        else parameter3(false);
      }
      if (Parameters["min_dot_size"]=="default") parameter4(DOT_SVG_MIN_DOT_SIZE_DEFAULT);
      else parameter4(atoi(Parameters["min_dot_size"].c_str()));
      if (Parameters["max_dot_size"]=="default") parameter5(DOT_SVG_MAX_DOT_SIZE_DEFAULT);
      else parameter5(atoi(Parameters["max_dot_size"].c_str()));

    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of STIPPLING_SVG filter");
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

void _filter_dot_svg_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  switch((int)parameter1()){
  case 0:sprintf(Aux,"%s","circle");break; // circle
  case 1:sprintf(Aux,"%s","star");break; // star
  case 2:sprintf(Aux,"%s","at");break; // at
  }
  Parameters["dot_type"]=std::string(Aux);

  if (parameter2()) sprintf(Aux,"%s","true");
  else sprintf(Aux,"%s","false");
  Parameters["modulate_dot_size"]=std::string(Aux);

  if (parameter3()) sprintf(Aux,"%s","true");
  else sprintf(Aux,"%s","false");
  Parameters["antialias"]=std::string(Aux);

  sprintf(Aux,"%d",parameter4());
  Parameters["min_dot_size"]=std::string(Aux);

  sprintf(Aux,"%d",parameter5());
  Parameters["max_dot_size"]=std::string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_dot_svg_ui::set_info1(unsigned int Value1)
{
  Qtw_filter_dot_svg->set_info1(Value1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_dot_svg::_qtw_filter_dot_svg(_gl_widget *GL_widget1,_filter_dot_svg_ui *Filter1,std::string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_main=new QVBoxLayout;

  // resolution
  // parameter 1
  Group_box_parameter1=new QGroupBox(tr(String_group_box_parameter1.c_str()));
  Group_box_parameter1->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter1 = new QVBoxLayout;

  Combo_parameter1 = new QComboBox;
  Combo_parameter1->addItem(tr("Circle"));
  Combo_parameter1->addItem(tr("Star"));
  Combo_parameter1->addItem(tr("@ symbol"));
  Combo_parameter1->setCurrentIndex((int)Filter->parameter1());
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
  if (Filter->parameter2()) Checkbox_parameter2->setChecked(true);
  else Checkbox_parameter2->setChecked(false);
  Checkbox_parameter2->setToolTip(tr(String_parameter2_tooltip.c_str()));

  connect(Checkbox_parameter2,SIGNAL(stateChanged(int)),this,SLOT(set_parameter2_slot(int)));

  Vertical_box_parameter2->addWidget(Checkbox_parameter2);

  Group_box_parameter2->setLayout(Vertical_box_parameter2);

  // antialias
  // Parameter 3
  Group_box_parameter3=new QGroupBox(tr(String_group_box_parameter3.c_str()));
  Group_box_parameter3->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter3 = new QVBoxLayout;

  Checkbox_parameter3=new QCheckBox(tr(String_checkbox_parameter3.c_str()));
  if (Filter->parameter3()) Checkbox_parameter3->setChecked(true);
  else Checkbox_parameter3->setChecked(false);
  Checkbox_parameter3->setToolTip(tr(String_parameter3_tooltip.c_str()));

  connect(Checkbox_parameter3,SIGNAL(stateChanged(int)),this,SLOT(set_parameter3_slot(int)));

  Vertical_box_parameter3->addWidget(Checkbox_parameter3);

  Group_box_parameter3->setLayout(Vertical_box_parameter3);

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

  connect(Slider_parameter4, SIGNAL(valueChanged(int)),this,SLOT(set_parameter4_slot(int)));

  // parameter 5
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
  Str=Str.number(Filter->parameter5());
  Line_edit_parameter5->setText(Str);

  Slider_parameter5 = new QSlider(Qt::Horizontal);
  Slider_parameter5->setRange(Parameter5_min_value,Parameter5_max_value);
  Slider_parameter5->setSingleStep(Parameter5_single_step);
  Slider_parameter5->setPageStep(Parameter5_page_step);
  Slider_parameter5->setTickInterval(Parameter5_tick_interval);
  Slider_parameter5->setTickPosition(QSlider::TicksRight);
  Slider_parameter5->setTracking(Parameter5_set_tracking);
  Slider_parameter5->setValue(Filter->parameter5());
  Slider_parameter5->setToolTip(tr(String_parameter5_slider_tooltip.c_str()));

  Grid_parameter5->addWidget(Line_edit_parameter5,0,1,Qt::AlignCenter);
  Grid_parameter5->addWidget(Label_parameter5_min,1,0,Qt::AlignRight);
  Grid_parameter5->addWidget(Slider_parameter5,1,1);
  Grid_parameter5->addWidget(Label_parameter5_max,1,2,Qt::AlignLeft);

  Group_box_parameter5->setLayout(Grid_parameter5);

  connect(Slider_parameter5, SIGNAL(valueChanged(int)),this,SLOT(set_parameter5_slot(int)));

  // Button to save in SVG
  QPushButton *Button_parameter6= new QPushButton(tr("Save SVG file"));

  connect(Button_parameter6, SIGNAL(clicked()),this,SLOT(set_parameter6_slot()));

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
  Vertical_box_main->addWidget(Group_box_parameter5);
  Vertical_box_main->addWidget(Group_box_info1);
  Vertical_box_main->addStretch(1);
  Vertical_box_main->addWidget(Button_parameter6);

  Group_box_main->setLayout(Vertical_box_main);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_svg::set_parameter1(int Value)
{
  Combo_parameter1->setCurrentIndex(Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_svg::set_parameter2(bool Value)
{
  if (Value) Checkbox_parameter2->setChecked(true);
  else Checkbox_parameter2->setChecked(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_svg::set_parameter3(bool Value)
{
  if (Value) Checkbox_parameter3->setChecked(true);
  else Checkbox_parameter3->setChecked(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_svg::set_parameter4(int Value)
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

void _qtw_filter_dot_svg::set_parameter5(int Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_parameter5->setText(Str);
  Slider_parameter5->setValue(Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_svg::set_info1(unsigned int Value)
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

void _qtw_filter_dot_svg::set_parameter1_slot(int Value)
{
  Filter->parameter1(Value);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_svg::set_parameter2_slot(int Value)
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

void _qtw_filter_dot_svg::set_parameter3_slot(int Value)
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

void _qtw_filter_dot_svg::set_parameter4_slot(int Size)
{
  QString Str;
  char Aux[100];

  if (Size>Filter->max_dot_size()){
    Size=Filter->max_dot_size();
    Slider_parameter4->blockSignals(true);
    Slider_parameter4->setValue(Size);
    Slider_parameter4->blockSignals(false);
  }

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter4->setText(Str);
  Filter->parameter4(Size);
  Filter->local_change(true);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_svg::set_parameter5_slot(int Size)
{
  QString Str;
  char Aux[100];

  if (Size<Filter->min_dot_size()){
    Size=Filter->max_dot_size();
    Slider_parameter5->blockSignals(true);
    Slider_parameter5->setValue(Size);
    Slider_parameter5->blockSignals(false);
  }

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter5->setText(Str);
  Filter->parameter5(Size);
  Filter->local_change(true);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_dot_svg::set_parameter6_slot()
{
  QString File_name1 = QFileDialog::getSaveFileName(this, tr("Save SVG"),"./images",tr("SVG (*.svg)"));

  if (!File_name1.isEmpty()) Filter->save_svg(File_name1);
}
