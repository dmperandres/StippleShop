/****************************************************************************
** Meta object code from reading C++ file 'filter_halftoning_acsp.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "filter_halftoning_acsp.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filter_halftoning_acsp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata__qtw_filter_halftoning_acsp_t {
    const uint offsetsAndSize[14];
    char stringdata0[115];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata__qtw_filter_halftoning_acsp_t, stringdata0) + ofs), len 
static const qt_meta_stringdata__qtw_filter_halftoning_acsp_t qt_meta_stringdata__qtw_filter_halftoning_acsp = {
    {
QT_MOC_LITERAL(0, 27), // "_qtw_filter_halftoning_acsp"
QT_MOC_LITERAL(28, 19), // "set_parameter1_slot"
QT_MOC_LITERAL(48, 0), // ""
QT_MOC_LITERAL(49, 5), // "Value"
QT_MOC_LITERAL(55, 19), // "set_parameter2_slot"
QT_MOC_LITERAL(75, 19), // "set_parameter3_slot"
QT_MOC_LITERAL(95, 19) // "set_parameter4_slot"

    },
    "_qtw_filter_halftoning_acsp\0"
    "set_parameter1_slot\0\0Value\0"
    "set_parameter2_slot\0set_parameter3_slot\0"
    "set_parameter4_slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data__qtw_filter_halftoning_acsp[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x09,    1 /* Protected */,
       4,    1,   41,    2, 0x09,    3 /* Protected */,
       5,    1,   44,    2, 0x09,    5 /* Protected */,
       6,    1,   47,    2, 0x09,    7 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void _qtw_filter_halftoning_acsp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<_qtw_filter_halftoning_acsp *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->set_parameter1_slot((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->set_parameter2_slot((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->set_parameter3_slot((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->set_parameter4_slot((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject _qtw_filter_halftoning_acsp::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata__qtw_filter_halftoning_acsp.offsetsAndSize,
    qt_meta_data__qtw_filter_halftoning_acsp,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata__qtw_filter_halftoning_acsp_t
, QtPrivate::TypeAndForceComplete<_qtw_filter_halftoning_acsp, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>


>,
    nullptr
} };


const QMetaObject *_qtw_filter_halftoning_acsp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *_qtw_filter_halftoning_acsp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata__qtw_filter_halftoning_acsp.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int _qtw_filter_halftoning_acsp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
