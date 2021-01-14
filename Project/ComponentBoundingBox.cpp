#include "ComponentBoundingBox.h"

#include "Logging.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "debugdraw.h"
#include "Math/float3x3.h"

#include "Leaks.h"

void ComponentBoundingBox::Update()
{
	CalculateWorldBoundingBox();
}

void ComponentBoundingBox::Save(JsonValue& j_component) const
{
	JsonValue& j_local_bounding_box = j_component["LocalBoundingBox"];
	j_local_bounding_box[0] = local_aabb.minPoint.x;
	j_local_bounding_box[1] = local_aabb.minPoint.y;
	j_local_bounding_box[2] = local_aabb.minPoint.z;
	j_local_bounding_box[3] = local_aabb.maxPoint.x;
	j_local_bounding_box[4] = local_aabb.maxPoint.y;
	j_local_bounding_box[5] = local_aabb.maxPoint.z;
}

void ComponentBoundingBox::Load(const JsonValue& j_component)
{
	const JsonValue& j_local_bounding_box = j_component["LocalBoundingBox"];
	local_aabb.minPoint.Set(j_local_bounding_box[0], j_local_bounding_box[1], j_local_bounding_box[2]);
	local_aabb.maxPoint.Set(j_local_bounding_box[3], j_local_bounding_box[4], j_local_bounding_box[5]);

	dirty = true;
}

void ComponentBoundingBox::SetLocalBoundingBox(const AABB& bounding_box)
{
	local_aabb = bounding_box;
	dirty = true;
}

void ComponentBoundingBox::CalculateWorldBoundingBox(bool force)
{
	if (dirty || force)
	{
		GameObject& owner = GetOwner();
		ComponentTransform* transform = owner.GetComponent<ComponentTransform>();
		float3x3 transformed = transform->GetGlobalMatrix().Float3x3Part();
		world_obb = local_aabb.Transform(transform->GetGlobalMatrix());
		world_aabb = world_obb.MinimalEnclosingAABB();
		dirty = false;
	}
}

void ComponentBoundingBox::DrawBoundingBox()
{
	float3 points[8];

	world_obb.GetCornerPoints(points);
	dd::box(points, dd::colors::White);
}

void ComponentBoundingBox::Invalidate()
{
	dirty = true;
}

const OBB& ComponentBoundingBox::GetWorldOBB() const
{
	return world_obb;
}

const AABB& ComponentBoundingBox::GetWorldAABB() const
{
	return world_aabb;
}