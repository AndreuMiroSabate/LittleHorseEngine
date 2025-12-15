#include "Globals.h"
#include "ModuleCamara.h"
#include "Application.h"

#include "Mouse.h"
#include "Keyboard.h"

ModuleCamara::ModuleCamara()
{
}

ModuleCamara::~ModuleCamara()
{
}

bool ModuleCamara::init()
{
	position = Vector3(0.0f, 0.0f, 5.0f);
	orientation = Quaternion::CreateFromAxisAngle(Vector3::Up, 0.0f);
	lookAt = Vector3::Zero;
	nearPlane = 0.1f;
	farPlane = 2000.0f;


	Quaternion inverseOrientation;
	orientation.Inverse(inverseOrientation);
	viewMatrix = Matrix::CreateFromQuaternion(inverseOrientation);
	viewMatrix.Translation(-position);

	viewMatrix = Matrix::CreateLookAt(position, lookAt, Vector3::Up);

	return true;
}

void ModuleCamara::update()
{
	Keyboard& keyboard = Keyboard::Get();
	Mouse& mouse = Mouse::Get();

	Keyboard::State kbState = keyboard.GetState();
	Mouse::State mouseState = mouse.GetState();

	Vector3 translation = Vector3::Zero;
	Vector2 rotation = Vector2::Zero;

	float speed = 0.1f;

	if(kbState.IsKeyDown(Keyboard::Keys::LeftShift))
	{
		speed = 0.2f;
	}

	if(mouseState.rightButton)
	{
		if (kbState.IsKeyDown(Keyboard::Keys::W))
		{
			translation.z -= 1 * speed;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::S))
		{
			translation.z += 1 * speed;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::A))
		{
			translation.x -= 1 * speed;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::D))
		{
			translation.x += 1 * speed;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::Q))
		{
			translation.y += 1 * speed;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::E))
		{
			translation.y -= 1 * speed;
		}

		rotation.x = (mouseState.x - dragX) * 0.005f;
		rotation.y = (mouseState.y - dragY) * 0.005f;
	}

	if(mouseState.scrollWheelValue > 0)
	{
		position += Vector3::Forward * 0.5f;
	}
	else if(mouseState.scrollWheelValue < 0)
	{
		position += Vector3::Backward * 0.5f;
	}

	if (kbState.IsKeyDown(Keyboard::Keys::F))
	{
		viewMatrix = Matrix::CreateLookAt(position, lookAt, Vector3::Up);
	}

	//pitch += XMConvertToRadians(rotation.y * 25.f);
	//yaw += XMConvertToRadians(rotation.x * 25.f);

	//Quaternion pitchQuat = Quaternion::CreateFromAxisAngle(Vector3::Right, pitch);
	//Quaternion yawQuat = Quaternion::CreateFromAxisAngle(Vector3::Up, yaw);

	//orientation += yawQuat * pitchQuat;

	position += translation;
	

	viewMatrix.Translation(Vector3::Transform(-position, Matrix::CreateFromQuaternion(orientation)));

	dragX = mouseState.x;
	dragY = mouseState.y;

	
}

bool ModuleCamara::cleanUp()
{
	return true;
}

void ModuleCamara::SetFOV(float v_fov)
{
}

void ModuleCamara::SetAspectRatio(float v_aspectRatio)
{
}

void ModuleCamara::SetPlaneDistances(float v_nearPlane, float v_farPlane)
{
}

void ModuleCamara::SetPosition(float x, float y, float z)
{
}
void ModuleCamara::SetLookAt(float x, float y, float z)
{
}

void ModuleCamara::SetOrientation(float v_pitch, float v_yaw, float v_roll)
{
}

Matrix ModuleCamara::GetViewMatrix() const
{
	return viewMatrix;
}

void ModuleCamara::GetProjectionMatrix(Matrix* matrix) const
{
}


