// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

UTriggerComponent::UTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Actor = GetAcceptableActor();
	if(Actor)
	{
		UPrimitiveComponent* Component = Cast<UPrimitiveComponent>( Actor->GetRootComponent() );
		if (Component)
		{
			Component->SetSimulatePhysics( false );
		}
		Actor->AttachToComponent( this, FAttachmentTransformRules::KeepWorldTransform );
		Mover->SetShouldMove( true );
	}
	else
	{
		Mover->SetShouldMove( false );
	}
}

void UTriggerComponent::SetMover(UMover* NewMover)
{
	Mover = NewMover;
}

AActor* UTriggerComponent::GetAcceptableActor() const
{
	TArray<AActor*> Actors;
	GetOverlappingActors( Actors );
	for(AActor* Actor : Actors)
	{
		if(Actor->ActorHasTag( AcceptActorTag ) && !Actor->ActorHasTag( "Grabbed" ))
		{
			return Actor;
		}
	}

	return nullptr;
}
