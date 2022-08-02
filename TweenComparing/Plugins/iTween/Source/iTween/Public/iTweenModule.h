//Copyright Jared Therriault and Bob Berkbile 2014

#pragma once

#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_CLASS(LogiTween, Log, All);

class iTweenModule : public IModuleInterface
{

public:
	iTweenModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
