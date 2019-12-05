/****************************************************************************
** Meta object code from reading C++ file 'LoadingView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../jlib/qt/View/LoadingView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LoadingView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_jlib__qt__LoadingView_t {
    QByteArrayData data[9];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_jlib__qt__LoadingView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_jlib__qt__LoadingView_t qt_meta_stringdata_jlib__qt__LoadingView = {
    {
QT_MOC_LITERAL(0, 0, 21), // "jlib::qt::LoadingView"
QT_MOC_LITERAL(1, 22, 10), // "slot_ready"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 3), // "tag"
QT_MOC_LITERAL(4, 38, 15), // "std::error_code"
QT_MOC_LITERAL(5, 54, 2), // "ec"
QT_MOC_LITERAL(6, 57, 13), // "slot_progress"
QT_MOC_LITERAL(7, 71, 24), // "jlib::qt::ThreadProgress"
QT_MOC_LITERAL(8, 96, 8) // "progress"

    },
    "jlib::qt::LoadingView\0slot_ready\0\0tag\0"
    "std::error_code\0ec\0slot_progress\0"
    "jlib::qt::ThreadProgress\0progress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_jlib__qt__LoadingView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x08 /* Private */,
       6,    2,   29,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 7,    3,    8,

       0        // eod
};

void jlib::qt::LoadingView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LoadingView *_t = static_cast<LoadingView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slot_ready((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< std::error_code(*)>(_a[2]))); break;
        case 1: _t->slot_progress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< jlib::qt::ThreadProgress(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< std::error_code >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< jlib::qt::ThreadProgress >(); break;
            }
            break;
        }
    }
}

const QMetaObject jlib::qt::LoadingView::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_jlib__qt__LoadingView.data,
      qt_meta_data_jlib__qt__LoadingView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *jlib::qt::LoadingView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *jlib::qt::LoadingView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_jlib__qt__LoadingView.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int jlib::qt::LoadingView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
