//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include "ThirdParty/Catch2/catch_amalgamated.hpp"
# include <algorithm>
# include <cmath>
# include <limits>
# include <ostream>
# include <stdexcept>
# include <string>
# include <string_view>
# include <vector>

namespace s3d::Test
{
	// Retains the suite's strict, symmetric tolerance: |a-b| < epsilon * (scale + max(|a|, |b|)).
	// Catch::Approx uses a different tolerance, including at zero and at the boundary.
	class Approx
	{
	public:

		explicit Approx(const double value) noexcept
			: m_value{ value } {}

		Approx& epsilon(const double value) noexcept
		{
			m_epsilon = value;
			return *this;
		}

		Approx& scale(const double value) noexcept
		{
			m_scale = value;
			return *this;
		}

		friend bool operator ==(const double actual, const Approx& expected) noexcept
		{
			return (std::abs(actual - expected.m_value)
				< expected.m_epsilon * (expected.m_scale + std::max(std::abs(actual), std::abs(expected.m_value))));
		}

		friend std::ostream& operator <<(std::ostream& output, const Approx& value)
		{
			return output << "Approx(" << value.m_value << ", epsilon=" << value.m_epsilon
				<< ", scale=" << value.m_scale << ')';
		}

	private:

		double m_value;
		double m_epsilon = (static_cast<double>(std::numeric_limits<float>::epsilon()) * 100);
		double m_scale = 1.0;
	};

	// Keep the repository's --test-case entry point; all other execution options use Catch2 syntax.
	inline std::vector<std::string> CatchArguments(const int argc, const char* const* argv)
	{
		std::vector<std::string> result;
		result.reserve(static_cast<size_t>(argc));
		bool positional = false;
		for (int i = 0; i < argc; ++i)
		{
			const std::string_view argument{ argv[i] };
			if ((i == 0) || positional)
			{
				result.emplace_back(argument);
			}
			else if (argument == "--")
			{
				positional = true;
				result.emplace_back(argument);
			}
			# if defined(__APPLE__)
			// Xcode injects this Cocoa preference even when the scheme has no user arguments.
			else if ((argument == "-NSDocumentRevisionsDebugMode") && ((i + 1) < argc)
				&& ((std::string_view{ argv[i + 1] } == "YES") || (std::string_view{ argv[i + 1] } == "NO")))
			{
				++i;
			}
			# endif
			else if ((argument == "--test-only") || (argument == "--test-verbose"))
			{
				continue;
			}
			else if (argument.starts_with("--test-case=") || (argument == "--test-case"))
			{
				std::string_view filter;
				if (argument == "--test-case")
				{
					if (++i == argc)
					{
						throw std::invalid_argument{ "--test-case requires a nonempty filter." };
					}
					filter = argv[i];
				}
				else
				{
					filter = argument.substr(std::string_view{ "--test-case=" }.size());
				}
				if (filter.empty() || filter.starts_with('-'))
				{
					throw std::invalid_argument{ "--test-case requires a nonempty filter, not an option." };
				}
				result.emplace_back(filter);
			}
			else
			{
				result.emplace_back(argument);
			}
		}
		return result;
	}
}
