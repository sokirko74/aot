#include "SynanView.h"

#include "synan/SynanLib/SyntaxHolder.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QElapsedTimer>
#include <QMenu>
#include <QPainter>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QSplitter>
#include <QTextDocumentFragment>
#include <QToolTip>
#include <QVBoxLayout>

SynanView::SynanView(QWidget* parent) : QAbstractScrollArea(parent) {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->setBackgroundRole(QPalette::Base);
    viewport()->setMouseTracking(true);
}

QPoint SynanView::toDocument(const QPoint& viewportPoint) const {
    return viewportPoint + QPoint(0, verticalScrollBar()->value());
}

void SynanView::updateScrollBars() {
    int height = m_Sentences.GetSentencesSize().height();
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setSingleStep(20);
    verticalScrollBar()->setRange(0, std::max(0, height + SPACE_BETWEEN_SENTENCE - viewport()->height()));
}

void SynanView::recalculate() {
    if (m_Sentences.SentCount() > 0) {
        m_Sentences.CalculateCoordinates(QFontMetrics(g_VisualFonts.m_Words), viewport()->width());
    }
    updateScrollBars();
    viewport()->update();
}

void SynanView::paintEvent(QPaintEvent*) {
    QPainter p(viewport());
    p.fillRect(viewport()->rect(), Qt::white);
    p.setFont(g_VisualFonts.m_Words);
    p.setPen(Qt::black);
    p.translate(0, -verticalScrollBar()->value());
    m_Sentences.PrintSentences(p);
}

void SynanView::resizeEvent(QResizeEvent* event) {
    QAbstractScrollArea::resizeEvent(event);
    recalculate();
}

void SynanView::scrollContentsBy(int, int) {
    viewport()->update();
}

bool SynanView::viewportEvent(QEvent* event) {
    if (event->type() == QEvent::ToolTip) {
        auto helpEvent = static_cast<QHelpEvent*>(event);
        int iSentence = -1, iWord = -1;
        if (m_Sentences.GetWordByPoint(toDocument(helpEvent->pos()), &iSentence, &iWord)) {
            QToolTip::showText(helpEvent->globalPos(), m_Sentences.GetActiveHomDescr(iSentence, iWord), viewport());
        }
        else {
            QToolTip::hideText();
            event->ignore();
        }
        return true;
    }
    return QAbstractScrollArea::viewportEvent(event);
}

// the menu of homonyms of the word: the user chooses the active homonym
void SynanView::contextMenuEvent(QContextMenuEvent* event) {
    int iSentence = -1, iWord = -1;
    CVisualWord* word = m_Sentences.GetWordByPoint(toDocument(viewport()->mapFrom(this, event->pos())), &iSentence, &iWord);
    if (!word) {
        return;
    }
    QMenu menu(this);
    for (size_t i = 0; i < word->m_arrHomonyms.size(); ++i) {
        const auto& hom = word->m_arrHomonyms[i];
        QString text = hom.m_strLemma;
        if (!hom.m_strCommonGrammems.isEmpty()) {
            text += " " + hom.m_strCommonGrammems;
        }
        auto action = menu.addAction(text);
        action->setCheckable(true);
        action->setChecked((int)i == word->m_iActiveHomonym);
        action->setData((int)i);
    }
    QAction* chosen = menu.exec(event->globalPos());
    if (chosen && m_Sentences.SetActiveHomonym(iSentence, iWord, chosen->data().toInt())) {
        recalculate();
    }
}

//----------------------------------------------------------------------------

SynanWindow::SynanWindow(QWidget* parent, bool withEditor) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_View = new SynanView(this);
    if (withEditor) {
        auto splitter = new QSplitter(Qt::Vertical, this);
        m_Edit = new QPlainTextEdit(splitter);
        m_Edit->setPlaceholderText("Type a text and run syntax (F5)");
        QFont f = m_Edit->font();
        f.setPointSize(f.pointSize() + 2);
        m_Edit->setFont(f);
        splitter->addWidget(m_Edit);
        splitter->addWidget(m_View);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 2);
        layout->addWidget(splitter);
    }
    else {
        layout->addWidget(m_View);
    }
    setAttribute(Qt::WA_DeleteOnClose);
}

bool SynanWindow::processInput(CSyntaxHolder& holder, const std::string& input, bool bFile) {
    m_View->sentences().Reset();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QElapsedTimer timer;
    timer.start();
    bool result = false;
    try {
        result = holder.GetSentencesFromSynAn(input, bFile)
            && m_View->sentences().FillSentencesArray(holder.m_Synan);
    }
    catch (std::exception& e) {
        ErrorMessage("VisualSynan", e.what());
    }
    catch (...) {
    }
    m_WorkTimeStr = QString("Synan time: %1 ms").arg(timer.elapsed());
    m_View->verticalScrollBar()->setValue(0);
    m_View->recalculate();
    QApplication::restoreOverrideCursor();
    emit statusMessage(result ? m_WorkTimeStr : QString("Synan failed"));
    return result;
}

bool SynanWindow::runSyntax(CSyntaxHolder& holder) {
    if (!m_Edit) {
        return false;
    }
    QTextCursor cursor = m_Edit->textCursor();
    QString text = cursor.hasSelection() ? cursor.selection().toPlainText() : m_Edit->toPlainText();
    return processInput(holder, text.toUtf8().toStdString(), false);
}

bool SynanWindow::runSyntaxOnFile(CSyntaxHolder& holder, const QString& path) {
    return processInput(holder, path.toUtf8().toStdString(), true);
}
