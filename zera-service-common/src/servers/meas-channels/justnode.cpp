#include <QDataStream>
#include <QString>

#include "justnode.h"


cJustNode::cJustNode(double corr, double arg)
    :m_fCorrection(corr),m_fArgument(arg)
{
}


void cJustNode::Serialize(QDataStream& qds)
{
    qds << m_fCorrection << m_fArgument;
}


void cJustNode::Deserialize(QDataStream& qds)
{
    qds >> m_fCorrection >> m_fArgument;
}


QString cJustNode::Serialize(int digits)
{
    QString s;
    s = QString("%1;%2;").arg(m_fCorrection,0,'f',digits)
                           .arg(m_fArgument,0,'f',digits);
    return s;
}


void cJustNode::Deserialize(const QString& s)
{
    m_fCorrection = s.section( ';',0,0).toDouble();
    m_fArgument = s.section( ';',1,1).toDouble();
}


cJustNode& cJustNode::operator = (const cJustNode& jn)
{
    m_fCorrection = jn.m_fCorrection;
    m_fArgument = jn.m_fArgument;
    return (*this);
}


void cJustNode::setCorrection(double value)
{
    m_fCorrection = value;
}

double cJustNode::getCorrection()
{
    return m_fCorrection;
}


void cJustNode::setArgument(double value)
{
   m_fArgument = value;
}


double cJustNode::getArgument()
{
    return m_fArgument;
}

