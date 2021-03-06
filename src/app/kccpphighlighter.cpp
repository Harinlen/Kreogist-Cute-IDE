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

#include <QKeyEvent>
#include <QTextCursor>

#include "textcharformatmap.h"

#include "kccpphighlighter.h"

KCCppHighlighter::KCCppHighlighter(QObject *parent) :
    KCHighlighter(parent)
{
    //get textCharFormatMap instance
    instance = textCharFormatMap::getInstance();

    //Declare values:
    int i;
    highlight_rule hlrDataType, hlrKeyWords, hlrSpTypes;//, hlrSingleLineCComments;
    QString strKeyWords;

    //Set Data Types:
    hlrDataType.type_name="datatype";
    hlrDataType.regexp.setPattern("\\b(bool|char|double|float|int|long|short|signed|unsigned|void|wchar_t|char16_t|char32_t|nullptr)\\b");
    rules<<hlrDataType;

    //Set Key Words:
    hlrKeyWords.type_name="keyword";
    QStringList _keyword;
    _keyword<<"__asm|__cdecl|__declspec|__export|__far16|"
            <<"__fastcall|__fortran|__import|"
            <<"__pascal|__rtti|__stdcall|_asm|_cdecl|"
            <<"__except|_export|_far16|_fastcall|"
            <<"__finally|_fortran|_import|_stdcall|__thread|__try|asm|auto|"
            <<"break|case|catch|cdecl|const|continue|default|"
            <<"do|else|enum|extern|for|goto|"
            <<"if|register|return|sizeof|static|"
            <<"struct|switch|"
            <<"typedef|union|"
            <<"volatile|while|"
            <<"class|const_cast|delete|"
            <<"dynamic_cast|explicit|false|friend|"
            <<"inline|mutable|namespace|new|operator|private|protected|"
            <<"public|reinterpret_cast|static_cast|"
            <<"template|this|throw|true|"
            <<"try|typeid|typename|"
            <<"using|virtual";
    strKeyWords="\\b(";
    for(i=0; i<_keyword.size(); i++)
    {
        strKeyWords+=_keyword[i];
    }
    strKeyWords+=QString(")\\b");
    hlrKeyWords.regexp.setPattern(strKeyWords);
    rules<<hlrKeyWords;

    //Numbers
    hlrSpTypes.type_name = "number";
    hlrSpTypes.regexp.setPattern(QString("\\b\\d+(\\.)?\\d*\\b"));
    rules<<hlrSpTypes;

    //Set Other Special Types:

    //Pre-Process back text
    hlrSpTypes.type_name = "preprocdata";
    hlrSpTypes.regexp.setPattern(QString("^[[:blank:]]*#([[:blank:]]*[[:word:]]*).*"));
    rules<<hlrSpTypes;

    //Pre-Process
    hlrSpTypes.type_name = "preproc";
    hlrSpTypes.regexp.setPattern(QString("^[[:blank:]]*#([[:blank:]]*[[:word:]]*)"));
    rules<<hlrSpTypes;

    //TODO
    hlrSpTypes.type_name="todo";
    hlrSpTypes.regexp.setPattern(QString("(TODO|FIXME|BUG)([:]?)"));
    rules<<hlrSpTypes;
}

void KCCppHighlighter::conmmentHighlightBlock(const QString &text)
{
    QRegExp startExpression("/\\*");
    QRegExp endExpression("\\*/");
    KCTextBlockData *data=static_cast<KCTextBlockData *>(currentBlockUserData());

    setCurrentBlockState(0);

    int startIndex=0;
    if(previousBlockState() != 1)
    {
        startIndex=text.indexOf(startExpression);
    }

    while(startIndex > -1)
    {
        bool searchNext=false;
        if(data->getLineCommentPos()>-1 &&
           startIndex > data->getLineCommentPos())
        {
            /*
             * Here means: begin char is no use!
             */
            startIndex=text.indexOf(startExpression, startIndex+1);
            continue;
        }
        for(auto i=data->getFirstQuotationInfo(),
            l=data->getEndQuotationInfo();
            i<l;
            i++)
        {
            if(startIndex > i->beginPos && startIndex < i->endPos)
            {
                startIndex=text.indexOf(startExpression, i->endPos+1);
                searchNext=true;
                break;
            }
        }
        if(searchNext)
        {
            continue;
        }
        int endIndex = text.indexOf(endExpression, startIndex);
        while(searchNext)
        {
            searchNext=false;
            for(auto i=data->getFirstQuotationInfo(),
                l=data->getEndQuotationInfo();
                i<l;
                i++)
            {
                if(endIndex > i->beginPos && endIndex < i->endPos)
                {
                    endIndex=text.indexOf(endExpression, i->endPos+1);
                    searchNext=true;
                    break;
                }
            }
        }
        int conmmentLength;
        if(endIndex == -1)
        {
            setCurrentBlockState(1);
            conmmentLength = text.length() - startIndex;
        }
        else
        {
            conmmentLength = endIndex - startIndex + endExpression.matchedLength();
        }
        setFormat(startIndex,conmmentLength,instance->getTextCharFormat("comment"));
        startIndex = text.indexOf(startExpression, startIndex+conmmentLength);
    }
}

QString KCCppHighlighter::parenthesesPair(const QString &parenthesesChar)
{
    if(!parenthesesChar.isEmpty())
    {
        int parenthesesIndex=leftParenthesesLists.indexOf(parenthesesChar);
        if(parenthesesIndex>-1)
        {
            return QString(rightParenthesesLists.at(parenthesesIndex));
        }
    }
    return QString();
}

void KCCppHighlighter::KCHighlightBlock(const QString &text)
{
    for(int i=0; i<rules.size(); i++)
    {
        QRegularExpressionMatchIterator matchIterator=rules[i].regexp.globalMatch(
                    text);
        while(matchIterator.hasNext())
        {
            QRegularExpressionMatch match=matchIterator.next();
            setFormat(match.capturedStart(),
                      match.capturedLength(),
                      instance->getTextCharFormat(rules[i].type_name));
            //QTextCharFormat());
        }
    }

    KCTextBlockData *data=static_cast<KCTextBlockData *>(currentBlockUserData());
    Q_ASSERT(data!=NULL);
    codeLevelUnit codeLevelInfo=data->getCodeLevelInfo();

    QTextBlock prevBlock=currentBlock().previous();

    int baseLevel=0, prevLevel=0;
    if(prevBlock.isValid())
    {
        KCTextBlockData *prevData=static_cast<KCTextBlockData *>(prevBlock.userData());
        Q_ASSERT(prevData!=NULL);
        codeLevelUnit prevCodeLevelInfo=prevData->getCodeLevelInfo();
        baseLevel=prevCodeLevelInfo.codeLevel;

        for(auto i=prevData->getFirstParenthesesInfo(),
            l=prevData->getEndParenthesesInfo();
            i<l;
            i++)
        {
            if(i->character == '{')
            {
                baseLevel++;
                continue;
            }
            if(i->character == '}')
            {
                baseLevel--;
            }
        }
        prevLevel=prevCodeLevelInfo.codeLevel;

        prevCodeLevelInfo.codeLevelUp=(baseLevel>prevLevel);
        prevCodeLevelInfo.codeLevelDown=(baseLevel<prevLevel);
        prevData->setCodeLevelInfo(prevCodeLevelInfo);
    }
    codeLevelInfo.codeLevel=baseLevel;
    data->setCodeLevelInfo(codeLevelInfo);

    //Highlight single line comment
    if(data->getLineCommentPos()!=-1)
    {
        setFormat(data->getLineCommentPos(),
                  text.length()-data->getLineCommentPos(),
                  instance->getTextCharFormat("comment"));
    }

    stringHighlightBlock(text);
    conmmentHighlightBlock(text);
}

bool KCCppHighlighter::eventFilter(QObject *object, QEvent *event)
{
    if(event->type()==QEvent::KeyPress && object==m_editor)
    {
        QKeyEvent *keyPress=static_cast<QKeyEvent *>(event);
        QTextCursor codeCursor=m_editor->textCursor();

        QString pairParethesesChar=parenthesesPair(keyPress->text());
        if(!pairParethesesChar.isEmpty())
        {
            if(codeCursor.selectedText().isEmpty())
            {
                m_editor->emulatePressKey(keyPress);
                m_editor->insertPlainText(pairParethesesChar);
                codeCursor.movePosition(QTextCursor::Left);
                m_editor->setTextCursor(codeCursor);
            }
            else
            {
                int start=codeCursor.selectionStart(),
                    end=codeCursor.selectionEnd();
                codeCursor.beginEditBlock();
                codeCursor.clearSelection();
                codeCursor.setPosition(start);
                codeCursor.insertText(keyPress->text());
                codeCursor.setPosition(end+1);
                codeCursor.insertText(pairParethesesChar);
                codeCursor.endEditBlock();
                m_editor->setTextCursor(codeCursor);
            }
            return true;
        }
        KCTextBlockData *currentData=m_editor->getBlockData(codeCursor.block());
        if(keyPress->text()==")" || keyPress->text()=="]")
        {
            if(QString(codeCursor.document()->characterAt(codeCursor.position()))
                    == keyPress->text())
            {
                if(currentData!=NULL)
                {
                    for(auto i=currentData->getFirstParenthesesInfo(),
                        l=currentData->getEndParenthesesInfo();
                        i<l;
                        i++)
                    {
                        if(i->pos==codeCursor.positionInBlock())
                        {
                            if(m_editor->matchParentheses(
                                        keyPress->text().at(0).toLatin1(),
                                        keyPress->text()==")"?'(':'[',
                                        i,
                                        codeCursor.block(),
                                        false) > -1)
                            {
                                codeCursor.movePosition(QTextCursor::Right);
                                m_editor->setTextCursor(codeCursor);
                                return true;
                            }
                        }
                    }
                }
            }
            return KCHighlighter::eventFilter(object, event);
        }
        if(keyPress->text()=="\"")
        {
            if(codeCursor.selectedText().isEmpty())
            {
                if(codeCursor.document()->characterAt(codeCursor.position())==QChar('\"') &&
                   currentData->getQuotationStatus()!=-1)
                {
                    codeCursor.movePosition(QTextCursor::Right);
                    m_editor->setTextCursor(codeCursor);
                    return true;
                }
                m_editor->emulatePressKey(keyPress);
                if(codeCursor.document()->characterAt(codeCursor.position()-1)!=QChar('\\') &&
                   currentData->getQuotationStatus()==-1)
                {
                    m_editor->insertPlainText("\"");
                    codeCursor.movePosition(QTextCursor::Left);
                    m_editor->setTextCursor(codeCursor);
                }
            }
            else
            {
                int start=codeCursor.selectionStart(),
                    end=codeCursor.selectionEnd();
                codeCursor.beginEditBlock();
                codeCursor.clearSelection();
                codeCursor.setPosition(start);
                codeCursor.insertText("\"");
                codeCursor.setPosition(end+1);
                codeCursor.insertText("\"");
                codeCursor.endEditBlock();
                m_editor->setTextCursor(codeCursor);
            }
            return true;
        }
        switch(keyPress->key())
        {
        case Qt::Key_Backspace:
        {
            if(codeCursor.selectedText().length()>0)
            {
                return KCHighlighter::eventFilter(object, event);
            }
            //Now we have to judge the status.
            QChar previousChar=codeCursor.document()->characterAt(codeCursor.position()-1),
                  currentChar=codeCursor.document()->characterAt(codeCursor.position());
            //If the previous is a panethese and there's the other part of the current panethese,
            //Delete all of them.
            if(parenthesesPair(previousChar)==currentChar)
            {
                codeCursor.deleteChar();
                return KCHighlighter::eventFilter(object, event);
            }
            //So as quotations.
            if(previousChar==QChar('\"') && currentChar==QChar('\"') &&
               currentData->getQuotationStatus()!=0)
            {
                codeCursor.deleteChar();
            }
            return KCHighlighter::eventFilter(object, event);
        }
        }
    }
    return KCHighlighter::eventFilter(object, event);
}

void KCCppHighlighter::stringHighlightBlock(const QString &text)
{
    KCTextBlockData *data=static_cast<KCTextBlockData *>(currentBlockUserData());
    for(auto i=data->getFirstQuotationInfo(),
        l=data->getEndQuotationInfo();
        i<l;
        i++)
    {
        if(i->endPos == 0)
        {
            setFormat(i->beginPos,
                      text.length() - i->beginPos,
                      instance->getTextCharFormat(QString("string")));
        }
        else
        {
            setFormat(i->beginPos,
                      i->endPos - i->beginPos,
                      instance->getTextCharFormat(QString("string")));
        }
    }
}
