#include "Config/Bootstrap.h"

#include "Framework/System/Utils.h"

#if defined(HD_PLATFORM_WIN64)

namespace hd
{
	void ShowCursor(bool value)
	{
		::ShowCursor(value ? TRUE : FALSE);
	}

	void GetCursorPosition(int32_t& x, int32_t& y)
	{
		POINT cursorPosition;
		if (::GetCursorPos(&cursorPosition))
		{
			x = cursorPosition.x;
			y = cursorPosition.y;
		}
		else
		{
			x = 0;
			y = 0;
		}
	}

	void SetCursorPosition(int32_t x, int32_t y)
	{
		::SetCursorPos(x, y);
	}
}

#endif
