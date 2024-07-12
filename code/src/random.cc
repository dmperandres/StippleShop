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


#include "random.h"

//using namespace std;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_random_normal::_random_normal()
{
  std::random_device Random_device;  //Will be used to obtain a seed for the random number engine
  Seed=Random_device();
  Generator.seed(Seed);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_random_normal::_random_normal(double Mean, double Stddev, double Min1, double Max1):
      Distribution(Mean, Stddev)
{
  std::random_device Random_device;  //Will be used to obtain a seed for the random number engine
  Seed=Random_device();
  Generator.seed(Seed);

  if (Min1<=Max1){
    Min=Min1;
    Max=Max1;
  }
  else std::cout << "Error:_random_normal: Min > Max " << std::endl;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_normal::init(double Mean, double Stddev, double Min1, double Max1)
{
  if (Min1<=Max1){
    Distribution.param(std::normal_distribution<double>::param_type(Mean,Stddev));
    Min=Min1;
    Max=Max1;
  }
  else std::cout << "Error:_random_normal: Min > Max " << std::endl;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void	_random_normal::seed()
{
  std::random_device Random_device;  //Will be used to obtain a seed for the random number engine
  Seed=Random_device();
  Generator.seed(Seed);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void	_random_normal::set_seed(long int Seed1)
{
  Seed=Seed1;
  Generator.seed(Seed);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

long int _random_normal::get_seed()
{
  return Seed;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void	_random_normal::save_seed()
{
  Saved_seed=Seed;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void  _random_normal::load_seed()
{
  Seed=Saved_seed;
  Generator.seed(Seed);
  Distribution.reset();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

double _random_normal::value()
{
  do{
    Value=Distribution(Generator);
  } while (Value<Min || Value>Max);
  return Value;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_random_uniform_int::_random_uniform_int()
{
  std::random_device Random_device;  //Will be used to obtain a seed for the random number engine
  Seed=Random_device();
  Generator.seed(Seed);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_random_uniform_int::_random_uniform_int(int Min1,int Max1)
{
  std::random_device Random_device;  //Will be used to obtain a seed for the random number engine
  Seed=Random_device();
  Generator.seed(Seed);

  if (Min1<=Max1){
    Distribution.param(std::uniform_int_distribution<int>::param_type(Min1,Max1));
  }
  else std::cout << "Error:_random_uniform_int: Min > Max " << std::endl;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_int::init(int Min1,int Max1)
{
  if (Min1<=Max1){
    Distribution.param(std::uniform_int_distribution<int>::param_type(Min1,Max1));
  }
  else std::cout << "Error:_random_uniform_int: Min > Max " << std::endl;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_int::seed()
{
  std::random_device Random_device;  //Will be used to obtain a seed for the random number engine
  Seed=Random_device();
  Generator.seed(Seed);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_int::set_seed(long int Seed1)
{
  Seed=Seed1;
  Generator.seed(Seed);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

long int _random_uniform_int::get_seed()
{
  return Seed;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_int::save_seed()
{
  Saved_seed=Seed;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_int::load_seed()
{
  Seed=Saved_seed;
  Generator.seed(Seed);
  Distribution.reset();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int _random_uniform_int::value()
{
  return Distribution(Generator);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_random_uniform_double::_random_uniform_double()
{
  std::random_device Random_device;  //Will be used to obtain a seed for the random number engine
  Seed=Random_device();
  Generator.seed(Seed);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_random_uniform_double::_random_uniform_double(double Min1,double Max1)
{
  std::random_device Random_device;  //Will be used to obtain a seed for the random number engine
  Seed=Random_device();
  Generator.seed(Seed);
  if (Min1<=Max1){
    Distribution.param(std::uniform_real_distribution<double>::param_type(Min1,Max1));
  }
  else std::cout << "Error:_random_uniform_double: Min > Max " << std::endl;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_double::init(double Min1,double Max1)
{
  if (Min1<=Max1){
    Distribution.param(std::uniform_real_distribution<double>::param_type(Min1,Max1));
  }
  else std::cout << "Error:_random_uniform_double: Min > Max " << std::endl;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_double::seed()
{
  std::random_device Random_device;  //Will be used to obtain a seed for the random number engine
  Seed=Random_device();
  Generator.seed(Seed);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_double::set_seed(long int Seed1)
{
  Seed=Seed1;
  Generator.seed(Seed);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

long int _random_uniform_double::get_seed()
{
  return Seed;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_double::save_seed()
{
  Saved_seed=Seed;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _random_uniform_double::load_seed()
{
  Seed=Saved_seed;
  Generator.seed(Seed);
  Generator.discard(700000);
  Distribution.reset();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

double _random_uniform_double::value()
{
  return Distribution(Generator);
}
