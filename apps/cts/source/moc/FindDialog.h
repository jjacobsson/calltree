#ifndef FINDDIALOG_H_INCLUDED
#define FINDDIALOG_H_INCLUDED

#include <QtGui/QDialog>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class FindDialog : public QDialog
{
	Q_OBJECT

public:

	FindDialog( QWidget* parent = 0 );

signals:
	void findNext(const QString& str, Qt::CaseSensitivity cs);
	void findPrevious(const QString& str, Qt::CaseSensitivity cs);

private slots:
	void findClicked();
	void enableFindButton(const QString& text);

private:

	QLabel* m_Label;
	QLineEdit* m_LineEdit;
	QCheckBox* m_CaseCheckBox;
	QCheckBox* m_BackwardCheckBox;
	QPushButton* m_FindButton;
	QPushButton* m_CloseButton;

};

#endif


