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


#include "tree_widget.h"
#include <iostream>


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_tree_widget::_tree_widget(_window *Window1, QWidget *Parent1):QTreeWidget(Parent1)
{
  Window=Window1;

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(context_menu_slot(const QPoint&)));

  Selected_item=nullptr;
  Icon.load(":/icons/filter.png");
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _tree_widget::context_menu_slot(const QPoint& Pos)
{
  QTreeWidgetItem* item = itemAt(Pos);

  if (item && item->type()==ItemType2){
    // Note: We must map the point to global from the viewport to
    // account for the header.
    show_context_menu(item, viewport()->mapToGlobal(Pos));
  }
  else item->setSelected(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _tree_widget::show_context_menu(QTreeWidgetItem* Item, const QPoint& globalPos)
{
  Q_UNUSED(Item)

  QMenu menu;

  QAction *Add_filter = new QAction(tr("Add filter"),nullptr);
  connect(Add_filter, &QAction::triggered, [&](){add_filter_slot(Item);});
  menu.addAction(Add_filter);

  QAction *Description = new QAction(tr("Description"),nullptr);
  connect(Description, &QAction::triggered, [&](){description_slot(Item);});
  menu.addAction(Description);

  menu.exec(globalPos);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _tree_widget::add_filter_slot(QTreeWidgetItem* Item)
{
  QVariant Value=Item->data(0,1);
  Window->add_filter(Value.toInt());
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _tree_widget::description_slot(QTreeWidgetItem* Item)
{
  QVariant Value=Item->data(0,1);
  Window->description(Value.toInt());
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _tree_widget::mousePressEvent(QMouseEvent *Event)
{
  if (Selected_item) Selected_item->setSelected(false);

  Selected_item =(_tree_widget_item *) itemAt(Event->pos());

  if (Selected_item && Selected_item->type()==ItemType2){
    // Note: We must map the point to global from the viewport to
    // account for the header.
    Selected_item->setSelected(true);
    Drag_start_position = Event->pos();
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _tree_widget::mouseMoveEvent(QMouseEvent *Event)
{
  if (!(Event->buttons() & Qt::LeftButton)) return;
  if ((Event->pos() - Drag_start_position).manhattanLength() < QApplication::startDragDistance()) return;

  if (Selected_item && Selected_item->type()!=ItemType2) return;

  QDrag *Drag = new QDrag(this);
  QMimeData *Mime_data = new QMimeData;

  QString Text(Selected_item->data(0,1).toString());
  Mime_data->setText(Text);
  Drag->setMimeData(Mime_data);
  Drag->setPixmap(Icon);
  Drag->setHotSpot(QPoint(Icon.width()/2,Icon.height()/2));

  Drag->exec(Qt::CopyAction | Qt::MoveAction);
}
