#pragma once
#include <ProcessController.h>

void ShowEntry(Muninn::Controller::ProcessController* pProcessController) noexcept;
void ShowModules(Muninn::Controller::ProcessController* pProcessController) noexcept;
void TrySimpleDllInjtect(Muninn::Controller::ProcessController* pProcessController) noexcept;