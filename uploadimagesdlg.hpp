#ifndef UPLOADIMAGESDLG_HPP
#define UPLOADIMAGESDLG_HPP

#include <memory>
#include <string>
#include <utility>

#include <QDialog>

#include <common/logger/logger.hpp>

#include "settings.hpp"
#include "types/uploadparams.hpp"
#include "utils/imageban.hpp"


namespace Ui {
class UploadImagesDlg;
}

class UploadImagesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UploadImagesDlg(QWidget *parent = nullptr);
    ~UploadImagesDlg();

    params::UploadParams uploadParams();

private:
    void addAlbum(const QString&);
    void albumsSelected(const QString&, const QString&);
    void createImageban(std::string);
    void fillAlbumsBox();
    void fillImagbanBox(const std::vector<Settings::Secret>&);
    void showNewAlbumsWidgets(bool);

private:
    Ui::UploadImagesDlg *ui;
    std::shared_ptr<common::Logger> m_logger;
    std::unique_ptr<imageban::ImageBan> m_imgImageBan;
};

#endif // UPLOADIMAGESDLG_HPP
