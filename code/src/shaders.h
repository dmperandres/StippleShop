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


#ifndef SHADERS_H
#define SHADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <GL/glew.h>
#include <GL/glu.h>
#include <iostream>
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include "vertex.h"
#include "map_list.h"

#define GL_GLEXT_PROTOTYPES


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _shaders
{
public:
  GLuint load_shaders(QString File_vertex_shader, QString File_fragment_shader);

protected:
  bool read_file(QString File_name,QString &Code);
  void show_error(GLint Shader);
};

#endif
