#ifndef DSPVAR_H
#define DSPVAR_H

namespace DSPDATA
{
    enum dType {dInt, dFloat}; // data value type int or float

    enum DspValueType {vDspResult = 1, vDspTemp = 2, vDspIntVar = 4, vDspParam = 8};

    enum DspSegType {sLocal = 1, sGlobal = 2};
    // slocal segmente sind für jeden client einzeln
    // sie werden beim dsp server für jeden client angelegt
    // es ist elegant und komfortabel daten über ein handle
    // zu schreiben bzw. zu lesen. dsp globale daten müssen aber
    // nicht separart deklariert werden, weil sie ja schon dsp intern
    // existieren. es wäre aber schön ein handle für diese benutzen
    // zu können. deshalb sglobal !!!
}

class cDspVar // dsp variable
{
public:
    cDspVar(QString name, int size, int type, int datatype = DSPDATA::dFloat )
        : m_sName(name), m_nsize(size), m_nType(type), m_nDataType(datatype) {}
    QString& Name() { return m_sName;}
    int size() { return m_nsize; }
    int type() { return m_nType; }
    int datatype() {return m_nDataType; }
private:
    QString m_sName; // a var. has its name
    int m_nsize; // it has a number of elements
    int m_nType; // an it can be of different type : vDspResult, vDspTemp , vDspConstant , vDspParam
    int m_nDataType; // it can be float or int
};
#endif // DSPVAR_H
