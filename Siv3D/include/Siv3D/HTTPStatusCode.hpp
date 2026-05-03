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
# include "Types.hpp"
# include "Utility.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	HTTPStatusCode
	//
	////////////////////////////////////////////////////////////////

	enum class HTTPStatusCode : uint16
	{
		// 1xx Informational
		Continue           = 100,
		SwitchingProtocols = 101,
		Processing         = 102, // WebDAV
		EarlyHints         = 103,

		// 2xx Successful
		Ok                          = 200,
		Created                     = 201,
		Accepted                    = 202,
		NonAuthoritativeInformation = 203,
		NoContent                   = 204,
		ResetContent                = 205,
		PartialContent              = 206,
		MultiStatus                 = 207, // WebDAV
		AlreadyReported             = 208, // WebDAV
		ImUsed                      = 226,

		// 3xx Redirection
		MultipleChoices   = 300,
		MovedPermanently  = 301,
		Found             = 302,
		SeeOther          = 303,
		NotModified       = 304,
		UseProxy          = 305, // deprecated
		TemporaryRedirect = 307,
		PermanentRedirect = 308,

		// 4xx Client Error
		BadRequest                  = 400,
		Unauthorized                = 401,
		PaymentRequired             = 402,
		Forbidden                   = 403,
		NotFound                    = 404,
		MethodNotAllowed            = 405,
		NotAcceptable               = 406,
		ProxyAuthenticationRequired = 407,
		RequestTimeout              = 408,
		Conflict                    = 409,
		Gone                        = 410,
		LengthRequired              = 411,
		PreconditionFailed          = 412,
		ContentTooLarge             = 413,
		UriTooLong                  = 414,
		UnsupportedMediaType        = 415,
		RangeNotSatisfiable         = 416,
		ExpectationFailed           = 417,
		MisdirectedRequest          = 421,
		UnprocessableContent        = 422,
		Locked                      = 423, // WebDAV
		FailedDependency            = 424, // WebDAV
		TooEarly                    = 425,
		UpgradeRequired             = 426,
		PreconditionRequired        = 428,
		TooManyRequests             = 429,
		RequestHeaderFieldsTooLarge = 431,
		UnavailableForLegalReasons  = 451,

		// 5xx Server Error
		InternalServerError           = 500,
		NotImplemented                = 501,
		BadGateway                    = 502,
		ServiceUnavailable            = 503,
		GatewayTimeout                = 504,
		HttpVersionNotSupported       = 505,
		VariantAlsoNegotiates         = 506,
		InsufficientStorage           = 507, // WebDAV
		LoopDetected                  = 508, // WebDAV
		NotExtended                   = 510,
		NetworkAuthenticationRequired = 511,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief HTTP ステータスコードを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value HTTP ステータスコード
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, HTTPStatusCode value);

	namespace SimpleHTTP
	{
		constexpr bool IsInformational(HTTPStatusCode code) noexcept;

		constexpr bool IsSuccess(HTTPStatusCode code) noexcept;

		constexpr bool IsRedirect(HTTPStatusCode code) noexcept;

		constexpr bool IsClientError(HTTPStatusCode code) noexcept;

		constexpr bool IsServerError(HTTPStatusCode code) noexcept;
	}
}

# include "detail/HTTPStatusCode.ipp"
