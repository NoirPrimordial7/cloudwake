#include "BHTestDriver.h"
#include "Bellheart.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Misc/OutputDevice.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "UnrealClient.h"

ABHTestDriver::ABHTestDriver() { PrimaryActorTick.bCanEverTick=true; }
bool ABHTestDriver::Check(bool OK,const TCHAR* What) {
 if(!OK) { UE_LOG(LogTemp,Error,TEXT("BH_TEST_FAIL step=%d %s"),Step,What); FPlatformMisc::RequestExitWithStatus(false,1); }
 else UE_LOG(LogTemp,Display,TEXT("BH_TEST_PASS %s"),What);
 return OK;
}
void ABHTestDriver::Next() {
 if(FParse::Param(FCommandLine::Get(),TEXT("BHShots")) && (Step==3 || Step==6 || Step==11 || Step==14)) {
  auto* P=Cast<ABHPlayer>(UGameplayStatics::GetPlayerPawn(this,0));
  if(P) {
   FVector Focus=P->Camera->GetComponentLocation()+P->Camera->GetForwardVector()*500;
   if(Step==3 && P->HookedFish)Focus=P->HookedFish->GetActorLocation();
   if(Step==11)Focus=FVector(2600,-1100,850);
   if(Step==14)Focus=FVector(0,7600,5300);
   P->GetController()->SetControlRotation((Focus-P->Camera->GetComponentLocation()).Rotation());
   P->Camera->SetWorldRotation(P->GetControlRotation());
  }
  FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/FString::Printf(TEXT("Screenshots/Bellheart_Step_%02d.png"),Step),true,false);
 }
 ++Step; Time=0;
}
void ABHTestDriver::Tick(float D) {
 Super::Tick(D); Time+=D; Total+=D;
 if(Total>120) { Check(false,TEXT("120 second loop timeout")); SetActorTickEnabled(false); return; }
 auto* P=Cast<ABHPlayer>(UGameplayStatics::GetPlayerPawn(this,0)); if(!P || Time<.4)return;
 if(FParse::Param(FCommandLine::Get(),TEXT("BHSaveTest"))) {
  UGameplayStatics::DeleteGameInSlot(P->SaveSlot(),0);
  P->Progress->Quest=8; P->Progress->Crowns=91; P->Progress->HasRod=true; P->Progress->HasKnife=true; P->Progress->KnifeDamage=30;
  auto* Heart=GetWorld()->SpawnActor<ABHPhysicalItem>(P->GetActorLocation(),FRotator::ZeroRotator);
  Heart->Kind=EBHItem::Bellheart; Heart->Health=0; Heart->State=EBHFishState::Dead; Heart->Use(P);
  if(!Check(P->SaveCheckpoint(),TEXT("Checkpoint writes to isolated slot")))return;
  P->Progress->Quest=0; P->Progress->Crowns=0; P->Progress->KnifeDamage=18;
  if(!Check(P->LoadCheckpoint(),TEXT("Checkpoint loads from disk")))return;
  if(!Check(P->Progress->Quest==8 && P->Progress->Crowns==91 && P->Progress->KnifeDamage==30 && P->HeldItem && P->HeldItem->Kind==EBHItem::Bellheart,TEXT("Quest money upgrade and carried heart survive load")))return;
  P->LoadCheckpoint(); int32 Hearts=0;
  for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It) if(It->Kind==EBHItem::Bellheart)++Hearts;
  if(!Check(Hearts==1,TEXT("Repeated load cannot duplicate Bellheart")))return;
  P->HeldItem->Destroy(); P->HeldItem=nullptr; P->SaveCheckpoint(); P->LoadCheckpoint(); Hearts=0;
  for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It)if(It->Kind==EBHItem::Bellheart)++Hearts;
  if(!Check(Hearts==1,TEXT("Missing quest object is recovered")))return;
  auto* Fish=GetWorld()->SpawnActor<ABHPhysicalItem>(P->GetActorLocation()+FVector(150,0,0),FRotator::ZeroRotator);
  Fish->Label=TEXT("PersistenceTestFish"); Fish->Value=47; Fish->Health=0; Fish->Land(Fish->GetActorLocation()); Fish->State=EBHFishState::Dead;
  P->Health=63; P->SaveCheckpoint(); P->Health=100; P->LoadCheckpoint();
  int32 SavedFish=0;
  for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It)if(It->Label==TEXT("PersistenceTestFish") && It->Value==47 && It->Health==0 && It->Mesh->IsSimulatingPhysics())++SavedFish;
  if(!Check(SavedFish==1 && P->Health==63,TEXT("Landed fish value physics and player health survive load")))return;
  P->Charging=true; P->CastCharge=1; P->CancelCast();
  if(!Check(!P->Charging && !P->Bobber && !P->HookedFish,TEXT("Cancel clears pending fishing input")))return;
  for(TActorIterator<ABHBellcrab> It(GetWorld());It;++It) { It->Active=true; It->Health=40; }
  P->RecoverAtDock();
  for(TActorIterator<ABHBellcrab> It(GetWorld());It;++It)if(!Check(!It->Active && It->Health==180,TEXT("Recovery resets interrupted boss encounter")))return;
  UGameplayStatics::DeleteGameInSlot(P->SaveSlot(),0);
  UE_LOG(LogTemp,Display,TEXT("BH_SAVE_TEST_COMPLETE persistence and recovery"));
  FPlatformMisc::RequestExitWithStatus(false,0); SetActorTickEnabled(false); return;
 }
 if(FParse::Param(FCommandLine::Get(),TEXT("BHWalk"))) {
  const TArray<FVector> Route={{0,-65,3},{0,-54,7},{0,-49,7},{17,-35,7},{27,-21,7},{29,5,7},{16,18,14},{12,22,14},{0,30,14},{-10,44,20},{-14,53,20},{-14,63,20},{-3,65,26},{0,65,26},{0,71,26}};
  if(WalkPoint>=Route.Num()) { UE_LOG(LogTemp,Display,TEXT("BH_WALK_COMPLETE dock-to-pond-to-village-to-elder-to-tower %.2fs"),Total); FPlatformMisc::RequestExitWithStatus(false,0); SetActorTickEnabled(false); return; }
  FVector Goal=Route[WalkPoint]*100;
  if(FVector::Dist2D(P->GetActorLocation(),Goal)<65) {
   if(!Check(FMath::Abs(P->GetActorLocation().Z-Goal.Z-90)<55 && P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Route waypoint reached on foot at intended elevation"))) { SetActorTickEnabled(false); return; }
   UE_LOG(LogTemp,Display,TEXT("BH_WALK_PASS point=%d position=%s"),WalkPoint,*P->GetActorLocation().ToString()); ++WalkPoint; Time=0;
  }
  else { FVector Direction=Goal-P->GetActorLocation(); Direction.Z=0; P->AddMovementInput(Direction.GetSafeNormal()); P->GetController()->SetControlRotation(Direction.Rotation()); }
  if(Time>18) { UE_LOG(LogTemp,Error,TEXT("BH_WALK_FAIL point=%d position=%s goal=%s"),WalkPoint,*P->GetActorLocation().ToString(),*Goal.ToString()); FPlatformMisc::RequestExitWithStatus(false,1); SetActorTickEnabled(false); }
  return;
 }
 auto Use=[&](FName Action) {
  for(TActorIterator<ABHInteractable> It(GetWorld());It;++It) if(It->Action==Action) {
   FVector Spot=It->GetActorLocation()+FVector(0,-180,0); Spot.Z=It->GetActorLocation().Z+45;
   P->SetActorLocation(Spot); P->GetCharacterMovement()->StopMovementImmediately();
   P->GetController()->SetControlRotation((It->GetActorLocation()-P->Camera->GetComponentLocation()).Rotation());
   P->Camera->SetWorldRotation(P->GetControlRotation()); P->Interact(); return;
  }
  Check(false,TEXT("Missing station"));
 };
 switch(Step) {
 case 0:
  P->GetController()->SetIgnoreLookInput(true);
  if(!Check(P->GetActorLocation().Z>80 && P->GetActorLocation().Z<150,TEXT("Player standing on dock at correct capsule height")))return;
  Use("Orin"); if(Check(P->Progress->Quest==1,TEXT("Camera trace talks to Orin")))Next(); break;
 case 1:
  Use("Mira"); if(Check(P->Progress->HasRod && P->Progress->Quest==2,TEXT("Mira gives rod and advances tutorial")))Next(); break;
 case 2:
  P->SetActorLocation(FVector(-1000,-4000,800)); P->GetCharacterMovement()->StopMovementImmediately();
  P->GetController()->SetControlRotation(FRotator(-10,90,0)); P->Camera->SetWorldRotation(P->GetControlRotation()); P->CastLine(.6f);
  if(Check(P->Bobber!=nullptr,TEXT("Cast creates traveling physical bobber")))Next(); break;
 case 3:
  for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It) if(It->State==EBHFishState::Bite) { P->Camera->SetWorldRotation(P->GetControlRotation()); P->PrimaryDown(); Check(P->HookedFish!=nullptr,TEXT("Fish AI investigates and bites; input hooks it")); Next(); break; }
  break;
 case 4:
  if(P->HookedFish) P->Reeling=P->Tension<.72f;
  else if(P->Progress->Quest==3) { Check(true,TEXT("Reeling lands physical fish and advances quest")); Next(); }
  break;
 case 5:
  for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It) if(It->State==EBHFishState::Landed || It->State==EBHFishState::Dead) {
   P->GetController()->SetControlRotation((It->GetActorLocation()-P->Camera->GetComponentLocation()).Rotation());
   if(It->Health>0) { P->Camera->SetWorldRotation(P->GetControlRotation()); P->PrimaryDown(); }
   else { P->Camera->SetWorldRotation(P->GetControlRotation()); P->Interact(); if(Check(P->HeldItem==*It,TEXT("Dead fish picked up through camera interaction")))Next(); }
   break;
  } break;
 case 6:
  Use("Sell"); if(Check(!P->HeldItem && P->Progress->Crowns>=44 && P->Progress->Quest==4,TEXT("Sale consumes carried fish and pays Crowns")))Next(); break;
 case 7:
  Use("Knife"); if(Check(P->Progress->HasKnife && P->Progress->Quest==5,TEXT("Shop purchase spends currency and equips knife")))Next(); break;
 case 8:
  Use("Sharpen"); if(Check(P->Progress->KnifeDamage==30,TEXT("Grindstone upgrade changes damage")))Next(); break;
 case 9:
  Use("Tower"); if(Check(P->Progress->Quest==6,TEXT("Tower inspection opens bait quest")))Next(); break;
 case 10:
  Use("Mira"); if(Check(P->Progress->HasBait && P->Progress->Quest==7,TEXT("Mira grants quest bait")))Next(); break;
 case 11:
  Use("Lure"); for(TActorIterator<ABHBellcrab> It(GetWorld());It;++It) if(Check(It->Active,TEXT("Quest bait summons boss")))Next(); break;
 case 12:
  for(TActorIterator<ABHBellcrab> It(GetWorld());It;++It) {
   if(It->Health<=0) { Next(); break; }
   P->SetActorLocation(It->GetActorLocation()+FVector(0,-310,0)); P->GetCharacterMovement()->StopMovementImmediately();
   P->GetController()->SetControlRotation((It->GetActorLocation()-P->Camera->GetComponentLocation()).Rotation()); P->Camera->SetWorldRotation(P->GetControlRotation()); P->PrimaryDown();
  } break;
 case 13:
  if(!Check(P->Progress->Quest==8,TEXT("Knife combat defeats boss and advances quest")))return;
  for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It) if(It->Kind==EBHItem::Bellheart) {
   P->SetActorLocation(It->GetActorLocation()+FVector(0,-150,40)); P->GetController()->SetControlRotation((It->GetActorLocation()-P->Camera->GetComponentLocation()).Rotation()); P->Camera->SetWorldRotation(P->GetControlRotation()); P->Interact();
  }
  if(Check(P->HeldItem && P->HeldItem->Kind==EBHItem::Bellheart,TEXT("Boss drops a physical carryable Bellheart")))Next(); break;
 case 14:
  Use("Tower"); if(Check(P->Progress->Quest==9 && !P->HeldItem,TEXT("Physical Bellheart installation completes quest")))Next(); break;
 case 15:
  Use("Tavi"); UE_LOG(LogTemp,Display,TEXT("BH_TEST_COMPLETE full arrival-to-restoration loop %.2fs"),Total); FPlatformMisc::RequestExitWithStatus(false,0); SetActorTickEnabled(false); break;
 }
}
