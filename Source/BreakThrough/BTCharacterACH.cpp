// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTCharacterACH.h"

void ABTCharacterACH::HitDetection()
{
	ABTCharacterBase::HitDetection();
}

void ABTCharacterACH::UpdateCharacter(int32 CurrentInputs, int32 FrameNumber)
{
	if (CurrentState.HitStop > 8 && CurrentState.bHitSuccess && Opponent->CurrentState.CharacterHitState & Piercing && Opponent->CurrentState.Health > 0 &&
		(IsCurrentAnimation(Normal5B) || IsCurrentAnimation(Normal2B) || IsCurrentAnimation(Normal6B) || IsCurrentAnimation(NormalJB)))
	{
		//make opponent flash magenta on pierce
		Opponent->StatusMix = .7f;
		Opponent->CurrentState.StatusTimer = 10;
		Opponent->StatusColor = FVector(1, .1, 1);
	}

	ABTCharacterBase::UpdateCharacter(CurrentInputs, FrameNumber);
}

void ABTCharacterACH::UpdatePosition()
{
	ABTCharacterBase::UpdatePosition();
}

void ABTCharacterACH::DrawCharacter()
{
	ABTCharacterBase::DrawCharacter();

	DynamicSpearEdge->SetScalarParameterValue(FName("SpearGlow"), (float)CurrentState.SpecialVariables[BreakCharge]/100);

	if (CurrentState.StatusTimer > 0)
	{
		DynamicBodyMain->SetVectorParameterValue(FName("StatusColor"), StatusColor);
		DynamicBodyMain->SetScalarParameterValue(FName("StatusMix"), StatusMix);
		DynamicBodyMetallic->SetVectorParameterValue(FName("StatusColor"), StatusColor);
		DynamicBodyMetallic->SetScalarParameterValue(FName("StatusMix"), StatusMix);
		DynamicBodySpec->SetVectorParameterValue(FName("StatusColor"), StatusColor);
		DynamicBodySpec->SetScalarParameterValue(FName("StatusMix"), StatusMix);
		DynamicSeals->SetVectorParameterValue(FName("StatusColor"), StatusColor);
		DynamicSeals->SetScalarParameterValue(FName("StatusMix"), StatusMix);
		DynamicRightEye->SetVectorParameterValue(FName("StatusColor"), StatusColor);
		DynamicRightEye->SetScalarParameterValue(FName("StatusMix"), StatusMix);
		DynamicSpearEdge->SetVectorParameterValue(FName("StatusColor"), StatusColor);
		DynamicSpearEdge->SetScalarParameterValue(FName("StatusMix"), StatusMix);
		DynamicSpearMetallic->SetVectorParameterValue(FName("StatusColor"), StatusColor);
		DynamicSpearMetallic->SetScalarParameterValue(FName("StatusMix"), StatusMix);
		DynamicSpearSpec->SetVectorParameterValue(FName("StatusColor"), StatusColor);
		DynamicSpearSpec->SetScalarParameterValue(FName("StatusMix"), StatusMix);
	}
	else
	{
		DynamicBodyMain->SetScalarParameterValue(FName("StatusMix"), 0);
		DynamicBodyMetallic->SetScalarParameterValue(FName("StatusMix"), 0);
		DynamicBodySpec->SetScalarParameterValue(FName("StatusMix"), 0);
		DynamicSpearEdge->SetScalarParameterValue(FName("StatusMix"), 0);
		DynamicSpearSpec->SetScalarParameterValue(FName("StatusMix"), 0);
		DynamicSpearMetallic->SetScalarParameterValue(FName("StatusMix"), 0);
		DynamicRightEye->SetScalarParameterValue(FName("StatusMix"), 0);
		DynamicSeals->SetScalarParameterValue(FName("StatusMix"), 0);
	}
}

bool ABTCharacterACH::NonKnockdownLanding()
{
	if (IsCurrentAnimation(Normal6B))
	{
		return true;
	}

	return ABTCharacterBase::NonKnockdownLanding();
}

bool ABTCharacterACH::ConditionalTransitions()
{
	return ABTCharacterBase::ConditionalTransitions();
}

bool ABTCharacterACH::PassiveTransitions()
{
	return ABTCharacterBase::PassiveTransitions();
}

bool ABTCharacterACH::ExitTimeTransitions()
{
	if (IsCurrentAnimation(NormalJB) || IsCurrentAnimation(NormalJH) || IsCurrentAnimation(NormalJM) || IsCurrentAnimation(NormalJL) || IsCurrentAnimation(TowerLeap))
		return EnterNewAnimation(MidJump);

	if (IsCurrentAnimation(Normal5B) || IsCurrentAnimation(Normal5H) || IsCurrentAnimation(Normal5M) || IsCurrentAnimation(Normal5L) || IsCurrentAnimation(Normal2B) || IsCurrentAnimation(Normal2H) ||
		IsCurrentAnimation(Normal6L) || IsCurrentAnimation(Normal6M) || IsCurrentAnimation(Normal6B))
		return EnterNewAnimation(IdleStand);

	if (IsCurrentAnimation(Normal2M) || IsCurrentAnimation(Normal2L))
		return EnterNewAnimation(IdleCrouch);

	return ABTCharacterBase::ExitTimeTransitions();
}

void ABTCharacterACH::AnimationEvents()
{
	ABTCharacterBase::AnimationEvents();

	//Add character specific animation logic here (adding more available actions based on current state, etc.)
	if (CurrentState.SpecialVariables[WCDuration] > 0 && CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0) //makes Achealis's special attacks able to cancel into other specials if her install is active
	{
		if (CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & IsSpecial)
		{
			for (int32 i = 0; i < CurrentState.CurrentAnimFrame.Hitboxes.Num(); i++)
			{
				CurrentState.CurrentAnimFrame.Hitboxes[i].PotentialActions |= AcceptSpecial;
			}
		}
	}

	if (IsCurrentAnimation(NormalThrow))
	{
		if (CurrentState.AnimFrameIndex < 8)
		{
			Opponent->EnterNewAnimation(Opponent->HitSLIn);
		}
	}

	if (IsCurrentAnimation(Normal6B) && CurrentState.AnimFrameIndex == 8)
	{
		CurrentState.Position.Y = 0;
		CurrentState.Velocity.X *= .95;
		CurrentState.bIsAirborne = false;
	}

	if (((IsCurrentAnimation(Normal5B) || IsCurrentAnimation(Normal2B) || IsCurrentAnimation(Normal6B) || IsCurrentAnimation(NormalJB)) && 
		(CurrentState.CurrentAnimFrame.bArmorActive || CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0)) || 
		(IsCurrentAnimation(NormalThrow) && CurrentState.AnimFrameIndex > 9 && CurrentState.AnimFrameIndex < 20))
	{
		if (CurrentState.PosePlayTime == 0)
		{
			if (CurrentState.SpecialVariables[BreakCharge] < 100)
				CurrentState.SpecialVariables[BreakCharge] += 20;
			else
				CurrentState.SpecialVariables[BreakCharge] = 100;
		}
	}
	else if (CurrentState.SpecialVariables[BreakCharge] != 0)
	{
		CurrentState.SpecialVariables[BreakCharge] -= 5;
		if (CurrentState.SpecialVariables[BreakCharge] <= 0)
			CurrentState.SpecialVariables[BreakCharge] = 0;
	}

	if (IsCurrentAnimation(TowerLeap) && CurrentState.AnimFrameIndex == 4 && CurrentState.PosePlayTime == 0)
	{
		if (CurrentState.SpecialVariables[MTowerLeap])
		{
			CurrentState.Velocity = FVector2D(3.5, 3.5);
		}
		else
		{
			CurrentState.Velocity = FVector2D(2.35, 4.5);
		}

		if (CurrentState.Dir4 == DirInputTime)
		{
			CurrentState.Velocity.X -= 1;
		}
		else if (CurrentState.Dir6 == DirInputTime)
		{
			CurrentState.Velocity.X += .5;

			if (CurrentState.SpecialVariables[MTowerLeap])
			{
				CurrentState.Velocity.X += .5f;
				CurrentState.Velocity.Y -= .5f;
			}
		}

		if (!CurrentState.bFacingRight)
			CurrentState.Velocity.X *= -1;
	}
}

void ABTCharacterACH::CreateMaterials()
{
	ABTCharacterBase::CreateMaterials();

	if (Main != nullptr)
		DynamicBodyMain = UMaterialInstanceDynamic::Create(Main, this);

	if (Metallic != nullptr)
	{
		DynamicBodyMetallic = UMaterialInstanceDynamic::Create(Metallic, this);
		DynamicSpearMetallic = UMaterialInstanceDynamic::Create(Metallic, this);
	}

	if (Specular != nullptr)
	{
		DynamicBodySpec = UMaterialInstanceDynamic::Create(Specular, this);
		DynamicSpearSpec = UMaterialInstanceDynamic::Create(Specular, this);
	}

	if (Seals != nullptr)
		DynamicSeals = UMaterialInstanceDynamic::Create(Seals, this);

	if (RightEye != nullptr)
		DynamicRightEye = UMaterialInstanceDynamic::Create(RightEye, this);

	if (SpearEdge != nullptr)
		DynamicSpearEdge = UMaterialInstanceDynamic::Create(SpearEdge, this);

	//set texture parameters and set materials
	if (DynamicOutline != nullptr)
		BaseMesh->SetMaterial(3, DynamicOutline);

	if (DynamicBodyMain != nullptr)
	{
		BaseMesh->SetMaterial(0, DynamicBodyMain);

		if (BodyBC != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodyILM != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicBodySpec != nullptr)
	{
		BaseMesh->SetMaterial(1, DynamicBodySpec);

		if (BodyBC != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodyILM != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicBodyMetallic != nullptr)
	{
		BaseMesh->SetMaterial(2, DynamicBodyMetallic);

		if (BodyBC != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodyILM != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicSeals != nullptr)
	{
		BaseMesh->SetMaterial(4, DynamicSeals);

		if (BodyBC != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (SealsBC != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("SealBaseColor"), SealsBC);
		if (BodyILM != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicRightEye != nullptr)
	{
		BaseMesh->SetMaterial(5, DynamicRightEye);

		if (BodyBC != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (AltBodyBC != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("WCBaseColor"), AltBodyBC);
		if (BodyILM != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicSpearSpec != nullptr)
	{
		BaseMesh->SetMaterial(8, DynamicSpearSpec);

		if (SpearBC != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearILM != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("ILM"), SpearILM);
		if (SpearLines != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("Lines"), SpearLines);
		if (SpearSSS != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (DynamicSpearMetallic != nullptr)
	{
		BaseMesh->SetMaterial(9, DynamicSpearMetallic);

		if (SpearBC != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearILM != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("ILM"), SpearILM);
		if (SpearLines != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("Lines"), SpearLines);
		if (SpearSSS != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (DynamicSpearEdge != nullptr)
	{
		BaseMesh->SetMaterial(10, DynamicSpearEdge);

		if (SpearBC != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearILM != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("ILM"), SpearILM);
		if (SpearLines != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("Lines"), SpearLines);
		if (SpearSSS != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (DynamicEyeShine)
		BaseMesh->SetMaterial(6, DynamicEyeShine);

}

void ABTCharacterACH::SetColor(uint8 ColorID)
{
	ABTCharacterBase::SetColor(ColorID);

	struct FACHFilePaths
	{
		TCHAR* BodyBaseColor;
		TCHAR* BodyShading;
		TCHAR* SpearBaseColor;
		TCHAR* SpearShading;
		TCHAR* SealBaseColor;
		TCHAR* AltBodyBaseColor;
	};

	FACHFilePaths TexPaths;

	switch (ColorID) //set parameter colors for character, blitzimage, and sigil based on colorID
	{
	case 1:
		TexPaths.BodyBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_01_BC.T_ACH_01_BC'");
		TexPaths.BodyShading = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_01_SSS.T_ACH_01_SSS'");
		TexPaths.SpearBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Spear_01_BC.T_ACH_Spear_01_BC'");
		TexPaths.SpearShading = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Spear_01_SSS.T_ACH_Spear_01_SSS'");
		TexPaths.SealBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Seal_01_BC.T_ACH_Seal_01_BC'");
		TexPaths.AltBodyBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_WC_01_BC.T_ACH_WC_01_BC'");
		Sigils[0]->SigilColor = FVector(1, .25f, .85f); //Change sigil colors based on color id
		Sigils[1]->SigilColor = Sigils[0]->SigilColor;
		Sigils[0]->EchoColor = FVector(1, .1f, 1);
		Sigils[1]->EchoColor = Sigils[0]->EchoColor;
		//BlitzImage->BlitzColor = FVector(1, .3f, 1);
		break;

	default:
		TexPaths.BodyBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_01_BC.T_ACH_01_BC'");
		TexPaths.BodyShading = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_01_SSS.T_ACH_01_SSS'");
		TexPaths.SpearBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Spear_01_BC.T_ACH_Spear_01_BC'");
		TexPaths.SpearShading = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Spear_01_SSS.T_ACH_Spear_01_SSS'");
		TexPaths.SealBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Seal_01_BC.T_ACH_Seal_01_BC'");
		TexPaths.AltBodyBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_WC_01_BC.T_ACH_WC_01_BC'");
		Sigils[0]->SigilColor = FVector(1, .25f, 1); //Change sigil colors based on color id
		Sigils[1]->SigilColor = Sigils[0]->SigilColor;
		Sigils[0]->EchoColor = FVector(1, 0, 1);
		Sigils[1]->EchoColor = Sigils[0]->EchoColor;
		break;
	}

	BodyBC = LoadObject<UTexture>(NULL, TexPaths.BodyBaseColor);
	BodySSS = LoadObject<UTexture>(NULL, TexPaths.BodyShading);
	SpearBC = LoadObject<UTexture>(NULL, TexPaths.SpearBaseColor);
	SpearSSS = LoadObject<UTexture>(NULL, TexPaths.SpearShading);
	SealsBC = LoadObject<UTexture>(NULL, TexPaths.SealBaseColor);
	AltBodyBC = LoadObject<UTexture>(NULL, TexPaths.AltBodyBaseColor);

	//set texture parameters

	if (DynamicBodyMain != nullptr)
	{
		if (BodyBC != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodySSS != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicBodySpec != nullptr)
	{
		if (BodyBC != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodySSS != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicBodyMetallic != nullptr)
	{
		if (BodyBC != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodySSS != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicSeals != nullptr)
	{
		if (BodyBC != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (SealsBC != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("SealBaseColor"), SealsBC);
		if (BodySSS != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicRightEye != nullptr)
	{
		if (BodyBC != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (AltBodyBC != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("WCBaseColor"), AltBodyBC);
		if (BodySSS != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicSpearSpec != nullptr)
	{
		if (SpearBC != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearSSS != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (DynamicSpearMetallic != nullptr)
	{
		if (SpearBC != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearSSS != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (DynamicSpearEdge != nullptr)
	{
		if (SpearBC != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearILM != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("ILM"), SpearILM);
		if (SpearLines != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("Lines"), SpearLines);
		if (SpearSSS != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (BlitzImage != nullptr)
	{
		ABlitzImageACH* ACHBlitz = Cast<ABlitzImageACH>(BlitzImage);

		if (ACHBlitz != nullptr)
		{
			if (ACHBlitz->DynamicBodyMaterial)
			{
				if (BodyBC != nullptr)
					ACHBlitz->DynamicBodyMaterial->SetTextureParameterValue(FName("BaseColor"), BodyBC);
				if (BodyILM != nullptr)
					ACHBlitz->DynamicBodyMaterial->SetTextureParameterValue(FName("ILM"), BodyILM);
				if (BodyLines != nullptr)
					ACHBlitz->DynamicBodyMaterial->SetTextureParameterValue(FName("Lines"), BodyLines);
				if (BodySSS != nullptr)
					ACHBlitz->DynamicBodyMaterial->SetTextureParameterValue(FName("SSS"), BodySSS);
			}

			if (ACHBlitz->DynamicSpearMaterial)
			{
				if (SpearBC != nullptr)
					ACHBlitz->DynamicSpearMaterial->SetTextureParameterValue(FName("BaseColor"), SpearBC);
				if (SpearILM != nullptr)
					ACHBlitz->DynamicSpearMaterial->SetTextureParameterValue(FName("ILM"), SpearILM);
				if (SpearLines != nullptr)
					ACHBlitz->DynamicSpearMaterial->SetTextureParameterValue(FName("Lines"), SpearLines);
				if (SpearSSS != nullptr)
					ACHBlitz->DynamicSpearMaterial->SetTextureParameterValue(FName("SSS"), SpearSSS);
			}

			if (ACHBlitz->DynamicSealMaterial)
			{
				if (SealsBC != nullptr)
					ACHBlitz->DynamicSealMaterial->SetTextureParameterValue(FName("SealBaseColor"), SealsBC);
				if (BodyBC != nullptr)
					ACHBlitz->DynamicSealMaterial->SetTextureParameterValue(FName("BaseColor"), BodyBC);
				if (BodyILM != nullptr)
					ACHBlitz->DynamicSealMaterial->SetTextureParameterValue(FName("ILM"), BodyILM);
				if (BodyLines != nullptr)
					ACHBlitz->DynamicSealMaterial->SetTextureParameterValue(FName("Lines"), BodyLines);
				if (BodySSS != nullptr)
					ACHBlitz->DynamicSealMaterial->SetTextureParameterValue(FName("SSS"), BodySSS);
			}
		}
	}
}

void ABTCharacterACH::LightSettings()
{
	ABTCharacterBase::LightSettings();

	FVector MainLightValue = MainLight->GetComponentLocation() - GetActorLocation();
	FVector FillLightValue = FillLight->GetComponentLocation() - GetActorLocation();

	if (DynamicBodyMain)
	{
		DynamicBodyMain->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
		DynamicBodyMain->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
		DynamicBodyMain->SetVectorParameterValue(FName("MainLightColor"), CurrentState.MainLightColor);
		DynamicBodyMain->SetVectorParameterValue(FName("FillLightColor"), CurrentState.FillLightColor);
		DynamicBodyMain->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);
		DynamicBodyMain->SetScalarParameterValue(FName("LightIntensity"), CurrentState.LightIntensity);
	}
	if (DynamicBodyMetallic)
	{
		DynamicBodyMetallic->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
		DynamicBodyMetallic->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
		DynamicBodyMetallic->SetVectorParameterValue(FName("MainLightColor"), CurrentState.MainLightColor);
		DynamicBodyMetallic->SetVectorParameterValue(FName("FillLightColor"), CurrentState.FillLightColor);
		DynamicBodyMetallic->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);
		DynamicBodyMetallic->SetScalarParameterValue(FName("LightIntensity"), CurrentState.LightIntensity);
	}
		
	if (DynamicBodySpec)
	{
		DynamicBodySpec->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
		DynamicBodySpec->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
		DynamicBodySpec->SetVectorParameterValue(FName("MainLightColor"), CurrentState.MainLightColor);
		DynamicBodySpec->SetVectorParameterValue(FName("FillLightColor"), CurrentState.FillLightColor);
		DynamicBodySpec->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);
		DynamicBodySpec->SetScalarParameterValue(FName("LightIntensity"), CurrentState.LightIntensity);
	}
	if (DynamicSeals)
	{
		DynamicSeals->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
		DynamicSeals->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
		DynamicSeals->SetVectorParameterValue(FName("MainLightColor"), CurrentState.MainLightColor);
		DynamicSeals->SetVectorParameterValue(FName("FillLightColor"), CurrentState.FillLightColor);
		DynamicSeals->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);
		DynamicSeals->SetScalarParameterValue(FName("LightIntensity"), CurrentState.LightIntensity);
	}
	if (DynamicRightEye)
	{
		DynamicRightEye->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
		DynamicRightEye->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
		DynamicRightEye->SetVectorParameterValue(FName("MainLightColor"), CurrentState.MainLightColor);
		DynamicRightEye->SetVectorParameterValue(FName("FillLightColor"), CurrentState.FillLightColor);
		DynamicRightEye->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);
		DynamicRightEye->SetScalarParameterValue(FName("LightIntensity"), CurrentState.LightIntensity);
	}
	if (DynamicSpearMetallic)
	{
		DynamicSpearMetallic->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
		DynamicSpearMetallic->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
		DynamicSpearMetallic->SetVectorParameterValue(FName("MainLightColor"), CurrentState.MainLightColor);
		DynamicSpearMetallic->SetVectorParameterValue(FName("FillLightColor"), CurrentState.FillLightColor);
		DynamicSpearMetallic->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);
		DynamicSpearMetallic->SetScalarParameterValue(FName("LightIntensity"), CurrentState.LightIntensity);
	}
	if (DynamicSpearSpec)
	{
		DynamicSpearSpec->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
		DynamicSpearSpec->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
		DynamicSpearSpec->SetVectorParameterValue(FName("MainLightColor"), CurrentState.MainLightColor);
		DynamicSpearSpec->SetVectorParameterValue(FName("FillLightColor"), CurrentState.FillLightColor);
		DynamicSpearSpec->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);
		DynamicSpearSpec->SetScalarParameterValue(FName("LightIntensity"), CurrentState.LightIntensity);
	}
	if (DynamicSpearEdge)
	{
		DynamicSpearEdge->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
		DynamicSpearEdge->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
		DynamicSpearEdge->SetVectorParameterValue(FName("MainLightColor"), CurrentState.MainLightColor);
		DynamicSpearEdge->SetVectorParameterValue(FName("FillLightColor"), CurrentState.FillLightColor);
		DynamicSpearEdge->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);
		DynamicSpearEdge->SetScalarParameterValue(FName("LightIntensity"), CurrentState.LightIntensity);
	}	
}

void ABTCharacterACH::SpawnPBS()
{
	ABTCharacterBase::SpawnPBS();

	//spawn unique projectiles to character and save their states to character state
}

void ABTCharacterACH::ResetSmear()
{
	ABTCharacterBase::ResetSmear();

	SmearMesh->SetMorphTarget(TEXT("ACH_5H"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_2H_00"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_2H_01"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_5B"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_2B_00"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_2B_01"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_6B"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_jH_00"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_jH_01"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_jB_00"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_jB_01"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_jB_02"), 0);
	SmearMesh->SetMorphTarget(TEXT("ACH_Throw"), 0);
}

void ABTCharacterACH::DrawSmear()
{
	ResetSmear();
	
	if (IsCurrentAnimation(NormalThrow) && CurrentState.AnimFrameIndex > 17 && CurrentState.AnimFrameIndex < 22)
	{
		FVector SmearFrameIndex = FVector(.5, 0, 0);
		FVector EmitFrameIndex = FVector((CurrentState.AnimFrameIndex - 18) % 2, (CurrentState.AnimFrameIndex - 18) / 2, 0);

		SmearMesh->SetMorphTarget(TEXT("ACH_Throw"), 1);

		if (DynamicSmear)
		{
			DynamicSmear->SetVectorParameterValue(TEXT("RowsAndColumns"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionRC"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("AnimIndex"), SmearFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionAnimIndex"), EmitFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("BodyEmissiveColor"), EffectColor);
			DynamicSmear->SetScalarParameterValue(TEXT("BodyEmissivity"), 10);

			if (SmearEmit)
				DynamicSmear->SetTextureParameterValue(TEXT("SpriteSheet"), SmearBody);
			if (SmearBody)
				DynamicSmear->SetTextureParameterValue(TEXT("EmissionSpriteSheet"), SmearEmit);
		}
	}
	else if (IsCurrentAnimation(Normal5B) && CurrentState.AnimFrameIndex > 2 && CurrentState.AnimFrameIndex < 7)
	{
		FVector SmearFrameIndex = FVector(0);
		FVector EmitFrameIndex = FVector(FMath::Min(1, (CurrentState.AnimFrameIndex - 3) % 2), FMath::Min(1, (CurrentState.AnimFrameIndex - 3) / 2), 0);

		SmearMesh->SetMorphTarget(TEXT("ACH_5B"), 1);

		if (CurrentState.AnimFrameIndex > 3)
		{
			if (CurrentState.AnimFrameIndex == 4)
				SmearFrameIndex.X = 1;
			else if (CurrentState.AnimFrameIndex == 5)
			{
				if (CurrentState.PosePlayTime < 2)
					SmearFrameIndex.Y = 1;
				else
				{
					SmearFrameIndex.X = 1;
					SmearFrameIndex.Y = 1;
				}
			}
			else
				SmearFrameIndex.X = .5f;
		}

		if (DynamicSmear)
		{
			DynamicSmear->SetVectorParameterValue(TEXT("RowsAndColumns"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionRC"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("AnimIndex"), SmearFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionAnimIndex"), EmitFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("BodyEmissiveColor"), EffectColor);
			DynamicSmear->SetScalarParameterValue(TEXT("BodyEmissivity"), 10);

			if (SmearEmit)
				DynamicSmear->SetTextureParameterValue(TEXT("SpriteSheet"), SmearBody);
			if (SmearBody)
				DynamicSmear->SetTextureParameterValue(TEXT("EmissionSpriteSheet"), SmearEmit);
		}
	}
	else if (IsCurrentAnimation(Normal2B) && CurrentState.AnimFrameIndex > 2 && CurrentState.AnimFrameIndex < 11)
	{
		FVector SmearFrameIndex = FVector(0);
		FVector EmitFrameIndex = FVector(0);

		if (CurrentState.AnimFrameIndex < 7)
			SmearMesh->SetMorphTarget(TEXT("ACH_2B_00"), 1);
		else
			SmearMesh->SetMorphTarget(TEXT("ACH_2B_01"), 1);

		if (CurrentState.AnimFrameIndex < 7)
		{
			if (CurrentState.AnimFrameIndex == 4)
			{
				SmearFrameIndex.X = 1;
				EmitFrameIndex.X = 1;
			}
			else if (CurrentState.AnimFrameIndex == 5)
			{
				SmearFrameIndex.Y = 1;
				EmitFrameIndex.Y = 1;
			}
			else if (CurrentState.AnimFrameIndex == 6)
			{
				SmearFrameIndex = FVector(1);
				EmitFrameIndex.Y = 1;
			}
		}
		else
		{
			if (CurrentState.AnimFrameIndex == 8)
			{
				SmearFrameIndex.X = 1;
				EmitFrameIndex.X = 1;
			}
			else if (CurrentState.AnimFrameIndex == 9)
			{
				SmearFrameIndex.Y = 1;
				EmitFrameIndex.Y = 1;
			}
			else if (CurrentState.AnimFrameIndex == 10)
			{
				SmearFrameIndex = FVector(1);
				if (CurrentState.PosePlayTime < 2)
					EmitFrameIndex.Y = 1;
				else
					EmitFrameIndex = FVector(1);
			}
		}

		if (DynamicSmear)
		{
			DynamicSmear->SetVectorParameterValue(TEXT("RowsAndColumns"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionRC"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("AnimIndex"), SmearFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionAnimIndex"), EmitFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("BodyEmissiveColor"), EffectColor);
			DynamicSmear->SetScalarParameterValue(TEXT("BodyEmissivity"), 10);

			if (SmearEmit)
				DynamicSmear->SetTextureParameterValue(TEXT("SpriteSheet"), SmearBody);
			if (SmearBody)
				DynamicSmear->SetTextureParameterValue(TEXT("EmissionSpriteSheet"), SmearEmit);
		}
	}
	else if (IsCurrentAnimation(Normal6B) && CurrentState.AnimFrameIndex > 7 && CurrentState.AnimFrameIndex < 12)
	{
		FVector SmearFrameIndex = FVector(.5f, 0, 0);
		FVector EmitFrameIndex = FVector(FMath::Min(1, (CurrentState.AnimFrameIndex - 8) % 2), FMath::Min(1, (CurrentState.AnimFrameIndex - 8) / 2), 0);

		SmearMesh->SetMorphTarget(TEXT("ACH_6B"), 1);

		if (CurrentState.AnimFrameIndex == 8)
			SmearFrameIndex = FVector(0);
		else if (CurrentState.AnimFrameIndex == 9 && CurrentState.PosePlayTime < 3)
			SmearFrameIndex.X = 1;
		else if (CurrentState.AnimFrameIndex == 9 || (CurrentState.AnimFrameIndex == 10 && CurrentState.PosePlayTime < 2))
			SmearFrameIndex = FVector(0, 1, 0);
		else if (CurrentState.AnimFrameIndex == 10)
			SmearFrameIndex = FVector(1);

		if (DynamicSmear)
		{
			DynamicSmear->SetVectorParameterValue(TEXT("RowsAndColumns"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionRC"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("AnimIndex"), SmearFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionAnimIndex"), EmitFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("BodyEmissiveColor"), EffectColor);
			DynamicSmear->SetScalarParameterValue(TEXT("BodyEmissivity"), 10);

			if (SmearEmit)
				DynamicSmear->SetTextureParameterValue(TEXT("SpriteSheet"), SmearBody);
			if (SmearBody)
				DynamicSmear->SetTextureParameterValue(TEXT("EmissionSpriteSheet"), SmearEmit);
		}
	}
	else if (IsCurrentAnimation(Normal5H) && CurrentState.AnimFrameIndex > 2 && CurrentState.AnimFrameIndex < 7)
	{
		FVector SmearFrameIndex = FVector(0);
		FVector EmitFrameIndex = FVector((CurrentState.AnimFrameIndex - 3) % 2, (CurrentState.AnimFrameIndex - 3) / 2, 0);

		SmearMesh->SetMorphTarget(TEXT("ACH_5H"), 1);

		if (CurrentState.AnimFrameIndex > 3)
		{
			if (CurrentState.AnimFrameIndex == 4)
				SmearFrameIndex.X = 1;
			else if (CurrentState.AnimFrameIndex == 5)
			{
				SmearFrameIndex.X = 1;
				SmearFrameIndex.Y = 1;
			}
			else
			{
				SmearFrameIndex.X = .5f;
				SmearFrameIndex.Y = 0;
			}
		}

		if (DynamicSmear)
		{
			DynamicSmear->SetVectorParameterValue(TEXT("RowsAndColumns"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionRC"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("AnimIndex"), SmearFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionAnimIndex"), EmitFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("BodyEmissiveColor"), FVector(1));
			DynamicSmear->SetScalarParameterValue(TEXT("BodyEmissivity"), 1);

			if (SmearEmit)
				DynamicSmear->SetTextureParameterValue(TEXT("SpriteSheet"), SmearBody);
			if (SmearBody)
				DynamicSmear->SetTextureParameterValue(TEXT("EmissionSpriteSheet"), SmearEmit);
		}
	}
	else if (IsCurrentAnimation(Normal2H) && CurrentState.AnimFrameIndex > 2 && CurrentState.AnimFrameIndex < 8)
	{
		FVector SmearFrameIndex = FVector(0);
		FVector EmitFrameIndex;

		if (CurrentState.AnimFrameIndex == 3)
			SmearMesh->SetMorphTarget(TEXT("ACH_2H_00"), 1);
		else
			SmearMesh->SetMorphTarget(TEXT("ACH_2H_01"), 1);

		if (CurrentState.AnimFrameIndex == 3)
		{
			EmitFrameIndex.X = 1;
			EmitFrameIndex.Y = 1;
			SmearFrameIndex.X = 1;
			SmearFrameIndex.Y = 1;
		}
		else
		{
			if (CurrentState.AnimFrameIndex == 4)
				EmitFrameIndex = FVector(0);
			else if ((CurrentState.AnimFrameIndex == 5 && CurrentState.PosePlayTime < 3))
			{
				SmearFrameIndex = FVector(1, 0, 0);
				EmitFrameIndex = FVector(0, 0, 0);
			}
			else if ((CurrentState.AnimFrameIndex == 5) || (CurrentState.AnimFrameIndex == 6 && CurrentState.PosePlayTime < 1))
			{
				SmearFrameIndex = FVector(1, 1, 0);
				EmitFrameIndex = FVector(1, 0, 0);
			}
			else if (CurrentState.AnimFrameIndex == 6)
			{
				SmearFrameIndex = FVector(.5, 0, 0);
				EmitFrameIndex = FVector(0, 1, 0);
			}
			else if (CurrentState.AnimFrameIndex == 7 && CurrentState.PosePlayTime < 2)
			{
				SmearFrameIndex = FVector(.5, 0, 0);
				EmitFrameIndex = FVector(1, 1, 0);
			}
			else
			{
				SmearFrameIndex = FVector(.5, 0, 0);
				EmitFrameIndex = FVector(.5, 0, 0);
			}

		}

		if (DynamicSmear)
		{
			DynamicSmear->SetVectorParameterValue(TEXT("RowsAndColumns"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionRC"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("AnimIndex"), SmearFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionAnimIndex"), EmitFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("BodyEmissiveColor"), FVector(1));
			DynamicSmear->SetScalarParameterValue(TEXT("BodyEmissivity"), 1);

			if (SmearEmit)
				DynamicSmear->SetTextureParameterValue(TEXT("SpriteSheet"), SmearBody);
			if (SmearBody)
				DynamicSmear->SetTextureParameterValue(TEXT("EmissionSpriteSheet"), SmearEmit);
		}
	}
	else if (IsCurrentAnimation(NormalJH) && CurrentState.AnimFrameIndex > 1 && CurrentState.AnimFrameIndex < 7)
	{
		FVector SmearFrameIndex = FVector(0);
		FVector EmitFrameIndex = FVector(0);

		if (CurrentState.AnimFrameIndex < 4)
			SmearMesh->SetMorphTarget(TEXT("ACH_jH_00"), 1);
		else
			SmearMesh->SetMorphTarget(TEXT("ACH_jH_01"), 1);

		if (CurrentState.AnimFrameIndex >= 4)
		{
			SmearFrameIndex = FVector(.5, 0, 0);

			if (CurrentState.AnimFrameIndex == 4)
			{
				SmearFrameIndex = FVector(0, 1, 0);
				EmitFrameIndex = FVector(1, 0, 0);
			}
			else if (CurrentState.AnimFrameIndex == 5)
				EmitFrameIndex = FVector(0, 1, 0);
			else
				EmitFrameIndex = FVector(1, 1, 0);
		}

		if (DynamicSmear)
		{
			DynamicSmear->SetVectorParameterValue(TEXT("RowsAndColumns"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionRC"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("AnimIndex"), SmearFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionAnimIndex"), EmitFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("BodyEmissiveColor"), FVector(1));
			DynamicSmear->SetScalarParameterValue(TEXT("BodyEmissivity"), 1);

			if (SmearEmit)
				DynamicSmear->SetTextureParameterValue(TEXT("SpriteSheet"), SmearBody);
			if (SmearBody)
				DynamicSmear->SetTextureParameterValue(TEXT("EmissionSpriteSheet"), SmearEmit);
		}
	}
	else if (IsCurrentAnimation(NormalJB) && CurrentState.AnimFrameIndex > 4)
	{
		FVector SmearFrameIndex = FVector(0);
		FVector EmitFrameIndex = FVector(0);

		if (CurrentState.AnimFrameIndex == 5)
			SmearMesh->SetMorphTarget(TEXT("ACH_jB_00"), 1);
		else if (CurrentState.AnimFrameIndex == 6)
			SmearMesh->SetMorphTarget(TEXT("ACH_jB_01"), 1);
		else
			SmearMesh->SetMorphTarget(TEXT("ACH_jB_02"), 1);

		if (CurrentState.AnimFrameIndex > 6)
		{
			SmearFrameIndex.X = .5;

			if (CurrentState.AnimFrameIndex == 7)
			{
				EmitFrameIndex.Y = 1;
				SmearFrameIndex = FVector(1);
			}
			else if (CurrentState.AnimFrameIndex == 8)
			{
				EmitFrameIndex = FVector(1);
			}
			else
			{
				EmitFrameIndex.X = .5;
			}
		}

		if (DynamicSmear)
		{
			DynamicSmear->SetVectorParameterValue(TEXT("RowsAndColumns"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionRC"), FVector(2));
			DynamicSmear->SetVectorParameterValue(TEXT("AnimIndex"), SmearFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("EmissionAnimIndex"), EmitFrameIndex);
			DynamicSmear->SetVectorParameterValue(TEXT("BodyEmissiveColor"), EffectColor);
			DynamicSmear->SetScalarParameterValue(TEXT("BodyEmissivity"), 10);

			if (SmearEmit)
				DynamicSmear->SetTextureParameterValue(TEXT("SpriteSheet"), SmearBody);
			if (SmearBody)
				DynamicSmear->SetTextureParameterValue(TEXT("EmissionSpriteSheet"), SmearEmit);
		}
	}
	else
		bShowSmear = false;

	ABTCharacterBase::DrawSmear();
}

bool ABTCharacterACH::NormalAttacks()
{
	//Command Normals
	if (CurrentState.AvailableActions & AcceptCommandNormal)
	{
		if (!CurrentState.bIsAirborne)
		{
			if (CurrentState.Dir6 == DirInputTime)
			{
				if(CurrentState.BPressed > 0 && (CurrentState.MoveList & n6B) == 0)
				{
					CurrentState.BPressed = 0;
					CurrentState.MoveList |= n6B;
					TurnAroundCheck();
					return EnterNewAnimation(Normal6B);
				}
				if (CurrentState.MPressed > 0 && (CurrentState.MoveList & n6M) == 0)
				{
					CurrentState.MPressed = 0;
					CurrentState.MoveList |= n6M;
					TurnAroundCheck();
					return EnterNewAnimation(Normal6M);
				}
				if (CurrentState.LPressed > 0 && (CurrentState.MoveList & n6L) == 0)
				{
					CurrentState.LPressed = 0;
					CurrentState.MoveList |= n6L;
					TurnAroundCheck();
					return EnterNewAnimation(Normal6L);
				}
			}
		}
	}
	//Normal Attacks
	if (CurrentState.bIsAirborne)
	{
		if (CurrentState.BPressed > 0 && (CurrentState.AvailableActions & AcceptBreak))
		{

			if ((CurrentState.MoveList & nJB) == 0)
			{
				CurrentState.BPressed = 0;
				CurrentState.MoveList |= nJB;
				return EnterNewAnimation(NormalJB);
			}
		}
		if (CurrentState.HPressed > 0 && (CurrentState.AvailableActions & AcceptHeavy))
		{
			if ((CurrentState.MoveList & nJH) == 0)
			{
				CurrentState.HPressed = 0;
				CurrentState.MoveList |= nJH;
				return EnterNewAnimation(NormalJH);
			}
		}
		if (CurrentState.MPressed > 0 && (CurrentState.AvailableActions & AcceptMedium))
		{
			if ((CurrentState.MoveList & nJM) == 0)
			{
				CurrentState.MPressed = 0;
				CurrentState.MoveList |= nJM;
				return EnterNewAnimation(NormalJM);
			}
		}
		if (CurrentState.LPressed > 0 && (CurrentState.AvailableActions & AcceptLight))
		{
			CurrentState.LPressed = 0;
			return EnterNewAnimation(NormalJL);
		}
	}
	else
	{
		if (CurrentState.Dir1 == DirInputTime || CurrentState.Dir2 == DirInputTime || CurrentState.Dir3 == DirInputTime) // holding the down direction
		{
			if (CurrentState.BPressed > 0 && (CurrentState.AvailableActions & AcceptBreak))
			{
				if ((CurrentState.MoveList & n2B) == 0)
				{
					CurrentState.BPressed = 0;
					CurrentState.MoveList |= n2B;
					TurnAroundCheck();
					return EnterNewAnimation(Normal2B);
				}
			}
			if (CurrentState.HPressed > 0 && (CurrentState.AvailableActions & AcceptHeavy))
			{
				if ((CurrentState.MoveList & n2H) == 0)
				{
					CurrentState.HPressed = 0;
					CurrentState.MoveList |= n2H;
					TurnAroundCheck();
					return EnterNewAnimation(Normal2H);
				}
			}
			if (CurrentState.MPressed > 0 && (CurrentState.AvailableActions & AcceptMedium))
			{
				if ((CurrentState.MoveList & n2M) == 0)
				{
					CurrentState.MPressed = 0;
					CurrentState.MoveList |= n2M;
					TurnAroundCheck();
					return EnterNewAnimation(Normal2M);
				}
			}
			if (CurrentState.LPressed > 0 && (CurrentState.AvailableActions & AcceptLight))
			{
				CurrentState.LPressed = 0;
				TurnAroundCheck();
				return EnterNewAnimation(Normal2L);
			}
		}
		else //otherwise
		{
			if (CurrentState.BPressed > 0 && (CurrentState.AvailableActions & AcceptBreak))
			{
				if(CurrentState.Dir6 == DirInputTime && (CurrentState.MoveList & n6B) == 0)
				{
					CurrentState.BPressed = 0;
					CurrentState.MoveList |= n6B;
					TurnAroundCheck();
					return EnterNewAnimation(Normal6B);
				}
				if ((CurrentState.MoveList & n5B) == 0)
				{
					CurrentState.BPressed = 0;
					CurrentState.MoveList |= n5B;
					TurnAroundCheck();
					return EnterNewAnimation(Normal5B);
				}
			}
			if (CurrentState.HPressed > 0 && (CurrentState.AvailableActions & AcceptHeavy))
			{
				if ((CurrentState.MoveList & n5H) == 0)
				{
					CurrentState.HPressed = 0;
					CurrentState.MoveList |= n5H;
					TurnAroundCheck();
					return EnterNewAnimation(Normal5H);
				}
			}
			if (CurrentState.MPressed > 0 && (CurrentState.AvailableActions & AcceptMedium))
			{
				if (CurrentState.Dir6 == DirInputTime && (CurrentState.MoveList & n6M) == 0)
				{
					CurrentState.MPressed = 0;
					CurrentState.MoveList |= n6M;
					TurnAroundCheck();
					return EnterNewAnimation(Normal6M);
				}
				if ((CurrentState.MoveList & n5M) == 0)
				{
					CurrentState.MPressed = 0;
					CurrentState.MoveList |= n5M;
					TurnAroundCheck();
					return EnterNewAnimation(Normal5M);
				}
			}
			if (CurrentState.LPressed > 0 && (CurrentState.AvailableActions & AcceptLight))
			{
				if (CurrentState.Dir6 == DirInputTime && (CurrentState.MoveList & n6L) == 0)
				{
					CurrentState.LPressed = 0;
					CurrentState.MoveList |= n6L;
					TurnAroundCheck();
					return EnterNewAnimation(Normal6L);
				}

				CurrentState.LPressed = 0;
				TurnAroundCheck();
				return EnterNewAnimation(Normal5L);
			}
		}
	}
	return ABTCharacterBase::NormalAttacks();
}

bool ABTCharacterACH::SpecialAttacks()
{
	//Special Attacks
	if (CurrentState.AvailableActions & AcceptSpecial)
	{
		if (!CurrentState.bIsAirborne)
		{
			if ((CurrentState.QCB > 0 && ((CurrentState.LPressed > 0 && CurrentState.QCB <= CurrentState.LPressed) || CurrentState.MPressed >= 0 && CurrentState.QCB <= CurrentState.MPressed)) && !(CurrentState.MoveList & SpTowerLeap))
			{
				CurrentState.MoveList |= SpTowerLeap;

				CurrentState.SpecialVariables[MTowerLeap] = (CurrentState.MPressed >= CurrentState.LPressed);

				CurrentState.QCB = 0;
				CurrentState.LPressed = 0;
				CurrentState.MPressed = 0;
				return EnterNewAnimation(TowerLeap);
			}
		}
	}

	return ABTCharacterBase::SpecialAttacks();
}

bool ABTCharacterACH::SuperAttacks()
{
	//Break Triggers/Supers
	return ABTCharacterBase::SuperAttacks();
}

void ABTCharacterACH::CreateVariables()
{
	for (uint8 i = 0; i < 4; i++)
	{
		CurrentState.SpecialVariables.Add(0);
	}
}
