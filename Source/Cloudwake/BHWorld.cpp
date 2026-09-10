#include "Bellheart.h"
#include "BHTestDriver.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "EngineUtils.h"

static const FLinearColor Stone(.52,.54,.53), PathColor(.65,.48,.22), Teal(.08,.35,.38), Wood(.32,.22,.13);
ABHWorld::ABHWorld() { PrimaryActorTick.bCanEverTick=false; }
void ABHWorld::BeginPlay() {
 Super::BeginPlay(); if(!Built) Build();
 Station("Bag",TEXT("Buy satchel - 4 items"),FVector(-27,25,14.48),40);
 Station("Bucket",TEXT("Buy fish bucket - 6 fish"),FVector(-29,25,14.48),28);
 // Non-colliding cloud-volume placeholders, below the playable island. Final volumetrics follow art review.
 for(int32 I=0;I<32;++I) {
  const float A=I*2.f*PI/32.f; const float Radius=180.f+(I%3)*45.f;
  AActor* Cloud=Box(FString::Printf(TEXT("Cloudsea placeholder %d"),I),FVector(FMath::Cos(A)*Radius,FMath::Sin(A)*Radius,-38-(I%3)*8),FVector(130,100,25),FLinearColor(.88f,.94f,1.f),false);
  if(auto* Mesh=Cast<AStaticMeshActor>(Cloud))Mesh->GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));
 }
}
AActor* ABHWorld::Box(const FString& Name,FVector P,FVector Size,FLinearColor Color,bool Collision) {
 AStaticMeshActor* A=GetWorld()->SpawnActor<AStaticMeshActor>(P*100,FRotator::ZeroRotator);
 A->Tags.Add(FName(*Name));
#if WITH_EDITOR
 A->SetActorLabel(Name);
#endif
 auto* M=A->GetStaticMeshComponent(); M->SetMobility(EComponentMobility::Movable);
 M->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));
 M->SetWorldScale3D(Size); M->SetCollisionEnabled(Collision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
 const TCHAR* MatName=Color.Equals(PathColor) ? TEXT("Path") : Color.Equals(Teal) ? TEXT("Teal") : Color.Equals(Wood) ? TEXT("Wood") : TEXT("Stone");
 if(Name.Contains(TEXT("water")) || Name.Contains(TEXT("Waterfall drop"))) MatName=TEXT("Water");
 if(Name.Contains(TEXT("canopy"))) MatName=TEXT("Tree");
 if(Name.Contains(TEXT("Bell placeholder"))) MatName=TEXT("Bronze");
 if(UMaterialInterface* Mat=LoadObject<UMaterialInterface>(nullptr,*FString::Printf(TEXT("/Game/Greybox/MI_%s.MI_%s"),MatName,MatName))) M->SetMaterial(0,Mat);
 return A;
}
void ABHWorld::Path(FVector A,FVector B,float Width) {
 FVector Delta=B-A; FVector Mid=(A+B)*.5-FVector(0,0,.18);
 AActor* Ramp=Box(TEXT("WalkablePath"),Mid,FVector(Delta.Size()+.6,Width,.36),PathColor);
 Ramp->SetActorRotation(Delta.Rotation());
}
void ABHWorld::Label(const FString& Text,FVector P) {
 AActor* A=GetWorld()->SpawnActor<AActor>(P*100,FRotator(0,-90,0));
 auto* T=NewObject<UTextRenderComponent>(A); A->SetRootComponent(T); T->RegisterComponent();
 T->SetWorldLocation(P*100); T->SetWorldRotation(FRotator(0,-90,0)); T->SetText(FText::FromString(Text)); T->SetWorldSize(45); T->SetHorizontalAlignment(EHTA_Center); T->SetTextRenderColor(FColor(255,231,177));
}
void ABHWorld::Building(const FString& Name,FVector P,FVector Size) {
 Box(Name+TEXT(" floor"),P-FVector(0,0,.2),FVector(Size.X,Size.Y,.4),Stone);
 Box(Name+TEXT(" rear"),P+FVector(0,Size.Y*.5,Size.Z*.5),FVector(Size.X,.3,Size.Z),Stone);
 for(int Sign:{-1,1})Box(Name+TEXT(" wall"),P+FVector(Sign*Size.X*.5,0,Size.Z*.5),FVector(.3,Size.Y,Size.Z),Stone);
 Box(Name+TEXT(" roof mass"),P+FVector(0,0,Size.Z),FVector(Size.X+.7,Size.Y+.7,.4),Teal);
 Label(Name,P+FVector(0,-Size.Y*.5,Size.Z-.7));
}
ABHInteractable* ABHWorld::Station(FName Action,const FString& Name,FVector P,int32 Price) {
 ABHInteractable* S=GetWorld()->SpawnActor<ABHInteractable>(P*100,FRotator::ZeroRotator);
 S->Action=Action; S->Label=Name; S->Price=Price;
#if WITH_EDITOR
 S->SetActorLabel(Name);
#endif
 const bool NPC=Action=="Mira" || Action=="Orin" || Action=="Bram" || Action=="Tavi";
 S->SetActorScale3D(NPC ? FVector(.6,.5,1.8) : FVector(.8,.65,.95));
 S->Mesh->SetMaterial(0,LoadObject<UMaterialInterface>(nullptr,NPC ? TEXT("/Game/Greybox/MI_Teal.MI_Teal") : TEXT("/Game/Greybox/MI_Wood.MI_Wood")));
 Label(Name,P+FVector(0,0,1.1)); return S;
}
void ABHWorld::Build() {
 if(Built)return;
 Built=true;
 Box(TEXT("Cloudsea horizon placeholder"),FVector(0,0,-60),FVector(5000,5000,.1),Stone,false);
 // Canonical meters from BH island top-down/flow/elevation; all environment forms are placeholders.
 Box(TEXT("Dock BH-01"),FVector(0,-86,-.2),FVector(9,15,.4),Wood);
 Box(TEXT("Skiff parking BH-15"),FVector(22,-83,-.2),FVector(10,12,.4),Wood);
 Path(FVector(0,-83,0),FVector(22,-83,0));
 Box(TEXT("Skiff hull placeholder"),FVector(23,-86,1),FVector(4,7,1.6),Teal);
 Box(TEXT("South bank"),FVector(0,-46,4),FVector(50,15,6),Stone);
 Box(TEXT("East bank"),FVector(30,-18,4),FVector(14,50,6),Stone);
 Box(TEXT("West bank"),FVector(-32,-18,4),FVector(18,50,6),Stone);
 Box(TEXT("Pond bed"),FVector(0,-20,4.8),FVector(46,34,.5),Stone);
 Box(TEXT("Pond water BH-03"),FVector(0,-20,6),FVector(46,34,.08),FLinearColor(.08,.6,.65),false);
 Box(TEXT("Village terrace"),FVector(0,31,12),FVector(67,23,4),Stone);
 Box(TEXT("Tree terrace"),FVector(-34,5,8),FVector(23,24,4),Stone);
 Box(TEXT("Workshop terrace"),FVector(46,10,8),FVector(17,19,4),Stone);
 Box(TEXT("Elder terrace"),FVector(-20,58,18),FVector(20,18,4),Stone);
 Box(TEXT("Tower hill"),FVector(0,76,23),FVector(22,20,6),Stone);
 Box(TEXT("Upper ruins"),FVector(0,92,27),FVector(16,10,6),Stone);
 Box(TEXT("Bellcrab arena BH-12"),FVector(26,-11,6.8),FVector(22,18,.4),FLinearColor(.4,.32,.28));
 Box(TEXT("Waterfall bank BH-11"),FVector(-47,-24,5.8),FVector(10,12,.4),Stone);
 Box(TEXT("Waterfall drop"),FVector(-52,-24,-6),FVector(.5,6,24),Teal,false);
 Box(TEXT("Island underside marker"),FVector(0,0,-23),FVector(95,130,44),FLinearColor(.22,.23,.25),false);
 const TArray<FVector> Main={ {0,-86,0},{0,-65,3},{0,-54,7},{0,-49,7},{17,-35,7},{27,-21,7},{29,5,7},{16,18,14},{12,22,14},{0,30,14},{-10,44,20},{-14,53,20},{-14,63,20},{-3,65,26},{0,65,26},{0,76,26} };
 for(int32 I=1;I<Main.Num();++I)Path(Main[I-1],Main[I]);
 Path(FVector(-14,53,20),FVector(-20,53,20));
 const TArray<FVector> Side={ {-10,-43,7},{-29,-36,7},{-47,-24,6},{-53,-2,9},{-51,17,11},{-38,5,10},{-38,17,10},{-24,18,14},{-24,24,14},{0,24,14},{27,24,14},{35,24,14},{38,3,10},{46,3,10} };
 for(int32 I=1;I<Side.Num();++I)Path(Side[I-1],Side[I],1.8);
 Path(FVector(0,-49,7),FVector(-10,-43,7)); Path(FVector(29,5,7),FVector(26,-11,7));
 Path(FVector(26,-11,7),FVector(27,-21,7)); Path(FVector(0,76,26),FVector(0,92,30));
 Building(TEXT("THE CLOUD CATCH / MIRA"),FVector(-24,30,14),FVector(9,7,6.2));
 Building(TEXT("BRAM'S FORGE"),FVector(27,30,14),FVector(11,8,6.8));
 Building(TEXT("BELLKEEPER'S HOUSE"),FVector(-20,58,20),FVector(8,7,6));
 Building(TEXT("TINKER'S WORKSHOP"),FVector(46,10,10),FVector(12,9,7));
 // Tower envelope: 8x7m, 32m high above hill. Open base allows socket access.
 for(int SX:{-1,1}) for(int SY:{-1,1}) Box(TEXT("Tower pillar"),FVector(SX*3.3,76+SY*2.8,42),FVector(1.2,1.2,32),Stone);
 Box(TEXT("Bell beam"),FVector(0,76,56),FVector(8,1,1),Wood);
 Box(TEXT("Bell placeholder"),FVector(0,76,53),FVector(3,3,3),FLinearColor(.65,.43,.15),false);
 Box(TEXT("Giant tree trunk envelope"),FVector(-34,5,16),FVector(2.8,2.8,12),Wood);
 Box(TEXT("Giant tree canopy envelope"),FVector(-34,5,24),FVector(20,20,6),FLinearColor(.23,.35,.18),false);
 Label(TEXT("BH-01 DOCK / ARRIVAL"),FVector(0,-80,3));
 Label(TEXT("BH-03 CENTRAL POND"),FVector(0,-19,8));
 Label(TEXT("BH-13 TUTORIAL FISHING"),FVector(-10,-41,9));
 Label(TEXT("BH-12 BELLCRAB ARENA"),FVector(28,-9,10));
 Label(TEXT("BH-10 BELL TOWER"),FVector(0,72,30));
 Station("Orin",TEXT("Elder Orin - Talk"),FVector(-20,56,20.95));
 Station("Mira",TEXT("Mira - Talk / Rod / Bait"),FVector(-25,28,14.95));
 Station("Sell",TEXT("Sell carried fish"),FVector(-22,26.8,14.48));
 Station("Bram",TEXT("Bram - Talk"),FVector(25,28,14.95));
 Station("Knife",TEXT("Buy Iron Knife"),FVector(28,27,14.48),24);
 Station("Sharpen",TEXT("Sharpen Knife"),FVector(30,27,14.48),12);
 Station("Tavi",TEXT("Tavi - Talk"),FVector(45,8,10.95));
 Station("Tower",TEXT("Inspect / Restore Bellheart"),FVector(0,75,26.95));
 Station("Lure",TEXT("Place Bellcrab bait"),FVector(32,-17,7.48));
 for(int32 I=0;I<12;++I) {
  FVector P(-1200+(I%4)*650,-3000+(I/4)*650,580);
  auto* F=GetWorld()->SpawnActor<ABHPhysicalItem>(P,FRotator::ZeroRotator); F->Home=P;
  F->Label=I%2 ? TEXT("Pebblecarp") : TEXT("Cloudfin"); F->Value=I%2 ? 36 : 32;
  F->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
 }
 Crab=GetWorld()->SpawnActor<ABHBellcrab>(FVector(2600,-1100,790),FRotator::ZeroRotator);
 Crab->SetActorHiddenInGame(true); Crab->SetActorEnableCollision(false);
 auto* Sun=GetWorld()->SpawnActor<ADirectionalLight>(FVector(0,0,10000),FRotator(-55,-30,0));
 Sun->GetLightComponent()->SetIntensity(4);
 Cast<UDirectionalLightComponent>(Sun->GetLightComponent())->bAtmosphereSunLight=true;
 GetWorld()->SpawnActor<ASkyAtmosphere>();
 auto* Sky=GetWorld()->SpawnActor<ASkyLight>(); Sky->GetLightComponent()->SetIntensity(1);
 GetWorld()->GetWorldSettings()->KillZ=-10000;
 UE_LOG(LogTemp,Display,TEXT("BH_WORLD_READY: 15 sites, 12 physical fish, core quest stations spawned"));
}
ABHGameMode::ABHGameMode() { DefaultPawnClass=ABHPlayer::StaticClass(); HUDClass=ABHHUD::StaticClass(); }
void ABHGameMode::BeginPlay() {
 Super::BeginPlay(); if(!UGameplayStatics::GetActorOfClass(this,ABHWorld::StaticClass())) GetWorld()->SpawnActor<ABHWorld>();
 if(APawn* P=UGameplayStatics::GetPlayerPawn(this,0)) { P->SetActorLocation(FVector(0,-8600,110)); if(P->GetController())P->GetController()->SetControlRotation(FRotator(0,90,0)); }
 if(FParse::Param(FCommandLine::Get(),TEXT("BHAirTest")) || FParse::Param(FCommandLine::Get(),TEXT("BHTest")) || (FParse::Param(FCommandLine::Get(),TEXT("BHWalk")) || FParse::Param(FCommandLine::Get(),TEXT("BHSideWalk"))) || FParse::Param(FCommandLine::Get(),TEXT("BHSaveTest"))) GetWorld()->SpawnActor<ABHTestDriver>();
}
void ABHWorld::Restore(bool IsRestored, bool PlayCue) {
 Restored=IsRestored;
 for(TActorIterator<AStaticMeshActor> It(GetWorld());It;++It) if(It->Tags.Contains(FName(TEXT("Bell placeholder")))) {
  It->GetStaticMeshComponent()->SetMaterial(0,LoadObject<UMaterialInterface>(nullptr,Restored ? TEXT("/Game/Greybox/MI_Restored.MI_Restored") : TEXT("/Game/Greybox/MI_Bronze.MI_Bronze")));
 }
 if(PlayCue && Restored) if(USoundBase* Sound=LoadObject<USoundBase>(nullptr,TEXT("/Game/Audio/S_Bell.S_Bell"))) UGameplayStatics::PlaySound2D(this,Sound);
}
void ABHHUD::DrawHUD() {
 Super::DrawHUD(); ABHPlayer* P=Cast<ABHPlayer>(GetOwningPawn()); if(!P || !Canvas)return;
 float W=Canvas->SizeX,H=Canvas->SizeY;
 DrawRect(FLinearColor(0,0,0,.65),20,20,FMath::Min(W-40,900.f),85);
 DrawText(TEXT("CLOUDWAKE / BELLHEART GREYBOX"),FLinearColor(.7,.9,.9),35,30,nullptr,1.25);
 DrawText(P->Progress->Objective(),FLinearColor::White,35,60,nullptr,1.05);
 DrawText(FString::Printf(TEXT("Health %.0f / 100     Crowns %d"),P->Health,P->Progress->Crowns),FLinearColor::White,30,H-70,nullptr,1.3);
 DrawText(TEXT("[E] Interact  [G] Drop  [F] Store  [Tab] Inventory"),FLinearColor::White,30,H-40,nullptr,1);
 DrawText(TEXT("F5 Save / F9 Load / R Retrieve line"),FLinearColor::White,W-320,H-40,nullptr,1);
 for(TActorIterator<ABHBellcrab> It(GetWorld());It;++It) if(It->Active) {
  DrawRect(FLinearColor(0,0,0,.7f),W*.5f-160,115,320,50);
  DrawText(It->Telegraph ? TEXT("BELLCRAB - DODGE!") : TEXT("BELLCRAB - STRIKE"),It->Telegraph ? FLinearColor::Red : FLinearColor::White,W*.5f-145,120);
  DrawRect(FLinearColor(.8f,.2f,.12f),W*.5f-145,146,290*It->Health/180,10);
 }
 DrawLine(W*.5f-6,H*.5f,W*.5f+6,H*.5f,FLinearColor::White); DrawLine(W*.5f,H*.5f-6,W*.5f,H*.5f+6,FLinearColor::White);
 if(P->Target) DrawText(P->Target->Prompt(P),FLinearColor(1,.85,.4),W*.5f-180,H*.5f+35,nullptr,1.2);
 if(P->MessageTime>0) {
  const bool Talking=IsValid(P->DialogueSpeaker);
  FVector2D Anchor=FVector2D::ZeroVector;
  bool Visible=!Talking;
  if(Talking && FVector::Dist(P->GetActorLocation(),P->DialogueSpeaker->GetActorLocation())<650)
   Visible=GetOwningPlayerController()->ProjectWorldLocationToScreen(P->DialogueSpeaker->GetActorLocation()+FVector(0,0,110),Anchor);
  if(Visible) {
   const float PanelW=FMath::Min(460.f,W-40.f);
   TArray<FString> Words,Lines; P->Message.ParseIntoArray(Words,TEXT(" "),true); FString Line;
   for(const FString& Word:Words) { FString Candidate=Line.IsEmpty()?Word:Line+TEXT(" ")+Word; float TW,TH; GetTextSize(Candidate,TW,TH,nullptr,1.f);
    if(TW>PanelW-32 && !Line.IsEmpty()) { Lines.Add(Line); Line=Word; } else Line=Candidate; }
   if(!Line.IsEmpty())Lines.Add(Line);
   const float PanelH=44+Lines.Num()*21;
   const float X=Talking?FMath::Clamp(Anchor.X-PanelW*.5f,20.f,W-PanelW-20):30.f;
   const float Y=Talking?FMath::Clamp(Anchor.Y-PanelH-18,115.f,FMath::Max(115.f,H-PanelH-100)):H-PanelH-100;
   DrawRect(FLinearColor(.035f,.09f,.10f,.94f),X,Y,PanelW,PanelH);
   DrawRect(FLinearColor(.75f,.58f,.28f),X,Y,PanelW,3);
   DrawText(Talking?P->SpeakerName:TEXT("JOURNAL"),FLinearColor(1,.82f,.48f),X+16,Y+10);
   for(int32 I=0;I<Lines.Num();++I)DrawText(Lines[I],FLinearColor::White,X+16,Y+35+I*21);
   if(Talking && Anchor.Y>Y+PanelH && Anchor.Y<H-90)DrawLine(X+PanelW*.5f,Y+PanelH,Anchor.X,Anchor.Y,FLinearColor(.75f,.58f,.28f));
  }
 }
 if(P->HookedFish) { DrawRect(FLinearColor(.1,.1,.1),W*.5f-150,H-240,300,20); DrawRect(FLinearColor(P->Tension,1-P->Tension,.15),W*.5f-150,H-240,300*P->Tension,20); DrawText(TEXT("LINE TENSION - release to ease"),FLinearColor::White,W*.5f-150,H-265); }
 P->DrawInventory(this);
 if(P->Charging && !P->InventoryOpen) DrawText(FString::Printf(TEXT("CAST %.0f%% - release"),FMath::Min(P->CastCharge/1.5f,1.f)*100),FLinearColor::White,W*.5f-100,H*.5f+75);
}
