#include "apimoduleauthorize.h"
#include "apimodulerequestvalidator.h"
#include "boolvalidator.h"
#include <intvalidator.h>
#include <jsonparamvalidator.h>
#include <qjsondocument.h>
#include <regexvalidator.h>
#include <QFile>
#include <scpi.h>

namespace APIMODULE
{
cApiModuleAuthorize::cApiModuleAuthorize(cApiModule *module): cModuleActivist(module->getVeinModuleName()), m_module(module)
{
}

void cApiModuleAuthorize::generateVeinInterface()
{
    m_pRequestStatusAct = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                QString("ACT_RequestStatus"),
                                                QString("Flag state for request status: 1=running, 2=accepted"),
                                                QVariant((int)0));
    m_pRequestStatusAct->setScpiInfo("AUTH", "REQUESTSTATE", SCPI::isQuery, m_pRequestStatusAct->getName());

    m_pPendingRequestPar = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                 QString("PAR_PendingRequest"),
                                                 QString("Json structure with name(string), tokenType(string->Certificate,Basic,PublicKey), token(string)"),
                                                 QJsonObject());
    m_pPendingRequestPar->setValidator(new cApiModuleRequestValidator());
    m_pPendingRequestPar->setScpiInfo("AUTH", "PENDINGREQUEST", SCPI::isQuery | SCPI::isCmdwP, m_pPendingRequestPar->getName());
    m_module->m_veinModuleParameterMap[m_pPendingRequestPar->getName()] = m_pPendingRequestPar;

    m_pPendingRequestAct = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                 QString("ACT_PendingRequest"),
                                                 QString("Json structure with name(string), tokenType(string->Certificate,Basic,PublicKey), token(string)"),
                                                 QJsonObject());
    m_pPendingRequestAct->setScpiInfo("AUTH", "PENDINGREQUEST", SCPI::isQuery, m_pPendingRequestPar->getName());

    m_pTrustListAct = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                QString("ACT_TrustList"),
                                                QString("Json array of trusted clients"),
                                                QJsonArray());
    m_pTrustListAct->setScpiInfo("AUTH", "TRUSTLIST", SCPI::isQuery, m_pTrustListAct->getName());

    m_pTrustListChangeCountAct = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                            QString("ACT_TLChangeCount"),
                                            QString("Change count of Trust List"),
                                            QVariant((int)0));
    m_pTrustListChangeCountAct->setScpiInfo("AUTH", "TLCHANGECOUNT", SCPI::isQuery, m_pTrustListChangeCountAct->getName());

    m_pGuiDialogFinished = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                 QString("PAR_GuiDialogFinished"),
                                                 QString("boolean if GUI dialog finished"),
                                                 QVariant(false));
    m_pGuiDialogFinished->setValidator(new cBoolValidator());
    m_pGuiDialogFinished->setScpiInfo("AUTH", "GUIDIALOGFINISHED", SCPI::isQuery | SCPI::isCmdwP, m_pPendingRequestPar->getName());
    m_module->m_veinModuleParameterMap[m_pPendingRequestPar->getName()] = m_pGuiDialogFinished;

    connect(m_pPendingRequestPar, &VfModuleParameter::sigValueChanged, this, &cApiModuleAuthorize::onNewPendingRequest);
    connect(m_pGuiDialogFinished, &VfModuleParameter::sigValueChanged, this, &cApiModuleAuthorize::onGuiDialogFinished);
}

void cApiModuleAuthorize::activate()
{
    emit m_module->activationContinue();
}

void cApiModuleAuthorize::deactivate()
{
    emit m_module->deactivationContinue();
}

QJsonArray cApiModuleAuthorize::readTrustList()
{
    QFile file(m_trustListPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return QJsonArray(); // Return empty array on error
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return QJsonArray();
    }
    return doc.array(); // Return the parsed array
}

bool cApiModuleAuthorize::jsonArrayContains(const QJsonArray &array, const QJsonObject &target)
{
    for (const QJsonValue& value : array) {
        if (value.isObject() && value.toObject() == target) {
            return true;
        }
    }
    return false;
}

void cApiModuleAuthorize::onNewPendingRequest(QVariant pendingRequest)
{
    if(m_pRequestStatusAct->getValue() != 1) {
        m_pRequestStatusAct->setValue(1);
        m_pGuiDialogFinished->setValue(false);
        m_pPendingRequestAct->setValue(pendingRequest.toJsonObject());
    }
}

void cApiModuleAuthorize::onGuiDialogFinished(QVariant dialogFinished)
{
    if(dialogFinished == true)
    {
        QJsonArray readList = readTrustList();
        if(jsonArrayContains(readList, m_pRequestStatusAct->getValue().toJsonObject())){
            m_pTrustListAct->setValue(readList);
            m_pTrustListChangeCountAct->setValue(m_pTrustListChangeCountAct->getValue().toInt() + 1);
            m_pRequestStatusAct->setValue(2);
        }
        else
        {
            m_pRequestStatusAct->setValue(0);
        }
    }
}

}
