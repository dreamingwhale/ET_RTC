// Fill out your copyright notice in the Description page of Project Settings.


#include "SignalServer.h"
#include "SocketIOClientComponent.h"

// Sets default values
ASignalServer::ASignalServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SocketIOClientCompoent = CreateDefaultSubobject<USocketIOClientComponent>(TEXT("SocketIOClientComponent"));
	SocketIOClientCompoent->Connect(TEXT("ws://ec2-13-125-143-170.ap-northeast-2.compute.amazonaws.com:3000"));
}

// Called when the game starts or when spawned
void ASignalServer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASignalServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void JoinRoom(FString roomName)
{
	SocketIOClientCompoent->Emit("join_room", roomName);
}

void Offer(RTCSessionDescription.sdp offer, FString roomName)
{
	SocketIOClientCompoent->Emit("offer", offer, roomName);
}

void Answer(RTCSessionDescription.sdp answer, FString roomName)
{
	SocketIOClientCompoent->Emit("answer", answer, roomName);
}

void Candidate(RTCIceCandidate.candidate candidate, FString roomName)
{
	SocketIOClientCompoent->Emit("candidate", candidate, roomName);
}
