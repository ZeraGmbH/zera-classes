#include <QTextStream>

#include "dspmeasdata.h"


cDspMeasData::cDspMeasData(QString name)
    :m_sname(name)
{
}


cDspMeasData::~cDspMeasData()
{
    if (DspVarList.count() > 0)
    {
        cDspVar* pDspVar;
        for (int i = 0; i < DspVarList.size(); ++i)
        {
            pDspVar = DspVarList.at(i);
            delete pDspVar;
        }
    }
}


float* cDspMeasData::data(QString name) // gibt einen zeiger zurück auf die var daten
{
    if (DspVarList.count() > 0)
    {
        cDspVar* pDspVar;
        for (int i = 0; i < DspVarList.size(); ++i)
        {
            pDspVar = DspVarList.at(i);
            if (pDspVar->Name() == name)
                return pDspVar->data();
        }
    }
    return 0; // caller has to pay attention !!!!!
}


void cDspMeasData::addVarItem(cDspVar* var) // eine neue dsp variable
{
    DspVarList.append(var);
    // wir brauchen speicher für daten, die wir lesen und/oder schreiben wollen
    // dies ist der fall für ergebnisse , dsp konstanten und parameter
    //
    /*
    if ( (var->type() & (DSPDATA::vDspResult | DSPDATA::vDspIntVar | DSPDATA::vDspParam)) > 0)
        DspVarData.resize ( DspVarData.size() + var->size());
    */
}

quint32 cDspMeasData::getSize()
{
    quint32 size = 0;

    if (DspVarList.count() > 0)
    {
        for (int i = 0; i < DspVarList.size(); ++i)
            size += DspVarList.at(i)->size();
    }

    return size;
}


quint32 cDspMeasData::getSize(QString name)
{
    quint32 size = 0;

    if (DspVarList.count() > 0)
    {
        cDspVar* pDspVar;
        for (int i = 0; i < DspVarList.size(); ++i)
        {
            pDspVar = DspVarList.at(i);
            if (pDspVar->Name() == name)
                return pDspVar->size();
        }
    }

    return size;
}


QString& cDspMeasData::VarList(int section, bool withType)
{
    sReturn="";
    QTextStream ts( &sReturn, QIODevice::WriteOnly );
    cDspVar *pDspVar;

    for (int i = 0; i < DspVarList.size(); ++i)
    {
        pDspVar = DspVarList.at(i);
        if ((section & pDspVar->type()) > 0) // do we want this value ?
        {
            if (withType)
                ts << QString("%1,%2,%3;").arg(pDspVar->Name()).arg(pDspVar->size()).arg(pDspVar->datatype());
            else
                ts << QString("%1,%2;").arg(pDspVar->Name()).arg(pDspVar->size());
        }
    }


    return sReturn; // eine liste aller variablen namen und deren länge
}


QString& cDspMeasData::name()
{
    return m_sname;
}

QString& cDspMeasData::writeCommand()
{
    sReturn="";
    QTextStream ts(&sReturn, QIODevice::WriteOnly );

    for (int i = 0; i < DspVarList.count(); i++)
    {
        cDspVar* pVar = DspVarList.at(i);
        ts << pVar->Name();

        float* fval = pVar->data();
        int type = pVar->datatype();

        if (type == DSPDATA::dInt)  // wir haben integer daten
        {
            ulong* lval = (ulong*) fval;
            for (int j = 0; j < pVar->size(); j++, lval++)
                ts << "," << *lval;
        }
        else
        {
            for (int j = 0; j < pVar->size(); j++, fval++)
                ts << "," << *fval;
        }

        ts << ";";
    }

    return sReturn;

}


QVector<float>& cDspMeasData::getData()
{
    vector.clear();
    for (int i = 0; i < DspVarList.count(); i++) // we fetch all data of all vars in this memory group
    {
        cDspVar* pVar = DspVarList.at(i);
        float* fval = pVar->data();
        for (int j = 0; j < pVar->size(); j++, fval++)
            vector.append(*fval);
    }
    return vector;
}




