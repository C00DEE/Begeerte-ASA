#include "../../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../../../external/SDK/SDK_Headers.hpp"
#include "Aimbot.h"
#include "../../../internal/Config/Configs.h"
#include "../../../internal/ESP/ESP.h"
#include "../../../internal/Util/Util.h"
#include <chrono>
#include <map>
#include <float.h>

namespace g_Aimbot {
	static bool bIsAutoFiring = false;
	static SDK::APrimalCharacter* pCurrentLockedTarget = nullptr;

	// 基于骨骼分布动态计算目标的真实物理半径
	float GetTargetBoneClusterRadius(SDK::APrimalCharacter* Char, const SDK::FVector& Center) {
		if (!Char || !Char->Mesh) return 0.0f;

		int BoneCount = Char->Mesh->GetNumBones();
		if (BoneCount <= 0) return 0.0f;

		float MaxDistSq = 0.0f;
		for (int i = 0; i < BoneCount; i++) {
			SDK::FVector BoneLoc = Char->Mesh->GetSocketLocation(Char->Mesh->GetBoneName(i));
			if (BoneLoc.IsZero()) continue;

			float DistSq = (float)SDK::UKismetMathLibrary::Vector_DistanceSquared(BoneLoc, Center);
			if (DistSq > MaxDistSq) MaxDistSq = DistSq;
		}

		return (float)SDK::UKismetMathLibrary::sqrt((double)MaxDistSq);
	}

	void DrawStatusText(TargetInfo& best) {
		SDK::APlayerController* LocalPC = g_Util::GetLocalPC();
		if (!LocalPC || !best.bIsValid || !best.Character) return;

		SDK::FVector2D ScreenPos;
		if (LocalPC->ProjectWorldLocationToScreen(best.BestComponentLocation, &ScreenPos, false)) {
			ImColor textColor = best.bIsTriggering ? ImColor(255, 0, 0) : (best.bIsLocked ? ImColor(0, 255, 0) : ImColor(255, 255, 0));

			char buf[128];
			_snprintf_s(buf, sizeof(buf), "HP: %.0f | DIST: %.0fm | HIT: %.0f%%",
				best.Health, best.Distance / 100.0f, best.HitChance);

			ImGui::GetBackgroundDrawList()->AddText(ImVec2(ScreenPos.X, ScreenPos.Y - 40), textColor, buf);

			if (best.bIsTriggering) {
				ImGui::GetBackgroundDrawList()->AddText(ImVec2(ScreenPos.X, ScreenPos.Y - 60), ImColor(255, 0, 0), ">>> SHOOTING <<<");
			}
		}
	}

	void VisualizeTargetBones(SDK::APrimalCharacter* Char, SDK::APlayerController* PC) {
		if (!Char || !Char->Mesh || !PC) return;
		int BoneCount = Char->Mesh->GetNumBones();
		SDK::FVector2D lastPos = { 0, 0 };
		for (int i = 0; i < BoneCount; i++) {
			SDK::FVector BoneLoc = Char->Mesh->GetSocketLocation(Char->Mesh->GetBoneName(i));
			SDK::FVector2D sPos;
			if (PC->ProjectWorldLocationToScreen(BoneLoc, &sPos, false)) {
				ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(sPos.X, sPos.Y), 1.2f, g_Util::GetU32Color(g_Config::AimPointsColor));
				if (lastPos.X != 0 && lastPos.Y != 0) {
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(lastPos.X, lastPos.Y), ImVec2(sPos.X, sPos.Y), g_Util::GetU32Color(g_Config::AimSkeletonColor));
				}
				lastPos = sPos;
			}
		}
	}

	float GetAngleDistance(SDK::FVector CamLoc, SDK::FVector TargetLoc, SDK::FRotator CamRot) {
		SDK::FVector Diff = { TargetLoc.X - CamLoc.X, TargetLoc.Y - CamLoc.Y, TargetLoc.Z - CamLoc.Z };
		SDK::FVector DirToTarget = SDK::UKismetMathLibrary::Normal(Diff, 0.0001f);
		SDK::FVector CamForward = SDK::UKismetMathLibrary::GetForwardVector(CamRot);
		double Dot = SDK::UKismetMathLibrary::Dot_VectorVector(DirToTarget, CamForward);
		Dot = SDK::UKismetMathLibrary::FClamp(Dot, -1.0f, 1.0f);
		return (float)SDK::UKismetMathLibrary::DegAcos(Dot);
	}

	bool IsLocationVisible(SDK::APlayerController* PC, SDK::FVector Start, SDK::FVector End, SDK::AActor* TargetActor) {
		SDK::FHitResult Hit;
		SDK::TArray<SDK::AActor*> Ignore;
		Ignore.Add(PC->Pawn);
		bool bHasHit = SDK::UKismetSystemLibrary::LineTraceSingle(PC, Start, End,
			SDK::ETraceTypeQuery::TraceTypeQuery1, false, Ignore, SDK::EDrawDebugTrace::None, &Hit, true,
			SDK::FLinearColor(0, 0, 0, 0), SDK::FLinearColor(0, 0, 0, 0), 0.0f);
		if (!bHasHit) return true;
		SDK::UObject* HitObj = Hit.HitObjectHandle.ReferenceObject.Get();
		return (HitObj == (SDK::UObject*)TargetActor);
	}

	TargetInfo GetBestTarget() {
		TargetInfo Best;
		SDK::UWorld* World = SDK::UWorld::GetWorld();
		if (!World || !World->PersistentLevel) return Best;

		SDK::APlayerController* LocalPC = g_Util::GetLocalPC();
		if (!LocalPC || !LocalPC->Pawn) return Best;

		SDK::AShooterCharacter* MyChar = (SDK::AShooterCharacter*)LocalPC->Pawn;
		SDK::AShooterWeapon* MyWeapon = MyChar->CurrentWeapon;

		SDK::FVector CamLoc; SDK::FRotator CamRot;
		LocalPC->GetPlayerViewPoint(&CamLoc, &CamRot);

		auto& Actors = World->PersistentLevel->Actors;
		SDK::APrimalCharacter* BestChar = nullptr;
		float MinDistance = FLT_MAX;
		float MinHealth = FLT_MAX;
		float MinAngle = FLT_MAX;
		SDK::FVector BestLoc = { 0,0,0 };
		float BestRadius = 0.0f;

		for (int i = 0; i < Actors.Num(); i++) {
			SDK::AActor* Actor = Actors[i];
			if (!Actor || Actor == LocalPC->Pawn || Actor->bHidden || !Actor->IsA(SDK::APrimalCharacter::StaticClass())) continue;

			SDK::APrimalCharacter* Char = (SDK::APrimalCharacter*)Actor;
			if (Char->IsDead() || g_ESP::GetRelation(Char, (SDK::APrimalCharacter*)LocalPC->Pawn) == g_ESP::RelationType::Team) continue;

			if (MyWeapon && !MyWeapon->ShouldDealDamage(Char)) continue;

			SDK::USkeletalMeshComponent* Mesh = Char->Mesh;
			if (!Mesh) continue;

			SDK::FVector MinB = { FLT_MAX, FLT_MAX, FLT_MAX };
			SDK::FVector MaxB = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
			int BoneCount = Mesh->GetNumBones();
			for (int j = 0; j < BoneCount; j++) {
				SDK::FVector BoneLoc = Mesh->GetSocketLocation(Mesh->GetBoneName(j));
				if (BoneLoc.IsZero()) continue;
				MinB.X = min(MinB.X, BoneLoc.X); MinB.Y = min(MinB.Y, BoneLoc.Y); MinB.Z = min(MinB.Z, BoneLoc.Z);
				MaxB.X = max(MaxB.X, BoneLoc.X); MaxB.Y = max(MaxB.Y, BoneLoc.Y); MaxB.Z = max(MaxB.Z, BoneLoc.Z);
			}
			SDK::FVector CurrentTargetLoc = (MinB + MaxB) / 2.0f;

			float CurrentRadius = GetTargetBoneClusterRadius(Char, CurrentTargetLoc);
			if (CurrentRadius <= 0.0f) continue;

			float Dist = (float)SDK::UKismetMathLibrary::Vector_Distance(CamLoc, CurrentTargetLoc);
			if (MyWeapon && Dist > MyWeapon->InstantConfig.WeaponRange) continue;

			if (!IsLocationVisible(LocalPC, CamLoc, CurrentTargetLoc, Char)) continue;

			float Angle = GetAngleDistance(CamLoc, CurrentTargetLoc, CamRot);
			if (Angle > g_Config::AimbotFOV) continue;

			float Bias = 0.0f;
			if (pCurrentLockedTarget && Char == pCurrentLockedTarget) Bias = 300.0f;

			bool bIsBetter = false;
			if ((Dist - Bias) < MinDistance - 150.0f) bIsBetter = true;
			else if (Dist <= MinDistance + 150.0f) {
				if (Char->GetHealth() < MinHealth - 1.0f) bIsBetter = true;
				else if (abs(Char->GetHealth() - MinHealth) < 1.0f && Angle < MinAngle) bIsBetter = true;
			}

			if (bIsBetter) {
				MinDistance = Dist; MinHealth = Char->GetHealth(); MinAngle = Angle;
				BestChar = Char; BestLoc = CurrentTargetLoc; BestRadius = CurrentRadius;
			}
		}

		if (BestChar) {
			pCurrentLockedTarget = BestChar;
			Best.Character = BestChar;
			Best.BestComponentLocation = BestLoc;
			Best.FovDistance = GetAngleDistance(CamLoc, BestLoc, CamRot);
			Best.Distance = MinDistance;
			Best.Health = MinHealth;
			Best.bIsValid = true;

			if (MyWeapon) {
				float TargetRadiusDeg = (float)SDK::UKismetMathLibrary::DegAtan2((double)BestRadius, (double)MinDistance);
				float CurrentSpreadDeg = (float)SDK::UKismetMathLibrary::RadiansToDegrees((double)MyWeapon->CurrentFiringSpread);
				float RecoilPitch = (float)SDK::UKismetMathLibrary::abs((double)MyWeapon->AimDriftPitchAngle);
				float RecoilYaw = (float)SDK::UKismetMathLibrary::abs((double)MyWeapon->AimDriftYawAngle);
				float TotalRecoilOffsetDeg = (float)SDK::UKismetMathLibrary::sqrt((double)(RecoilPitch * RecoilPitch + RecoilYaw * RecoilYaw));

				float TotalDispersion = Best.FovDistance + CurrentSpreadDeg + TotalRecoilOffsetDeg;

				if (TotalDispersion <= TargetRadiusDeg) {
					Best.HitChance = 100.0f;
				}
				else {
					double Ratio = (double)TargetRadiusDeg / (double)TotalDispersion;
					Best.HitChance = (float)SDK::UKismetMathLibrary::FClamp(Ratio * 100.0, 0.0, 100.0);
				}

				Best.bIsLocked = (Best.HitChance >= (float)g_Config::TriggerHitChance);
			}
		}
		else {
			pCurrentLockedTarget = nullptr;
		}
		return Best;
	}

	void Tick() {
		if (!g_Config::bAimbotEnabled && !g_Config::bTriggerbotEnabled) {
			pCurrentLockedTarget = nullptr;
			return;
		}

		SDK::UWorld* World = SDK::UWorld::GetWorld();
		if (!World || !World->PersistentLevel) return;

		SDK::APlayerController* LocalPC = g_Util::GetLocalPC();
		if (!LocalPC || !LocalPC->Pawn) return;

		SDK::AShooterCharacter* MyChar = (SDK::AShooterCharacter*)LocalPC->Pawn;
		SDK::AShooterWeapon* MyWeapon = MyChar->CurrentWeapon;

		if (!MyWeapon || MyWeapon->GetAmmoReloadState() != SDK::EWeaponAmmoReloadState::Ready || MyWeapon->GetCurrentAmmo() <= 0) {
			if (bIsAutoFiring) { MyWeapon->StopFire(); bIsAutoFiring = false; }
			return;
		}

		TargetInfo Best = GetBestTarget();

		if (g_Config::bDrawAimPoints && Best.bIsValid) {
			VisualizeTargetBones(Best.Character, LocalPC);
		}

		if (Best.bIsValid) {
			SDK::FVector CamLoc; SDK::FRotator CamRot;
			LocalPC->GetPlayerViewPoint(&CamLoc, &CamRot);
			SDK::FRotator TargetRot = SDK::UKismetMathLibrary::FindLookAtRotation(CamLoc, Best.BestComponentLocation);

			// --- 压枪补偿逻辑 (Recoil Compensation System) ---
			if (g_Config::AimbotRCX > 0.0f || g_Config::AimbotRCY > 0.0f) {
				// 获取引擎计算好的当前枪口偏移
				float DriftPitch = MyWeapon->AimDriftPitchAngle;
				float DriftYaw = MyWeapon->AimDriftYawAngle;

				// 根据配置百分比应用压枪强度 (100 = 完全抵消)
				TargetRot.Pitch -= (DriftPitch * (g_Config::AimbotRCY / 100.0f));
				TargetRot.Yaw -= (DriftYaw * (g_Config::AimbotRCX / 100.0f));
			}

			// Aimbot 转向控制
			if (g_Config::bAimbotEnabled) {
				if (g_Config::AimbotSmooth >= 100.0f) {
					LocalPC->SetControlRotation(TargetRot);
				}
				else {
					SDK::FRotator CurrentRot = LocalPC->ControlRotation;
					float Alpha = (float)SDK::UKismetMathLibrary::FClamp((double)g_Config::AimbotSmooth / 100.0, 0.01, 1.0);
					SDK::FRotator InterpRot = SDK::UKismetMathLibrary::RLerp(CurrentRot, TargetRot, Alpha, true);
					LocalPC->SetControlRotation(InterpRot);
				}
			}

			// Triggerbot 逻辑
			if (g_Config::bTriggerbotEnabled) {
				if (Best.bIsLocked && MyWeapon->CanFire(false)) {
					if (!bIsAutoFiring) {
						MyWeapon->StartFire(false);
						bIsAutoFiring = true;
					}
					Best.bIsTriggering = true;
				}
				else if (bIsAutoFiring) {
					MyWeapon->StopFire();
					bIsAutoFiring = false;
				}
			}
		}
		else {
			if (bIsAutoFiring) {
				MyWeapon->StopFire();
				bIsAutoFiring = false;
			}

			// 即使没有目标，如果正在射击且开启了压枪，也可以选择独立压枪逻辑（可选）
			// 这里根据需求只在有目标时补偿 Aimbot 轨迹
		}

		DrawStatusText(Best);
	}
}