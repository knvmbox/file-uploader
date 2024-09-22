#include <QPushButton>

#include <common/logger/loggerfactory.hpp>

#include "uploadimagesdlg.hpp"
#include "ui_uploadimagesdlg.h"


//-----------------------------------------------------------------------------
UploadImagesDlg::UploadImagesDlg(
    std::string secretKey, std::string thumbSecretKey, QWidget *parent
) : QDialog(parent),
    ui(new Ui::UploadImagesDlg),
    m_logger{common::LoggerFactory::instance()},
    m_imgImageBan{std::move(secretKey)},
    m_thumbImageBan{std::move(thumbSecretKey)} {
    ui->setupUi(this);

    showNewAlbumsWidgets(false);

    auto slot = [this](int index) {
        albumsSelected(
            ui->albumsBox->itemText(index),
            ui->albumsBox->itemData(index).toStringList()
        );
    };

    connect(ui->albumsBox, QOverload<int>::of(&QComboBox::currentIndexChanged), slot);
    connect(ui->addAlbumBtn, &QToolButton::clicked, [=]() {
        addAlbum(ui->albumNameEdit->text());
    });
    connect(ui->reloadAlbumsBtn, &QToolButton::clicked, [=]() {
        fillAlbumsBox();
    });

    fillAlbumsBox();
    slot(ui->albumsBox->currentIndex());
}

//-----------------------------------------------------------------------------
UploadImagesDlg::~UploadImagesDlg() {
    delete ui;
}

//-----------------------------------------------------------------------------
std::pair<imageban::album_t, imageban::album_t> UploadImagesDlg::album() {
    auto list = ui->albumsBox->currentData().toStringList();

    return {{
        list.at(0).toStdString(),
        ui->albumsBox->currentText().toStdString()
    },{
        list.at(1).toStdString(),
        ui->albumsBox->currentText().toStdString()
    }};
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::addAlbum(const QString &str) {
    try {
        auto album = m_imgImageBan.createAlbum(str.toStdString());
        auto thumbAlbum = m_thumbImageBan.createAlbum(str.toStdString());
        fillAlbumsBox();

        ui->albumsBox->setCurrentText(album.name.c_str());
        ui->albumNameEdit->clear();

    } catch(imageban::imageban_error &e) {
        m_logger->error(e.what());
    }
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::albumsSelected(const QString &name, const QStringList &ids) {
    showNewAlbumsWidgets(ids.isEmpty());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(ids.isEmpty());
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::fillAlbumsBox() {
    ui->albumsBox->clear();
    auto list = m_imgImageBan.albumsList();
    auto thumbList = m_thumbImageBan.albumsList();

    for(const auto &item : list) {
        auto it = std::find_if(
            thumbList.begin(),
            thumbList.end(),
            [&item](const imageban::album_t &album){
                return (album.name == item.name);
        });

        if(it == thumbList.end()) {
            continue;
        }

        ui->albumsBox->addItem(
            item.name.c_str(),
            QStringList{item.id.c_str(), it->id.c_str()}
        );
    }

    ui->albumsBox->addItem("Другой", QStringList{});
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::showNewAlbumsWidgets(bool state) {
    ui->newAlbumLabel->setVisible(state);
    ui->albumNameEdit->setVisible(state);
    ui->addAlbumBtn->setVisible(state);
}
