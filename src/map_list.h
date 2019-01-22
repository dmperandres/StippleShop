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


#ifndef _MAP_LIST_H
#define _MAP_LIST_H

#ifdef X64
  #include <opencv.hpp>
#else
  #include <imgproc/imgproc.hpp>
  #include <highgui/highgui.hpp>
#endif

#include <string>
#include <stdexcept>
#include <iostream>
#include <QMessageBox>



/*****************************************************************************//**
 * aux class for doing search by name or position
 *
 *
 *
 *****************************************************************************/

template <class Type>
class _map_list
{
public:

  int size(){return Data_by_string.size();};
  void clear();

  void set(std::string Name,Type Data1);
  Type get_data(std::string Name);
  Type get_data(int Position);
  int get_position(std::string Name);
  std::string get_string(int Position);
  
  std::map<std::string,Type> Data_by_string;
  std::map<int,Type> Data_by_position;
  std::map<std::string,int> Key_by_string;
  std::map<int, std::string> Key_by_position;
};


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

template <class Type>
void _map_list<Type>::clear()
{
  Data_by_string.clear();
  Data_by_position.clear();
  Key_by_string.clear();
  Key_by_position.clear();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

template <class Type>
void _map_list<Type>::set(std::string Name,Type Data1)
{
  try{
    Data_by_string.at(Name);
  }
  catch (const std::out_of_range& oor) {

    int Position=Data_by_string.size();

    Data_by_string[Name]=Data1;
    Data_by_position[Position]=Data1;
    Key_by_string[Name]=Position;
    Key_by_position[Position]=Name;
    return;
  }
  QString Error("Error: the name is duplicated:  ");
  Error+=Name.c_str();
  QMessageBox MsgBox;
  MsgBox.setText(Error);
  MsgBox.exec();

}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

template <class Type>
Type _map_list<Type>::get_data(std::string Name)
{
  try{
    Data_by_string.at(Name);
  }
  catch (const std::out_of_range& oor) {
    QString Error("Error in the definition of filter with this text:  ");
    Error+=Name.c_str();
    QMessageBox MsgBox;
    MsgBox.setText(Error);
    MsgBox.exec();
    return ((Type)nullptr);
  }
  return Data_by_string.at(Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

template <class Type>
Type _map_list<Type>::get_data(int Position)
{
  try{
    Data_by_position.at(Position);
  }
  catch (const std::out_of_range& oor) {
    QString Error("Error: the position does not exist:  ");
    QMessageBox MsgBox;
    MsgBox.setText(Error);
    MsgBox.exec();
    return ((Type) nullptr);
  }
  return Data_by_position.at(Position);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

template <class Type>
int _map_list< Type>::get_position(std::string Name)
{
  try{
    Key_by_string.at(Name);
  }
  catch (const std::out_of_range& oor) {
    QString Error("Error: the name does not exist:  ");
    Error+=Name.c_str();
    QMessageBox MsgBox;
    MsgBox.setText(Error);
    MsgBox.exec();
    return (-1);
  }
  return Key_by_string.at(Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

template <class Type>
std::string _map_list< Type>::get_string(int Position)
{
  try{
    Key_by_position.at(Position);
  }
  catch (const std::out_of_range& oor) {
    QString Error("Error: the position does not exist");
    QMessageBox MsgBox;
    MsgBox.setText(Error);
    MsgBox.exec();
    return ("");
  }
  return Key_by_position.at(Position);
}

#endif
