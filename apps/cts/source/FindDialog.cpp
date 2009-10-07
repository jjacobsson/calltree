#include <QtGui/QtGui>

#include "FindDialog.h"

FindDialog::FindDialog( QWidget* parent )
	: QDialog( parent )
{
	m_Label = new QLabel( tr("Find &what:") );
	m_LineEdit = new QLineEdit;
	m_Label->setBuddy( m_LineEdit );
	m_CaseCheckBox = new QCheckBox( tr("Match &case") );
	m_BackwardCheckBox = new QCheckBox( tr("Search &backward") );

	m_FindButton = new QPushButton( tr("&Find") );
	m_FindButton->setDefault(true);
	m_FindButton->setEnabled(false);

	m_CloseButton = new QPushButton( tr("close") );

	connect(m_LineEdit, SIGNAL(textChanged(const QString&)),
			this, SLOT(enableFindButton(const QString&)));
	connect(m_FindButton, SIGNAL(clicked()),
			this, SLOT(findClicked()));
	connect(m_CloseButton, SIGNAL(clicked()),
			this, SLOT(close()));

	QHBoxLayout *topLeftLayout = new QHBoxLayout;
	topLeftLayout->addWidget(m_Label);
	topLeftLayout->addWidget(m_LineEdit);

	QVBoxLayout *leftLayout = new QVBoxLayout;
	leftLayout->addLayout(topLeftLayout);
	leftLayout->addWidget(m_CaseCheckBox);
	leftLayout->addWidget(m_BackwardCheckBox);

	QVBoxLayout *rightLayout = new QVBoxLayout;
	rightLayout->addWidget(m_FindButton);
	rightLayout->addWidget(m_CloseButton);
	rightLayout->addStretch();

	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addLayout(leftLayout);
	mainLayout->addLayout(rightLayout);

	setLayout(mainLayout);
	setWindowTitle( tr("Find") );
	setFixedHeight(sizeHint().height());
}

void FindDialog::findClicked()
{
    QString text = m_LineEdit->text();
    Qt::CaseSensitivity cs =m_CaseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    if (m_BackwardCheckBox->isChecked())
        emit findPrevious(text, cs);
    else
        emit findNext(text, cs);
}

void FindDialog::enableFindButton(const QString &text)
{
    m_FindButton->setEnabled(!text.isEmpty());
}
