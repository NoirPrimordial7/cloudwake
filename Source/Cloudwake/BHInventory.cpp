#include "Bellheart.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Canvas.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"

bool ABHPlayer::OwnsGear(int32 G) const {
 return G==1 ? Progress->HasRod : G==2 ? Progress->HasKnife : G==3 ? HasBag : G==4 ? HasBucket : false;
}
FString ABHPlayer::GearName(int32 G) const {
 return G==1 ? TEXT("Fishing rod") : G==2 ? TEXT("Iron knife") : G==3 ? TEXT("Satchel") : G==4 ? TEXT("Fish bucket") : TEXT("Empty");
}
int32 ABHPlayer::ActiveGear() const { return Hotbar.IsValidIndex(SelectedSlot) && OwnsGear(Hotbar[SelectedSlot]) ? Hotbar[SelectedSlot] : 0; }
void ABHPlayer::SelectSlot(int32 S) {
 if(!Hotbar.IsValidIndex(S))return;
 CancelCast(); SelectedSlot=S; KnifeEquipped=false; HeldTool->SetVisibility(false);
 const int32 G=ActiveGear();
 if(G==1)EquipRod(); else if(G==2)EquipKnife();
 // Containers use their inventory panel until their approved held assets are produced.
}
void ABHPlayer::SlotOne() { SelectSlot(0); } void ABHPlayer::SlotTwo() { SelectSlot(1); }
void ABHPlayer::SlotThree() { SelectSlot(2); } void ABHPlayer::SlotFour() { SelectSlot(3); }
void ABHPlayer::ScrollNext() { SelectSlot((SelectedSlot+1)%4); }
void ABHPlayer::ScrollPrevious() { SelectSlot((SelectedSlot+3)%4); }
void ABHPlayer::AssignToSlot(int32 Gear,int32 Slot) {
 if(!OwnsGear(Gear) || !Hotbar.IsValidIndex(Slot))return;
 const int32 Old=Hotbar.Find(Gear); if(Old!=INDEX_NONE)Hotbar.Swap(Old,Slot); else Hotbar[Slot]=Gear;
 SelectSlot(Slot); AssignGear=0;
}
void ABHPlayer::ToggleInventory() {
 InventoryOpen=!InventoryOpen; AssignGear=0; CancelCast();
 if(auto* PC=Cast<APlayerController>(GetController())) {
  PC->bShowMouseCursor=InventoryOpen;
  if(InventoryOpen) { FInputModeGameAndUI Mode; Mode.SetHideCursorDuringCapture(false); Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); PC->SetInputMode(Mode); }
  else { PC->SetInputMode(FInputModeGameOnly()); SaveCheckpoint(); }
 }
}
TArray<ABHPhysicalItem*> ABHPlayer::StoredItems(int32 Container) const {
 TArray<ABHPhysicalItem*> Result;
 for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It)if(It->StoredIn && (!Container || It->StoredIn==Container))Result.Add(*It);
 return Result;
}
bool ABHPlayer::StoreIn(int32 C) {
 if((C!=3 && C!=4) || !OwnsGear(C)) { Say(TEXT("Buy a satchel or bucket from Mira's outdoor display.")); return false; }
 if(!HeldItem) { Say(TEXT("Carry an item first [E], then store it.")); return false; }
 if(HeldItem->Health>0 || (C==4 && HeldItem->Kind!=EBHItem::Fish)) { Say(TEXT("Finish fish before storing. Quest objects need the satchel.")); return false; }
 if(StoredItems(C).Num()>=(C==3?4:6)) { Say(TEXT("Container full. Open Tab to retrieve an item.")); return false; }
 auto* Item=HeldItem; HeldItem=nullptr; Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
 Item->StoredIn=C; Item->Mesh->SetSimulatePhysics(false); Item->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); Item->SetActorHiddenInGame(true);
 Say(TEXT("Stored. Open Tab and click a stored item to retrieve it.")); return true;
}
void ABHPlayer::StoreHeld() { if(StoreIn(ActiveGear()))SaveCheckpoint(); }
void ABHPlayer::RetrieveStored(int32 Index) {
 if(HeldItem) { Say(TEXT("Your hands are full. Drop or store the carried item first.")); return; }
 auto Items=StoredItems(); if(!Items.IsValidIndex(Index))return;
 auto* Item=Items[Index]; Item->StoredIn=0; Item->SetActorHiddenInGame(false); Item->Use(this); SaveCheckpoint();
}

// HUD and click hitboxes share this geometry, in viewport pixels.
static FVector2D PanelOrigin(int32 W,int32 H) { return FVector2D(FMath::Max(10,(W-760)/2),FMath::Max(110,(H-540)/2)); }
void ABHPlayer::InventoryClick() {
 auto* PC=Cast<APlayerController>(GetController()); if(!PC)return;
 int32 W,H; PC->GetViewportSize(W,H); float MX,MY; if(!PC->GetMousePosition(MX,MY))return;
 auto O=PanelOrigin(W,H); const float X=MX-O.X,Y=MY-O.Y;
 if(Y>=70 && Y<122 && X>=20 && X<740) { int32 G=FMath::FloorToInt((X-20)/180)+1; if(OwnsGear(G))AssignGear=G; return; }
 if(Y>=158 && Y<216 && X>=20 && X<740) { int32 S=FMath::FloorToInt((X-20)/180); if(AssignGear)AssignToSlot(AssignGear,S); else SelectSlot(S); return; }
 if(Y>=250 && Y<292) { if(X>=20 && X<350) { if(StoreIn(3))SaveCheckpoint(); } else if(X>=390 && X<720) { if(StoreIn(4))SaveCheckpoint(); } return; }
 if(Y>=316 && Y<496 && X>=20 && X<740) { int32 Index=FMath::FloorToInt((Y-316)/36)*2+(X>=380?1:0); RetrieveStored(Index); }
}
void ABHPlayer::DrawInventory(ABHHUD* UI) {
 auto* PC=Cast<APlayerController>(GetController()); if(!PC)return; int32 W,H; PC->GetViewportSize(W,H);
 const FLinearColor Gold(.9f,.72f,.38f),Ink(.025f,.065f,.075f,.96f),Muted(.48f,.53f,.53f);
 for(int32 I=0;I<4;++I) {
  const float X=W*.5f-230+I*116; UI->DrawRect(I==SelectedSlot?Gold:Ink,X,H-115,110,54);
  UI->DrawRect(Ink,X+2,H-113,106,50);
  UI->DrawText(FString::Printf(TEXT("%d  %s"),I+1,OwnsGear(Hotbar[I])?*GearName(Hotbar[I]):TEXT("Empty")),I==SelectedSlot?Gold:Muted,X+7,H-99,nullptr,.85f);
 }
 if(!InventoryOpen)return;
 UI->DrawRect(FLinearColor(0,0,0,.65f),0,0,W,H);
 auto O=PanelOrigin(W,H); const float X=O.X,Y=O.Y; UI->DrawRect(Ink,X,Y,760,540);
 UI->DrawText(TEXT("YOUR KIT"),Gold,X+20,Y+16,nullptr,1.4f);
 UI->DrawText(TEXT("Click owned gear, then a slot to move it. Tab closes. World stays active."),FLinearColor::White,X+20,Y+45);
 for(int32 I=0;I<4;++I) {
  UI->DrawRect(AssignGear==I+1?FLinearColor(.25f,.27f,.18f):FLinearColor(.08f,.14f,.15f),X+20+I*180,Y+70,172,52);
  UI->DrawText(OwnsGear(I+1)?GearName(I+1):GearName(I+1)+TEXT(" (not owned)"),OwnsGear(I+1)?Gold:Muted,X+26+I*180,Y+86,nullptr,.9f);
  UI->DrawRect(SelectedSlot==I?FLinearColor(.25f,.27f,.18f):FLinearColor(.08f,.14f,.15f),X+20+I*180,Y+158,172,58);
  UI->DrawText(FString::Printf(TEXT("%d: %s"),I+1,OwnsGear(Hotbar[I])?*GearName(Hotbar[I]):TEXT("Empty")),Gold,X+26+I*180,Y+178);
 }
 UI->DrawText(TEXT("HOTBAR  /  Scroll or keys 1-4 to select"),Muted,X+20,Y+134);
 UI->DrawText(FString::Printf(TEXT("Carrying: %s"),HeldItem?(HeldItem->Kind==EBHItem::Bellheart?TEXT("Bellheart"):*HeldItem->Label):TEXT("nothing")),FLinearColor::White,X+20,Y+226);
 for(int32 C=3;C<=4;++C) {
  const float CX=X+20+(C-3)*370; UI->DrawRect(FLinearColor(.08f,.14f,.15f),CX,Y+250,330,42);
  UI->DrawText(FString::Printf(TEXT("%s %d/%d  - click to store"),*GearName(C),StoredItems(C).Num(),OwnsGear(C)?(C==3?4:6):0),OwnsGear(C)?Gold:Muted,CX+8,Y+263);
 }
 const auto Items=StoredItems();
 for(int32 I=0;I<Items.Num();++I) {
  float IX=X+20+(I%2)*360,IY=Y+316+(I/2)*36;
  UI->DrawRect(FLinearColor(.08f,.14f,.15f),IX,IY,350,32);
  UI->DrawText(FString::Printf(TEXT("%s: %s  [retrieve]"),Items[I]->StoredIn==3?TEXT("Bag"):TEXT("Bucket"),Items[I]->Kind==EBHItem::Bellheart?TEXT("Bellheart"):*Items[I]->Label),FLinearColor::White,IX+6,IY+9,nullptr,.9f);
 }
 UI->DrawText(MessageTime>0?Message:TEXT("Buy containers at Mira's outdoor display. Bag 40 / bucket 28 Crowns."),Gold,X+20,Y+512,nullptr,.8f);
}
