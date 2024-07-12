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


#ifndef _FILTER_RWT_H
#define _FILTER_RWT_H

#include <QDialog>
#include <QGroupBox>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include "line_edit.h"
#include <string>
#include "filter.h"

#define DEFINED_FILTER_RWT


/*********************************************************************************************
 geometry.h
 *********************************************************************************************/

class Vec2;
class Vec3;

/*******************
********************
****            ****
****    Vec2    ****
****            ****
********************
*******************/

class Vec2
{
public:
  Vec2(const Vec2 & v) { x = v.x; y = v.y; };
  Vec2(const Vec3 & v);
  Vec2() : x(0), y(0) {};
  Vec2(float _x, float _y) : x(_x), y(_y) {};

  Vec2 operator+(const Vec2& v) const { return Vec2(x+v.x, y+v.y); };
  Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; };

  Vec2 operator-(const Vec2& v) const { return Vec2(x-v.x, y-v.y); };
  Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; };

  Vec2 operator*(float f) const { return Vec2(x*f, y*f); };
  Vec2& operator*=(float f) { x *= f; y *= f; return *this; };

  Vec2 operator/(float f) const { return this->operator*(1.0f / f); }
  Vec2& operator/=(float f) { return this->operator*=(1.0f / f); }

  float operator*(const Vec2& v) const { return x*v.x + y*v.y; };

  float operator%(const Vec2& v) const { return x*v.y - y*v.x; };

  Vec2 operator-() const { return Vec2(-x, -y); };
  Vec2& normalize() { this->operator/=(sqrtf(x*x + y*y)); return *this; };

  float magnitude() { return sqrtf(x*x + y*y); };
  float sqrMagnitude() { return x*x + y*y; };

  Vec2 perp() { return Vec2(-y, x); };

  float get(int element) const { return (&x)[element]; }
  void set(int element, float NewValue) { (&x)[element] = NewValue; }

  float x, y;
};

/*******************
********************
****            ****
****    Vec3    ****
****            ****
********************
*******************/

class Vec3
{
public:
  Vec3(const Vec3& v) { x = v.x; y = v.y; z = v.z; };
  Vec3() : x(0), y(0), z(0) {};
  Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
  Vec3(Vec2 & v) { x = v.x; y = v.y; z = 1.f; };

  bool operator!=(const Vec2 & v) const { return ((x != v.x) || (y != v.y)); };

  Vec3 operator+(const Vec3& v) const { return Vec3(x+v.x, y+v.y, z+v.z); };
  Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; };

  Vec3 operator-(const Vec3& v) const { return Vec3(x-v.x, y-v.y, z-v.z); };
  Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; };

  Vec3 operator*(float f) const { return Vec3(x*f, y*f, z*f); };
  Vec3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; };

  Vec3 operator/(float f) const { return this->operator*(1.0f / f); }
  Vec3& operator/=(float f) { return this->operator*=(1.0f / f); }

  float operator*(const Vec3& v) const { return x*v.x + y*v.y + z*v.z; };

  Vec3 operator%(const Vec3 & v) const { return Vec3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); };

  Vec3 operator-() const { return Vec3(-x, -y, -z); };
  Vec3& normalize() { this->operator/=(sqrtf(x*x + y*y + z*z)); return *this; };

  float magnitude() { return sqrtf(x*x + y*y + z*z); };
  float sqrMagnitude() { return x*x + y*y + z*z; };

  float get(int element) const { return (&x)[element]; }
  void set(int element, float NewValue) { (&x)[element] = NewValue; }

  float x, y, z;
};

/*********************************************************************************************
 geometry.h
 *********************************************************************************************/
#define PI 3.141592654f
#define FLOAT_MAX  99999999999999999.f
#define FLOAT_MIN -99999999999999999.f

typedef unsigned char ubyte;

// math quickies
inline float cotf(float a) { return cosf(a)/sinf(a); };
inline float fractf(float a) { return a - floorf(a); };
inline float sqrf(float a) { return a*a; };
inline int sqri(int a) { return a*a; };
inline int powi(int base, int exp) { return int(powf((float)base, (float)exp)); };
inline float logf(float base, float x) { return logf(x)/logf(base); };
inline int logi(int base, int x) { return int(logf((float)x)/logf((float)base)); };
inline float signf(float x) { if (x > 0) return 1; else if (x < 0) return -1; else return 0; };
inline float maxf(float a, float b) { if (a < b) return b; else return a; };
inline float minf(float a, float b) { if (a < b) return a; else return b; };
inline int maxi(int a, int b) { if (a < b) return b; else return a; };
inline int mini(int a, int b) { if (a < b) return a; else return b; };
inline int mini(int a, int b, int c) { if (a < b) { if(a < c) return a; else return c; } else { if (b < c) return b; else return c; } };
inline int absi(int a) { if (a < 0) return -a; else return a; };
inline short abss(short a) { if (a < 0) return -a; else return a; };
inline void swapi(int & a, int & b) { int t = a; a = b; b = t; };
inline int floori(float a) { return int(floor(a)); };
inline int ceili(float a) { return int(ceil(a)); };
inline float clampf(float x, float min, float max)
{
  if (x < min) x = min;
  if (x > max) x = max;
  return x;
}
inline int clampi(int x, int min, int max)
{
  if (x < min) x = min;
  if (x > max) x = max;
  return x;
}

// errors
void error(const char * szErrorString, ...);
void error(char * szErrorString);
void error(const unsigned char * szErrorString);
void error(std::string sErrorString);

// file handling
unsigned char freaduc(FILE * fIn);
short freads(FILE * fIn);
unsigned short freadus(FILE * fIn);
int freadi(FILE * fIn);
unsigned int freadui(FILE * fIn);
float freadf(FILE * fIn);
double freadd(FILE * fIn);

void fwrites(FILE * fOut, short s);
void fwrited(FILE * fOut, double d);
void fwritef(FILE * fOut, float f);
void fwritei(FILE * fOut, int i);
void fwriteui(FILE * fOut, unsigned int ui);
void fwriteuc(FILE * fOut, unsigned char ui);

/****************************************************************************************/

namespace _f_rwt_ns
{
  // info 1
  const std::string String_group_box_info1("Number of dots");
  const unsigned int Num_renderbuffers=2;
  const int Front_plane=-100;
  const int Back_plane=100;
  const int Apex_height=60;
  const int Point_height=80;

  class _pixel
  {
  public:
    unsigned char Red;
    unsigned char Green;
    unsigned char Blue;
  };

#ifdef WIN_COMPILER
  const double M_PI = 3.1415926535897932;
#endif

  #define MAX_POINTS 1024*1024
}

// rwt
struct Tile
{
  int n, e, s, w;
  int numSubtiles, numSubdivs, numPoints, numSubPoints;
  int ** subdivs;
  Vec2 * points, * subPoints;
};


class _gl_widget;
class _qtw_filter_rwt;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_rwt : public _filter
{
public:
    _filter_rwt();
    ~_filter_rwt(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};

    void stippling(cv::Mat *Input_image0,cv::Mat *Output_image0);
    void update();

    // rwt
    float sampleDensMap(float x, float y);
    void recurseTile(Tile & t, float x, float y, int level);
    void paintPoints();
    void savePoints(const char * fileName);
    void loadTileSet(const char * fileName);
    void recursive_wang_tiles(_gl_widget *GLWidget1);

    virtual void set_info1(unsigned int Value){Q_UNUSED(Value)};

    int Window_width;
    int Window_height;

    bool Modulate_dot_size;
    int Min_dot_size;
    int Max_dot_size;
    bool Antialias;

    bool Local_change;

    unsigned int Framebuffer; // id for the framebuffer
    unsigned int Renderbuffers[_f_rwt_ns::Num_renderbuffers]; // id for the renderbuffers

    // for reading the image
    std::vector<_f_rwt_ns::_pixel> Pixels;

    // rwt
    float toneScale = 200000;

    float clipMinX, clipMaxX, clipMinY, clipMaxY;
    int numTiles, numSubtiles, numSubdivs;
    Tile *tiles;
    Vec2 *points;
    int numPoints;
    int densTexSize;
    float *densTex = 0;
    Vec3 vpos;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filter_rwt_ui : public _filter_rwt
{
public:
  _filter_rwt_ui(_gl_widget *GL_widget1,std::string Name1="RWT Stippling parameters");
  _filter_rwt_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters, std::string Box_name="RWT Stippling parameters");
  ~_filter_rwt_ui();
  void reset_data();


  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);

  void local_change(bool Value){Local_change=Value;};

  void set_info1(unsigned int Value1);

private:
  _qtw_filter_rwt *Qtw_filter_rwt;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _qtw_filter_rwt: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_rwt(_gl_widget *GL_widget1,_filter_rwt_ui *Filter1,std::string Box_name="RWT Stippling parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_info1(unsigned int Value);

private:
  QGroupBox *Group_box_main;
  QGroupBox *Group_box_info1;

  // number of dots
   QLineEdit *Line_edit_info1;

  _filter_rwt_ui *Filter;
  _gl_widget *GL_widget;
};
#endif




