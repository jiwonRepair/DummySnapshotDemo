#include "ResetManager.h"
#include <QPointer>
#include <QDebug>

QVector<Resettable*> ResetManager::m_resettables;

void ResetManager::registerResettable(QObject* obj) {
    auto* r = qobject_cast<Resettable*>(obj);
    if (r && !m_resettables.contains(r)) {
        m_resettables.append(r);
    }
}

void ResetManager::resetAll() {
    const auto copy = m_resettables;
    for (auto* r : copy) {
        if (r) r->reset();
    }
}

void ResetManager::resetOne(QObject* obj) {
    auto* r = qobject_cast<Resettable*>(obj);
    if (r) {
        r->reset();
    }
}

