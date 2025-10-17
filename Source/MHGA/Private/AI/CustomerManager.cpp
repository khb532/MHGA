// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerManager.h"

#include "AI/CustomerAI.h"
#include "AI/CustomerFSM.h"

// Sets default values
ACustomerManager::ACustomerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACustomerManager::BeginPlay()
{
	Super::BeginPlay();
	// 스폰 시간 랜덤으로 지정
	float spawnTime = FMath::RandRange(minTime, maxTime);
	// 스폰 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(spawnTimer, this, &ACustomerManager::SpawnCustomer, spawnTime);

	waitingCustomers.Init(nullptr, waitingPoints.Num());
}

// Called every frame
void ACustomerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACustomerManager::SpawnCustomer()
{
	auto transform = spawnPoint->GetActorTransform();
	// 손님 스폰
	GetWorld()->SpawnActor<ACustomerAI>(spawnFactory, transform.GetLocation(), transform.Rotator());

	// 스폰 시간 랜덤으로 다시 지정
	float spawnTime = FMath::RandRange(minTime, maxTime);
	// 스폰 타이머 재설정
	GetWorld()->GetTimerManager().SetTimer(spawnTimer, this, &ACustomerManager::SpawnCustomer, spawnTime);
}

ATargetPoint* ACustomerManager::RequestWaitingPoint(ACustomerAI* customer)
{
	// 대기열에 빈 자리가 있는지 확인
	int32 emptySpotIdx = waitingCustomers.Find(nullptr);

	// 빈자리가 있고, 대기열이 비어있을 때만 신규 손님을 들여보낸다
	if (emptySpotIdx != INDEX_NONE && wanderingCustomers.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("줄도 비어있고 대기자도 없어서 새 손님을 %d번 자리에 배정합니다"), emptySpotIdx);
		waitingCustomers[emptySpotIdx] = customer;
		return waitingPoints[emptySpotIdx];
	}
	else
	{
		// 빈자리가 없거나, 빈자리가 있어도 먼저 온 대기자가 있다면 신규 손님을 대기자 명단에 추가하고 배회시킨다
		wanderingCustomers.Add(customer);
		return nullptr;
	}
}

void ACustomerManager::OnCustomerFinished(ACustomerAI* customer)
{
	// 배열에서 떠나는 손님 찾기
	for (int32 i = 0; i < waitingCustomers.Num(); ++i)
	{
		if (waitingCustomers[i] == customer)
		{
			// 손님 배열 지우기
			waitingCustomers[i] = nullptr;
			break;
		}
	}
	// 대기열 앞당기기
	UpdateWaitingPosition();

	// 대기자 명단에서 다음 손님 호출
	CallNextCustomerFromWandering();
}

void ACustomerManager::UpdateWaitingPosition()
{
	// 1번 인덱스부터 확인
	for (int32 i = 1; i < waitingCustomers.Num(); ++i)
	{
		// 현재 자리에 손님이 있다면
		if (waitingCustomers[i] != nullptr)
		{
			// 바로 앞자리가 비어있다면
			if (waitingCustomers[i - 1] == nullptr)
			{
				// i번째 손님에게 i-1번째 위치로 이동시키기
				ACustomerAI* customerToMove = waitingCustomers[i];
				if (customerToMove)
				{
					// 해당 위치로 이동
					customerToMove->fsm->MoveToTarget(waitingPoints[i-1]);
				}
				// 배열에서도 앞당기기
				waitingCustomers[i -1] = waitingCustomers[i];
				// 원래 있던 자리 비우기
				waitingCustomers[i] = nullptr;
			}
		}
	}
}

void ACustomerManager::CallNextCustomerFromWandering()
{
	// 대기자 명단에 손님이 있다면
	if (wanderingCustomers.Num() > 0)
	{
		// 대기열에 빈자리가 있는지 다시 확인
		int32 emptySpotIdx = waitingCustomers.Find(nullptr);
		if (emptySpotIdx != INDEX_NONE)
		{
			// 대기자 명단의 가장 첫번째 손님을 데려온다
			ACustomerAI* nextCustomer = wanderingCustomers[0];
			// 대기자 명단에서 이 손님 제거
			wanderingCustomers.RemoveAt(0);
			// 대기열의 빈자리에 이 손님 배정
			waitingCustomers[emptySpotIdx] = nextCustomer;

			// 손님에게 다음 줄로 이동하도록 명령
			if (nextCustomer)
			{
				nextCustomer->fsm->MoveToTarget(waitingPoints[emptySpotIdx]);
			}
		}
	}
}

