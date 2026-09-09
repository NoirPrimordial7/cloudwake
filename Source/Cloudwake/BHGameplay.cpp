#include "Bellheart.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"

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
  "Find Elder Orin at the Bellkeeper's House",
  "Visit Mira: collect your first fishing rod",
  "Fish at the lower pond bank. Reel a fish ashore",
  "Kill the landed fish, carry it to Mira's sell counter",
  "Buy an Iron Knife from Bram (24 Crowns)",
  "Inspect the empty Bellheart socket at the tower",
  "Ask Mira for Bellcrab bait",
  "Use the bait at the arena lure; defeat Bellcrab",
  "Carry Bellheart to the tower and restore the wind bell",
  "Bellheart restored! Return to Tavi and the Cloud Skiff"
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
 if (Action == "Orin") {
  S->Event("MeetOrin");
  P->Say(S->Quest < 6 ? TEXT("ORIN: Our wind bell is silent. Mira will lend you a rod. Earn your tools, then inspect the tower.") : TEXT("ORIN: Bellcrab took the Bellheart. Lure it out, then bring the heart back to its socket."));
 } else if (Action == "Mira") {
  if (S->Quest == 1) { S->HasRod = true; S->Event("GetRod"); P->EquipRod(); }
  if (S->Quest == 6) { S->HasBait = true; S->Event("GetBait"); }
  P->Say(S->HasBait ? TEXT("MIRA: Your special bait is ready. Take it to the east arena lure.") : TEXT("MIRA: Cast from the lower bank. Click at BITE, hold to reel, release to ease tension. Land it, finish it, then sell at my counter."));
 } else if (Action == "Bram") {
  P->Say(TEXT("BRAM: The counter has an Iron Knife. Use the grindstone after buying it. A bright shell means Bellcrab is preparing a strike."));
 } else if (Action == "Tavi") {
  P->Say(S->Quest >= 9 ? TEXT("TAVI: The wind routes are open! Bellheart Isle slice complete. The next island is a future milestone.") : TEXT("TAVI: No wind, no voyage. Restore the bell and we can sail again."));
 } else if (Action == "Sell") {
  ABHPhysicalItem* I = P->HeldItem;
  if (!I || I->Kind != EBHItem::Fish || I->Health > 0) { P->Say(TEXT("Carry a dead fish here to sell it. [E] picks it up; [G] drops it.")); return; }
  int32 Earned = I->Value; S->Crowns += Earned; ++S->FishSold; S->Event("SellFish");
  P->HeldItem = nullptr; I->Destroy(); P->Say(FString::Printf(TEXT("Sold fish for %d Crowns."), Earned));
 } else if (Action == "Knife") {
  if (S->HasKnife) { P->Say(TEXT("You already own the Iron Knife.")); return; }
  if (!S->Spend(Price)) { P->Say(TEXT("Not enough Crowns. Sell another fish.")); return; }
  S->HasKnife = true; S->Event("BuyKnife"); P->EquipKnife(); P->Say(TEXT("Iron Knife equipped. [2] knife, [1] rod. Left click to strike."));
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
  P->Say(TEXT("BELLCRAB EMERGES! Dodge the glowing slam. Strike between attacks. [2] equips the knife."));
 }
}

ABHPhysicalItem::ABHPhysicalItem() {
 PrimaryActorTick.bCanEverTick = true; Label = TEXT("Pick up fish");
 Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));
 SetActorScale3D(FVector(.35,.13,.18));
}
void ABHPhysicalItem::Tick(float D) {
 Super::Tick(D); Age += D;
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
}
void ABHPlayer::BeginPlay() { Super::BeginPlay(); EquipRod(); Say(TEXT("BELLHEART ISLE - Greybox. WASD move, mouse look, E interact, Shift sprint, Space jump. Follow the gold paths.")); }
void ABHPlayer::SetupPlayerInputComponent(UInputComponent* I) {
 Super::SetupPlayerInputComponent(I);
 I->BindAxis("Forward",this,&ABHPlayer::Forward); I->BindAxis("Right",this,&ABHPlayer::Right);
 I->BindAxis("Turn",this,&ABHPlayer::Turn); I->BindAxis("Look",this,&ABHPlayer::Look);
 I->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump); I->BindAction("Jump",IE_Released,this,&ACharacter::StopJumping);
 I->BindAction("Interact",IE_Pressed,this,&ABHPlayer::Interact);
 I->BindAction("Primary",IE_Pressed,this,&ABHPlayer::PrimaryDown); I->BindAction("Primary",IE_Released,this,&ABHPlayer::PrimaryUp);
 I->BindAction("Drop",IE_Pressed,this,&ABHPlayer::Drop);
 I->BindAction("Rod",IE_Pressed,this,&ABHPlayer::EquipRod); I->BindAction("Knife",IE_Pressed,this,&ABHPlayer::EquipKnife);
 I->BindAction("Sprint",IE_Pressed,this,&ABHPlayer::Sprint); I->BindAction("Sprint",IE_Released,this,&ABHPlayer::Walk);
}
void ABHPlayer::Forward(float V) { AddMovementInput(FRotationMatrix(FRotator(0,GetControlRotation().Yaw,0)).GetUnitAxis(EAxis::X),V); }
void ABHPlayer::Right(float V) { AddMovementInput(FRotationMatrix(FRotator(0,GetControlRotation().Yaw,0)).GetUnitAxis(EAxis::Y),V); }
void ABHPlayer::Turn(float V) { AddControllerYawInput(V); } void ABHPlayer::Look(float V) { AddControllerPitchInput(V); }
void ABHPlayer::Sprint() { GetCharacterMovement()->MaxWalkSpeed = 800; } void ABHPlayer::Walk() { GetCharacterMovement()->MaxWalkSpeed = 500; }
FHitResult ABHPlayer::Trace(float Range) const {
 FHitResult H; FCollisionQueryParams Q; Q.AddIgnoredActor(this); if (HeldItem) Q.AddIgnoredActor(HeldItem);
 GetWorld()->LineTraceSingleByChannel(H,Camera->GetComponentLocation(),Camera->GetComponentLocation()+Camera->GetForwardVector()*Range,ECC_Visibility,Q); return H;
}
void ABHPlayer::Say(const FString& T) { Message=T; MessageTime=9; UE_LOG(LogTemp,Display,TEXT("BH_MESSAGE %s"),*T); }
void ABHPlayer::Interact() { Target=Cast<ABHInteractable>(Trace(350).GetActor()); if(Target) Target->Use(this); else UE_LOG(LogTemp,Display,TEXT("BH_INTERACT no target hit=%s camera=%s direction=%s"),*GetNameSafe(Trace(350).GetActor()),*Camera->GetComponentLocation().ToString(),*Camera->GetForwardVector().ToString()); }
void ABHPlayer::EquipRod() { KnifeEquipped=false; HeldTool->SetVisibility(Progress->HasRod); HeldTool->SetRelativeLocation(FVector(75,28,-35)); HeldTool->SetRelativeRotation(FRotator(35,0,0)); HeldTool->SetRelativeScale3D(FVector(.025,.025,1.9)); }
void ABHPlayer::EquipKnife() { if(!Progress->HasKnife) { Say(TEXT("Buy an Iron Knife from Bram. Bare hands can finish your first fish.")); return; } CancelCast(); KnifeEquipped=true; HeldTool->SetVisibility(true); HeldTool->SetRelativeLocation(FVector(40,23,-25)); HeldTool->SetRelativeRotation(FRotator(55,0,0)); HeldTool->SetRelativeScale3D(FVector(.04,.08,.28)); }
void ABHPlayer::Drop() { if(!HeldItem)return; ABHPhysicalItem* I=HeldItem; HeldItem=nullptr; I->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform); I->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); I->Mesh->SetSimulatePhysics(true); I->Mesh->AddImpulse(Camera->GetForwardVector()*180,NAME_None,true); }
void ABHPlayer::PrimaryDown() {
 if(AttackCooldown>0)return;
 ABHPhysicalItem* Fish=Cast<ABHPhysicalItem>(Trace(260).GetActor());
 if(Fish && Fish->State==EBHFishState::Landed) { Fish->Hurt(Progress->HasKnife ? Progress->KnifeDamage : 8); AttackCooldown=.4; Say(Fish->Health<=0 ? TEXT("Fish ready to sell. [E] pick up.") : TEXT("Fish struck. Strike again to finish it.")); return; }
 if(KnifeEquipped) { if(auto* C=Cast<ABHBellcrab>(Trace(380).GetActor())) C->Hurt(Progress->KnifeDamage); AttackCooldown=.4; return; }
 if(Bobber) {
  if(!HookedFish) for(TActorIterator<ABHPhysicalItem> It(GetWorld());It;++It) if(It->State==EBHFishState::Bite) { HookedFish=*It; It->State=EBHFishState::Hooked; Say(TEXT("HOOKED! Hold to reel. Release when tension is high.")); break; }
  Reeling=true; return;
 }
 if(!Progress->HasRod) { Say(TEXT("Talk to Orin, then collect a rod from Mira.")); return; }
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
void ABHPlayer::CancelCast() { if(Bobber)Bobber->Destroy(); Bobber=nullptr; BobberInWater=false; if(HookedFish && HookedFish->State==EBHFishState::Hooked)HookedFish->State=EBHFishState::Wander; HookedFish=nullptr; Reeling=false; Tension=0; }
void ABHPlayer::Tick(float D) {
 Super::Tick(D); MessageTime-=D; AttackCooldown-=D; if(Charging)CastCharge+=D;
 Target=Cast<ABHInteractable>(Trace(350).GetActor());
 if(GetActorLocation().Z < -2500) { SetActorLocation(FVector(0,-8600,110)); DamagePlayer(10); Say(TEXT("Recovered at the dock.")); }
 if(Bobber) {
  if(!BobberInWater) { Bobber->SetActorLocation(FMath::VInterpConstantTo(Bobber->GetActorLocation(),BobberGoal,D,1100)); BobberInWater=FVector::Dist(Bobber->GetActorLocation(),BobberGoal)<10; }
  DrawDebugLine(GetWorld(),Camera->GetComponentLocation()+Camera->GetForwardVector()*90,Bobber->GetActorLocation(),FColor::White,false,0,0,1.5);
 }
 if(HookedFish) {
  Tension=FMath::Clamp(Tension+D*(Reeling ? .27f : -.5f),0.f,1.f);
  FVector Bank=GetActorLocation()+GetActorForwardVector()*110; Bank.Z=GetActorLocation().Z+20;
  if(Reeling) { FVector Pos=FMath::VInterpConstantTo(HookedFish->GetActorLocation(),Bank,D,230); HookedFish->SetActorLocation(Pos); if(Bobber)Bobber->SetActorLocation(Pos); }
  if(Tension>=1) { CancelCast(); Say(TEXT("The line snapped. The fish escaped. Ease tension by releasing the mouse.")); }
  else if(FVector::Dist2D(HookedFish->GetActorLocation(),Bank)<100) { ABHPhysicalItem* I=HookedFish; HookedFish=nullptr; CancelCast(); I->Land(Bank); Progress->Event("CatchFish"); Say(TEXT("LANDED! Click the flopping fish to finish it, then [E] carry it to Mira.")); }
 }
}
void ABHPlayer::DamagePlayer(float D) { Health=FMath::Max(0.f,Health-D); if(Health<=0) { Drop(); CancelCast(); Health=100; SetActorLocation(FVector(0,-8600,110)); Say(TEXT("Recovered at the dock. Your quest and Crowns are retained.")); } }

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
 }
}
