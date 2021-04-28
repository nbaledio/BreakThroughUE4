// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BlitzImageACH.h"

void ABlitzImageACH::CreateMaterials()
{
    ABlitzImageBase::CreateMaterials();

    if (BaseMesh != nullptr)
    {
        if (BaseMesh->GetNumMaterials() == 12)
        {
            if (BlitzMaterial != nullptr)
            {
                DynamicBodyMaterial = UMaterialInstanceDynamic::Create(BlitzMaterial, this);
                DynamicSpearMaterial = UMaterialInstanceDynamic::Create(BlitzMaterial, this);
            }

            if (SealBlitzMaterial != nullptr)
                DynamicSealMaterial = UMaterialInstanceDynamic::Create(SealBlitzMaterial, this);

            if (DynamicBodyMaterial != nullptr)
            {
                BaseMesh->SetMaterial(0, DynamicBodyMaterial);
                BaseMesh->SetMaterial(1, DynamicBodyMaterial);
                BaseMesh->SetMaterial(2, DynamicBodyMaterial);
                BaseMesh->SetMaterial(5, DynamicBodyMaterial);
            }

            if (DynamicLineMaterial != nullptr)
            {
                BaseMesh->SetMaterial(3, DynamicLineMaterial);
            }

            if (DynamicSealMaterial != nullptr)
            {
                BaseMesh->SetMaterial(4, DynamicSealMaterial);
            }  

            if (InvisibleMaterial != nullptr)
            {
                BaseMesh->SetMaterial(6, InvisibleMaterial);
                BaseMesh->SetMaterial(7, InvisibleMaterial);
                BaseMesh->SetMaterial(11, InvisibleMaterial);
            }

            if (DynamicSpearMaterial != nullptr)
            {
                BaseMesh->SetMaterial(8, DynamicSpearMaterial);
                BaseMesh->SetMaterial(9, DynamicSpearMaterial);
                BaseMesh->SetMaterial(10, DynamicSpearMaterial);
            }
        }
    }
}

void ABlitzImageACH::DrawBlitz()
{
    ABlitzImageBase::DrawBlitz();

    if (CurrentState.bIsActive)
    {
        DynamicBodyMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.Alpha);
        DynamicSpearMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.Alpha);
        DynamicSealMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.Alpha);

        DynamicBodyMaterial->SetVectorParameterValue(FName("GlowColor"), BlitzColor);
        DynamicSpearMaterial->SetVectorParameterValue(FName("GlowColor"), BlitzColor);
        DynamicSealMaterial->SetVectorParameterValue(FName("GlowColor"), BlitzColor);

        DynamicBodyMaterial->SetVectorParameterValue(FName("MainLightVector"), LightVector);
        DynamicSpearMaterial->SetVectorParameterValue(FName("MainLightVector"), LightVector);
        DynamicSealMaterial->SetVectorParameterValue(FName("MainLightVector"), LightVector);
    }
}
