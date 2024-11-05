#pragma once

#ifndef SKINNEDDATA_H
#define  SKINNEDDATA_H
#include "d3dUtil.h"

// ¶¯»­¹Ø¼üÖ¡
struct VectorKey {
	float TimePos;
	DirectX::XMFLOAT3 Value;
};
struct QuatKey {
	float TimePos;
	DirectX::XMFLOAT4 Value;
};

class BoneAnimation {
public:

	float GetStartTime()const;
	float GetEndTime()const;
	void Interpolate(float t, DirectX::XMFLOAT4X4& M);

	std::vector<VectorKey> mTranslation;
	std::vector<VectorKey> mScale;
	std::vector<QuatKey> mRotationQuat;

	DirectX::XMFLOAT4X4 mDefaultTransform;

private:
	DirectX::XMVECTOR LerpKeys(float t, const std::vector<VectorKey>& keys);
	DirectX::XMVECTOR LerpKeys(float t, const std::vector<QuatKey>& keys);
};

class AnimationClip {
public:
	float GetClipStartTime()const;
	float GetClipEndTime()const;
	void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransform);

	std::vector<BoneAnimation> mBoneAnimations;
};

class SkinnedData {
public:
	UINT GetBoneCount()const { return mBoneHierarchy.size(); }
	float GetClipStartTime(const std::string& clipName)const;
	float GetClipEndTime(const std::string& clipName)const;
	void Set(std::vector<int>& boneHierarchy,
		std::vector<DirectX::XMFLOAT4X4>& boneOffsets,
		std::unordered_map<std::string, AnimationClip>& animations);
	void GetFinalTransform(const std::string& clipName, float timePos, std::vector<DirectX::XMFLOAT4X4>& finalTransforms);

private:
	std::vector<int> mBoneHierarchy;
	std::vector<DirectX::XMFLOAT4X4> mBoneOffsets;
	std::unordered_map<std::string, AnimationClip> mAnimations;
};


struct SkinnedModelInstance {
	SkinnedData* SkinnedInfo = nullptr;
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;

	void UpdateSkinnedAnimation(float deltaTime) {
		TimePos += deltaTime;
		SkinnedInfo;

		//Loop
		if (TimePos > SkinnedInfo->GetClipEndTime(ClipName))
			TimePos = 0.0f;

		SkinnedInfo->GetFinalTransform(ClipName, TimePos, FinalTransforms);
	}
};
#endif