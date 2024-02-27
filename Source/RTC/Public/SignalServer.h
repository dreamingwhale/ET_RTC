// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SessionDescription.h"
#include "SignalServer.generated.h"


class USocketIOClientComponent;


UCLASS()
class RTC_API ASignalServer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASignalServer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	USocketIOClientComponent* SocketIOClientCompoent;

public:
	void JoinRoom(FString roomName);
	void Offer(sdp::SessionDescription offer, FString roomName);
	void Answer(sdp::SessionDescription answer, FString roomName);
	void Candidate(candidate candidate, FString roomName);
};
