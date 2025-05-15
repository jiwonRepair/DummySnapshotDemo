#include "Dummy.h"

Dummy::Dummy(SnapshotManager* sharedSnapshot, QObject* parent)
    : QObject(parent), sm(sharedSnapshot) {}

void Dummy::setInputs(int a, int b) {
    a_ = a;
    b_ = b;
}

void Dummy::performOperation() {
    int result = a_ + b_;
    state_ = (result == 2) ? "OK" : "FAIL";
    emit stateChanged();

    if (state_ != "OK") {
        QByteArray meta = sm->buildMeta(__func__, "a + b", 2, result, state_);
        sm->capture("Dummy", this, snapshot(), meta);
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

    // 🔹 2. 상태 판단
    bool isOk = (actual == expected);
    state_ = isOk ? "OK" : "FAIL";

    if (isOk) {
        // ✅ 성공한 경우에만 출력 저장 및 QML 반영
        output_ = actual.toString();
        emit stateChanged();
    } else {
        // ❌ 실패한 경우 출력은 저장하지 않고 snapshot 복원에 맡김
        QJsonObject meta;
        meta["status"] = state_;
        meta["operation"] = operation;
        meta["expected"] = QJsonValue::fromVariant(expected);
        meta["actual"] = QJsonValue::fromVariant(actual);
        meta["reason"] = QString("Computation failed: expected %1 but got %2")
                             .arg(expected.toString(), actual.toString());
        meta["function"] = __func__;
        meta["input"] = input;

        sm->capture("Dummy", this, snapshot(), QJsonDocument(meta).toJson(QJsonDocument::Compact));
        reset();  // ✅ snapshot에서 이전 output_ 복원됨
    }
}


void Dummy::reset() {
    if (!sm->getSnapshotList().isEmpty()) {
        sm->restoreLastSnapshot();                     // ✅ 마지막 스냅샷 복원              // state_는 여기서 복원됨
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
        QJsonObject result = obj["result"].toObject();
        state_ = result["state"].toString();
        output_ = result["output"].toString();  // ✅ 출력 복원
        //emit stateChanged();  // output이 Q_PROPERTY로 바인딩되었으므로 QML에 반영됨
    }

    // ✅ 여기서 직접 QML 입력란에 값 주입
    if (inputAField)
        inputAField->setProperty("text", QString::number(a_));
    if (inputBField)
        inputBField->setProperty("text", QString::number(b_));

    emit stateChanged();    // ✅ QML 반영 (state + output)
    emit resetInvoked();    // ✅ 로그 찍히게
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
    result["output"] = output_;
    obj["result"] = result;

    return QJsonDocument(obj).toJson();
}

QString Dummy::state() const {
    return state_;
}

QString Dummy::resetReason() const {
    return resetReason_;
}

QString Dummy::output() const {
    return output_;
}

void Dummy::registerTextFields(QObject* a, QObject* b) {
    inputAField = a;
    inputBField = b;
}

