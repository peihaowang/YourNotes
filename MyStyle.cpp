#include "common_headers.h"
#include "MyStyle.h"

QMyScrollBarStyle::QMyScrollBarStyle(QStyle* base)
	: QProxyStyle(base)
{
}

void QMyScrollBarStyle::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const
{
	switch(cc){
		case CC_ScrollBar:
			drawScrollBar(opt, p, widget);
			break;
		default:
			QProxyStyle::drawComplexControl(cc, opt, p, widget);
			break;
	}
}

void QMyScrollBarStyle::drawScrollBar(const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const
{
	const QStyleOptionSlider* pScrollBarOpt = qstyleoption_cast<const QStyleOptionSlider*>(opt);
	if(pScrollBarOpt){
		if(pScrollBarOpt->maximum != pScrollBarOpt->minimum){
			p->save();
			p->setRenderHint(QPainter::Antialiasing);
//			QRect rect = opt->rect;

//			const State state = opt->state;
//			const SubControls sub = opt->subControls;

//			State flags = opt->state;
//			if(widget && widget->testAttribute(Qt::WA_UnderMouse) && widget->isActiveWindow()){
//				flags |= State_MouseOver;
//			}

			{
				QPalette xPal = widget->palette();

				QRect rtDraw;
				rtDraw= subControlRect(CC_ScrollBar, opt, SC_ScrollBarAddLine, widget);
				p->fillRect(rtDraw, xPal.color(QPalette::Base));

				rtDraw= subControlRect(CC_ScrollBar, opt, SC_ScrollBarSubLine, widget);
				p->fillRect(rtDraw, xPal.color(QPalette::Base));

				rtDraw = subControlRect(CC_ScrollBar, opt, SC_ScrollBarGroove, widget);
				p->fillRect(rtDraw, xPal.color(QPalette::Base));

				rtDraw = subControlRect(CC_ScrollBar, opt, SC_ScrollBarSlider, widget);

				QPainterPath xPath;
				xPath.addRoundedRect(rtDraw.left(), rtDraw.top(), rtDraw.width(), rtDraw.height(), 4, 4);
				p->fillPath(xPath, xPal.color(QPalette::Window));
			}

			p->restore();
		}
	}
}
