#include <iostream>
#include <vector>
#include <algorithm>
#include <initializer_list>

#include "chain_v2.hpp"

template<typename FwdIt>
void select_sort(FwdIt first, FwdIt last)
{
	while (first != last) {
		auto min = std::min_element(first, last);
		std::swap(*first, *min);
		++first;
	}
}

template<typename T>
std::ostream& output(std::ostream& os, std::vector<T>& vals)
{
	for (const auto& i : vals)
		os << i << ' ';
	return os;
}

template<typename T>
std::ostream& output(std::ostream& os, mako::chain<T>& vals)
{
	for (const auto& i : vals)
		os << i << ' ';
	return os;
}

template<typename T>
std::ostream& output(std::ostream& os, typename mako::chain<T>::iterator it)
{ return os << *it << ' '; }

int main()
{
	std::vector<int> v1 {4, 9, 7};
	std::vector<int> v2 {8, 2, 1, 6};
	std::vector<int> v3 {3, 10, 5};
	std::vector<int> v4 {0, 333, 100, 44};
	std::vector<int> v5 {380, 323, 1000, 9844, 45, 45, 31, 11};
	std::vector<int> v6 {550, 33, 1100, 666};
	mako::chain<std::vector<int>> c1 {&v1, &v2, &v3, &v4, &v5, &v6};
	mako::chain<std::vector<int>> c2 {&v2, &v1, &v3, &v4, &v5, &v6};
	std::ostream& os = std::cout;

	os << "sizeof chain<T>::iterator: " << sizeof(c1.begin()) << '\n';
	os << "elements of chain<T>: " << c1.size() << '\n';
	os << v1[0] << '\n';
	v1[0] = v1[0] * 13;
	os << "real before: ";
	for (const auto i : {&v1, &v2, &v3, &v4, &v5, &v6}) output(os, *i);
	os << '\n';
	//std::for_each(b, e2, [](const auto& i) {os << i << ' ';}); os << '\n';
	//select_sort(b, e);
	//std::cout << "sorting:\n";
	//c1.begin();
	std::sort(c1.begin(), c1.end());
	//std::for_each(b, e, [&os](const auto& i) {os << i << ' ';}); os << '\n';
	//std::for_each(
	//		b + 1,
	//		e, [&os](const auto& i) {os << i << ' ';}); os << '\n';

	os << "c1 difference: ";
	os << (c1.end() - c1.begin()) << '\n';

	os << "c1: ";
	output(os, c1) << '\n';

	os << "c1-first: ";
	output
		<std::vector<int>> // how to make this call compile without explicitly mentioning this???
		(os, c1.begin()) << '\n';

	os << "c2: ";
	output(os, c2) << '\n';

	os << "real after: ";
	for (const auto i : {&v1, &v2, &v3, &v4, &v5, &v6}) output(os, *i);
	os << '\n'; 
}

