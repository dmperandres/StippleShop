/****************************************************************************
** Meta object code from reading C++ file 'window.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata__window_t {
    const uint offsetsAndSize[26];
    char stringdata0[176];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata__window_t, stringdata0) + ofs), len 
static const qt_meta_stringdata__window_t qt_meta_stringdata__window = {
    {
QT_MOC_LITERAL(0, 7), // "_window"
QT_MOC_LITERAL(8, 14), // "open_file_slot"
QT_MOC_LITERAL(23, 0), // ""
QT_MOC_LITERAL(24, 14), // "save_file_slot"
QT_MOC_LITERAL(39, 15), // "new_effect_slot"
QT_MOC_LITERAL(55, 21), // "open_effect_file_slot"
QT_MOC_LITERAL(77, 21), // "save_effect_file_slot"
QT_MOC_LITERAL(99, 19), // "tabbar_clicked_slot"
QT_MOC_LITERAL(119, 6), // "Index1"
QT_MOC_LITERAL(126, 20), // "filter_selected_slot"
QT_MOC_LITERAL(147, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(164, 4), // "item"
QT_MOC_LITERAL(169, 6) // "column"

    },
    "_window\0open_file_slot\0\0save_file_slot\0"
    "new_effect_slot\0open_effect_file_slot\0"
    "save_effect_file_slot\0tabbar_clicked_slot\0"
    "Index1\0filter_selected_slot\0"
    "QTreeWidgetItem*\0item\0column"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data__window[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x08,    1 /* Private */,
       3,    0,   57,    2, 0x08,    2 /* Private */,
       4,    0,   58,    2, 0x08,    3 /* Private */,
       5,    0,   59,    2, 0x08,    4 /* Private */,
       6,    0,   60,    2, 0x08,    5 /* Private */,
       7,    1,   61,    2, 0x08,    6 /* Private */,
       9,    2,   64,    2, 0x08,    8 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Int,   11,   12,

       0        // eod
};

void _window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<_window *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->open_file_slot(); break;
        case 1: { bool _r = _t->save_file_slot();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->new_effect_slot(); break;
        case 3: _t->open_effect_file_slot(); break;
        case 4: _t->save_effect_file_slot(); break;
        case 5: _t->tabbar_clicked_slot((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->filter_selected_slot((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject _window::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata__window.offsetsAndSize,
    qt_meta_data__window,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata__window_t
, QtPrivate::TypeAndForceComplete<_window, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>


>,
    nullptr
} };


const QMetaObject *_window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *_window::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata__window.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int _window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
