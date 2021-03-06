/*
 *  Copyright 2013 Kreogist Dev Team
 *
 *  This file is part of Kreogist Cuties.
 *
 *    Kreogist Cuties is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *    Kreogist Cuties is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kreogist Cuties.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMouseEvent>

#include "kcpreferenceitembase.h"
#include "kccolorconfigure.h"

KCPreferenceOriginalLabel::KCPreferenceOriginalLabel(QWidget *parent) :
    QLabel(parent)
{
    ;
}

void KCPreferenceOriginalLabel::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);
    emit requireResetCurrentValue();
}

KCPreferenceItemBase::KCPreferenceItemBase(QWidget *parent) :
    QWidget(parent)
{
    //Set properties
    setObjectName("KCPreferenceItemBase");
    setAutoFillBackground(true);
    setContentsMargins(0,0,0,0);
    setFixedHeight(27);

    //Set Language
    retranslate();

    //Set the original value displayer.
    valueDisplayer=new QLabel(this);
    originalValueDisplayer=new KCPreferenceOriginalLabel(this);
    originalValueDisplayer->setObjectName("OriginalValueDisplayer");
    setOriginalDisplayVisible(false);

    pal=originalValueDisplayer->palette();
    KCColorConfigure::getInstance()->getPalette(pal,
                                                originalValueDisplayer->objectName());
    originalValueDisplayer->setPalette(pal);

    connect(originalValueDisplayer, SIGNAL(requireResetCurrentValue()),
            this, SLOT(resetCurrentValue()));

    //Set default switch
    valueChanged=false;
    editMode=false;

    //Set palette
    valueChangedAlpha=100;
    pal=palette();
    KCColorConfigure::getInstance()->getPalette(pal,
                                                objectName());
    backgroundColor=pal.color(QPalette::Window);
    backgroundColor.setAlpha(0);
    setBackgroundAlpha(0);

    //Init animation
    //Fade out Animation
    fadeMeOut=new QTimeLine(100, this);
    fadeMeOut->setUpdateInterval(1);
    fadeMeOut->setStartFrame(255);
    connect(fadeMeOut, SIGNAL(frameChanged(int)),
            this, SLOT(setBackgroundAlpha(int)));

    //Expand Animation
    unfoldAnimation=new QTimeLine(100, this);
    unfoldAnimation->setUpdateInterval(1);
    unfoldAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(unfoldAnimation, SIGNAL(frameChanged(int)),
            this, SLOT(setItemHeight(int)));
    connect(unfoldAnimation, SIGNAL(finished()),
            this, SLOT(showEditWidget()));

    //Fold Animation
    foldAnimation=new QTimeLine(100, this);
    foldAnimation->setUpdateInterval(1);
    foldAnimation->setEndFrame(27);
    foldAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(foldAnimation, SIGNAL(frameChanged(int)),
            this, SLOT(setItemHeight(int)));

    //Show Animation
    showAnimation=new QTimeLine(100, this);
    showAnimation->setUpdateInterval(1);
    showAnimation->setEndFrame(27);
    showAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(showAnimation, SIGNAL(frameChanged(int)),
            this, SLOT(setItemHeight(int)));

    //Hide Animation
    hideAnimation=new QTimeLine(100, this);
    hideAnimation->setUpdateInterval(1);
    hideAnimation->setEndFrame(0);
    hideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(hideAnimation, SIGNAL(frameChanged(int)),
            this, SLOT(setItemHeight(int)));
    connect(hideAnimation, SIGNAL(finished()),
            this, SLOT(hide()));
}

QVariant KCPreferenceItemBase::getCurrentValue() const
{
    return currentValue;
}

void KCPreferenceItemBase::setCurrentValue(const QVariant &value)
{
    currentValue=value;
    valueChanged=(currentValue!=originalValue);
}

QVariant KCPreferenceItemBase::getOriginalValue() const
{
    return originalValue;
}

void KCPreferenceItemBase::setOriginalValue(const QVariant &value)
{
    originalValue = value;
    currentValue = value;
}

void KCPreferenceItemBase::editFinished()
{
    unfoldAnimation->stop();
    if(valueChanged)
    {
        fadeMeOut->setEndFrame(valueChangedAlpha);
    }
    else
    {
        fadeMeOut->setEndFrame(0);
    }
    editMode=false;
    foldAnimation->setStartFrame(height());
    hideEditWidget();
    foldAnimation->start();
    fadeMeOut->start();
}

void KCPreferenceItemBase::setExpandFinishedHeight(const int &endHeight)
{
    expandEndHeight=endHeight;
    unfoldAnimation->setEndFrame(endHeight);
}

int KCPreferenceItemBase::getExpandFinishedHeight() const
{
    return expandEndHeight;
}

void KCPreferenceItemBase::retranslate()
{
    originalValueIs=tr("Original Setting: ");
}

void KCPreferenceItemBase::retranslateAndSet()
{
    retranslate();
    refreshValueDisplay();
}

void KCPreferenceItemBase::heightState(bool newState)
{
    if(newState)
    {
        setFixedHeight(27);
        show();
    }
    else
    {
        setFixedHeight(0);
        hide();
    }
}

void KCPreferenceItemBase::animateShow()
{
    if(showAnimation->state()!=QTimeLine::Running)
    {
        //Prepare animation
        show();
        //Stop hide animation
        hideAnimation->stop();
        //Set show animation
        showAnimation->setStartFrame(height());
        showAnimation->start();
    }
}

void KCPreferenceItemBase::animateHide()
{
    if(hideAnimation->state()!=QTimeLine::Running)
    {
        //Stop show animation
        showAnimation->stop();
        //Set hide animation
        hideAnimation->setStartFrame(height());
        hideAnimation->start();
    }
}

void KCPreferenceItemBase::resetCurrentValue()
{
    setWidgetValue(getOriginalValue());
}

void KCPreferenceItemBase::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    setBackgroundAlpha(255);
    if(!editMode)
    {
        foldAnimation->stop();
        unfoldAnimation->setStartFrame(height());
        unfoldAnimation->start();
        editMode=true;
    }
    emit editFocusCapture();
}
QString KCPreferenceItemBase::getKeyNames() const
{
    return keyNames;
}

void KCPreferenceItemBase::setKeyNames(const QString &value)
{
    keyNames = value;
}


void KCPreferenceItemBase::setBackgroundAlpha(int newAlpha)
{
    backgroundColor.setAlpha(newAlpha);
    pal.setColor(QPalette::Window, backgroundColor);
    setPalette(pal);
}

void KCPreferenceItemBase::setItemHeight(int newHeight)
{
    setFixedHeight(newHeight);
}

void KCPreferenceItemBase::showEditWidget()
{
    setEditWidgetStatus(true);
}

void KCPreferenceItemBase::hideEditWidget()
{
    setEditWidgetStatus(false);
}

void KCPreferenceItemBase::valueChangedEvent()
{
    setCurrentValue(getUserNewValue());
    refreshValueDisplay();
}

void KCPreferenceItemBase::setOriginalDisplayVisible(bool statue)
{
    originalValueDisplayer->setEnabled(statue);
    originalValueDisplayer->setVisible(statue);
}
