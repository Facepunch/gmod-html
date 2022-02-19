#pragma once

#include <queue>

#include "html/IHtmlClientListener.h"

#include "cef_start.h"
#include "include/cef_base.h"
#include "include/base/cef_lock.h"
#include "cef_end.h"

//
// Thread-Safe queue used to send messages from Chromium to the main thread.
//
class MessageQueue
{
public:
	enum class Type
	{
		OnAddressChange,
		OnConsoleMessage,
		OnTitleChange,
		OnCursorChange,
		OnLoadStart,
		OnLoadEnd,
		OnCreateChildView,
		OnJavaScriptCall
	};

	struct Message
	{
		Type type;
		int integer;
		std::string string1;
		std::string string2;
		JSValue jsParams;
	};

	void Lock()
	{
		m_Lock.Acquire();
	}

	void Unlock()
	{
		m_Lock.Release();
	}

	void Push( Message&& message )
	{
		m_Messages.emplace( std::move( message ) );
	}

	const Message& Front()
	{
		return m_Messages.front();
	}

	bool Empty()
	{
		return m_Messages.empty();
	}

	void Pop()
	{
		m_Messages.pop();
	}

private:
	base::Lock m_Lock;
	std::queue<Message> m_Messages;
};
