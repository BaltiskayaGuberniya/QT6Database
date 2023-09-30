#ifndef DATABASE_H
#define DATABASE_H

//QMainWindow is a class which inherits QWidgets which in turn inherits QOBJECT.
//QOBJECT is the base class of most classes in QT.
//It provides many features to classes such as baked in signal and slot mechanisms.
//by using the MOC(Meta Object Compiler) to autogen required code for our classes.
//The MOC also helps QT avoid native C++ compiler RTTI(Run Time Type Information) support
//i.e. qt is creating its own data types.
#include <QMainWindow>
#include <QStandardItemModel>
#include<Qtsql/QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class DataBase; }
QT_END_NAMESPACE

class DataBase : public QMainWindow {   //inheriting the class QMainWindow

    // QOBJECT MACRO.This macro in the private section of a class
    //is used to enable meta object features such as dynamic properties , signals and slots
    Q_OBJECT
	
public:
    // default constructor. adding a parent-child heirarchy for automatic memory managemnt. default parent is nullptr
    //when parent is deleted , it deletes all of its children.
    DataBase(QWidget *parent = nullptr);
    ~DataBase();                        //desructor
    void addClient();                   //member functions
    void addActivity();
    void openCalendar();
    void removeItem();
    void setWidgetText();
    void generateExample();
    void updateWidgetText();
    void clearWidget();

private:    
    Ui::DataBase *ui;                // a pointer to an object of datatype DataBase defined inside Ui namespace
	
    QStandardItemModel model;       // generic qt model for storing data . included above
	
    QModelIndex _selectedIndex;     //used to index data

    void insertDataFromModel(QSqlQuery& query, const QModelIndex& parentIndex,const QString& parentId, const QString& Id);

    void importDataFromDB(const QString & pId,const QModelIndex& Parent_index );

private slots:
    void on_treeView_clicked(const QModelIndex &index);
    void on_btnAddActivity_clicked();
    void on_actionremove_selection_triggered();
    void save();
    void load();
};
#endif // DATABASE_H
