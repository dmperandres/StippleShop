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


#include "filter_ccvt.h"
#include "glwidget.h"

#include "aux_code/ccvt/ccvt_metric.h"
#include "aux_code/ccvt/ccvt_optimizer.h"
#include "aux_code/ccvt/ccvt_point.h"
#include "aux_code/ccvt/ccvt_site.h"

using namespace _f_ccvt_ns;
using namespace ccvt;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_ccvt::_filter_ccvt()
{
  Number_of_dots=CCVT_NUMBER_OF_DOTS_DEFAULT;
  Number_of_points_per_dot=CCVT_NUMBER_OF_POINTS_PER_DOT_DEFAULT;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_CCVT;

  Scaling_factor=1;

  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_ccvt::reset_data()
{
  Number_of_dots=CCVT_NUMBER_OF_DOTS_DEFAULT;
  Number_of_points_per_dot=CCVT_NUMBER_OF_POINTS_PER_DOT_DEFAULT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_ccvt::ccvt(cv::Mat *Input_image,cv::Mat *Output_image)
{
  double  Torus_size;
  bool CENTROIDAL=true;
  unsigned int GRAY_LEVEL=250;

  vector<_dot_data> Vec_dots_data;

  if (Input_image->cols>Input_image->rows) Torus_size=Input_image->cols;
  else Torus_size=Input_image->rows;

  Vec_dots_data.resize(Input_image->cols*Input_image->rows);

  // save all the dots that have a gray level in a vector
  unsigned int Num_points=0;
  for (int Row=0;Row<Input_image->rows;Row++){
    for (int Col=0;Col<Input_image->cols;Col++){
      if (Input_image->at<unsigned char>(Row,Col)<GRAY_LEVEL){
        Vec_dots_data[Num_points].x=Col;
        Vec_dots_data[Num_points].y=Row;
        Vec_dots_data[Num_points].Value=Input_image->at<unsigned char>(Row,Col);
        Num_points++;
      }
    }
  }

  // order from darker to clearer
  // the priority list is ordered using the compare function
  std::sort(Vec_dots_data.begin(),Vec_dots_data.begin()+Num_points);

  Point2 P2;
  typedef Optimizer<Site<Point2>, Point2, MetricToroidalEuclidean2> Optimizer;
  // intializing the underlying discrete space
  Point2::List points;

  for (unsigned int i=0; i<Num_points;i++){
    points.push_back(Point2(Vec_dots_data[i].x,Vec_dots_data[i].y));
  }

  // compute the number of dots=sites
  if (Number_of_points_per_dot<=0) Number_of_points_per_dot=1;
  else{
    if (Number_of_points_per_dot>Num_points) Number_of_points_per_dot=Num_points;
  }

  if (Number_of_dots<=0) Number_of_dots=CCVT_MINIMUM_NUMBER_OF_DOTS_DEFAULT;

  if (Num_points/Number_of_points_per_dot<Number_of_dots){
    Number_of_dots=Num_points/Number_of_points_per_dot;
  }

  // initializing the Voronoi sites with equal capacity
  unsigned int overallCapacity = static_cast<int>(points.size());
  Site<Point2>::List sites;
  for (unsigned int i = 0; i <Number_of_dots; ++i) {
    double x = static_cast<double>(rand() % RAND_MAX) / RAND_MAX * Torus_size;;
    double y = static_cast<double>(rand() % RAND_MAX) / RAND_MAX * Torus_size;;
    int capacity = overallCapacity / (Number_of_dots - i);
    overallCapacity -= capacity;
  //    sites.push_back(Site<Point2>(i, capacity, Point2(x, y)));
    sites.push_back(Site<Point2>(i, capacity, Point2(x, y)));
  }

  Optimizer optimizer;
  MetricToroidalEuclidean2 metric(Point2(Torus_size,Torus_size));
  optimizer.initialize(sites, points, metric);

  // optimization
  int Count_progress;
  QProgressDialog Progress("Computing CCVT...", "Abort",0,Number_of_dots);
  Progress.setWindowModality(Qt::WindowModal);
  Progress.setMinimumDuration(0);
  Progress.setCancelButton(0);

  bool stable;
  int Count_falses;
  do {
//    printf("iteration %d...", ++iteration);
    Count_falses=0;
    stable = optimizer.optimize(CENTROIDAL,Count_falses);
    Count_progress=Number_of_dots-Count_falses;
    Progress.setValue(Count_progress);
    if (Progress.wasCanceled()) break;
//    printf("done\n");
  } while (!stable);

  Progress.setValue(Number_of_dots);

  const Site<Point2>::Vector& result = optimizer.sites();

  // drawing the Voronoi sites
  Output_image->setTo(255);
  for (unsigned int i = 0; i < result.size(); ++i) {
  Output_image->at<unsigned char>(result[i].location.y,result[i].location.x)=(unsigned char)0;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_ccvt::update()
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
      Aux_output_image->create(Output_image_0->rows,Output_image_0->cols,CV_8UC1);

      ccvt(Aux_input_image,Aux_output_image);
      cvtColor(*Aux_output_image,*Output_image_0,CV_GRAY2RGB,3);
    }
    else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    ccvt(Input_image_0,Output_image_0);
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;

  update_number_of_dots();
  update_number_of_points_per_dots();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_ccvt_ui::_filter_ccvt_ui(_gl_widget *GL_widget1, string Name1)
{
  Name=Name1;
  Qtw_filter_ccvt=new _qtw_filter_ccvt(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_ccvt_ui::_filter_ccvt_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_ccvt=new _qtw_filter_ccvt(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_ccvt_ui::~_filter_ccvt_ui()
{
  delete Qtw_filter_ccvt;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_ccvt_ui::reset_data()
{
  _filter_ccvt::reset_data();
  Qtw_filter_ccvt->set_parameter1(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_ccvt_ui::show()
{
  Qtw_filter_ccvt->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_ccvt_ui::hide()
{
  Qtw_filter_ccvt->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_ccvt_ui::get_link()
{
  return Qtw_filter_ccvt->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_ccvt_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(CCVT_NUMBER_OF_DOTS_DEFAULT);
    parameter2(CCVT_NUMBER_OF_POINTS_PER_DOT_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["number_of_dots"]=="default") parameter1(CCVT_NUMBER_OF_DOTS_DEFAULT);
      else parameter1(atoi(Parameters["number_of_dots"].c_str()));

      if (Parameters["number_of_points_per_dots"]=="default") parameter2(CCVT_NUMBER_OF_POINTS_PER_DOT_DEFAULT);
      else parameter2(atoi(Parameters["number_of_points_per_dots"].c_str()));
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of CCVT filter");
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

void _filter_ccvt_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  char Aux[100];

  sprintf(Aux,"%d",parameter1());
  Parameters["number_of_dots"]=string(Aux);
  sprintf(Aux,"%d",parameter2());
  Parameters["number_of_points_per_dots"]=string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_ccvt_ui::update_number_of_dots()
{
  Qtw_filter_ccvt->set_parameter1(parameter1());
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_ccvt_ui::update_number_of_points_per_dots()
{
  Qtw_filter_ccvt->set_parameter2(parameter2());
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_ccvt::_qtw_filter_ccvt(_gl_widget *GL_widget1,_filter_ccvt_ui *Filter1,string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_main=new QVBoxLayout;

  // Parameter1
  // Number of dots
  QGroupBox *Group_box_parameter1=new QGroupBox(tr(String_group_box_parameter1.c_str()));
  Group_box_parameter1->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter1 = new QVBoxLayout;

  Spinbox_parameter1=new QSpinBox;
  Spinbox_parameter1->setRange(10,100000);
  Spinbox_parameter1->setValue(Filter->parameter1());
  Spinbox_parameter1->setToolTip(tr(String_parameter1_tooltip.c_str()));
  Spinbox_parameter1->setKeyboardTracking(false);
  Spinbox_parameter1->setAlignment(Qt::AlignRight);

  connect(Spinbox_parameter1, SIGNAL(valueChanged(int)),this,SLOT(set_parameter1_slot(int)));

  Vertical_box_parameter1->addWidget(Spinbox_parameter1);

  Group_box_parameter1->setLayout(Vertical_box_parameter1);

  // Parameter2
  // Number of points per dot
  QGroupBox *Group_box_parameter2=new QGroupBox(tr(String_group_box_parameter2.c_str()));
  Group_box_parameter2->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter2 = new QVBoxLayout;

  Spinbox_parameter2=new QSpinBox;
  Spinbox_parameter2->setRange(10,100000);
  Spinbox_parameter2->setValue(Filter->parameter2());
  Spinbox_parameter2->setToolTip(tr(String_parameter2_tooltip.c_str()));
  Spinbox_parameter2->setKeyboardTracking(false);
  Spinbox_parameter2->setAlignment(Qt::AlignRight);

  connect(Spinbox_parameter2, SIGNAL(valueChanged(int)),this,SLOT(set_parameter2_slot(int)));

  Vertical_box_parameter2->addWidget(Spinbox_parameter2);

  Group_box_parameter2->setLayout(Vertical_box_parameter2);

  //
  Vertical_box_main->addWidget(Group_box_parameter1);
  Vertical_box_main->addWidget(Group_box_parameter2);

  Group_box_main->setLayout(Vertical_box_main);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_ccvt::set_parameter1(int Value)
{
  Spinbox_parameter1->blockSignals(true);
  Spinbox_parameter1->setValue(Value);
  Spinbox_parameter1->blockSignals(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_ccvt::set_parameter2(int Value)
{
  Spinbox_parameter2->blockSignals(true);
  Spinbox_parameter2->setValue(Value);
  Spinbox_parameter2->blockSignals(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_ccvt::set_parameter1_slot(int Size)
{
  Filter->parameter1(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_ccvt::set_parameter2_slot(int Size)
{
  Filter->parameter2(Size);
  GL_widget->update_effect(Filter->Name);
}
