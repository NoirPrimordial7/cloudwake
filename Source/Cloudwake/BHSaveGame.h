#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Bellheart.h"
#include "BHSaveGame.generated.h"

USTRUCT()
struct FBHSavedItem {
 GENERATED_BODY()
 UPROPERTY() EBHItem Kind = EBHItem::Fish;
 UPROPERTY() float Health = 0;
 UPROPERTY() int32 Value = 32;
 UPROPERTY() FString Label;
 UPROPERTY() FVector Position = FVector::ZeroVector;
 UPROPERTY() bool Held = false;
 UPROPERTY() int32 StoredIn = 0;
};

UCLASS()
class UBHSaveGame : public USaveGame {
 GENERATED_BODY()
public:
 UPROPERTY() int32 Version = 1;
 UPROPERTY() bool HasBag = false;
 UPROPERTY() bool HasBucket = false;
 UPROPERTY() TArray<int32> Hotbar;
 UPROPERTY() int32 SelectedSlot = 0;
 UPROPERTY() int32 Crowns = 12;
 UPROPERTY() int32 Quest = 0;
 UPROPERTY() int32 FishSold = 0;
 UPROPERTY() bool HasRod = false;
 UPROPERTY() bool HasKnife = false;
 UPROPERTY() bool HasBait = false;
 UPROPERTY() int32 KnifeDamage = 18;
 UPROPERTY() float Health = 100;
 UPROPERTY() FVector Position = FVector(0,-8600,110);
 UPROPERTY() FRotator View = FRotator(0,90,0);
 UPROPERTY() bool KnifeEquipped = false;
 UPROPERTY() TArray<FBHSavedItem> Items;
};
