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
# include "Mat4x4.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Mesh3DPlacement
	//
	////////////////////////////////////////////////////////////////

	/// @brief Mesh3DBuilder の形状、Mesh3DAssembly の部品、および Loft の断面の配置変換
	/// @remark Builder と Assembly は鏡映時も形状の表裏を維持します。Loft の断面フレームには Loft 固有の向き・進行方向の条件があります。
	/// @remark この型の構築時には入力を検査しません。数値的な事前条件は使用先の API に従います。
	/// @remark `Vec3` から暗黙に変換できるため、平行移動だけを指定する add 関数の呼び出しは `addShape(..., offset)` と書けます。
	/// @remark `Quaternion` から暗黙に変換できるため、原点を中心とする回転だけなら `addShape(..., rotation)` と書けます。
	/// @remark 回転と平行移動を指定する場合は `addShape(..., { offset, rotation })` と書けます。
	/// @remark `{ offset, rotation }` は原点を中心に回転してから平行移動します。Assembly で両者を独立した関節として操作しない場合は、1 つの部品の配置にまとめられます。
	/// @remark `{ offset, rotation, scale }` はローカル軸に沿って拡大・縮小し、原点を中心に回転してから平行移動します。scale は Vec3 で指定します。
	/// @remark `Mat4x4` から暗黙に変換できるため、任意のアフィン変換を指定する呼び出しは `addShape(..., transform)` と書けます。
	/// @code
	/// builder.addTube(path, 0.25, offset);
	/// builder.addTube(path, 0.25, rotation);
	/// builder.addTube(path, 0.25, { offset, rotation });
	/// builder.addTube(path, 0.25, { offset, rotation, Vec3{ 2, 1, 0.5 } });
	/// builder.addTube(path, 0.25, transform);
	///
	/// const Mesh3DPlacement placement{ offset, rotation };
	/// builder.addTube(pathA, 0.25, placement);
	/// builder.addTube(pathB, 0.15, placement);
	/// @endcode
	class Mesh3DPlacement
	{
	public:

		/// @brief 平行移動を表す配置変換を作成します。
		/// @param offset 平行移動量
		[[nodiscard]]
		Mesh3DPlacement(Vec3 offset) noexcept
			: m_transform{ Mat4x4::Translate(Float3{ offset }) } {}

		/// @brief 原点を中心とする回転を表す配置変換を作成します。平行移動は行いません。
		/// @param rotation 単位クォータニオン
		[[nodiscard]]
		Mesh3DPlacement(const Quaternion& rotation) noexcept
			: m_transform{ Mat4x4::Rotate(rotation) } {}

		/// @brief 回転および平行移動を表す配置変換を作成します。
		/// @param offset 平行移動量
		/// @param rotation 原点を中心とする回転を表す単位クォータニオン
		[[nodiscard]]
		Mesh3DPlacement(Vec3 offset, const Quaternion& rotation) noexcept
			: Mesh3DPlacement{ offset, rotation, Vec3::One() } {}

		/// @brief 拡大・縮小、回転、および平行移動を表す配置変換を作成します。
		/// @param offset 回転後に適用する平行移動量
		/// @param rotation 拡大・縮小後に適用する、原点を中心とする単位クォータニオン
		/// @param scale 回転前のローカル軸に沿った各軸の拡大率
		/// @remark Mat4x4::AffineTransform(Float3{ scale }, rotation, Float3{ offset }) と同じ変換です。値は内部で float に変換されます。
		/// @remark 入力の検査・正規化は行いません。0 や負の scale も行列として保持しますが、使用先の事前条件に従う必要があります。Loft は特異な断面フレームや determinant が負の断面フレームを受け付けません。
		[[nodiscard]]
		Mesh3DPlacement(Vec3 offset, const Quaternion& rotation, Vec3 scale) noexcept
			: m_transform{ Mat4x4::AffineTransform(Float3{ scale }, rotation, Float3{ offset }) } {}

		/// @brief アフィン変換行列から配置変換を作成します。
		/// @param transform 適用するアフィン変換行列
		[[nodiscard]]
		Mesh3DPlacement(const Mat4x4& transform) noexcept
			: m_transform{ transform } {}

		/// @brief 配置変換行列を返します。
		/// @return 配置変換行列
		[[nodiscard]]
		const Mat4x4& getTransform() const noexcept
		{
			return m_transform;
		}

		/// @brief 元の座標系を配置先の座標系へ一致させる配置変換を作成します。
		/// @param sourceFrame 取り付け座標系から形状・部品のローカル座標系への変換
		/// @param targetFrame 取り付け座標系から配置先（親）のローカル座標系への変換
		/// @return 行列として `sourceFrame.inverse() * targetFrame`。浮動小数点の誤差を除き `sourceFrame * 戻り値 == targetFrame` となる変換
		/// @remark 両入力は有限のアフィン変換で、sourceFrame は逆変換可能、その逆行列と結果も float で表現可能な有限値である必要があります。数値的な事前条件は検査しません。
		/// @remark 平行移動と回転だけでなく、スケール、せん断、鏡映も含めて座標系を合わせます。面を向かい合わせる回転や隙間は targetFrame に明示してください。
		/// @remark 一度だけ配置を計算します。入力の変更への自動追従や、親の world 変換の適用は行いません。寸法変更時は形状と取り付け座標系をレシピで再生成してください。
		/// @code
		/// // 高さ 2 の箱の底面中央を、親の原点へ合わせる
		/// const auto placement = Mesh3DPlacement::Align(Vec3{ 0, -1, 0 }, Vec3::Zero());
		/// @endcode
		[[nodiscard]]
		static Mesh3DPlacement Align(const Mesh3DPlacement& sourceFrame, const Mesh3DPlacement& targetFrame) noexcept
		{
			return (sourceFrame.m_transform.inverse() * targetFrame.m_transform);
		}

	private:

		Mat4x4 m_transform;
	};

}
