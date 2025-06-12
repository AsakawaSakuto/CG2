#pragma once

enum BlendMode {
	// ブレンドなし
	kBlendModeNone,
	// 通常αブレンド
	kBlendModeNormal,
	// 加算
	kBlendModeAdd,
	// 減算
	kBlendModeSubtract,
	// 乗算
	kBlendModeMultily,
	// スクリーン
	kBlendModeScreen,
	// 利用してはいけない
	kCountOfBlendMode,
};