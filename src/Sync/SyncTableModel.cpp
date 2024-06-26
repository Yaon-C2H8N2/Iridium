#include "SyncTableModel.hpp"

SyncTableModel::SyncTableModel(QObject *parent) : QAbstractTableModel(parent)
{
	_headers = {
					tr("Source"), tr("État"), tr("Destination"), tr("Taille"), tr("Temps restant"), tr("Temps écoulé"),
					tr("Vitesse"),
					tr("Vitesse moyenne")
			};
}

int SyncTableModel::rowCount(const QModelIndex &parent) const { return _data.size(); }
int SyncTableModel::columnCount(const QModelIndex &parent) const { return _headers.size(); }

QVariant SyncTableModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) { return _data[index.row()]->data(index.column()); }
	return QVariant();
}

void SyncTableModel::clear()
{
	beginResetModel();
	_data.clear();
	endResetModel();
}

QVariant SyncTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) { return _headers[section]; }
	return QString::number(section + 1);
}

void SyncTableModel::setData(const std::vector<SyncRow *> &rows)
{
	beginInsertRows(QModelIndex(), 0, rows.size() - 1);
	_data = rows;
	endInsertRows();
}

void SyncTableModel::updateRowData()
{
	// sort the data
	emit layoutChanged();
}

void SyncTableModel::sort(int column, Qt::SortOrder order)
{
	std::sort(_data.begin(), _data.end(), [column, order](SyncRow *a, SyncRow *b)
	{
		return a->compare(column, order, *b);
	});
	emit layoutChanged();
}
