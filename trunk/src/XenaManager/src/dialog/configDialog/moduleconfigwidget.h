#ifndef MODULECONFIGWIDGET_H
#define MODULECONFIGWIDGET_H

#include <QWidget>
#include "XenaManager/src/dialog/configDialog/baseconfigwidget.h"

class CXenaTester;
class ModuleConfigWidget : public BaseConfigWidget
{
	Q_OBJECT

public:
	ModuleConfigWidget(CXenaTester *xenaTester,QWidget *parent = 0);
	virtual ~ModuleConfigWidget();
	//override from BaseConfigWidget
	virtual void updateConfigWidget();

protected:
	//override from BaseConfigWidget
	virtual void initUI();

private:
	void setMainLayout();

private:
	CXenaTester			*m_pXenaTester;
};

#endif // MODULECONFIGWIDGET_H
