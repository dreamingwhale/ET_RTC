// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SocketIOClientComponent.h"
#include "SDP.generated.h"


// 미디어 설명을 위한 구조체
struct MediaDescription {
	FString type; // 예: "audio", "video"
	int32 port;
	FString protocol; // 예: "RTP/AVP"
	TArray<int32> format; // 타입들
};

// 미디어 속성을 위한 구조체
struct AttributeDescription {
	FString a;
};

// SDP 전체를 나타내는 구조체
struct SessionDescription {
	int32 v; // 프로토콜 버전, 보통 0
	FString o; // originator
	FString s; // sessionName
	FString c; // connectionInfo
	FString t; // timing
	TArray<MediaDescription> m; // mediaDescriptions 미디어 설명들
	TArray<AttributeDescription> a; // attribute 미디어 속성들
	//참고:https://brunch.co.kr/@linecard/141
};

// ICE Candidate를 나타내는 구조체
struct IceCandidate
{

	FString Candidate;
	FString SdpMid;
	int32 SdpMLineIndex;
	FString Foundation;
	int32 Component;
	FString Protocol;
	uint32 Priority;
	FString Address;
	int32 Port;
	FString Type;
};



UCLASS()
class RTC_API ASDP: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASDP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	USocketIOClientComponent* siocc;

public:
	void JoinRoom(FString roomName);
	void Offer(SessionDescription offer, FString roomName);
	void Answer(SessionDescription answer, FString roomName);
	void Candidate(IceCandidate candidate, FString roomName);
	USIOJsonValue* ConvertSessionDescriptionToSIOJsonValue(const SessionDescription& SessionDesc);
	USIOJsonValue* IceCandidateToSIOJsonValue(const IceCandidate& Candidate);

public:
	void OnConnectionFail();

};
