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


#ifndef IMAGES_TAB_H
#define IMAGES_TAB_H

#include <QDialog>
#include <QListWidget>
#include <QGroupBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <string>

#include "line_edit.h"

class _gl_widget;


/*****************************************************************************//**
 * class for the tab that includes the images
 *
 *
 *
 *****************************************************************************/

class _images_tab: public QWidget
{
  Q_OBJECT

public:
        _images_tab(_gl_widget *GL_widget, QWidget *Parent = 0);
  void  set_item(std::string Name);
  void  set_selected_item(int Pos);
  void  clear(){Images_list->clear();};

protected slots:
  void item_clicked(QListWidgetItem *);

private:
  QListWidget *Images_list;
  _gl_widget *GL_widget;
};

#endif
