#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QDebug>

#include "StyleEbook.h"

QRect StyleEbook::subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget ) const
{
    return QStyle::CC_ScrollBar == control ?
        QCommonStyle::subControlRect( control, option, subControl, widget ) :
        QProxyStyle::subControlRect( control, option, subControl, widget );
}

void StyleEbook::drawControl ( ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
{
    switch ( element )
    {
    case QStyle::CE_ScrollBarAddLine:
        {
            QPalette pal(qApp->palette());
            bool hrz = option->state & State_Horizontal;
            int dx = -1;
            int dy = -1;
            painter->setBrush(pal.brush(QPalette::Window));
            painter->drawRect(option->rect.adjusted(0,0,dx,dy));
            drawArrow( painter, option->rect, hrz ? 90:180 );
        }
        break;
    case QStyle::CE_ScrollBarSubLine:
        {
            QPalette pal(qApp->palette());
            bool hrz = option->state & State_Horizontal;
            int dx = hrz? 0:-1;
            int dy = hrz? -1:0;
            painter->setBrush(pal.brush(QPalette::Window));
            painter->drawRect(option->rect.adjusted(0,0,dx,dy));
            drawArrow( painter, option->rect, hrz ? 270:0 );
        }
        break;
    case QStyle::CE_ScrollBarAddPage:
    case QStyle::CE_ScrollBarSubPage:
        {
            QPalette pal(qApp->palette());
            bool hrz = option->state & State_Horizontal;
            int dx = hrz? 0:-1;
            int dy = hrz? -1:0;
            painter->setBrush(pal.brush(QPalette::Window));
            painter->drawRect( option->rect.adjusted(0,0,dx,dy) );
        }
        break;
    case QStyle::CE_ScrollBarSlider:
        {
            QPalette pal(qApp->palette());
            bool hrz = option->state & State_Horizontal;
            int dx = hrz? 0:-1;
            int dy = hrz? -1:0;
            painter->setBrush(pal.brush(QPalette::Window));
            painter->drawRect( option->rect.adjusted(0,0,dx,dy) );
            painter->setBrush(QBrush(Qt::Dense4Pattern));
            painter->drawRect( option->rect.adjusted(0,0,dx,dy) );
        }
        break;
    case QStyle::CE_ScrollBarFirst:
    case QStyle::CE_ScrollBarLast:
        break;
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
        break;
    }
}

void StyleEbook::drawArrow(QPainter* painter, const QRect& r, qreal rotation ) const
{
    painter->save();
    QPoint c(r.center());
    painter->translate(c);
    painter->rotate(rotation);
    int size = r.width()/6;
    QPolygon points(3);
    points[0] = QPoint(-2*size, size);
    points[1] = QPoint(0, -size);
    points[2] = QPoint(2*size, size);
    painter->setBrush(QBrush(Qt::black));
    painter->drawPolygon(points);
    painter->restore();
}
