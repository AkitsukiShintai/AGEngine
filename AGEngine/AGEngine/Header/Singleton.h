#pragma once
template <class T>
class Singleton
{
private:
	static T* mInstatce;
	Singleton(const Singleton<T>& c) {}
	Singleton& operator=(const Singleton<T>& c) {}
	template <class T>
	class DeleteSingleton
	{
	public:
		DeleteSingleton() {
			int a = 0;
		}
		~DeleteSingleton()
		{
			if (Singleton::mInstatce)
			{
				delete Singleton::mInstatce;
				Singleton::mInstatce = nullptr;
			}
		}
	};
protected:
	Singleton() {
		static DeleteSingleton<T> deleteSingleton;
	}
	virtual ~Singleton() {}
public:
	static T* GetInstance()
	{
		if (mInstatce == nullptr)
			mInstatce = new T();
		return mInstatce;
	};


};
template <class T>
T* Singleton<T>::mInstatce = nullptr;