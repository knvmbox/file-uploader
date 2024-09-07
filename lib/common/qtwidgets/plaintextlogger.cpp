#include <QApplication>
#include <QTime>

#include "plaintextlogger.hpp"


namespace common {

//-----------------------------------------------------------------------------
PlainTextLogger::PlainTextLogger(QPlainTextEdit *text, bool events):
    QObject(),
    m_text(text),
    m_black("#000000"),
    m_blue("#0000BB"),
    m_red("#BB0000"),
    m_yellow("#FF9955"),
    m_logLevel(LOG_DEBUG),
    m_events(events)
{
    m_logMode.emplace_back("CRITICAL");
    m_logMode.emplace_back("ERROR");
    m_logMode.emplace_back("WARN");
    m_logMode.emplace_back("INFO");
    m_logMode.emplace_back("DEBUG");

    connect(this, SIGNAL(haveNewMsg()), this, SLOT(output()));
}

//-----------------------------------------------------------------------------
QString PlainTextLogger::currentTime()
{
    QTime time = QTime::currentTime();
    return time.toString("HH:mm:ss");
}

//-----------------------------------------------------------------------------
void PlainTextLogger::output()
{
    if(m_msgQueue.empty())
        return;

    LoggerItem msg = m_msgQueue.pop();
    if(std::get<1>(msg) > m_logLevel)
        return;

    QString format = "<font color=\"%4\">%1 [%2] %3 </font>";
    QString result = format.arg(currentTime())
            .arg(m_logMode[static_cast<size_t>(std::get<1>(msg))])          // Режим mode
            .arg(std::get<0>(msg).c_str())                                  // Текст сообщения
            .arg(std::get<2>(msg));                                         // Цвет

    m_text->appendHtml(result);

    if(std::get<3>(msg) == QThread::currentThreadId() && m_events)
        QApplication::processEvents();
}

//-----------------------------------------------------------------------------
void PlainTextLogger::saveToQueue(
    const std::string &text, Logger::LoggerLevel mode, const QString &color
) {
    m_msgQueue.push(std::make_tuple(text, mode, color, QThread::currentThreadId()));
    emit haveNewMsg();
}

}

