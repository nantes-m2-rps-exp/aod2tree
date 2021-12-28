/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef TaskDimuonppMC_H
#define TaskDimuonppMC_H

#include "AliAnalysisTaskSE.h"

class TaskDimuonppMC : public AliAnalysisTaskSE  
{
    public:
                                TaskDimuonppMC();
                                TaskDimuonppMC(const char *name,int firstRun, int lastRun);
        virtual                 ~TaskDimuonppMC();
        
        virtual void            NotifyRun();
        virtual void            UserCreateOutputObjects();
        virtual void            UserExec(Option_t* option);
        virtual void            Terminate(Option_t* option);

    private:
        AliAODEvent*            fAODEvent;      //! input event
        AliMCEvent*             fMCEvent;       //! input event
        AliVEvent*              fVEvent;        //! input event
        int                     fFirstRun, fLastRun;
        Int_t                   fRunNumber;
        Int_t                   fEventNumber;
        Float_t                 fCentrality;
        Bool_t                  fIsPbPb;

        //REC info
        Double_t fX;      
        Double_t fY;      
        Double_t fZ;      
        Bool_t fMatchCINTEvent;    
        Bool_t fMatchCMSLEvent;    
        Bool_t fMatchCMSHEvent;    
        Bool_t fMatchCMLLEvent;    
        Bool_t fMatchCMULEvent;

        Int_t fnMuons;    
        std::vector<Float_t>  fMuon_E;      
        std::vector<Float_t>  fMuon_Px;      
        std::vector<Float_t>  fMuon_Py;      
        std::vector<Float_t>  fMuon_Pz;      
        std::vector<Short_t>  fMuon_Charge;
        std::vector<Float_t>  fMuon_ThetaAbs;
        std::vector<Double_t> fMuon_xDCA;      
        std::vector<Double_t> fMuon_yDCA;      
        std::vector<Double_t> fMuon_zDCA; 
        std::vector<Int_t>    fMuon_MatchTrig;
        std::vector<UInt_t>   fMuon_TrkClsMap; //GetMUONClusterMap() 
        std::vector<UInt_t>   fMuon_TrgHitsMapTrg;//GetMUONTrigHitsMapTrg()
        std::vector<UInt_t>   fMuon_TrgHitsMapTrk;//GetMUONTrigHitsMapTrk()

        //MC info
        std::vector<Int_t>    fMuon_MCLabel;
        std::vector<Int_t>    fMuon_MCPDGCode;


        //GEN info
        Double_t fXMC;      
        Double_t fYMC;      
        Double_t fZMC;      
        Int_t fnMuonsGen;
        std::vector<Float_t>  fMuon_GenE;
        std::vector<Float_t>  fMuon_GenPx;
        std::vector<Float_t>  fMuon_GenPy;
        std::vector<Float_t>  fMuon_GenPz;
        std::vector<Int_t>    fMuon_GenLabel;
        std::vector<Int_t>    fMuon_GenMotherPDGCode;

        //Trees
        TTree *fTreeEvent;   
        TTree *fTreeGen;   

        TaskDimuonppMC(const TaskDimuonppMC&); // not implemented
        TaskDimuonppMC& operator=(const TaskDimuonppMC&); // not implemented

        ClassDef(TaskDimuonppMC, 2);
};

#endif
