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


#include "filter_combination.h"
#include "glwidget.h"

using namespace _f_combination_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_combination::_filter_combination()
{
  Combination_type=COMBINATION_PRODUCT;

  Invert_input_image_0=false;
  Invert_input_image_1=false;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;
  Num_channels_input_image_1=1;

  Type_filter=_f_filter_ns::FILTER_COMBINATION;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_combination::reset_data()
{
  Combination_type=COMBINATION_PRODUCT;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_combination::update()
{
  if (Input_image_0->cols!=Input_image_1->cols || Input_image_0->rows!=Input_image_1->rows){
    QMessageBox MsgBox;
    MsgBox.setText("Filter Combination");
    MsgBox.setInformativeText("Warning: the input images must have the same sizes");
    MsgBox.setStandardButtons(QMessageBox::Ok);
    MsgBox.exec();
    copy_input_to_output();
  }
  else{
    cv::Mat *Aux_input_image_0=nullptr;

    // check the output size
    if (Input_image_0->cols!=Output_image_0->cols || Input_image_0->rows!=Output_image_0->rows){
      Output_image_0->release();
      Output_image_0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
    }

    if (Input_image_1->channels()!=Num_channels_input_image_1){// Different number of channels
      cout << "Error: the number of channels in the input_image_1 is not 1" << __LINE__ << " " << __FILE__ << endl;
    }

    Aux_input_image_0=new cv::Mat;
    Aux_input_image_0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
    // Check the number of input channels
    if (Input_image_0->channels()!=Num_channels_input_image_0){// Different number of channels
      if (Input_image_0->channels()==3){
        // conversion
        cvtColor(*Input_image_0,*Aux_input_image_0,CV_BGR2GRAY,1);
      }
      else cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << endl;
    }
    else{// the same number of channels
      *Aux_input_image_0=Input_image_0->clone();
    }

    switch(Combination_type) {
    case COMBINATION_PRODUCT:
      *Aux_input_image_0=Aux_input_image_0->mul(*Input_image_1)/255;
      break;
    case COMBINATION_ADD:
      *Aux_input_image_0=(*Aux_input_image_0+*Input_image_1)/2;
      break;
    case COMBINATION_AND:
      *Aux_input_image_0=*Aux_input_image_0 & *Input_image_1;
      break;
    case COMBINATION_OR:
      *Aux_input_image_0=*Aux_input_image_0 | *Input_image_1;
      break;
    case COMBINATION_SUB:
      *Aux_input_image_0=*Aux_input_image_0 - *Input_image_1;
      break;
    case COMBINATION_DIF:
      *Aux_input_image_0=*Aux_input_image_0 - *Input_image_1;
      *Aux_input_image_0=cv::abs(*Aux_input_image_0);
      *Aux_input_image_0=255-*Aux_input_image_0;
      break;
    case COMBINATION_LAST:
      break;
    }


    // Check the number of output channels
    if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
      if (Output_image_0->channels()==3){
        // conversion
        cvtColor(*Aux_input_image_0,*Output_image_0,CV_GRAY2RGB,3);
      }
      else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
    }
    else{// the same number of channels
      *Output_image_0=*Aux_input_image_0;
    }

    delete Aux_input_image_0;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_combination_ui::_filter_combination_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_combination=new _qtw_filter_combination(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_combination_ui::_filter_combination_ui(_gl_widget *GL_widget1, std::map<string, string> &Parameters, string Name1)
{
  read_parameters(Parameters);
  Name=Name1;
  Qtw_filter_combination=new _qtw_filter_combination(GL_widget1,this,Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_combination_ui::~_filter_combination_ui()
{
  delete Qtw_filter_combination;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_combination_ui::reset_data()
{
  _filter_combination::reset_data();
  Qtw_filter_combination->set_parameter1(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_combination_ui::show()
{
  Qtw_filter_combination->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_combination_ui::hide()
{
  Qtw_filter_combination->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_combination_ui::get_link()
{
  return Qtw_filter_combination->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_combination_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(COMBINATION_PRODUCT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["operation"]=="default") parameter1(COMBINATION_PRODUCT);
      else parameter1(Operations_inv[Parameters["operation"]]);
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of COMBINATION filter");
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

void _filter_combination_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  Parameters["operation"]=Operations[parameter1()];
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_combination::_qtw_filter_combination(_gl_widget *GL_widget1,_filter_combination_ui *Filter1,string Box_name)
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

  QVBoxLayout *Vertical_parameter1=new QVBoxLayout;

  Combo_parameter1 = new QComboBox;
  for (unsigned int i=0;i<(unsigned int) COMBINATION_LAST;i++){
    Combo_parameter1->addItem(tr(Parameter1_text[i].c_str()));
  }
  Combo_parameter1->setCurrentIndex((int)Filter->parameter1());
  Combo_parameter1->setToolTip(tr("This value allows the operation"));

  Vertical_parameter1->addWidget(Combo_parameter1);
  Group_box_parameter1->setLayout(Vertical_parameter1);
  //
  Vertical_box_main->addWidget(Group_box_parameter1);

  Group_box_main->setLayout(Vertical_box_main);

  connect(Combo_parameter1, SIGNAL(activated(int)), this,SLOT(set_parameter1_slot(int)));
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_combination::set_parameter1(_combination_type Value)
{
  Combo_parameter1->setCurrentIndex((int)Value);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_combination::set_parameter1_slot(int Value)
{
  Filter->parameter1((_combination_type) Value);
  GL_widget->update_effect(Filter->Name);
}
