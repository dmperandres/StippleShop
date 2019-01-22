#ifndef LINE_EDIT_H
#define LINE_EDIT_H

#include <QLineEdit>
#include <QKeyEvent>

class _line_edit : public QLineEdit
{
Q_OBJECT

public:
  _line_edit();

signals:
//void tabPressed();

protected:
//void focusOutEvent(QFocusEvent *Event);
//bool event(QEvent *Event);
//void keyPressEvent(QKeyEvent *Event);

};

#endif // LINE_EDIT_H
