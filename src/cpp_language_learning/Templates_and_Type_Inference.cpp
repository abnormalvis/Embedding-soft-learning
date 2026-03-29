/*
要求：

实现一个函数模板 make_printable，接受任意类型参数
使用 std::enable_if 区分整数和浮点数处理
实现完美转发，将参数转发给内部函数
*/
#include <iostream>
#include <type_traits>
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
make_printable(T&& t)
{
    std::cout << t << std::endl;
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, void>::type
make_printable(T&& t)
{
    std::cout << t << std::endl;
}
int main()
{
    make_printable(42);
    make_printable(3.14);
    return 0;
}