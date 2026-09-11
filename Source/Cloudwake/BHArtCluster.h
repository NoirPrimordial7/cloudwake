#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BHArtCluster.generated.h"
class UHierarchicalInstancedStaticMeshComponent;
// Editor-authored repeated scenery, batched without changing gameplay collision.
UCLASS()
class CLOUDWAKE_API ABHArtCluster : public AActor {
 GENERATED_BODY()
public:
 ABHArtCluster();
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UHierarchicalInstancedStaticMeshComponent* Instances;
};
