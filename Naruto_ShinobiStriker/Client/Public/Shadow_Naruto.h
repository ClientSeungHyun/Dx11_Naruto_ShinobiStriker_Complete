#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CShadow_Naruto final : public CPlayer
{
public:
	enum SHADOWNARUTO_SKILL { RASENSHURIKEN, RASENSHURIKEN_AERIAL, MASSIVERASENGAN, SKILL_END };
	enum SHADOWNARUTO_DIR { LEFT, RIGHT, DIR_END };

public:
	typedef struct : public CPlayer::PLAYER_DESC {
		SHADOWNARUTO_SKILL eSkill;
		SHADOWNARUTO_DIR eDir;
	}SHADOW_NARUTO_DESC;

private:
	CShadow_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShadow_Naruto(const CShadow_Naruto& Prototype);
	virtual ~CShadow_Naruto() = default;

public:
	class CPlayer_Naruto*		Get_Owner() { return m_pOwner; }
	void						Set_Owner(class CPlayer_Naruto* pOwner) { m_pOwner = pOwner; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Create_ShadowNaruto(SHADOW_NARUTO_DESC tDesc);

private:
	HRESULT Ready_Components();

private:
	class CPlayer_Naruto* m_pOwner = { nullptr };

	_uint	m_iAnimationIndex = {};

	SHADOWNARUTO_SKILL		m_eSkill = { SKILL_END };
	SHADOWNARUTO_DIR		m_eDir = { DIR_END };

public:
	static CShadow_Naruto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END