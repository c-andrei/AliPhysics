/*************************************************************************
* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *f
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  * 
**************************************************************************/

/**************************************
 *    analysis task for fitting       * 
 *         q-distribution             *
 *                                    * 
 * authors: Naomi van der Kolk        *
 *           (kolk@nikhef.nl)         *  
 *          Raimond Snellings         *
 *           (snelling@nikhef.nl)     * 
 *          Ante Bilandzic            *
 *           (abilandzic@gmail.com)   * 
 * ***********************************/
 
class TFile;
class TString;
class TList;
class AliAnalysisTaskSE; 
 
#include "Riostream.h"
#include "AliFlowEventSimple.h"
#include "AliAnalysisTaskFittingQDistribution.h"
#include "AliFlowAnalysisWithFittingQDistribution.h"
#include "TH2D.h"

using std::cout;
using std::endl;
ClassImp(AliAnalysisTaskFittingQDistribution)

//================================================================================================================

AliAnalysisTaskFittingQDistribution::AliAnalysisTaskFittingQDistribution(const char *name, Bool_t useWeights): 
 AliAnalysisTaskSE(name), 
 fEvent(NULL),
 fFQD(NULL),
 fListHistos(NULL),
 fBookOnlyBasicCCH(kTRUE), 
 fUseWeights(useWeights),
 fUsePhiWeights(kFALSE),
 fListWeights(NULL),
 fHarmonic(2), 
 fqMin(0.),
 fqMax(100.),
 fqNbins(10000),
 fStoreqDistributionVsMult(kFALSE),
 fqDistributionVsMult(NULL),
 fMinMult(0.),
 fMaxMult(10000.),
 fnBinsMult(1000),
 fDoFit(kTRUE),
 fExactNoRPs(0),
 fMultiplicityIs(AliFlowCommonConstants::kRP)  
 {
  //constructor
  //cout<<"AliAnalysisTaskFittingQDistribution::AliAnalysisTaskFittingQDistribution(const char *name, Bool_t useWeights)"<<endl;
  
  // Define input and output slots here
  // Input slot #0 works with an AliFlowEventSimple
  DefineInput(0, AliFlowEventSimple::Class());
  
  // Input slot #1 is needed for the weights input files 
  if(useWeights) 
  {
   DefineInput(1, TList::Class());   
  }
  // Output slot #0 is reserved
  // Output slot #1 writes into a TList container
  DefineOutput(1, TList::Class());  
 }

AliAnalysisTaskFittingQDistribution::AliAnalysisTaskFittingQDistribution(): 
 AliAnalysisTaskSE(),
 fEvent(NULL),
 fFQD(NULL),
 fListHistos(NULL), 
 fBookOnlyBasicCCH(kFALSE),   
 fUseWeights(kFALSE),
 fUsePhiWeights(kFALSE),
 fListWeights(NULL),
 fHarmonic(0), 
 fqMin(0.),
 fqMax(0.),
 fqNbins(0),
 fStoreqDistributionVsMult(kFALSE),
 fqDistributionVsMult(NULL),
 fMinMult(0.),
 fMaxMult(0.),
 fnBinsMult(0),
 fDoFit(kTRUE),
 fExactNoRPs(0),
 fMultiplicityIs(AliFlowCommonConstants::kRP)  
 {
  // Dummy constructor
  //cout<<"AliAnalysisTaskFittingQDistribution::AliAnalysisTaskFittingQDistribution()"<<endl;
 }

//================================================================================================================

void AliAnalysisTaskFittingQDistribution::UserCreateOutputObjects() 
{
  // Called at every worker node to initialize
  cout<<"AliAnalysisTaskFittingQDistribution::UserCreateOutputObjects()"<<endl;
  
  // Analyser:
  fFQD = new AliFlowAnalysisWithFittingQDistribution();
  
  fFQD->SetBookOnlyBasicCCH(fBookOnlyBasicCCH);
  // Particle weights:
  if(fUseWeights) 
  {
   // Pass the flags to class:
   if(fUsePhiWeights) fFQD->SetUsePhiWeights(fUsePhiWeights);
   // Get data from input slot #1 which is used for weights:
   if(GetNinputs()==2) 
   {                   
    fListWeights = (TList*)GetInputData(1); 
   }
   // Pass the list with weights to class:
   if(fListWeights) fFQD->SetWeightsList(fListWeights);
  }
  fFQD->SetHarmonic(fHarmonic);
  // Settings for q-distribution:
  fFQD->SetqMin(fqMin);
  fFQD->SetqMax(fqMax);
  fFQD->SetqNbins(fqNbins); 
  fFQD->SetStoreqDistributionVsMult(fStoreqDistributionVsMult);
  fFQD->SetqDistributionVsMult(fqDistributionVsMult);
  fFQD->SetMinMult(fMinMult);
  fFQD->SetMaxMult(fMaxMult);
  fFQD->SetnBinsMult(fnBinsMult);
  fFQD->SetDoFit(fDoFit);
  fFQD->SetExactNoRPs(fExactNoRPs);
  fFQD->SetMultiplicityIs(fMultiplicityIs);

  fFQD->Init();
  
  if(fFQD->GetHistList()) 
  {
   fListHistos = fFQD->GetHistList();
   //fListHistos->Print();
  } else 
    {
      Printf("ERROR: Could not retrieve histogram list (FQD, Task::UserCreateOutputObjects()) !!!!"); 
    }

 PostData(1,fListHistos);
  
} // end of void AliAnalysisTaskFittingQDistribution::UserCreateOutputObjects()

//================================================================================================================

void AliAnalysisTaskFittingQDistribution::UserExec(Option_t *) 
{
  // Main loop (called for each event):
  fEvent = dynamic_cast<AliFlowEventSimple*>(GetInputData(0));

  // Fitting q-distribution: 
  if(fEvent) 
  {
   fFQD->Make(fEvent);
  } else 
    {
     cout<<"WARNING: No input data (FQD, Task::UserExec()) !!!!"<<endl;
    }
  
 PostData(1,fListHistos); 
}

//================================================================================================================

void AliAnalysisTaskFittingQDistribution::Terminate(Option_t *) 
{  
  //accessing the output list
  fListHistos = (TList*)GetOutputData(1);
  
  fFQD = new AliFlowAnalysisWithFittingQDistribution();
  
  if(fListHistos) 
  {	     
   fFQD->GetOutputHistograms(fListHistos);
   fFQD->Finish();  
   PostData(1,fListHistos);
  } else 
    {
     cout<<" WARNING: histogram list pointer is empty (FQD, Task::Terminate()) !!!!"<<endl;
     cout<<endl;
    }
    
} // end of void AliAnalysisTaskFittingQDistribution::Terminate(Option_t *)

//================================================================================================================



















