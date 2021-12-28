/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* AliAnaysisTaskMyTask
 *
 * empty task which can serve as a starting point for building an analysis
 * as an example, one histogram is filled
 */

#include "TaskDimuonppData.h"

#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "AliAODMCHeader.h"
#include "AliAODMCParticle.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisMuonUtility.h"
#include "AliAnalysisTask.h"
#include "AliCentrality.h"
#include "AliCounterCollection.h"
#include "AliMultSelection.h"
#include "AliMuonTrackCuts.h"
#include "AliOADBMuonTrackCutsParam.h"
#include "AliVEvent.h"
#include "AliVTrack.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH1F.h"
#include "THnSparse.h"
#include "TList.h"
#include "TMath.h"
#include <iostream>

class TaskDimuonppData; // your analysis class

using namespace std; // std namespace: so you can do things like 'cout'

ClassImp(TaskDimuonppData) // classimp: necessary for root

    TaskDimuonppData::TaskDimuonppData()
    : AliAnalysisTaskSE(), fAODEvent(0), fVEvent(0), fFirstRun(0), fLastRun(0),
      fRunNumber(0), fEventNumber(0), fCentrality(0), fTreeEvent(0),
      fIsPbPb(false), fnMuons(0) {
  // default constructor, don't allocate memory here!
  // this is used by root for IO purposes, it needs to remain empty
}
//_____________________________________________________________________________
TaskDimuonppData::TaskDimuonppData(const char *name, int firstRun, int lastRun)
    : AliAnalysisTaskSE(name), fAODEvent(0), fVEvent(0), fFirstRun(firstRun),
      fLastRun(lastRun), fRunNumber(0), fEventNumber(0), fCentrality(0),
      fTreeEvent(0), fIsPbPb(false), fnMuons(0) {
  // constructor
  DefineInput(
      0, TChain::Class()); // define the input of the analysis: in this case we
                           // take a 'chain' of events this chain is created by
                           // the analysis manager, so no need to worry about
                           // it, it does its work automatically
  DefineOutput(
      1, TTree::Class()); // define the ouptut of the analysis: in this case
                          // it's a list of histograms you can add more output
                          // objects by calling DefineOutput(2,
                          // classname::Class()) if you add more output objects,
                          // make sure to call PostData for all of them, and to
                          // make changes to your AddTask macro!

  DefineOutput(2, AliCounterCollection::Class());
}
//_____________________________________________________________________________
TaskDimuonppData::~TaskDimuonppData() {
  if (!AliAnalysisManager::GetAnalysisManager()->IsProofMode()) {
    delete fTreeEvent;
    delete fEventCounters;
  }
}
//_____________________________________________________________________________
void TaskDimuonppData::NotifyRun() {}
//_____________________________________________________________________________
void TaskDimuonppData::UserCreateOutputObjects() {

  OpenFile(1);

  fTreeEvent =
      new TTree("eventsTree", "tree that contains information of the event");
  fTreeEvent->Branch("runNumber", &fRunNumber);
  if (fIsPbPb)
    fTreeEvent->Branch("centrality", &fCentrality);
  fTreeEvent->Branch("xVtx", &fX);
  fTreeEvent->Branch("yVtx", &fY);
  fTreeEvent->Branch("zVtx", &fZ);
  fTreeEvent->Branch("isCINT", &fMatchCINTEvent);
  fTreeEvent->Branch("isCMSL", &fMatchCMSLEvent);
  fTreeEvent->Branch("isCMSH", &fMatchCMSHEvent);
  fTreeEvent->Branch("isCMLL", &fMatchCMLLEvent);
  fTreeEvent->Branch("isCMUL", &fMatchCMULEvent);

  fTreeEvent->Branch("nMuons", &fnMuons);
  fTreeEvent->Branch("Muon_E", &fMuon_E);
  fTreeEvent->Branch("Muon_Px", &fMuon_Px);
  fTreeEvent->Branch("Muon_Py", &fMuon_Py);
  fTreeEvent->Branch("Muon_Pz", &fMuon_Pz);
  fTreeEvent->Branch("Muon_Charge", &fMuon_Charge);
  fTreeEvent->Branch("Muon_thetaAbs", &fMuon_ThetaAbs);
  fTreeEvent->Branch("Muon_xDCA", &fMuon_xDCA);
  fTreeEvent->Branch("Muon_yDCA", &fMuon_yDCA);
  fTreeEvent->Branch("Muon_zDCA", &fMuon_zDCA);
  fTreeEvent->Branch("Muon_matchedTrgThreshold", &fMuon_MatchTrig);
  fTreeEvent->Branch("Muon_MCHClusterMap", &fMuon_TrkClsMap);
  fTreeEvent->Branch("Muon_MTRHitMapTrg", &fMuon_TrgHitsMapTrg);
  fTreeEvent->Branch("Muon_MTRHitMapTrk", &fMuon_TrgHitsMapTrk);

  // This is needed to save the outputs.
  PostData(1, fTreeEvent);

  fEventCounters = new AliCounterCollection("CC");
  fEventCounters->AddRubric("trigger", 100);
  fEventCounters->AddRubric("run", 10000);
  fEventCounters->Init();

  PostData(2, fEventCounters);
}
//_____________________________________________________________________________
void TaskDimuonppData::UserExec(Option_t *) {
  // user exec this function is called once for each event
  // the manager will take care of reading the events from file, and with the
  // static function InputEvent() you have access to the current event. once
  // you return from the UserExec function, the manager will retrieve the next
  // event from the chain
  fAODEvent = dynamic_cast<AliAODEvent *>(InputEvent());
  if (!fAODEvent) {
    AliError("ERROR: Could not retrieve AOD event !!");
    return;
  }

  // Triggers
  TString strFiredTriggers = fAODEvent->GetFiredTriggerClasses();

  // Physics Selection
  UInt_t IsSelected =
      (((AliInputEventHandler *)(AliAnalysisManager::GetAnalysisManager()
                                     ->GetInputEventHandler()))
           ->IsEventSelected());

  // Trigger logic
  fMatchCINTEvent = kFALSE;
  fMatchCMSLEvent = kFALSE;
  fMatchCMSHEvent = kFALSE;
  fMatchCMLLEvent = kFALSE;
  fMatchCMULEvent = kFALSE;

  fEventCounters->Count(
      TString::Format("trigger:ANY/run:%d", fAODEvent->GetRunNumber()));

  if (strFiredTriggers.Contains("CINT7-B-NOPF-MUFAST") &&
      (IsSelected & AliVEvent::kINT7inMUON)) {
    fEventCounters->Count(TString::Format("trigger:CINT7-B-NOPF-MUFAST/run:%d",
                                          fAODEvent->GetRunNumber()));
    fMatchCINTEvent = kTRUE;
  }
  if (strFiredTriggers.Contains("CMSL7-B-NOPF-MUFAST") &&
      (IsSelected & AliVEvent::kMuonSingleLowPt7)) {
    fEventCounters->Count(TString::Format("trigger:CMSL7-B-NOPF-MUFAST/run:%d",
                                          fAODEvent->GetRunNumber()));
    fMatchCMSLEvent = kTRUE;
  }
  if (strFiredTriggers.Contains("CMSH7-B-NOPF-MUFAST") &&
      (IsSelected & AliVEvent::kMuonSingleHighPt7)) {
    fEventCounters->Count(TString::Format("trigger:CMSH7-B-NOPF-MUFAST/run:%d",
                                          fAODEvent->GetRunNumber()));
    fMatchCMSHEvent = kTRUE;
  }
  if (strFiredTriggers.Contains("CMLL7-B-NOPF-MUFAST") &&
      (IsSelected & AliVEvent::kMuonLikeLowPt7)) {
    fEventCounters->Count(TString::Format("trigger:CMLL7-B-NOPF-MUFAST/run:%d",
                                          fAODEvent->GetRunNumber()));
    fMatchCMLLEvent = kTRUE;
  }
  if (strFiredTriggers.Contains("CMUL7-B-NOPF-MUFAST") &&
      (IsSelected & AliVEvent::kMuonUnlikeLowPt7)) {
    fEventCounters->Count(TString::Format("trigger:CMUL7-B-NOPF-MUFAST/run:%d",
                                          fAODEvent->GetRunNumber()));
    fMatchCMULEvent = kTRUE;
  }

  // Access events
  fnMuons = 0;
  if (IsSelected && (fMatchCINTEvent || fMatchCMSLEvent || fMatchCMSHEvent ||
                     fMatchCMLLEvent || fMatchCMULEvent)) {
    // Event histos after physics selection
    // Get the centrality
    if (fIsPbPb) {
      AliMultSelection *multSelection =
          (AliMultSelection *)fAODEvent->FindListObject("MultSelection");
      fCentrality = multSelection->GetMultiplicityPercentile("V0M", false);
      if (fCentrality > 90.)
        return;
    }

    fVEvent = static_cast<AliVEvent *>(InputEvent());
    fRunNumber = fAODEvent->GetRunNumber();
    fEventNumber = Entry();

    AliVParticle *muonTrack;

    Float_t muonMass2 =
        AliAnalysisMuonUtility::MuonMass2(); // the PDG rest mass of the muon
                                             // (constante, used for getting
                                             // the kinematics) en GeV
    fnMuons = AliAnalysisMuonUtility::GetNTracks(
        fVEvent); // get the number of muon tracks in the event

    // Get the vertex
    fX = fAODEvent->GetPrimaryVertex()->GetX();
    fY = fAODEvent->GetPrimaryVertex()->GetY();
    fZ = fAODEvent->GetPrimaryVertex()->GetZ();

    // resize vectors respect to Nmuons
    fMuon_E.resize(fnMuons);
    fMuon_Px.resize(fnMuons);
    fMuon_Py.resize(fnMuons);
    fMuon_Pz.resize(fnMuons);
    fMuon_Charge.resize(fnMuons);
    fMuon_ThetaAbs.resize(fnMuons);
    fMuon_xDCA.resize(fnMuons);
    fMuon_yDCA.resize(fnMuons);
    fMuon_zDCA.resize(fnMuons);
    fMuon_MatchTrig.resize(fnMuons);
    fMuon_TrkClsMap.resize(fnMuons);
    fMuon_TrgHitsMapTrg.resize(fnMuons);
    fMuon_TrgHitsMapTrk.resize(fnMuons);

    for (Int_t iMuon = 0; iMuon < fnMuons;
         iMuon++) // loop over all these tracks
    {
      muonTrack = AliAnalysisMuonUtility::GetTrack(iMuon, fVEvent);
      if (!muonTrack) {
        AliError(Form("ERROR: Could not retrieve AOD or ESD track %d", iMuon));
        continue;
      }

      // single muon properties
      fMuon_E.at(iMuon) =
          TMath::Sqrt(muonTrack->P() * muonTrack->P() + muonMass2);
      fMuon_Px.at(iMuon) = muonTrack->Px();
      fMuon_Py.at(iMuon) = muonTrack->Py();
      fMuon_Pz.at(iMuon) = muonTrack->Pz();
      fMuon_Charge.at(iMuon) = muonTrack->Charge();
      fMuon_ThetaAbs.at(iMuon) =
          AliAnalysisMuonUtility::GetThetaAbsDeg(muonTrack);
      fMuon_xDCA.at(iMuon) = AliAnalysisMuonUtility::GetXatDCA(
          muonTrack); // Missing info on pTot corr and DCAcorr from OADB
      fMuon_yDCA.at(iMuon) = AliAnalysisMuonUtility::GetYatDCA(
          muonTrack); //+ massive number of info for pDCA cut...
      fMuon_zDCA.at(iMuon) = AliAnalysisMuonUtility::GetZatDCA(muonTrack); //
      fMuon_MatchTrig.at(iMuon) = AliAnalysisMuonUtility::GetMatchTrigger(
          muonTrack); // Missing info on the trigger threshold from OADB
      fMuon_TrkClsMap.at(iMuon) =
          static_cast<const AliAODTrack *>(muonTrack)->GetMUONClusterMap();
      fMuon_TrgHitsMapTrg.at(iMuon) =
          AliAnalysisMuonUtility::GetMUONTrigHitsMapTrg(muonTrack);
      fMuon_TrgHitsMapTrk.at(iMuon) =
          AliAnalysisMuonUtility::GetMUONTrigHitsMapTrk(muonTrack);
    }

    fTreeEvent->Fill();
  }

  fMuon_E.clear();
  fMuon_Px.clear();
  fMuon_Py.clear();
  fMuon_Pz.clear();
  fMuon_Charge.clear();
  fMuon_ThetaAbs.clear();
  fMuon_xDCA.clear();
  fMuon_yDCA.clear();
  fMuon_zDCA.clear();
  fMuon_MatchTrig.clear();
  fMuon_TrkClsMap.clear();
  fMuon_TrgHitsMapTrg.clear();
  fMuon_TrgHitsMapTrk.clear();

  PostData(1, fTreeEvent);
  PostData(2, fEventCounters);
}
//_____________________________________________________________________________
void TaskDimuonppData::Terminate(Option_t *) {
  // terminate
  // called at the END of the analysis (when all events are processed)
}
//_____________________________________________________________________________
