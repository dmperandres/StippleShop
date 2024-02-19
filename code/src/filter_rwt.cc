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


#include "filter_rwt.h"
#include "glwidget.h"

using namespace _f_rwt_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_rwt::_filter_rwt()
{
  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_RWT;

  Scaling_factor=1;
  Local_change=false;

  Change_output_image_size=false;
  Use_dots=false;

  loadTileSet("aux_code/rwt/tileset.dat");
  points = new Vec2[MAX_POINTS];
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/
void _filter_rwt::reset_data()
{

}


/*****************************************************************************//**
 * Sample the density texture. By default we use linear filtering here.
 *
 *
 *
 *****************************************************************************/

float _filter_rwt::sampleDensMap(float x, float y)
{
#define SAMPLE_LINEAR
#ifdef SAMPLE_LINEAR
  float tx = x*densTexSize;
  float ty = y*densTexSize;

//  cout << " sample " << densTexSize << endl;
  int ix = clampi(floori(tx), 0, densTexSize-2);
  int iy = clampi(floori(ty), 0, densTexSize-2);
  tx -= ix;
  ty -= iy;
  float sample = (densTex[iy*densTexSize+ix]*(1-tx)*(1-ty) +
          densTex[iy*densTexSize+ix+1]*tx*(1-ty) +
          densTex[(iy+1)*densTexSize+ix]*(1-tx)*ty +
          densTex[(iy+1)*densTexSize+ix+1]*tx*ty);
  return sample;
#else
  int ix = clampi(x*densTexSize, 0, densTexSize-1);
  int iy = clampi(y*densTexSize, 0, densTexSize-1);
  return densTex[iy*densTexSize+ix];
#endif
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt::recurseTile(Tile & t, float x, float y, int level)
{
  float tileSize = 1.f/powf(float(numSubtiles), float(level));
  if ((x+tileSize < clipMinX) || (x > clipMaxX) || (y+tileSize < clipMinY) || (y > clipMaxY))
    return;

  int numTests = mini(t.numSubPoints, int(powf(vpos.z, -2.f)/powf(float(numSubtiles), 2.f*level)*toneScale-t.numPoints));
  float factor = 1.f/powf(vpos.z, -2.f)*powf(float(numSubtiles), 2.f*level)/toneScale;

  for (int i = 0; i < numTests; i++)
  {
    float px = x+t.subPoints[i].x*tileSize, py = y+t.subPoints[i].y*tileSize;

    // skip point if it lies outside of the clipping window
    if ((px < clipMinX) || (px > clipMaxX) || (py < clipMinY) || (py > clipMaxY))
      continue;

    // reject point based on its rank
    if (sampleDensMap(px, py) < (i+t.numPoints)*factor)
      continue;

    // "draw" point
    points[numPoints].x = px;
    points[numPoints].y = py;
    numPoints++;
  }

  // recursion
  if (powf(vpos.z, -2.f)/powf(float(numSubtiles), 2.f*level)*toneScale-t.numPoints > t.numSubPoints)
  {
    for (int ty = 0; ty < numSubtiles; ty++)
      for (int tx = 0; tx < numSubtiles; tx++)
        recurseTile(tiles[t.subdivs[0][ty*numSubtiles+tx]], x+tx*tileSize/numSubtiles, y+ty*tileSize/numSubtiles, level+1);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt::paintPoints()
{
  clipMinX = vpos.x;
  clipMaxX = vpos.x+vpos.z;
  clipMinY = vpos.y;
  clipMaxY = vpos.y+vpos.z;
  numPoints = 0;

//	float startTime = getTime();

//  cout << "Valor1=" << (int)tiles[0].numPoints << endl;
//  cout << "Valor2=" << powf(vpos.z, -2.f) << endl;
//  cout << "Valor3=" << vpos.z << endl;

  int numTests = mini((int)tiles[0].numPoints, int(powf(vpos.z, -2.f)*toneScale));
  float factor = 1.f/powf(vpos.z, -2)/toneScale;
  for (int i = 0; i < numTests; i++)
  {
    float px = tiles[0].points[i].x, py = tiles[0].points[i].y;

    // skip point if it lies outside of the clipping window
    if ((px < clipMinX) || (px > clipMaxX) || (py < clipMinY) || (py > clipMaxY))
      continue;

    // reject point based on its rank
    if (sampleDensMap(px, py) < i*factor)
      continue;

    // "draw" point
    points[numPoints].x = px;
    points[numPoints].y = py;
    numPoints++;

//    cout << "Punto: " << px << " " << py << endl;
  }

  // recursion
  recurseTile(tiles[0], 0, 0, 0);

//	float endTime = getTime();

//	printf("%d points in %.2f ms = %.0f points/s\n", numPoints, (endTime-startTime)*1000, numPoints/(endTime-startTime));
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt::savePoints(const char * fileName)
{
  FILE * fout = fopen(fileName, "wb");

  fwritei(fout, numPoints);
  for (int i = 0; i < numPoints; i++)
  {
    fwritef(fout, points[i].x);
    fwritef(fout, points[i].y);
  }

  fclose(fout);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt::loadTileSet(const char * fileName)
{
  FILE *fin;


  if ((fin=fopen(fileName,"rb"))!=nullptr){

    numTiles = freadi(fin);
    numSubtiles = freadi(fin);
    numSubdivs = freadi(fin);

//    cout << "Num tiles " << numTiles << endl;
//    cout << "Num subtiles " << numSubtiles << endl;
//    cout << "Num subdivs " << numSubdivs << endl;

    tiles = new Tile[numTiles];

    for (int i = 0; i < numTiles; i++)
    {
      tiles[i].n = freadi(fin);
      tiles[i].e = freadi(fin);
      tiles[i].s = freadi(fin);
      tiles[i].w = freadi(fin);

      tiles[i].subdivs = new int * [numSubdivs];
      for (int j = 0; j < numSubdivs; j++)
      {
        int * subdiv = new int[sqri(numSubtiles)];
        for (int k = 0; k < sqri(numSubtiles); k++)
          subdiv[k] = freadi(fin);
        tiles[i].subdivs[j] = subdiv;
      }

      tiles[i].numPoints = freadi(fin);
  //    cout << "NumPoints " << tiles[i].numPoints << endl;

      tiles[i].points = new Vec2[tiles[i].numPoints];
      for (int j = 0; j < tiles[i].numPoints; j++)
      {
        tiles[i].points[j].x = freadf(fin);
        tiles[i].points[j].y = freadf(fin);

  //      cout << "Points " << tiles[i].points[j].x << " " << tiles[i].points[j].y << endl;
        freadi(fin);freadi(fin);freadi(fin);freadi(fin);
      }

      tiles[i].numSubPoints = freadi(fin);
      tiles[i].subPoints = new Vec2[tiles[i].numSubPoints];
      for (int j = 0; j < tiles[i].numSubPoints; j++)
      {
        tiles[i].subPoints[j].x = freadf(fin);
        tiles[i].subPoints[j].y = freadf(fin);
  //      cout << "SubPoints " << tiles[i].subPoints[j].x << " " << tiles[i].subPoints[j].y << endl;
        freadi(fin);freadi(fin);freadi(fin);freadi(fin);
      }
    }

    fclose(fin);
  }
  else{
    cout << "Error: cannot open the file "  << fileName << endl;
    exit(-1);
  }
}


/*****************************************************************************//**
 * This functions assign the selected result of a filter to the frame buffer to show it
 *
 *
 *
 *****************************************************************************/

void _filter_rwt::stippling(cv::Mat *Input_image0,cv::Mat *Output_image0)
{
  if (Local_change==false){

    densTexSize=Input_image0->cols;

    densTex = new float[sqri(densTexSize)];
    for (int i = 0; i < sqri(densTexSize); i++){
      densTex[i] = 1 - Input_image0->at<unsigned char>(i)/255.f;
    }
    // This data is used to define the window (x,y,Width=x+z,Height=y+z)
    // the coordiantes are normalized
    vpos.x =0;
    vpos.y =0;
    vpos.z =1;
    paintPoints();

    Output_image0->setTo(255);
    for (int i=0;i<numPoints;i++){
      Output_image0->at<unsigned char>(points[i].y*densTexSize,points[i].x*densTexSize)=(unsigned char)0;
    }

  }
  else{
    Output_image0->setTo(255);
    for (int i=0;i<numPoints;i++){
      Output_image0->at<unsigned char>(points[i].y*densTexSize,points[i].x*densTexSize)=(unsigned char)0;
    }
    Local_change=false;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt::update()
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

      stippling(Aux_input_image,Aux_output_image);

      cvtColor(*Aux_output_image,*Output_image_0,cv::COLOR_GRAY2RGB,3);
    }
    else cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << endl;
  }
  else{// the same number of channels
    stippling(Aux_input_image,Output_image_0);
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;

  set_info1(numPoints);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_rwt_ui::_filter_rwt_ui(_gl_widget *GL_widget1,string Name1)
{
  Name=Name1;
  Qtw_filter_rwt=new _qtw_filter_rwt(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_rwt_ui::_filter_rwt_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters,string Box_name)
{
  read_parameters(Parameters);
  Name=Box_name;
  Qtw_filter_rwt=new _qtw_filter_rwt(GL_widget1,this,Box_name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_rwt_ui::~_filter_rwt_ui()
{
  delete Qtw_filter_rwt;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt_ui::reset_data()
{
  _filter_rwt::reset_data();
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt_ui::show()
{
  Qtw_filter_rwt->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt_ui::hide()
{
  Qtw_filter_rwt->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_rwt_ui::get_link()
{
  return Qtw_filter_rwt->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  Q_UNUSED(Parameters)
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt_ui::write_parameters(std::map<string, string> &Parameters)
{
  Q_UNUSED(Parameters)
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_rwt_ui::set_info1(unsigned int Value1)
{
  Qtw_filter_rwt->set_info1(Value1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_rwt::_qtw_filter_rwt(_gl_widget *GL_widget1,_filter_rwt_ui *Filter1,string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_main=new QVBoxLayout;

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

  Vertical_box_main->addWidget(Group_box_info1);

  Group_box_main->setLayout(Vertical_box_main);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_rwt::set_info1(unsigned int Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_info1->setText(Str);
}

/*********************************************************************************************
 util.h
**********************************************************************************************/


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void error(const char * szErrorString, ...)
{
  Q_UNUSED(szErrorString)
//  char cpBuffer[1024];
//	va_list tArgPtr;
//	va_start(tArgPtr, szErrorString);
//	vsprintf(cpBuffer, szErrorString, tArgPtr);

//	std::cout << cpBuffer << std::endl;
//	MessageBoxA(NULL, (LPCSTR)cpBuffer, (LPCSTR)"ERROR", MB_OK|MB_ICONEXCLAMATION);
//	exit(0);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void error(char * szErrorString)
{
  error((const char *)szErrorString);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void error(const unsigned char * szErrorString)
{
  error((const char *)szErrorString);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void error(std::string sErrorString)
{
  error(sErrorString.c_str());
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

unsigned char freaduc(FILE * fIn)
{
  unsigned char ucTemp;
  fread(&ucTemp, sizeof(unsigned char), 1, fIn);
  return ucTemp;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

short freads(FILE * fIn)
{
  short sTemp;
  fread(&sTemp, sizeof(short), 1, fIn);
  return sTemp;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

unsigned short freadus(FILE * fIn)
{
  unsigned short usTemp;
  fread(&usTemp, sizeof(unsigned short), 1, fIn);
  return usTemp;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int freadi(FILE * fIn)
{
  int iTemp;
  fread(&iTemp, sizeof(int), 1, fIn);
  return iTemp;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

unsigned int freadui(FILE * fIn)
{
  unsigned int uiTemp;
  fread(&uiTemp, sizeof(unsigned int), 1, fIn);
  return uiTemp;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

float freadf(FILE * fIn)
{
  float fTemp;
  fread(&fTemp, sizeof(float), 1, fIn);
  return fTemp;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

double freadd(FILE * fIn)
{
  double dTemp;
  fread(&dTemp, sizeof(double), 1, fIn);
  return dTemp;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void fwrites(FILE * fOut, short s)
{
  fwrite(&s, sizeof(short), 1, fOut);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void fwrited(FILE * fOut, double d)
{
  fwrite(&d, sizeof(double), 1, fOut);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void fwritef(FILE * fOut, float f)
{
  fwrite(&f, sizeof(float), 1, fOut);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void fwritei(FILE * fOut, int i)
{
  fwrite(&i, sizeof(int), 1, fOut);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void fwriteui(FILE * fOut, unsigned int ui)
{
  fwrite(&ui, sizeof(unsigned int), 1, fOut);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void fwriteuc(FILE * fOut, unsigned char uc)
{
  fwrite(&uc, sizeof(unsigned char), 1, fOut);
}
