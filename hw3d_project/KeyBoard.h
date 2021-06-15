#pragma once
#include <bitset>
#include <queue>

class KeyBoard
{
public:
	friend class Window;
private:
	class Event
	{
	public:
		enum class Type
		{
			Pressed,
			Realeased,
			InValid
		};
	public:
		Type type;
		unsigned char code;
		
	public:
		Event()noexcept : type(Type::InValid) , code(0u)
		{
		}
		Event(Type typein, unsigned char codein)noexcept
			:type(typein), code(codein)
		{
			
		}
		bool IsPress()const noexcept
		{
			return type == Type::Pressed;
		}
		bool IsRelease()const noexcept
		{
			return type == Type::Realeased;
		}
		bool IsValid()const noexcept
		{
			return type != Type::InValid;
		}
		unsigned char GetCode()const noexcept
		{
			return code;
		}
			

		
	};
public:
	KeyBoard() = default;
	KeyBoard(const KeyBoard&) = delete;
	KeyBoard& operator=(KeyBoard&) = delete;
public:
	//KeyEvent
	bool KeyIsPressed(unsigned char keycode)const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty()const noexcept;
	void FlushKey()noexcept;
	//CharEvent
	char  ReadChar()noexcept;
	bool  CharIsEmpty()const noexcept;
	void  FlushChar()noexcept;
	void  Flush()noexcept;
	//auto Repeated
	void EnableAutoRepeat()noexcept;
	void DisableAutoRepeat()noexcept;
	bool AutoRepeatedIsEnabled()const noexcept;
private:
	void OnKeyPressed(unsigned char keycode)noexcept;
	void OnKeyReleased(unsigned char keycode)noexcept;
	void OnChar(char character)noexcept;
	void ClearState()noexcept;
	
	template<typename T>
	void TrimBuffer(std::queue<T>& buffer)noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};

template<typename T>
void KeyBoard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}
