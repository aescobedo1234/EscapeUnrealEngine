// Copyright Arnold Escobedo 2020
#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();	
}


void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner() -> FindComponentByClass<UInputComponent>();

	if(InputComponent){
		
		InputComponent -> BindAction ("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent -> BindAction ("Grab", IE_Released, this, &UGrabber::Release);

	}else{
		UE_LOG(LogTemp, Warning, TEXT("Input Component missing "), *GetOwner()-> GetName());
	}

}


void UGrabber::FindPhysicsHandle()
{	
	PhysicsHandle = GetOwner() -> FindComponentByClass<UPhysicsHandleComponent>();
	if(PhysicsHandle == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("No physics handle found for %s"), *GetOwner() -> GetName());
		}
}


void UGrabber::Grab()
{


	

	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	if(HitResult.GetActor()){
		PhysicsHandle -> GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			GetPlayersReach()

		);
	}
}

FVector UGrabber:: GetPlayersReach() const {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld() -> GetFirstPlayerController() -> GetPlayerViewPoint(
		PlayerViewPointLocation, 
		PlayerViewPointRotation
		);



	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

void UGrabber::Release()
{

	PhysicsHandle -> ReleaseComponent();

}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	

	if(PhysicsHandle-> GrabbedComponent)
	{
		PhysicsHandle-> SetTargetLocation(GetPlayersReach());
	}

}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{

	

	FHitResult Hit;

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld() -> LineTraceSingleByObjectType(
		Hit,
		GetPlayersWorldPosition(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);
	return Hit;
}

FVector UGrabber:: GetPlayersWorldPosition() const {
	
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld() -> GetFirstPlayerController() -> GetPlayerViewPoint(
		PlayerViewPointLocation, 
		PlayerViewPointRotation
		);



	return PlayerViewPointLocation ;
}
