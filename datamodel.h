#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QStandardItemModel>
#include <QObject>

class DataModel : public QObject
{
	Q_OBJECT
public:
	explicit DataModel(int row, QStandardItem item, QObject *parent = nullptr);
	
private:
	int _row;
	QStandardItem *_item;
	QList<DataModel> _children;
signals:
	
};

#endif // DATAMODEL_H
