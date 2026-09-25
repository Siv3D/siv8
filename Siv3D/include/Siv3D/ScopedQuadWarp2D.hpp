//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Mat3x3.hpp"
# include "Optional.hpp"

namespace s3d
{
	/// @brief スコープ内の 2D 描画に射影変換を適用します。
	/// @remark 適用順は Local、Camera、このスコープの変換、Viewport の画面変換です。
	/// @remark 座標は Local と Camera 適用後の、現在の Viewport 原点を基準とします。
	/// @remark カーソル、シザー矩形、曲線の分割品質、RMS 拡大率には適用されません。
	/// @remark 変換元の四角形によるクリッピングは行いません。
	/// @remark カスタム VS は同次座標の W を保持する必要があります。
	class ScopedQuadWarp2D
	{
	public:

		enum class Target : uint8
		{
			Push, ///< 新しい変換の後に、外側のスコープの変換を適用します。
			Set,  ///< 外側のスコープの変換を置き換えます。
		};

		/// @brief 描画状態を変更しないスコープを作成します。
		[[nodiscard]]
		ScopedQuadWarp2D() = default;

		/// @brief 射影変換を適用し、破棄時に以前の変換を復元します。
		/// @param transform 射影変換行列
		/// @param target 外側のスコープとの合成方法
		/// @pre 合成後の行列が有限かつ正則で、描画する全頂点で同次座標の W が正であること。
		/// @remark Identity() と Target::Set の組合せは外側の射影変換を一時的に無効にします。
		[[nodiscard]]
		explicit ScopedQuadWarp2D(const Mat3x3& transform, Target target = Target::Push);

		/// @brief 状態を復元する責任を移動します。移動元の破棄では復元しません。
		/// @param other 移動元
		[[nodiscard]]
		ScopedQuadWarp2D(ScopedQuadWarp2D&& other) noexcept;

		/// @brief 保存した射影変換を復元します。
		~ScopedQuadWarp2D();

		ScopedQuadWarp2D(const ScopedQuadWarp2D&) = delete;
		ScopedQuadWarp2D& operator =(const ScopedQuadWarp2D&) = delete;

	private:

		Optional<Mat3x3> m_oldTransform;
	};
}
