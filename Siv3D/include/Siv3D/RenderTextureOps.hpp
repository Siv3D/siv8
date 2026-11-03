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
# include <memory>
# include "TextureFilter.hpp"

namespace s3d
{
	class Texture;
	class RenderTexture;
	struct TextureRegion;
	struct Rect;

	namespace detail
	{
		struct RenderTextureOpsAccess;
	}

	/// @brief テクスチャの内容を処理し、指定した RenderTexture へ書き込みます。
	/// @anchor render_texture_ops_contract
	/// @par 入出力
	/// 入出力は mip 0 を使います。矩形の単位はそれぞれの画像の実ピクセルです。
	/// Scene の解像度倍率と、通常描画の変換・色・ブレンド・viewport・シザー・シェーダは適用しません。
	/// 出力矩形内の全成分を上書きし、矩形外と下位 mip の内容を保持します。
	/// 出力リソースのサイズ・形式は変更しません。矩形を省略した便利形は、入力・出力とも画像全体を対象にします。
	/// TextureRegion または sourceRect を指定する場合は、destinationRect も指定します。
	/// @par 領域と失敗
	/// 空の画像、負の寸法、画像外の矩形、操作に適合しないサイズ・形式・引数には Error を送出します。
	/// 出力矩形の幅または高さが 0 の場合は書き込みません。この場合も、各操作の引数の条件は満たす必要があります。
	/// 非空の出力には非空の入力が必要です。領域を画像の範囲へ自動で切り詰めません。
	/// 引数の検証と中間資源の準備は、この操作による出力への書き込み前に行います。
	/// GPU コマンドの記録・実行に失敗した場合、出力内容のロールバックは保証しません。
	/// @par 描画との順序
	/// 先行する描画、画像処理、後続の描画の順序を保ちます。GPU の処理完了は待ちません。
	/// 呼び出し前の描画状態を保持します。resolve とミップマップ生成は行いません。
	/// MSRenderTexture を入力にすると、最後に resolve した画像を参照します。
	/// @par 対応形式
	/// 非圧縮の色形式を入力・出力に使います。Copy は同形式、フィルタは対応する色形式間で変換します。
	/// 線形補間には、その形式のフィルタリングを GPU がサポートしている必要があります。非対応の場合は Error を送出します。
	/// 中間画像は 16-bit float、32-bit float 形式を含む処理では 32-bit float です。チャンネル数は処理に応じて選択します。
	/// @par フィルタの共通規則
	/// Copy 以外では、入力領域を独立した画像として扱い、境界は端の画素を延長します。
	/// TextureRegion は反転していない整数画素領域を指定します。size は使用しません。
	/// UV から復元した境界が整数から 1/512 画素以内なら整数へ丸め、それ以外は Error を送出します。
	/// 画素の中心を対応させてサンプリングします。計算・補間・保存の精度による丸め誤差があります。
	/// 色画像は乗算済みアルファとして処理し、アルファ表現の変換は行いません。
	/// sRGB 形式は読み込み時に復号し、書き込み時に符号化します。
	/// Copy は補間も色変換も行いません。
	/// @par 同じ画像への書き込み
	/// Workspace を使う操作は、入力と出力が同じ GPU 画像でも使用できます。
	/// 領域が重なる場合も、この操作の書き込み前の入力内容を使います。同一性は共有する GPU 画像で判定します。
	/// Copy と Resample の制限は各関数を参照してください。
	namespace RenderTextureOps
	{
		/// @brief 画像処理の中間テクスチャと準備情報を再利用する作業領域
		/// @details 必要な資源は処理時に準備します。互換性のある形式と確保済み容量で処理できる場合、中間テクスチャを再利用します。
		/// @remark 有効領域や使用する縮小段数が減っても、それだけでは確保済みの容量を縮小しません。
		/// @remark 処理結果は確保容量や以前の操作に依存しません。中間テクスチャの有効領域外の画素は、処理結果に寄与しません。
		/// @remark 入力・出力画像や処理結果を所有せず、内部テクスチャは公開しません。
		/// @remark コピーはできません。移動後の移動元は空の作業領域として再使用できます。
		class Workspace
		{
		public:

			////////////////////////////////////////////////////////////////
			//
			//	(constructor)
			//
			////////////////////////////////////////////////////////////////

			/// @brief 中間テクスチャを持たない作業領域を作成します。
			[[nodiscard]]
			Workspace();

			Workspace(const Workspace& other) = delete;

			/// @brief 作業領域の所有を移動します。
			/// @param other 移動元
			[[nodiscard]]
			Workspace(Workspace&& other) noexcept;

			////////////////////////////////////////////////////////////////
			//
			//	(destructor)
			//
			////////////////////////////////////////////////////////////////

			/// @brief 保持している資源の所有を手放します。
			~Workspace();

			////////////////////////////////////////////////////////////////
			//
			//	operator =
			//
			////////////////////////////////////////////////////////////////

			Workspace& operator =(const Workspace& other) = delete;

			/// @brief 現在の資源の所有を手放し、作業領域の所有を移動します。
			/// @param other 移動元
			/// @return *this
			Workspace& operator =(Workspace&& other) noexcept;

			////////////////////////////////////////////////////////////////
			//
			//	release
			//
			////////////////////////////////////////////////////////////////

			/// @brief 中間テクスチャと準備情報を手放し、空の作業領域に戻します。
			/// @remark 記録済みの GPU 処理は継続します。GPU の完了待ちや、使用中の資源の即時解放は行いません。
			void release() noexcept;

		private:

			class WorkspaceDetail;
			std::unique_ptr<WorkspaceDetail> pImpl;

			friend struct s3d::detail::RenderTextureOpsAccess;
		};

		////////////////////////////////////////////////////////////////
		//
		//	Copy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像全体をコピーします。入出力は同サイズ・同形式にします。
		/// @param source 入力画像全体
		/// @param destination 書き込み先。画像全体を上書きします。
		/// @see @ref render_texture_ops_copy
		void Copy(const Texture& source, const RenderTexture& destination);

		/// @brief 入力領域の画素を、同サイズ・同形式の出力領域へコピーします。
		/// @anchor render_texture_ops_copy
		/// @param source 入力画像
		/// @param sourceRect 入力領域（実ピクセル）
		/// @param destination 書き込み先
		/// @param destinationRect 出力領域（実ピクセル）
		/// @throws Error 入出力領域のサイズまたは画像の形式が異なる場合。同じ GPU 画像の異なる矩形を指定した場合。
		/// @remark 同じ GPU 画像の同じ矩形を指定すると何もしません。
		/// @see @ref render_texture_ops_contract
		void Copy(const Texture& source, const Rect& sourceRect,
				  const RenderTexture& destination, const Rect& destinationRect);

		////////////////////////////////////////////////////////////////
		//
		//	Resample
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力画像全体を、出力画像全体へ再サンプリングします。
		/// @param source 入力画像全体
		/// @param destination 書き込み先。画像全体を上書きします。
		/// @param filter 補間方式。Nearest または Linear。
		/// @see @ref render_texture_ops_resample
		void Resample(const Texture& source, const RenderTexture& destination,
					  TextureFilter filter = TextureFilter::Linear);

		/// @brief 入力領域全体を、出力領域のサイズへ再サンプリングします。
		/// @details 指定した補間方式で拡大・縮小し、対応する形式間で変換します。
		/// @anchor render_texture_ops_resample
		/// @param source 入力領域。描画サイズは使用しません。
		/// @param destination 書き込み先
		/// @param destinationRect 出力領域（実ピクセル）
		/// @param filter 補間方式。Nearest または Linear。
		/// @throws Error 入力と出力が同じ GPU 画像の場合。
		/// @see @ref render_texture_ops_contract
		void Resample(const TextureRegion& source,
					  const RenderTexture& destination, const Rect& destinationRect,
					  TextureFilter filter = TextureFilter::Linear);

		/// @brief 整数矩形で指定した入力領域を再サンプリングします。
		/// @param source 入力画像
		/// @param sourceRect 入力領域（実ピクセル）
		/// @param destination 書き込み先
		/// @param destinationRect 出力領域（実ピクセル）
		/// @param filter 補間方式。Nearest または Linear。
		/// @see @ref render_texture_ops_resample
		void Resample(const Texture& source, const Rect& sourceRect,
					  const RenderTexture& destination, const Rect& destinationRect,
					  TextureFilter filter = TextureFilter::Linear);

		////////////////////////////////////////////////////////////////
		//
		//	Downsample
		//
		////////////////////////////////////////////////////////////////

		/// @brief 入力画像全体を、出力画像全体へ縮小します。
		/// @param source 入力画像全体
		/// @param destination 書き込み先。画像全体を上書きします。
		/// @param work 再利用する作業領域
		/// @see @ref render_texture_ops_downsample
		void Downsample(const Texture& source, const RenderTexture& destination,
						Workspace& work);

		/// @brief 縮小用の低域通過フィルタを用いて、入力領域全体を出力領域へ縮小します。
		/// @details 各段で各辺を最大 2 倍縮小する段階的なフィルタです。一般の縮小率では 4 点の線形補間を平均します。厳密な面積平均ではありません。
		/// @anchor render_texture_ops_downsample
		/// @param source 入力領域。描画サイズは使用しません。
		/// @param destination 書き込み先
		/// @param destinationRect 出力領域（実ピクセル）。各辺を入力領域以下にします。
		/// @param work 再利用する作業領域
		/// @throws Error 出力領域の幅または高さが入力領域を上回る場合。
		/// @remark 片方の軸だけの縮小もできます。入出力が同サイズなら、入力を出力へ写します。
		/// @see @ref render_texture_ops_contract
		void Downsample(const TextureRegion& source,
						const RenderTexture& destination, const Rect& destinationRect,
						Workspace& work);

		/// @brief 整数矩形で指定した入力領域を縮小します。
		/// @param source 入力画像
		/// @param sourceRect 入力領域（実ピクセル）
		/// @param destination 書き込み先
		/// @param destinationRect 出力領域（実ピクセル）
		/// @param work 再利用する作業領域
		/// @see @ref render_texture_ops_downsample
		void Downsample(const Texture& source, const Rect& sourceRect,
						const RenderTexture& destination, const Rect& destinationRect,
						Workspace& work);

		////////////////////////////////////////////////////////////////
		//
		//	GaussianBlur
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像全体にガウスぼかしを適用します。入出力は同サイズにします。
		/// @param source 入力画像全体
		/// @param destination 書き込み先。画像全体を上書きします。
		/// @param sigma 標準偏差（入力の texel 単位）
		/// @param work 再利用する作業領域
		/// @see @ref render_texture_ops_gaussian
		void GaussianBlur(const Texture& source, const RenderTexture& destination,
						  double sigma, Workspace& work);

		/// @brief ガウスぼかしを適用し、同サイズの出力領域へ書き込みます。
		/// @details 半径 ceil(3 * sigma) で打ち切った離散ガウス係数を正規化し、水平・垂直に適用します。線形補間で隣接係数をまとめるため、補間精度による近似を含みます。
		/// @anchor render_texture_ops_gaussian
		/// @param source 入力領域。描画サイズは使用しません。
		/// @param destination 書き込み先
		/// @param destinationRect 出力領域（実ピクセル）。入力領域と同じサイズにします。
		/// @param sigma 標準偏差（入力の texel 単位）。0～32 の有限値。
		/// @param work 再利用する作業領域
		/// @throws Error 入出力領域のサイズが異なる場合。sigma が対応範囲外の場合。
		/// @remark sigma が 0 の場合は入力を出力へ写します。sigma が 0 かつ同じ GPU 画像の同じ矩形を指定した場合は、何もしません。
		/// @see @ref render_texture_ops_contract
		void GaussianBlur(const TextureRegion& source,
						  const RenderTexture& destination, const Rect& destinationRect,
						  double sigma, Workspace& work);

		/// @brief 整数矩形で指定した入力領域にガウスぼかしを適用します。
		/// @param source 入力画像
		/// @param sourceRect 入力領域（実ピクセル）
		/// @param destination 書き込み先
		/// @param destinationRect 出力領域（実ピクセル）
		/// @param sigma 標準偏差（入力の texel 単位）
		/// @param work 再利用する作業領域
		/// @see @ref render_texture_ops_gaussian
		void GaussianBlur(const Texture& source, const Rect& sourceRect,
						  const RenderTexture& destination, const Rect& destinationRect,
						  double sigma, Workspace& work);

		////////////////////////////////////////////////////////////////
		//
		//	DualKawaseBlur
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像全体に Dual Kawase 方式のぼかしを適用します。入出力は同サイズにします。
		/// @param source 入力画像全体
		/// @param destination 書き込み先。画像全体を上書きします。
		/// @param radius 公称ぼかし半径（入力の texel 単位）
		/// @param work 再利用する作業領域
		/// @see @ref render_texture_ops_dual_kawase
		void DualKawaseBlur(const Texture& source, const RenderTexture& destination,
			double radius, Workspace& work);

		/// @brief Dual Kawase 方式のぼかしを適用し、同サイズの出力領域へ書き込みます。
		/// @anchor render_texture_ops_dual_kawase
		/// @param source 入力領域。描画サイズは使用しません。
		/// @param destination 書き込み先
		/// @param destinationRect 出力領域（実ピクセル）。入力領域と同じサイズにします。
		/// @param radius 公称ぼかし半径（入力の texel 単位）。0～256 の有限値。
		/// @param work 再利用する作業領域
		/// @throws Error 入出力領域のサイズが異なる場合。radius が対応範囲外の場合。
		/// @remark radius はぼけの広がりの目安で、最大参照距離や GaussianBlur の sigma を表しません。
		/// @remark 縮小段数の間を補間し、radius の変化で結果が不連続に切り替わることを避けます。縮小画像が 1×1 に達すると、それ以上はぼかしを強めません。
		/// @remark radius が 0 の場合は入力を出力へ写します。radius が 0 かつ同じ GPU 画像の同じ矩形を指定した場合は、何もしません。
		/// @see @ref render_texture_ops_contract
		void DualKawaseBlur(const TextureRegion& source,
			const RenderTexture& destination, const Rect& destinationRect,
			double radius, Workspace& work);

		/// @brief 整数矩形で指定した入力領域に Dual Kawase 方式のぼかしを適用します。
		/// @param source 入力画像
		/// @param sourceRect 入力領域（実ピクセル）
		/// @param destination 書き込み先
		/// @param destinationRect 出力領域（実ピクセル）
		/// @param radius 公称ぼかし半径（入力の texel 単位）
		/// @param work 再利用する作業領域
		/// @see @ref render_texture_ops_dual_kawase
		void DualKawaseBlur(const Texture& source, const Rect& sourceRect,
			const RenderTexture& destination, const Rect& destinationRect,
			double radius, Workspace& work);
	}
}
