/****************************************************************************
** Meta object code from reading C++ file 'IconBtn.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../jlib/qt/View/IconBtn.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'IconBtn.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_jlib__qt__IconBtn_t {
    QByteArrayData data[8];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_jlib__qt__IconBtn_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_jlib__qt__IconBtn_t qt_meta_stringdata_jlib__qt__IconBtn = {
    {
QT_MOC_LITERAL(0, 0, 17), // "jlib::qt::IconBtn"
QT_MOC_LITERAL(1, 18, 7), // "clicked"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 18), // "long_press_trigger"
QT_MOC_LITERAL(4, 46, 8), // "is_press"
QT_MOC_LITERAL(5, 55, 15), // "sig_mouse_enter"
QT_MOC_LITERAL(6, 71, 15), // "sig_mouse_leave"
QT_MOC_LITERAL(7, 87, 23) // "slot_long_press_timeout"

    },
    "jlib::qt::IconBtn\0clicked\0\0"
    "long_press_trigger\0is_press\0sig_mouse_enter\0"
    "sig_mouse_leave\0slot_long_press_timeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_jlib__qt__IconBtn[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    1,   40,    2, 0x06 /* Public */,
       5,    0,   43,    2, 0x06 /* Public */,
       6,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void jlib::qt::IconBtn::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IconBtn *_t = static_cast<IconBtn *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clicked(); break;
        case 1: _t->long_press_trigger((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->sig_mouse_enter(); break;
        case 3: _t->sig_mouse_leave(); break;
        case 4: _t->slot_long_press_timeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (IconBtn::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&IconBtn::clicked)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (IconBtn::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&IconBtn::long_press_trigger)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (IconBtn::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&IconBtn::sig_mouse_enter)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (IconBtn::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&IconBtn::sig_mouse_leave)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject jlib::qt::IconBtn::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_jlib__qt__IconBtn.data,
      qt_meta_data_jlib__qt__IconBtn,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *jlib::qt::IconBtn::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *jlib::qt::IconBtn::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_jlib__qt__IconBtn.stringdata0))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int jlib::qt::IconBtn::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void jlib::qt::IconBtn::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void jlib::qt::IconBtn::long_press_trigger(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void jlib::qt::IconBtn::sig_mouse_enter()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void jlib::qt::IconBtn::sig_mouse_leave()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
