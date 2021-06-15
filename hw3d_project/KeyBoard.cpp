#include "KeyBoard.h"

bool KeyBoard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return keystates[keycode];
}

KeyBoard::Event KeyBoard::ReadKey() noexcept
{
	if (keybuffer.size() > 0u)
	{
		KeyBoard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else
	{
		return KeyBoard::Event();
	}
}

bool KeyBoard::KeyIsEmpty() const noexcept
{
	return keybuffer.empty();
}

void KeyBoard::FlushKey() noexcept
{
	keybuffer = std::queue<Event>();
}

char KeyBoard::ReadChar() noexcept
{
	if (charbuffer.size() > 0u)
	{
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

bool KeyBoard::CharIsEmpty() const noexcept
{
	return charbuffer.empty();
}

void KeyBoard::FlushChar() noexcept
{
	charbuffer = std::queue<char>();
}

void KeyBoard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void KeyBoard::EnableAutoRepeat() noexcept
{
	autorepeatEnabled = true;
}

void KeyBoard::DisableAutoRepeat() noexcept
{
	autorepeatEnabled = false;
}

bool KeyBoard::AutoRepeatedIsEnabled() const noexcept
{
	return autorepeatEnabled;
}

void KeyBoard::OnKeyPressed(unsigned char keycode) noexcept
{
	keystates[keycode] = true;
	keybuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Pressed,keycode));
	TrimBuffer(keybuffer);
}

void KeyBoard::OnKeyReleased(unsigned char keycode) noexcept
{
	keystates[keycode] = false;
	keybuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Realeased,keycode));
	TrimBuffer(keybuffer);
}

void KeyBoard::OnChar(char character) noexcept
{
	charbuffer.push(character);
	TrimBuffer(charbuffer);
}

void KeyBoard::ClearState() noexcept
{
	keystates.reset();
}
