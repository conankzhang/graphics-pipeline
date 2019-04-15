// Includes
//=========

#include "cRenderSystem.h"

#include "cCameraComponent.h"
#include "cRenderComponent.h"

#include <Engine/Graphics/Graphics.h>
#include <Engine/Math/cMatrix_transformation.h>

// Initialization / Clean Up
//--------------------------

eae6320::cRenderSystem::cRenderSystem()
{
}

eae6320::cRenderSystem::~cRenderSystem()
{

}

void eae6320::cRenderSystem::Initialize()
{
	m_cameraComponent = *m_componentManager->begin<cCameraComponent>();
}

// Implementation
//===============

void eae6320::cRenderSystem::Update(float deltaTime)
{
	m_cameraComponent->Update(deltaTime);

	for (auto renderComponent = m_componentManager->begin<cRenderComponent>(); renderComponent != m_componentManager->end<cRenderComponent>(); ++renderComponent)
	{
		if (*renderComponent && (*renderComponent)->IsActive())
		{
			(*renderComponent)->Update(deltaTime);
		}
	}
}

void eae6320::cRenderSystem::SubmitDataToBeRendered(const float i_deltaSystemTime, const float i_deltaTime)
{
	Graphics::SubmitCamera(m_cameraComponent->GetWorldToCameraTransform(i_deltaTime), m_cameraComponent->GetCameraToProjectedTransform(), m_cameraComponent->GetPosition(), i_deltaSystemTime, i_deltaTime);

	for (auto renderComponent = m_componentManager->begin<cRenderComponent>(); renderComponent != m_componentManager->end<cRenderComponent>(); ++renderComponent)
	{
		if ((*renderComponent) && (*renderComponent)->IsActive())
		{
		}
	}
}
