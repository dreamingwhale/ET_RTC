// Fill out your copyright notice in the Description page of Project Settings.


#include "SDP.h"
#include "SocketIOClientComponent.h"
#include "Windows.h"

// Sets default values
ASDP::ASDP()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	siocc = CreateDefaultSubobject<USocketIOClientComponent>(TEXT("SocketIOClientComponent"));
	SIOConnectParams.AddressAndPort = TEXT("ws://ec2-13-125-143-170.ap-northeast-2.compute.amazonaws.com:3000");
	siocc->URLParams = SIOConnectParams;

}

// Called when the game starts or when spawned
void ASDP::BeginPlay()
{
	//위치를 바꿀 필요가 있음.
	siocc->OnNativeEvent(TEXT("all_users")
		, [this](const FString& Event, const TSharedPtr<FJsonValue>& usersInThisRoom)
		{
			FSessionDescription sdp;
			UE_LOG(LogTemp, Warning, TEXT("Received: %s"), *USIOJConvert::ToJsonString(usersInThisRoom));
			Offer(TEXT("abc@naver.com"),sdp);
		});
	Super::BeginPlay();
}

// Called every frame
void ASDP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDP::JoinRoom(FString roomName, FString email)
{
	siocc->EmitNative(TEXT("join_room"), ConvertDataToJsonObject(roomName, email));
}

void ASDP::Offer( FString offerSendEmail, FSessionDescription sdp)
{
	UE_LOG(LogTemp, Warning, TEXT("Offer"));
	siocc->EmitNative(TEXT("offer"), ConvertOfferDataToJsonObject(FString::FromInt(siocc->GetUniqueID()),offerSendEmail, sdp, siocc->SocketId));
}

void ASDP::Answer(FString answerSendID, FSessionDescription sdp, FString answerReceiveID)
{
	UE_LOG(LogTemp, Warning, TEXT("Answer"));
	siocc->EmitNative(TEXT("answer"), ConvertAnswerDataToJsonObject(answerSendID, sdp, answerReceiveID));
}

void ASDP::Candidate(FString candidateSendID, FIceCandidate candidate, FString candidateReceiveID)
{
	UE_LOG(LogTemp, Warning, TEXT("Candidate"));
	siocc->EmitNative(TEXT("candidate"), ConvertCandidateDataToJsonObject(candidateSendID,candidate, candidateReceiveID));

}

void ASDP::PrintOnNativeEvent()
{
	siocc->OnNativeEvent(TEXT("getOffer")
		, [this](const FString& Event, const TSharedPtr<FJsonValue>& data)
		{
			UE_LOG(LogTemp, Warning, TEXT("Get Offer: %s"), *USIOJConvert::ToJsonString(data));
			GetOffer();
		});
	siocc->OnNativeEvent(TEXT("getAnswer")
		, [](const FString& Event, const TSharedPtr<FJsonValue>& data)
		{
			UE_LOG(LogTemp, Warning, TEXT("Get Answer: %s"), *USIOJConvert::ToJsonString(data));
		});

	siocc->OnNativeEvent(TEXT("getCandidate")
		, [](const FString& Event, const TSharedPtr<FJsonValue>& data)
		{
			UE_LOG(LogTemp, Warning, TEXT("Get Candidate: %s"), *USIOJConvert::ToJsonString(data));
		});
}

void ASDP::GetOffer()
{
	FSessionDescription sdp;
	UE_LOG(LogTemp, Warning, TEXT("Get Offer"));
	Answer("offerSendID", sdp, "offerReceiveID");
}

void ASDP::GetAnswer()
{
	UE_LOG(LogTemp, Warning, TEXT("Get Answer"));
}

void ASDP::GetCandidate()
{
	UE_LOG(LogTemp, Warning, TEXT("Get Candidate"));
}

void ASDP::GetUsersInThisRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("GetUsersInThisRoom"));
}

USIOJsonValue* ASDP::ConvertSessionDescriptionToSIOJsonValue(const FSessionDescription& SessionDesc)
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

USIOJsonValue* ASDP::IceCandidateToSIOJsonValue(const FIceCandidate& Candidate)
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

TSharedPtr<FJsonObject> ASDP::ConvertDataToJsonObject(FString roomName, FString email)
{
	TSharedPtr<FJsonObject> JsonObject = USIOJConvert::MakeJsonObject();
	JsonObject->SetStringField(TEXT("room"), roomName);
	JsonObject->SetStringField(TEXT("email"), email);
	return JsonObject;
}

TSharedPtr<FJsonObject> ASDP::ConvertOfferDataToJsonObject(FString offerSendID, FString offerSendEmail, FSessionDescription sdp, FString offerReceiveID)
{
	TSharedPtr<FJsonObject> JsonObject = USIOJConvert::MakeJsonObject();
	JsonObject->SetObjectField(TEXT("sdp"), ConvertSessionDescriptionToJsonObject(sdp));
	JsonObject->SetStringField(TEXT("offerSendID"), offerSendID);
	JsonObject->SetStringField(TEXT("offerSendEmail"), offerSendEmail);
	JsonObject->SetStringField(TEXT("offerReceiveID"), offerReceiveID);
	return JsonObject;
}

TSharedPtr<FJsonObject> ASDP::ConvertAnswerDataToJsonObject(FString answerSendID, FSessionDescription sdp, FString answerReceiveID)
{
	TSharedPtr<FJsonObject> JsonObject = USIOJConvert::MakeJsonObject();
	JsonObject->SetStringField(TEXT("answerSendID"), answerSendID);
	JsonObject->SetObjectField(TEXT("sdp"), ConvertSessionDescriptionToJsonObject(sdp));
	JsonObject->SetStringField(TEXT("answerReceiveID"), answerReceiveID);
	return JsonObject;
}

TSharedPtr<FJsonObject> ASDP::ConvertCandidateDataToJsonObject(FString candidateSendID, FIceCandidate candidate, FString candidateReceiveID)
{
	TSharedPtr<FJsonObject> JsonObject = USIOJConvert::MakeJsonObject();
	JsonObject->SetStringField(TEXT("candidateSendID"), candidateSendID);
	JsonObject->SetObjectField(TEXT("candidate"), ConvertCandidateToJsonObject(candidate));
	JsonObject->SetStringField(TEXT("candidateReceiveID"), candidateReceiveID);
	return JsonObject;
}

TSharedPtr<FJsonObject> ASDP::ConvertSessionDescriptionToJsonObject(FSessionDescription sdp)
{
	TSharedPtr<FJsonObject> JsonObject = USIOJConvert::MakeJsonObject();
	sdp.v = 0;
	sdp.o = TEXT("o");
	sdp.s = TEXT("s");
	sdp.c = TEXT("c");
	sdp.t = TEXT("t");
	
	JsonObject->SetNumberField(TEXT("v"), sdp.v);
	JsonObject->SetStringField(TEXT("o"), sdp.o);
	JsonObject->SetStringField(TEXT("s"), sdp.s);
	JsonObject->SetStringField(TEXT("c"), sdp.c);
	JsonObject->SetStringField(TEXT("t"), sdp.t);
	return JsonObject;
}

TSharedPtr<FJsonObject> ASDP::ConvertCandidateToJsonObject(FIceCandidate& Candidate)
{
	TSharedPtr<FJsonObject> JsonObject = USIOJConvert::MakeJsonObject();
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
	return JsonObject;
}

void ASDP::OnConnectionFail()
{
	UE_LOG(LogTemp, Warning, TEXT("SocketIO connection failed."));
}
