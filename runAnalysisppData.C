// include the header of your analysis task here! for classes already compiled
// by aliBuild, precompiled header files (with extension pcm) are available, so
// that you do not need to specify includes for those. for your own task
// however, you (probably) have not generated a pcm file, so we need to include
// it explicitly
//#include "TaskDimuonppData.h"
#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include <TSystem.h>
#include <fstream>
#include <TChain.h>
#include "AliAODInputHandler.h"

void runAnalysisppData(Bool_t isLocal = kTRUE, Bool_t isGridTest = kTRUE,
                       Bool_t isMerging = kFALSE, Bool_t mergeJDL = kTRUE) {
  // set if you want to run the analysis locally (kTRUE), or on grid (kFALSE)
  Bool_t local = isLocal;
  // if you run on grid, specify test mode (kTRUE) or full grid model (kFALSE)
  Bool_t gridTest = isGridTest;

  // since we will compile a class, tell root where to look for headers
#if !defined(__CINT__) || defined(__CLING__)
  gInterpreter->ProcessLine(".include $ROOTSYS/include");
  gInterpreter->ProcessLine(".include $ALICE_ROOT/include");
#else
  gROOT->ProcessLine(".include $ROOTSYS/include");
  gROOT->ProcessLine(".include $ALICE_ROOT/include");
#endif

  // create the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("AnalysisTaskExample");
  AliAODInputHandler *aodH = new AliAODInputHandler();
  mgr->SetInputEventHandler(aodH);

  // compile the class and load the add task macro
  // gInterpreter->LoadMacro("TaskDimuonppData.cxx++g");
  AliAnalysisTask *task = reinterpret_cast<AliAnalysisTask *>(
      gInterpreter->ExecuteMacro("AddMyTaskppData.C"));

  if (!mgr->InitAnalysis())
    return;
  mgr->SetDebugLevel(2);
  mgr->PrintStatus();
  mgr->SetUseProgressBar(1, 1000);

  if (local) {
    // if you want to run locally, we need to define some input
    TChain *chain = new TChain("aodTree");
    // add a few files to the chain (change this so that your local files are
    // added)
    std::ifstream in("list.aod.txt");
    if (!in.is_open()) {
      chain->Add("AliAOD.Muons.root");

    } else {
      std::string line;
      while (std::getline(in, line)) {
        chain->Add(line.c_str());
      }
    }
    // start the analysis locally, reading the events from the tchain
    mgr->StartAnalysis("local", chain);
  } else {
    // if we want to run on grid, we create and configure the plugin
    AliAnalysisAlien *alienHandler = new AliAnalysisAlien();

    // also specify the include (header) paths on grid
    alienHandler->AddIncludePath(
        "-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include "
        "-I$ALICE_PHYSICS/include");

    // make sure your source files get copied to grid
    alienHandler->SetAdditionalLibs("TaskDimuonppData.cxx TaskDimuonppData.h");
    alienHandler->SetAnalysisSource("TaskDimuonppData.cxx");

    // select the aliphysics version. all other packages
    // are LOADED AUTOMATICALLY!
    alienHandler->SetAliPhysicsVersion("vAN-20200201_ROOT6-1");

    // set the Alien API version
    alienHandler->SetAPIVersion("V1.1x");

    // select additionnal data file

    // select the input data
    alienHandler->SetGridDataDir("/alice/data/2018/LHC18m");
    alienHandler->SetDataPattern("*muon_calo_pass1/AOD/*AliAOD.Muons.root");

    // MC has no prefix, data has prefix 000
    alienHandler->SetRunPrefix("000");

    // runnumber
    TString inputFile = gSystem->GetFromPipe("cat runList18m.txt");
    alienHandler->AddRunList(inputFile.Data());

    // number of files per subjob
    alienHandler->SetSplitMaxInputFileNumber(100);
    alienHandler->SetExecutable("myTask.sh");

    // specify how many seconds your job may take
    alienHandler->SetTTL(35000);
    alienHandler->SetJDLName("myTask.jdl");

    alienHandler->SetOutputToRunNo(kTRUE);
    alienHandler->SetKeepLogs(kTRUE);

    // merging: run with kTRUE to merge on grid
    // after re-running the jobs in SetRunMode("terminate")
    // (see below) mode, set SetMergeViaJDL(kFALSE)
    // to collect final results
    alienHandler->SetMaxMergeStages(1);
    alienHandler->SetMergeViaJDL(mergeJDL);

    // define the output folders
    alienHandler->SetGridWorkingDir("dimuonData_LHC18m");
    alienHandler->SetGridOutputDir("myOutputDir");

    // connect the alien plugin to the manager
    mgr->SetGridHandler(alienHandler);
    if (gridTest) {
      // speficy on how many files you want to run
      alienHandler->SetNtestFiles(1);
      // and launch the analysis
      alienHandler->SetRunMode("test");
      mgr->StartAnalysis("grid");
    } else {
      // else launch the full grid analysis
      if (!isMerging)
        alienHandler->SetRunMode("full");
      else
        alienHandler->SetRunMode("terminate");
      mgr->StartAnalysis("grid");
    }
  }
}
