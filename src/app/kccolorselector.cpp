
#include <QDebug>

#include <QGradient>
#include <QRadioButton>
#include <QSlider>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QBoxLayout>
#include <QLinearGradient>
#include <QConicalGradient>
#include <QRadialGradient>
#include <QPainter>

#include "kccolorselector.h"


KCColorDoubleBoardBase::KCColorDoubleBoardBase(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(259,259);
    setAutoFillBackground(true);
    hsvGreyGradient.setStart(0,0);
}

void KCColorDoubleBoardBase::setColor(const QString &elementName,
                                      const QColor &color)
{
    if(elementName=="S:")
    {
        currentMode=saturationMode;
    }
    else if(elementName=="V:")
    {
        currentMode=valueMode;
    }
    else if(elementName=="H:")
    {
        currentMode=hueMode;
    }
    else if(elementName=="R:")
    {
        currentMode=redMode;
    }
    syncColor(color);
}

void KCColorDoubleBoardBase::syncColor(const QColor &color)
{
    if(currentMode==saturationMode)
    {
        saturationGradient=QLinearGradient(0,0,width(),0);
        QColor color0, color16, color33, color50, color66, color83, color100;
        color0.setHsv(0, color.saturation(), 255);
        color16.setHsv(60, color.saturation(), 255);
        color33.setHsv(120, color.saturation(), 255);
        color50.setHsv(180, color.saturation(), 255);
        color66.setHsv(240, color.saturation(), 255);
        color83.setHsv(300, color.saturation(), 255);
        color100.setHsv(0, color.saturation(), 255);
        saturationGradient.setColorAt(0.00, color0);
        saturationGradient.setColorAt(0.16, color16);
        saturationGradient.setColorAt(0.33, color33);
        saturationGradient.setColorAt(0.50, color50);
        saturationGradient.setColorAt(0.66, color66);
        saturationGradient.setColorAt(0.83, color83);
        saturationGradient.setColorAt(1.00, color100);
        hsvGreyGradient.setColorAt(0, QColor(0,0,0,0));
        hsvGreyGradient.setColorAt(1, QColor(0,0,0,255));
    }
    else if(currentMode==valueMode)
    {
        saturationGradient=QLinearGradient(0,0,width(),0);
        QColor color0, color16, color33, color50, color66, color83, color100;
        color0.setHsv(0, 255, color.value());
        color16.setHsv(60, 255, color.value());
        color33.setHsv(120, 255, color.value());
        color50.setHsv(180, 255, color.value());
        color66.setHsv(240, 255, color.value());
        color83.setHsv(300, 255, color.value());
        color100.setHsv(0, 255, color.value());
        saturationGradient.setColorAt(0.00, color0);
        saturationGradient.setColorAt(0.16, color16);
        saturationGradient.setColorAt(0.33, color33);
        saturationGradient.setColorAt(0.50, color50);
        saturationGradient.setColorAt(0.66, color66);
        saturationGradient.setColorAt(0.83, color83);
        saturationGradient.setColorAt(1.00, color100);
        hsvGreyGradient.setColorAt(0, QColor(255,255,255,0));
        hsvGreyGradient.setColorAt(1, QColor(255,255,255,255));
    }
    else if(currentMode==hueMode)
    {
        hueLevelGradient=QRadialGradient(width(),0,width(),width(),0);
        QColor startColor, endColor;
        startColor.setHsv(color.hue(), 255, 255);
        endColor.setHsv(color.hue(), 0, 255);
        hueLevelGradient.setColorAt(0, startColor);
        hueLevelGradient.setColorAt(1, endColor);
        hsvGreyGradient.setColorAt(0, QColor(0,0,0,0));
        hsvGreyGradient.setColorAt(1, QColor(0,0,0,255));
    }
    else if(currentMode==redMode)
    {
        rgbHorizontalGradient=QLinearGradient(0,0,width(),0);
        rgbHorizontalGradient.setColorAt(0,QColor(color.red(),0,0));
        rgbHorizontalGradient.setColorAt(1,QColor(color.red(),0,255));
        rgbVerticalGradient=QLinearGradient(0,0,0,width());
        rgbVerticalGradient.setColorAt(0,QColor(color.red(),255,0));
        rgbVerticalGradient.setColorAt(1,QColor(color.red(),0,0));
    }
    update();
}

void KCColorDoubleBoardBase::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    QRect renderRect(2,2,255,255);
    painter.setPen(QColor(0xcf, 0xcf, 0xcf));
    //painter.drawRect(0,0,258,258);
    painter.setPen(Qt::NoPen);
    switch(currentMode)
    {
    case hueMode:
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QBrush(hueLevelGradient));
        painter.drawRect(renderRect);
        painter.setBrush(QBrush(hsvGreyGradient));
        painter.drawRect(renderRect);
        break;
    case saturationMode:
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QBrush(saturationGradient));
        painter.drawRect(renderRect);
        painter.setBrush(QBrush(hsvGreyGradient));
        painter.drawRect(renderRect);
        break;
    case valueMode:
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QBrush(saturationGradient));
        painter.drawRect(renderRect);
        painter.setBrush(QBrush(hsvGreyGradient));
        painter.drawRect(renderRect);
        break;
    case redMode:
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QBrush(rgbHorizontalGradient));
        painter.drawRect(renderRect);
        painter.setCompositionMode(QPainter::CompositionMode_Plus);
        painter.setBrush(QBrush(rgbVerticalGradient));
        painter.drawRect(renderRect);
    }
}

void KCColorDoubleBoardBase::resizeEvent(QResizeEvent *event)
{
    int widthElement=event->size().width(),
        heightElement=event->size().height();
    if(widthElement!=heightElement)
    {
        int minimumElement=heightElement<widthElement?
                    heightElement:
                    widthElement;
        event->ignore();
        resize(minimumElement,minimumElement);
        return;
    }
    hueLevelGradient.setCenter(widthElement,0);
    hueLevelGradient.setFocalPoint(widthElement,0);
    hueLevelGradient.setRadius(widthElement);
    hsvGreyGradient.setFinalStop(0,widthElement);
    saturationGradient.setFinalStop(widthElement,0);
    rgbHorizontalGradient.setFinalStop(widthElement, 0);
    rgbVerticalGradient.setFinalStop(0, widthElement);
    QWidget::resizeEvent(event);
}

KCColorRingBoard::KCColorRingBoard(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(424,424);
    ringGradient.setColorAt(0.00, QColor(255,0,0));
    ringGradient.setColorAt(0.16, QColor(255,0,255));
    ringGradient.setColorAt(0.33, QColor(0,0,255));
    ringGradient.setColorAt(0.50, QColor(0,255,255));
    ringGradient.setColorAt(0.66, QColor(0,255,0));
    ringGradient.setColorAt(0.83, QColor(255,255,0));
    ringGradient.setColorAt(1.00, QColor(255,0,0));
}

void KCColorRingBoard::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QRect paintRect(2,2,width()-4,width()-4);
    QPainter painter(this);
    painter.setPen(QColor(0xcf, 0xcf, 0xcf));
    painter.drawEllipse(0,0,width()-1,width()-1);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(ringGradient));
    painter.drawEllipse(paintRect);
    painter.setBrush(QBrush(palette().color(QPalette::Window)));
    paintRect.setTopLeft(QPoint(ringWidth-1, ringWidth-1));
    paintRect.setBottomRight(QPoint(width()-ringWidth+1,
                                    width()-ringWidth+1));
    painter.drawEllipse(paintRect);
    painter.setPen(QColor(0xcf, 0xcf, 0xcf));
    paintRect.setTopLeft(QPoint(ringWidth, ringWidth));
    paintRect.setBottomRight(QPoint(width()-ringWidth,
                                    width()-ringWidth));
    painter.drawEllipse(paintRect);
}

void KCColorRingBoard::resizeEvent(QResizeEvent *event)
{
    int widthElement=event->size().width(),
        heightElement=event->size().height();
    if(widthElement!=heightElement)
    {
        int minimumElement=heightElement<widthElement?
                    heightElement:
                    widthElement;
        event->ignore();
        resize(minimumElement,minimumElement);
        return;
    }
    ringGradient.setCenter(widthElement/2,
                           widthElement/2);
    QWidget::resizeEvent(event);
}

KCColorHSVRing::KCColorHSVRing(QWidget *parent) :
    QWidget(parent)
{
    setContentsMargins(0,0,0,0);
    setFixedSize(424,424);
    ringBoard=new KCColorRingBoard(this);
    doubleBoard=new KCColorDoubleBoardBase(this);
    doubleBoard->move(83,83);
}

void KCColorHSVRing::setColorMode(QString elementName,
                                  int value,
                                  QColor color)
{
    doubleBoard->setColor(elementName,
                          color);
}

void KCColorHSVRing::syncColor(QColor color)
{
    doubleBoard->syncColor(color);
}

/*!
 *\brief KCColorLevelRenderBase is a color slider.
 */
KCColorLevelRenderBase::KCColorLevelRenderBase(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(260,25);
    levelGradient.setStart(0,0);
    levelGradient.setFinalStop(width(),
                               height());
}

void KCColorLevelRenderBase::renderRed(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    levelGradient.setColorAt(0.0, QColor(0,color.green(),color.blue()));
    levelGradient.setColorAt(1.0, QColor(255,color.green(),color.blue()));
    update();
}

void KCColorLevelRenderBase::renderGreen(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    levelGradient.setColorAt(0.0, QColor(color.red(),0,color.blue()));
    levelGradient.setColorAt(1.0, QColor(color.red(),255,color.blue()));
    update();
}

void KCColorLevelRenderBase::renderBlue(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    levelGradient.setColorAt(0.0, QColor(color.red(),color.green(),0));
    levelGradient.setColorAt(1.0, QColor(color.red(),color.green(),255));
    update();
}

void KCColorLevelRenderBase::renderHue(const QColor &color)
{
    Q_UNUSED(color);
    levelGradient=QLinearGradient(0,0,width(),height());
    levelGradient.setColorAt(0.00, QColor(255,0,0));
    levelGradient.setColorAt(0.16, QColor(255,255,0));
    levelGradient.setColorAt(0.33, QColor(0,255,0));
    levelGradient.setColorAt(0.50, QColor(0,255,255));
    levelGradient.setColorAt(0.66, QColor(0,0,255));
    levelGradient.setColorAt(0.83, QColor(255,0,255));
    levelGradient.setColorAt(1.00, QColor(255,0,0));
    update();
}

void KCColorLevelRenderBase::renderSaturation(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    QColor startColor, endColor;
    startColor.setHsv(color.hue(), 0, color.value());
    endColor.setHsv(color.hue(), 255, color.value());
    levelGradient.setColorAt(0.0, startColor);
    levelGradient.setColorAt(1.0, endColor);
    update();
}

void KCColorLevelRenderBase::renderValue(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    QColor startColor, endColor;
    startColor.setHsv(color.hue(), color.saturation(), 0);
    endColor.setHsv(color.hue(), color.saturation(), 255);
    levelGradient.setColorAt(0.0, startColor);
    levelGradient.setColorAt(1.0, endColor);
    update();
}

void KCColorLevelRenderBase::renderLightness(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    QColor startColor, endColor;
    startColor.setHsl(color.hue(), color.saturation(), 0);
    endColor.setHsl(color.hue(), color.saturation(), 255);
    levelGradient.setColorAt(0.0, startColor);
    levelGradient.setColorAt(1.0, endColor);
    update();
}

void KCColorLevelRenderBase::renderCyan(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    QColor startColor, endColor;
    startColor.setCmyk(0, color.magenta(), color.yellow(), color.black());
    endColor.setCmyk(255, color.magenta(), color.yellow(), color.black());
    levelGradient.setColorAt(0.0, startColor);
    levelGradient.setColorAt(1.0, endColor);
    update();
}

void KCColorLevelRenderBase::renderMagenta(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    QColor startColor, endColor;
    startColor.setCmyk(color.cyan(), 0, color.yellow(), color.black());
    endColor.setCmyk(color.cyan(), 255, color.yellow(), color.black());
    levelGradient.setColorAt(0.0, startColor);
    levelGradient.setColorAt(1.0, endColor);
    update();
}

void KCColorLevelRenderBase::renderYellow(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    QColor startColor, endColor;
    startColor.setCmyk(color.cyan(), color.magenta(), 0, color.black());
    endColor.setCmyk(color.cyan(), color.magenta(), 255, color.black());
    levelGradient.setColorAt(0.0, startColor);
    levelGradient.setColorAt(1.0, endColor);
    update();
}

void KCColorLevelRenderBase::renderBlack(const QColor &color)
{
    levelGradient=QLinearGradient(0,0,width(),height());
    QColor startColor, endColor;
    startColor.setCmyk(color.cyan(), color.magenta(), color.yellow(), 0);
    endColor.setCmyk(color.cyan(), color.magenta(), color.yellow(), 255);
    levelGradient.setColorAt(0.0, startColor);
    levelGradient.setColorAt(1.0, endColor);
    update();
}

void KCColorLevelRenderBase::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setPen(QColor(0xcf, 0xcf, 0xcf));
    painter.drawRect(QRect(0,0,259,23));
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(levelGradient));
    painter.drawRect(QRect(2,2,256,20));
}

void KCColorLevelRenderBase::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    levelGradient.setFinalStop(width(),
                               height());
}

void KCColorLevelRenderBase::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    emit sliderPressed();
    isPressed=true;
    int haruka=event->pos().x()-2;
    if(haruka>-1 && haruka<257)
    {
        emit valueChanged(haruka);
    }
}

void KCColorLevelRenderBase::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if(isPressed)
    {
        int haruka=event->pos().x()-2;
        if(haruka<0)
        {
            emit valueChanged(0);
        }
        else if(haruka>255)
        {
            emit valueChanged(255);
        }
        else
        {
            emit valueChanged(haruka);
        }
    }
}

void KCColorLevelRenderBase::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    isPressed=false;
    emit sliderReleased();
}

KCColorLevelSelector::KCColorLevelSelector(QWidget *parent) :
    QWidget(parent)
{
    colorRender=new KCColorLevelRenderBase(this);
    colorRender->move(2,8);
    QPixmap colorPointer(":/img/image/ColorPointer.png");
    colorSelector=new QLabel(this);
    colorSelector->setPixmap(colorPointer);
    setFixedSize(264,colorPointer.height());

    //Selector will update it self when the parent call the update itself.

    connect(colorRender, &KCColorLevelRenderBase::sliderPressed,
            this, &KCColorLevelSelector::sliderPressed);
    connect(colorRender, &KCColorLevelRenderBase::sliderReleased,
            this, &KCColorLevelSelector::sliderReleased);
    connect(colorRender, &KCColorLevelRenderBase::valueChanged,
            this, &KCColorLevelSelector::valueUpdate);
}

void KCColorLevelSelector::focusOnElement(QString elementName)
{
    if(elementName=="H:")
    {
        currentMode=hueMode;
    }
    else if(elementName=="R:")
    {
        currentMode=redMode;
    }
    else if(elementName=="G:")
    {
        currentMode=greenMode;
    }
    else if(elementName=="B:")
    {
        currentMode=blueMode;
    }
    else if(elementName=="S:")
    {
        currentMode=saturationMode;
    }
    else if(elementName=="V:")
    {
        currentMode=valueMode;
    }
    else if(elementName=="C:")
    {
        currentMode=cyanMode;
    }
    else if(elementName=="M:")
    {
        currentMode=magentaMode;
    }
    else if(elementName=="Y:")
    {
        currentMode=yellowMode;
    }
    else if(elementName=="K:")
    {
        currentMode=blackMode;
    }
}

void KCColorLevelSelector::colorUpdate(const QColor &color)
{
    currentColor=color;
    switch(currentMode)
    {
    case redMode:
        colorRender->renderRed(color);
        slide(color.red());
        break;
    case greenMode:
        colorRender->renderGreen(color);
        slide(color.green());
        break;
    case blueMode:
        colorRender->renderBlue(color);
        slide(color.blue());
        break;
    case hueMode:
        colorRender->renderHue(color);
        slide(color.hue());
        break;
    case saturationMode:
        colorRender->renderSaturation(color);
        slide(color.saturation());
        break;
    case valueMode:
        colorRender->renderValue(color);
        slide(color.value());
        break;
    case cyanMode:
        colorRender->renderCyan(color);
        slide(color.cyan());
        break;
    case magentaMode:
        colorRender->renderMagenta(color);
        slide(color.magenta());
        break;
    case yellowMode:
        colorRender->renderYellow(color);
        slide(color.yellow());
        break;
    case blackMode:
        colorRender->renderBlack(color);
        slide(color.black());
        break;
    }
}

void KCColorLevelSelector::setValue(int value)
{
    slide(value);
}

int KCColorLevelSelector::getValue()
{
    int outPosition=colorSelector->x();
    if(maximum!=255)
    {
        outPosition=outPosition*maximum/255;
    }
    return outPosition;
}

void KCColorLevelSelector::slide(int position)
{
    int innerPosition=position;
    if(maximum!=255)
    {
        innerPosition=position*255/maximum;
    }
    colorSelector->move(innerPosition, 0);
}

void KCColorLevelSelector::valueUpdate(int element)
{
    int outPosition=element;
    if(maximum!=255)
    {
        outPosition=outPosition*maximum/255;
    }
    emit valueChanged(outPosition);
}

void KCColorLevelSelector::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}
int KCColorLevelSelector::getMaximum() const
{
    return maximum;
}

void KCColorLevelSelector::setMaximum(int value)
{
    maximum = value;
}

KCColorSpinBox::KCColorSpinBox(QWidget *parent) :
    QSpinBox(parent)
{
    ;
}

void KCColorSpinBox::mousePressEvent(QMouseEvent *event)
{
    QSpinBox::mousePressEvent(event);
    emit spinPressed();
}

KCColorViewerBase::KCColorViewerBase(QWidget *parent) :
    QWidget(parent)
{
    currentViewer=new QWidget(this);
    currentViewer->setAutoFillBackground(true);
    originalViewer=new QWidget(this);
    originalViewer->setAutoFillBackground(true);
    currentPalette=currentViewer->palette();
    buildViewer();
}

void KCColorViewerBase::buildViewer()
{
    mainLayout=new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    QLabel *currentColor=new QLabel(tr("Current Color"), this);
    mainLayout->addWidget(currentColor,0,Qt::AlignCenter);
    mainLayout->addWidget(currentViewer,1);
    mainLayout->addWidget(originalViewer,1);
    QLabel *originalColor=new QLabel(tr("Original Color"), this);
    mainLayout->addWidget(originalColor,0,Qt::AlignCenter);
}

void KCColorViewerBase::setOriginalColor(QColor color)
{
    QPalette pal=originalViewer->palette();
    pal.setColor(QPalette::Window, color);
    originalViewer->setPalette(pal);
}

void KCColorViewerBase::setCurrentColor(QColor color)
{
    currentPalette.setColor(QPalette::Window, color);
    currentViewer->setPalette(currentPalette);
}

KCColorSliderItemBase::KCColorSliderItemBase(QWidget *parent,
                                             bool autoBuild) :
    QWidget(parent)
{
    mainLayout=new QBoxLayout(QBoxLayout::LeftToRight, this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(3);
    setLayout(mainLayout);

    elementCaption=new QRadioButton(this);
    elementSpinBox=new KCColorSpinBox(this);
    elementSpinBox->setRange(0, 255);
    levelSelector=new KCColorLevelSelector(this);

    connect(elementCaption, SIGNAL(clicked()),
            this, SLOT(onActionClickElement()));

    if(autoBuild)
    {
        buildSlider();
    }
}

void KCColorSliderItemBase::buildSlider()
{
    mainLayout->addWidget(elementCaption);
    mainLayout->addWidget(elementSpinBox);
    mainLayout->addWidget(levelSelector);
    connect(elementSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(setValue(int)));
    connect(levelSelector, SIGNAL(valueChanged(int)),
            this, SLOT(setValue(int)));
    connect(elementSpinBox, SIGNAL(spinPressed()),
            this, SLOT(onActionClickElement()));
    connect(levelSelector, SIGNAL(sliderPressed()),
            this, SLOT(onActionClickElement()));
}

void KCColorSliderItemBase::setCaption(const QString &value)
{
    elementCaption->setText(value);
    levelSelector->focusOnElement(value);
}

int KCColorSliderItemBase::getValue()
{
    return levelSelector->getValue();
}

void KCColorSliderItemBase::setMaximum(const int &value)
{
    elementSpinBox->setRange(0, value);
    levelSelector->setMaximum(value);
}

void KCColorSliderItemBase::syncValue(const int &value,
                                      const QColor &color)
{
    //The new color must rerender.
    levelSelector->colorUpdate(color);
    //Check the signal sender.
    if(syncRequireSentByMe)
    {
        syncRequireSentByMe=false;
        return;
    }
    //Sync all the value.
    syncMode=true;
    setValue(value);
    syncMode=false;
}

void KCColorSliderItemBase::setValue(int value)
{
    if(valueSetMode)
    {
        return;
    }
    valueSetMode=true;
    elementSpinBox->setValue(value);
    levelSelector->setValue(value);
    syncRequireSentByMe=true;
    if(!syncMode)
    {
        emit requireElementChanged(elementCaption->text(),
                                   value);
    }
    valueSetMode=false;
}

void KCColorSliderItemBase::setEraseFocus()
{
    if(focusSignalSent)
    {
        focusSignalSent=false;
    }
    else
    {
        elementCaption->setChecked(false);
    }
}

void KCColorSliderItemBase::onActionClickElement()
{
    elementCaption->setChecked(true);
    focusSignalSent=true;
    emit requireFocusOnElement(elementCaption->text(),
                               elementSpinBox->value());
}

KCColorSliderItemPercent::KCColorSliderItemPercent(QWidget *parent) :
    KCColorSliderItemBase(parent, false)
{
    elementSpinBox->setRange(0, 100);
    buildSlider();
}

void KCColorSliderItemPercent::buildSlider()
{
    percentCaption=new QLabel("%", this);
    mainLayout->addWidget(elementCaption);
    mainLayout->addWidget(elementSpinBox);
    mainLayout->addWidget(percentCaption);
    mainLayout->addWidget(levelSelector);
    levelSelector->setVisible(false);
    connect(elementSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(setValue(int)));
    connect(levelSelector, SIGNAL(valueChanged(int)),
            this, SLOT(setValueFromLevel(int)));
    connect(elementSpinBox, SIGNAL(spinPressed()),
            this, SLOT(onActionClickElement()));
    connect(levelSelector, SIGNAL(sliderPressed()),
            this, SLOT(onActionClickElement()));
}

void KCColorSliderItemPercent::syncValue(const int &value,
                                      const QColor &color)
{
    if(syncRequireSentByMe)
    {
        syncRequireSentByMe=false;
    }
    syncMode=true;
    levelSelector->colorUpdate(color);
    setValueFromLevel(value);
    syncMode=false;
}

void KCColorSliderItemPercent::setValue(int value)
{
    /*
     * This value is greater than 0 and smaller than 100, translate this value
     * to 0-255, set to levelSelector.
     * All the signal will be the 255 mode, so save the result at the beginning.
     */
    if(valueSetMode)
    {
        return;
    }
    int irori=value*51/20;
    valueSetMode=true;
    elementSpinBox->setValue(value);
    levelSelector->setValue(irori);
    syncRequireSentByMe=true;
    if(!syncMode)
    {
        emit requireElementChanged(elementCaption->text(),
                                   irori);
    }
    valueSetMode=false;
}

void KCColorSliderItemPercent::setValueFromLevel(int value)
{
    /*
     * This value is greater than 0 and smaller than 255, translate this value
     * to 0-100, set to elementSpinBox.
     * emit value as the param of the signal.
     */
    if(valueSetMode)
    {
        return;
    }
    valueSetMode=true;
    elementSpinBox->setValue(value*20/51);
    levelSelector->setValue(value);
    syncRequireSentByMe=true;
    if(!syncMode)
    {
        emit requireElementChanged(elementCaption->text(),
                                   value);
    }
    valueSetMode=false;
}

KCColorSliderBase::KCColorSliderBase(QWidget *parent) :
    QWidget(parent)
{
    ;
}

KCColorSliderItemBase *KCColorSliderBase::addElement(const QString &name)
{
    KCColorSliderItemBase *currentElement=new KCColorSliderItemBase(this);
    currentElement->setCaption(name);
    connect(currentElement, &KCColorSliderItemBase::requireFocusOnElement,
            this, &KCColorSliderBase::focusRequire);
    connect(this, &KCColorSliderBase::requireClearElementsFocus,
            currentElement, &KCColorSliderItemBase::setEraseFocus);
    return currentElement;
}

void KCColorSliderBase::syncColor(QColor color)
{
    //Just save the color, all the other things should do in the inhreit class.
    currentColor=color;
}

void KCColorSliderBase::focusRequire(QString elementName,
                                     int value)
{
    emit requireFocusOnElement(elementName, value, currentColor);
}

KCColorSliderCMYKP::KCColorSliderCMYKP(QWidget *parent) :
    KCColorSliderBase(parent)
{
    mainLayout=new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    cyanElement=addElement("C:");
    magentaElement=addElement("M:");
    yellowElement=addElement("Y:");
    blackElement=addElement("K:");

    connect(cyanElement, &KCColorSliderItemPercent::requireElementChanged,
            this, &KCColorSliderCMYKP::onElementChanged);
    connect(magentaElement, &KCColorSliderItemPercent::requireElementChanged,
            this, &KCColorSliderCMYKP::onElementChanged);
    connect(yellowElement, &KCColorSliderItemPercent::requireElementChanged,
            this, &KCColorSliderCMYKP::onElementChanged);
    connect(blackElement, &KCColorSliderItemPercent::requireElementChanged,
            this, &KCColorSliderCMYKP::onElementChanged);

    mainLayout->addWidget(cyanElement);
    mainLayout->addWidget(magentaElement);
    mainLayout->addWidget(yellowElement);
    mainLayout->addWidget(blackElement);
}

KCColorSliderItemPercent *KCColorSliderCMYKP::addElement(const QString &name)
{
    KCColorSliderItemPercent *currentElement=new KCColorSliderItemPercent(this);
    currentElement->setCaption(name);
    connect(currentElement, &KCColorSliderItemPercent::requireFocusOnElement,
            this, &KCColorSliderCMYKP::focusRequire);
    connect(this, SIGNAL(requireClearElementsFocus()),
            currentElement, SLOT(setEraseFocus()));
    return currentElement;
}

void KCColorSliderCMYKP::onElementChanged(QString elementName, int value)
{
    if(elementName=="C:")
    {
        currentColor.setCmyk(value,
                             magentaElement->getValue(),
                             yellowElement->getValue(),
                             blackElement->getValue());
    }
    else if(elementName=="M:")
    {
        currentColor.setCmyk(cyanElement->getValue(),
                             value,
                             yellowElement->getValue(),
                             blackElement->getValue());
    }
    else if(elementName=="Y:")
    {
        currentColor.setCmyk(cyanElement->getValue(),
                             magentaElement->getValue(),
                             value,
                             blackElement->getValue());
    }
    else if(elementName=="K:")
    {
        currentColor.setCmyk(cyanElement->getValue(),
                             magentaElement->getValue(),
                             yellowElement->getValue(),
                             value);
    }
    else
    {
        //Here should never comes.
        qWarning()<<"Unexpected elements occur.";
        return;
    }
    syncElement();
    signalSentByMe=true;
    emit requireSyncColor(currentColor);
}

void KCColorSliderCMYKP::syncColor(QColor color)
{
    if(signalSentByMe)
    {
        signalSentByMe=false;
        return;
    }
    KCColorSliderBase::syncColor(color);
    syncElement();
}

void KCColorSliderCMYKP::focusRequire(QString elementName, int value)
{
    emit requireFocusOnElement(elementName,
                               value,
                               currentColor);
}

void KCColorSliderCMYKP::syncElement()
{
    cyanElement->syncValue(currentColor.cyan(), currentColor);
    magentaElement->syncValue(currentColor.magenta(), currentColor);
    yellowElement->syncValue(currentColor.yellow(), currentColor);
    blackElement->syncValue(currentColor.black(), currentColor);
}

KCColorSliderCMYK::KCColorSliderCMYK(QWidget *parent) :
    KCColorSliderBase(parent)
{
    mainLayout=new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    cyanElement=addElement("C:");
    magentaElement=addElement("M:");
    yellowElement=addElement("Y:");
    blackElement=addElement("K:");

    connect(cyanElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderCMYK::onElementChanged);
    connect(magentaElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderCMYK::onElementChanged);
    connect(yellowElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderCMYK::onElementChanged);
    connect(blackElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderCMYK::onElementChanged);

    mainLayout->addWidget(cyanElement);
    mainLayout->addWidget(magentaElement);
    mainLayout->addWidget(yellowElement);
    mainLayout->addWidget(blackElement);
}

void KCColorSliderCMYK::onElementChanged(QString elementName, int value)
{
    if(elementName=="C:")
    {
        currentColor.setCmyk(value,
                             magentaElement->getValue(),
                             yellowElement->getValue(),
                             blackElement->getValue());
    }
    else if(elementName=="M:")
    {
        currentColor.setCmyk(cyanElement->getValue(),
                             value,
                             yellowElement->getValue(),
                             blackElement->getValue());
    }
    else if(elementName=="Y:")
    {
        currentColor.setCmyk(cyanElement->getValue(),
                             magentaElement->getValue(),
                             value,
                             blackElement->getValue());
    }
    else if(elementName=="K:")
    {
        currentColor.setCmyk(cyanElement->getValue(),
                             magentaElement->getValue(),
                             yellowElement->getValue(),
                             value);
    }
    else
    {
        //Here should never comes.
        qWarning()<<"Unexpected elements occur.";
        return;
    }
    //Update self element.
    syncElement();
    signalSentByMe=true;
    emit requireSyncColor(currentColor);
}

void KCColorSliderCMYK::syncColor(QColor color)
{
    if(signalSentByMe)
    {
        signalSentByMe=false;
        return;
    }
    KCColorSliderBase::syncColor(color);
    syncElement();
}

void KCColorSliderCMYK::syncElement()
{
    cyanElement->syncValue(currentColor.cyan(), currentColor);
    magentaElement->syncValue(currentColor.magenta(), currentColor);
    yellowElement->syncValue(currentColor.yellow(), currentColor);
    blackElement->syncValue(currentColor.black(), currentColor);
}

KCColorSliderRGB::KCColorSliderRGB(QWidget *parent) :
    KCColorSliderBase(parent)
{
    mainLayout=new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    redElement=addElement("R:");
    greenElement=addElement("G:");
    blueElement=addElement("B:");

    connect(redElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderRGB::onElementChanged);
    connect(greenElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderRGB::onElementChanged);
    connect(blueElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderRGB::onElementChanged);

    mainLayout->addWidget(redElement);
    mainLayout->addWidget(greenElement);
    mainLayout->addWidget(blueElement);
}

void KCColorSliderRGB::onElementChanged(QString elementName, int value)
{
    if(elementName=="R:")
    {
        currentColor.setRed(value);
    }
    else if(elementName=="G:")
    {
        currentColor.setGreen(value);
    }
    else if(elementName=="B:")
    {
        currentColor.setBlue(value);
    }
    else
    {
        //Here should never comes.
        qWarning()<<"Unexpected elements occur.";
        return;
    }
    syncElement();
    signalSentByMe=true;
    emit requireSyncColor(currentColor);
}

void KCColorSliderRGB::syncColor(QColor color)
{
    if(signalSentByMe)
    {
        signalSentByMe=false;
        return;
    }
    KCColorSliderBase::syncColor(color);
    syncElement();
}

void KCColorSliderRGB::syncElement()
{
    redElement->syncValue(currentColor.red(), currentColor);
    greenElement->syncValue(currentColor.green(), currentColor);
    blueElement->syncValue(currentColor.blue(), currentColor);
}

KCColorSliderHSV::KCColorSliderHSV(QWidget *parent) :
    KCColorSliderBase(parent)
{
    mainLayout=new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    hueElement=addElement("H:");
    hueElement->setMaximum(359);
    saturationElement=addElement("S:");
    valueElement=addElement("V:");

    connect(hueElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderHSV::onElementChanged);
    connect(saturationElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderHSV::onElementChanged);
    connect(valueElement, &KCColorSliderItemBase::requireElementChanged,
            this, &KCColorSliderHSV::onElementChanged);

    mainLayout->addWidget(hueElement);
    mainLayout->addWidget(saturationElement);
    mainLayout->addWidget(valueElement);;
}

void KCColorSliderHSV::onElementChanged(QString elementName, int value)
{
    if(elementName=="H:")
    {
        currentColor.setHsv(value,
                            saturationElement->getValue(),
                            valueElement->getValue());
    }
    else if(elementName=="S:")
    {
        currentColor.setHsv(hueElement->getValue(),
                            value,
                            valueElement->getValue());
    }
    else if(elementName=="V:")
    {
        currentColor.setHsv(hueElement->getValue(),
                            saturationElement->getValue(),
                            value);
    }
    else
    {
        //Here should never comes.
        qWarning()<<"Unexpected elements occur.";
        return;
    }
    syncElement();
    signalSentByMe=true;
    emit requireSyncColor(currentColor);
}

void KCColorSliderHSV::syncColor(QColor color)
{
    if(signalSentByMe)
    {
        signalSentByMe=false;
        return;
    }
    KCColorSliderBase::syncColor(color);
    syncElement();
}

void KCColorSliderHSV::syncElement()
{
    hueElement->syncValue(currentColor.hue(), currentColor);
    saturationElement->syncValue(currentColor.saturation(), currentColor);
    valueElement->syncValue(currentColor.value(), currentColor);
}

KCColorSelector::KCColorSelector(QWidget *parent) :
    QDialog(parent)
{
    mainLayout=new QBoxLayout(QBoxLayout::LeftToRight, this);
    setLayout(mainLayout);

    colorRingLayout=new QBoxLayout(QBoxLayout::TopToBottom);
    KCColorHSVRing *colorRing=new KCColorHSVRing(this);
    registerHSVRing(colorRing);
    colorRingLayout->addWidget(colorRing);
    mainLayout->addLayout(colorRingLayout);

    yayaModelLayout=new QBoxLayout(QBoxLayout::TopToBottom);
    yayaModelLayout->setContentsMargins(0,0,0,0);
    yayaModelLayout->setSpacing(0);
    KCColorSliderHSV *hsv=new KCColorSliderHSV(this);
    registerSelector(hsv);
    yayaModelLayout->addWidget(hsv);
    KCColorSliderCMYK *cmyk=new KCColorSliderCMYK(this);
    registerSelector(cmyk);
    yayaModelLayout->addWidget(cmyk);
    KCColorSliderRGB *rgb=new KCColorSliderRGB(this);
    registerSelector(rgb);
    yayaModelLayout->addWidget(rgb);
    yayaModelLayout->addStretch();
    mainLayout->addLayout(yayaModelLayout);

    iroriModelLayout=new QBoxLayout(QBoxLayout::TopToBottom);
    KCColorSliderCMYKP *cmykp=new KCColorSliderCMYKP(this);
    registerSelector(cmykp);
    iroriModelLayout->addWidget(cmykp);
    //yayaModelLayout->addStretch();

    KCColorViewerBase *viewer=new KCColorViewerBase(this);
    registerViewer(viewer);
    iroriModelLayout->addWidget(viewer);
    mainLayout->addLayout(iroriModelLayout);

    emit requireSyncColor(QColor(0,0,0));
}

KCColorSelector::~KCColorSelector()
{
    colorRingLayout->deleteLater();
    yayaModelLayout->deleteLater();
    iroriModelLayout->deleteLater();
}

void KCColorSelector::registerSelector(KCColorSliderBase *selector)
{
    //**Ask every one to sync color to the selector, and apply others require**
    connect(selector, &KCColorSliderBase::requireSyncColor,
            this, &KCColorSelector::requireSyncColor);
    connect(this, &KCColorSelector::requireSyncColor,
            selector, &KCColorSliderBase::syncColor);

    //**Ask every one to focus on the the element, and apply others require**
    connect(selector, &KCColorSliderBase::requireFocusOnElement,
            this, &KCColorSelector::requireFocusOnElement);
    connect(selector, SIGNAL(requireFocusOnElement(QString,int,QColor)),
            this, SIGNAL(requireClearFocus()));
    connect(this, &KCColorSelector::requireClearFocus,
            selector, &KCColorSliderBase::requireClearElementsFocus);
}

void KCColorSelector::registerViewer(KCColorViewerBase *viewer)
{
    connect(this, SIGNAL(requireSyncColor(QColor)),
            viewer, SLOT(setCurrentColor(QColor)));
}

void KCColorSelector::registerHSVRing(KCColorHSVRing *hsvRing)
{
    connect(this, &KCColorSelector::requireFocusOnElement,
            hsvRing, &KCColorHSVRing::setColorMode);
    connect(this, &KCColorSelector::requireSyncColor,
            hsvRing, &KCColorHSVRing::syncColor);
}
