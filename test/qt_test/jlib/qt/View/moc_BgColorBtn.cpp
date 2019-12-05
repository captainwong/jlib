/****************************************************************************
** Meta object code from reading C++ file 'BgColorBtn.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../jlib/qt/View/BgColorBtn.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BgColorBtn.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_jlib__qt__BgColorBtn_t {
    QByteArrayData data[5];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_jlib__qt__BgColorBtn_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_jlib__qt__BgColorBtn_t qt_meta_stringdata_jlib__qt__BgColorBtn = {
    {
QT_MOC_LITERAL(0, 0, 20), // "jlib::qt::BgColorBtn"
QT_MOC_LITERAL(1, 21, 11), // "sig_clicked"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 3), // "tag"
QT_MOC_LITERAL(4, 38, 12) // "slot_clicked"

    },
    "jlib::qt::BgColorBtn\0sig_clicked\0\0tag\0"
    "slot_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_jlib__qt__BgColorBtn[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   27,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void jlib::qt::BgColorBtn::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BgColorBtn *_t = static_cast<BgColorBtn *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sig_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->slot_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (BgColorBtn::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BgColorBtn::sig_clicked)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject jlib::qt::BgColorBtn::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_jlib__qt__BgColorBtn.data,
      qt_meta_data_jlib__qt__BgColorBtn,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *jlib::qt::BgColorBtn::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *jlib::qt::BgColorBtn::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_jlib__qt__BgColorBtn.stringdata0))
        return static_cast<void*>(this);
    return QPushButton::qt_metacast(_clname);
}

int jlib::qt::BgColorBtn::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void jlib::qt::BgColorBtn::sig_clicked(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
