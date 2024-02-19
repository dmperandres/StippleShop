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


#ifndef _FILTER_STIPPLING_SBE_H
#define _FILTER_STIPPLING_SBE_H

#ifdef WINDOWS
#include <direct.h>
#endif

#include <QDialog>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QProgressDialog>
#include "line_edit.h"
#include <string>
#include "filter.h"

#include <vector>
#include <fstream>
#include <string>
#include <iostream>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>

#ifdef WINDOWS
#include <io.h>
#include "dirent_w.h"
#else
#include <unistd.h>
#include <dirent.h>
#endif

//#include <unistd.h>
#include <stdio.h>


#ifdef WINDOWS
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif


#define DEFINED_FILTER_SBE

using namespace std;

class _gl_widget;
class _qtw_filter_stippling_sbe;

namespace _f_stippling_sbe1_ns
{
  class myPoint {
    public:
    float x, y;
    myPoint(const float a, const float b): x(a), y(b) { }
    myPoint(void) { }
  };

  inline std::ostream& operator<<(std::ostream& o, const myPoint& p) {
      return o << '(' << p.x << ',' << p.y << ')';
  }

  class myRect {
      public:
      float l1, l2, orient;
      myRect(const float length1, const float length2, const float orientation):
          l1(length1), l2(length2), orient(orientation) { }
      myRect(void) { }
  };

  inline std::ostream& operator<<(std::ostream& o, const myRect& r) {
      return o << r.l1 << 'x' << r.l2 << " orient. " << r.orient;
  }

  class Stipple: public myPoint {
      public:
      float radius;
      Stipple(const float a, const float b, const float c) :
          myPoint(a, b), radius(c) { }
      Stipple(void) { }
  };

  inline bool operator<(const myPoint& a, const myPoint& b) {
    if (a.y == b.y) return (a.x < b.x);
    return (a.y < b.y);
  }

  class Limits {
    public:
    float xmin, ymin, xmax, ymax;
    Limits(const float x1, const float y1, const float x2, const float y2):
        xmin(x1), ymin(y1), xmax(x2), ymax(y2) { }
    Limits(void) { }
    bool inside(const myPoint& p) const {
      return (p.x >= xmin && p.x <= xmax && p.y >= ymin && p.y <= ymax);
    }
};

inline std::ostream& operator<<(std::ostream& o, const Limits& l) {
    return o << "xmin: " << l.xmin << " xmax: " << l.xmax
             << " ymin: " << l.ymin << " ymax: " << l.ymax;
}


//#define INFO

// Procedures: Open what you want to perform
//#define EXTRACT_EXAMPLE_TEXTURES
//#define DOWN_SAMPLING
//#define LOAD_DOWN_SAMPLED_IMG		// TEST 0404	// NPAR09
//#define ANALYZE_TEXTURES
//#define SYNTHESIZE_TEXTURES
//# define LOAD_SYNTHESIZE_TEXTURE	// TEST 0406	// NPAR09
//#define PLACE_STIPPLES
//#define MERGE_STIPPLES	// Do not use this. It doesn't work
#define RENDER
//#define CHECK_RENDER_TIME
#define USE_TONE_SELECTION_BASEDON_PROBABILITY true
#define USE_FULL_LINEAR		true
#define USE_TANH_FLAG		false//true//

#define SHOW_IMAGES			false
#define USE_GPU_FLAG		false

// Set this as 1 when an example texture is one of tone 4~7 among Bill's tones
#define DARK_TONE_FLAG		0//1//
#define MAX_ITERATION_OPT_DIST	20
#define INTERVAL_OPT_DIST	10

// For 256x256 texture, 8
// For 30x60 texture, 4
#define CONNECTED_COMPONENT_NEIGHBORS	8//4//

// To use offset information as structural analysis
#define USE_OFFSET_BETWEEN_STIPPLES		true

// To use count buffer mode, white seed texture should be use
#define USE_COUNT_BUFFER	true

// Count buffer
#define PIXELS_PER_COUNTBUFFER_CELL	10

// This can be used for white stipple marks on a black background
// black = 0, white = 255
#define STIPPLE_COLOR		0//255//
#define BACKGROUND_COLOR	255//0//

// 0: standard synthesis
// 1: synthesis of center pixels
#define SYNTHESIS_TYPE		1//0	// Now, this is not meaningful because of using USE_OFFSET_BETWEEN_STIPPLES
#define MAX_ITERATION		100//30//

#define DEST_DIR_SYNTHESIS			"..\\synthesized\\"
#define DEST_DIR_RENDER				"..\\rendered\\"
#define DEST_DIR_DISTRIBUTION		"..\\analyzed\\"
#define DEST_DIR_EXAMPLE			"..\\segmented\\"

// Hand-drawn stippling
#define HANDDRAWN_STIPPLING_WORK	"..\\example\\damstra_stipple_blastoid.png"//Artist-BillAndrew.png"//
#define SCALE_OF_RESIZE				0.5
#define PIXELS_PER_CELL				30
#define THRESHOLD_CC				3
//#define HANDDRAWN_STIPPLING_WORK	"..\\example\\scan0004_bw.bmp"//Trudy Nicholson
//#define SCALE_OF_RESIZE			0.5
//#define PIXELS_PER_CELL			10
//#define THRESHOLD_CC				10

// Example and Seed textures
#define EXAMPLE_TEXTURE_NAME		"..\\example\\tone1_256_blackandwhite.bmp"//..\\WorkFor_NPAR09\\Hand-drawn_EmilyDamstra\\tone4_example.png"//tone1_256_blackandwhite.bmp"//tone1_ed_60.bmp"//..\\WorkFor_NPAR09\\Hand-drawn_EmilyDamstra\\tone1_example.png"//
#define SEED_TEXTURE_NAME			"..\\example\\tone0_256.bmp"//white_60.bmp"//..\\WorkFor_NPAR09\\Hand-drawn_EmilyDamstra\\tone0_125.png"//
#define EXAMPLE_DIST_TEXTURE_NAME	"..\\analyzed\\example_distribution.png"

// For rendering
#define SOURCE_IMAGE_FILE			"..\\images\\gray_gradient_4096.bmp"//bubble_4096.png"//water_flea_4096.png"//..\\WorkFor_NPAR09\\Hand-drawn_BillAndrew\\Artist-BillAndrew_noline_withWhite_GaussianBlur25.png"//..\\images\\horse_960.png"//Hand-drawn_EmilyDamstra\\downsampled_image_GaussianBlur10.png"//..\\images\\gray_gradient_4096.bmp"////	// NPAR09
//#define DISTRIBUTION_MAP_FILE		"..\\..\\tone10_BillAndrew_01112009_256_dist\\dist10_info_4096x4096_from256.txt"
//#define DISTRIBUTION_MAP_FILE		"..\\..\\tone10_EmilyDamstra_01182009_60_dist\\dist10_info_960x960_from60.txt"
#define DISTRIBUTION_MAP_FILE		"..\\..\\tone10_BillAndrew_04062009_256_dist\\dist10_info_4096x4096_from256_8.txt"//NPAR09_dist6_info_4096x4096_from256.txt"	// NPAR09
//#define DISTRIBUTION_MAP_FILE		"..\\..\\tone10_EmilyDamstra_01182009_60_dist\\dist10_info_960x960_from60_npar09.txt"											// NPAR09

#define STIPPLE_MARK_TYPE			1	// 0:circle, 1:real stipple mark

#define LARGE_STIPPLE_MARKS_DIR		"..\\stipple_large\\"
#define RENDER_STIPPLE_MARK_FLAG	true//false//

#define STIPPLE_MARKS_DIR			"..\\stipple_black_regular\\"//..\\WorkFor_NPAR09\\Hand-drawn_EmilyDamstra\\stipple_black_npar09\\"//..\\stipples_backup\\Damstra_stipple_black_regular\\"////////D:\\usr\\Sungye\\Stippling_GLCM\\ExampleByStippling\\stipples_backup\\Damstra_stipple_black_regular\\"//stipple_black_large_npar09\\"//stipple_black_regular\\"//stipple_black\\"//		// NPAR09
#define WHITE_STIPPLE_MARKS_DIR		"..\\stipple_white\\"//..\\WorkFor_NPAR09\\Hand-drawn_EmilyDamstra\\stipple_white_npar09\\"//..\\stipples_backup\\Damstra_stipple_white_regular\\"//"////D:\\usr\\Sungye\\Stippling_GLCM\\ExampleByStippling\\stipples_backup\\Damstra_stipple_white_regular\\"//
#define WHITE_STIPPLE_START_INDEX	7//			// NPAR09 // Bill Andrews=7
//#define STIPPLE_MARKS_DIR			"..\\stipples_backup\\Damstra_stipple_black_regular\\"// NPAR09
//#define WHITE_STIPPLE_MARKS_DIR		"..\\stipples_backup\\Damstra_stipple_white_regular\\" // NPAR09
//#define WHITE_STIPPLE_START_INDEX	6//			// NPAR09 // Emily Damstra=6

#define GAMMA						2.0
#define MAX_STIPPLE_RADIUS			4
#define WEIGHT_DARKNESS				1//0.5
#define	WEIGHT_GRADIENT_MAGNITUDE	0//0.5

#define RENDER_EDGE_FLAG			false
#define GIVEN_EDGE_IMAGE			false		// NPAR09
#define EDGE_IMAGE_FILE				"..\\images\\kidney2_edge.png"//..\\WorkFor_NPAR09\\Hand-drawn_BillAndrew\\StippledHorse_line_4096.png"//..\\images\\Artist-BillAndrew_allEdge_4096.png"//		// NPAR09

#define POST_PROCESSING_FLAG		false//false//

#define DOG_SIGMA1					1.3f
#define	DOG_SIGMA2					1.8f
#define SHORT_EDGE_SIZE				200
#define VISIT_MARK					10

// Not completed...
// For merging stipples
#define	STIPPLE_MERGE_T1			0.5f
#define	STIPPLE_MERGE_T2			1.0f

// Not used now...
#define GLCM_WINDOW_SIZE			30
#define SYNTH_IMAGE_SIZE			60

//--------------------------------------------------
// Change this value for each Tone level
// For Tone1~Tone3, 1.5f makes better results.
// For Tone4~Tone6, 2.5f makes better results.
// For _SYNTHESIS_TYPE_ = 1, 0.f
// For just in case, this is hardcoded in synthesize function for type = 1
//--------------------------------------------------
// Not used now... so, defined with zero
#define NEIGHBOR_RANGE			0.f//1.5f//0.f//

struct pixel {
  int x;
  int y;
};
typedef struct pixel _pixel;

// Voronoi pixel
struct vpixel {
  int _x;		// x coord
  int _y;		// y coord
  float _w;	// weight
};
typedef struct vpixel	_vpixel;


class float2 {
public:
  float _x, _y;
};

// Double pixel
struct double2 {
  double _x;
  double _y;
};
typedef struct double2	_double2;


#define BOOSTBLURFACTOR 0.9//90.0
}

/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

namespace _f_stippling_sbe_ns
{
  // parameter 1
  const std::string String_group_box_parameter1("Save intermediate results");
  const std::string String_parameter1_tooltip("Controls if the intermediate images are saved");

  // parameter 2
  const std::string String_group_box_parameter2("Combine edges");
  const std::string String_parameter2_tooltip("Controls if the edges are combined");


  //parameter3
  const std::string String_group_box_parameter3("Post processing");
  const std::string String_parameter3_tooltip("Controls if the postprocessing is applied");

  //
  const bool STIPPLING_SBE_SAVE_INTERMEDIATE_RESULTS_DEFAULT=false;
  const bool STIPPLING_SBE_COMBINE_EDGES_DEFAULT=false;
  const bool STIPPLING_SBE_POST_PROCESSING_DEFAULT=true;
}





class _filter_stippling_sbe : public _filter
{
public:
    _filter_stippling_sbe();
    ~_filter_stippling_sbe(){};
    void reset_data();
    bool change_output_image_size(){return Change_output_image_size;};
    bool use_dots(){return Use_dots;};

    void stippling(cv::Mat *Input_image0,cv::Mat *Output_image0);
    void update();

    void save_intermediate_results(bool Save_intermediate_results1){Save_intermediate_results=Save_intermediate_results1;};
    void combine_edges(bool Combine_edges1){Combine_edges=Combine_edges1;};
    void post_processing(bool Post_processing1){Post_processing=Post_processing1;};

    bool save_intermediate_results(){return Save_intermediate_results;};
    bool combine_edges(){return Combine_edges;};
    bool post_processing(){return Post_processing;};

    int Canny_size,Threshold1,Threshold2;
    bool Save_intermediate_results=false;
    bool Combine_edges=false;
    bool Post_processing=true;
};



class _filter_stippling_sbe_ui : public _filter_stippling_sbe
{
public:
  _filter_stippling_sbe_ui(_gl_widget *GL_widget1,std::string Name1="Stippling by Example parameters");
  _filter_stippling_sbe_ui(_gl_widget *GL_widget1,std::map<std::string,std::string> &Parameters, std::string Box_name="Stippling by Example parameters");
  ~_filter_stippling_sbe_ui();
  void reset_data();


  void show();
  void hide();
  void *get_link();
  void read_parameters(std::map<std::string,std::string> &Parameters);
  void write_parameters(std::map<std::string,std::string> &Parameters);

  void parameter1(bool Value){_filter_stippling_sbe::save_intermediate_results(Value);};
  void parameter2(bool Value){_filter_stippling_sbe::combine_edges(Value);};
  void parameter3(bool Value){_filter_stippling_sbe::post_processing(Value);};

  bool parameter1(){return _filter_stippling_sbe::save_intermediate_results();};
  bool parameter2(){return _filter_stippling_sbe::combine_edges();};
  bool parameter3(){return _filter_stippling_sbe::post_processing();};




private:
  _qtw_filter_stippling_sbe *Qtw_filter_stippling_sbe;
  };


class _qtw_filter_stippling_sbe: public QWidget
{
  Q_OBJECT
public:

  _qtw_filter_stippling_sbe(_gl_widget *GL_widget1,_filter_stippling_sbe_ui *Filter1,std::string Box_name="Stippling by Example parameters");

  void show(){Group_box_main->show();};
  void hide(){Group_box_main->hide();};
  QGroupBox *get_link(){return Group_box_main;};

  void set_parameter1(bool Value);
  void set_parameter2(bool Value);
  void set_parameter3(bool Value);


protected slots:
  void set_parameter1_slot(int Value);
  void set_parameter2_slot(int Value);
  void set_parameter3_slot(int Value);



private:
  QGroupBox *Group_box_main;

  QGroupBox *Group_box_parameter1;
  QGroupBox *Group_box_parameter2;
  QGroupBox *Group_box_parameter3;

  // save intermediate results
  QCheckBox *Checkbox_parameter1;

  // combine edges
  QCheckBox *Checkbox_parameter2;

  // postprocessing
  QCheckBox *Checkbox_parameter3;


  _filter_stippling_sbe_ui *Filter;
  _gl_widget *GL_widget;
};


/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

class CDistributionMap {
public:
  char _info[256];
  vector<IplImage*> _distMap;
  vector<double> _avgGrayValue;

public:
  CDistributionMap();
  ~CDistributionMap();

  IplImage* getMap(int i);
  double getAvgGrayValue(int i);
  void addMap(IplImage* img);
  int getCount();

  int load(char* filename);
  void computeAvgGrayValue();
};

inline CDistributionMap::CDistributionMap()
{
  _info[0] = '\0';
};

inline CDistributionMap::~CDistributionMap()
{
  _distMap.clear();
  _avgGrayValue.clear();
};

inline IplImage* CDistributionMap::getMap(int i)
{
  return _distMap.at(i);
}
inline void CDistributionMap::addMap(IplImage* img)
{
  _distMap.push_back(img);
}

inline int CDistributionMap::getCount()
{
  return (int) (_distMap.size());
}

inline double CDistributionMap::getAvgGrayValue(int i)
{
  return _avgGrayValue.at(i);
}

/***************************************************************************************/

/*------------------------------------------------------------------------
 *
 * Purdue University Rendering and Perceptualizaton Lab
 * Electrical and Computer Engineering Department
 * Purdue University
 *
 *//**
 *
 * \brief
 * \author	SungYe Kim (inside@purdue.edu)
 *
 *//*
 *
 * $Date: 2008/04/00 $
 *//*-------------------------------------------------------------------*/

// Reference: http://www.gamedev.net/reference/programming/features/imageproc/page2.asp

class CGaussianFilter {
public:
  int _width;
  float* _kernel;
  float _sum;

public:
  CGaussianFilter()
  {
    _kernel = 0;
    _sum = 0;
    _width = 0;
  }
  ~CGaussianFilter()
  {
    if (_kernel)
      delete []_kernel;
  };

  void GaussianSmooth(unsigned char* image, int rows, int cols, int widthStep, float sigma, short int **smoothedim);
  void MakeGaussianKernel(float sigma, float **kernel, int *windowsize);

};

/****************************************************************************************/


class CDoGFilter {
public:
  CGaussianFilter* _pGaussianFilter;

public:
  CDoGFilter();
  ~CDoGFilter();

  void DoGFilter(IplImage* srcImg, IplImage* edgeImg, float sigma1, float sigma2);
};


/****************************************************************************************/

class PostScript {
  //static const int scale = 7 * 72;		// 7 inches * 72 points per inch
  static const int scale = 1;

  public:
  enum LineCap { BUTT_CAP = 0, ROUND_CAP, SQUARE_CAP };
  enum LineJoin { MITER_JOIN = 0, ROUND_JOIN, BEVEL_JOIN };

  // Prepare/Finish are for simple single page documents
  static void EPSPreparePage(ostream& o, const string& title, const _f_stippling_sbe1_ns::Limits& limits);
  static void EPSFinishPage(ostream& o);

  static void EPSHeader(ostream& o, const string title, const _f_stippling_sbe1_ns::Limits& limits);
  static void EPSPageHeader(ostream& o, const _f_stippling_sbe1_ns::Limits& limits);
  static void EPSProlog(ostream& o);
  static void EPSTrailer(ostream& o);

    static void EPSLineWidth(ostream& o, const float w);
  static void EPSLineCap(ostream& o, const LineCap cap);
  static void EPSLineJoin(ostream& o, const LineJoin join);
  static void EPSPoint(ostream& o, const float x, const float y, const float r);
  static void EPSLine(ostream& o, const float x1, const float y1, const float x2, const float y2);
  static void EPSBox(ostream& o, const float x1, const float y1, const float x2, const float y2);
};

/****************************************************************************************/

/*************************** RANDOMC.H ***************** 2007-09-22 Agner Fog *
*
* This file contains class declarations and other definitions for the C++
* library of uniform random number generators.
*
* Overview of classes:
* ====================
*
* class CRandomMersenne:
* Random number generator of type Mersenne twister.
* Source file mersenne.cpp
*
* class CRandomMother:
* Random number generator of type Mother-of-All (Multiply with carry).
* Source file mother.cpp
*
*
* Member functions (methods):
* ===========================
*
* All these classes have identical member functions:
*
* Constructor(uint32 seed):
* The seed can be any integer. Usually the time is used as seed.
* Executing a program twice with the same seed will give the same sequence of
* random numbers. A different seed will give a different sequence.
*
* void RandomInit(uint32 seed);
* Re-initializes the random number generator with a new seed.
*
* void RandomInitByArray(uint32 seeds[], int length);
* In CRandomMersenne only: Use this function if you want to initialize with
* a seed with more than 32 bits. All bits in the seeds[] array will influence
* the sequence of random numbers generated. length is the number of entries
* in the seeds[] array.
*
* double Random();
* Gives a floating point random number in the interval 0 <= x < 1.
* The resolution is 32 bits in CRandomMother and CRandomMersenne.
*
* int IRandom(int min, int max);
* Gives an integer random number in the interval min <= x <= max.
* (max-min < MAXINT).
* The precision is 2^-32 (defined as the difference in frequency between
* possible output values). The frequencies are exact if max-min+1 is a
* power of 2.
*
* int IRandomX(int min, int max);
* Same as IRandom, but exact. In CRandomMersenne only.
* The frequencies of all output values are exactly the same for an
* infinitely long sequence. (Only relevant for extremely long sequences).
*
* uint32 BRandom();
* Gives 32 random bits.
*
*
* Example:
* ========
* The file EX-RAN.CPP contains an example of how to generate random numbers.
*
*
* Optimized version:
* ==================
* Faster versions of these random number generators are provided as function
* libraries in asmlib.zip. These function libraries are coded in assembly
* language and support only x86 platforms, including 32-bit and 64-bit
* Windows, Linux, BSD, Mac OS-X (Intel based). Use asmlibran.h from asmlib.zip
*
*
* Non-uniform random number generators:
* =====================================
* Random number generators with various non-uniform distributions are available
* in stocc.zip (www.agner.org/random).
*
*
* Further documentation:
* ======================
* The file randomc.htm contains further documentation on these random number
* generators.
*
*
* Copyright:
============
* © 1997 - 2007 Agner Fog. All software in this library is published under the
* GNU General Public License with the further restriction that it cannot be
* used for gambling applications. See licence.htm
*******************************************************************************/

// Define 32 bit signed and unsigned integers.
// Change these definitions, if necessary, to match a particular platform
#if defined(_WIN16) || defined(__MSDOS__) || defined(_MSDOS)
   // 16 bit systems use long int for 32 bit integer
   typedef long int           int32;   // 32 bit signed integer
   typedef unsigned long int  uint32;  // 32 bit unsigned integer
#else
   // Most other systems use int for 32 bit integer
   typedef int                int32;   // 32 bit signed integer
   typedef unsigned int       uint32;  // 32 bit unsigned integer
#endif

// Define 64 bit signed and unsigned integers, if possible
#if (defined(__WINDOWS__) || defined(_WIN32)) && (defined(_MSC_VER) || defined(__INTEL_COMPILER))
   // Microsoft and other compilers under Windows use __int64
   typedef __int64            int64;   // 64 bit signed integer
   typedef unsigned __int64   uint64;  // 64 bit unsigned integer
   #define INT64_DEFINED               // Remember that int64 is defined
#elif defined(__unix__) && (defined(_M_IX86) || defined(_M_X64))
   // Gnu and other compilers under Linux etc. use long long
   typedef long long          int64;   // 64 bit signed integer
   typedef unsigned long long uint64;  // 64 bit unsigned integer
   #define INT64_DEFINED               // Remember that int64 is defined
#else
   // 64 bit integers not defined
   // You may include definitions for other platforms here
#endif


/***********************************************************************
System-specific user interface functions
***********************************************************************/

void EndOfProgram(void);               // System-specific exit code (userintf.cpp)

void FatalError(char * ErrorText);     // System-specific error reporting (userintf.cpp)


/***********************************************************************
Define random number generator classes
***********************************************************************/

class CRandomMersenne {                // Encapsulate random number generator
#if 0
   // Define constants for type MT11213A:
#define MERS_N   351
#define MERS_M   175
#define MERS_R   19
#define MERS_U   11
#define MERS_S   7
#define MERS_T   15
#define MERS_L   17
#define MERS_A   0xE4BD75F5
#define MERS_B   0x655E5280
#define MERS_C   0xFFD58000
#else
   // or constants for type MT19937:
#define MERS_N   624
#define MERS_M   397
#define MERS_R   31
#define MERS_U   11
#define MERS_S   7
#define MERS_T   15
#define MERS_L   18
#define MERS_A   0x9908B0DF
#define MERS_B   0x9D2C5680
#define MERS_C   0xEFC60000
#endif
public:
  // Random number TEST
  CRandomMersenne() { LastInterval = 0; }
  CRandomMersenne(uint32 seed) { RandomInit(seed); LastInterval = 0; }
  void RandomInit(uint32 seed);       // Re-seed
    void RandomInitByArray(uint32 seeds[], int length); // Seed by more than 32 bits
    int IRandom (int min, int max);     // Output random integer
    int IRandomX(int min, int max);     // Output random integer, exact
    double Random();                    // Output random float
    uint32 BRandom();                   // Output random bits
private:
    void Init0(uint32 seed);            // Basic initialization procedure
    uint32 mt[MERS_N];                  // State vector
    int mti;                            // Index into mt
    uint32 LastInterval;                // Last interval length for IRandomX
    uint32 RLimit;                      // Rejection limit used by IRandomX
    enum TArch {LITTLE_ENDIAN1, BIG_ENDIAN1, NONIEEE}; // Definition of architecture
    TArch Architecture;                 // Conversion to float depends on architecture
};

class CRandomMother {             // Encapsulate random number generator
public:
    void RandomInit(uint32 seed);       // Initialization
    int IRandom(int min, int max);      // Get integer random number in desired interval
    double Random();                    // Get floating point random number
    uint32 BRandom();                   // Output random bits
    // Random number TEST
    CRandomMother() {;};   // Constructor
    CRandomMother(uint32 seed) { RandomInit(seed); }
protected:
    uint32 x[5];                        // History buffer
};

/****************************************************************************************/

class CStippleDB;
class CDistributionMap;

class CRender {
public:

public:
  CRender() {};
  ~CRender() {};

  void render(IplImage* srcImg, IplImage* dstImg, CDistributionMap* distMap,
        int mark_type, const char* stipple_dir = 0, const char* white_stipple_dir = 0,
        int stipple_flag = false,
        bool edge_flag = 0, IplImage* edgeImg_in = 0,
        int post_processing_flag = 0,
        const char* dir_dest = 0,
        int given_edge_image = GIVEN_EDGE_IMAGE,
        int white_stipple_start_index = WHITE_STIPPLE_START_INDEX,
        int use_tone_selection_basedon_prob = USE_TONE_SELECTION_BASEDON_PROBABILITY,
        int use_full_linear = USE_FULL_LINEAR,
        int use_tanh = USE_TANH_FLAG,
        int stipple_color = STIPPLE_COLOR,
        int background_color = BACKGROUND_COLOR,
        float dog_sigma1 = DOG_SIGMA1,
        float dog_sigma2 = DOG_SIGMA2,
        int short_edge_size = SHORT_EDGE_SIZE);

  IplImage* computeEdge(IplImage* srcImg,
        const char* dir_dest = 0,
        float dog_sigma1 = DOG_SIGMA1,
        float dog_sigma2 = DOG_SIGMA2,
        int short_edge_size = SHORT_EDGE_SIZE,
        int background_color = BACKGROUND_COLOR,
        int stipple_color = STIPPLE_COLOR);

  void combineEdges(IplImage* img, IplImage* edge);

  bool traverse(IplImage* img, int x, int y, int& length);
  void removeShortEdges(IplImage* img, int minCount,
        int background_color = BACKGROUND_COLOR,
        int stipple_color = STIPPLE_COLOR);
  void deleteEdge(IplImage* img, int x, int y);

  void smoothTransition(IplImage* img, IplImage* levelMap);
  void checkUnnecessaryBlackPixels(int x, int y, IplImage* img, IplImage* levelMap);
  void checkUnnecessaryWhitePixels(int x, int y, IplImage* img, IplImage* levelMap);

  void smoothTransition(IplImage* img, IplImage* stipplePosMap, IplImage* levelMap);
  void checkUnnecessaryBlackPixels(int x, int y, IplImage* img, IplImage* stipplePosMap, IplImage* levelMap);
  void checkUnnecessaryWhitePixels(int x, int y, IplImage* img, IplImage* stipplePosMap, IplImage* levelMap);
  bool isFound(int x, int y, vector<_f_stippling_sbe1_ns::_pixel>* pointList);
  int findMin(vector<_f_stippling_sbe1_ns::_pixel>* list);

  void outputEPSPoints(const char* filename, const char* title, IplImage* toneImg, IplImage* gradientImg, IplImage* edgeImg, vector<_f_stippling_sbe1_ns::_pixel>* pointList);
  bool outputEPSPoints(std::ostream& o, IplImage* toneImg, IplImage* gradientImg, IplImage* edgeImg, vector<_f_stippling_sbe1_ns::_pixel>* pointList);
};


/****************************************************************************************/

class CStippleDB {
public:
  int _avgStippleSize;			// average stipple size in pixels
  vector<IplImage*> _stipples;	// list of stipple images

public:
  CStippleDB();
  ~CStippleDB();

  void addStipple(IplImage* stippleImg);
  IplImage* getStipple(int i);


  int getCount();

  int getStippleSize(int i, int stipple_color = STIPPLE_COLOR);
  void removeSmallStipples(int stipple_color = STIPPLE_COLOR);
  void removeStipple(int i);

  int load(const char* dir, int background_color = BACKGROUND_COLOR);

};

inline CStippleDB::CStippleDB()
{
  _avgStippleSize = 0;
}

inline CStippleDB::~CStippleDB()
{
  _stipples.clear();
}

inline void CStippleDB::addStipple(IplImage* stippleImg)
{
  if (stippleImg->nChannels == 1)
  {
    _stipples.push_back(stippleImg);
  }
  else
  {
    printf("Error in CStippleDB: stipple image doesn't have 1 channel so that it isn't added.");
  }
}

inline IplImage* CStippleDB::getStipple(int i)
{
  return _stipples.at(i);
}

inline int CStippleDB::getCount()
{
  return (int) _stipples.size();
}

/****************************************************************************************/

class CStippleDB;
class CStipplePlacer {
public:


public:
  CStipplePlacer() {};
  ~CStipplePlacer() {};

  void placeStipples(IplImage* img, vector<_f_stippling_sbe1_ns::_pixel>* pointList, CStippleDB* stippleDB, vector<IplImage*>* stippleList,
            unsigned char stipple_color, IplImage* edgeImg = 0);

  void putStipple(IplImage* img, IplImage* stippleImg, int x, int y, int rotation_angle, unsigned char stipple_color);

  void putPixel(IplImage* img, int x, int y, unsigned char pixel);

};

/***************************************************************************************/
#endif
