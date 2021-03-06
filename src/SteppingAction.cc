//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B1SteppingAction.cc 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class
// Modify this file to KOMAC-simulator

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "HistoManager.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolume) {
    const DetectorConstruction* detectorConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detectorConstruction->GetScoringVolume();
  }

  // get volume of the current step
  G4LogicalVolume* volume
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();

  // check if we are in scoring volume
  if (volume != fScoringVolume) return;

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // collect energy deposited in this step
  G4double edepStep = step->GetTotalEnergyDeposit();
  fEventAction->AddEdep(edepStep);

  G4double stepLength = step->GetStepLength();

  G4Track* track         = step->GetTrack();
  G4double kinEnergy     = track->GetKineticEnergy();
  G4int pid              = track->GetDynamicParticle()->GetPDGcode();

  G4ThreeVector preStep = step->GetPreStepPoint()->GetPosition();
  G4ThreeVector postStep = step->GetPostStepPoint()->GetPosition();

  G4double kinEnergy_preStep = step->GetPreStepPoint()->GetKineticEnergy();
  G4double kinEnergy_postStep = step->GetPostStepPoint()->GetKineticEnergy();

  G4cout << "Step information" << G4endl;
  G4cout << "------------------------------------------------" << G4endl;
  G4cout << "Pre-step position(mm): (" << preStep.x()/CLHEP::mm << ", " << preStep.y()/CLHEP::mm << ", " << preStep.z()/CLHEP::mm << ")" << G4endl;
  G4cout << "Post-step position(mm): (" << postStep.x()/CLHEP::mm << ", " << postStep.y()/CLHEP::mm << ", " << postStep.z()/CLHEP::mm << ")" << G4endl;
  G4cout << "Step Energy: " << kinEnergy/CLHEP::MeV << " MeV\t|\t";
  G4cout << "Prestep Kinetic Energy: " << kinEnergy_preStep/CLHEP::MeV << " MeV\t|\t";
  G4cout << "Poststep Kinetic Energy: " << kinEnergy_postStep/CLHEP::MeV << " MeV\t|\t";  
  G4cout << "Deposit Energy" <<  edepStep/CLHEP::MeV << " MeV\t|\tStepLength: " << stepLength/CLHEP::cm << " cm" << G4endl;

  analysisManager->FillNtupleIColumn(2,0,pid);
  analysisManager->FillNtupleDColumn(2,1,kinEnergy_preStep);
  analysisManager->FillNtupleDColumn(2,2,edepStep);
  analysisManager->FillNtupleDColumn(2,3,preStep.x());
  analysisManager->FillNtupleDColumn(2,4,preStep.y());
  analysisManager->FillNtupleDColumn(2,5,preStep.z());
  analysisManager->FillNtupleDColumn(2,6,postStep.x());
  analysisManager->FillNtupleDColumn(2,7,postStep.y());
  analysisManager->FillNtupleDColumn(2,8,postStep.z());
  analysisManager->AddNtupleRow();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
