#include <QTextStream>

#include "dspmeasdata.h"


cDspMeasData::cDspMeasData(QString name, DSPDATA::DspSegType st)
    :m_sname(name), m_SegmentType(st)
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


float* cDspMeasData::data() // gibt einen zeiger zurück auf die var daten vom typ vDspResult bzw. vmemory
{
    return DspVarData.data();
}


void cDspMeasData::addVarItem(cDspVar* var) // eine neue dsp variable
{
    DspVarList.append(var);
    // wir brauchen speicher für daten, die wir lesen und/oder schreiben wollen
    // dies ist der fall für ergebnisse , dsp konstanten und parameter
    //
    if ( (var->type() & (DSPDATA::vDspResult | DSPDATA::vDspIntVar | DSPDATA::vDspParam)) > 0)
        DspVarData.resize ( DspVarData.size() + var->size());
}

quint32 cDspMeasData::getDataCount()
{
    return DspVarData.count();
}


QString& cDspMeasData::MeasVarList()
{
    m_slist="";
    QTextStream ts( &m_slist, QIODevice::WriteOnly );
    cDspVar *DspVar;

    for (int i = 0; i < DspVarList.size(); ++i)
    {
        DspVar = DspVarList.at(i);
        if ((DspVar->type() & (DSPDATA::vDspResult)) > 0)
            ts << QString("%1;").arg(DspVar->Name());
    }

    return m_slist; // eine liste aller variablen namen vom typ vdspresult
}


QString& cDspMeasData::VarList(bool withType)
{
    m_slist="";
    QTextStream ts( &m_slist, QIODevice::WriteOnly );
    cDspVar *DspVar;

    if (m_SegmentType == DSPDATA::sLocal) // global data must not be created
    {
        for (int i = 0; i < DspVarList.size(); ++i)
        {
            DspVar = DspVarList.at(i);
            if (withType)
                    ts << QString("%1,%2,%3;").arg(DspVar->Name()).arg(DspVar->size()).arg(DspVar->datatype());
            else
                ts << QString("%1,%2;").arg(DspVar->Name()).arg(DspVar->size());

        }
    }

    return m_slist; // eine liste aller variablen namen und deren länge
}


QString& cDspMeasData::name()
{
    return m_sname;
}

DSPDATA::DspSegType cDspMeasData::segType()
{
    return m_SegmentType;
}


