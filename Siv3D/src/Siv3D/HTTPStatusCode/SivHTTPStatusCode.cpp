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

# include <Siv3D/HTTPStatusCode.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr StringView ToString(const HTTPStatusCode code)
		{
			using enum HTTPStatusCode;

			switch (code)
			{
			case Continue: return U"Continue"_sv;
			case SwitchingProtocols: return U"SwitchingProtocols"_sv;
			case Processing: return U"Processing"_sv;
			case EarlyHints: return U"EarlyHints"_sv;

			case Ok: return U"Ok"_sv;
			case Created: return U"Created"_sv;
			case Accepted: return U"Accepted"_sv;
			case NonAuthoritativeInformation: return U"NonAuthoritativeInformation"_sv;
			case NoContent: return U"NoContent"_sv;
			case ResetContent: return U"ResetContent"_sv;
			case PartialContent: return U"PartialContent"_sv;
			case MultiStatus: return U"MultiStatus"_sv;
			case AlreadyReported: return U"AlreadyReported"_sv;
			case ImUsed: return U"ImUsed"_sv;

			case MultipleChoices: return U"MultipleChoices"_sv;
			case MovedPermanently: return U"MovedPermanently"_sv;
			case Found: return U"Found"_sv;
			case SeeOther: return U"SeeOther"_sv;
			case NotModified: return U"NotModified"_sv;
			case UseProxy: return U"UseProxy"_sv;
			case TemporaryRedirect: return U"TemporaryRedirect"_sv;
			case PermanentRedirect: return U"PermanentRedirect"_sv;

			case BadRequest: return U"BadRequest"_sv;
			case Unauthorized: return U"Unauthorized"_sv;
			case PaymentRequired: return U"PaymentRequired"_sv;
			case Forbidden: return U"Forbidden"_sv;
			case NotFound: return U"NotFound"_sv;
			case MethodNotAllowed: return U"MethodNotAllowed"_sv;
			case NotAcceptable: return U"NotAcceptable"_sv;
			case ProxyAuthenticationRequired: return U"ProxyAuthenticationRequired"_sv;
			case RequestTimeout: return U"RequestTimeout"_sv;
			case Conflict: return U"Conflict"_sv;
			case Gone: return U"Gone"_sv;
			case LengthRequired: return U"LengthRequired"_sv;
			case PreconditionFailed: return U"PreconditionFailed"_sv;
			case ContentTooLarge: return U"ContentTooLarge"_sv;
			case UriTooLong: return U"UriTooLong"_sv;
			case UnsupportedMediaType: return U"UnsupportedMediaType"_sv;
			case RangeNotSatisfiable: return U"RangeNotSatisfiable"_sv;
			case ExpectationFailed: return U"ExpectationFailed"_sv;
			case MisdirectedRequest: return U"MisdirectedRequest"_sv;
			case UnprocessableContent: return U"UnprocessableContent"_sv;
			case Locked: return U"Locked"_sv;
			case FailedDependency: return U"FailedDependency"_sv;
			case TooEarly: return U"TooEarly"_sv;
			case UpgradeRequired: return U"UpgradeRequired"_sv;
			case PreconditionRequired: return U"PreconditionRequired"_sv;
			case TooManyRequests: return U"TooManyRequests"_sv;
			case RequestHeaderFieldsTooLarge: return U"RequestHeaderFieldsTooLarge"_sv;
			case UnavailableForLegalReasons: return U"UnavailableForLegalReasons"_sv;

			case InternalServerError: return U"InternalServerError"_sv;
			case NotImplemented: return U"NotImplemented"_sv;
			case BadGateway: return U"BadGateway"_sv;
			case ServiceUnavailable: return U"ServiceUnavailable"_sv;
			case GatewayTimeout: return U"GatewayTimeout"_sv;
			case HttpVersionNotSupported: return U"HttpVersionNotSupported"_sv;
			case VariantAlsoNegotiates: return U"VariantAlsoNegotiates"_sv;
			case InsufficientStorage: return U"InsufficientStorage"_sv;
			case LoopDetected: return U"LoopDetected"_sv;
			case NotExtended: return U"NotExtended"_sv;
			case NetworkAuthenticationRequired: return U"NetworkAuthenticationRequired"_sv;
			
			default: return U"Unknown"_sv;
			}
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const HTTPStatusCode value)
	{
		formatData.string.append(ToString(value));
	}
}
