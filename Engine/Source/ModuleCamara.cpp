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

	ImGuiIO& io = ImGui::GetIO();

	bool blockMouse = io.WantCaptureMouse;

	float speed = moveSpeed;

	float dt = ImGui::GetIO().DeltaTime;

	float deltaX = 0.0f;
	float deltaY = 0.0f;

	if (!blockMouse)
	{
		//Camera speed boost
		if (kbState.IsKeyDown(Keyboard::Keys::LeftShift))
		{
			speed *= 5.0f;
		}

		//Camera rotation
		if (mouseState.rightButton) //Rotation for free look
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
		else if (mouseState.leftButton && kbState.IsKeyDown(Keyboard::Keys::LeftAlt)) //Rotation for orbiting
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

		if (mouseState.rightButton) //Free look movement
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

		if (kbState.IsKeyDown(Keyboard::Keys::LeftAlt) && mouseState.leftButton) //Orbiting movement
		{
			Vector3 offset = Vector3::Backward * distanceToPivot;
			offset = Vector3::Transform(offset, orientation);
			position = lookAt + offset;
		}

		//Zooming
		if (mouseState.scrollWheelValue > scrollValue)
		{
			if (distanceToPivot > 1)
			{
				distanceToPivot -= 1.f;
				position = pivotPoint - forward * distanceToPivot;
				
			}
		}
		if (mouseState.scrollWheelValue < scrollValue)
		{
			distanceToPivot += 1.f;
			position = pivotPoint - forward * distanceToPivot;
		}


		position += translation;

		pivotPoint = position + forward * distanceToPivot;

		if (kbState.IsKeyDown(Keyboard::Keys::F)) //Focus on lookAt point
		{
			pivotPoint = lookAt;
			position = pivotPoint - forward * distanceToPivot;
		}

		Quaternion inverseOrientation;
		orientation.Inverse(inverseOrientation);


		viewMatrix = Matrix::CreateFromQuaternion(inverseOrientation);
		viewMatrix.Translation(Vector3::Transform(-position, inverseOrientation));

	}
	scrollValue = mouseState.scrollWheelValue;

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

void ModuleCamara::GetProjectionMatrix(Matrix* matrix) const
{
}


