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


#ifndef WINDOW
#define WINDOW

#include <QWidget>
#include <QSizePolicy>
#include <QSettings>
#include <QFileInfo>
#include <QDateTime>
#include <QMainWindow>
#include <QFrame>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QTimer>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QRect>
#include <QScreen>

#include <string>
#include <map>
#include "vertex.h"

#include "nodes_editor.h"

#include "tree_widget.h"
#include "tree_widget_item.h"

#include "graphics_view.h"
#include "graphics_scene.h"

class _gl_widget;

typedef enum{SELECTION_MODE_RUBBER_BOX,SELECTION_MODE_BOX,SELECTION_MODE_CIRCLE} _selection_mode;
typedef enum{SELECTION_STATE_START,SELECTION_STATE_STOP,SELECTION_STATE_NOTHING} _selection_state;
typedef enum{MODE_MOUSE_SELECTION,MODE_MOUSE_DELETE,MODE_MOUSE_NOTHING} _mode_mouse;
typedef enum{MODE_CURSOR_ARROW,MODE_CURSOR_CROSS,MODE_CURSOR_MOVE,MODE_CURSOR_LAST} _mode_cursor;

const int MIN_WIN_WIDTH=1100;
const int MIN_WIN_HEIGHT=900;

namespace _window_ns {
  const float SCREEN_WIDTH_FACTOR=0.5;
  const float SCREEN_HEIGHT_FACTOR=0.9;

  const int WIDTH_TAB_CHARS=20;

  const QColor Stippling_separator_color=QColor(255,220,220);
  const QColor Halftoning_separator_color=QColor(200,255,220);
  const QColor Dot_separator_color=QColor(200,220,255);
  const QColor Contrast_separator_color=QColor(220,255,255);
  const QColor Edges_separator_color=QColor(255,220,255);
  const QColor Blur_separator_color=QColor(255,255,220);
  const QColor Effect_separator_color=QColor(240,210,210);
  const QColor Combination_separator_color=QColor(210,240,210);
  const QColor Measure_separator_color=QColor(210,210,240);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _window : public QMainWindow
{
Q_OBJECT

public:
  // relates the name of the filter with the parameters of the filter, which can be read from a file or initialized from editor
  std::map<std::string,std::map<std::string,std::string>> Map_filters_parameters;

  typedef enum {SAVE_RESOLUTION_300PPI,SAVE_RESOLUTION_1200PPI, LAST_SAVE_RESOLUTION} _save_resolution;

  _window();

  void read_settings();
  void write_settings();
  void set_frame_warning(bool Estate);
  void set_tab(int Pos){Tab_widget->setCurrentIndex(Pos);};

  void add_filter(int Filter_pos);
  void add_filter(int Filter_pos,float Col,float Row);
  void description(int Filter_pos);

protected:
  void closeEvent(QCloseEvent *event);

  void create_filter_list();

private slots:
  void open_file_slot();
  bool save_file_slot();
  void new_effect_slot();
  void open_effect_file_slot();
  void save_effect_file_slot();

  void tabbar_clicked_slot(int Index1);
  void filter_selected_slot(QTreeWidgetItem * item, int column);

private:
  _gl_widget *GL_widget;
  QTabWidget *Main_tab_widget;
  QTabWidget *Tab_widget;
  QTimer *Timer;
  QDateTime File_time;

  QWidget *Edit_widget;
  QWidget *Result_widget;

  QTreeWidget *Filters_list;

  QHBoxLayout *Edit_horizontal_frame;

  QFrame *Framed_widget;

  _nodes_editor *Nodes_editor;

  _graphics_view *View;
  _graphics_scene *Scene;

  std::string File_name;
  std::string Effect_name;
  bool Link_file;

  QAction *Open_file;
  QAction *Save_file;

  QAction *New_effect;

  QAction *Open_effect_file;
  QAction *Save_effect_file;

  QAction *Save_svg_file;

  int Previous_tab=0;

  _f_filter_ns::_filter_parameters Filter_parameters;
};

#endif
