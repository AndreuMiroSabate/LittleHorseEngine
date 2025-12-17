#include "Globals.h"
#include "ModuleCamara.h"
#include "Application.h"
#include "ModuleInput.h"

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
	position = Vector3(0.0f, 2.0f, 10.0f);
	orientation = Quaternion::CreateFromAxisAngle(Vector3::Up, 0.0f);
	pivotPoint = Vector3::Zero;
	lookAt = Vector3::Zero;

	yaw = 0.0f;
	pitch = 0.0f;
	distanceToPivot = 10.0f;

	nearPlane = 0.1f;
	farPlane = 2000.0f;

	scrollValue = 0.0f;

	moveSpeed = 1.0f;


	Quaternion inverseOrientation;
	orientation.Inverse(inverseOrientation);
	viewMatrix = Matrix::CreateFromQuaternion(inverseOrientation);
	viewMatrix.Translation(-position);

	viewMatrix = Matrix::CreateLookAt(position, pivotPoint, Vector3::Up);

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

	float speed = moveSpeed;
	//float elapsedTime = app->getElapsedMilis()*0.001f;

	float dt = ImGui::GetIO().DeltaTime;

	float deltaX = 0.0f;
	float deltaY = 0.0f;

	if (kbState.IsKeyDown(Keyboard::Keys::LeftShift))
	{
		speed *= 5.0f;
	}

	if(mouseState.rightButton)
	{
		if (!isDragging)
		{
			lastMouseX = mouseState.x;
			lastMouseY = mouseState.y;
			isDragging = true;
		}
		else
		{
			deltaX = (mouseState.x - lastMouseX);
			deltaY = (mouseState.y - lastMouseY);
			lastMouseX = mouseState.x;
			lastMouseY = mouseState.y;
			yaw -= deltaX * 0.001f;
			pitch -= deltaY * 0.001f;
		}
	}
	else
	{
		isDragging = false;
	}

	pitch = Clamp(pitch, -1.55f, 1.55f);

	Quaternion quatYaw = Quaternion::CreateFromAxisAngle(Vector3::Up, yaw);
	Quaternion quatPitch = Quaternion::CreateFromAxisAngle(Vector3::Right, pitch);
	orientation = quatPitch * quatYaw;

	Vector3 forward = Vector3::Transform(Vector3::Forward, orientation);
	Vector3 right = Vector3::Transform(Vector3::Right, orientation);
	Vector3 up = Vector3::Transform(Vector3::Up, orientation);

	if (mouseState.rightButton)
	{

		if (kbState.IsKeyDown(Keyboard::Keys::W))
		{
			translation += forward * speed * dt;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::S))
		{
			translation -= forward * speed * dt;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::A))
		{
			translation -= right * speed * dt;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::D))
		{
			translation += right * speed * dt;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::E))
		{
			translation += up * speed * dt;
		}
		if (kbState.IsKeyDown(Keyboard::Keys::Q))
		{
			translation -= up * speed * dt;
		}

	}

	if (mouseState.scrollWheelValue > scrollValue)
	{
		if(distanceToPivot > 1)
		{
			distanceToPivot -= 1.f;
			position = pivotPoint - forward * distanceToPivot;
			scrollValue = mouseState.scrollWheelValue;
		}
	}
	if (mouseState.scrollWheelValue < scrollValue)
	{
		distanceToPivot += 1.f;
		position = pivotPoint - forward * distanceToPivot;
		scrollValue = mouseState.scrollWheelValue;
	}


	position += translation;

	pivotPoint = position + forward * distanceToPivot;

	if (kbState.IsKeyDown(Keyboard::Keys::F))
	{
		pivotPoint = lookAt;
		position = pivotPoint - forward * distanceToPivot;
	}

	Quaternion inverseOrientation;
	orientation.Inverse(inverseOrientation);


	//viewMatrix = Matrix::CreateFromQuaternion(inverseOrientation);
	viewMatrix.Translation(Vector3::Transform(-position,inverseOrientation));


	viewMatrix = Matrix::CreateLookAt(position, pivotPoint, Vector3::Up);
	
}

bool ModuleCamara::cleanUp()
{
	return true;
}

float ModuleCamara::Clamp(float value, float min, float max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
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
void ModuleCamara::SetLookAt(Vector3 point)
{
	lookAt = Vector3(point.x, point.y, point.z);
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


