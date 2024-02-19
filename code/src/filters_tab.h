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


#ifndef FILTERS_TAB_H
#define FILTERS_TAB_H

#include <QDialog>
#include <QListWidget>
#include <QGroupBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <string>
#include "map_list.h"

#include "line_edit.h"

class _gl_widget;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _filters_tab: public QWidget
{
Q_OBJECT

public:
  _filters_tab(_gl_widget *GL_widget, QWidget *Parent = 0);
  void  set_item(std::string Name);
  void  insert_widgets();
  void  remove_widgets();
  void  set_selected_item(int Pos);
  void  clear(){Filters_list->clear();};

protected slots:
  void item_clicked(QListWidgetItem *);

private:
  QGroupBox *Group_box_filters;
  QListWidget *Filters_list;
  QVBoxLayout *Vertical_parameters;
  _gl_widget *GL_widget;
};

#endif
