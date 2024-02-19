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


#ifndef _TREE_WIDGET_ITEM_H
#define _TREE_WIDGET_ITEM_H

#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <iostream>


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _tree_widget_item : public QTreeWidgetItem
{
public:
  _tree_widget_item(QTreeWidget *Parent, int Type1 = Type):QTreeWidgetItem(Parent,Type1){};
  _tree_widget_item(_tree_widget_item *Tree_widget_item1, int Type1 = Type):QTreeWidgetItem(Tree_widget_item1,Type1){};
};

#endif
