#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

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
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	
	if(PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		const FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation( TargetLocation, GetComponentRotation() );
	}
}

void UGrabber::Grab()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if(!PhysicsHandle) return;
	
	FHitResult HitResult;
	if ( GetGrabbableInReach( HitResult ) )
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitComponent->SetSimulatePhysics( true );
		HitComponent->WakeAllRigidBodies();
		AActor* HitActor = HitResult.GetActor();
		HitActor->Tags.Add( "Grabbed" );
		HitActor->DetachFromActor( FDetachmentTransformRules::KeepWorldTransform );
		PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitComponent,
				NAME_None,
				HitResult.ImpactPoint,
				GetComponentRotation()
			);
	}
}

void UGrabber::Release()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();

	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		AActor* GrabbedActor = PhysicsHandle->GetGrabbedComponent()->GetOwner();
		GrabbedActor->Tags.Remove( "Grabbed" );
		PhysicsHandle->GetGrabbedComponent()->WakeAllRigidBodies();
		
		PhysicsHandle->ReleaseComponent();
	}
}


UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent* Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!Result)
	{
		UE_LOG( LogTemp, Error, TEXT("Grabber requires Physics Handle Component") );
	}
	return Result;
}

bool UGrabber::GetGrabbableInReach(FHitResult& HitResult) const
{
	const FVector Start = GetComponentLocation();
	const FVector End = Start + GetForwardVector() * MaxGrabDistance;
	//DrawDebugLine( GetWorld(), Start, End, FColor::Red );
	//DrawDebugSphere( GetWorld(), End, 10, 10, FColor::Blue, false, 5 );
	FCollisionShape Sphere = FCollisionShape::MakeSphere( GrabRadius );

	return GetWorld()->SweepSingleByChannel(
		HitResult,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel2,	//grabber trace channel
		Sphere
		);
}

