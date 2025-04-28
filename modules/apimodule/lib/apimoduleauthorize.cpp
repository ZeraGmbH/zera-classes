#include "apimoduleauthorize.h"
#include "apimodulerequestvalidator.h"
#include <intvalidator.h>
#include <jsonparamvalidator.h>
#include <regexvalidator.h>
#include <scpi.h>

namespace APIMODULE
{
cApiModuleAuthorize::cApiModuleAuthorize(cApiModule *module): cModuleActivist(module->getVeinModuleName()), m_module(module)
{
}

void cApiModuleAuthorize::generateVeinInterface()
{
    // tbd actpendingrequest
    m_pRequestStatusAct = new VfModuleComponent(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                QString("ACT_RequestStatus"),
                                                QString("Flag state for request status: 1=running, 2=accepted"),
                                                QVariant((int)0));
    m_pRequestStatusAct->setScpiInfo("AUTH", "REQUESTSTATE", SCPI::isQuery, m_pRequestStatusAct->getName());

    m_pPendingRequestPar = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                 QString("PAR_PendingRequest"),
                                                 QString("Json structure with name(string), type(string->Certificate,Basic,PublicKey), token(string)"),
                                                 QJsonObject());
    m_pPendingRequestPar->setValidator(new cApiModuleRequestValidator());
    m_pPendingRequestPar->setScpiInfo("AUTH", "PENDINGREQUEST", SCPI::isQuery | SCPI::isCmdwP, m_pPendingRequestPar->getName());
    m_module->m_veinModuleParameterMap[m_pPendingRequestPar->getName()] = m_pPendingRequestPar;

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
}

void cApiModuleAuthorize::activate()
{
    emit m_module->activationContinue();
}

void cApiModuleAuthorize::deactivate()
{
    emit m_module->deactivationContinue();
}

}
