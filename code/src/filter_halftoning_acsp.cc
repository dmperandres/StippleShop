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


#include "filter_halftoning_acsp.h"
#include "glwidget.h"

using namespace _f_halftoning_acsp;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_acsp::_filter_halftoning_acsp()
{
  Cluster_size=HALFTONING_ACSP_CLUSTER_SIZE_DEFAULT;
  Selective_precipitation=HALFTONING_ACSP_SELECTIVE_PRECIPITATION_DEFAULT;
  Adaptive_clustering=HALFTONING_ACSP_ADAPTIVE_CLUSTERING_DEFAULT;
  Threshold=HALFTONING_ACSP_THRESHOLD_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_HALFTONING_ACSP;

  Scaling_factor=1;

  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_acsp::reset_data()
{
  Cluster_size=HALFTONING_ACSP_CLUSTER_SIZE_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_acsp::step(int &Angle)
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

void _filter_halftoning_acsp::hilbert(int Orient,int &Angle, int Level1)
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

void _filter_halftoning_acsp::generate_space_fill()
{
  int Angle=0;

  Path_x=Path_y=0;                         /* Start of the trace */
  hilbert(1,Angle,Level);               /* generates a hilbert curve */
  Path.at<unsigned char>(Path_y*Width_image+Path_x)=END; /* indicates end of path */
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_acsp::space_fill(cv::Mat *Input_image0, cv::Mat *Output_image0)
{
  char Change;           /* Flag indicate sudden change detected */
  char Ending; 		 /* flag indicates end of space filling curve */
  int Accumulator;       /* Accumulate gray value */
  int Current_cluster_size;   /* Record size of current cluster */
  int Front_x, Front_y;    /* Pointer to the front of the cluster */
  int Window_x, Window_y;  /* Pointer to first pixel applied with filter */
  int Cluster_x, Cluster_y;/* Pointer to first pixel in current cluster */
  int Window_length;	 /* Size of the moving window */
  int Window_sum;		 /* Current moving window's sum */
  int Max_sum;		 /* Maximum moving window's sum recorded */
  int Start;		 /* Position of the moving window with max sum */
  vector<int> Cluster;		 /* An array hold the pixel of current cluster */
  int Last, i, Temp_x, Temp_y, Current_x, Current_y;	/* temp variables */
  long LoG_filter[7] = {-1, -5, 0, 13, 0, -5, -1};  /* filter */
  long Convolution;	 /* Convolution value in this turn */
  long Last_convolution;  /* Convolution value in last turn */
  long Cluster_count=0;	 /* Record the total number of clusters */

  // resize Path
  Path.create(Input_image0->rows,Input_image0->cols,CV_8U);

  Level=log2f((float)Input_image0->rows);
  Width_image=Input_image0->cols;

  generate_space_fill();    /* generates the spacefilling path    */

  Cluster.resize(Cluster_size);

  Convolution=0;
  Current_cluster_size=0;
  Accumulator=0;
  for (Front_x=0, Front_y=0, i=0 ; i<7 ; i++){
    if (i<3)    {
      Cluster[Current_cluster_size] = Input_image0->at<unsigned char>(Front_y*Width_image+Front_x);
      Accumulator += Cluster[Current_cluster_size];
      Current_cluster_size++;
    }
    if (i==3){
      Current_x = Front_x;  Current_y = Front_y;
    }
    Convolution += LoG_filter[i]*(long)(Input_image0->at<unsigned char>(Front_y*Width_image+Front_x));
    move(Front_x,Front_y);  /* assume the image at least has 7 pixels */
  }
  Last_convolution = Convolution;
  Cluster_x=0;   Cluster_y=0;
  Window_x=0;    Window_y=0;
  Change=false;
  Ending=false;

  while (true){
    if (Adaptive_clustering){ /* switch on/off adaptive clustering */
      /* do convolution */
      Convolution = 0;
      for (Temp_x=Window_x, Temp_y=Window_y, i=0 ; i<7 ; i++){
        Convolution += LoG_filter[i]*Input_image0->at<unsigned char>(Temp_y*Width_image+Temp_x);
        move(Temp_x,Temp_y);
      }

      /* detect sudden change */
      if ( (Convolution >= 0 && Last_convolution <=0 && abs(Convolution-Last_convolution)>Threshold)
         ||(Convolution <= 0  && Last_convolution >=0 && abs(Convolution-Last_convolution)>Threshold))
        Change=true; /* force output pixel */
    }

    /* Output pixels if necessary */
    if (Change || Current_cluster_size >= Cluster_size || Ending){
      Change=false;
      Cluster_count++;

      /* Search the best position withn cluster to preciptate */
      Start = 0;
      if (Selective_precipitation){ /* switch on/off selective precipitation */
        Window_length = Accumulator/BLACK;
        Window_sum = 0;
        for (i=0; i<Window_length; i++) Window_sum += Cluster[i];

        for (Max_sum=Window_sum, Last=0; i<Current_cluster_size; i++, Last++){
          Window_sum+= Cluster[i] - Cluster[Last];
          if (Window_sum > Max_sum){
            Start=Last+1;
            Max_sum=Window_sum;
          }
        }
      }

      /* Output pixels */
      for (i=0 ; Current_cluster_size!=0 ; Current_cluster_size--, i++){
        if (Accumulator>=BLACK && i>=Start){  /* precipitates */
          Output_image0->at<unsigned char>(Cluster_y*Width_image+Cluster_x)=BLACK;
          Accumulator-=BLACK;
        }
        else
          Output_image0->at<unsigned char>(Cluster_y*Width_image+Cluster_x)=WHITE;
        move(Cluster_x,Cluster_y);
      } /* for */

      if (Ending) break;
    } /* if */

    Cluster[Current_cluster_size] =Input_image0->at<unsigned char>(Current_y*Width_image+Current_x);
    Accumulator += Cluster[Current_cluster_size];
    Current_cluster_size++;
    if (Path.at<unsigned char>(Current_y*Width_image+Current_x)==END) Ending= true;
    move(Current_x,Current_y);
    move(Window_x,Window_y);
    move(Front_x,Front_y);
  } /* while */
  //fprintf (stderr, "no of cluster = %li\naverage cluster size = %f\n",Cluster_count, (float)OXMAX*OYMAX/Cluster_count);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_acsp::move(int &x,int &y)
{
  switch (Path.at<unsigned char>(y*Width_image+x)){
     case UP:   y++; break;
     case DOWN: y--; break;
     case LEFT: x--; break;
     case RIGHT:x++; break;
   }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_acsp::update()
{
  if (Input_image_0->cols!=Input_image_0->rows || (log2(Input_image_0->cols)-(int)log2(Input_image_0->cols)!=0)){
    QMessageBox MsgBox;
    MsgBox.setIcon(QMessageBox::Warning);
    MsgBox.setText("Filter Adaptive Clustering Selective Precipitation");
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

        space_fill(Aux_input_image,Aux_output_image);
        cvtColor(*Aux_output_image,*Output_image_0,cv::COLOR_GRAY2RGB,3);

      }
      else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
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

_filter_halftoning_acsp_ui::_filter_halftoning_acsp_ui(_gl_widget *GL_widget1, string Name1)
{
  Name=Name1;
  Qtw_filter_halftoning_acsp=new _qtw_filter_halftoning_acsp(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_acsp_ui::_filter_halftoning_acsp_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_halftoning_acsp=new _qtw_filter_halftoning_acsp(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_halftoning_acsp_ui::~_filter_halftoning_acsp_ui()
{
  delete Qtw_filter_halftoning_acsp;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_acsp_ui::reset_data()
{
  _filter_halftoning_acsp::reset_data();
  Qtw_filter_halftoning_acsp->set_parameter1(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_acsp_ui::show()
{
  Qtw_filter_halftoning_acsp->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_acsp_ui::hide()
{
  Qtw_filter_halftoning_acsp->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_halftoning_acsp_ui::get_link()
{
  return Qtw_filter_halftoning_acsp->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_halftoning_acsp_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(HALFTONING_ACSP_CLUSTER_SIZE_DEFAULT);
    parameter2(HALFTONING_ACSP_THRESHOLD_DEFAULT);
    parameter3(HALFTONING_ACSP_ADAPTIVE_CLUSTERING_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["cluster_size"]=="default") parameter1(HALFTONING_ACSP_CLUSTER_SIZE_DEFAULT);
      else parameter1(atoi(Parameters["cluster_size"].c_str()));

      if (Parameters["threshold"]=="default") parameter2(HALFTONING_ACSP_THRESHOLD_DEFAULT);
      else parameter2(atoi(Parameters["threshold"].c_str()));

      if (Parameters["adaptive_clustering"]=="default") parameter3(HALFTONING_ACSP_ADAPTIVE_CLUSTERING_DEFAULT);
      else{
        if (Parameters["adaptive_clustering"]=="true") parameter3(true);
        else parameter3(false);
      }

      if (Parameters["selective_precipitation"]=="default") parameter4(HALFTONING_ACSP_SELECTIVE_PRECIPITATION_DEFAULT);
      else{
        if (Parameters["selective_precipitation"]=="true") parameter4(true);
        else parameter4(false);
      }

    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of HALFTONING_ACSP filter");
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

void _filter_halftoning_acsp_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%d",parameter1());
  Parameters["cluster_size"]=string(Aux);
  sprintf(Aux,"%d",parameter2());
  Parameters["threshold"]=string(Aux);
  if (parameter3()) sprintf(Aux,"%s","true");
  else sprintf(Aux,"%s","false");
  Parameters["adaptive_clustering"]=string(Aux);
  if (parameter4()) sprintf(Aux,"%s","true");
  else sprintf(Aux,"%s","false");
  Parameters["selective_precipitation"]=string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_halftoning_acsp::_qtw_filter_halftoning_acsp(_gl_widget *GL_widget1,_filter_halftoning_acsp_ui *Filter1,string Box_name)
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

  connect(Slider_parameter2, SIGNAL(valueChanged(int)),this,SLOT(set_parameter2_slot(int)));

  // Adaptive clustering
  // Parameter 3
  Group_box_parameter3=new QGroupBox(tr(String_group_box_parameter3.c_str()));
  Group_box_parameter3->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter3 = new QVBoxLayout;

  Checkbox_parameter3=new QCheckBox(tr(String_checkbox_parameter3.c_str()));
  if (Filter->parameter1()) Checkbox_parameter3->setChecked(true);
  else Checkbox_parameter3->setChecked(false);
  Checkbox_parameter3->setToolTip(tr(String_parameter3_tooltip.c_str()));

  connect(Checkbox_parameter3,SIGNAL(stateChanged(int)),this,SLOT(set_parameter3_slot(int)));

  Vertical_box_parameter3->addWidget(Checkbox_parameter3);

  Group_box_parameter3->setLayout(Vertical_box_parameter3);

  // Adaptive clustering
  // Parameter 4
  Group_box_parameter4=new QGroupBox(tr(String_group_box_parameter4.c_str()));
  Group_box_parameter4->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter4 = new QVBoxLayout;

  Checkbox_parameter4=new QCheckBox(tr(String_checkbox_parameter4.c_str()));
  if (Filter->parameter1()) Checkbox_parameter4->setChecked(true);
  else Checkbox_parameter4->setChecked(false);
  Checkbox_parameter4->setToolTip(tr(String_parameter4_tooltip.c_str()));

  connect(Checkbox_parameter4,SIGNAL(stateChanged(int)),this,SLOT(set_parameter4_slot(int)));

  Vertical_box_parameter4->addWidget(Checkbox_parameter4);

  Group_box_parameter4->setLayout(Vertical_box_parameter4);

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

void _qtw_filter_halftoning_acsp::set_parameter1(int Value)
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

void _qtw_filter_halftoning_acsp::set_parameter2(int Value)
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

void _qtw_filter_halftoning_acsp::set_parameter3(bool Value)
{
  if (Value) Checkbox_parameter3->setChecked(true);
  else Checkbox_parameter3->setChecked(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_halftoning_acsp::set_parameter4(bool Value)
{
  if (Value) Checkbox_parameter4->setChecked(true);
  else Checkbox_parameter4->setChecked(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_halftoning_acsp::set_parameter1_slot(int Size)
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

void _qtw_filter_halftoning_acsp::set_parameter2_slot(int Size)
{
  QString Str;
  char Aux[100];

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

void _qtw_filter_halftoning_acsp::set_parameter3_slot(int Value)
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

void _qtw_filter_halftoning_acsp::set_parameter4_slot(int Value)
{
  if (Value==Qt::Checked) Filter->parameter4(true);
  else Filter->parameter4(false);
  GL_widget->update_effect(Filter->Name);
}
