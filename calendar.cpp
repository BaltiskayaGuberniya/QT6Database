#include "calendar.h"
#include "./ui_calendar.h"

Calendar::Calendar(QDate databaseDate, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::Calendar)
{
	ui->setupUi(this);
	
	if (databaseDate.isValid()) ui->calendarWidget->setSelectedDate(databaseDate);
}


Calendar::~Calendar()
{
	delete ui;
}



void Calendar::on_btnCancel_clicked()
{
    reject();
}


void Calendar::on_btnOk_clicked()
{
	if (ui->calendarWidget->selectedDate().isValid()) date = ui->calendarWidget->selectedDate();
    accept();   
}

