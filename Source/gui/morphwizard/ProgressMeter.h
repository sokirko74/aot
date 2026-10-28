#pragma once

#include "morph_dict/morph_wizard/wizard.h"

#include <memory>

class QProgressDialog;

// a modal progress dialog, which is updated by COperationMeterRML descendants
class ProgressBar {
public:
    ProgressBar();
    ~ProgressBar();
    void setText(const char* info);
    void setRange(uint32_t maxPos);
    void setPos(uint32_t pos);
private:
    std::unique_ptr<QProgressDialog> m_Dialog;
};

// a progress meter for file operations (import/export)
class ProgressMeter : public CFileMeterRML {
public:
    void SetInfo(const char* info) override { m_Bar.setText(info); }
protected:
    void UpdateMaxPos() override { m_Bar.setRange(GetMaxPos()); }
    void UpdatePos() override { m_Bar.setPos(GetPos()); }
private:
    ProgressBar m_Bar;
};

// a progress meter, which is attached to a wizard while it exists (loading, saving, searching)
class WizardProgressMeter : public MorphWizardMeter {
public:
    WizardProgressMeter(MorphoWizard& wizard) : MorphWizardMeter(wizard) {}
    void SetInfo(const char* info) override { m_Bar.setText(info); }
protected:
    void UpdateMaxPos() override { m_Bar.setRange(GetMaxPos()); }
    void UpdatePos() override { m_Bar.setPos(GetPos()); }
private:
    ProgressBar m_Bar;
};
