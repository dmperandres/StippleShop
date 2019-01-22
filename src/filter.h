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


#ifndef FILTER_H
#define FILTER_H

#ifdef X64
  #include <opencv.hpp>
#else
  #include <imgproc/imgproc.hpp>
  #include <highgui/highgui.hpp>
#endif
#include <string>
#include <memory>
#include <string>


namespace _f_filter_ns
{
  typedef struct {
    std::string Type;
    std::string Name;
    int Num_parameters;
    int Num_input_ports;
    unsigned char Num_channels;
  } _filter_parameters;

  typedef struct {
    std::string Name;
    std::string Description;
  } _filter_text;

  // 1 include the name of the filter as an identifier
  typedef enum {FILTER_CANNY=10,FILTER_SOBEL,FILTER_COMBINATION,FILTER_CONTRAST_BRIGHT,FILTER_DILATION,FILTER_EROTION,FILTER_DOG,FILTER_HALFTONING_OST,FILTER_KANG,FILTER_RETINEX,FILTER_DISTANCE_FIELD,FILTER_HALFTONING_CAH,FILTER_GAUSSIAN,FILTER_INVERSION,FILTER_STIPPLING_EBG,FILTER_STIPPLING_EBG_MASK,FILTER_STIPPLING_SAS,FILTER_STIPPLING_SBE,FILTER_RWT,FILTER_MEASURE_SSIM_PSNR,FILTER_WCVD,FILTER_THRESHOLD,FILTER_DOT_EBG,FILTER_DOT_SVG,FILTER_CCVT,FILTER_HALFTONING_SFC, FILTER_HALFTONING_ACSP, FILTER_BILATERAL} _type_filter;

  // 2 include the name of the filter as a string
  static std::map<std::string,_type_filter> Type_filter_name={{"CANNY",FILTER_CANNY},{"SOBEL",FILTER_SOBEL},{"CONTRAST",FILTER_CONTRAST_BRIGHT},{"DILATION",FILTER_DILATION},{"EROTION",FILTER_EROTION},{"DOG",FILTER_DOG},{"HALFTONING_OST",FILTER_HALFTONING_OST},{"KANG",FILTER_KANG},{"RETINEX",FILTER_RETINEX},{"COMBINATION",FILTER_COMBINATION},{"HALFTONING_CAH",FILTER_HALFTONING_CAH},{"GAUSSIAN",FILTER_GAUSSIAN},{"INVERSION",FILTER_INVERSION},{"DISTANCE_FIELD",FILTER_DISTANCE_FIELD},{"STIPPLING_EBG",FILTER_STIPPLING_EBG},{"STIPPLING_SAS",FILTER_STIPPLING_SAS},{"STIPPLING_SBE",FILTER_STIPPLING_SBE},{"RWT",FILTER_RWT},{"SSIM_PSNR",FILTER_MEASURE_SSIM_PSNR},{"WCVD",FILTER_WCVD},{"THRESHOLD",FILTER_THRESHOLD},{"DOT_EBG",FILTER_DOT_EBG},{"DOT_SVG",FILTER_DOT_SVG},{"CCVT",FILTER_CCVT},{"HALFTONING_SFC",FILTER_HALFTONING_SFC},{"HALFTONING_ACSP",FILTER_HALFTONING_ACSP},{"BILATERAL",FILTER_BILATERAL}};

  // 3 information for the node editor
  // Type, Name, Num_parameters, Num_input_ports, num_channels (for 3 channels draw in orange)
  static std::map<_type_filter,_filter_parameters> Filter_name_parameters={{FILTER_CANNY,{"CANNY","canny",3,1,1}},{FILTER_CONTRAST_BRIGHT,{"CONTRAST","contrast",2,1,1}},{FILTER_DILATION,{"DILATION","dilation",2,1,1}},{FILTER_EROTION,{"EROTION","erotion",2,1,1}},{FILTER_DOG,{"DOG","dog",3,1,1}},{FILTER_HALFTONING_OST,{"HALFTONING_OST","halftoning_ost",0,1,1}},{FILTER_KANG,{"KANG","kang",0,1,1}},{FILTER_RETINEX,{"RETINEX","retinex",1,1,3}},{FILTER_COMBINATION,{"COMBINATION","combination",1,2,1}},{FILTER_HALFTONING_CAH,{"HALFTONING_CAH","halftoning_cah",2,1,1}},{FILTER_GAUSSIAN,{"GAUSSIAN","gaussian",1,1,1}},{FILTER_INVERSION,{"INVERSION","inversion",2,1,1}},{FILTER_DISTANCE_FIELD,{"DISTANCE_FIELD","distance_field",1,1,1}},{FILTER_STIPPLING_EBG,{"STIPPLING_EBG","stippling_ebg",1,1,1}},{FILTER_STIPPLING_EBG_MASK,{"STIPPLING_EBG_MASK","stippling_ebg_mask",1,1,1}},{FILTER_STIPPLING_SAS,{"STIPPLING_SAS","stippling_sas",5,1,1}},{FILTER_STIPPLING_SBE,{"STIPPLING_SBE","stippling_sbe",3,1,1}},{FILTER_RWT,{"RWT","rwt",4,1,1}},{FILTER_MEASURE_SSIM_PSNR,{"SSIM_PSNR","ssim_psnr",0,2,1}},{FILTER_WCVD,{"WCVD","wcvd",2,1,1}},{FILTER_THRESHOLD,{"THRESHOLD","threshold",2,1,1}},{FILTER_DOT_EBG,{"DOT_EBG","dot_ebg",1,2,1}},{FILTER_DOT_SVG,{"DOT_SVG","dot_svg",1,2,1}},{FILTER_CCVT,{"CCVT","ccvt",3,1,1}},{FILTER_HALFTONING_SFC,{"HALFTONING_SFC","halftoning_sfc",1,1,1}},{FILTER_HALFTONING_ACSP,{"HALFTONING_ACSP","halftoning_acsp",4,1,1}},{FILTER_BILATERAL,{"BILATERAL","bilateral",2,1,1}}};

  // 4 help information
  static std::map<_type_filter,_filter_text> Filter_name_text={
  {FILTER_CANNY,{"<b>Canny</b>","<p>This filter applies Canny edge detector</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_CONTRAST_BRIGHT,{"<b>Contrast-Bright</b>","<p>This filter applies a change in contrast and/or bright</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_DILATION,{"<b>Dilation</b>","<p>This filter applies a dilation</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_EROTION,{"<b>Erotion</b>","<p>This filter applies a erotion</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_DOG,{"<b>Difference of Gaussians</b>","<p>This filter applies a Difference of Gaussians selecting only the edges</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_HALFTONING_OST,{"<b>Ostromoukhov</b>","<p>This filter applies Ostromoukhov's halftoning</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_KANG,{"<b>Kang</b>","<p>This filter applies Kang's lines detector</p><p><b>Input:</b> <font color='#ff0000' font size=4>Color image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_RETINEX,{"<b>Retinex</b>","<p>This filter applies a local contrast with Retinex</p><p><b>Input:</b> <font color='#ff0000' font size=4>Color image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_COMBINATION,{"<b>Combination</b>","<p>This filter combines two inputs to produce one output applying an operation</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_HALFTONING_CAH,{"<b>Contrast-Aware Halftoning</b>","<p>This filter applies the Contrast-Aware halftoning</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_GAUSSIAN,{"<b>Gaussian</b>","<p>This filter applies a Gaussian blur</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_INVERSION,{"<b>Inversion</b>","<p>This filter applies an Inversion</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_DISTANCE_FIELD,{"<b>Distance Field</b>","<p>This filter computes a distance field</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_STIPPLING_EBG,{"<b>Stippling Exaple-Based Grayscale</b>","<p>This filter applies the Example-Based Grayscale stippling method</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_STIPPLING_EBG_MASK,{"<b>Stippling Exaple-Based Grayscale with Mask</b>","<p>This filter applies the Example-Based Grayscale stippling with mask method </p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_STIPPLING_SAS,{"<b>Structure-Aware Stippling</b>","<p>This filter applies Structure-Aware stipling</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_STIPPLING_SBE,{"<b>Stippling by Example</b>","<p>This filter applies Stippling by Example method</p><p><b>Input:</b> Grayscale image <font color='#ff0000' font size=4>(best for 4096x4096 size)</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_RWT,{"<b>Recursive Wang Tiling</b>","<p>This filter applies Recursive Wang Tiling method</p><p><b>Input:</b> Grayscale image <font color='#ff0000' font size=4>(square)</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_MEASURE_SSIM_PSNR,{"<b>Measure SSIM and PSNR</b>","<p>This filter measures the SSIM and PSNR of Input 1 with relation to Input 0. The output is equal to Input 0</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_WCVD,{"<b>Weigthed Centroidal Voronoi Diagram</b>","<p>This filter applies the Weigthed Centroidal Voronoi Diagram method</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_THRESHOLD,{"<b>Threshold</b>","<p>This filter applies a threshold</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_DOT_EBG,{"<b>Dot EBG</b>","<p>This filter draw the dots with scanned real dots</p><p><b>Input 0:</b> Grayscale image (placement)</p><b>Input 1:</b> Grayscale image (dot size modulation)</p><p><b>Output:</b> Grayscale image</p><p>Use: black pixels of the input image 0 are converted to dots. The input image 1 can be used to modulate the size of the dots: the darker the bigger</p>"}},
  {FILTER_DOT_SVG,{"<b>Dot SVG</b>","<p>This filter draw the dots with vectorial figures</p><p><b>Input 0:</b> Grayscale image (placement)</p><b>Input 1:</b> Grayscale image (dot size modulation)</p><p><b>Output:</b> Grayscale image</p><p>Use: black pixels of the input image 0 are converted to dots. The input image 1 can be used to modulate the size of the dots: the darker the bigger</p>"}},
  {FILTER_CCVT,{"<b>Capacity-Constrained Voronoid Tessellation</b>","<p>This filter applies Capacity-Constrained Voronoid Tesseallation method</p><p><b>Input:</b> Grayscale image <font color='#ff0000' font size=4>(square)</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_HALFTONING_SFC,{"Space Filling Curve","<p>This filter applies Space Filling Curve halftoning</p><p><b>Input:</b> Grayscale image <font color='#ff0000' font size=4>(square, power of 2)</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_HALFTONING_ACSP,{"<b>Adaptive Clustering Selective Precipitation</b>","<p>This filter applies Adaptive Clustering Selective Precipitation halftoning</p><p><b>Input:</b> Grayscale image <font color='#ff0000' font size=4>(square, power of 2)</p><p><b>Output:</b> Grayscale image</p>"}},
  {FILTER_BILATERAL,{"<b>Bilateral</b>","<p>This filter applies a bilateral filter</p><p><b>Input:</b> Grayscale image</p><p><b>Output:</b> Grayscale image</p>"}}};
}


class _gl_widget;

class _filter
{
public:

  virtual ~_filter()=0;
  std::string name(){return Name;};
  void set_images(cv::Mat *Input_image0, cv::Mat *Output_image0, cv::Mat *Input_image1=nullptr);
  _f_filter_ns::_type_filter type_filter(){return Type_filter;};
  bool change_output_image_size(){return Change_output_image_size;};
  void set_change_output_image_size(bool Change_output_image_size1){Change_output_image_size=Change_output_image_size1;};
  void set_scaling_factor(float Scaling_factor1){Scaling_factor=Scaling_factor1;};

  float scaling_factor(){return Scaling_factor;};

  virtual void update()=0;
  virtual void show()=0;
  virtual void hide()=0;
  virtual void *get_link()=0;
  virtual void reset_data()=0;
  virtual void read_parameters(std::map<std::string,std::string> &Parameters)=0;
  virtual void write_parameters(std::map<std::string,std::string> &Parameters)=0;
  virtual bool use_dots()=0;

  void copy_input_to_output();

  cv::Mat *Input_image_0;
  cv::Mat *Output_image_0;
  cv::Mat *Input_image_1;

  int Num_channels_input_image_0;
  int Num_channels_output_image_0;
  int Num_channels_input_image_1;
  
  bool Ready=false;
  std::string Name;
  _f_filter_ns::_type_filter Type_filter;
  bool Change_output_image_size=false;
  float Scaling_factor=1.0;
  bool Use_dots=false;
};

#endif
