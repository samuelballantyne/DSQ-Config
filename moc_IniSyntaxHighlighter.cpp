#include <QtCore/qobject.h>
#include <QtCore/qmetatype.h>
#include <cstring>
#include "IniSyntaxHighlighter.h"

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED

struct qt_meta_stringdata_IniSyntaxHighlighter_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IniSyntaxHighlighter_t, stringdata0) + ofs \
    - idx * sizeof(QByteArrayData)))
static const qt_meta_stringdata_IniSyntaxHighlighter_t qt_meta_stringdata_IniSyntaxHighlighter = {
    { QT_MOC_LITERAL(0, 0, 19) },
    "IniSyntaxHighlighter"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IniSyntaxHighlighter[] = {
    7,       // revision
    0,       // classname
    0,    0, // classinfo
    0,    0, // methods
    0,    0, // properties
    0,    0, // enums/sets
    0,    0, // constructors
    0,       // flags
    0        // signalCount
};

const QMetaObject IniSyntaxHighlighter::staticMetaObject = {
    { &QSyntaxHighlighter::staticMetaObject, qt_meta_stringdata_IniSyntaxHighlighter.data,
      qt_meta_data_IniSyntaxHighlighter, nullptr, nullptr, nullptr, nullptr }
};

const QMetaObject *IniSyntaxHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IniSyntaxHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!std::strcmp(_clname, qt_meta_stringdata_IniSyntaxHighlighter.stringdata0))
        return static_cast<void*>(this);
    return QSyntaxHighlighter::qt_metacast(_clname);
}

int IniSyntaxHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSyntaxHighlighter::qt_metacall(_c, _id, _a);
    return _id;
}

QT_WARNING_POP
QT_END_MOC_NAMESPACE
