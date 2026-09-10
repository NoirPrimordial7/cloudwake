#include "BHTestDriver.h"
#include "Bellheart.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

void ABHTestDriver::AirTest(float D) {
 auto* P=Cast<ABHPlayer>(UGameplayStatics::GetPlayerPawn(this,0)); if(!P)return;
 auto* M=P->GetCharacterMovement();
 static const TCHAR* Names[]={TEXT("W jump"),TEXT("Sprint W jump"),TEXT("WA jump"),TEXT("WD jump"),TEXT("Sprint jump steer A"),TEXT("Sprint jump steer D"),TEXT("Air A-to-D reversal"),TEXT("Sprint diagonal"),TEXT("Release sprint airborne then steer/land")};
 if(Total>35) { Check(false,TEXT("Air test timeout")); SetActorTickEnabled(false); return; }
 if(Step==0) {
  if(WalkPoint==0)for(TActorIterator<ABHWorld> It(GetWorld());It;++It) { It->Box(TEXT("Air test floor"),FVector(1000,0,-.5),FVector(100,100,1),FLinearColor::Gray); break; }
  P->SetActorLocation(FVector(100000,0,95)); M->StopMovementImmediately(); P->GetController()->SetControlRotation(FRotator::ZeroRotator);
  if(WalkPoint==1 || WalkPoint>=4)P->Sprint(); else P->Walk();
  AirPeak=0; AirReverse=0; Step=1; Time=0; return;
 }
 const float GroundSide=(WalkPoint==2 || WalkPoint==7)?-1.f:WalkPoint==3?1.f:0.f;
 if(Step==1) {
  P->Forward(1); P->Right(GroundSide);
  if(Time>.65f && M->IsMovingOnGround()) { AirStart=P->GetActorLocation(); AirTakeoff=M->Velocity.Size2D(); P->Jump(); Step=2; Time=0; }
  return;
 }
 P->StopJumping();
 float Side=GroundSide;
 if(WalkPoint==4)Side=-1; if(WalkPoint==5 || WalkPoint==8)Side=1;
 if(WalkPoint==6)Side=Time<.42f?-1.f:1.f;
 if(WalkPoint<4 || WalkPoint==7)P->Forward(1);
 P->Right(Side);
 if(WalkPoint==8)P->Walk(); // Shift release must not clamp horizontal speed during flight.
 AirPeak=FMath::Max(AirPeak,P->GetActorLocation().Z-AirStart.Z);
 if(WalkPoint==6 && Time<.42f)AirReverse=M->Velocity.Y;
 if(WalkPoint>=4 && Time>.15f && Time<.25f)if(!Check(M->Velocity.X>AirTakeoff*.65f,TEXT("Forward takeoff momentum retained during lateral steering"))) { SetActorTickEnabled(false); return; }
 if(Time>.25f && M->IsMovingOnGround()) {
  const FVector Travel=P->GetActorLocation()-AirStart;
  bool OK=AirPeak>90 && AirPeak<150 && Time<1.4f && Travel.X>200;
  if(WalkPoint==2 || WalkPoint==4 || WalkPoint==7)OK &= Travel.Y < -100;
  if(WalkPoint==3 || WalkPoint==5 || WalkPoint==8)OK &= Travel.Y > 100;
  if(WalkPoint==6)OK &= AirReverse< -150 && M->Velocity.Y>AirReverse+200;
  UE_LOG(LogTemp,Display,TEXT("BH_AIR_CASE %s takeoff=%.1f peak=%.1f flight=%.2f displacement=%s landing=%s"),Names[WalkPoint],AirTakeoff,AirPeak,Time,*Travel.ToString(),*M->Velocity.ToString());
  if(!Check(OK,TEXT("Air steering, ballistic arc and grounded landing"))) { SetActorTickEnabled(false); return; }
  ++WalkPoint; Step=0; Time=0;
  if(WalkPoint==9) { UE_LOG(LogTemp,Display,TEXT("BH_AIR_COMPLETE nine movement scenarios")); FPlatformMisc::RequestExitWithStatus(false,0); SetActorTickEnabled(false); }
 }
}
