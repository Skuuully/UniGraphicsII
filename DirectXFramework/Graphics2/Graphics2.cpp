#include "Graphics2.h"



Graphics2 app;

void Graphics2::CreateSceneGraph()
{
	// initialise camera sensitivity
	_cameraSensitivity = 10;
	// give an offset for the camera
	_offset = XMFLOAT3{ 0.0f, 20.0f, 0.0f };

	SceneGraphPointer sceneGraph = GetSceneGraph();
	
	GetCamera()->SetCameraPosition(0.0f, 1000.0f, -500.0f);
	XMFLOAT4 col = { 0.0f, 0.2f, 1.0f, 0.0f };
	DirectXFramework::SetBackgroundColour(col);

	shared_ptr<Cube> cube = make_shared<Cube>(L"", L"woodbox.bmp");
	sceneGraph->Add(cube);
	shared_ptr<SkyNode> sky = make_shared<SkyNode>(L"sky", L"skymap.dds", 5000);
	sceneGraph->Add(sky);
	shared_ptr<MeshNode> plane = make_shared<MeshNode>(L"Plane", L"Plane\\Bonanza.3DS");	sceneGraph->Add(plane);
	_terrain = make_shared<TerrainNode>(L"terrain", L"Example_HeightMap.RAW");
	sceneGraph->Add(_terrain);
	cube->SetWorldTransform(XMMatrixScaling(50.0f, 50.0f, 50.0f) * XMMatrixTranslation(0.0f, 600.0f, 0.0f));
	//shared_ptr<MoveableNode> planeTwo = make_shared<MoveableNode>(L"Plane2", L"Plane\\Bonanza.3DS");
	//sceneGraph->Add(planeTwo);

}

void Graphics2::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	MoveCamera();

	// spin the world
	//_rotationAngle += 1;
	//float angle = _rotationAngle * XM_PI / 180.0F;
	//sceneGraph->SetWorldTransform(XMMatrixRotationAxis(XMVectorSet(0.0F, 1.0F, 0.0F, 0.0F), angle));
	_planeMoveInZ -= 3.0f;
	SceneNodePointer plane = sceneGraph->Find(L"Plane");
	plane->SetWorldTransform(XMMatrixRotationX(DegreesToRadians(90.0f)) * XMMatrixScaling(3.0f, 3.0f, 3.0f) * XMMatrixTranslation(0.0f, 1000.0f, 0.0f + _planeMoveInZ));
}

void Graphics2::MoveCamera()
{
	float moveSpeed = _cameraSensitivity * 2.0f;
	float rotateSpeed = _cameraSensitivity / 2.0f;
	if (GetAsyncKeyState(VK_SPACE) == 0)
	{
		CameraHover(_offset);
	}

	// W KEY 0x57
	if (GetAsyncKeyState(0x57) < 0)
	{
		GetCamera()->SetForwardBack(moveSpeed);
	}
	
	// S key 0x53
	 if (GetAsyncKeyState(0x53) < 0)
	 {
		GetCamera()->SetForwardBack(-moveSpeed);
	 }

	// A key 0x41
	 if (GetAsyncKeyState(0x41) < 0)
  	 {
		GetCamera()->SetLeftRight(-moveSpeed);
	 }

	// D key 0x44
	 if (GetAsyncKeyState(0x44) < 0)
	 {
		GetCamera()->SetLeftRight(moveSpeed);
	 }

 	 if(GetAsyncKeyState(VK_UP) < 0)
	 {
		GetCamera()->SetPitch(-rotateSpeed);
	 }

	 if (GetAsyncKeyState(VK_DOWN) < 0)
	 {
		GetCamera()->SetPitch(rotateSpeed);
	 }

	 if (GetAsyncKeyState(VK_LEFT) < 0)
	 {
		GetCamera()->SetYaw(-rotateSpeed);
	 }

	 if (GetAsyncKeyState(VK_RIGHT) < 0)
	 {
		GetCamera()->SetYaw(rotateSpeed);
	 }

	 //Able to chnage senstivity of camera capped between 1 and 10
	 // Q key 0x51
	 if (GetAsyncKeyState(0x51) < 0 && _cameraSensitivity > 2)
	 {
		 _cameraSensitivity -= 1;
	 }

	 // E key 0x45
	 if (GetAsyncKeyState(0x45) < 0 && _cameraSensitivity < 9)
	 {
		 _cameraSensitivity += 1;
	 }
}

void Graphics2::CameraHover(XMFLOAT3 offset)
{
	// get starting camera postion
	XMVECTOR cameraPos = GetCamera()->GetCameraPosition();
	float cameraX = XMVectorGetX(cameraPos);	
	float cameraZ = XMVectorGetZ(cameraPos);
	//add offset to the values
	float cameraY = _terrain->GetHeightAtPoint(cameraX, cameraZ) + offset.y;
	cameraX += offset.x;
	cameraZ += offset.z;
	// Set new position
	GetCamera()->SetCameraPosition(cameraX, cameraY, cameraZ);
}

float Graphics2::DegreesToRadians(float degrees)
{
	return degrees * (XM_PI / 180.0f);
}
