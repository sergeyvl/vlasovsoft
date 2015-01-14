#include "props.h"
#include "cr3widget.h"

#include "actiondisplayer.h"

ActionDisplayer::ActionDisplayer(QWidget* parent, CR3View* docView, const QString& title)
    : QLabel(parent)
{
    setText(title);
    setAttribute( Qt::WA_DeleteOnClose );
    setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet("border: 1px solid black; background: white");

    int fontSize = 0;
    if ( docView->getOptions()->getInt( PROP_APP_UI_ADISP_FONT_SIZE, fontSize ) )
    {
        QFont f(font());
        f.setPointSize(fontSize);
        setFont(f);
    }

    int time = docView->getOptions()->getIntDef( PROP_APP_UI_ADISP_TIME, 1500 );

    timer.setSingleShot(true);

    QObject::connect( &timer, SIGNAL(timeout()), this, SLOT(close()) );

    raise();

    int height = 2*QFontInfo(font()).pixelSize();

    move(0, parent->height() - height);
    resize(parent->width(), height);

    show();

    timer.start(time);
}
