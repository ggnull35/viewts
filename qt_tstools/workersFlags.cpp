#include "workersFlags.h"

////////////////////////////////
// Timestamp worker - base class
workerFlag::workerFlag(std::string &tsFileName, Chart *chart) :
    timeStampWorker(tsFileName, chart)

{
    // new drawing
    m_scatterSeries = new QScatterSeries();
}

workerFlag::~workerFlag()
{
    m_chart->removeSeries(m_scatterSeries);
    delete(m_scatterSeries);
}

void workerFlag::showSeries()
{
    m_chart->addSeries(m_scatterSeries);
    m_chart->createDefaultAxes();
}

void workerFlag::hideSeries()
{
    m_chart->removeSeries(m_scatterSeries);
}

void workerFlag::run()
{
    m_isRunning = true;

    while (m_timestamp->run(m_WindowPacket) == true && m_isAborting == false)
    {
        unsigned int index;
        double val;
        while(getData(index, val) == true  && m_isAborting == false)
        {
            if (m_isTimeXaxis) {
                double time;
                // time for X axis
                if (m_timestamp->getTimeFromIndex(index, time) == true) {
                    m_scatterSeries->append( time, static_cast<qreal>(val));
                    qDebug() << m_scatterSeries->name() << " - index " << index << " - " << time << " s - " << val;
                }
            }
            else {
                // packet index for X axis
                m_scatterSeries->append( index, static_cast<qreal>(val));
                qDebug() << m_scatterSeries->name() << " - index " << index << " - " << val;
            }
        }
        updateProgress();
    }

    m_isRunning = false;
    emit updated(100);
    emit finished();
}

void workerFlag::serializeSeries(std::ofstream *outFile)
{
    *outFile << m_scatterSeries->name().toStdString().c_str() << std::endl;
    for (int i = 0; i < m_scatterSeries->count(); i++)
    {
        QString line = QString::number(m_scatterSeries->at(i).x(), 'f', 0) + ", " + QString::number(m_scatterSeries->at(i).y(), 'f', 9);
        *outFile << line.toStdString().c_str() << std::endl;
    }
}

////////////////////
// Flag worker
ccWorker::ccWorker(std::string &tsFile, unsigned int pid, Chart *chart) :
    workerFlag(tsFile, chart), m_pid(pid)
{
    // customize base class
    m_timestamp = new timestamp(tsFile, pid);
    m_scatterSeries->setName(QString(tr("CC")));
    m_scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_scatterSeries->setMarkerSize(15.0);
}

rapFlagWorker::rapFlagWorker(std::string &tsFile, unsigned int pid, Chart *chart) :
    workerFlag(tsFile, chart), m_pid(pid), m_index(0)
{
    // customize base class
    m_timestamp = new timestamp(tsFile, pid);
    m_scatterSeries->setName(QString(tr("RAP")));
    m_scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    m_scatterSeries->setMarkerSize(15.0);
}
