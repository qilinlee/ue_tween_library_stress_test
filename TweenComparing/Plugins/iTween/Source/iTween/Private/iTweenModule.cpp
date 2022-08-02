//Copyright Jared Therriault and Bob Berkbile 2014

#include "iTweenModule.h"

#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(iTweenModule, iTween);

iTweenModule::iTweenModule()
{

}

void iTweenModule::StartupModule()
{
	//Startup LOG message
	//UE_LOG(ModuleLog, Warning, TEXT("iTween: Log Started"));
}

void iTweenModule::ShutdownModule()
{
	//Shutdown LOG message
	//UE_LOG(ModuleLog, Warning, TEXT("iTween: Log Ended"));
}
