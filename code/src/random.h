// LIC

#ifndef _RANDOM_M_H
#define _RANDOM_M_H

#include <iostream>
#include <random>

namespace _random_constant {
  const long int RANDOM_CONSTANT=51753;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _random_normal
{
public:

  _random_normal();
  _random_normal(double Mean, double Stddev, double Min1, double Max1);
  void init(double Mean, double Stddev, double Min1, double Max1);
  void	seed();
  void	set_seed(long int Seed1);
  long int get_seed();
  void	save_seed();
  void  load_seed();
  double value();

  std::mt19937 Generator;
  std::normal_distribution<double> Distribution;
  unsigned int Seed;
  unsigned int Saved_seed;
  double Min;
  double Max;
  double Value;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _random_uniform_int
{
public:

  _random_uniform_int();
  _random_uniform_int(int Min1,int Max1);
  void init(int Min1,int Max1);
  void seed();
  void set_seed(long int Seed1);
  long int get_seed();
  void save_seed();
  void load_seed();
  int value();

  std::mt19937 Generator;
  std::uniform_int_distribution<int> Distribution;
  unsigned int Seed;
  unsigned int Saved_seed;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _random_uniform_double
{
public:

  _random_uniform_double();
  _random_uniform_double(double Min1,double Max1);
  void init(double Min1,double Max1);
  void seed();
  void set_seed(long int Seed1);
  long int get_seed();
  void save_seed();
  void load_seed();
  double value();

  std::mt19937 Generator;
  std::uniform_real_distribution<double> Distribution;
  unsigned int Seed;
  unsigned int Saved_seed;
};
#endif
