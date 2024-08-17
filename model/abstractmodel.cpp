#include "abstractmodel.hpp"


//-----------------------------------------------------------------------------
AbstractModel::AbstractModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

//-----------------------------------------------------------------------------
QVariant AbstractModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();
    if(section >= static_cast<int>(m_columns.size()))
        return QVariant();

    return QString::fromStdString(m_columns[section].c_str());
}

//-----------------------------------------------------------------------------
QVariant AbstractModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(index.row() >= rowCount())
        return QVariant();


    switch(role)
    {
    case Qt::BackgroundRole: return backgroundColor(index);        
    case Qt::CheckStateRole: return checkStateData(index);
    case Qt::DisplayRole: return displayData(index);
    case Qt::EditRole: return editData(index);
    case Qt::ForegroundRole: return foregroundColor(index);
    case Qt::TextAlignmentRole: return textAlignment(index);
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
bool AbstractModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    if(index.row() >= rowCount())
        return false;

    bool result = false;

    switch(role)
    {
    case Qt::CheckStateRole:
        result = setCheckData(index, value);
        break;
    case Qt::EditRole:
        result = setValueData(index, value);
        break;
    }

    if(result)
        emit dataChanged(index, index);

    return result;
}
