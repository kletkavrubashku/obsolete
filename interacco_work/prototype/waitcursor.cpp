#include "waitcursor.h"
#include <QApplication>

WaitCursor::WaitCursor(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

WaitCursor::~WaitCursor(){
    QApplication::restoreOverrideCursor();
}

