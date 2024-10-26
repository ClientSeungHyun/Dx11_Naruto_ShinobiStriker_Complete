#pragma once

#include "Component_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"

#include "fmod.h"
#include "fmod.hpp"

/* 1. ������ Ŭ���̾�Ʈ�� ������ ���� Ŭ������. */
/* 2. �������� Ŭ���̾�Ʈ�� �����ְ� ���� �Լ����� ��Ƴ��´�. */
/* 3. �Լ��� -> Ŭ������ ����Լ�. ��ü�� �ʿ��ϴ�! �׷��� ������� �������ֱ����� Ŭ���� ��ü�� �����Ѵ�. */
/* 4. ��ü���� �����ϱ����� Initialize_Engine() �Լ� �ȿ��� ����� �������ֱ����� Ŭ�������� ��üȭ�Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw_Engine();
	void FinalUpdate_Engine();
	HRESULT Clear(_uint iLevelIndex);

public:
	POINT Get_MosePos();

	_float Get_Random_Normal() { return (_float)rand() / (_float)RAND_MAX; }
	_float Get_Random(_float fMin, _float fMax) { return fMin + Get_Random_Normal() * (fMax - fMin); }
	
	_float Lerp(_float fSour, _float fDest, _float fRatio);

#pragma region GRAPHIC_DEVICE
public:
	ID3D11ShaderResourceView* Get_BackBuffer_SRV() const;
	void Render_Begin();
	void Render_End();
#pragma endregion

#pragma region INPUT_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	class CLevel* Get_CurLevel();
	_uint   Get_CurLevelIndex();
	void Set_CurLevelIndex(_uint iLevelIndex);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Object_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
	class CGameObject* Clone_GameObject(const _wstring& strPrototypeTag, void* pArg);
	class CGameObject* Get_CloneObject_AddLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	class CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);

	class CGameObject* Find_Player(_uint iLevelIndex = 0);
	class CCamera* Find_Camera(_uint iLevelIndex = 0);

	class CGameObject* Find_NearObject(class CGameObject* pOwner, _uint iLevelIndex, const _wstring& strLayerTag);
#pragma endregion


#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr );
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);
	_float Get_TimeDelta(const _wstring& strTimerTag);
	void Start_Lack(const _wstring& strTimerTag, _uint iLackCount);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	list<class CGameObject*>& Get_RenderList(CRenderer::RENDERGROUP eGroup);
	void Set_IsRenderStaticLight(_bool isRender);

	HRESULT Load_StaticLightDepthTexture(const _wstring& strTextureFilePath);
#pragma endregion

#pragma region PIPELINE
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	void Set_StaticTransform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_StaticTransform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;

	_float4 Get_CamPosition_Float4() const;
	_vector Get_CamPosition_Vector() const;

	_vector Get_LigthDepthPosition()const;
	_vector Get_LigthDepthAt()const;
	void Set_LigthDepth(_fvector vShadowLightPosition, _fvector vShadowLightAt);
#pragma endregion

#pragma region PICKING
	void Transform_MouseRay_ToLocalSpace(const _fmatrix& WorldMatrix);
	_bool isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _float3* pOut);
	void Add_ModelPickingList(class CGameObject* pObj);
	_vector			Get_RayPos();
	void			Set_RayPos(_vector vRayPos);
	_vector			Get_RayDir();
	void			Set_RayDir(_vector vRayDir);
	class CGameObject* Get_Player_Picking();
	void				Set_Player_Picking(class CGameObject* pPlayer);
#pragma endregion

#pragma region KEY_MANAGER
	KEY_STATE   Get_KeyState(KEY _eKey);
#pragma endregion

#pragma region SOUND_MANAGER
	void Play_BGM(const TCHAR* pSoundKey, _float fVolume);
	void Play_Effect(const TCHAR* pSoundKey, _float fVolume);
	void Stop_BGM();
	void Pause_BGM();
	void SetVolume_BGM(_float fVolume);
	void	Set_Listener(class CGameObject* pListener);
	FMOD::System* Get_SoundSystem();
	map<TCHAR*, FMOD::Sound*>& Get_Sounds();
	void	LoadSoundFile(const char* pFolderName);
#pragma endregion

#pragma region EVENT_MANAGER
	HRESULT Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize = 0, void* pArg = nullptr);
	HRESULT Event_DestoryObject(class CGameObject* pGameObject);
#pragma endregion

#pragma region PhysX_MANAGER
	HRESULT AddScene_ColMesh(class CGameObject* pObject, _wstring strModelName);
	HRESULT SetUp_Player(class CGameObject* pPlayer);
	void    Reset_PhsyX();

	_bool   RayCast(_vector vRayPos, _vector vRayDir, _vector* vHitPos, _vector* vNormal, _float* fHitDistance);
	_bool   RayCast_PlayerDown(_vector* vHitPos, _vector* vNormal, _float* fHitDistance);
	_bool   RayCast_PlayerLook(_vector* vHitPos, _vector* vNormal, _float* fHitDistance);
	void    Compute_Collision(_vector* vHitDir, _float* fHitLength);

	HRESULT Add_PhysX_Monster(class CGameObject* pMonster);
#pragma endregion

#pragma region Collider_Manager
	HRESULT Add_ColliderList(class CCollider* pCollider);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f);
	HRESULT Render_TextCenter(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f);
#pragma endregion

#pragma region LIGHT_MANAGER
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void Clear_Light();
	_float4x4 Get_StaticLightViewMatrix()const;
	void Set_StaticLightViewMatrix(_matrix vStaticViewMatirx);
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(class CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName);

	HRESULT Create_RenderTargetTexture(const _wstring& strFilePath, const _wstring& strTargetTag);
	HRESULT Load_RenderTargetTexture(const _wstring& strTargetTag, const _wstring& strTextureFilePath);
#pragma region FRUSTUM
		_bool isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius = 0.f);
#pragma endregion

#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CPicking_Manager*			m_pPicking_Manager = { nullptr };
	class CCollider_Manager*		m_pCollider_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };

	/*�߰��� ��ɵ�*/
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CUI_Manager*				m_pUI_Manager = { nullptr };

	class CPhysX_Manager*			m_pPhysX_Manager = { nullptr };
	class CEvent_Manager*			m_pEvent_Manager = { nullptr };

public:	
	void Release_Engine();
	virtual void Free() override;
};

END