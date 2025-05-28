#pragma once
#include <string>
#include <iostream>
#include <format>
#include <fstream>

class Logger {
public:
    // 初期化（ファイルログ開始）
    static void Initialize();

    // ログ出力（コンソール & ファイル）
    static void Log(const std::string& message);

    // エラー出力（赤文字）
    static void Error(const std::string& message);

    // 終了処理
    static void Finalize();
};
