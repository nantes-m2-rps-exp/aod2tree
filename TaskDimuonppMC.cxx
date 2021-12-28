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

// include root libraries
#include <iostream>
#include "TChain.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TList.h"
#include "TChain.h"
#include "TMath.h"
#include "THnSparse.h"
#include "TFile.h"
// include AliRoot Libraries
#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAODEvent.h"
#include "AliVEvent.h"
#include "AliAODInputHandler.h"
#include "AliMuonTrackCuts.h"
#include "AliVTrack.h"
#include "AliOADBMuonTrackCutsParam.h"
#include "AliCentrality.h"
#include "AliAnalysisMuonUtility.h"
#include "AliMultSelection.h"

#include "AliAODMCHeader.h"
#include "AliAODMCParticle.h"

#include "TaskDimuonppMC.h"

class TaskDimuonppMC;    // your analysis class

using namespace std;            // std namespace: so you can do things like 'cout'

ClassImp(TaskDimuonppMC) // classimp: necessary for root

TaskDimuonppMC::TaskDimuonppMC() : AliAnalysisTaskSE(), 
    fAODEvent(0x0),
    fMCEvent(0x0),
    fVEvent(0),
    fFirstRun(0),
    fLastRun(0),
    fRunNumber(0),
    fEventNumber(0),
    fCentrality(0),
    fTreeEvent(0),
    fTreeGen(0x0),
    fIsPbPb(false), 
    fnMuons(0),
    fnMuonsGen(0)
{
    // default constructor, don't allocate memory here!
    // this is used by root for IO purposes, it needs to remain empty
}
//_____________________________________________________________________________
TaskDimuonppMC::TaskDimuonppMC(const char* name,int firstRun, int lastRun) : AliAnalysisTaskSE(name),
    fAODEvent(0x0),
    fMCEvent(0x0),
    fVEvent(0),
    fFirstRun(firstRun),
    fLastRun(lastRun),
    fRunNumber(0),
    fEventNumber(0),
    fCentrality(0),
    fTreeEvent(0x0),
    fTreeGen(0x0),
    fIsPbPb(false),
    fnMuons(0),
    fnMuonsGen(0)
{
    // constructor
    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take a 'chain' of events
                                        // this chain is created by the analysis manager, so no need to worry about it, 
                                        // it does its work automatically
    DefineOutput(1, TTree::Class());    // define the ouptut of the analysis: in this case it's a list of histograms 
    DefineOutput(2, TTree::Class());    // you can add more output objects by calling DefineOutput(2, classname::Class())
                                        // if you add more output objects, make sure to call PostData for all of them, and to
                                        // make changes to your AddTask macro!
}
//_____________________________________________________________________________
TaskDimuonppMC::~TaskDimuonppMC()
{
    if(!AliAnalysisManager::GetAnalysisManager()->IsProofMode()) {
        if(fTreeEvent) delete fTreeEvent;
        if(fTreeGen) delete fTreeGen;
    }
}
//_____________________________________________________________________________
void TaskDimuonppMC::NotifyRun()
{

}
//_____________________________________________________________________________
void TaskDimuonppMC::UserCreateOutputObjects()
{
    fTreeEvent = new TTree("eventsTree", "tree that contains information of the event");
    fTreeEvent->Branch("runNumber", &fRunNumber);
    if (fIsPbPb) fTreeEvent->Branch("centrality", &fCentrality);
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
    fTreeEvent->Branch("Muon_Charge",   &fMuon_Charge);
    fTreeEvent->Branch("Muon_thetaAbs", &fMuon_ThetaAbs);
    fTreeEvent->Branch("Muon_xDCA", &fMuon_xDCA);
    fTreeEvent->Branch("Muon_yDCA", &fMuon_yDCA);
    fTreeEvent->Branch("Muon_zDCA", &fMuon_zDCA);
    fTreeEvent->Branch("Muon_matchedTrgThreshold", &fMuon_MatchTrig);
    fTreeEvent->Branch("Muon_MCHClusterMap", &fMuon_TrkClsMap);
    fTreeEvent->Branch("Muon_MTRHitMapTrg", &fMuon_TrgHitsMapTrg);
    fTreeEvent->Branch("Muon_MTRHitMapTrk", &fMuon_TrgHitsMapTrk);

    fTreeEvent->Branch("Muon_MCLabel", &fMuon_MCLabel);
    fTreeEvent->Branch("Muon_MCPDGCode", &fMuon_MCPDGCode);

    fTreeGen = new TTree("genTree", "tree that contains information of the generated events and tracks");
    fTreeGen->Branch("xVtxMC", &fXMC);
    fTreeGen->Branch("yVtxMC", &fYMC);
    fTreeGen->Branch("zVtxMC", &fZMC);
    fTreeGen->Branch("nMuonsGen", &fnMuonsGen);
    fTreeGen->Branch("Muon_GenE",             &fMuon_GenE);
    fTreeGen->Branch("Muon_GenPx",            &fMuon_GenPx);
    fTreeGen->Branch("Muon_GenPy",            &fMuon_GenPy);
    fTreeGen->Branch("Muon_GenPz",            &fMuon_GenPz);
    fTreeGen->Branch("Muon_GenLabel",         &fMuon_GenLabel);
    fTreeGen->Branch("Muon_GenMotherPDGCode", &fMuon_GenMotherPDGCode);
    //This is needed to save the outputs.
    PostData(1, fTreeEvent);
    PostData(2, fTreeGen);

}
//_____________________________________________________________________________
void TaskDimuonppMC::UserExec(Option_t *)
{
    // user exec this function is called once for each event
    // the manager will take care of reading the events from file, and with the static function InputEvent() you 
    // have access to the current event. 
    // once you return from the UserExec function, the manager will retrieve the next event from the chain
    fAODEvent = dynamic_cast<AliAODEvent*>(InputEvent());    
    if(!fAODEvent) {
        AliError("ERROR: Could not retrieve AOD event !!");
        return;
    }

    // get MC event
    fMCEvent = dynamic_cast<AliMCEvent*>(MCEvent());
    if(!fMCEvent) {
        AliError("ERROR: Could not retrieve MC event !!");
        return;
    }
  
    // Triggers
    AliAODHeader * aodHeader = (AliAODHeader *)fAODEvent->GetHeader();
    TString strFiredTriggers = fAODEvent->GetFiredTriggerClasses();

    //Trigger logic
    fMatchCINTEvent = kFALSE;
    fMatchCMSLEvent = kFALSE;
    fMatchCMSHEvent = kFALSE;
    fMatchCMLLEvent = kFALSE;
    fMatchCMULEvent = kFALSE;

    if (AliVEvent::kINT7inMUON) {
       fMatchCINTEvent = kTRUE;
    }
    if (AliVEvent::kMuonSingleLowPt7) {
       fMatchCMSLEvent = kTRUE;
    }
    if (AliVEvent::kMuonSingleHighPt7) {
       fMatchCMSHEvent = kTRUE;
    }
    if (strFiredTriggers.Contains("MULL") && AliVEvent::kMuonLikeLowPt7) {
       fMatchCMLLEvent = kTRUE;
    }
    if (strFiredTriggers.Contains("MULU") && AliVEvent::kMuonUnlikeLowPt7) {
       fMatchCMULEvent = kTRUE;
    }

    //Access events
    fnMuons=0;

    //Event histos after physics selection
    //Get the centrality
    AliMultSelection *multSelection = (AliMultSelection * ) fAODEvent->FindListObject("MultSelection");
    if (fIsPbPb)
    {
      fCentrality = multSelection->GetMultiplicityPercentile("V0M", false);
      if (fCentrality > 90.) return;  
    }

    fVEvent = static_cast<AliVEvent *>(InputEvent());
    fRunNumber = fAODEvent->GetRunNumber();
    fEventNumber = Entry();

    AliVParticle* muonTrack;

    Float_t muonMass2 = AliAnalysisMuonUtility::MuonMass2(); // the PDG rest mass of the muon (constante, used for getting the kinematics) en GeV
    fnMuons = AliAnalysisMuonUtility::GetNTracks(fVEvent); // get the number of muon tracks in the event

    //Get the vertex
    fX = fAODEvent->GetPrimaryVertex()->GetX();
    fY = fAODEvent->GetPrimaryVertex()->GetY();
    fZ = fAODEvent->GetPrimaryVertex()->GetZ();

    //resize vectors respect to Nmuons
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

    fMuon_MCLabel.resize(fnMuons);
    fMuon_MCPDGCode.resize(fnMuons);

    for(Int_t iMuon = 0 ; iMuon < fnMuons ; iMuon++) // loop over all these tracks
    {
        muonTrack = AliAnalysisMuonUtility::GetTrack(iMuon,fVEvent);
        if( !muonTrack ) 
        {
          AliError(Form("ERROR: Could not retrieve AOD or ESD track %d", iMuon)); 
          continue;
        }

        //single muon properties 
        fMuon_E.at(iMuon) = TMath::Sqrt(muonTrack->P()*muonTrack->P() + muonMass2);
        fMuon_Px.at(iMuon) = muonTrack->Px();
        fMuon_Py.at(iMuon) = muonTrack->Py();
        fMuon_Pz.at(iMuon) = muonTrack->Pz();
        fMuon_Charge.at(iMuon)   = muonTrack->Charge();
        fMuon_ThetaAbs.at(iMuon) = AliAnalysisMuonUtility::GetThetaAbsDeg(muonTrack);
        fMuon_xDCA.at(iMuon) = AliAnalysisMuonUtility::GetXatDCA(muonTrack); //Missing info on pTot corr and DCAcorr from OADB
        fMuon_yDCA.at(iMuon) = AliAnalysisMuonUtility::GetYatDCA(muonTrack); //+ massive number of info for pDCA cut...
        fMuon_zDCA.at(iMuon) = AliAnalysisMuonUtility::GetZatDCA(muonTrack); //
        fMuon_MatchTrig.at(iMuon) = AliAnalysisMuonUtility::GetMatchTrigger(muonTrack); //Missing info on the trigger threshold from OADB
        fMuon_TrkClsMap.at(iMuon) = static_cast<const AliAODTrack*>(muonTrack)->GetMUONClusterMap();
        fMuon_TrgHitsMapTrg.at(iMuon) = AliAnalysisMuonUtility::GetMUONTrigHitsMapTrg(muonTrack);
        fMuon_TrgHitsMapTrk.at(iMuon) = AliAnalysisMuonUtility::GetMUONTrigHitsMapTrk(muonTrack);

        Int_t label = muonTrack->GetLabel();
        fMuon_MCLabel.at(iMuon) = label;        

        if(label > 0) fMuon_MCPDGCode.at(iMuon) = fMCEvent->GetTrack(muonTrack->GetLabel())->PdgCode();
        else          fMuon_MCPDGCode.at(iMuon) = 0;
    }


    //MC part

    fXMC = fMCEvent->GetPrimaryVertex()->GetX();
    fYMC = fMCEvent->GetPrimaryVertex()->GetY();
    fZMC = fMCEvent->GetPrimaryVertex()->GetZ();

    Int_t nMCTracks = fMCEvent->GetNumberOfTracks();
    AliVParticle *mctrack = 0x0;
    AliVParticle *mctrackMother = 0x0;

    fnMuonsGen=0;
    for(Int_t iMCTrack = 0; iMCTrack < nMCTracks; ++iMCTrack) {
        mctrack = fMCEvent->GetTrack(iMCTrack); 
        if(AliAnalysisMuonUtility::IsPrimary(mctrack,fMCEvent) && TMath::Abs(mctrack->PdgCode()) == 13) {
            fnMuonsGen++;
            fMuon_GenE.push_back(mctrack->E());
            fMuon_GenPx.push_back(mctrack->Px());
            fMuon_GenPy.push_back(mctrack->Py());
            fMuon_GenPz.push_back(mctrack->Pz());
            fMuon_GenLabel.push_back(mctrack->GetLabel());

            Int_t iMother = mctrack->GetMother();
            mctrackMother = fMCEvent->GetTrack(iMother);
 
            if(mctrackMother && (mctrackMother->PdgCode() == 443 || mctrackMother->PdgCode() == 100443)) {
               fMuon_GenMotherPDGCode.push_back(mctrackMother->PdgCode());
            }
            else {
               fMuon_GenMotherPDGCode.push_back(0);
            }
        }               
    }

    //Fill trees
    fTreeEvent->Fill();
    fTreeGen->Fill();


    //REC info
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

    //MC info
    fMuon_MCLabel.clear();
    fMuon_MCPDGCode.clear();

    //GEN info
    fMuon_GenE.clear();
    fMuon_GenPx.clear();
    fMuon_GenPy.clear();
    fMuon_GenPz.clear();
    fMuon_GenLabel.clear();
    fMuon_GenMotherPDGCode.clear();

    PostData(1, fTreeEvent);
    PostData(2, fTreeGen);
}
//_____________________________________________________________________________
void TaskDimuonppMC::Terminate(Option_t *)
{
    // terminate
    // called at the END of the analysis (when all events are processed)
}    
//_____________________________________________________________________________
