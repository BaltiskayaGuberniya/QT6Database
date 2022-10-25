#include "database.h"
#include "./ui_database.h"
#include "calendar.h"
#include <QDate>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItem>

//TO DO:
// LET SAVE TO LOCAL FILE
// LET SAVE TO NETWORK


DataBase::DataBase(QWidget *parent) : QMainWindow(parent), ui(new Ui::DataBase) {
	ui->setupUi(this);
    connect(ui->btnAddClient, &QPushButton::clicked, this, &DataBase::addClient);
	connect(ui->btnAddActivity, &QPushButton::clicked, this, &DataBase::addActivity);
    connect(ui->btnRemoveClient, &QPushButton::clicked, this, &DataBase::removeItem);
    connect(ui->btnRemoveActivity, &QPushButton::clicked, this, &DataBase::removeItem);
	connect(ui->btnCalendar, &QPushButton::clicked, this, &DataBase::openCalendar);

    connect(ui->actionGenerate_Example, &QAction::triggered, this, &DataBase::generateExample);
    connect(ui->actionRemove_Selected, &QAction::triggered, this, &DataBase::removeItem);
	
    connect(ui->leditClient, &QLineEdit::textChanged, this, &DataBase::updateWidgetText);
    connect(ui->spinYearClient, &QSpinBox::textChanged, this, &DataBase::updateWidgetText);
    connect(ui->notesClient, &QPlainTextEdit::textChanged, this, &DataBase::updateWidgetText);
    connect(ui->leditActivity, &QLineEdit::textChanged, this, &DataBase::updateWidgetText);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &DataBase::updateWidgetText);
    connect(ui->notesActivity, &QPlainTextEdit::textChanged, this, &DataBase::updateWidgetText);

	//sets the headers
	QStringList headers;
	headers << "Name" << "Date" << "Notes";
    model.setHorizontalHeaderLabels(headers);
    auto header=ui->treeView->header();

	ui->treeView->setModel(&model);
	
	//gets the current year and set it for the spinbox. get current date and set it for datebox
	QString date = QDate::currentDate().toString();
	date = date.sliced(date.size()-4,4);
    int year = date.toInt();
	ui->spinYearClient->setValue(year);
	ui->dateEdit->setDate(QDate::currentDate());
}

DataBase::~DataBase() {
	delete ui;
}

void DataBase::addClient() {
	QStandardItem *root = model.invisibleRootItem();
	int rowIndex = root->rowCount();
	QString clientName = ui->leditClient->text();
    if (clientName == "") {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("You need to set a name!");
        msgBox.setWindowTitle("Name!");
        msgBox.exec();
        return;
    }
	QString notes = ui->notesClient->toPlainText();
	QString year = ui->spinYearClient->text();
	
	root->setChild(rowIndex,0, new QStandardItem(clientName));
	root->setChild(rowIndex,1, new QStandardItem(year));
	root->setChild(rowIndex,2, new QStandardItem(notes));
	
	
}

void DataBase::addActivity() {
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


void DataBase::on_treeView_pressed(const QModelIndex &index) {
    if (index.column() == 0 || index.column() == 1 || index.column() == 2 ) {
        char type;
        _selectedIndex = index.siblingAtColumn(0);
        setWidgetText();
    }
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

void DataBase::generateExample() {
    QStandardItem *root = model.invisibleRootItem();
    int rowIndex = root->rowCount();

    QString clientName = "Bob's Farm";
    QString notes = "Decided not to renew our contract in 2021. \n Email: Bob.farm@email.com. \n Phone: 99993123";
    QString year = "2020";
    root->setChild(rowIndex,0, new QStandardItem(clientName));
    root->setChild(rowIndex,1, new QStandardItem(year));
    root->setChild(rowIndex,2, new QStandardItem(notes));

    clientName = "Alan's Farm";
    notes = "Still organizing activities. Phone: 93923121";
    year = "2022";
    root->setChild(rowIndex+1,0, new QStandardItem(clientName));
    root->setChild(rowIndex+1,1, new QStandardItem(year));
    root->setChild(rowIndex+1,2, new QStandardItem(notes));

    QStandardItem *root2 = root->child(rowIndex);

    QString activityName = "Mapping the Farm";
    QString activityNotes = "500 HA, 600 cows";
    QDate activityDate;
    if (!activityDate.setDate(2020,3,4)) {
        qInfo("Invalid Date!");
        return;
    }
    root2->setChild(0,0, new QStandardItem(activityName));
    root2->setChild(0,1, new QStandardItem(activityDate.toString()));
    root2->setChild(0,2, new QStandardItem(activityNotes));
    activityName = "Building Fences";
    activityNotes = "Went without a hitch, cost 50000$";
    if (!activityDate.setDate(2020,5,5)) {
        qInfo("Invalid Date!");
        return;
    }
    root2->setChild(1,0, new QStandardItem(activityName));
    root2->setChild(1,1, new QStandardItem(activityDate.toString()));
    root2->setChild(1,2, new QStandardItem(activityNotes));
    activityName = "Building Electric Fences";
    activityNotes = "Electrified without a hitch, cost 10000$";
    if (!activityDate.setDate(2020,6,8)) {
        qInfo("Invalid Date!");
        return;
    }
    QStandardItem *root3 = root2->child(1);
    root3->setChild(0,0, new QStandardItem(activityName));
    root3->setChild(0,1, new QStandardItem(activityDate.toString()));
    root3->setChild(0,2, new QStandardItem(activityNotes));
}

