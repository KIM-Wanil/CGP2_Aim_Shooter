////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "bitmapclass.h"

#include "lightshaderclass.h"
#include "lightclass.h"
#include "textureshaderclass.h"
#include "specmapshaderclass.h"
#include "FireShaderClass.h"
#include "TextClass.h"
#include "SkyDomeClass.h"
#include "SkyDomeShaderClass.h"
#include "inputclass.h"
#include <directxmath.h>
#include <vector>
#include <random>
/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	struct SetMatrix
	{
		XMFLOAT3 scale;
		XMFLOAT3 rotate;
		XMFLOAT3 trans;
	};//ÀÛ¼ºÇÑ ±¸Á¶Ã¼
	enum EStage { Title, Main, Over };

	EStage GetStage() { return m_stage; }
	void SetStage(EStage stage) { m_stage = stage; }

	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int,int,float,float);
	void OnOffAmbientLight();
	void OnOffDiffuseLight();
	void OnOffSpecularLight();
	float GetCamYaw();
	float GetCamPitch();
	void SetMoveLeftRight(float);
	void SetMoveBackForward(float);
	void SetMoveUpDown(float);
	void SetYawPitch(float,float);
	void PickBoundBox(DIMOUSESTATE, HWND);

	void CreateBoundingVolumes(std::vector<XMFLOAT3>& vertPosArray,
		std::vector<XMFLOAT3>& boundingBoxVerts,
		std::vector<DWORD>& boundingBoxIndex);

	void pickRayVector(float mouseX, float mouseY, XMVECTOR& pickRayInWorldSpacePos, XMVECTOR& pickRayInWorldSpaceDir);
	float pick(XMVECTOR pickRayInWorldSpacePos,
		XMVECTOR pickRayInWorldSpaceDir,
		std::vector<XMFLOAT3>& vertPosArray,
		std::vector<DWORD>& indexPosArray,
		XMMATRIX& worldSpace);
	bool PointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point);

	void AddTitleNum();
	void Reset();
	void calculateCount();
private:
	bool Render(float,float,float);
	XMMATRIX ConvertWorldMatrix(SetMatrix);
	void SpawnEnemy();
	void DieCoolTime(int);
	void SpawnCoolTime();
	void ShootCoolTime();
	void ReloadCoolTime();


private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextClass* m_Text = nullptr;

	ModelClass* m_Model[5];
	BitmapClass* m_Title[3];
	BitmapClass* m_GameOver;

	int modelCount;
	LightShaderClass* m_LightShader;
	TextureShaderClass* m_TextureShader;
	SpecMapShaderClass* m_SpecMapShader;
	FireShaderClass* m_FireShader = nullptr;

	LightClass* m_Light;
	LightClass* m_Light1, * m_Light2, * m_Light3, * m_Light4;
	SkyDomeClass* m_SkyDome = nullptr;
	SkyDomeShaderClass* m_SkyDomeShader = nullptr;

	SetMatrix m_SetMatrix[9];
	SetMatrix m_EnemySetMatrix[10];
	XMMATRIX m_EnemyWorldMatrix[10];
	SetMatrix m_FireSetMatrix[10];
	XMMATRIX m_FireWorldMatrix[10];
	XMMATRIX m_ViewMatrix2D;
	

	bool isDie[10] ;
	bool isHit[10];
	float m_frameTime = 0.0f;

	int m_screenWidth = 0;
	int m_screenHeight = 0;

	bool isShoot = false;
	std::vector<XMFLOAT3> enemyBoundingBoxVertPosArray;
	std::vector<DWORD> enemyBoundingBoxVertIndexArray;

	EStage m_stage;

public:
	int m_mouseX = 0;
	int m_mouseY = 0;
	int score = 0;
	int numOfBullet = 10;
	bool canShoot = true;
	bool canSpawn = true;
	float dieTime[10] = {0.0f};
	float spawnTime = 0.0f;
	float shootTime = 0.0f;
	float reloadTime = 0.0f;
	int titleNum = 0;
	int numOfObject = 0;
	int numOfPolygon = 0;
	int fireCount = 0;
	int enemyCount = 0;
	int floorCount = 0;
};
/*
obj0: ·Îº¿
obj1: ¹Ù´Ú
obj2: ÃÑ
obj3: ¿¡ÀÓ
obj4: ºÒ²É


*/
#endif