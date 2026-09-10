#include "Bellheart.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

bool UBHProgress::Spend(int32 Amount) {
 if (Amount < 0 || Crowns < Amount) return false;
 Crowns -= Amount; return true;
}
void UBHProgress::Event(FName Name) {
 static const TArray<FName> Events = {"MeetOrin", "GetRod", "CatchFish", "SellFish", "BuyKnife", "InspectTower", "GetBait", "DefeatCrab", "Restore"};
 if (Events.IsValidIndex(Quest) && Events[Quest] == Name) {
  ++Quest; UE_LOG(LogTemp, Display, TEXT("BH_QUEST advanced=%d event=%s"), Quest, *Name.ToString());
 }
}
FString UBHProgress::Objective() const {
 static const TArray<FString> Text = {
  "STRANDED: ask Elder Orin why the sky route is closed",
  "EARN YOUR PASSAGE: collect Mira's loan rod",
  "Fish at the lower pond bank. Reel a fish ashore",
  "Kill the landed fish, carry it to Mira's sell counter",
  "Buy an Iron Knife from Bram (24 Crowns)",
  "Inspect the empty Bellheart socket at the tower",
  "Ask Mira for Bellcrab bait",
  "Use the bait at the arena lure; defeat Bellcrab",
  "Carry Bellheart to the tower and restore the wind bell",
  "FIRST BEACON RESTORED: meet Tavi at the workshop"
 }; return Text[FMath::Clamp(Quest, 0, Text.Num()-1)];
}

ABHInteractable::ABHInteractable() {
 Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body")); RootComponent = Mesh;
 Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")));
 Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}
FString ABHInteractable::Prompt(ABHPlayer* P) const {
 return FString::Printf(TEXT("[E] %s%s"), *Label, Price > 0 ? *FString::Printf(TEXT(" - %d Crowns"), Price) : TEXT(""));
}
void ABHInteractable::Use(ABHPlayer* P) {
 UBHProgress* S = P->Progress;
 if(Action=="Bag" || Action=="Bucket") {
  bool& Owned=Action=="Bag" ? P->HasBag : P->HasBucket;
  if(Owned) { P->Say(TEXT("You already own this container.")); return; }
  if(!S->Spend(Price)) { P->Say(TEXT("Not enough Crowns. Sell catches to earn more.")); return; }
  Owned=true; P->Say(TEXT("Container purchased. Scroll to it, then F stores your carried item. Tab opens storage.")); return;
 }
 if (Action == "Orin") {
  S->Event("MeetOrin");
  P->Say(S->Quest >= 9 ? TEXT("You brought our wind back. Bellheart can trade again. Find Tavi: four more silent bells lie beyond the Cloudsea.") : S->Quest < 6 ? TEXT("Five wind bells once guided skiffs across the Cloudsea. Ours fell silent when something tore out its heart. You are stranded here. Mira can lend you a rod: sell pond fish for tools, then inspect the tower.") : TEXT("Those bronze marks belong to Bellcrab. It hoards ringing metal beneath the pond. Recover our Bellheart and the first safe sky route will return."));
 } else if (Action == "Mira") {
  if (S->Quest == 1) { S->HasRod = true; S->Event("GetRod"); P->EquipRod(); }
  if (S->Quest == 6) { S->HasBait = true; S->Event("GetBait"); }
  P->Say(S->HasBait ? TEXT("Bellcrab cannot resist this bell-scented bait. Take it to the east arena lure. Bring a knife, and leave room to dodge!") : TEXT("The village needs food while trade is cut off. Here is a loan rod. Fish the island pond, finish your catch and carry it to my sell counter. Click at BITE; hold to reel, release to ease tension."));
 } else if (Action == "Bram") {
  P->Say(TEXT("Your catches pay for steel. An Iron Knife costs 24 Crowns; sharpening costs 12. Bellcrab guards the stolen heart. Dodge its glowing slam, then strike while it recovers."));
 } else if (Action == "Tavi") {
  P->Say(S->Quest >= 9 ? TEXT("One bell awake, four still silent. The first current points toward Mosshollow. I will ready the skiff here; that voyage is not available in this prototype.") : TEXT("Skiffs ride the currents above the Cloudsea, not pond water. Without our bell, the route is blind. Your rescue tether returns you here if you fall; your gear and Crowns stay with you."));
 } else if (Action == "Sell") {
  ABHPhysicalItem* I = P->HeldItem;
  if (!I || I->Kind != EBHItem::Fish || I->Health > 0) { P->Say(TEXT("Carry a dead fish here to sell it. [E] picks it up; [G] drops it.")); return; }
  int32 Earned = I->Value; S->Crowns += Earned; ++S->FishSold; S->Event("SellFish");
  P->HeldItem = nullptr; I->Destroy(); P->Say(FString::Printf(TEXT("Sold fish for %d Crowns."), Earned));
 } else if (Action == "Knife") {
  if (S->HasKnife) { P->Say(TEXT("You already own the Iron Knife.")); return; }
  if (!S->Spend(Price)) { P->Say(TEXT("Not enough Crowns. Sell another fish.")); return; }
  S->HasKnife = true; S->Event("BuyKnife"); P->EquipKnife(); P->Say(TEXT("Iron Knife equipped. Scroll to switch tools; Tab arranges your slots. Left click to strike."));
 } else if (Action == "Sharpen") {
  if (!S->HasKnife) { P->Say(TEXT("Buy the Iron Knife first.")); return; }
  if (S->KnifeDamage >= 30) { P->Say(TEXT("Knife is fully sharpened for this slice.")); return; }
  if (!S->Spend(Price)) { P->Say(TEXT("Sharpening costs 12 Crowns.")); return; }
  S->KnifeDamage = 30; P->Say(TEXT("Sharpened: Iron Knife damage 18 -> 30."));
 } else if (Action == "Tower") {
  if (S->Quest == 5) { S->Event("InspectTower"); P->Say(TEXT("The Bellheart socket is empty. Bronze claw marks lead toward the pond. Ask Mira for special bait.")); }
  else if (S->Quest == 8 && P->HeldItem && P->HeldItem->Kind == EBHItem::Bellheart) {
   P->HeldItem->Destroy(); P->HeldItem = nullptr; S->Event("Restore");
   for (TActorIterator<ABHWorld> It(GetWorld()); It; ++It) It->Restore();
   P->Say(TEXT("BELLHEART RESTORED. The bell rings. The cloud routes are open. Return to Tavi."));
  } else P->Say(TEXT("An empty bronze socket. The wind bell needs its Bellheart."));
 } else if (Action == "Lure") {
  if (S->Quest != 7 || !S->HasBait) { P->Say(TEXT("Mira's Bellcrab bait is needed. Follow the current quest.")); return; }
  for (TActorIterator<ABHBellcrab> It(GetWorld()); It; ++It) { It->Active = true; It->SetActorHiddenInGame(false); It->SetActorEnableCollision(true); }
  P->Say(TEXT("BELLCRAB EMERGES! Dodge the glowing slam. Strike between attacks. Select your knife from the hotbar."));
 }
}

ABHPhysicalItem::ABHPhysicalItem() {
 PrimaryActorTick.bCanEverTick = true; Label = TEXT("Pick up fish");
 Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));
 SetActorScale3D(FVector(.35,.13,.18));
}
void ABHPhysicalItem::Tick(float D) {
 Super::Tick(D); if(StoredIn)return; Age += D;
 if(Kind==EBHItem::Bellheart && GetActorLocation().Z < -2000) { Mesh->SetSimulatePhysics(false); SetActorLocation(FVector(0,-8200,150)); Mesh->SetSimulatePhysics(true); }
 if (Kind != EBHItem::Fish) return;
 ABHPlayer* P = Cast<ABHPlayer>(UGameplayStatics::GetPlayerPawn(this,0));
 if (!P) return;
 if (State == EBHFishState::Landed) {
  if (Health > 0 && Mesh->IsSimulatingPhysics() && FMath::Fmod(Age,1.3f) < D) Mesh->AddImpulse(FVector(15*FMath::Sin(Age),10,35),NAME_None,true);
  return;
 }
 if (State == EBHFishState::Dead || State == EBHFishState::Hooked) return;
 if (P->Bobber && P->BobberInWater && !P->HookedFish && FVector::Dist2D(Home,P->BobberGoal) < 1800) {
  if (State != EBHFishState::Bite) {
   State = EBHFishState::Investigate;
   FVector Goal = P->BobberGoal - FVector(0,0,20);
   FVector Next = FMath::VInterpConstantTo(GetActorLocation(),Goal,D,180);
   SetActorRotation((Goal-GetActorLocation()).Rotation()); SetActorLocation(Next);
   if (FVector::Dist(Next,Goal) < 45) { State = EBHFishState::Bite; BiteTime = 2.5f; P->Say(TEXT("BITE! Click now to hook the fish.")); }
  } else { BiteTime -= D; if (BiteTime <= 0) { P->CancelCast(); State = EBHFishState::Wander; P->Say(TEXT("The fish escaped the hook. Cast again.")); } }
 } else {
  State = EBHFishState::Wander;
  FVector Goal = Home + FVector(140*FMath::Sin(Age*.4f),110*FMath::Cos(Age*.3f),0);
  SetActorRotation((Goal-GetActorLocation()).Rotation()); SetActorLocation(FMath::VInterpTo(GetActorLocation(),Goal,D,1));
 }
}
void ABHPhysicalItem::Land(FVector Position) {
 State = EBHFishState::Landed; SetActorLocation(Position); Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); Mesh->SetSimulatePhysics(true);
 Mesh->SetMassOverrideInKg(NAME_None,2,true);
}
void ABHPhysicalItem::Hurt(float D) {
 if (State != EBHFishState::Landed || Kind != EBHItem::Fish) return;
 Health = FMath::Max(0.f,Health-D); if (Health <= 0) State = EBHFishState::Dead;
}
void ABHPhysicalItem::Use(ABHPlayer* P) {
 if(StoredIn)return;
 if (Kind == EBHItem::Fish && State != EBHFishState::Dead && State != EBHFishState::Landed) return;
 if (P->HeldItem) { P->Say(TEXT("Drop the carried item first [G].")); return; }
 Mesh->SetSimulatePhysics(false); Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 P->HeldItem = this; AttachToComponent(P->Camera,FAttachmentTransformRules::KeepWorldTransform);
 SetActorRelativeLocation(FVector(100,15,-28)); SetActorRelativeRotation(FRotator(0,90,0));
}
FString ABHPhysicalItem::Prompt(ABHPlayer* P) const {
 if (Kind == EBHItem::Bellheart) return TEXT("[E] Carry Bellheart to the tower");
 if (State == EBHFishState::Dead) return FString::Printf(TEXT("[E] Carry fish - %d Crowns"),Value);
 if (State == EBHFishState::Landed) return TEXT("[LMB] Finish fish / [E] Carry / [G] Drop");
 return TEXT("");
}

ABHPlayer::ABHPlayer() {
 PrimaryActorTick.bCanEverTick = true;
 GetCapsuleComponent()->InitCapsuleSize(34,90);
 Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
 Camera->SetupAttachment(GetCapsuleComponent()); Camera->SetRelativeLocation(FVector(0,0,78)); Camera->bUsePawnControlRotation = true;
 HeldTool = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldTool")); HeldTool->SetupAttachment(Camera);
 HeldTool->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cylinder.Cylinder")));
 HeldTool->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 Progress = CreateDefaultSubobject<UBHProgress>(TEXT("Progress"));
 GetCharacterMovement()->MaxWalkSpeed = 500; GetCharacterMovement()->JumpZVelocity = 500;
 GetCharacterMovement()->MaxStepHeight = 35;
 // Responsive steering without lateral drag or airborne braking erasing takeoff momentum.
 GetCharacterMovement()->AirControl = .62f;
 GetCharacterMovement()->AirControlBoostMultiplier = 1.6f;
 GetCharacterMovement()->AirControlBoostVelocityThreshold = 250.f;
 GetCharacterMovement()->FallingLateralFriction = 0.f;
 GetCharacterMovement()->BrakingDecelerationFalling = 0.f;
}
void ABHPlayer::BeginPlay() { Super::BeginPlay(); EquipRod(); Say(TEXT("BELLHEART ISLE - WASD move, E interact, F5 save, F9 load, R cancel cast.")); GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this,[this]() { if(!FParse::Param(FCommandLine::Get(),TEXT("BHSaveTest")) && !FParse::Param(FCommandLine::Get(),TEXT("BHFresh"))) LoadCheckpoint(); })); }
void ABHPlayer::SetupPlayerInputComponent(UInputComponent* I) {
 Super::SetupPlayerInputComponent(I);
 I->BindAxis("Forward",this,&ABHPlayer::Forward); I->BindAxis("Right",this,&ABHPlayer::Right);
 I->BindAxis("Turn",this,&ABHPlayer::Turn); I->BindAxis("Look",this,&ABHPlayer::Look);
 I->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump); I->BindAction("Jump",IE_Released,this,&ACharacter::StopJumping);
 I->BindAction("Interact",IE_Pressed,this,&ABHPlayer::Interact);
 I->BindAction("Primary",IE_Pressed,this,&ABHPlayer::PrimaryDown); I->BindAction("Primary",IE_Released,this,&ABHPlayer::PrimaryUp);
 I->BindAction("Drop",IE_Pressed,this,&ABHPlayer::Drop);
 I->BindAction("Rod",IE_Pressed,this,&ABHPlayer::SlotOne); I->BindAction("Knife",IE_Pressed,this,&ABHPlayer::SlotTwo);
 I->BindKey(EKeys::Three,IE_Pressed,this,&ABHPlayer::SlotThree); I->BindKey(EKeys::Four,IE_Pressed,this,&ABHPlayer::SlotFour);
 I->BindKey(EKeys::MouseScrollUp,IE_Pressed,this,&ABHPlayer::ScrollPrevious); I->BindKey(EKeys::MouseScrollDown,IE_Pressed,this,&ABHPlayer::ScrollNext);
 I->BindKey(EKeys::Tab,IE_Pressed,this,&ABHPlayer::ToggleInventory);
 I->BindKey(EKeys::F,IE_Pressed,this,&ABHPlayer::StoreHeld);
 I->BindAction("Sprint",IE_Pressed,this,&ABHPlayer::Sprint); I->BindAction("Sprint",IE_Released,this,&ABHPlayer::Walk);
 I->BindKey(EKeys::F5,IE_Pressed,this,&ABHPlayer::QuickSave);
 I->BindKey(EKeys::F9,IE_Pressed,this,&ABHPlayer::QuickLoad);
 I->BindKey(EKeys::R,IE_Pressed,this,&ABHPlayer::CancelCast);
}
void ABHPlayer::Forward(float V) { if(InventoryOpen)return; AddMovementInput(FRotationMatrix(FRotator(0,GetControlRotation().Yaw,0)).GetUnitAxis(EAxis::X),V); }
void ABHPlayer::Right(float V) { if(InventoryOpen)return; AddMovementInput(FRotationMatrix(FRotator(0,GetControlRotation().Yaw,0)).GetUnitAxis(EAxis::Y),V); }
void ABHPlayer::Turn(float V) { if(InventoryOpen)return; AddControllerYawInput(V); } void ABHPlayer::Look(float V) { if(InventoryOpen)return; AddControllerPitchInput(V); }
void ABHPlayer::Sprint() { SprintRequested=true; GetCharacterMovement()->MaxWalkSpeed = 800; }
void ABHPlayer::Walk() { SprintRequested=false; if(!GetCharacterMovement()->IsFalling())GetCharacterMovement()->MaxWalkSpeed = 500; }
FHitResult ABHPlayer::Trace(float Range) const {
 FHitResult H; FCollisionQueryParams Q; Q.AddIgnoredActor(this); if (HeldItem) Q.AddIgnoredActor(HeldItem);
 GetWorld()->LineTraceSingleByChannel(H,Camera->GetComponentLocation(),Camera->GetComponentLocation()+Camera->GetForwardVector()*Range,ECC_Visibility,Q); return H;
}
void ABHPlayer::Say(const FString& T) { DialogueSpeaker=nullptr; SpeakerName.Empty(); Message=T; MessageTime=16; UE_LOG(LogTemp,Display,TEXT("BH_MESSAGE %s"),*T); }
void ABHPlayer::Interact() { if(InventoryOpen)return; Target=Cast<ABHInteractable>(Trace(350).GetActor()); if(Target) { Target->Use(this); if(Target && (Target->Action=="Orin" || Target->Action=="Mira" || Target->Action=="Bram" || Target->Action=="Tavi")) { DialogueSpeaker=Target; SpeakerName=Target->Action.ToString(); } SaveCheckpoint(); } else UE_LOG(LogTemp,Display,TEXT("BH_INTERACT no target hit=%s camera=%s direction=%s"),*GetNameSafe(Trace(350).GetActor()),*Camera->GetComponentLocation().ToString(),*Camera->GetForwardVector().ToString()); }
void ABHPlayer::EquipRod() { int32 Index=Hotbar.Find(1); if(Index!=INDEX_NONE)SelectedSlot=Index; KnifeEquipped=false; HeldTool->SetVisibility(Progress->HasRod); HeldTool->SetRelativeLocation(FVector(75,28,-35)); HeldTool->SetRelativeRotation(FRotator(35,0,0)); HeldTool->SetRelativeScale3D(FVector(.025,.025,1.9)); }
void ABHPlayer::EquipKnife() { if(!Progress->HasKnife) { Say(TEXT("Buy an Iron Knife from Bram. Bare hands can finish your first fish.")); return; } CancelCast(); int32 Index=Hotbar.Find(2); if(Index!=INDEX_NONE)SelectedSlot=Index; KnifeEquipped=true; HeldTool->SetVisibility(true); HeldTool->SetRelativeLocation(FVector(40,23,-25)); HeldTool->SetRelativeRotation(FRotator(55,0,0)); HeldTool->SetRelativeScale3D(FVector(.04,.08,.28)); }
void ABHPlayer::Drop() { if(!HeldItem)return; ABHPhysicalItem* I=HeldItem; HeldItem=nullptr; I->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform); I->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); I->Mesh->SetSimulatePhysics(true); I->Mesh->AddImpulse(Camera->GetForwardVector()*180,NAME_None,true); }
void ABHPlayer::PrimaryDown() {
 if(InventoryOpen) { InventoryClick(); return; }
 if(ActiveGear()==3 || ActiveGear()==4) { StoreHeld(); return; }
 if(AttackCooldown>0)return;
 ABHPhysicalItem* Fish=Cast<ABHPhysicalItem>(Trace(260).GetActor());
 if(Fish && Fish->State==EBHFishState::Landed) { Fish->Hurt(KnifeEquipped ? Progress->KnifeDamage : 8); AttackCooldown=.4; Say(Fish->Health<=0 ? TEXT("Fish ready to sell. [E] pick up.") : TEXT("Fish struck. Strike again to finish it.")); return; }
 if(KnifeEquipped) { if(auto* C=Cast<ABHBellcrab>(Trace(380).GetActor())) C->Hurt(Progress->KnifeDamage); AttackCooldown=.4; return; }
 if(Bobber) {
  if(!HookedFish) for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It) if(It->State==EBHFishState::Bite) { HookedFish=*It; It->State=EBHFishState::Hooked; Say(TEXT("HOOKED! Hold to reel. Release when tension is high.")); break; }
  Reeling=true; return;
 }
 if(!Progress->HasRod || ActiveGear()!=1) { Say(TEXT("Select your fishing rod from the hotbar first.")); return; }
 if(HeldItem) { Say(TEXT("Drop the carried item before casting.")); return; }
 Charging=true; CastCharge=0;
}
void ABHPlayer::PrimaryUp() { Reeling=false; if(Charging) { Charging=false; CastLine(CastCharge); } }
void ABHPlayer::CastLine(float Charge) {
 FVector Start=Camera->GetComponentLocation(); FVector Dir=Camera->GetForwardVector();
 FVector Goal=Start+Dir*(700+FMath::Clamp(Charge,0.f,1.5f)*550);
 Goal.Z=600;
 // Pond is the measured ellipse from the approved layout; don't fish through buildings.
 float Ellipse=FMath::Square(Goal.X/2300.f)+FMath::Square((Goal.Y+2000)/1700.f);
 if(Ellipse>.95 || FVector::Dist2D(Start,Goal)>1800) { Say(TEXT("Aim into the central pond from its bank. Hold briefly, then release to cast.")); return; }
 FActorSpawnParameters Params;
 Bobber=GetWorld()->SpawnActor<AActor>(AActor::StaticClass(),Start,FRotator::ZeroRotator,Params);
 UStaticMeshComponent* B=NewObject<UStaticMeshComponent>(Bobber); Bobber->SetRootComponent(B); B->RegisterComponent();
 B->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere"))); B->SetWorldScale3D(FVector(.12)); B->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 BobberGoal=Goal; BobberInWater=false; Tension=0; Say(TEXT("Cast! Watch the bobber and wait for BITE."));
}
void ABHPlayer::CancelCast() { Charging=false; CastCharge=0; if(Bobber)Bobber->Destroy(); Bobber=nullptr; BobberInWater=false; if(HookedFish && HookedFish->State==EBHFishState::Hooked)HookedFish->State=EBHFishState::Wander; HookedFish=nullptr; Reeling=false; Tension=0; }
void ABHPlayer::Tick(float D) {
 Super::Tick(D); if(GetCharacterMovement()->IsMovingOnGround())GetCharacterMovement()->MaxWalkSpeed=SprintRequested?800.f:500.f; MessageTime-=D; AttackCooldown-=D; if(Charging)CastCharge+=D;
 Target=Cast<ABHInteractable>(Trace(350).GetActor());
 if(GetActorLocation().Z < -2500) RecoverAtDock();
 AutoSaveTime+=D; if(AutoSaveTime>=30) { AutoSaveTime=0; SaveCheckpoint(); }
 if(Bobber && FVector::Dist2D(GetActorLocation(),BobberGoal)>2200) { CancelCast(); Say(TEXT("Line retrieved: you moved too far from the pond.")); }
 if(Bobber) {
  if(!BobberInWater) { Bobber->SetActorLocation(FMath::VInterpConstantTo(Bobber->GetActorLocation(),BobberGoal,D,1100)); BobberInWater=FVector::Dist(Bobber->GetActorLocation(),BobberGoal)<10; }
  DrawDebugLine(GetWorld(),Camera->GetComponentLocation()+Camera->GetForwardVector()*90,Bobber->GetActorLocation(),FColor::White,false,0,0,1.5);
 }
 if(HookedFish) {
  Tension=FMath::Clamp(Tension+D*(Reeling ? .27f : -.5f),0.f,1.f);
  FVector Bank=GetActorLocation()+GetActorForwardVector()*110; Bank.Z=GetActorLocation().Z+20;
  if(Reeling) { FVector Pos=FMath::VInterpConstantTo(HookedFish->GetActorLocation(),Bank,D,230); HookedFish->SetActorLocation(Pos); if(Bobber)Bobber->SetActorLocation(Pos); }
  if(Tension>=1) { CancelCast(); Say(TEXT("The line snapped. The fish escaped. Ease tension by releasing the mouse.")); }
  else if(FVector::Dist2D(HookedFish->GetActorLocation(),Bank)<100) { ABHPhysicalItem* I=HookedFish; HookedFish=nullptr; CancelCast(); I->Land(Bank); Progress->Event("CatchFish"); SaveCheckpoint(); Say(TEXT("LANDED! Click the flopping fish to finish it, then [E] carry it to Mira.")); }
 }
}
void ABHPlayer::DamagePlayer(float D) { Health=FMath::Max(0.f,Health-D); if(Health<=0) RecoverAtDock(); }

ABHBellcrab::ABHBellcrab() {
 PrimaryActorTick.bCanEverTick=true; Label=TEXT("Bellcrab"); SetActorScale3D(FVector(3.5,3.5,1.5));
 for(int32 Side=0;Side<2;++Side) {
  auto* Claw=CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("Claw%d"),Side));
  Claw->SetupAttachment(Mesh); Claw->SetStaticMesh(Mesh->GetStaticMesh()); Claw->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  Claw->SetRelativeLocation(FVector(30,Side ? 55 : -55,10)); Claw->SetRelativeScale3D(FVector(.45,.32,.45));
  for(int32 Leg=0;Leg<3;++Leg) {
   auto* Limb=CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("Leg%d_%d"),Side,Leg));
   Limb->SetupAttachment(Mesh); Limb->SetStaticMesh(Mesh->GetStaticMesh()); Limb->SetCollisionEnabled(ECollisionEnabled::NoCollision);
   Limb->SetRelativeLocation(FVector(-30+Leg*30,Side ? 45 : -45,-35)); Limb->SetRelativeScale3D(FVector(.18,.6,.12));
  }
 }
}
void ABHBellcrab::Tick(float D) {
 Super::Tick(D); if(!Active || Health<=0)return;
 ABHPlayer* P=Cast<ABHPlayer>(UGameplayStatics::GetPlayerPawn(this,0)); if(!P)return;
 PhaseTime+=D; float Dist=FVector::Dist2D(GetActorLocation(),P->GetActorLocation());
 Telegraph=PhaseTime>2.5f;
 if(Telegraph) DrawDebugCircle(GetWorld(),GetActorLocation()-FVector(0,0,65),430,32,FColor::Red,false,0,0,8,FVector(1,0,0),FVector(0,1,0),false);
 if(PhaseTime>=3.7f) { if(Dist<430) P->DamagePlayer(22); PhaseTime=0; }
 if(!Telegraph && Dist>280 && FVector::Dist2D(P->GetActorLocation(),Arena)<1000) {
  FVector Goal=P->GetActorLocation(); Goal.Z=GetActorLocation().Z; SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(),Goal,D,130));
 }
}
void ABHBellcrab::Hurt(float D) {
 if(!Active || Health<=0)return; Health=FMath::Max(0.f,Health-D);
 UE_LOG(LogTemp,Display,TEXT("BH_CRAB health=%.0f"),Health);
 if(Health<=0) {
  ABHPhysicalItem* Heart=GetWorld()->SpawnActor<ABHPhysicalItem>(GetActorLocation()+FVector(0,0,100),FRotator::ZeroRotator);
  Heart->Kind=EBHItem::Bellheart; Heart->Health=0; Heart->State=EBHFishState::Dead; Heart->SetActorScale3D(FVector(.3,.3,.7)); Heart->Mesh->SetSimulatePhysics(true);
  if(auto* P=Cast<ABHPlayer>(UGameplayStatics::GetPlayerPawn(this,0))) { P->Progress->Event("DefeatCrab"); P->Say(TEXT("Bellcrab defeated. [E] carry the Bellheart back to the tower.")); }
  SetActorEnableCollision(false); SetActorHiddenInGame(true); Active=false;
  if(auto* P=Cast<ABHPlayer>(UGameplayStatics::GetPlayerPawn(this,0)))P->SaveCheckpoint();
 }
}
