#include "BHArtCluster.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
ABHArtCluster::ABHArtCluster() {
 Instances=CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Instances"));
 RootComponent=Instances;
 Instances->SetMobility(EComponentMobility::Static);
 Instances->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 Tags.Add(TEXT("BH_Art"));
}
