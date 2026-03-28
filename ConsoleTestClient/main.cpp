#include <iostream>
#include <ProcessController.h>

int main()
{
	Muninn::Controller::ProcessController pc{ 0x1, PROCESS_ALL_ACCESS };
	std::cout << "Hello World!\n";
}