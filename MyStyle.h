#ifndef MYSTYLE_H
#define MYSTYLE_H

#include <QProxyStyle>

class QMyScrollBarStyle : public QProxyStyle
{

	Q_OBJECT

public:

	explicit QMyScrollBarStyle(QStyle* base = 0);

public:

	virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const;
//	virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const;

protected:

	void drawScrollBar(const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const;

signals:

public slots:

};

#endif // MYSTYLE_H
