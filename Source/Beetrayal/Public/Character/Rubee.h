// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Rubee.generated.h"

class AItem;
class URubeeWidget;
class UEquipmentComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UHoneyComponent;
class UHealthComponent;
class UExperienceComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogRubee, Log, All);

UCLASS(config = Game)
class BEETRAYAL_API ARubee : public ACharacter
{
	GENERATED_BODY()

protected:
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> cameraFP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resources")
	TObjectPtr<UExperienceComponent> experience;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resources")
	TObjectPtr<UHealthComponent> health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resources")
	TObjectPtr<UHoneyComponent> honey;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> jumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> moveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> lookAction;

	/** Interact/PickUp input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> interactAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UEquipmentComponent> equipment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	double pickUpRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	TObjectPtr<URubeeWidget> rubeeWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool bIsDebug;

private:
	TObjectPtr<AItem> focusedItem;

public:
	ARubee();

	virtual void Tick(float DeltaTime) override;

	UCameraComponent *get_camera_fp() const;

protected:
	virtual void BeginPlay();

	/** Called for movement input */
	void move(const FInputActionValue &value);

	/** Called for looking input */
	void look(const FInputActionValue &value);

	void interact();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent *InputComponent) override;
};
