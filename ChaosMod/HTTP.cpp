#include <stdafx.h>

#include "HTTP.h"

bool HTTP::Get(
	const std::wstring& path,
	const std::wstring& requestHeader,
	HttpResponse& response)
{
	static const std::wstring verb = L"GET";
	static std::string body;
	return Request(verb, path, requestHeader, body, response);
}

bool HTTP::Post(
	const std::wstring& path,
	const std::wstring& requestHeader,
	const std::string& body,
	HttpResponse& response)
{
	static const std::wstring verb = L"POST";
	return Request(verb, path, requestHeader, body, response);
}

bool HTTP::Put(
	const std::wstring& path,
	const std::wstring& requestHeader,
	const std::string& body,
	HttpResponse& response)
{
	static const std::wstring verb = L"PUT";
	return Request(verb, path, requestHeader, body, response);
}

bool HTTP::Delete(
	const std::wstring& path,
	const std::wstring& requestHeader,
	const std::string& body,
	HttpResponse& response)
{
	static const std::wstring verb = L"DELETE";
	return Request(verb, path, requestHeader, body, response);
}

bool HTTP::Request(
	const std::wstring& verb,
	const std::wstring& path,
	const std::wstring& requestHeader,
	const std::string& body,
	HttpResponse& response)
{
	return http(
		verb, m_UserAgent, m_Domain,
		path, m_Port, m_Secure,
		requestHeader, body,
		response.text, response.header,
		response.statusCode, response.error
	);
}

bool HTTP::http(
	const std::wstring& verb, 
	const std::wstring& user_agent, 
	const std::wstring& domain,
	const std::wstring& path, 
	int port, bool secure,
	const std::wstring& requestHeader, 
	const std::string& body,
	std::string& text, 
	std::wstring& responseHeader,
	DWORD& dwStatusCode, 
	std::wstring& error)
{
	DWORD dwLastStatus = 0;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	BOOL bResults = FALSE;
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	BOOL bDone = FALSE;

	dwStatusCode = 0;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(
		user_agent.c_str(),
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0
	);

	// Specify an HTTP server.
	if (hSession)
	{
		hConnect = WinHttpConnect(hSession, domain.c_str(), port, 0);
	}
	else
	{
		error = L"WinHttpOpen fails!";
		return false;
	}

	// Create an HTTP request handle.
	if (hConnect)
	{
		DWORD flag = secure ? WINHTTP_FLAG_SECURE : 0;
		hRequest = WinHttpOpenRequest(
			hConnect, verb.c_str(), 
			path.c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_REFRESH | flag
		);
	}
	else
	{
		WinHttpCloseHandle(hSession);
		error = L"WinHttpConnect fails!";
		return false;
	}

	if (hRequest == NULL) bDone = TRUE;

	while (!bDone)
	{
		// Send a request.
		if (hRequest)
		{
			if (requestHeader.empty())
			{
				bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)body.data(), body.size(), body.size(), 0);
			}
			else
			{
				bResults = WinHttpSendRequest(hRequest, requestHeader.c_str(), requestHeader.size(), (LPVOID)body.data(), body.size(), body.size(), 0);
			}
			if (!bResults)
			{
				error = L"WinHttpSendRequest fails!";
			}
		}

		// End the request.
		if (bResults)
		{
			bResults = WinHttpReceiveResponse(hRequest, NULL);
			if (!bResults)
			{
				error = L"WinHttpReceiveResponse fails!";
			}
		}

		// Resend the request in case of ERROR_WINHTTP_RESEND_REQUEST error.
		if (!bResults && GetLastError() == ERROR_WINHTTP_RESEND_REQUEST) continue;

		// Check the status code.
		if (bResults)
		{
			dwSize = sizeof(dwStatusCode);
			bResults = WinHttpQueryHeaders(
				hRequest,
				WINHTTP_QUERY_STATUS_CODE |
				WINHTTP_QUERY_FLAG_NUMBER,
				WINHTTP_HEADER_NAME_BY_INDEX,
				&dwStatusCode,
				&dwSize,
				WINHTTP_NO_HEADER_INDEX
			);

			if (!bResults)
			{
				error = L"WinHttpQueryHeaders fails!";
			}

			// Get response header
			WinHttpQueryHeaders(
				hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
				WINHTTP_HEADER_NAME_BY_INDEX, NULL,
				&dwSize, WINHTTP_NO_HEADER_INDEX
			);

			// Allocate memory for the buffer.
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				responseHeader.resize(dwSize + 1);

				// Now, use WinHttpQueryHeaders to retrieve the header.
				bResults = WinHttpQueryHeaders(
					hRequest,
					WINHTTP_QUERY_RAW_HEADERS_CRLF,
					WINHTTP_HEADER_NAME_BY_INDEX,
					(LPVOID)responseHeader.data(),
					&dwSize,
					WINHTTP_NO_HEADER_INDEX
				);
			}
		}

		// Keep checking for data until there is nothing left.
		if (bResults)
		{
			switch (dwStatusCode)
			{
			case 200:
			{
				std::string temp;
				text = "";
				do
				{
					// Check for available data.
					dwSize = 0;
					if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
					{
						error = L"Error in WinHttpQueryDataAvailable: " + std::to_wstring(GetLastError());
					}

					if (dwSize == 0) break;

					// Allocate space for the buffer.
					temp = "";
					temp.resize(dwSize);

					// Read the data.
					ZeroMemory((void*)(&temp[0]), dwSize);
					if (WinHttpReadData(hRequest, (LPVOID)(&temp[0]), dwSize, &dwDownloaded))
					{
						text += temp;
					}
					else
					{
						error = L"Error in WinHttpReadData: " + std::to_wstring(GetLastError());
					}
				} while (dwSize > 0);

				break;
			}
			default:
				// The status code does not indicate success.
				error = L"Error. Status code returned: " + std::to_wstring(dwStatusCode);
				bDone = TRUE;
				break;
			}

			bDone = TRUE;
			break;
		}

		// Keep track of the last status code.
		dwLastStatus = dwStatusCode;

		// If there are any errors, break out of the loop.
		if (!bResults) bDone = TRUE;
	}

	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	// Report any errors.
	if (!bResults) return false;

	return true;
}