#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>

#include "fileselector.hpp"


//-----------------------------------------------------------------------------
FileSelector::FileSelector(QWidget *parent, Mode mode) :
    QWidget(parent),
    m_mode(mode),
    m_dlgCaption(tr("Select file ...")),
    m_workingDir(tr(".")),
    m_filesFilter(tr("All file (*.*)"))
{
    QHBoxLayout *layout = new QHBoxLayout();

    m_filenameEdit = new QLineEdit();
    m_selectBtn = new QToolButton(this);

    layout->setSpacing(2);
    layout->setContentsMargins(0, 0, 0, 0);
    m_filenameEdit->setReadOnly(true);
    m_selectBtn->setText("...");

    layout->addWidget(m_filenameEdit);
    layout->addWidget(m_selectBtn);

    this->setLayout(layout);

    connect(m_selectBtn, SIGNAL(clicked()), this, SLOT(selectFile()));
}

//-----------------------------------------------------------------------------
void FileSelector::selectFile()
{
    switch(m_mode){
    case OpenFile:
        m_filename = openFileDialog();
        break;
    case SaveFile:
        m_filename = saveFileDialog();
        break;
    case OpenDir:
        m_filename = openDirDialog();
    }

    m_filenameEdit->setText(m_filename);
    if(!m_filename.isEmpty())
        emit fileSelected(m_filename);
}

//-----------------------------------------------------------------------------
QString FileSelector::openDirDialog()
{
    return QFileDialog::getExistingDirectory(this, m_dlgCaption, m_workingDir);
}

//-----------------------------------------------------------------------------
QString FileSelector::openFileDialog()
{
    return QFileDialog::getOpenFileName(
        this,
        m_dlgCaption,
        m_workingDir,
        m_filesFilter
    );
}

//-----------------------------------------------------------------------------
QString FileSelector::saveFileDialog()
{
    return QFileDialog::getSaveFileName(
        this,
        m_dlgCaption,
        m_workingDir,
        m_filesFilter
    );
}
