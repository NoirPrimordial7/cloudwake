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
 if(FParse::Param(FCommandLine::Get(),TEXT("BHAirTest"))) { AirTest(D); return; }
 if(Total>(FParse::Param(FCommandLine::Get(),TEXT("BHManualPreview"))?1200:120)) { Check(false,TEXT("120 second loop timeout")); SetActorTickEnabled(false); return; }
 auto* P=Cast<ABHPlayer>(UGameplayStatics::GetPlayerPawn(this,0)); if(!P || Time<.4)return;
 if(FParse::Param(FCommandLine::Get(),TEXT("BHArtShots"))) {
  // Fixed photographic stations in the real playable map; not a movement test.
  const FVector Locations[]={ {0,-8500,100},{-1000,-4200,800},{1500,-4200,800},{0,1900,1500},{-2400,2250,1500},{2700,2200,1500},{-2800,-300,1100},{0,6200,2400},{12000,-19000,13500} };
  const FVector Targets[]={ {0,3000,2000},{-500,-1400,900},{0,7600,4800},{-2000,3300,1900},{-2400,3000,1650},{2700,3000,1700},{-3400,500,2100},{0,7600,4400},{0,0,1400} };
  if(Step>=9) { UE_LOG(LogTemp,Display,TEXT("BH_ART_SHOTS_COMPLETE")); FPlatformMisc::RequestExit(false); return; }
  if(Step==8)P->GetCharacterMovement()->DisableMovement();
  if(Time<1.f) {
   P->SetActorLocation(Locations[Step]); P->GetCharacterMovement()->StopMovementImmediately();
   P->GetController()->SetControlRotation((Targets[Step]-P->Camera->GetComponentLocation()).Rotation());
  }
  if(Time>5.f) {
   FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/FString::Printf(TEXT("Screenshots/ArtPass/Bellheart_%02d.png"),Step+1),true,false);
   ++Step; Time=0;
  }
  return;
 }
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
  // Containers retain actual physical item instances and round-trip alongside a carried object.
  P->Progress->Crowns=200;
  for(TActorIterator<ABHInteractable> It(GetWorld());It;++It)if(It->Action=="Bag" || It->Action=="Bucket")It->Use(P);
  if(!Check(P->HasBag && P->HasBucket && P->Progress->Crowns==132,TEXT("Bag and bucket purchases charge exactly once")))return;
  for(TActorIterator<ABHInteractable> It(GetWorld());It;++It)if(It->Action=="Bag" || It->Action=="Bucket")It->Use(P);
  if(!Check(P->Progress->Crowns==132,TEXT("Duplicate container purchase rejected")))return;
  P->AssignToSlot(1,3); P->SlotFour();
  if(!Check(P->ActiveGear()==1,TEXT("Rod can be moved to slot four")))return;
  P->ScrollNext(); if(!Check(P->SelectedSlot==0 && P->ActiveGear()==4,TEXT("Wheel wraps and selects assigned bucket")))return;
  for(int32 I=0;I<7;++I) {
   auto* F=GetWorld()->SpawnActor<ABHPhysicalItem>(P->GetActorLocation()+FVector(100,0,0),FRotator::ZeroRotator);
   F->Label=TEXT("StoredTestFish"); F->Value=33+I; F->Health=0; F->State=EBHFishState::Dead; F->Use(P);
   const bool Stored=P->StoreIn(4);
   if(!Check(Stored==(I<6),TEXT("Bucket enforces six-fish capacity")))return;
  }
  // Seventh fish remains in hands; stored items must load even with hands occupied.
  P->SaveCheckpoint(); P->LoadCheckpoint();
  if(!Check(P->StoredItems(4).Num()==6 && P->HeldItem && P->HeldItem->Value==39 && P->Hotbar[3]==1 && P->ActiveGear()==4,TEXT("Stored catches carried fish and hotbar survive disk reload")))return;
  P->Drop(); P->RetrieveStored(0);
  if(!Check(P->StoredItems(4).Num()==5 && P->HeldItem && !P->HeldItem->StoredIn && !P->HeldItem->IsHidden(),TEXT("Retrieve restores one physical fish without duplication")))return;
  P->Drop();
  for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It)if(It->Kind==EBHItem::Bellheart) { It->Use(P); break; }
  if(!Check(P->HeldItem && !P->StoreIn(4) && P->StoreIn(3),TEXT("Quest heart rejected by bucket and accepted by bag")))return;
  P->SaveCheckpoint(); P->LoadCheckpoint(); P->LoadCheckpoint();
  if(!Check(P->StoredItems(3).Num()==1 && P->StoredItems(4).Num()==5,TEXT("Repeated reload preserves storage without duplication")))return;
  UGameplayStatics::DeleteGameInSlot(P->SaveSlot(),0);
  UE_LOG(LogTemp,Display,TEXT("BH_SAVE_TEST_COMPLETE persistence and recovery"));
  FPlatformMisc::RequestExitWithStatus(false,0); SetActorTickEnabled(false); return;
 }
 if(FParse::Param(FCommandLine::Get(),TEXT("BHWalk")) || FParse::Param(FCommandLine::Get(),TEXT("BHSideWalk"))) {
  const bool SideWalk=FParse::Param(FCommandLine::Get(),TEXT("BHSideWalk"));
  if(SideWalk && Step==0) { P->SetActorLocation(FVector(-1000,-4300,800)); P->GetCharacterMovement()->StopMovementImmediately(); Step=1; Time=0; return; }
  TArray<FVector> Route={{0,-65,3},{0,-54,7},{0,-49,7},{17,-35,7},{27,-21,7},{29,5,7},{16,18,14},{12,22,14},{0,30,14},{-10,44,20},{-14,53,20},{-14,63,20},{-3,65,26},{0,65,26},{0,71,26}};
  if(SideWalk)Route={{-29,-36,7},{-47,-24,6},{-53,-2,9},{-51,17,11},{-38,5,10},{-38,17,10},{-24,18,14},{-24,24,14},{0,24,14},{27,24,14},{35,24,14},{38,3,10},{46,3,10}};
  if(WalkPoint>=Route.Num()) { UE_LOG(LogTemp,Display,TEXT("BH_WALK_COMPLETE route=%s %.2fs"),SideWalk ? TEXT("waterfall-hidden-tree-shops-workshop") : TEXT("dock-pond-village-elder-tower"),Total); FPlatformMisc::RequestExitWithStatus(false,0); SetActorTickEnabled(false); return; }
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
 if(FParse::Param(FCommandLine::Get(),TEXT("BHInventoryPreview")) && Step==2) {
  if(FParse::Param(FCommandLine::Get(),TEXT("BHManualPreview")))return;
  // Native clicks are verified separately; OS cursor warping is unreliable in unattended windows.
  if(Time<1.2f)FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("Screenshots/Bellheart_Inventory.png"),true,false);
  if(Time>2.f) { UE_LOG(LogTemp,Display,TEXT("BH_INVENTORY_PREVIEW_COMPLETE")); FPlatformMisc::RequestExitWithStatus(false,0); }
  return;
 }
 switch(Step) {
 case 0:
  P->GetController()->SetIgnoreLookInput(true);
  if(!Check(P->GetActorLocation().Z>80 && P->GetActorLocation().Z<150,TEXT("Player standing on dock at correct capsule height")))return;
  Use("Orin"); if(Check(P->Progress->Quest==1 && P->DialogueSpeaker && P->SpeakerName==TEXT("Orin"),TEXT("Camera trace talks to Orin and anchors dialogue")))Next(); break;
 case 1:
  if(FParse::Param(FCommandLine::Get(),TEXT("BHInventoryPreview"))) {
   P->Progress->HasRod=true; P->Progress->HasKnife=true; P->HasBag=true; P->HasBucket=true;
   for(int32 I=0;I<3;++I) { auto* F=GetWorld()->SpawnActor<ABHPhysicalItem>(P->GetActorLocation(),FRotator::ZeroRotator); F->Health=0; F->State=EBHFishState::Dead; F->Label=I==0?TEXT("Cloudfin"):TEXT("Pebblecarp"); F->Use(P); P->StoreIn(4); }
   P->AssignToSlot(1,3); P->ToggleInventory(); Next(); return;
  }
  if(FParse::Param(FCommandLine::Get(),TEXT("BHDialoguePreview"))) {
   if(Time<1.0f)return;
   if(Time<1.2f)FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("Screenshots/Bellheart_Dialogue.png"),true,false);
   if(Time>2.f)FPlatformMisc::RequestExitWithStatus(false,0);
   return;
  }
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
