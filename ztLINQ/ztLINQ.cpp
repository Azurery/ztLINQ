/*!
 * \file   ztLINQ.cpp
 *
 * \author Magicmanoooo
 * \mail   zt838713968@gmail.com
 * \date   2018/08/22 
 *
 * \brief  
 *
 * TODO:   long description
 *
 * // Copyright (c) 2018 The ztLINQ Authors. All rights reserved.
 * // Use of this source code is governed by a BSD-style license that can be
 * // found in the LICENSE file. See the AUTHORS file for names of contributors.
 * 
 */
#include <iostream>
#include <vector>
#include <cassert>
#include <type_traits>


/*!
 * \class linq_enumerable
 *
 * \brief 该class不保存对象本身，而是保存容器的迭代器，这也是function programming的的一般实现方式。
 *
 */
template<typename Iterator>
struct linq_enumerable {
	using linq_element = typename std::remove_cv<
							typename std::remove_reference<
								iterator_type<Iterator>>
							::type>>
						 ::type;
private:
	Iterator begin_;
	Iterator end_;
public:
	linq_enumerable(){}
	linq_enumerable(const Iterator& begin, const Iterator& end_)
		:begin_(begin), end_(end) {}
	Iterator begin() const { return begin_; }
	Iterator end() const { return end_; }
};


//对于各种Iterator类，它需要接收一个或多个表示不确定类型的迭代器，并且视情况需要接受一个或多个不确定类型的lambda

//将nullptr强行转化为指向Iterator的指针，然后对其解引用得到一个不存在的Iterator对象：
//*(Iterator*)nullptr,再对迭代器对象进行解引用，便可得到迭代器所指向的元素。
//最后对其使用decltype操作，得到元素类型

//************************************
// \method name:    iterator_type
//
// \brief:			
//************************************
template <typename Iterator>
using iterator_type = decltype(**(Iterator*)nullptr);

/*!
 * \class select_iterator
 *
 * \brief select_iterator为select函数的迭代器。在进行取值操作时，返回迭代器所指向的元素的类型
 *
 */
template <typename Iterator, typename Function>
class select_iterator {
	using select_iterator_self = select_iterator<Iterator, Function>;
private:
	Iterator _iterator;
	Function _function;
public:
	select_iterator(const Iterator& i, const Function& f) :
		_iterator(i), _function(f) {}

	select_iterator& operator++() {
		++_iterator;
		return *this;
	}

	select_iterator operator++(int) {
		select_iterator_self r = *this;
		iterator++;
		return r;
	}

	auto operator*() const->decltype(_function(*_iterator)) {
		return _function(*_iterator);
	}

	bool operator ==(const select_iterator& other) const {
		return other._iterator == _iterator;
	}

	bool operator !=(const select_iterator& other) const {
		return !(other._iterator == _iterator);
	}
};

/*当where_iterator自增时，他会有一个谓词条件，若没有满足这个条件，则会继续自增，以此过滤不满足条件的元素
*where_iterator的取值操作为：直接对它所包装的迭代器进行*操作即可
*
*/
/*!
 * \class where_iterator
 *
 * \brief 
 *
 */
template <typename Iterator, typename Function>
class where_iterator {
private:
	Iterator _iterator;
	Iterator _end;
	Function _function;
public:
	where_iterator(const Iterator& i, const Iterator& e, const Function& f) :
		_iterator(i), _end(e), _function(f) {
		while (_iterator != _end && !_function(*_iterator)) {
			++_iterator;
		}
	}

	where_iterator& operator++() {
		if (_iterator == _end)
			return *this;
		++_iterator;
		while (_iterator != _end && !_function(*_iterator)) {
			++_iterator;
		}
		return *this;
	}

	iterator_type<Iterator> operator*() const {
		return *_iterator;
	}

	bool operator== (const where_iterator& self) const {
		return self._iterator == _iterator;
	}

	bool operator!= (const where_iterator& self) const {
		return !(self._iterator == _iterator);
	}

};


//take函数的作用：从查询结果中提取前n个元素
template <typename Iterator>
class take_iterator {
private:
	Iterator _iterator;	//当前容器的迭代器
	Iterator _end;		//当前容器的尾后位置
	int _count;		//总的元素个数
	int _cur;		//当前元素个数
public:
	//take_iterator的构造函数默认初始化为从容器起始位置开始到容器尾后位置截止的范围内的第0个位置
	take_iterator(const Iterator& i, Iterator& e, int c) :
		_iterator(i), _end(e), _count(c), _cur(0) {
		//如果当前元素个数等于总的元素个数，则说明处在容器的末尾位置		
		if (_cur == _count) {
			_iterator = _end;
		}
	}

	iterator_type<Iterator> operator* () const {
		return *_iterator;
	}

	take_iterator& operator++() {
		//首先检查cur是否指向了容器的尾后位置
		if (++_cur == _count)
			_iterator = _end;
		else
			++_iterator;
		return *this;
	}

	bool operator==(const take_iterator& _self) const {
		return _self._iterator == _iterator;
	}

	bool operator!=(const take_iterator& _self) const {
		return !(_self._iterator == _iterator);
	}

};


/*
TakeWhile<TSource>(IEnumerable<TSource>, Func<TSource, Boolean> predicate)方法使用 predicate 对
source 中的每个元素进行测试，如果结果为true，则生成该元素。
当谓词函数对某个元素返回false或source中不再包含元素时，枚举将停止。
*/
template <typename Iterator, typename Function>
class take_while_iterator {
private:
	Iterator _iterator;
	Iterator _end;
	Function _function;
public:
	take_while_iterator(const Iterator& i, const Iterator& e, const Function& f) :
		_iterator(i), _end(e), _function(f) {
		if (_iterator != end && !_function(*_iterator)) {
			_iterator = _end;
		}
	}

	take_while_iterator& operator++() {
		++_iterator;
		//用于测试每个元素是否满足条件的函数
		//如果不满足，则将迭代器位置移到容器末尾位置
		if (!_function(*_iterator)) {
			_iterator = _end;
		}
		return *this;
	}

	iterator_type<Iterator> operator*() const {
		return *_iterator;
	}

	bool operator==(const take_while_iterator& _self) const {
		return _self._iterator == _iterator;
	}

	bool operator!=(const take_while_iterator& _self) const {
		return !(_self._iterator == _iterator);
	}
};

//skip_iterator: Skip<TSource>
//跳过序列中指定数量的元素，然后返回剩余的元素。
template <typename Iterator>
class skip_iterator {
private:
	Iterator _iterator;
	Iterator _end;
	int _count;
public:
	skip_iterator(const Iterator& i, const Iterator& e, int c) :
		_iterator(i), _end(e), _count(c) {
		while (_iterator != _end && (count--)>0) {
			++_iterator;
		}
	}

	iterator_type<Iterator> operator*() const {
		return *_iterator;
	}

	skip_iterator& operator++() {
		++_iterator;
		return *this;
	}

	bool operator==(const skip_iterator& _self) const {
		return _self._iterator == _iterator;
	}

	bool operator!=(const skip_iterator& _self) const {
		return !(_self._iterator == _iterator);
	}


};

//linq_enumerable类中不保存对象本身，而是保存容器的迭代器，这也是一般的函数式语言的实现方式。
//select返回的对象与where返回的对象的唯一区别就是迭代器不同。我们可以设计一个通用的迭代器，
//它可以处理多种不同的迭代需求，包括跳过一些元素或者对一些元素做处理，但是更好的方法是对每一个
//操作都设计一个新类型的迭代器，比如select_iterator和where_iterator等，然后用模板来处理不同的迭代器。
//数或者表示区间的参数，返回类linq_enumerable的对对象
// 

template <typename Container>
auto from(const Container& _container) -> linq_enumerable<decltype(std::begin(_container))> {
	return linq_enumerable<decltype(std::begin(_container))>(
		std::begin(_container),
		std::end(_container)
		);
}


int main() {
	//std::vector<int> v={1,2,3,4,5,6,7,8,9};
	/*for(auto x:from(v)){
	std::cout<<x<<std::endl;
	}
	*/
	std::vector<int> v = { 2,3,4,5,6,7,8,9,15,20 };
	auto p = from(v).where([](int x) {
		return x % 2 == 1;
	});

	for (auto first = p.begin(); first != p.end(); ++first) {
		std::cout << *first << std::endl;
	}


	return 0;
}
