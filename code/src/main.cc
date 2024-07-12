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


#include <QApplication>
#include <QWizardPage>
#include <QLabel>
#include <QVBoxLayout>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <fstream>

#include "window.h"


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

QWizardPage *createIntroPage()
{
  QWizardPage *page = new QWizardPage;
  page->setTitle("General information");

  page->setPixmap(QWizard::WatermarkPixmap, QPixmap("./splash.png"));

  QLabel *label = new QLabel("Most parts of this program and its associated data have been developed by Domingo Martín (dmartin@ugr.es)\n\nThe program can be used in non-commercial applications\n\nThis program is the complement of the article \"A survey of Digital Stippling\" by Domingo Martín, Germán Arroyo, Alex Rodríguez and Tobias Isenberg published in Computer & Graphics.\n\nPlease, in case that you use this software, add a reference to it");
  label->setWordWrap(true);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(label);
  page->setLayout(layout);

  return page;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int main(int argc, char *argv[])
{
  std::string File_name;


  QApplication Application(argc, argv);
  _window Window;
  //Window.read_settings();

  //QWizard wizard;
  //wizard.setOptions(QWizard::NoCancelButton | QWizard::NoBackButtonOnLastPage);
  //wizard.addPage(createIntroPage());
  //wizard.setWindowTitle("License");
  //wizard.show();
  //wizard.raise();

  setlocale(LC_NUMERIC, "C");

  QSurfaceFormat Format;
  Format.setDepthBufferSize(24);
  Format.setStencilBufferSize(8);
  QSurfaceFormat::setDefaultFormat(Format);

  QFont Font("Arial");
  Font.setPointSize(10);
  Application.setFont(Font);

  //QFontDatabase Database;
  //int Result = Database.addApplicationFont("fonts/Ubuntu-R.ttf");
  //QFont Font=QFont("Ubuntu",10);
  //Application.setFont(Font);

  Window.show();
  return Application.exec();
}
