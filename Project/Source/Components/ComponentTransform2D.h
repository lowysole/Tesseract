#pragma once

#include "Component.h"
#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

class ComponentTransform2D : public Component {
public:
	REGISTER_COMPONENT(ComponentTransform2D, ComponentType::TRANSFORM2D);
	void Update() override;
	void OnEditorUpdate() override;

	void SetPosition(float3 position);
	void SetSize(float2 size);
	void SetRotation(Quat rotation);
	void SetScale(float3 scale);
	void SetAnchorX(float2 anchorX);
	void SetAnchorY(float2 anchorY);
	const float4x4 GetGlobalMatrix();

	float3 GetPosition() const;
	float2 GetSize() const;
	float3 GetScale() const;

	void InvalidateHierarchy();
	void Invalidate();

private:
	bool dirty = true;
	float3 position = float3::zero; // The offset position
	float2 pivot = float2(0.5, 0.5); // The position of the pivot
	float2 size = float2::one; // The size of the item

	Quat rotation = Quat::identity; // The rotation of the element
	float3 scale = float3::one; // The scale of the element

	float2 anchorX = float2::zero; // The Anchor of X axis. AnchorX.x -> Min position, AnchorX.y -> Max position
	float2 anchorY = float2::zero; // The Anchor of Y axis. AnchorY.x -> Min position, AnchorY.y -> Max position
};
