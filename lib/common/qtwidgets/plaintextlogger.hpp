#ifndef PLAINTEXTLOGGER_HPP
#define PLAINTEXTLOGGER_HPP

#include <vector>
#include <tuple>
#include <memory>

#include <QObject>
#include <QPlainTextEdit>
#include <QThread>

#include "../logger/logger.hpp"
#include "../queue.hpp"


namespace common {

class PlainTextLogger : public QObject, public Logger
{
    Q_OBJECT
public:
    typedef std::tuple<std::string,LoggerLevel,QString, Qt::HANDLE> LoggerItem;

public:
    explicit PlainTextLogger(QPlainTextEdit *text, bool events = false);

    virtual void debug(const std::string &msg) override
        {   saveToQueue(msg, LOG_DEBUG, m_blue);   }
    virtual void info(const std::string &msg) override
        {   saveToQueue(msg, LOG_INFO, m_black);   }
    virtual void warn(const std::string &msg) override
        {   saveToQueue(msg, LOG_WARN, m_yellow);   }
    virtual void error(const std::string &msg) override
        {   saveToQueue(msg, LOG_ERROR, m_red);   }
    virtual void critical(const std::string &msg) override
        {   saveToQueue(msg, LOG_CRITICAL, m_red);   }

    void setLogLevel(Logger::LoggerLevel level)
        {   m_logLevel = level;   }

signals:
    void haveNewMsg();

private:
    QString currentTime();
    void saveToQueue(const std::string &text, Logger::LoggerLevel mode, const QString &color);

private slots:
    void output();

private:
    QPlainTextEdit *m_text;

    QString m_black;
    QString m_blue;
    QString m_red;
    QString m_yellow;

    Logger::LoggerLevel m_logLevel;
    std::vector<QString> m_logMode;
    common::Queue<LoggerItem> m_msgQueue;
    bool m_events;
};
}

#endif
