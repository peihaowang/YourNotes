#ifndef CPP_UTILITIES_H
#define CPP_UTILITIES_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>

#define __EACH(it, v) for(it=(v).begin(); it!=(v).end(); ++it)
#define __EACH_R(it, v) for(it=(v).rbegin(); it!=(v).rend(); ++it)

template<class T> class _CTempValueChange{

private:

	T&	m_valRef;
	T	m_valOld;

public:

	virtual ~_CTempValueChange(void){m_valRef=m_valOld;}
	_CTempValueChange(T& t, const T& newval)
		: m_valRef(t)
		, m_valOld(t)
	{
		m_valRef=newval;
	}

};

//2009.11.6 This class works like std::map<>, except it keeps elements in original order added, without sorting;
template<typename T1, typename T2> class _CPairVector : public std::vector< std::pair<T1, T2> >{

public:

	using std::vector< std::pair<T1, T2> >::begin;
	using std::vector< std::pair<T1, T2> >::end;
	using std::vector< std::pair<T1, T2> >::size;

	typedef	typename std::vector< std::pair<T1, T2> >::iterator		iterator;
	typedef	typename std::vector< std::pair<T1, T2> >::const_iterator	const_iterator;

	iterator _find(const T2& d2)
	{
		iterator it=this->end();
		for(it=begin(); it!=end(); ++it){
			if(it->second==d2){
				break;
			}
		}
		return it;
	}

	const_iterator _find(const T2& d2)const
	{
		const_iterator it=end();
		for(it=begin(); it!=end(); ++it){
			if(it->second==d2){
				break;
			}
		}
		return it;
	}

	iterator _find(const T1& d1)
	{
		iterator it=end();
		for(it=begin(); it!=end(); ++it){
			if(it->first==d1){
				break;
			}
		}
		return it;
	}

	const_iterator _find(const T1& d1)const
	{
		const_iterator it=end();
		for(it=begin(); it!=end(); ++it){
			if(it->first==d1){
				break;
			}
		}
		return it;
	}

public:

	_CPairVector(void){return;}

	T1& operator[](const T2& d2)
	{
		iterator it=_find(d2);
		if(it!=end()){
			return it->first;
		}else{
			this->push_back(std::pair<T1, T2>());
			it=begin()+(size()-1);
			it->second=d2;
			return it->first;
		}
	}

	T2& operator[](const T1& d1)
	{
		iterator it=_find(d1);
		if(it!=end()){
			return it->second;
		}else{
			this->push_back(std::pair<T1, T2>());
			it=begin()+(size()-1);
			it->first=d1;
			return it->second;
		}
	}

	T1 operator()(const T2& d2, const T1& xDef)const
	{
		const_iterator it;
		for(it=begin(); it!=end(); ++it){
			if(it->second==d2){
				return it->first;
			}
		}
		return xDef;
	}

	T2 operator()(const T1& d1, const T2& xDef)const
	{
		const_iterator it;
		for(it=begin(); it!=end(); ++it){
			if(it->first==d1){
				return it->second;
			}
		}
		return xDef;
	}

};

//2009.8.24 These 'predicate' containers are usually passed into the 'enumerate/list' function as a container;
template<typename T> class _CPredVector : public std::vector<T>{

public:

	virtual void operator()(const T& t){this->push_back(t);}

};

template<typename T> class _CPredSet : public std::set<T>{

public:

	virtual void operator()(const T& t){this->insert(t);}

};

#endif // CPP_UTILITIES_H
