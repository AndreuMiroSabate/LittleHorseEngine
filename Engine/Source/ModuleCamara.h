#pragma once
#include "Module.h"
class ModuleCamara :
    public Module
{
public:

    ModuleCamara();
    ~ModuleCamara();
    bool init() override;
    void update() override;
	bool cleanUp() override;

	float Clamp(float value, float min, float max);

	void SetFOV(float v_fov);
	void SetAspectRatio(float v_aspectRatio);
	void SetPlaneDistances(float v_nearPlane, float v_farPlane);
	void SetPosition(float x, float y, float z);
	void SetLookAt(Vector3 point);
	void SetOrientation(float v_pitch, float v_yaw, float v_roll);
	const Matrix& GetViewMatrix() const { return viewMatrix; };
	void GetProjectionMatrix(Matrix* matrix) const;

	private:
	// Camera parameters
	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
	
	Vector3 lookAt;
	float roll;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	float dragX;
	float dragY;

	float scrollValue;

	Vector3 pivotPoint;
	float distanceToPivot;
	float yaw;
	float pitch;
	Quaternion orientation;
	Vector3 position;

	int lastMouseX;
	int lastMouseY;
	bool isDragging = false;

	float moveSpeed;


};

