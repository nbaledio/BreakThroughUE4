// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTGameStateBase.h"
#include "BTOfflineVSGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API ABTOfflineVSGameStateBase : public ABTGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

};
