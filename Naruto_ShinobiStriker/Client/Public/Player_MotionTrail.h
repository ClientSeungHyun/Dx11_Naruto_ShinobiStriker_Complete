#pragma once

#include "Client_Defines.h"
#include "Player_Custom.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer_MotionTrail final : public CGameObject
{
	typedef struct {

		_float4x4			BoneMatrices[g_iMaxMeshBones];
	}MOTIONTRAIL_INFO;

private:
	CPlayer_MotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_MotionTrail(const CPlayer_MotionTrail& Prototype);
	virtual ~CPlayer_MotionTrail() = default;

public:
	_uint Get_MotionCount() { return (_uint)m_MotionTrailsInfos.size(); }

	_float4x4** Get_BoneMatrix(_uint iIndex) { return m_MotionTrailsInfos[iIndex]; }
	_float4x4* Get_WorldMatrix(_uint iIndex) { return &m_MotionWorldMatrixs[iIndex]; }
	_float*	Get_Alpha(_uint iIndex) { return &m_AlphaTimes[iIndex]; }

public:
	virtual HRESULT Initialize(_uint iNumMesh);
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Add_Info(_float4x4** BoneMatrix, _float4x4 WorldMatrix);
	void Clear();

public:
	_uint										m_iNumMesh = {};
	_uint										m_iMaxMotionTrailCount = {};
	_float										m_fDeleteTime = {};
	vector<_float4x4**>							m_MotionTrailsInfos;
	vector<_float4x4>							m_MotionWorldMatrixs;
	vector<_float>								m_AlphaTimes;

public:
	static CPlayer_MotionTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumMesh);
	virtual CGameObject* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;

};

END