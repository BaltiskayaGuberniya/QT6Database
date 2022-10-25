#ifndef DATABASE_H
#define DATABASE_H

#include <QMainWindow>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class DataBase; }
QT_END_NAMESPACE

class DataBase : public QMainWindow {
	Q_OBJECT
	
public:
	DataBase(QWidget *parent = nullptr);
	~DataBase();
    void addClient();
    void addActivity();
    void openCalendar();
    void removeItem();
    void setWidgetText();
    void generateExample();
    void updateWidgetText();
	
private:
	Ui::DataBase *ui;
	
	QStandardItemModel model;
	
	QModelIndex _selectedIndex;
	
private slots:
    void on_treeView_pressed(const QModelIndex &index);
};
#endif // DATABASE_H
