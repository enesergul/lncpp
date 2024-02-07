// @Author  : enesergul
// @Time    : 09/01/2024
// @OS      : ubuntu 18.04
// @Software: VSCode

#include <iostream>

int main()
{

	[[maybe_unused]] double pi { 3.14159 };
    [[maybe_unused]] double gravity { 9.8 };
    double phi { 1.61803 };
	int width( 5 );
	int a;         // no initializer (default initialization)
	int b = 5;     // initializer after equals sign (copy initialization)
	int c( 6 );    // initializer in parenthesis (direct initialization)

	// List initialization methods (C++11) (preferred)
	int d { 7 };   // initializer in braces (direct list initialization)
	int e = { 8 }; // initializer in braces after equals sign (copy list initialization)
	int f {};      // initializer is empty braces (value initialization)
	std::cout << a << std::endl;
	std::cout << b << std::endl;
	std::cout << c << std::endl;
	std::cout << d << std::endl;
	std::cout << e << std::endl;
	std::cout << f << std::endl;
	std::cout << width << std::endl;

	int x { 0 };    // explicit initialization to value 0
	std::cout << x; // we're using that zero value
	int y {};      // value initialization
	std::cout <<  y; // we're immediately replacing that value
	std::cout << "Enter a number: "; // ask user for a number
    int p{}; // define variable x to hold user input
    std::cin >> p; // get number from keyboard and store it in variable x
    std::cout << "You entered " << p << '\n';
	std::cout << sizeof(int) << '\n'; // print how many bytes of memory an int value takes
	int num;
	std::cin >> num;
	std::cout <<"Enter an integer: " << num << '\n';
	std::cout <<"Double " << num <<" is: "<< num*2<<'\n';
	std::cout <<"Triple " << num <<" is: "<< num*3<<'\n';
	return 0;
}