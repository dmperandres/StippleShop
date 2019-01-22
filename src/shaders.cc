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


#include "shaders.h"


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool _shaders::read_file(QString File_name, QString &Code)
{
  QFile File(File_name);
  if (!File.open(QIODevice::ReadOnly | QIODevice::Text)){
    QMessageBox::information(0,"Error",File.errorString());
    return false;
  }
  QTextStream Input(&File);
  Code=Input.readAll();
  File.close();
  return true;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

GLuint _shaders::load_shaders(QString File_vertex_shader,QString File_fragment_shader)
{
  GLuint Vertex_shader;
  GLuint Fragment_shader;
  GLuint Program;
  GLint  Vertices_shader_compiled, Fragment_shader_compiled;
  GLint  Shaders_compiled;

  // Read files
  QString Vertex_shader_source;
  read_file(File_vertex_shader,Vertex_shader_source);

  QString Fragment_shader_source;
  read_file(File_fragment_shader,Fragment_shader_source);

  // Create the shader
  Vertex_shader=glCreateShader(GL_VERTEX_SHADER);
  Fragment_shader=glCreateShader(GL_FRAGMENT_SHADER);

  // Assign the source code
  string Str=Vertex_shader_source.toStdString();
  const char *Aux=Str.c_str();
  glShaderSource(Vertex_shader,1,(const GLchar **) &Aux,NULL);

  Str=Fragment_shader_source.toStdString();
  const char *Aux1=Str.c_str();
  glShaderSource(Fragment_shader,1,(const GLchar **) &Aux1,NULL);

  // Compiling
  glCompileShader(Vertex_shader);
  // Check
  glGetShaderiv(Vertex_shader, GL_COMPILE_STATUS, &Vertices_shader_compiled);
  if (Vertices_shader_compiled==GL_FALSE){
    cout << "Error compiling the vertex shader" << endl;
    return(0);
  }

  glCompileShader(Fragment_shader);
  // Check
  glGetShaderiv(Fragment_shader, GL_COMPILE_STATUS, &Fragment_shader_compiled);
  if (Fragment_shader_compiled==GL_FALSE){
    cout << "Error compiling the fragment shader" << endl;
    return(0);
  }

  // Create the program
  Program=glCreateProgram();

  // Attach
  glAttachShader(Program,Vertex_shader);
  glAttachShader(Program,Fragment_shader);

  // Link
  glLinkProgram(Program);
  // check
  glGetProgramiv(Program, GL_LINK_STATUS, &Shaders_compiled);
  if (Shaders_compiled==GL_FALSE){
    cout << "Error linking" << endl;
    return(0);
  }

  return(Program);
}
