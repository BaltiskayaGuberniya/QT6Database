#include "database.h"
#include "./ui_database.h"
#include "calendar.h"
#include <QDate>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItem>
#include<QMenu>
#include<QAction>
#include<QToolButton>
#include <QIcon>
#include<QKeySequence>
#include<QFileDialog>
#include<QString>
#include<QtSql/QSqlDatabase>
#include<Qtsql/QSqlError>
#include<Qtsql/QSqlQuery>
#include<QMap>
#include<QDir>

//class::constructor(parameter):initializations(QOBJECT, Data member  Ui::DataBase *ui  )
DataBase::DataBase(QWidget *parent) : QMainWindow(parent), ui(new Ui::DataBase) {
    ui->setupUi(this);    //inherited from QMainWindow

    //setting up the file toolbutton(QAction) in toolbar
    auto Action_save= new QAction("Save",this);
    auto Action_load=new QAction ("Open new Database",this);
    Action_save->setObjectName("Action_save");
    Action_load->setObjectName("Action_load");
    Action_save->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    Action_load->setShortcut(QKeySequence(Qt::CTRL |  Qt::Key_O));

    QMenu* File_Menu= new QMenu(ui->toolBar);
    File_Menu->addAction(Action_save);
    File_Menu->addAction(Action_load);
    QToolButton *File_Button= new QToolButton(ui->toolBar);
    File_Button->setText("File");
    File_Button->setIcon(QIcon(":/Icons/Icons/file.png"));
    File_Button->setMenu(File_Menu);
    File_Button->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBar->insertWidget(ui->toolBar->actions().first(),File_Button);

    //EVENT HANDLERS (function pointer, signal,functor, slot, connection type:(default)AutoConnection )
    // btnCalendar etc. are custom label QAction
    connect(ui->btnAddClient, &QPushButton::clicked, this, &DataBase::addClient);
    //connect(ui->btnAddActivity, &QPushButton::clicked, this, &DataBase::addActivity);
    connect(ui->btnRemoveClient, &QPushButton::clicked, this, &DataBase::removeItem);
    connect(ui->btnRemoveActivity, &QPushButton::clicked, this, &DataBase::removeItem);
    connect(ui->btnCalendar, &QPushButton::clicked, this, &DataBase::openCalendar);

    connect(ui->actionGenerate_Example, &QAction::triggered, this, &DataBase::generateExample);
    connect(ui->actionRemove_Selected, &QAction::triggered, this, &DataBase::removeItem);
    connect(Action_save,&QAction::triggered,this,&DataBase::save);
    connect(Action_load,&QAction::triggered,this,&DataBase::load);
	
    connect(ui->leditClient, &QLineEdit::textChanged, this, &DataBase::updateWidgetText);
    connect(ui->spinYearClient, &QSpinBox::textChanged, this, &DataBase::updateWidgetText);
    connect(ui->notesClient, &QPlainTextEdit::textChanged, this, &DataBase::updateWidgetText);
    connect(ui->leditActivity, &QLineEdit::textChanged, this, &DataBase::updateWidgetText);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &DataBase::updateWidgetText);
    connect(ui->notesActivity, &QPlainTextEdit::textChanged, this, &DataBase::updateWidgetText);

    //sets the headers of the gui
    QStringList headers; //using QStringList=QList<QString> i.e.  headers is a list of strings
    headers << "Name" << "Date" << "Notes";
    model.setHorizontalHeaderLabels(headers);
    auto header=ui->treeView->header();

    ui->treeView->setModel(&model);
	
    //gets the current year and set it for the spinbox. get current date and set it for datebox
    QString date = QDate::currentDate().toString();
    date = date.sliced(date.size()-4,4); // slicing the year from the whole date
    int year = date.toInt();
    ui->spinYearClient->setValue(year);
    ui->dateEdit->setDate(QDate::currentDate());
}
//constructor end. All of this is done as soon as a database object is created.

DataBase::~DataBase() {
    delete ui;
}

void DataBase::addClient() {
    QStandardItem *root = model.invisibleRootItem(); //initialising the root
    int rowIndex = root->rowCount();
    QString clientName = ui->leditClient->text();
    if (clientName == "") {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Please Enter the name of client!");
        msgBox.setWindowTitle("Name!");
        msgBox.exec();
        return;
    }
    QString notes = ui->notesClient->toPlainText();
    QString year = ui->spinYearClient->text();

    // adding children to the root in our tree model
    //root->setChild(rowIndex (intitialised to rowcount()), column index , QStandardItem * item)
    root->setChild(rowIndex,0, new QStandardItem(clientName));
    root->setChild(rowIndex,1, new QStandardItem(year));
    root->setChild(rowIndex,2, new QStandardItem(notes));
	
	
}

//void DataBase::addActivity() {
//	if (!_selectedIndex.isValid()) {
//        QMessageBox msgBox(this);
//        msgBox.setIcon(QMessageBox::Warning);
//        msgBox.setText("You need to first select a Client\nbefore adding an activity!");
//        msgBox.setWindowTitle("Warning!");
//        msgBox.exec();
//		return;
//	}
	
//	QStandardItem *root = model.itemFromIndex(_selectedIndex);
//	int rowIndex = root->rowCount();

//    QString activityName = ui->leditActivity->text();
//	QString notes = ui->notesActivity->toPlainText();
//    QString year = ui->dateEdit->date().toString();
			
//	root->setChild(rowIndex,0, new QStandardItem(activityName));
//	root->setChild(rowIndex,1, new QStandardItem(year));
//	root->setChild(rowIndex,2, new QStandardItem(notes));
//}

void DataBase::on_btnAddActivity_clicked()
{
    if (!_selectedIndex.isValid()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("You need to first select a Client\nbefore adding an activity!");
        msgBox.setWindowTitle("Warning!");
        msgBox.exec();
        return;
    }

    QStandardItem *root = model.itemFromIndex(_selectedIndex);
    int rowIndex = root->rowCount();

    QString activityName = ui->leditActivity->text();
    QString notes = ui->notesActivity->toPlainText();
    QString year = ui->dateEdit->date().toString();

    root->setChild(rowIndex,0, new QStandardItem(activityName));
    root->setChild(rowIndex,1, new QStandardItem(year));
    root->setChild(rowIndex,2, new QStandardItem(notes));

}


void DataBase::openCalendar() {
    Calendar *calendar = new Calendar(ui->dateEdit->date(),this);
    calendar->exec();
    if (!calendar->date.isNull()) ui->dateEdit->setDate(calendar->date);
}


void DataBase::on_treeView_clicked(const QModelIndex &index) {
    if (index.isValid()) {
      //  char type;
        _selectedIndex = index.siblingAtColumn(0);
        setWidgetText();
     }
}
void DataBase::on_actionremove_selection_triggered()
{
     _selectedIndex=QModelIndex();
     ui->treeView->clearSelection();
      clearWidget();
     //qDebug()<<"working";
}

void DataBase::setWidgetText() {
    QStandardItem *name = model.itemFromIndex(_selectedIndex);
    QStandardItem *date = model.itemFromIndex(_selectedIndex.siblingAtColumn(1));
    QStandardItem *notes = model.itemFromIndex(_selectedIndex.siblingAtColumn(2));

    //necessary due to signal/slots reacting even when values are changed programatically
    QString nameString = name->text();
    QString dateString = date->text();
    QString notesString = notes->text();


    if (_selectedIndex.parent().isValid()) { //ACTIVITY
        ui->toolBox->setCurrentIndex(0);
        ui->leditActivity->setText(nameString);
        ui->dateEdit->setDate(QDate::fromString(dateString));
        ui->notesActivity->setPlainText(notesString);
    }
    else { //CLIENT
        ui->toolBox->setCurrentIndex(1);
        ui->leditClient->setText(nameString);
        ui->spinYearClient->setValue(dateString.toInt());
        ui->notesClient->setPlainText(notesString);
    }
}

void DataBase::updateWidgetText() {
    if (_selectedIndex.isValid()) {
        QStandardItem *name = model.itemFromIndex(_selectedIndex);
        QStandardItem *date = model.itemFromIndex(_selectedIndex.siblingAtColumn(1));
        QStandardItem *notes = model.itemFromIndex(_selectedIndex.siblingAtColumn(2));

        if (_selectedIndex.parent().isValid()) { //ACTIVITY
            if (name->text() != ui->leditActivity->text()) {
                name->setText(ui->leditActivity->text());
            }
            if (date->text() != ui->dateEdit->date().toString()) {
                date->setText(ui->dateEdit->date().toString());
            }
            if (notes->text() != ui->notesActivity->toPlainText()) {
                notes->setText(ui->notesActivity->toPlainText());
            }
        }
        else { //CLIENT
            if (name->text() != ui->leditClient->text()) {
                name->setText(ui->leditClient->text());
            }
            if (date->text() != ui->spinYearClient->text()) {
                date->setText(ui->spinYearClient->text());
            }
            if (notes->text() != ui->notesClient->toPlainText()) {
                notes->setText(ui->notesClient->toPlainText());
            }
        }
    }
}
void DataBase::clearWidget(){
        ui->toolBox->setCurrentIndex(0);
        ui->leditActivity->setText("");
        ui->dateEdit->setDate(QDate::fromString(""));
        ui->notesActivity->setPlainText("");

       //CLIENT
        ui->toolBox->setCurrentIndex(1);
        ui->leditClient->setText("");
        ui->notesClient->setPlainText("");
        QString date = QDate::currentDate().toString();
        date = date.sliced(date.size()-4,4); // slicing the year from the whole date
        int year = date.toInt();
        ui->spinYearClient->setValue(year);
}

void DataBase::removeItem() {
    if (!_selectedIndex.isValid()) {
        QMessageBox msgBox = QMessageBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("You need to first select an item\nbefore removing it!");
        msgBox.setWindowTitle("Warning!");
        msgBox.exec();
        return;
    }
    QMessageBox::StandardButton msgBox;
    msgBox = QMessageBox::question(this, "Confirmation", "Are you sure you want\nto delete the selected item?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (msgBox == QMessageBox::No) {
      return;
    }

    QStandardItem *root;
    if (_selectedIndex.parent().isValid()) {
    root = model.itemFromIndex(_selectedIndex.parent());
    }
    else {
    root = model.invisibleRootItem();
    }
    QStandardItem *item = model.itemFromIndex(_selectedIndex);
    root->removeRow(item->row());
    _selectedIndex = QModelIndex();
}

QMap<QString, int> nodeCounters; // Map to store counters for each node type

QString generateNodeId(const QString& nodeType) {
    int& counter = nodeCounters[nodeType];
    counter++;
    return nodeType + QString::number(counter);
}

void DataBase::insertDataFromModel(QSqlQuery& query, const QModelIndex& parentIndex,const QString& parentId, const QString& Id){
    unsigned numRows=model.rowCount(parentIndex);
    for (unsigned row=0;row<numRows;++row){
    QModelIndex currentIndex=model.index(row,0,parentIndex);

    QString nodeId=generateNodeId(Id);
    QString New_Id=Id+QString("A");

    query.bindValue(0, nodeId);
    query.bindValue(1, currentIndex.data(Qt::DisplayRole).toString());
    query.bindValue(2, currentIndex.siblingAtColumn(1).data(Qt::DisplayRole).toString());
    query.bindValue(3, currentIndex.siblingAtColumn(2).data(Qt::DisplayRole).toString());
    query.bindValue(4, parentId);

    query.exec();

    insertDataFromModel(query,currentIndex,nodeId,New_Id);
    }

}

void DataBase::save(){
    QString selectedFileName = QFileDialog::getSaveFileName(this, "Choose Database File", QDir::currentPath(), "SQLite Database Files (*.sqlite *.db)");
    if (selectedFileName.isEmpty()) {
    // User canceled the file dialog
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(selectedFileName);

    if (!db.open()) {
    QMessageBox::critical(this, "Database Error", "Failed to open database:\n" + db.lastError().text());
    }
    QSqlQuery dropquery;
    dropquery.exec("DROP TABLE IF EXISTS Clients");
    QSqlQuery addquery;
    addquery.exec("CREATE TABLE Clients (node_id TEXT PRIMARY KEY, name TEXT, date TEXT, notes TEXT, parent_id TEXT)");

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO Clients (node_id, name, date, notes, parent_id) VALUES (?, ?, ?, ?, ?)");
    // Iterate through the model and insert data into the database
    QModelIndex rootIndex = model.indexFromItem(model.invisibleRootItem());
    insertDataFromModel(insertQuery, rootIndex,"root", "C"); // Start from the root item
    db.close();
}

void DataBase::importDataFromDB(const QString & pId , const QModelIndex& Parent_index){
    QSqlQuery Selectquery;

    // Use a prepared statement to handle the parameter safely
    Selectquery.prepare("SELECT * FROM Clients WHERE parent_id = :parentId");

    Selectquery.bindValue(":parentId", pId);
    if(Selectquery.exec()){
    while(Selectquery.next()){
            QString nodeId=Selectquery.value(0).toString();
            qDebug()<<nodeId;
            QString name= Selectquery.value(1).toString();
            QString date=Selectquery.value(2).toString();
            QString notes=Selectquery.value(3).toString();

            if(pId=="root"){
                QStandardItem *root = model.invisibleRootItem();
                int rowIndex = root->rowCount();
                //auto nameItem=  new QStandardItem(name);

                root->setChild(rowIndex,0, new QStandardItem(name));
                root->setChild(rowIndex,1, new QStandardItem(date));
                root->setChild(rowIndex,2, new QStandardItem(notes));

                QModelIndex currentIndex= model.index(rowIndex,0,model.indexFromItem(root));
                importDataFromDB(nodeId,currentIndex);
            }
            else{
                auto root=model.itemFromIndex(Parent_index);
                int rowIndex=root->rowCount();
                //auto nameItem=  new QStandardItem(name);

                root->setChild(rowIndex,0, new QStandardItem(name));
                root->setChild(rowIndex,1, new QStandardItem(date));
                root->setChild(rowIndex,2, new QStandardItem(notes));

                QModelIndex currentIndex= model.index(rowIndex,0,model.indexFromItem(root));;
                importDataFromDB(nodeId,currentIndex);
            }
    }
    }
}
void DataBase::load(){
    //connecting to our database
    QString filePath= QFileDialog::getOpenFileName(this,"Choose the Database File",QDir::currentPath(),"SQLite Database Files(*.sqlite *.db)");
    if(!filePath.isEmpty()){
    //removing any existing default connection
    if(QSqlDatabase::contains()){
            QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setConnectOptions(QLatin1StringView("QSQLITE_OPEN_READONLY"));
    db.setDatabaseName(filePath);
    if (!db.open()) {
            QMessageBox::critical(this, "Database Error", "Failed to open database:\n" + db.lastError().text());
    }

    //updating our model
    model.clear();
    QStringList headers;
    headers << "Name" << "Date" << "Notes";
    model.setHorizontalHeaderLabels(headers);

    importDataFromDB("root",QModelIndex());

    db.close();
    }
}

void DataBase::generateExample() {
    QStandardItem *root = model.invisibleRootItem();
    int rowIndex = root->rowCount();

    QString clientName = "DCRUST CSE Department";
    QString notes = "Still building their computer lab. Phone: 9868126590";
    QString year = "2023";
    root->setChild(rowIndex,0, new QStandardItem(clientName));
    root->setChild(rowIndex,1, new QStandardItem(year));
    root->setChild(rowIndex,2, new QStandardItem(notes));

    clientName = "DCRUST Boys Hostel";
    notes = "Decided not to renew JIO WI-FI contract in 2019. \n Email: chiefwarden.boys@dcrustm.org. \n Phone: 9416232747";
    year = "2018";
    root->setChild(rowIndex+1,0, new QStandardItem(clientName));
    root->setChild(rowIndex+1,1, new QStandardItem(year));
    root->setChild(rowIndex+1,2, new QStandardItem(notes));

    QStandardItem *root2 = root->child(rowIndex);

    QString activityName = "Installation of new PCs";
    QString activityNotes = "50 PCs , total cost : Rupees 35,00,000";
    QDate activityDate;
    if (!activityDate.setDate(2019,3,4)) {
        qInfo("Invalid Date!");
        return;
    }
    root2->setChild(0,0, new QStandardItem(activityName));
    root2->setChild(0,1, new QStandardItem(activityDate.toString()));
    root2->setChild(0,2, new QStandardItem(activityNotes));
    activityName = "Installation of GLOBUS Smart Teaching Solutions ";
    activityNotes = "Project delayed due to inefficiency of the client, cost:- Rupees 5,00,000";
    if (!activityDate.setDate(2022,9,5)) {
        qInfo("Invalid Date!");
        return;
    }
    root2->setChild(1,0, new QStandardItem(activityName));
    root2->setChild(1,1, new QStandardItem(activityDate.toString()));
    root2->setChild(1,2, new QStandardItem(activityNotes));
    activityName = "Building new Electric Connections and Pathways";
    activityNotes = " Completed without a hitch, cost:- Rupees10,000";
    if (!activityDate.setDate(2020,6,8)) {
        qInfo("Invalid Date!");
        return;
    }
    QStandardItem *root3 = root2->child(1);
    root3->setChild(0,0, new QStandardItem(activityName));
    root3->setChild(0,1, new QStandardItem(activityDate.toString()));
    root3->setChild(0,2, new QStandardItem(activityNotes));
}


