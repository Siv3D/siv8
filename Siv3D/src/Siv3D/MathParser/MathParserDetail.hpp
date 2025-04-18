﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/MathParser.hpp>
# include <ThirdParty/muparser/muParser.h>

namespace s3d
{
	class MathParser::MathParserDetail
	{
	public:

		[[nodiscard]]
		String getErrorMessage() const;

		void setExpression(StringView expression);

		[[nodiscard]]
		bool setConstant(StringView name, double value);

		[[nodiscard]]
		bool setVariable(StringView name, double* value);

		template <class Fty>
		[[nodiscard]]
		bool setFunction(const StringView name, Fty f)
		{
			try
			{
				m_parser.DefineFun(Unicode::ToWstring(name), f);
				m_errorMessage.clear();
				return true;
			}
			catch (mu::Parser::exception_type& e)
			{
				m_errorMessage = e.GetMsg();
				return false;
			}
		}

		[[nodiscard]]
		bool setPrefixOperator(StringView name, Fty1 f);

		[[nodiscard]]
		bool setPostfixOperator(StringView name, Fty1 f);

		void removeVariable(StringView name);

		void clearVariables();

		void clearFunctions();

		void clearOperators();

		void clear();

		[[nodiscard]]
		String getExpression() const;

		[[nodiscard]]
		HashMap<String, double*> getUsedVariables() const;

		[[nodiscard]]
		HashMap<String, double*> getVariables() const;

		[[nodiscard]]
		HashMap<String, double> getConstants() const;

		[[nodiscard]]
		String validNameCharacters() const;

		[[nodiscard]]
		String validPrefixCharacters() const;

		[[nodiscard]]
		String validPostfixCharacters() const;

		[[nodiscard]]
		double eval() const;

		[[nodiscard]]
		Array<double> evalArray() const;

		void eval(double* dst, size_t count) const;

	private:

		mu::Parser m_parser;

		mutable std::wstring m_errorMessage;
	};
}
