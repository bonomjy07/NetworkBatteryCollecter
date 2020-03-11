// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "Engine/StaticMesh.h"
#include "ConstructorHelpers.h"
#include "Components/BoxComponent.h"


// Sets default values
AFloor::AFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	// Create static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaicMesh"));
	StaticMesh->SetGenerateOverlapEvents(true);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Asset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	if (Asset.Succeeded())
	{
		StaticMesh->SetStaticMesh(Asset.Object);
	}
	RootComponent = StaticMesh;

	BoxCollision1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision_1"));
	BoxCollision1->SetupAttachment(StaticMesh);
	BoxCollision1->bHiddenInGame = false;
	BoxCollisions.Add(BoxCollision1);

	BoxCollision2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision_2"));
	BoxCollision2->SetupAttachment(StaticMesh);
	BoxCollision2->bHiddenInGame = false;
	BoxCollisions.Add(BoxCollision2);

	BoxCollision3 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision_3"));
	BoxCollision3->SetupAttachment(StaticMesh);
	BoxCollision3->bHiddenInGame = false;
	BoxCollisions.Add(BoxCollision3);

	BoxCollision4 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision_4"));
	BoxCollision4->SetupAttachment(StaticMesh);
	BoxCollision4->bHiddenInGame = false;
	BoxCollisions.Add(BoxCollision4);

	BoxCollision5 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision_5"));
	BoxCollision5->SetupAttachment(StaticMesh);
	BoxCollision5->bHiddenInGame = false;
	BoxCollisions.Add(BoxCollision5);
}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

