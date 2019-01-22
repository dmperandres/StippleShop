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


#include "filter_measure_ssim_psnr.h"
#include "glwidget.h"

using namespace _f_measure_ssim_psnr_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_measure_ssim_psnr::_filter_measure_ssim_psnr()
{

  Num_channels_input_image_0=1;
  Num_channels_input_image_1=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_MEASURE_SSIM_PSNR;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_measure_ssim_psnr::reset_data()
{

}



/*****************************************************************************//**
 * http://docs.opencv.org/doc/tutorials/gpu/gpu-basics-similarity/gpu-basics-similarity.html
 *
 *
 *
 *****************************************************************************/

cv::Scalar _filter_measure_ssim_psnr::getMSSIM(cv::Mat *i1,cv::Mat *i2)
{
  const double C1 = 6.5025, C2 = 58.5225;
  /***************************** INITS **********************************/
  int d     = CV_32F;

  cv::Mat I1, I2;
  i1->convertTo(I1, d);           // cannot calculate on one byte large values
  i2->convertTo(I2, d);

  cv::Mat I2_2   = I2.mul(I2);        // I2^2
  cv::Mat I1_2   = I1.mul(I1);        // I1^2
  cv::Mat I1_I2  = I1.mul(I2);        // I1 * I2

  /*************************** END INITS **********************************/

  cv::Mat mu1, mu2;   // PRELIMINARY COMPUTING
  GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
  GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);

  cv::Mat mu1_2   =   mu1.mul(mu1);
  cv::Mat mu2_2   =   mu2.mul(mu2);
  cv::Mat mu1_mu2 =   mu1.mul(mu2);

  cv::Mat sigma1_2, sigma2_2, sigma12;

  GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
  sigma1_2 -= mu1_2;

  GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
  sigma2_2 -= mu2_2;

  GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
  sigma12 -= mu1_mu2;

  ///////////////////////////////// FORMULA ////////////////////////////////
  cv::Mat t1, t2, t3;

  t1 = 2 * mu1_mu2 + C1;
  t2 = 2 * sigma12 + C2;
  t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

  t1 = mu1_2 + mu2_2 + C1;
  t2 = sigma1_2 + sigma2_2 + C2;
  t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

  cv::Mat ssim_map;
  divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

  cv::Scalar mssim = mean( ssim_map ); // mssim = average of ssim map

  return mssim;
}


/*****************************************************************************//**
 * http://docs.opencv.org/doc/tutorials/gpu/gpu-basics-similarity/gpu-basics-similarity.html
 *
 *
 *
 *****************************************************************************/

double _filter_measure_ssim_psnr::getPSNR(cv::Mat *I1,cv::Mat *I2)
{
  cv::Mat s1;
  cv::absdiff(*I1, *I2, s1);       // |I1 - I2|
  s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
  s1 = s1.mul(s1);           // |I1 - I2|^2

  cv::Scalar s = sum(s1);         // sum elements per channel

  double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

  if( sse <= 1e-10){ // for small values return zero
//    cout << "PSNR: " << 0 << endl;
    return 0;
  }
  else
  {
      double  mse =sse /(double)(I1->channels() * I1->total());
      double psnr = 10.0*log10((255.0*255.0)/mse);
      return psnr;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_measure_ssim_psnr::update()
{

  cv::Mat *Aux_input_image0=nullptr;
  cv::Mat *Aux_input_image1=nullptr;
  cv::Mat *Aux_output_image=nullptr;

  // check the output size
  if (Input_image_0->cols!=Output_image_0->cols || Input_image_0->rows!=Output_image_0->rows){
    Output_image_0->release();
    Output_image_0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
  }

  if (Input_image_0->channels()!=Input_image_1->channels() || Input_image_0->cols!=Input_image_1->cols || Input_image_0->rows!=Input_image_1->rows){
    QMessageBox Message_box;
    Message_box.setWindowTitle(QObject::tr("Warning"));
    Message_box.setText("Error in the input images of filter measure_ssim_psnr (different number of channels or sizes)");
    Message_box.exec();
    return;
  }

  // Check the number of input channels for image 0
  if (Input_image_0->channels()!=Num_channels_input_image_0){// Different number of channels
    if (Input_image_0->channels()==3){
      // conversion
      Aux_input_image0=new cv::Mat;
      Aux_input_image0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
      cvtColor(*Input_image_0,*Aux_input_image0,CV_BGR2GRAY,1);
    }
    else cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    Aux_input_image0=Input_image_0;
  }

  // Check the number of input channels for image 1
  if (Input_image_1->channels()!=Num_channels_input_image_1){// Different number of channels
    if (Input_image_1->channels()==3){
      // conversion
      Aux_input_image1=new cv::Mat;
      Aux_input_image1->create(Input_image_1->rows,Input_image_1->cols,CV_8UC1);
      cvtColor(*Input_image_1,*Aux_input_image1,CV_BGR2GRAY,1);
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

      Aux_input_image0->copyTo(*Output_image_0);
      cv::Scalar MSSIM=getMSSIM(Aux_input_image0,Aux_input_image1);
      double PSNR=getPSNR(Aux_input_image0,Aux_input_image1);

      set_info1(MSSIM[0]*10,PSNR);

      cvtColor(*Aux_input_image0,*Output_image_0,CV_GRAY2RGB,3);
    }
    else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    // copy the input
    Aux_input_image0->copyTo(*Output_image_0);
    cv::Scalar MSSIM=getMSSIM(Aux_input_image0,Aux_input_image1);
    double PSNR=getPSNR(Aux_input_image0,Aux_input_image1);

    set_info1(MSSIM[0]*100,PSNR);
  }

  if (Aux_input_image0!=nullptr && Aux_input_image0!=Input_image_0) delete Aux_input_image0;
  if (Aux_input_image1!=nullptr && Aux_input_image1!=Input_image_1) delete Aux_input_image1;
  if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_measure_ssim_psnr_ui::_filter_measure_ssim_psnr_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_measure_ssim_psnr=new _qtw_filter_measure_ssim_psnr(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_measure_ssim_psnr_ui::_filter_measure_ssim_psnr_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,string Box_name)
{
  read_parameters(Parameters);
  Name=Box_name;
  Qtw_filter_measure_ssim_psnr=new _qtw_filter_measure_ssim_psnr(GL_widget1,this,Box_name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_measure_ssim_psnr_ui::~_filter_measure_ssim_psnr_ui()
{
  delete Qtw_filter_measure_ssim_psnr;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_measure_ssim_psnr_ui::reset_data()
{
  _filter_measure_ssim_psnr::reset_data();
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_measure_ssim_psnr_ui::show()
{
  Qtw_filter_measure_ssim_psnr->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_measure_ssim_psnr_ui::hide()
{
  Qtw_filter_measure_ssim_psnr->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_measure_ssim_psnr_ui::get_link()
{
  return Qtw_filter_measure_ssim_psnr->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_measure_ssim_psnr_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  Q_UNUSED(Parameters)
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_measure_ssim_psnr_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  Q_UNUSED(Parameters)
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_measure_ssim_psnr_ui::set_info1(float SSIM1,float PSNR1)
{
  Qtw_filter_measure_ssim_psnr->set_info1(SSIM1,PSNR1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_measure_ssim_psnr::_qtw_filter_measure_ssim_psnr(_gl_widget *GL_widget1,_filter_measure_ssim_psnr_ui *Filter1,string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_main=new QVBoxLayout;

  // info 1
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

  // info 2
  QGroupBox *Group_box_info2=new QGroupBox(tr(String_group_box_info2.c_str()));
  Group_box_info2->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Verticalbox_info2 = new QVBoxLayout;

  Line_edit_info2=new QLineEdit();
  Line_edit_info2->setAlignment(Qt::AlignRight);
  Line_edit_info2->setReadOnly(true);
  Line_edit_info2->setEnabled(false);
  // warnig to the adjust
  Str=Str.number(0);
  Line_edit_info2->setText(Str);

  Verticalbox_info2->addWidget(Line_edit_info2);

  Group_box_info2->setLayout(Verticalbox_info2);

  Vertical_box_main->addWidget(Group_box_info1);
  Vertical_box_main->addWidget(Group_box_info2);

  Group_box_main->setLayout(Vertical_box_main);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_measure_ssim_psnr::set_info1(float SSIM1,float PSNR1)
{
  QString Str;

  Str=Str.number(SSIM1);
  Line_edit_info1->setText(Str);

  Str=Str.number(PSNR1);
  Line_edit_info2->setText(Str);
}
