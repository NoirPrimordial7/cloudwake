#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "Components/ActorComponent.h"
#include "Bellheart.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class ABHPlayer;
class ABHWorld;
class ABHHUD;

UENUM(BlueprintType)
enum class EBHItem : uint8 { None, Fish, Bellheart };
UENUM(BlueprintType)
enum class EBHFishState : uint8 { Wander, Investigate, Bite, Hooked, Landed, Dead };

// Shared, event-driven quest progression; world actors never own the quest state.
UCLASS(ClassGroup=(Cloudwake), meta=(BlueprintSpawnableComponent))
class CLOUDWAKE_API UBHProgress : public UActorComponent {
 GENERATED_BODY()
public:
 UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Crowns = 12;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 Quest = 0;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 FishSold = 0;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool HasRod = false;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool HasKnife = false;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool HasBait = false;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 KnifeDamage = 18;
 UFUNCTION(BlueprintCallable) bool Spend(int32 Amount);
 UFUNCTION(BlueprintCallable) void Event(FName Name);
 FString Objective() const;
};

UCLASS()
class CLOUDWAKE_API ABHInteractable : public AActor {
 GENERATED_BODY()
public:
 ABHInteractable();
 UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh;
 UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Action;
 UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Label;
 UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Price = 0;
 UFUNCTION(BlueprintCallable) virtual void Use(ABHPlayer* Player);
 virtual FString Prompt(ABHPlayer* Player) const;
};

UCLASS()
class CLOUDWAKE_API ABHPhysicalItem : public ABHInteractable {
 GENERATED_BODY()
public:
 ABHPhysicalItem();
 UPROPERTY(EditAnywhere, BlueprintReadWrite) EBHItem Kind = EBHItem::Fish;
 UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Value = 32;
 UPROPERTY(EditAnywhere, BlueprintReadWrite) float Health = 18;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) EBHFishState State = EBHFishState::Wander;
 UPROPERTY(VisibleAnywhere) FVector Home;
 UPROPERTY() int32 StoredIn = 0;
 float Age = 0;
 float BiteTime = 0;
 virtual void Tick(float DeltaSeconds) override;
 virtual void Use(ABHPlayer* Player) override;
 virtual FString Prompt(ABHPlayer* Player) const override;
 void Hurt(float Damage);
 void Land(FVector Position);
};

UCLASS()
class CLOUDWAKE_API ABHBellcrab : public ABHInteractable {
 GENERATED_BODY()
public:
 ABHBellcrab();
 UPROPERTY(EditAnywhere, BlueprintReadWrite) float Health = 180;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool Active = false;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool Telegraph = false;
 float PhaseTime = 0;
 FVector Arena = FVector(2600,-1100,700);
 virtual void Tick(float DeltaSeconds) override;
 void Hurt(float Damage);
};

UCLASS()
class CLOUDWAKE_API ABHPlayer : public ACharacter {
 GENERATED_BODY()
public:
 ABHPlayer();
 UPROPERTY(VisibleAnywhere) UCameraComponent* Camera;
 UPROPERTY(VisibleAnywhere) UStaticMeshComponent* HeldTool;
 UPROPERTY(VisibleAnywhere) UBHProgress* Progress;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float Health = 100;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) ABHPhysicalItem* HeldItem = nullptr;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) ABHPhysicalItem* HookedFish = nullptr;
 UPROPERTY(VisibleAnywhere) ABHInteractable* Target = nullptr;
 UPROPERTY(VisibleAnywhere) AActor* Bobber = nullptr;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool KnifeEquipped = false;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool Reeling = false;
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float Tension = 0;
 UPROPERTY() ABHInteractable* DialogueSpeaker = nullptr;
 FString SpeakerName;
 UPROPERTY() bool HasBag = false;
 UPROPERTY() bool HasBucket = false;
 UPROPERTY() TArray<int32> Hotbar = {1,2,3,4};
 UPROPERTY() int32 SelectedSlot = 0;
 bool InventoryOpen = false;
 int32 AssignGear = 0;
 FString Message;
 float MessageTime = 0;
 float AttackCooldown = 0;
 float CastCharge = 0;
 bool Charging = false;
 FVector BobberGoal;
 bool BobberInWater = false;
 float AutoSaveTime = 0;
 virtual void BeginPlay() override;
 virtual void Tick(float DeltaSeconds) override;
 virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
 void Forward(float V); void Right(float V); void Turn(float V); void Look(float V);
 void Interact(); void PrimaryDown(); void PrimaryUp(); void Drop();
 void EquipRod(); void EquipKnife(); void Sprint(); void Walk();
 void Say(const FString& Text);
 void DamagePlayer(float Damage);
 void CancelCast();
 FHitResult Trace(float Range) const;
 void CastLine(float Charge);
 bool SaveCheckpoint();
 bool LoadCheckpoint();
 FString SaveSlot() const;
 bool IsLoopTest() const;
 void QuickSave();
 void QuickLoad();
 void RecoverAtDock();
 bool OwnsGear(int32 Gear) const;
 FString GearName(int32 Gear) const;
 int32 ActiveGear() const;
 void SelectSlot(int32 Slot);
 void SlotOne(); void SlotTwo(); void SlotThree(); void SlotFour();
 void ScrollNext(); void ScrollPrevious();
 void ToggleInventory(); void InventoryClick();
 void AssignToSlot(int32 Gear,int32 Slot);
 void StoreHeld(); bool StoreIn(int32 Container);
 void RetrieveStored(int32 Index);
 TArray<ABHPhysicalItem*> StoredItems(int32 Container=0) const;
 void DrawInventory(ABHHUD* HUD);

};

UCLASS()
class CLOUDWAKE_API ABHWorld : public AActor {
 GENERATED_BODY()
public:
 ABHWorld();
 virtual void BeginPlay() override;
 UPROPERTY(VisibleAnywhere) ABHBellcrab* Crab = nullptr;
 UPROPERTY(VisibleAnywhere) bool Restored = false;
 UPROPERTY(VisibleAnywhere) bool Built = false;
 AActor* Box(const FString& Name, FVector Meters, FVector Size, FLinearColor Color, bool Collision = true);
 void Path(FVector A, FVector B, float Width = 3);
 void Building(const FString& Name, FVector P, FVector Size);
 ABHInteractable* Station(FName Action, const FString& Label, FVector P, int32 Price = 0);
 void Label(const FString& Text, FVector P);
 UFUNCTION(BlueprintCallable, CallInEditor) void Build();
 void Restore(bool IsRestored = true, bool PlayCue = true);
};

UCLASS()
class CLOUDWAKE_API ABHHUD : public AHUD {
 GENERATED_BODY()
public:
 virtual void DrawHUD() override;
};

UCLASS()
class CLOUDWAKE_API ABHGameMode : public AGameModeBase {
 GENERATED_BODY()
public:
 ABHGameMode();
 virtual void BeginPlay() override;
};
