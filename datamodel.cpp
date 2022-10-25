#include "datamodel.h"

DataModel::DataModel(int row, QStandardItem item, QObject *parent)
	: QObject{parent}
{
	_row = row;
	_item = item.clone();
}
