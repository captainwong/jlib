/****************************************************************************
** Meta object code from reading C++ file 'PageTitle.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../jlib/qt/View/PageTitle.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PageTitle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_jlib__qt__PageTitle_t {
    QByteArrayData data[8];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_jlib__qt__PageTitle_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_jlib__qt__PageTitle_t qt_meta_stringdata_jlib__qt__PageTitle = {
    {
QT_MOC_LITERAL(0, 0, 19), // "jlib::qt::PageTitle"
QT_MOC_LITERAL(1, 20, 9), // "sig_close"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 13), // "sig_maximized"
QT_MOC_LITERAL(4, 45, 5), // "isMax"
QT_MOC_LITERAL(5, 51, 13), // "slot_minimize"
QT_MOC_LITERAL(6, 65, 24), // "slot_maximize_or_restore"
QT_MOC_LITERAL(7, 90, 10) // "slot_close"

    },
    "jlib::qt::PageTitle\0sig_close\0\0"
    "sig_maximized\0isMax\0slot_minimize\0"
    "slot_maximize_or_restore\0slot_close"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_jlib__qt__PageTitle[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    1,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   43,    2, 0x08 /* Private */,
       6,    0,   44,    2, 0x08 /* Private */,
       7,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void jlib::qt::PageTitle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PageTitle *_t = static_cast<PageTitle *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sig_close(); break;
        case 1: _t->sig_maximized((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->slot_minimize(); break;
        case 3: _t->slot_maximize_or_restore(); break;
        case 4: _t->slot_close(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (PageTitle::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PageTitle::sig_close)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (PageTitle::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PageTitle::sig_maximized)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject jlib::qt::PageTitle::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_jlib__qt__PageTitle.data,
      qt_meta_data_jlib__qt__PageTitle,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *jlib::qt::PageTitle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *jlib::qt::PageTitle::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_jlib__qt__PageTitle.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int jlib::qt::PageTitle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void jlib::qt::PageTitle::sig_close()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void jlib::qt::PageTitle::sig_maximized(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
