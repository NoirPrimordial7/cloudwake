#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BHTestDriver.generated.h"
UCLASS()
class ABHTestDriver : public AActor {
 GENERATED_BODY()
public:
 ABHTestDriver();
 virtual void Tick(float D) override;
 int32 Step=0;
 float Time=0;
 float Total=0;
 int32 WalkPoint=0;
 void AirTest(float D);
 FVector AirStart;
 float AirPeak=0, AirTakeoff=0, AirReverse=0;
 bool Check(bool Condition,const TCHAR* What);
 void Next();
};
