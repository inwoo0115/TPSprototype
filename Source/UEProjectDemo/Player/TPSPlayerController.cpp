// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TPSPlayerController.h"
#include "TPSPlayerController.h"

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ������ �� ���콺�� ����Ʈ�� �ٷ� �Էµǰ�
	FInputModeGameOnly GameInput;

	SetInputMode(GameInput);
}
