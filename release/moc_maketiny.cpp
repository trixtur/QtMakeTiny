/****************************************************************************
** Meta object code from reading C++ file 'maketiny.h'
**
** Created: Tue Dec 21 16:09:07 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../maketiny.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'maketiny.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MakeTiny[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x08,
      18,    9,    9,    9, 0x08,
      43,    9,    9,    9, 0x08,
      71,    9,    9,    9, 0x08,
      81,    9,    9,    9, 0x08,
      97,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MakeTiny[] = {
    "MakeTiny\0\0MkTny()\0PageData(QNetworkReply*)\0"
    "CallReverse(QNetworkReply*)\0Reverse()\0"
    "FrameLoad(bool)\0CopyToClip()\0"
};

const QMetaObject MakeTiny::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MakeTiny,
      qt_meta_data_MakeTiny, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MakeTiny::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MakeTiny::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MakeTiny::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MakeTiny))
        return static_cast<void*>(const_cast< MakeTiny*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MakeTiny::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: MkTny(); break;
        case 1: PageData((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 2: CallReverse((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 3: Reverse(); break;
        case 4: FrameLoad((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: CopyToClip(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
