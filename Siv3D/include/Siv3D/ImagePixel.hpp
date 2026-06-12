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
# include "ColorHSV.hpp"

namespace s3d
{
    namespace ImagePixel
    {
        /// @brief Image の画素に対する描画時の合成方式
        enum class BlendMode : uint8
        {
            /// @brief src で dst を置き換えます。
            Overwrite,

            /// @brief straight alpha の src を dst に Source Over 合成します。
            /// @remark RGB は格納されている 8-bit channel 値の空間で合成されます。linear-light blending は行いません。
            SourceOver,
        };

        //-------------------------------------------------------------------------
        //
        //  1 pixel operations
        //
        //-------------------------------------------------------------------------

        /// @brief dst を src で完全に置き換えます。
        /// @param src 書き込む色
        /// @param dst 現在の色。この関数では使用されません。
        /// @return src
        [[nodiscard]]
        constexpr Color Overwrite(Color src, Color dst) noexcept;

        /// @brief straight alpha の src を dst に Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の色。straight alpha として扱われます。
        /// @return src を dst に Source Over 合成した色
        /// @remark RGB は格納されている 8-bit channel 値の空間で合成されます。linear-light blending は行いません。
        [[nodiscard]]
        constexpr Color SourceOver(Color src, Color dst) noexcept;

        /// @brief dst が完全不透明であると仮定して、straight alpha の src を dst に Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の色。alpha は 255 でなければなりません。
        /// @return src を dst に Source Over 合成した色。戻り値の alpha は 255 です。
        /// @pre dst.a は 255 でなければなりません。
        /// @remark この関数は高速化のため dst.a を確認しません。
        /// @remark precondition を満たさない場合の結果は保証されません。
        [[nodiscard]]
        constexpr Color SourceOverAssumeOpaqueDst(Color src, Color dst) noexcept;

        /// @brief coverage に応じて dst から src へ線形補間します。
        /// @param src 補間先の色
        /// @param dst 補間元の色
        /// @param coverage 補間係数。0 で dst、255 で src
        /// @return dst と src を coverage で補間した色
        /// @remark alpha compositing ではなく、RGBA 各成分の値を補間します。
        [[nodiscard]]
        constexpr Color OverwriteWithCoverage(Color src, Color dst, uint8 coverage) noexcept;

        /// @brief coverage を src の alpha に適用してから、dst に Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の色。straight alpha として扱われます。
        /// @param coverage src の alpha に掛ける coverage。0 で無効、255 で完全に有効
        /// @return coverage 適用後の src を dst に Source Over 合成した色
        [[nodiscard]]
        constexpr Color SourceOverWithCoverage(Color src, Color dst, uint8 coverage) noexcept;

        /// @brief dst が完全不透明であると仮定して、coverage 適用後の src を Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の色。alpha は 255 でなければなりません。
        /// @param coverage src の alpha に掛ける coverage。0 で無効、255 で完全に有効
        /// @return coverage 適用後の src を dst に Source Over 合成した色。戻り値の alpha は 255 です。
        /// @pre dst.a は 255 でなければなりません。
        /// @remark この関数は高速化のため dst.a を確認しません。
        /// @remark precondition を満たさない場合の結果は保証されません。
        [[nodiscard]]
        constexpr Color SourceOverAssumeOpaqueDstWithCoverage(Color src, Color dst, uint8 coverage) noexcept;

        /// @brief 指定した合成方式で src を dst に適用します。
        /// @param blendMode 適用する合成方式
        /// @param src 適用する色
        /// @param dst 現在の色
        /// @return blendMode に従って src を dst に適用した色
        [[nodiscard]]
        constexpr Color Apply(BlendMode blendMode, Color src, Color dst) noexcept;

        /// @brief 指定した合成方式で、coverage を考慮して src を dst に適用します。
        /// @param blendMode 適用する合成方式
        /// @param src 適用する色
        /// @param dst 現在の色
        /// @param coverage 適用する coverage。0 で無効、255 で完全に有効
        /// @return blendMode と coverage に従って src を dst に適用した色
        [[nodiscard]]
        constexpr Color ApplyWithCoverage(BlendMode blendMode, Color src, Color dst, uint8 coverage) noexcept;

        //-------------------------------------------------------------------------
        //
        //  Contiguous pixel operations
        //
        //-------------------------------------------------------------------------

        /// @brief 複数の画素を src で完全に置き換えます。
        /// @param src 書き込む色
        /// @param dst 書き込み先の先頭ポインタ
        /// @param count 書き込む画素数
        /// @remark dst が nullptr または count が 0 の場合、この関数は何もしません。
        void Overwrite(Color src, Color* dst, size_t count) noexcept;

        /// @brief 複数の画素に対して、straight alpha の src を Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の先頭ポインタ
        /// @param count 合成する画素数
        /// @remark dst が nullptr または count が 0 の場合、この関数は何もしません。
        void SourceOver(Color src, Color* dst, size_t count) noexcept;

        /// @brief 複数の完全不透明画素に対して、straight alpha の src を Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の先頭ポインタ。各画素の alpha は 255 でなければなりません。
        /// @param count 合成する画素数
        /// @pre dst が nullptr ではなく count が 0 より大きい場合、dst[0] から dst[count - 1] までの各 alpha は 255 でなければなりません。
        /// @remark dst が nullptr または count が 0 の場合、この関数は何もしません。
        /// @remark この関数は高速化のため各画素の alpha を確認しません。
        /// @remark precondition を満たさない場合の結果は保証されません。
        void SourceOverAssumeOpaqueDst(Color src, Color* dst, size_t count) noexcept;

        /// @brief 複数の画素に対して、coverage に応じて dst から src へ線形補間します。
        /// @param src 補間先の色
        /// @param dst 書き込み先の先頭ポインタ
        /// @param coverage 各画素に適用する coverage 配列。0 で現在値を維持、255 で src に置換
        /// @param count 処理する画素数
        /// @remark dst または coverage が nullptr の場合、この関数は何もしません。
        /// @remark count が 0 の場合、この関数は何もしません。
        /// @remark alpha compositing ではなく、RGBA 各成分の値を補間します。
        void OverwriteWithCoverage(Color src, Color* dst, const uint8* coverage, size_t count) noexcept;

        /// @brief 複数の画素に対して、coverage を src の alpha に適用してから Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の先頭ポインタ
        /// @param coverage 各画素に適用する coverage 配列。0 で無効、255 で完全に有効
        /// @param count 処理する画素数
        /// @remark dst または coverage が nullptr の場合、この関数は何もしません。
        /// @remark count が 0 の場合、この関数は何もしません。
        void SourceOverWithCoverage(Color src, Color* dst, const uint8* coverage, size_t count) noexcept;

        /// @brief 複数の完全不透明画素に対して、coverage 適用後の src を Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の先頭ポインタ。各画素の alpha は 255 でなければなりません。
        /// @param coverage 各画素に適用する coverage 配列。0 で無効、255 で完全に有効
        /// @param count 処理する画素数
        /// @pre dst が nullptr ではなく count が 0 より大きい場合、dst[0] から dst[count - 1] までの各 alpha は 255 でなければなりません。
        /// @remark dst または coverage が nullptr の場合、この関数は何もしません。
        /// @remark count が 0 の場合、この関数は何もしません。
        /// @remark この関数は高速化のため各画素の alpha を確認しません。
        /// @remark precondition を満たさない場合の結果は保証されません。
        void SourceOverAssumeOpaqueDstWithCoverage(Color src, Color* dst, const uint8* coverage, size_t count) noexcept;

        /// @brief 指定した合成方式で、複数の画素に src を適用します。
        /// @param blendMode 適用する合成方式
        /// @param src 適用する色
        /// @param dst 書き込み先または合成先の先頭ポインタ
        /// @param count 処理する画素数
        /// @remark dst が nullptr または count が 0 の場合、この関数は何もしません。
        void Apply(BlendMode blendMode, Color src, Color* dst, size_t count) noexcept;

        /// @brief 指定した合成方式で、coverage を考慮して複数の画素に src を適用します。
        /// @param blendMode 適用する合成方式
        /// @param src 適用する色
        /// @param dst 書き込み先または合成先の先頭ポインタ
        /// @param coverage 各画素に適用する coverage 配列。0 で無効、255 で完全に有効
        /// @param count 処理する画素数
        /// @remark dst または coverage が nullptr の場合、この関数は何もしません。
        /// @remark count が 0 の場合、この関数は何もしません。
        void ApplyWithCoverage(BlendMode blendMode, Color src, Color* dst, const uint8* coverage, size_t count) noexcept;

        //-------------------------------------------------------------------------
        //
        //  Row / stride pixel operations
        //
        //-------------------------------------------------------------------------

        /// @brief 複数行の画素を src で完全に置き換えます。
        /// @param src 書き込む色
        /// @param dst 書き込み先の左上画素へのポインタ
        /// @param width 1 行あたりの処理画素数
        /// @param height 処理する行数
        /// @param dstStride 次の行へ進むための要素数。byte 数ではなく Color の個数です。
        /// @pre dst が nullptr ではなく width と height が 0 より大きい場合、dstStride は width 以上でなければなりません。
        /// @remark dst が nullptr、width が 0、または height が 0 の場合、この関数は何もしません。
        void OverwriteRows(Color src, Color* dst, size_t width, size_t height, size_t dstStride) noexcept;

        /// @brief 複数行の画素に対して、straight alpha の src を Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の左上画素へのポインタ
        /// @param width 1 行あたりの処理画素数
        /// @param height 処理する行数
        /// @param dstStride 次の行へ進むための要素数。byte 数ではなく Color の個数です。
        /// @pre dst が nullptr ではなく width と height が 0 より大きい場合、dstStride は width 以上でなければなりません。
        /// @remark dst が nullptr、width が 0、または height が 0 の場合、この関数は何もしません。
        void SourceOverRows(Color src, Color* dst, size_t width, size_t height, size_t dstStride) noexcept;

        /// @brief 複数行の完全不透明画素に対して、straight alpha の src を Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の左上画素へのポインタ。各画素の alpha は 255 でなければなりません。
        /// @param width 1 行あたりの処理画素数
        /// @param height 処理する行数
        /// @param dstStride 次の行へ進むための要素数。byte 数ではなく Color の個数です。
        /// @pre dst が nullptr ではなく width と height が 0 より大きい場合、dstStride は width 以上でなければなりません。
        /// @pre 処理対象の各画素の alpha は 255 でなければなりません。
        /// @remark dst が nullptr、width が 0、または height が 0 の場合、この関数は何もしません。
        /// @remark この関数は高速化のため各画素の alpha を確認しません。
        /// @remark precondition を満たさない場合の結果は保証されません。
        void SourceOverAssumeOpaqueDstRows(Color src, Color* dst, size_t width, size_t height, size_t dstStride) noexcept;

        /// @brief 複数行の画素に対して、coverage に応じて dst から src へ線形補間します。
        /// @param src 補間先の色
        /// @param dst 書き込み先の左上画素へのポインタ
        /// @param coverage 各画素に適用する coverage 配列の左上要素へのポインタ
        /// @param width 1 行あたりの処理画素数
        /// @param height 処理する行数
        /// @param dstStride 次の行へ進むための要素数。byte 数ではなく Color の個数です。
        /// @param coverageStride 次の行へ進むための要素数。byte 数ではなく uint8 の個数です。
        /// @pre dst と coverage が nullptr ではなく width と height が 0 より大きい場合、dstStride と coverageStride は width 以上でなければなりません。
        /// @remark dst または coverage が nullptr、width が 0、または height が 0 の場合、この関数は何もしません。
        /// @remark alpha compositing ではなく、RGBA 各成分の値を補間します。
        void OverwriteWithCoverageRows(Color src, Color* dst, const uint8* coverage, size_t width, size_t height, size_t dstStride, size_t coverageStride) noexcept;

        /// @brief 複数行の画素に対して、coverage を src の alpha に適用してから Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の左上画素へのポインタ
        /// @param coverage 各画素に適用する coverage 配列の左上要素へのポインタ
        /// @param width 1 行あたりの処理画素数
        /// @param height 処理する行数
        /// @param dstStride 次の行へ進むための要素数。byte 数ではなく Color の個数です。
        /// @param coverageStride 次の行へ進むための要素数。byte 数ではなく uint8 の個数です。
        /// @pre dst と coverage が nullptr ではなく width と height が 0 より大きい場合、dstStride と coverageStride は width 以上でなければなりません。
        /// @remark dst または coverage が nullptr、width が 0、または height が 0 の場合、この関数は何もしません。
        void SourceOverWithCoverageRows(Color src, Color* dst, const uint8* coverage, size_t width, size_t height, size_t dstStride, size_t coverageStride) noexcept;

        /// @brief 複数行の完全不透明画素に対して、coverage 適用後の src を Source Over 合成します。
        /// @param src 合成する色。straight alpha として扱われます。
        /// @param dst 合成先の左上画素へのポインタ。各画素の alpha は 255 でなければなりません。
        /// @param coverage 各画素に適用する coverage 配列の左上要素へのポインタ
        /// @param width 1 行あたりの処理画素数
        /// @param height 処理する行数
        /// @param dstStride 次の行へ進むための要素数。byte 数ではなく Color の個数です。
        /// @param coverageStride 次の行へ進むための要素数。byte 数ではなく uint8 の個数です。
        /// @pre dst と coverage が nullptr ではなく width と height が 0 より大きい場合、dstStride と coverageStride は width 以上でなければなりません。
        /// @pre 処理対象の各画素の alpha は 255 でなければなりません。
        /// @remark dst または coverage が nullptr、width が 0、または height が 0 の場合、この関数は何もしません。
        /// @remark この関数は高速化のため各画素の alpha を確認しません。
        /// @remark precondition を満たさない場合の結果は保証されません。
        void SourceOverAssumeOpaqueDstWithCoverageRows(Color src, Color* dst, const uint8* coverage, size_t width, size_t height, size_t dstStride, size_t coverageStride) noexcept;

        /// @brief 指定した合成方式で、複数行の画素に src を適用します。
        /// @param blendMode 適用する合成方式
        /// @param src 適用する色
        /// @param dst 書き込み先または合成先の左上画素へのポインタ
        /// @param width 1 行あたりの処理画素数
        /// @param height 処理する行数
        /// @param dstStride 次の行へ進むための要素数。byte 数ではなく Color の個数です。
        /// @pre dst が nullptr ではなく width と height が 0 より大きい場合、dstStride は width 以上でなければなりません。
        /// @remark dst が nullptr、width が 0、または height が 0 の場合、この関数は何もしません。
        void ApplyRows(BlendMode blendMode, Color src, Color* dst, size_t width, size_t height, size_t dstStride) noexcept;

        /// @brief 指定した合成方式で、coverage を考慮して複数行の画素に src を適用します。
        /// @param blendMode 適用する合成方式
        /// @param src 適用する色
        /// @param dst 書き込み先または合成先の左上画素へのポインタ
        /// @param coverage 各画素に適用する coverage 配列の左上要素へのポインタ
        /// @param width 1 行あたりの処理画素数
        /// @param height 処理する行数
        /// @param dstStride 次の行へ進むための要素数。byte 数ではなく Color の個数です。
        /// @param coverageStride 次の行へ進むための要素数。byte 数ではなく uint8 の個数です。
        /// @pre dst と coverage が nullptr ではなく width と height が 0 より大きい場合、dstStride と coverageStride は width 以上でなければなりません。
        /// @remark dst または coverage が nullptr、width が 0、または height が 0 の場合、この関数は何もしません。
        void ApplyWithCoverageRows(BlendMode blendMode, Color src, Color* dst, const uint8* coverage, size_t width, size_t height, size_t dstStride, size_t coverageStride) noexcept;
    }
}

#include "detail/ImagePixel.ipp"
