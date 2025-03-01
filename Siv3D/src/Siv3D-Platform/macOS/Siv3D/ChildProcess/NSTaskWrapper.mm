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

# include <Foundation/Foundation.h>
# include "NSTaskWrapper.hpp"

namespace s3d
{
	OPStream::OPStream()
		: std::ostream(this)
	{
		
	}
	
	void OPStream::setWriteHandle(NSFileHandle* writeHandle)
	{
		m_writeHandle = writeHandle;
	}
	
	int OPStream::overflow(int c)
	{
		if (not m_writeHandle)
		{
			m_buffer.clear();
			return traits_type::eof();
		}
		
		const char ch = static_cast<char>(c);
		m_buffer.push_back(ch);

		if (ch == '\n')
		{
			NSData* data = [NSData dataWithBytesNoCopy:(void*)m_buffer.c_str() length:m_buffer.size()];
			
			[m_writeHandle writeData:data];

			m_buffer.clear();
		}

		return 0;
	}
	
	NSTaskWrapper::NSTaskWrapper()
	{
		
	}
	
	NSTaskWrapper::NSTaskWrapper(const char* path, const Array<String>& commands, const Pipe pipe)
	{

	}
	
	NSTaskWrapper::~NSTaskWrapper()
	{
		if (m_writeHandle)
		{
			[m_writeHandle closeFile];
		}
		
		if (m_readHandle)
		{
			[m_readHandle closeFile];
		}
		
		if (m_task)
		{
			[m_task release];
		}
	}
	
	bool NSTaskWrapper::isValid() const
	{
		return m_valid;
	}
	
	bool NSTaskWrapper::isRunning()
	{
		if (not isValid())
		{
			return false;
		}
		
		return [m_task isRunning] == YES;
	}
	
	void NSTaskWrapper::wait()
	{
		if (not isValid())
		{
			return;
		}
		
		[m_task waitUntilExit];
	}
	
	void NSTaskWrapper::terminate()
	{
		if (not isValid())
		{
			return;
		}
		
		[m_task terminate];
	}
	
	Optional<int32> NSTaskWrapper::getExitCode()
	{
		if ((not isValid()) || [m_task isRunning])
		{
			return none;
		}
		
		return [m_task terminationStatus];
	}
	
	std::istream& NSTaskWrapper::istream()
	{
		if ((not isValid()) || (not m_readHandle))
		{
			return m_is;
		}
		
		NSData* data = [m_readHandle availableData];
		NSString* output = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
		std::string s = [output UTF8String];
		[output release];
		
		m_is << s;

		return m_is;
	}
	
	std::ostream& NSTaskWrapper::ostream()
	{
		return m_os;
	}
}
