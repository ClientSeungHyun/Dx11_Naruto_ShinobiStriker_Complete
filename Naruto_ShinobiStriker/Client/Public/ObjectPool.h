#pragma once
#include "Client_Defines.h"

#include "GameObject.h"

BEGIN(Client)

template <typename T>
class CObjectPool final
{
public:

	// Get�ϸ� ť���� ��������
	static T* Get_GameObject()
	{
		if (Objects.empty()) 
			return nullptr;

		T* pObj = Objects.front();
		if (pObj == nullptr)
			return nullptr;

		Objects.pop();

		return pObj;
	}

	// �� ������ ������ ��ȯ���ֱ�
	static _bool Return_GameObject(T* pObj)
	{
		if (pObj == nullptr)
			return nullptr;
		Objects.push(pObj);

		return TRUE;
	}

	static void Clear()
	{
		while (!Objects.empty())   Objects.pop();

	}

	// ���̾ �߰��صθ� ���� ȣ������ �ʾƵ� ��
	static void Free()
	{
		while (!Objects.empty())
		{
			T* pObj = Objects.front();
			Safe_Release(pObj);
			Objects.pop();
		}
	}

	static queue<T*>& Get_Queue() { return Objects; }

private:
	static queue<T*> Objects; 


private:
	CObjectPool() {}
	~CObjectPool() = default;

};

template <typename T>
queue<T*> CObjectPool<T>::Objects;

END