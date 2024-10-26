#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CShadow_Custom final : public CPlayer
{
public:
	enum PARTID { PART_BODYONCE, PART_BODYUPPER, PART_BODYLOWER, PART_FACE, PART_HEAD, PART_ACCESSORIE, PART_END };
	enum SHADOW_SKILL { RASENSHURIKEN, RASENSHURIKEN_AERIAL, MASSIVERASENGAN, SKILL_END };
	enum SHADOW_DIR { LEFT, RIGHT, DIR_END };

public:
	typedef struct : public CPlayer::PLAYER_DESC {
		SHADOW_SKILL eSkill;
		SHADOW_DIR eDir;
	}SHADOW_DESC;

	typedef struct {
		_uint iUpperID;
		_uint iLowerID;
		_uint iFaceID;
		_uint iHeadID;
		_uint iAcceID;
		_uint iOnceID;
		_uint iEyeID;
		_bool isOnceClothes;
	}CUSTOM_INFO;

private:
	CShadow_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShadow_Custom(const CShadow_Custom& Prototype);
	virtual ~CShadow_Custom() = default;

public:
	class CPlayer_Custom*		Get_Owner() { return m_pOwner; }
	void						Set_Owner(class CPlayer_Custom* pOwner) { m_pOwner = pOwner; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Create_Shadow(SHADOW_DESC tDesc);

private:
	class CPlayer_Custom*	m_pOwner = { nullptr };

	_uint					m_iAnimationIndex = {};

	SHADOW_SKILL			m_eSkill = { SKILL_END };
	SHADOW_DIR				m_eDir = { DIR_END };

	CUSTOM_INFO				m_tCustomInfo = {};

private:
	void Parts_Priority_Update(_float fTimeDelta);
	void Parts_Update(_float fTimeDelta);
	void Parts_Late_Update(_float fTimeDelta);

	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

	HRESULT Load_CustomInfo();

public:
	static CShadow_Custom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END