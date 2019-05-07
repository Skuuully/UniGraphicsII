#include "Graphics2.h"

Graphics2 app;

void Graphics2::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// initialise camera properties
	_offset = XMFLOAT3{ 0.0f, 100.0f, -100.0f };

	//add terrain
	_terrain = make_shared<TerrainNode>(L"terrain", L"Example_HeightMap.RAW");
	sceneGraph->Add(_terrain);

	// add player
	_player = make_shared<MoveableNode>(L"Plane2", L"Plane\\Bonanza.3DS");
	sceneGraph->Add(_player);
	_player->SetPosition(0.0f, 500.0f, 0.0f);
	GetCamera()->SetNodeToFollow(_player, _offset);

	// add ant other stuff
	shared_ptr<Cube> cube = make_shared<Cube>(L"", L"woodbox.bmp");
	sceneGraph->Add(cube);
	cube->SetWorldTransform(XMMatrixScaling(50.0f, 50.0f, 50.0f) * XMMatrixTranslation(0.0f, 600.0f, 0.0f));
	shared_ptr<SkyNode> sky = make_shared<SkyNode>(L"sky", L"skymap.dds", 5000);
	sceneGraph->Add(sky);
	shared_ptr<MeshNode> plane = make_shared<MeshNode>(L"Plane", L"Plane\\Bonanza.3DS");
	sceneGraph->Add(plane);
	shared_ptr<MeshNode> fir = make_shared<MeshNode>(L"Fir", L"Fir\\Fir.3DS");
	sceneGraph->Add(fir);
	shared_ptr<MeshNode> oak = make_shared<MeshNode>(L"Oak", L"Trees\\Oak.3ds");
	sceneGraph->Add(oak);
	shared_ptr<MeshNode> palm = make_shared<MeshNode>(L"Palm", L"Trees\\Palm.3ds");
	sceneGraph->Add(palm);
	shared_ptr<MeshNode> poplar = make_shared<MeshNode>(L"Poplar", L"Trees\\Poplar.3ds");
	sceneGraph->Add(poplar);
}

void Graphics2::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	MovePlayer();


	// spin the world
	//_rotationAngle += 1;
	//float angle = _rotationAngle * XM_PI / 180.0F;
	//sceneGraph->SetWorldTransform(XMMatrixRotationAxis(XMVectorSet(0.0F, 1.0F, 0.0F, 0.0F), angle));
	_planeMoveInZ -= 1.0f;
	SceneNodePointer plane = sceneGraph->Find(L"Plane");
	plane->SetWorldTransform(XMMatrixRotationX(XMConvertToDegrees(-90.0f)) * XMMatrixScaling(3.0f, 3.0f, 3.0f) * XMMatrixTranslation(-200.0f, 800.0f, 0.0f + _planeMoveInZ));

	SceneNodePointer fir = sceneGraph->Find(L"Fir");
	float firX = 500.0f;
	float firZ = 100.0f;
	fir->SetWorldTransform(XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixRotationX(90.0f) * XMMatrixTranslation(firX, _terrain->GetHeightAtPoint(firX, firZ), firZ));

	SceneNodePointer oak = sceneGraph->Find(L"Oak");
	float oakX = 800.0f;
	float oakZ = 120.0f;
	oak->SetWorldTransform(XMMatrixScaling(20.0f, 20.0f, 20.0f) * XMMatrixRotationX(90.0f) * XMMatrixTranslation(oakX, _terrain->GetHeightAtPoint(oakX, oakZ), oakZ));

	SceneNodePointer palm = sceneGraph->Find(L"Palm");
	float palmX = 250.0f;
	float palmZ = 60.0f;
	palm->SetWorldTransform(XMMatrixScaling(20.0f, 20.0f, 20.0f) * XMMatrixRotationX(90.0f) * XMMatrixTranslation(palmX, _terrain->GetHeightAtPoint(palmX, palmZ), palmZ));

	SceneNodePointer poplar = sceneGraph->Find(L"Poplar");
	float poplarX = -100.0f;
	float poplarZ = 200.0f;
	poplar->SetWorldTransform(XMMatrixScaling(20.0f, 20.0f, 20.0f) * XMMatrixRotationX(90.0f) * XMMatrixTranslation(poplarX, _terrain->GetHeightAtPoint(poplarX, poplarZ), poplarZ));

}

//From when the camera was first person, would lock the player ot the terrain
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

void Graphics2::MovePlayer()
{
	// W key
	if (GetAsyncKeyState(0x57) < 0)
	{
		_player->SetForwardBack(-5.0f);
	}

	// S key 0x53
	if (GetAsyncKeyState(0x53) < 0)
	{
		_player->SetForwardBack(5.0f);
	}

	// A key 0x41
	if (GetAsyncKeyState(0x41) < 0)
	{
		_player->SetLeftRight(-5.0f);
	}
	float rot = 5;
	// D key 0x44
	if (GetAsyncKeyState(0x44) < 0)
	{
		_player->SetLeftRight(5.0f);
	}

	if (GetAsyncKeyState(VK_UP) < 0)
	{
		_player->SetPitch(-rot);
	}

	if (GetAsyncKeyState(VK_DOWN) < 0)
	{
		_player->SetPitch(rot);
	}

	if (GetAsyncKeyState(VK_LEFT) < 0)
	{
		_player->SetYaw(rot);
	}

	if (GetAsyncKeyState(VK_RIGHT) < 0)
	{
		_player->SetYaw(-rot);
	}
}
