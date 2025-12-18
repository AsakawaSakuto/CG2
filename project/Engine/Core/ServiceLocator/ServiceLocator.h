#pragma once
#include "Core/DirectXCommon/DirectXCommon.h"

/// <summary>
/// サービスロケーターパターン
/// グローバルな依存関係を管理するクラス
/// </summary>
class ServiceLocator {
public:
	/// <summary>
	/// DirectXCommonを登録
	/// </summary>
	/// <param name="dxCommon">DirectXCommonのポインタ</param>
	static void Provide(DirectXCommon* dxCommon) {
		dxCommon_ = dxCommon;
	}

	/// <summary>
	/// DirectXCommonを取得
	/// </summary>
	/// <returns>DirectXCommonのポインタ</returns>
	static DirectXCommon* GetDXCommon() {
		return dxCommon_;
	}

	/// <summary>
	/// サービスがセットアップされているか確認
	/// </summary>
	/// <returns>セットアップされていればtrue</returns>
	static bool IsInitialized() {
		return dxCommon_ != nullptr;
	}

private:
	static DirectXCommon* dxCommon_;
};
