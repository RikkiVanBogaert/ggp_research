#pragma once
#include "stdafx.h"
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

//From Framework---------------------------------------------------------
//std::vector<HouseInfo> GetHousesInFOV(IExamInterface* interfacePtr) 
//{
//	std::vector<HouseInfo> vHousesInFOV = {};
//
//	HouseInfo hi = {};
//	for (int i = 0;; ++i)
//	{
//		if (interfacePtr->Fov_GetHouseByIndex(i, hi))
//		{
//			vHousesInFOV.push_back(hi);
//			continue;
//		}
//
//		break;
//	}
//
//	return vHousesInFOV;
//}
//
//std::vector<EntityInfo> GetEntitiesInFOV(IExamInterface* interfacePtr)
//{
//	std::vector<EntityInfo> vEntitiesInFOV = {};
//
//	EntityInfo ei = {};
//	for (int i = 0;; ++i)
//	{
//		if (interfacePtr->Fov_GetEntityByIndex(i, ei))
//		{
//			vEntitiesInFOV.push_back(ei);
//			continue;
//		}
//
//		break;
//	}
//
//	return vEntitiesInFOV;
//}


//OWN------------------------------------------------------------------------------
//bool ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos)
//{
//	if (agent.Position.Distance(targetPos) <= agent.GrabRange)
//		return true;
//
//	return false;
//}
#pragma endregion