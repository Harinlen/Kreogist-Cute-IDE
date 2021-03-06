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

/*!
 * \details
 * This file is the function implementation file of class KCCodeEditor.
 * KCCodeEditor is the base class of KCTabManager. KCCodeEditor is a
 * combination of the following classes:
 *
 * KCTextEditor
 *
 * KCLanguageMode
 *
 * KCTextEditor is used to edit the code. And KCLanguageMode will process
 * the compile and debug process.
 */

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QFile>
#include <QWidget>
#include <QTextCursor>
#include <QList>
#include <QFileInfo>
#include "kccodecompileprogress.h"
#include "kcconnectionhandler.h"
#include "kclanguagemode.h"

class QBoxLayout;
class KCSearchWindow;
class KCReplaceWindow;
class KCTextEditor;
class KCSearchWidget;

class KCCodeEditor : public QWidget
{
    Q_OBJECT
public:
    explicit KCCodeEditor(QWidget *parent = 0);

    struct KCCodeEditorSettings
    {
        bool usingBlankInsteadTab;
        int spacePerTab;
        int tabSpace;
        QTextOption::WrapMode wrapMode;
        int cursorWidth;
        bool lineNumberPanelVisible;
        bool codeLevelVisible;
        QColor lineColor;
        QColor searchResultColor;
        QColor noMatchedParenthesesColor;
        QColor matchedParenthesesColor;
    };

    void applyEditorSettings(KCCodeEditor::KCCodeEditorSettings settings);
    QFileDevice::FileError error();
    void setDocumentTitle(const QString &title);
    QString getDocumentTitle();
    void setDocumentCursor(int block, int column);
    void setTextFocus();
    QString getFilePath();
    QString getExecFileName();
    QTextCursor getTextCursor();

    void setVScrollValue(int value);
    void setHScrollValue(int value);
    int getVScrollValue();
    int getHScrollValue();

    int getTextLines();
    bool isModified();
    QTextDocument *document();
    KCLanguageMode *langMode() const;
    void setLanguageMode(KCLanguageMode *value);
    bool getOverwriteMode();

    void insertTextAtCursor(QString insertText);
    bool getCacheNewFileMode() const;
    void setCacheNewFileMode(bool value);
    bool getDebugging() const;
    void setDebugging(bool value);
    void hideCompileBar();
    void setCompileBarState(KCCodeCompileProgress::CompileState state);
    void resetCompileErrorCache();

signals:
    void filenameChanged(QString newName);
    void fileTextCursorChanged();
    void rewriteStateChanged(bool nowValue);
    void requiredHideDocks();
    void requiredCompileFile();
    void requiredAddRecentRecord(QString newName);
    void requireSetHistoryDir(QString dirPath);

public slots:
    //KCTextEditor link slots
    bool open(const QString &fileName,
              bool cacheMode=false);
    bool save();
    bool saveAs();
    bool saveAs(const QString &fileName,
                bool cacheMode=false);
    void redo();
    void undo();
    void copy();
    void cut();
    void paste();
    void selectAll();
    void cursorChanged();
    QString getSelectedText();
    void showSearchBar();
    void showReplaceBar();
    void showCompileBar();
    void setOverwriteMode(bool newValue);
    bool readCacheFile(const QString &cachedfilePath);
    bool writeCacheFile(const QString &filePath);
    QList<int> getBreakpoints();

private slots:
    void onDebugJumpLine(int blockNumber);
    void onModificationChanged(bool changed);
    void onHideOtherWidgets();
    void onSearchNext(QString searchTextSets,
                      bool regularExpressionSets,
                      bool caseSensitivelySets,
                      bool wholeWordSets);
    void onShowNextSearchResult();
    void onShowPreviousSearchResult();
    void setUseLastCuror();
    void addErrorsToStack(int lineNum);
    void redrawSmartPanel();

protected:
    void closeEvent(QCloseEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    bool searchUseLastCursor;
    bool processSaveAsAction(const QString &dialogCaption);
    bool requireSaveAs(const QString &caption);
    void connectSearchWidgetWithEditor(KCSearchWidget *widget);

    KCLanguageMode *languageMode;

    KCCodeCompileProgress *currentCompileProgress;

    QBoxLayout *mainLayout;
    KCTextEditor *editor;

    KCSearchWidget *currentSearchWidget;
    KCSearchWindow *searchBar;
    KCReplaceWindow *replaceBar;

    KCConnectionHandler searcherConnections;

    bool cacheNewFileMode;
    bool debugging;

    QList<int> errorOccurLines;
};

#endif // TEXTEDITOR_H
