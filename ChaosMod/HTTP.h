#pragma once

struct Response
{
	Response() : statusCode(0) {}
	void Reset()
	{
		text = "";
		header = L"";
		statusCode = 0;
		error = L"";
	}

	std::string text;
	std::wstring header;
	DWORD statusCode;
	std::wstring error;
};

class HTTP
{
public:
	HTTP(
		const std::wstring& domain,
		int port,
		bool secure,
		const std::wstring& user_agent = L"ChaosModV")
		: m_Domain(domain)
		, m_Port(port)
		, m_Secure(secure)
		, m_UserAgent(user_agent)
	{}

	bool Get(const std::wstring& path,
		const std::wstring& requestHeader,
		Response& response);
	bool Post(const std::wstring& path,
		const std::wstring& requestHeader,
		const std::string& body,
		Response& response);
	bool Put(const std::wstring& path,
		const std::wstring& requestHeader,
		const std::string& body,
		Response& response);
	bool Delete(const std::wstring& path,
		const std::wstring& requestHeader,
		const std::string& body,
		Response& response);

private:
	// Request is wrapper around http()
	bool Request(
		const std::wstring& verb,
		const std::wstring& path,
		const std::wstring& requestHeader,
		const std::string& body,
		Response& response);
	static bool http(
		const std::wstring& verb, const std::wstring& user_agent, const std::wstring& domain,
		const std::wstring& path, int port, bool secure,
		const std::wstring& requestHeader, const std::string& body,
		std::string& text, std::wstring& responseHeader,
		DWORD& statusCode, std::wstring& error);

	std::wstring m_Domain;
	int m_Port;
	bool m_Secure;
	std::wstring m_UserAgent;
};