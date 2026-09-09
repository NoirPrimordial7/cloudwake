#include "BHSaveGame.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

bool ABHPlayer::IsLoopTest() const {
 return FParse::Param(FCommandLine::Get(),TEXT("BHTest")) || FParse::Param(FCommandLine::Get(),TEXT("BHWalk"));
}
FString ABHPlayer::SaveSlot() const {
 return FParse::Param(FCommandLine::Get(),TEXT("BHSaveTest")) ? TEXT("Bellheart_AutomationOnly") : TEXT("Bellheart_Checkpoint_v1");
}
bool ABHPlayer::SaveCheckpoint() {
 if(IsLoopTest()) return false; // Progression and traversal tests never touch a player's save.
 auto* Save=Cast<UBHSaveGame>(UGameplayStatics::CreateSaveGameObject(UBHSaveGame::StaticClass()));
 Save->Crowns=Progress->Crowns; Save->Quest=Progress->Quest; Save->FishSold=Progress->FishSold;
 Save->HasRod=Progress->HasRod; Save->HasKnife=Progress->HasKnife; Save->HasBait=Progress->HasBait;
 Save->KnifeDamage=Progress->KnifeDamage; Save->Health=Health; Save->KnifeEquipped=KnifeEquipped;
 if(GetCharacterMovement()->IsMovingOnGround()) Save->Position=GetActorLocation();
 Save->View=GetControlRotation();
 for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It) {
  if(It->Kind!=EBHItem::Bellheart && It->State!=EBHFishState::Landed && It->State!=EBHFishState::Dead)continue;
  FBHSavedItem Record;
  Record.Kind=It->Kind; Record.Health=It->Health; Record.Value=It->Value; Record.Label=It->Label;
  Record.Position=It->GetActorLocation(); Record.Held=HeldItem==*It;
  if(Record.Position.Z < -2000) Record.Position=FVector(0,-8200,150);
  Save->Items.Add(Record);
 }
 const bool OK=UGameplayStatics::SaveGameToSlot(Save,SaveSlot(),0);
 UE_LOG(LogTemp,Display,TEXT("BH_SAVE %s quest=%d items=%d"),OK ? TEXT("OK") : TEXT("FAILED"),Save->Quest,Save->Items.Num());
 return OK;
}
bool ABHPlayer::LoadCheckpoint() {
 if(IsLoopTest() || !UGameplayStatics::DoesSaveGameExist(SaveSlot(),0))return false;
 auto* Save=Cast<UBHSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot(),0));
 if(!Save || Save->Version!=1 || Save->Quest<0 || Save->Quest>9 || Save->Items.Num()>128) {
  Say(TEXT("Checkpoint could not be loaded. Current session was kept.")); return false;
 }
 CancelCast();
 HeldItem=nullptr; Target=nullptr;
 for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It) {
  if(It->Kind==EBHItem::Bellheart || It->State==EBHFishState::Landed || It->State==EBHFishState::Dead)It->Destroy();
 }
 Progress->Crowns=FMath::Max(0,Save->Crowns); Progress->Quest=Save->Quest; Progress->FishSold=Save->FishSold;
 Progress->HasRod=Save->HasRod; Progress->HasKnife=Save->HasKnife; Progress->HasBait=Save->HasBait;
 Progress->KnifeDamage=Save->KnifeDamage>=30 ? 30 : 18; Health=FMath::Clamp(Save->Health,1.f,100.f);
 GetCharacterMovement()->StopMovementImmediately();
 FVector Position=Save->Position;
 if(Position.ContainsNaN() || Position.Z < -100 || Position.Z>6000)Position=FVector(0,-8600,110);
 SetActorLocation(Position); GetController()->SetControlRotation(Save->View);
 Camera->SetWorldRotation(Save->View);
 if(Save->KnifeEquipped && Progress->HasKnife)EquipKnife(); else EquipRod();
 bool FoundHeart=false;
 for(const FBHSavedItem& Record:Save->Items) {
  if(Record.Kind==EBHItem::Bellheart && (Progress->Quest!=8 || FoundHeart))continue;
  auto* Item=GetWorld()->SpawnActor<ABHPhysicalItem>(Record.Position,FRotator::ZeroRotator);
  Item->Kind=Record.Kind; Item->Health=FMath::Max(0.f,Record.Health); Item->Value=Record.Value; Item->Label=Record.Label;
  Item->Land(Record.Position); if(Item->Health<=0)Item->State=EBHFishState::Dead;
  if(Item->Kind==EBHItem::Bellheart) { FoundHeart=true; Item->SetActorScale3D(FVector(.3,.3,.7)); }
  if(Record.Held && !HeldItem)Item->Use(this);
 }
 // A dropped quest object must never permanently block restoration.
 if(Progress->Quest==8 && !FoundHeart) {
  auto* Heart=GetWorld()->SpawnActor<ABHPhysicalItem>(FVector(0,-8200,150),FRotator::ZeroRotator);
  Heart->Kind=EBHItem::Bellheart; Heart->Health=0; Heart->State=EBHFishState::Dead;
  Heart->SetActorScale3D(FVector(.3,.3,.7)); Heart->Mesh->SetSimulatePhysics(true);
 }
 for(TActorIterator<ABHBellcrab> It(GetWorld());It;++It) {
  It->Active=false; It->Telegraph=false; It->PhaseTime=0; It->Health=Progress->Quest>=8 ? 0 : 180;
  It->SetActorLocation(It->Arena+FVector(0,0,90)); It->SetActorHiddenInGame(true); It->SetActorEnableCollision(false);
 }
 for(TActorIterator<ABHWorld> It(GetWorld());It;++It)It->Restore(Progress->Quest>=9,false);
 AutoSaveTime=0; Say(TEXT("Checkpoint loaded. Interrupted fishing and boss encounters can be restarted."));
 UE_LOG(LogTemp,Display,TEXT("BH_LOAD OK quest=%d items=%d"),Progress->Quest,Save->Items.Num());
 return true;
}
void ABHPlayer::QuickSave() { Say(SaveCheckpoint() ? TEXT("Checkpoint saved.") : TEXT("Checkpoint was not saved.")); }
void ABHPlayer::QuickLoad() { if(!LoadCheckpoint())Say(TEXT("No compatible checkpoint loaded. Your current session is unchanged.")); }
void ABHPlayer::RecoverAtDock() {
 CancelCast(); GetCharacterMovement()->StopMovementImmediately(); Health=100;
 SetActorLocation(FVector(0,-8600,110));
 for(TActorIterator<ABHBellcrab> It(GetWorld());It;++It) if(It->Active) {
  It->Active=false; It->Health=180; It->PhaseTime=0; It->Telegraph=false;
  It->SetActorLocation(It->Arena+FVector(0,0,90)); It->SetActorHiddenInGame(true); It->SetActorEnableCollision(false);
 }
 Say(TEXT("Recovered at the dock. Items and Crowns retained. Use the arena lure to retry Bellcrab.")); SaveCheckpoint();
}
