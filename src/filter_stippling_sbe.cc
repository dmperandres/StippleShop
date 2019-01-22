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


#include "filter_stippling_sbe.h"
#include "glwidget.h"

using namespace _f_stippling_sbe1_ns;
using namespace _f_stippling_sbe_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_sbe::_filter_stippling_sbe()
{

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_STIPPLING_SBE;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_sbe::reset_data()
{
  Save_intermediate_results=STIPPLING_SBE_SAVE_INTERMEDIATE_RESULTS_DEFAULT;
  Combine_edges=STIPPLING_SBE_COMBINE_EDGES_DEFAULT;
  Post_processing=STIPPLING_SBE_POST_PROCESSING_DEFAULT;

}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_sbe::stippling(cv::Mat *Input_image0,cv::Mat *Output_image0)
{


#ifdef INFO
  printf("//----------------------------------//\n");
  printf("//      Stippling By Example        //\n");
  printf("//----------------------------------//\n");
#endif

  //char filename[1024];

  //----------------------------------------------
  // Inpute: distribution map, stippleDB, gray source image
  // Output: final rendering image
  //----------------------------------------------
#ifdef INFO
  printf("\n[Process: Rendering]\n");
#endif
  // Source image
//  IplImage* srcImg = cvLoadImage(SOURCE_IMAGE_FILE, -1);
//  IplImage* srcImg = cvLoadImage("./images/input.png", -1);
//  if (srcImg == 0)
//  {
//    printf("Can't find source image...\n");
//    exit(-1);
//  }

//  cvFlip(srcImg,srcImg,0);


  IplImage *srcImg=new IplImage(*Input_image0);
//  IplImage *graySrcImg =srcImg;

//  IplImage* graySrcImg = cvCreateImage(cvSize(srcImg->width, srcImg->height), 8, 1);
//  if (srcImg->nChannels != 1)
//    cvCvtColor(srcImg, graySrcImg, CV_BGR2GRAY);
//  else
//    cvCopy(srcImg, graySrcImg);

//  sprintf(filename, "%ssource.png", "./sbe/rendered/");
//  cvSaveImage(filename, graySrcImg);
//  // Temp...

  // Load synthesized distributions of stipple textures
  CDistributionMap distMap;
  //  distMap.load(DISTRIBUTION_MAP_FILE);
  distMap.load((char *)"./sbe/tones/tones.txt");

  // Edge image given by a user
  IplImage* edgeImg_in = 0;
  if (GIVEN_EDGE_IMAGE)
  {
    edgeImg_in = cvLoadImage(EDGE_IMAGE_FILE, 0);
    if (!edgeImg_in)
    {
      printf("Can't find the edge image, %s...\n", EDGE_IMAGE_FILE);
      exit (-1);
    }
  }

  // Final image for rendering
  IplImage* renderImg = cvCreateImage(cvSize(srcImg->width, srcImg->height), 8, 1);
  cvSet(renderImg, cvScalar((unsigned char)BACKGROUND_COLOR));

  // Stipple rendering
  CRender render;

  render.render(srcImg, renderImg, &distMap,
          STIPPLE_MARK_TYPE,"./sbe/stipple_black/", "./sbe/stipple_white/",
          RENDER_STIPPLE_MARK_FLAG,
          Combine_edges, edgeImg_in,
          Post_processing,
          "./sbe/rendered/",
          GIVEN_EDGE_IMAGE,
          WHITE_STIPPLE_START_INDEX,
          USE_TONE_SELECTION_BASEDON_PROBABILITY,
          USE_FULL_LINEAR,
          USE_TANH_FLAG,
          STIPPLE_COLOR,
          BACKGROUND_COLOR);


  // Save final result
//  sprintf(filename, "%sresult.png","./sbe/rendered/");
//  cvSaveImage(filename, renderImg);


#ifdef X64
  cv::Mat Image1=cv::cvarrToMat(renderImg,true);
#else
  cv::Mat Image1(renderImg);
#endif

  Image1.copyTo(*Output_image0);

//  cvReleaseImage(&srcImg);
//  cvReleaseImage(&graySrcImg);
//  cvReleaseImage(&renderImg);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_sbe::update()
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
    if (change_output_image_size()){
      Aux_input_image=new cv::Mat;
			cv::resize(*Input_image_0, *Aux_input_image, cv::Size(), Scaling_factor, Scaling_factor, cv::INTER_LINEAR);
    }
    else{
      Aux_input_image=Input_image_0;
    }
  }

  // Check the number of output channels
  if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
    if (Output_image_0->channels()==3){
      // conversion
      Aux_output_image=new cv::Mat;
      Aux_output_image->create(Output_image_0->rows,Output_image_0->cols,CV_8UC1);

      stippling(Aux_input_image,Aux_output_image);
      cvtColor(*Aux_output_image,*Output_image_0,CV_GRAY2RGB,3);
    }
    else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
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

_filter_stippling_sbe_ui::_filter_stippling_sbe_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_stippling_sbe=new _qtw_filter_stippling_sbe(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_sbe_ui::_filter_stippling_sbe_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,string Box_name)
{
  read_parameters(Parameters);
  Name=Box_name;
  Qtw_filter_stippling_sbe=new _qtw_filter_stippling_sbe(GL_widget1,this,Box_name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_stippling_sbe_ui::~_filter_stippling_sbe_ui()
{
  delete Qtw_filter_stippling_sbe;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_sbe_ui::reset_data()
{
  _filter_stippling_sbe::reset_data();
  Qtw_filter_stippling_sbe->set_parameter1(parameter1());
  Qtw_filter_stippling_sbe->set_parameter2(parameter2());
  Qtw_filter_stippling_sbe->set_parameter3(parameter3());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_sbe_ui::show()
{
  Qtw_filter_stippling_sbe->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_sbe_ui::hide()
{
  Qtw_filter_stippling_sbe->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_stippling_sbe_ui::get_link()
{
  return Qtw_filter_stippling_sbe->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_stippling_sbe_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    parameter1(STIPPLING_SBE_SAVE_INTERMEDIATE_RESULTS_DEFAULT);
    parameter2(STIPPLING_SBE_COMBINE_EDGES_DEFAULT);
    parameter3(STIPPLING_SBE_POST_PROCESSING_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      if (Parameters["save_intermediate_results"]=="default") parameter1(STIPPLING_SBE_SAVE_INTERMEDIATE_RESULTS_DEFAULT);
      else{
        if (Parameters["save_intermediate_results"]=="true") parameter1(true);
        else parameter1(false);
      }
      if (Parameters["combine_edges"]=="default") parameter2(STIPPLING_SBE_COMBINE_EDGES_DEFAULT);
      else{
        if (Parameters["combine_edges"]=="true") parameter2(true);
        else parameter2(false);
      }
      if (Parameters["post_processing"]=="default") parameter3(STIPPLING_SBE_POST_PROCESSING_DEFAULT);
      else{
        if (Parameters["post_processing"]=="true") parameter3(true);
        else parameter3(false);
      }
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of SBE filter");
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

void _filter_stippling_sbe_ui::write_parameters(std::map<std::string,std::string> &Parameters)
{
  if (parameter1()) Parameters["save_intermediate_results"]=string("true");
  else Parameters["save_intermediate_results"]=string("false");
  if (parameter2()) Parameters["combine_edges"]=string("true");
  else Parameters["combine_edges"]=string("false");
  if (parameter3()) Parameters["post_processing"]=string("true");
  else Parameters["post_processing"]=string("false");
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_stippling_sbe::_qtw_filter_stippling_sbe(_gl_widget *GL_widget1,_filter_stippling_sbe_ui *Filter1,string Box_name)
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

  Checkbox_parameter1=new QCheckBox;
  if (Filter->parameter1()) Checkbox_parameter1->setCheckState(Qt::Checked);
  else Checkbox_parameter1->setCheckState(Qt::Unchecked);
  Checkbox_parameter1->setToolTip(tr(String_parameter1_tooltip.c_str()));

  Vertical_box_parameter1->addWidget(Checkbox_parameter1);

  Group_box_parameter1->setLayout(Vertical_box_parameter1);

  // Parameter 2
  QGroupBox *Group_box_parameter2=new QGroupBox(tr(String_group_box_parameter2.c_str()));
  Group_box_parameter2->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter2=new QVBoxLayout;

  Checkbox_parameter2=new QCheckBox;
  if (Filter->parameter2()) Checkbox_parameter2->setCheckState(Qt::Checked);
  else Checkbox_parameter2->setCheckState(Qt::Unchecked);
  Checkbox_parameter2->setToolTip(tr(String_parameter2_tooltip.c_str()));

  Vertical_box_parameter2->addWidget(Checkbox_parameter2);

  Group_box_parameter2->setLayout(Vertical_box_parameter2);

  // parameter 3
  QGroupBox *Group_box_parameter3=new QGroupBox(tr(String_group_box_parameter3.c_str()));
  Group_box_parameter3->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter3=new QVBoxLayout;

  Checkbox_parameter3=new QCheckBox;
  if (Filter->parameter3()) Checkbox_parameter3->setCheckState(Qt::Checked);
  else Checkbox_parameter3->setCheckState(Qt::Unchecked);
  Checkbox_parameter3->setToolTip(tr(String_parameter3_tooltip.c_str()));

  Vertical_box_parameter3->addWidget(Checkbox_parameter3);

  Group_box_parameter3->setLayout(Vertical_box_parameter3);

  Vertical_box_main->addWidget(Group_box_parameter1);
  Vertical_box_main->addWidget(Group_box_parameter2);
  Vertical_box_main->addWidget(Group_box_parameter3);

  Group_box_main->setLayout(Vertical_box_main);

  connect(Checkbox_parameter1, SIGNAL(stateChanged(int)),this,SLOT(set_parameter1_slot(int)));
  connect(Checkbox_parameter2, SIGNAL(stateChanged(int)),this,SLOT(set_parameter2_slot(int)));
  connect(Checkbox_parameter3, SIGNAL(stateChanged(int)),this,SLOT(set_parameter3_slot(int)));
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_sbe::set_parameter1(bool Value)
{
  if (Value) Checkbox_parameter1->setCheckState(Qt::Checked);
  else Checkbox_parameter1->setCheckState(Qt::Unchecked);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_sbe::set_parameter2(bool Value)
{
  if (Value) Checkbox_parameter2->setCheckState(Qt::Checked);
  else Checkbox_parameter2->setCheckState(Qt::Unchecked);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_sbe::set_parameter3(bool Value)
{
  if (Value) Checkbox_parameter3->setCheckState(Qt::Checked);
  else Checkbox_parameter3->setCheckState(Qt::Unchecked);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_sbe::set_parameter1_slot(int Size)
{
  if (Size==Qt::Checked) Filter->parameter1(true);
  else Filter->parameter1(false);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_sbe::set_parameter2_slot(int Size)
{
  if (Size==Qt::Checked) Filter->parameter2(true);
  else Filter->parameter2(false);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_stippling_sbe::set_parameter3_slot(int Size)
{
  if (Size==Qt::Checked) Filter->parameter3(true);
  else Filter->parameter3(false);
  GL_widget->update_effect(Filter->Name);
}



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int CDistributionMap::load(char* filename)
{
  setlocale(LC_NUMERIC, "C");

  char Name1[255];

  int count = 0;
  std::string line ;
  std::string File_name(filename);
  std::string Directory;

  Directory=File_name.substr(0,File_name.find_last_of("/\\")+1);

//  cout << "Directory: " << Directory << endl;

  std::ifstream infile(filename) ;
  if ( infile ) {
    // first line
    getline( infile , line );
    // number of elements
    getline( infile , line );
    count=std::stoi(line);
    // read the names of the images
    for (int i=0;i<count;i++){
      getline( infile , line );
      File_name=Directory+line;
#ifdef INFO
      cout << "File: " << File_name << endl;
#endif

      sprintf(Name1,"./sbe/tones/tone%d_4096x4096.png",i);

      IplImage* img = cvLoadImage(Name1, -1);

//      IplImage* img = cvLoadImage("./tones/tone0_4096x4096.bmp", -1);
      if (img==nullptr) cout << "error" << endl;
      addMap(img);
    }


    // read the tones
    float avgGrayValue;
    for (int i=0;i<count;i++){
      getline( infile , line );
      avgGrayValue=stof(line);
#ifdef INFO
      cout << "Value: " << avgGrayValue << endl;
#endif
      _avgGrayValue.push_back((double)avgGrayValue);
    }
  }
  infile.close() ;

  return count;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CDistributionMap::computeAvgGrayValue()
{
  int count = getCount();
  IplImage* img = 0;
  int width, height, widthStep, size;

  double graySum;

  for (int i = 0; i < count; ++i)
  {
    img = getMap(i);

    width = img->width;
    height = img->height;
    widthStep = img->widthStep;
    size = width * height;

    graySum = 0;

    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        graySum += (unsigned char) img->imageData[y*widthStep + x];
      }
    }

    graySum /= size;

    _avgGrayValue.push_back(graySum);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CGaussianFilter::MakeGaussianKernel(float sigma, float **kernel, int *windowsize)
{
   int i, center;
   float x, fx, sum=0.0;

   *windowsize = 1 + int (2.0 * ceil(2.5 * sigma));
   center = (*windowsize) / 2;

   if((*kernel = (float *) calloc((*windowsize), sizeof(float))) == 0){
      fprintf(stderr, "Error callocing the gaussian kernel array.\n");
      exit(1);
   }

   for(i=0;i<(*windowsize);i++){
      x = (float)(i - center);
      fx = pow(2.71828f, -0.5f*x*x/(sigma*sigma)) / (sigma * sqrt(6.2831853f));
      (*kernel)[i] = fx;
      sum += fx;
   }

   for(i=0;i<(*windowsize);i++) (*kernel)[i] /= sum;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CGaussianFilter::GaussianSmooth(unsigned char* image, int rows, int cols, int widthStep, float sigma, short int **smoothedim)
{
   int r, c, rr, cc,     /* Counter variables. */
      windowsize,        /* Dimension of the gaussian kernel. */
      center;            /* Half of the windowsize. */
   float *tempim,        /* Buffer for separable filter gaussian smoothing. */
         *kernel,        /* A one dimensional gaussian kernel. */
         dot,            /* Dot product summing variable. */
         sum;            /* Sum of the kernel weights variable. */

   MakeGaussianKernel(sigma, &kernel, &windowsize);
   center = windowsize / 2;

   if((tempim = (float *) calloc(rows*cols, sizeof(float))) == 0){
      fprintf(stderr, "Error allocating the buffer image.\n");
      exit(1);
   }
   if(((*smoothedim) = (short int *) calloc(rows*cols,
         sizeof(short int))) == NULL){
      fprintf(stderr, "Error allocating the smoothed image.\n");
      exit(1);
   }

   for(r=0;r<rows;r++){
      for(c=0;c<cols;c++){
         dot = 0.0;
         sum = 0.0;
         for(cc=(-center);cc<=center;cc++){
            if(((c+cc) >= 0) && ((c+cc) < cols)){
               dot += (float)image[r*cols+(c+cc)] * kernel[center+cc];
         sum += kernel[center+cc];
            }
         }
         tempim[r*cols+c] = dot/sum;
      }
   }

   for(c=0;c<cols;c++){
      for(r=0;r<rows;r++){
         sum = 0.0;
         dot = 0.0;
         for(rr=(-center);rr<=center;rr++){
            if(((r+rr) >= 0) && ((r+rr) < rows)){
               dot += tempim[(r+rr)*cols+c] * kernel[center+rr];
               sum += kernel[center+rr];
            }
         }
         (*smoothedim)[r*cols+c] = (short int)(dot*BOOSTBLURFACTOR/sum + 0.5);
      }
   }

   free(tempim);
   free(kernel);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

CDoGFilter::CDoGFilter()
{
  _pGaussianFilter = new CGaussianFilter();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

CDoGFilter::~CDoGFilter()
{
  delete _pGaussianFilter;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CDoGFilter::DoGFilter(IplImage* srcImg, IplImage* edgeImg, float sigma1, float sigma2)
{
  if (!srcImg)
    return;

  int w = srcImg->width;
  int h = srcImg->height;
  int widthStep = srcImg->widthStep;

  short int *smoothed_image1;
  short int *smoothed_image2;
  _pGaussianFilter->GaussianSmooth((unsigned char*)(srcImg->imageData), h, w, widthStep, sigma1, &smoothed_image1);
  _pGaussianFilter->GaussianSmooth((unsigned char*)(srcImg->imageData), h, w, widthStep, sigma2, &smoothed_image2);

  unsigned char* data = new unsigned char[w * h];
  for (int i = 0; i < w*h; ++i)
  {
    // To change black and white
    data[i] = 255 - ((unsigned char) (smoothed_image1[i] - smoothed_image2[i]));
  }

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      edgeImg->imageData[i*widthStep + j] = data[i*w + j];
    }
  }

  free(data);
}


/*****************************************************************************//**
 * Simple single-page document header
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSPreparePage(ostream& o, const string& title, const Limits& limits)
{
  EPSHeader(o, title, limits);
  EPSProlog(o);
  EPSPageHeader(o, limits);
}


/*****************************************************************************//**
 * Simple single-page document finish
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSFinishPage(ostream& o)
{
  EPSTrailer(o);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSHeader(ostream& o, const string title, const Limits& limits)
{
  o << "%!PS-Adobe-3.0 EPSF-3.0\n"
  << "%%Title: " << title << '\n'
  << "%%BoundingBox: "
  << (int) floor(limits.xmin * scale) << ' '
  << (int) floor(limits.ymin * scale) << ' '
  << (int) ceil(limits.xmax * scale) << ' '
  << (int) ceil(limits.ymax * scale) << '\n'
  << "%%Pages: 1\n"
  << "%%EndComments\n"
  << '\n';
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSPageHeader(ostream& o, const Limits& limits)
{
  o << "%%Page: 1 1\n"
  << "%%BeginPageSetup\n"
  << "%%PageBoundingBox: "
  << (int) floor(limits.xmin * scale) << ' '
  << (int) floor(limits.ymin * scale) << ' '
  << (int) ceil(limits.xmax * scale) << ' '
  << (int) ceil(limits.ymax * scale) << '\n'
  << "%%EndPageSetup\n"
  << '\n';
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSProlog(ostream& o)
{
  o << "%%BeginProlog\n";

    /*
  o << "%Stipple procedure with given radius. e.g. x y r PDR\n"
  << "/PDR {\n"
  << "\tnewpath 1 setlinecap setlinewidth 2 copy moveto lineto stroke\n"
  << "} bind def\n"
  << '\n';
    */

  o << "%Stipple procedure with given radius. e.g. x y r PDR\n"
  << "/PDR {\n"
  << "\tnewpath 0 360 arc fill\n"
  << "} bind def\n"
  << '\n';

  o << "%Line procedure with current line width. e.g. x1 y1 x2 y2 PL\n"
  << "/PL {\n"
  << "\tnewpath moveto lineto stroke\n"
  << "} bind def\n"
  << '\n';

  o << "%Box procedure with current line width & join. e.g. x y w h PB\n"
  << "/PB {\n"
  << "\tnewpath \n"
  << "\t4 2 roll 2 copy moveto pop pop\n"
  << "\t1 index 0 rlineto\n"
  << "\t0 1 index rlineto\n"
  << "\t1 index neg 0 rlineto\n"
  << "\tclosepath stroke\n"
  << "} bind def\n"
  << '\n';

  o << "%Line cap procedure. e.g. 2 LC\n"
  << "/LC {\n"
  << "\tsetlinecap\n"
  << "} bind def\n"
  << '\n';

  o << "%Line join procedure. e.g. 2 LJ\n"
  << "/LJ {\n"
  << "\tsetlinejoin\n"
  << "} bind def\n"
  << '\n';

  o << "%%EndProlog\n"
  << '\n';
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSTrailer(ostream& o)
{
  o << "showpage\n"
  << "%%Trailer\n"
  << "%%EOF\n";
}


/*****************************************************************************//**
 * Set the line width
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSLineWidth(ostream& o, const float w)
{
    o << w * scale << " setlinewidth\n";
}


/*****************************************************************************//**
 * Set the line cap
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSLineCap(ostream& o, const LineCap cap)
{
  o << (unsigned long) cap << " LC\n";
}


/*****************************************************************************//**
 * Set the line join
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSLineJoin(ostream& o, const LineJoin join)
{
  o << (unsigned long) join << " LJ\n";
}


/*****************************************************************************//**
 * Emit a point and a width
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSPoint(ostream& o, const float x, const float y, const float r)
{
  o << x * scale << ' ' << y * scale << ' ' << r * scale << " PDR\n";
}


/*****************************************************************************//**
 * Emit a line which will use the current radius, cap, and join
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSLine(ostream& o, const float x1, const float y1, const float x2, const float y2)
{
  o << x1 * scale << ' ' << y1 * scale << ' ' << x2 * scale << ' ' << y2 * scale << " PL\n";
}


/*****************************************************************************//**
 * Emit an unfilled box which will use the current radius and join
 *
 *
 *
 *****************************************************************************/

void PostScript::EPSBox(ostream& o, const float x1, const float y1, const float x2, const float y2)
{
  o << x1 * scale << ' ' << y1 * scale << ' ' << x2 * scale << ' ' << y2 * scale << " PB\n";
}



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

/************************** MERSENNE.CPP ******************** AgF 2007-08-01 *
*  Random Number generator 'Mersenne Twister'                                *
*                                                                            *
*  This random number generator is described in the article by               *
*  M. Matsumoto & T. Nishimura, in:                                          *
*  ACM Transactions on Modeling and Computer Simulation,                     *
*  vol. 8, no. 1, 1998, pp. 3-30.                                            *
*  Details on the initialization scheme can be found at                      *
*  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html                  *
*                                                                            *
*  Experts consider this an excellent random number generator.               *
*                                                                            *
*  © 2001 - 2007 A. Fog. Published under the GNU General Public License      *
*  (www.gnu.org/copyleft/gpl.html) with the further restriction that it      *
*  cannot be used for gambling applications.                                 *
*****************************************************************************/


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRandomMersenne::Init0(uint32 seed) {
   // Detect computer architecture
   union {double f; uint32 i[2];} convert;
   convert.f = 1.0;
   if (convert.i[1] == 0x3FF00000) Architecture = LITTLE_ENDIAN1;
   else if (convert.i[0] == 0x3FF00000) Architecture = BIG_ENDIAN1;
   else Architecture = NONIEEE;

   // Seed generator
   mt[0]= seed;
   for (mti=1; mti < MERS_N; mti++) {
      mt[mti] = (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
   }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRandomMersenne::RandomInit(uint32 seed) {
   // Initialize and seed
   Init0(seed);

   // Randomize some more
   for (int i = 0; i < 37; i++) BRandom();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRandomMersenne::RandomInitByArray(uint32 seeds[], int length) {
   // Seed by more than 32 bits
   int i, j, k;

   // Initialize
   Init0(19650218);

   if (length <= 0) return;

   // Randomize mt[] using whole seeds[] array
   i = 1;  j = 0;
   k = (MERS_N > length ? MERS_N : length);
   for (; k; k--) {
      mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL)) + seeds[j] + j;
      i++; j++;
      if (i >= MERS_N) {mt[0] = mt[MERS_N-1]; i=1;}
      if (j >= length) j=0;}
   for (k = MERS_N-1; k; k--) {
      mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL)) - i;
      if (++i >= MERS_N) {mt[0] = mt[MERS_N-1]; i=1;}}
   mt[0] = 0x80000000UL;  // MSB is 1; assuring non-zero initial array

   // Randomize some more
   mti = 0;
   for (int i = 0; i <= MERS_N; i++) BRandom();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

uint32 CRandomMersenne::BRandom() {
   // Generate 32 random bits
   uint32 y;

   if (mti >= MERS_N) {
      // Generate MERS_N words at one time
      const uint32 LOWER_MASK = (1LU << MERS_R) - 1;       // Lower MERS_R bits
      const uint32 UPPER_MASK = 0xFFFFFFFF << MERS_R;      // Upper (32 - MERS_R) bits
      static const uint32 mag01[2] = {0, MERS_A};

      int kk;
      for (kk=0; kk < MERS_N-MERS_M; kk++) {
         y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
         mt[kk] = mt[kk+MERS_M] ^ (y >> 1) ^ mag01[y & 1];}

      for (; kk < MERS_N-1; kk++) {
         y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
         mt[kk] = mt[kk+(MERS_M-MERS_N)] ^ (y >> 1) ^ mag01[y & 1];}

      y = (mt[MERS_N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
      mt[MERS_N-1] = mt[MERS_M-1] ^ (y >> 1) ^ mag01[y & 1];
      mti = 0;
   }

   y = mt[mti++];

#if 1
   // Tempering (May be omitted):
   y ^=  y >> MERS_U;
   y ^= (y << MERS_S) & MERS_B;
   y ^= (y << MERS_T) & MERS_C;
   y ^=  y >> MERS_L;
#endif

   return y;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

double CRandomMersenne::Random() {
   // Output random float number in the interval 0 <= x < 1
   union {double f; uint32 i[2];} convert;
   uint32 r = BRandom();               // Get 32 random bits
   // The fastest way to convert random bits to floating point is as follows:
   // Set the binary exponent of a floating point number to 1+bias and set
   // the mantissa to random bits. This will give a random number in the
   // interval [1,2). Then subtract 1.0 to get a random number in the interval
   // [0,1). This procedure requires that we know how floating point numbers
   // are stored. The storing method is tested in function RandomInit and saved
   // in the variable Architecture.

   // This shortcut allows the compiler to optimize away the following switch
   // statement for the most common architectures:
#if defined(_M_IX86) || defined(_M_X64) || defined(__LITTLE_ENDIAN__)
   Architecture = LITTLE_ENDIAN1;
#elif defined(__BIG_ENDIAN__)
   Architecture = BIG_ENDIAN1;
#endif

   switch (Architecture) {
   case LITTLE_ENDIAN1:
      convert.i[0] =  r << 20;
      convert.i[1] = (r >> 12) | 0x3FF00000;
      return convert.f - 1.0;
   case BIG_ENDIAN1:
      convert.i[1] =  r << 20;
      convert.i[0] = (r >> 12) | 0x3FF00000;
      return convert.f - 1.0;
   case NONIEEE: default: ;
   }
   // This somewhat slower method works for all architectures, including
   // non-IEEE floating point representation:
   return (double)r * (1./((double)(uint32)(-1L)+1.));
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int CRandomMersenne::IRandom(int min, int max) {
   // Output random integer in the interval min <= x <= max
   // Relative error on frequencies < 2^-32
   if (max <= min) {
      if (max == min) return min; else return 0x80000000;
   }
   // Multiply interval with random and truncate
   int r = int((max - min + 1) * Random()) + min;
   if (r > max) r = max;
   return r;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int CRandomMersenne::IRandomX(int min, int max) {
   // Output random integer in the interval min <= x <= max
   // Each output value has exactly the same probability.
   // This is obtained by rejecting certain bit values so that the number
   // of possible bit values is divisible by the interval length
   if (max <= min) {
      if (max == min) return min; else return 0x80000000;
   }
#ifdef  INT64_DEFINED
   // 64 bit integers available. Use multiply and shift method
   uint32 interval;                    // Length of interval
   uint64 longran;                     // Random bits * interval
   uint32 iran;                        // Longran / 2^32
   uint32 remainder;                   // Longran % 2^32

   interval = uint32(max - min + 1);
   if (interval != LastInterval) {
      // Interval length has changed. Must calculate rejection limit
      // Reject when remainder = 2^32 / interval * interval
      // RLimit will be 0 if interval is a power of 2. No rejection then
      RLimit = uint32(((uint64)1 << 32) / interval) * interval - 1;
      LastInterval = interval;
   }
   do { // Rejection loop
      longran  = (uint64)BRandom() * interval;
      iran = (uint32)(longran >> 32);
      remainder = (uint32)longran;
   } while (remainder > RLimit);
   // Convert back to signed and return result
   return (int32)iran + min;

#else
   // 64 bit integers not available. Use modulo method
   uint32 interval;                    // Length of interval
   uint32 bran;                        // Random bits
   uint32 iran;                        // bran / interval
   uint32 remainder;                   // bran % interval

   interval = uint32(max - min + 1);
   if (interval != LastInterval) {
      // Interval length has changed. Must calculate rejection limit
      // Reject when iran = 2^32 / interval
      // We can't make 2^32 so we use 2^32-1 and correct afterwards
      RLimit = (uint32)0xFFFFFFFF / interval;
      if ((uint32)0xFFFFFFFF % interval == interval - 1) RLimit++;
   }
   do { // Rejection loop
      bran = BRandom();
      iran = bran / interval;
      remainder = bran % interval;
   } while (iran >= RLimit);
   // Convert back to signed and return result
   return (int32)remainder + min;

#endif
}


/*****************************************************************************//**
 * For 8 neighbor system
 *
 *
 *
 *****************************************************************************/

static _pixel g_neighbor[8] = {
  {-1, 0},	// left
  {1, 0},		// right
  {0, -1},	// top
  {0, 1},		// bottom
  {-1, -1},	// left-top
  {1, -1},	// right-top
  {-1, 1},	// left-bottom
  {1, 1}		// right-bottom
};



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRender::render(IplImage* srcImg, IplImage* dstImg, CDistributionMap* distMap, int mark_type, const char* black_stipple_dir, const char* white_stipple_dir, int stipple_flag, bool edge_flag, IplImage* edgeImg_in, int post_processing_flag,
           const char* dir_dest, int given_edge_image, int white_stipple_start_index, int use_tone_selection_basedon_prob, int use_full_linear, int use_tanh, int stipple_color, int background_color, float dog_sigma1, float dog_sigma2, int short_edge_size)
{
  // Input image info.
  int width = srcImg->width;
  int height = srcImg->height;
  int widthStep = srcImg->widthStep;

  // Choose tone value from the tone textures
  int nTotalLevels = distMap->getCount();
  unsigned char img_gValue = 0;
  unsigned char tone_gValue = 0;
  int left_level, right_level;
  double left_gap, right_gap, total_gap;
  int left_gap_percent, right_gap_percent;
  IplImage* distMapImg = 0;
  int index = 0;
  int randomNum = 0;
  int tone_level;

  double Tl, Tr;

  // To save the position white stipples
  /*IplImage* whiteStippleMap = cvCreateImage(cvSize(width, height), 8, 1);
  cvSet(whiteStippleMap, cvScalar(0));*/

  // To save tone level for pixel
  IplImage* levelMap = cvCreateImage(cvSize(width, height), 8, 1);
  cvSet(levelMap, cvScalar(-1));

  // To save position for center pixels of stipples
  // If it is stipple ceter pixel, set 1 or 2 (default is 0)
  IplImage* stipplePosMap = cvCreateImage(cvSize(width, height), 8, 1);
  cvSet(stipplePosMap, cvScalar(0));

  CRandomMersenne RNG;
  RNG.RandomInit((unsigned)time(0));

#ifdef LINUX
    int Count_progress=0;
    int Max_progress=2*height*width;
    QProgressDialog Progress("Computing Stippling by example...", "Abort",0,Max_progress);
    Progress.setWindowModality(Qt::WindowModal);
    Progress.setMinimumDuration(0);
    Progress.setCancelButton(0);

#endif


  // First rendering for distribution
  for (int y = 1; y < height-1; ++y)
  {
#ifdef LINUX
    Count_progress+=width;
    Progress.setValue(Count_progress);
    if (Progress.wasCanceled()) break;
    // we cannot acces directly to qApp
//    (QCoreApplication::instance())->processEvents();
#endif
    for (int x = 1; x < width-1; ++x)
    {
      index = y * widthStep + x;

      // Real gray value
      img_gValue = (unsigned char) (srcImg->imageData[index]);

      // Decide the discrete tone level
      left_level = 0;
      right_level = nTotalLevels - 1;
      left_gap_percent = right_gap_percent = 0;

      // e.g., tone 0 = pure white ~ tone 9 = pure black
      if (img_gValue == 255)
        tone_level = left_level;		// level 0
      else if (img_gValue == 0)
        tone_level = right_level;		// level N-1
      else if (img_gValue < 255 && img_gValue >= (distMap->getAvgGrayValue(left_level+1) - distMap->getAvgGrayValue(left_level+2))/2 + distMap->getAvgGrayValue(left_level+2))
        tone_level = left_level + 1;	// level 1
      else if (img_gValue > 0 && img_gValue <= (distMap->getAvgGrayValue(right_level-2) - distMap->getAvgGrayValue(right_level-1))/2 + distMap->getAvgGrayValue(right_level-1))
        tone_level = right_level - 1;	// level N-2
      else
      {
        for (int i = 0; i < nTotalLevels; ++i)
        {
          if (img_gValue == cvRound(distMap->getAvgGrayValue(i)))
          {
            left_level = right_level = i;
            break;
          }
          if (img_gValue >= distMap->getAvgGrayValue(i))
          {
            right_level = i;
            left_level = ((i-1) < 0) ? 0 : (i-1);
            break;
          }
          else
          {
            left_level = i;
            right_level = ((i+1) > (nTotalLevels-1)) ? (nTotalLevels-1) : (i+1);
          }
        }

        Tl = distMap->getAvgGrayValue(left_level);
        Tr = distMap->getAvgGrayValue(right_level);

        left_gap = Tl - (double)img_gValue;
        right_gap = (double)img_gValue - Tr;
        total_gap = left_gap + right_gap;

        {
//#ifdef USE_TONE_SELECTION_BASEDON_PROBABILITY
        if (use_tone_selection_basedon_prob)
        {
          if (total_gap != 0)
          {
            if (use_tanh)
            {
//#ifndef USE_FULL_LINEAR
//							// tanh() within 1/3 range
//							double range = 3;
//							double tmp = total_gap / range;
//							double x = 2 * ((img_gValue - (Tr + tmp)) / tmp) - 1;
//#else
              // tanh() within full total gap range (Tl > Tr)
              double x = 2 * ((img_gValue - Tr) / total_gap) - 1;
//#endif
              double pl = (tanh(x) + 1) / 2;	// tanh(x): [-1, 1], pl: [0, 1]
              double pr = 1 - pl;
              left_gap_percent = (int) (pl * 100) + 0.5;
              right_gap_percent = 100 - left_gap_percent;//(int) (pr * 100) + 0.5;
              randomNum = RNG.IRandomX(0, 99);
              // The closer it is the higher probability
              if (randomNum < right_gap_percent)
              {
                tone_level = right_level;
              }
              else if ((randomNum >= right_gap_percent) && (randomNum <= 99))
              {
                tone_level = left_level;
              }
            }
            else
            {
              left_gap_percent = (int) (((left_gap / total_gap) * 100) + 0.5);
              right_gap_percent = 100 - left_gap_percent;//(int) (((right_gap / total_gap) * 100) + 0.5);
              randomNum = RNG.IRandomX(0, 99);
              // The closer it is the higher probability
              if (randomNum < right_gap_percent)
              {
                tone_level = left_level;
              }
              else if ((randomNum >= right_gap_percent) && (randomNum <= 99))
              {
                tone_level = right_level;
              }
            }
          }
          else
          {
            // In this case, left_level equals to right_level
            tone_level = left_level;
          }
        }
        else
        {
//#else
          // In this case, we don't use probability, choose just closer level
          if (cvRound(left_gap) <= cvRound(right_gap))
            tone_level = left_level;
          else
            tone_level = right_level;
//#endif
        }
        }
      }

      // Get tone value from the tone texture
      distMapImg = distMap->getMap(tone_level);
      tone_gValue = (unsigned char) (distMapImg->imageData[index]);

      // Set result pixel value
      dstImg->imageData[index] = (unsigned char) tone_gValue;

      // Save the position for white stipples
      // e.g., For Bill Andrew's tone map, 1~5: black stipple, 6~8: white stipple
      /*if ((tone_level >= WHITE_STIPPLE_START_INDEX) && (tone_gValue == 255))
        whiteStippleMap->imageData[index] = (unsigned char)255;*/

      // Save tone level for each pixel
      levelMap->imageData[index] = (unsigned char) tone_level;

      // Set stipplePosMap with 1 if the current pixel is for a black stipple, or 2 if it is a white stipple
      if (tone_level < white_stipple_start_index && tone_gValue == 0)			// black stipple and white background for a ligth level
      {
        stipplePosMap->imageData[index] = 1;
      }
      else if (tone_level >= white_stipple_start_index && tone_gValue == 255)	// white stipple and black background for a dark level
      {
        stipplePosMap->imageData[index] = 2;
      }
    }
  }

  //// Display edge
  IplImage* edgeImg = 0;

  // Save final result
  char filename[1024];
  sprintf(filename, "%sresult_dist.png", dir_dest);
  cvSaveImage(filename, dstImg);

  // Display stipple marks
  if (stipple_flag)
  {
    #ifdef INFO
    printf("Placing stipple marks...\n");
#endif

    if (mark_type == 1)
    {
      CStipplePlacer stipplePlacer;
      int count = 0;

      // Make a list of center points of stipple marks
      vector<_pixel> blackStipplePointList;
      vector<_pixel> whiteStipplePointList;
      for (int i = 0; i < dstImg->height; ++i){
#ifdef LINUX
        Count_progress+=width;
        Progress.setValue(Count_progress);
        if (Progress.wasCanceled()) break;
        // we cannot acces directly to qApp
//        (QCoreApplication::instance())->processEvents();
#endif
        for (int j = 0; j < dstImg->width; ++j){
          // For all black stipple pixels
          if ((unsigned char) (dstImg->imageData[i*dstImg->widthStep + j]) == 0 &&					// this can be a black stipple or black background
            (unsigned char) (levelMap->imageData[i*widthStep + j]) >= 0 &&							// now, should be black stipples
            (unsigned char) (levelMap->imageData[i*widthStep + j]) < white_stipple_start_index)
          //if ((unsigned char) (dstImg->imageData[i*dstImg->widthStep + j]) == 0)						// New for NPAR09: For 1~8 level, all black pixels -> place black stipples, and for 7~8 level, place white stipples
          {
            _pixel point;
            point.x = j;
            point.y = i;
            blackStipplePointList.push_back(point);
          }
          // For all white stipple pixels
          else if ((unsigned char) (dstImg->imageData[i*dstImg->widthStep + j]) == 255 &&				// This can be a white stipple or white background
             (unsigned char) (levelMap->imageData[i*widthStep + j]) >= white_stipple_start_index)	// Now, should be white stipples
          //else if ((unsigned char) (dstImg->imageData[i*dstImg->widthStep + j]) == 255)
          {
            _pixel point;
            point.x = j;
            point.y = i;
            whiteStipplePointList.push_back(point);
          }
          else	// For all background pixels
          {
            if ((unsigned char) (dstImg->imageData[i*dstImg->widthStep + j]) == 0)
              int test = 0;
          }
        }
      }

#ifdef INFO
      printf("Black stipple points = %d\n", (int)(blackStipplePointList.size()));
      printf("White stipple points = %d\n", (int)(whiteStipplePointList.size()));
#endif

      // For black stipple marks

      // Load black stipple marks
      CStippleDB blackStippleDB;
      count = blackStippleDB.load(black_stipple_dir, background_color);
#ifdef INFO
      printf("Black stipple marks = %d\n", count);
#endif
      // Stipple images used
      vector<IplImage*> blackStippleList;
      if (count > 0)
      {
        stipplePlacer.placeStipples(dstImg, &blackStipplePointList, &blackStippleDB, &blackStippleList, (unsigned char)0, edgeImg);
      }
      else
      {
        printf("\nThere doesn't exist black stipple marks...\n");
      }

      // For white stipple marks

      // Load white stipple marks
      CStippleDB whiteStippleDB;
      count = whiteStippleDB.load(white_stipple_dir, background_color);
#ifdef INFO
      printf("White stipple marks = %d\n", count);
#endif
      // Stipple images used
      vector<IplImage*> whiteStippleList;
      if (count > 0)
      {
        stipplePlacer.placeStipples(dstImg, &whiteStipplePointList, &whiteStippleDB, &whiteStippleList, (unsigned char)255, edgeImg);
      }
      else
      {
        printf("\nThere doesn't exist white stipple marks...\n");
      }

      // Display edge
      if (edge_flag)
      {
        // Found a bug
#ifdef INFO
        printf("Computing edges...\n");
#endif
        edgeImg = computeEdge(srcImg, dir_dest, dog_sigma1, dog_sigma2, short_edge_size, background_color, stipple_color);

#ifdef INFO
        printf("Combining edges...\n");
#endif
        combineEdges(dstImg, edgeImg);
      }
      //else if (GIVEN_EDGE_IMAGE && edgeImg_in)
      else if (given_edge_image && edgeImg_in)
      {
#ifdef INFO
        printf("Combining given edges...\n");
#endif
        combineEdges(dstImg, edgeImg_in);
      }

      // Based on Tobias's comment
      // Reduce white and black pixels distributed in a middel level tone area
      // Input: dstImg (image including stipple marks), stipplePosMap, levelMap
      // Output: modified dstImg
      if (post_processing_flag)
      {
        // Save final result
        sprintf(filename, "%sbefore.png", dir_dest);
        cvSaveImage(filename, dstImg);
#ifdef INFO
        printf("Reducing unnecessary pixels in a middle level tone area...\n");
#endif
        smoothTransition(dstImg, stipplePosMap, levelMap);
      }
    }
//    else	// Use circles : Not complete
//    {
//#ifdef INFO
//      printf("Circle marks...\n");
//#endif
//      int count = 0;

//      // Make a list of center points of stipple marks
//      vector<_pixel> stipplePointList;
//      vector<_pixel> whiteStipplePointList;

//      for (int i = 0; i < dstImg->height; ++i)
//      {
//        for (int j = 0; j < dstImg->width; ++j)
//        {
//          // For all black stipple pixels
//          if ((unsigned char) (dstImg->imageData[i*dstImg->widthStep + j]) == 0 &&
//            (unsigned char) (levelMap->imageData[i*widthStep + j]) >= 0 &&
//            (unsigned char) (levelMap->imageData[i*widthStep + j]) < white_stipple_start_index)
//          {
//            _pixel point;
//            point.x = j;
//            point.y = i;
//            stipplePointList.push_back(point);
//          }
//          // For all white stipple pixels
//          else if ((unsigned char) (dstImg->imageData[i*dstImg->widthStep + j]) == 255 &&				// This can be a white stipple or white background
//             (unsigned char) (levelMap->imageData[i*widthStep + j]) >= white_stipple_start_index)	// Now, should be white stipples
//          {
//            _pixel point;
//            point.x = j;
//            point.y = i;
//            whiteStipplePointList.push_back(point);
//          }
//          else	// For all background pixels
//          {
//            if ((unsigned char) (dstImg->imageData[i*dstImg->widthStep + j]) == 0)
//              int test = 0;
//          }
//        }
//      }
//#ifdef INFO
//      printf("Stipple points = %d\n", (int)(stipplePointList.size()));
//#endif
//      sprintf(filename, "%srender_withCircles.eps", dir_dest);
//      outputEPSPoints(filename, "S.Y.", srcImg, 0/*gradientImg*/, edgeImg_in, &stipplePointList);
//    }

  }
#ifdef LINUX
  Progress.setValue(Max_progress);
#endif

  // Release
  cvReleaseImage(&levelMap);

  cvReleaseImage(&stipplePosMap);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

IplImage* CRender::computeEdge(IplImage* srcImg, const char* dir_dest, float dog_sigma1, float dog_sigma2, int short_edge_size, int background_color, int stipple_color)
{
  IplImage* edgeImg = cvCreateImage(cvSize(srcImg->width, srcImg->height), 8, 1);
  cvSet(edgeImg, cvScalar((unsigned char)background_color));

  CDoGFilter dog;
  dog.DoGFilter(srcImg, edgeImg, dog_sigma1, dog_sigma2);
  removeShortEdges(edgeImg, short_edge_size, background_color, stipple_color);

  char filename[1024];
  sprintf(filename, "%sedge.png", dir_dest);
  cvSaveImage(filename, edgeImg);

  return edgeImg;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRender::combineEdges(IplImage* img, IplImage* edge)
{
  if (img != 0 && edge != 0)
  {
    int width = img->width;
    int height = img->height;
    int widthStep = img->widthStep;

    unsigned char pixel = 0;
    int index = 0;
    if (width == edge->width && height == edge->height)
    {
      for (int y = 0; y < height; ++y)
      {
        for (int x = 0; x < width; ++x)
        {
          index = y * widthStep + x;
          pixel = (unsigned char) (edge->imageData[index]);

          // Let's think 0~15 gray level as edges
          if (pixel <= 15)
            img->imageData[index] = pixel;
        }
      }
    }
    else
      printf("Error: different edge image size!\n");
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRender::removeShortEdges(IplImage* img, int minCount, int background_color, int stipple_color)
{
  if (img)
  {
    int h = img->height;
    int w = img->width;
    int widthStep = img->widthStep;

    int count = h * w;

    int length = 0;
    //int limit = count - w - 1;

    int index = 0;

    int gap = 100;

    for (int i = gap; i < h-gap; ++i)
    {
      //printf("%d\n", i);

      for (int j = gap; j < w-gap; ++j)
      {
        index = i * widthStep + j;

        if ((unsigned char)(img->imageData[index]) ==  stipple_color)
        {
          //_edgeMap->setPixel(i, VISIT_MARK);
          img->imageData[index] = (unsigned char) VISIT_MARK;

          traverse(img, j, i, ++length);//index);

          //printf("Length = %d\n", length);

          if (length < minCount)
          {
            //_edgeMap->setPixel(i, 255);
            img->imageData[index] = (unsigned char) background_color;

            deleteEdge(img, j, i);//index);

          }
          length = 0;
        }
      }

    }

    for (int i = 0; i < h; ++i)
    {
      for (int j = 0; j < w; ++j)
      {
        if ((unsigned char)(img->imageData[i*widthStep + j]) == VISIT_MARK)
          img->imageData[i*widthStep + j] = (unsigned char) stipple_color;
      }
    }
  }
}

//int CRender::traverse(IplImage* img, int i, int count)


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool CRender::traverse(IplImage* img, int x, int y, int& length)//int i)//, int count)
{
  int widthStep = img->widthStep;

  for (int k = 0; k < 8; ++k)
  {
    int x1 = x + g_neighbor[k].x;
    int y1 = y + g_neighbor[k].y;

    if ((x1 < 0) || (x1 >= img->width)) continue;
    if ((y1 < 0) || (y1 >= img->height)) continue;

    int index = y1 * widthStep + x1;
    //if (index >= 0 && index < limit)
    {
      //if ((index >= 0 && index < count) && _edgeMap->getPixel(index) == 0)
      if (((unsigned char) (img->imageData[index])) == 0)
      {
        //_edgeMap->setPixel(index, VISIT_MARK);
        img->imageData[index]= (unsigned char) VISIT_MARK;
        length++;
        //if (!traverse(img, x1, y1, length)) return false;
        traverse(img, x1, y1, length);

        //if (length > SHORT_EDGE_SIZE)
        //	return false;
      }
    }
  }
  return true;

  //int j[8] = {1, -width, -1, width,
  //			-1 - width, 1 - width,
  //			-1 + width, 1 + width};
  //
  //int length = 1;
  //unsigned char pixel = 0;
  //for (int x = 0; x < 8; x++)
  //{
  //	int index = i + j[x];
  //	if (index >= 0 && index < count)
  //	{
  //		pixel = (unsigned char) (img->imageData[index]);

  //		//if ((index >= 0 && index < count) && _edgeMap->getPixel(index) == 0)
  //		if (pixel == 0)
  //		{
  //			//_edgeMap->setPixel(index, VISIT_MARK);
  //			img->imageData[index]= (unsigned char) VISIT_MARK;
  //			length = length + traverse(img, index, count);
  //		}
  //	}
  //}
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRender::deleteEdge(IplImage* img, int x, int y)//int i)//, int count)
{
  int widthStep = img->widthStep;

  /*int j[8] = {1, -widthStep, -1, widthStep,
        -1 - widthStep, 1 - widthStep,
        -1 + widthStep, 1 + widthStep};*/
  for (int k = 0; k < 8; ++k)
  {
    int x1 = x + g_neighbor[k].x;
    int y1 = y + g_neighbor[k].y;

    if ((x1 < 0) || (x1 >= img->width)) continue;
    if ((y1 < 0) || (y1 >= img->height)) continue;

    int index = y1 * widthStep + x1;

    //if (index >= 0 && index < count)
    //if (index >= 0 && index < limit)
    {
      //if ((index >= 0 && index < count) && _edgeMap->getPixel(index) == VISIT_MARK)
      if (((unsigned char) (img->imageData[index])) == VISIT_MARK)
      {
        //_edgeMap->setPixel(index, 255);
        img->imageData[index] = (unsigned char) BACKGROUND_COLOR;
        deleteEdge(img, x1, y1);//index);//, count);
      }
    }
  }

  //int j[8] = {1, -width, -1, width,
  //			-1 - width, 1 - width,
  //			-1 + width, 1 + width};
  //
  //unsigned char pixel = 0;
  //for (int x = 0; x < 8; x++)
  //{
  //	int index = i + j[x];
  //	if (index >= 0 && index < count)
  //	{
  //		pixel = (unsigned char) (img->imageData[index]);

  //		//if ((index >= 0 && index < count) && _edgeMap->getPixel(index) == VISIT_MARK)
  //		if (pixel == VISIT_MARK)
  //		{
  //			//_edgeMap->setPixel(index, 255);
  //			img->imageData[index] = (unsigned char) BACKGROUND_COLOR;
  //			deleteEdge(img, index, count);
  //		}
  //	}
  //}
}


/*****************************************************************************//**
 * Input: img (stipple image), stipplePosMap, levelMap
 * Output: modified img
 *
 *
 *
 *****************************************************************************/


void CRender::smoothTransition(IplImage* img, IplImage* stipplePosMap, IplImage* levelMap)
{
  int width = img->width;
  int height = img->height;
  int widthStep = img->widthStep;

  for (int i = 4; i < height-4; ++i)
  {
    for (int j = 4; j < width-4; ++j)
    {
      if ((unsigned char)(img->imageData[i*widthStep + j]) == 255)
      {
        checkUnnecessaryWhitePixels(j, i, img, stipplePosMap, levelMap);
      }
      else if ((unsigned char)(img->imageData[i*widthStep + j]) == 0)
      {
        checkUnnecessaryBlackPixels(j, i, img, stipplePosMap, levelMap);
      }


      //if ((unsigned char)(levelMap->imageData[i*widthStep + j]) < WHITE_STIPPLE_START_INDEX &&
      //	(unsigned char)(img->imageData[i*widthStep + j]) == 255)
      //{
      //	checkUnnecessaryWhitePixels(j, i, img, levelMap);//,255, 0);
      //}
      //
      //if ((unsigned char)(levelMap->imageData[i*widthStep + j]) >= WHITE_STIPPLE_START_INDEX &&
      //	(unsigned char)(img->imageData[i*widthStep + j]) == 0)
      //{
      //	checkUnnecessaryBlackPixels(j, i, img, levelMap);//, 0, 255);
      //}
    }
  }
}


/*****************************************************************************//**
 * Input: img (distribution image), levelMap
 * Output: modified img
 *
 *
 *
 *****************************************************************************/

void CRender::smoothTransition(IplImage* img, IplImage* levelMap)
{
  int width = levelMap->width;
  int height = levelMap->height;
  int widthStep = levelMap->widthStep;

  // This case makes a problem in a middle tone level area
  int h_level = WHITE_STIPPLE_START_INDEX;	// e.g., Tone level = 6 (white stipple, black background)
  int l_level = h_level - 1;					// e.g., Tone level = 5 (black stipple, white background)

  for (int i = 0; i < height; ++i)
  {
    for (int j = 0; j < width; ++j)
    {
      if ((unsigned char)(levelMap->imageData[i*widthStep + j]) <= l_level &&
        (unsigned char)(img->imageData[i*widthStep + j]) == 255)
      {
        checkUnnecessaryWhitePixels(j, i, img, levelMap);//,255, 0);
      }

      if ((unsigned char)(levelMap->imageData[i*widthStep + j]) >= h_level &&
        (unsigned char)(img->imageData[i*widthStep + j]) == 0)
      {
        checkUnnecessaryBlackPixels(j, i, img, levelMap);//, 0, 255);
      }
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRender::checkUnnecessaryBlackPixels(int x, int y, IplImage* img, IplImage* levelMap)
{
  int width = img->width;
  int height = img->height;
  int widthStep = img->widthStep;

  vector<_pixel> points;

  _pixel p;
  p.x = x;
  p.y = y;
  points.push_back(p);

  int nx, ny;

  for (int i = 0; i < 8; ++i)
  {
    nx = x + g_neighbor[i].x;
    ny = y + g_neighbor[i].y;

    if (nx < 0 || nx >= width) continue;
    if (ny < 0 || ny >= height) continue;

    if ((unsigned char)(img->imageData[ny*widthStep + nx]) == 0 &&
      (unsigned char)(levelMap->imageData[ny*widthStep + nx]) >= WHITE_STIPPLE_START_INDEX)
    {
      _pixel pp;
      pp.x = nx;
      pp.y = ny;
      points.push_back(pp);
    }
  }

  int count = (int) points.size();
  if (count < 4)
  {
    for (int i = 0; i < count; ++i)
    {
      img->imageData[points.at(i).y * widthStep + points.at(i).x] = (unsigned char) 255;
    }
  }

  points.clear();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRender::checkUnnecessaryWhitePixels(int x, int y, IplImage* img, IplImage* levelMap)
{
  int width = img->width;
  int height = img->height;
  int widthStep = img->widthStep;

  vector<_pixel> points;

  _pixel p;
  p.x = x;
  p.y = y;
  points.push_back(p);

  int nx, ny;

  for (int i = 0; i < 8; ++i)
  {
    nx = x + g_neighbor[i].x;
    ny = y + g_neighbor[i].y;

    if (nx < 0 || nx >= width) continue;
    if (ny < 0 || ny >= height) continue;

    if ((unsigned char)(img->imageData[ny*widthStep + nx]) == 255 &&
      (unsigned char)(levelMap->imageData[ny*widthStep + nx]) < WHITE_STIPPLE_START_INDEX)
    {
      _pixel pp;
      pp.x = nx;
      pp.y = ny;
      points.push_back(pp);
    }
  }

  int count = (int) points.size();
  if (count < 4)
  {
    for (int i = 0; i < count; ++i)
    {
      img->imageData[points.at(i).y * widthStep + points.at(i).x] = 0;
    }
  }

  points.clear();
}



static _pixel g_rings[40] = {
  {0,-2},{2,0},{0,2},{-2,0},{1,-2},{2,-1},{2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2},{2,-2},{2,2},{-2,2},{-2,-2},	// second-ring 16
  {0,-3},{3,0},{0,3},{-3,0},{1,-3},{3,-1},{3,1},{1,3},{-1,3},{-3,1},{-3,-1},{-1,-3},{2,-3},{3,-2},{3,2},{2,3},{-2,3},{-3,2},{-3,-2},{-2,-3},{3,-3},{3,3},{-3,3},{-3,-3} // third-ring 24
};
#define COUNT_RING_ELEM 40


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRender::checkUnnecessaryBlackPixels(int x, int y, IplImage* img, IplImage* stipplePosMap, IplImage* levelMap)
{
  int width = img->width;
  int height = img->height;
  int widthStep = img->widthStep;

  vector<_pixel> points;

  _pixel p;
  p.x = x;
  p.y = y;
  points.push_back(p);

  int nx, ny;

  for (int i = 0; i < 8; ++i)
  {
    nx = x + g_neighbor[i].x;
    ny = y + g_neighbor[i].y;

    if (nx < 0 || nx >= width) continue;
    if (ny < 0 || ny >= height) continue;

    if ((unsigned char)(img->imageData[ny*widthStep + nx]) == 0)
    {
      _pixel pp;
      pp.x = nx;
      pp.y = ny;
      points.push_back(pp);
    }
  }

  int count = (int) points.size();

  if (count < 4)
  {
    for (int n = 0; n < count; ++n)
    {
      nx = points.at(n).x;
      ny = points.at(n).y;
      img->imageData[ny*widthStep + nx] = (unsigned char) 255;
    }
  }

  if (count >= 4 && count < 8)
  {
    int cx, cy;
    int kx, ky;

    vector<_pixel> diffs;

    for (int n = 0; n < count; ++n)
    {
      for (int i = 0; i < COUNT_RING_ELEM; ++i)
      {
        kx = (points.at(n).x + g_rings[i].x) + (points.at(0).x - points.at(n).x);
        ky = (points.at(n).y + g_rings[i].y) + (points.at(0).y - points.at(n).y);

        if (kx < 0 || kx >= width) continue;
        if (ky < 0 || ky >= height) continue;

        if ((unsigned char)(img->imageData[ky*widthStep + kx]) == 0 &&
           !isFound(kx, ky, &points))
        {
          _pixel diff;
          diff.x = g_rings[i].x + (points.at(0).x - points.at(n).x);
          diff.y = g_rings[i].y + (points.at(0).y - points.at(n).y);
          diffs.push_back(diff);

          break;
        }
      }
    }

    int diffX, diffY;
    diffX = diffY = 0;
    int index = findMin(&diffs);
    if (index >= 0)
    {
      diffX = diffs.at(index).x;
      diffY = diffs.at(index).y;
    }
    if (diffX > 1 && diffY > 1)
    {
      for (int i = 0; i < count; ++i)
      {
        cx = points.at(i).x;
        cy = points.at(i).y;
        img->imageData[cy*widthStep + cx] = (unsigned char) 255;

        nx = cx + diffX;
        ny = cy + diffY;
        if (nx < 0 || nx >= width) continue;
        if (ny < 0 || ny >= height) continue;
        img->imageData[ny*widthStep + nx] = (unsigned char) 0;
      }
    }
  }

  points.clear();

  //int width = img->width;
  //int height = img->height;
  //int widthStep = img->widthStep;
  //
  //vector<_pixel> points;

  //_pixel p;
  //p.x = x;
  //p.y = y;
  //points.push_back(p);

  //int nx, ny;

  //for (int i = 0; i < 8; ++i)
  //{
  //	nx = x + g_neighbor[i].x;
  //	ny = y + g_neighbor[i].y;

  //	if (nx < 0 || nx >= width) continue;
  //	if (ny < 0 || ny >= height) continue;

  //	if ((unsigned char)(img->imageData[ny*widthStep + nx]) == 0)
  //	{
  //		_pixel pp;
  //		pp.x = nx;
  //		pp.y = ny;
  //		points.push_back(pp);
  //	}
  //}

  //int count = (int) points.size();
  //if (count < 4)
  //{
  //	for (int i = 0; i < count; ++i)
  //	{
  //		img->imageData[points.at(i).y * widthStep + points.at(i).x] = (unsigned char) 255;
  //	}
  //}

  //points.clear();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRender::checkUnnecessaryWhitePixels(int x, int y, IplImage* img, IplImage* stipplePosMap, IplImage* levelMap)
{
  int width = img->width;
  int height = img->height;
  int widthStep = img->widthStep;

  vector<_pixel> points;

  _pixel p;
  p.x = x;
  p.y = y;
  points.push_back(p);

  int nx, ny;

  for (int i = 0; i < 8; ++i)
  {
    nx = x + g_neighbor[i].x;
    ny = y + g_neighbor[i].y;

    if (nx < 0 || nx >= width) continue;
    if (ny < 0 || ny >= height) continue;

    if ((unsigned char)(img->imageData[ny*widthStep + nx]) == 255)
    {
      _pixel pp;
      pp.x = nx;
      pp.y = ny;
      points.push_back(pp);
    }
  }

  int count = (int) points.size();
  if (count < 4)
  {
    for (int n = 0; n < count; ++n)
    {
      nx = points.at(n).x;
      ny = points.at(n).y;
      img->imageData[ny*widthStep + nx] = (unsigned char) 0;
    }
  }

  if (count >= 4 && count < 8)
  {
    int cx, cy;
    int kx, ky;

    vector<_pixel> diffs;

    for (int n = 0; n < count; ++n)
    {
      for (int i = 0; i < COUNT_RING_ELEM; ++i)
      {
        kx = (points.at(n).x + g_rings[i].x) + (points.at(0).x - points.at(n).x);
        ky = (points.at(n).y + g_rings[i].y) + (points.at(0).y - points.at(n).y);

        if (kx < 0 || kx >= width) continue;
        if (ky < 0 || ky >= height) continue;

        if ((unsigned char)(img->imageData[ky*widthStep + kx]) == 255 &&
           !isFound(kx, ky, &points))
        {
          _pixel diff;
          diff.x = g_rings[i].x + (points.at(0).x - points.at(n).x);
          diff.y = g_rings[i].y + (points.at(0).y - points.at(n).y);
          diffs.push_back(diff);

          break;
        }
      }
    }

    int diffX, diffY;
    diffX = diffY = 0;
    int index = findMin(&diffs);
    if (index >= 0)
    {
      diffX = diffs.at(index).x;
      diffY = diffs.at(index).y;
    }
    if (diffX > 1 && diffY > 1)
    {
      for (int i = 0; i < count; ++i)
      {
        cx = points.at(i).x;
        cy = points.at(i).y;
        img->imageData[cy*widthStep + cx] = (unsigned char) 0;

        nx = cx + diffX;
        ny = cy + diffY;
        if (nx < 0 || nx >= width) continue;
        if (ny < 0 || ny >= height) continue;
        img->imageData[ny*widthStep + nx] = (unsigned char) 255;
      }
    }


    //for (int i = 0; i < COUNT_RING_ELEM; ++i)
    //{
    //	kx = x + g_rings[i].x;
    //	ky = y + g_rings[i].y;

    //	if (kx < 0 || kx >= width) continue;
    //	if (ky < 0 || ky >= height) continue;

    //	if ((unsigned char)(img->imageData[ky*widthStep + kx]) == 255)
    //	{
    //		diffX = kx - x;
    //		diffY = ky - y;

    //		for (int j = 0; j < count; ++j)
    //		{
    //			cx = points.at(j).x;
    //			cy = points.at(j).y;
    //			img->imageData[cy*widthStep + cx] = (unsigned char) 0;

    //			// Shift pixels
    //			nx = cx + diffX;
    //			ny = cy + diffY;
    //			if (nx < 0 || nx >= width) continue;
    //			if (ny < 0 || ny >= height) continue;

    //			img->imageData[ny*widthStep + nx] = (unsigned char) 255;
    //		}

    //		// Since we found the cloest with the same color, break this loop
    //		break;
    //	}
    //}

    /*
    for (int i = 0; i < count; ++i)
    {
      img->imageData[points.at(i).y * widthStep + points.at(i).x] = 0;
    }*/
  }

  points.clear();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool CRender::isFound(int x, int y, vector<_pixel>* pointList)
{
  int count = (int) pointList->size();

  for (int i = 0; i < count; ++i)
  {
    if (pointList->at(i).x == x && pointList->at(i).y == y)
      return true;
  }

  return false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int CRender::findMin(vector<_pixel>* list)
{
  int index = -1;

  int count = (int) list->size();

  if (count > 0)
  {
    int mx = list->at(0).x;
    int my = list->at(0).y;
    int min_i = 0;

    int x, y;

    for (int j = 1; j < count; ++j)
    {
      x = list->at(j).x;
      y = list->at(j).y;

      if ((mx*mx + my*my) > (x*x + y*y))
      {
        mx = x;
        my = y;
        min_i = j;
      }
    }
  }

  return index;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CRender::outputEPSPoints(const char* filename, const char* title, IplImage* toneImg, IplImage* gradientImg, IplImage* edgeImg, vector<_pixel>* pointList)
{
  int w = toneImg->width;
  int h = toneImg->height;

  Limits unitLimits;
  /*if (w >= h)
  {
        unitLimits.xmin = 0;
    unitLimits.xmax = 1;
        unitLimits.ymin = 0;
        unitLimits.ymax = h / (double)w;
    }
  else
  {
        unitLimits.xmin = 0;
        unitLimits.xmax = w / (double)h;
        unitLimits.ymin = 0;
        unitLimits.ymax = 1;
    }*/
  unitLimits.xmin = 0;
  unitLimits.xmax = w;
  unitLimits.ymin = 0;
  unitLimits.ymax = h;

  ofstream o(filename);

    const Limits& limits = unitLimits;
    PostScript::EPSPreparePage(o, string(title), limits);

  outputEPSPoints(o, toneImg, gradientImg, edgeImg, pointList);

    PostScript::EPSFinishPage(o);
    o.close();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool CRender::outputEPSPoints(std::ostream& o, IplImage* toneImg, IplImage* gradientImg, IplImage* edgeImg, vector<_pixel>* pointList)
{
  int nPoints = (int)(pointList->size());

  if (toneImg == 0 || pointList == 0 || nPoints <= 0)
    return false;

  int h = toneImg->height;
  int w = toneImg->width;
  int ws = toneImg->widthStep;

  int x, y;
  float radius = 0.f;

  // Refer: David Mould, "Stipple Placement using Distance in a Weighted Graph", CA2007
  // pixel importance = alpha * d + beta * g
  // normalizing factors: alpha = 1/sigma(d), beta = 1/sigma(g) over the entire image
  // d = (1 - intensity), g = gradient magnitude
  double importance = 1;	// value within [0, 1]

  PostScript::EPSLineCap(o, PostScript::ROUND_CAP);
    PostScript::EPSLineJoin(o, PostScript::ROUND_JOIN);
    PostScript::EPSLineWidth(o, 0);

  double intensity = 0;
  double gradMag = 0;

  for (int i = 0; i < nPoints; ++i)
  {
    // Position of stipple dots
    x = cvRound(pointList->at(i).x);
    y = cvRound(pointList->at(i).y);
    radius = MAX_STIPPLE_RADIUS;
    // Importance
    //intensity = (double)((unsigned char)(toneImg->imageData[y*ws + x])) / 255;	// to be [0, 1]

    //if (intensity > 0.95)
    //	radius = 0.f;
    //else
    //{
    //	//gradMag = ((unsigned char)(gradientImg->imageData[y*ws + x])) / 255;	// to be [0, 1]
    //	importance = (double) (WEIGHT_DARKNESS * (1 - intensity) + WEIGHT_GRADIENT_MAGNITUDE * gradMag);	// to be [0, 1]

    //	// Compare two cases for point radius
    //	radius = (float) (MAX_STIPPLE_RADIUS * pow(importance, (1 / GAMMA)));
    //}

    //if (radius > 0.f)
    {
      // Draw circle
      PostScript::EPSPoint(o, x, (h-y), radius);
    }
  }

  printf("-- Overlay edges...\n");
  // Overlay edges
  for (int y = 0; y < h; ++y)
  {
    for (int x = 0; x < w; ++x)
    {
      if ((unsigned char)(edgeImg->imageData[y*ws + x]) == (unsigned char) 0)
      {
        // Draw one pixel
        PostScript::EPSPoint(o, x, (h-y), 1.f);
      }
    }
  }

  return true;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int CStippleDB::getStippleSize(int i, int stipple_color)
{
  int size = 0;

  IplImage* img = _stipples.at(i);
  int width = img->width;
  int height = img->height;
  int widthStep = img->widthStep;

  for (int i = 0; i < height; ++i)
  {
    for (int j = 0; j < width; ++j)
    {
      if ((unsigned char)(img->imageData[i*widthStep + j]) == stipple_color)
        ++size;
    }
  }

  return size;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CStippleDB::removeStipple(int i)
{
  _stipples.erase(_stipples.begin() + i);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CStippleDB::removeSmallStipples(int stipple_color)
{
  int count = getCount();
  int stippleSize = 0;
  int sumStippleSize = 0;
  int i = 0;
  while (i < count)
  {
    stippleSize = getStippleSize(i, stipple_color);

    if (stippleSize < (int)(_avgStippleSize * 0.5))
    {
      removeStipple(i);
      --count;
    }
    else
    {
      sumStippleSize += stippleSize;
      ++i;
    }
  }

  _avgStippleSize = sumStippleSize / getCount();

}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int CStippleDB::load(const char* dir, int background_color)
{
  float sumStippleSize = 0;
  float curStippleSize;
  int nStipples = 0;

  char oldDir[256];

//  _getcwd(oldDir, 256);
  #ifdef LINUX
  getcwd(oldDir, 256);
  #else
  _getcwd(oldDir,255);
  #endif


//    chdir(dir);
  #ifdef LINUX
  chdir(dir);
  #else
  _chdir(dir);
  #endif

  // Find all *.png files for stipples

  dirent* file;
  DIR *hFile;


  hFile=opendir(".");
  int i=0;
  if (hFile==NULL) {
    if (i==0) {
      cout << "No .png files found in current directory" << endl;
      return (-1);
    }
  }
  else{
    while ((file=readdir(hFile)) != NULL ){
      char *extension=strrchr(file->d_name,'.');
      if (extension!=NULL) {
        if (strcasecmp(extension,".png")==0){

          // Usage: cvLoadImage(filename, iscolor=1)
          // "iscolor" specifies colorness of the loaded image.
          // if >0, the loaded image is forced to be color 3-channel image;
          // if 0, the loaded image is forced to be grayscale;
          // if <0, the loaded image will be loaded as is (with number of channels depends on the file).
          IplImage* stippleImg = cvLoadImage(file->d_name, -1);
          addStipple(stippleImg);
          ++nStipples;

          // Compute a stipple size
          curStippleSize = 0;
          for (int i = 0; i < stippleImg->height; ++i)
          {
            for (int j = 0; j <= stippleImg->width; ++j)
            {
              if (((unsigned char)(stippleImg->imageData[i*stippleImg->widthStep + j])) != background_color)
                ++curStippleSize;
            }
          }
          sumStippleSize += curStippleSize;
          i++;
        }
      }
    }
  }
  closedir(hFile);
  // Set an average stipple size
  _avgStippleSize = sumStippleSize / nStipples;

  chdir(oldDir);

  return nStipples;
}



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/
/****************************************************************************************/
// stippleList: output for sitpple merging

void CStipplePlacer::placeStipples(IplImage* img, vector<_pixel>* pointList, CStippleDB* stippleDB, vector<IplImage*>* stippleList, unsigned char stipple_color, IplImage* edgeImg)
{
//	int width = img->width;
//	int height = img->height;
  int widthStep = img->widthStep;

  int nStipple = stippleDB->getCount();
  int* usedStipple = new int[nStipple];
  int usedStippleCount = 0;
  memset(usedStipple, 0, sizeof(int)*nStipple);

  CRandomMersenne rng;
  rng.RandomInit((unsigned)time(0));

  int stippleId = 0;
  IplImage* stippleImg = 0;
  int rotation_angle = 0;

  // Copy stipple to output image
  int x, y;
  int count = (int) pointList->size();
  for (int i = 0; i < count; ++i)
  {
    x = pointList->at(i).x;
    y = pointList->at(i).y;

    // Case: # of synthesized points is greater than # of example points
    if (usedStippleCount >= nStipple)
    {
      memset(usedStipple, 0, sizeof(int)*nStipple);
    }
    // Get random stipple
    do {
      stippleId = rng.IRandomX(0, nStipple-1);
    }while (usedStipple[stippleId]);
    usedStipple[stippleId] = 1;
    ++usedStippleCount;
    stippleImg = stippleDB->getStipple(stippleId);

    // Get random rotation angle (degree: 0~359)
    rotation_angle = rng.IRandomX(0, 359);

    // Do not put stipples on edges
    if (edgeImg != 0 && (unsigned char)(edgeImg->imageData[y*widthStep+x]) == 255)	// For rendering process
    {
      // Place a stipple
      putStipple(img, stippleImg, x, y, rotation_angle, stipple_color);

      // Saving for stipple merging
      stippleList->push_back(stippleImg);
    }
    else if (edgeImg == 0)					// For texture synthesis with stipple marks
    {
      // Place a stipple
      putStipple(img, stippleImg, x, y, rotation_angle, stipple_color);
    }
    else									// For rendering process
    {
      //pointList->erase(pointList->begin() + i);
      stippleList->push_back(0);

      // TEST
      //printf("Found stipple point on edge.\n");
    }

  }

  delete []usedStipple;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CStipplePlacer::putStipple(IplImage* img, IplImage* stippleImg, int x, int y, int rotation_angle, unsigned char stipple_color)
{
  if (img->nChannels != 1 || stippleImg->nChannels != 1)
  {
    printf("Error in CStipplePlacer: imgage and stipple should be grayscale\n");
    exit(1);
  }

  int width = img->width;
  int height = img->height;
  int widthStep = img->widthStep;

  int stipple_w = stippleImg->width;
  int stipple_h = stippleImg->height;
  int stipple_ws = stippleImg->widthStep;
  int cx = (int) (stipple_w * 0.5f);
  int cy = (int) (stipple_h * 0.5f);

  // Left-top corner coordinates for stippleImg on img
  int sx = x - cx;
  int sy = y - cy;

  // Place this stipple image on the img
  unsigned char pixel = 0;
  for (int i = 0; i < stipple_h; ++i)
  {
    for (int j = 0; j < stipple_w; ++j)
    {
      pixel = (unsigned char) stippleImg->imageData[i*stipple_ws + j];
      if (pixel == stipple_color)//STIPPLE_COLOR)
      {
        // TEST for no rotation
        putPixel(img, sx+j, sy+i, pixel);
      }
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void CStipplePlacer::putPixel(IplImage* img, int x, int y, unsigned char pixel)
{
  if (img->nChannels != 1)
  {
    printf("Error: imgage should be grayscale\n");
    exit(1);
  }

  int width = img->width;
  int height = img->height;
  int widthStep = img->widthStep;

  if (x >= 0 && x < width && y >= 0 && y < height)
    img->imageData[y*widthStep + x] = pixel;
}
