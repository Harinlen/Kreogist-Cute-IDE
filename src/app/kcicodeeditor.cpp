/*
 *  Copyright 2013 Kreogist Dev Team
 *
 *  This file is part of Kreogist-Cuties.
 *
 *    Kreogist-Cuties is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *    Kreogist-Cuties is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kreogist-Cuties.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kcicodeeditor.h"

static const int SearchBarOffset = 20;

KCICodeEditor::KCICodeEditor(QWidget *parent) :
    QWidget(parent)
{
    setFont(QString("Monaco"));

    replaceLayout=new QVBoxLayout(this);
    replaceLayout->setContentsMargins(0,0,0,0);
    replaceLayout->setMargin(0);
    replaceLayout->setSpacing(0);
    setLayout(replaceLayout);

    //setWindowFlags(Qt::AnchorPoint);
    mainLayout=new QHBoxLayout();
    mainLayout->setSpacing(0);
    setContentsMargins(0,0,0,0);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    markPanel=new KCIMarkPanel(this);
    markPanel->setVisible(false);
    markPanel->setEnabled(false);
    mainLayout->addWidget(markPanel);

    linePanel=new KCILinenumPanel(this);
    mainLayout->addWidget(linePanel);

    editor=new KCITextEditor(this);
    linePanel->setKciTextEditor(editor);
    markPanel->setKciTextEditor(editor);
    mainLayout->addWidget(editor);

    replaceLayout->addLayout(mainLayout);

    replaceBar=new KCIReplaceDock(this);
    replaceBar->hide();
    replaceLayout->addWidget(replaceBar);

    connect(editor->document(),SIGNAL(modificationChanged(bool)),
            this,SLOT(onModificationChanged(bool)));
    connect(editor,SIGNAL(cursorPositionChanged()),
            this,SLOT(cursorChanged()));
    connect(editor,SIGNAL(overwriteModeChanged(bool)),
            this, SIGNAL(rewriteStateChanged(bool)));

    //Default Disable Overwrite Mode.
    editor->setOverwriteMode(false);

    m_langMode=new KCILanguageMode(this);

    QPalette pal = palette();
    pal.setColor(QPalette::Base,QColor(0x53,0x53,0x53));
    pal.setColor(QPalette::Text,QColor(255,255,255));
    setPalette(pal);

    filePath.clear();
    fileError=QFileDevice::NoError;

    searchBar=new KCISearchWindow(editor);
    searchBar->hide();
}

KCICodeEditor::~KCICodeEditor()
{
    mainLayout->deleteLater();
}

bool KCICodeEditor::getOverwriteMode()
{
    return editor->overwriteMode();
}

void KCICodeEditor::setOverwriteMode(bool newValue)
{
    editor->setOverwriteMode(newValue);
    emit rewriteStateChanged(newValue);
}

QString KCICodeEditor::getExecFileName()
{
    return execFileName;
}

QTextDocument *KCICodeEditor::document()
{
    return editor->document();
}

void KCICodeEditor::computeExecFileName()
{
    QFileInfo _fileInfo(filePath);
    execFileName=_fileInfo.absolutePath()+QString("/")+_fileInfo.completeBaseName();
#ifdef Q_OS_WIN32
    execFileName+=".exe";
#endif
}

void KCICodeEditor::connectSearchWidgetWithEditor(KCISearchWidget *widget)
{
    searcherConnections+=connect(widget, &KCISearchWidget::requireHide,
                                 this, &KCICodeEditor::hideSearchBar);
    searcherConnections+=connect(widget,&KCISearchWidget::requireSearch,
                                 editor,&KCITextEditor::searchString);
    searcherConnections+=connect(widget,&KCISearchWidget::requireShowNextResult,
                                 editor,&KCITextEditor::showNextSearchResult);
    searcherConnections+=connect(widget,&KCISearchWidget::requireShowPreviousResult,
                                 editor,&KCITextEditor::showPreviousSearchResult);
}

void KCICodeEditor::showSearchBar()
{
    if(replaceBar->isVisible())
    {
        searcherConnections.disConnectAll();
        replaceBar->hideAnime();
    }

    if(!searchBar->isVisible())
    {
        QPropertyAnimation *searchAnime=new QPropertyAnimation(searchBar,"geometry");
        QRect animeEndPos=searchBar->rect();
        animeEndPos.setX(editor->width()-searchBar->width()-SearchBarOffset);
        QRect animeStartPos=animeEndPos;
        animeStartPos.setTop(-animeStartPos.height());
        searchAnime->setStartValue(animeStartPos);
        searchAnime->setDuration(300);
        searchAnime->setEndValue(animeEndPos);
        searchAnime->setEasingCurve(QEasingCurve::OutCubic);
        searchBar->show();
        searchAnime->start(QPropertyAnimation::DeleteWhenStopped);

        connectSearchWidgetWithEditor(searchBar);
    }

    QTextCursor _textCursor=editor->textCursor();
    if(_textCursor.hasSelection())
    {
        searchBar->setText(_textCursor.selectedText());
    }
    searchBar->setTextFocus();
}

void KCICodeEditor::hideSearchBar()
{
    if(searchBar->isVisible())
    {
        QPropertyAnimation *searchAnime=new QPropertyAnimation(searchBar,"geometry");
        QRect animeStartPos=searchBar->geometry();
        QRect animeEndPos=animeStartPos;
        animeEndPos.setTop(-animeStartPos.height() - 20);
        searchAnime->setStartValue(animeStartPos);
        searchAnime->setDuration(300);
        searchAnime->setEndValue(animeEndPos);
        searchAnime->setEasingCurve(QEasingCurve::OutCubic);
        connect(searchAnime, SIGNAL(finished()),
                searchBar, SLOT(hide()));
        searchAnime->start(QPropertyAnimation::DeleteWhenStopped);
    }

    editor->setFocus();
}

void KCICodeEditor::showReplaceBar()
{
    if(searchBar->isVisible())
    {
        hideSearchBar();
        searcherConnections.disConnectAll();
    }

    if(!replaceBar->isVisible())
    {
        replaceBar->showAnime();

        connectSearchWidgetWithEditor(replaceBar);

        searcherConnections+=connect(replaceBar,&KCIReplaceDock::requireReplace,
                                     editor,&KCITextEditor::replace);
        searcherConnections+=connect(replaceBar,&KCIReplaceDock::requireReplaceAndFind,
                                     editor,&KCITextEditor::replaceAndFind);
        searcherConnections+=connect(replaceBar,&KCIReplaceDock::requireReplaceAll,
                                     editor,&KCITextEditor::replaceAll);
    }

    QTextCursor _textCursor=editor->textCursor();
    if(_textCursor.hasSelection())
    {
        replaceBar->setText(_textCursor.selectedText());
    }
    replaceBar->setTextFocus();
}

bool KCICodeEditor::open(const QString &fileName)
{
    QFile _file(fileName);

    if(_file.open(QIODevice::ReadOnly |QIODevice::Text))
    {
        QTextStream _textIn(&_file);

        editor->clear();
        editor->setPlainText(QString(_textIn.readAll()));

        fileInfoChanged(_file);
        kciHistoryConfigure::getInstance()->addRecentFileRecord(filePath);
        return true;
    }
    else
    {
        fileError=_file.error();
        return false;
    }
}

QFileDevice::FileError KCICodeEditor::error()
{
    return fileError;
}

bool KCICodeEditor::save()
{
    if(!filePath.isEmpty())
    {
        return saveAs(filePath);
    }
    else
    {
        if(!dosaveas(tr("Save")))
        {
            if(fileError!=QFileDevice::AbortError)
            {
                QErrorMessage error(this);
                error.showMessage(tr("Saving file failed!"));
                error.exec();
            }
            return false;
        }
        else
        {
            return true;
        }
    }
}

bool KCICodeEditor::saveAs()
{
    if(!dosaveas(tr("Save As")))
    {
        if(fileError!=QFileDevice::AbortError)
        {
            QErrorMessage error(this);
            error.showMessage(tr("Saving file failed!"));
            error.exec();
        }
        return false;
    }
    else
    {
        return true;
    }
}

bool KCICodeEditor::dosaveas(const QString &Caption)
{
    if(kciGeneralConfigure::getInstance()->getUseDefaultLanguageWhenSave())
    {
        QString defaultSelectFilter;
        switch(kciGeneralConfigure::getInstance()->getDefaultLanguageMode())
        {
        case 1:
            defaultSelectFilter=kciGeneralConfigure::getInstance()->getCfFilter();
            break;
        case 2:
            defaultSelectFilter=kciGeneralConfigure::getInstance()->getCppfFilter();
            break;
        case 3:
            defaultSelectFilter=kciGeneralConfigure::getInstance()->getPasfFilter();
            break;
        default:
            defaultSelectFilter=kciGeneralConfigure::getInstance()->getAsfFilter();
        }
        filePath=QFileDialog::getSaveFileName(this,
                                              Caption,
                                              kciHistoryConfigure::getInstance()->getHistoryDir(),
                                              kciGeneralConfigure::getInstance()->getStrFileFilter(),
                                              &defaultSelectFilter);
    }
    else
    {
        filePath=QFileDialog::getSaveFileName(this,
                                              Caption,
                                              kciHistoryConfigure::getInstance()->getHistoryDir(),
                                              kciGeneralConfigure::getInstance()->getStrFileFilter());
    }
    if(!filePath.isEmpty())
    {
        return saveAs(filePath);
    }
    else
    {
        fileError=QFileDevice::AbortError;
        return false;
    }
}

bool KCICodeEditor::saveAs(const QString &fileName)
{

    QFile _file(fileName);

    if(_file.open(QIODevice::WriteOnly |QIODevice::Text))
    {
        QTextStream _textOut(&_file);
        _textOut<<editor->toPlainText()<<flush;
        fileInfoChanged(_file);
        return true;
    }
    else
    {
        fileError=_file.error();
        return false;
    }
}

void KCICodeEditor::closeEvent(QCloseEvent *e)
{
    if(editor->document()->isModified())
    {
        QMessageBox msgbox(this);
        QString strDisplayFileName;

        if(filePath.isEmpty())
        {
            strDisplayFileName=editor->documentTitle();
        }
        else
        {
            strDisplayFileName=filePath;
        }

        msgbox.setText(tr("Will you save changes to the the following file?") + "\n" +
                       strDisplayFileName);
        msgbox.setInformativeText(tr("If you don't save the changes, all the changes will be lost."));

        msgbox.setStandardButtons(QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        msgbox.setDefaultButton(QMessageBox::Save);
        msgbox.setButtonText(QMessageBox::Save,tr("&Save"));
        msgbox.setButtonText(QMessageBox::Discard,tr("&Don't Save"));
        msgbox.setButtonText(QMessageBox::Cancel,tr("&Cancel"));

        int ret=msgbox.exec();

        switch(ret)
        {
        case QMessageBox::Save:
            // Save was clicked
            if(!save())
            {
                e->ignore();
                break;
            }
            else
            {
                e->accept();
                break;
            }
        case QMessageBox::Discard:
            // Don't Save was clicked
            e->accept();

            break;
        case QMessageBox::Cancel:
            // Cancel was clicked
            e->ignore();

            break;
        default:
            // should never be reached
            qWarning("codeeditor.cpp: switch(ret) reached an unexcepted line!");
            break;
        }
    }
    else
    {
        e->accept();
    }

    return ;
}

void KCICodeEditor::setDocumentTitle(const QString &title)
{
    editor->setDocumentTitle(title);
    emit filenameChanged(title);
}

QString KCICodeEditor::getDocumentTitle()
{
    return editor->documentTitle();
}

void KCICodeEditor::redo()
{
    editor->redo();
}

void KCICodeEditor::undo()
{
    editor->undo();
}

void KCICodeEditor::copy()
{
    editor->copy();
}

void KCICodeEditor::cut()
{
    editor->cut();
}

void KCICodeEditor::paste()
{
    editor->paste();
}

void KCICodeEditor::selectAll()
{
    editor->selectAll();
}

void KCICodeEditor::setTextFocus()
{
    editor->setFocus();
}

void KCICodeEditor::onModificationChanged(bool changed)
{
    if(changed)
    {
        emit filenameChanged(editor->documentTitle()+"*");
    }
    else
    {
        emit filenameChanged(editor->documentTitle());
    }
}

QString KCICodeEditor::getFilePath()
{
    return filePath;
}

QString KCICodeEditor::getSelectedText()
{
    return editor->textCursor().selectedText();
}

void KCICodeEditor::cursorChanged()
{
    fileTextCursor=editor->textCursor();
    emit fileTextCursorChanged();
}

QTextCursor KCICodeEditor::getTextCursor()
{
    return fileTextCursor;
}

int KCICodeEditor::getTextLines()
{
    return editor->document()->blockCount();
}

void KCICodeEditor::setDocumentCursor(int nLine, int linePos)
{
    editor->setDocumentCursor(nLine,linePos);
}

void KCICodeEditor::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    searchBar->setGeometry(editor->width()-searchBar->width()-SearchBarOffset,
                           0,
                           searchBar->width(),
                           searchBar->height());
}

void KCICodeEditor::fileInfoChanged(const QFile &file)
{
    QFileInfo _fileInfo(file);
    editor->setDocumentTitle(_fileInfo.fileName());
    emit filenameChanged(_fileInfo.fileName());

    m_langMode->setFileSuffix(_fileInfo.suffix());

    filePath=file.fileName();
    fileError=QFileDevice::NoError;
    editor->document()->setModified(false);

    computeExecFileName();
    kciHistoryConfigure::getInstance()->setHistoryDir(_fileInfo.absolutePath());
}

KCILanguageMode *KCICodeEditor::langMode() const
{
    return m_langMode;
}

bool KCICodeEditor::isModified()
{
    return editor->document()->isModified();
}

void KCICodeEditor::insertTextAtCursor(QString insertText)
{
    editor->insertPlainText(insertText);
}

