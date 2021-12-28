#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TString.h"
#include "TObjArray.h"

#include "AliLog.h"
#include "AliVEventHandler.h"

#include "AliAnalysisManager.h"
#include "AliAnalysisDataContainer.h"

#include "AliMuonTrackCuts.h"
#include "TaskDimuonppMC.h"
#endif

///Add the names of the output containers. The method is not fully automatic with the cxx file, so you have to keep the same order here and there (when defining the output containers in TaskDimuonppMC.cxx)
TString outputListNames[] = {"EventTree","GenTree"};
int numberOfOutputLists = sizeof( outputListNames ) / sizeof( outputListNames[0] );

AliAnalysisTask* AddMyTaskppMC(Bool_t isESD=kFALSE, TString name = "name", int firstRun = 244918, int lastRun = 297595)
{
    // get the manager via the static access member. since it's static, you don't need
    // to create an instance of the class here to call the function
    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        Error("AddTaskDimuonppMC","AliAnalysisManager not set!");
        return 0x0;
    }
    // get the input event handler, again via a static method. 
    // this handler is part of the managing system and feeds events to your task
    TString type = mgr->GetInputEventHandler()->GetDataType();
    if (!type.Contains("AOD") && !isESD) 
    {
        Error("AddTaskDimuonppMC", "AOD input handler needed!");
        return 0x0;
    }
    if (!type.Contains("ESD") && isESD) 
    {
        Error("AddTaskDimuonppMC", "ESD input handler needed!");
        return 0x0;
    }

    // by default, a file is open for writing. here, we get the filename
    TString outputFileName = AliAnalysisManager::GetCommonFileName();

    TaskDimuonppMC *task;
    task = new TaskDimuonppMC("TaskDimuonppMC", firstRun, lastRun);
        
    if(!task) return 0x0;
    // add your task to the manager
    mgr->AddTask(task);
    // your task needs input: here we connect the manager to your task
    mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());

    AliAnalysisDataContainer *evtTree = mgr->CreateContainer(Form("%s",outputListNames[0].Data()), TTree::Class(), AliAnalysisManager::kOutputContainer, outputFileName);
    AliAnalysisDataContainer *genTree = mgr->CreateContainer(Form("%s",outputListNames[1].Data()), TTree::Class(), AliAnalysisManager::kOutputContainer, outputFileName);
    // same for the output
    mgr->ConnectOutput(task, 1, evtTree);
    mgr->ConnectOutput(task, 2, genTree);

    return task;
}
