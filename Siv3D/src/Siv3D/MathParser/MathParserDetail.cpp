//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "MathParserDetail.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	getErrorMessage
	//
	////////////////////////////////////////////////////////////////

	String MathParser::MathParserDetail::getErrorMessage() const
	{
		return Unicode::FromWstring(m_errorMessage);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setExpression
	//
	////////////////////////////////////////////////////////////////

	void MathParser::MathParserDetail::setExpression(const StringView expression)
	{
		m_parser.SetExpr(Unicode::ToWstring(expression));
		m_errorMessage.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setConstant
	//
	////////////////////////////////////////////////////////////////

	bool MathParser::MathParserDetail::setConstant(const StringView name, const double value)
	{
		try
		{
			m_parser.DefineConst(Unicode::ToWstring(name), value);
			m_errorMessage.clear();
			return true;
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setVariable
	//
	////////////////////////////////////////////////////////////////

	bool MathParser::MathParserDetail::setVariable(const StringView name, double* value)
	{
		try
		{
			m_parser.DefineVar(Unicode::ToWstring(name), value);
			m_errorMessage.clear();
			return true;
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPrefixOperator
	//
	////////////////////////////////////////////////////////////////

	bool MathParser::MathParserDetail::setPrefixOperator(const StringView name, Fty1 f)
	{
		try
		{
			m_parser.DefineInfixOprt(Unicode::ToWstring(name), f);
			m_errorMessage.clear();
			return true;
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPostfixOperator
	//
	////////////////////////////////////////////////////////////////

	bool MathParser::MathParserDetail::setPostfixOperator(const StringView name, Fty1 f)
	{
		try
		{
			m_parser.DefinePostfixOprt(Unicode::ToWstring(name), f);
			m_errorMessage.clear();
			return true;
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	removeVariable
	//
	////////////////////////////////////////////////////////////////

	void MathParser::MathParserDetail::removeVariable(const StringView name)
	{
		m_parser.RemoveVar(Unicode::ToWstring(name));
		m_errorMessage.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearVariables
	//
	////////////////////////////////////////////////////////////////

	void MathParser::MathParserDetail::clearVariables()
	{
		m_parser.ClearConst();
		m_parser.ClearVar();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearFunctions
	//
	////////////////////////////////////////////////////////////////

	void MathParser::MathParserDetail::clearFunctions()
	{
		m_parser.ClearFun();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearOperators
	//
	////////////////////////////////////////////////////////////////

	void MathParser::MathParserDetail::clearOperators()
	{
		m_parser.ClearInfixOprt();
		m_parser.ClearPostfixOprt();
		m_parser.ClearOprt();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void MathParser::MathParserDetail::clear()
	{
		m_parser = mu::Parser{};
		m_errorMessage.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getExpression
	//
	////////////////////////////////////////////////////////////////

	String MathParser::MathParserDetail::getExpression() const
	{
		return Unicode::FromWstring(m_parser.GetExpr());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getUsedVariables
	//
	////////////////////////////////////////////////////////////////

	HashMap<String, double*> MathParser::MathParserDetail::getUsedVariables() const
	{
		try
		{
			HashMap<String, double*> result;

			for (const auto&[name, value] : m_parser.GetUsedVar())
			{
				result.emplace(Unicode::FromWstring(name), value);
			}

			m_errorMessage.clear();
			return result;
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();
			return{};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getVariables
	//
	////////////////////////////////////////////////////////////////

	HashMap<String, double*> MathParser::MathParserDetail::getVariables() const
	{
		try
		{
			HashMap<String, double*> result;

			for (const auto& [name, value] : m_parser.GetVar())
			{
				result.emplace(Unicode::FromWstring(name), value);
			}

			m_errorMessage.clear();
			return result;
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();
			return{};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getConstants
	//
	////////////////////////////////////////////////////////////////

	HashMap<String, double> MathParser::MathParserDetail::getConstants() const
	{
		try
		{
			HashMap<String, double> result;

			for (const auto& [name, value] : m_parser.GetConst())
			{
				result.emplace(Unicode::FromWstring(name), value);
			}

			m_errorMessage.clear();
			return result;
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();
			return{};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	validNameCharacters
	//
	////////////////////////////////////////////////////////////////

	String MathParser::MathParserDetail::validNameCharacters() const
	{
		return Unicode::FromWstring(m_parser.ValidNameChars());
	}

	////////////////////////////////////////////////////////////////
	//
	//	validPrefixCharacters
	//
	////////////////////////////////////////////////////////////////

	String MathParser::MathParserDetail::validPrefixCharacters() const
	{
		return Unicode::FromWstring(m_parser.ValidInfixOprtChars());
	}

	////////////////////////////////////////////////////////////////
	//
	//	validPostfixCharacters
	//
	////////////////////////////////////////////////////////////////

	String MathParser::MathParserDetail::validPostfixCharacters() const
	{
		return Unicode::FromWstring(m_parser.ValidOprtChars());
	}

	////////////////////////////////////////////////////////////////
	//
	//	eval
	//
	////////////////////////////////////////////////////////////////

	double MathParser::MathParserDetail::eval() const
	{
		try
		{
			const double result = m_parser.Eval();
			m_errorMessage.clear();
			return result;
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();
			return Math::NaN;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	evalArray
	//
	////////////////////////////////////////////////////////////////

	Array<double> MathParser::MathParserDetail::evalArray() const
	{
		try
		{
			int32 num_results;
			const double* v = m_parser.Eval(num_results);
			m_errorMessage.clear();

			return Array<double>(v, (v + num_results));
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();
			return{};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	eval
	//
	////////////////////////////////////////////////////////////////

	void MathParser::MathParserDetail::eval(double* dst, const size_t count) const
	{
		try
		{
			int32 num_results;
			const double* v = m_parser.Eval(num_results);	
			m_errorMessage.clear();

			int32 i = 0;

			for (; i < num_results; ++i)
			{
				dst[i] = v[i];
			}

			for (; i < static_cast<int32>(count); ++i)
			{
				dst[i] = Math::NaN;
			}
		}
		catch (mu::Parser::exception_type& e)
		{
			m_errorMessage = e.GetMsg();

			for (size_t i = 0; i < count; ++i)
			{
				dst[i] = Math::NaN;
			}
		}
	}
}
