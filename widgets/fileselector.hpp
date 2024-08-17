#ifndef FILESELECTOR_HPP
#define FILESELECTOR_HPP

#include <QLineEdit>
#include <QToolButton>
#include <QWidget>


class FileSelector : public QWidget
{
    Q_OBJECT
public:
    enum Mode { OpenFile, SaveFile, OpenDir };

public:
    explicit FileSelector(QWidget *parent = 0, Mode mode = OpenFile);
    ~FileSelector()
        {}

    QString filename()
        {   return m_filename;   }

    void setMode(Mode mode)
        {   m_mode = mode;   }
    void setFilter(const QString &filter)
        {   m_filesFilter = filter;   }

signals:
    void fileSelected(const QString &file);

public slots:

private slots:
    void selectFile();

private:
    QString openDirDialog();
    QString openFileDialog();
    QString saveFileDialog();

private:
    Mode m_mode;
    QString m_filename;

    QLineEdit *m_filenameEdit;
    QToolButton *m_selectBtn;

    QString m_dlgCaption;
    QString m_workingDir;
    QString m_filesFilter;
};

#endif // FILESELECTOR_HPP
