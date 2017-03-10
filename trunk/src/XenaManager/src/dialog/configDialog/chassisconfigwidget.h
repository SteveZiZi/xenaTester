#ifndef CHASSISCONFIGWIDGET_H
#define CHASSISCONFIGWIDGET_H

#include <QWidget>
#include "XenaManager/src/dialog/configDialog/baseconfigwidget.h"

class CXenaTester;
class ChassisConfigWidget : public BaseConfigWidget
{
	Q_OBJECT

public:
	ChassisConfigWidget(CXenaTester *xenaTester,QWidget *parent = 0);
	virtual ~ChassisConfigWidget();
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

#endif // CHASSISCONFIGWIDGET_H
