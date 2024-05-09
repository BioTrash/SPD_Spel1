// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SlimeBossAI.h"
#include "PlayerCharacter.h"
#include "Projectile.h"

void ASlimeBossAIController::BeginPlay()
{
	Super::BeginPlay();
	SetPlayer();
	if (Boss)
	{
		PawnMesh = Boss->FindComponentByClass<UStaticMeshComponent>();
		ProjectileSpawn = Boss->FindComponentByClass<USceneComponent>();
		if (ProjectileSpawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("SPAWN FOUND" ));
		}
	}
	//Sätter dens olika behaviors i Beginplay från behaviortree och blackboard
	//Hanna
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
		
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"),true);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseThree"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), false);
	}
	SetFocus(Player);
}
void ASlimeBossAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LastShotTime += DeltaSeconds;

	SetFocus(Player);
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());

	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		RotateHead(PlayerLocation);
	}
	UpdateBossPhase();
}

void ASlimeBossAIController::Shoot()
{
	//Louis gjorde denna jag optimerade den och gjorde det till en egen metod
	//Hanna
	FVector SpawnLocation = ProjectileSpawn->GetComponentLocation();
	FRotator SpawnRotation = ProjectileSpawn->GetComponentRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (Projectile)
	{
		Projectile->SetDamage(ProjectileDamage);
	}
}
void ASlimeBossAIController::RotateHead(FVector TargetLocation)
{
	//Får dens huvud att rotera
	//Hanna
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PawnMesh->GetComponentLocation(), TargetLocation);
	LookAtRotation.Pitch = 0;
	LookAtRotation.Roll = 0;
	LookAtRotation.Yaw += -90.f; 
	PawnMesh->SetWorldRotation(LookAtRotation);
}
void ASlimeBossAIController::FireCooldown()
{
	UE_LOG(LogTemp, Warning, TEXT("HEJHEJ"));
}

void ASlimeBossAIController::SetPlayer()
{
	if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
	if (Player == nullptr)
	{
		//Shrek ;)
	}

	if (Cast<ASlimeBossAI>(GetPawn()))
	{
		Boss = Cast<ASlimeBossAI>(GetPawn());
	}

	if (Boss == nullptr)
	{
	}
}
void ASlimeBossAIController::UpdateBossPhase()
{
	//Uppdatera vilken del av fasen bossen är i beroende på Bossens Health
	//Hämtar health från ASlimeBossAI
	//Hanna
	ASlimeBossAI* SlimeBoss = Cast<ASlimeBossAI>(GetPawn());
	if (SlimeBoss)
	{
		float Health = SlimeBoss->GetHealth();
		if (Health <= 150 && Health > 100)
		{
			UE_LOG(LogTemp, Warning, TEXT("nu börjar phase one"));
			BossPhaseOne();
		}
		else if (Health <= 100 && Health > 50)
		{
			UE_LOG(LogTemp, Warning, TEXT("nu börjar phase two"));
			BossPhaseTwo();
		}
		else if (Health <= 50)
		{
			UE_LOG(LogTemp, Warning, TEXT("nu börjar phase three"));
			BossPhaseThree();
		}
	}
}
void ASlimeBossAIController::BossPhaseOne()
{
	//Skjuter endast på spelaren i första fasen
	//Hanna
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"), true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), true);
	if (LastShotTime >= ShootCooldown)
	{
		Shoot();
		LastShotTime = 0;
	}
}
void ASlimeBossAIController::BossPhaseTwo()
{
	//Andra bossfasen, den skjuter och spawnar in fiender
	//Hanna
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"),false);
	GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), true);
	
}
void ASlimeBossAIController::BossPhaseThree()
{
	//Tredje och sista bossfasen, den har en slamattack där den spawnar in fiender
	//Hanna
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"), false);
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseThree"), false);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
	GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), true);
	SlamAttack();
	//Göra en slam här där den spawnar in mer fiender 
}
void ASlimeBossAIController::SlamAttack()
{
	//Hanna
	//slamattacken
	FVector GroundLocation = FVector(Boss->GetActorLocation().X, Boss->GetActorLocation().Y, 0.0f);
	Boss->SetActorLocation(GroundLocation);
	FVector SlamCenter = Boss->GetActorLocation();
	float DamageRadius = 500.0f; 
	TArray<AActor*> OverlappingActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		float Distance = FVector::Distance(Actor->GetActorLocation(), SlamCenter);
		float Damage = FMath::Max(0.0f, 100.0f - Distance); 
		Actor->TakeDamage(Damage, FDamageEvent(), nullptr, this);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Slam Attack"));
	GetWorldTimerManager().SetTimer(SlamAttackTimerHandle, this, &ASlimeBossAIController::EndSlamAttack, 2.0f, false);
}
void ASlimeBossAIController::EndSlamAttack()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"), true); 
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), false); 
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseThree"), false); 
	GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), true);
}
void ASlimeBossAIController::SpawnEnemies()
{
	//Hanna
	//spawna in fiender
}

