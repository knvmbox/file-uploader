#include <QPushButton>

#include <common/logger/loggerfactory.hpp>

#include "uploadimagesdlg.hpp"
#include "ui_uploadimagesdlg.h"


//-----------------------------------------------------------------------------
UploadImagesDlg::UploadImagesDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UploadImagesDlg),
    m_logger{common::LoggerFactory::instance()} {
    ui->setupUi(this);
    ui->dirSelector->setMode(FileSelector::OpenDir);

    auto albumSlot = [this](int index) {
        albumsSelected(
            ui->albumsBox->itemText(index),
            ui->albumsBox->itemData(index).toString()
        );
    };

    auto imagebanSlot = [this](int index) {
        auto secret = ui->imagebanBox->itemData(index).toString().toStdString();
        createImageban(std::move(secret));
        fillAlbumsBox();
        showNewAlbumsWidgets(false);
    };

    Settings settings;
    auto secrets = settings.secrets();
    fillImagbanBox(secrets);
    imagebanSlot(0);

    connect(ui->imagebanBox, QOverload<int>::of(&QComboBox::currentIndexChanged), imagebanSlot);
    connect(ui->albumsBox, QOverload<int>::of(&QComboBox::currentIndexChanged), albumSlot);
    connect(ui->addAlbumBtn, &QToolButton::clicked, [this]() {
        addAlbum(ui->albumNameEdit->text());
    });
    connect(ui->reloadAlbumsBtn, &QToolButton::clicked, [this]() {
        fillAlbumsBox();
    });
    connect(ui->saveImageBheck, &QCheckBox::toggled, [this](bool checked) {
        ui->dirSelector->setEnabled(checked);
    });

    albumSlot(ui->albumsBox->currentIndex());
}

//-----------------------------------------------------------------------------
UploadImagesDlg::~UploadImagesDlg() {
    delete ui;
}

//-----------------------------------------------------------------------------
params::UploadParams UploadImagesDlg::uploadParams() {
    auto secret = ui->imagebanBox->currentData().toString().toStdString();
    params::UploadParams params = {
        .secretKey = ui->imagebanBox->currentData().toString().toStdString(),
        .album = {
            .id = ui->albumsBox->currentData().toString().toStdString(),
            .name = ui->albumsBox->currentText().toStdString()
        },
        .isSave = ui->saveImageBheck->isChecked(),
        .dirPath = ui->dirSelector->filename().toStdString()
    };

    return params;
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::addAlbum(const QString &str) {
    try {
        auto album = m_imgImageBan->createAlbum(str.toStdString());
        fillAlbumsBox();

        ui->albumsBox->setCurrentText(album.name.c_str());
        ui->albumNameEdit->clear();

    } catch(imageban::imageban_error &e) {
        m_logger->error(e.what());
    }
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::albumsSelected(const QString &name, const QString &id) {
    showNewAlbumsWidgets(id.isEmpty());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(id.isEmpty());
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::createImageban(std::string secretKey) {
    m_imgImageBan.reset(new imageban::ImageBan{std::move(secretKey)});
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::fillAlbumsBox() {
    ui->albumsBox->clear();
    auto list = m_imgImageBan->albumsList();

    for(const auto &item : list) {
        ui->albumsBox->addItem(item.name.c_str(), item.id.c_str());
    }

    ui->albumsBox->addItem("Другой", QStringList{});
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::fillImagbanBox(const std::vector<Settings::Secret> &s) {
    for(const auto &item : s) {
        ui->imagebanBox->addItem(item.title.c_str(), item.key.c_str());
    }
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::showNewAlbumsWidgets(bool state) {
    ui->newAlbumLabel->setVisible(state);
    ui->albumNameEdit->setVisible(state);
    ui->addAlbumBtn->setVisible(state);
}
