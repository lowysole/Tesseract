#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "GL/glew.h"
#include "Math/float3x3.h"
#include "Math/Quat.h"

ModuleCamera::ModuleCamera()
{
    frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    frustum.SetViewPlaneDistances(0.1f, 200.0f);
    frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);

    frustum.SetPos(float3(0, 1, -2));
    frustum.SetFront(float3::unitZ);
    frustum.SetUp(float3::unitY);
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
    return true;
}

update_status ModuleCamera::PreUpdate()
{
    float4x4 projectionGL = GetProjectionMatrix();

    //Send the frustum projection matrix to OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(*projectionGL.v);

    float4x4 viewGL = GetViewMatrix();

    //Send the frustum view matrix to OpenGL
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(*viewGL.v);

    return UPDATE_CONTINUE;
}

update_status ModuleCamera::Update()
{
    float delta_time = App->GetDeltaTime();
    if (App->input->GetKey(SDL_SCANCODE_W))
    {
        Translate(frustum.Front().Normalized() * movement_speed * delta_time);
    }
    if (App->input->GetKey(SDL_SCANCODE_S))
    {
        Translate(frustum.Front().Normalized() * -movement_speed * delta_time);
    }
    if (App->input->GetKey(SDL_SCANCODE_A))
    {
        Translate(frustum.WorldRight().Normalized() * -movement_speed * delta_time);
    }
    if (App->input->GetKey(SDL_SCANCODE_D))
    {
        Translate(frustum.WorldRight().Normalized() * movement_speed * delta_time);
    }

    if (App->input->GetKey(SDL_SCANCODE_UP))
    {
        Rotate(float3x3::RotateAxisAngle(frustum.WorldRight().Normalized(), rotation_speed * DEGTORAD * delta_time));
    }
    if (App->input->GetKey(SDL_SCANCODE_DOWN))
    {
        Rotate(float3x3::RotateAxisAngle(frustum.WorldRight().Normalized(), -rotation_speed * DEGTORAD * delta_time));
    }
    if (App->input->GetKey(SDL_SCANCODE_LEFT))
    {
        Rotate(float3x3::RotateY(rotation_speed * DEGTORAD * delta_time));
    }
    if (App->input->GetKey(SDL_SCANCODE_RIGHT))
    {
        Rotate(float3x3::RotateY(-rotation_speed * DEGTORAD * delta_time));
    }

    return UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate()
{
    return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
    return true;
}

void ModuleCamera::WindowResized(Sint32 width, Sint32 height)
{
    SetAspectRatio(width / (float) height);
}

void ModuleCamera::SetFOV(float h_fov)
{
    frustum.SetHorizontalFovAndAspectRatio(h_fov, frustum.AspectRatio());
}

void ModuleCamera::SetAspectRatio(float aspect_ratio)
{
    frustum.SetHorizontalFovAndAspectRatio(frustum.HorizontalFov(), aspect_ratio);
}

void ModuleCamera::SetPlaneDistances(float near_plane, float far_plane)
{
    frustum.SetViewPlaneDistances(near_plane, far_plane);
}

void ModuleCamera::SetPosition(float x, float y, float z)
{
    frustum.SetPos(vec(x, y, z));
}

void ModuleCamera::SetOrientation(float x, float y, float z)
{
    float3x3 rotationMatrix = float3x3::FromEulerXYZ(x, y, z);
    frustum.SetFront(rotationMatrix * float3::unitZ);
    frustum.SetUp(rotationMatrix * float3::unitY);
}

void ModuleCamera::Translate(vec translation)
{
    frustum.SetPos(frustum.Pos() + translation);
}

void ModuleCamera::Rotate(float3x3 rotationMatrix)
{
    vec oldFront = frustum.Front().Normalized();
    vec oldUp = frustum.Up().Normalized();
    frustum.SetFront(rotationMatrix * oldFront);
    frustum.SetUp(rotationMatrix * oldUp);
}

void ModuleCamera::LookAt(float x, float y, float z)
{
    vec direction = vec(x, y, z) - frustum.Pos();
    direction.Normalize();
    Rotate(float3x3::LookAt(frustum.Front().Normalized(), direction, frustum.Up().Normalized(), float3::unitY));
}

float4x4 ModuleCamera::GetProjectionMatrix()
{
    return frustum.ProjectionMatrix().Transposed();
}

float4x4 ModuleCamera::GetViewMatrix()
{
    return float4x4(frustum.ViewMatrix()).Transposed();
}
