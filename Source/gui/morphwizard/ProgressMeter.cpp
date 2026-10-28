#include "ProgressMeter.h"

#include <QApplication>
#include <QProgressDialog>

ProgressBar::ProgressBar() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_Dialog = std::make_unique<QProgressDialog>(QApplication::activeWindow());
    m_Dialog->setWindowTitle("MorphWizard");
    m_Dialog->setCancelButton(nullptr);
    m_Dialog->setWindowModality(Qt::ApplicationModal);
    m_Dialog->setMinimumDuration(500);
    m_Dialog->setAutoReset(false);
    m_Dialog->setAutoClose(false);
    m_Dialog->setRange(0, 100);
    m_Dialog->setValue(0);
}

ProgressBar::~ProgressBar() {
    m_Dialog->close();
    QApplication::restoreOverrideCursor();
}

void ProgressBar::setText(const char* info) {
    m_Dialog->setLabelText(QString::fromUtf8(info));
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void ProgressBar::setRange(uint32_t maxPos) {
    // QProgressDialog works with int
    m_Dialog->setRange(0, (int)std::min<uint32_t>(maxPos, INT_MAX));
}

void ProgressBar::setPos(uint32_t pos) {
    m_Dialog->setValue((int)std::min<uint32_t>(pos, (uint32_t)m_Dialog->maximum()));
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}
