#ifndef IMAGEBANCONFIGVIEWMODEL_HPP
#define IMAGEBANCONFIGVIEWMODEL_HPP

#include <QVector>

#include "abstractmodel.hpp"


//-----------------------------------------------------------------------------
class ImagebanConfigViewModel : public AbstractModel
{
    Q_OBJECT

public:
    struct Item {
        QString title;
        QString key;
    };

public:
    explicit ImagebanConfigViewModel(QObject *parent = nullptr);
    ~ImagebanConfigViewModel();

public:
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    int rowCount(const QModelIndex&) const override {
        return m_items.size() + 1;
    }

protected:
    QVariant displayData(const QModelIndex&) const override;
    QVariant editData(const QModelIndex&) const override;
    QVariant foregroundColor(const QModelIndex&) const override;
    bool setValueData(const QModelIndex&, const QVariant&) override;

private:
    void load();
    void save() const;

private:
    QVector<Item> m_items;
};

#endif // IMAGEBANCONFIGVIEWMODEL_HPP
