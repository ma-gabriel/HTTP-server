#include <iostream>
#include "ClapTrap.hpp"

int main()
{
	ClapTrap Peter("The pacifist");
	ClapTrap Steve("The violent");
	
	for (int i = 0; i<11; i++){
		Steve.attack("Peter");
		Peter.takeDamage(0);
		Peter.beRepaired(0);
	}
}