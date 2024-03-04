// Fill out your copyright notice in the Description page of Project Settings.


#include "SDP.h"
#include "SocketIOClientComponent.h"

// Sets default values
ASDP::ASDP()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	siocc = CreateDefaultSubobject<USocketIOClientComponent>(TEXT("SocketIOClientComponent"));
	
}

// Called when the game starts or when spawned
void ASDP::BeginPlay()
{
	Super::BeginPlay();
	if (siocc != nullptr) {
		siocc->Connect(TEXT("ws://ec2-13-125-143-170.ap-northeast-2.compute.amazonaws.com:3000"));
		UE_LOG(LogTemp, Warning, TEXT("Connected"));
	}
}

// Called every frame
void ASDP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDP::JoinRoom(FString roomName)
{
	siocc->EmitNative("join_room", roomName);
}

void ASDP::Offer(SessionDescription offer, FString roomName)
{
	siocc->Emit("offer", ConvertSessionDescriptionToSIOJsonValue(offer), roomName);
}

void ASDP::Answer(SessionDescription answer, FString roomName)
{
	siocc->Emit("answer", ConvertSessionDescriptionToSIOJsonValue(answer), roomName);
}

void ASDP::Candidate(IceCandidate candidate, FString roomName)
{
	siocc->Emit("candidate", IceCandidateToSIOJsonValue(candidate), roomName);
}

USIOJsonValue* ASDP::ConvertSessionDescriptionToSIOJsonValue(const SessionDescription& SessionDesc)
{
	// 새로운 JSON 객체 생성
	USIOJsonObject* JsonObject = NewObject<USIOJsonObject>();

	// SessionDescription의 필드를 JSON 객체에 추가
	JsonObject->SetNumberField(TEXT("v"), SessionDesc.v);
	JsonObject->SetStringField(TEXT("o"), SessionDesc.o);
	JsonObject->SetStringField(TEXT("s"), SessionDesc.s);
	JsonObject->SetStringField(TEXT("c"), SessionDesc.c);
	JsonObject->SetStringField(TEXT("t"), SessionDesc.t);

	// MediaDescription 배열 처리
	TArray<USIOJsonValue*> MediaArray;
	for (const auto& MediaDesc : SessionDesc.m)
	{
		USIOJsonObject* MediaObject = NewObject<USIOJsonObject>();
		MediaObject->SetStringField(TEXT("type"), MediaDesc.type);
		MediaObject->SetNumberField(TEXT("port"), MediaDesc.port);
		MediaObject->SetStringField(TEXT("protocol"), MediaDesc.protocol);

		TArray<USIOJsonValue*> FormatArray;
		for (int Format : MediaDesc.format)
		{
			FormatArray.Add(USIOJsonValue::ConstructJsonValueNumber(this, Format));
		}
		MediaObject->SetArrayField(TEXT("format"), FormatArray);

		MediaArray.Add(USIOJsonValue::ConstructJsonValueObject(MediaObject, this));
	}
	JsonObject->SetArrayField(TEXT("m"), MediaArray);

	// 최종적으로 SessionDescription을 나타내는 USIOJsonValue 객체 생성
	return USIOJsonValue::ConstructJsonValueObject(JsonObject, this);
}

USIOJsonValue* ASDP::IceCandidateToSIOJsonValue(const IceCandidate& Candidate)
{
	// 새로운 JSON 객체 생성
	USIOJsonObject* JsonObject = NewObject<USIOJsonObject>();

	// SessionDescription의 필드를 JSON 객체에 추가
	JsonObject->SetStringField(TEXT("Candidate"), Candidate.Candidate);
	JsonObject->SetStringField(TEXT("SdpMid"), Candidate.SdpMid);
	JsonObject->SetNumberField(TEXT("SdpMLineIndex"), Candidate.SdpMLineIndex);
	JsonObject->SetStringField(TEXT("Foundation"), Candidate.Foundation);
	JsonObject->SetNumberField(TEXT("Component"), Candidate.Component);
	JsonObject->SetStringField(TEXT("Protocol"), Candidate.Protocol);
	JsonObject->SetNumberField(TEXT("Priority"), Candidate.Priority);
	JsonObject->SetStringField(TEXT("Address"), Candidate.Address);
	JsonObject->SetNumberField(TEXT("Port"), Candidate.Port);
	JsonObject->SetStringField(TEXT("Type"), Candidate.Type);

	return USIOJsonValue::ConstructJsonValueObject(JsonObject, this);
}

void ASDP::OnConnectionFail()
{
	UE_LOG(LogTemp, Warning, TEXT("SocketIO connection failed."));
}
