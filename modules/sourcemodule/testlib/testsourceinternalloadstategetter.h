#ifndef TESTSOURCEINTERNALLOADSTATEGETTER_H
#define TESTSOURCEINTERNALLOADSTATEGETTER_H

#include <abstractserverInterface.h>
#include <jsonparamapi.h>
#include <jsonstructapi.h>
#include <taskcontainerinterface.h>

class TestSourceInternalLoadStateGetter : public QObject
{
    Q_OBJECT
public:
    TestSourceInternalLoadStateGetter(AbstractServerInterfacePtr serverInterface);

    bool getOn(phaseType type, int phaseIdxBase0) const;
    bool getSourceModeOn(phaseType type, int phaseIdxBase0)  const;
    double getDspPeakAmplitude(phaseType type, int phaseIdxBase0) const;
    double getDspAngle(phaseType type, int phaseIdxBase0) const;
    double getDspFreqVal(phaseType type, int phaseIdxBase0) const;
    //bool getFreqVarOn(phaseType type, int phaseIdxBase0) const;

private:
    TaskContainerInterfacePtr createFetchLoadTasks(const JsonParamApi &paramState);


    AbstractServerInterfacePtr m_serverInterface;
    TaskContainerInterfacePtr m_currentTasks;
};

#endif // TESTSOURCEINTERNALLOADSTATEGETTER_H
