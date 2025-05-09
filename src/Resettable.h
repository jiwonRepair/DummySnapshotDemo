#pragma once
#include <QObject>

class Resettable : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;
    virtual void reset() = 0;
    virtual QByteArray snapshot() const = 0;
    virtual void loadSnapshot(const QByteArray&) = 0;
};
