#include "apimoduleauthorize.h"
#include "apimodulerequestvalidator.h"
#include "boolvalidator.h"
#include <intvalidator.h>
#include <jsonparamvalidator.h>
#include <qjsondocument.h>
#include <regexvalidator.h>
#include <QFile>
#include <QUuid>
#include <scpi.h>
#include <vcmp_remoteproceduredata.h>

namespace APIMODULE
{
cApiModuleAuthorize::cApiModuleAuthorize(cApiModule *module, QString persistencyBasePath):
        cModuleActivist(module->getVeinModuleName()),
        m_trustListPath(persistencyBasePath + "/opt/websam-vein-api/authorize/trustlist.json"),
        m_module(module),
        m_bDialogIsOpen(false)
    {
        // [TBD] common m_trustListPath for module and UI, BETTER SOLUTION (to be found, discussion with AM): only module handles the file
    }

    void cApiModuleAuthorize::generateVeinInterface()
    {
        m_spPendingRequestAct = std::make_shared<VfModuleComponent>(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                     QString("ACT_PendingRequest"),
                                                     QString("Json structure with name(string), tokenType(string->Certificate,Basic,PublicKey), token(string)"),
                                                     QJsonObject());

        m_spTrustListAct = std::make_shared<VfModuleComponent>(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                    QString("ACT_TrustList"),
                                                    QString("Json array of trusted clients"),
                                                    readTrustList());

        m_spTrustListChangeCountAct = std::make_shared<VfModuleComponent>(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                QString("ACT_TLChangeCount"),
                                                QString("Change count of Trust List"),
                                                (int)0);

        m_pGuiDialogFinished = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                     QString("PAR_GuiDialogFinished"),
                                                     QString("boolean if GUI dialog finished"),
                                                     false);
        m_pGuiDialogFinished->setValidator(new cBoolValidator());
        m_module->m_veinModuleParameterMap[m_pGuiDialogFinished->getComponentName()] = m_pGuiDialogFinished;

        m_pReloadTrustList = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                     QString("PAR_ReloadTrustList"),
                                                     QString("set boolean to true to reload trust list"),
                                                     false);
        m_pReloadTrustList->setValidator(new cBoolValidator());
        m_module->m_veinModuleParameterMap[m_pReloadTrustList->getComponentName()] = m_pReloadTrustList;

        m_spRpcAuthenticateInterface = VfCpp::cVeinModuleRpc::Ptr(
            new VfCpp::cVeinModuleRpc(
                m_module->getEntityId(),
                m_module->getValidatorEventSystem(),
                this,
                "RPC_Authenticate",
                VfCpp::cVeinModuleRpc::Param({
                    {"p_displayName", "QString"},
                    {"p_tokenType", "QString"},
                    {"p_token", "QString"}
                }),
                false, // not using thread
                false // not blocking
        ));

        m_module->getRpcEventSystem()->addRpc(m_spRpcAuthenticateInterface);

        connect(m_pReloadTrustList, &VfModuleParameter::sigValueChanged, this, &cApiModuleAuthorize::onReloadTrustList);

        // May all be fired in RPC startup code and must delay sending the response.
        connect(m_pGuiDialogFinished, &VfModuleParameter::sigValueChanged, this, &cApiModuleAuthorize::onGuiDialogFinished, Qt::QueuedConnection);

        connect(this, &cApiModuleAuthorize::finishDialog, this, &cApiModuleAuthorize::onGuiDialogFinished, Qt::QueuedConnection);
        connect(this, &cApiModuleAuthorize::rejectRpc, this, &cApiModuleAuthorize::onRpcRejected, Qt::QueuedConnection);
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

    void cApiModuleAuthorize::onRpcRejected(QUuid uuid)
    {
        m_spRpcAuthenticateInterface->sendRpcResult(
           uuid,
           VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_EINVAL,
           "trust request already active",
           0
        );
    }

    void cApiModuleAuthorize::onReloadTrustList(QVariant reload){
        if (reload == false) return;

        m_pReloadTrustList->setValue(false);

        m_spTrustListAct->setValue(readTrustList());
        m_spTrustListChangeCountAct->setValue(m_spTrustListChangeCountAct->getValue().toInt() + 1);
    }

    void cApiModuleAuthorize::onGuiDialogFinished(QVariant dialogFinished)
    {
        m_bDialogIsOpen = false;

        bool added = false;

        if(dialogFinished == true)
        {
            QJsonArray readList = readTrustList();

            if(jsonArrayContains(readList, m_spPendingRequestAct->getValue().toJsonObject())){
                m_spTrustListAct->setValue(readList);
                m_spTrustListChangeCountAct->setValue(m_spTrustListChangeCountAct->getValue().toInt() + 1);

                added = true;
            }

            m_spPendingRequestAct->setValue(QJsonObject());
        }

        QUuid uuid = m_rpcRequest;

        if(uuid.isNull()) return;

        m_rpcRequest = QUuid();

        m_spRpcAuthenticateInterface->sendRpcResult(
            uuid,
            VeinComponent::RemoteProcedureData::RPCResultCodes::RPC_SUCCESS,
            "",
            added
        );
    }

    QVariant cApiModuleAuthorize::RPC_Authenticate(QVariantMap p_params)
    {
        // Cancel any outstandig request.
        if(!m_rpcRequest.isNull()) emit rejectRpc(m_rpcRequest);

        // Remember identifier of the RPC call.
        m_rpcRequest = p_params[VeinComponent::RemoteProcedureData::s_callIdString].toUuid();

        // Dialog is open - force close and finish this RPC call.
        if (m_bDialogIsOpen)
            emit finishDialog(true);
        else{
            // Create new request object.
            QJsonObject request;
            request["name"] = p_params["p_displayName"].toString();
            request["tokenType"] = p_params["p_tokenType"].toString();
            request["token"] = p_params["p_token"].toString();

            // See if we alrady know this trust - eventually simulate dialog close.
            if(jsonArrayContains(m_spTrustListAct->getValue().toJsonArray(), request))
                emit finishDialog(true);
            else{
                m_bDialogIsOpen = true;

                // Force dialog to open.
                m_pGuiDialogFinished->setValue(false);
                m_spPendingRequestAct->setValue(request);
            }
        }

        return QVariant();
    }

    void cApiModuleAuthorize::setTrustListPath(const QString &path)
    {
        m_trustListPath = path;

        emit onReloadTrustList(true);
    }
}
