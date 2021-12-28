#if !defined(__CINT__) || defined(__MAKECINT__)
#include "AliAnalysisDataContainer.h"
#include "AliAnalysisManager.h"
#include "AliCounterCollection.h"
#include "AliLog.h"
#include "AliMuonTrackCuts.h"
#include "AliVEventHandler.h"
#include "TObjArray.h"
#include "TString.h"
#include "TaskDimuonppData.h"
#endif

/// Add the names of the output containers. The method is not fully automatic
/// with the cxx file, so you have to keep the same order here and there (when
/// defining the output containers in TaskDimuonppData.cxx)
TString outputListNames[] = {"EventTree"};
int numberOfOutputLists = sizeof(outputListNames) / sizeof(outputListNames[0]);

AliAnalysisTask *AddMyTaskppData(Bool_t isESD = kFALSE, TString name = "name",
                                 int firstRun = 244918, int lastRun = 297595) {
  // get the manager via the static access member. since it's static, you don't
  // need to create an instance of the class here to call the function
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error("AddTaskDimuonppData", "AliAnalysisManager not set!");
    return 0x0;
  }
  // get the input event handler, again via a static method.
  // this handler is part of the managing system and feeds events to your task
  TString type = mgr->GetInputEventHandler()->GetDataType();
  if (!type.Contains("AOD") && !isESD) {
    Error("AddTaskDimuonppData", "AOD input handler needed!");
    return 0x0;
  }
  if (!type.Contains("ESD") && isESD) {
    Error("AddTaskDimuonppData", "ESD input handler needed!");
    return 0x0;
  }

  // by default, a file is open for writing. here, we get the filename
  TString outputFileName = AliAnalysisManager::GetCommonFileName();

  TaskDimuonppData *task =
      new TaskDimuonppData("TaskDimuonppData", firstRun, lastRun);

  // add your task to the manager
  mgr->AddTask(task);
  // your task needs input: here we connect the manager to your task
  mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());

  AliAnalysisDataContainer *outputList = mgr->CreateContainer(
      Form("%s", outputListNames[0].Data()), TTree::Class(),
      AliAnalysisManager::kOutputContainer, outputFileName);
  // same for the output
  mgr->ConnectOutput(task, 1, outputList);

  AliAnalysisDataContainer *coutputCC = mgr->CreateContainer(
      "CC", AliCounterCollection::Class(), AliAnalysisManager::kOutputContainer,
      outputFileName);

  mgr->ConnectOutput(task, 2, coutputCC);

  return task;
}
