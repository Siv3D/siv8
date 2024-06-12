//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <memory>
# include "Common.hpp"
# include "Array.hpp"
# include "HashTable.hpp"
# include "Optional.hpp"
# include "PointVector.hpp"
# include "ColorHSV.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MathParser
	//
	////////////////////////////////////////////////////////////////

	/// @brief 数式パーサ
	class MathParser
	{
	public:

		using Fty0	= double(*)();
		using Fty1	= double(*)(double);
		using Fty2	= double(*)(double, double);
		using Fty3	= double(*)(double, double, double);
		using Fty4	= double(*)(double, double, double, double);
		using Fty5	= double(*)(double, double, double, double, double);
		using Fty6	= double(*)(double, double, double, double, double, double);
		using Fty7	= double(*)(double, double, double, double, double, double, double);
		using Fty8	= double(*)(double, double, double, double, double, double, double, double);
		using Fty9	= double(*)(double, double, double, double, double, double, double, double, double);
		using Fty10	= double(*)(double, double, double, double, double, double, double, double, double, double);

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		MathParser();

		/// @brief 数式を設定します。
		/// @param expression 数式
		[[nodiscard]]
		explicit MathParser(StringView expression);

		////////////////////////////////////////////////////////////////
		//
		//	isValid
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isValid() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit operator bool() const;

		////////////////////////////////////////////////////////////////
		//
		//	getErrorMessage
		//
		////////////////////////////////////////////////////////////////

		/// @brief 最後に発生したエラーメッセージを返します。
		/// @return 最後に発生したエラーメッセージ
		[[nodiscard]]
		String getErrorMessage() const;

		////////////////////////////////////////////////////////////////
		//
		//	setExpression
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を設定します。
		/// @param expression 数式
		void setExpression(StringView expression);

		////////////////////////////////////////////////////////////////
		//
		//	setConstant
		//
		////////////////////////////////////////////////////////////////

		/// @brief 定数を登録します。
		/// @param name 定数名
		/// @param value 定数の値
		/// @return 定数の登録に成功した場合 true, それ以外の場合は false
		bool setConstant(StringView name, double value);

		////////////////////////////////////////////////////////////////
		//
		//	setVariable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 変数を登録します。
		/// @param name 変数名
		/// @param value 変数へのポインタ
		/// @return 変数の登録に成功した場合 true, それ以外の場合は false
		bool setVariable(StringView name, double* value);

		////////////////////////////////////////////////////////////////
		//
		//	setFunction
		//
		////////////////////////////////////////////////////////////////

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数の無い関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty0 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 1 つの関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty1 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 2 つの関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty2 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 3 つの関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty3 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 4 つの関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty4 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 5 つの関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty5 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 6 つの関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty6 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 7 つの関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty7 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 8 つの関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty8 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 9 つの関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty9 f);

		/// @brief 関数を登録します。
		/// @param name 関数名
		/// @param f 引数が 10 個の関数
		/// @return 関数の登録に成功した場合 true, それ以外の場合は false
		bool setFunction(StringView name, Fty10 f);

		////////////////////////////////////////////////////////////////
		//
		//	setPrefixOperator
		//
		////////////////////////////////////////////////////////////////

		/// @brief プレフィックス演算子を登録します。
		/// @param name 演算子の名前
		/// @param f 引数が 1 つの関数
		/// @remark `validPrefixCharacters()` で示されている文字が使えます。
		/// @return 演算子の登録に成功した場合 true, それ以外の場合は false
		bool setPrefixOperator(StringView name, Fty1 f);

		////////////////////////////////////////////////////////////////
		//
		//	setPostfixOperator
		//
		////////////////////////////////////////////////////////////////

		/// @brief ポストフィックス演算子を登録します。
		/// @param name 演算子の名前
		/// @param f 引数が 1 つの関数
		/// @remark `validPostfixCharacters()` で示されている文字が使えます。
		/// @return 演算子の登録に成功した場合 true, それ以外の場合は false
		bool setPostfixOperator(StringView name, Fty1 f);

		////////////////////////////////////////////////////////////////
		//
		//	removeVariable
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録した定数または変数を削除します。
		/// @param name 定数または変数の名前
		void removeVariable(StringView name);

		////////////////////////////////////////////////////////////////
		//
		//	clearVariables
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録したすべての定数と変数を削除します。
		void clearVariables();

		////////////////////////////////////////////////////////////////
		//
		//	clearFunctions
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録したすべての関数を削除します。
		void clearFunctions();

		////////////////////////////////////////////////////////////////
		//
		//	clearOperators
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録したすべての演算子を削除します。
		void clearOperators();

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式と登録したすべての内容を削除します。
		void clear();

		////////////////////////////////////////////////////////////////
		//
		//	getExpression
		//
		////////////////////////////////////////////////////////////////

		/// @brief 設定されている数式を返します。
		/// @return 設定されている数式
		[[nodiscard]]
		String getExpression() const;

		////////////////////////////////////////////////////////////////
		//
		//	getUsedVariables
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式で使用されている変数一覧を返します。
		/// @return 数式で使用されている変数一覧
		[[nodiscard]]
		HashTable<String, double*> getUsedVariables() const;

		////////////////////////////////////////////////////////////////
		//
		//	getVariables
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録されている変数一覧を返します。
		/// @return 登録されている変数一覧
		[[nodiscard]]
		HashTable<String, double*> getVariables() const;

		////////////////////////////////////////////////////////////////
		//
		//	getConstants
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録されている定数一覧を返します。
		/// @return 登録されている定数一覧
		[[nodiscard]]
		HashTable<String, double> getConstants() const;

		////////////////////////////////////////////////////////////////
		//
		//	validNameCharacters
		//
		////////////////////////////////////////////////////////////////

		/// @brief 定数名、変数名、関数名に使える文字一覧を返します。
		/// @return "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
		[[nodiscard]]
		String validNameCharacters() const;

		////////////////////////////////////////////////////////////////
		//
		//	validPrefixCharacters
		//
		////////////////////////////////////////////////////////////////

		/// @brief プレフィックス演算子に使える文字一覧を返します。
		/// @return "/+-*^?<>=#!$%&|~'_"
		[[nodiscard]]
		String validPrefixCharacters() const;

		////////////////////////////////////////////////////////////////
		//
		//	validPostfixCharacters
		//
		////////////////////////////////////////////////////////////////

		/// @brief ポストフィックス演算子に使える文字一覧を返します。
		/// @return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-*^/?<>=#!$%&|~'_"
		[[nodiscard]]
		String validPostfixCharacters() const;

		////////////////////////////////////////////////////////////////
		//
		//	eval
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を評価した結果を返します。
		/// @return 数式を評価した結果。エラーが発生した場合は Math::NaN
		[[nodiscard]]
		double eval() const;

		////////////////////////////////////////////////////////////////
		//
		//	evalOr
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を評価した結果を返します。
		/// @param defaultValue エラーが発生した場合の代わりの結果
		/// @return 数式を評価した結果。エラーが発生した場合は defaultValue
		[[nodiscard]]
		double evalOr(double defaultValue = Math::NaN) const;

		////////////////////////////////////////////////////////////////
		//
		//	evalOpt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を評価した結果を返します。
		/// @return 数式を評価した結果。エラーが発生した場合は none
		[[nodiscard]]
		Optional<double> evalOpt() const;

		////////////////////////////////////////////////////////////////
		//
		//	evalArray
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を評価した結果を返します。
		/// @return 数式を評価した結果
		[[nodiscard]]
		Array<double> evalArray() const;

		////////////////////////////////////////////////////////////////
		//
		//	evalVec2
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を評価した結果を返します。
		/// @return 数式を評価した結果
		[[nodiscard]]
		Vec2 evalVec2() const;

		////////////////////////////////////////////////////////////////
		//
		//	evalVec3
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を評価した結果を返します。
		/// @return 数式を評価した結果
		[[nodiscard]]
		Vec3 evalVec3() const;

		////////////////////////////////////////////////////////////////
		//
		//	evalVec4
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を評価した結果を返します。
		/// @return 数式を評価した結果
		[[nodiscard]]
		Vec4 evalVec4() const;

		////////////////////////////////////////////////////////////////
		//
		//	evalColorF
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を評価した結果を返します。
		/// @return 数式を評価した結果
		[[nodiscard]]
		ColorF evalColorF() const;

		////////////////////////////////////////////////////////////////
		//
		//	evalHSV
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数式を評価した結果を返します。
		/// @return 数式を評価した結果
		[[nodiscard]]
		HSV evalHSV() const;

	private:

		class MathParserDetail;

		std::shared_ptr<MathParserDetail> pImpl;
	};

	////////////////////////////////////////////////////////////////
	//
	//	Eval
	//
	////////////////////////////////////////////////////////////////

	/// @brief 数式を評価してその結果を返します。
	/// @param expression 数式
	/// @return 数式を評価した結果。エラーの場合は `Math::NaN`
	[[nodiscard]]
	double Eval(StringView expression);

	////////////////////////////////////////////////////////////////
	//
	//	EvalOpt
	//
	////////////////////////////////////////////////////////////////

	/// @brief 数式を評価してその結果を返します。
	/// @param expression 数式
	/// @return 数式を評価した結果。エラーの場合は none
	[[nodiscard]]
	Optional<double> EvalOpt(StringView expression);
}
