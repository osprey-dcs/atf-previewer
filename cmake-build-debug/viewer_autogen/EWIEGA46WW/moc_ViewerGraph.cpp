/****************************************************************************
** Meta object code from reading C++ file 'ViewerGraph.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../ViewerGraph.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ViewerGraph.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ViewerGraph_t {
    QByteArrayData data[22];
    char stringdata0[180];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewerGraph_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewerGraph_t qt_meta_stringdata_ViewerGraph = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ViewerGraph"
QT_MOC_LITERAL(1, 12, 13), // "horizontalPan"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 2), // "id"
QT_MOC_LITERAL(4, 30, 11), // "curSigIndex"
QT_MOC_LITERAL(5, 42, 8), // "QString&"
QT_MOC_LITERAL(6, 51, 11), // "curFileName"
QT_MOC_LITERAL(7, 63, 4), // "minX"
QT_MOC_LITERAL(8, 68, 4), // "maxX"
QT_MOC_LITERAL(9, 73, 4), // "minY"
QT_MOC_LITERAL(10, 78, 4), // "maxY"
QT_MOC_LITERAL(11, 83, 11), // "verticalPan"
QT_MOC_LITERAL(12, 95, 5), // "scale"
QT_MOC_LITERAL(13, 101, 5), // "reset"
QT_MOC_LITERAL(14, 107, 15), // "rubberBandScale"
QT_MOC_LITERAL(15, 123, 8), // "prevView"
QT_MOC_LITERAL(16, 132, 8), // "mousePos"
QT_MOC_LITERAL(17, 141, 6), // "mouseX"
QT_MOC_LITERAL(18, 148, 6), // "mouseY"
QT_MOC_LITERAL(19, 155, 13), // "performAction"
QT_MOC_LITERAL(20, 169, 8), // "QAction*"
QT_MOC_LITERAL(21, 178, 1) // "a"

    },
    "ViewerGraph\0horizontalPan\0\0id\0curSigIndex\0"
    "QString&\0curFileName\0minX\0maxX\0minY\0"
    "maxY\0verticalPan\0scale\0reset\0"
    "rubberBandScale\0prevView\0mousePos\0"
    "mouseX\0mouseY\0performAction\0QAction*\0"
    "a"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewerGraph[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    7,   54,    2, 0x06 /* Public */,
      11,    7,   69,    2, 0x06 /* Public */,
      12,    7,   84,    2, 0x06 /* Public */,
      13,    3,   99,    2, 0x06 /* Public */,
      14,    3,  106,    2, 0x06 /* Public */,
      15,    3,  113,    2, 0x06 /* Public */,
      16,    3,  120,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      19,    1,  127,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 5, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double,    3,    4,    6,    7,    8,    9,   10,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 5, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double,    3,    4,    6,    7,    8,    9,   10,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 5, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double,    3,    4,    6,    7,    8,    9,   10,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 5,    3,    4,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 5,    3,    4,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 5,    3,    4,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double,    3,   17,   18,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 20,   21,

       0        // eod
};

void ViewerGraph::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ViewerGraph *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->horizontalPan((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4])),(*reinterpret_cast< double(*)>(_a[5])),(*reinterpret_cast< double(*)>(_a[6])),(*reinterpret_cast< double(*)>(_a[7]))); break;
        case 1: _t->verticalPan((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4])),(*reinterpret_cast< double(*)>(_a[5])),(*reinterpret_cast< double(*)>(_a[6])),(*reinterpret_cast< double(*)>(_a[7]))); break;
        case 2: _t->scale((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4])),(*reinterpret_cast< double(*)>(_a[5])),(*reinterpret_cast< double(*)>(_a[6])),(*reinterpret_cast< double(*)>(_a[7]))); break;
        case 3: _t->reset((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 4: _t->rubberBandScale((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 5: _t->prevView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 6: _t->mousePos((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 7: _t->performAction((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ViewerGraph::*)(int , int , QString & , double , double , double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewerGraph::horizontalPan)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ViewerGraph::*)(int , int , QString & , double , double , double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewerGraph::verticalPan)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ViewerGraph::*)(int , int , QString & , double , double , double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewerGraph::scale)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ViewerGraph::*)(int , int , QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewerGraph::reset)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ViewerGraph::*)(int , int , QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewerGraph::rubberBandScale)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ViewerGraph::*)(int , int , QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewerGraph::prevView)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ViewerGraph::*)(int , double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewerGraph::mousePos)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ViewerGraph::staticMetaObject = { {
    QMetaObject::SuperData::link<QtCharts::QChartView::staticMetaObject>(),
    qt_meta_stringdata_ViewerGraph.data,
    qt_meta_data_ViewerGraph,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ViewerGraph::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewerGraph::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ViewerGraph.stringdata0))
        return static_cast<void*>(this);
    return QtCharts::QChartView::qt_metacast(_clname);
}

int ViewerGraph::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtCharts::QChartView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ViewerGraph::horizontalPan(int _t1, int _t2, QString & _t3, double _t4, double _t5, double _t6, double _t7)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ViewerGraph::verticalPan(int _t1, int _t2, QString & _t3, double _t4, double _t5, double _t6, double _t7)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ViewerGraph::scale(int _t1, int _t2, QString & _t3, double _t4, double _t5, double _t6, double _t7)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ViewerGraph::reset(int _t1, int _t2, QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ViewerGraph::rubberBandScale(int _t1, int _t2, QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ViewerGraph::prevView(int _t1, int _t2, QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ViewerGraph::mousePos(int _t1, double _t2, double _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
