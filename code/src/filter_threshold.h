#ifndef _FILTER_THRESHOLD_H
#define _FILTER_THRESHOLD_H

#include <QDialog>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>

#ifdef X64
  #include <opencv.hpp>
#else
  #include <core/core.hpp>
  #include <highgui/highgui.hpp>
#endif

#include "common.h"
#include "line_edit.h"
#include <string>
#include "filter.h"


namespace _f_threshold
{

typedef enum {THRESHOLD_BINARY,THRESHOLD_BINARY_INVERTED,THRESHOLD_TRUNCATED,THRESHOLD_TO_ZERO,THRESHOLD_TO_ZERO_INVERTED,THRESHOLD_LAST} _threshold_type;

// Type
// parameter 1
const string String_group_box_parameter1("Threshold type");
const string Parameter1_text[THRESHOLD_LAST]={"binary","binary_inverted","truncated","to_zero","to_zero_inverted"};

// threshold
//parameter2
const string String_group_box_parameter2("Threshold");
const string String_label_parameter2_min("0 ");
const string String_label_parameter2_max("255");
const string String_label_parameter2_tooltip("Display the value of the threshold");
const int Parameter2_min_value=0;
const int Parameter2_max_value=255;
const int Parameter2_single_step=1;
const int Parameter2_page_step=5;
const int Parameter2_tick_interval=1;
const bool Parameter2_set_tracking=false;
const string String_parameter2_slider_tooltip("Controls the threshold");

// Max_binary_value
//parameter3
const string String_group_box_parameter3("Max binary value");
const string String_label_parameter3_min("0 ");
const string String_label_parameter3_max("255");
const string String_label_parameter3_tooltip("Display the max binary value");
const int Parameter3_min_value=0;
const int Parameter3_max_value=255;
const int Parameter3_single_step=1;
const int Parameter3_page_step=5;
const int Parameter3_tick_interval=1;
const bool Parameter3_set_tracking=false;
const string String_parameter3_slider_tooltip("Controls the max binary value");

// Default values
const _threshold_type THRESHOLD_TYPE_DEFAULT=THRESHOLD_BINARY;
const int THRESHOLD_THRESHOLD_DEFAULT=128;
const int THRESHOLD_MAX_BINARY_VALUE_DEFAULT=255;

static std::map<_threshold_type,std::string> Operations={{THRESHOLD_BINARY,"binary"},{THRESHOLD_BINARY_INVERTED,"binary_inverted"},{THRESHOLD_TRUNCATED,"truncated"},{THRESHOLD_TO_ZERO,"to_zero"},{THRESHOLD_TO_ZERO_INVERTED,"to_zero_inverted"}};

static std::map<std::string,_threshold_type> Operations_inv={{"binary",THRESHOLD_BINARY},{"binary_inverted",THRESHOLD_BINARY_INVERTED},{"truncated",THRESHOLD_TRUNCATED},{"to_zero",THRESHOLD_TO_ZERO},{"to_zero_inverted",THRESHOLD_TO_ZERO_INVERTED}};


}

class _gl_widget;
class _qtw_filter_threshold;

class _filter_threshold : public _filter
{
public:
    _filter_threshold();
    ~_filter_threshold(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};


    void update();

    void threshold_type(_f_threshold::_threshold_type Threshold_type1){Threshold_type=Threshold_type1;};
    _f_threshold::_threshold_type threshold_type(){return Threshold_type;};
    void threshold(int Threshold1){Threshold=Threshold1;};
    int threshold(){return Threshold;};
    void max_binary_value(int Max_binary_value1){Max_binary_value=Max_binary_value1;};
    int  max_binary_value(){return Max_binary_value;};

    _f_threshold::_threshold_type Threshold_type;
    int Threshold;
    int Max_binary_value;
};



class _filter_threshold_ui :public _filter_threshold
{
public:
  _filter_threshold_ui(_gl_widget *GL_widget1,string Name1="Threshold parameters");
  _filter_threshold_ui(_gl_widget *GL_widget1,std::map<string,string> &Parameters,string Box_name="Threshold parameters");
  ~_filter_threshold_ui();
  void reset_data();

  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<string, string> &Parameters);

  void parameter1(_f_threshold::_threshold_type Value){threshold_type(Value);};
  int parameter1(){return (int)threshold_type();};
  void parameter2(int Value){threshold(Value);};
  int parameter2(){return threshold();};
  void parameter3(int Value){max_binary_value(Value);};
  int parameter3(){return max_binary_value();};

  private:
  _qtw_filter_threshold *Qtw_filter_threshold;

};


class _qtw_filter_threshold: public QWidget
{
  Q_OBJECT
public:
  _qtw_filter_threshold(_gl_widget *GL_widget1,_filter_threshold_ui *Filter1,string Box_name="Threshold parameters");
  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(int Value);
  void set_parameter2(int Value);
  void set_parameter3(int Value);


protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);
  void set_parameter3_slot(int Value);

private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter1;
  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;

  // type
  QComboBox *Combo_parameter1;

  // threshold
  QSlider *Slider_parameter2;
  QLineEdit *Line_edit_parameter2;

  // max_binary_value
  QSlider *Slider_parameter3;
  QLineEdit *Line_edit_parameter3;


  _filter_threshold_ui *Filter;
  _gl_widget *GL_widget;
};
#endif // _FILTER_threshold_H
