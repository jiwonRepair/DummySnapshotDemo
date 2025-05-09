#ifndef RESETMANAGER_H
#define RESETMANAGER_H

#pragma once
#include <QObject>
#include <QVector>
#include "Resettable.h"

class ResetManager {
public:
    static void registerResettable(QObject* obj);
    static void resetAll();
    void resetOne(QObject* obj); // 이 줄 추가


private:
    static QVector<Resettable*> m_resettables;
};


#endif // RESETMANAGER_H
