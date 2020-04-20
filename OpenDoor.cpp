// Copyright Arnold Escobedo 2020
#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	//InitialYaw, CurrentYaw, and OpenAngle are declared in OpenDoor.h
	InitialYaw = GetOwner() -> GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle = OpenAngle + InitialYaw;

	ActorThatOpen = GetWorld() -> GetFirstPlayerController() -> GetPawn();

	FindPressurePlate();

	FindAudioComponent();

}

void UOpenDoor::FindPressurePlate()
{
	 	if(!PressurePlate){
		UE_LOG(LogTemp, Error, TEXT("%s has no pressure plate set"), *GetOwner() -> GetName());
	}	

}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner() -> FindComponentByClass<UAudioComponent>();

	if(!AudioComponent){
		UE_LOG(LogTemp, Error, TEXT("%s , No audio component found"), *GetOwner() -> GetName());
	}	

}
// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
		if(TotalMassOfActors() > MassToOpenDoors)
			{

			OpenDoor(DeltaTime);
			DoorLastOpened = GetWorld() ->GetTimeSeconds();

			}

		else{
				if(GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay){

				CloseDoor(DeltaTime);
				}
			}
	
		
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	//console log messages
	
	CurrentYaw = FMath :: Lerp(CurrentYaw, OpenAngle, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = GetOwner() -> GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner() -> SetActorRotation(DoorRotation);
	//Set Actor Rotation

	ClosedDoorSound = false;

	if(!AudioComponent){
		return;
	}

	if(!OpenDoorSound){
	AudioComponent -> Play();
	OpenDoorSound = true;
	}
}


void UOpenDoor::CloseDoor(float DeltaTime)
{
	//console log messages
	
	CurrentYaw = FMath :: Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner() -> GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner() -> SetActorRotation(DoorRotation);
	//Set Actor Rotation
	OpenDoorSound = false;

	if(!AudioComponent){

		return;
	}
	
	if(!ClosedDoorSound){

	AudioComponent -> Play();
	ClosedDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors;
	PressurePlate -> GetOverlappingActors(OverlappingActors);
	for(AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor ->FindComponentByClass<UPrimitiveComponent>() -> GetMass();
	}

	return TotalMass;

}