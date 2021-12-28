/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef TaskDimuonppData_H
#define TaskDimuonppData_H

#include "AliAnalysisTaskSE.h"

class AliCounterCollection;

class TaskDimuonppData : public AliAnalysisTaskSE {
public:
  TaskDimuonppData();
  TaskDimuonppData(const char *name, int firstRun, int lastRun);
  virtual ~TaskDimuonppData();

  virtual void NotifyRun();
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t *option);

private:
  AliAODEvent *fAODEvent; //! input event
  AliVEvent *fVEvent;     //! input event
  int fFirstRun, fLastRun;
  Int_t fRunNumber;
  Int_t fEventNumber;
  Float_t fCentrality;
  Bool_t fIsPbPb;

  Double_t fX;
  Double_t fY;
  Double_t fZ;
  Bool_t fMatchCINTEvent;
  Bool_t fMatchCMSLEvent;
  Bool_t fMatchCMSHEvent;
  Bool_t fMatchCMLLEvent;
  Bool_t fMatchCMULEvent;

  Int_t fnMuons;
  std::vector<Float_t> fMuon_E;
  std::vector<Float_t> fMuon_Px;
  std::vector<Float_t> fMuon_Py;
  std::vector<Float_t> fMuon_Pz;
  std::vector<Short_t> fMuon_Charge;
  std::vector<Float_t> fMuon_ThetaAbs;
  std::vector<Double_t> fMuon_xDCA;
  std::vector<Double_t> fMuon_yDCA;
  std::vector<Double_t> fMuon_zDCA;
  std::vector<Int_t> fMuon_MatchTrig;
  std::vector<UInt_t> fMuon_TrkClsMap;     // GetMUONClusterMap()
  std::vector<UInt_t> fMuon_TrgHitsMapTrg; // GetMUONTrigHitsMapTrg()
  std::vector<UInt_t> fMuon_TrgHitsMapTrk; // GetMUONTrigHitsMapTrk()

  TTree *fTreeEvent;

  AliCounterCollection *fEventCounters = nullptr;

  TaskDimuonppData(const TaskDimuonppData &);            // not implemented
  TaskDimuonppData &operator=(const TaskDimuonppData &); // not implemented

  ClassDef(TaskDimuonppData, 2);
};

#endif
