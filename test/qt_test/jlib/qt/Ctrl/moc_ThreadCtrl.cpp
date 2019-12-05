/****************************************************************************
** Meta object code from reading C++ file 'ThreadCtrl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../jlib/qt/Ctrl/ThreadCtrl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ThreadCtrl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_jlib__qt__ThreadCtrl_t {
    QByteArrayData data[9];
    char stringdata0[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_jlib__qt__ThreadCtrl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_jlib__qt__ThreadCtrl_t qt_meta_stringdata_jlib__qt__ThreadCtrl = {
    {
QT_MOC_LITERAL(0, 0, 20), // "jlib::qt::ThreadCtrl"
QT_MOC_LITERAL(1, 21, 9), // "sig_ready"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 3), // "tag"
QT_MOC_LITERAL(4, 36, 15), // "std::error_code"
QT_MOC_LITERAL(5, 52, 6), // "result"
QT_MOC_LITERAL(6, 59, 12), // "sig_progress"
QT_MOC_LITERAL(7, 72, 24), // "jlib::qt::ThreadProgress"
QT_MOC_LITERAL(8, 97, 8) // "progress"

    },
    "jlib::qt::ThreadCtrl\0sig_ready\0\0tag\0"
    "std::error_code\0result\0sig_progress\0"
    "jlib::qt::ThreadProgress\0progress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_jlib__qt__ThreadCtrl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x06 /* Public */,
       6,    2,   29,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 7,    3,    8,

       0        // eod
};

void jlib::qt::ThreadCtrl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ThreadCtrl *_t = static_cast<ThreadCtrl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sig_ready((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< std::error_code(*)>(_a[2]))); break;
        case 1: _t->sig_progress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< jlib::qt::ThreadProgress(*)>(_a[2]))); break;
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
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ThreadCtrl::*_t)(int , std::error_code );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ThreadCtrl::sig_ready)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ThreadCtrl::*_t)(int , jlib::qt::ThreadProgress );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ThreadCtrl::sig_progress)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject jlib::qt::ThreadCtrl::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_jlib__qt__ThreadCtrl.data,
      qt_meta_data_jlib__qt__ThreadCtrl,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *jlib::qt::ThreadCtrl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *jlib::qt::ThreadCtrl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_jlib__qt__ThreadCtrl.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int jlib::qt::ThreadCtrl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void jlib::qt::ThreadCtrl::sig_ready(int _t1, std::error_code _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void jlib::qt::ThreadCtrl::sig_progress(int _t1, jlib::qt::ThreadProgress _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
