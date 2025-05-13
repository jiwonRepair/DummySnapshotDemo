#include "Dummy.h"

Dummy::Dummy(QObject* parent) : QObject(parent) {}

void Dummy::setInputs(int a, int b) {
    a_ = a;
    b_ = b;
}

void Dummy::performOperation() {
    int result = a_ + b_;
    state_ = (result == 2) ? "OK" : "FAIL";
    emit stateChanged();

    if (state_ != "OK") {
        QByteArray meta = sm.buildMeta(__func__, "a + b", 2, result, state_);
        sm.capture("Dummy", this, snapshot(), meta);
        this->reset();
    }
}

void Dummy::performOperationGeneralized(const QJsonObject& input,
                                        const QString& operation,
                                        const QVariant& expected,
                                        const QVariant& actual)
{
    // 🔹 1. 입력 복원
    if (input.contains("a")) a_ = input["a"].toInt();
    if (input.contains("b")) b_ = input["b"].toInt();

    // 🔹 2. 출력 저장
    output_ = actual.toString();

    // 🔹 3. 상태 판단
    state_ = (actual == expected) ? "OK" : "FAIL";
    emit stateChanged();

    if (state_ != "OK") {
        QJsonObject meta;
        meta["status"] = state_;
        meta["operation"] = operation;
        meta["expected"] = QJsonValue::fromVariant(expected);
        meta["actual"] = QJsonValue::fromVariant(actual);
        meta["reason"] = QString("Computation failed: expected %1 but got %2")
                             .arg(expected.toString(), actual.toString());
        meta["function"] = __func__;
        meta["input"] = input;

        sm.capture("Dummy", this, snapshot(), QJsonDocument(meta).toJson(QJsonDocument::Compact));
        this->reset();
    }
}

void Dummy::reset() {
    if (!sm.restore().isEmpty()) {
        sm.restoreToTarget();                 // state_는 여기서 복원됨
        resetReason_ = "SnapshotRestored";    // 복원 경로만 표시
        emit resetReasonChanged();
    } else {
        a_ = 0;
        b_ = 0;
        state_ = "ResetToDefault";            // 수동 초기화이므로 명시
        resetReason_ = "DefaultReset";
        emit stateChanged();                  // 이 경우엔 직접 호출
        emit resetReasonChanged();
    }

    emit resetInvoked();                      // QML 감지용
}



void Dummy::loadSnapshot(const QByteArray& data) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();
    if (obj.contains("input")) {
        QJsonObject input = obj["input"].toObject();
        a_ = input["a"].toInt();
        b_ = input["b"].toInt();
    }
    if (obj.contains("result")) {
        state_ = obj["result"].toObject()["state"].toString();
        emit stateChanged();
    }
}

QByteArray Dummy::snapshot() const {
    QJsonObject obj;
    obj["resource"] = "Dummy";
    obj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    obj["context"] = "";

    QJsonObject input;
    input["a"] = a_;
    input["b"] = b_;
    obj["input"] = input;

    QJsonObject result;
    result["state"] = state_;
    obj["result"] = result;

    return QJsonDocument(obj).toJson();
}

QString Dummy::state() const {
    return state_;
}

QString Dummy::resetReason() const {
    return resetReason_;
}

