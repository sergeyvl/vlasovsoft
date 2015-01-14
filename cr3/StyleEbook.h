#ifndef STYLEEBOOK_H_
#define STYLEEBOOK_H_

#include <QIcon>
#include <QProxyStyle>

class StyleEbook: public QProxyStyle
{
    int scrollBarSize;
    int buttonIconSize;

public:
	StyleEbook( QStyle* style )
	    : QProxyStyle(style)
        , scrollBarSize(40)
        , buttonIconSize(48)
	{}

    void setScrollBarSize( int val )
    { scrollBarSize = val; }

    void setButtonIconSize( int val )
    { buttonIconSize = val; }

	virtual ~StyleEbook()
	{}

        virtual int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const
        {
            switch ( metric )
            {
            case PM_ScrollBarExtent:
                return scrollBarSize;
            case PM_LayoutHorizontalSpacing:
                return 10;
            case PM_LayoutVerticalSpacing:
                return 10;
            case PM_LayoutLeftMargin:
            case PM_LayoutRightMargin:
            case PM_LayoutTopMargin:
            case PM_LayoutBottomMargin:
                return 10;
            case PM_ButtonIconSize:
                return buttonIconSize;
            default:
                return QProxyStyle::pixelMetric(metric, option, widget);
            }
        }

    virtual QRect subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget = 0 ) const;
    virtual void drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const;

private:
    void drawArrow( QPainter* painter, const QRect& rect, qreal rotation ) const;
};

#endif /* STYLEEBOOK_H_ */
