#ifndef CALENDAR_H
#define CALENDAR_H

#include <QDialog>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class Calendar; }
QT_END_NAMESPACE

class Calendar : public QDialog
{
	Q_OBJECT
	
public:
	Calendar(QDate databaseDate, QWidget *parent = nullptr);
	~Calendar();
	QDate date;
	
private slots:
	
	void on_btnCancel_clicked();
	
	void on_btnOk_clicked();
	
private:
	Ui::Calendar *ui;
};
#endif // CALENDAR_H
