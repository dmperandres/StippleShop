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


#include "filters_tab.h"
#include "glwidget.h"


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filters_tab::_filters_tab(_gl_widget *GL_widget1, QWidget *Parent)
: QWidget(Parent)
{
  QString Str;

  GL_widget=GL_widget1;

  // images
  Group_box_filters=new QGroupBox(tr("Filters"));
  Group_box_filters->setAlignment(Qt::AlignCenter);

  Filters_list = new QListWidget;
  Filters_list->setToolTip(tr("Selects which filter is shown or saved"));

  // for adjusting the size of the widget depending on the height of the font
  QFontMetrics Font_metrics(Filters_list->fontMetrics());
  Filters_list->setMaximumHeight(Font_metrics.height()*10);

  QVBoxLayout *Vertical_filters = new QVBoxLayout;

  Vertical_filters->addWidget(Filters_list,0,Qt::AlignTop);
  Group_box_filters->setLayout(Vertical_filters);

  connect(Filters_list,SIGNAL(itemClicked(QListWidgetItem *)), this,SLOT(item_clicked(QListWidgetItem *)));

  // general
  Vertical_parameters=new QVBoxLayout;

  Vertical_parameters->addWidget(Group_box_filters);

  for(int i=0;i<GL_widget->Filters.size();i++){
    Vertical_parameters->addWidget((QGroupBox *)(GL_widget->Filters.get_data(i)->get_link()),0,Qt::AlignTop);
  }

  setLayout(Vertical_parameters);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filters_tab::set_item(string Name)
{
  QListWidgetItem *Item=new QListWidgetItem;
  Item->setText(tr(Name.c_str()));
  Filters_list->insertItem(Filters_list->count(),Item);
  Filters_list->updateGeometry();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filters_tab::insert_widgets()
{
  for (int i=0;i<GL_widget->Filters.size();i++){
    Vertical_parameters->insertWidget(-1,(QGroupBox *)GL_widget->Filters.get_data(i)->get_link());
  }

  Vertical_parameters->addStretch(-1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filters_tab::remove_widgets()
{
  QLayoutItem *Child;
  while ((Child = Vertical_parameters->takeAt(0)) != 0) {
    //cout << "Borrando " << Vertical_parameters->count() << endl;
    delete Child->widget();
    delete Child;
  }


  Group_box_filters=new QGroupBox(tr("Filters"));
  Group_box_filters->setAlignment(Qt::AlignCenter);

  Filters_list = new QListWidget;
  Filters_list->setToolTip(tr("Selects which filter is shown or saved"));

  // for adjusting the size of the widget depending on the height of the font
  QFontMetrics Font_metrics(Filters_list->fontMetrics());
  Filters_list->setMaximumHeight(Font_metrics.height()*10);

  QVBoxLayout *Vertical_filters = new QVBoxLayout;
  Vertical_filters->addWidget(Filters_list,0,Qt::AlignTop);
  Group_box_filters->setLayout(Vertical_filters);

  Vertical_parameters->addWidget(Group_box_filters);

  connect(Filters_list,SIGNAL(itemClicked(QListWidgetItem *)), this,SLOT(item_clicked(QListWidgetItem *)));
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filters_tab::set_selected_item(int Pos)
{
  Filters_list->setCurrentRow(Pos);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filters_tab::item_clicked(QListWidgetItem *Item)
{  
  Q_UNUSED(Item)
  GL_widget->set_filter((Item->text()).toStdString());
}
