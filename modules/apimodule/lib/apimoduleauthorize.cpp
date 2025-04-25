#include "apimoduleauthorize.h"
#include "apimodulerequestvalidator.h"
#include <intvalidator.h>
#include <jsonparamvalidator.h>
#include <regexvalidator.h>
#include <scpi.h>

namespace APIMODULE
{
cApiModuleAuthorize::cApiModuleAuthorize(cApiModule *module): cModuleActivist(module->getVeinModuleName()), m_module(module) {}

void cApiModuleAuthorize::generateVeinInterface()
{
    m_pRequestStatusAct = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                QString("ACT_RequestStatus"),
                                                QString("Flag state for request status: 1=running, 2=accepted"),
                                                QVariant((int)0));
    m_pRequestStatusAct->setValidator(new cIntValidator(0,3,1));
    m_pRequestStatusAct->setScpiInfo("AUTH", "REQUESTSTATE", SCPI::isQuery | SCPI::isCmdwP, m_pRequestStatusAct->getName());
    m_module->m_veinModuleParameterMap[m_pRequestStatusAct->getName()] = m_pRequestStatusAct;

    m_pPendingRequestPar = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                 QString("PAR_PendingRequest"),
                                                 QString("Json structure with Name(string), Type(string->Certificate,Basic,PublicKey), Token(string)"),
                                                 QJsonObject());
    m_pPendingRequestPar->setValidator(new cRegExValidator(".*"));
    m_pPendingRequestPar->setScpiInfo("AUTH", "PENDINGREQUEST", SCPI::isQuery | SCPI::isCmdwP, m_pPendingRequestPar->getName());
    m_module->m_veinModuleParameterMap[m_pPendingRequestPar->getName()] = m_pPendingRequestPar;

    m_pTrustListAct = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                QString("ACT_TrustList"),
                                                QString("Json array of trusted clients"),
                                                QJsonObject());
    m_pTrustListAct->setValidator(new cJsonParamValidator());
    m_pTrustListAct->setScpiInfo("AUTH", "TRUSTLIST", SCPI::isQuery, m_pTrustListAct->getName());
    m_module->m_veinModuleParameterMap[m_pTrustListAct->getName()] = m_pTrustListAct;

    m_pTrustListChangeCountAct = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                            QString("ACT_TLChangeCount"),
                                            QString("Change count of Trust List"),
                                            QVariant((int)0));
    m_pTrustListChangeCountAct->setValidator(new cIntValidator(0,INT_MAX,1));
    m_pTrustListChangeCountAct->setScpiInfo("AUTH", "TLCHANGECOUNT", SCPI::isQuery, m_pTrustListChangeCountAct->getName());
    m_module->m_veinModuleParameterMap[m_pTrustListChangeCountAct->getName()] = m_pTrustListChangeCountAct;
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
