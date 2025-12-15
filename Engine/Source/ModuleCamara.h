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

	void SetFOV(float v_fov);
	void SetAspectRatio(float v_aspectRatio);
	void SetPlaneDistances(float v_nearPlane, float v_farPlane);
	void SetPosition(float x, float y, float z);
	void SetLookAt(float x, float y, float z);
	void SetOrientation(float v_pitch, float v_yaw, float v_roll);
	Matrix GetViewMatrix() const;
	void GetProjectionMatrix(Matrix* matrix) const;

	private:
	// Camera parameters
	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
	Vector3 position;
	Quaternion orientation;
	Vector3 lookAt;
	float pitch;
	float yaw;
	float roll;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	float dragX;
	float dragY;
};

